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
v){struct Cyc_List_List*_tmp5E5;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5E5=_cycalloc(sizeof(*_tmp5E5)),((_tmp5E5->hd=v,((_tmp5E5->tl=0,_tmp5E5)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5E6;return(_tmp5E6="\\a",
_tag_dyneither(_tmp5E6,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5E7;
return(_tmp5E7="\\b",_tag_dyneither(_tmp5E7,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5E8;return(_tmp5E8="\\f",_tag_dyneither(_tmp5E8,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5E9;return(_tmp5E9="\\n",_tag_dyneither(_tmp5E9,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5EA;return(_tmp5EA="\\r",
_tag_dyneither(_tmp5EA,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5EB;
return(_tmp5EB="\\t",_tag_dyneither(_tmp5EB,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5EC;return(_tmp5EC="\\v",_tag_dyneither(_tmp5EC,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5ED;return(_tmp5ED="\\\\",_tag_dyneither(_tmp5ED,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5EE;return(_tmp5EE="\"",
_tag_dyneither(_tmp5EE,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5EF;
return(_tmp5EF="\\'",_tag_dyneither(_tmp5EF,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5F2;
char _tmp5F1;struct _dyneither_ptr _tmp5F0;(_tmp5F0=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5F1=*((char*)_check_dyneither_subscript(_tmp5F0,sizeof(
char),0)),((_tmp5F2=c,((_get_dyneither_size(_tmp5F0,sizeof(char))== 1  && (
_tmp5F1 == '\000'  && _tmp5F2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F0.curr)=
_tmp5F2)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5F5;char _tmp5F4;struct
_dyneither_ptr _tmp5F3;(_tmp5F3=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F4=*((char*)_check_dyneither_subscript(_tmp5F3,sizeof(char),0)),((_tmp5F5='\\',((
_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (_tmp5F4 == '\000'  && _tmp5F5 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=_tmp5F5)))))));}{char _tmp5F8;char
_tmp5F7;struct _dyneither_ptr _tmp5F6;(_tmp5F6=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5F7=*((char*)_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((
_tmp5F8=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5F6,
sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?_throw_arraybounds():
1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}{char _tmp5FB;char _tmp5FA;struct
_dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5FA=*((char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((_tmp5FB=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5F9,sizeof(char))== 1  && (
_tmp5FA == '\000'  && _tmp5FB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F9.curr)=
_tmp5FB)))))));}{char _tmp5FE;char _tmp5FD;struct _dyneither_ptr _tmp5FC;(_tmp5FC=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5FD=*((char*)
_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((_tmp5FE=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'  && _tmp5FE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=_tmp5FE)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp601;char _tmp600;struct
_dyneither_ptr _tmp5FF;(_tmp5FF=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp600=*((
char*)_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((_tmp601='\\',((
_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'  && _tmp601 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}{char _tmp604;char
_tmp603;struct _dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp603=*((char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((
_tmp604='a',((_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'
 && _tmp604 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp607;char _tmp606;struct
_dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp606=*((
char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((_tmp607='\\',((
_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'  && _tmp607 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}{char _tmp60A;char
_tmp609;struct _dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp609=*((char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((
_tmp60A='b',((_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'
 && _tmp60A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp60D;char _tmp60C;struct
_dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60C=*((
char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((_tmp60D='\\',((
_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'  && _tmp60D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}{char _tmp610;char
_tmp60F;struct _dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60F=*((char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((
_tmp610='f',((_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'
 && _tmp610 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp613;char _tmp612;struct
_dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp612=*((
char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((_tmp613='\\',((
_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'  && _tmp613 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}{char _tmp616;char
_tmp615;struct _dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp615=*((char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((
_tmp616='n',((_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'
 && _tmp616 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp619;char _tmp618;struct
_dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp618=*((
char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((_tmp619='\\',((
_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'  && _tmp619 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}{char _tmp61C;char
_tmp61B;struct _dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61B=*((char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((
_tmp61C='r',((_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'
 && _tmp61C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp61F;char _tmp61E;struct
_dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61E=*((
char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((_tmp61F='\\',((
_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'  && _tmp61F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}{char _tmp622;char
_tmp621;struct _dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp621=*((char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((
_tmp622='t',((_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'
 && _tmp622 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp625;char _tmp624;struct
_dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp624=*((
char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625='\\',((
_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}{char _tmp628;char
_tmp627;struct _dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp627=*((char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((
_tmp628='v',((_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'
 && _tmp628 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp62B;char _tmp62A;struct
_dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62A=*((
char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((_tmp62B='\\',((
_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'  && _tmp62B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}{char _tmp62E;char
_tmp62D;struct _dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62D=*((char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((
_tmp62E='\\',((_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'
 && _tmp62E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp631;char _tmp630;struct
_dyneither_ptr _tmp62F;(_tmp62F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp630=*((
char*)_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((_tmp631='\\',((
_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'  && _tmp631 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}{char _tmp634;char
_tmp633;struct _dyneither_ptr _tmp632;(_tmp632=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp633=*((char*)_check_dyneither_subscript(_tmp632,sizeof(char),0)),((
_tmp634='"',((_get_dyneither_size(_tmp632,sizeof(char))== 1  && (_tmp633 == '\000'
 && _tmp634 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp632.curr)=_tmp634)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp637;char _tmp636;
struct _dyneither_ptr _tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp636=*((char*)_check_dyneither_subscript(_tmp635,sizeof(char),0)),((_tmp637=
_tmp1E,((_get_dyneither_size(_tmp635,sizeof(char))== 1  && (_tmp636 == '\000'  && 
_tmp637 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp635.curr)=_tmp637)))))));}
else{{char _tmp63A;char _tmp639;struct _dyneither_ptr _tmp638;(_tmp638=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp639=*((char*)
_check_dyneither_subscript(_tmp638,sizeof(char),0)),((_tmp63A='\\',((
_get_dyneither_size(_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'  && _tmp63A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));}{char _tmp63D;char
_tmp63C;struct _dyneither_ptr _tmp63B;(_tmp63B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63C=*((char*)_check_dyneither_subscript(_tmp63B,sizeof(char),0)),((
_tmp63D=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp63B,sizeof(char))
== 1  && (_tmp63C == '\000'  && _tmp63D != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp63B.curr)=_tmp63D)))))));}{char _tmp640;char _tmp63F;struct _dyneither_ptr
_tmp63E;(_tmp63E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63F=*((char*)
_check_dyneither_subscript(_tmp63E,sizeof(char),0)),((_tmp640=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp63E,sizeof(char))== 1  && (_tmp63F == '\000'
 && _tmp640 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63E.curr)=_tmp640)))))));}{
char _tmp643;char _tmp642;struct _dyneither_ptr _tmp641;(_tmp641=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp642=*((char*)_check_dyneither_subscript(_tmp641,
sizeof(char),0)),((_tmp643=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp641,sizeof(char))== 1  && (_tmp642 == '\000'  && _tmp643 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp641.curr)=_tmp643)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp644;l=((_tmp644=_cycalloc(sizeof(*_tmp644)),((_tmp644->hd=
Cyc_Absynpp_restrict_sp,((_tmp644->tl=l,_tmp644))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp645;l=((_tmp645=_cycalloc(sizeof(*_tmp645)),((_tmp645->hd=Cyc_Absynpp_volatile_sp,((
_tmp645->tl=l,_tmp645))))));}if(tq.print_const){struct Cyc_List_List*_tmp646;l=((
_tmp646=_cycalloc(sizeof(*_tmp646)),((_tmp646->hd=Cyc_Absynpp_const_sp,((_tmp646->tl=
l,_tmp646))))));}{const char*_tmp649;const char*_tmp648;const char*_tmp647;return
Cyc_PP_egroup(((_tmp647="",_tag_dyneither(_tmp647,sizeof(char),1))),((_tmp648=" ",
_tag_dyneither(_tmp648,sizeof(char),2))),((_tmp649=" ",_tag_dyneither(_tmp649,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*
_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(
_tmp6F){case Cyc_Absyn_AnyKind: _LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp64A;return(_tmp64A="A",_tag_dyneither(_tmp64A,sizeof(char),2));}
case Cyc_Absyn_Unique: _LL39: {const char*_tmp64B;return(_tmp64B="UA",
_tag_dyneither(_tmp64B,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {const char*
_tmp64C;return(_tmp64C="TA",_tag_dyneither(_tmp64C,sizeof(char),3));}}case Cyc_Absyn_MemKind:
_LL37: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3D: {const char*_tmp64D;return(
_tmp64D="M",_tag_dyneither(_tmp64D,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp64E;return(_tmp64E="UM",_tag_dyneither(_tmp64E,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3F: {const char*_tmp64F;return(_tmp64F="TM",_tag_dyneither(
_tmp64F,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL42: {const char*_tmp650;return(_tmp650="B",_tag_dyneither(_tmp650,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL43: {const char*_tmp651;return(_tmp651="UB",
_tag_dyneither(_tmp651,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {const char*
_tmp652;return(_tmp652="TB",_tag_dyneither(_tmp652,sizeof(char),3));}}case Cyc_Absyn_RgnKind:
_LL41: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL47: {const char*_tmp653;return(
_tmp653="R",_tag_dyneither(_tmp653,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp654;return(_tmp654="UR",_tag_dyneither(_tmp654,sizeof(char),3));}
case Cyc_Absyn_Top: _LL49: {const char*_tmp655;return(_tmp655="TR",_tag_dyneither(
_tmp655,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {const char*_tmp656;
return(_tmp656="E",_tag_dyneither(_tmp656,sizeof(char),2));}case Cyc_Absyn_IntKind:
_LL4B: {const char*_tmp657;return(_tmp657="I",_tag_dyneither(_tmp657,sizeof(char),
2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k);struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp658;
return(_tmp658="{?}",_tag_dyneither(_tmp658,sizeof(char),4));}else{const char*
_tmp659;return(_tmp659="?",_tag_dyneither(_tmp659,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp65D;
void*_tmp65C[1];struct Cyc_String_pa_struct _tmp65B;return(struct _dyneither_ptr)((
_tmp65B.tag=0,((_tmp65B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp65C[0]=& _tmp65B,Cyc_aprintf(((_tmp65D="<=%s",_tag_dyneither(
_tmp65D,sizeof(char),5))),_tag_dyneither(_tmp65C,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp8A=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8C;struct
Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(
Cyc_Absynpp_kind2string(_tmp8C));_LL57: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}
_LL58: if(Cyc_PP_tex_output){const char*_tmp65E;return Cyc_PP_text_width(((_tmp65E="{?}",
_tag_dyneither(_tmp65E,sizeof(char),4))),1);}else{const char*_tmp65F;return Cyc_PP_text(((
_tmp65F="?",_tag_dyneither(_tmp65F,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp662;const char*_tmp661;
const char*_tmp660;return Cyc_PP_egroup(((_tmp660="<",_tag_dyneither(_tmp660,
sizeof(char),2))),((_tmp661=">",_tag_dyneither(_tmp661,sizeof(char),2))),((
_tmp662=",",_tag_dyneither(_tmp662,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp95=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind*_tmp9A;struct Cyc_Absyn_Kind*
_tmp9C;_LL5C: {struct Cyc_Absyn_Unknown_kb_struct*_tmp96=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp95;if(_tmp96->tag != 1)goto _LL5E;}_LL5D: goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp97=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95;if(_tmp97->tag != 0)goto _LL60;else{
_tmp98=_tmp97->f1;if(_tmp98 != Cyc_Absyn_BoxKind)goto _LL60;}}_LL5F: return Cyc_PP_textptr(
tv->name);_LL60: {struct Cyc_Absyn_Less_kb_struct*_tmp99=(struct Cyc_Absyn_Less_kb_struct*)
_tmp95;if(_tmp99->tag != 2)goto _LL62;else{_tmp9A=_tmp99->f2;}}_LL61: _tmp9C=_tmp9A;
goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_struct*_tmp9B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp95;if(_tmp9B->tag != 0)goto _LL5B;else{_tmp9C=_tmp9B->f1;}}_LL63: {const char*
_tmp665;struct Cyc_PP_Doc*_tmp664[3];return(_tmp664[2]=Cyc_Absynpp_kind2doc(
_tmp9C),((_tmp664[1]=Cyc_PP_text(((_tmp665="::",_tag_dyneither(_tmp665,sizeof(
char),3)))),((_tmp664[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp664,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*
tvs){const char*_tmp668;const char*_tmp667;const char*_tmp666;return Cyc_PP_egroup(((
_tmp666="<",_tag_dyneither(_tmp666,sizeof(char),2))),((_tmp667=">",
_tag_dyneither(_tmp667,sizeof(char),2))),((_tmp668=",",_tag_dyneither(_tmp668,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp66B;const char*_tmp66A;const char*
_tmp669;return Cyc_PP_egroup(((_tmp669="<",_tag_dyneither(_tmp669,sizeof(char),2))),((
_tmp66A=">",_tag_dyneither(_tmp66A,sizeof(char),2))),((_tmp66B=",",
_tag_dyneither(_tmp66B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple15*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts);
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp66E;
const char*_tmp66D;const char*_tmp66C;return Cyc_PP_group(((_tmp66C="(",
_tag_dyneither(_tmp66C,sizeof(char),2))),((_tmp66D=")",_tag_dyneither(_tmp66D,
sizeof(char),2))),((_tmp66E=",",_tag_dyneither(_tmp66E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmpA8=att;
_LL65: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA9=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpA8;if(_tmpA9->tag != 1)goto _LL67;}_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAA=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA8;if(_tmpAA->tag != 2)goto _LL69;}
_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_struct*_tmpAB=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpA8;if(_tmpAB->tag != 3)goto _LL6B;}_LL6A: return Cyc_PP_nil_doc();_LL6B:;_LL6C:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmpAC=(void*)atts->hd;_LL6E: {struct
Cyc_Absyn_Stdcall_att_struct*_tmpAD=(struct Cyc_Absyn_Stdcall_att_struct*)_tmpAC;
if(_tmpAD->tag != 1)goto _LL70;}_LL6F: {const char*_tmp66F;return Cyc_PP_text(((
_tmp66F=" _stdcall ",_tag_dyneither(_tmp66F,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpAE=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAC;if(
_tmpAE->tag != 2)goto _LL72;}_LL71: {const char*_tmp670;return Cyc_PP_text(((_tmp670=" _cdecl ",
_tag_dyneither(_tmp670,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpAF=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAC;if(_tmpAF->tag != 3)goto _LL74;}
_LL73: {const char*_tmp671;return Cyc_PP_text(((_tmp671=" _fastcall ",
_tag_dyneither(_tmp671,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmpB3=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB4=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB3;if(_tmpB4->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB5=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB3;if(_tmpB5->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB6=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB3;if(_tmpB6->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp67C;const char*_tmp67B;const char*_tmp67A;const char*_tmp679;const
char*_tmp678;struct Cyc_PP_Doc*_tmp677[3];return(_tmp677[2]=Cyc_PP_text(((_tmp678=")",
_tag_dyneither(_tmp678,sizeof(char),2)))),((_tmp677[1]=Cyc_PP_group(((_tmp67B="",
_tag_dyneither(_tmp67B,sizeof(char),1))),((_tmp67A="",_tag_dyneither(_tmp67A,
sizeof(char),1))),((_tmp679=" ",_tag_dyneither(_tmp679,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp677[0]=Cyc_PP_text(((_tmp67C=" __declspec(",
_tag_dyneither(_tmp67C,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp677,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(
atts);case Cyc_Cyclone_Gcc_c: _LL80: {const char*_tmp685;const char*_tmp684;const
char*_tmp683;const char*_tmp682;struct Cyc_PP_Doc*_tmp681[2];return(_tmp681[1]=Cyc_PP_group(((
_tmp684="((",_tag_dyneither(_tmp684,sizeof(char),3))),((_tmp683="))",
_tag_dyneither(_tmp683,sizeof(char),3))),((_tmp682=",",_tag_dyneither(_tmp682,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp681[0]=Cyc_PP_text(((
_tmp685=" __attribute__",_tag_dyneither(_tmp685,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp681,sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpC2=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC3=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC2;if(_tmpC3->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC4=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC2;if(_tmpC4->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question();static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){const char*
_tmp686;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp686="{?}",_tag_dyneither(_tmp686,sizeof(char),4))),1);}else{const char*
_tmp687;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp687="?",
_tag_dyneither(_tmp687,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp688;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp688="{\\lb}",
_tag_dyneither(_tmp688,sizeof(char),6))),1);}else{const char*_tmp689;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp689="{",_tag_dyneither(_tmp689,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp68A;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp68A="{\\rb}",_tag_dyneither(_tmp68A,sizeof(char),6))),1);}else{const char*
_tmp68B;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68B="}",
_tag_dyneither(_tmp68B,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp68C;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp68C="\\$",
_tag_dyneither(_tmp68C,sizeof(char),3))),1);}else{const char*_tmp68D;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp68D="$",_tag_dyneither(_tmp68D,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp68E[3];return(_tmp68E[2]=Cyc_Absynpp_rb(),((_tmp68E[1]=
Cyc_PP_seq(sep,ss),((_tmp68E[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp68E,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpCE=(void*)tms->hd;void*_tmpD2;struct Cyc_List_List*
_tmpD4;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*_tmpCF=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpCE;if(_tmpCF->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp691;void*_tmp690;(
_tmp690=0,Cyc_fprintf(Cyc_stderr,((_tmp691="Carray_mod ",_tag_dyneither(_tmp691,
sizeof(char),12))),_tag_dyneither(_tmp690,sizeof(void*),0)));}goto _LL8C;_LL8F: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpD0=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCE;if(_tmpD0->tag != 1)goto _LL91;}_LL90:{const char*_tmp694;void*_tmp693;(
_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="ConstArray_mod ",_tag_dyneither(
_tmp694,sizeof(char),16))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD1=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCE;if(_tmpD1->tag != 3)goto _LL93;else{_tmpD2=(void*)_tmpD1->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD3=(struct Cyc_Absyn_WithTypes_struct*)_tmpD2;if(_tmpD3->tag != 1)goto _LL93;
else{_tmpD4=_tmpD3->f1;}};}}_LL92:{const char*_tmp697;void*_tmp696;(_tmp696=0,Cyc_fprintf(
Cyc_stderr,((_tmp697="Function_mod(",_tag_dyneither(_tmp697,sizeof(char),14))),
_tag_dyneither(_tmp696,sizeof(void*),0)));}for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){
struct Cyc_Core_Opt*_tmpDF=(*((struct _tuple7*)_tmpD4->hd)).f1;if(_tmpDF == 0){
const char*_tmp69A;void*_tmp699;(_tmp699=0,Cyc_fprintf(Cyc_stderr,((_tmp69A="?",
_tag_dyneither(_tmp69A,sizeof(char),2))),_tag_dyneither(_tmp699,sizeof(void*),0)));}
else{void*_tmp69B;(_tmp69B=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpDF->v),_tag_dyneither(_tmp69B,sizeof(void*),0)));}if(_tmpD4->tl != 0){const
char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E=",",
_tag_dyneither(_tmp69E,sizeof(char),2))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}}{
const char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,((_tmp6A1=") ",
_tag_dyneither(_tmp6A1,sizeof(char),3))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCE;if(_tmpD5->tag != 3)goto _LL95;}_LL94:{const char*_tmp6A4;void*_tmp6A3;(
_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="Function_mod()",_tag_dyneither(
_tmp6A4,sizeof(char),15))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD6=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCE;if(_tmpD6->tag != 5)goto _LL97;}_LL96:{const char*_tmp6A7;void*_tmp6A6;(
_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="Attributes_mod ",_tag_dyneither(
_tmp6A7,sizeof(char),16))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpD7=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCE;if(_tmpD7->tag != 4)goto _LL99;}_LL98:{const char*_tmp6AA;void*_tmp6A9;(
_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="TypeParams_mod ",_tag_dyneither(
_tmp6AA,sizeof(char),16))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpD8=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCE;if(_tmpD8->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6AD;void*_tmp6AC;(
_tmp6AC=0,Cyc_fprintf(Cyc_stderr,((_tmp6AD="Pointer_mod ",_tag_dyneither(_tmp6AD,
sizeof(char),13))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6B0;void*_tmp6AF;(_tmp6AF=0,Cyc_fprintf(Cyc_stderr,((_tmp6B0="\n",
_tag_dyneither(_tmp6B0,sizeof(char),2))),_tag_dyneither(_tmp6AF,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6B5;const char*_tmp6B4;struct Cyc_PP_Doc*
_tmp6B3[3];struct Cyc_PP_Doc*p_rest=(_tmp6B3[2]=Cyc_PP_text(((_tmp6B4=")",
_tag_dyneither(_tmp6B4,sizeof(char),2)))),((_tmp6B3[1]=rest,((_tmp6B3[0]=Cyc_PP_text(((
_tmp6B5="(",_tag_dyneither(_tmp6B5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6B3,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF1=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;void*_tmpF8;
struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_Position_Segment*
_tmpFD;int _tmpFE;struct Cyc_Absyn_PtrAtts _tmp100;void*_tmp101;union Cyc_Absyn_Constraint*
_tmp102;union Cyc_Absyn_Constraint*_tmp103;union Cyc_Absyn_Constraint*_tmp104;
struct Cyc_Absyn_Tqual _tmp105;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF2=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF1;if(_tmpF2->tag != 0)goto _LL9E;else{
_tmpF3=_tmpF2->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6BA;const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[2];return(_tmp6B8[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3)?Cyc_PP_text(((
_tmp6B9="[]@zeroterm",_tag_dyneither(_tmp6B9,sizeof(char),12)))): Cyc_PP_text(((
_tmp6BA="[]",_tag_dyneither(_tmp6BA,sizeof(char),3)))),((_tmp6B8[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF4=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF1;if(_tmpF4->tag != 1)goto
_LLA0;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6C1;const char*_tmp6C0;const char*_tmp6BF;
struct Cyc_PP_Doc*_tmp6BE[4];return(_tmp6BE[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((_tmp6BF="]@zeroterm",
_tag_dyneither(_tmp6BF,sizeof(char),11)))): Cyc_PP_text(((_tmp6C0="]",
_tag_dyneither(_tmp6C0,sizeof(char),2)))),((_tmp6BE[2]=Cyc_Absynpp_exp2doc(
_tmpF5),((_tmp6BE[1]=Cyc_PP_text(((_tmp6C1="[",_tag_dyneither(_tmp6C1,sizeof(
char),2)))),((_tmp6BE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpF7=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF1;if(_tmpF7->tag != 3)goto _LLA2;else{_tmpF8=(void*)_tmpF7->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp10D=_tmpF8;struct Cyc_List_List*_tmp10F;int _tmp110;
struct Cyc_Absyn_VarargInfo*_tmp111;struct Cyc_Core_Opt*_tmp112;struct Cyc_List_List*
_tmp113;struct Cyc_List_List*_tmp115;struct Cyc_Position_Segment*_tmp116;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp10E=(struct Cyc_Absyn_WithTypes_struct*)
_tmp10D;if(_tmp10E->tag != 1)goto _LLAB;else{_tmp10F=_tmp10E->f1;_tmp110=_tmp10E->f2;
_tmp111=_tmp10E->f3;_tmp112=_tmp10E->f4;_tmp113=_tmp10E->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6C2[2];return(_tmp6C2[1]=Cyc_Absynpp_funargs2doc(_tmp10F,_tmp110,_tmp111,
_tmp112,_tmp113),((_tmp6C2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C2,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp114=(
struct Cyc_Absyn_NoTypes_struct*)_tmp10D;if(_tmp114->tag != 0)goto _LLA8;else{
_tmp115=_tmp114->f1;_tmp116=_tmp114->f2;}}_LLAC: {const char*_tmp6C9;const char*
_tmp6C8;const char*_tmp6C7;struct Cyc_PP_Doc*_tmp6C6[2];return(_tmp6C6[1]=Cyc_PP_group(((
_tmp6C9="(",_tag_dyneither(_tmp6C9,sizeof(char),2))),((_tmp6C8=")",
_tag_dyneither(_tmp6C8,sizeof(char),2))),((_tmp6C7=",",_tag_dyneither(_tmp6C7,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp115)),((
_tmp6C6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6C6,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpF9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF1;if(_tmpF9->tag != 5)goto _LLA4;else{_tmpFA=_tmpF9->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6CA[2];return(_tmp6CA[1]=Cyc_Absynpp_atts2doc(_tmpFA),((
_tmp6CA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CA,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6CB[2];return(_tmp6CB[1]=rest,((_tmp6CB[0]=Cyc_Absynpp_callconv2doc(_tmpFA),
Cyc_PP_cat(_tag_dyneither(_tmp6CB,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFB=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF1;if(_tmpFB->tag != 4)goto _LLA6;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f2;
_tmpFE=_tmpFB->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpFE){struct Cyc_PP_Doc*_tmp6CC[2];return(_tmp6CC[1]=Cyc_Absynpp_ktvars2doc(
_tmpFC),((_tmp6CC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CC,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6CD[2];return(_tmp6CD[1]=Cyc_Absynpp_tvars2doc(
_tmpFC),((_tmp6CD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CD,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmpFF=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF1;if(_tmpFF->tag != 2)goto _LL9B;else{_tmp100=_tmpFF->f1;_tmp101=_tmp100.rgn;
_tmp102=_tmp100.nullable;_tmp103=_tmp100.bounds;_tmp104=_tmp100.zero_term;
_tmp105=_tmpFF->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp120=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp103);struct Cyc_Absyn_Exp*_tmp123;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp121=(struct Cyc_Absyn_DynEither_b_struct*)_tmp120;if(_tmp121->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp122=(struct Cyc_Absyn_Upper_b_struct*)_tmp120;if(_tmp122->tag != 1)goto _LLB0;
else{_tmp123=_tmp122->f1;}}_LLB4:{const char*_tmp6CF;const char*_tmp6CE;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp102)?(
_tmp6CF="*",_tag_dyneither(_tmp6CF,sizeof(char),2)):((_tmp6CE="@",_tag_dyneither(
_tmp6CE,sizeof(char),2))));}{unsigned int _tmp127;int _tmp128;struct _tuple10
_tmp126=Cyc_Evexp_eval_const_uint_exp(_tmp123);_tmp127=_tmp126.f1;_tmp128=
_tmp126.f2;if(!_tmp128  || _tmp127 != 1){struct Cyc_PP_Doc*_tmp6D0[4];ptr=((_tmp6D0[
3]=Cyc_Absynpp_rb(),((_tmp6D0[2]=Cyc_Absynpp_exp2doc(_tmp123),((_tmp6D0[1]=Cyc_Absynpp_lb(),((
_tmp6D0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6D0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp104)){const char*
_tmp6D3;struct Cyc_PP_Doc*_tmp6D2[2];ptr=((_tmp6D2[1]=Cyc_PP_text(((_tmp6D3="@zeroterm ",
_tag_dyneither(_tmp6D3,sizeof(char),11)))),((_tmp6D2[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6D2,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp104)){
const char*_tmp6D6;struct Cyc_PP_Doc*_tmp6D5[2];ptr=((_tmp6D5[1]=Cyc_PP_text(((
_tmp6D6="@nozeroterm ",_tag_dyneither(_tmp6D6,sizeof(char),13)))),((_tmp6D5[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6D5,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp12E=Cyc_Tcutil_compress(_tmp101);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp12F=(struct Cyc_Absyn_HeapRgn_struct*)_tmp12E;if(_tmp12F->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp130=(struct Cyc_Absyn_Evar_struct*)
_tmp12E;if(_tmp130->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp6D9;struct Cyc_PP_Doc*_tmp6D8[
3];ptr=((_tmp6D8[2]=Cyc_PP_text(((_tmp6D9=" ",_tag_dyneither(_tmp6D9,sizeof(char),
2)))),((_tmp6D8[1]=Cyc_Absynpp_typ2doc(_tmp101),((_tmp6D8[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6D8,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp6DA[2];ptr=((_tmp6DA[1]=Cyc_Absynpp_tqual2doc(_tmp105),((_tmp6DA[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6DA,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6DB[2];return(_tmp6DB[1]=rest,((_tmp6DB[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6DB,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp138=Cyc_Tcutil_compress(
t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*_tmp139=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp138;if(_tmp139->tag != 21)goto _LLBF;}_LLBE: {const char*_tmp6DC;return Cyc_PP_text(((
_tmp6DC="`H",_tag_dyneither(_tmp6DC,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp13A=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp138;if(_tmp13A->tag != 22)goto
_LLC1;}_LLC0: {const char*_tmp6DD;return Cyc_PP_text(((_tmp6DD="`U",_tag_dyneither(
_tmp6DD,sizeof(char),3))));}_LLC1:;_LLC2: return Cyc_Absynpp_ntyp2doc(t);_LLBC:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp13D=Cyc_Tcutil_compress(t);void*
_tmp13F;struct Cyc_List_List*_tmp141;_LLC4: {struct Cyc_Absyn_AccessEff_struct*
_tmp13E=(struct Cyc_Absyn_AccessEff_struct*)_tmp13D;if(_tmp13E->tag != 23)goto
_LLC6;else{_tmp13F=(void*)_tmp13E->f1;}}_LLC5:{struct Cyc_List_List*_tmp6DE;*
rgions=((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE->hd=Cyc_Absynpp_rgn2doc(
_tmp13F),((_tmp6DE->tl=*rgions,_tmp6DE))))));}goto _LLC3;_LLC6: {struct Cyc_Absyn_JoinEff_struct*
_tmp140=(struct Cyc_Absyn_JoinEff_struct*)_tmp13D;if(_tmp140->tag != 24)goto _LLC8;
else{_tmp141=_tmp140->f1;}}_LLC7: for(0;_tmp141 != 0;_tmp141=_tmp141->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp141->hd);}goto _LLC3;_LLC8:;_LLC9:{struct Cyc_List_List*
_tmp6DF;*effects=((_tmp6DF=_cycalloc(sizeof(*_tmp6DF)),((_tmp6DF->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6DF->tl=*effects,_tmp6DF))))));}goto _LLC3;_LLC3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6E2;const char*_tmp6E1;const
char*_tmp6E0;return Cyc_PP_group(((_tmp6E0="",_tag_dyneither(_tmp6E0,sizeof(char),
1))),((_tmp6E1="",_tag_dyneither(_tmp6E1,sizeof(char),1))),((_tmp6E2="+",
_tag_dyneither(_tmp6E2,sizeof(char),2))),effects);}else{const char*_tmp6E3;struct
Cyc_PP_Doc*_tmp147=Cyc_Absynpp_group_braces(((_tmp6E3=",",_tag_dyneither(_tmp6E3,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6E7;const char*_tmp6E6;const
char*_tmp6E5;const char*_tmp6E4;return Cyc_PP_group(((_tmp6E4="",_tag_dyneither(
_tmp6E4,sizeof(char),1))),((_tmp6E5="",_tag_dyneither(_tmp6E5,sizeof(char),1))),((
_tmp6E6="+",_tag_dyneither(_tmp6E6,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6E7=_cycalloc(sizeof(*_tmp6E7)),((_tmp6E7->hd=_tmp147,((_tmp6E7->tl=0,
_tmp6E7))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLCA: {const char*_tmp6E8;return Cyc_PP_text(((_tmp6E8="struct ",
_tag_dyneither(_tmp6E8,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCB: {const
char*_tmp6E9;return Cyc_PP_text(((_tmp6E9="union ",_tag_dyneither(_tmp6E9,sizeof(
char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp14F=t;struct Cyc_Core_Opt*
_tmp155;struct Cyc_Core_Opt*_tmp156;int _tmp157;struct Cyc_Core_Opt*_tmp158;struct
Cyc_Absyn_Tvar*_tmp15A;struct Cyc_Absyn_DatatypeInfo _tmp15C;union Cyc_Absyn_DatatypeInfoU
_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_DatatypeFieldInfo _tmp160;
union Cyc_Absyn_DatatypeFieldInfoU _tmp161;struct Cyc_List_List*_tmp162;enum Cyc_Absyn_Sign
_tmp164;enum Cyc_Absyn_Size_of _tmp165;int _tmp168;struct Cyc_List_List*_tmp16A;
struct Cyc_Absyn_AggrInfo _tmp16C;union Cyc_Absyn_AggrInfoU _tmp16D;struct Cyc_List_List*
_tmp16E;enum Cyc_Absyn_AggrKind _tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*
_tmp173;struct _tuple0*_tmp175;struct Cyc_Absyn_Exp*_tmp177;struct _tuple0*_tmp179;
struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Typedefdecl*_tmp17B;void*_tmp17D;
void*_tmp17F;void*_tmp180;void*_tmp182;void*_tmp186;_LLCE: {struct Cyc_Absyn_ArrayType_struct*
_tmp150=(struct Cyc_Absyn_ArrayType_struct*)_tmp14F;if(_tmp150->tag != 9)goto _LLD0;}
_LLCF: {const char*_tmp6EA;return Cyc_PP_text(((_tmp6EA="[[[array]]]",
_tag_dyneither(_tmp6EA,sizeof(char),12))));}_LLD0: {struct Cyc_Absyn_FnType_struct*
_tmp151=(struct Cyc_Absyn_FnType_struct*)_tmp14F;if(_tmp151->tag != 10)goto _LLD2;}
_LLD1: return Cyc_PP_nil_doc();_LLD2: {struct Cyc_Absyn_PointerType_struct*_tmp152=(
struct Cyc_Absyn_PointerType_struct*)_tmp14F;if(_tmp152->tag != 5)goto _LLD4;}_LLD3:
return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_VoidType_struct*_tmp153=(struct
Cyc_Absyn_VoidType_struct*)_tmp14F;if(_tmp153->tag != 0)goto _LLD6;}_LLD5:{const
char*_tmp6EB;s=Cyc_PP_text(((_tmp6EB="void",_tag_dyneither(_tmp6EB,sizeof(char),
5))));}goto _LLCD;_LLD6: {struct Cyc_Absyn_Evar_struct*_tmp154=(struct Cyc_Absyn_Evar_struct*)
_tmp14F;if(_tmp154->tag != 1)goto _LLD8;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;
_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;}}_LLD7: if(_tmp156 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp156->v);else{const char*_tmp6FB;const char*_tmp6FA;const char*_tmp6F9;
struct Cyc_Int_pa_struct _tmp6F8;void*_tmp6F7[1];const char*_tmp6F6;const char*
_tmp6F5;struct Cyc_PP_Doc*_tmp6F4[6];s=((_tmp6F4[5]=_tmp155 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp155->v),((_tmp6F4[4]=Cyc_PP_text(((
_tmp6F5=")::",_tag_dyneither(_tmp6F5,sizeof(char),4)))),((_tmp6F4[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp158 == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp158->v),((_tmp6F4[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6F8.tag=1,((
_tmp6F8.f1=(unsigned long)_tmp157,((_tmp6F7[0]=& _tmp6F8,Cyc_aprintf(((_tmp6F6="%d",
_tag_dyneither(_tmp6F6,sizeof(char),3))),_tag_dyneither(_tmp6F7,sizeof(void*),1))))))))),((
_tmp6F4[1]=Cyc_PP_text(((_tmp6F9="(",_tag_dyneither(_tmp6F9,sizeof(char),2)))),((
_tmp6F4[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6FA="\\%",
_tag_dyneither(_tmp6FA,sizeof(char),3)))): Cyc_PP_text(((_tmp6FB="%",
_tag_dyneither(_tmp6FB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6F4,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: {struct Cyc_Absyn_VarType_struct*
_tmp159=(struct Cyc_Absyn_VarType_struct*)_tmp14F;if(_tmp159->tag != 2)goto _LLDA;
else{_tmp15A=_tmp159->f1;}}_LLD9: s=Cyc_PP_textptr(_tmp15A->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[3];s=((_tmp6FD[2]=Cyc_Absynpp_kindbound2doc(
_tmp15A->kind),((_tmp6FD[1]=Cyc_PP_text(((_tmp6FE="::",_tag_dyneither(_tmp6FE,
sizeof(char),3)))),((_tmp6FD[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6FD,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15A)){const char*_tmp703;const char*_tmp702;struct Cyc_PP_Doc*_tmp701[3];s=((
_tmp701[2]=Cyc_PP_text(((_tmp702=" */",_tag_dyneither(_tmp702,sizeof(char),4)))),((
_tmp701[1]=s,((_tmp701[0]=Cyc_PP_text(((_tmp703="_ /* ",_tag_dyneither(_tmp703,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: {struct Cyc_Absyn_DatatypeType_struct*_tmp15B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp14F;if(_tmp15B->tag != 3)goto _LLDC;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15C.datatype_info;
_tmp15E=_tmp15C.targs;}}_LLDB:{union Cyc_Absyn_DatatypeInfoU _tmp198=_tmp15D;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp199;struct _tuple0*_tmp19A;int _tmp19B;
struct Cyc_Absyn_Datatypedecl**_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp19D;
struct Cyc_Absyn_Datatypedecl _tmp19E;struct _tuple0*_tmp19F;int _tmp1A0;_LL103: if((
_tmp198.UnknownDatatype).tag != 1)goto _LL105;_tmp199=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp198.UnknownDatatype).val;_tmp19A=_tmp199.name;_tmp19B=_tmp199.is_extensible;
_LL104: _tmp19F=_tmp19A;_tmp1A0=_tmp19B;goto _LL106;_LL105: if((_tmp198.KnownDatatype).tag
!= 2)goto _LL102;_tmp19C=(struct Cyc_Absyn_Datatypedecl**)(_tmp198.KnownDatatype).val;
_tmp19D=*_tmp19C;_tmp19E=*_tmp19D;_tmp19F=_tmp19E.name;_tmp1A0=_tmp19E.is_extensible;
_LL106: {const char*_tmp704;struct Cyc_PP_Doc*_tmp1A1=Cyc_PP_text(((_tmp704="datatype ",
_tag_dyneither(_tmp704,sizeof(char),10))));const char*_tmp705;struct Cyc_PP_Doc*
_tmp1A2=_tmp1A0?Cyc_PP_text(((_tmp705="@extensible ",_tag_dyneither(_tmp705,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp706[4];s=((_tmp706[3]=
Cyc_Absynpp_tps2doc(_tmp15E),((_tmp706[2]=Cyc_Absynpp_qvar2doc(_tmp19F),((
_tmp706[1]=_tmp1A1,((_tmp706[0]=_tmp1A2,Cyc_PP_cat(_tag_dyneither(_tmp706,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp15F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp14F;if(_tmp15F->tag != 4)goto _LLDE;else{_tmp160=_tmp15F->f1;_tmp161=_tmp160.field_info;
_tmp162=_tmp160.targs;}}_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1A6=_tmp161;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1A7;struct _tuple0*_tmp1A8;struct
_tuple0*_tmp1A9;int _tmp1AA;struct _tuple1 _tmp1AB;struct Cyc_Absyn_Datatypedecl*
_tmp1AC;struct Cyc_Absyn_Datatypedecl _tmp1AD;struct _tuple0*_tmp1AE;int _tmp1AF;
struct Cyc_Absyn_Datatypefield*_tmp1B0;struct Cyc_Absyn_Datatypefield _tmp1B1;
struct _tuple0*_tmp1B2;_LL108: if((_tmp1A6.UnknownDatatypefield).tag != 1)goto
_LL10A;_tmp1A7=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1A6.UnknownDatatypefield).val;
_tmp1A8=_tmp1A7.datatype_name;_tmp1A9=_tmp1A7.field_name;_tmp1AA=_tmp1A7.is_extensible;
_LL109: _tmp1AE=_tmp1A8;_tmp1AF=_tmp1AA;_tmp1B2=_tmp1A9;goto _LL10B;_LL10A: if((
_tmp1A6.KnownDatatypefield).tag != 2)goto _LL107;_tmp1AB=(struct _tuple1)(_tmp1A6.KnownDatatypefield).val;
_tmp1AC=_tmp1AB.f1;_tmp1AD=*_tmp1AC;_tmp1AE=_tmp1AD.name;_tmp1AF=_tmp1AD.is_extensible;
_tmp1B0=_tmp1AB.f2;_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.name;_LL10B: {const char*
_tmp708;const char*_tmp707;struct Cyc_PP_Doc*_tmp1B3=Cyc_PP_text(_tmp1AF?(_tmp708="@extensible datatype ",
_tag_dyneither(_tmp708,sizeof(char),22)):((_tmp707="datatype ",_tag_dyneither(
_tmp707,sizeof(char),10))));{const char*_tmp70B;struct Cyc_PP_Doc*_tmp70A[4];s=((
_tmp70A[3]=Cyc_Absynpp_qvar2doc(_tmp1B2),((_tmp70A[2]=Cyc_PP_text(((_tmp70B=".",
_tag_dyneither(_tmp70B,sizeof(char),2)))),((_tmp70A[1]=Cyc_Absynpp_qvar2doc(
_tmp1AE),((_tmp70A[0]=_tmp1B3,Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL107;}_LL107:;}goto _LLCD;_LLDE: {struct Cyc_Absyn_IntType_struct*
_tmp163=(struct Cyc_Absyn_IntType_struct*)_tmp14F;if(_tmp163->tag != 6)goto _LLE0;
else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp164){case Cyc_Absyn_None: _LL10C: goto _LL10D;case
Cyc_Absyn_Signed: _LL10D:{const char*_tmp70C;sns=((_tmp70C="",_tag_dyneither(
_tmp70C,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL10E:{const char*
_tmp70D;sns=((_tmp70D="unsigned ",_tag_dyneither(_tmp70D,sizeof(char),10)));}
break;}switch(_tmp165){case Cyc_Absyn_Char_sz: _LL110: switch(_tmp164){case Cyc_Absyn_None:
_LL112:{const char*_tmp70E;sns=((_tmp70E="",_tag_dyneither(_tmp70E,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL113:{const char*_tmp70F;sns=((_tmp70F="signed ",
_tag_dyneither(_tmp70F,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL114:{
const char*_tmp710;sns=((_tmp710="unsigned ",_tag_dyneither(_tmp710,sizeof(char),
10)));}break;}{const char*_tmp711;ts=((_tmp711="char",_tag_dyneither(_tmp711,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL111:{const char*_tmp712;ts=((
_tmp712="short",_tag_dyneither(_tmp712,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL116:{const char*_tmp713;ts=((_tmp713="int",_tag_dyneither(_tmp713,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL117:{const char*_tmp714;ts=((_tmp714="long",
_tag_dyneither(_tmp714,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL118:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL11A:{const char*_tmp715;
ts=((_tmp715="long long",_tag_dyneither(_tmp715,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL11B:{const char*_tmp716;ts=((_tmp716="__int64",_tag_dyneither(
_tmp716,sizeof(char),8)));}break;}break;}{const char*_tmp71B;void*_tmp71A[2];
struct Cyc_String_pa_struct _tmp719;struct Cyc_String_pa_struct _tmp718;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp71A[0]=& _tmp719,((_tmp71A[1]=& _tmp718,Cyc_aprintf(((
_tmp71B="%s%s",_tag_dyneither(_tmp71B,sizeof(char),5))),_tag_dyneither(_tmp71A,
sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: {struct Cyc_Absyn_FloatType_struct*
_tmp166=(struct Cyc_Absyn_FloatType_struct*)_tmp14F;if(_tmp166->tag != 7)goto _LLE2;}
_LLE1:{const char*_tmp71C;s=Cyc_PP_text(((_tmp71C="float",_tag_dyneither(_tmp71C,
sizeof(char),6))));}goto _LLCD;_LLE2: {struct Cyc_Absyn_DoubleType_struct*_tmp167=(
struct Cyc_Absyn_DoubleType_struct*)_tmp14F;if(_tmp167->tag != 8)goto _LLE4;else{
_tmp168=_tmp167->f1;}}_LLE3: if(_tmp168){const char*_tmp71D;s=Cyc_PP_text(((
_tmp71D="long double",_tag_dyneither(_tmp71D,sizeof(char),12))));}else{const char*
_tmp71E;s=Cyc_PP_text(((_tmp71E="double",_tag_dyneither(_tmp71E,sizeof(char),7))));}
goto _LLCD;_LLE4: {struct Cyc_Absyn_TupleType_struct*_tmp169=(struct Cyc_Absyn_TupleType_struct*)
_tmp14F;if(_tmp169->tag != 11)goto _LLE6;else{_tmp16A=_tmp169->f1;}}_LLE5:{struct
Cyc_PP_Doc*_tmp71F[2];s=((_tmp71F[1]=Cyc_Absynpp_args2doc(_tmp16A),((_tmp71F[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp71F,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCD;_LLE6: {struct Cyc_Absyn_AggrType_struct*_tmp16B=(struct Cyc_Absyn_AggrType_struct*)
_tmp14F;if(_tmp16B->tag != 12)goto _LLE8;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16C.aggr_info;
_tmp16E=_tmp16C.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1CC;struct _tuple0*
_tmp1CD;struct _tuple9 _tmp1CB=Cyc_Absyn_aggr_kinded_name(_tmp16D);_tmp1CC=_tmp1CB.f1;
_tmp1CD=_tmp1CB.f2;{struct Cyc_PP_Doc*_tmp720[3];s=((_tmp720[2]=Cyc_Absynpp_tps2doc(
_tmp16E),((_tmp720[1]=Cyc_Absynpp_qvar2doc(_tmp1CD),((_tmp720[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1CC),Cyc_PP_cat(_tag_dyneither(_tmp720,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp16F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14F;if(_tmp16F->tag != 13)goto _LLEA;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp721[4];s=((_tmp721[3]=Cyc_Absynpp_rb(),((_tmp721[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp171)),((_tmp721[1]=Cyc_Absynpp_lb(),((
_tmp721[0]=Cyc_Absynpp_aggr_kind2doc(_tmp170),Cyc_PP_cat(_tag_dyneither(_tmp721,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp172=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp14F;if(_tmp172->tag != 15)goto
_LLEC;else{_tmp173=_tmp172->f1;}}_LLEB:{const char*_tmp724;struct Cyc_PP_Doc*
_tmp723[4];s=((_tmp723[3]=Cyc_Absynpp_rb(),((_tmp723[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp173)),((_tmp723[1]=Cyc_Absynpp_lb(),((_tmp723[0]=Cyc_PP_text(((_tmp724="enum ",
_tag_dyneither(_tmp724,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp723,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp174=(struct Cyc_Absyn_EnumType_struct*)_tmp14F;if(_tmp174->tag != 14)goto _LLEE;
else{_tmp175=_tmp174->f1;}}_LLED:{const char*_tmp727;struct Cyc_PP_Doc*_tmp726[2];
s=((_tmp726[1]=Cyc_Absynpp_qvar2doc(_tmp175),((_tmp726[0]=Cyc_PP_text(((_tmp727="enum ",
_tag_dyneither(_tmp727,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp726,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp176=(struct Cyc_Absyn_ValueofType_struct*)_tmp14F;if(_tmp176->tag != 19)goto
_LLF0;else{_tmp177=_tmp176->f1;}}_LLEF:{const char*_tmp72C;const char*_tmp72B;
struct Cyc_PP_Doc*_tmp72A[3];s=((_tmp72A[2]=Cyc_PP_text(((_tmp72B=")",
_tag_dyneither(_tmp72B,sizeof(char),2)))),((_tmp72A[1]=Cyc_Absynpp_exp2doc(
_tmp177),((_tmp72A[0]=Cyc_PP_text(((_tmp72C="valueof_t(",_tag_dyneither(_tmp72C,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp72A,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp178=(struct
Cyc_Absyn_TypedefType_struct*)_tmp14F;if(_tmp178->tag != 18)goto _LLF2;else{
_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp72D[2];s=((_tmp72D[1]=Cyc_Absynpp_tps2doc(_tmp17A),((_tmp72D[0]=Cyc_Absynpp_qvar2doc(
_tmp179),Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCD;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp17C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp14F;if(_tmp17C->tag != 16)goto _LLF4;else{_tmp17D=(void*)_tmp17C->f1;}}_LLF3:{
const char*_tmp732;const char*_tmp731;struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=
Cyc_PP_text(((_tmp731=">",_tag_dyneither(_tmp731,sizeof(char),2)))),((_tmp730[1]=
Cyc_Absynpp_rgn2doc(_tmp17D),((_tmp730[0]=Cyc_PP_text(((_tmp732="region_t<",
_tag_dyneither(_tmp732,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp730,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp17E=(struct Cyc_Absyn_DynRgnType_struct*)_tmp14F;if(_tmp17E->tag != 17)goto
_LLF6;else{_tmp17F=(void*)_tmp17E->f1;_tmp180=(void*)_tmp17E->f2;}}_LLF5:{const
char*_tmp739;const char*_tmp738;const char*_tmp737;struct Cyc_PP_Doc*_tmp736[5];s=((
_tmp736[4]=Cyc_PP_text(((_tmp737=">",_tag_dyneither(_tmp737,sizeof(char),2)))),((
_tmp736[3]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp736[2]=Cyc_PP_text(((_tmp738=",",
_tag_dyneither(_tmp738,sizeof(char),2)))),((_tmp736[1]=Cyc_Absynpp_rgn2doc(
_tmp17F),((_tmp736[0]=Cyc_PP_text(((_tmp739="dynregion_t<",_tag_dyneither(
_tmp739,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp736,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp181=(struct
Cyc_Absyn_TagType_struct*)_tmp14F;if(_tmp181->tag != 20)goto _LLF8;else{_tmp182=(
void*)_tmp181->f1;}}_LLF7:{const char*_tmp73E;const char*_tmp73D;struct Cyc_PP_Doc*
_tmp73C[3];s=((_tmp73C[2]=Cyc_PP_text(((_tmp73D=">",_tag_dyneither(_tmp73D,
sizeof(char),2)))),((_tmp73C[1]=Cyc_Absynpp_typ2doc(_tmp182),((_tmp73C[0]=Cyc_PP_text(((
_tmp73E="tag_t<",_tag_dyneither(_tmp73E,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp73C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp183=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp14F;if(_tmp183->tag != 22)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp184=(struct Cyc_Absyn_HeapRgn_struct*)_tmp14F;if(_tmp184->tag != 21)goto _LLFC;}
_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp185=(struct Cyc_Absyn_RgnsEff_struct*)_tmp14F;if(_tmp185->tag != 25)goto _LLFE;
else{_tmp186=(void*)_tmp185->f1;}}_LLFD:{const char*_tmp743;const char*_tmp742;
struct Cyc_PP_Doc*_tmp741[3];s=((_tmp741[2]=Cyc_PP_text(((_tmp742=")",
_tag_dyneither(_tmp742,sizeof(char),2)))),((_tmp741[1]=Cyc_Absynpp_typ2doc(
_tmp186),((_tmp741[0]=Cyc_PP_text(((_tmp743="regions(",_tag_dyneither(_tmp743,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp741,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: {struct Cyc_Absyn_AccessEff_struct*_tmp187=(struct Cyc_Absyn_AccessEff_struct*)
_tmp14F;if(_tmp187->tag != 23)goto _LL100;}_LLFF: goto _LL101;_LL100: {struct Cyc_Absyn_JoinEff_struct*
_tmp188=(struct Cyc_Absyn_JoinEff_struct*)_tmp14F;if(_tmp188->tag != 24)goto _LLCD;}
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple16*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16 _tmp1E6;void*_tmp1E7;void*_tmp1E8;struct _tuple16*_tmp1E5=cmp;
_tmp1E6=*_tmp1E5;_tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;{const char*_tmp746;struct
Cyc_PP_Doc*_tmp745[3];return(_tmp745[2]=Cyc_Absynpp_rgn2doc(_tmp1E8),((_tmp745[1]=
Cyc_PP_text(((_tmp746=" > ",_tag_dyneither(_tmp746,sizeof(char),4)))),((_tmp745[
0]=Cyc_Absynpp_rgn2doc(_tmp1E7),Cyc_PP_cat(_tag_dyneither(_tmp745,sizeof(struct
Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp749;const char*_tmp748;const char*_tmp747;return Cyc_PP_group(((_tmp747="",
_tag_dyneither(_tmp747,sizeof(char),1))),((_tmp748="",_tag_dyneither(_tmp748,
sizeof(char),1))),((_tmp749=",",_tag_dyneither(_tmp749,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp74A;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp74A=_cycalloc(
sizeof(*_tmp74A)),((_tmp74A->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp74A))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1EF=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp74D;struct Cyc_List_List*
_tmp74C;_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1EF,((_tmp74C=_cycalloc(sizeof(*_tmp74C)),((_tmp74C->hd=
Cyc_PP_text(((_tmp74D="...",_tag_dyneither(_tmp74D,sizeof(char),4)))),((_tmp74C->tl=
0,_tmp74C)))))));}else{if(cyc_varargs != 0){const char*_tmp756;const char*_tmp755;
const char*_tmp754;struct _tuple7*_tmp753;struct Cyc_PP_Doc*_tmp752[3];struct Cyc_PP_Doc*
_tmp1F2=(_tmp752[2]=Cyc_Absynpp_funarg2doc(((_tmp753=_cycalloc(sizeof(*_tmp753)),((
_tmp753->f1=cyc_varargs->name,((_tmp753->f2=cyc_varargs->tq,((_tmp753->f3=
cyc_varargs->type,_tmp753))))))))),((_tmp752[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp754=" inject ",_tag_dyneither(_tmp754,sizeof(char),9)))): Cyc_PP_text(((
_tmp755=" ",_tag_dyneither(_tmp755,sizeof(char),2)))),((_tmp752[0]=Cyc_PP_text(((
_tmp756="...",_tag_dyneither(_tmp756,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp757;_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1EF,((_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->hd=
_tmp1F2,((_tmp757->tl=0,_tmp757)))))));}}{const char*_tmp75A;const char*_tmp759;
const char*_tmp758;struct Cyc_PP_Doc*_tmp1F9=Cyc_PP_group(((_tmp758="",
_tag_dyneither(_tmp758,sizeof(char),1))),((_tmp759="",_tag_dyneither(_tmp759,
sizeof(char),1))),((_tmp75A=",",_tag_dyneither(_tmp75A,sizeof(char),2))),_tmp1EF);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp75D;struct Cyc_PP_Doc*
_tmp75C[3];_tmp1F9=((_tmp75C[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp75C[
1]=Cyc_PP_text(((_tmp75D=";",_tag_dyneither(_tmp75D,sizeof(char),2)))),((_tmp75C[
0]=_tmp1F9,Cyc_PP_cat(_tag_dyneither(_tmp75C,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[3];_tmp1F9=((_tmp75F[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp75F[1]=Cyc_PP_text(((_tmp760=":",
_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75F[0]=_tmp1F9,Cyc_PP_cat(
_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp765;
const char*_tmp764;struct Cyc_PP_Doc*_tmp763[3];return(_tmp763[2]=Cyc_PP_text(((
_tmp764=")",_tag_dyneither(_tmp764,sizeof(char),2)))),((_tmp763[1]=_tmp1F9,((
_tmp763[0]=Cyc_PP_text(((_tmp765="(",_tag_dyneither(_tmp765,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp763,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){struct Cyc_Core_Opt*_tmp768;struct _tuple7*_tmp767;return(
_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767->f1=((_tmp768=_cycalloc(sizeof(*
_tmp768)),((_tmp768->v=(*arg).f1,_tmp768)))),((_tmp767->f2=(*arg).f2,((_tmp767->f3=(*
arg).f3,_tmp767)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp206=0;int match;{union Cyc_Absyn_Nmspace _tmp207=(*q).f1;int _tmp208;struct Cyc_List_List*
_tmp209;struct Cyc_List_List*_tmp20A;_LL11E: if((_tmp207.Loc_n).tag != 3)goto _LL120;
_tmp208=(int)(_tmp207.Loc_n).val;_LL11F: _tmp209=0;goto _LL121;_LL120: if((_tmp207.Rel_n).tag
!= 1)goto _LL122;_tmp209=(struct Cyc_List_List*)(_tmp207.Rel_n).val;_LL121: match=0;
_tmp206=_tmp209;goto _LL11D;_LL122: if((_tmp207.Abs_n).tag != 2)goto _LL11D;_tmp20A=(
struct Cyc_List_List*)(_tmp207.Abs_n).val;_LL123: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20A,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp769;_tmp206=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp769=_cycalloc(sizeof(*_tmp769)),((_tmp769->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp769->tl=_tmp20A,_tmp769))))): _tmp20A;}goto _LL11D;_LL11D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp76B;struct Cyc_List_List*_tmp76A;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp206,((_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A->hd=(*q).f2,((_tmp76A->tl=
0,_tmp76A))))))),((_tmp76B="_",_tag_dyneither(_tmp76B,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp76D;struct Cyc_List_List*_tmp76C;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp206,((_tmp76C=_cycalloc(sizeof(*
_tmp76C)),((_tmp76C->hd=(*q).f2,((_tmp76C->tl=0,_tmp76C))))))),((_tmp76D="::",
_tag_dyneither(_tmp76D,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp210=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp76F;const char*_tmp76E;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp76E="\\textbf{",
_tag_dyneither(_tmp76E,sizeof(char),9))),(struct _dyneither_ptr)_tmp210),((
_tmp76F="}",_tag_dyneither(_tmp76F,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp210));}else{return Cyc_PP_text(_tmp210);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp213=(*v).f1;int
_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp216;_LL125: if((
_tmp213.Loc_n).tag != 3)goto _LL127;_tmp214=(int)(_tmp213.Loc_n).val;_LL126: goto
_LL128;_LL127: if((_tmp213.Rel_n).tag != 1)goto _LL129;_tmp215=(struct Cyc_List_List*)(
_tmp213.Rel_n).val;if(_tmp215 != 0)goto _LL129;_LL128: return*(*v).f2;_LL129: if((
_tmp213.Abs_n).tag != 2)goto _LL12B;_tmp216=(struct Cyc_List_List*)(_tmp213.Abs_n).val;
_LL12A: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp216,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL12C;}_LL12B:;
_LL12C: {const char*_tmp770;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp770="/* bad namespace : */ ",
_tag_dyneither(_tmp770,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL124:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp218=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp772;const char*_tmp771;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp771="\\textbf{",
_tag_dyneither(_tmp771,sizeof(char),9))),(struct _dyneither_ptr)_tmp218),((
_tmp772="}",_tag_dyneither(_tmp772,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp218));}else{return Cyc_PP_text(_tmp218);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp21B=e->r;enum Cyc_Absyn_Primop
_tmp220;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22D;_LL12E: {struct
Cyc_Absyn_Const_e_struct*_tmp21C=(struct Cyc_Absyn_Const_e_struct*)_tmp21B;if(
_tmp21C->tag != 0)goto _LL130;}_LL12F: goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp21D=(struct Cyc_Absyn_Var_e_struct*)_tmp21B;if(_tmp21D->tag != 1)goto _LL132;}
_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnknownId_e_struct*_tmp21E=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp21B;if(_tmp21E->tag != 2)goto _LL134;}_LL133: return 10000;_LL134: {struct Cyc_Absyn_Primop_e_struct*
_tmp21F=(struct Cyc_Absyn_Primop_e_struct*)_tmp21B;if(_tmp21F->tag != 3)goto _LL136;
else{_tmp220=_tmp21F->f1;}}_LL135: switch(_tmp220){case Cyc_Absyn_Plus: _LL17E:
return 100;case Cyc_Absyn_Times: _LL17F: return 110;case Cyc_Absyn_Minus: _LL180: return
100;case Cyc_Absyn_Div: _LL181: goto _LL182;case Cyc_Absyn_Mod: _LL182: return 110;case
Cyc_Absyn_Eq: _LL183: goto _LL184;case Cyc_Absyn_Neq: _LL184: return 70;case Cyc_Absyn_Gt:
_LL185: goto _LL186;case Cyc_Absyn_Lt: _LL186: goto _LL187;case Cyc_Absyn_Gte: _LL187:
goto _LL188;case Cyc_Absyn_Lte: _LL188: return 80;case Cyc_Absyn_Not: _LL189: goto _LL18A;
case Cyc_Absyn_Bitnot: _LL18A: return 130;case Cyc_Absyn_Bitand: _LL18B: return 60;case
Cyc_Absyn_Bitor: _LL18C: return 40;case Cyc_Absyn_Bitxor: _LL18D: return 50;case Cyc_Absyn_Bitlshift:
_LL18E: return 90;case Cyc_Absyn_Bitlrshift: _LL18F: return 80;case Cyc_Absyn_Bitarshift:
_LL190: return 80;case Cyc_Absyn_Numelts: _LL191: return 140;}_LL136: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp221=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp21B;if(_tmp221->tag != 4)goto
_LL138;}_LL137: return 20;_LL138: {struct Cyc_Absyn_Increment_e_struct*_tmp222=(
struct Cyc_Absyn_Increment_e_struct*)_tmp21B;if(_tmp222->tag != 5)goto _LL13A;}
_LL139: return 130;_LL13A: {struct Cyc_Absyn_Conditional_e_struct*_tmp223=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp21B;if(_tmp223->tag != 6)goto _LL13C;}_LL13B:
return 30;_LL13C: {struct Cyc_Absyn_And_e_struct*_tmp224=(struct Cyc_Absyn_And_e_struct*)
_tmp21B;if(_tmp224->tag != 7)goto _LL13E;}_LL13D: return 35;_LL13E: {struct Cyc_Absyn_Or_e_struct*
_tmp225=(struct Cyc_Absyn_Or_e_struct*)_tmp21B;if(_tmp225->tag != 8)goto _LL140;}
_LL13F: return 30;_LL140: {struct Cyc_Absyn_SeqExp_e_struct*_tmp226=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp21B;if(_tmp226->tag != 9)goto _LL142;}_LL141: return 10;_LL142: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp227=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp21B;if(_tmp227->tag != 10)goto
_LL144;}_LL143: goto _LL145;_LL144: {struct Cyc_Absyn_FnCall_e_struct*_tmp228=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp21B;if(_tmp228->tag != 11)goto _LL146;}_LL145:
return 140;_LL146: {struct Cyc_Absyn_Throw_e_struct*_tmp229=(struct Cyc_Absyn_Throw_e_struct*)
_tmp21B;if(_tmp229->tag != 12)goto _LL148;}_LL147: return 130;_LL148: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp22A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp21B;if(_tmp22A->tag != 13)
goto _LL14A;else{_tmp22B=_tmp22A->f1;}}_LL149: return Cyc_Absynpp_exp_prec(_tmp22B);
_LL14A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp22C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp21B;if(_tmp22C->tag != 14)goto _LL14C;else{_tmp22D=_tmp22C->f1;}}_LL14B: return
Cyc_Absynpp_exp_prec(_tmp22D);_LL14C: {struct Cyc_Absyn_Cast_e_struct*_tmp22E=(
struct Cyc_Absyn_Cast_e_struct*)_tmp21B;if(_tmp22E->tag != 15)goto _LL14E;}_LL14D:
return 120;_LL14E: {struct Cyc_Absyn_New_e_struct*_tmp22F=(struct Cyc_Absyn_New_e_struct*)
_tmp21B;if(_tmp22F->tag != 17)goto _LL150;}_LL14F: return 15;_LL150: {struct Cyc_Absyn_Address_e_struct*
_tmp230=(struct Cyc_Absyn_Address_e_struct*)_tmp21B;if(_tmp230->tag != 16)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp231=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp21B;if(_tmp231->tag != 18)goto _LL154;}
_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp232=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp21B;if(_tmp232->tag != 19)goto _LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Valueof_e_struct*
_tmp233=(struct Cyc_Absyn_Valueof_e_struct*)_tmp21B;if(_tmp233->tag != 39)goto
_LL158;}_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp234=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp21B;if(_tmp234->tag != 38)goto _LL15A;}
_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Offsetof_e_struct*_tmp235=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp21B;if(_tmp235->tag != 20)goto _LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Deref_e_struct*
_tmp236=(struct Cyc_Absyn_Deref_e_struct*)_tmp21B;if(_tmp236->tag != 21)goto _LL15E;}
_LL15D: return 130;_LL15E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp237=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp21B;if(_tmp237->tag != 22)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp238=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp21B;if(_tmp238->tag != 23)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Subscript_e_struct*_tmp239=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp21B;if(_tmp239->tag != 24)goto _LL164;}
_LL163: return 140;_LL164: {struct Cyc_Absyn_Tuple_e_struct*_tmp23A=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp21B;if(_tmp23A->tag != 25)goto _LL166;}_LL165: return 150;_LL166: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp23B=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp21B;if(_tmp23B->tag != 26)goto
_LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Array_e_struct*_tmp23C=(
struct Cyc_Absyn_Array_e_struct*)_tmp21B;if(_tmp23C->tag != 27)goto _LL16A;}_LL169:
goto _LL16B;_LL16A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp23D=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp21B;if(_tmp23D->tag != 28)goto _LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp23E=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp21B;if(_tmp23E->tag != 29)goto
_LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp23F=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp21B;if(_tmp23F->tag != 30)goto _LL170;}
_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_Datatype_e_struct*_tmp240=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp21B;if(_tmp240->tag != 31)goto _LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Enum_e_struct*
_tmp241=(struct Cyc_Absyn_Enum_e_struct*)_tmp21B;if(_tmp241->tag != 32)goto _LL174;}
_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp242=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp21B;if(_tmp242->tag != 33)goto _LL176;}_LL175: goto _LL177;_LL176: {struct Cyc_Absyn_Malloc_e_struct*
_tmp243=(struct Cyc_Absyn_Malloc_e_struct*)_tmp21B;if(_tmp243->tag != 34)goto
_LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Swap_e_struct*_tmp244=(struct
Cyc_Absyn_Swap_e_struct*)_tmp21B;if(_tmp244->tag != 35)goto _LL17A;}_LL179: goto
_LL17B;_LL17A: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp245=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp21B;if(_tmp245->tag != 36)goto _LL17C;}_LL17B: return 140;_LL17C: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp246=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp21B;if(_tmp246->tag != 37)goto
_LL12D;}_LL17D: return 10000;_LL12D:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp247=e->r;union Cyc_Absyn_Cnst _tmp249;struct _tuple0*_tmp24B;struct
_tuple0*_tmp24D;enum Cyc_Absyn_Primop _tmp24F;struct Cyc_List_List*_tmp250;struct
Cyc_Absyn_Exp*_tmp252;struct Cyc_Core_Opt*_tmp253;struct Cyc_Absyn_Exp*_tmp254;
struct Cyc_Absyn_Exp*_tmp256;enum Cyc_Absyn_Incrementor _tmp257;struct Cyc_Absyn_Exp*
_tmp259;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*
_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*
_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*
_tmp266;struct Cyc_List_List*_tmp267;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_List_List*
_tmp26A;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*
_tmp270;void*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp275;
struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp278;void*_tmp27A;struct Cyc_Absyn_Exp*
_tmp27C;void*_tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct _dyneither_ptr*_tmp281;
void*_tmp283;void*_tmp284;struct _dyneither_ptr*_tmp286;void*_tmp288;void*_tmp289;
unsigned int _tmp28B;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Exp*_tmp28F;
struct _dyneither_ptr*_tmp290;struct Cyc_Absyn_Exp*_tmp292;struct _dyneither_ptr*
_tmp293;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_List_List*
_tmp298;struct _tuple7*_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*
_tmp29D;struct Cyc_Absyn_Vardecl*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*
_tmp2A1;struct _tuple0*_tmp2A3;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*
_tmp2A5;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Datatypefield*
_tmp2AA;struct _tuple0*_tmp2AC;struct _tuple0*_tmp2AE;struct Cyc_Absyn_MallocInfo
_tmp2B0;int _tmp2B1;struct Cyc_Absyn_Exp*_tmp2B2;void**_tmp2B3;struct Cyc_Absyn_Exp*
_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Core_Opt*
_tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Stmt*_tmp2BC;_LL194: {struct
Cyc_Absyn_Const_e_struct*_tmp248=(struct Cyc_Absyn_Const_e_struct*)_tmp247;if(
_tmp248->tag != 0)goto _LL196;else{_tmp249=_tmp248->f1;}}_LL195: s=Cyc_Absynpp_cnst2doc(
_tmp249);goto _LL193;_LL196: {struct Cyc_Absyn_Var_e_struct*_tmp24A=(struct Cyc_Absyn_Var_e_struct*)
_tmp247;if(_tmp24A->tag != 1)goto _LL198;else{_tmp24B=_tmp24A->f1;}}_LL197: _tmp24D=
_tmp24B;goto _LL199;_LL198: {struct Cyc_Absyn_UnknownId_e_struct*_tmp24C=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp247;if(_tmp24C->tag != 2)goto _LL19A;else{
_tmp24D=_tmp24C->f1;}}_LL199: s=Cyc_Absynpp_qvar2doc(_tmp24D);goto _LL193;_LL19A: {
struct Cyc_Absyn_Primop_e_struct*_tmp24E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp247;if(_tmp24E->tag != 3)goto _LL19C;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;}}
_LL19B: s=Cyc_Absynpp_primapp2doc(myprec,_tmp24F,_tmp250);goto _LL193;_LL19C: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp251=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp247;if(_tmp251->tag != 4)goto _LL19E;else{_tmp252=_tmp251->f1;_tmp253=_tmp251->f2;
_tmp254=_tmp251->f3;}}_LL19D:{const char*_tmp777;const char*_tmp776;struct Cyc_PP_Doc*
_tmp775[5];s=((_tmp775[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),((_tmp775[3]=
Cyc_PP_text(((_tmp776="= ",_tag_dyneither(_tmp776,sizeof(char),3)))),((_tmp775[2]=
_tmp253 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp253->v),((
_tmp775[1]=Cyc_PP_text(((_tmp777=" ",_tag_dyneither(_tmp777,sizeof(char),2)))),((
_tmp775[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp252),Cyc_PP_cat(_tag_dyneither(
_tmp775,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;_LL19E: {struct Cyc_Absyn_Increment_e_struct*
_tmp255=(struct Cyc_Absyn_Increment_e_struct*)_tmp247;if(_tmp255->tag != 5)goto
_LL1A0;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}_LL19F: {struct Cyc_PP_Doc*
_tmp2C0=Cyc_Absynpp_exp2doc_prec(myprec,_tmp256);switch(_tmp257){case Cyc_Absyn_PreInc:
_LL1E6:{const char*_tmp77A;struct Cyc_PP_Doc*_tmp779[2];s=((_tmp779[1]=_tmp2C0,((
_tmp779[0]=Cyc_PP_text(((_tmp77A="++",_tag_dyneither(_tmp77A,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp779,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1E7:{const char*_tmp77D;struct Cyc_PP_Doc*_tmp77C[2];s=((
_tmp77C[1]=_tmp2C0,((_tmp77C[0]=Cyc_PP_text(((_tmp77D="--",_tag_dyneither(
_tmp77D,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp77C,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1E8:{const char*_tmp780;struct Cyc_PP_Doc*
_tmp77F[2];s=((_tmp77F[1]=Cyc_PP_text(((_tmp780="++",_tag_dyneither(_tmp780,
sizeof(char),3)))),((_tmp77F[0]=_tmp2C0,Cyc_PP_cat(_tag_dyneither(_tmp77F,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1E9:{const char*
_tmp783;struct Cyc_PP_Doc*_tmp782[2];s=((_tmp782[1]=Cyc_PP_text(((_tmp783="--",
_tag_dyneither(_tmp783,sizeof(char),3)))),((_tmp782[0]=_tmp2C0,Cyc_PP_cat(
_tag_dyneither(_tmp782,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL193;}
_LL1A0: {struct Cyc_Absyn_Conditional_e_struct*_tmp258=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp247;if(_tmp258->tag != 6)goto _LL1A2;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;
_tmp25B=_tmp258->f3;}}_LL1A1:{const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*
_tmp786[5];s=((_tmp786[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),((_tmp786[3]=
Cyc_PP_text(((_tmp787=" : ",_tag_dyneither(_tmp787,sizeof(char),4)))),((_tmp786[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp25A),((_tmp786[1]=Cyc_PP_text(((_tmp788=" ? ",
_tag_dyneither(_tmp788,sizeof(char),4)))),((_tmp786[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp259),Cyc_PP_cat(_tag_dyneither(_tmp786,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL193;_LL1A2: {struct Cyc_Absyn_And_e_struct*_tmp25C=(struct Cyc_Absyn_And_e_struct*)
_tmp247;if(_tmp25C->tag != 7)goto _LL1A4;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}
_LL1A3:{const char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[3];s=((_tmp78A[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25E),((_tmp78A[1]=Cyc_PP_text(((_tmp78B=" && ",_tag_dyneither(_tmp78B,
sizeof(char),5)))),((_tmp78A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),Cyc_PP_cat(
_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1A4: {
struct Cyc_Absyn_Or_e_struct*_tmp25F=(struct Cyc_Absyn_Or_e_struct*)_tmp247;if(
_tmp25F->tag != 8)goto _LL1A6;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}
_LL1A5:{const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[3];s=((_tmp78D[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp261),((_tmp78D[1]=Cyc_PP_text(((_tmp78E=" || ",_tag_dyneither(_tmp78E,
sizeof(char),5)))),((_tmp78D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),Cyc_PP_cat(
_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1A6: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp262=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp247;if(_tmp262->tag != 9)goto _LL1A8;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}
_LL1A7:{const char*_tmp795;const char*_tmp794;const char*_tmp793;struct Cyc_PP_Doc*
_tmp792[5];s=((_tmp792[4]=Cyc_PP_text(((_tmp793=")",_tag_dyneither(_tmp793,
sizeof(char),2)))),((_tmp792[3]=Cyc_Absynpp_exp2doc(_tmp264),((_tmp792[2]=Cyc_PP_text(((
_tmp794=", ",_tag_dyneither(_tmp794,sizeof(char),3)))),((_tmp792[1]=Cyc_Absynpp_exp2doc(
_tmp263),((_tmp792[0]=Cyc_PP_text(((_tmp795="(",_tag_dyneither(_tmp795,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL193;_LL1A8: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp265=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp247;if(_tmp265->tag != 10)goto _LL1AA;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}
_LL1A9:{const char*_tmp79A;const char*_tmp799;struct Cyc_PP_Doc*_tmp798[4];s=((
_tmp798[3]=Cyc_PP_text(((_tmp799=")",_tag_dyneither(_tmp799,sizeof(char),2)))),((
_tmp798[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp267),((_tmp798[1]=Cyc_PP_text(((
_tmp79A="(",_tag_dyneither(_tmp79A,sizeof(char),2)))),((_tmp798[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp266),Cyc_PP_cat(_tag_dyneither(_tmp798,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1AA: {struct Cyc_Absyn_FnCall_e_struct*_tmp268=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp247;if(_tmp268->tag != 11)goto _LL1AC;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}
_LL1AB:{const char*_tmp79F;const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[4];s=((
_tmp79D[3]=Cyc_PP_text(((_tmp79E=")",_tag_dyneither(_tmp79E,sizeof(char),2)))),((
_tmp79D[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26A),((_tmp79D[1]=Cyc_PP_text(((
_tmp79F="(",_tag_dyneither(_tmp79F,sizeof(char),2)))),((_tmp79D[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp269),Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1AC: {struct Cyc_Absyn_Throw_e_struct*_tmp26B=(struct Cyc_Absyn_Throw_e_struct*)
_tmp247;if(_tmp26B->tag != 12)goto _LL1AE;else{_tmp26C=_tmp26B->f1;}}_LL1AD:{const
char*_tmp7A2;struct Cyc_PP_Doc*_tmp7A1[2];s=((_tmp7A1[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26C),((_tmp7A1[0]=Cyc_PP_text(((_tmp7A2="throw ",_tag_dyneither(
_tmp7A2,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7A1,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL193;_LL1AE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp26D=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp247;if(_tmp26D->tag != 13)goto _LL1B0;
else{_tmp26E=_tmp26D->f1;}}_LL1AF: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26E);
goto _LL193;_LL1B0: {struct Cyc_Absyn_Instantiate_e_struct*_tmp26F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp247;if(_tmp26F->tag != 14)goto _LL1B2;else{_tmp270=_tmp26F->f1;}}_LL1B1: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp270);goto _LL193;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp271=(
struct Cyc_Absyn_Cast_e_struct*)_tmp247;if(_tmp271->tag != 15)goto _LL1B4;else{
_tmp272=(void*)_tmp271->f1;_tmp273=_tmp271->f2;}}_LL1B3:{const char*_tmp7A7;const
char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[4];s=((_tmp7A5[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp273),((_tmp7A5[2]=Cyc_PP_text(((_tmp7A6=")",_tag_dyneither(_tmp7A6,
sizeof(char),2)))),((_tmp7A5[1]=Cyc_Absynpp_typ2doc(_tmp272),((_tmp7A5[0]=Cyc_PP_text(((
_tmp7A7="(",_tag_dyneither(_tmp7A7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7A5,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1B4: {struct Cyc_Absyn_Address_e_struct*
_tmp274=(struct Cyc_Absyn_Address_e_struct*)_tmp247;if(_tmp274->tag != 16)goto
_LL1B6;else{_tmp275=_tmp274->f1;}}_LL1B5:{const char*_tmp7AA;struct Cyc_PP_Doc*
_tmp7A9[2];s=((_tmp7A9[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp7A9[0]=
Cyc_PP_text(((_tmp7AA="&",_tag_dyneither(_tmp7AA,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;_LL1B6: {
struct Cyc_Absyn_New_e_struct*_tmp276=(struct Cyc_Absyn_New_e_struct*)_tmp247;if(
_tmp276->tag != 17)goto _LL1B8;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;}}
_LL1B7: if(_tmp277 == 0){const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[2];s=((_tmp7AC[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp278),((_tmp7AC[0]=Cyc_PP_text(((_tmp7AD="new ",
_tag_dyneither(_tmp7AD,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7AC,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7B2;const char*_tmp7B1;
struct Cyc_PP_Doc*_tmp7B0[4];s=((_tmp7B0[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp278),((_tmp7B0[2]=Cyc_PP_text(((_tmp7B1=") ",_tag_dyneither(_tmp7B1,sizeof(
char),3)))),((_tmp7B0[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp277),((
_tmp7B0[0]=Cyc_PP_text(((_tmp7B2="rnew(",_tag_dyneither(_tmp7B2,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7B0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL193;_LL1B8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp279=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp247;if(_tmp279->tag != 18)goto _LL1BA;else{_tmp27A=(void*)_tmp279->f1;}}_LL1B9:{
const char*_tmp7B7;const char*_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[3];s=((_tmp7B5[2]=
Cyc_PP_text(((_tmp7B6=")",_tag_dyneither(_tmp7B6,sizeof(char),2)))),((_tmp7B5[1]=
Cyc_Absynpp_typ2doc(_tmp27A),((_tmp7B5[0]=Cyc_PP_text(((_tmp7B7="sizeof(",
_tag_dyneither(_tmp7B7,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7B5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1BA: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp27B=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp247;if(_tmp27B->tag != 19)goto
_LL1BC;else{_tmp27C=_tmp27B->f1;}}_LL1BB:{const char*_tmp7BC;const char*_tmp7BB;
struct Cyc_PP_Doc*_tmp7BA[3];s=((_tmp7BA[2]=Cyc_PP_text(((_tmp7BB=")",
_tag_dyneither(_tmp7BB,sizeof(char),2)))),((_tmp7BA[1]=Cyc_Absynpp_exp2doc(
_tmp27C),((_tmp7BA[0]=Cyc_PP_text(((_tmp7BC="sizeof(",_tag_dyneither(_tmp7BC,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7BA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL193;_LL1BC: {struct Cyc_Absyn_Valueof_e_struct*_tmp27D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp247;if(_tmp27D->tag != 39)goto _LL1BE;else{_tmp27E=(void*)_tmp27D->f1;}}_LL1BD:{
const char*_tmp7C1;const char*_tmp7C0;struct Cyc_PP_Doc*_tmp7BF[3];s=((_tmp7BF[2]=
Cyc_PP_text(((_tmp7C0=")",_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BF[1]=
Cyc_Absynpp_typ2doc(_tmp27E),((_tmp7BF[0]=Cyc_PP_text(((_tmp7C1="valueof(",
_tag_dyneither(_tmp7C1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7BF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1BE: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp27F=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp247;if(_tmp27F->tag != 38)goto
_LL1C0;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}_LL1BF:{const char*_tmp7C8;
const char*_tmp7C7;const char*_tmp7C6;struct Cyc_PP_Doc*_tmp7C5[5];s=((_tmp7C5[4]=
Cyc_PP_text(((_tmp7C6=")",_tag_dyneither(_tmp7C6,sizeof(char),2)))),((_tmp7C5[3]=
Cyc_PP_textptr(_tmp281),((_tmp7C5[2]=Cyc_PP_text(((_tmp7C7=".",_tag_dyneither(
_tmp7C7,sizeof(char),2)))),((_tmp7C5[1]=Cyc_Absynpp_exp2doc(_tmp280),((_tmp7C5[0]=
Cyc_PP_text(((_tmp7C8="tagcheck(",_tag_dyneither(_tmp7C8,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7C5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;
_LL1C0: {struct Cyc_Absyn_Offsetof_e_struct*_tmp282=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp247;if(_tmp282->tag != 20)goto _LL1C2;else{_tmp283=(void*)_tmp282->f1;_tmp284=(
void*)_tmp282->f2;{struct Cyc_Absyn_StructField_struct*_tmp285=(struct Cyc_Absyn_StructField_struct*)
_tmp284;if(_tmp285->tag != 0)goto _LL1C2;else{_tmp286=_tmp285->f1;}};}}_LL1C1:{
const char*_tmp7CF;const char*_tmp7CE;const char*_tmp7CD;struct Cyc_PP_Doc*_tmp7CC[5];
s=((_tmp7CC[4]=Cyc_PP_text(((_tmp7CD=")",_tag_dyneither(_tmp7CD,sizeof(char),2)))),((
_tmp7CC[3]=Cyc_PP_textptr(_tmp286),((_tmp7CC[2]=Cyc_PP_text(((_tmp7CE=",",
_tag_dyneither(_tmp7CE,sizeof(char),2)))),((_tmp7CC[1]=Cyc_Absynpp_typ2doc(
_tmp283),((_tmp7CC[0]=Cyc_PP_text(((_tmp7CF="offsetof(",_tag_dyneither(_tmp7CF,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7CC,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL193;_LL1C2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp287=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp247;if(_tmp287->tag != 20)goto _LL1C4;else{
_tmp288=(void*)_tmp287->f1;_tmp289=(void*)_tmp287->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp28A=(struct Cyc_Absyn_TupleIndex_struct*)_tmp289;if(_tmp28A->tag != 1)goto
_LL1C4;else{_tmp28B=_tmp28A->f1;}};}}_LL1C3:{const char*_tmp7DD;const char*_tmp7DC;
struct Cyc_Int_pa_struct _tmp7DB;void*_tmp7DA[1];const char*_tmp7D9;const char*
_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[5];s=((_tmp7D7[4]=Cyc_PP_text(((_tmp7D8=")",
_tag_dyneither(_tmp7D8,sizeof(char),2)))),((_tmp7D7[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7DB.tag=1,((_tmp7DB.f1=(unsigned long)((int)_tmp28B),((
_tmp7DA[0]=& _tmp7DB,Cyc_aprintf(((_tmp7D9="%d",_tag_dyneither(_tmp7D9,sizeof(
char),3))),_tag_dyneither(_tmp7DA,sizeof(void*),1))))))))),((_tmp7D7[2]=Cyc_PP_text(((
_tmp7DC=",",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7D7[1]=Cyc_Absynpp_typ2doc(
_tmp288),((_tmp7D7[0]=Cyc_PP_text(((_tmp7DD="offsetof(",_tag_dyneither(_tmp7DD,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL193;_LL1C4: {struct Cyc_Absyn_Deref_e_struct*_tmp28C=(
struct Cyc_Absyn_Deref_e_struct*)_tmp247;if(_tmp28C->tag != 21)goto _LL1C6;else{
_tmp28D=_tmp28C->f1;}}_LL1C5:{const char*_tmp7E0;struct Cyc_PP_Doc*_tmp7DF[2];s=((
_tmp7DF[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),((_tmp7DF[0]=Cyc_PP_text(((
_tmp7E0="*",_tag_dyneither(_tmp7E0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7DF,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;_LL1C6: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp28E=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp247;if(_tmp28E->tag != 22)goto
_LL1C8;else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;}}_LL1C7:{const char*_tmp7E3;
struct Cyc_PP_Doc*_tmp7E2[3];s=((_tmp7E2[2]=Cyc_PP_textptr(_tmp290),((_tmp7E2[1]=
Cyc_PP_text(((_tmp7E3=".",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E2[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F),Cyc_PP_cat(_tag_dyneither(_tmp7E2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1C8: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp291=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp247;if(_tmp291->tag != 23)goto
_LL1CA;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}}_LL1C9:{const char*_tmp7E6;
struct Cyc_PP_Doc*_tmp7E5[3];s=((_tmp7E5[2]=Cyc_PP_textptr(_tmp293),((_tmp7E5[1]=
Cyc_PP_text(((_tmp7E6="->",_tag_dyneither(_tmp7E6,sizeof(char),3)))),((_tmp7E5[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),Cyc_PP_cat(_tag_dyneither(_tmp7E5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL193;_LL1CA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp294=(struct Cyc_Absyn_Subscript_e_struct*)_tmp247;if(_tmp294->tag != 24)goto
_LL1CC;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1CB:{const char*_tmp7EB;
const char*_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[4];s=((_tmp7E9[3]=Cyc_PP_text(((
_tmp7EA="]",_tag_dyneither(_tmp7EA,sizeof(char),2)))),((_tmp7E9[2]=Cyc_Absynpp_exp2doc(
_tmp296),((_tmp7E9[1]=Cyc_PP_text(((_tmp7EB="[",_tag_dyneither(_tmp7EB,sizeof(
char),2)))),((_tmp7E9[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),Cyc_PP_cat(
_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1CC: {
struct Cyc_Absyn_Tuple_e_struct*_tmp297=(struct Cyc_Absyn_Tuple_e_struct*)_tmp247;
if(_tmp297->tag != 25)goto _LL1CE;else{_tmp298=_tmp297->f1;}}_LL1CD:{const char*
_tmp7F0;const char*_tmp7EF;struct Cyc_PP_Doc*_tmp7EE[4];s=((_tmp7EE[3]=Cyc_PP_text(((
_tmp7EF=")",_tag_dyneither(_tmp7EF,sizeof(char),2)))),((_tmp7EE[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp298),((_tmp7EE[1]=Cyc_PP_text(((_tmp7F0="(",_tag_dyneither(_tmp7F0,
sizeof(char),2)))),((_tmp7EE[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7EE,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL193;_LL1CE: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp299=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp247;if(_tmp299->tag != 26)goto
_LL1D0;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;}}_LL1CF:{const char*_tmp7F7;
const char*_tmp7F6;const char*_tmp7F5;struct Cyc_PP_Doc*_tmp7F4[4];s=((_tmp7F4[3]=
Cyc_Absynpp_group_braces(((_tmp7F5=",",_tag_dyneither(_tmp7F5,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29B)),((_tmp7F4[2]=Cyc_PP_text(((_tmp7F6=")",
_tag_dyneither(_tmp7F6,sizeof(char),2)))),((_tmp7F4[1]=Cyc_Absynpp_typ2doc((*
_tmp29A).f3),((_tmp7F4[0]=Cyc_PP_text(((_tmp7F7="(",_tag_dyneither(_tmp7F7,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL193;_LL1D0: {struct Cyc_Absyn_Array_e_struct*_tmp29C=(struct Cyc_Absyn_Array_e_struct*)
_tmp247;if(_tmp29C->tag != 27)goto _LL1D2;else{_tmp29D=_tmp29C->f1;}}_LL1D1:{const
char*_tmp7F8;s=Cyc_Absynpp_group_braces(((_tmp7F8=",",_tag_dyneither(_tmp7F8,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29D));}goto _LL193;
_LL1D2: {struct Cyc_Absyn_Comprehension_e_struct*_tmp29E=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp247;if(_tmp29E->tag != 28)goto _LL1D4;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;
_tmp2A1=_tmp29E->f3;}}_LL1D3:{const char*_tmp7FF;const char*_tmp7FE;const char*
_tmp7FD;struct Cyc_PP_Doc*_tmp7FC[8];s=((_tmp7FC[7]=Cyc_Absynpp_rb(),((_tmp7FC[6]=
Cyc_Absynpp_exp2doc(_tmp2A1),((_tmp7FC[5]=Cyc_PP_text(((_tmp7FD=" : ",
_tag_dyneither(_tmp7FD,sizeof(char),4)))),((_tmp7FC[4]=Cyc_Absynpp_exp2doc(
_tmp2A0),((_tmp7FC[3]=Cyc_PP_text(((_tmp7FE=" < ",_tag_dyneither(_tmp7FE,sizeof(
char),4)))),((_tmp7FC[2]=Cyc_PP_text(*(*_tmp29F->name).f2),((_tmp7FC[1]=Cyc_PP_text(((
_tmp7FF="for ",_tag_dyneither(_tmp7FF,sizeof(char),5)))),((_tmp7FC[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7FC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL193;_LL1D4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2A2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp247;if(_tmp2A2->tag != 29)goto _LL1D6;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;
_tmp2A5=_tmp2A2->f3;}}_LL1D5: {struct Cyc_List_List*_tmp313=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2A5);{const char*_tmp804;struct Cyc_List_List*_tmp803;struct Cyc_PP_Doc*_tmp802[
2];s=((_tmp802[1]=Cyc_Absynpp_group_braces(((_tmp804=",",_tag_dyneither(_tmp804,
sizeof(char),2))),_tmp2A4 != 0?(_tmp803=_cycalloc(sizeof(*_tmp803)),((_tmp803->hd=
Cyc_Absynpp_tps2doc(_tmp2A4),((_tmp803->tl=_tmp313,_tmp803))))): _tmp313),((
_tmp802[0]=Cyc_Absynpp_qvar2doc(_tmp2A3),Cyc_PP_cat(_tag_dyneither(_tmp802,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;}_LL1D6: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2A6=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp247;if(_tmp2A6->tag != 30)goto
_LL1D8;else{_tmp2A7=_tmp2A6->f2;}}_LL1D7:{const char*_tmp805;s=Cyc_Absynpp_group_braces(((
_tmp805=",",_tag_dyneither(_tmp805,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2A7));}goto _LL193;_LL1D8: {struct Cyc_Absyn_Datatype_e_struct*_tmp2A8=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp247;if(_tmp2A8->tag != 31)goto _LL1DA;else{
_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f3;}}_LL1D9: if(_tmp2A9 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2AA->name);else{const char*_tmp80C;const char*_tmp80B;const char*_tmp80A;struct
Cyc_PP_Doc*_tmp809[2];s=((_tmp809[1]=Cyc_PP_egroup(((_tmp80C="(",_tag_dyneither(
_tmp80C,sizeof(char),2))),((_tmp80B=")",_tag_dyneither(_tmp80B,sizeof(char),2))),((
_tmp80A=",",_tag_dyneither(_tmp80A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2A9)),((_tmp809[0]=Cyc_Absynpp_qvar2doc(_tmp2AA->name),
Cyc_PP_cat(_tag_dyneither(_tmp809,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL193;
_LL1DA: {struct Cyc_Absyn_Enum_e_struct*_tmp2AB=(struct Cyc_Absyn_Enum_e_struct*)
_tmp247;if(_tmp2AB->tag != 32)goto _LL1DC;else{_tmp2AC=_tmp2AB->f1;}}_LL1DB: s=Cyc_Absynpp_qvar2doc(
_tmp2AC);goto _LL193;_LL1DC: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2AD=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp247;if(_tmp2AD->tag != 33)goto _LL1DE;else{
_tmp2AE=_tmp2AD->f1;}}_LL1DD: s=Cyc_Absynpp_qvar2doc(_tmp2AE);goto _LL193;_LL1DE: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2AF=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp247;if(_tmp2AF->tag != 34)goto _LL1E0;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2B0.is_calloc;
_tmp2B2=_tmp2B0.rgn;_tmp2B3=_tmp2B0.elt_type;_tmp2B4=_tmp2B0.num_elts;}}_LL1DF:
if(_tmp2B1){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2B3)),0);if(_tmp2B2 == 0){const char*_tmp813;const char*_tmp812;const char*
_tmp811;struct Cyc_PP_Doc*_tmp810[5];s=((_tmp810[4]=Cyc_PP_text(((_tmp811=")",
_tag_dyneither(_tmp811,sizeof(char),2)))),((_tmp810[3]=Cyc_Absynpp_exp2doc(st),((
_tmp810[2]=Cyc_PP_text(((_tmp812=",",_tag_dyneither(_tmp812,sizeof(char),2)))),((
_tmp810[1]=Cyc_Absynpp_exp2doc(_tmp2B4),((_tmp810[0]=Cyc_PP_text(((_tmp813="calloc(",
_tag_dyneither(_tmp813,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp810,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp81C;const char*
_tmp81B;const char*_tmp81A;const char*_tmp819;struct Cyc_PP_Doc*_tmp818[7];s=((
_tmp818[6]=Cyc_PP_text(((_tmp819=")",_tag_dyneither(_tmp819,sizeof(char),2)))),((
_tmp818[5]=Cyc_Absynpp_exp2doc(st),((_tmp818[4]=Cyc_PP_text(((_tmp81A=",",
_tag_dyneither(_tmp81A,sizeof(char),2)))),((_tmp818[3]=Cyc_Absynpp_exp2doc(
_tmp2B4),((_tmp818[2]=Cyc_PP_text(((_tmp81B=",",_tag_dyneither(_tmp81B,sizeof(
char),2)))),((_tmp818[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2B2),((
_tmp818[0]=Cyc_PP_text(((_tmp81C="rcalloc(",_tag_dyneither(_tmp81C,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp818,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2B3 == 0)new_e=_tmp2B4;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2B3,0),_tmp2B4,0);}if(_tmp2B2 == 0){const char*
_tmp821;const char*_tmp820;struct Cyc_PP_Doc*_tmp81F[3];s=((_tmp81F[2]=Cyc_PP_text(((
_tmp820=")",_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81F[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp81F[0]=Cyc_PP_text(((_tmp821="malloc(",_tag_dyneither(_tmp821,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp81F,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp828;const char*_tmp827;const char*_tmp826;struct Cyc_PP_Doc*
_tmp825[5];s=((_tmp825[4]=Cyc_PP_text(((_tmp826=")",_tag_dyneither(_tmp826,
sizeof(char),2)))),((_tmp825[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp825[2]=Cyc_PP_text(((
_tmp827=",",_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp825[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2B2),((_tmp825[0]=Cyc_PP_text(((_tmp828="rmalloc(",
_tag_dyneither(_tmp828,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp825,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL193;_LL1E0: {struct Cyc_Absyn_Swap_e_struct*
_tmp2B5=(struct Cyc_Absyn_Swap_e_struct*)_tmp247;if(_tmp2B5->tag != 35)goto _LL1E2;
else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;}}_LL1E1:{const char*_tmp82F;const
char*_tmp82E;const char*_tmp82D;struct Cyc_PP_Doc*_tmp82C[5];s=((_tmp82C[4]=Cyc_PP_text(((
_tmp82D=")",_tag_dyneither(_tmp82D,sizeof(char),2)))),((_tmp82C[3]=Cyc_Absynpp_exp2doc(
_tmp2B7),((_tmp82C[2]=Cyc_PP_text(((_tmp82E=",",_tag_dyneither(_tmp82E,sizeof(
char),2)))),((_tmp82C[1]=Cyc_Absynpp_exp2doc(_tmp2B6),((_tmp82C[0]=Cyc_PP_text(((
_tmp82F="cycswap(",_tag_dyneither(_tmp82F,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp82C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL193;
_LL1E2: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2B8=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp247;if(_tmp2B8->tag != 36)goto _LL1E4;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B8->f2;}}
_LL1E3:{const char*_tmp830;s=Cyc_Absynpp_group_braces(((_tmp830=",",
_tag_dyneither(_tmp830,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2BA));}goto _LL193;_LL1E4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2BB=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp247;if(_tmp2BB->tag != 37)goto _LL193;else{_tmp2BC=
_tmp2BB->f1;}}_LL1E5:{const char*_tmp835;const char*_tmp834;struct Cyc_PP_Doc*
_tmp833[7];s=((_tmp833[6]=Cyc_PP_text(((_tmp834=")",_tag_dyneither(_tmp834,
sizeof(char),2)))),((_tmp833[5]=Cyc_Absynpp_rb(),((_tmp833[4]=Cyc_PP_blank_doc(),((
_tmp833[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2BC,1)),((_tmp833[2]=Cyc_PP_blank_doc(),((
_tmp833[1]=Cyc_Absynpp_lb(),((_tmp833[0]=Cyc_PP_text(((_tmp835="(",
_tag_dyneither(_tmp835,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp833,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL193;_LL193:;}if(inprec >= 
myprec){const char*_tmp83A;const char*_tmp839;struct Cyc_PP_Doc*_tmp838[3];s=((
_tmp838[2]=Cyc_PP_text(((_tmp839=")",_tag_dyneither(_tmp839,sizeof(char),2)))),((
_tmp838[1]=s,((_tmp838[0]=Cyc_PP_text(((_tmp83A="(",_tag_dyneither(_tmp83A,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp838,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp337=d;struct Cyc_Absyn_Exp*_tmp339;
struct _dyneither_ptr*_tmp33B;_LL1EC: {struct Cyc_Absyn_ArrayElement_struct*
_tmp338=(struct Cyc_Absyn_ArrayElement_struct*)_tmp337;if(_tmp338->tag != 0)goto
_LL1EE;else{_tmp339=_tmp338->f1;}}_LL1ED: {const char*_tmp83F;const char*_tmp83E;
struct Cyc_PP_Doc*_tmp83D[3];return(_tmp83D[2]=Cyc_PP_text(((_tmp83E="]",
_tag_dyneither(_tmp83E,sizeof(char),2)))),((_tmp83D[1]=Cyc_Absynpp_exp2doc(
_tmp339),((_tmp83D[0]=Cyc_PP_text(((_tmp83F=".[",_tag_dyneither(_tmp83F,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp83D,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1EE: {struct Cyc_Absyn_FieldName_struct*_tmp33A=(struct Cyc_Absyn_FieldName_struct*)
_tmp337;if(_tmp33A->tag != 1)goto _LL1EB;else{_tmp33B=_tmp33A->f1;}}_LL1EF: {const
char*_tmp842;struct Cyc_PP_Doc*_tmp841[2];return(_tmp841[1]=Cyc_PP_textptr(
_tmp33B),((_tmp841[0]=Cyc_PP_text(((_tmp842=".",_tag_dyneither(_tmp842,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp841,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1EB:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp849;const char*_tmp848;const char*_tmp847;struct Cyc_PP_Doc*
_tmp846[2];return(_tmp846[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp846[0]=Cyc_PP_egroup(((
_tmp849="",_tag_dyneither(_tmp849,sizeof(char),1))),((_tmp848="=",_tag_dyneither(
_tmp848,sizeof(char),2))),((_tmp847="=",_tag_dyneither(_tmp847,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp846,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp84C;const char*_tmp84B;const char*_tmp84A;
return Cyc_PP_group(((_tmp84A="",_tag_dyneither(_tmp84A,sizeof(char),1))),((
_tmp84B="",_tag_dyneither(_tmp84B,sizeof(char),1))),((_tmp84C=",",_tag_dyneither(
_tmp84C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp348=
c;struct _tuple3 _tmp349;enum Cyc_Absyn_Sign _tmp34A;char _tmp34B;struct _tuple4
_tmp34C;enum Cyc_Absyn_Sign _tmp34D;short _tmp34E;struct _tuple5 _tmp34F;enum Cyc_Absyn_Sign
_tmp350;int _tmp351;struct _tuple5 _tmp352;enum Cyc_Absyn_Sign _tmp353;int _tmp354;
struct _tuple5 _tmp355;enum Cyc_Absyn_Sign _tmp356;int _tmp357;struct _tuple6 _tmp358;
enum Cyc_Absyn_Sign _tmp359;long long _tmp35A;struct _dyneither_ptr _tmp35B;int
_tmp35C;struct _dyneither_ptr _tmp35D;_LL1F1: if((_tmp348.Char_c).tag != 2)goto
_LL1F3;_tmp349=(struct _tuple3)(_tmp348.Char_c).val;_tmp34A=_tmp349.f1;_tmp34B=
_tmp349.f2;_LL1F2: {const char*_tmp850;void*_tmp84F[1];struct Cyc_String_pa_struct
_tmp84E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84E.tag=0,((_tmp84E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34B)),((
_tmp84F[0]=& _tmp84E,Cyc_aprintf(((_tmp850="'%s'",_tag_dyneither(_tmp850,sizeof(
char),5))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))))));}_LL1F3: if((_tmp348.Short_c).tag
!= 3)goto _LL1F5;_tmp34C=(struct _tuple4)(_tmp348.Short_c).val;_tmp34D=_tmp34C.f1;
_tmp34E=_tmp34C.f2;_LL1F4: {const char*_tmp854;void*_tmp853[1];struct Cyc_Int_pa_struct
_tmp852;return Cyc_PP_text((struct _dyneither_ptr)((_tmp852.tag=1,((_tmp852.f1=(
unsigned long)((int)_tmp34E),((_tmp853[0]=& _tmp852,Cyc_aprintf(((_tmp854="%d",
_tag_dyneither(_tmp854,sizeof(char),3))),_tag_dyneither(_tmp853,sizeof(void*),1)))))))));}
_LL1F5: if((_tmp348.Int_c).tag != 4)goto _LL1F7;_tmp34F=(struct _tuple5)(_tmp348.Int_c).val;
_tmp350=_tmp34F.f1;if(_tmp350 != Cyc_Absyn_None)goto _LL1F7;_tmp351=_tmp34F.f2;
_LL1F6: _tmp354=_tmp351;goto _LL1F8;_LL1F7: if((_tmp348.Int_c).tag != 4)goto _LL1F9;
_tmp352=(struct _tuple5)(_tmp348.Int_c).val;_tmp353=_tmp352.f1;if(_tmp353 != Cyc_Absyn_Signed)
goto _LL1F9;_tmp354=_tmp352.f2;_LL1F8: {const char*_tmp858;void*_tmp857[1];struct
Cyc_Int_pa_struct _tmp856;return Cyc_PP_text((struct _dyneither_ptr)((_tmp856.tag=1,((
_tmp856.f1=(unsigned long)_tmp354,((_tmp857[0]=& _tmp856,Cyc_aprintf(((_tmp858="%d",
_tag_dyneither(_tmp858,sizeof(char),3))),_tag_dyneither(_tmp857,sizeof(void*),1)))))))));}
_LL1F9: if((_tmp348.Int_c).tag != 4)goto _LL1FB;_tmp355=(struct _tuple5)(_tmp348.Int_c).val;
_tmp356=_tmp355.f1;if(_tmp356 != Cyc_Absyn_Unsigned)goto _LL1FB;_tmp357=_tmp355.f2;
_LL1FA: {const char*_tmp85C;void*_tmp85B[1];struct Cyc_Int_pa_struct _tmp85A;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp85A.tag=1,((_tmp85A.f1=(unsigned int)
_tmp357,((_tmp85B[0]=& _tmp85A,Cyc_aprintf(((_tmp85C="%u",_tag_dyneither(_tmp85C,
sizeof(char),3))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))))));}_LL1FB: if((
_tmp348.LongLong_c).tag != 5)goto _LL1FD;_tmp358=(struct _tuple6)(_tmp348.LongLong_c).val;
_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;_LL1FC: {const char*_tmp85D;return Cyc_PP_text(((
_tmp85D="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp85D,sizeof(char),27))));}
_LL1FD: if((_tmp348.Float_c).tag != 6)goto _LL1FF;_tmp35B=(struct _dyneither_ptr)(
_tmp348.Float_c).val;_LL1FE: return Cyc_PP_text(_tmp35B);_LL1FF: if((_tmp348.Null_c).tag
!= 1)goto _LL201;_tmp35C=(int)(_tmp348.Null_c).val;_LL200: {const char*_tmp85E;
return Cyc_PP_text(((_tmp85E="NULL",_tag_dyneither(_tmp85E,sizeof(char),5))));}
_LL201: if((_tmp348.String_c).tag != 7)goto _LL1F0;_tmp35D=(struct _dyneither_ptr)(
_tmp348.String_c).val;_LL202: {const char*_tmp863;const char*_tmp862;struct Cyc_PP_Doc*
_tmp861[3];return(_tmp861[2]=Cyc_PP_text(((_tmp862="\"",_tag_dyneither(_tmp862,
sizeof(char),2)))),((_tmp861[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp35D)),((
_tmp861[0]=Cyc_PP_text(((_tmp863="\"",_tag_dyneither(_tmp863,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp861,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F0:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp870;const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_struct _tmp86D;
struct Cyc_Core_Failure_struct*_tmp86C;(int)_throw((void*)((_tmp86C=_cycalloc(
sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp870.tag=Cyc_Core_Failure,((_tmp870.f1=(
struct _dyneither_ptr)((_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp86E[0]=& _tmp86D,Cyc_aprintf(((
_tmp86F="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp86F,
sizeof(char),47))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),_tmp870)))),
_tmp86C)))));}{const char*_tmp875;const char*_tmp874;struct Cyc_PP_Doc*_tmp873[3];
return(_tmp873[2]=Cyc_PP_text(((_tmp874=")",_tag_dyneither(_tmp874,sizeof(char),
2)))),((_tmp873[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp873[0]=
Cyc_PP_text(((_tmp875="numelts(",_tag_dyneither(_tmp875,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp873,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp882;const char*_tmp881;void*_tmp880[1];
struct Cyc_String_pa_struct _tmp87F;struct Cyc_Core_Failure_struct*_tmp87E;(int)
_throw((void*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp882.tag=
Cyc_Core_Failure,((_tmp882.f1=(struct _dyneither_ptr)((_tmp87F.tag=0,((_tmp87F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp880[0]=& _tmp87F,Cyc_aprintf(((_tmp881="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp881,sizeof(char),38))),_tag_dyneither(_tmp880,sizeof(void*),1)))))))),
_tmp882)))),_tmp87E)))));}else{if(ds->tl == 0){const char*_tmp885;struct Cyc_PP_Doc*
_tmp884[3];return(_tmp884[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp884[1]=Cyc_PP_text(((
_tmp885=" ",_tag_dyneither(_tmp885,sizeof(char),2)))),((_tmp884[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp884,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp892;const char*
_tmp891;void*_tmp890[1];struct Cyc_String_pa_struct _tmp88F;struct Cyc_Core_Failure_struct*
_tmp88E;(int)_throw((void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((
_tmp892.tag=Cyc_Core_Failure,((_tmp892.f1=(struct _dyneither_ptr)((_tmp88F.tag=0,((
_tmp88F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp890[0]=& _tmp88F,Cyc_aprintf(((_tmp891="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp891,sizeof(char),47))),_tag_dyneither(_tmp890,sizeof(void*),1)))))))),
_tmp892)))),_tmp88E)))));}else{const char*_tmp897;const char*_tmp896;struct Cyc_PP_Doc*
_tmp895[5];return(_tmp895[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp895[3]=Cyc_PP_text(((_tmp896=" ",_tag_dyneither(
_tmp896,sizeof(char),2)))),((_tmp895[2]=ps,((_tmp895[1]=Cyc_PP_text(((_tmp897=" ",
_tag_dyneither(_tmp897,sizeof(char),2)))),((_tmp895[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp895,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL203: {const char*_tmp898;return(_tmp898="+",_tag_dyneither(_tmp898,sizeof(char),
2));}case Cyc_Absyn_Times: _LL204: {const char*_tmp899;return(_tmp899="*",
_tag_dyneither(_tmp899,sizeof(char),2));}case Cyc_Absyn_Minus: _LL205: {const char*
_tmp89A;return(_tmp89A="-",_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Div:
_LL206: {const char*_tmp89B;return(_tmp89B="/",_tag_dyneither(_tmp89B,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL207: {const char*_tmp89D;const char*_tmp89C;return Cyc_Absynpp_print_for_cycdoc?(
_tmp89D="\\%",_tag_dyneither(_tmp89D,sizeof(char),3)):((_tmp89C="%",
_tag_dyneither(_tmp89C,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL208: {const char*
_tmp89E;return(_tmp89E="==",_tag_dyneither(_tmp89E,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL209: {const char*_tmp89F;return(_tmp89F="!=",_tag_dyneither(_tmp89F,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL20A: {const char*_tmp8A0;return(_tmp8A0=">",
_tag_dyneither(_tmp8A0,sizeof(char),2));}case Cyc_Absyn_Lt: _LL20B: {const char*
_tmp8A1;return(_tmp8A1="<",_tag_dyneither(_tmp8A1,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL20C: {const char*_tmp8A2;return(_tmp8A2=">=",_tag_dyneither(_tmp8A2,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL20D: {const char*_tmp8A3;return(_tmp8A3="<=",
_tag_dyneither(_tmp8A3,sizeof(char),3));}case Cyc_Absyn_Not: _LL20E: {const char*
_tmp8A4;return(_tmp8A4="!",_tag_dyneither(_tmp8A4,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL20F: {const char*_tmp8A5;return(_tmp8A5="~",_tag_dyneither(_tmp8A5,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL210: {const char*_tmp8A6;return(_tmp8A6="&",
_tag_dyneither(_tmp8A6,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL211: {const char*
_tmp8A7;return(_tmp8A7="|",_tag_dyneither(_tmp8A7,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL212: {const char*_tmp8A8;return(_tmp8A8="^",_tag_dyneither(_tmp8A8,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL213: {const char*_tmp8A9;return(_tmp8A9="<<",
_tag_dyneither(_tmp8A9,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL214: {const
char*_tmp8AA;return(_tmp8AA=">>",_tag_dyneither(_tmp8AA,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL215: {const char*_tmp8AB;return(_tmp8AB=">>>",
_tag_dyneither(_tmp8AB,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL216: {const
char*_tmp8AC;return(_tmp8AC="numelts",_tag_dyneither(_tmp8AC,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp39B=s->r;_LL219: {struct Cyc_Absyn_Decl_s_struct*
_tmp39C=(struct Cyc_Absyn_Decl_s_struct*)_tmp39B;if(_tmp39C->tag != 12)goto _LL21B;}
_LL21A: return 1;_LL21B:;_LL21C: return 0;_LL218:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp39D=st->r;struct Cyc_Absyn_Exp*
_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Stmt*_tmp3A3;struct Cyc_Absyn_Exp*
_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Stmt*
_tmp3A9;struct _tuple8 _tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Stmt*
_tmp3AD;struct _dyneither_ptr*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;struct _tuple8
_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;struct _tuple8 _tmp3B6;struct Cyc_Absyn_Exp*
_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*
_tmp3BB;struct Cyc_List_List*_tmp3BD;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Decl*
_tmp3C1;struct Cyc_Absyn_Stmt*_tmp3C2;struct _dyneither_ptr*_tmp3C4;struct Cyc_Absyn_Stmt*
_tmp3C5;struct Cyc_Absyn_Stmt*_tmp3C7;struct _tuple8 _tmp3C8;struct Cyc_Absyn_Exp*
_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CB;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Exp*
_tmp3CE;_LL21E: {struct Cyc_Absyn_Skip_s_struct*_tmp39E=(struct Cyc_Absyn_Skip_s_struct*)
_tmp39D;if(_tmp39E->tag != 0)goto _LL220;}_LL21F:{const char*_tmp8AD;s=Cyc_PP_text(((
_tmp8AD=";",_tag_dyneither(_tmp8AD,sizeof(char),2))));}goto _LL21D;_LL220: {
struct Cyc_Absyn_Exp_s_struct*_tmp39F=(struct Cyc_Absyn_Exp_s_struct*)_tmp39D;if(
_tmp39F->tag != 1)goto _LL222;else{_tmp3A0=_tmp39F->f1;}}_LL221:{const char*_tmp8B0;
struct Cyc_PP_Doc*_tmp8AF[2];s=((_tmp8AF[1]=Cyc_PP_text(((_tmp8B0=";",
_tag_dyneither(_tmp8B0,sizeof(char),2)))),((_tmp8AF[0]=Cyc_Absynpp_exp2doc(
_tmp3A0),Cyc_PP_cat(_tag_dyneither(_tmp8AF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL21D;_LL222: {struct Cyc_Absyn_Seq_s_struct*_tmp3A1=(struct Cyc_Absyn_Seq_s_struct*)
_tmp39D;if(_tmp3A1->tag != 2)goto _LL224;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A1->f2;}}
_LL223: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp3A2)){
struct Cyc_PP_Doc*_tmp8BB[5];struct Cyc_PP_Doc*_tmp8BA[7];const char*_tmp8B9;const
char*_tmp8B8;struct Cyc_PP_Doc*_tmp8B7[7];s=((_tmp8B7[6]=Cyc_Absynpp_is_declaration(
_tmp3A3)?stmtexp?(_tmp8BA[6]=Cyc_PP_line_doc(),((_tmp8BA[5]=Cyc_PP_text(((
_tmp8B9=");",_tag_dyneither(_tmp8B9,sizeof(char),3)))),((_tmp8BA[4]=Cyc_Absynpp_rb(),((
_tmp8BA[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp)),((_tmp8BA[2]=Cyc_PP_blank_doc(),((
_tmp8BA[1]=Cyc_Absynpp_lb(),((_tmp8BA[0]=Cyc_PP_text(((_tmp8B8="(",
_tag_dyneither(_tmp8B8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BA,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8BB[4]=Cyc_PP_line_doc(),((
_tmp8BB[3]=Cyc_Absynpp_rb(),((_tmp8BB[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A3,stmtexp)),((_tmp8BB[1]=Cyc_PP_blank_doc(),((_tmp8BB[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp3A3,stmtexp),((_tmp8B7[5]=Cyc_PP_line_doc(),((_tmp8B7[4]=Cyc_Absynpp_rb(),((
_tmp8B7[3]=Cyc_PP_line_doc(),((_tmp8B7[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A2,0)),((_tmp8B7[1]=Cyc_PP_blank_doc(),((_tmp8B7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp3A3)){struct Cyc_PP_Doc*_tmp8C6[4];struct Cyc_PP_Doc*_tmp8C5[6];const char*
_tmp8C4;const char*_tmp8C3;struct Cyc_PP_Doc*_tmp8C2[4];s=((_tmp8C2[3]=Cyc_PP_line_doc(),((
_tmp8C2[2]=stmtexp?(_tmp8C5[5]=Cyc_PP_text(((_tmp8C4=");",_tag_dyneither(_tmp8C4,
sizeof(char),3)))),((_tmp8C5[4]=Cyc_Absynpp_rb(),((_tmp8C5[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A3,stmtexp)),((_tmp8C5[2]=Cyc_PP_blank_doc(),((_tmp8C5[1]=Cyc_Absynpp_lb(),((
_tmp8C5[0]=Cyc_PP_text(((_tmp8C3="(",_tag_dyneither(_tmp8C3,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8C6[3]=Cyc_Absynpp_rb(),((_tmp8C6[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3A3,stmtexp)),((_tmp8C6[1]=Cyc_PP_blank_doc(),((_tmp8C6[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8C6,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8C2[1]=Cyc_PP_line_doc(),((_tmp8C2[0]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(
_tag_dyneither(_tmp8C2,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8C7[3];s=((_tmp8C7[2]=Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp),((_tmp8C7[1]=Cyc_PP_line_doc(),((
_tmp8C7[0]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(_tag_dyneither(_tmp8C7,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8C8[3];s=((
_tmp8C8[2]=Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp),((_tmp8C8[1]=Cyc_PP_line_doc(),((
_tmp8C8[0]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(_tag_dyneither(_tmp8C8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL21D;_LL224: {struct Cyc_Absyn_Return_s_struct*
_tmp3A4=(struct Cyc_Absyn_Return_s_struct*)_tmp39D;if(_tmp3A4->tag != 3)goto _LL226;
else{_tmp3A5=_tmp3A4->f1;}}_LL225: if(_tmp3A5 == 0){const char*_tmp8C9;s=Cyc_PP_text(((
_tmp8C9="return;",_tag_dyneither(_tmp8C9,sizeof(char),8))));}else{const char*
_tmp8CE;const char*_tmp8CD;struct Cyc_PP_Doc*_tmp8CC[3];s=((_tmp8CC[2]=Cyc_PP_text(((
_tmp8CD=";",_tag_dyneither(_tmp8CD,sizeof(char),2)))),((_tmp8CC[1]=_tmp3A5 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3A5),((_tmp8CC[0]=
Cyc_PP_text(((_tmp8CE="return ",_tag_dyneither(_tmp8CE,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8CC,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL21D;_LL226: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp3A6=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp39D;if(_tmp3A6->tag != 4)goto _LL228;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;
_tmp3A9=_tmp3A6->f3;}}_LL227: {int print_else;{void*_tmp3E2=_tmp3A9->r;_LL243: {
struct Cyc_Absyn_Skip_s_struct*_tmp3E3=(struct Cyc_Absyn_Skip_s_struct*)_tmp3E2;
if(_tmp3E3->tag != 0)goto _LL245;}_LL244: print_else=0;goto _LL242;_LL245:;_LL246:
print_else=1;goto _LL242;_LL242:;}{const char*_tmp8DD;const char*_tmp8DC;struct Cyc_PP_Doc*
_tmp8DB[2];struct Cyc_PP_Doc*_tmp8DA[6];struct Cyc_PP_Doc*_tmp8D9[2];const char*
_tmp8D8;struct Cyc_PP_Doc*_tmp8D7[8];s=((_tmp8D7[7]=print_else?(_tmp8DA[5]=Cyc_Absynpp_rb(),((
_tmp8DA[4]=Cyc_PP_line_doc(),((_tmp8DA[3]=Cyc_PP_nest(2,((_tmp8D9[1]=Cyc_Absynpp_stmt2doc(
_tmp3A9,0),((_tmp8D9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D9,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DA[2]=Cyc_Absynpp_lb(),((_tmp8DA[1]=
Cyc_PP_text(((_tmp8D8="else ",_tag_dyneither(_tmp8D8,sizeof(char),6)))),((
_tmp8DA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DA,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8D7[6]=Cyc_Absynpp_rb(),((_tmp8D7[5]=Cyc_PP_line_doc(),((
_tmp8D7[4]=Cyc_PP_nest(2,((_tmp8DB[1]=Cyc_Absynpp_stmt2doc(_tmp3A8,0),((_tmp8DB[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DB,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8D7[3]=Cyc_Absynpp_lb(),((_tmp8D7[2]=Cyc_PP_text(((_tmp8DC=") ",
_tag_dyneither(_tmp8DC,sizeof(char),3)))),((_tmp8D7[1]=Cyc_Absynpp_exp2doc(
_tmp3A7),((_tmp8D7[0]=Cyc_PP_text(((_tmp8DD="if (",_tag_dyneither(_tmp8DD,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL21D;}_LL228: {struct Cyc_Absyn_While_s_struct*_tmp3AA=(struct Cyc_Absyn_While_s_struct*)
_tmp39D;if(_tmp3AA->tag != 5)goto _LL22A;else{_tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AB.f1;
_tmp3AD=_tmp3AA->f2;}}_LL229:{const char*_tmp8E4;const char*_tmp8E3;struct Cyc_PP_Doc*
_tmp8E2[2];struct Cyc_PP_Doc*_tmp8E1[7];s=((_tmp8E1[6]=Cyc_Absynpp_rb(),((_tmp8E1[
5]=Cyc_PP_line_doc(),((_tmp8E1[4]=Cyc_PP_nest(2,((_tmp8E2[1]=Cyc_Absynpp_stmt2doc(
_tmp3AD,0),((_tmp8E2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E2,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E1[3]=Cyc_Absynpp_lb(),((_tmp8E1[2]=
Cyc_PP_text(((_tmp8E3=") ",_tag_dyneither(_tmp8E3,sizeof(char),3)))),((_tmp8E1[1]=
Cyc_Absynpp_exp2doc(_tmp3AC),((_tmp8E1[0]=Cyc_PP_text(((_tmp8E4="while (",
_tag_dyneither(_tmp8E4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8E1,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL21D;_LL22A: {struct Cyc_Absyn_Break_s_struct*
_tmp3AE=(struct Cyc_Absyn_Break_s_struct*)_tmp39D;if(_tmp3AE->tag != 6)goto _LL22C;}
_LL22B:{const char*_tmp8E5;s=Cyc_PP_text(((_tmp8E5="break;",_tag_dyneither(
_tmp8E5,sizeof(char),7))));}goto _LL21D;_LL22C: {struct Cyc_Absyn_Continue_s_struct*
_tmp3AF=(struct Cyc_Absyn_Continue_s_struct*)_tmp39D;if(_tmp3AF->tag != 7)goto
_LL22E;}_LL22D:{const char*_tmp8E6;s=Cyc_PP_text(((_tmp8E6="continue;",
_tag_dyneither(_tmp8E6,sizeof(char),10))));}goto _LL21D;_LL22E: {struct Cyc_Absyn_Goto_s_struct*
_tmp3B0=(struct Cyc_Absyn_Goto_s_struct*)_tmp39D;if(_tmp3B0->tag != 8)goto _LL230;
else{_tmp3B1=_tmp3B0->f1;}}_LL22F:{const char*_tmp8EA;void*_tmp8E9[1];struct Cyc_String_pa_struct
_tmp8E8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8E8.tag=0,((_tmp8E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3B1),((_tmp8E9[0]=& _tmp8E8,Cyc_aprintf(((
_tmp8EA="goto %s;",_tag_dyneither(_tmp8EA,sizeof(char),9))),_tag_dyneither(
_tmp8E9,sizeof(void*),1)))))))));}goto _LL21D;_LL230: {struct Cyc_Absyn_For_s_struct*
_tmp3B2=(struct Cyc_Absyn_For_s_struct*)_tmp39D;if(_tmp3B2->tag != 9)goto _LL232;
else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B2->f3;
_tmp3B7=_tmp3B6.f1;_tmp3B8=_tmp3B2->f4;}}_LL231:{const char*_tmp8F5;const char*
_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_PP_Doc*_tmp8F1[2];struct
Cyc_PP_Doc*_tmp8F0[11];s=((_tmp8F0[10]=Cyc_Absynpp_rb(),((_tmp8F0[9]=Cyc_PP_line_doc(),((
_tmp8F0[8]=Cyc_PP_nest(2,((_tmp8F1[1]=Cyc_Absynpp_stmt2doc(_tmp3B8,0),((_tmp8F1[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F1,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8F0[7]=Cyc_Absynpp_lb(),((_tmp8F0[6]=Cyc_PP_text(((_tmp8F2=") ",
_tag_dyneither(_tmp8F2,sizeof(char),3)))),((_tmp8F0[5]=Cyc_Absynpp_exp2doc(
_tmp3B7),((_tmp8F0[4]=Cyc_PP_text(((_tmp8F3="; ",_tag_dyneither(_tmp8F3,sizeof(
char),3)))),((_tmp8F0[3]=Cyc_Absynpp_exp2doc(_tmp3B5),((_tmp8F0[2]=Cyc_PP_text(((
_tmp8F4="; ",_tag_dyneither(_tmp8F4,sizeof(char),3)))),((_tmp8F0[1]=Cyc_Absynpp_exp2doc(
_tmp3B3),((_tmp8F0[0]=Cyc_PP_text(((_tmp8F5="for(",_tag_dyneither(_tmp8F5,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL21D;_LL232: {struct Cyc_Absyn_Switch_s_struct*
_tmp3B9=(struct Cyc_Absyn_Switch_s_struct*)_tmp39D;if(_tmp3B9->tag != 10)goto
_LL234;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}_LL233:{const char*_tmp8FA;
const char*_tmp8F9;struct Cyc_PP_Doc*_tmp8F8[8];s=((_tmp8F8[7]=Cyc_Absynpp_rb(),((
_tmp8F8[6]=Cyc_PP_line_doc(),((_tmp8F8[5]=Cyc_Absynpp_switchclauses2doc(_tmp3BB),((
_tmp8F8[4]=Cyc_PP_line_doc(),((_tmp8F8[3]=Cyc_Absynpp_lb(),((_tmp8F8[2]=Cyc_PP_text(((
_tmp8F9=") ",_tag_dyneither(_tmp8F9,sizeof(char),3)))),((_tmp8F8[1]=Cyc_Absynpp_exp2doc(
_tmp3BA),((_tmp8F8[0]=Cyc_PP_text(((_tmp8FA="switch (",_tag_dyneither(_tmp8FA,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL21D;_LL234: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3BC=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp39D;if(_tmp3BC->tag != 11)goto _LL236;else{_tmp3BD=_tmp3BC->f1;if(_tmp3BD != 0)
goto _LL236;}}_LL235:{const char*_tmp8FB;s=Cyc_PP_text(((_tmp8FB="fallthru;",
_tag_dyneither(_tmp8FB,sizeof(char),10))));}goto _LL21D;_LL236: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3BE=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp39D;if(_tmp3BE->tag != 11)goto
_LL238;else{_tmp3BF=_tmp3BE->f1;}}_LL237:{const char*_tmp900;const char*_tmp8FF;
struct Cyc_PP_Doc*_tmp8FE[3];s=((_tmp8FE[2]=Cyc_PP_text(((_tmp8FF=");",
_tag_dyneither(_tmp8FF,sizeof(char),3)))),((_tmp8FE[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3BF),((_tmp8FE[0]=Cyc_PP_text(((_tmp900="fallthru(",_tag_dyneither(
_tmp900,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL21D;_LL238: {struct Cyc_Absyn_Decl_s_struct*_tmp3C0=(struct Cyc_Absyn_Decl_s_struct*)
_tmp39D;if(_tmp3C0->tag != 12)goto _LL23A;else{_tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;}}
_LL239:{struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=Cyc_Absynpp_stmt2doc(_tmp3C2,
stmtexp),((_tmp901[1]=Cyc_PP_line_doc(),((_tmp901[0]=Cyc_Absynpp_decl2doc(
_tmp3C1),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL21D;_LL23A: {struct Cyc_Absyn_Label_s_struct*_tmp3C3=(struct Cyc_Absyn_Label_s_struct*)
_tmp39D;if(_tmp3C3->tag != 13)goto _LL23C;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}
_LL23B: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C5)){if(
stmtexp){const char*_tmp906;const char*_tmp905;struct Cyc_PP_Doc*_tmp904[8];s=((
_tmp904[7]=Cyc_PP_text(((_tmp905=");",_tag_dyneither(_tmp905,sizeof(char),3)))),((
_tmp904[6]=Cyc_Absynpp_rb(),((_tmp904[5]=Cyc_PP_line_doc(),((_tmp904[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C5,1)),((_tmp904[3]=Cyc_PP_line_doc(),((_tmp904[2]=
Cyc_Absynpp_lb(),((_tmp904[1]=Cyc_PP_text(((_tmp906=": (",_tag_dyneither(_tmp906,
sizeof(char),4)))),((_tmp904[0]=Cyc_PP_textptr(_tmp3C4),Cyc_PP_cat(
_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp909;struct Cyc_PP_Doc*_tmp908[7];s=((_tmp908[6]=Cyc_Absynpp_rb(),((
_tmp908[5]=Cyc_PP_line_doc(),((_tmp908[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C5,0)),((_tmp908[3]=Cyc_PP_line_doc(),((_tmp908[2]=Cyc_Absynpp_lb(),((
_tmp908[1]=Cyc_PP_text(((_tmp909=": ",_tag_dyneither(_tmp909,sizeof(char),3)))),((
_tmp908[0]=Cyc_PP_textptr(_tmp3C4),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp90C;struct Cyc_PP_Doc*
_tmp90B[3];s=((_tmp90B[2]=Cyc_Absynpp_stmt2doc(_tmp3C5,stmtexp),((_tmp90B[1]=Cyc_PP_text(((
_tmp90C=": ",_tag_dyneither(_tmp90C,sizeof(char),3)))),((_tmp90B[0]=Cyc_PP_textptr(
_tmp3C4),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL21D;_LL23C: {struct Cyc_Absyn_Do_s_struct*_tmp3C6=(struct Cyc_Absyn_Do_s_struct*)
_tmp39D;if(_tmp3C6->tag != 14)goto _LL23E;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;
_tmp3C9=_tmp3C8.f1;}}_LL23D:{const char*_tmp913;const char*_tmp912;const char*
_tmp911;struct Cyc_PP_Doc*_tmp910[9];s=((_tmp910[8]=Cyc_PP_text(((_tmp911=");",
_tag_dyneither(_tmp911,sizeof(char),3)))),((_tmp910[7]=Cyc_Absynpp_exp2doc(
_tmp3C9),((_tmp910[6]=Cyc_PP_text(((_tmp912=" while (",_tag_dyneither(_tmp912,
sizeof(char),9)))),((_tmp910[5]=Cyc_Absynpp_rb(),((_tmp910[4]=Cyc_PP_line_doc(),((
_tmp910[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C7,0)),((_tmp910[2]=Cyc_PP_line_doc(),((
_tmp910[1]=Cyc_Absynpp_lb(),((_tmp910[0]=Cyc_PP_text(((_tmp913="do ",
_tag_dyneither(_tmp913,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp910,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL21D;_LL23E: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3CA=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp39D;if(_tmp3CA->tag != 15)goto
_LL240;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL23F:{const char*_tmp918;
const char*_tmp917;struct Cyc_PP_Doc*_tmp916[12];s=((_tmp916[11]=Cyc_Absynpp_rb(),((
_tmp916[10]=Cyc_PP_line_doc(),((_tmp916[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3CC)),((_tmp916[8]=Cyc_PP_line_doc(),((_tmp916[7]=Cyc_Absynpp_lb(),((_tmp916[
6]=Cyc_PP_text(((_tmp917=" catch ",_tag_dyneither(_tmp917,sizeof(char),8)))),((
_tmp916[5]=Cyc_Absynpp_rb(),((_tmp916[4]=Cyc_PP_line_doc(),((_tmp916[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3CB,0)),((_tmp916[2]=Cyc_PP_line_doc(),((_tmp916[1]=
Cyc_Absynpp_lb(),((_tmp916[0]=Cyc_PP_text(((_tmp918="try ",_tag_dyneither(
_tmp918,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp916,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL21D;_LL240: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3CD=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp39D;if(_tmp3CD->tag != 16)goto
_LL21D;else{_tmp3CE=_tmp3CD->f1;}}_LL241:{const char*_tmp91D;const char*_tmp91C;
struct Cyc_PP_Doc*_tmp91B[3];s=((_tmp91B[2]=Cyc_PP_text(((_tmp91C=");",
_tag_dyneither(_tmp91C,sizeof(char),3)))),((_tmp91B[1]=Cyc_Absynpp_exp2doc(
_tmp3CE),((_tmp91B[0]=Cyc_PP_text(((_tmp91D="reset_region(",_tag_dyneither(
_tmp91D,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL21D;_LL21D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp413=p->r;enum Cyc_Absyn_Sign _tmp417;int _tmp418;
char _tmp41A;struct _dyneither_ptr _tmp41C;struct Cyc_Absyn_Vardecl*_tmp41E;struct
Cyc_Absyn_Pat*_tmp41F;struct Cyc_Absyn_Pat _tmp420;void*_tmp421;struct Cyc_Absyn_Vardecl*
_tmp424;struct Cyc_Absyn_Pat*_tmp425;struct Cyc_Absyn_Tvar*_tmp427;struct Cyc_Absyn_Vardecl*
_tmp428;struct Cyc_List_List*_tmp42A;int _tmp42B;struct Cyc_Absyn_Pat*_tmp42D;
struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Pat*_tmp430;struct Cyc_Absyn_Pat
_tmp431;void*_tmp432;struct Cyc_Absyn_Vardecl*_tmp435;struct Cyc_Absyn_Pat*_tmp436;
struct _tuple0*_tmp438;struct _tuple0*_tmp43A;struct Cyc_List_List*_tmp43B;int
_tmp43C;struct Cyc_Absyn_AggrInfo*_tmp43E;struct Cyc_Absyn_AggrInfo _tmp43F;union
Cyc_Absyn_AggrInfoU _tmp440;struct Cyc_List_List*_tmp441;struct Cyc_List_List*
_tmp442;int _tmp443;struct Cyc_Absyn_AggrInfo*_tmp445;struct Cyc_List_List*_tmp446;
struct Cyc_List_List*_tmp447;int _tmp448;struct Cyc_Absyn_Enumfield*_tmp44A;struct
Cyc_Absyn_Enumfield*_tmp44C;struct Cyc_Absyn_Datatypefield*_tmp44E;struct Cyc_List_List*
_tmp44F;struct Cyc_Absyn_Datatypefield*_tmp451;struct Cyc_List_List*_tmp452;int
_tmp453;struct Cyc_Absyn_Exp*_tmp455;_LL248: {struct Cyc_Absyn_Wild_p_struct*
_tmp414=(struct Cyc_Absyn_Wild_p_struct*)_tmp413;if(_tmp414->tag != 0)goto _LL24A;}
_LL249:{const char*_tmp91E;s=Cyc_PP_text(((_tmp91E="_",_tag_dyneither(_tmp91E,
sizeof(char),2))));}goto _LL247;_LL24A: {struct Cyc_Absyn_Null_p_struct*_tmp415=(
struct Cyc_Absyn_Null_p_struct*)_tmp413;if(_tmp415->tag != 8)goto _LL24C;}_LL24B:{
const char*_tmp91F;s=Cyc_PP_text(((_tmp91F="NULL",_tag_dyneither(_tmp91F,sizeof(
char),5))));}goto _LL247;_LL24C: {struct Cyc_Absyn_Int_p_struct*_tmp416=(struct Cyc_Absyn_Int_p_struct*)
_tmp413;if(_tmp416->tag != 9)goto _LL24E;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;}}
_LL24D: if(_tmp417 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp923;
void*_tmp922[1];struct Cyc_Int_pa_struct _tmp921;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp921.tag=1,((_tmp921.f1=(unsigned long)_tmp418,((_tmp922[0]=&
_tmp921,Cyc_aprintf(((_tmp923="%d",_tag_dyneither(_tmp923,sizeof(char),3))),
_tag_dyneither(_tmp922,sizeof(void*),1)))))))));}else{const char*_tmp927;void*
_tmp926[1];struct Cyc_Int_pa_struct _tmp925;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp925.tag=1,((_tmp925.f1=(unsigned int)_tmp418,((_tmp926[0]=& _tmp925,Cyc_aprintf(((
_tmp927="%u",_tag_dyneither(_tmp927,sizeof(char),3))),_tag_dyneither(_tmp926,
sizeof(void*),1)))))))));}goto _LL247;_LL24E: {struct Cyc_Absyn_Char_p_struct*
_tmp419=(struct Cyc_Absyn_Char_p_struct*)_tmp413;if(_tmp419->tag != 10)goto _LL250;
else{_tmp41A=_tmp419->f1;}}_LL24F:{const char*_tmp92B;void*_tmp92A[1];struct Cyc_String_pa_struct
_tmp929;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp929.tag=0,((_tmp929.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp41A)),((
_tmp92A[0]=& _tmp929,Cyc_aprintf(((_tmp92B="'%s'",_tag_dyneither(_tmp92B,sizeof(
char),5))),_tag_dyneither(_tmp92A,sizeof(void*),1)))))))));}goto _LL247;_LL250: {
struct Cyc_Absyn_Float_p_struct*_tmp41B=(struct Cyc_Absyn_Float_p_struct*)_tmp413;
if(_tmp41B->tag != 11)goto _LL252;else{_tmp41C=_tmp41B->f1;}}_LL251: s=Cyc_PP_text(
_tmp41C);goto _LL247;_LL252: {struct Cyc_Absyn_Var_p_struct*_tmp41D=(struct Cyc_Absyn_Var_p_struct*)
_tmp413;if(_tmp41D->tag != 1)goto _LL254;else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;
_tmp420=*_tmp41F;_tmp421=_tmp420.r;{struct Cyc_Absyn_Wild_p_struct*_tmp422=(
struct Cyc_Absyn_Wild_p_struct*)_tmp421;if(_tmp422->tag != 0)goto _LL254;};}}_LL253:
s=Cyc_Absynpp_qvar2doc(_tmp41E->name);goto _LL247;_LL254: {struct Cyc_Absyn_Var_p_struct*
_tmp423=(struct Cyc_Absyn_Var_p_struct*)_tmp413;if(_tmp423->tag != 1)goto _LL256;
else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}_LL255:{const char*_tmp92E;struct
Cyc_PP_Doc*_tmp92D[3];s=((_tmp92D[2]=Cyc_Absynpp_pat2doc(_tmp425),((_tmp92D[1]=
Cyc_PP_text(((_tmp92E=" as ",_tag_dyneither(_tmp92E,sizeof(char),5)))),((_tmp92D[
0]=Cyc_Absynpp_qvar2doc(_tmp424->name),Cyc_PP_cat(_tag_dyneither(_tmp92D,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL247;_LL256: {struct Cyc_Absyn_TagInt_p_struct*
_tmp426=(struct Cyc_Absyn_TagInt_p_struct*)_tmp413;if(_tmp426->tag != 3)goto _LL258;
else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}_LL257:{const char*_tmp933;const
char*_tmp932;struct Cyc_PP_Doc*_tmp931[4];s=((_tmp931[3]=Cyc_PP_text(((_tmp932=">",
_tag_dyneither(_tmp932,sizeof(char),2)))),((_tmp931[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp427)),((_tmp931[1]=Cyc_PP_text(((_tmp933="<",_tag_dyneither(_tmp933,sizeof(
char),2)))),((_tmp931[0]=Cyc_Absynpp_qvar2doc(_tmp428->name),Cyc_PP_cat(
_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;_LL258: {
struct Cyc_Absyn_Tuple_p_struct*_tmp429=(struct Cyc_Absyn_Tuple_p_struct*)_tmp413;
if(_tmp429->tag != 4)goto _LL25A;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}
_LL259:{const char*_tmp93C;const char*_tmp93B;const char*_tmp93A;const char*_tmp939;
struct Cyc_PP_Doc*_tmp938[4];s=((_tmp938[3]=_tmp42B?Cyc_PP_text(((_tmp939=", ...)",
_tag_dyneither(_tmp939,sizeof(char),7)))): Cyc_PP_text(((_tmp93A=")",
_tag_dyneither(_tmp93A,sizeof(char),2)))),((_tmp938[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp93B=",",_tag_dyneither(_tmp93B,
sizeof(char),2))),_tmp42A),((_tmp938[1]=Cyc_PP_text(((_tmp93C="(",_tag_dyneither(
_tmp93C,sizeof(char),2)))),((_tmp938[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;_LL25A: {
struct Cyc_Absyn_Pointer_p_struct*_tmp42C=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp413;if(_tmp42C->tag != 5)goto _LL25C;else{_tmp42D=_tmp42C->f1;}}_LL25B:{const
char*_tmp93F;struct Cyc_PP_Doc*_tmp93E[2];s=((_tmp93E[1]=Cyc_Absynpp_pat2doc(
_tmp42D),((_tmp93E[0]=Cyc_PP_text(((_tmp93F="&",_tag_dyneither(_tmp93F,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL247;_LL25C: {struct Cyc_Absyn_Reference_p_struct*_tmp42E=(struct Cyc_Absyn_Reference_p_struct*)
_tmp413;if(_tmp42E->tag != 2)goto _LL25E;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;
_tmp431=*_tmp430;_tmp432=_tmp431.r;{struct Cyc_Absyn_Wild_p_struct*_tmp433=(
struct Cyc_Absyn_Wild_p_struct*)_tmp432;if(_tmp433->tag != 0)goto _LL25E;};}}_LL25D:{
const char*_tmp942;struct Cyc_PP_Doc*_tmp941[2];s=((_tmp941[1]=Cyc_Absynpp_qvar2doc(
_tmp42F->name),((_tmp941[0]=Cyc_PP_text(((_tmp942="*",_tag_dyneither(_tmp942,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL247;_LL25E: {struct Cyc_Absyn_Reference_p_struct*_tmp434=(struct Cyc_Absyn_Reference_p_struct*)
_tmp413;if(_tmp434->tag != 2)goto _LL260;else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}
_LL25F:{const char*_tmp947;const char*_tmp946;struct Cyc_PP_Doc*_tmp945[4];s=((
_tmp945[3]=Cyc_Absynpp_pat2doc(_tmp436),((_tmp945[2]=Cyc_PP_text(((_tmp946=" as ",
_tag_dyneither(_tmp946,sizeof(char),5)))),((_tmp945[1]=Cyc_Absynpp_qvar2doc(
_tmp435->name),((_tmp945[0]=Cyc_PP_text(((_tmp947="*",_tag_dyneither(_tmp947,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp945,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL247;_LL260: {struct Cyc_Absyn_UnknownId_p_struct*_tmp437=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp413;if(_tmp437->tag != 14)goto _LL262;else{_tmp438=_tmp437->f1;}}_LL261: s=Cyc_Absynpp_qvar2doc(
_tmp438);goto _LL247;_LL262: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp439=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp413;if(_tmp439->tag != 15)goto _LL264;
else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;_tmp43C=_tmp439->f3;}}_LL263: {
const char*_tmp949;const char*_tmp948;struct _dyneither_ptr term=_tmp43C?(_tmp949=", ...)",
_tag_dyneither(_tmp949,sizeof(char),7)):((_tmp948=")",_tag_dyneither(_tmp948,
sizeof(char),2)));{const char*_tmp94E;const char*_tmp94D;struct Cyc_PP_Doc*_tmp94C[
2];s=((_tmp94C[1]=Cyc_PP_group(((_tmp94E="(",_tag_dyneither(_tmp94E,sizeof(char),
2))),term,((_tmp94D=",",_tag_dyneither(_tmp94D,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp43B)),((_tmp94C[0]=Cyc_Absynpp_qvar2doc(_tmp43A),Cyc_PP_cat(
_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL247;}_LL264: {
struct Cyc_Absyn_Aggr_p_struct*_tmp43D=(struct Cyc_Absyn_Aggr_p_struct*)_tmp413;
if(_tmp43D->tag != 6)goto _LL266;else{_tmp43E=_tmp43D->f1;if(_tmp43E == 0)goto
_LL266;_tmp43F=*_tmp43E;_tmp440=_tmp43F.aggr_info;_tmp441=_tmp43D->f2;_tmp442=
_tmp43D->f3;_tmp443=_tmp43D->f4;}}_LL265: {const char*_tmp950;const char*_tmp94F;
struct _dyneither_ptr term=_tmp443?(_tmp950=", ...}",_tag_dyneither(_tmp950,
sizeof(char),7)):((_tmp94F="}",_tag_dyneither(_tmp94F,sizeof(char),2)));struct
_tuple0*_tmp478;struct _tuple9 _tmp477=Cyc_Absyn_aggr_kinded_name(_tmp440);_tmp478=
_tmp477.f2;{const char*_tmp95B;const char*_tmp95A;const char*_tmp959;const char*
_tmp958;const char*_tmp957;struct Cyc_PP_Doc*_tmp956[4];s=((_tmp956[3]=Cyc_PP_group(((
_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),term,((_tmp957=",",
_tag_dyneither(_tmp957,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp442)),((_tmp956[2]=Cyc_PP_egroup(((_tmp95B="[",_tag_dyneither(_tmp95B,
sizeof(char),2))),((_tmp95A="]",_tag_dyneither(_tmp95A,sizeof(char),2))),((
_tmp959=",",_tag_dyneither(_tmp959,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp441))),((_tmp956[1]=
Cyc_Absynpp_lb(),((_tmp956[0]=Cyc_Absynpp_qvar2doc(_tmp478),Cyc_PP_cat(
_tag_dyneither(_tmp956,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL247;}_LL266: {
struct Cyc_Absyn_Aggr_p_struct*_tmp444=(struct Cyc_Absyn_Aggr_p_struct*)_tmp413;
if(_tmp444->tag != 6)goto _LL268;else{_tmp445=_tmp444->f1;if(_tmp445 != 0)goto
_LL268;_tmp446=_tmp444->f2;_tmp447=_tmp444->f3;_tmp448=_tmp444->f4;}}_LL267: {
const char*_tmp95D;const char*_tmp95C;struct _dyneither_ptr term=_tmp448?(_tmp95D=", ...}",
_tag_dyneither(_tmp95D,sizeof(char),7)):((_tmp95C="}",_tag_dyneither(_tmp95C,
sizeof(char),2)));{const char*_tmp968;const char*_tmp967;const char*_tmp966;const
char*_tmp965;const char*_tmp964;struct Cyc_PP_Doc*_tmp963[3];s=((_tmp963[2]=Cyc_PP_group(((
_tmp965="",_tag_dyneither(_tmp965,sizeof(char),1))),term,((_tmp964=",",
_tag_dyneither(_tmp964,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp447)),((_tmp963[1]=Cyc_PP_egroup(((_tmp968="[",_tag_dyneither(_tmp968,
sizeof(char),2))),((_tmp967="]",_tag_dyneither(_tmp967,sizeof(char),2))),((
_tmp966=",",_tag_dyneither(_tmp966,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp446))),((_tmp963[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp963,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL247;}_LL268: {struct Cyc_Absyn_Enum_p_struct*_tmp449=(struct Cyc_Absyn_Enum_p_struct*)
_tmp413;if(_tmp449->tag != 12)goto _LL26A;else{_tmp44A=_tmp449->f2;}}_LL269:
_tmp44C=_tmp44A;goto _LL26B;_LL26A: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp44B=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp413;if(_tmp44B->tag != 13)goto _LL26C;else{
_tmp44C=_tmp44B->f2;}}_LL26B: s=Cyc_Absynpp_qvar2doc(_tmp44C->name);goto _LL247;
_LL26C: {struct Cyc_Absyn_Datatype_p_struct*_tmp44D=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp413;if(_tmp44D->tag != 7)goto _LL26E;else{_tmp44E=_tmp44D->f2;_tmp44F=_tmp44D->f3;
if(_tmp44F != 0)goto _LL26E;}}_LL26D: s=Cyc_Absynpp_qvar2doc(_tmp44E->name);goto
_LL247;_LL26E: {struct Cyc_Absyn_Datatype_p_struct*_tmp450=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp413;if(_tmp450->tag != 7)goto _LL270;else{_tmp451=_tmp450->f2;_tmp452=_tmp450->f3;
_tmp453=_tmp450->f4;}}_LL26F: {const char*_tmp96A;const char*_tmp969;struct
_dyneither_ptr term=_tmp453?(_tmp96A=", ...)",_tag_dyneither(_tmp96A,sizeof(char),
7)):((_tmp969=")",_tag_dyneither(_tmp969,sizeof(char),2)));{const char*_tmp96F;
const char*_tmp96E;struct Cyc_PP_Doc*_tmp96D[2];s=((_tmp96D[1]=Cyc_PP_egroup(((
_tmp96F="(",_tag_dyneither(_tmp96F,sizeof(char),2))),term,((_tmp96E=",",
_tag_dyneither(_tmp96E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp452)),((_tmp96D[0]=Cyc_Absynpp_qvar2doc(_tmp451->name),Cyc_PP_cat(
_tag_dyneither(_tmp96D,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL247;}_LL270: {
struct Cyc_Absyn_Exp_p_struct*_tmp454=(struct Cyc_Absyn_Exp_p_struct*)_tmp413;if(
_tmp454->tag != 16)goto _LL247;else{_tmp455=_tmp454->f1;}}_LL271: s=Cyc_Absynpp_exp2doc(
_tmp455);goto _LL247;_LL247:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){const char*
_tmp976;const char*_tmp975;const char*_tmp974;struct Cyc_PP_Doc*_tmp973[2];return(
_tmp973[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp973[0]=Cyc_PP_egroup(((_tmp976="",
_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp975="=",_tag_dyneither(_tmp975,
sizeof(char),2))),((_tmp974="=",_tag_dyneither(_tmp974,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp973,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[2];struct Cyc_PP_Doc*
_tmp979[2];return(_tmp979[1]=Cyc_PP_nest(2,((_tmp97A[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp97A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp979[0]=Cyc_PP_text(((_tmp97B="default: ",
_tag_dyneither(_tmp97B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp979,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp982;
const char*_tmp981;struct Cyc_PP_Doc*_tmp980[2];struct Cyc_PP_Doc*_tmp97F[4];return(
_tmp97F[3]=Cyc_PP_nest(2,((_tmp980[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp980[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp980,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp97F[2]=Cyc_PP_text(((_tmp981=": ",_tag_dyneither(_tmp981,sizeof(
char),3)))),((_tmp97F[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp97F[0]=Cyc_PP_text(((
_tmp982="case ",_tag_dyneither(_tmp982,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp97F,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp98B;const char*_tmp98A;const char*_tmp989;struct Cyc_PP_Doc*_tmp988[2];struct
Cyc_PP_Doc*_tmp987[6];return(_tmp987[5]=Cyc_PP_nest(2,((_tmp988[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp988[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp988,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp987[4]=Cyc_PP_text(((_tmp989=": ",
_tag_dyneither(_tmp989,sizeof(char),3)))),((_tmp987[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp987[2]=Cyc_PP_text(((
_tmp98A=" && ",_tag_dyneither(_tmp98A,sizeof(char),5)))),((_tmp987[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp987[0]=Cyc_PP_text(((_tmp98B="case ",_tag_dyneither(_tmp98B,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp987,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp98C;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp98C="",_tag_dyneither(_tmp98C,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp98F;struct Cyc_PP_Doc*_tmp98E[3];return(_tmp98E[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp98E[1]=Cyc_PP_text(((_tmp98F=" = ",
_tag_dyneither(_tmp98F,sizeof(char),4)))),((_tmp98E[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp990;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp990=",",_tag_dyneither(_tmp990,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp991;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp991=",",_tag_dyneither(_tmp991,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp4A4;enum Cyc_Absyn_Scope _tmp4A5;struct _tuple0*_tmp4A6;struct Cyc_Absyn_Tqual
_tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_List_List*_tmp4AA;
struct Cyc_Absyn_Vardecl*_tmp4A3=vd;_tmp4A4=*_tmp4A3;_tmp4A5=_tmp4A4.sc;_tmp4A6=
_tmp4A4.name;_tmp4A7=_tmp4A4.tq;_tmp4A8=_tmp4A4.type;_tmp4A9=_tmp4A4.initializer;
_tmp4AA=_tmp4A4.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4A6);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4AA);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL272:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL273:{void*_tmp4AB=Cyc_Tcutil_compress(
_tmp4A8);struct Cyc_Absyn_FnInfo _tmp4AD;struct Cyc_List_List*_tmp4AE;_LL276: {
struct Cyc_Absyn_FnType_struct*_tmp4AC=(struct Cyc_Absyn_FnType_struct*)_tmp4AB;
if(_tmp4AC->tag != 10)goto _LL278;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AD.attributes;}}
_LL277: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4AE);goto _LL275;_LL278:;_LL279:
beforenamedoc=Cyc_PP_nil_doc();goto _LL275;_LL275:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL27A: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL27B: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp99E;struct Cyc_PP_Doc*_tmp99D[2];struct Cyc_PP_Doc*_tmp99C[2];const char*
_tmp99B;const char*_tmp99A;struct Cyc_PP_Doc*_tmp999[5];s=((_tmp999[4]=Cyc_PP_text(((
_tmp99A=";",_tag_dyneither(_tmp99A,sizeof(char),2)))),((_tmp999[3]=_tmp4A9 == 0?
Cyc_PP_nil_doc():((_tmp99C[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4A9),((
_tmp99C[0]=Cyc_PP_text(((_tmp99B=" = ",_tag_dyneither(_tmp99B,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp999[2]=
Cyc_Absynpp_tqtd2doc(_tmp4A7,_tmp4A8,((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((
_tmp99E->v=((_tmp99D[1]=sn,((_tmp99D[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp99D,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp99E))))),((_tmp999[1]=Cyc_Absynpp_scope2doc(
_tmp4A5),((_tmp999[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4B6;
struct _tuple17 _tmp4B5=*x;_tmp4B6=_tmp4B5.f2;return Cyc_Absynpp_qvar2doc(_tmp4B6);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4B7=d->r;
struct Cyc_Absyn_Fndecl*_tmp4B9;struct Cyc_Absyn_Aggrdecl*_tmp4BB;struct Cyc_Absyn_Vardecl*
_tmp4BD;struct Cyc_Absyn_Tvar*_tmp4BF;struct Cyc_Absyn_Vardecl*_tmp4C0;int _tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Tvar*
_tmp4C5;struct Cyc_Absyn_Vardecl*_tmp4C6;struct Cyc_Absyn_Datatypedecl*_tmp4C8;
struct Cyc_Absyn_Datatypedecl _tmp4C9;enum Cyc_Absyn_Scope _tmp4CA;struct _tuple0*
_tmp4CB;struct Cyc_List_List*_tmp4CC;struct Cyc_Core_Opt*_tmp4CD;int _tmp4CE;struct
Cyc_Absyn_Pat*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_List_List*_tmp4D3;
struct Cyc_Absyn_Enumdecl*_tmp4D5;struct Cyc_Absyn_Enumdecl _tmp4D6;enum Cyc_Absyn_Scope
_tmp4D7;struct _tuple0*_tmp4D8;struct Cyc_Core_Opt*_tmp4D9;struct Cyc_Absyn_Typedefdecl*
_tmp4DB;struct _dyneither_ptr*_tmp4DD;struct Cyc_List_List*_tmp4DE;struct _tuple0*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E3;struct Cyc_List_List*
_tmp4E5;struct Cyc_List_List*_tmp4E6;_LL27E: {struct Cyc_Absyn_Fn_d_struct*_tmp4B8=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4B7;if(_tmp4B8->tag != 1)goto _LL280;else{_tmp4B9=
_tmp4B8->f1;}}_LL27F: {struct Cyc_Absyn_FnType_struct _tmp9A4;struct Cyc_Absyn_FnInfo
_tmp9A3;struct Cyc_Absyn_FnType_struct*_tmp9A2;void*t=(void*)((_tmp9A2=_cycalloc(
sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A4.tag=10,((_tmp9A4.f1=((_tmp9A3.tvars=
_tmp4B9->tvs,((_tmp9A3.effect=_tmp4B9->effect,((_tmp9A3.ret_typ=_tmp4B9->ret_type,((
_tmp9A3.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4B9->args),((
_tmp9A3.c_varargs=_tmp4B9->c_varargs,((_tmp9A3.cyc_varargs=_tmp4B9->cyc_varargs,((
_tmp9A3.rgn_po=_tmp4B9->rgn_po,((_tmp9A3.attributes=0,_tmp9A3)))))))))))))))),
_tmp9A4)))),_tmp9A2))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4B9->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4B9->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL29E:{const char*_tmp9A5;inlinedoc=Cyc_PP_text(((_tmp9A5="inline ",
_tag_dyneither(_tmp9A5,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL29F:{
const char*_tmp9A6;inlinedoc=Cyc_PP_text(((_tmp9A6="__inline ",_tag_dyneither(
_tmp9A6,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4B9->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2A1: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2A2: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4B9->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4B9->name);struct Cyc_PP_Doc*_tmp9A9[2];struct Cyc_Core_Opt*_tmp9A8;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9A8=_cycalloc(
sizeof(*_tmp9A8)),((_tmp9A8->v=((_tmp9A9[1]=namedoc,((_tmp9A9[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp9A9,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9A8)))));
struct Cyc_PP_Doc*_tmp9AC[2];struct Cyc_PP_Doc*_tmp9AB[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9AB[4]=Cyc_Absynpp_rb(),((_tmp9AB[3]=Cyc_PP_line_doc(),((_tmp9AB[2]=Cyc_PP_nest(
2,((_tmp9AC[1]=Cyc_Absynpp_stmt2doc(_tmp4B9->body,0),((_tmp9AC[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9AB[1]=
Cyc_Absynpp_lb(),((_tmp9AB[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9AB,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9AD[4];s=((
_tmp9AD[3]=bodydoc,((_tmp9AD[2]=tqtddoc,((_tmp9AD[1]=scopedoc,((_tmp9AD[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9AD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2A4:{struct Cyc_PP_Doc*
_tmp9AE[2];s=((_tmp9AE[1]=s,((_tmp9AE[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9AE,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2A5: break;}goto
_LL27D;};};}_LL280: {struct Cyc_Absyn_Aggr_d_struct*_tmp4BA=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4B7;if(_tmp4BA->tag != 6)goto _LL282;else{_tmp4BB=_tmp4BA->f1;}}_LL281: if(
_tmp4BB->impl == 0){const char*_tmp9B1;struct Cyc_PP_Doc*_tmp9B0[5];s=((_tmp9B0[4]=
Cyc_PP_text(((_tmp9B1=";",_tag_dyneither(_tmp9B1,sizeof(char),2)))),((_tmp9B0[3]=
Cyc_Absynpp_ktvars2doc(_tmp4BB->tvs),((_tmp9B0[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4BB->name),((_tmp9B0[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4BB->kind),((_tmp9B0[0]=
Cyc_Absynpp_scope2doc(_tmp4BB->sc),Cyc_PP_cat(_tag_dyneither(_tmp9B0,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9BB[2];const char*
_tmp9BA;struct Cyc_PP_Doc*_tmp9B9[2];const char*_tmp9B8;struct Cyc_PP_Doc*_tmp9B7[
13];s=((_tmp9B7[12]=Cyc_PP_text(((_tmp9B8=";",_tag_dyneither(_tmp9B8,sizeof(char),
2)))),((_tmp9B7[11]=Cyc_Absynpp_atts2doc(_tmp4BB->attributes),((_tmp9B7[10]=Cyc_Absynpp_rb(),((
_tmp9B7[9]=Cyc_PP_line_doc(),((_tmp9B7[8]=Cyc_PP_nest(2,((_tmp9B9[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BB->impl))->fields),((_tmp9B9[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B9,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9B7[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BB->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9BB[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4BB->impl))->rgn_po),((_tmp9BB[0]=Cyc_PP_text(((_tmp9BA=":",
_tag_dyneither(_tmp9BA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9BB,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9B7[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BB->impl))->exist_vars),((_tmp9B7[5]=
Cyc_Absynpp_lb(),((_tmp9B7[4]=Cyc_PP_blank_doc(),((_tmp9B7[3]=Cyc_Absynpp_ktvars2doc(
_tmp4BB->tvs),((_tmp9B7[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4BB->name),((_tmp9B7[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4BB->kind),((_tmp9B7[0]=Cyc_Absynpp_scope2doc(
_tmp4BB->sc),Cyc_PP_cat(_tag_dyneither(_tmp9B7,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL27D;_LL282: {struct Cyc_Absyn_Var_d_struct*_tmp4BC=(struct Cyc_Absyn_Var_d_struct*)
_tmp4B7;if(_tmp4BC->tag != 0)goto _LL284;else{_tmp4BD=_tmp4BC->f1;}}_LL283: s=Cyc_Absynpp_vardecl2doc(
_tmp4BD);goto _LL27D;_LL284: {struct Cyc_Absyn_Region_d_struct*_tmp4BE=(struct Cyc_Absyn_Region_d_struct*)
_tmp4B7;if(_tmp4BE->tag != 4)goto _LL286;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BE->f2;
_tmp4C1=_tmp4BE->f3;_tmp4C2=_tmp4BE->f4;}}_LL285:{const char*_tmp9CE;const char*
_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[3];const
char*_tmp9C9;const char*_tmp9C8;const char*_tmp9C7;struct Cyc_PP_Doc*_tmp9C6[8];s=((
_tmp9C6[7]=Cyc_PP_text(((_tmp9C7=";",_tag_dyneither(_tmp9C7,sizeof(char),2)))),((
_tmp9C6[6]=_tmp4C2 != 0?(_tmp9CA[2]=Cyc_PP_text(((_tmp9C9=")",_tag_dyneither(
_tmp9C9,sizeof(char),2)))),((_tmp9CA[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4C2),((_tmp9CA[0]=Cyc_PP_text(((_tmp9C8=" = open(",_tag_dyneither(_tmp9C8,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9C6[5]=_tmp4C1?Cyc_PP_text(((_tmp9CB=" @resetable",
_tag_dyneither(_tmp9CB,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9C6[4]=Cyc_Absynpp_qvar2doc(
_tmp4C0->name),((_tmp9C6[3]=Cyc_PP_text(((_tmp9CC=">",_tag_dyneither(_tmp9CC,
sizeof(char),2)))),((_tmp9C6[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4BF)),((
_tmp9C6[1]=Cyc_PP_text(((_tmp9CD="<",_tag_dyneither(_tmp9CD,sizeof(char),2)))),((
_tmp9C6[0]=Cyc_PP_text(((_tmp9CE="region",_tag_dyneither(_tmp9CE,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C6,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL27D;_LL286: {struct Cyc_Absyn_Alias_d_struct*_tmp4C3=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4B7;if(_tmp4C3->tag != 5)goto _LL288;else{_tmp4C4=_tmp4C3->f1;_tmp4C5=_tmp4C3->f2;
_tmp4C6=_tmp4C3->f3;}}_LL287:{const char*_tmp9D9;const char*_tmp9D8;const char*
_tmp9D7;const char*_tmp9D6;const char*_tmp9D5;struct Cyc_PP_Doc*_tmp9D4[8];s=((
_tmp9D4[7]=Cyc_PP_text(((_tmp9D5=";",_tag_dyneither(_tmp9D5,sizeof(char),2)))),((
_tmp9D4[6]=Cyc_Absynpp_exp2doc(_tmp4C4),((_tmp9D4[5]=Cyc_PP_text(((_tmp9D6=" = ",
_tag_dyneither(_tmp9D6,sizeof(char),4)))),((_tmp9D4[4]=Cyc_Absynpp_qvar2doc(
_tmp4C6->name),((_tmp9D4[3]=Cyc_PP_text(((_tmp9D7="> ",_tag_dyneither(_tmp9D7,
sizeof(char),3)))),((_tmp9D4[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C5)),((
_tmp9D4[1]=Cyc_PP_text(((_tmp9D8=" <",_tag_dyneither(_tmp9D8,sizeof(char),3)))),((
_tmp9D4[0]=Cyc_PP_text(((_tmp9D9="alias",_tag_dyneither(_tmp9D9,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9D4,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL27D;_LL288: {struct Cyc_Absyn_Datatype_d_struct*_tmp4C7=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4B7;if(_tmp4C7->tag != 7)goto _LL28A;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=*_tmp4C8;
_tmp4CA=_tmp4C9.sc;_tmp4CB=_tmp4C9.name;_tmp4CC=_tmp4C9.tvs;_tmp4CD=_tmp4C9.fields;
_tmp4CE=_tmp4C9.is_extensible;}}_LL289: if(_tmp4CD == 0){const char*_tmp9E0;const
char*_tmp9DF;const char*_tmp9DE;struct Cyc_PP_Doc*_tmp9DD[6];s=((_tmp9DD[5]=Cyc_PP_text(((
_tmp9DE=";",_tag_dyneither(_tmp9DE,sizeof(char),2)))),((_tmp9DD[4]=Cyc_Absynpp_ktvars2doc(
_tmp4CC),((_tmp9DD[3]=_tmp4CE?Cyc_Absynpp_qvar2bolddoc(_tmp4CB): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4CB),((_tmp9DD[2]=Cyc_PP_text(((_tmp9DF="datatype ",_tag_dyneither(_tmp9DF,
sizeof(char),10)))),((_tmp9DD[1]=_tmp4CE?Cyc_PP_text(((_tmp9E0="@extensible",
_tag_dyneither(_tmp9E0,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9DD[0]=Cyc_Absynpp_scope2doc(
_tmp4CA),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9E9;const char*_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[2];const char*
_tmp9E6;struct Cyc_PP_Doc*_tmp9E5[11];s=((_tmp9E5[10]=Cyc_PP_text(((_tmp9E6=";",
_tag_dyneither(_tmp9E6,sizeof(char),2)))),((_tmp9E5[9]=Cyc_Absynpp_rb(),((
_tmp9E5[8]=Cyc_PP_line_doc(),((_tmp9E5[7]=Cyc_PP_nest(2,((_tmp9E7[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4CD->v),((_tmp9E7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E5[6]=Cyc_Absynpp_lb(),((
_tmp9E5[5]=Cyc_PP_blank_doc(),((_tmp9E5[4]=Cyc_Absynpp_ktvars2doc(_tmp4CC),((
_tmp9E5[3]=_tmp4CE?Cyc_Absynpp_qvar2bolddoc(_tmp4CB): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4CB),((_tmp9E5[2]=Cyc_PP_text(((_tmp9E8="datatype ",_tag_dyneither(_tmp9E8,
sizeof(char),10)))),((_tmp9E5[1]=_tmp4CE?Cyc_PP_text(((_tmp9E9="@extensible ",
_tag_dyneither(_tmp9E9,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E5[0]=Cyc_Absynpp_scope2doc(
_tmp4CA),Cyc_PP_cat(_tag_dyneither(_tmp9E5,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL27D;_LL28A: {struct Cyc_Absyn_Let_d_struct*_tmp4CF=(struct Cyc_Absyn_Let_d_struct*)
_tmp4B7;if(_tmp4CF->tag != 2)goto _LL28C;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f3;}}
_LL28B:{const char*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_PP_Doc*
_tmp9ED[5];s=((_tmp9ED[4]=Cyc_PP_text(((_tmp9EE=";",_tag_dyneither(_tmp9EE,
sizeof(char),2)))),((_tmp9ED[3]=Cyc_Absynpp_exp2doc(_tmp4D1),((_tmp9ED[2]=Cyc_PP_text(((
_tmp9EF=" = ",_tag_dyneither(_tmp9EF,sizeof(char),4)))),((_tmp9ED[1]=Cyc_Absynpp_pat2doc(
_tmp4D0),((_tmp9ED[0]=Cyc_PP_text(((_tmp9F0="let ",_tag_dyneither(_tmp9F0,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL27D;_LL28C: {struct Cyc_Absyn_Letv_d_struct*_tmp4D2=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4B7;if(_tmp4D2->tag != 3)goto _LL28E;else{_tmp4D3=_tmp4D2->f1;}}_LL28D:{const
char*_tmp9F5;const char*_tmp9F4;struct Cyc_PP_Doc*_tmp9F3[3];s=((_tmp9F3[2]=Cyc_PP_text(((
_tmp9F4=";",_tag_dyneither(_tmp9F4,sizeof(char),2)))),((_tmp9F3[1]=Cyc_Absynpp_ids2doc(
_tmp4D3),((_tmp9F3[0]=Cyc_PP_text(((_tmp9F5="let ",_tag_dyneither(_tmp9F5,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL27D;_LL28E: {struct Cyc_Absyn_Enum_d_struct*_tmp4D4=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4B7;if(_tmp4D4->tag != 8)goto _LL290;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=*_tmp4D5;
_tmp4D7=_tmp4D6.sc;_tmp4D8=_tmp4D6.name;_tmp4D9=_tmp4D6.fields;}}_LL28F: if(
_tmp4D9 == 0){const char*_tmp9FA;const char*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[4];s=((
_tmp9F8[3]=Cyc_PP_text(((_tmp9F9=";",_tag_dyneither(_tmp9F9,sizeof(char),2)))),((
_tmp9F8[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D8),((_tmp9F8[1]=Cyc_PP_text(((
_tmp9FA="enum ",_tag_dyneither(_tmp9FA,sizeof(char),6)))),((_tmp9F8[0]=Cyc_Absynpp_scope2doc(
_tmp4D7),Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmpA01;struct Cyc_PP_Doc*_tmpA00[2];const char*_tmp9FF;struct Cyc_PP_Doc*
_tmp9FE[9];s=((_tmp9FE[8]=Cyc_PP_text(((_tmp9FF=";",_tag_dyneither(_tmp9FF,
sizeof(char),2)))),((_tmp9FE[7]=Cyc_Absynpp_rb(),((_tmp9FE[6]=Cyc_PP_line_doc(),((
_tmp9FE[5]=Cyc_PP_nest(2,((_tmpA00[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4D9->v),((_tmpA00[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA00,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9FE[4]=Cyc_Absynpp_lb(),((_tmp9FE[3]=
Cyc_PP_blank_doc(),((_tmp9FE[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4D8),((_tmp9FE[1]=
Cyc_PP_text(((_tmpA01="enum ",_tag_dyneither(_tmpA01,sizeof(char),6)))),((
_tmp9FE[0]=Cyc_Absynpp_scope2doc(_tmp4D7),Cyc_PP_cat(_tag_dyneither(_tmp9FE,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL27D;_LL290: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4DA=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4B7;if(_tmp4DA->tag != 9)goto
_LL292;else{_tmp4DB=_tmp4DA->f1;}}_LL291: {void*t;if(_tmp4DB->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4DB->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4DB->kind,0);}{const char*_tmpA0B;struct Cyc_Core_Opt*_tmpA0A;struct Cyc_PP_Doc*
_tmpA09[2];const char*_tmpA08;struct Cyc_PP_Doc*_tmpA07[4];s=((_tmpA07[3]=Cyc_PP_text(((
_tmpA08=";",_tag_dyneither(_tmpA08,sizeof(char),2)))),((_tmpA07[2]=Cyc_Absynpp_atts2doc(
_tmp4DB->atts),((_tmpA07[1]=Cyc_Absynpp_tqtd2doc(_tmp4DB->tq,t,((_tmpA0A=
_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->v=((_tmpA09[1]=Cyc_Absynpp_tvars2doc(
_tmp4DB->tvs),((_tmpA09[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4DB->name),Cyc_PP_cat(
_tag_dyneither(_tmpA09,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA0A))))),((_tmpA07[
0]=Cyc_PP_text(((_tmpA0B="typedef ",_tag_dyneither(_tmpA0B,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmpA07,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL27D;}_LL292: {struct Cyc_Absyn_Namespace_d_struct*_tmp4DC=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4B7;if(_tmp4DC->tag != 10)goto _LL294;else{_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;}}
_LL293: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DD);{
const char*_tmpA10;const char*_tmpA0F;struct Cyc_PP_Doc*_tmpA0E[8];s=((_tmpA0E[7]=
Cyc_Absynpp_rb(),((_tmpA0E[6]=Cyc_PP_line_doc(),((_tmpA0E[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0F="",_tag_dyneither(_tmpA0F,
sizeof(char),1))),_tmp4DE),((_tmpA0E[4]=Cyc_PP_line_doc(),((_tmpA0E[3]=Cyc_Absynpp_lb(),((
_tmpA0E[2]=Cyc_PP_blank_doc(),((_tmpA0E[1]=Cyc_PP_textptr(_tmp4DD),((_tmpA0E[0]=
Cyc_PP_text(((_tmpA10="namespace ",_tag_dyneither(_tmpA10,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA0E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL27D;
_LL294: {struct Cyc_Absyn_Using_d_struct*_tmp4DF=(struct Cyc_Absyn_Using_d_struct*)
_tmp4B7;if(_tmp4DF->tag != 11)goto _LL296;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}
_LL295: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA15;const char*_tmpA14;
struct Cyc_PP_Doc*_tmpA13[8];s=((_tmpA13[7]=Cyc_Absynpp_rb(),((_tmpA13[6]=Cyc_PP_line_doc(),((
_tmpA13[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA14="",_tag_dyneither(_tmpA14,sizeof(char),1))),_tmp4E1),((_tmpA13[4]=Cyc_PP_line_doc(),((
_tmpA13[3]=Cyc_Absynpp_lb(),((_tmpA13[2]=Cyc_PP_blank_doc(),((_tmpA13[1]=Cyc_Absynpp_qvar2doc(
_tmp4E0),((_tmpA13[0]=Cyc_PP_text(((_tmpA15="using ",_tag_dyneither(_tmpA15,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA13,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;const char*_tmpA1D;
const char*_tmpA1C;struct Cyc_PP_Doc*_tmpA1B[11];s=((_tmpA1B[10]=Cyc_PP_text(((
_tmpA1C=" */",_tag_dyneither(_tmpA1C,sizeof(char),4)))),((_tmpA1B[9]=Cyc_Absynpp_rb(),((
_tmpA1B[8]=Cyc_PP_text(((_tmpA1D="/* ",_tag_dyneither(_tmpA1D,sizeof(char),4)))),((
_tmpA1B[7]=Cyc_PP_line_doc(),((_tmpA1B[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA1E="",_tag_dyneither(_tmpA1E,sizeof(char),1))),
_tmp4E1),((_tmpA1B[5]=Cyc_PP_line_doc(),((_tmpA1B[4]=Cyc_PP_text(((_tmpA1F=" */",
_tag_dyneither(_tmpA1F,sizeof(char),4)))),((_tmpA1B[3]=Cyc_Absynpp_lb(),((
_tmpA1B[2]=Cyc_PP_blank_doc(),((_tmpA1B[1]=Cyc_Absynpp_qvar2doc(_tmp4E0),((
_tmpA1B[0]=Cyc_PP_text(((_tmpA20="/* using ",_tag_dyneither(_tmpA20,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA1B,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL27D;_LL296: {struct Cyc_Absyn_ExternC_d_struct*_tmp4E2=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4B7;if(_tmp4E2->tag != 12)goto _LL298;else{_tmp4E3=_tmp4E2->f1;}}_LL297: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA25;const char*_tmpA24;struct Cyc_PP_Doc*_tmpA23[6];s=((_tmpA23[5]=
Cyc_Absynpp_rb(),((_tmpA23[4]=Cyc_PP_line_doc(),((_tmpA23[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA24="",_tag_dyneither(_tmpA24,
sizeof(char),1))),_tmp4E3),((_tmpA23[2]=Cyc_PP_line_doc(),((_tmpA23[1]=Cyc_Absynpp_lb(),((
_tmpA23[0]=Cyc_PP_text(((_tmpA25="extern \"C\" ",_tag_dyneither(_tmpA25,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA23,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA30;const char*_tmpA2F;const char*_tmpA2E;const char*_tmpA2D;
const char*_tmpA2C;struct Cyc_PP_Doc*_tmpA2B[9];s=((_tmpA2B[8]=Cyc_PP_text(((
_tmpA2C=" */",_tag_dyneither(_tmpA2C,sizeof(char),4)))),((_tmpA2B[7]=Cyc_Absynpp_rb(),((
_tmpA2B[6]=Cyc_PP_text(((_tmpA2D="/* ",_tag_dyneither(_tmpA2D,sizeof(char),4)))),((
_tmpA2B[5]=Cyc_PP_line_doc(),((_tmpA2B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA2E="",_tag_dyneither(_tmpA2E,sizeof(char),1))),
_tmp4E3),((_tmpA2B[3]=Cyc_PP_line_doc(),((_tmpA2B[2]=Cyc_PP_text(((_tmpA2F=" */",
_tag_dyneither(_tmpA2F,sizeof(char),4)))),((_tmpA2B[1]=Cyc_Absynpp_lb(),((
_tmpA2B[0]=Cyc_PP_text(((_tmpA30="/* extern \"C\" ",_tag_dyneither(_tmpA30,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA2B,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL27D;_LL298: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4E4=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4B7;if(_tmp4E4->tag != 13)
goto _LL29A;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E4->f2;}}_LL299: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4E6 != 0){const char*_tmpA35;const char*_tmpA34;
struct Cyc_PP_Doc*_tmpA33[7];exs_doc=((_tmpA33[6]=Cyc_Absynpp_rb(),((_tmpA33[5]=
Cyc_PP_line_doc(),((_tmpA33[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA34=",",_tag_dyneither(_tmpA34,sizeof(char),2))),
_tmp4E6),((_tmpA33[3]=Cyc_PP_line_doc(),((_tmpA33[2]=Cyc_Absynpp_lb(),((_tmpA33[
1]=Cyc_PP_text(((_tmpA35=" export ",_tag_dyneither(_tmpA35,sizeof(char),9)))),((
_tmpA33[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA33,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA3A;const char*
_tmpA39;struct Cyc_PP_Doc*_tmpA38[6];s=((_tmpA38[5]=exs_doc,((_tmpA38[4]=Cyc_PP_line_doc(),((
_tmpA38[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA39="",_tag_dyneither(_tmpA39,sizeof(char),1))),_tmp4E5),((_tmpA38[2]=Cyc_PP_line_doc(),((
_tmpA38[1]=Cyc_Absynpp_lb(),((_tmpA38[0]=Cyc_PP_text(((_tmpA3A="extern \"C include\" ",
_tag_dyneither(_tmpA3A,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA38,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA45;const char*
_tmpA44;const char*_tmpA43;const char*_tmpA42;const char*_tmpA41;struct Cyc_PP_Doc*
_tmpA40[9];s=((_tmpA40[8]=Cyc_PP_text(((_tmpA41=" */",_tag_dyneither(_tmpA41,
sizeof(char),4)))),((_tmpA40[7]=Cyc_Absynpp_rb(),((_tmpA40[6]=Cyc_PP_text(((
_tmpA42="/* ",_tag_dyneither(_tmpA42,sizeof(char),4)))),((_tmpA40[5]=Cyc_PP_line_doc(),((
_tmpA40[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA43="",_tag_dyneither(_tmpA43,sizeof(char),1))),_tmp4E5),((_tmpA40[3]=Cyc_PP_line_doc(),((
_tmpA40[2]=Cyc_PP_text(((_tmpA44=" */",_tag_dyneither(_tmpA44,sizeof(char),4)))),((
_tmpA40[1]=Cyc_Absynpp_lb(),((_tmpA40[0]=Cyc_PP_text(((_tmpA45="/* extern \"C include\" ",
_tag_dyneither(_tmpA45,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA40,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL27D;_LL29A: {struct Cyc_Absyn_Porton_d_struct*
_tmp4E7=(struct Cyc_Absyn_Porton_d_struct*)_tmp4B7;if(_tmp4E7->tag != 14)goto
_LL29C;}_LL29B:{const char*_tmpA48;struct Cyc_PP_Doc*_tmpA47[2];s=((_tmpA47[1]=Cyc_Absynpp_lb(),((
_tmpA47[0]=Cyc_PP_text(((_tmpA48="__cyclone_port_on__;",_tag_dyneither(_tmpA48,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA47,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL27D;_LL29C: {struct Cyc_Absyn_Portoff_d_struct*_tmp4E8=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4B7;if(_tmp4E8->tag != 15)goto _LL27D;}_LL29D:{const char*_tmpA4B;struct Cyc_PP_Doc*
_tmpA4A[2];s=((_tmpA4A[1]=Cyc_Absynpp_lb(),((_tmpA4A[0]=Cyc_PP_text(((_tmpA4B="__cyclone_port_off__;",
_tag_dyneither(_tmpA4B,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA4A,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL27D;_LL27D:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2A7: {const char*_tmpA4C;return Cyc_PP_text(((
_tmpA4C="static ",_tag_dyneither(_tmpA4C,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL2A8: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2A9: {const char*_tmpA4D;
return Cyc_PP_text(((_tmpA4D="extern ",_tag_dyneither(_tmpA4D,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL2AA: {const char*_tmpA4E;return Cyc_PP_text(((_tmpA4E="extern \"C\" ",
_tag_dyneither(_tmpA4E,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2AB: {
const char*_tmpA4F;return Cyc_PP_text(((_tmpA4F="abstract ",_tag_dyneither(_tmpA4F,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2AC: {const char*_tmpA50;return Cyc_PP_text(((
_tmpA50="register ",_tag_dyneither(_tmpA50,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp550=t;struct
Cyc_Absyn_Tvar*_tmp552;struct Cyc_List_List*_tmp554;_LL2AF: {struct Cyc_Absyn_VarType_struct*
_tmp551=(struct Cyc_Absyn_VarType_struct*)_tmp550;if(_tmp551->tag != 2)goto _LL2B1;
else{_tmp552=_tmp551->f1;}}_LL2B0: return Cyc_Tcutil_is_temp_tvar(_tmp552);_LL2B1: {
struct Cyc_Absyn_JoinEff_struct*_tmp553=(struct Cyc_Absyn_JoinEff_struct*)_tmp550;
if(_tmp553->tag != 24)goto _LL2B3;else{_tmp554=_tmp553->f1;}}_LL2B2: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp554);_LL2B3:;_LL2B4: return 0;_LL2AE:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp555=t;void**_tmp559;void*
_tmp55A;_LL2B6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp556=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp555;if(_tmp556->tag != 13)goto _LL2B8;}_LL2B7: return 1;_LL2B8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp557=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp555;if(_tmp557->tag != 15)goto
_LL2BA;}_LL2B9: return 1;_LL2BA: {struct Cyc_Absyn_TypedefType_struct*_tmp558=(
struct Cyc_Absyn_TypedefType_struct*)_tmp555;if(_tmp558->tag != 18)goto _LL2BC;
else{_tmp559=_tmp558->f4;if(_tmp559 == 0)goto _LL2BC;_tmp55A=*_tmp559;}}_LL2BB:
return Cyc_Absynpp_is_anon_aggrtype(_tmp55A);_LL2BC:;_LL2BD: return 0;_LL2B5:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple16 _tmpA51;struct _tuple16 _tmp55C=(_tmpA51.f1=(void*)tms->hd,((
_tmpA51.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA51)));
void*_tmp55D;void*_tmp55F;_LL2BF: _tmp55D=_tmp55C.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp55E=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp55D;if(_tmp55E->tag != 2)goto
_LL2C1;};_tmp55F=_tmp55C.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp560=(struct
Cyc_Absyn_Function_mod_struct*)_tmp55F;if(_tmp560->tag != 3)goto _LL2C1;};_LL2C0: {
struct Cyc_List_List*_tmpA54;struct Cyc_List_List*_tmpA53;return(_tmpA53=
_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53->hd=(void*)tms->hd,((_tmpA53->tl=((
_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA54->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA54)))))),_tmpA53)))));}
_LL2C1:;_LL2C2: {struct Cyc_List_List*_tmpA55;return(_tmpA55=_region_malloc(r,
sizeof(*_tmpA55)),((_tmpA55->hd=atts,((_tmpA55->tl=tms,_tmpA55)))));}_LL2BE:;}
else{struct Cyc_List_List*_tmpA56;return(_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((
_tmpA56->hd=atts,((_tmpA56->tl=tms,_tmpA56)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp565=t;struct Cyc_Absyn_ArrayInfo
_tmp567;void*_tmp568;struct Cyc_Absyn_Tqual _tmp569;struct Cyc_Absyn_Exp*_tmp56A;
union Cyc_Absyn_Constraint*_tmp56B;struct Cyc_Position_Segment*_tmp56C;struct Cyc_Absyn_PtrInfo
_tmp56E;void*_tmp56F;struct Cyc_Absyn_Tqual _tmp570;struct Cyc_Absyn_PtrAtts _tmp571;
struct Cyc_Absyn_FnInfo _tmp573;struct Cyc_List_List*_tmp574;struct Cyc_Core_Opt*
_tmp575;void*_tmp576;struct Cyc_List_List*_tmp577;int _tmp578;struct Cyc_Absyn_VarargInfo*
_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_Core_Opt*
_tmp57D;struct Cyc_Core_Opt*_tmp57E;int _tmp57F;struct _tuple0*_tmp581;struct Cyc_List_List*
_tmp582;void**_tmp583;_LL2C4: {struct Cyc_Absyn_ArrayType_struct*_tmp566=(struct
Cyc_Absyn_ArrayType_struct*)_tmp565;if(_tmp566->tag != 9)goto _LL2C6;else{_tmp567=
_tmp566->f1;_tmp568=_tmp567.elt_type;_tmp569=_tmp567.tq;_tmp56A=_tmp567.num_elts;
_tmp56B=_tmp567.zero_term;_tmp56C=_tmp567.zt_loc;}}_LL2C5: {struct Cyc_Absyn_Tqual
_tmp585;void*_tmp586;struct Cyc_List_List*_tmp587;struct _tuple12 _tmp584=Cyc_Absynpp_to_tms(
r,_tmp569,_tmp568);_tmp585=_tmp584.f1;_tmp586=_tmp584.f2;_tmp587=_tmp584.f3;{
void*tm;if(_tmp56A == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA59;struct Cyc_Absyn_Carray_mod_struct*
_tmpA58;tm=(void*)((_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58[0]=((
_tmpA59.tag=0,((_tmpA59.f1=_tmp56B,((_tmpA59.f2=_tmp56C,_tmpA59)))))),_tmpA58))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA5C;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA5B;tm=(void*)((_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((_tmpA5B[0]=((
_tmpA5C.tag=1,((_tmpA5C.f1=(struct Cyc_Absyn_Exp*)_tmp56A,((_tmpA5C.f2=_tmp56B,((
_tmpA5C.f3=_tmp56C,_tmpA5C)))))))),_tmpA5B))));}{struct Cyc_List_List*_tmpA5F;
struct _tuple12 _tmpA5E;return(_tmpA5E.f1=_tmp585,((_tmpA5E.f2=_tmp586,((_tmpA5E.f3=((
_tmpA5F=_region_malloc(r,sizeof(*_tmpA5F)),((_tmpA5F->hd=tm,((_tmpA5F->tl=
_tmp587,_tmpA5F)))))),_tmpA5E)))));};};}_LL2C6: {struct Cyc_Absyn_PointerType_struct*
_tmp56D=(struct Cyc_Absyn_PointerType_struct*)_tmp565;if(_tmp56D->tag != 5)goto
_LL2C8;else{_tmp56E=_tmp56D->f1;_tmp56F=_tmp56E.elt_typ;_tmp570=_tmp56E.elt_tq;
_tmp571=_tmp56E.ptr_atts;}}_LL2C7: {struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;
struct Cyc_List_List*_tmp591;struct _tuple12 _tmp58E=Cyc_Absynpp_to_tms(r,_tmp570,
_tmp56F);_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;_tmp591=_tmp58E.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA65;struct Cyc_Absyn_Pointer_mod_struct _tmpA64;struct Cyc_List_List*_tmpA63;
_tmp591=((_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63->hd=(void*)((
_tmpA65=_region_malloc(r,sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA64.tag=2,((
_tmpA64.f1=_tmp571,((_tmpA64.f2=tq,_tmpA64)))))),_tmpA65)))),((_tmpA63->tl=
_tmp591,_tmpA63))))));}{struct _tuple12 _tmpA66;return(_tmpA66.f1=_tmp58F,((
_tmpA66.f2=_tmp590,((_tmpA66.f3=_tmp591,_tmpA66)))));};}_LL2C8: {struct Cyc_Absyn_FnType_struct*
_tmp572=(struct Cyc_Absyn_FnType_struct*)_tmp565;if(_tmp572->tag != 10)goto _LL2CA;
else{_tmp573=_tmp572->f1;_tmp574=_tmp573.tvars;_tmp575=_tmp573.effect;_tmp576=
_tmp573.ret_typ;_tmp577=_tmp573.args;_tmp578=_tmp573.c_varargs;_tmp579=_tmp573.cyc_varargs;
_tmp57A=_tmp573.rgn_po;_tmp57B=_tmp573.attributes;}}_LL2C9: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp575 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp575->v)){
_tmp575=0;_tmp574=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp574);}{struct Cyc_Absyn_Tqual _tmp597;void*_tmp598;struct Cyc_List_List*_tmp599;
struct _tuple12 _tmp596=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp576);
_tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;{struct Cyc_List_List*
tms=_tmp599;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2D0: if(
_tmp57B != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA69;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA68;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA68=_region_malloc(r,
sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA69.tag=5,((_tmpA69.f1=0,((_tmpA69.f2=
_tmp57B,_tmpA69)))))),_tmpA68)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA78;struct Cyc_Absyn_WithTypes_struct*_tmpA77;struct Cyc_Absyn_WithTypes_struct
_tmpA76;struct Cyc_Absyn_Function_mod_struct _tmpA75;struct Cyc_List_List*_tmpA74;
tms=((_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->hd=(void*)((_tmpA78=
_region_malloc(r,sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA75.tag=3,((_tmpA75.f1=(
void*)((void*)((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77[0]=((
_tmpA76.tag=1,((_tmpA76.f1=_tmp577,((_tmpA76.f2=_tmp578,((_tmpA76.f3=_tmp579,((
_tmpA76.f4=_tmp575,((_tmpA76.f5=_tmp57A,_tmpA76)))))))))))),_tmpA77))))),_tmpA75)))),
_tmpA78)))),((_tmpA74->tl=tms,_tmpA74))))));}break;case Cyc_Cyclone_Vc_c: _LL2D1:{
struct Cyc_Absyn_Function_mod_struct*_tmpA87;struct Cyc_Absyn_WithTypes_struct*
_tmpA86;struct Cyc_Absyn_WithTypes_struct _tmpA85;struct Cyc_Absyn_Function_mod_struct
_tmpA84;struct Cyc_List_List*_tmpA83;tms=((_tmpA83=_region_malloc(r,sizeof(*
_tmpA83)),((_tmpA83->hd=(void*)((_tmpA87=_region_malloc(r,sizeof(*_tmpA87)),((
_tmpA87[0]=((_tmpA84.tag=3,((_tmpA84.f1=(void*)((void*)((_tmpA86=_region_malloc(
r,sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA85.tag=1,((_tmpA85.f1=_tmp577,((_tmpA85.f2=
_tmp578,((_tmpA85.f3=_tmp579,((_tmpA85.f4=_tmp575,((_tmpA85.f5=_tmp57A,_tmpA85)))))))))))),
_tmpA86))))),_tmpA84)))),_tmpA87)))),((_tmpA83->tl=tms,_tmpA83))))));}for(0;
_tmp57B != 0;_tmp57B=_tmp57B->tl){void*_tmp5A6=(void*)_tmp57B->hd;_LL2D4: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5A7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5A6;if(_tmp5A7->tag != 1)goto _LL2D6;}_LL2D5: goto _LL2D7;_LL2D6: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5A8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5A6;if(_tmp5A8->tag != 2)goto
_LL2D8;}_LL2D7: goto _LL2D9;_LL2D8: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5A9=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5A6;if(_tmp5A9->tag != 3)goto _LL2DA;}
_LL2D9:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA91;struct Cyc_List_List*
_tmpA90;struct Cyc_Absyn_Attributes_mod_struct _tmpA8F;struct Cyc_List_List*_tmpA8E;
tms=((_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E->hd=(void*)((_tmpA91=
_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA8F.tag=5,((_tmpA8F.f1=0,((
_tmpA8F.f2=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->hd=(void*)_tmp57B->hd,((
_tmpA90->tl=0,_tmpA90)))))),_tmpA8F)))))),_tmpA91)))),((_tmpA8E->tl=tms,_tmpA8E))))));}
goto AfterAtts;_LL2DA:;_LL2DB: goto _LL2D3;_LL2D3:;}break;}AfterAtts: if(_tmp574 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA97;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA96;struct Cyc_List_List*_tmpA95;tms=((_tmpA95=_region_malloc(r,sizeof(*
_tmpA95)),((_tmpA95->hd=(void*)((_tmpA97=_region_malloc(r,sizeof(*_tmpA97)),((
_tmpA97[0]=((_tmpA96.tag=4,((_tmpA96.f1=_tmp574,((_tmpA96.f2=0,((_tmpA96.f3=1,
_tmpA96)))))))),_tmpA97)))),((_tmpA95->tl=tms,_tmpA95))))));}{struct _tuple12
_tmpA98;return(_tmpA98.f1=_tmp597,((_tmpA98.f2=_tmp598,((_tmpA98.f3=tms,_tmpA98)))));};};};
_LL2CA: {struct Cyc_Absyn_Evar_struct*_tmp57C=(struct Cyc_Absyn_Evar_struct*)
_tmp565;if(_tmp57C->tag != 1)goto _LL2CC;else{_tmp57D=_tmp57C->f1;_tmp57E=_tmp57C->f2;
_tmp57F=_tmp57C->f3;}}_LL2CB: if(_tmp57E == 0){struct _tuple12 _tmpA99;return(
_tmpA99.f1=tq,((_tmpA99.f2=t,((_tmpA99.f3=0,_tmpA99)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp57E->v);}_LL2CC: {struct Cyc_Absyn_TypedefType_struct*_tmp580=(
struct Cyc_Absyn_TypedefType_struct*)_tmp565;if(_tmp580->tag != 18)goto _LL2CE;
else{_tmp581=_tmp580->f1;_tmp582=_tmp580->f2;_tmp583=_tmp580->f4;}}_LL2CD: if((
_tmp583 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp583)){struct _tuple12 _tmpA9A;return(_tmpA9A.f1=tq,((_tmpA9A.f2=t,((_tmpA9A.f3=
0,_tmpA9A)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp583);}_LL2CE:;_LL2CF: {
struct _tuple12 _tmpA9B;return(_tmpA9B.f1=tq,((_tmpA9B.f2=t,((_tmpA9B.f3=0,_tmpA9B)))));}
_LL2C3:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5B5=t;struct Cyc_Core_Opt*_tmp5B7;struct Cyc_Core_Opt _tmp5B8;void*
_tmp5B9;struct Cyc_Absyn_PtrInfo _tmp5BB;void*_tmp5BC;_LL2DD: {struct Cyc_Absyn_Evar_struct*
_tmp5B6=(struct Cyc_Absyn_Evar_struct*)_tmp5B5;if(_tmp5B6->tag != 1)goto _LL2DF;
else{_tmp5B7=_tmp5B6->f2;if(_tmp5B7 == 0)goto _LL2DF;_tmp5B8=*_tmp5B7;_tmp5B9=(
void*)_tmp5B8.v;}}_LL2DE: return Cyc_Absynpp_is_char_ptr(_tmp5B9);_LL2DF: {struct
Cyc_Absyn_PointerType_struct*_tmp5BA=(struct Cyc_Absyn_PointerType_struct*)
_tmp5B5;if(_tmp5BA->tag != 5)goto _LL2E1;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BB.elt_typ;}}
_LL2E0: L: {void*_tmp5BD=_tmp5BC;struct Cyc_Core_Opt*_tmp5BF;struct Cyc_Core_Opt
_tmp5C0;void*_tmp5C1;void**_tmp5C3;void*_tmp5C4;enum Cyc_Absyn_Size_of _tmp5C6;
_LL2E4: {struct Cyc_Absyn_Evar_struct*_tmp5BE=(struct Cyc_Absyn_Evar_struct*)
_tmp5BD;if(_tmp5BE->tag != 1)goto _LL2E6;else{_tmp5BF=_tmp5BE->f2;if(_tmp5BF == 0)
goto _LL2E6;_tmp5C0=*_tmp5BF;_tmp5C1=(void*)_tmp5C0.v;}}_LL2E5: _tmp5BC=_tmp5C1;
goto L;_LL2E6: {struct Cyc_Absyn_TypedefType_struct*_tmp5C2=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5BD;if(_tmp5C2->tag != 18)goto _LL2E8;else{_tmp5C3=_tmp5C2->f4;if(_tmp5C3 == 0)
goto _LL2E8;_tmp5C4=*_tmp5C3;}}_LL2E7: _tmp5BC=_tmp5C4;goto L;_LL2E8: {struct Cyc_Absyn_IntType_struct*
_tmp5C5=(struct Cyc_Absyn_IntType_struct*)_tmp5BD;if(_tmp5C5->tag != 6)goto _LL2EA;
else{_tmp5C6=_tmp5C5->f2;if(_tmp5C6 != Cyc_Absyn_Char_sz)goto _LL2EA;}}_LL2E9:
return 1;_LL2EA:;_LL2EB: return 0;_LL2E3:;}_LL2E1:;_LL2E2: return 0;_LL2DC:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5C7=_new_region("temp");struct
_RegionHandle*temp=& _tmp5C7;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5C9;
void*_tmp5CA;struct Cyc_List_List*_tmp5CB;struct _tuple12 _tmp5C8=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5C9=_tmp5C8.f1;_tmp5CA=_tmp5C8.f2;_tmp5CB=_tmp5C8.f3;_tmp5CB=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5CB);if(
_tmp5CB == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA9C[2];struct Cyc_PP_Doc*_tmp5CD=(
_tmpA9C[1]=Cyc_Absynpp_ntyp2doc(_tmp5CA),((_tmpA9C[0]=Cyc_Absynpp_tqual2doc(
_tmp5C9),Cyc_PP_cat(_tag_dyneither(_tmpA9C,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5CD;}else{const char*_tmpA9F;struct Cyc_PP_Doc*_tmpA9E[
4];struct Cyc_PP_Doc*_tmp5D0=(_tmpA9E[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5CB),((_tmpA9E[2]=
Cyc_PP_text(((_tmpA9F=" ",_tag_dyneither(_tmpA9F,sizeof(char),2)))),((_tmpA9E[1]=
Cyc_Absynpp_ntyp2doc(_tmp5CA),((_tmpA9E[0]=Cyc_Absynpp_tqual2doc(_tmp5C9),Cyc_PP_cat(
_tag_dyneither(_tmpA9E,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5D0;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2EC: if(f->width != 0){struct Cyc_Core_Opt*_tmpAA6;const char*_tmpAA5;const char*
_tmpAA4;struct Cyc_PP_Doc*_tmpAA3[5];return(_tmpAA3[4]=Cyc_PP_text(((_tmpAA4=";",
_tag_dyneither(_tmpAA4,sizeof(char),2)))),((_tmpAA3[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpAA3[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpAA3[1]=Cyc_PP_text(((_tmpAA5=":",_tag_dyneither(_tmpAA5,sizeof(char),2)))),((
_tmpAA3[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA6=_cycalloc(sizeof(*
_tmpAA6)),((_tmpAA6->v=Cyc_PP_textptr(f->name),_tmpAA6))))),Cyc_PP_cat(
_tag_dyneither(_tmpAA3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAAB;const char*_tmpAAA;struct Cyc_PP_Doc*_tmpAA9[3];return(_tmpAA9[2]=Cyc_PP_text(((
_tmpAAA=";",_tag_dyneither(_tmpAAA,sizeof(char),2)))),((_tmpAA9[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAA9[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAAB=
_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->v=Cyc_PP_textptr(f->name),_tmpAAB))))),
Cyc_PP_cat(_tag_dyneither(_tmpAA9,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2ED: if(f->width != 0){struct Cyc_Core_Opt*_tmpAB2;const char*_tmpAB1;const char*
_tmpAB0;struct Cyc_PP_Doc*_tmpAAF[5];return(_tmpAAF[4]=Cyc_PP_text(((_tmpAB0=";",
_tag_dyneither(_tmpAB0,sizeof(char),2)))),((_tmpAAF[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAAF[2]=Cyc_PP_text(((_tmpAB1=":",
_tag_dyneither(_tmpAB1,sizeof(char),2)))),((_tmpAAF[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->v=Cyc_PP_textptr(f->name),
_tmpAB2))))),((_tmpAAF[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAAF,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAB7;const char*_tmpAB6;struct Cyc_PP_Doc*_tmpAB5[3];return(_tmpAB5[2]=Cyc_PP_text(((
_tmpAB6=";",_tag_dyneither(_tmpAB6,sizeof(char),2)))),((_tmpAB5[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->v=Cyc_PP_textptr(
f->name),_tmpAB7))))),((_tmpAB5[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAB5,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpAB8;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAB8="",
_tag_dyneither(_tmpAB8,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpAB9[3];return(_tmpAB9[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAB9[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpAB9[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpAB9,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpABA;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpABA=",",
_tag_dyneither(_tmpABA,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpABD;
void*_tmpABC;(_tmpABC=0,Cyc_fprintf(f,((_tmpABD="\n",_tag_dyneither(_tmpABD,
sizeof(char),2))),_tag_dyneither(_tmpABC,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpABE;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpABE="",_tag_dyneither(
_tmpABE,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
