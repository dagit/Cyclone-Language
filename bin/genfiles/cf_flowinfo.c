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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};extern
char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int
Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(
void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
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
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
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
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_nontagged_union_type(void*);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);int
Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple10{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple10 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,
void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct
Cyc_CfFlowInfo_IsZero_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*
f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct
Cyc_Absyn_Exp*f1;int f2;void*f3;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*
unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict
mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(struct
Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,
struct _tuple12 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x);union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){union
Cyc_CfFlowInfo_AbsLVal _tmp44D;return((_tmp44D.PlaceL).val=x,(((_tmp44D.PlaceL).tag=
1,_tmp44D)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal _tmp44E;return((_tmp44E.UnknownL).val=
0,(((_tmp44E.UnknownL).tag=2,_tmp44E)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp44F;return((_tmp44F.BottomFL).val=0,(((_tmp44F.BottomFL).tag=1,_tmp44F)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){struct _tuple11 _tmp452;union Cyc_CfFlowInfo_FlowInfo
_tmp451;return((_tmp451.ReachableFL).val=((_tmp452.f1=fd,((_tmp452.f2=r,_tmp452)))),(((
_tmp451.ReachableFL).tag=2,_tmp451)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp489;struct Cyc_CfFlowInfo_MallocPt_struct _tmp488;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp487;struct Cyc_CfFlowInfo_Esc_struct*
_tmp486;struct Cyc_CfFlowInfo_Esc_struct _tmp485;struct Cyc_CfFlowInfo_Esc_struct*
_tmp484;struct Cyc_CfFlowInfo_Esc_struct _tmp483;struct Cyc_CfFlowInfo_Esc_struct*
_tmp482;struct Cyc_CfFlowInfo_Esc_struct _tmp481;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp480;struct Cyc_CfFlowInfo_UnknownR_struct _tmp47F;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp47E;struct Cyc_CfFlowInfo_UnknownR_struct _tmp47D;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp47C;struct Cyc_CfFlowInfo_UnknownR_struct _tmp47B;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp47A;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp479;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp478;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp477;struct Cyc_CfFlowInfo_Zero_struct*
_tmp476;struct Cyc_CfFlowInfo_Zero_struct _tmp475;struct Cyc_CfFlowInfo_FlowEnv*
_tmp474;return(_tmp474=_region_malloc(r,sizeof(*_tmp474)),((_tmp474->r=r,((
_tmp474->zero=(void*)((_tmp476=_region_malloc(r,sizeof(*_tmp476)),((_tmp476[0]=((
_tmp475.tag=0,_tmp475)),_tmp476)))),((_tmp474->notzeroall=(void*)((_tmp478=
_region_malloc(r,sizeof(*_tmp478)),((_tmp478[0]=((_tmp477.tag=1,_tmp477)),
_tmp478)))),((_tmp474->notzerothis=(void*)((_tmp47A=_region_malloc(r,sizeof(*
_tmp47A)),((_tmp47A[0]=((_tmp479.tag=2,_tmp479)),_tmp47A)))),((_tmp474->unknown_none=(
void*)((_tmp47C=_region_malloc(r,sizeof(*_tmp47C)),((_tmp47C[0]=((_tmp47B.tag=3,((
_tmp47B.f1=Cyc_CfFlowInfo_NoneIL,_tmp47B)))),_tmp47C)))),((_tmp474->unknown_this=(
void*)((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E[0]=((_tmp47D.tag=3,((
_tmp47D.f1=Cyc_CfFlowInfo_ThisIL,_tmp47D)))),_tmp47E)))),((_tmp474->unknown_all=(
void*)((_tmp480=_region_malloc(r,sizeof(*_tmp480)),((_tmp480[0]=((_tmp47F.tag=3,((
_tmp47F.f1=Cyc_CfFlowInfo_AllIL,_tmp47F)))),_tmp480)))),((_tmp474->esc_none=(
void*)((_tmp482=_region_malloc(r,sizeof(*_tmp482)),((_tmp482[0]=((_tmp481.tag=4,((
_tmp481.f1=Cyc_CfFlowInfo_NoneIL,_tmp481)))),_tmp482)))),((_tmp474->esc_this=(
void*)((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484[0]=((_tmp483.tag=4,((
_tmp483.f1=Cyc_CfFlowInfo_ThisIL,_tmp483)))),_tmp484)))),((_tmp474->esc_all=(
void*)((_tmp486=_region_malloc(r,sizeof(*_tmp486)),((_tmp486[0]=((_tmp485.tag=4,((
_tmp485.f1=Cyc_CfFlowInfo_AllIL,_tmp485)))),_tmp486)))),((_tmp474->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp474->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp474->dummy_place=((_tmp489=
_region_malloc(r,sizeof(*_tmp489)),((_tmp489->root=(void*)((_tmp487=
_region_malloc(r,sizeof(*_tmp487)),((_tmp487[0]=((_tmp488.tag=1,((_tmp488.f1=&
dummy_exp,((_tmp488.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp488)))))),
_tmp487)))),((_tmp489->fields=0,_tmp489)))))),_tmp474)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc);int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,
void*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp1F;struct Cyc_List_List*_tmp20;struct
Cyc_CfFlowInfo_Place _tmp1E=*place;_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*
_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp21;if(_tmp22->tag != 0)goto _LL3;
else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)return Cyc_Absynpp_qvar2string(_tmp23->name);
else{const char*_tmp48D;void*_tmp48C[1];struct Cyc_String_pa_struct _tmp48B;return(
struct _dyneither_ptr)((_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23->name)),((_tmp48C[0]=& _tmp48B,Cyc_aprintf(((
_tmp48D="reachable from %s",_tag_dyneither(_tmp48D,sizeof(char),18))),
_tag_dyneither(_tmp48C,sizeof(void*),1))))))));}_LL3:;_LL4: {const char*_tmp490;
void*_tmp48F;(_tmp48F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp490="error locations not for VarRoots",_tag_dyneither(
_tmp490,sizeof(char),33))),_tag_dyneither(_tmp48F,sizeof(void*),0)));}_LL0:;};}
static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*env,void*
place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2);static
struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){return loc1;}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp29=0;struct Cyc_Dict_Dict
_tmp2A=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(int*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),int*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
int*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct
Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,s1,s2);
return _tmp2A;}struct _tuple13{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))
_tmp2D=1;if(!_tmp2D){{struct _RegionHandle _tmp2E=_new_region("temp");struct
_RegionHandle*temp=& _tmp2E;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple13 _tmp2F=*((struct _tuple13*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple13*))Cyc_Iter_next)(iter,& _tmp2F)){void*_tmp30=_tmp2F.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp30)){int _tmp31=0;
_npop_handler(1);return _tmp31;}}{int _tmp32=1;_npop_handler(1);return _tmp32;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;void*
_tmp34=_tmp2C;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp34);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,
struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct
_dyneither_ptr*_tmp35=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmp35,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp49D;const char*
_tmp49C;void*_tmp49B[1];struct Cyc_String_pa_struct _tmp49A;struct Cyc_Core_Impossible_struct*
_tmp499;(int)_throw((void*)((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499[0]=((
_tmp49D.tag=Cyc_Core_Impossible,((_tmp49D.f1=(struct _dyneither_ptr)((_tmp49A.tag=
0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp49B[0]=&
_tmp49A,Cyc_aprintf(((_tmp49C="get_field_index_fs failed: %s",_tag_dyneither(
_tmp49C,sizeof(char),30))),_tag_dyneither(_tmp49B,sizeof(void*),1)))))))),
_tmp49D)))),_tmp499)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3D;union Cyc_Absyn_AggrInfoU
_tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct Cyc_Absyn_AggrType_struct*_tmp3C=(
struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(_tmp3C->tag != 12)goto _LLD;else{_tmp3D=
_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {struct Cyc_List_List*_tmp41=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp3E))->impl))->fields;
_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp3F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;else{
_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);_LLF:;
_LL10: {struct Cyc_Core_Impossible_struct _tmp4AA;const char*_tmp4A9;void*_tmp4A8[1];
struct Cyc_String_pa_struct _tmp4A7;struct Cyc_Core_Impossible_struct*_tmp4A6;(int)
_throw((void*)((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4AA.tag=
Cyc_Core_Impossible,((_tmp4AA.f1=(struct _dyneither_ptr)((_tmp4A7.tag=0,((_tmp4A7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp4A8[
0]=& _tmp4A7,Cyc_aprintf(((_tmp4A9="get_field_index failed: %s",_tag_dyneither(
_tmp4A9,sizeof(char),27))),_tag_dyneither(_tmp4A8,sizeof(void*),1)))))))),
_tmp4AA)))),_tmp4A6)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp4AB;struct _tuple0 _tmp48=(_tmp4AB.f1=r1,((_tmp4AB.f2=r2,_tmp4AB)));
void*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4B;void*_tmp4C;struct Cyc_Absyn_Vardecl*
_tmp4E;void*_tmp4F;void*_tmp51;void*_tmp53;struct Cyc_Absyn_Exp*_tmp55;void*
_tmp56;struct Cyc_Absyn_Exp*_tmp58;void*_tmp59;void*_tmp5B;void*_tmp5D;int _tmp5F;
void*_tmp60;int _tmp62;_LL12: _tmp49=_tmp48.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4A=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp49;if(_tmp4A->tag != 0)goto _LL14;
else{_tmp4B=_tmp4A->f1;}};_tmp4C=_tmp48.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4C;if(_tmp4D->tag != 0)goto _LL14;
else{_tmp4E=_tmp4D->f1;}};_LL13: return(int)_tmp4B - (int)_tmp4E;_LL14: _tmp4F=
_tmp48.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp50=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4F;if(_tmp50->tag != 0)goto _LL16;};_LL15: return - 1;_LL16: _tmp51=_tmp48.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp52=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp51;if(_tmp52->tag != 0)goto _LL18;};_LL17: return 1;_LL18: _tmp53=_tmp48.f1;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp54=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp53;if(_tmp54->tag != 1)goto _LL1A;else{_tmp55=_tmp54->f1;}};_tmp56=_tmp48.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp57=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1A;else{_tmp58=_tmp57->f1;}};_LL19: return(int)
_tmp55 - (int)_tmp58;_LL1A: _tmp59=_tmp48.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp5A=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp59;if(_tmp5A->tag != 1)goto
_LL1C;};_LL1B: return - 1;_LL1C: _tmp5B=_tmp48.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp5C=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp5B;if(_tmp5C->tag != 1)goto
_LL1E;};_LL1D: return 1;_LL1E: _tmp5D=_tmp48.f1;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5E=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp5D;if(_tmp5E->tag != 2)goto
_LL11;else{_tmp5F=_tmp5E->f1;}};_tmp60=_tmp48.f2;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp61=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp60;if(_tmp61->tag != 2)goto
_LL11;else{_tmp62=_tmp61->f1;}};_LL1F: return _tmp5F - _tmp62;_LL11:;};}static int
Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2);
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*
list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{void*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_CfFlowInfo_Place
_tmp63=*p1;_tmp64=_tmp63.root;_tmp65=_tmp63.fields;{void*_tmp67;struct Cyc_List_List*
_tmp68;struct Cyc_CfFlowInfo_Place _tmp66=*p2;_tmp67=_tmp66.root;_tmp68=_tmp66.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp64,_tmp67);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp65,_tmp68);};};};}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p);static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp6A;struct Cyc_List_List*
_tmp6B;struct Cyc_CfFlowInfo_Place _tmp69=*p;_tmp6A=_tmp69.root;_tmp6B=_tmp69.fields;{
void*_tmp6C=_tmp6A;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Exp*_tmp70;int
_tmp72;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp6D=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp6C;if(_tmp6D->tag != 0)goto _LL23;else{_tmp6E=_tmp6D->f1;}}_LL22:{struct
_dyneither_ptr*_tmp4B8;const char*_tmp4B7;void*_tmp4B6[1];struct Cyc_String_pa_struct
_tmp4B5;struct Cyc_List_List*_tmp4B4;sl=((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((
_tmp4B4->hd=((_tmp4B8=_cycalloc(sizeof(*_tmp4B8)),((_tmp4B8[0]=((_tmp4B5.tag=0,((
_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6E->name).f2),((
_tmp4B6[0]=& _tmp4B5,Cyc_aprintf(((_tmp4B7="%s",_tag_dyneither(_tmp4B7,sizeof(
char),3))),_tag_dyneither(_tmp4B6,sizeof(void*),1)))))))),_tmp4B8)))),((_tmp4B4->tl=
sl,_tmp4B4))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{
_tmp70=_tmp6F->f1;}}_LL24:{struct _dyneither_ptr*_tmp4C5;const char*_tmp4C4;void*
_tmp4C3[1];struct Cyc_Int_pa_struct _tmp4C2;struct Cyc_List_List*_tmp4C1;sl=((
_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1->hd=((_tmp4C5=_cycalloc(sizeof(*
_tmp4C5)),((_tmp4C5[0]=((_tmp4C2.tag=1,((_tmp4C2.f1=(unsigned long)((int)_tmp70),((
_tmp4C3[0]=& _tmp4C2,Cyc_aprintf(((_tmp4C4="mpt%d",_tag_dyneither(_tmp4C4,sizeof(
char),6))),_tag_dyneither(_tmp4C3,sizeof(void*),1)))))))),_tmp4C5)))),((_tmp4C1->tl=
sl,_tmp4C1))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{
_tmp72=_tmp71->f1;}}_LL26:{struct _dyneither_ptr*_tmp4D2;const char*_tmp4D1;void*
_tmp4D0[1];struct Cyc_Int_pa_struct _tmp4CF;struct Cyc_List_List*_tmp4CE;sl=((
_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE->hd=((_tmp4D2=_cycalloc(sizeof(*
_tmp4D2)),((_tmp4D2[0]=((_tmp4CF.tag=1,((_tmp4CF.f1=(unsigned long)_tmp72,((
_tmp4D0[0]=& _tmp4CF,Cyc_aprintf(((_tmp4D1="param%d",_tag_dyneither(_tmp4D1,
sizeof(char),8))),_tag_dyneither(_tmp4D0,sizeof(void*),1)))))))),_tmp4D2)))),((
_tmp4CE->tl=sl,_tmp4CE))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;_tmp6B=_tmp6B->tl){
struct _dyneither_ptr*_tmp4DF;const char*_tmp4DE;void*_tmp4DD[1];struct Cyc_Int_pa_struct
_tmp4DC;struct Cyc_List_List*_tmp4DB;sl=((_tmp4DB=_cycalloc(sizeof(*_tmp4DB)),((
_tmp4DB->hd=((_tmp4DF=_cycalloc(sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4DC.tag=1,((
_tmp4DC.f1=(unsigned long)((int)_tmp6B->hd),((_tmp4DD[0]=& _tmp4DC,Cyc_aprintf(((
_tmp4DE="%d",_tag_dyneither(_tmp4DE,sizeof(char),3))),_tag_dyneither(_tmp4DD,
sizeof(void*),1)))))))),_tmp4DF)))),((_tmp4DB->tl=sl,_tmp4DB))))));}{struct Cyc_String_pa_struct
_tmp4EA;const char*_tmp4E9;void*_tmp4E8[1];const char*_tmp4E7;struct _dyneither_ptr*
_tmp4E6;struct _dyneither_ptr*_tmp87=(_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((
_tmp4E6[0]=((_tmp4EA.tag=0,((_tmp4EA.f1=(struct _dyneither_ptr)((_tmp4E9="",
_tag_dyneither(_tmp4E9,sizeof(char),1))),((_tmp4E8[0]=& _tmp4EA,Cyc_aprintf(((
_tmp4E7="%s",_tag_dyneither(_tmp4E7,sizeof(char),3))),_tag_dyneither(_tmp4E8,
sizeof(void*),1)))))))),_tmp4E6)));for(0;sl != 0;sl=sl->tl){const char*_tmp4EF;
void*_tmp4EE[2];struct Cyc_String_pa_struct _tmp4ED;struct Cyc_String_pa_struct
_tmp4EC;*_tmp87=((_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp87),((_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp4EE[0]=& _tmp4ED,((
_tmp4EE[1]=& _tmp4EC,Cyc_aprintf(((_tmp4EF="%s.%s",_tag_dyneither(_tmp4EF,sizeof(
char),6))),_tag_dyneither(_tmp4EE,sizeof(void*),2))))))))))))));}return _tmp87;};}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp4F5(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4F4,unsigned int*
_tmp4F3,void***_tmp4F1){for(*_tmp4F4=0;*_tmp4F4 < *_tmp4F3;(*_tmp4F4)++){(*
_tmp4F1)[*_tmp4F4]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp4F4;unsigned int _tmp4F3;struct _dyneither_ptr _tmp4F2;void**
_tmp4F1;unsigned int _tmp4F0;struct _dyneither_ptr d=(_tmp4F0=sz,((_tmp4F1=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F0)),((_tmp4F2=
_tag_dyneither(_tmp4F1,sizeof(void*),_tmp4F0),((((_tmp4F3=_tmp4F0,_tmp4F5(& fenv,&
_tmp4F4,& _tmp4F3,& _tmp4F1))),_tmp4F2)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp92;struct _dyneither_ptr*_tmp93;void*_tmp94;struct
Cyc_Absyn_Aggrfield*_tmp91=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp92=*_tmp91;_tmp93=_tmp92.name;_tmp94=_tmp92.type;if(
_get_dyneither_size(*_tmp93,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmp94,leafval);}}return d;}struct _tuple14{struct _RegionHandle*f1;struct
Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
struct _tuple14 _tmp9A;struct _RegionHandle*_tmp9B;struct Cyc_List_List*_tmp9C;
struct _tuple14*_tmp99=env;_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;{void*
_tmp9D=Cyc_Tcutil_rsubstitute(_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*
_tmp4F6;return(_tmp4F6=_region_malloc(_tmp9B,sizeof(*_tmp4F6)),((_tmp4F6->name=f->name,((
_tmp4F6->tq=f->tq,((_tmp4F6->type=_tmp9D,((_tmp4F6->width=f->width,((_tmp4F6->attributes=
f->attributes,_tmp4F6)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,void*leafval);static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,void*leafval){struct _RegionHandle _tmp9F=_new_region("temp");
struct _RegionHandle*temp=& _tmp9F;_push_region(temp);{struct Cyc_List_List*inst=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14
_tmp4F7;struct _tuple14 _tmpA0=(_tmp4F7.f1=temp,((_tmp4F7.f2=inst,_tmp4F7)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,&
_tmpA0,fs);struct _dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,
subs_fs,leafval);_npop_handler(0);return _tmpA1;};_pop_region(temp);}struct
_tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);static
void _tmp4FD(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4FC,unsigned int*
_tmp4FB,void***_tmp4F9){for(*_tmp4FC=0;*_tmp4FC < *_tmp4FB;(*_tmp4FC)++){(*
_tmp4F9)[*_tmp4FC]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(
t))return fenv->unknown_all;{void*_tmpA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpA5;union Cyc_Absyn_DatatypeFieldInfoU _tmpA6;struct _tuple2 _tmpA7;struct Cyc_Absyn_Datatypefield*
_tmpA8;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_AggrInfo _tmpAC;union Cyc_Absyn_AggrInfoU
_tmpAD;struct Cyc_List_List*_tmpAE;enum Cyc_Absyn_AggrKind _tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_ArrayInfo _tmpB3;void*_tmpB4;union Cyc_Absyn_Constraint*
_tmpB5;void*_tmpB7;struct Cyc_Absyn_PtrInfo _tmpB9;struct Cyc_Absyn_PtrAtts _tmpBA;
union Cyc_Absyn_Constraint*_tmpBB;_LL28: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA4=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA3;if(_tmpA4->tag != 4)goto
_LL2A;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.field_info;if((_tmpA6.KnownDatatypefield).tag
!= 2)goto _LL2A;_tmpA7=(struct _tuple2)(_tmpA6.KnownDatatypefield).val;_tmpA8=
_tmpA7.f2;}}_LL29: if(_tmpA8->typs == 0)return leafval;_tmpAA=_tmpA8->typs;goto
_LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpA9=(struct Cyc_Absyn_TupleType_struct*)
_tmpA3;if(_tmpA9->tag != 11)goto _LL2C;else{_tmpAA=_tmpA9->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpAA);unsigned int _tmp4FC;unsigned int _tmp4FB;struct _dyneither_ptr _tmp4FA;void**
_tmp4F9;unsigned int _tmp4F8;struct _dyneither_ptr d=(_tmp4F8=sz,((_tmp4F9=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F8)),((_tmp4FA=
_tag_dyneither(_tmp4F9,sizeof(void*),_tmp4F8),((((_tmp4FB=_tmp4F8,_tmp4FD(& fenv,&
_tmp4FC,& _tmp4FB,& _tmp4F9))),_tmp4FA)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpAA))->hd)).f2,
leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp500;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4FF;return(void*)((_tmp4FF=
_region_malloc(fenv->r,sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp500.tag=6,((_tmp500.f1=
0,((_tmp500.f2=d,_tmp500)))))),_tmp4FF))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC2=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC2->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC3=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC2->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC2->kind;_tmpB1=
_tmpC3;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp503;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp502;return(void*)((_tmp502=_region_malloc(fenv->r,sizeof(*_tmp502)),((
_tmp502[0]=((_tmp503.tag=6,((_tmp503.f1=_tmpC2->kind == Cyc_Absyn_UnionA,((
_tmp503.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC2->tvs,
_tmpAE,_tmpC3,leafval),_tmp503)))))),_tmp502))));};};}_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpA3;if(_tmpAF->tag != 13)goto
_LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct
_tmp506;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp505;return(void*)((_tmp505=
_region_malloc(fenv->r,sizeof(*_tmp505)),((_tmp505[0]=((_tmp506.tag=6,((_tmp506.f1=
_tmpB0 == Cyc_Absyn_UnionA,((_tmp506.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmpB1,leafval),_tmp506)))))),_tmp505))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpB2=(struct Cyc_Absyn_ArrayType_struct*)_tmpA3;if(_tmpB2->tag != 9)goto _LL32;
else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB3.elt_type;_tmpB5=_tmpB3.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB5))goto
_LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpB4)?fenv->unknown_all:
leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpB6=(struct Cyc_Absyn_TagType_struct*)
_tmpA3;if(_tmpB6->tag != 20)goto _LL34;else{_tmpB7=(void*)_tmpB6->f1;}}_LL33:
return leafval;_LL34:{struct Cyc_Absyn_PointerType_struct*_tmpB8=(struct Cyc_Absyn_PointerType_struct*)
_tmpA3;if(_tmpB8->tag != 5)goto _LL36;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.ptr_atts;
_tmpBB=_tmpBA.nullable;}}if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpBB)))goto _LL36;_LL35:{void*_tmpC8=leafval;enum Cyc_CfFlowInfo_InitLevel
_tmpCA;enum Cyc_CfFlowInfo_InitLevel _tmpCC;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpC9=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpC8;if(_tmpC9->tag != 3)goto
_LL3B;else{_tmpCA=_tmpC9->f1;if(_tmpCA != Cyc_CfFlowInfo_ThisIL)goto _LL3B;}}_LL3A:
return fenv->notzerothis;_LL3B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpCB=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpC8;if(_tmpCB->tag != 3)goto _LL3D;else{
_tmpCC=_tmpCB->f1;if(_tmpCC != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C: return fenv->notzeroall;
_LL3D:;_LL3E: goto _LL38;_LL38:;}goto _LL27;_LL36:;_LL37: goto _LL27;_LL27:;}return
Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp510(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp50F,unsigned int*_tmp50E,
void***_tmp50C){for(*_tmp50F=0;*_tmp50F < *_tmp50E;(*_tmp50F)++){(*_tmp50C)[*
_tmp50F]=(*fenv)->unknown_all;}}static void _tmp519(struct _dyneither_ptr*_tmpE6,
unsigned int*_tmp518,unsigned int*_tmp517,void***_tmp515){for(*_tmp518=0;*
_tmp518 < *_tmp517;(*_tmp518)++){(*_tmp515)[*_tmp518]=((void**)(*_tmpE6).curr)[(
int)*_tmp518];}}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(
t)){struct Cyc_CfFlowInfo_Consumed_struct _tmp509;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp508;return(void*)((_tmp508=_region_malloc(fenv->r,sizeof(*_tmp508)),((
_tmp508[0]=((_tmp509.tag=7,((_tmp509.f1=consumer,((_tmp509.f2=iteration,((
_tmp509.f3=(void*)r,_tmp509)))))))),_tmp508))));}{struct _tuple0 _tmp50A;struct
_tuple0 _tmpD0=(_tmp50A.f1=Cyc_Tcutil_compress(t),((_tmp50A.f2=r,_tmp50A)));void*
_tmpD1;struct Cyc_List_List*_tmpD3;void*_tmpD4;struct _dyneither_ptr _tmpD6;void*
_tmpD7;struct Cyc_Absyn_AggrInfo _tmpD9;union Cyc_Absyn_AggrInfoU _tmpDA;void*_tmpDB;
int _tmpDD;struct _dyneither_ptr _tmpDE;void*_tmpDF;enum Cyc_Absyn_AggrKind _tmpE1;
struct Cyc_List_List*_tmpE2;void*_tmpE3;int _tmpE5;struct _dyneither_ptr _tmpE6;
_LL40: _tmpD1=_tmpD0.f1;{struct Cyc_Absyn_TupleType_struct*_tmpD2=(struct Cyc_Absyn_TupleType_struct*)
_tmpD1;if(_tmpD2->tag != 11)goto _LL42;else{_tmpD3=_tmpD2->f1;}};_tmpD4=_tmpD0.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD4;if(_tmpD5->tag != 6)goto _LL42;else{_tmpD6=_tmpD5->f2;}};_LL41: {
unsigned int _tmpE7=_get_dyneither_size(_tmpD6,sizeof(void*));unsigned int _tmp50F;
unsigned int _tmp50E;struct _dyneither_ptr _tmp50D;void**_tmp50C;unsigned int
_tmp50B;struct _dyneither_ptr d2=(_tmp50B=_tmpE7,((_tmp50C=(void**)_region_malloc(
fenv->r,_check_times(sizeof(void*),_tmp50B)),((_tmp50D=_tag_dyneither(_tmp50C,
sizeof(void*),_tmp50B),((((_tmp50E=_tmp50B,_tmp510(& fenv,& _tmp50F,& _tmp50E,&
_tmp50C))),_tmp50D)))))));{int i=0;for(0;i < _tmpE7;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpD3))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i)));
_tmpD3=_tmpD3->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp513;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp512;return(void*)((_tmp512=_region_malloc(fenv->r,sizeof(*_tmp512)),((
_tmp512[0]=((_tmp513.tag=6,((_tmp513.f1=0,((_tmp513.f2=d2,_tmp513)))))),_tmp512))));};}
_LL42: _tmpD7=_tmpD0.f1;{struct Cyc_Absyn_AggrType_struct*_tmpD8=(struct Cyc_Absyn_AggrType_struct*)
_tmpD7;if(_tmpD8->tag != 12)goto _LL44;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.aggr_info;}};
_tmpDB=_tmpD0.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmpDC=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpDB;if(_tmpDC->tag != 6)goto _LL44;else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDC->f2;}};
_LL43: {struct Cyc_Absyn_Aggrdecl*_tmpEE=Cyc_Absyn_get_known_aggrdecl(_tmpDA);if(
_tmpEE->impl == 0)return r;_tmpE1=_tmpEE->kind;_tmpE2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpEE->impl))->fields;_tmpE5=_tmpDD;_tmpE6=_tmpDE;goto _LL45;}_LL44:
_tmpDF=_tmpD0.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmpE0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDF;if(_tmpE0->tag != 13)goto _LL46;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;}};
_tmpE3=_tmpD0.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmpE4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpE3;if(_tmpE4->tag != 6)goto _LL46;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;}};
_LL45: {unsigned int _tmp518;unsigned int _tmp517;struct _dyneither_ptr _tmp516;void**
_tmp515;unsigned int _tmp514;struct _dyneither_ptr d2=(_tmp514=_get_dyneither_size(
_tmpE6,sizeof(void*)),((_tmp515=(void**)_region_malloc(fenv->r,_check_times(
sizeof(void*),_tmp514)),((_tmp516=_tag_dyneither(_tmp515,sizeof(void*),_tmp514),((((
_tmp517=_tmp514,_tmp519(& _tmpE6,& _tmp518,& _tmp517,& _tmp515))),_tmp516)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpE2);{int i=0;for(0;i < sz;(i ++,_tmpE2=_tmpE2->tl)){struct Cyc_Absyn_Aggrfield
_tmpF0;struct _dyneither_ptr*_tmpF1;void*_tmpF2;struct Cyc_Absyn_Aggrfield*_tmpEF=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpE2))->hd;
_tmpF0=*_tmpEF;_tmpF1=_tmpF0.name;_tmpF2=_tmpF0.type;if(_get_dyneither_size(*
_tmpF1,sizeof(char))!= 1)*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=
Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpF2,consumer,iteration,*((void**)
_check_dyneither_subscript(_tmpE6,sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp51C;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp51B;return(void*)((_tmp51B=
_region_malloc(fenv->r,sizeof(*_tmp51B)),((_tmp51B[0]=((_tmp51C.tag=6,((_tmp51C.f1=
_tmpE5,((_tmp51C.f2=_tmpE6,_tmp51C)))))),_tmp51B))));};}_LL46:;_LL47: return r;
_LL3F:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmpF9=_new_region("r");struct
_RegionHandle*r=& _tmpF9;_push_region(r);{struct _tuple16 _tmp51D;struct _tuple16
_tmpFA=(_tmp51D.f1=place,((_tmp51D.f2=0,_tmp51D)));struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next)(iter,&
_tmpFA)){struct Cyc_CfFlowInfo_Place*_tmpFB=_tmpFA.f1;void*_tmpFD;struct Cyc_List_List*
_tmpFE;struct Cyc_CfFlowInfo_Place _tmpFC=*place;_tmpFD=_tmpFC.root;_tmpFE=_tmpFC.fields;{
void*_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_CfFlowInfo_Place _tmpFF=*
_tmpFB;_tmp100=_tmpFF.root;_tmp101=_tmpFF.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmpFD,_tmp100)!= 0)continue;for(0;_tmpFE != 0  && _tmp101 != 0;(_tmpFE=_tmpFE->tl,
_tmp101=_tmp101->tl)){if((int)_tmpFE->hd != (int)_tmp101->hd)break;}if(_tmpFE == 0){
int _tmp102=1;_npop_handler(0);return _tmp102;}};}{int _tmp103=0;_npop_handler(0);
return _tmp103;};};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place);static void
Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*
place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,
pile->places,place)){struct Cyc_List_List*_tmp51E;pile->places=((_tmp51E=
_region_malloc(pile->rgn,sizeof(*_tmp51E)),((_tmp51E->hd=place,((_tmp51E->tl=
pile->places,_tmp51E))))));}}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r);static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r){void*_tmp106=r;void*_tmp108;struct Cyc_CfFlowInfo_Place*_tmp10A;int
_tmp10C;struct _dyneither_ptr _tmp10D;_LL49: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp107=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp106;if(_tmp107->tag != 7)goto
_LL4B;else{_tmp108=(void*)_tmp107->f3;}}_LL4A: Cyc_CfFlowInfo_add_places(pile,
_tmp108);return;_LL4B: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp109=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp106;if(_tmp109->tag != 5)goto _LL4D;else{_tmp10A=_tmp109->f1;}}_LL4C: Cyc_CfFlowInfo_add_place(
pile,_tmp10A);return;_LL4D: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp10B=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp106;if(_tmp10B->tag != 6)goto _LL4F;
else{_tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;}}_LL4E:{int i=0;for(0;i < 
_get_dyneither_size(_tmp10D,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(pile,((
void**)_tmp10D.curr)[i]);}}return;_LL4F:;_LL50: return;_LL48:;}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp524(struct
_RegionHandle**r,void**new_val,unsigned int*_tmp523,unsigned int*_tmp522,void***
_tmp520,struct _dyneither_ptr*_tmp111){for(*_tmp523=0;*_tmp523 < *_tmp522;(*
_tmp523)++){(*_tmp520)[*_tmp523]=Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((
void**)(*_tmp111).curr)[(int)*_tmp523]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp10E=old_val;int _tmp110;
struct _dyneither_ptr _tmp111;_LL52: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp10F=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10E;if(_tmp10F->tag != 6)goto
_LL54;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}}_LL53: {unsigned int _tmp523;
unsigned int _tmp522;struct _dyneither_ptr _tmp521;void**_tmp520;unsigned int
_tmp51F;struct _dyneither_ptr d2=(_tmp51F=_get_dyneither_size(_tmp111,sizeof(void*)),((
_tmp520=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp51F)),((_tmp521=
_tag_dyneither(_tmp520,sizeof(void*),_tmp51F),((((_tmp522=_tmp51F,_tmp524(& r,&
new_val,& _tmp523,& _tmp522,& _tmp520,& _tmp111))),_tmp521)))))));struct Cyc_CfFlowInfo_Aggregate_struct
_tmp527;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp526;return(void*)((_tmp526=
_region_malloc(r,sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=6,((_tmp527.f1=
_tmp110,((_tmp527.f2=d2,_tmp527)))))),_tmp526))));}_LL54:;_LL55: return new_val;
_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp52D(struct
_dyneither_ptr*d,unsigned int*_tmp52C,unsigned int*_tmp52B,void***_tmp529){for(*
_tmp52C=0;*_tmp52C < *_tmp52B;(*_tmp52C)++){(*_tmp529)[*_tmp52C]=((void**)(*d).curr)[(
int)*_tmp52C];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp118=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp118 == rval)return d;{
unsigned int _tmp52C;unsigned int _tmp52B;struct _dyneither_ptr _tmp52A;void**
_tmp529;unsigned int _tmp528;struct _dyneither_ptr res=(_tmp528=_get_dyneither_size(
d,sizeof(void*)),((_tmp529=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp528)),((_tmp52A=_tag_dyneither(_tmp529,sizeof(void*),_tmp528),((((_tmp52B=
_tmp528,_tmp52D(& d,& _tmp52C,& _tmp52B,& _tmp529))),_tmp52A)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple17 _tmp52E;struct _tuple17 _tmp11E=(_tmp52E.f1=fs,((
_tmp52E.f2=old_val,_tmp52E)));struct Cyc_List_List*_tmp11F;struct Cyc_List_List
_tmp120;int _tmp121;struct Cyc_List_List*_tmp122;void*_tmp123;int _tmp125;struct
_dyneither_ptr _tmp126;_LL57: _tmp11F=_tmp11E.f1;if(_tmp11F == 0)goto _LL59;_tmp120=*
_tmp11F;_tmp121=(int)_tmp120.hd;_tmp122=_tmp120.tl;_tmp123=_tmp11E.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp124=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp123;if(_tmp124->tag != 6)goto
_LL59;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}};_LL58: {void*_tmp127=Cyc_CfFlowInfo_insert_place_outer(
r,_tmp122,*((void**)_check_dyneither_subscript(_tmp126,sizeof(void*),_tmp121)),
new_val);struct _dyneither_ptr _tmp128=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp126,
_tmp121,_tmp127);if((void**)_tmp128.curr == (void**)_tmp126.curr)return old_val;
else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp531;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp530;return(void*)((_tmp530=_region_malloc(r,sizeof(*_tmp530)),((_tmp530[0]=((
_tmp531.tag=6,((_tmp531.f1=_tmp125,((_tmp531.f2=_tmp128,_tmp531)))))),_tmp530))));}}
_LL59:;_LL5A: {struct Cyc_Core_Impossible_struct _tmp537;const char*_tmp536;struct
Cyc_Core_Impossible_struct*_tmp535;(int)_throw((void*)((_tmp535=_cycalloc(
sizeof(*_tmp535)),((_tmp535[0]=((_tmp537.tag=Cyc_Core_Impossible,((_tmp537.f1=((
_tmp536="bad insert place",_tag_dyneither(_tmp536,sizeof(char),17))),_tmp537)))),
_tmp535)))));}_LL56:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp12E=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp12E,0);{void*oldval;void*
newval;{struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(
setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp12E);;_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp133=
_tmp130;_LL5C: {struct Cyc_Dict_Absent_struct*_tmp134=(struct Cyc_Dict_Absent_struct*)
_tmp133;if(_tmp134->tag != Cyc_Dict_Absent)goto _LL5E;}_LL5D: continue;_LL5E:;_LL5F:(
void)_throw(_tmp133);_LL5B:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL60: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL61: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL62: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp136;struct Cyc_List_List*
_tmp137;struct Cyc_CfFlowInfo_Place _tmp135=*_tmp12E;_tmp136=_tmp135.root;_tmp137=
_tmp135.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp136,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp137,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp136),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r);static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*
r){void*_tmp138=r;enum Cyc_CfFlowInfo_InitLevel _tmp13A;enum Cyc_CfFlowInfo_InitLevel
_tmp13C;void*_tmp141;_LL65: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp139=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp138;if(_tmp139->tag != 3)goto _LL67;
else{_tmp13A=_tmp139->f1;}}_LL66: return _tmp13A;_LL67: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp13B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp138;if(_tmp13B->tag != 4)goto _LL69;
else{_tmp13C=_tmp13B->f1;}}_LL68: return _tmp13C;_LL69: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp13D=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp138;if(_tmp13D->tag != 0)goto _LL6B;}
_LL6A: goto _LL6C;_LL6B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp13E=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp138;if(_tmp13E->tag != 1)goto _LL6D;}_LL6C:
return Cyc_CfFlowInfo_AllIL;_LL6D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp13F=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp138;if(_tmp13F->tag != 2)
goto _LL6F;}_LL6E: return Cyc_CfFlowInfo_ThisIL;_LL6F: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp140=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp138;if(_tmp140->tag != 7)goto
_LL71;else{_tmp141=(void*)_tmp140->f3;}}_LL70: return Cyc_CfFlowInfo_NoneIL;_LL71:;
_LL72: {struct Cyc_Core_Impossible_struct _tmp53D;const char*_tmp53C;struct Cyc_Core_Impossible_struct*
_tmp53B;(int)_throw((void*)((_tmp53B=_cycalloc(sizeof(*_tmp53B)),((_tmp53B[0]=((
_tmp53D.tag=Cyc_Core_Impossible,((_tmp53D.f1=((_tmp53C="initlevel_approx",
_tag_dyneither(_tmp53C,sizeof(char),17))),_tmp53D)))),_tmp53B)))));}_LL64:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc);static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum 
Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)
return Cyc_CfFlowInfo_NoneIL;{void*_tmp145=r;int _tmp147;struct _dyneither_ptr
_tmp148;int _tmp14A;struct _dyneither_ptr _tmp14B;struct Cyc_CfFlowInfo_Place*
_tmp14D;_LL74: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp146=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp145;if(_tmp146->tag != 6)goto _LL76;else{_tmp147=_tmp146->f1;if(_tmp147 != 1)
goto _LL76;_tmp148=_tmp146->f2;}}_LL75: {unsigned int _tmp14E=_get_dyneither_size(
_tmp148,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;{int i=0;for(0;i < _tmp14E;
++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(
_tmp148,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){this_ans=
Cyc_CfFlowInfo_AllIL;break;}}}goto _LL73;}_LL76: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp149=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp145;if(_tmp149->tag != 6)goto
_LL78;else{_tmp14A=_tmp149->f1;if(_tmp14A != 0)goto _LL78;_tmp14B=_tmp149->f2;}}
_LL77: {unsigned int _tmp14F=_get_dyneither_size(_tmp14B,sizeof(void*));this_ans=
Cyc_CfFlowInfo_AllIL;{int i=0;for(0;i < _tmp14F;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp14B,sizeof(void*),i)),this_ans);}}
goto _LL73;}_LL78: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp14C=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp145;if(_tmp14C->tag != 5)goto _LL7A;else{_tmp14D=_tmp14C->f1;}}_LL79: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp14D))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp53E;env->seen=((
_tmp53E=_region_malloc(env->e,sizeof(*_tmp53E)),((_tmp53E->hd=_tmp14D,((_tmp53E->tl=
env->seen,_tmp53E))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp14D),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL73;_LL7A:;_LL7B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL73:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct
_RegionHandle*_tmp151=env->r;struct Cyc_CfFlowInfo_InitlevelEnv _tmp53F;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp152=(_tmp53F.e=_tmp151,((_tmp53F.d=d,((_tmp53F.seen=0,_tmp53F)))));return Cyc_CfFlowInfo_initlevel_rec(&
_tmp152,r,Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp155;struct Cyc_List_List*
_tmp156;struct Cyc_CfFlowInfo_Place _tmp154=*place;_tmp155=_tmp154.root;_tmp156=
_tmp154.fields;{void*_tmp157=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp155);for(0;_tmp156 != 0;_tmp156=_tmp156->tl){struct _tuple18 _tmp540;struct
_tuple18 _tmp159=(_tmp540.f1=_tmp157,((_tmp540.f2=(int)_tmp156->hd,_tmp540)));
void*_tmp15A;int _tmp15C;struct _dyneither_ptr _tmp15D;int _tmp15E;_LL7D: _tmp15A=
_tmp159.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp15B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp15A;if(_tmp15B->tag != 6)goto _LL7F;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}};
_tmp15E=_tmp159.f2;_LL7E: _tmp157=*((void**)_check_dyneither_subscript(_tmp15D,
sizeof(void*),_tmp15E));goto _LL7C;_LL7F:;_LL80: {struct Cyc_Core_Impossible_struct
_tmp546;const char*_tmp545;struct Cyc_Core_Impossible_struct*_tmp544;(int)_throw((
void*)((_tmp544=_cycalloc(sizeof(*_tmp544)),((_tmp544[0]=((_tmp546.tag=Cyc_Core_Impossible,((
_tmp546.f1=((_tmp545="bad lookup_place",_tag_dyneither(_tmp545,sizeof(char),17))),
_tmp546)))),_tmp544)))));}_LL7C:;}return _tmp157;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp162=
rval;void*_tmp165;int _tmp167;struct _dyneither_ptr _tmp168;_LL82: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp163=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp162;if(_tmp163->tag != 4)goto _LL84;}
_LL83: return 0;_LL84: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp164=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp162;if(_tmp164->tag != 7)goto _LL86;else{_tmp165=(void*)_tmp164->f3;}}_LL85:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL86: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp166=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp162;if(_tmp166->tag != 6)goto
_LL88;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LL87: {unsigned int sz=
_get_dyneither_size(_tmp168,sizeof(void*));{int i=0;for(0;i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((
void**)_tmp168.curr)[i]))return 0;}}return 1;}_LL88:;_LL89: return 1;_LL81:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp169=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp547;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp547=_region_malloc(_tmp169,sizeof(*_tmp547)),((_tmp547->rgn=_tmp169,((
_tmp547->places=0,_tmp547)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp551(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp550,unsigned int*_tmp54F,void***_tmp54D,struct
_dyneither_ptr*_tmp17B,struct _dyneither_ptr*_tmp177){for(*_tmp550=0;*_tmp550 < *
_tmp54F;(*_tmp550)++){(*_tmp54D)[*_tmp550]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp177).curr)[(int)*_tmp550],*((void**)
_check_dyneither_subscript(*_tmp17B,sizeof(void*),(int)*_tmp550)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp548;struct _tuple0 _tmp16C=(_tmp548.f1=oldval,((
_tmp548.f2=newval,_tmp548)));void*_tmp16D;void*_tmp16F;struct Cyc_CfFlowInfo_Place*
_tmp171;void*_tmp172;void*_tmp174;int _tmp176;struct _dyneither_ptr _tmp177;void*
_tmp178;int _tmp17A;struct _dyneither_ptr _tmp17B;void*_tmp17C;enum Cyc_CfFlowInfo_InitLevel
_tmp17E;_LL8B: _tmp16D=_tmp16C.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp16E=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp16D;if(_tmp16E->tag != 4)goto _LL8D;};_tmp16F=
_tmp16C.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp170=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp16F;if(_tmp170->tag != 5)goto _LL8D;else{_tmp171=_tmp170->f1;}};_LL8C: Cyc_CfFlowInfo_add_place(
env->pile,_tmp171);goto _LL8E;_LL8D: _tmp172=_tmp16C.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp173=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp172;if(_tmp173->tag != 4)goto _LL8F;};
_LL8E: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp54B;void*_tmp54A;(_tmp54A=0,Cyc_Tcutil_terr(env->loc,((_tmp54B="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp54B,sizeof(char),67))),_tag_dyneither(_tmp54A,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL8F: _tmp174=_tmp16C.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp175=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp174;if(_tmp175->tag != 6)goto
_LL91;else{_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;}};_tmp178=_tmp16C.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp179=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp178;if(_tmp179->tag != 6)goto _LL91;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;}};
_LL90: {unsigned int _tmp550;unsigned int _tmp54F;struct _dyneither_ptr _tmp54E;void**
_tmp54D;unsigned int _tmp54C;struct _dyneither_ptr new_d=(_tmp54C=
_get_dyneither_size(_tmp177,sizeof(void*)),((_tmp54D=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp54C)),((_tmp54E=_tag_dyneither(_tmp54D,sizeof(
void*),_tmp54C),((((_tmp54F=_tmp54C,_tmp551(& env,& _tmp550,& _tmp54F,& _tmp54D,&
_tmp17B,& _tmp177))),_tmp54E)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp177,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp177.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp177,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp17B,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp554;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp553;return(void*)((_tmp553=_region_malloc((env->fenv)->r,sizeof(*_tmp553)),((
_tmp553[0]=((_tmp554.tag=6,((_tmp554.f1=_tmp176,((_tmp554.f2=new_d,_tmp554)))))),
_tmp553))));};}_LL91: _tmp17C=_tmp16C.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp17D=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp17C;if(_tmp17D->tag != 4)goto _LL93;else{
_tmp17E=_tmp17D->f1;}};_LL92: switch(_tmp17E){case Cyc_CfFlowInfo_NoneIL: _LL95:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL96: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL97: return(env->fenv)->unknown_all;}_LL93:;_LL94:
return newval;_LL8A:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval);static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmp555;struct _tuple17 _tmp188=(_tmp555.f1=fs,((_tmp555.f2=oldval,
_tmp555)));struct Cyc_List_List*_tmp189;struct Cyc_List_List _tmp18A;int _tmp18B;
struct Cyc_List_List*_tmp18C;void*_tmp18D;int _tmp18F;struct _dyneither_ptr _tmp190;
_LL9A: _tmp189=_tmp188.f1;if(_tmp189 == 0)goto _LL9C;_tmp18A=*_tmp189;_tmp18B=(int)
_tmp18A.hd;_tmp18C=_tmp18A.tl;_tmp18D=_tmp188.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp18E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp18D;if(_tmp18E->tag != 6)goto
_LL9C;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}};_LL9B: {void*_tmp191=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp18C,*((void**)_check_dyneither_subscript(_tmp190,sizeof(void*),_tmp18B)),
newval);struct _dyneither_ptr _tmp192=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp190,_tmp18B,_tmp191);if((void**)_tmp192.curr == (void**)_tmp190.curr)return
oldval;else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp558;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp557;return(void*)((_tmp557=_region_malloc((env->fenv)->r,sizeof(*_tmp557)),((
_tmp557[0]=((_tmp558.tag=6,((_tmp558.f1=_tmp18F,((_tmp558.f2=_tmp192,_tmp558)))))),
_tmp557))));}}_LL9C:;_LL9D: {struct Cyc_Core_Impossible_struct _tmp55E;const char*
_tmp55D;struct Cyc_Core_Impossible_struct*_tmp55C;(int)_throw((void*)((_tmp55C=
_cycalloc(sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55E.tag=Cyc_Core_Impossible,((
_tmp55E.f1=((_tmp55D="bad assign place",_tag_dyneither(_tmp55D,sizeof(char),17))),
_tmp55E)))),_tmp55C)))));}_LL99:;};}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp198=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp19A;void*_tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_CfFlowInfo_Place*
_tmp199=place;_tmp19A=*_tmp199;_tmp19B=_tmp19A.root;_tmp19C=_tmp19A.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp561;struct Cyc_CfFlowInfo_AssignEnv _tmp560;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp560.fenv=fenv,((_tmp560.pile=((_tmp561=
_region_malloc(_tmp198,sizeof(*_tmp561)),((_tmp561->rgn=_tmp198,((_tmp561->places=
0,_tmp561)))))),((_tmp560.d=d,((_tmp560.loc=loc,_tmp560)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp19C,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp19B),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp19B,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp571(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp570,unsigned int*_tmp56F,void***_tmp56D,struct
_dyneither_ptr*_tmp1E5,struct _dyneither_ptr*_tmp1E1){for(*_tmp570=0;*_tmp570 < *
_tmp56F;(*_tmp570)++){(*_tmp56D)[*_tmp570]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1E1).curr)[(
int)*_tmp570],*((void**)_check_dyneither_subscript(*_tmp1E5,sizeof(void*),(int)*
_tmp570)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp19F=(
env->fenv)->r;{struct _tuple0 _tmp562;struct _tuple0 _tmp1A1=(_tmp562.f1=r1,((
_tmp562.f2=r2,_tmp562)));void*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;int _tmp1A5;
void*_tmp1A6;void*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;int _tmp1AA;void*_tmp1AB;
void*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;int _tmp1AF;void*_tmp1B0;void*_tmp1B1;
struct Cyc_Absyn_Exp*_tmp1B3;int _tmp1B4;void*_tmp1B5;void*_tmp1B6;struct Cyc_CfFlowInfo_Place*
_tmp1B8;void*_tmp1B9;struct Cyc_CfFlowInfo_Place*_tmp1BB;void*_tmp1BC;struct Cyc_CfFlowInfo_Place*
_tmp1BE;void*_tmp1BF;void*_tmp1C1;void*_tmp1C3;struct Cyc_CfFlowInfo_Place*
_tmp1C5;void*_tmp1C6;struct Cyc_CfFlowInfo_Place*_tmp1C8;void*_tmp1C9;void*
_tmp1CB;void*_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp1CF;void*_tmp1D0;struct Cyc_CfFlowInfo_Place*
_tmp1D2;void*_tmp1D3;struct Cyc_CfFlowInfo_Place*_tmp1D5;void*_tmp1D6;void*
_tmp1D8;void*_tmp1DA;void*_tmp1DC;void*_tmp1DE;int _tmp1E0;struct _dyneither_ptr
_tmp1E1;void*_tmp1E2;int _tmp1E4;struct _dyneither_ptr _tmp1E5;_LL9F: _tmp1A2=
_tmp1A1.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1A3=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1A2;if(_tmp1A3->tag != 7)goto _LLA1;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;
_tmp1A6=(void*)_tmp1A3->f3;}};_tmp1A7=_tmp1A1.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1A8=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1A7;if(_tmp1A8->tag != 7)goto
_LLA1;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;_tmp1AB=(void*)_tmp1A8->f3;}};
_LLA0: if(_tmp1A4 == _tmp1A9  && _tmp1A5 == _tmp1AA){struct Cyc_CfFlowInfo_Consumed_struct
_tmp565;struct Cyc_CfFlowInfo_Consumed_struct*_tmp564;return(void*)((_tmp564=
_region_malloc(_tmp19F,sizeof(*_tmp564)),((_tmp564[0]=((_tmp565.tag=7,((_tmp565.f1=
_tmp1A4,((_tmp565.f2=_tmp1A5,((_tmp565.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1A6,_tmp1AB),_tmp565)))))))),_tmp564))));}{void*_tmp1E8=_tmp1A6;struct Cyc_CfFlowInfo_Place*
_tmp1EA;_LLBC: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1E9=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1E8;if(_tmp1E9->tag != 5)goto _LLBE;else{_tmp1EA=_tmp1E9->f1;}}_LLBD: Cyc_CfFlowInfo_add_place(
env->pile,_tmp1EA);goto _LLBB;_LLBE:;_LLBF: goto _LLBB;_LLBB:;}{void*_tmp1EB=
_tmp1AB;struct Cyc_CfFlowInfo_Place*_tmp1ED;_LLC1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1EC=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EB;if(_tmp1EC->tag != 5)goto
_LLC3;else{_tmp1ED=_tmp1EC->f1;}}_LLC2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1ED);goto _LLC0;_LLC3:;_LLC4: goto _LLC0;_LLC0:;}goto _LL9E;_LLA1: _tmp1AC=
_tmp1A1.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1AD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1AC;if(_tmp1AD->tag != 7)goto _LLA3;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;
_tmp1B0=(void*)_tmp1AD->f3;}};_LLA2: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp568;struct Cyc_CfFlowInfo_Consumed_struct*_tmp567;return(void*)((_tmp567=
_region_malloc(_tmp19F,sizeof(*_tmp567)),((_tmp567[0]=((_tmp568.tag=7,((_tmp568.f1=
_tmp1AE,((_tmp568.f2=_tmp1AF,((_tmp568.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1B0,r2),_tmp568)))))))),_tmp567))));}_LLA3: _tmp1B1=_tmp1A1.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1B2=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1B1;if(_tmp1B2->tag != 7)goto
_LLA5;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=(void*)_tmp1B2->f3;}};
_LLA4: {struct Cyc_CfFlowInfo_Consumed_struct _tmp56B;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp56A;return(void*)((_tmp56A=_region_malloc(_tmp19F,sizeof(*_tmp56A)),((
_tmp56A[0]=((_tmp56B.tag=7,((_tmp56B.f1=_tmp1B3,((_tmp56B.f2=_tmp1B4,((_tmp56B.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1B5),_tmp56B)))))))),_tmp56A))));}
_LLA5: _tmp1B6=_tmp1A1.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1B7=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B6;if(_tmp1B7->tag != 5)goto _LLA7;else{
_tmp1B8=_tmp1B7->f1;}};_tmp1B9=_tmp1A1.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1BA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B9;if(_tmp1BA->tag != 5)goto
_LLA7;else{_tmp1BB=_tmp1BA->f1;}};_LLA6: if(Cyc_CfFlowInfo_place_cmp(_tmp1B8,
_tmp1BB)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1B8);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1BB);goto _LL9E;_LLA7: _tmp1BC=_tmp1A1.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1BD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1BC;if(_tmp1BD->tag != 5)goto
_LLA9;else{_tmp1BE=_tmp1BD->f1;}};_tmp1BF=_tmp1A1.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1C0=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1BF;if(_tmp1C0->tag != 2)
goto _LLA9;};_LLA8: _tmp1C5=_tmp1BE;goto _LLAA;_LLA9: _tmp1C1=_tmp1A1.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1C2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1C1;if(_tmp1C2->tag != 2)
goto _LLAB;};_tmp1C3=_tmp1A1.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1C4=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1C3;if(_tmp1C4->tag != 5)goto _LLAB;
else{_tmp1C5=_tmp1C4->f1;}};_LLAA: Cyc_CfFlowInfo_add_place(env->pile,_tmp1C5);
return(env->fenv)->notzerothis;_LLAB: _tmp1C6=_tmp1A1.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1C7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1C6;if(_tmp1C7->tag != 5)goto
_LLAD;else{_tmp1C8=_tmp1C7->f1;}};_tmp1C9=_tmp1A1.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1CA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1C9;if(_tmp1CA->tag != 1)
goto _LLAD;};_LLAC: Cyc_CfFlowInfo_add_place(env->pile,_tmp1C8);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLC5: return(env->fenv)->notzeroall;
default: _LLC6: return(env->fenv)->notzerothis;}_LLAD: _tmp1CB=_tmp1A1.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1CC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1CB;if(_tmp1CC->tag != 1)goto _LLAF;};_tmp1CD=_tmp1A1.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1CE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1CD;if(_tmp1CE->tag != 5)goto
_LLAF;else{_tmp1CF=_tmp1CE->f1;}};_LLAE: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1CF);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLC8: return(env->fenv)->notzeroall;default: _LLC9: return(env->fenv)->notzerothis;}
_LLAF: _tmp1D0=_tmp1A1.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1D1=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D0;if(_tmp1D1->tag != 5)goto _LLB1;else{
_tmp1D2=_tmp1D1->f1;}};_LLB0: Cyc_CfFlowInfo_add_place(env->pile,_tmp1D2);goto
_LL9E;_LLB1: _tmp1D3=_tmp1A1.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1D4=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D3;if(_tmp1D4->tag != 5)goto _LLB3;
else{_tmp1D5=_tmp1D4->f1;}};_LLB2: Cyc_CfFlowInfo_add_place(env->pile,_tmp1D5);
goto _LL9E;_LLB3: _tmp1D6=_tmp1A1.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1D7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1D6;if(_tmp1D7->tag != 1)
goto _LLB5;};_tmp1D8=_tmp1A1.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp1D9=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1D8;if(_tmp1D9->tag != 2)goto _LLB5;};
_LLB4: goto _LLB6;_LLB5: _tmp1DA=_tmp1A1.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1DB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1DA;if(_tmp1DB->tag != 2)
goto _LLB7;};_tmp1DC=_tmp1A1.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1DD=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1DC;if(_tmp1DD->tag != 1)goto _LLB7;};
_LLB6: return(env->fenv)->notzerothis;_LLB7: _tmp1DE=_tmp1A1.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1DF=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1DE;if(_tmp1DF->tag != 6)goto
_LLB9;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}};_tmp1E2=_tmp1A1.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1E3=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1E2;if(_tmp1E3->tag != 6)goto _LLB9;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}};
_LLB8: {unsigned int _tmp570;unsigned int _tmp56F;struct _dyneither_ptr _tmp56E;void**
_tmp56D;unsigned int _tmp56C;struct _dyneither_ptr new_d=(_tmp56C=
_get_dyneither_size(_tmp1E1,sizeof(void*)),((_tmp56D=(void**)_region_malloc(
_tmp19F,_check_times(sizeof(void*),_tmp56C)),((_tmp56E=_tag_dyneither(_tmp56D,
sizeof(void*),_tmp56C),((((_tmp56F=_tmp56C,_tmp571(& env,& _tmp570,& _tmp56F,&
_tmp56D,& _tmp1E5,& _tmp1E1))),_tmp56E)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1E1,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1E1.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1E1,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1E5,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp574;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp573;return(void*)((_tmp573=_region_malloc(_tmp19F,sizeof(*_tmp573)),((
_tmp573[0]=((_tmp574.tag=6,((_tmp574.f1=_tmp1E0,((_tmp574.f2=new_d,_tmp574)))))),
_tmp573))));};}_LLB9:;_LLBA: goto _LL9E;_LL9E:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp575;struct
_tuple0 _tmp1F9=(_tmp575.f1=r1,((_tmp575.f2=r2,_tmp575)));void*_tmp1FA;void*
_tmp1FC;_LLCC: _tmp1FA=_tmp1F9.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1FB=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1FA;if(_tmp1FB->tag != 4)goto _LLCE;};_LLCD:
goto _LLCF;_LLCE: _tmp1FC=_tmp1F9.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1FD=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1FC;if(_tmp1FD->tag != 4)goto _LLD0;};_LLCF: {
struct _tuple19 _tmp576;struct _tuple19 _tmp1FF=(_tmp576.f1=il1,((_tmp576.f2=il2,
_tmp576)));enum Cyc_CfFlowInfo_InitLevel _tmp200;enum Cyc_CfFlowInfo_InitLevel
_tmp201;enum Cyc_CfFlowInfo_InitLevel _tmp202;enum Cyc_CfFlowInfo_InitLevel
_tmp203;_LLD3: _tmp200=_tmp1FF.f2;if(_tmp200 != Cyc_CfFlowInfo_NoneIL)goto _LLD5;
_LLD4: goto _LLD6;_LLD5: _tmp201=_tmp1FF.f1;if(_tmp201 != Cyc_CfFlowInfo_NoneIL)goto
_LLD7;_LLD6: return(env->fenv)->esc_none;_LLD7: _tmp202=_tmp1FF.f2;if(_tmp202 != 
Cyc_CfFlowInfo_ThisIL)goto _LLD9;_LLD8: goto _LLDA;_LLD9: _tmp203=_tmp1FF.f1;if(
_tmp203 != Cyc_CfFlowInfo_ThisIL)goto _LLDB;_LLDA: return(env->fenv)->esc_this;
_LLDB:;_LLDC: return(env->fenv)->esc_all;_LLD2:;}_LLD0:;_LLD1: {struct _tuple19
_tmp577;struct _tuple19 _tmp205=(_tmp577.f1=il1,((_tmp577.f2=il2,_tmp577)));enum 
Cyc_CfFlowInfo_InitLevel _tmp206;enum Cyc_CfFlowInfo_InitLevel _tmp207;enum Cyc_CfFlowInfo_InitLevel
_tmp208;enum Cyc_CfFlowInfo_InitLevel _tmp209;_LLDE: _tmp206=_tmp205.f2;if(_tmp206
!= Cyc_CfFlowInfo_NoneIL)goto _LLE0;_LLDF: goto _LLE1;_LLE0: _tmp207=_tmp205.f1;if(
_tmp207 != Cyc_CfFlowInfo_NoneIL)goto _LLE2;_LLE1: return(env->fenv)->unknown_none;
_LLE2: _tmp208=_tmp205.f2;if(_tmp208 != Cyc_CfFlowInfo_ThisIL)goto _LLE4;_LLE3: goto
_LLE5;_LLE4: _tmp209=_tmp205.f1;if(_tmp209 != Cyc_CfFlowInfo_ThisIL)goto _LLE6;
_LLE5: return(env->fenv)->unknown_this;_LLE6:;_LLE7: return(env->fenv)->unknown_all;
_LLDD:;}_LLCB:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp578;return((_tmp578.EqualConst).val=i,(((_tmp578.EqualConst).tag=
1,_tmp578)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple10 _tmp57B;union Cyc_CfFlowInfo_RelnOp _tmp57A;return((
_tmp57A.LessVar).val=((_tmp57B.f1=v,((_tmp57B.f2=t,_tmp57B)))),(((_tmp57A.LessVar).tag=
2,_tmp57A)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp57C;return((_tmp57C.LessNumelts).val=v,(((
_tmp57C.LessNumelts).tag=3,_tmp57C)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp57D;return((_tmp57D.LessConst).val=i,(((_tmp57D.LessConst).tag=
4,_tmp57D)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp
_tmp57E;return((_tmp57E.LessEqConst).val=i,(((_tmp57E.LessEqConst).tag=6,_tmp57E)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp57F;return((_tmp57F.LessEqNumelts).val=v,(((
_tmp57F.LessEqNumelts).tag=5,_tmp57F)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp580;struct _tuple20 _tmp212=(
_tmp580.f1=r1,((_tmp580.f2=r2,_tmp580)));union Cyc_CfFlowInfo_RelnOp _tmp213;
unsigned int _tmp214;union Cyc_CfFlowInfo_RelnOp _tmp215;unsigned int _tmp216;union
Cyc_CfFlowInfo_RelnOp _tmp217;struct _tuple10 _tmp218;struct Cyc_Absyn_Vardecl*
_tmp219;union Cyc_CfFlowInfo_RelnOp _tmp21A;struct _tuple10 _tmp21B;struct Cyc_Absyn_Vardecl*
_tmp21C;union Cyc_CfFlowInfo_RelnOp _tmp21D;struct Cyc_Absyn_Vardecl*_tmp21E;union
Cyc_CfFlowInfo_RelnOp _tmp21F;struct Cyc_Absyn_Vardecl*_tmp220;union Cyc_CfFlowInfo_RelnOp
_tmp221;unsigned int _tmp222;union Cyc_CfFlowInfo_RelnOp _tmp223;unsigned int
_tmp224;union Cyc_CfFlowInfo_RelnOp _tmp225;unsigned int _tmp226;union Cyc_CfFlowInfo_RelnOp
_tmp227;unsigned int _tmp228;union Cyc_CfFlowInfo_RelnOp _tmp229;struct Cyc_Absyn_Vardecl*
_tmp22A;union Cyc_CfFlowInfo_RelnOp _tmp22B;struct Cyc_Absyn_Vardecl*_tmp22C;_LLE9:
_tmp213=_tmp212.f1;if((_tmp213.EqualConst).tag != 1)goto _LLEB;_tmp214=(
unsigned int)(_tmp213.EqualConst).val;_tmp215=_tmp212.f2;if((_tmp215.EqualConst).tag
!= 1)goto _LLEB;_tmp216=(unsigned int)(_tmp215.EqualConst).val;_LLEA: return
_tmp214 == _tmp216;_LLEB: _tmp217=_tmp212.f1;if((_tmp217.LessVar).tag != 2)goto
_LLED;_tmp218=(struct _tuple10)(_tmp217.LessVar).val;_tmp219=_tmp218.f1;_tmp21A=
_tmp212.f2;if((_tmp21A.LessVar).tag != 2)goto _LLED;_tmp21B=(struct _tuple10)(
_tmp21A.LessVar).val;_tmp21C=_tmp21B.f1;_LLEC: return _tmp219 == _tmp21C;_LLED:
_tmp21D=_tmp212.f1;if((_tmp21D.LessNumelts).tag != 3)goto _LLEF;_tmp21E=(struct Cyc_Absyn_Vardecl*)(
_tmp21D.LessNumelts).val;_tmp21F=_tmp212.f2;if((_tmp21F.LessNumelts).tag != 3)
goto _LLEF;_tmp220=(struct Cyc_Absyn_Vardecl*)(_tmp21F.LessNumelts).val;_LLEE:
return _tmp21E == _tmp220;_LLEF: _tmp221=_tmp212.f1;if((_tmp221.LessConst).tag != 4)
goto _LLF1;_tmp222=(unsigned int)(_tmp221.LessConst).val;_tmp223=_tmp212.f2;if((
_tmp223.LessConst).tag != 4)goto _LLF1;_tmp224=(unsigned int)(_tmp223.LessConst).val;
_LLF0: return _tmp222 == _tmp224;_LLF1: _tmp225=_tmp212.f1;if((_tmp225.LessEqConst).tag
!= 6)goto _LLF3;_tmp226=(unsigned int)(_tmp225.LessEqConst).val;_tmp227=_tmp212.f2;
if((_tmp227.LessEqConst).tag != 6)goto _LLF3;_tmp228=(unsigned int)(_tmp227.LessEqConst).val;
_LLF2: return _tmp226 == _tmp228;_LLF3: _tmp229=_tmp212.f1;if((_tmp229.LessEqNumelts).tag
!= 5)goto _LLF5;_tmp22A=(struct Cyc_Absyn_Vardecl*)(_tmp229.LessEqNumelts).val;
_tmp22B=_tmp212.f2;if((_tmp22B.LessEqNumelts).tag != 5)goto _LLF5;_tmp22C=(struct
Cyc_Absyn_Vardecl*)(_tmp22B.LessEqNumelts).val;_LLF4: return _tmp22A == _tmp22C;
_LLF5:;_LLF6: return 0;_LLE8:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp22D=r1s;for(0;_tmp22D != 0;_tmp22D=_tmp22D->tl){
struct Cyc_CfFlowInfo_Reln*_tmp22E=(struct Cyc_CfFlowInfo_Reln*)_tmp22D->hd;int
found=0;{struct Cyc_List_List*_tmp22F=r2s;for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
struct Cyc_CfFlowInfo_Reln*_tmp230=(struct Cyc_CfFlowInfo_Reln*)_tmp22F->hd;if(
_tmp22E == _tmp230  || _tmp22E->vd == _tmp230->vd  && Cyc_CfFlowInfo_same_relop(
_tmp22E->rop,_tmp230->rop)){{struct Cyc_List_List*_tmp581;res=((_tmp581=
_region_malloc(r,sizeof(*_tmp581)),((_tmp581->hd=_tmp22E,((_tmp581->tl=res,
_tmp581))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;};}
struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp582;struct _tuple21 _tmp233=(_tmp582.f1=f1,((_tmp582.f2=f2,
_tmp582)));union Cyc_CfFlowInfo_FlowInfo _tmp234;int _tmp235;union Cyc_CfFlowInfo_FlowInfo
_tmp236;int _tmp237;union Cyc_CfFlowInfo_FlowInfo _tmp238;struct _tuple11 _tmp239;
struct Cyc_Dict_Dict _tmp23A;struct Cyc_List_List*_tmp23B;union Cyc_CfFlowInfo_FlowInfo
_tmp23C;struct _tuple11 _tmp23D;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*
_tmp23F;_LLF8: _tmp234=_tmp233.f1;if((_tmp234.BottomFL).tag != 1)goto _LLFA;_tmp235=(
int)(_tmp234.BottomFL).val;_LLF9: return f2;_LLFA: _tmp236=_tmp233.f2;if((_tmp236.BottomFL).tag
!= 1)goto _LLFC;_tmp237=(int)(_tmp236.BottomFL).val;_LLFB: return f1;_LLFC: _tmp238=
_tmp233.f1;if((_tmp238.ReachableFL).tag != 2)goto _LLF7;_tmp239=(struct _tuple11)(
_tmp238.ReachableFL).val;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_tmp23C=_tmp233.f2;
if((_tmp23C.ReachableFL).tag != 2)goto _LLF7;_tmp23D=(struct _tuple11)(_tmp23C.ReachableFL).val;
_tmp23E=_tmp23D.f1;_tmp23F=_tmp23D.f2;_LLFD: if(_tmp23A.t == _tmp23E.t  && _tmp23B
== _tmp23F)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(
Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct _RegionHandle*_tmp240=
fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp585;struct Cyc_CfFlowInfo_JoinEnv _tmp584;
struct Cyc_CfFlowInfo_JoinEnv _tmp241=(_tmp584.fenv=fenv,((_tmp584.pile=((_tmp585=
_region_malloc(_tmp240,sizeof(*_tmp585)),((_tmp585->rgn=_tmp240,((_tmp585->places=
0,_tmp585)))))),((_tmp584.d1=_tmp23A,((_tmp584.d2=_tmp23E,_tmp584)))))));struct
Cyc_Dict_Dict _tmp242=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,
void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct
Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp241,_tmp23A,_tmp23E);
struct Cyc_List_List*_tmp243=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp23B,_tmp23F);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp241.pile,
all_changed,_tmp242),_tmp243);};_LLF7:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct
_tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2);struct
_tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){union Cyc_CfFlowInfo_FlowInfo
_tmp247;void*_tmp248;struct _tuple12 _tmp246=pr1;_tmp247=_tmp246.f1;_tmp248=
_tmp246.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp24A;void*_tmp24B;struct _tuple12
_tmp249=pr2;_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;{union Cyc_CfFlowInfo_FlowInfo
outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp247,_tmp24A);struct
_tuple22 _tmp586;struct _tuple22 _tmp24D=(_tmp586.f1=_tmp247,((_tmp586.f2=_tmp24A,((
_tmp586.f3=outflow,_tmp586)))));union Cyc_CfFlowInfo_FlowInfo _tmp24E;int _tmp24F;
union Cyc_CfFlowInfo_FlowInfo _tmp250;int _tmp251;union Cyc_CfFlowInfo_FlowInfo
_tmp252;struct _tuple11 _tmp253;struct Cyc_Dict_Dict _tmp254;union Cyc_CfFlowInfo_FlowInfo
_tmp255;struct _tuple11 _tmp256;struct Cyc_Dict_Dict _tmp257;union Cyc_CfFlowInfo_FlowInfo
_tmp258;struct _tuple11 _tmp259;struct Cyc_Dict_Dict _tmp25A;struct Cyc_List_List*
_tmp25B;_LLFF: _tmp24E=_tmp24D.f1;if((_tmp24E.BottomFL).tag != 1)goto _LL101;
_tmp24F=(int)(_tmp24E.BottomFL).val;_LL100: {struct _tuple12 _tmp587;return(
_tmp587.f1=outflow,((_tmp587.f2=_tmp24B,_tmp587)));}_LL101: _tmp250=_tmp24D.f2;
if((_tmp250.BottomFL).tag != 1)goto _LL103;_tmp251=(int)(_tmp250.BottomFL).val;
_LL102: {struct _tuple12 _tmp588;return(_tmp588.f1=outflow,((_tmp588.f2=_tmp248,
_tmp588)));}_LL103: _tmp252=_tmp24D.f1;if((_tmp252.ReachableFL).tag != 2)goto
_LL105;_tmp253=(struct _tuple11)(_tmp252.ReachableFL).val;_tmp254=_tmp253.f1;
_tmp255=_tmp24D.f2;if((_tmp255.ReachableFL).tag != 2)goto _LL105;_tmp256=(struct
_tuple11)(_tmp255.ReachableFL).val;_tmp257=_tmp256.f1;_tmp258=_tmp24D.f3;if((
_tmp258.ReachableFL).tag != 2)goto _LL105;_tmp259=(struct _tuple11)(_tmp258.ReachableFL).val;
_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;_LL104: if(((int(*)(int ignore,void*r1,void*
r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp248,_tmp24B)){struct _tuple12
_tmp589;return(_tmp589.f1=outflow,((_tmp589.f2=_tmp24B,_tmp589)));}if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp24B,
_tmp248)){struct _tuple12 _tmp58A;return(_tmp58A.f1=outflow,((_tmp58A.f2=_tmp248,
_tmp58A)));}{struct _RegionHandle*_tmp260=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp58D;struct Cyc_CfFlowInfo_JoinEnv _tmp58C;struct Cyc_CfFlowInfo_JoinEnv _tmp261=(
_tmp58C.fenv=fenv,((_tmp58C.pile=((_tmp58D=_region_malloc(_tmp260,sizeof(*
_tmp58D)),((_tmp58D->rgn=_tmp260,((_tmp58D->places=0,_tmp58D)))))),((_tmp58C.d1=
_tmp254,((_tmp58C.d2=_tmp257,_tmp58C)))))));void*_tmp262=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp261,0,_tmp248,
_tmp24B);struct _tuple12 _tmp58E;return(_tmp58E.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp261.pile,all_changed,_tmp25A),_tmp25B),((_tmp58E.f2=_tmp262,_tmp58E)));};
_LL105:;_LL106: {struct Cyc_Core_Impossible_struct _tmp594;const char*_tmp593;
struct Cyc_Core_Impossible_struct*_tmp592;(int)_throw((void*)((_tmp592=_cycalloc(
sizeof(*_tmp592)),((_tmp592[0]=((_tmp594.tag=Cyc_Core_Impossible,((_tmp594.f1=((
_tmp593="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp593,sizeof(
char),37))),_tmp594)))),_tmp592)))));}_LLFE:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp59B(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp59A,unsigned int*_tmp599,void***_tmp597,struct
_dyneither_ptr*_tmp276,struct _dyneither_ptr*_tmp272){for(*_tmp59A=0;*_tmp59A < *
_tmp599;(*_tmp59A)++){(*_tmp597)[*_tmp59A]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp59A,((void**)(*_tmp272).curr)[(int)*_tmp59A],*((void**)
_check_dyneither_subscript(*_tmp276,sizeof(void*),(int)*_tmp59A)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp595;
struct _tuple0 _tmp26A=(_tmp595.f1=r1,((_tmp595.f2=r2,_tmp595)));void*_tmp26B;void*
_tmp26D;void*_tmp26F;int _tmp271;struct _dyneither_ptr _tmp272;void*_tmp273;int
_tmp275;struct _dyneither_ptr _tmp276;_LL108: _tmp26B=_tmp26A.f1;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp26C=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp26B;if(_tmp26C->tag != 7)goto
_LL10A;};_LL109: goto _LL10B;_LL10A: _tmp26D=_tmp26A.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp26E=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp26D;if(_tmp26E->tag != 7)goto
_LL10C;};_LL10B: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL10C: _tmp26F=
_tmp26A.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp270=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp26F;if(_tmp270->tag != 6)goto _LL10E;else{_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;}};
_tmp273=_tmp26A.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp274=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp273;if(_tmp274->tag != 6)goto _LL10E;else{_tmp275=_tmp274->f1;_tmp276=_tmp274->f2;}};
_LL10D: {unsigned int _tmp59A;unsigned int _tmp599;struct _dyneither_ptr _tmp598;
void**_tmp597;unsigned int _tmp596;struct _dyneither_ptr new_d=(_tmp596=
_get_dyneither_size(_tmp272,sizeof(void*)),((_tmp597=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp596)),((_tmp598=
_tag_dyneither(_tmp597,sizeof(void*),_tmp596),((((_tmp599=_tmp596,_tmp59B(& env,&
_tmp59A,& _tmp599,& _tmp597,& _tmp276,& _tmp272))),_tmp598)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp272,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp272.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp272,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp276,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp59E;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp59D;return(void*)((_tmp59D=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp59D)),((_tmp59D[0]=((_tmp59E.tag=6,((_tmp59E.f1=_tmp271,((_tmp59E.f2=new_d,
_tmp59E)))))),_tmp59D))));};}_LL10E:;_LL10F: {struct Cyc_Core_Impossible_struct
_tmp5A4;const char*_tmp5A3;struct Cyc_Core_Impossible_struct*_tmp5A2;(int)_throw((
void*)((_tmp5A2=_cycalloc(sizeof(*_tmp5A2)),((_tmp5A2[0]=((_tmp5A4.tag=Cyc_Core_Impossible,((
_tmp5A4.f1=((_tmp5A3="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5A3,
sizeof(char),33))),_tmp5A4)))),_tmp5A2)))));}_LL107:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp280=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp281=env->changed;{struct Cyc_List_List*_tmp5A5;*env->last_field_cell=((
_tmp5A5=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5A5)),((_tmp5A5->hd=(
void*)key,((_tmp5A5->tl=0,_tmp5A5))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp283=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp280;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp281;return _tmp283;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp5A6;*env->curr_place=((_tmp5A6.root=root,((_tmp5A6.fields=
0,_tmp5A6))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple21 _tmp5A7;struct _tuple21
_tmp286=(_tmp5A7.f1=f1,((_tmp5A7.f2=f2,_tmp5A7)));union Cyc_CfFlowInfo_FlowInfo
_tmp287;int _tmp288;union Cyc_CfFlowInfo_FlowInfo _tmp289;int _tmp28A;union Cyc_CfFlowInfo_FlowInfo
_tmp28B;struct _tuple11 _tmp28C;struct Cyc_Dict_Dict _tmp28D;struct Cyc_List_List*
_tmp28E;union Cyc_CfFlowInfo_FlowInfo _tmp28F;struct _tuple11 _tmp290;struct Cyc_Dict_Dict
_tmp291;struct Cyc_List_List*_tmp292;_LL111: _tmp287=_tmp286.f1;if((_tmp287.BottomFL).tag
!= 1)goto _LL113;_tmp288=(int)(_tmp287.BottomFL).val;_LL112: goto _LL114;_LL113:
_tmp289=_tmp286.f2;if((_tmp289.BottomFL).tag != 1)goto _LL115;_tmp28A=(int)(
_tmp289.BottomFL).val;_LL114: return Cyc_CfFlowInfo_BottomFL();_LL115: _tmp28B=
_tmp286.f1;if((_tmp28B.ReachableFL).tag != 2)goto _LL110;_tmp28C=(struct _tuple11)(
_tmp28B.ReachableFL).val;_tmp28D=_tmp28C.f1;_tmp28E=_tmp28C.f2;_tmp28F=_tmp286.f2;
if((_tmp28F.ReachableFL).tag != 2)goto _LL110;_tmp290=(struct _tuple11)(_tmp28F.ReachableFL).val;
_tmp291=_tmp290.f1;_tmp292=_tmp290.f2;_LL116: if(_tmp28D.t == _tmp291.t  && _tmp28E
== _tmp292)return f1;{struct _RegionHandle*_tmp293=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp294=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp5AD;struct Cyc_CfFlowInfo_EscPile*
_tmp5AC;struct Cyc_CfFlowInfo_AfterEnv _tmp5AB;struct Cyc_CfFlowInfo_AfterEnv
_tmp295=(_tmp5AB.joinenv=((_tmp5AD.fenv=fenv,((_tmp5AD.pile=((_tmp5AC=
_region_malloc(_tmp293,sizeof(*_tmp5AC)),((_tmp5AC->rgn=_tmp293,((_tmp5AC->places=
0,_tmp5AC)))))),((_tmp5AD.d1=_tmp28D,((_tmp5AD.d2=_tmp291,_tmp5AD)))))))),((
_tmp5AB.chg1=chg1,((_tmp5AB.chg2=chg2,((_tmp5AB.curr_place=_tmp294,((_tmp5AB.last_field_cell=(
struct Cyc_List_List**)& _tmp294->fields,((_tmp5AB.changed=Cyc_CfFlowInfo_Neither,
_tmp5AB)))))))))));struct Cyc_Dict_Dict _tmp296=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp295,_tmp28D,_tmp291);struct Cyc_List_List*_tmp297=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp28E,_tmp292);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp295.joinenv).pile,all_changed,_tmp296),_tmp297);};_LL110:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp5AE;struct
_tuple0 _tmp29C=(_tmp5AE.f1=r1,((_tmp5AE.f2=r2,_tmp5AE)));void*_tmp29D;struct Cyc_CfFlowInfo_Place*
_tmp29F;void*_tmp2A0;struct Cyc_CfFlowInfo_Place*_tmp2A2;void*_tmp2A3;void*
_tmp2A5;void*_tmp2A7;int _tmp2A9;struct _dyneither_ptr _tmp2AA;void*_tmp2AB;int
_tmp2AD;struct _dyneither_ptr _tmp2AE;void*_tmp2AF;void*_tmp2B1;void*_tmp2B3;void*
_tmp2B5;void*_tmp2B7;void*_tmp2B9;void*_tmp2BB;void*_tmp2BD;_LL118: _tmp29D=
_tmp29C.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp29E=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp29D;if(_tmp29E->tag != 5)goto _LL11A;else{_tmp29F=_tmp29E->f1;}};_tmp2A0=
_tmp29C.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2A1=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A0;if(_tmp2A1->tag != 5)goto _LL11A;else{_tmp2A2=_tmp2A1->f1;}};_LL119: return
Cyc_CfFlowInfo_place_cmp(_tmp29F,_tmp2A2)== 0;_LL11A: _tmp2A3=_tmp29C.f1;{struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp2A4=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A3;if(_tmp2A4->tag != 5)goto _LL11C;};_LL11B: goto _LL11D;_LL11C: _tmp2A5=
_tmp29C.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2A6=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A5;if(_tmp2A6->tag != 5)goto _LL11E;};_LL11D: return 0;_LL11E: _tmp2A7=_tmp29C.f1;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2A8=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2A7;if(_tmp2A8->tag != 6)goto _LL120;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f2;}};
_tmp2AB=_tmp29C.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2AC=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2AB;if(_tmp2AC->tag != 6)goto _LL120;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}};
_LL11F: if((void**)_tmp2AA.curr == (void**)_tmp2AE.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2AA,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2AA.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2AE,sizeof(void*),i))))return 0;}}return 1;
_LL120: _tmp2AF=_tmp29C.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2B0=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2AF;if(_tmp2B0->tag != 1)goto _LL122;};
_tmp2B1=_tmp29C.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2B2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2B1;if(_tmp2B2->tag != 2)goto _LL122;};_LL121: return 1;_LL122: _tmp2B3=_tmp29C.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2B4=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2B3;if(_tmp2B4->tag != 2)goto _LL124;};_LL123: return 0;_LL124: _tmp2B5=_tmp29C.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2B6=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2B5;if(_tmp2B6->tag != 0)goto _LL126;};_LL125: goto _LL127;_LL126: _tmp2B7=
_tmp29C.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2B8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2B7;if(_tmp2B8->tag != 1)goto _LL128;};_LL127: return 0;_LL128: _tmp2B9=_tmp29C.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2BA=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2B9;if(_tmp2BA->tag != 4)goto _LL12A;};_tmp2BB=_tmp29C.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2BC=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2BB;if(_tmp2BC->tag != 4)goto _LL12A;};
_LL129: goto _LL117;_LL12A: _tmp2BD=_tmp29C.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2BE=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2BD;if(_tmp2BE->tag != 4)goto _LL12C;};
_LL12B: return 0;_LL12C:;_LL12D: goto _LL117;_LL117:;}{struct _tuple19 _tmp5AF;struct
_tuple19 _tmp2C0=(_tmp5AF.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5AF.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp5AF)));enum Cyc_CfFlowInfo_InitLevel _tmp2C1;enum Cyc_CfFlowInfo_InitLevel
_tmp2C2;enum Cyc_CfFlowInfo_InitLevel _tmp2C3;enum Cyc_CfFlowInfo_InitLevel
_tmp2C4;enum Cyc_CfFlowInfo_InitLevel _tmp2C5;enum Cyc_CfFlowInfo_InitLevel
_tmp2C6;_LL12F: _tmp2C1=_tmp2C0.f1;if(_tmp2C1 != Cyc_CfFlowInfo_AllIL)goto _LL131;
_tmp2C2=_tmp2C0.f2;if(_tmp2C2 != Cyc_CfFlowInfo_AllIL)goto _LL131;_LL130: return 1;
_LL131: _tmp2C3=_tmp2C0.f2;if(_tmp2C3 != Cyc_CfFlowInfo_NoneIL)goto _LL133;_LL132:
return 1;_LL133: _tmp2C4=_tmp2C0.f1;if(_tmp2C4 != Cyc_CfFlowInfo_NoneIL)goto _LL135;
_LL134: return 0;_LL135: _tmp2C5=_tmp2C0.f2;if(_tmp2C5 != Cyc_CfFlowInfo_ThisIL)goto
_LL137;_LL136: return 1;_LL137: _tmp2C6=_tmp2C0.f1;if(_tmp2C6 != Cyc_CfFlowInfo_ThisIL)
goto _LL12E;_LL138: return 0;_LL12E:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C7=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp2C8=r2s;for(0;_tmp2C8 != 0;_tmp2C8=
_tmp2C8->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C9=(struct Cyc_CfFlowInfo_Reln*)
_tmp2C8->hd;if(_tmp2C7 == _tmp2C9  || _tmp2C7->vd == _tmp2C9->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2C7->rop,_tmp2C9->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp5B0;struct _tuple21 _tmp2CB=(_tmp5B0.f1=f1,((_tmp5B0.f2=f2,_tmp5B0)));union Cyc_CfFlowInfo_FlowInfo
_tmp2CC;int _tmp2CD;union Cyc_CfFlowInfo_FlowInfo _tmp2CE;int _tmp2CF;union Cyc_CfFlowInfo_FlowInfo
_tmp2D0;struct _tuple11 _tmp2D1;struct Cyc_Dict_Dict _tmp2D2;struct Cyc_List_List*
_tmp2D3;union Cyc_CfFlowInfo_FlowInfo _tmp2D4;struct _tuple11 _tmp2D5;struct Cyc_Dict_Dict
_tmp2D6;struct Cyc_List_List*_tmp2D7;_LL13A: _tmp2CC=_tmp2CB.f1;if((_tmp2CC.BottomFL).tag
!= 1)goto _LL13C;_tmp2CD=(int)(_tmp2CC.BottomFL).val;_LL13B: return 1;_LL13C:
_tmp2CE=_tmp2CB.f2;if((_tmp2CE.BottomFL).tag != 1)goto _LL13E;_tmp2CF=(int)(
_tmp2CE.BottomFL).val;_LL13D: return 0;_LL13E: _tmp2D0=_tmp2CB.f1;if((_tmp2D0.ReachableFL).tag
!= 2)goto _LL139;_tmp2D1=(struct _tuple11)(_tmp2D0.ReachableFL).val;_tmp2D2=
_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;_tmp2D4=_tmp2CB.f2;if((_tmp2D4.ReachableFL).tag != 
2)goto _LL139;_tmp2D5=(struct _tuple11)(_tmp2D4.ReachableFL).val;_tmp2D6=_tmp2D5.f1;
_tmp2D7=_tmp2D5.f2;_LL13F: if(_tmp2D2.t == _tmp2D6.t  && _tmp2D3 == _tmp2D7)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2D2,_tmp2D6) && Cyc_CfFlowInfo_relns_approx(_tmp2D3,_tmp2D7);_LL139:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2D8=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2D8->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp2D9=_tmp2D8->rop;struct _tuple10 _tmp2DA;struct Cyc_Absyn_Vardecl*
_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DD;_LL141:
if((_tmp2D9.LessVar).tag != 2)goto _LL143;_tmp2DA=(struct _tuple10)(_tmp2D9.LessVar).val;
_tmp2DB=_tmp2DA.f1;_LL142: _tmp2DC=_tmp2DB;goto _LL144;_LL143: if((_tmp2D9.LessNumelts).tag
!= 3)goto _LL145;_tmp2DC=(struct Cyc_Absyn_Vardecl*)(_tmp2D9.LessNumelts).val;
_LL144: _tmp2DD=_tmp2DC;goto _LL146;_LL145: if((_tmp2D9.LessEqNumelts).tag != 5)goto
_LL147;_tmp2DD=(struct Cyc_Absyn_Vardecl*)(_tmp2D9.LessEqNumelts).val;_LL146: if(v
== _tmp2DD)found=1;goto _LL140;_LL147:;_LL148: goto _LL140;_LL140:;};}if(!found)
return rs;{struct Cyc_List_List*_tmp2DE=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2DF=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2DF->vd != v){{union Cyc_CfFlowInfo_RelnOp
_tmp2E0=_tmp2DF->rop;struct _tuple10 _tmp2E1;struct Cyc_Absyn_Vardecl*_tmp2E2;
struct Cyc_Absyn_Vardecl*_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E4;_LL14A: if((
_tmp2E0.LessVar).tag != 2)goto _LL14C;_tmp2E1=(struct _tuple10)(_tmp2E0.LessVar).val;
_tmp2E2=_tmp2E1.f1;_LL14B: _tmp2E3=_tmp2E2;goto _LL14D;_LL14C: if((_tmp2E0.LessNumelts).tag
!= 3)goto _LL14E;_tmp2E3=(struct Cyc_Absyn_Vardecl*)(_tmp2E0.LessNumelts).val;
_LL14D: _tmp2E4=_tmp2E3;goto _LL14F;_LL14E: if((_tmp2E0.LessEqNumelts).tag != 5)goto
_LL150;_tmp2E4=(struct Cyc_Absyn_Vardecl*)(_tmp2E0.LessEqNumelts).val;_LL14F: if(v
== _tmp2E4)continue;goto _LL149;_LL150:;_LL151: goto _LL149;_LL149:;}{struct Cyc_List_List*
_tmp5B1;_tmp2DE=((_tmp5B1=_region_malloc(rgn,sizeof(*_tmp5B1)),((_tmp5B1->hd=
_tmp2DF,((_tmp5B1->tl=_tmp2DE,_tmp5B1))))));};}}return _tmp2DE;};}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct
_RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp2E6=e->r;
void*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2EA;void*_tmp2EC;struct Cyc_Absyn_Vardecl*
_tmp2EE;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F2;void*_tmp2F4;struct Cyc_Absyn_Vardecl*
_tmp2F6;_LL153: {struct Cyc_Absyn_Var_e_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2E6;if(_tmp2E7->tag != 1)goto _LL155;else{_tmp2E8=(void*)_tmp2E7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2E9=(struct Cyc_Absyn_Global_b_struct*)_tmp2E8;if(
_tmp2E9->tag != 1)goto _LL155;else{_tmp2EA=_tmp2E9->f1;}};}}_LL154: _tmp2EE=_tmp2EA;
goto _LL156;_LL155: {struct Cyc_Absyn_Var_e_struct*_tmp2EB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2E6;if(_tmp2EB->tag != 1)goto _LL157;else{_tmp2EC=(void*)_tmp2EB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2ED=(struct Cyc_Absyn_Param_b_struct*)_tmp2EC;if(
_tmp2ED->tag != 3)goto _LL157;else{_tmp2EE=_tmp2ED->f1;}};}}_LL156: _tmp2F2=_tmp2EE;
goto _LL158;_LL157: {struct Cyc_Absyn_Var_e_struct*_tmp2EF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2E6;if(_tmp2EF->tag != 1)goto _LL159;else{_tmp2F0=(void*)_tmp2EF->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2F1=(struct Cyc_Absyn_Local_b_struct*)_tmp2F0;if(
_tmp2F1->tag != 4)goto _LL159;else{_tmp2F2=_tmp2F1->f1;}};}}_LL158: _tmp2F6=_tmp2F2;
goto _LL15A;_LL159: {struct Cyc_Absyn_Var_e_struct*_tmp2F3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2E6;if(_tmp2F3->tag != 1)goto _LL15B;else{_tmp2F4=(void*)_tmp2F3->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2F5=(struct Cyc_Absyn_Pat_b_struct*)_tmp2F4;if(_tmp2F5->tag
!= 5)goto _LL15B;else{_tmp2F6=_tmp2F5->f1;}};}}_LL15A: if(!_tmp2F6->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2F6);goto _LL152;_LL15B:;_LL15C: goto _LL152;
_LL152:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*
_tmp5B2;return(_tmp5B2=_region_malloc(r2,sizeof(*_tmp5B2)),((_tmp5B2[0]=*r,
_tmp5B2)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,
struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp2F8=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp2F9=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp2F8->vd != _tmp2F9->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp2F8->rop,_tmp2F9->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp2FA=e->r;struct Cyc_Absyn_MallocInfo _tmp2FC;struct Cyc_Absyn_Exp*
_tmp2FD;int _tmp2FE;_LL15E: {struct Cyc_Absyn_Malloc_e_struct*_tmp2FB=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2FA;if(_tmp2FB->tag != 34)goto _LL160;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FC.num_elts;
_tmp2FE=_tmp2FC.fat_result;if(_tmp2FE != 1)goto _LL160;}}_LL15F: malloc_loop: {void*
_tmp2FF=_tmp2FD->r;struct Cyc_Absyn_Exp*_tmp301;void*_tmp303;struct Cyc_Absyn_Vardecl*
_tmp305;void*_tmp307;struct Cyc_Absyn_Vardecl*_tmp309;void*_tmp30B;struct Cyc_Absyn_Vardecl*
_tmp30D;void*_tmp30F;struct Cyc_Absyn_Vardecl*_tmp311;_LL163: {struct Cyc_Absyn_Cast_e_struct*
_tmp300=(struct Cyc_Absyn_Cast_e_struct*)_tmp2FF;if(_tmp300->tag != 15)goto _LL165;
else{_tmp301=_tmp300->f2;}}_LL164: _tmp2FD=_tmp301;goto malloc_loop;_LL165: {
struct Cyc_Absyn_Var_e_struct*_tmp302=(struct Cyc_Absyn_Var_e_struct*)_tmp2FF;if(
_tmp302->tag != 1)goto _LL167;else{_tmp303=(void*)_tmp302->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp304=(struct Cyc_Absyn_Pat_b_struct*)_tmp303;if(_tmp304->tag != 5)goto _LL167;
else{_tmp305=_tmp304->f1;}};}}_LL166: _tmp309=_tmp305;goto _LL168;_LL167: {struct
Cyc_Absyn_Var_e_struct*_tmp306=(struct Cyc_Absyn_Var_e_struct*)_tmp2FF;if(_tmp306->tag
!= 1)goto _LL169;else{_tmp307=(void*)_tmp306->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp308=(struct Cyc_Absyn_Local_b_struct*)_tmp307;if(_tmp308->tag != 4)goto _LL169;
else{_tmp309=_tmp308->f1;}};}}_LL168: _tmp30D=_tmp309;goto _LL16A;_LL169: {struct
Cyc_Absyn_Var_e_struct*_tmp30A=(struct Cyc_Absyn_Var_e_struct*)_tmp2FF;if(_tmp30A->tag
!= 1)goto _LL16B;else{_tmp30B=(void*)_tmp30A->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp30C=(struct Cyc_Absyn_Param_b_struct*)_tmp30B;if(_tmp30C->tag != 3)goto _LL16B;
else{_tmp30D=_tmp30C->f1;}};}}_LL16A: _tmp311=_tmp30D;goto _LL16C;_LL16B: {struct
Cyc_Absyn_Var_e_struct*_tmp30E=(struct Cyc_Absyn_Var_e_struct*)_tmp2FF;if(_tmp30E->tag
!= 1)goto _LL16D;else{_tmp30F=(void*)_tmp30E->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp310=(struct Cyc_Absyn_Global_b_struct*)_tmp30F;if(_tmp310->tag != 1)goto _LL16D;
else{_tmp311=_tmp310->f1;}};}}_LL16C: if(_tmp311->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp5B5;struct Cyc_List_List*_tmp5B4;return(_tmp5B4=_region_malloc(rgn,sizeof(*
_tmp5B4)),((_tmp5B4->hd=((_tmp5B5=_region_malloc(rgn,sizeof(*_tmp5B5)),((_tmp5B5->vd=
_tmp311,((_tmp5B5->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp5B5)))))),((_tmp5B4->tl=
r,_tmp5B4)))));};_LL16D:;_LL16E: return r;_LL162:;}_LL160:;_LL161: goto _LL15D;
_LL15D:;}{void*_tmp314=Cyc_Tcutil_compress(v->type);_LL170: {struct Cyc_Absyn_IntType_struct*
_tmp315=(struct Cyc_Absyn_IntType_struct*)_tmp314;if(_tmp315->tag != 6)goto _LL172;}
_LL171: goto _LL16F;_LL172:;_LL173: return r;_LL16F:;}loop: {void*_tmp316=e->r;
struct Cyc_Absyn_Exp*_tmp318;union Cyc_Absyn_Cnst _tmp31A;struct _tuple6 _tmp31B;int
_tmp31C;enum Cyc_Absyn_Primop _tmp31E;struct Cyc_List_List*_tmp31F;struct Cyc_List_List
_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List _tmp322;struct Cyc_Absyn_Exp*
_tmp323;enum Cyc_Absyn_Primop _tmp325;struct Cyc_List_List*_tmp326;struct Cyc_List_List
_tmp327;struct Cyc_Absyn_Exp*_tmp328;_LL175: {struct Cyc_Absyn_Cast_e_struct*
_tmp317=(struct Cyc_Absyn_Cast_e_struct*)_tmp316;if(_tmp317->tag != 15)goto _LL177;
else{_tmp318=_tmp317->f2;}}_LL176: e=_tmp318;goto loop;_LL177: {struct Cyc_Absyn_Const_e_struct*
_tmp319=(struct Cyc_Absyn_Const_e_struct*)_tmp316;if(_tmp319->tag != 0)goto _LL179;
else{_tmp31A=_tmp319->f1;if((_tmp31A.Int_c).tag != 4)goto _LL179;_tmp31B=(struct
_tuple6)(_tmp31A.Int_c).val;_tmp31C=_tmp31B.f2;}}_LL178: {struct Cyc_CfFlowInfo_Reln*
_tmp5B8;struct Cyc_List_List*_tmp5B7;return(_tmp5B7=_region_malloc(rgn,sizeof(*
_tmp5B7)),((_tmp5B7->hd=((_tmp5B8=_region_malloc(rgn,sizeof(*_tmp5B8)),((_tmp5B8->vd=
v,((_tmp5B8->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp31C),_tmp5B8)))))),((
_tmp5B7->tl=r,_tmp5B7)))));}_LL179: {struct Cyc_Absyn_Primop_e_struct*_tmp31D=(
struct Cyc_Absyn_Primop_e_struct*)_tmp316;if(_tmp31D->tag != 3)goto _LL17B;else{
_tmp31E=_tmp31D->f1;if(_tmp31E != Cyc_Absyn_Mod)goto _LL17B;_tmp31F=_tmp31D->f2;
if(_tmp31F == 0)goto _LL17B;_tmp320=*_tmp31F;_tmp321=_tmp320.tl;if(_tmp321 == 0)
goto _LL17B;_tmp322=*_tmp321;_tmp323=(struct Cyc_Absyn_Exp*)_tmp322.hd;}}_LL17A:{
void*_tmp32B=_tmp323->r;enum Cyc_Absyn_Primop _tmp32D;struct Cyc_List_List*_tmp32E;
struct Cyc_List_List _tmp32F;struct Cyc_Absyn_Exp*_tmp330;_LL180: {struct Cyc_Absyn_Primop_e_struct*
_tmp32C=(struct Cyc_Absyn_Primop_e_struct*)_tmp32B;if(_tmp32C->tag != 3)goto _LL182;
else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;if(_tmp32E == 0)goto _LL182;_tmp32F=*
_tmp32E;_tmp330=(struct Cyc_Absyn_Exp*)_tmp32F.hd;}}_LL181:{void*_tmp331=_tmp330->r;
void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*
_tmp341;_LL185: {struct Cyc_Absyn_Var_e_struct*_tmp332=(struct Cyc_Absyn_Var_e_struct*)
_tmp331;if(_tmp332->tag != 1)goto _LL187;else{_tmp333=(void*)_tmp332->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp334=(struct Cyc_Absyn_Global_b_struct*)_tmp333;if(
_tmp334->tag != 1)goto _LL187;else{_tmp335=_tmp334->f1;}};}}_LL186: _tmp339=_tmp335;
goto _LL188;_LL187: {struct Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)
_tmp331;if(_tmp336->tag != 1)goto _LL189;else{_tmp337=(void*)_tmp336->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp338=(struct Cyc_Absyn_Local_b_struct*)_tmp337;if(
_tmp338->tag != 4)goto _LL189;else{_tmp339=_tmp338->f1;}};}}_LL188: _tmp33D=_tmp339;
goto _LL18A;_LL189: {struct Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)
_tmp331;if(_tmp33A->tag != 1)goto _LL18B;else{_tmp33B=(void*)_tmp33A->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp33C=(struct Cyc_Absyn_Param_b_struct*)_tmp33B;if(
_tmp33C->tag != 3)goto _LL18B;else{_tmp33D=_tmp33C->f1;}};}}_LL18A: _tmp341=_tmp33D;
goto _LL18C;_LL18B: {struct Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)
_tmp331;if(_tmp33E->tag != 1)goto _LL18D;else{_tmp33F=(void*)_tmp33E->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp340=(struct Cyc_Absyn_Pat_b_struct*)_tmp33F;if(_tmp340->tag
!= 5)goto _LL18D;else{_tmp341=_tmp340->f1;}};}}_LL18C: if(_tmp341->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5BB;struct Cyc_List_List*_tmp5BA;return(_tmp5BA=
_region_malloc(rgn,sizeof(*_tmp5BA)),((_tmp5BA->hd=((_tmp5BB=_region_malloc(rgn,
sizeof(*_tmp5BB)),((_tmp5BB->vd=v,((_tmp5BB->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp341),_tmp5BB)))))),((_tmp5BA->tl=r,_tmp5BA)))));};_LL18D:;_LL18E: goto _LL184;
_LL184:;}goto _LL17F;_LL182:;_LL183: goto _LL17F;_LL17F:;}goto _LL174;_LL17B: {
struct Cyc_Absyn_Primop_e_struct*_tmp324=(struct Cyc_Absyn_Primop_e_struct*)
_tmp316;if(_tmp324->tag != 3)goto _LL17D;else{_tmp325=_tmp324->f1;_tmp326=_tmp324->f2;
if(_tmp326 == 0)goto _LL17D;_tmp327=*_tmp326;_tmp328=(struct Cyc_Absyn_Exp*)_tmp327.hd;}}
_LL17C:{void*_tmp344=_tmp328->r;void*_tmp346;struct Cyc_Absyn_Vardecl*_tmp348;
void*_tmp34A;struct Cyc_Absyn_Vardecl*_tmp34C;void*_tmp34E;struct Cyc_Absyn_Vardecl*
_tmp350;void*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;_LL190: {struct Cyc_Absyn_Var_e_struct*
_tmp345=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp345->tag != 1)goto _LL192;
else{_tmp346=(void*)_tmp345->f2;{struct Cyc_Absyn_Global_b_struct*_tmp347=(struct
Cyc_Absyn_Global_b_struct*)_tmp346;if(_tmp347->tag != 1)goto _LL192;else{_tmp348=
_tmp347->f1;}};}}_LL191: _tmp34C=_tmp348;goto _LL193;_LL192: {struct Cyc_Absyn_Var_e_struct*
_tmp349=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp349->tag != 1)goto _LL194;
else{_tmp34A=(void*)_tmp349->f2;{struct Cyc_Absyn_Local_b_struct*_tmp34B=(struct
Cyc_Absyn_Local_b_struct*)_tmp34A;if(_tmp34B->tag != 4)goto _LL194;else{_tmp34C=
_tmp34B->f1;}};}}_LL193: _tmp350=_tmp34C;goto _LL195;_LL194: {struct Cyc_Absyn_Var_e_struct*
_tmp34D=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp34D->tag != 1)goto _LL196;
else{_tmp34E=(void*)_tmp34D->f2;{struct Cyc_Absyn_Param_b_struct*_tmp34F=(struct
Cyc_Absyn_Param_b_struct*)_tmp34E;if(_tmp34F->tag != 3)goto _LL196;else{_tmp350=
_tmp34F->f1;}};}}_LL195: _tmp354=_tmp350;goto _LL197;_LL196: {struct Cyc_Absyn_Var_e_struct*
_tmp351=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp351->tag != 1)goto _LL198;
else{_tmp352=(void*)_tmp351->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp353=(struct Cyc_Absyn_Pat_b_struct*)
_tmp352;if(_tmp353->tag != 5)goto _LL198;else{_tmp354=_tmp353->f1;}};}}_LL197: if(
_tmp354->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp5BE;struct Cyc_List_List*
_tmp5BD;return(_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD)),((_tmp5BD->hd=((
_tmp5BE=_region_malloc(rgn,sizeof(*_tmp5BE)),((_tmp5BE->vd=v,((_tmp5BE->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp354),_tmp5BE)))))),((_tmp5BD->tl=r,_tmp5BD)))));};_LL198:;_LL199: goto _LL18F;
_LL18F:;}goto _LL174;_LL17D:;_LL17E: goto _LL174;_LL174:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp357=e1->r;void*_tmp359;struct Cyc_Absyn_Vardecl*_tmp35B;void*
_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35F;void*_tmp361;struct Cyc_Absyn_Vardecl*
_tmp363;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp367;_LL19B: {struct Cyc_Absyn_Var_e_struct*
_tmp358=(struct Cyc_Absyn_Var_e_struct*)_tmp357;if(_tmp358->tag != 1)goto _LL19D;
else{_tmp359=(void*)_tmp358->f2;{struct Cyc_Absyn_Global_b_struct*_tmp35A=(struct
Cyc_Absyn_Global_b_struct*)_tmp359;if(_tmp35A->tag != 1)goto _LL19D;else{_tmp35B=
_tmp35A->f1;}};}}_LL19C: _tmp35F=_tmp35B;goto _LL19E;_LL19D: {struct Cyc_Absyn_Var_e_struct*
_tmp35C=(struct Cyc_Absyn_Var_e_struct*)_tmp357;if(_tmp35C->tag != 1)goto _LL19F;
else{_tmp35D=(void*)_tmp35C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp35E=(struct
Cyc_Absyn_Param_b_struct*)_tmp35D;if(_tmp35E->tag != 3)goto _LL19F;else{_tmp35F=
_tmp35E->f1;}};}}_LL19E: _tmp363=_tmp35F;goto _LL1A0;_LL19F: {struct Cyc_Absyn_Var_e_struct*
_tmp360=(struct Cyc_Absyn_Var_e_struct*)_tmp357;if(_tmp360->tag != 1)goto _LL1A1;
else{_tmp361=(void*)_tmp360->f2;{struct Cyc_Absyn_Local_b_struct*_tmp362=(struct
Cyc_Absyn_Local_b_struct*)_tmp361;if(_tmp362->tag != 4)goto _LL1A1;else{_tmp363=
_tmp362->f1;}};}}_LL1A0: _tmp367=_tmp363;goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Var_e_struct*
_tmp364=(struct Cyc_Absyn_Var_e_struct*)_tmp357;if(_tmp364->tag != 1)goto _LL1A3;
else{_tmp365=(void*)_tmp364->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp366=(struct Cyc_Absyn_Pat_b_struct*)
_tmp365;if(_tmp366->tag != 5)goto _LL1A3;else{_tmp367=_tmp366->f1;}};}}_LL1A2: if(!
_tmp367->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp367,e2);goto
_LL19A;_LL1A3:;_LL1A4: goto _LL19A;_LL19A:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp5C2;void*_tmp5C1[1];struct Cyc_String_pa_struct _tmp5C0;(_tmp5C0.tag=
0,((_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp5C1[0]=& _tmp5C0,Cyc_fprintf(Cyc_stderr,((_tmp5C2="%s",
_tag_dyneither(_tmp5C2,sizeof(char),3))),_tag_dyneither(_tmp5C1,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp36B=r->rop;unsigned int _tmp36C;struct _tuple10
_tmp36D;struct Cyc_Absyn_Vardecl*_tmp36E;struct Cyc_Absyn_Vardecl*_tmp36F;
unsigned int _tmp370;unsigned int _tmp371;struct Cyc_Absyn_Vardecl*_tmp372;_LL1A6:
if((_tmp36B.EqualConst).tag != 1)goto _LL1A8;_tmp36C=(unsigned int)(_tmp36B.EqualConst).val;
_LL1A7:{const char*_tmp5C6;void*_tmp5C5[1];struct Cyc_Int_pa_struct _tmp5C4;(
_tmp5C4.tag=1,((_tmp5C4.f1=(unsigned long)((int)_tmp36C),((_tmp5C5[0]=& _tmp5C4,
Cyc_fprintf(Cyc_stderr,((_tmp5C6="==%d",_tag_dyneither(_tmp5C6,sizeof(char),5))),
_tag_dyneither(_tmp5C5,sizeof(void*),1)))))));}goto _LL1A5;_LL1A8: if((_tmp36B.LessVar).tag
!= 2)goto _LL1AA;_tmp36D=(struct _tuple10)(_tmp36B.LessVar).val;_tmp36E=_tmp36D.f1;
_LL1A9:{const char*_tmp5CA;void*_tmp5C9[1];struct Cyc_String_pa_struct _tmp5C8;(
_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp36E->name)),((_tmp5C9[0]=& _tmp5C8,Cyc_fprintf(Cyc_stderr,((_tmp5CA="<%s",
_tag_dyneither(_tmp5CA,sizeof(char),4))),_tag_dyneither(_tmp5C9,sizeof(void*),1)))))));}
goto _LL1A5;_LL1AA: if((_tmp36B.LessNumelts).tag != 3)goto _LL1AC;_tmp36F=(struct Cyc_Absyn_Vardecl*)(
_tmp36B.LessNumelts).val;_LL1AB:{const char*_tmp5CE;void*_tmp5CD[1];struct Cyc_String_pa_struct
_tmp5CC;(_tmp5CC.tag=0,((_tmp5CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp36F->name)),((_tmp5CD[0]=& _tmp5CC,Cyc_fprintf(Cyc_stderr,((
_tmp5CE="<numelts(%s)",_tag_dyneither(_tmp5CE,sizeof(char),13))),_tag_dyneither(
_tmp5CD,sizeof(void*),1)))))));}goto _LL1A5;_LL1AC: if((_tmp36B.LessConst).tag != 4)
goto _LL1AE;_tmp370=(unsigned int)(_tmp36B.LessConst).val;_LL1AD:{const char*
_tmp5D2;void*_tmp5D1[1];struct Cyc_Int_pa_struct _tmp5D0;(_tmp5D0.tag=1,((_tmp5D0.f1=(
unsigned long)((int)_tmp370),((_tmp5D1[0]=& _tmp5D0,Cyc_fprintf(Cyc_stderr,((
_tmp5D2="<%d",_tag_dyneither(_tmp5D2,sizeof(char),4))),_tag_dyneither(_tmp5D1,
sizeof(void*),1)))))));}goto _LL1A5;_LL1AE: if((_tmp36B.LessEqConst).tag != 6)goto
_LL1B0;_tmp371=(unsigned int)(_tmp36B.LessEqConst).val;_LL1AF:{const char*_tmp5D6;
void*_tmp5D5[1];struct Cyc_Int_pa_struct _tmp5D4;(_tmp5D4.tag=1,((_tmp5D4.f1=(
unsigned long)((int)_tmp371),((_tmp5D5[0]=& _tmp5D4,Cyc_fprintf(Cyc_stderr,((
_tmp5D6="<=%d",_tag_dyneither(_tmp5D6,sizeof(char),5))),_tag_dyneither(_tmp5D5,
sizeof(void*),1)))))));}goto _LL1A5;_LL1B0: if((_tmp36B.LessEqNumelts).tag != 5)
goto _LL1A5;_tmp372=(struct Cyc_Absyn_Vardecl*)(_tmp36B.LessEqNumelts).val;_LL1B1:{
const char*_tmp5DA;void*_tmp5D9[1];struct Cyc_String_pa_struct _tmp5D8;(_tmp5D8.tag=
0,((_tmp5D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp372->name)),((_tmp5D9[0]=& _tmp5D8,Cyc_fprintf(Cyc_stderr,((_tmp5DA="<=numelts(%s)",
_tag_dyneither(_tmp5DA,sizeof(char),14))),_tag_dyneither(_tmp5D9,sizeof(void*),1)))))));}
goto _LL1A5;_LL1A5:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp5DD;void*_tmp5DC;(
_tmp5DC=0,Cyc_fprintf(Cyc_stderr,((_tmp5DD=",",_tag_dyneither(_tmp5DD,sizeof(
char),2))),_tag_dyneither(_tmp5DC,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1B2:{const char*_tmp5E0;void*_tmp5DF;(
_tmp5DF=0,Cyc_fprintf(Cyc_stderr,((_tmp5E0="uninitialized",_tag_dyneither(
_tmp5E0,sizeof(char),14))),_tag_dyneither(_tmp5DF,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1B3:{const char*_tmp5E3;void*_tmp5E2;(_tmp5E2=0,Cyc_fprintf(
Cyc_stderr,((_tmp5E3="this-initialized",_tag_dyneither(_tmp5E3,sizeof(char),17))),
_tag_dyneither(_tmp5E2,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1B4:{
const char*_tmp5E6;void*_tmp5E5;(_tmp5E5=0,Cyc_fprintf(Cyc_stderr,((_tmp5E6="all-initialized",
_tag_dyneither(_tmp5E6,sizeof(char),16))),_tag_dyneither(_tmp5E5,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp38D=root;struct Cyc_Absyn_Vardecl*_tmp38F;struct Cyc_Absyn_Exp*
_tmp391;void*_tmp392;_LL1B7: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp38E=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp38D;if(_tmp38E->tag != 0)goto _LL1B9;
else{_tmp38F=_tmp38E->f1;}}_LL1B8:{const char*_tmp5EA;void*_tmp5E9[1];struct Cyc_String_pa_struct
_tmp5E8;(_tmp5E8.tag=0,((_tmp5E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp38F->name)),((_tmp5E9[0]=& _tmp5E8,Cyc_fprintf(Cyc_stderr,((
_tmp5EA="Root(%s)",_tag_dyneither(_tmp5EA,sizeof(char),9))),_tag_dyneither(
_tmp5E9,sizeof(void*),1)))))));}goto _LL1B6;_LL1B9: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp390=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp38D;if(_tmp390->tag != 1)goto
_LL1BB;else{_tmp391=_tmp390->f1;_tmp392=(void*)_tmp390->f2;}}_LL1BA:{const char*
_tmp5EF;void*_tmp5EE[2];struct Cyc_String_pa_struct _tmp5ED;struct Cyc_String_pa_struct
_tmp5EC;(_tmp5EC.tag=0,((_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp392)),((_tmp5ED.tag=0,((_tmp5ED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp391)),((_tmp5EE[
0]=& _tmp5ED,((_tmp5EE[1]=& _tmp5EC,Cyc_fprintf(Cyc_stderr,((_tmp5EF="MallocPt(%s,%s)",
_tag_dyneither(_tmp5EF,sizeof(char),16))),_tag_dyneither(_tmp5EE,sizeof(void*),2)))))))))))));}
goto _LL1B6;_LL1BB: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp393=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp38D;if(_tmp393->tag != 2)goto _LL1B6;}_LL1BC:{const char*_tmp5F2;void*_tmp5F1;(
_tmp5F1=0,Cyc_fprintf(Cyc_stderr,((_tmp5F2="InitParam(_,_)",_tag_dyneither(
_tmp5F2,sizeof(char),15))),_tag_dyneither(_tmp5F1,sizeof(void*),0)));}goto _LL1B6;
_LL1B6:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*
_tmp39D=p->fields;while(_tmp39D != 0){{const char*_tmp5F6;void*_tmp5F5[1];struct
Cyc_Int_pa_struct _tmp5F4;(_tmp5F4.tag=1,((_tmp5F4.f1=(unsigned long)((int)
_tmp39D->hd),((_tmp5F5[0]=& _tmp5F4,Cyc_fprintf(Cyc_stderr,((_tmp5F6=".%d",
_tag_dyneither(_tmp5F6,sizeof(char),4))),_tag_dyneither(_tmp5F5,sizeof(void*),1)))))));}
_tmp39D=_tmp39D->tl;}{const char*_tmp5F9;void*_tmp5F8;(_tmp5F8=0,Cyc_fprintf(Cyc_stderr,((
_tmp5F9=" ",_tag_dyneither(_tmp5F9,sizeof(char),2))),_tag_dyneither(_tmp5F8,
sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*
pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp3A3=1;while(x != 0){if(!_tmp3A3){{const char*_tmp5FC;void*_tmp5FB;(_tmp5FB=
0,Cyc_fprintf(Cyc_stderr,((_tmp5FC=", ",_tag_dyneither(_tmp5FC,sizeof(char),3))),
_tag_dyneither(_tmp5FB,sizeof(void*),0)));}_tmp3A3=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,Cyc_fprintf(Cyc_stderr,((_tmp5FF="\n",
_tag_dyneither(_tmp5FF,sizeof(char),2))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3A8=_new_region("r");
struct _RegionHandle*r=& _tmp3A8;_push_region(r);{struct _handler_cons _tmp3A9;
_push_handler(& _tmp3A9);{int _tmp3AB=0;if(setjmp(_tmp3A9.handler))_tmp3AB=1;if(!
_tmp3AB){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3AC=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_fprintf(Cyc_stderr,((
_tmp602="{ ",_tag_dyneither(_tmp602,sizeof(char),3))),_tag_dyneither(_tmp601,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3AC)){void*_tmp3AF=_tmp3AC.f1;if(!first){{const char*_tmp605;void*
_tmp604;(_tmp604=0,Cyc_fprintf(Cyc_stderr,((_tmp605=", ",_tag_dyneither(_tmp605,
sizeof(char),3))),_tag_dyneither(_tmp604,sizeof(void*),0)));}first=0;}pr(_tmp3AF);}{
const char*_tmp608;void*_tmp607;(_tmp607=0,Cyc_fprintf(Cyc_stderr,((_tmp608="}\n",
_tag_dyneither(_tmp608,sizeof(char),3))),_tag_dyneither(_tmp607,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3AA=(void*)_exn_thrown;void*_tmp3B5=_tmp3AA;_LL1BE:;
_LL1BF:{const char*_tmp60B;void*_tmp60A;(_tmp60A=0,Cyc_fprintf(Cyc_stderr,((
_tmp60B="{ }\n",_tag_dyneither(_tmp60B,sizeof(char),5))),_tag_dyneither(_tmp60A,
sizeof(void*),0)));}_npop_handler(0);return;_LL1C0:;_LL1C1:(void)_throw(_tmp3B5);
_LL1BD:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp3B8=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3BD;enum Cyc_CfFlowInfo_InitLevel
_tmp3BF;struct Cyc_CfFlowInfo_Place*_tmp3C1;int _tmp3C3;struct _dyneither_ptr
_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C6;int _tmp3C7;void*_tmp3C8;_LL1C3: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3B9=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3B8;if(_tmp3B9->tag != 0)goto
_LL1C5;}_LL1C4:{const char*_tmp60E;void*_tmp60D;(_tmp60D=0,Cyc_fprintf(Cyc_stderr,((
_tmp60E="Zero",_tag_dyneither(_tmp60E,sizeof(char),5))),_tag_dyneither(_tmp60D,
sizeof(void*),0)));}goto _LL1C2;_LL1C5: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3BA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3B8;if(_tmp3BA->tag != 1)
goto _LL1C7;}_LL1C6:{const char*_tmp611;void*_tmp610;(_tmp610=0,Cyc_fprintf(Cyc_stderr,((
_tmp611="NotZeroAll",_tag_dyneither(_tmp611,sizeof(char),11))),_tag_dyneither(
_tmp610,sizeof(void*),0)));}goto _LL1C2;_LL1C7: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3BB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3B8;if(_tmp3BB->tag != 2)
goto _LL1C9;}_LL1C8:{const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_fprintf(Cyc_stderr,((
_tmp614="NotZeroThis",_tag_dyneither(_tmp614,sizeof(char),12))),_tag_dyneither(
_tmp613,sizeof(void*),0)));}goto _LL1C2;_LL1C9: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3BC=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3B8;if(_tmp3BC->tag != 3)goto
_LL1CB;else{_tmp3BD=_tmp3BC->f1;}}_LL1CA:{const char*_tmp617;void*_tmp616;(
_tmp616=0,Cyc_fprintf(Cyc_stderr,((_tmp617="Unknown(",_tag_dyneither(_tmp617,
sizeof(char),9))),_tag_dyneither(_tmp616,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3BD);{const char*_tmp61A;void*_tmp619;(_tmp619=0,Cyc_fprintf(Cyc_stderr,((
_tmp61A=")",_tag_dyneither(_tmp61A,sizeof(char),2))),_tag_dyneither(_tmp619,
sizeof(void*),0)));}goto _LL1C2;_LL1CB: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3BE=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3B8;if(_tmp3BE->tag != 4)goto _LL1CD;else{
_tmp3BF=_tmp3BE->f1;}}_LL1CC:{const char*_tmp61D;void*_tmp61C;(_tmp61C=0,Cyc_fprintf(
Cyc_stderr,((_tmp61D="Esc(",_tag_dyneither(_tmp61D,sizeof(char),5))),
_tag_dyneither(_tmp61C,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3BF);{const char*_tmp620;void*_tmp61F;(_tmp61F=0,Cyc_fprintf(Cyc_stderr,((
_tmp620=")",_tag_dyneither(_tmp620,sizeof(char),2))),_tag_dyneither(_tmp61F,
sizeof(void*),0)));}goto _LL1C2;_LL1CD: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3C0=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3B8;if(_tmp3C0->tag != 5)goto
_LL1CF;else{_tmp3C1=_tmp3C0->f1;}}_LL1CE:{const char*_tmp623;void*_tmp622;(
_tmp622=0,Cyc_fprintf(Cyc_stderr,((_tmp623="AddrOf(",_tag_dyneither(_tmp623,
sizeof(char),8))),_tag_dyneither(_tmp622,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3C1);{const char*_tmp626;void*_tmp625;(_tmp625=0,Cyc_fprintf(Cyc_stderr,((
_tmp626=")",_tag_dyneither(_tmp626,sizeof(char),2))),_tag_dyneither(_tmp625,
sizeof(void*),0)));}goto _LL1C2;_LL1CF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3C2=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3B8;if(_tmp3C2->tag != 6)goto
_LL1D1;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL1D0: if(_tmp3C3){const
char*_tmp629;void*_tmp628;(_tmp628=0,Cyc_fprintf(Cyc_stderr,((_tmp629="AggrUnion{",
_tag_dyneither(_tmp629,sizeof(char),11))),_tag_dyneither(_tmp628,sizeof(void*),0)));}
else{const char*_tmp62C;void*_tmp62B;(_tmp62B=0,Cyc_fprintf(Cyc_stderr,((_tmp62C="AggrStruct{",
_tag_dyneither(_tmp62C,sizeof(char),12))),_tag_dyneither(_tmp62B,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp3C4,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3C4.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3C4,sizeof(void*))){
const char*_tmp62F;void*_tmp62E;(_tmp62E=0,Cyc_fprintf(Cyc_stderr,((_tmp62F=",",
_tag_dyneither(_tmp62F,sizeof(char),2))),_tag_dyneither(_tmp62E,sizeof(void*),0)));}}}{
const char*_tmp632;void*_tmp631;(_tmp631=0,Cyc_fprintf(Cyc_stderr,((_tmp632="}",
_tag_dyneither(_tmp632,sizeof(char),2))),_tag_dyneither(_tmp631,sizeof(void*),0)));}
goto _LL1C2;_LL1D1: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3C5=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3B8;if(_tmp3C5->tag != 7)goto _LL1C2;else{_tmp3C6=_tmp3C5->f1;_tmp3C7=_tmp3C5->f2;
_tmp3C8=(void*)_tmp3C5->f3;}}_LL1D2:{const char*_tmp637;void*_tmp636[2];struct Cyc_String_pa_struct
_tmp635;struct Cyc_Int_pa_struct _tmp634;(_tmp634.tag=1,((_tmp634.f1=(
unsigned long)_tmp3C7,((_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3C6)),((_tmp636[0]=& _tmp635,((
_tmp636[1]=& _tmp634,Cyc_fprintf(Cyc_stderr,((_tmp637="[Consumed(%s,%d,",
_tag_dyneither(_tmp637,sizeof(char),17))),_tag_dyneither(_tmp636,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3C8);{const char*_tmp63A;void*_tmp639;(_tmp639=0,
Cyc_fprintf(Cyc_stderr,((_tmp63A="]",_tag_dyneither(_tmp63A,sizeof(char),2))),
_tag_dyneither(_tmp639,sizeof(void*),0)));};_LL1C2:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval){{const char*_tmp63D;void*_tmp63C;(_tmp63C=0,Cyc_fprintf(Cyc_stderr,((
_tmp63D="    ",_tag_dyneither(_tmp63D,sizeof(char),5))),_tag_dyneither(_tmp63C,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp640;void*
_tmp63F;(_tmp63F=0,Cyc_fprintf(Cyc_stderr,((_tmp640=" --> ",_tag_dyneither(
_tmp640,sizeof(char),6))),_tag_dyneither(_tmp63F,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp643;void*_tmp642;(_tmp642=0,Cyc_fprintf(Cyc_stderr,((
_tmp643="\n",_tag_dyneither(_tmp643,sizeof(char),2))),_tag_dyneither(_tmp642,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((void(*)(void(*f)(void*,
void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp3EF=f;int _tmp3F0;
struct _tuple11 _tmp3F1;struct Cyc_Dict_Dict _tmp3F2;_LL1D4: if((_tmp3EF.BottomFL).tag
!= 1)goto _LL1D6;_tmp3F0=(int)(_tmp3EF.BottomFL).val;_LL1D5:{const char*_tmp646;
void*_tmp645;(_tmp645=0,Cyc_fprintf(Cyc_stderr,((_tmp646="  BottomFL\n",
_tag_dyneither(_tmp646,sizeof(char),12))),_tag_dyneither(_tmp645,sizeof(void*),0)));}
goto _LL1D3;_LL1D6: if((_tmp3EF.ReachableFL).tag != 2)goto _LL1D3;_tmp3F1=(struct
_tuple11)(_tmp3EF.ReachableFL).val;_tmp3F2=_tmp3F1.f1;_LL1D7:{const char*_tmp649;
void*_tmp648;(_tmp648=0,Cyc_fprintf(Cyc_stderr,((_tmp649="  ReachableFL:\n",
_tag_dyneither(_tmp649,sizeof(char),16))),_tag_dyneither(_tmp648,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp3F2);{const char*_tmp64C;void*_tmp64B;(_tmp64B=
0,Cyc_fprintf(Cyc_stderr,((_tmp64C="\n",_tag_dyneither(_tmp64C,sizeof(char),2))),
_tag_dyneither(_tmp64B,sizeof(void*),0)));}goto _LL1D3;_LL1D3:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp3F9=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp405;struct Cyc_Absyn_DatatypeInfo
_tmp407;struct Cyc_List_List*_tmp408;struct Cyc_List_List*_tmp40A;struct Cyc_Absyn_AggrInfo
_tmp40C;struct Cyc_List_List*_tmp40D;struct Cyc_Absyn_DatatypeFieldInfo _tmp40F;
struct Cyc_List_List*_tmp410;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_PtrInfo
_tmp414;void*_tmp415;struct Cyc_Absyn_PtrAtts _tmp416;void*_tmp417;struct Cyc_List_List*
_tmp41A;struct Cyc_List_List*_tmp41C;struct Cyc_Absyn_ArrayInfo _tmp41E;void*
_tmp41F;void*_tmp421;void*_tmp423;void*_tmp425;void*_tmp426;void*_tmp428;_LL1D9: {
struct Cyc_Absyn_VoidType_struct*_tmp3FA=(struct Cyc_Absyn_VoidType_struct*)
_tmp3F9;if(_tmp3FA->tag != 0)goto _LL1DB;}_LL1DA: goto _LL1DC;_LL1DB: {struct Cyc_Absyn_IntType_struct*
_tmp3FB=(struct Cyc_Absyn_IntType_struct*)_tmp3F9;if(_tmp3FB->tag != 6)goto _LL1DD;}
_LL1DC: goto _LL1DE;_LL1DD: {struct Cyc_Absyn_FloatType_struct*_tmp3FC=(struct Cyc_Absyn_FloatType_struct*)
_tmp3F9;if(_tmp3FC->tag != 7)goto _LL1DF;}_LL1DE: goto _LL1E0;_LL1DF: {struct Cyc_Absyn_DoubleType_struct*
_tmp3FD=(struct Cyc_Absyn_DoubleType_struct*)_tmp3F9;if(_tmp3FD->tag != 8)goto
_LL1E1;}_LL1E0: goto _LL1E2;_LL1E1: {struct Cyc_Absyn_EnumType_struct*_tmp3FE=(
struct Cyc_Absyn_EnumType_struct*)_tmp3F9;if(_tmp3FE->tag != 14)goto _LL1E3;}_LL1E2:
goto _LL1E4;_LL1E3: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3FF=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3F9;if(_tmp3FF->tag != 15)goto _LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_Absyn_TagType_struct*
_tmp400=(struct Cyc_Absyn_TagType_struct*)_tmp3F9;if(_tmp400->tag != 20)goto _LL1E7;}
_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_Absyn_HeapRgn_struct*_tmp401=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp3F9;if(_tmp401->tag != 21)goto _LL1E9;}_LL1E8: goto _LL1EA;_LL1E9: {struct Cyc_Absyn_Evar_struct*
_tmp402=(struct Cyc_Absyn_Evar_struct*)_tmp3F9;if(_tmp402->tag != 1)goto _LL1EB;}
_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_UniqueRgn_struct*_tmp403=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp3F9;if(_tmp403->tag != 22)goto _LL1ED;}_LL1EC: {void*_tmp42A=rgn;_LL20E: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp42B=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp42A;if(_tmp42B->tag != 0)goto _LL210;}_LL20F: return 1;_LL210:;_LL211: return 0;
_LL20D:;}_LL1ED: {struct Cyc_Absyn_VarType_struct*_tmp404=(struct Cyc_Absyn_VarType_struct*)
_tmp3F9;if(_tmp404->tag != 2)goto _LL1EF;else{_tmp405=_tmp404->f1;}}_LL1EE: {void*
_tmp42C=rgn;struct Cyc_Absyn_Tvar*_tmp42F;_LL213: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp42D=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp42C;if(_tmp42D->tag != 0)
goto _LL215;}_LL214: return 0;_LL215: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp42E=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp42C;if(_tmp42E->tag != 1)goto _LL212;
else{_tmp42F=_tmp42E->f1;}}_LL216: return Cyc_Absyn_tvar_cmp(_tmp405,_tmp42F)== 0;
_LL212:;}_LL1EF: {struct Cyc_Absyn_DatatypeType_struct*_tmp406=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp3F9;if(_tmp406->tag != 3)goto _LL1F1;else{_tmp407=_tmp406->f1;_tmp408=_tmp407.targs;}}
_LL1F0: _tmp40A=_tmp408;goto _LL1F2;_LL1F1: {struct Cyc_Absyn_TypedefType_struct*
_tmp409=(struct Cyc_Absyn_TypedefType_struct*)_tmp3F9;if(_tmp409->tag != 18)goto
_LL1F3;else{_tmp40A=_tmp409->f2;}}_LL1F2: _tmp40D=_tmp40A;goto _LL1F4;_LL1F3: {
struct Cyc_Absyn_AggrType_struct*_tmp40B=(struct Cyc_Absyn_AggrType_struct*)
_tmp3F9;if(_tmp40B->tag != 12)goto _LL1F5;else{_tmp40C=_tmp40B->f1;_tmp40D=_tmp40C.targs;}}
_LL1F4: _tmp410=_tmp40D;goto _LL1F6;_LL1F5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp40E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3F9;if(_tmp40E->tag != 4)
goto _LL1F7;else{_tmp40F=_tmp40E->f1;_tmp410=_tmp40F.targs;}}_LL1F6: _tmp412=
_tmp410;goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_struct*_tmp411=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3F9;if(_tmp411->tag != 24)goto _LL1F9;else{_tmp412=_tmp411->f1;}}_LL1F8: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp412);_LL1F9: {struct Cyc_Absyn_PointerType_struct*
_tmp413=(struct Cyc_Absyn_PointerType_struct*)_tmp3F9;if(_tmp413->tag != 5)goto
_LL1FB;else{_tmp414=_tmp413->f1;_tmp415=_tmp414.elt_typ;_tmp416=_tmp414.ptr_atts;
_tmp417=_tmp416.rgn;}}_LL1FA: return Cyc_CfFlowInfo_contains_region(rgn,_tmp417)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp415);_LL1FB: {struct Cyc_Absyn_FnType_struct*
_tmp418=(struct Cyc_Absyn_FnType_struct*)_tmp3F9;if(_tmp418->tag != 10)goto _LL1FD;}
_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_TupleType_struct*_tmp419=(struct Cyc_Absyn_TupleType_struct*)
_tmp3F9;if(_tmp419->tag != 11)goto _LL1FF;else{_tmp41A=_tmp419->f1;}}_LL1FE: for(0;
_tmp41A != 0;_tmp41A=_tmp41A->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp41A->hd)).f2))return 1;}return 0;_LL1FF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp41B=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3F9;if(_tmp41B->tag != 13)goto
_LL201;else{_tmp41C=_tmp41B->f2;}}_LL200: for(0;_tmp41C != 0;_tmp41C=_tmp41C->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp41C->hd)->type))
return 1;}return 0;_LL201: {struct Cyc_Absyn_ArrayType_struct*_tmp41D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3F9;if(_tmp41D->tag != 9)goto _LL203;else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41E.elt_type;}}
_LL202: _tmp421=_tmp41F;goto _LL204;_LL203: {struct Cyc_Absyn_AccessEff_struct*
_tmp420=(struct Cyc_Absyn_AccessEff_struct*)_tmp3F9;if(_tmp420->tag != 23)goto
_LL205;else{_tmp421=(void*)_tmp420->f1;}}_LL204: _tmp423=_tmp421;goto _LL206;
_LL205: {struct Cyc_Absyn_RgnsEff_struct*_tmp422=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3F9;if(_tmp422->tag != 25)goto _LL207;else{_tmp423=(void*)_tmp422->f1;}}_LL206:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp423);_LL207: {struct Cyc_Absyn_DynRgnType_struct*
_tmp424=(struct Cyc_Absyn_DynRgnType_struct*)_tmp3F9;if(_tmp424->tag != 17)goto
_LL209;else{_tmp425=(void*)_tmp424->f1;_tmp426=(void*)_tmp424->f2;}}_LL208:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp425) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp426);_LL209: {struct Cyc_Absyn_RgnHandleType_struct*_tmp427=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3F9;if(_tmp427->tag != 16)goto _LL20B;else{_tmp428=(void*)_tmp427->f1;}}_LL20A:
return 0;_LL20B: {struct Cyc_Absyn_ValueofType_struct*_tmp429=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3F9;if(_tmp429->tag != 19)goto _LL1D8;}_LL20C: return 0;_LL1D8:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp431;struct Cyc_CfFlowInfo_FlowEnv*
_tmp432;struct Cyc_Dict_Dict _tmp433;struct Cyc_Dict_Dict*_tmp434;void*_tmp435;
struct _tuple23*_tmp430=env;_tmp431=*_tmp430;_tmp432=_tmp431.f1;_tmp433=_tmp431.f2;
_tmp434=(struct Cyc_Dict_Dict*)&(*_tmp430).f2;_tmp435=_tmp431.f3;{void*_tmp436=
root;struct Cyc_Absyn_Vardecl*_tmp438;void*_tmp43A;_LL218: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp437=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp436;if(_tmp437->tag != 0)goto
_LL21A;else{_tmp438=_tmp437->f1;}}_LL219: if(Cyc_CfFlowInfo_contains_region(
_tmp435,_tmp438->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp432,_tmp438->type,
_tmp432->unknown_none);*_tmp434=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp434,root,rval);goto _LL217;_LL21A: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp439=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp436;if(_tmp439->tag != 1)goto _LL21C;else{_tmp43A=(void*)_tmp439->f2;}}_LL21B:
if(!Cyc_CfFlowInfo_contains_region(_tmp435,_tmp43A))*_tmp434=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp434,root,rval);goto
_LL217;_LL21C: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp43B=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp436;if(_tmp43B->tag != 2)goto _LL217;}_LL21D: goto _LL217;_LL217:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp43C=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp43E;_LL21F: {struct Cyc_Absyn_VarType_struct*
_tmp43D=(struct Cyc_Absyn_VarType_struct*)_tmp43C;if(_tmp43D->tag != 2)goto _LL221;
else{_tmp43E=_tmp43D->f1;}}_LL220: {struct Cyc_CfFlowInfo_Region_k_struct _tmp64D;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp64D.tag=1,((_tmp64D.f1=_tmp43E,
_tmp64D)));void*v2=(void*)& v;struct _tuple23 _tmp64E;struct _tuple23 env=(_tmp64E.f1=
fenv,((_tmp64E.f2=fenv->mt_flowdict,((_tmp64E.f3=v2,_tmp64E)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL221: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp43F=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp43C;if(_tmp43F->tag != 22)goto
_LL223;}_LL222: {struct _tuple23 _tmp64F;struct _tuple23 env=(_tmp64F.f1=fenv,((
_tmp64F.f2=fenv->mt_flowdict,((_tmp64F.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp64F)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL223:;_LL224: {struct Cyc_Core_Impossible_struct _tmp655;const char*
_tmp654;struct Cyc_Core_Impossible_struct*_tmp653;(int)_throw((void*)((_tmp653=
_cycalloc(sizeof(*_tmp653)),((_tmp653[0]=((_tmp655.tag=Cyc_Core_Impossible,((
_tmp655.f1=((_tmp654="kill_flowdict_region",_tag_dyneither(_tmp654,sizeof(char),
21))),_tmp655)))),_tmp653)))));}_LL21E:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp446=
f;int _tmp447;struct _tuple11 _tmp448;struct Cyc_Dict_Dict _tmp449;struct Cyc_List_List*
_tmp44A;_LL226: if((_tmp446.BottomFL).tag != 1)goto _LL228;_tmp447=(int)(_tmp446.BottomFL).val;
_LL227: return f;_LL228: if((_tmp446.ReachableFL).tag != 2)goto _LL225;_tmp448=(
struct _tuple11)(_tmp446.ReachableFL).val;_tmp449=_tmp448.f1;_tmp44A=_tmp448.f2;
_LL229: {struct Cyc_Dict_Dict _tmp44B=Cyc_CfFlowInfo_kill_flowdict_region(fenv,
_tmp449,rgn);struct Cyc_List_List*_tmp44C=Cyc_CfFlowInfo_kill_relns_region(
_tmp44A,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp44B,_tmp44C);}_LL225:;}
