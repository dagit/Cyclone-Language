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
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_delete_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter(
void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};enum Cyc_CfFlowInfo_InitLevel{
Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,
void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop
cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[11]="HasTagCmps";struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct
Cyc_CfFlowInfo_IsZero_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*
f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*,struct Cyc_CfFlowInfo_ConsumeInfo);
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(
void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*
fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(void*rval);void
Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
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
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,
int);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2,int);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo f,int clear);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo
c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_IsZero_struct
Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val={0};union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal _tmp4BD;return((_tmp4BD.PlaceL).val=
x,(((_tmp4BD.PlaceL).tag=1,_tmp4BD)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp4BE;return((_tmp4BE.UnknownL).val=0,(((_tmp4BE.UnknownL).tag=2,_tmp4BE)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp4BF;return((_tmp4BF.BottomFL).val=
0,(((_tmp4BF.BottomFL).tag=1,_tmp4BF)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c){struct _tuple12 _tmp4C2;
union Cyc_CfFlowInfo_FlowInfo _tmp4C1;return((_tmp4C1.ReachableFL).val=((_tmp4C2.f1=
fd,((_tmp4C2.f2=r,((_tmp4C2.f3=c,_tmp4C2)))))),(((_tmp4C1.ReachableFL).tag=2,
_tmp4C1)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*
p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={
1,0}}};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4F9;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4F8;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4F7;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4F6;struct Cyc_CfFlowInfo_Esc_struct _tmp4F5;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4F4;struct Cyc_CfFlowInfo_Esc_struct _tmp4F3;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4F2;struct Cyc_CfFlowInfo_Esc_struct _tmp4F1;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4F0;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4EF;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4EE;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4ED;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4EC;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4EB;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4EA;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp4E9;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4E8;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp4E7;struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E6;struct Cyc_CfFlowInfo_Zero_struct _tmp4E5;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4E4;return(_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4->r=r,((
_tmp4E4->zero=(void*)((_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6)),((_tmp4E6[0]=((
_tmp4E5.tag=0,_tmp4E5)),_tmp4E6)))),((_tmp4E4->notzeroall=(void*)((_tmp4E8=
_region_malloc(r,sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4E7.tag=1,_tmp4E7)),
_tmp4E8)))),((_tmp4E4->notzerothis=(void*)((_tmp4EA=_region_malloc(r,sizeof(*
_tmp4EA)),((_tmp4EA[0]=((_tmp4E9.tag=2,_tmp4E9)),_tmp4EA)))),((_tmp4E4->unknown_none=(
void*)((_tmp4EC=_region_malloc(r,sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4EB.tag=3,((
_tmp4EB.f1=Cyc_CfFlowInfo_NoneIL,_tmp4EB)))),_tmp4EC)))),((_tmp4E4->unknown_this=(
void*)((_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE)),((_tmp4EE[0]=((_tmp4ED.tag=3,((
_tmp4ED.f1=Cyc_CfFlowInfo_ThisIL,_tmp4ED)))),_tmp4EE)))),((_tmp4E4->unknown_all=(
void*)((_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4EF.tag=3,((
_tmp4EF.f1=Cyc_CfFlowInfo_AllIL,_tmp4EF)))),_tmp4F0)))),((_tmp4E4->esc_none=(
void*)((_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2)),((_tmp4F2[0]=((_tmp4F1.tag=4,((
_tmp4F1.f1=Cyc_CfFlowInfo_NoneIL,_tmp4F1)))),_tmp4F2)))),((_tmp4E4->esc_this=(
void*)((_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F3.tag=4,((
_tmp4F3.f1=Cyc_CfFlowInfo_ThisIL,_tmp4F3)))),_tmp4F4)))),((_tmp4E4->esc_all=(
void*)((_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F5.tag=4,((
_tmp4F5.f1=Cyc_CfFlowInfo_AllIL,_tmp4F5)))),_tmp4F6)))),((_tmp4E4->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4E4->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4E4->dummy_place=((_tmp4F9=
_region_malloc(r,sizeof(*_tmp4F9)),((_tmp4F9->root=(void*)((_tmp4F7=
_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7[0]=((_tmp4F8.tag=1,((_tmp4F8.f1=&
dummy_exp,((_tmp4F8.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4F8)))))),
_tmp4F7)))),((_tmp4F9->fields=0,_tmp4F9)))))),_tmp4E4)))))))))))))))))))))))))));}
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
else{const char*_tmp4FD;void*_tmp4FC[1];struct Cyc_String_pa_struct _tmp4FB;return(
struct _dyneither_ptr)((_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23->name)),((_tmp4FC[0]=& _tmp4FB,Cyc_aprintf(((
_tmp4FD="reachable from %s",_tag_dyneither(_tmp4FD,sizeof(char),18))),
_tag_dyneither(_tmp4FC,sizeof(void*),1))))))));}_LL3:;_LL4: {const char*_tmp500;
void*_tmp4FF;(_tmp4FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp500="error locations not for VarRoots",_tag_dyneither(
_tmp500,sizeof(char),33))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}_LL0:;};}
struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place);struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc);static void Cyc_CfFlowInfo_unique_err(void*place,struct _dyneither_ptr(*
toString)(void*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc){struct
_dyneither_ptr _tmp29=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp2A=Cyc_Position_string_of_segment(
consumed_loc);void*_tmp503[2];struct Cyc_String_pa_struct _tmp502;struct Cyc_String_pa_struct
_tmp501;(_tmp501.tag=0,((_tmp501.f1=(struct _dyneither_ptr)_tmp2A,((_tmp502.tag=0,((
_tmp502.f1=(struct _dyneither_ptr)_tmp29,((_tmp503[0]=& _tmp502,((_tmp503[1]=&
_tmp501,Cyc_Tcutil_terr(loc,err_msg1,_tag_dyneither(_tmp503,sizeof(void*),2)))))))))))));}
else{void*_tmp505[1];struct Cyc_String_pa_struct _tmp504;(_tmp504.tag=0,((_tmp504.f1=(
struct _dyneither_ptr)_tmp29,((_tmp505[0]=& _tmp504,Cyc_Tcutil_terr(loc,err_msg2,
_tag_dyneither(_tmp505,sizeof(void*),1)))))));}}struct _tuple15{int f1;struct
_dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(
struct _tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2);static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(struct
_tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2){struct _tuple15 _tmp31;int _tmp32;struct _dyneither_ptr(*_tmp33)(void*);struct
_tuple15*_tmp30=env;_tmp31=*_tmp30;_tmp32=_tmp31.f1;_tmp33=_tmp31.f2;if(_tmp32){
const char*_tmp507;const char*_tmp506;Cyc_CfFlowInfo_unique_err(place,_tmp33,((
_tmp506="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp506,sizeof(char),59))),((_tmp507="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp507,sizeof(char),50))),loc1,loc2);}return loc1;}struct _tuple16{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple16 _tmp508;struct _tuple16 _tmp36=(
_tmp508.f1=disjoint,((_tmp508.f2=Cyc_CfFlowInfo_place_err_string,_tmp508)));
struct Cyc_Dict_Dict _tmp37=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*
f)(struct _tuple16*,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),struct _tuple16*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple16*env,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp36,s1,s2);return _tmp37;}struct
_tuple17{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*place);
};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple17 _tmp509;
struct _tuple17 _tmp39=(_tmp509.f1=disjoint,((_tmp509.f2=Cyc_CfFlowInfo_uplace_err_string,
_tmp509)));return((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct
_tuple17*,struct Cyc_CfFlowInfo_UniquePlace*,struct Cyc_Position_Segment*,struct
Cyc_Position_Segment*),struct _tuple17*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple17*env,
struct Cyc_CfFlowInfo_UniquePlace*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp39,s1,s2);}struct _tuple18{void*f1;
struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{struct _handler_cons
_tmp3B;_push_handler(& _tmp3B);{int _tmp3D=0;if(setjmp(_tmp3B.handler))_tmp3D=1;
if(!_tmp3D){{struct _RegionHandle _tmp3E=_new_region("temp");struct _RegionHandle*
temp=& _tmp3E;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple18 _tmp3F=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))
Cyc_Iter_next)(iter,& _tmp3F)){void*_tmp40=_tmp3F.f1;if(!((int(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_member)(s2,_tmp40)){int _tmp41=0;_npop_handler(1);return _tmp41;}}{
int _tmp42=1;_npop_handler(1);return _tmp42;};};_pop_region(temp);};_pop_handler();}
else{void*_tmp3C=(void*)_exn_thrown;void*_tmp44=_tmp3C;_LL6:;_LL7: return 1;_LL8:;
_LL9:(void)_throw(_tmp44);_LL5:;}};};}int Cyc_CfFlowInfo_place_set_equals(struct
Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(struct
Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){return Cyc_CfFlowInfo_place_set_subset(s1,
s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dyneither_ptr*_tmp45=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp45,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct
_tmp516;const char*_tmp515;void*_tmp514[1];struct Cyc_String_pa_struct _tmp513;
struct Cyc_Core_Impossible_struct*_tmp512;(int)_throw((void*)((_tmp512=_cycalloc(
sizeof(*_tmp512)),((_tmp512[0]=((_tmp516.tag=Cyc_Core_Impossible,((_tmp516.f1=(
struct _dyneither_ptr)((_tmp513.tag=0,((_tmp513.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp514[0]=& _tmp513,Cyc_aprintf(((_tmp515="get_field_index_fs failed: %s",
_tag_dyneither(_tmp515,sizeof(char),30))),_tag_dyneither(_tmp514,sizeof(void*),1)))))))),
_tmp516)))),_tmp512)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp4B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp4D;union Cyc_Absyn_AggrInfoU
_tmp4E;struct Cyc_List_List*_tmp50;_LLB: {struct Cyc_Absyn_AggrType_struct*_tmp4C=(
struct Cyc_Absyn_AggrType_struct*)_tmp4B;if(_tmp4C->tag != 12)goto _LLD;else{_tmp4D=
_tmp4C->f1;_tmp4E=_tmp4D.aggr_info;}}_LLC: {struct Cyc_List_List*_tmp51=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp4E))->impl))->fields;
_tmp50=_tmp51;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp4B;if(_tmp4F->tag != 13)goto _LLF;else{
_tmp50=_tmp4F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp50,f);_LLF:;
_LL10: {struct Cyc_Core_Impossible_struct _tmp523;const char*_tmp522;void*_tmp521[1];
struct Cyc_String_pa_struct _tmp520;struct Cyc_Core_Impossible_struct*_tmp51F;(int)
_throw((void*)((_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp523.tag=
Cyc_Core_Impossible,((_tmp523.f1=(struct _dyneither_ptr)((_tmp520.tag=0,((_tmp520.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp521[
0]=& _tmp520,Cyc_aprintf(((_tmp522="get_field_index failed: %s",_tag_dyneither(
_tmp522,sizeof(char),27))),_tag_dyneither(_tmp521,sizeof(void*),1)))))))),
_tmp523)))),_tmp51F)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp524;struct _tuple0 _tmp58=(_tmp524.f1=r1,((_tmp524.f2=r2,_tmp524)));
void*_tmp59;struct Cyc_Absyn_Vardecl*_tmp5B;void*_tmp5C;struct Cyc_Absyn_Vardecl*
_tmp5E;void*_tmp5F;void*_tmp61;void*_tmp63;struct Cyc_Absyn_Exp*_tmp65;void*
_tmp66;struct Cyc_Absyn_Exp*_tmp68;void*_tmp69;void*_tmp6B;void*_tmp6D;int _tmp6F;
void*_tmp70;int _tmp72;_LL12: _tmp59=_tmp58.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5A=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp59;if(_tmp5A->tag != 0)goto _LL14;
else{_tmp5B=_tmp5A->f1;}};_tmp5C=_tmp58.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5D=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp5C;if(_tmp5D->tag != 0)goto _LL14;
else{_tmp5E=_tmp5D->f1;}};_LL13: return(int)_tmp5B - (int)_tmp5E;_LL14: _tmp5F=
_tmp58.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp60=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5F;if(_tmp60->tag != 0)goto _LL16;};_LL15: return - 1;_LL16: _tmp61=_tmp58.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp62=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp61;if(_tmp62->tag != 0)goto _LL18;};_LL17: return 1;_LL18: _tmp63=_tmp58.f1;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp64=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp63;if(_tmp64->tag != 1)goto _LL1A;else{_tmp65=_tmp64->f1;}};_tmp66=_tmp58.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp67=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp66;if(_tmp67->tag != 1)goto _LL1A;else{_tmp68=_tmp67->f1;}};_LL19: return(int)
_tmp65 - (int)_tmp68;_LL1A: _tmp69=_tmp58.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6A=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp69;if(_tmp6A->tag != 1)goto
_LL1C;};_LL1B: return - 1;_LL1C: _tmp6B=_tmp58.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6C=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6B;if(_tmp6C->tag != 1)goto
_LL1E;};_LL1D: return 1;_LL1E: _tmp6D=_tmp58.f1;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp6E=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6D;if(_tmp6E->tag != 2)goto
_LL11;else{_tmp6F=_tmp6E->f1;}};_tmp70=_tmp58.f2;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp71=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp70;if(_tmp71->tag != 2)goto
_LL11;else{_tmp72=_tmp71->f1;}};_LL1F: return _tmp6F - _tmp72;_LL11:;};}static int
Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2);
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*
list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{void*_tmp74;struct Cyc_List_List*_tmp75;struct Cyc_CfFlowInfo_Place
_tmp73=*p1;_tmp74=_tmp73.root;_tmp75=_tmp73.fields;{void*_tmp77;struct Cyc_List_List*
_tmp78;struct Cyc_CfFlowInfo_Place _tmp76=*p2;_tmp77=_tmp76.root;_tmp78=_tmp76.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp74,_tmp77);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp75,_tmp78);};};};}int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*
p1,struct Cyc_CfFlowInfo_UniquePlace*p2);int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*
p1,struct Cyc_CfFlowInfo_UniquePlace*p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place
_tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_CfFlowInfo_UniquePlace _tmp79=*p1;
_tmp7A=_tmp79.place;_tmp7B=_tmp79.path;{struct Cyc_CfFlowInfo_Place _tmp7D;struct
Cyc_List_List*_tmp7E;struct Cyc_CfFlowInfo_UniquePlace _tmp7C=*p2;_tmp7D=_tmp7C.place;
_tmp7E=_tmp7C.path;{int i=Cyc_CfFlowInfo_place_cmp(& _tmp7A,& _tmp7D);if(i != 0)
return i;return((int(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,
_tmp7B,_tmp7E);};};};}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p);static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp80;struct Cyc_List_List*
_tmp81;struct Cyc_CfFlowInfo_Place _tmp7F=*p;_tmp80=_tmp7F.root;_tmp81=_tmp7F.fields;{
void*_tmp82=_tmp80;struct Cyc_Absyn_Vardecl*_tmp84;struct Cyc_Absyn_Exp*_tmp86;int
_tmp88;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp83=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp82;if(_tmp83->tag != 0)goto _LL23;else{_tmp84=_tmp83->f1;}}_LL22:{struct
_dyneither_ptr*_tmp531;const char*_tmp530;void*_tmp52F[1];struct Cyc_String_pa_struct
_tmp52E;struct Cyc_List_List*_tmp52D;sl=((_tmp52D=_cycalloc(sizeof(*_tmp52D)),((
_tmp52D->hd=((_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531[0]=((_tmp52E.tag=0,((
_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp84->name).f2),((
_tmp52F[0]=& _tmp52E,Cyc_aprintf(((_tmp530="%s",_tag_dyneither(_tmp530,sizeof(
char),3))),_tag_dyneither(_tmp52F,sizeof(void*),1)))))))),_tmp531)))),((_tmp52D->tl=
sl,_tmp52D))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp85=(
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp82;if(_tmp85->tag != 1)goto _LL25;else{
_tmp86=_tmp85->f1;}}_LL24:{struct _dyneither_ptr*_tmp53E;const char*_tmp53D;void*
_tmp53C[1];struct Cyc_Int_pa_struct _tmp53B;struct Cyc_List_List*_tmp53A;sl=((
_tmp53A=_cycalloc(sizeof(*_tmp53A)),((_tmp53A->hd=((_tmp53E=_cycalloc(sizeof(*
_tmp53E)),((_tmp53E[0]=((_tmp53B.tag=1,((_tmp53B.f1=(unsigned long)((int)_tmp86),((
_tmp53C[0]=& _tmp53B,Cyc_aprintf(((_tmp53D="mpt%d",_tag_dyneither(_tmp53D,sizeof(
char),6))),_tag_dyneither(_tmp53C,sizeof(void*),1)))))))),_tmp53E)))),((_tmp53A->tl=
sl,_tmp53A))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp87=(
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp82;if(_tmp87->tag != 2)goto _LL20;else{
_tmp88=_tmp87->f1;}}_LL26:{struct _dyneither_ptr*_tmp54B;const char*_tmp54A;void*
_tmp549[1];struct Cyc_Int_pa_struct _tmp548;struct Cyc_List_List*_tmp547;sl=((
_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=((_tmp54B=_cycalloc(sizeof(*
_tmp54B)),((_tmp54B[0]=((_tmp548.tag=1,((_tmp548.f1=(unsigned long)_tmp88,((
_tmp549[0]=& _tmp548,Cyc_aprintf(((_tmp54A="param%d",_tag_dyneither(_tmp54A,
sizeof(char),8))),_tag_dyneither(_tmp549,sizeof(void*),1)))))))),_tmp54B)))),((
_tmp547->tl=sl,_tmp547))))));}goto _LL20;_LL20:;}for(0;_tmp81 != 0;_tmp81=_tmp81->tl){
struct _dyneither_ptr*_tmp558;const char*_tmp557;void*_tmp556[1];struct Cyc_Int_pa_struct
_tmp555;struct Cyc_List_List*_tmp554;sl=((_tmp554=_cycalloc(sizeof(*_tmp554)),((
_tmp554->hd=((_tmp558=_cycalloc(sizeof(*_tmp558)),((_tmp558[0]=((_tmp555.tag=1,((
_tmp555.f1=(unsigned long)((int)_tmp81->hd),((_tmp556[0]=& _tmp555,Cyc_aprintf(((
_tmp557="%d",_tag_dyneither(_tmp557,sizeof(char),3))),_tag_dyneither(_tmp556,
sizeof(void*),1)))))))),_tmp558)))),((_tmp554->tl=sl,_tmp554))))));}{struct Cyc_String_pa_struct
_tmp563;const char*_tmp562;void*_tmp561[1];const char*_tmp560;struct _dyneither_ptr*
_tmp55F;struct _dyneither_ptr*_tmp9D=(_tmp55F=_cycalloc(sizeof(*_tmp55F)),((
_tmp55F[0]=((_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((_tmp562="",
_tag_dyneither(_tmp562,sizeof(char),1))),((_tmp561[0]=& _tmp563,Cyc_aprintf(((
_tmp560="%s",_tag_dyneither(_tmp560,sizeof(char),3))),_tag_dyneither(_tmp561,
sizeof(void*),1)))))))),_tmp55F)));for(0;sl != 0;sl=sl->tl){const char*_tmp568;
void*_tmp567[2];struct Cyc_String_pa_struct _tmp566;struct Cyc_String_pa_struct
_tmp565;*_tmp9D=((_tmp565.tag=0,((_tmp565.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp9D),((_tmp566.tag=0,((_tmp566.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp567[0]=& _tmp566,((
_tmp567[1]=& _tmp565,Cyc_aprintf(((_tmp568="%s.%s",_tag_dyneither(_tmp568,sizeof(
char),6))),_tag_dyneither(_tmp567,sizeof(void*),2))))))))))))));}return _tmp9D;};}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp56E(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp56D,unsigned int*
_tmp56C,void***_tmp56A){for(*_tmp56D=0;*_tmp56D < *_tmp56C;(*_tmp56D)++){(*
_tmp56A)[*_tmp56D]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp56D;unsigned int _tmp56C;struct _dyneither_ptr _tmp56B;void**
_tmp56A;unsigned int _tmp569;struct _dyneither_ptr d=(_tmp569=sz,((_tmp56A=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp569)),((_tmp56B=
_tag_dyneither(_tmp56A,sizeof(void*),_tmp569),((((_tmp56C=_tmp569,_tmp56E(& fenv,&
_tmp56D,& _tmp56C,& _tmp56A))),_tmp56B)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmpA8;struct _dyneither_ptr*_tmpA9;void*_tmpAA;struct
Cyc_Absyn_Aggrfield*_tmpA7=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmpA8=*_tmpA7;_tmpA9=_tmpA8.name;_tmpAA=_tmpA8.type;if(
_get_dyneither_size(*_tmpA9,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmpAA,leafval);}}return d;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;
};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
int allow_zeroterm,void*t,void*leafval);static void _tmp574(struct Cyc_CfFlowInfo_FlowEnv**
fenv,unsigned int*_tmp573,unsigned int*_tmp572,void***_tmp570){for(*_tmp573=0;*
_tmp573 < *_tmp572;(*_tmp573)++){(*_tmp570)[*_tmp573]=(*fenv)->unknown_all;}}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))
return fenv->unknown_all;{void*_tmpAF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpB1;union Cyc_Absyn_DatatypeFieldInfoU _tmpB2;struct _tuple2 _tmpB3;struct Cyc_Absyn_Datatypefield*
_tmpB4;struct Cyc_List_List*_tmpB6;struct Cyc_Absyn_AggrInfo _tmpB8;union Cyc_Absyn_AggrInfoU
_tmpB9;enum Cyc_Absyn_AggrKind _tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_Absyn_ArrayInfo
_tmpBE;void*_tmpBF;union Cyc_Absyn_Constraint*_tmpC0;void*_tmpC2;struct Cyc_Absyn_PtrInfo
_tmpC4;struct Cyc_Absyn_PtrAtts _tmpC5;union Cyc_Absyn_Constraint*_tmpC6;_LL28: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB0=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpAF;if(_tmpB0->tag != 4)goto _LL2A;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB1.field_info;
if((_tmpB2.KnownDatatypefield).tag != 2)goto _LL2A;_tmpB3=(struct _tuple2)(_tmpB2.KnownDatatypefield).val;
_tmpB4=_tmpB3.f2;}}_LL29: if(_tmpB4->typs == 0)return leafval;_tmpB6=_tmpB4->typs;
goto _LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpB5=(struct Cyc_Absyn_TupleType_struct*)
_tmpAF;if(_tmpB5->tag != 11)goto _LL2C;else{_tmpB6=_tmpB5->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB6);unsigned int _tmp573;unsigned int _tmp572;struct _dyneither_ptr _tmp571;void**
_tmp570;unsigned int _tmp56F;struct _dyneither_ptr d=(_tmp56F=sz,((_tmp570=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp56F)),((_tmp571=
_tag_dyneither(_tmp570,sizeof(void*),_tmp56F),((((_tmp572=_tmp56F,_tmp574(& fenv,&
_tmp573,& _tmp572,& _tmp570))),_tmp571)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmpB6))->hd)).f2,
leafval);_tmpB6=_tmpB6->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp577;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp576;return(void*)((_tmp576=
_region_malloc(fenv->r,sizeof(*_tmp576)),((_tmp576[0]=((_tmp577.tag=7,((_tmp577.f1=
0,((_tmp577.f2=d,_tmp577)))))),_tmp576))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpB7=(struct Cyc_Absyn_AggrType_struct*)_tmpAF;if(_tmpB7->tag != 12)goto _LL2E;
else{_tmpB8=_tmpB7->f1;_tmpB9=_tmpB8.aggr_info;}}_LL2D: {struct Cyc_Absyn_Aggrdecl*
_tmpCD=Cyc_Absyn_get_known_aggrdecl(_tmpB9);if(_tmpCD->impl == 0)goto _LL27;_tmpBB=
_tmpCD->kind;_tmpBC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD->impl))->fields;
goto _LL2F;}_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpBA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAF;if(_tmpBA->tag != 13)goto _LL30;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp57A;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp579;return(void*)((_tmp579=_region_malloc(fenv->r,sizeof(*_tmp579)),((
_tmp579[0]=((_tmp57A.tag=7,((_tmp57A.f1=_tmpBB == Cyc_Absyn_UnionA,((_tmp57A.f2=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpBC,leafval),_tmp57A)))))),_tmp579))));}
_LL30:{struct Cyc_Absyn_ArrayType_struct*_tmpBD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAF;if(_tmpBD->tag != 9)goto _LL32;else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBE.elt_type;
_tmpC0=_tmpBE.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpC0))goto _LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpBF)?
fenv->unknown_all: leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpC1=(struct
Cyc_Absyn_TagType_struct*)_tmpAF;if(_tmpC1->tag != 20)goto _LL34;else{_tmpC2=(void*)
_tmpC1->f1;}}_LL33: {void*_tmpD0=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpD2;
enum Cyc_CfFlowInfo_InitLevel _tmpD4;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpD1=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpD0;if(_tmpD1->tag != 3)goto
_LL3B;else{_tmpD2=_tmpD1->f1;if(_tmpD2 != Cyc_CfFlowInfo_AllIL)goto _LL3B;}}_LL3A:
goto _LL3C;_LL3B: {struct Cyc_CfFlowInfo_Esc_struct*_tmpD3=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmpD0;if(_tmpD3->tag != 4)goto _LL3D;else{_tmpD4=_tmpD3->f1;if(_tmpD4 != Cyc_CfFlowInfo_AllIL)
goto _LL3D;}}_LL3C: goto _LL3E;_LL3D: {struct Cyc_CfFlowInfo_Zero_struct*_tmpD5=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmpD0;if(_tmpD5->tag != 0)goto _LL3F;}_LL3E:
goto _LL40;_LL3F: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpD6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpD0;if(_tmpD6->tag != 1)goto _LL41;}_LL40: {struct Cyc_CfFlowInfo_TagCmps_struct
_tmp584;struct Cyc_CfFlowInfo_TagCmp*_tmp583;struct Cyc_CfFlowInfo_TagCmp*_tmp582[
1];struct Cyc_CfFlowInfo_TagCmps_struct*_tmp581;return(void*)((_tmp581=
_region_malloc(fenv->r,sizeof(*_tmp581)),((_tmp581[0]=((_tmp584.tag=6,((_tmp584.f1=((
_tmp582[0]=((_tmp583=_region_malloc(fenv->r,sizeof(*_tmp583)),((_tmp583->cmp=Cyc_Absyn_Eq,((
_tmp583->bd=_tmpC2,_tmp583)))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,_tag_dyneither(_tmp582,sizeof(
struct Cyc_CfFlowInfo_TagCmp*),1)))),_tmp584)))),_tmp581))));}_LL41:;_LL42: return
leafval;_LL38:;}_LL34:{struct Cyc_Absyn_PointerType_struct*_tmpC3=(struct Cyc_Absyn_PointerType_struct*)
_tmpAF;if(_tmpC3->tag != 5)goto _LL36;else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC4.ptr_atts;
_tmpC6=_tmpC5.nullable;}}if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpC6)))goto _LL36;_LL35:{void*_tmpDB=leafval;enum Cyc_CfFlowInfo_InitLevel
_tmpDD;enum Cyc_CfFlowInfo_InitLevel _tmpDF;_LL44: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpDC=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpDB;if(_tmpDC->tag != 3)goto
_LL46;else{_tmpDD=_tmpDC->f1;if(_tmpDD != Cyc_CfFlowInfo_ThisIL)goto _LL46;}}_LL45:
return fenv->notzerothis;_LL46: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpDE=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpDB;if(_tmpDE->tag != 3)goto _LL48;else{
_tmpDF=_tmpDE->f1;if(_tmpDF != Cyc_CfFlowInfo_AllIL)goto _LL48;}}_LL47: return fenv->notzeroall;
_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto _LL27;_LL27:;}return
Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}struct _tuple20{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmpE0=_new_region("r");struct
_RegionHandle*r=& _tmpE0;_push_region(r);{struct _tuple20 _tmp585;struct _tuple20
_tmpE1=(_tmp585.f1=place,((_tmp585.f2=0,_tmp585)));struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple20*))Cyc_Iter_next)(iter,&
_tmpE1)){struct Cyc_CfFlowInfo_Place*_tmpE2=_tmpE1.f1;void*_tmpE4;struct Cyc_List_List*
_tmpE5;struct Cyc_CfFlowInfo_Place _tmpE3=*place;_tmpE4=_tmpE3.root;_tmpE5=_tmpE3.fields;{
void*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_CfFlowInfo_Place _tmpE6=*_tmpE2;
_tmpE7=_tmpE6.root;_tmpE8=_tmpE6.fields;if(Cyc_CfFlowInfo_root_cmp(_tmpE4,_tmpE7)
!= 0)continue;for(0;_tmpE5 != 0  && _tmpE8 != 0;(_tmpE5=_tmpE5->tl,_tmpE8=_tmpE8->tl)){
if((int)_tmpE5->hd != (int)_tmpE8->hd)break;}if(_tmpE5 == 0){int _tmpE9=1;
_npop_handler(0);return _tmpE9;}};}{int _tmpEA=0;_npop_handler(0);return _tmpEA;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp586;pile->places=((_tmp586=_region_malloc(pile->rgn,sizeof(*_tmp586)),((
_tmp586->hd=place,((_tmp586->tl=pile->places,_tmp586))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r);static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmpED=r;struct Cyc_CfFlowInfo_Place*
_tmpEF;int _tmpF1;struct _dyneither_ptr _tmpF2;_LL4B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpEE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpED;if(_tmpEE->tag != 5)goto
_LL4D;else{_tmpEF=_tmpEE->f1;}}_LL4C: Cyc_CfFlowInfo_add_place(pile,_tmpEF);
return;_LL4D: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpED;if(_tmpF0->tag != 7)goto _LL4F;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}}
_LL4E:{int i=0;for(0;i < _get_dyneither_size(_tmpF2,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmpF2.curr)[i]);}}return;_LL4F:;_LL50: return;_LL4A:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp58C(struct _RegionHandle**r,void**new_val,struct
_dyneither_ptr*_tmpF6,unsigned int*_tmp58B,unsigned int*_tmp58A,void***_tmp588){
for(*_tmp58B=0;*_tmp58B < *_tmp58A;(*_tmp58B)++){(*_tmp588)[*_tmp58B]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmpF6).curr)[(int)*_tmp58B]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmpF3=old_val;int _tmpF5;
struct _dyneither_ptr _tmpF6;_LL52: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF4=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF3;if(_tmpF4->tag != 7)goto _LL54;else{
_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}_LL53: {unsigned int _tmp58B;unsigned int
_tmp58A;struct _dyneither_ptr _tmp589;void**_tmp588;unsigned int _tmp587;struct
_dyneither_ptr d2=(_tmp587=_get_dyneither_size(_tmpF6,sizeof(void*)),((_tmp588=(
void**)_region_malloc(r,_check_times(sizeof(void*),_tmp587)),((_tmp589=
_tag_dyneither(_tmp588,sizeof(void*),_tmp587),((((_tmp58A=_tmp587,_tmp58C(& r,&
new_val,& _tmpF6,& _tmp58B,& _tmp58A,& _tmp588))),_tmp589)))))));struct Cyc_CfFlowInfo_Aggregate_struct
_tmp58F;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp58E;return(void*)((_tmp58E=
_region_malloc(r,sizeof(*_tmp58E)),((_tmp58E[0]=((_tmp58F.tag=7,((_tmp58F.f1=
_tmpF5,((_tmp58F.f2=d2,_tmp58F)))))),_tmp58E))));}_LL54:;_LL55: return new_val;
_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp595(struct
_dyneither_ptr*d,unsigned int*_tmp594,unsigned int*_tmp593,void***_tmp591){for(*
_tmp594=0;*_tmp594 < *_tmp593;(*_tmp594)++){(*_tmp591)[*_tmp594]=((void**)(*d).curr)[(
int)*_tmp594];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpFD=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpFD == rval)return d;{
unsigned int _tmp594;unsigned int _tmp593;struct _dyneither_ptr _tmp592;void**
_tmp591;unsigned int _tmp590;struct _dyneither_ptr res=(_tmp590=_get_dyneither_size(
d,sizeof(void*)),((_tmp591=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp590)),((_tmp592=_tag_dyneither(_tmp591,sizeof(void*),_tmp590),((((_tmp593=
_tmp590,_tmp595(& d,& _tmp594,& _tmp593,& _tmp591))),_tmp592)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple21{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple21 _tmp596;struct _tuple21 _tmp103=(_tmp596.f1=fs,((
_tmp596.f2=old_val,_tmp596)));struct Cyc_List_List*_tmp104;struct Cyc_List_List
_tmp105;int _tmp106;struct Cyc_List_List*_tmp107;void*_tmp108;int _tmp10A;struct
_dyneither_ptr _tmp10B;_LL57: _tmp104=_tmp103.f1;if(_tmp104 == 0)goto _LL59;_tmp105=*
_tmp104;_tmp106=(int)_tmp105.hd;_tmp107=_tmp105.tl;_tmp108=_tmp103.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp109=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp108;if(_tmp109->tag != 7)goto
_LL59;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}};_LL58: {void*_tmp10C=Cyc_CfFlowInfo_insert_place_outer(
r,_tmp107,*((void**)_check_dyneither_subscript(_tmp10B,sizeof(void*),_tmp106)),
new_val);struct _dyneither_ptr _tmp10D=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp10B,
_tmp106,_tmp10C);if((void**)_tmp10D.curr == (void**)_tmp10B.curr)return old_val;
else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp599;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp598;return(void*)((_tmp598=_region_malloc(r,sizeof(*_tmp598)),((_tmp598[0]=((
_tmp599.tag=7,((_tmp599.f1=_tmp10A,((_tmp599.f2=_tmp10D,_tmp599)))))),_tmp598))));}}
_LL59:;_LL5A: {struct Cyc_Core_Impossible_struct _tmp59F;const char*_tmp59E;struct
Cyc_Core_Impossible_struct*_tmp59D;(int)_throw((void*)((_tmp59D=_cycalloc(
sizeof(*_tmp59D)),((_tmp59D[0]=((_tmp59F.tag=Cyc_Core_Impossible,((_tmp59F.f1=((
_tmp59E="bad insert place",_tag_dyneither(_tmp59E,sizeof(char),17))),_tmp59F)))),
_tmp59D)))));}_LL56:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp113=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp113,0);{void*oldval;void*
newval;{struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(
setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp113);;_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp118=
_tmp115;_LL5C: {struct Cyc_Dict_Absent_struct*_tmp119=(struct Cyc_Dict_Absent_struct*)
_tmp118;if(_tmp119->tag != Cyc_Dict_Absent)goto _LL5E;}_LL5D: continue;_LL5E:;_LL5F:(
void)_throw(_tmp118);_LL5B:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL60: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL61: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL62: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp11B;struct Cyc_List_List*
_tmp11C;struct Cyc_CfFlowInfo_Place _tmp11A=*_tmp113;_tmp11B=_tmp11A.root;_tmp11C=
_tmp11A.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp11B,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp11C,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp11B),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r);static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*
r){void*_tmp11D=r;enum Cyc_CfFlowInfo_InitLevel _tmp11F;enum Cyc_CfFlowInfo_InitLevel
_tmp121;_LL65: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp11E=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp11D;if(_tmp11E->tag != 3)goto _LL67;else{_tmp11F=_tmp11E->f1;}}_LL66: return
_tmp11F;_LL67: {struct Cyc_CfFlowInfo_Esc_struct*_tmp120=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp11D;if(_tmp120->tag != 4)goto _LL69;else{_tmp121=_tmp120->f1;}}_LL68: return
_tmp121;_LL69: {struct Cyc_CfFlowInfo_Zero_struct*_tmp122=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp11D;if(_tmp122->tag != 0)goto _LL6B;}_LL6A: goto _LL6C;_LL6B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp123=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp11D;if(_tmp123->tag != 1)
goto _LL6D;}_LL6C: return Cyc_CfFlowInfo_AllIL;_LL6D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp124=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp11D;if(_tmp124->tag != 2)
goto _LL6F;}_LL6E: return Cyc_CfFlowInfo_ThisIL;_LL6F: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp125=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp11D;if(_tmp125->tag != 6)goto
_LL71;}_LL70: return Cyc_CfFlowInfo_AllIL;_LL71:;_LL72: {struct Cyc_Core_Impossible_struct
_tmp5A5;const char*_tmp5A4;struct Cyc_Core_Impossible_struct*_tmp5A3;(int)_throw((
void*)((_tmp5A3=_cycalloc(sizeof(*_tmp5A3)),((_tmp5A3[0]=((_tmp5A5.tag=Cyc_Core_Impossible,((
_tmp5A5.f1=((_tmp5A4="initlevel_approx",_tag_dyneither(_tmp5A4,sizeof(char),17))),
_tmp5A5)))),_tmp5A3)))));}_LL64:;}static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc);
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp129=r;int _tmp12B;struct _dyneither_ptr _tmp12C;int _tmp12E;struct _dyneither_ptr
_tmp12F;struct Cyc_CfFlowInfo_Place*_tmp131;_LL74: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp129;if(_tmp12A->tag != 7)goto
_LL76;else{_tmp12B=_tmp12A->f1;if(_tmp12B != 1)goto _LL76;_tmp12C=_tmp12A->f2;}}
_LL75: {unsigned int _tmp132=_get_dyneither_size(_tmp12C,sizeof(void*));this_ans=
Cyc_CfFlowInfo_NoneIL;{int i=0;for(0;i < _tmp132;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp12C,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}goto _LL73;}_LL76: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp129;if(_tmp12D->tag != 7)goto _LL78;else{_tmp12E=_tmp12D->f1;if(_tmp12E != 0)
goto _LL78;_tmp12F=_tmp12D->f2;}}_LL77: {unsigned int _tmp133=_get_dyneither_size(
_tmp12F,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;for(0;i < _tmp133;++
i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp12F,sizeof(void*),i)),this_ans);}}goto _LL73;}
_LL78: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp130=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp129;if(_tmp130->tag != 5)goto _LL7A;else{_tmp131=_tmp130->f1;}}_LL79: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp131))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp5A6;env->seen=((
_tmp5A6=_region_malloc(env->e,sizeof(*_tmp5A6)),((_tmp5A6->hd=_tmp131,((_tmp5A6->tl=
env->seen,_tmp5A6))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp131),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL73;_LL7A:;_LL7B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL73:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct
_RegionHandle*_tmp135=env->r;struct Cyc_CfFlowInfo_InitlevelEnv _tmp5A7;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp136=(_tmp5A7.e=_tmp135,((_tmp5A7.d=d,((_tmp5A7.seen=0,_tmp5A7)))));return Cyc_CfFlowInfo_initlevel_rec(&
_tmp136,r,Cyc_CfFlowInfo_AllIL);}struct _tuple22{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp139;struct Cyc_List_List*
_tmp13A;struct Cyc_CfFlowInfo_Place _tmp138=*place;_tmp139=_tmp138.root;_tmp13A=
_tmp138.fields;{void*_tmp13B=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp139);for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){struct _tuple22 _tmp5A8;struct
_tuple22 _tmp13D=(_tmp5A8.f1=_tmp13B,((_tmp5A8.f2=(int)_tmp13A->hd,_tmp5A8)));
void*_tmp13E;int _tmp140;struct _dyneither_ptr _tmp141;int _tmp142;_LL7D: _tmp13E=
_tmp13D.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp13F=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp13E;if(_tmp13F->tag != 7)goto _LL7F;else{_tmp140=_tmp13F->f1;_tmp141=_tmp13F->f2;}};
_tmp142=_tmp13D.f2;_LL7E: _tmp13B=*((void**)_check_dyneither_subscript(_tmp141,
sizeof(void*),_tmp142));goto _LL7C;_LL7F:;_LL80: {struct Cyc_Core_Impossible_struct
_tmp5AE;const char*_tmp5AD;struct Cyc_Core_Impossible_struct*_tmp5AC;(int)_throw((
void*)((_tmp5AC=_cycalloc(sizeof(*_tmp5AC)),((_tmp5AC[0]=((_tmp5AE.tag=Cyc_Core_Impossible,((
_tmp5AE.f1=((_tmp5AD="bad lookup_place",_tag_dyneither(_tmp5AD,sizeof(char),17))),
_tmp5AE)))),_tmp5AC)))));}_LL7C:;}return _tmp13B;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp146=
rval;int _tmp149;struct _dyneither_ptr _tmp14A;_LL82: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp147=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp146;if(_tmp147->tag != 4)goto _LL84;}
_LL83: return 0;_LL84: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp148=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp146;if(_tmp148->tag != 7)goto _LL86;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}
_LL85: {unsigned int sz=_get_dyneither_size(_tmp14A,sizeof(void*));{int i=0;for(0;
i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp14A.curr)[i]))
return 0;}}return 1;}_LL86:;_LL87: return 1;_LL81:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp14B=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5AF;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp5AF=_region_malloc(_tmp14B,sizeof(*_tmp5AF)),((_tmp5AF->rgn=_tmp14B,((
_tmp5AF->places=0,_tmp5AF)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp5B9(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp5B8,unsigned int*_tmp5B7,void***_tmp5B5,struct
_dyneither_ptr*_tmp15D,struct _dyneither_ptr*_tmp159){for(*_tmp5B8=0;*_tmp5B8 < *
_tmp5B7;(*_tmp5B8)++){(*_tmp5B5)[*_tmp5B8]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp159).curr)[(int)*_tmp5B8],*((void**)
_check_dyneither_subscript(*_tmp15D,sizeof(void*),(int)*_tmp5B8)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp5B0;struct _tuple0 _tmp14E=(_tmp5B0.f1=oldval,((
_tmp5B0.f2=newval,_tmp5B0)));void*_tmp14F;void*_tmp151;struct Cyc_CfFlowInfo_Place*
_tmp153;void*_tmp154;void*_tmp156;int _tmp158;struct _dyneither_ptr _tmp159;void*
_tmp15A;int _tmp15C;struct _dyneither_ptr _tmp15D;void*_tmp15E;enum Cyc_CfFlowInfo_InitLevel
_tmp160;_LL89: _tmp14F=_tmp14E.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp150=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp14F;if(_tmp150->tag != 4)goto _LL8B;};_tmp151=
_tmp14E.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp152=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp151;if(_tmp152->tag != 5)goto _LL8B;else{_tmp153=_tmp152->f1;}};_LL8A: Cyc_CfFlowInfo_add_place(
env->pile,_tmp153);goto _LL8C;_LL8B: _tmp154=_tmp14E.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp155=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp154;if(_tmp155->tag != 4)goto _LL8D;};
_LL8C: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp5B3;void*_tmp5B2;(_tmp5B2=0,Cyc_Tcutil_terr(env->loc,((_tmp5B3="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp5B3,sizeof(char),67))),_tag_dyneither(_tmp5B2,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL8D: _tmp156=_tmp14E.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp157=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp156;if(_tmp157->tag != 7)goto
_LL8F;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;}};_tmp15A=_tmp14E.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp15B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp15A;if(_tmp15B->tag != 7)goto _LL8F;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}};
_LL8E: {unsigned int _tmp5B8;unsigned int _tmp5B7;struct _dyneither_ptr _tmp5B6;void**
_tmp5B5;unsigned int _tmp5B4;struct _dyneither_ptr new_d=(_tmp5B4=
_get_dyneither_size(_tmp159,sizeof(void*)),((_tmp5B5=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp5B4)),((_tmp5B6=_tag_dyneither(_tmp5B5,sizeof(
void*),_tmp5B4),((((_tmp5B7=_tmp5B4,_tmp5B9(& env,& _tmp5B8,& _tmp5B7,& _tmp5B5,&
_tmp15D,& _tmp159))),_tmp5B6)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp159,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp159.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp159,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp15D,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5BC;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5BB;return(void*)((_tmp5BB=_region_malloc((env->fenv)->r,sizeof(*_tmp5BB)),((
_tmp5BB[0]=((_tmp5BC.tag=7,((_tmp5BC.f1=_tmp158,((_tmp5BC.f2=new_d,_tmp5BC)))))),
_tmp5BB))));};}_LL8F: _tmp15E=_tmp14E.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp15F=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp15E;if(_tmp15F->tag != 4)goto _LL91;else{
_tmp160=_tmp15F->f1;}};_LL90: switch(_tmp160){case Cyc_CfFlowInfo_NoneIL: _LL93:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL94: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL95: return(env->fenv)->unknown_all;}_LL91:;_LL92:
return newval;_LL88:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval);static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple21 _tmp5BD;struct _tuple21 _tmp16A=(_tmp5BD.f1=fs,((_tmp5BD.f2=oldval,
_tmp5BD)));struct Cyc_List_List*_tmp16B;struct Cyc_List_List _tmp16C;int _tmp16D;
struct Cyc_List_List*_tmp16E;void*_tmp16F;int _tmp171;struct _dyneither_ptr _tmp172;
_LL98: _tmp16B=_tmp16A.f1;if(_tmp16B == 0)goto _LL9A;_tmp16C=*_tmp16B;_tmp16D=(int)
_tmp16C.hd;_tmp16E=_tmp16C.tl;_tmp16F=_tmp16A.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp170=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16F;if(_tmp170->tag != 7)goto
_LL9A;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}};_LL99: {void*_tmp173=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp16E,*((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),_tmp16D)),
newval);struct _dyneither_ptr _tmp174=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp172,_tmp16D,_tmp173);if((void**)_tmp174.curr == (void**)_tmp172.curr)return
oldval;else{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5C0;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5BF;return(void*)((_tmp5BF=_region_malloc((env->fenv)->r,sizeof(*_tmp5BF)),((
_tmp5BF[0]=((_tmp5C0.tag=7,((_tmp5C0.f1=_tmp171,((_tmp5C0.f2=_tmp174,_tmp5C0)))))),
_tmp5BF))));}}_LL9A:;_LL9B: {struct Cyc_Core_Impossible_struct _tmp5C6;const char*
_tmp5C5;struct Cyc_Core_Impossible_struct*_tmp5C4;(int)_throw((void*)((_tmp5C4=
_cycalloc(sizeof(*_tmp5C4)),((_tmp5C4[0]=((_tmp5C6.tag=Cyc_Core_Impossible,((
_tmp5C6.f1=((_tmp5C5="bad assign place",_tag_dyneither(_tmp5C5,sizeof(char),17))),
_tmp5C6)))),_tmp5C4)))));}_LL97:;};}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp17A=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp17C;void*_tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_CfFlowInfo_Place*
_tmp17B=place;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.root;_tmp17E=_tmp17C.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp5C9;struct Cyc_CfFlowInfo_AssignEnv _tmp5C8;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5C8.fenv=fenv,((_tmp5C8.pile=((_tmp5C9=
_region_malloc(_tmp17A,sizeof(*_tmp5C9)),((_tmp5C9->rgn=_tmp17A,((_tmp5C9->places=
0,_tmp5C9)))))),((_tmp5C8.d=d,((_tmp5C8.loc=loc,_tmp5C8)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp17E,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp17D),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp17D,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct _RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(struct _RegionHandle*r,struct Cyc_List_List*
l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*_tmp181=0;
for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmp183;enum Cyc_Absyn_Primop
_tmp184;void*_tmp185;struct Cyc_CfFlowInfo_TagCmp*_tmp182=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp183=*_tmp182;_tmp184=_tmp183.cmp;_tmp185=_tmp183.bd;{int found=0;enum 
Cyc_Absyn_Primop joined_cmp=Cyc_Absyn_Lte;{struct Cyc_List_List*_tmp186=l1;for(0;
_tmp186 != 0;_tmp186=_tmp186->tl){struct Cyc_CfFlowInfo_TagCmp _tmp188;enum Cyc_Absyn_Primop
_tmp189;void*_tmp18A;struct Cyc_CfFlowInfo_TagCmp*_tmp187=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp186->hd;_tmp188=*_tmp187;_tmp189=_tmp188.cmp;_tmp18A=_tmp188.bd;if(Cyc_Tcutil_typecmp(
_tmp185,_tmp18A)== 0){found=1;if(_tmp189 == _tmp184){joined_cmp=_tmp189;break;}}}}
if(found){struct Cyc_CfFlowInfo_TagCmp*_tmp5CC;struct Cyc_List_List*_tmp5CB;
_tmp181=((_tmp5CB=_region_malloc(r,sizeof(*_tmp5CB)),((_tmp5CB->hd=((_tmp5CC=
_region_malloc(r,sizeof(*_tmp5CC)),((_tmp5CC->cmp=joined_cmp,((_tmp5CC->bd=
_tmp185,_tmp5CC)))))),((_tmp5CB->tl=_tmp181,_tmp5CB))))));}};}return _tmp181;};}
struct _tuple23{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;}
;static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,
void*r1,void*r2);static void _tmp5D3(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp5D2,unsigned int*_tmp5D1,void***_tmp5CF,struct _dyneither_ptr*
_tmp1AB,struct _dyneither_ptr*_tmp1A7){for(*_tmp5D2=0;*_tmp5D2 < *_tmp5D1;(*
_tmp5D2)++){(*_tmp5CF)[*_tmp5D2]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1A7).curr)[(
int)*_tmp5D2],*((void**)_check_dyneither_subscript(*_tmp1AB,sizeof(void*),(int)*
_tmp5D2)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp18D=(
env->fenv)->r;{struct _tuple0 _tmp5CD;struct _tuple0 _tmp18F=(_tmp5CD.f1=r1,((
_tmp5CD.f2=r2,_tmp5CD)));void*_tmp190;struct Cyc_CfFlowInfo_Place*_tmp192;void*
_tmp193;struct Cyc_CfFlowInfo_Place*_tmp195;void*_tmp196;struct Cyc_CfFlowInfo_Place*
_tmp198;void*_tmp199;struct Cyc_CfFlowInfo_Place*_tmp19B;void*_tmp19C;void*
_tmp19E;void*_tmp1A0;void*_tmp1A2;void*_tmp1A4;int _tmp1A6;struct _dyneither_ptr
_tmp1A7;void*_tmp1A8;int _tmp1AA;struct _dyneither_ptr _tmp1AB;void*_tmp1AC;struct
Cyc_List_List*_tmp1AE;void*_tmp1AF;struct Cyc_List_List*_tmp1B1;void*_tmp1B2;void*
_tmp1B4;_LL9D: _tmp190=_tmp18F.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp191=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp190;if(_tmp191->tag != 5)goto _LL9F;
else{_tmp192=_tmp191->f1;}};_tmp193=_tmp18F.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp194=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp193;if(_tmp194->tag != 5)goto
_LL9F;else{_tmp195=_tmp194->f1;}};_LL9E: if(Cyc_CfFlowInfo_place_cmp(_tmp192,
_tmp195)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp192);Cyc_CfFlowInfo_add_place(
env->pile,_tmp195);goto _LL9C;_LL9F: _tmp196=_tmp18F.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp197=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp196;if(_tmp197->tag != 5)goto
_LLA1;else{_tmp198=_tmp197->f1;}};_LLA0: Cyc_CfFlowInfo_add_place(env->pile,
_tmp198);goto _LL9C;_LLA1: _tmp199=_tmp18F.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp19A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp199;if(_tmp19A->tag != 5)goto
_LLA3;else{_tmp19B=_tmp19A->f1;}};_LLA2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp19B);goto _LL9C;_LLA3: _tmp19C=_tmp18F.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp19D=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp19C;if(_tmp19D->tag != 1)
goto _LLA5;};_tmp19E=_tmp18F.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp19F=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp19E;if(_tmp19F->tag != 2)goto _LLA5;};
_LLA4: goto _LLA6;_LLA5: _tmp1A0=_tmp18F.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1A1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1A0;if(_tmp1A1->tag != 2)
goto _LLA7;};_tmp1A2=_tmp18F.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1A3=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1A2;if(_tmp1A3->tag != 1)goto _LLA7;};
_LLA6: return(env->fenv)->notzerothis;_LLA7: _tmp1A4=_tmp18F.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1A5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A4;if(_tmp1A5->tag != 7)goto
_LLA9;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}};_tmp1A8=_tmp18F.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1A9=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1A8;if(_tmp1A9->tag != 7)goto _LLA9;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}};
_LLA8: {unsigned int _tmp5D2;unsigned int _tmp5D1;struct _dyneither_ptr _tmp5D0;void**
_tmp5CF;unsigned int _tmp5CE;struct _dyneither_ptr new_d=(_tmp5CE=
_get_dyneither_size(_tmp1A7,sizeof(void*)),((_tmp5CF=(void**)_region_malloc(
_tmp18D,_check_times(sizeof(void*),_tmp5CE)),((_tmp5D0=_tag_dyneither(_tmp5CF,
sizeof(void*),_tmp5CE),((((_tmp5D1=_tmp5CE,_tmp5D3(& env,& _tmp5D2,& _tmp5D1,&
_tmp5CF,& _tmp1AB,& _tmp1A7))),_tmp5D0)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A7,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A7.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A7,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1AB,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5D6;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5D5;return(void*)((_tmp5D5=_region_malloc(_tmp18D,sizeof(*_tmp5D5)),((
_tmp5D5[0]=((_tmp5D6.tag=7,((_tmp5D6.f1=_tmp1A6,((_tmp5D6.f2=new_d,_tmp5D6)))))),
_tmp5D5))));};}_LLA9: _tmp1AC=_tmp18F.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1AD=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1AC;if(_tmp1AD->tag != 6)goto
_LLAB;else{_tmp1AE=_tmp1AD->f1;}};_tmp1AF=_tmp18F.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B0=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1AF;if(_tmp1B0->tag != 6)goto
_LLAB;else{_tmp1B1=_tmp1B0->f1;}};_LLAA: {struct Cyc_List_List*_tmp1BC=Cyc_CfFlowInfo_join_tag_cmps(
_tmp18D,_tmp1AE,_tmp1B1);if(_tmp1BC == _tmp1AE)return r1;{struct Cyc_CfFlowInfo_TagCmps_struct
_tmp5D9;struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5D8;return(void*)((_tmp5D8=
_region_malloc(_tmp18D,sizeof(*_tmp5D8)),((_tmp5D8[0]=((_tmp5D9.tag=6,((_tmp5D9.f1=
_tmp1BC,_tmp5D9)))),_tmp5D8))));};}_LLAB: _tmp1B2=_tmp18F.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B3=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B2;if(_tmp1B3->tag != 6)goto
_LLAD;};_LLAC: return r2;_LLAD: _tmp1B4=_tmp18F.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B5=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B4;if(_tmp1B5->tag != 6)goto
_LLAF;};_LLAE: return r1;_LLAF:;_LLB0: goto _LL9C;_LL9C:;}{enum Cyc_CfFlowInfo_InitLevel
il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel
il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5DA;struct
_tuple0 _tmp1C0=(_tmp5DA.f1=r1,((_tmp5DA.f2=r2,_tmp5DA)));void*_tmp1C1;void*
_tmp1C3;_LLB2: _tmp1C1=_tmp1C0.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1C2=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C1;if(_tmp1C2->tag != 4)goto _LLB4;};_LLB3:
goto _LLB5;_LLB4: _tmp1C3=_tmp1C0.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1C4=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C3;if(_tmp1C4->tag != 4)goto _LLB6;};_LLB5: {
struct _tuple23 _tmp5DB;struct _tuple23 _tmp1C6=(_tmp5DB.f1=il1,((_tmp5DB.f2=il2,
_tmp5DB)));enum Cyc_CfFlowInfo_InitLevel _tmp1C7;enum Cyc_CfFlowInfo_InitLevel
_tmp1C8;enum Cyc_CfFlowInfo_InitLevel _tmp1C9;enum Cyc_CfFlowInfo_InitLevel
_tmp1CA;_LLB9: _tmp1C7=_tmp1C6.f2;if(_tmp1C7 != Cyc_CfFlowInfo_NoneIL)goto _LLBB;
_LLBA: goto _LLBC;_LLBB: _tmp1C8=_tmp1C6.f1;if(_tmp1C8 != Cyc_CfFlowInfo_NoneIL)goto
_LLBD;_LLBC: return(env->fenv)->esc_none;_LLBD: _tmp1C9=_tmp1C6.f2;if(_tmp1C9 != 
Cyc_CfFlowInfo_ThisIL)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1CA=_tmp1C6.f1;if(
_tmp1CA != Cyc_CfFlowInfo_ThisIL)goto _LLC1;_LLC0: return(env->fenv)->esc_this;
_LLC1:;_LLC2: return(env->fenv)->esc_all;_LLB8:;}_LLB6:;_LLB7: {struct _tuple23
_tmp5DC;struct _tuple23 _tmp1CC=(_tmp5DC.f1=il1,((_tmp5DC.f2=il2,_tmp5DC)));enum 
Cyc_CfFlowInfo_InitLevel _tmp1CD;enum Cyc_CfFlowInfo_InitLevel _tmp1CE;enum Cyc_CfFlowInfo_InitLevel
_tmp1CF;enum Cyc_CfFlowInfo_InitLevel _tmp1D0;_LLC4: _tmp1CD=_tmp1CC.f2;if(_tmp1CD
!= Cyc_CfFlowInfo_NoneIL)goto _LLC6;_LLC5: goto _LLC7;_LLC6: _tmp1CE=_tmp1CC.f1;if(
_tmp1CE != Cyc_CfFlowInfo_NoneIL)goto _LLC8;_LLC7: return(env->fenv)->unknown_none;
_LLC8: _tmp1CF=_tmp1CC.f2;if(_tmp1CF != Cyc_CfFlowInfo_ThisIL)goto _LLCA;_LLC9: goto
_LLCB;_LLCA: _tmp1D0=_tmp1CC.f1;if(_tmp1D0 != Cyc_CfFlowInfo_ThisIL)goto _LLCC;
_LLCB: return(env->fenv)->unknown_this;_LLCC:;_LLCD: return(env->fenv)->unknown_all;
_LLC3:;}_LLB1:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5DD;return((_tmp5DD.EqualConst).val=i,(((_tmp5DD.EqualConst).tag=
1,_tmp5DD)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple11 _tmp5E0;union Cyc_CfFlowInfo_RelnOp _tmp5DF;return((
_tmp5DF.LessVar).val=((_tmp5E0.f1=v,((_tmp5E0.f2=t,_tmp5E0)))),(((_tmp5DF.LessVar).tag=
2,_tmp5DF)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp5E1;return((_tmp5E1.LessNumelts).val=v,(((
_tmp5E1.LessNumelts).tag=3,_tmp5E1)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5E2;return((_tmp5E2.LessConst).val=i,(((_tmp5E2.LessConst).tag=
4,_tmp5E2)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp
_tmp5E3;return((_tmp5E3.LessEqConst).val=i,(((_tmp5E3.LessEqConst).tag=6,_tmp5E3)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp5E4;return((_tmp5E4.LessEqNumelts).val=v,(((
_tmp5E4.LessEqNumelts).tag=5,_tmp5E4)));}struct _tuple24{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple24 _tmp5E5;struct _tuple24 _tmp1D9=(
_tmp5E5.f1=r1,((_tmp5E5.f2=r2,_tmp5E5)));union Cyc_CfFlowInfo_RelnOp _tmp1DA;
unsigned int _tmp1DB;union Cyc_CfFlowInfo_RelnOp _tmp1DC;unsigned int _tmp1DD;union
Cyc_CfFlowInfo_RelnOp _tmp1DE;struct _tuple11 _tmp1DF;struct Cyc_Absyn_Vardecl*
_tmp1E0;union Cyc_CfFlowInfo_RelnOp _tmp1E1;struct _tuple11 _tmp1E2;struct Cyc_Absyn_Vardecl*
_tmp1E3;union Cyc_CfFlowInfo_RelnOp _tmp1E4;struct Cyc_Absyn_Vardecl*_tmp1E5;union
Cyc_CfFlowInfo_RelnOp _tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E7;union Cyc_CfFlowInfo_RelnOp
_tmp1E8;unsigned int _tmp1E9;union Cyc_CfFlowInfo_RelnOp _tmp1EA;unsigned int
_tmp1EB;union Cyc_CfFlowInfo_RelnOp _tmp1EC;unsigned int _tmp1ED;union Cyc_CfFlowInfo_RelnOp
_tmp1EE;unsigned int _tmp1EF;union Cyc_CfFlowInfo_RelnOp _tmp1F0;struct Cyc_Absyn_Vardecl*
_tmp1F1;union Cyc_CfFlowInfo_RelnOp _tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;_LLCF:
_tmp1DA=_tmp1D9.f1;if((_tmp1DA.EqualConst).tag != 1)goto _LLD1;_tmp1DB=(
unsigned int)(_tmp1DA.EqualConst).val;_tmp1DC=_tmp1D9.f2;if((_tmp1DC.EqualConst).tag
!= 1)goto _LLD1;_tmp1DD=(unsigned int)(_tmp1DC.EqualConst).val;_LLD0: return
_tmp1DB == _tmp1DD;_LLD1: _tmp1DE=_tmp1D9.f1;if((_tmp1DE.LessVar).tag != 2)goto
_LLD3;_tmp1DF=(struct _tuple11)(_tmp1DE.LessVar).val;_tmp1E0=_tmp1DF.f1;_tmp1E1=
_tmp1D9.f2;if((_tmp1E1.LessVar).tag != 2)goto _LLD3;_tmp1E2=(struct _tuple11)(
_tmp1E1.LessVar).val;_tmp1E3=_tmp1E2.f1;_LLD2: return _tmp1E0 == _tmp1E3;_LLD3:
_tmp1E4=_tmp1D9.f1;if((_tmp1E4.LessNumelts).tag != 3)goto _LLD5;_tmp1E5=(struct Cyc_Absyn_Vardecl*)(
_tmp1E4.LessNumelts).val;_tmp1E6=_tmp1D9.f2;if((_tmp1E6.LessNumelts).tag != 3)
goto _LLD5;_tmp1E7=(struct Cyc_Absyn_Vardecl*)(_tmp1E6.LessNumelts).val;_LLD4:
return _tmp1E5 == _tmp1E7;_LLD5: _tmp1E8=_tmp1D9.f1;if((_tmp1E8.LessConst).tag != 4)
goto _LLD7;_tmp1E9=(unsigned int)(_tmp1E8.LessConst).val;_tmp1EA=_tmp1D9.f2;if((
_tmp1EA.LessConst).tag != 4)goto _LLD7;_tmp1EB=(unsigned int)(_tmp1EA.LessConst).val;
_LLD6: return _tmp1E9 == _tmp1EB;_LLD7: _tmp1EC=_tmp1D9.f1;if((_tmp1EC.LessEqConst).tag
!= 6)goto _LLD9;_tmp1ED=(unsigned int)(_tmp1EC.LessEqConst).val;_tmp1EE=_tmp1D9.f2;
if((_tmp1EE.LessEqConst).tag != 6)goto _LLD9;_tmp1EF=(unsigned int)(_tmp1EE.LessEqConst).val;
_LLD8: return _tmp1ED == _tmp1EF;_LLD9: _tmp1F0=_tmp1D9.f1;if((_tmp1F0.LessEqNumelts).tag
!= 5)goto _LLDB;_tmp1F1=(struct Cyc_Absyn_Vardecl*)(_tmp1F0.LessEqNumelts).val;
_tmp1F2=_tmp1D9.f2;if((_tmp1F2.LessEqNumelts).tag != 5)goto _LLDB;_tmp1F3=(struct
Cyc_Absyn_Vardecl*)(_tmp1F2.LessEqNumelts).val;_LLDA: return _tmp1F1 == _tmp1F3;
_LLDB:;_LLDC: return 0;_LLCE:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp1F4=r1s;for(0;_tmp1F4 != 0;_tmp1F4=_tmp1F4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1F5=(struct Cyc_CfFlowInfo_Reln*)_tmp1F4->hd;int
found=0;{struct Cyc_List_List*_tmp1F6=r2s;for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1F7=(struct Cyc_CfFlowInfo_Reln*)_tmp1F6->hd;if(
_tmp1F5 == _tmp1F7  || _tmp1F5->vd == _tmp1F7->vd  && Cyc_CfFlowInfo_same_relop(
_tmp1F5->rop,_tmp1F7->rop)){{struct Cyc_List_List*_tmp5E6;res=((_tmp5E6=
_region_malloc(r,sizeof(*_tmp5E6)),((_tmp5E6->hd=_tmp1F5,((_tmp5E6->tl=res,
_tmp5E6))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;};}
static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place);static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place){struct _handler_cons _tmp1F9;_push_handler(& _tmp1F9);{int _tmp1FB=0;if(
setjmp(_tmp1F9.handler))_tmp1FB=1;if(!_tmp1FB){*l=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,
place);;_pop_handler();}else{void*_tmp1FA=(void*)_exn_thrown;void*_tmp1FD=
_tmp1FA;_LLDE: {struct Cyc_Core_Not_found_struct*_tmp1FE=(struct Cyc_Core_Not_found_struct*)
_tmp1FD;if(_tmp1FE->tag != Cyc_Core_Not_found)goto _LLE0;}_LLDF: goto _LLDD;_LLE0:;
_LLE1:(void)_throw(_tmp1FD);_LLDD:;}};}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;struct _RegionHandle*_tmp1FF=r;{struct
Cyc_List_List*_tmp200=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp1FF,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp200,c2.may_consume);while(_tmp200 != 0){{struct Cyc_List_List*
_tmp5E7;mc=((_tmp5E7=_region_malloc(r,sizeof(*_tmp5E7)),((_tmp5E7->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp200))->hd,((_tmp5E7->tl=mc,_tmp5E7))))));}
_tmp200=((struct Cyc_List_List*)_check_null(_tmp200))->tl;}}{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5E8;return(_tmp5E8.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0),((_tmp5E8.may_consume=mc,_tmp5E8)));};}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp203=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);struct Cyc_CfFlowInfo_ConsumeInfo _tmp5E9;return(_tmp5E9.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1),((_tmp5E9.may_consume=
_tmp203,_tmp5E9)));}struct _tuple25{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,int or_consumed);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,int or_consumed){struct _tuple25 _tmp5EA;struct
_tuple25 _tmp206=(_tmp5EA.f1=f1,((_tmp5EA.f2=f2,_tmp5EA)));union Cyc_CfFlowInfo_FlowInfo
_tmp207;int _tmp208;union Cyc_CfFlowInfo_FlowInfo _tmp209;int _tmp20A;union Cyc_CfFlowInfo_FlowInfo
_tmp20B;struct _tuple12 _tmp20C;struct Cyc_Dict_Dict _tmp20D;struct Cyc_List_List*
_tmp20E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp20F;union Cyc_CfFlowInfo_FlowInfo
_tmp210;struct _tuple12 _tmp211;struct Cyc_Dict_Dict _tmp212;struct Cyc_List_List*
_tmp213;struct Cyc_CfFlowInfo_ConsumeInfo _tmp214;_LLE3: _tmp207=_tmp206.f1;if((
_tmp207.BottomFL).tag != 1)goto _LLE5;_tmp208=(int)(_tmp207.BottomFL).val;_LLE4:
return f2;_LLE5: _tmp209=_tmp206.f2;if((_tmp209.BottomFL).tag != 1)goto _LLE7;
_tmp20A=(int)(_tmp209.BottomFL).val;_LLE6: return f1;_LLE7: _tmp20B=_tmp206.f1;if((
_tmp20B.ReachableFL).tag != 2)goto _LLE2;_tmp20C=(struct _tuple12)(_tmp20B.ReachableFL).val;
_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;_tmp20F=_tmp20C.f3;_tmp210=_tmp206.f2;if((
_tmp210.ReachableFL).tag != 2)goto _LLE2;_tmp211=(struct _tuple12)(_tmp210.ReachableFL).val;
_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;_tmp214=_tmp211.f3;_LLE8: if(((_tmp20D.t == 
_tmp212.t  && _tmp20E == _tmp213) && _tmp20F.may_consume == _tmp214.may_consume)
 && (_tmp20F.consumed).t == (_tmp214.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp215=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5ED;struct Cyc_CfFlowInfo_JoinEnv
_tmp5EC;struct Cyc_CfFlowInfo_JoinEnv _tmp216=(_tmp5EC.fenv=fenv,((_tmp5EC.pile=((
_tmp5ED=_region_malloc(_tmp215,sizeof(*_tmp5ED)),((_tmp5ED->rgn=_tmp215,((
_tmp5ED->places=0,_tmp5ED)))))),((_tmp5EC.d1=_tmp20D,((_tmp5EC.d2=_tmp212,
_tmp5EC)))))));struct Cyc_Dict_Dict _tmp217=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp216,_tmp20D,_tmp212);struct Cyc_List_List*_tmp218=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp20E,_tmp213);struct Cyc_CfFlowInfo_ConsumeInfo _tmp219=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp20F,_tmp214): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp20F,_tmp214);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp216.pile,
all_changed,_tmp217),_tmp218,_tmp219);};_LLE2:;}struct _tuple26{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct
_tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed);struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed){union Cyc_CfFlowInfo_FlowInfo _tmp21D;void*_tmp21E;struct _tuple13
_tmp21C=pr1;_tmp21D=_tmp21C.f1;_tmp21E=_tmp21C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp220;void*_tmp221;struct _tuple13 _tmp21F=pr2;_tmp220=_tmp21F.f1;_tmp221=
_tmp21F.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp21D,_tmp220,or_consumed);struct _tuple26 _tmp5EE;struct _tuple26
_tmp223=(_tmp5EE.f1=_tmp21D,((_tmp5EE.f2=_tmp220,((_tmp5EE.f3=outflow,_tmp5EE)))));
union Cyc_CfFlowInfo_FlowInfo _tmp224;int _tmp225;union Cyc_CfFlowInfo_FlowInfo
_tmp226;int _tmp227;union Cyc_CfFlowInfo_FlowInfo _tmp228;struct _tuple12 _tmp229;
struct Cyc_Dict_Dict _tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp22B;struct _tuple12
_tmp22C;struct Cyc_Dict_Dict _tmp22D;union Cyc_CfFlowInfo_FlowInfo _tmp22E;struct
_tuple12 _tmp22F;struct Cyc_Dict_Dict _tmp230;struct Cyc_List_List*_tmp231;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp232;_LLEA: _tmp224=_tmp223.f1;if((_tmp224.BottomFL).tag != 1)goto _LLEC;_tmp225=(
int)(_tmp224.BottomFL).val;_LLEB: {struct _tuple13 _tmp5EF;return(_tmp5EF.f1=
outflow,((_tmp5EF.f2=_tmp221,_tmp5EF)));}_LLEC: _tmp226=_tmp223.f2;if((_tmp226.BottomFL).tag
!= 1)goto _LLEE;_tmp227=(int)(_tmp226.BottomFL).val;_LLED: {struct _tuple13 _tmp5F0;
return(_tmp5F0.f1=outflow,((_tmp5F0.f2=_tmp21E,_tmp5F0)));}_LLEE: _tmp228=_tmp223.f1;
if((_tmp228.ReachableFL).tag != 2)goto _LLF0;_tmp229=(struct _tuple12)(_tmp228.ReachableFL).val;
_tmp22A=_tmp229.f1;_tmp22B=_tmp223.f2;if((_tmp22B.ReachableFL).tag != 2)goto _LLF0;
_tmp22C=(struct _tuple12)(_tmp22B.ReachableFL).val;_tmp22D=_tmp22C.f1;_tmp22E=
_tmp223.f3;if((_tmp22E.ReachableFL).tag != 2)goto _LLF0;_tmp22F=(struct _tuple12)(
_tmp22E.ReachableFL).val;_tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;_tmp232=_tmp22F.f3;
_LLEF: if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp21E,_tmp221)){struct _tuple13 _tmp5F1;return(_tmp5F1.f1=outflow,((_tmp5F1.f2=
_tmp221,_tmp5F1)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp221,_tmp21E)){struct _tuple13 _tmp5F2;return(_tmp5F2.f1=outflow,((_tmp5F2.f2=
_tmp21E,_tmp5F2)));}{struct _RegionHandle*_tmp237=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5F5;struct Cyc_CfFlowInfo_JoinEnv _tmp5F4;struct Cyc_CfFlowInfo_JoinEnv _tmp238=(
_tmp5F4.fenv=fenv,((_tmp5F4.pile=((_tmp5F5=_region_malloc(_tmp237,sizeof(*
_tmp5F5)),((_tmp5F5->rgn=_tmp237,((_tmp5F5->places=0,_tmp5F5)))))),((_tmp5F4.d1=
_tmp22A,((_tmp5F4.d2=_tmp22D,_tmp5F4)))))));void*_tmp239=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp238,0,_tmp21E,
_tmp221);struct _tuple13 _tmp5F6;return(_tmp5F6.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp238.pile,all_changed,_tmp230),_tmp231,_tmp232),((_tmp5F6.f2=_tmp239,
_tmp5F6)));};_LLF0:;_LLF1: {struct Cyc_Core_Impossible_struct _tmp5FC;const char*
_tmp5FB;struct Cyc_Core_Impossible_struct*_tmp5FA;(int)_throw((void*)((_tmp5FA=
_cycalloc(sizeof(*_tmp5FA)),((_tmp5FA[0]=((_tmp5FC.tag=Cyc_Core_Impossible,((
_tmp5FC.f1=((_tmp5FB="join_flow_and_rval: BottomFL outflow",_tag_dyneither(
_tmp5FB,sizeof(char),37))),_tmp5FC)))),_tmp5FA)))));}_LLE9:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp603(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp602,unsigned int*_tmp601,void***_tmp5FF,struct
_dyneither_ptr*_tmp249,struct _dyneither_ptr*_tmp245){for(*_tmp602=0;*_tmp602 < *
_tmp601;(*_tmp602)++){(*_tmp5FF)[*_tmp602]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp602,((void**)(*_tmp245).curr)[(int)*_tmp602],*((void**)
_check_dyneither_subscript(*_tmp249,sizeof(void*),(int)*_tmp602)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5FD;
struct _tuple0 _tmp241=(_tmp5FD.f1=r1,((_tmp5FD.f2=r2,_tmp5FD)));void*_tmp242;int
_tmp244;struct _dyneither_ptr _tmp245;void*_tmp246;int _tmp248;struct _dyneither_ptr
_tmp249;_LLF3: _tmp242=_tmp241.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp243=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp242;if(_tmp243->tag != 7)goto _LLF5;
else{_tmp244=_tmp243->f1;_tmp245=_tmp243->f2;}};_tmp246=_tmp241.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp247=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp246;if(_tmp247->tag != 7)goto
_LLF5;else{_tmp248=_tmp247->f1;_tmp249=_tmp247->f2;}};_LLF4: {unsigned int
_tmp602;unsigned int _tmp601;struct _dyneither_ptr _tmp600;void**_tmp5FF;
unsigned int _tmp5FE;struct _dyneither_ptr new_d=(_tmp5FE=_get_dyneither_size(
_tmp245,sizeof(void*)),((_tmp5FF=(void**)_region_malloc(((env->joinenv).fenv)->r,
_check_times(sizeof(void*),_tmp5FE)),((_tmp600=_tag_dyneither(_tmp5FF,sizeof(
void*),_tmp5FE),((((_tmp601=_tmp5FE,_tmp603(& env,& _tmp602,& _tmp601,& _tmp5FF,&
_tmp249,& _tmp245))),_tmp600)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp245,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp245.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp245,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp249,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp606;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp605;return(void*)((_tmp605=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp605)),((_tmp605[0]=((_tmp606.tag=7,((_tmp606.f1=_tmp244,((_tmp606.f2=new_d,
_tmp606)))))),_tmp605))));};}_LLF5:;_LLF6: {struct Cyc_Core_Impossible_struct
_tmp60C;const char*_tmp60B;struct Cyc_Core_Impossible_struct*_tmp60A;(int)_throw((
void*)((_tmp60A=_cycalloc(sizeof(*_tmp60A)),((_tmp60A[0]=((_tmp60C.tag=Cyc_Core_Impossible,((
_tmp60C.f1=((_tmp60B="after_absRval -- non-aggregates!",_tag_dyneither(_tmp60B,
sizeof(char),33))),_tmp60C)))),_tmp60A)))));}_LLF2:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp253=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp254=env->changed;{struct Cyc_List_List*_tmp60D;*env->last_field_cell=((
_tmp60D=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp60D)),((_tmp60D->hd=(
void*)key,((_tmp60D->tl=0,_tmp60D))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp256=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp253;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp254;return _tmp256;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp60E;*env->curr_place=((_tmp60E.root=root,((_tmp60E.fields=
0,_tmp60E))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple25 _tmp60F;struct _tuple25
_tmp259=(_tmp60F.f1=f1,((_tmp60F.f2=f2,_tmp60F)));union Cyc_CfFlowInfo_FlowInfo
_tmp25A;int _tmp25B;union Cyc_CfFlowInfo_FlowInfo _tmp25C;int _tmp25D;union Cyc_CfFlowInfo_FlowInfo
_tmp25E;struct _tuple12 _tmp25F;struct Cyc_Dict_Dict _tmp260;struct Cyc_List_List*
_tmp261;struct Cyc_CfFlowInfo_ConsumeInfo _tmp262;union Cyc_CfFlowInfo_FlowInfo
_tmp263;struct _tuple12 _tmp264;struct Cyc_Dict_Dict _tmp265;struct Cyc_List_List*
_tmp266;struct Cyc_CfFlowInfo_ConsumeInfo _tmp267;_LLF8: _tmp25A=_tmp259.f1;if((
_tmp25A.BottomFL).tag != 1)goto _LLFA;_tmp25B=(int)(_tmp25A.BottomFL).val;_LLF9:
goto _LLFB;_LLFA: _tmp25C=_tmp259.f2;if((_tmp25C.BottomFL).tag != 1)goto _LLFC;
_tmp25D=(int)(_tmp25C.BottomFL).val;_LLFB: return Cyc_CfFlowInfo_BottomFL();_LLFC:
_tmp25E=_tmp259.f1;if((_tmp25E.ReachableFL).tag != 2)goto _LLF7;_tmp25F=(struct
_tuple12)(_tmp25E.ReachableFL).val;_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_tmp262=
_tmp25F.f3;_tmp263=_tmp259.f2;if((_tmp263.ReachableFL).tag != 2)goto _LLF7;_tmp264=(
struct _tuple12)(_tmp263.ReachableFL).val;_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;
_tmp267=_tmp264.f3;_LLFD: if(((_tmp260.t == _tmp265.t  && _tmp261 == _tmp266) && 
_tmp262.may_consume == _tmp267.may_consume) && (_tmp262.consumed).t == (_tmp267.consumed).t)
return f1;{struct _RegionHandle*_tmp268=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp269=
fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp615;struct Cyc_CfFlowInfo_EscPile*
_tmp614;struct Cyc_CfFlowInfo_AfterEnv _tmp613;struct Cyc_CfFlowInfo_AfterEnv
_tmp26A=(_tmp613.joinenv=((_tmp615.fenv=fenv,((_tmp615.pile=((_tmp614=
_region_malloc(_tmp268,sizeof(*_tmp614)),((_tmp614->rgn=_tmp268,((_tmp614->places=
0,_tmp614)))))),((_tmp615.d1=_tmp260,((_tmp615.d2=_tmp265,_tmp615)))))))),((
_tmp613.chg1=chg1,((_tmp613.chg2=chg2,((_tmp613.curr_place=_tmp269,((_tmp613.last_field_cell=(
struct Cyc_List_List**)& _tmp269->fields,((_tmp613.changed=Cyc_CfFlowInfo_Neither,
_tmp613)))))))))));struct Cyc_Dict_Dict _tmp26B=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp26A,_tmp260,_tmp265);struct Cyc_List_List*_tmp26C=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp261,_tmp266);struct Cyc_CfFlowInfo_ConsumeInfo _tmp26D=Cyc_CfFlowInfo_and_consume(
fenv->r,_tmp262,_tmp267);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp26A.joinenv).pile,all_changed,_tmp26B),_tmp26C,_tmp26D);};_LLF7:;}
static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*l1,struct
Cyc_List_List*l2);static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*
l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp272;enum Cyc_Absyn_Primop _tmp273;void*_tmp274;struct Cyc_CfFlowInfo_TagCmp*
_tmp271=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp272=*_tmp271;_tmp273=_tmp272.cmp;
_tmp274=_tmp272.bd;{struct Cyc_List_List*_tmp275=l1;for(0;_tmp275 != 0;_tmp275=
_tmp275->tl){struct Cyc_CfFlowInfo_TagCmp _tmp277;enum Cyc_Absyn_Primop _tmp278;
void*_tmp279;struct Cyc_CfFlowInfo_TagCmp*_tmp276=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp275->hd;_tmp277=*_tmp276;_tmp278=_tmp277.cmp;_tmp279=_tmp277.bd;if(_tmp278 == 
_tmp273  && Cyc_Tcutil_typecmp(_tmp279,_tmp274)== 0)break;}if(_tmp275 == 0)return 0;};}
return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp616;struct _tuple0 _tmp27B=(_tmp616.f1=
r1,((_tmp616.f2=r2,_tmp616)));void*_tmp27C;struct Cyc_CfFlowInfo_Place*_tmp27E;
void*_tmp27F;struct Cyc_CfFlowInfo_Place*_tmp281;void*_tmp282;void*_tmp284;void*
_tmp286;int _tmp288;struct _dyneither_ptr _tmp289;void*_tmp28A;int _tmp28C;struct
_dyneither_ptr _tmp28D;void*_tmp28E;void*_tmp290;void*_tmp292;void*_tmp294;void*
_tmp296;void*_tmp298;void*_tmp29A;void*_tmp29C;void*_tmp29E;struct Cyc_List_List*
_tmp2A0;void*_tmp2A1;struct Cyc_List_List*_tmp2A3;void*_tmp2A4;_LLFF: _tmp27C=
_tmp27B.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp27D=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27C;if(_tmp27D->tag != 5)goto _LL101;else{_tmp27E=_tmp27D->f1;}};_tmp27F=
_tmp27B.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp280=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27F;if(_tmp280->tag != 5)goto _LL101;else{_tmp281=_tmp280->f1;}};_LL100: return
Cyc_CfFlowInfo_place_cmp(_tmp27E,_tmp281)== 0;_LL101: _tmp282=_tmp27B.f1;{struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp283=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp282;if(_tmp283->tag != 5)goto _LL103;};_LL102: goto _LL104;_LL103: _tmp284=
_tmp27B.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp285=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp284;if(_tmp285->tag != 5)goto _LL105;};_LL104: return 0;_LL105: _tmp286=_tmp27B.f1;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp287=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp286;if(_tmp287->tag != 7)goto _LL107;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}};
_tmp28A=_tmp27B.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp28B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp28A;if(_tmp28B->tag != 7)goto _LL107;else{_tmp28C=_tmp28B->f1;_tmp28D=_tmp28B->f2;}};
_LL106: if((void**)_tmp289.curr == (void**)_tmp28D.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp289,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp289.curr)[i],*((
void**)_check_dyneither_subscript(_tmp28D,sizeof(void*),i))))return 0;}}return 1;
_LL107: _tmp28E=_tmp27B.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp28F=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp28E;if(_tmp28F->tag != 1)goto _LL109;};
_tmp290=_tmp27B.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp291=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp290;if(_tmp291->tag != 2)goto _LL109;};_LL108: return 1;_LL109: _tmp292=_tmp27B.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp293=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp292;if(_tmp293->tag != 2)goto _LL10B;};_LL10A: return 0;_LL10B: _tmp294=_tmp27B.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp295=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp294;if(_tmp295->tag != 0)goto _LL10D;};_LL10C: goto _LL10E;_LL10D: _tmp296=
_tmp27B.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp297=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp296;if(_tmp297->tag != 1)goto _LL10F;};_LL10E: return 0;_LL10F: _tmp298=_tmp27B.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp299=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp298;if(_tmp299->tag != 4)goto _LL111;};_tmp29A=_tmp27B.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp29B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp29A;if(_tmp29B->tag != 4)goto _LL111;};
_LL110: goto _LLFE;_LL111: _tmp29C=_tmp27B.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp29D=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp29C;if(_tmp29D->tag != 4)goto _LL113;};
_LL112: return 0;_LL113: _tmp29E=_tmp27B.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp29F=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29E;if(_tmp29F->tag != 6)goto
_LL115;else{_tmp2A0=_tmp29F->f1;}};_tmp2A1=_tmp27B.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp2A2=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp2A1;if(_tmp2A2->tag != 6)goto
_LL115;else{_tmp2A3=_tmp2A2->f1;}};_LL114: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp2A0,_tmp2A3);_LL115: _tmp2A4=_tmp27B.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp2A5=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp2A4;if(_tmp2A5->tag != 6)goto
_LL117;};_LL116: return 0;_LL117:;_LL118: goto _LLFE;_LLFE:;}{struct _tuple23 _tmp617;
struct _tuple23 _tmp2A7=(_tmp617.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp617.f2=
Cyc_CfFlowInfo_initlevel_approx(r2),_tmp617)));enum Cyc_CfFlowInfo_InitLevel
_tmp2A8;enum Cyc_CfFlowInfo_InitLevel _tmp2A9;enum Cyc_CfFlowInfo_InitLevel
_tmp2AA;enum Cyc_CfFlowInfo_InitLevel _tmp2AB;enum Cyc_CfFlowInfo_InitLevel
_tmp2AC;enum Cyc_CfFlowInfo_InitLevel _tmp2AD;_LL11A: _tmp2A8=_tmp2A7.f1;if(
_tmp2A8 != Cyc_CfFlowInfo_AllIL)goto _LL11C;_tmp2A9=_tmp2A7.f2;if(_tmp2A9 != Cyc_CfFlowInfo_AllIL)
goto _LL11C;_LL11B: return 1;_LL11C: _tmp2AA=_tmp2A7.f2;if(_tmp2AA != Cyc_CfFlowInfo_NoneIL)
goto _LL11E;_LL11D: return 1;_LL11E: _tmp2AB=_tmp2A7.f1;if(_tmp2AB != Cyc_CfFlowInfo_NoneIL)
goto _LL120;_LL11F: return 0;_LL120: _tmp2AC=_tmp2A7.f2;if(_tmp2AC != Cyc_CfFlowInfo_ThisIL)
goto _LL122;_LL121: return 1;_LL122: _tmp2AD=_tmp2A7.f1;if(_tmp2AD != Cyc_CfFlowInfo_ThisIL)
goto _LL119;_LL123: return 0;_LL119:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2AE=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp2AF=r2s;for(0;_tmp2AF != 0;_tmp2AF=
_tmp2AF->tl){struct Cyc_CfFlowInfo_Reln*_tmp2B0=(struct Cyc_CfFlowInfo_Reln*)
_tmp2AF->hd;if(_tmp2AE == _tmp2B0  || _tmp2AE->vd == _tmp2B0->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2AE->rop,_tmp2B0->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume
!= c2.may_consume){struct Cyc_List_List*_tmp2B1=c1.may_consume;struct Cyc_List_List*
_tmp2B2=c2.may_consume;for(0;_tmp2B2 != 0  && _tmp2B1 != 0;_tmp2B2=_tmp2B2->tl){if((
unsigned int)_tmp2B1 == (unsigned int)_tmp2B2)goto compare_consumed_set;{int
_tmp2B3=Cyc_CfFlowInfo_place_cmp((struct Cyc_CfFlowInfo_Place*)_tmp2B1->hd,(
struct Cyc_CfFlowInfo_Place*)_tmp2B2->hd);if(_tmp2B3 == 0)_tmp2B1=_tmp2B1->tl;};}
if(_tmp2B1 != 0)return 0;}compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2))Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union
Cyc_CfFlowInfo_FlowInfo f2){struct _tuple25 _tmp618;struct _tuple25 _tmp2B5=(_tmp618.f1=
f1,((_tmp618.f2=f2,_tmp618)));union Cyc_CfFlowInfo_FlowInfo _tmp2B6;int _tmp2B7;
union Cyc_CfFlowInfo_FlowInfo _tmp2B8;int _tmp2B9;union Cyc_CfFlowInfo_FlowInfo
_tmp2BA;struct _tuple12 _tmp2BB;struct Cyc_Dict_Dict _tmp2BC;struct Cyc_List_List*
_tmp2BD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2BE;union Cyc_CfFlowInfo_FlowInfo
_tmp2BF;struct _tuple12 _tmp2C0;struct Cyc_Dict_Dict _tmp2C1;struct Cyc_List_List*
_tmp2C2;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C3;_LL125: _tmp2B6=_tmp2B5.f1;if((
_tmp2B6.BottomFL).tag != 1)goto _LL127;_tmp2B7=(int)(_tmp2B6.BottomFL).val;_LL126:
return 1;_LL127: _tmp2B8=_tmp2B5.f2;if((_tmp2B8.BottomFL).tag != 1)goto _LL129;
_tmp2B9=(int)(_tmp2B8.BottomFL).val;_LL128: return 0;_LL129: _tmp2BA=_tmp2B5.f1;if((
_tmp2BA.ReachableFL).tag != 2)goto _LL124;_tmp2BB=(struct _tuple12)(_tmp2BA.ReachableFL).val;
_tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;_tmp2BE=_tmp2BB.f3;_tmp2BF=_tmp2B5.f2;if((
_tmp2BF.ReachableFL).tag != 2)goto _LL124;_tmp2C0=(struct _tuple12)(_tmp2BF.ReachableFL).val;
_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;_tmp2C3=_tmp2C0.f3;_LL12A: if(((_tmp2BC.t == 
_tmp2C1.t  && _tmp2BD == _tmp2C2) && _tmp2BE.may_consume == _tmp2C3.may_consume)
 && (_tmp2BE.consumed).t == (_tmp2C3.consumed).t)return 1;return(((int(*)(int(*f)(
void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((
int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2BC,_tmp2C1) && Cyc_CfFlowInfo_relns_approx(_tmp2BD,_tmp2C2)) && Cyc_CfFlowInfo_consume_approx(
_tmp2BE,_tmp2C3);_LL124:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v);struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C4=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2C4->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp2C5=
_tmp2C4->rop;struct _tuple11 _tmp2C6;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*
_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C9;_LL12C: if((_tmp2C5.LessVar).tag != 2)goto
_LL12E;_tmp2C6=(struct _tuple11)(_tmp2C5.LessVar).val;_tmp2C7=_tmp2C6.f1;_LL12D:
_tmp2C8=_tmp2C7;goto _LL12F;_LL12E: if((_tmp2C5.LessNumelts).tag != 3)goto _LL130;
_tmp2C8=(struct Cyc_Absyn_Vardecl*)(_tmp2C5.LessNumelts).val;_LL12F: _tmp2C9=
_tmp2C8;goto _LL131;_LL130: if((_tmp2C5.LessEqNumelts).tag != 5)goto _LL132;_tmp2C9=(
struct Cyc_Absyn_Vardecl*)(_tmp2C5.LessEqNumelts).val;_LL131: if(v == _tmp2C9)found=
1;goto _LL12B;_LL132:;_LL133: goto _LL12B;_LL12B:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp2CA=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2CB=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2CB->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp2CC=_tmp2CB->rop;struct
_tuple11 _tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2CE;struct Cyc_Absyn_Vardecl*_tmp2CF;
struct Cyc_Absyn_Vardecl*_tmp2D0;_LL135: if((_tmp2CC.LessVar).tag != 2)goto _LL137;
_tmp2CD=(struct _tuple11)(_tmp2CC.LessVar).val;_tmp2CE=_tmp2CD.f1;_LL136: _tmp2CF=
_tmp2CE;goto _LL138;_LL137: if((_tmp2CC.LessNumelts).tag != 3)goto _LL139;_tmp2CF=(
struct Cyc_Absyn_Vardecl*)(_tmp2CC.LessNumelts).val;_LL138: _tmp2D0=_tmp2CF;goto
_LL13A;_LL139: if((_tmp2CC.LessEqNumelts).tag != 5)goto _LL13B;_tmp2D0=(struct Cyc_Absyn_Vardecl*)(
_tmp2CC.LessEqNumelts).val;_LL13A: if(v == _tmp2D0)continue;goto _LL134;_LL13B:;
_LL13C: goto _LL134;_LL134:;}{struct Cyc_List_List*_tmp619;_tmp2CA=((_tmp619=
_region_malloc(rgn,sizeof(*_tmp619)),((_tmp619->hd=_tmp2CB,((_tmp619->tl=_tmp2CA,
_tmp619))))));};}}return _tmp2CA;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e){{void*_tmp2D2=e->r;void*_tmp2D4;struct Cyc_Absyn_Vardecl*
_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*
_tmp2DE;void*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2E2;_LL13E: {struct Cyc_Absyn_Var_e_struct*
_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)_tmp2D2;if(_tmp2D3->tag != 1)goto _LL140;
else{_tmp2D4=(void*)_tmp2D3->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D5=(struct
Cyc_Absyn_Global_b_struct*)_tmp2D4;if(_tmp2D5->tag != 1)goto _LL140;else{_tmp2D6=
_tmp2D5->f1;}};}}_LL13F: _tmp2DA=_tmp2D6;goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_struct*
_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)_tmp2D2;if(_tmp2D7->tag != 1)goto _LL142;
else{_tmp2D8=(void*)_tmp2D7->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2D9=(struct
Cyc_Absyn_Param_b_struct*)_tmp2D8;if(_tmp2D9->tag != 3)goto _LL142;else{_tmp2DA=
_tmp2D9->f1;}};}}_LL141: _tmp2DE=_tmp2DA;goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_struct*
_tmp2DB=(struct Cyc_Absyn_Var_e_struct*)_tmp2D2;if(_tmp2DB->tag != 1)goto _LL144;
else{_tmp2DC=(void*)_tmp2DB->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2DD=(struct
Cyc_Absyn_Local_b_struct*)_tmp2DC;if(_tmp2DD->tag != 4)goto _LL144;else{_tmp2DE=
_tmp2DD->f1;}};}}_LL143: _tmp2E2=_tmp2DE;goto _LL145;_LL144: {struct Cyc_Absyn_Var_e_struct*
_tmp2DF=(struct Cyc_Absyn_Var_e_struct*)_tmp2D2;if(_tmp2DF->tag != 1)goto _LL146;
else{_tmp2E0=(void*)_tmp2DF->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2E1=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2E0;if(_tmp2E1->tag != 5)goto _LL146;else{_tmp2E2=_tmp2E1->f1;}};}}_LL145: if(!
_tmp2E2->escapes)return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2E2);goto _LL13D;
_LL146:;_LL147: goto _LL13D;_LL13D:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t);struct Cyc_CfFlowInfo_TagCmp*
Cyc_CfFlowInfo_copy_tagcmp(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){
struct Cyc_CfFlowInfo_TagCmp*_tmp61A;return(_tmp61A=_region_malloc(r2,sizeof(*
_tmp61A)),((_tmp61A[0]=*t,_tmp61A)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts);struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*f)(struct _RegionHandle*,
struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*
Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp61B;return(_tmp61B=_region_malloc(r2,sizeof(*
_tmp61B)),((_tmp61B[0]=*r,_tmp61B)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp2E5=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp2E6=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp2E5->vd != _tmp2E6->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp2E5->rop,_tmp2E6->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp2E7=e->r;struct Cyc_Absyn_MallocInfo _tmp2E9;struct Cyc_Absyn_Exp*
_tmp2EA;int _tmp2EB;_LL149: {struct Cyc_Absyn_Malloc_e_struct*_tmp2E8=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2E7;if(_tmp2E8->tag != 34)goto _LL14B;else{_tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E9.num_elts;
_tmp2EB=_tmp2E9.fat_result;if(_tmp2EB != 1)goto _LL14B;}}_LL14A: malloc_loop: {void*
_tmp2EC=_tmp2EA->r;struct Cyc_Absyn_Exp*_tmp2EE;void*_tmp2F0;struct Cyc_Absyn_Vardecl*
_tmp2F2;void*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F6;void*_tmp2F8;struct Cyc_Absyn_Vardecl*
_tmp2FA;void*_tmp2FC;struct Cyc_Absyn_Vardecl*_tmp2FE;_LL14E: {struct Cyc_Absyn_Cast_e_struct*
_tmp2ED=(struct Cyc_Absyn_Cast_e_struct*)_tmp2EC;if(_tmp2ED->tag != 15)goto _LL150;
else{_tmp2EE=_tmp2ED->f2;}}_LL14F: _tmp2EA=_tmp2EE;goto malloc_loop;_LL150: {
struct Cyc_Absyn_Var_e_struct*_tmp2EF=(struct Cyc_Absyn_Var_e_struct*)_tmp2EC;if(
_tmp2EF->tag != 1)goto _LL152;else{_tmp2F0=(void*)_tmp2EF->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2F1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2F0;if(_tmp2F1->tag != 5)goto _LL152;
else{_tmp2F2=_tmp2F1->f1;}};}}_LL151: _tmp2F6=_tmp2F2;goto _LL153;_LL152: {struct
Cyc_Absyn_Var_e_struct*_tmp2F3=(struct Cyc_Absyn_Var_e_struct*)_tmp2EC;if(_tmp2F3->tag
!= 1)goto _LL154;else{_tmp2F4=(void*)_tmp2F3->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp2F5=(struct Cyc_Absyn_Local_b_struct*)_tmp2F4;if(_tmp2F5->tag != 4)goto _LL154;
else{_tmp2F6=_tmp2F5->f1;}};}}_LL153: _tmp2FA=_tmp2F6;goto _LL155;_LL154: {struct
Cyc_Absyn_Var_e_struct*_tmp2F7=(struct Cyc_Absyn_Var_e_struct*)_tmp2EC;if(_tmp2F7->tag
!= 1)goto _LL156;else{_tmp2F8=(void*)_tmp2F7->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp2F9=(struct Cyc_Absyn_Param_b_struct*)_tmp2F8;if(_tmp2F9->tag != 3)goto _LL156;
else{_tmp2FA=_tmp2F9->f1;}};}}_LL155: _tmp2FE=_tmp2FA;goto _LL157;_LL156: {struct
Cyc_Absyn_Var_e_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_struct*)_tmp2EC;if(_tmp2FB->tag
!= 1)goto _LL158;else{_tmp2FC=(void*)_tmp2FB->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp2FD=(struct Cyc_Absyn_Global_b_struct*)_tmp2FC;if(_tmp2FD->tag != 1)goto _LL158;
else{_tmp2FE=_tmp2FD->f1;}};}}_LL157: if(_tmp2FE->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp61E;struct Cyc_List_List*_tmp61D;return(_tmp61D=_region_malloc(rgn,sizeof(*
_tmp61D)),((_tmp61D->hd=((_tmp61E=_region_malloc(rgn,sizeof(*_tmp61E)),((_tmp61E->vd=
_tmp2FE,((_tmp61E->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp61E)))))),((_tmp61D->tl=
r,_tmp61D)))));};_LL158:;_LL159: return r;_LL14D:;}_LL14B:;_LL14C: goto _LL148;
_LL148:;}{void*_tmp301=Cyc_Tcutil_compress(v->type);_LL15B: {struct Cyc_Absyn_IntType_struct*
_tmp302=(struct Cyc_Absyn_IntType_struct*)_tmp301;if(_tmp302->tag != 6)goto _LL15D;}
_LL15C: goto _LL15A;_LL15D:;_LL15E: return r;_LL15A:;}loop: {void*_tmp303=e->r;
struct Cyc_Absyn_Exp*_tmp305;union Cyc_Absyn_Cnst _tmp307;struct _tuple6 _tmp308;int
_tmp309;enum Cyc_Absyn_Primop _tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_List_List
_tmp30D;struct Cyc_List_List*_tmp30E;struct Cyc_List_List _tmp30F;struct Cyc_Absyn_Exp*
_tmp310;enum Cyc_Absyn_Primop _tmp312;struct Cyc_List_List*_tmp313;struct Cyc_List_List
_tmp314;struct Cyc_Absyn_Exp*_tmp315;_LL160: {struct Cyc_Absyn_Cast_e_struct*
_tmp304=(struct Cyc_Absyn_Cast_e_struct*)_tmp303;if(_tmp304->tag != 15)goto _LL162;
else{_tmp305=_tmp304->f2;}}_LL161: e=_tmp305;goto loop;_LL162: {struct Cyc_Absyn_Const_e_struct*
_tmp306=(struct Cyc_Absyn_Const_e_struct*)_tmp303;if(_tmp306->tag != 0)goto _LL164;
else{_tmp307=_tmp306->f1;if((_tmp307.Int_c).tag != 4)goto _LL164;_tmp308=(struct
_tuple6)(_tmp307.Int_c).val;_tmp309=_tmp308.f2;}}_LL163: {struct Cyc_CfFlowInfo_Reln*
_tmp621;struct Cyc_List_List*_tmp620;return(_tmp620=_region_malloc(rgn,sizeof(*
_tmp620)),((_tmp620->hd=((_tmp621=_region_malloc(rgn,sizeof(*_tmp621)),((_tmp621->vd=
v,((_tmp621->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp309),_tmp621)))))),((
_tmp620->tl=r,_tmp620)))));}_LL164: {struct Cyc_Absyn_Primop_e_struct*_tmp30A=(
struct Cyc_Absyn_Primop_e_struct*)_tmp303;if(_tmp30A->tag != 3)goto _LL166;else{
_tmp30B=_tmp30A->f1;if(_tmp30B != Cyc_Absyn_Mod)goto _LL166;_tmp30C=_tmp30A->f2;
if(_tmp30C == 0)goto _LL166;_tmp30D=*_tmp30C;_tmp30E=_tmp30D.tl;if(_tmp30E == 0)
goto _LL166;_tmp30F=*_tmp30E;_tmp310=(struct Cyc_Absyn_Exp*)_tmp30F.hd;}}_LL165:{
void*_tmp318=_tmp310->r;enum Cyc_Absyn_Primop _tmp31A;struct Cyc_List_List*_tmp31B;
struct Cyc_List_List _tmp31C;struct Cyc_Absyn_Exp*_tmp31D;_LL16B: {struct Cyc_Absyn_Primop_e_struct*
_tmp319=(struct Cyc_Absyn_Primop_e_struct*)_tmp318;if(_tmp319->tag != 3)goto _LL16D;
else{_tmp31A=_tmp319->f1;_tmp31B=_tmp319->f2;if(_tmp31B == 0)goto _LL16D;_tmp31C=*
_tmp31B;_tmp31D=(struct Cyc_Absyn_Exp*)_tmp31C.hd;}}_LL16C:{void*_tmp31E=_tmp31D->r;
void*_tmp320;struct Cyc_Absyn_Vardecl*_tmp322;void*_tmp324;struct Cyc_Absyn_Vardecl*
_tmp326;void*_tmp328;struct Cyc_Absyn_Vardecl*_tmp32A;void*_tmp32C;struct Cyc_Absyn_Vardecl*
_tmp32E;_LL170: {struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct Cyc_Absyn_Var_e_struct*)
_tmp31E;if(_tmp31F->tag != 1)goto _LL172;else{_tmp320=(void*)_tmp31F->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp321=(struct Cyc_Absyn_Global_b_struct*)_tmp320;if(
_tmp321->tag != 1)goto _LL172;else{_tmp322=_tmp321->f1;}};}}_LL171: _tmp326=_tmp322;
goto _LL173;_LL172: {struct Cyc_Absyn_Var_e_struct*_tmp323=(struct Cyc_Absyn_Var_e_struct*)
_tmp31E;if(_tmp323->tag != 1)goto _LL174;else{_tmp324=(void*)_tmp323->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp325=(struct Cyc_Absyn_Local_b_struct*)_tmp324;if(
_tmp325->tag != 4)goto _LL174;else{_tmp326=_tmp325->f1;}};}}_LL173: _tmp32A=_tmp326;
goto _LL175;_LL174: {struct Cyc_Absyn_Var_e_struct*_tmp327=(struct Cyc_Absyn_Var_e_struct*)
_tmp31E;if(_tmp327->tag != 1)goto _LL176;else{_tmp328=(void*)_tmp327->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp329=(struct Cyc_Absyn_Param_b_struct*)_tmp328;if(
_tmp329->tag != 3)goto _LL176;else{_tmp32A=_tmp329->f1;}};}}_LL175: _tmp32E=_tmp32A;
goto _LL177;_LL176: {struct Cyc_Absyn_Var_e_struct*_tmp32B=(struct Cyc_Absyn_Var_e_struct*)
_tmp31E;if(_tmp32B->tag != 1)goto _LL178;else{_tmp32C=(void*)_tmp32B->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp32D=(struct Cyc_Absyn_Pat_b_struct*)_tmp32C;if(_tmp32D->tag
!= 5)goto _LL178;else{_tmp32E=_tmp32D->f1;}};}}_LL177: if(_tmp32E->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp624;struct Cyc_List_List*_tmp623;return(_tmp623=
_region_malloc(rgn,sizeof(*_tmp623)),((_tmp623->hd=((_tmp624=_region_malloc(rgn,
sizeof(*_tmp624)),((_tmp624->vd=v,((_tmp624->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp32E),_tmp624)))))),((_tmp623->tl=r,_tmp623)))));};_LL178:;_LL179: goto _LL16F;
_LL16F:;}goto _LL16A;_LL16D:;_LL16E: goto _LL16A;_LL16A:;}goto _LL15F;_LL166: {
struct Cyc_Absyn_Primop_e_struct*_tmp311=(struct Cyc_Absyn_Primop_e_struct*)
_tmp303;if(_tmp311->tag != 3)goto _LL168;else{_tmp312=_tmp311->f1;_tmp313=_tmp311->f2;
if(_tmp313 == 0)goto _LL168;_tmp314=*_tmp313;_tmp315=(struct Cyc_Absyn_Exp*)_tmp314.hd;}}
_LL167:{void*_tmp331=_tmp315->r;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;
void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;_LL17B: {struct Cyc_Absyn_Var_e_struct*
_tmp332=(struct Cyc_Absyn_Var_e_struct*)_tmp331;if(_tmp332->tag != 1)goto _LL17D;
else{_tmp333=(void*)_tmp332->f2;{struct Cyc_Absyn_Global_b_struct*_tmp334=(struct
Cyc_Absyn_Global_b_struct*)_tmp333;if(_tmp334->tag != 1)goto _LL17D;else{_tmp335=
_tmp334->f1;}};}}_LL17C: _tmp339=_tmp335;goto _LL17E;_LL17D: {struct Cyc_Absyn_Var_e_struct*
_tmp336=(struct Cyc_Absyn_Var_e_struct*)_tmp331;if(_tmp336->tag != 1)goto _LL17F;
else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Local_b_struct*_tmp338=(struct
Cyc_Absyn_Local_b_struct*)_tmp337;if(_tmp338->tag != 4)goto _LL17F;else{_tmp339=
_tmp338->f1;}};}}_LL17E: _tmp33D=_tmp339;goto _LL180;_LL17F: {struct Cyc_Absyn_Var_e_struct*
_tmp33A=(struct Cyc_Absyn_Var_e_struct*)_tmp331;if(_tmp33A->tag != 1)goto _LL181;
else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp33C=(struct
Cyc_Absyn_Param_b_struct*)_tmp33B;if(_tmp33C->tag != 3)goto _LL181;else{_tmp33D=
_tmp33C->f1;}};}}_LL180: _tmp341=_tmp33D;goto _LL182;_LL181: {struct Cyc_Absyn_Var_e_struct*
_tmp33E=(struct Cyc_Absyn_Var_e_struct*)_tmp331;if(_tmp33E->tag != 1)goto _LL183;
else{_tmp33F=(void*)_tmp33E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp340=(struct Cyc_Absyn_Pat_b_struct*)
_tmp33F;if(_tmp340->tag != 5)goto _LL183;else{_tmp341=_tmp340->f1;}};}}_LL182: if(
_tmp341->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp627;struct Cyc_List_List*
_tmp626;return(_tmp626=_region_malloc(rgn,sizeof(*_tmp626)),((_tmp626->hd=((
_tmp627=_region_malloc(rgn,sizeof(*_tmp627)),((_tmp627->vd=v,((_tmp627->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp341),_tmp627)))))),((_tmp626->tl=r,_tmp626)))));};_LL183:;_LL184: goto _LL17A;
_LL17A:;}goto _LL15F;_LL168:;_LL169: goto _LL15F;_LL15F:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp344=e1->r;void*_tmp346;struct Cyc_Absyn_Vardecl*_tmp348;void*
_tmp34A;struct Cyc_Absyn_Vardecl*_tmp34C;void*_tmp34E;struct Cyc_Absyn_Vardecl*
_tmp350;void*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;_LL186: {struct Cyc_Absyn_Var_e_struct*
_tmp345=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp345->tag != 1)goto _LL188;
else{_tmp346=(void*)_tmp345->f2;{struct Cyc_Absyn_Global_b_struct*_tmp347=(struct
Cyc_Absyn_Global_b_struct*)_tmp346;if(_tmp347->tag != 1)goto _LL188;else{_tmp348=
_tmp347->f1;}};}}_LL187: _tmp34C=_tmp348;goto _LL189;_LL188: {struct Cyc_Absyn_Var_e_struct*
_tmp349=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp349->tag != 1)goto _LL18A;
else{_tmp34A=(void*)_tmp349->f2;{struct Cyc_Absyn_Param_b_struct*_tmp34B=(struct
Cyc_Absyn_Param_b_struct*)_tmp34A;if(_tmp34B->tag != 3)goto _LL18A;else{_tmp34C=
_tmp34B->f1;}};}}_LL189: _tmp350=_tmp34C;goto _LL18B;_LL18A: {struct Cyc_Absyn_Var_e_struct*
_tmp34D=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp34D->tag != 1)goto _LL18C;
else{_tmp34E=(void*)_tmp34D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp34F=(struct
Cyc_Absyn_Local_b_struct*)_tmp34E;if(_tmp34F->tag != 4)goto _LL18C;else{_tmp350=
_tmp34F->f1;}};}}_LL18B: _tmp354=_tmp350;goto _LL18D;_LL18C: {struct Cyc_Absyn_Var_e_struct*
_tmp351=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp351->tag != 1)goto _LL18E;
else{_tmp352=(void*)_tmp351->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp353=(struct Cyc_Absyn_Pat_b_struct*)
_tmp352;if(_tmp353->tag != 5)goto _LL18E;else{_tmp354=_tmp353->f1;}};}}_LL18D: if(!
_tmp354->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp354,e2);goto
_LL185;_LL18E:;_LL18F: goto _LL185;_LL185:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp62B;void*_tmp62A[1];struct Cyc_String_pa_struct _tmp629;(_tmp629.tag=
0,((_tmp629.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp62A[0]=& _tmp629,Cyc_fprintf(Cyc_stderr,((_tmp62B="%s",
_tag_dyneither(_tmp62B,sizeof(char),3))),_tag_dyneither(_tmp62A,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp358=r->rop;unsigned int _tmp359;struct _tuple11
_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_Absyn_Vardecl*_tmp35C;
unsigned int _tmp35D;unsigned int _tmp35E;struct Cyc_Absyn_Vardecl*_tmp35F;_LL191:
if((_tmp358.EqualConst).tag != 1)goto _LL193;_tmp359=(unsigned int)(_tmp358.EqualConst).val;
_LL192:{const char*_tmp62F;void*_tmp62E[1];struct Cyc_Int_pa_struct _tmp62D;(
_tmp62D.tag=1,((_tmp62D.f1=(unsigned long)((int)_tmp359),((_tmp62E[0]=& _tmp62D,
Cyc_fprintf(Cyc_stderr,((_tmp62F="==%d",_tag_dyneither(_tmp62F,sizeof(char),5))),
_tag_dyneither(_tmp62E,sizeof(void*),1)))))));}goto _LL190;_LL193: if((_tmp358.LessVar).tag
!= 2)goto _LL195;_tmp35A=(struct _tuple11)(_tmp358.LessVar).val;_tmp35B=_tmp35A.f1;
_LL194:{const char*_tmp633;void*_tmp632[1];struct Cyc_String_pa_struct _tmp631;(
_tmp631.tag=0,((_tmp631.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp35B->name)),((_tmp632[0]=& _tmp631,Cyc_fprintf(Cyc_stderr,((_tmp633="<%s",
_tag_dyneither(_tmp633,sizeof(char),4))),_tag_dyneither(_tmp632,sizeof(void*),1)))))));}
goto _LL190;_LL195: if((_tmp358.LessNumelts).tag != 3)goto _LL197;_tmp35C=(struct Cyc_Absyn_Vardecl*)(
_tmp358.LessNumelts).val;_LL196:{const char*_tmp637;void*_tmp636[1];struct Cyc_String_pa_struct
_tmp635;(_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp35C->name)),((_tmp636[0]=& _tmp635,Cyc_fprintf(Cyc_stderr,((
_tmp637="<numelts(%s)",_tag_dyneither(_tmp637,sizeof(char),13))),_tag_dyneither(
_tmp636,sizeof(void*),1)))))));}goto _LL190;_LL197: if((_tmp358.LessConst).tag != 4)
goto _LL199;_tmp35D=(unsigned int)(_tmp358.LessConst).val;_LL198:{const char*
_tmp63B;void*_tmp63A[1];struct Cyc_Int_pa_struct _tmp639;(_tmp639.tag=1,((_tmp639.f1=(
unsigned long)((int)_tmp35D),((_tmp63A[0]=& _tmp639,Cyc_fprintf(Cyc_stderr,((
_tmp63B="<%d",_tag_dyneither(_tmp63B,sizeof(char),4))),_tag_dyneither(_tmp63A,
sizeof(void*),1)))))));}goto _LL190;_LL199: if((_tmp358.LessEqConst).tag != 6)goto
_LL19B;_tmp35E=(unsigned int)(_tmp358.LessEqConst).val;_LL19A:{const char*_tmp63F;
void*_tmp63E[1];struct Cyc_Int_pa_struct _tmp63D;(_tmp63D.tag=1,((_tmp63D.f1=(
unsigned long)((int)_tmp35E),((_tmp63E[0]=& _tmp63D,Cyc_fprintf(Cyc_stderr,((
_tmp63F="<=%d",_tag_dyneither(_tmp63F,sizeof(char),5))),_tag_dyneither(_tmp63E,
sizeof(void*),1)))))));}goto _LL190;_LL19B: if((_tmp358.LessEqNumelts).tag != 5)
goto _LL190;_tmp35F=(struct Cyc_Absyn_Vardecl*)(_tmp358.LessEqNumelts).val;_LL19C:{
const char*_tmp643;void*_tmp642[1];struct Cyc_String_pa_struct _tmp641;(_tmp641.tag=
0,((_tmp641.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp35F->name)),((_tmp642[0]=& _tmp641,Cyc_fprintf(Cyc_stderr,((_tmp643="<=numelts(%s)",
_tag_dyneither(_tmp643,sizeof(char),14))),_tag_dyneither(_tmp642,sizeof(void*),1)))))));}
goto _LL190;_LL190:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp646;void*_tmp645;(
_tmp645=0,Cyc_fprintf(Cyc_stderr,((_tmp646=",",_tag_dyneither(_tmp646,sizeof(
char),2))),_tag_dyneither(_tmp645,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL19D:{const char*_tmp649;void*_tmp648;(
_tmp648=0,Cyc_fprintf(Cyc_stderr,((_tmp649="uninitialized",_tag_dyneither(
_tmp649,sizeof(char),14))),_tag_dyneither(_tmp648,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL19E:{const char*_tmp64C;void*_tmp64B;(_tmp64B=0,Cyc_fprintf(
Cyc_stderr,((_tmp64C="this-initialized",_tag_dyneither(_tmp64C,sizeof(char),17))),
_tag_dyneither(_tmp64B,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL19F:{
const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_fprintf(Cyc_stderr,((_tmp64F="all-initialized",
_tag_dyneither(_tmp64F,sizeof(char),16))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp37A=root;struct Cyc_Absyn_Vardecl*_tmp37C;struct Cyc_Absyn_Exp*
_tmp37E;void*_tmp37F;_LL1A2: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp37B=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp37A;if(_tmp37B->tag != 0)goto _LL1A4;
else{_tmp37C=_tmp37B->f1;}}_LL1A3:{const char*_tmp653;void*_tmp652[1];struct Cyc_String_pa_struct
_tmp651;(_tmp651.tag=0,((_tmp651.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp37C->name)),((_tmp652[0]=& _tmp651,Cyc_fprintf(Cyc_stderr,((
_tmp653="Root(%s)",_tag_dyneither(_tmp653,sizeof(char),9))),_tag_dyneither(
_tmp652,sizeof(void*),1)))))));}goto _LL1A1;_LL1A4: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp37D=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp37A;if(_tmp37D->tag != 1)goto
_LL1A6;else{_tmp37E=_tmp37D->f1;_tmp37F=(void*)_tmp37D->f2;}}_LL1A5:{const char*
_tmp658;void*_tmp657[2];struct Cyc_String_pa_struct _tmp656;struct Cyc_String_pa_struct
_tmp655;(_tmp655.tag=0,((_tmp655.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp37F)),((_tmp656.tag=0,((_tmp656.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp37E)),((_tmp657[
0]=& _tmp656,((_tmp657[1]=& _tmp655,Cyc_fprintf(Cyc_stderr,((_tmp658="MallocPt(%s,%s)",
_tag_dyneither(_tmp658,sizeof(char),16))),_tag_dyneither(_tmp657,sizeof(void*),2)))))))))))));}
goto _LL1A1;_LL1A6: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp380=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp37A;if(_tmp380->tag != 2)goto _LL1A1;}_LL1A7:{const char*_tmp65B;void*_tmp65A;(
_tmp65A=0,Cyc_fprintf(Cyc_stderr,((_tmp65B="InitParam(_,_)",_tag_dyneither(
_tmp65B,sizeof(char),15))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}goto _LL1A1;
_LL1A1:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*
_tmp38A=p->fields;while(_tmp38A != 0){{const char*_tmp65F;void*_tmp65E[1];struct
Cyc_Int_pa_struct _tmp65D;(_tmp65D.tag=1,((_tmp65D.f1=(unsigned long)((int)
_tmp38A->hd),((_tmp65E[0]=& _tmp65D,Cyc_fprintf(Cyc_stderr,((_tmp65F=".%d",
_tag_dyneither(_tmp65F,sizeof(char),4))),_tag_dyneither(_tmp65E,sizeof(void*),1)))))));}
_tmp38A=_tmp38A->tl;}{const char*_tmp662;void*_tmp661;(_tmp661=0,Cyc_fprintf(Cyc_stderr,((
_tmp662=" ",_tag_dyneither(_tmp662,sizeof(char),2))),_tag_dyneither(_tmp661,
sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*
pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp390=1;while(x != 0){if(!_tmp390){{const char*_tmp665;void*_tmp664;(_tmp664=
0,Cyc_fprintf(Cyc_stderr,((_tmp665=", ",_tag_dyneither(_tmp665,sizeof(char),3))),
_tag_dyneither(_tmp664,sizeof(void*),0)));}_tmp390=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp668;void*_tmp667;(_tmp667=0,Cyc_fprintf(Cyc_stderr,((_tmp668="\n",
_tag_dyneither(_tmp668,sizeof(char),2))),_tag_dyneither(_tmp667,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp395=_new_region("r");
struct _RegionHandle*r=& _tmp395;_push_region(r);{struct _handler_cons _tmp396;
_push_handler(& _tmp396);{int _tmp398=0;if(setjmp(_tmp396.handler))_tmp398=1;if(!
_tmp398){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple18 _tmp399=*((
struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp66B;void*_tmp66A;(_tmp66A=0,Cyc_fprintf(Cyc_stderr,((
_tmp66B="{ ",_tag_dyneither(_tmp66B,sizeof(char),3))),_tag_dyneither(_tmp66A,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
iter,& _tmp399)){void*_tmp39C=_tmp399.f1;if(!first){{const char*_tmp66E;void*
_tmp66D;(_tmp66D=0,Cyc_fprintf(Cyc_stderr,((_tmp66E=", ",_tag_dyneither(_tmp66E,
sizeof(char),3))),_tag_dyneither(_tmp66D,sizeof(void*),0)));}first=0;}pr(_tmp39C);}{
const char*_tmp671;void*_tmp670;(_tmp670=0,Cyc_fprintf(Cyc_stderr,((_tmp671="}\n",
_tag_dyneither(_tmp671,sizeof(char),3))),_tag_dyneither(_tmp670,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp397=(void*)_exn_thrown;void*_tmp3A2=_tmp397;_LL1A9:;
_LL1AA:{const char*_tmp674;void*_tmp673;(_tmp673=0,Cyc_fprintf(Cyc_stderr,((
_tmp674="{ }\n",_tag_dyneither(_tmp674,sizeof(char),5))),_tag_dyneither(_tmp673,
sizeof(void*),0)));}_npop_handler(0);return;_LL1AB:;_LL1AC:(void)_throw(_tmp3A2);
_LL1A8:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp3A5=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3AA;enum Cyc_CfFlowInfo_InitLevel
_tmp3AC;struct Cyc_CfFlowInfo_Place*_tmp3AE;int _tmp3B1;struct _dyneither_ptr
_tmp3B2;_LL1AE: {struct Cyc_CfFlowInfo_Zero_struct*_tmp3A6=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3A5;if(_tmp3A6->tag != 0)goto _LL1B0;}_LL1AF:{const char*_tmp677;void*_tmp676;(
_tmp676=0,Cyc_fprintf(Cyc_stderr,((_tmp677="Zero",_tag_dyneither(_tmp677,sizeof(
char),5))),_tag_dyneither(_tmp676,sizeof(void*),0)));}goto _LL1AD;_LL1B0: {struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp3A7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp3A5;if(_tmp3A7->tag != 1)goto _LL1B2;}_LL1B1:{const char*_tmp67A;void*_tmp679;(
_tmp679=0,Cyc_fprintf(Cyc_stderr,((_tmp67A="NotZeroAll",_tag_dyneither(_tmp67A,
sizeof(char),11))),_tag_dyneither(_tmp679,sizeof(void*),0)));}goto _LL1AD;_LL1B2: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp3A8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp3A5;if(_tmp3A8->tag != 2)goto _LL1B4;}_LL1B3:{const char*_tmp67D;void*_tmp67C;(
_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D="NotZeroThis",_tag_dyneither(_tmp67D,
sizeof(char),12))),_tag_dyneither(_tmp67C,sizeof(void*),0)));}goto _LL1AD;_LL1B4: {
struct Cyc_CfFlowInfo_UnknownR_struct*_tmp3A9=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3A5;if(_tmp3A9->tag != 3)goto _LL1B6;else{_tmp3AA=_tmp3A9->f1;}}_LL1B5:{const
char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680="Unknown(",
_tag_dyneither(_tmp680,sizeof(char),9))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_initlevel(_tmp3AA);{const char*_tmp683;void*_tmp682;(_tmp682=
0,Cyc_fprintf(Cyc_stderr,((_tmp683=")",_tag_dyneither(_tmp683,sizeof(char),2))),
_tag_dyneither(_tmp682,sizeof(void*),0)));}goto _LL1AD;_LL1B6: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp3AB=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp3A5;if(_tmp3AB->tag != 4)goto _LL1B8;
else{_tmp3AC=_tmp3AB->f1;}}_LL1B7:{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(
Cyc_stderr,((_tmp686="Esc(",_tag_dyneither(_tmp686,sizeof(char),5))),
_tag_dyneither(_tmp685,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3AC);{const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((
_tmp689=")",_tag_dyneither(_tmp689,sizeof(char),2))),_tag_dyneither(_tmp688,
sizeof(void*),0)));}goto _LL1AD;_LL1B8: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3AD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3A5;if(_tmp3AD->tag != 5)goto
_LL1BA;else{_tmp3AE=_tmp3AD->f1;}}_LL1B9:{const char*_tmp68C;void*_tmp68B;(
_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="AddrOf(",_tag_dyneither(_tmp68C,
sizeof(char),8))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3AE);{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((
_tmp68F=")",_tag_dyneither(_tmp68F,sizeof(char),2))),_tag_dyneither(_tmp68E,
sizeof(void*),0)));}goto _LL1AD;_LL1BA: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp3AF=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp3A5;if(_tmp3AF->tag != 6)goto
_LL1BC;}_LL1BB:{const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(Cyc_stderr,((
_tmp692="TagCmps(?)",_tag_dyneither(_tmp692,sizeof(char),11))),_tag_dyneither(
_tmp691,sizeof(void*),0)));}goto _LL1AD;_LL1BC: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3B0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3A5;if(_tmp3B0->tag != 7)goto
_LL1AD;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;}}_LL1BD: if(_tmp3B1){const
char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_fprintf(Cyc_stderr,((_tmp695="AggrUnion{",
_tag_dyneither(_tmp695,sizeof(char),11))),_tag_dyneither(_tmp694,sizeof(void*),0)));}
else{const char*_tmp698;void*_tmp697;(_tmp697=0,Cyc_fprintf(Cyc_stderr,((_tmp698="AggrStruct{",
_tag_dyneither(_tmp698,sizeof(char),12))),_tag_dyneither(_tmp697,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp3B2,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3B2.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3B2,sizeof(void*))){
const char*_tmp69B;void*_tmp69A;(_tmp69A=0,Cyc_fprintf(Cyc_stderr,((_tmp69B=",",
_tag_dyneither(_tmp69B,sizeof(char),2))),_tag_dyneither(_tmp69A,sizeof(void*),0)));}}}{
const char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E="}",
_tag_dyneither(_tmp69E,sizeof(char),2))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}
goto _LL1AD;_LL1AD:;}static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){{const
char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,((_tmp6A1="    ",
_tag_dyneither(_tmp6A1,sizeof(char),5))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);{const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(
Cyc_stderr,((_tmp6A4=" --> ",_tag_dyneither(_tmp6A4,sizeof(char),6))),
_tag_dyneither(_tmp6A3,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="\n",
_tag_dyneither(_tmp6A7,sizeof(char),2))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(
Cyc_CfFlowInfo_print_flow_mapping,d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo
f);void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp3D5=f;int _tmp3D6;struct _tuple12 _tmp3D7;struct Cyc_Dict_Dict _tmp3D8;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3D9;_LL1BF: if((_tmp3D5.BottomFL).tag != 1)goto _LL1C1;_tmp3D6=(int)(_tmp3D5.BottomFL).val;
_LL1C0:{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((
_tmp6AA="  BottomFL\n",_tag_dyneither(_tmp6AA,sizeof(char),12))),_tag_dyneither(
_tmp6A9,sizeof(void*),0)));}goto _LL1BE;_LL1C1: if((_tmp3D5.ReachableFL).tag != 2)
goto _LL1BE;_tmp3D7=(struct _tuple12)(_tmp3D5.ReachableFL).val;_tmp3D8=_tmp3D7.f1;
_tmp3D9=_tmp3D7.f3;_LL1C2:{const char*_tmp6AD;void*_tmp6AC;(_tmp6AC=0,Cyc_fprintf(
Cyc_stderr,((_tmp6AD="  ReachableFL:\n",_tag_dyneither(_tmp6AD,sizeof(char),16))),
_tag_dyneither(_tmp6AC,sizeof(void*),0)));}((void(*)(void(*f)(void*,void*),
struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,_tmp3D8);{
const char*_tmp6B0;void*_tmp6AF;(_tmp6AF=0,Cyc_fprintf(Cyc_stderr,((_tmp6B0="\n    consumed: ",
_tag_dyneither(_tmp6B0,sizeof(char),16))),_tag_dyneither(_tmp6AF,sizeof(void*),0)));}((
void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(
_tmp3D9.consumed,Cyc_CfFlowInfo_print_place);{const char*_tmp6B3;void*_tmp6B2;(
_tmp6B2=0,Cyc_fprintf(Cyc_stderr,((_tmp6B3="\n    may_consume: ",_tag_dyneither(
_tmp6B3,sizeof(char),19))),_tag_dyneither(_tmp6B2,sizeof(void*),0)));}((void(*)(
struct Cyc_List_List*x,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp3D9.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp6B6;void*_tmp6B5;(
_tmp6B5=0,Cyc_fprintf(Cyc_stderr,((_tmp6B6="\n",_tag_dyneither(_tmp6B6,sizeof(
char),2))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}goto _LL1BE;_LL1BE:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp3E4=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3F0;struct Cyc_Absyn_DatatypeInfo
_tmp3F2;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_Absyn_AggrInfo
_tmp3F7;struct Cyc_List_List*_tmp3F8;struct Cyc_Absyn_DatatypeFieldInfo _tmp3FA;
struct Cyc_List_List*_tmp3FB;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_PtrInfo
_tmp3FF;void*_tmp400;struct Cyc_Absyn_PtrAtts _tmp401;void*_tmp402;struct Cyc_List_List*
_tmp405;struct Cyc_List_List*_tmp407;struct Cyc_Absyn_ArrayInfo _tmp409;void*
_tmp40A;void*_tmp40C;void*_tmp40E;void*_tmp410;void*_tmp411;void*_tmp413;_LL1C4: {
struct Cyc_Absyn_VoidType_struct*_tmp3E5=(struct Cyc_Absyn_VoidType_struct*)
_tmp3E4;if(_tmp3E5->tag != 0)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_IntType_struct*
_tmp3E6=(struct Cyc_Absyn_IntType_struct*)_tmp3E4;if(_tmp3E6->tag != 6)goto _LL1C8;}
_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_FloatType_struct*_tmp3E7=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E4;if(_tmp3E7->tag != 7)goto _LL1CA;}_LL1C9: goto _LL1CB;_LL1CA: {struct Cyc_Absyn_DoubleType_struct*
_tmp3E8=(struct Cyc_Absyn_DoubleType_struct*)_tmp3E4;if(_tmp3E8->tag != 8)goto
_LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_EnumType_struct*_tmp3E9=(
struct Cyc_Absyn_EnumType_struct*)_tmp3E4;if(_tmp3E9->tag != 14)goto _LL1CE;}_LL1CD:
goto _LL1CF;_LL1CE: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3EA=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3E4;if(_tmp3EA->tag != 15)goto _LL1D0;}_LL1CF: goto _LL1D1;_LL1D0: {struct Cyc_Absyn_TagType_struct*
_tmp3EB=(struct Cyc_Absyn_TagType_struct*)_tmp3E4;if(_tmp3EB->tag != 20)goto _LL1D2;}
_LL1D1: goto _LL1D3;_LL1D2: {struct Cyc_Absyn_HeapRgn_struct*_tmp3EC=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp3E4;if(_tmp3EC->tag != 21)goto _LL1D4;}_LL1D3: goto _LL1D5;_LL1D4: {struct Cyc_Absyn_Evar_struct*
_tmp3ED=(struct Cyc_Absyn_Evar_struct*)_tmp3E4;if(_tmp3ED->tag != 1)goto _LL1D6;}
_LL1D5: return 0;_LL1D6: {struct Cyc_Absyn_UniqueRgn_struct*_tmp3EE=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp3E4;if(_tmp3EE->tag != 22)goto _LL1D8;}_LL1D7: {void*_tmp415=rgn;_LL1F9: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp416=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp415;if(_tmp416->tag != 0)goto _LL1FB;}_LL1FA: return 1;_LL1FB:;_LL1FC: return 0;
_LL1F8:;}_LL1D8: {struct Cyc_Absyn_VarType_struct*_tmp3EF=(struct Cyc_Absyn_VarType_struct*)
_tmp3E4;if(_tmp3EF->tag != 2)goto _LL1DA;else{_tmp3F0=_tmp3EF->f1;}}_LL1D9: {void*
_tmp417=rgn;struct Cyc_Absyn_Tvar*_tmp41A;_LL1FE: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp418=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp417;if(_tmp418->tag != 0)
goto _LL200;}_LL1FF: return 0;_LL200: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp419=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp417;if(_tmp419->tag != 1)goto _LL1FD;
else{_tmp41A=_tmp419->f1;}}_LL201: return Cyc_Absyn_tvar_cmp(_tmp3F0,_tmp41A)== 0;
_LL1FD:;}_LL1DA: {struct Cyc_Absyn_DatatypeType_struct*_tmp3F1=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp3E4;if(_tmp3F1->tag != 3)goto _LL1DC;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F2.targs;}}
_LL1DB: _tmp3F5=_tmp3F3;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_TypedefType_struct*
_tmp3F4=(struct Cyc_Absyn_TypedefType_struct*)_tmp3E4;if(_tmp3F4->tag != 18)goto
_LL1DE;else{_tmp3F5=_tmp3F4->f2;}}_LL1DD: _tmp3F8=_tmp3F5;goto _LL1DF;_LL1DE: {
struct Cyc_Absyn_AggrType_struct*_tmp3F6=(struct Cyc_Absyn_AggrType_struct*)
_tmp3E4;if(_tmp3F6->tag != 12)goto _LL1E0;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F7.targs;}}
_LL1DF: _tmp3FB=_tmp3F8;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3F9=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3E4;if(_tmp3F9->tag != 4)
goto _LL1E2;else{_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3FA.targs;}}_LL1E1: _tmp3FD=
_tmp3FB;goto _LL1E3;_LL1E2: {struct Cyc_Absyn_JoinEff_struct*_tmp3FC=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3E4;if(_tmp3FC->tag != 24)goto _LL1E4;else{_tmp3FD=_tmp3FC->f1;}}_LL1E3: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp3FD);_LL1E4: {struct Cyc_Absyn_PointerType_struct*
_tmp3FE=(struct Cyc_Absyn_PointerType_struct*)_tmp3E4;if(_tmp3FE->tag != 5)goto
_LL1E6;else{_tmp3FF=_tmp3FE->f1;_tmp400=_tmp3FF.elt_typ;_tmp401=_tmp3FF.ptr_atts;
_tmp402=_tmp401.rgn;}}_LL1E5: return Cyc_CfFlowInfo_contains_region(rgn,_tmp402)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp400);_LL1E6: {struct Cyc_Absyn_FnType_struct*
_tmp403=(struct Cyc_Absyn_FnType_struct*)_tmp3E4;if(_tmp403->tag != 10)goto _LL1E8;}
_LL1E7: return 0;_LL1E8: {struct Cyc_Absyn_TupleType_struct*_tmp404=(struct Cyc_Absyn_TupleType_struct*)
_tmp3E4;if(_tmp404->tag != 11)goto _LL1EA;else{_tmp405=_tmp404->f1;}}_LL1E9: for(0;
_tmp405 != 0;_tmp405=_tmp405->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple19*)_tmp405->hd)).f2))return 1;}return 0;_LL1EA: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp406=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3E4;if(_tmp406->tag != 13)goto
_LL1EC;else{_tmp407=_tmp406->f2;}}_LL1EB: for(0;_tmp407 != 0;_tmp407=_tmp407->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp407->hd)->type))
return 1;}return 0;_LL1EC: {struct Cyc_Absyn_ArrayType_struct*_tmp408=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3E4;if(_tmp408->tag != 9)goto _LL1EE;else{_tmp409=_tmp408->f1;_tmp40A=_tmp409.elt_type;}}
_LL1ED: _tmp40C=_tmp40A;goto _LL1EF;_LL1EE: {struct Cyc_Absyn_AccessEff_struct*
_tmp40B=(struct Cyc_Absyn_AccessEff_struct*)_tmp3E4;if(_tmp40B->tag != 23)goto
_LL1F0;else{_tmp40C=(void*)_tmp40B->f1;}}_LL1EF: _tmp40E=_tmp40C;goto _LL1F1;
_LL1F0: {struct Cyc_Absyn_RgnsEff_struct*_tmp40D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3E4;if(_tmp40D->tag != 25)goto _LL1F2;else{_tmp40E=(void*)_tmp40D->f1;}}_LL1F1:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp40E);_LL1F2: {struct Cyc_Absyn_DynRgnType_struct*
_tmp40F=(struct Cyc_Absyn_DynRgnType_struct*)_tmp3E4;if(_tmp40F->tag != 17)goto
_LL1F4;else{_tmp410=(void*)_tmp40F->f1;_tmp411=(void*)_tmp40F->f2;}}_LL1F3:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp410) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp411);_LL1F4: {struct Cyc_Absyn_RgnHandleType_struct*_tmp412=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3E4;if(_tmp412->tag != 16)goto _LL1F6;else{_tmp413=(void*)_tmp412->f1;}}_LL1F5:
return 0;_LL1F6: {struct Cyc_Absyn_ValueofType_struct*_tmp414=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3E4;if(_tmp414->tag != 19)goto _LL1C3;}_LL1F7: return 0;_LL1C3:;}struct _tuple27{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple27*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(
struct _tuple27*env,void*root,void*rval){struct _tuple27 _tmp41C;struct Cyc_CfFlowInfo_FlowEnv*
_tmp41D;struct Cyc_Dict_Dict _tmp41E;struct Cyc_Dict_Dict*_tmp41F;void*_tmp420;
struct _tuple27*_tmp41B=env;_tmp41C=*_tmp41B;_tmp41D=_tmp41C.f1;_tmp41E=_tmp41C.f2;
_tmp41F=(struct Cyc_Dict_Dict*)&(*_tmp41B).f2;_tmp420=_tmp41C.f3;{void*_tmp421=
root;struct Cyc_Absyn_Vardecl*_tmp423;void*_tmp425;_LL203: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp422=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp421;if(_tmp422->tag != 0)goto
_LL205;else{_tmp423=_tmp422->f1;}}_LL204: if(Cyc_CfFlowInfo_contains_region(
_tmp420,_tmp423->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp41D,_tmp423->type,
_tmp41D->unknown_none);*_tmp41F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp41F,root,rval);goto _LL202;_LL205: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp424=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp421;if(_tmp424->tag != 1)goto _LL207;else{_tmp425=(void*)_tmp424->f2;}}_LL206:
if(!Cyc_CfFlowInfo_contains_region(_tmp420,_tmp425))*_tmp41F=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp41F,root,rval);goto
_LL202;_LL207: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp426=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp421;if(_tmp426->tag != 2)goto _LL202;}_LL208: goto _LL202;_LL202:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp427=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp429;_LL20A: {struct Cyc_Absyn_VarType_struct*
_tmp428=(struct Cyc_Absyn_VarType_struct*)_tmp427;if(_tmp428->tag != 2)goto _LL20C;
else{_tmp429=_tmp428->f1;}}_LL20B: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6B7;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6B7.tag=1,((_tmp6B7.f1=_tmp429,
_tmp6B7)));void*v2=(void*)& v;struct _tuple27 _tmp6B8;struct _tuple27 env=(_tmp6B8.f1=
fenv,((_tmp6B8.f2=fenv->mt_flowdict,((_tmp6B8.f3=v2,_tmp6B8)))));((void(*)(void(*
f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL20C: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp42A=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp427;if(_tmp42A->tag != 22)goto
_LL20E;}_LL20D: {struct _tuple27 _tmp6B9;struct _tuple27 env=(_tmp6B9.f1=fenv,((
_tmp6B9.f2=fenv->mt_flowdict,((_tmp6B9.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6B9)))));((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL20E:;_LL20F: {struct Cyc_Core_Impossible_struct _tmp6BF;const char*
_tmp6BE;struct Cyc_Core_Impossible_struct*_tmp6BD;(int)_throw((void*)((_tmp6BD=
_cycalloc(sizeof(*_tmp6BD)),((_tmp6BD[0]=((_tmp6BF.tag=Cyc_Core_Impossible,((
_tmp6BF.f1=((_tmp6BE="kill_flowdict_region",_tag_dyneither(_tmp6BE,sizeof(char),
21))),_tmp6BF)))),_tmp6BD)))));}_LL209:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp431=
f;int _tmp432;struct _tuple12 _tmp433;struct Cyc_Dict_Dict _tmp434;struct Cyc_List_List*
_tmp435;struct Cyc_CfFlowInfo_ConsumeInfo _tmp436;_LL211: if((_tmp431.BottomFL).tag
!= 1)goto _LL213;_tmp432=(int)(_tmp431.BottomFL).val;_LL212: return f;_LL213: if((
_tmp431.ReachableFL).tag != 2)goto _LL210;_tmp433=(struct _tuple12)(_tmp431.ReachableFL).val;
_tmp434=_tmp433.f1;_tmp435=_tmp433.f2;_tmp436=_tmp433.f3;_LL214: {struct Cyc_Dict_Dict
_tmp437=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp434,rgn);struct Cyc_List_List*
_tmp438=Cyc_CfFlowInfo_kill_relns_region(_tmp435,rgn);return Cyc_CfFlowInfo_ReachableFL(
_tmp437,_tmp438,_tmp436);}_LL210:;}static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path);static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp439=e->r;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*_tmp43D;
struct Cyc_Absyn_Exp*_tmp43F;void*_tmp441;struct Cyc_Absyn_Vardecl*_tmp443;void*
_tmp445;struct Cyc_Absyn_Vardecl*_tmp447;void*_tmp449;struct Cyc_Absyn_Vardecl*
_tmp44B;void*_tmp44D;struct Cyc_Absyn_Vardecl*_tmp44F;struct Cyc_Absyn_Exp*_tmp451;
struct _dyneither_ptr*_tmp452;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*
_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_Absyn_Exp*_tmp459;struct
_dyneither_ptr*_tmp45A;_LL216: {struct Cyc_Absyn_Cast_e_struct*_tmp43A=(struct Cyc_Absyn_Cast_e_struct*)
_tmp439;if(_tmp43A->tag != 15)goto _LL218;else{_tmp43B=_tmp43A->f2;}}_LL217:
_tmp43D=_tmp43B;goto _LL219;_LL218: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp43C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp439;if(_tmp43C->tag != 13)
goto _LL21A;else{_tmp43D=_tmp43C->f1;}}_LL219: _tmp43F=_tmp43D;goto _LL21B;_LL21A: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp43E=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp439;if(_tmp43E->tag != 14)goto _LL21C;else{_tmp43F=_tmp43E->f1;}}_LL21B: return
Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp43F,flds,path);_LL21C: {struct Cyc_Absyn_Var_e_struct*
_tmp440=(struct Cyc_Absyn_Var_e_struct*)_tmp439;if(_tmp440->tag != 1)goto _LL21E;
else{_tmp441=(void*)_tmp440->f2;{struct Cyc_Absyn_Param_b_struct*_tmp442=(struct
Cyc_Absyn_Param_b_struct*)_tmp441;if(_tmp442->tag != 3)goto _LL21E;else{_tmp443=
_tmp442->f1;}};}}_LL21D: _tmp447=_tmp443;goto _LL21F;_LL21E: {struct Cyc_Absyn_Var_e_struct*
_tmp444=(struct Cyc_Absyn_Var_e_struct*)_tmp439;if(_tmp444->tag != 1)goto _LL220;
else{_tmp445=(void*)_tmp444->f2;{struct Cyc_Absyn_Local_b_struct*_tmp446=(struct
Cyc_Absyn_Local_b_struct*)_tmp445;if(_tmp446->tag != 4)goto _LL220;else{_tmp447=
_tmp446->f1;}};}}_LL21F: _tmp44B=_tmp447;goto _LL221;_LL220: {struct Cyc_Absyn_Var_e_struct*
_tmp448=(struct Cyc_Absyn_Var_e_struct*)_tmp439;if(_tmp448->tag != 1)goto _LL222;
else{_tmp449=(void*)_tmp448->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp44A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp449;if(_tmp44A->tag != 5)goto _LL222;else{_tmp44B=_tmp44A->f1;}};}}_LL221: {
struct Cyc_CfFlowInfo_Place _tmp6C9;struct Cyc_CfFlowInfo_VarRoot_struct _tmp6C8;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp6C7;struct Cyc_CfFlowInfo_UniquePlace*
_tmp6C6;return(_tmp6C6=_region_malloc(r,sizeof(*_tmp6C6)),((_tmp6C6->place=((
_tmp6C9.root=(void*)((_tmp6C7=_region_malloc(r,sizeof(*_tmp6C7)),((_tmp6C7[0]=((
_tmp6C8.tag=0,((_tmp6C8.f1=_tmp44B,_tmp6C8)))),_tmp6C7)))),((_tmp6C9.fields=flds,
_tmp6C9)))),((_tmp6C6->path=path,_tmp6C6)))));}_LL222: {struct Cyc_Absyn_Var_e_struct*
_tmp44C=(struct Cyc_Absyn_Var_e_struct*)_tmp439;if(_tmp44C->tag != 1)goto _LL224;
else{_tmp44D=(void*)_tmp44C->f2;{struct Cyc_Absyn_Global_b_struct*_tmp44E=(struct
Cyc_Absyn_Global_b_struct*)_tmp44D;if(_tmp44E->tag != 1)goto _LL224;else{_tmp44F=
_tmp44E->f1;}};}}_LL223: {struct Cyc_Core_Impossible_struct _tmp6CF;const char*
_tmp6CE;struct Cyc_Core_Impossible_struct*_tmp6CD;(int)_throw((void*)((_tmp6CD=
_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CF.tag=Cyc_Core_Impossible,((
_tmp6CF.f1=((_tmp6CE="non-unique path",_tag_dyneither(_tmp6CE,sizeof(char),16))),
_tmp6CF)))),_tmp6CD)))));}_LL224: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp450=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp439;if(_tmp450->tag != 23)goto _LL226;
else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}_LL225:{void*_tmp462=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp451->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp464;void*_tmp465;_LL22F: {struct Cyc_Absyn_PointerType_struct*_tmp463=(struct
Cyc_Absyn_PointerType_struct*)_tmp462;if(_tmp463->tag != 5)goto _LL231;else{
_tmp464=_tmp463->f1;_tmp465=_tmp464.elt_typ;}}_LL230: if(!Cyc_Absyn_is_union_type(
_tmp465)){struct Cyc_List_List*_tmp6D0;flds=((_tmp6D0=_region_malloc(r,sizeof(*
_tmp6D0)),((_tmp6D0->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp465,_tmp452),((
_tmp6D0->tl=flds,_tmp6D0))))));}goto _LL22E;_LL231:;_LL232: {struct Cyc_Core_Impossible_struct
_tmp6D6;const char*_tmp6D5;struct Cyc_Core_Impossible_struct*_tmp6D4;(int)_throw((
void*)((_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4[0]=((_tmp6D6.tag=Cyc_Core_Impossible,((
_tmp6D6.f1=((_tmp6D5="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp6D5,sizeof(
char),25))),_tmp6D6)))),_tmp6D4)))));}_LL22E:;}_tmp454=_tmp451;goto _LL227;_LL226: {
struct Cyc_Absyn_Deref_e_struct*_tmp453=(struct Cyc_Absyn_Deref_e_struct*)_tmp439;
if(_tmp453->tag != 21)goto _LL228;else{_tmp454=_tmp453->f1;}}_LL227: {struct Cyc_List_List*
_tmp6D7;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp454,0,((_tmp6D7=
_region_malloc(r,sizeof(*_tmp6D7)),((_tmp6D7->hd=flds,((_tmp6D7->tl=path,_tmp6D7)))))));}
_LL228: {struct Cyc_Absyn_Subscript_e_struct*_tmp455=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp439;if(_tmp455->tag != 24)goto _LL22A;else{_tmp456=_tmp455->f1;_tmp457=_tmp455->f2;}}
_LL229: {void*_tmp46B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp456->topt))->v);struct Cyc_Absyn_PtrInfo _tmp46E;void*_tmp46F;
struct Cyc_Absyn_PtrAtts _tmp470;union Cyc_Absyn_Constraint*_tmp471;_LL234: {struct
Cyc_Absyn_TupleType_struct*_tmp46C=(struct Cyc_Absyn_TupleType_struct*)_tmp46B;
if(_tmp46C->tag != 11)goto _LL236;}_LL235: {unsigned int _tmp472=(Cyc_Evexp_eval_const_uint_exp(
_tmp457)).f1;struct Cyc_List_List*_tmp6D8;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp456,((_tmp6D8=_region_malloc(r,sizeof(*_tmp6D8)),((_tmp6D8->hd=(void*)((
int)_tmp472),((_tmp6D8->tl=flds,_tmp6D8)))))),path);}_LL236: {struct Cyc_Absyn_PointerType_struct*
_tmp46D=(struct Cyc_Absyn_PointerType_struct*)_tmp46B;if(_tmp46D->tag != 5)goto
_LL238;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.elt_typ;_tmp470=_tmp46E.ptr_atts;
_tmp471=_tmp470.bounds;}}_LL237: {struct Cyc_Core_Impossible_struct _tmp6DE;const
char*_tmp6DD;struct Cyc_Core_Impossible_struct*_tmp6DC;(int)_throw((void*)((
_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DE.tag=Cyc_Core_Impossible,((
_tmp6DE.f1=((_tmp6DD="subscripts unsupported for unique places",_tag_dyneither(
_tmp6DD,sizeof(char),41))),_tmp6DE)))),_tmp6DC)))));}_LL238:;_LL239: {struct Cyc_Core_Impossible_struct
_tmp6E4;const char*_tmp6E3;struct Cyc_Core_Impossible_struct*_tmp6E2;(int)_throw((
void*)((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2[0]=((_tmp6E4.tag=Cyc_Core_Impossible,((
_tmp6E4.f1=((_tmp6E3="unique place subscript -- bad type",_tag_dyneither(_tmp6E3,
sizeof(char),35))),_tmp6E4)))),_tmp6E2)))));}_LL233:;}_LL22A: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp458=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp439;if(_tmp458->tag != 22)goto
_LL22C;else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;}}_LL22B: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp459->topt))->v)){struct Cyc_Core_Impossible_struct
_tmp6EA;const char*_tmp6E9;struct Cyc_Core_Impossible_struct*_tmp6E8;(int)_throw((
void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6EA.tag=Cyc_Core_Impossible,((
_tmp6EA.f1=((_tmp6E9="unions can't contain unique places",_tag_dyneither(_tmp6E9,
sizeof(char),35))),_tmp6EA)))),_tmp6E8)))));}{int _tmp47D=Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp459->topt))->v,_tmp45A);struct Cyc_List_List*
_tmp6EB;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp459,((_tmp6EB=
_region_malloc(r,sizeof(*_tmp6EB)),((_tmp6EB->hd=(void*)_tmp47D,((_tmp6EB->tl=
flds,_tmp6EB)))))),path);};_LL22C:;_LL22D: {struct Cyc_Core_Impossible_struct
_tmp6F1;const char*_tmp6F0;struct Cyc_Core_Impossible_struct*_tmp6EF;(int)_throw((
void*)((_tmp6EF=_cycalloc(sizeof(*_tmp6EF)),((_tmp6EF[0]=((_tmp6F1.tag=Cyc_Core_Impossible,((
_tmp6F1.f1=((_tmp6F0="bad unique path",_tag_dyneither(_tmp6F0,sizeof(char),16))),
_tmp6F1)))),_tmp6EF)))));}_LL215:;}struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_UniquePlace*
Cyc_CfFlowInfo_unique_place_of(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_CfFlowInfo_unique_place_of_rec(r,e,0,0);}struct _tuple28{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict f3;struct Cyc_Position_Segment*f4;};
static void Cyc_CfFlowInfo_consume_f(struct _tuple28*env,struct Cyc_CfFlowInfo_Place*
place);static void Cyc_CfFlowInfo_consume_f(struct _tuple28*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple28 _tmp483;struct Cyc_CfFlowInfo_FlowEnv*_tmp484;struct Cyc_Dict_Dict*
_tmp485;struct Cyc_Dict_Dict _tmp486;struct Cyc_Position_Segment*_tmp487;struct
_tuple28*_tmp482=env;_tmp483=*_tmp482;_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;
_tmp486=_tmp483.f3;_tmp487=_tmp483.f4;{void*rval=_tmp484->zero;if(((int(*)(
struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp486,place->root,&
rval) && rval == _tmp484->zero)return;if(((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((
struct Cyc_Dict_Dict*)_tmp485,place,_tmp487)){struct Cyc_Position_Segment*_tmp488=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp485,place);const char*_tmp6F3;const char*_tmp6F2;((void(*)(
struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),
struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,((_tmp6F2="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp6F2,sizeof(char),59))),((_tmp6F3="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp6F3,sizeof(char),50))),_tmp488,_tmp487);}};}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){union Cyc_CfFlowInfo_FlowInfo _tmp48B=f;int _tmp48C;struct _tuple12 _tmp48D;struct
Cyc_Dict_Dict _tmp48E;struct Cyc_List_List*_tmp48F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp490;_LL23B: if((_tmp48B.BottomFL).tag != 1)goto _LL23D;_tmp48C=(int)(_tmp48B.BottomFL).val;
_LL23C: return f;_LL23D: if((_tmp48B.ReachableFL).tag != 2)goto _LL23A;_tmp48D=(
struct _tuple12)(_tmp48B.ReachableFL).val;_tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f2;
_tmp490=_tmp48D.f3;_LL23E: if(_tmp490.may_consume == 0)return f;{struct Cyc_Dict_Dict
_tmp491=_tmp490.consumed;struct _tuple28 _tmp6F4;struct _tuple28 _tmp492=(_tmp6F4.f1=
fenv,((_tmp6F4.f2=& _tmp491,((_tmp6F4.f3=_tmp48E,((_tmp6F4.f4=loc,_tmp6F4)))))));((
void(*)(void(*f)(struct _tuple28*,struct Cyc_CfFlowInfo_Place*),struct _tuple28*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,& _tmp492,
_tmp490.may_consume);{struct Cyc_CfFlowInfo_ConsumeInfo _tmp6F5;return Cyc_CfFlowInfo_ReachableFL(
_tmp48E,_tmp48F,((_tmp6F5.consumed=_tmp491,((_tmp6F5.may_consume=0,_tmp6F5)))));};};
_LL23A:;}void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,
union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp495=f;int
_tmp496;struct _tuple12 _tmp497;struct Cyc_Dict_Dict _tmp498;struct Cyc_List_List*
_tmp499;struct Cyc_CfFlowInfo_ConsumeInfo _tmp49A;_LL240: if((_tmp495.BottomFL).tag
!= 1)goto _LL242;_tmp496=(int)(_tmp495.BottomFL).val;_LL241: return;_LL242: if((
_tmp495.ReachableFL).tag != 2)goto _LL23F;_tmp497=(struct _tuple12)(_tmp495.ReachableFL).val;
_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;_tmp49A=_tmp497.f3;_LL243: {struct Cyc_List_List*
_tmp49B=_tmp49A.may_consume;while(_tmp49B != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp49A.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp49B->hd)){struct Cyc_Position_Segment*_tmp49C=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp49A.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp49B->hd);{const char*_tmp6F7;const char*_tmp6F6;((
void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct
Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)((
struct Cyc_CfFlowInfo_Place*)_tmp49B->hd,Cyc_CfFlowInfo_place_err_string,((
_tmp6F6="Read through possibly consumed unique pointer %s (cf. %s)",
_tag_dyneither(_tmp6F6,sizeof(char),58))),((_tmp6F7="Read through possibly consumed unique pointer %s",
_tag_dyneither(_tmp6F7,sizeof(char),49))),_tmp49C,loc);}break;}_tmp49B=_tmp49B->tl;}
return;}_LL23F:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){union Cyc_CfFlowInfo_FlowInfo _tmp49F=f;int _tmp4A0;struct _tuple12 _tmp4A1;struct
Cyc_Dict_Dict _tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4A4;_LL245: if((_tmp49F.BottomFL).tag != 1)goto _LL247;_tmp4A0=(int)(_tmp49F.BottomFL).val;
_LL246: return f;_LL247: if((_tmp49F.ReachableFL).tag != 2)goto _LL244;_tmp4A1=(
struct _tuple12)(_tmp49F.ReachableFL).val;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;
_tmp4A4=_tmp4A1.f3;_LL248: _tmp4A4.may_consume=0;return Cyc_CfFlowInfo_ReachableFL(
_tmp4A2,_tmp4A3,_tmp4A4);_LL244:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo _tmp4A5=
f;int _tmp4A6;struct _tuple12 _tmp4A7;struct Cyc_Dict_Dict _tmp4A8;struct Cyc_List_List*
_tmp4A9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4AA;_LL24A: if((_tmp4A5.BottomFL).tag
!= 1)goto _LL24C;_tmp4A6=(int)(_tmp4A5.BottomFL).val;_LL24B: return f;_LL24C: if((
_tmp4A5.ReachableFL).tag != 2)goto _LL249;_tmp4A7=(struct _tuple12)(_tmp4A5.ReachableFL).val;
_tmp4A8=_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;_tmp4AA=_tmp4A7.f3;_LL24D: _tmp4AA.may_consume=
0;return Cyc_CfFlowInfo_ReachableFL(_tmp4A8,_tmp4A9,_tmp4AA);_LL249:;};}struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo f,int clear);struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,int clear){union
Cyc_CfFlowInfo_FlowInfo _tmp4AB=f;int _tmp4AC;struct _tuple12 _tmp4AD;struct Cyc_Dict_Dict
_tmp4AE;struct Cyc_List_List*_tmp4AF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4B0;
_LL24F: if((_tmp4AB.BottomFL).tag != 1)goto _LL251;_tmp4AC=(int)(_tmp4AB.BottomFL).val;
_LL250: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp6FA;struct _tuple14 _tmp6F9;return(
_tmp6F9.f1=((_tmp6FA.consumed=fenv->mt_place_set,((_tmp6FA.may_consume=0,_tmp6FA)))),((
_tmp6F9.f2=f,_tmp6F9)));}_LL251: if((_tmp4AB.ReachableFL).tag != 2)goto _LL24E;
_tmp4AD=(struct _tuple12)(_tmp4AB.ReachableFL).val;_tmp4AE=_tmp4AD.f1;_tmp4AF=
_tmp4AD.f2;_tmp4B0=_tmp4AD.f3;_LL252: if(clear){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6FD;struct _tuple14 _tmp6FC;return(_tmp6FC.f1=_tmp4B0,((_tmp6FC.f2=Cyc_CfFlowInfo_ReachableFL(
_tmp4AE,_tmp4AF,((_tmp6FD.consumed=fenv->mt_place_set,((_tmp6FD.may_consume=0,
_tmp6FD))))),_tmp6FC)));}else{struct _tuple14 _tmp6FE;return(_tmp6FE.f1=_tmp4B0,((
_tmp6FE.f2=f,_tmp6FE)));}_LL24E:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only){union Cyc_CfFlowInfo_FlowInfo _tmp4B6=f;int _tmp4B7;struct
_tuple12 _tmp4B8;struct Cyc_Dict_Dict _tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4BB;_LL254: if((_tmp4B6.BottomFL).tag != 1)goto _LL256;_tmp4B7=(int)(_tmp4B6.BottomFL).val;
_LL255: return f;_LL256: if((_tmp4B6.ReachableFL).tag != 2)goto _LL253;_tmp4B8=(
struct _tuple12)(_tmp4B6.ReachableFL).val;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
_tmp4BB=_tmp4B8.f3;_LL257: if(may_consume_only){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6FF;return Cyc_CfFlowInfo_ReachableFL(_tmp4B9,_tmp4BA,((_tmp6FF.consumed=
_tmp4BB.consumed,((_tmp6FF.may_consume=c.may_consume,_tmp6FF)))));}return Cyc_CfFlowInfo_ReachableFL(
_tmp4B9,_tmp4BA,c);_LL253:;}
