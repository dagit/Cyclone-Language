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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct
Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);unsigned long Cyc_fread(struct _dyneither_ptr,
unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int
remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dyneither_ptr,struct _dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
int isspace(int);char*getenv(const char*);int system(const char*);struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct _dyneither_ptr Cstring_to_string(char*);struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];
struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[
6];struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;
void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);int Cyc_strncmp(struct _dyneither_ptr s1,
struct _dyneither_ptr s2,unsigned long len);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);int Cyc_Filename_check_suffix(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
char*tag;};extern int Cyc_Absyn_porting_c_code;extern int Cyc_Absyn_no_regions;
extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple9{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple9*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple10{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple11*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple12{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple13{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple14*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple15{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple15 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop
cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct _union_FlowInfo_BottomFL{int tag;
int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple17{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple17*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);
void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int
Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=
0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;
static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=
0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;static int Cyc_elim_se_r=0;static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static
int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=
1;static int Cyc_generate_line_directives_r=0;static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=
0;static int Cyc_nocyc_setjmp_r=0;static int Cyc_compile_for_boot_r=0;static struct
Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _dyneither_ptr s);static
void Cyc_add_ccarg(struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3A9;struct Cyc_List_List*
_tmp3A8;Cyc_ccargs=((_tmp3A8=_cycalloc(sizeof(*_tmp3A8)),((_tmp3A8->hd=((_tmp3A9=
_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=s,_tmp3A9)))),((_tmp3A8->tl=Cyc_ccargs,
_tmp3A8))))));}void Cyc_set_c_compiler(struct _dyneither_ptr s);void Cyc_set_c_compiler(
struct _dyneither_ptr s){const char*_tmp3AA;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3AA="vc",_tag_dyneither(_tmp3AA,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp3AB;Cyc_add_ccarg(((_tmp3AB="-DVC_C",
_tag_dyneither(_tmp3AB,sizeof(char),7))));};}else{const char*_tmp3AC;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3AC="gcc",_tag_dyneither(_tmp3AC,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp3AD;Cyc_add_ccarg(((
_tmp3AD="-DGCC_C",_tag_dyneither(_tmp3AD,sizeof(char),8))));};}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dyneither_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dyneither_ptr s);static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3AE;Cyc_output_file=((_tmp3AE=
_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE[0]=s,_tmp3AE))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s);static void Cyc_set_cpp(struct
_dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dyneither_ptr s);static void Cyc_add_cpparg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp3B1;struct Cyc_List_List*_tmp3B0;Cyc_cppargs=((
_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->hd=((_tmp3B1=_cycalloc(sizeof(*
_tmp3B1)),((_tmp3B1[0]=s,_tmp3B1)))),((_tmp3B0->tl=Cyc_cppargs,_tmp3B0))))));}
static void Cyc_add_iprefix(struct _dyneither_ptr s);static void Cyc_add_iprefix(
struct _dyneither_ptr s){const char*_tmp3B5;void*_tmp3B4[1];struct Cyc_String_pa_struct
_tmp3B3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3B3.tag=0,((_tmp3B3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3B4[0]=& _tmp3B3,Cyc_aprintf(((
_tmp3B5="-iprefix %s",_tag_dyneither(_tmp3B5,sizeof(char),12))),_tag_dyneither(
_tmp3B4,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefix(struct
_dyneither_ptr s);static void Cyc_add_iwithprefix(struct _dyneither_ptr s){const char*
_tmp3B9;void*_tmp3B8[1];struct Cyc_String_pa_struct _tmp3B7;Cyc_add_cpparg((struct
_dyneither_ptr)((_tmp3B7.tag=0,((_tmp3B7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp3B8[0]=& _tmp3B7,Cyc_aprintf(((_tmp3B9="-iwithprefix %s",
_tag_dyneither(_tmp3B9,sizeof(char),16))),_tag_dyneither(_tmp3B8,sizeof(void*),1)))))))));}
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s);static void Cyc_add_iwithprefixbefore(
struct _dyneither_ptr s){const char*_tmp3BD;void*_tmp3BC[1];struct Cyc_String_pa_struct
_tmp3BB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3BB.tag=0,((_tmp3BB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3BC[0]=& _tmp3BB,Cyc_aprintf(((
_tmp3BD="-iwithprefixbefore %s",_tag_dyneither(_tmp3BD,sizeof(char),22))),
_tag_dyneither(_tmp3BC,sizeof(void*),1)))))))));}static void Cyc_add_isystem(
struct _dyneither_ptr s);static void Cyc_add_isystem(struct _dyneither_ptr s){const
char*_tmp3C1;void*_tmp3C0[1];struct Cyc_String_pa_struct _tmp3BF;Cyc_add_cpparg((
struct _dyneither_ptr)((_tmp3BF.tag=0,((_tmp3BF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp3C0[0]=& _tmp3BF,Cyc_aprintf(((_tmp3C1="-isystem %s",
_tag_dyneither(_tmp3C1,sizeof(char),12))),_tag_dyneither(_tmp3C0,sizeof(void*),1)))))))));}
static void Cyc_add_idirafter(struct _dyneither_ptr s);static void Cyc_add_idirafter(
struct _dyneither_ptr s){const char*_tmp3C5;void*_tmp3C4[1];struct Cyc_String_pa_struct
_tmp3C3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C3.tag=0,((_tmp3C3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C4[0]=& _tmp3C3,Cyc_aprintf(((
_tmp3C5="-idirafter %s",_tag_dyneither(_tmp3C5,sizeof(char),14))),_tag_dyneither(
_tmp3C4,sizeof(void*),1)))))))));}static struct _dyneither_ptr Cyc_target_arch=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_target_arch(
struct _dyneither_ptr s);static void Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=
1;Cyc_target_arch=s;}static void Cyc_print_version();static void Cyc_print_version(){{
const char*_tmp3C9;void*_tmp3C8[1];struct Cyc_String_pa_struct _tmp3C7;(_tmp3C7.tag=
0,((_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cversion)),((_tmp3C8[0]=& _tmp3C7,Cyc_printf(((_tmp3C9="The Cyclone compiler, version %s\n",
_tag_dyneither(_tmp3C9,sizeof(char),34))),_tag_dyneither(_tmp3C8,sizeof(void*),1)))))));}{
const char*_tmp3CE;void*_tmp3CD[2];struct Cyc_String_pa_struct _tmp3CC;struct Cyc_String_pa_struct
_tmp3CB;(_tmp3CB.tag=0,((_tmp3CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3CC.tag=0,((_tmp3CC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3CD[0]=& _tmp3CC,((_tmp3CD[1]=&
_tmp3CB,Cyc_printf(((_tmp3CE="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp3CE,sizeof(char),39))),_tag_dyneither(_tmp3CD,sizeof(void*),2)))))))))))));}{
const char*_tmp3D2;void*_tmp3D1[1];struct Cyc_String_pa_struct _tmp3D0;(_tmp3D0.tag=
0,((_tmp3D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp3D1[0]=& _tmp3D0,Cyc_printf(((_tmp3D2="Standard library directory: %s\n",
_tag_dyneither(_tmp3D2,sizeof(char),32))),_tag_dyneither(_tmp3D1,sizeof(void*),1)))))));}{
const char*_tmp3D6;void*_tmp3D5[1];struct Cyc_String_pa_struct _tmp3D4;(_tmp3D4.tag=
0,((_tmp3D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp3D5[0]=& _tmp3D4,Cyc_printf(((_tmp3D6="Standard include directory: %s\n",
_tag_dyneither(_tmp3D6,sizeof(char),32))),_tag_dyneither(_tmp3D5,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s);static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp26 <= 4)return 0;else{const char*_tmp3D7;return Cyc_strcmp((struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp3D7=".cyc",
_tag_dyneither(_tmp3D7,sizeof(char),5))))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s);static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp28 - 2);struct _dyneither_ptr*_tmp3DA;struct Cyc_List_List*
_tmp3D9;Cyc_cyclone_exec_path=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->hd=((
_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA[0]=_tmp29,_tmp3DA)))),((_tmp3D9->tl=
Cyc_cyclone_exec_path,_tmp3D9))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s);static void Cyc_add_libarg(struct
_dyneither_ptr s){const char*_tmp3DB;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3DB="-lxml",_tag_dyneither(_tmp3DB,sizeof(char),6))))== 0)Cyc_add_ccarg(s);
else{struct _dyneither_ptr*_tmp3DE;struct Cyc_List_List*_tmp3DD;Cyc_libargs=((
_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->hd=((_tmp3DE=_cycalloc(sizeof(*
_tmp3DE)),((_tmp3DE[0]=s,_tmp3DE)))),((_tmp3DD->tl=Cyc_libargs,_tmp3DD))))));}}
static void Cyc_add_marg(struct _dyneither_ptr s);static void Cyc_add_marg(struct
_dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{const char*_tmp3DF;Cyc_add_ccarg(((_tmp3DF="-save-temps",
_tag_dyneither(_tmp3DF,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp3E0;Cyc_add_cpparg(((
_tmp3E0="-M",_tag_dyneither(_tmp3E0,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s);static void Cyc_set_dependencies_target(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp3E1;Cyc_dependencies_target=((_tmp3E1=
_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=s,_tmp3E1))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp3E2;Cyc_add_ccarg(((_tmp3E2="-c",_tag_dyneither(_tmp3E2,sizeof(char),3))));};}
static void Cyc_set_nocppprecomp();static void Cyc_set_nocppprecomp(){{const char*
_tmp3E3;Cyc_add_cpparg(((_tmp3E3="-no-cpp-precomp",_tag_dyneither(_tmp3E3,
sizeof(char),16))));}{const char*_tmp3E4;Cyc_add_ccarg(((_tmp3E4="-no-cpp-precomp",
_tag_dyneither(_tmp3E4,sizeof(char),16))));};}static void Cyc_set_lineno();static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp3E5;Cyc_add_ccarg(((_tmp3E5="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3E5,
sizeof(char),18))));}Cyc_set_elim_se();}static void Cyc_set_noboundschecks();
static void Cyc_set_noboundschecks(){const char*_tmp3E6;Cyc_add_ccarg(((_tmp3E6="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp3E6,sizeof(char),23))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp3E7;Cyc_add_ccarg(((_tmp3E7="-DNO_CYC_NULL_CHECKS",
_tag_dyneither(_tmp3E7,sizeof(char),21))));}static void Cyc_set_nochecks();static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;{
const char*_tmp3E8;Cyc_add_ccarg(((_tmp3E8="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp3E8,sizeof(char),16))));};}static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=
1;{const char*_tmp3E9;Cyc_add_ccarg(((_tmp3E9="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp3E9,sizeof(char),21))));};}static void Cyc_set_pg();static void
Cyc_set_pg(){Cyc_pg_r=1;{const char*_tmp3EA;Cyc_add_ccarg(((_tmp3EA="-pg",
_tag_dyneither(_tmp3EA,sizeof(char),4))));};}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp3EB;Cyc_add_ccarg(((_tmp3EB="-S",_tag_dyneither(_tmp3EB,sizeof(char),3))));};}
static void Cyc_set_all_warnings();static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE
 = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(
struct _dyneither_ptr s);static void Cyc_set_inputtype(struct _dyneither_ptr s){const
char*_tmp3EC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3EC="cyc",
_tag_dyneither(_tmp3EC,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp3ED;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3ED="none",
_tag_dyneither(_tmp3ED,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp3F1;void*_tmp3F0[1];struct Cyc_String_pa_struct _tmp3EF;(_tmp3EF.tag=
0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F0[0]=&
_tmp3EF,Cyc_fprintf(Cyc_stderr,((_tmp3F1="Input type '%s' not supported\n",
_tag_dyneither(_tmp3F1,sizeof(char),31))),_tag_dyneither(_tmp3F0,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file);struct _dyneither_ptr Cyc_make_base_filename(struct
_dyneither_ptr s,struct _dyneither_ptr*output_file){struct _dyneither_ptr _tmp41=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp42=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp43=Cyc_strlen((
struct _dyneither_ptr)_tmp41)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp41,(
struct _dyneither_ptr)_tmp42): _tmp42;return(struct _dyneither_ptr)_tmp43;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s);static void Cyc_add_other(struct _dyneither_ptr s){if(Cyc_intype == 
Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{struct _dyneither_ptr*_tmp3F4;
struct Cyc_List_List*_tmp3F3;Cyc_cyclone_files=((_tmp3F3=_cycalloc(sizeof(*
_tmp3F3)),((_tmp3F3->hd=((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=s,
_tmp3F4)))),((_tmp3F3->tl=Cyc_cyclone_files,_tmp3F3))))));}{struct _dyneither_ptr
_tmp46=Cyc_make_base_filename(s,Cyc_output_file);const char*_tmp3F5;struct
_dyneither_ptr _tmp47=Cyc_strconcat((struct _dyneither_ptr)_tmp46,((_tmp3F5=".c",
_tag_dyneither(_tmp3F5,sizeof(char),3))));Cyc_add_ccarg((struct _dyneither_ptr)
_tmp47);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(struct
_dyneither_ptr s);static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)
return;else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct
_tuple18{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*
Cyc_read_specs(struct _dyneither_ptr file);struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp49=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp3F9;void*_tmp3F8[1];struct Cyc_String_pa_struct _tmp3F7;(_tmp3F7.tag=
0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3F8[0]=&
_tmp3F7,Cyc_fprintf(Cyc_stderr,((_tmp3F9="Error opening spec file %s\n",
_tag_dyneither(_tmp3F9,sizeof(char),28))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp3FE;void*_tmp3FD[2];struct Cyc_String_pa_struct _tmp3FC;struct
Cyc_Int_pa_struct _tmp3FB;(_tmp3FB.tag=1,((_tmp3FB.f1=(unsigned long)c,((_tmp3FC.tag=
0,((_tmp3FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3FD[0]=&
_tmp3FC,((_tmp3FD[1]=& _tmp3FB,Cyc_fprintf(Cyc_stderr,((_tmp3FE="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp3FE,sizeof(char),55))),_tag_dyneither(_tmp3FD,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp402;void*_tmp401[1];struct Cyc_String_pa_struct _tmp400;(
_tmp400.tag=0,((_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp401[0]=& _tmp400,Cyc_fprintf(Cyc_stderr,((_tmp402="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp402,sizeof(char),44))),_tag_dyneither(_tmp401,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp406;void*_tmp405[1];struct Cyc_String_pa_struct _tmp404;(_tmp404.tag=0,((
_tmp404.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp405[0]=&
_tmp404,Cyc_fprintf(Cyc_stderr,((_tmp406="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp406,sizeof(char),50))),_tag_dyneither(_tmp405,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple18*_tmp413;struct _dyneither_ptr*_tmp412;const char*_tmp411;struct
_dyneither_ptr*_tmp410;struct Cyc_List_List*_tmp40F;_tmp49=((_tmp40F=_cycalloc(
sizeof(*_tmp40F)),((_tmp40F->hd=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->f1=((
_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp412)))),((_tmp413->f2=((_tmp410=
_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp411="",_tag_dyneither(_tmp411,sizeof(char),1)))),_tmp410)))),_tmp413)))))),((
_tmp40F->tl=_tmp49,_tmp40F))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp418;void*_tmp417[2];struct Cyc_String_pa_struct _tmp416;struct Cyc_String_pa_struct
_tmp415;(_tmp415.tag=0,((_tmp415.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp416.tag=0,((_tmp416.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp417[0]=& _tmp416,((_tmp417[1]=& _tmp415,Cyc_fprintf(Cyc_stderr,((
_tmp418="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp418,sizeof(char),50))),_tag_dyneither(_tmp417,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple18*_tmp421;struct _dyneither_ptr*_tmp420;struct _dyneither_ptr*_tmp41F;
struct Cyc_List_List*_tmp41E;_tmp49=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((
_tmp41E->hd=((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421->f1=((_tmp420=
_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp420)))),((_tmp421->f2=((_tmp41F=
_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp41F)))),_tmp421)))))),((_tmp41E->tl=
_tmp49,_tmp41E))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp49;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp42B(struct _dyneither_ptr*_tmp6A,
unsigned int*_tmp42A,unsigned int*_tmp429,struct _dyneither_ptr**_tmp427){for(*
_tmp42A=0;*_tmp42A < *_tmp429;(*_tmp42A)++){(*_tmp427)[*_tmp42A]=*((struct
_dyneither_ptr**)(*_tmp6A).curr)[(int)*_tmp42A];}}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline){if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return _tag_dyneither(0,0,0);{unsigned long _tmp64=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp65=0;char buf[4096];int i=0;int j=0;
if(_tmp64 > 4096)goto DONE;while(1){while(1){if(i >= _tmp64)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp64)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp64)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';{struct _dyneither_ptr*_tmp424;struct Cyc_List_List*_tmp423;_tmp65=((
_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->hd=((_tmp424=_cycalloc(sizeof(*
_tmp424)),((_tmp424[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp424)))),((_tmp423->tl=_tmp65,_tmp423))))));};}DONE:
_tmp65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp65);{
struct Cyc_List_List*_tmp425;_tmp65=((_tmp425=_cycalloc(sizeof(*_tmp425)),((
_tmp425->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp425->tl=_tmp65,_tmp425))))));}{struct _dyneither_ptr _tmp6A=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp65);
unsigned int _tmp42A;unsigned int _tmp429;struct _dyneither_ptr _tmp428;struct
_dyneither_ptr*_tmp427;unsigned int _tmp426;struct _dyneither_ptr _tmp6B=(_tmp426=
_get_dyneither_size(_tmp6A,sizeof(struct _dyneither_ptr*)),((_tmp427=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp426)),((
_tmp428=_tag_dyneither(_tmp427,sizeof(struct _dyneither_ptr),_tmp426),((((_tmp429=
_tmp426,_tmp42B(& _tmp6A,& _tmp42A,& _tmp429,& _tmp427))),_tmp428)))))));return
_tmp6B;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part);struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,
struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp70;
_push_handler(& _tmp70);{int _tmp72=0;if(setjmp(_tmp70.handler))_tmp72=1;if(!
_tmp72){{struct Cyc___cycFILE*_tmp73=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp73;};_pop_handler();}else{void*_tmp71=(void*)
_exn_thrown;void*_tmp75=_tmp71;_LL1:;_LL2: Cyc_compile_failure=1;{const char*
_tmp430;void*_tmp42F[2];struct Cyc_String_pa_struct _tmp42E;struct Cyc_String_pa_struct
_tmp42D;(_tmp42D.tag=0,((_tmp42D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename),((_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg_part),((_tmp42F[0]=& _tmp42E,((_tmp42F[1]=& _tmp42D,Cyc_fprintf(
Cyc_stderr,((_tmp430="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp430,
sizeof(char),29))),_tag_dyneither(_tmp42F,sizeof(void*),2)))))))))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp75);_LL0:;}};}
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*
Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){const char*_tmp431;struct
_dyneither_ptr exn_string=(_tmp431="",_tag_dyneither(_tmp431,sizeof(char),1));
const char*_tmp432;struct _dyneither_ptr explain_string=(_tmp432="",_tag_dyneither(
_tmp432,sizeof(char),1));int other_exn=0;struct Cyc_Core_Impossible_struct _tmp438;
const char*_tmp437;struct Cyc_Core_Impossible_struct*_tmp436;void*ex=(void*)((
_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=((_tmp438.tag=Cyc_Core_Impossible,((
_tmp438.f1=((_tmp437="",_tag_dyneither(_tmp437,sizeof(char),1))),_tmp438)))),
_tmp436))));struct Cyc_List_List*_tmp7A=0;{struct _handler_cons _tmp7B;
_push_handler(& _tmp7B);{int _tmp7D=0;if(setjmp(_tmp7B.handler))_tmp7D=1;if(!
_tmp7D){_tmp7A=f(env,tds);;_pop_handler();}else{void*_tmp7C=(void*)_exn_thrown;
void*_tmp7F=_tmp7C;struct _dyneither_ptr _tmp81;struct _dyneither_ptr _tmp84;struct
_dyneither_ptr _tmp86;_LL6: {struct Cyc_Core_Impossible_struct*_tmp80=(struct Cyc_Core_Impossible_struct*)
_tmp7F;if(_tmp80->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp81=_tmp80->f1;}}
_LL7:{const char*_tmp439;exn_string=((_tmp439="Exception Core::Impossible",
_tag_dyneither(_tmp439,sizeof(char),27)));}explain_string=_tmp81;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp82=(struct Cyc_Dict_Absent_struct*)_tmp7F;if(
_tmp82->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp43A;exn_string=((
_tmp43A="Exception Dict::Absent",_tag_dyneither(_tmp43A,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp83=(struct Cyc_Core_Invalid_argument_struct*)
_tmp7F;if(_tmp83->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp84=_tmp83->f1;}}
_LLB:{const char*_tmp43B;exn_string=((_tmp43B="Exception Core::Invalid_argument",
_tag_dyneither(_tmp43B,sizeof(char),33)));}explain_string=_tmp84;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp85=(struct Cyc_Core_Failure_struct*)_tmp7F;if(
_tmp85->tag != Cyc_Core_Failure)goto _LLE;else{_tmp86=_tmp85->f1;}}_LLD:{const char*
_tmp43C;exn_string=((_tmp43C="Exception Core::Failure",_tag_dyneither(_tmp43C,
sizeof(char),24)));}explain_string=_tmp86;goto _LL5;_LLE:;_LLF: ex=_tmp7F;
other_exn=1;{const char*_tmp43D;exn_string=((_tmp43D="Uncaught exception",
_tag_dyneither(_tmp43D,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp7F);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp43E;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp43E="",
_tag_dyneither(_tmp43E,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp444;void*_tmp443[3];struct Cyc_String_pa_struct _tmp442;struct Cyc_String_pa_struct
_tmp441;struct Cyc_String_pa_struct _tmp440;(_tmp440.tag=0,((_tmp440.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp441.tag=0,((_tmp441.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp442.tag=0,((
_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp443[0]=&
_tmp442,((_tmp443[1]=& _tmp441,((_tmp443[2]=& _tmp440,Cyc_fprintf(Cyc_stderr,((
_tmp444="\n%s thrown during %s: %s",_tag_dyneither(_tmp444,sizeof(char),25))),
_tag_dyneither(_tmp443,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp447;void*_tmp446;(_tmp446=0,Cyc_fprintf(Cyc_stderr,((_tmp447="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp447,sizeof(char),22))),_tag_dyneither(_tmp446,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp7A;}else{if(Cyc_v_r){{const char*_tmp44B;void*_tmp44A[1];struct Cyc_String_pa_struct
_tmp449;(_tmp449.tag=0,((_tmp449.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp44A[0]=& _tmp449,Cyc_fprintf(Cyc_stderr,((_tmp44B="%s completed.\n",
_tag_dyneither(_tmp44B,sizeof(char),15))),_tag_dyneither(_tmp44A,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp7A;}}return _tmp7A;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore);
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*
ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*_tmp9C=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);return _tmp9C;};}struct _tuple19{struct _RegionHandle*f1;struct
Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(struct _tuple19*tcenv,
struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_typecheck(struct _tuple19*
tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp9E;struct Cyc_Tcenv_Tenv*
_tmp9F;struct _tuple19 _tmp9D=*tcenv;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;Cyc_Tc_tc(
_tmp9E,_tmp9F,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp9F,tds);return
tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds);
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple20{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;
struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple20*
params,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_interface(struct
_tuple20*params,struct Cyc_List_List*tds){struct _tuple20 _tmpA1;struct Cyc_Tcenv_Tenv*
_tmpA2;struct Cyc___cycFILE*_tmpA3;struct Cyc___cycFILE*_tmpA4;struct _tuple20*
_tmpA0=params;_tmpA1=*_tmpA0;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;{
struct Cyc_Interface_I*_tmpA5=Cyc_Interface_extract(_tmpA2->ae);if(_tmpA3 == 0)Cyc_Interface_save(
_tmpA5,_tmpA4);else{struct Cyc_Interface_I*_tmpA6=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmpA3);const char*_tmp450;const char*_tmp44F;struct _tuple17*_tmp44E;if(!Cyc_Interface_is_subinterface(
_tmpA6,_tmpA5,((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E->f1=((_tmp44F="written interface",
_tag_dyneither(_tmp44F,sizeof(char),18))),((_tmp44E->f2=((_tmp450="maximal interface",
_tag_dyneither(_tmp450,sizeof(char),18))),_tmp44E))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmpA6,_tmpA4);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_translate(int
ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_tovc(int ignore,
struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}static struct _dyneither_ptr Cyc_cyc_setjmp=(struct
_dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static struct _dyneither_ptr Cyc_cyc_include=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_cyc_include(
struct _dyneither_ptr f);static void Cyc_set_cyc_include(struct _dyneither_ptr f){Cyc_cyc_include=
f;}int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*
out_file);int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*
out_file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){{const char*
_tmp453;void*_tmp452;(_tmp452=0,Cyc_fprintf(Cyc_stderr,((_tmp453="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp453,sizeof(char),53))),_tag_dyneither(_tmp452,sizeof(void*),0)));}
return 1;}{const char*_tmp455;const char*_tmp454;struct Cyc___cycFILE*_tmpAC=Cyc_try_file_open(
file,((_tmp454="r",_tag_dyneither(_tmp454,sizeof(char),2))),((_tmp455="internal compiler file",
_tag_dyneither(_tmp455,sizeof(char),23))));if(_tmpAC == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpB3=1024;
unsigned int i;for(i=0;i < _tmpB3;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmpAC);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAC)){Cyc_compile_failure=
1;{const char*_tmp459;void*_tmp458[1];struct Cyc_String_pa_struct _tmp457;(_tmp457.tag=
0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp458[0]=&
_tmp457,Cyc_fprintf(Cyc_stderr,((_tmp459="\nError: problem copying %s\n",
_tag_dyneither(_tmp459,sizeof(char),28))),_tag_dyneither(_tmp458,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp45D;void*_tmp45C[1];struct Cyc_String_pa_struct
_tmp45B;(_tmp45B.tag=0,((_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp45C[0]=& _tmp45B,Cyc_fprintf(Cyc_stderr,((_tmp45D="\nError: problem copying %s\n",
_tag_dyneither(_tmp45D,sizeof(char),28))),_tag_dyneither(_tmp45C,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmpAC);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpB6=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpB6.expand_typedefs=!Cyc_noexpand_r;_tmpB6.to_VC=Cyc_Cyclone_tovc_r;
_tmpB6.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpB6.generate_line_directives=
Cyc_generate_line_directives_r;_tmpB6.print_full_evars=Cyc_print_full_evars_r;
_tmpB6.print_all_tvars=Cyc_print_all_tvars_r;_tmpB6.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB6.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_fprintf(out_file,((_tmp460="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp460,sizeof(char),27))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r){
const char*_tmp463;void*_tmp462;(_tmp462=0,Cyc_fprintf(out_file,((_tmp463="#include <setjmp.h>\n",
_tag_dyneither(_tmp463,sizeof(char),21))),_tag_dyneither(_tmp462,sizeof(void*),0)));}
else{if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}if(Cyc_copy_internal_file(
Cyc_cyc_include,out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpB6);
Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpB6);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles();
static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((
struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static
struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c);static struct
Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){if((char*)s.curr == (
char*)(_tag_dyneither(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmpBB=0;
unsigned long _tmpBC=Cyc_strlen((struct _dyneither_ptr)s);while(_tmpBC > 0){struct
_dyneither_ptr _tmpBD=Cyc_strchr(s,c);if((char*)_tmpBD.curr == (char*)(
_tag_dyneither(0,0,0)).curr){{struct _dyneither_ptr*_tmp466;struct Cyc_List_List*
_tmp465;_tmpBB=((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465->hd=((_tmp466=
_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=s,_tmp466)))),((_tmp465->tl=_tmpBB,
_tmp465))))));}break;}else{{struct _dyneither_ptr*_tmp469;struct Cyc_List_List*
_tmp468;_tmpBB=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468->hd=((_tmp469=
_cycalloc(sizeof(*_tmp469)),((_tmp469[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBD).curr - s.curr)
/ sizeof(char))),_tmp469)))),((_tmp468->tl=_tmpBB,_tmp468))))));}_tmpBC -=(((
struct _dyneither_ptr)_tmpBD).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(
_tmpBD,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpBB);};}static int Cyc_file_exists(struct _dyneither_ptr file);
static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{
struct _handler_cons _tmpC2;_push_handler(& _tmpC2);{int _tmpC4=0;if(setjmp(_tmpC2.handler))
_tmpC4=1;if(!_tmpC4){{const char*_tmp46A;f=(struct Cyc___cycFILE*)Cyc_file_open(
file,((_tmp46A="r",_tag_dyneither(_tmp46A,sizeof(char),2))));};_pop_handler();}
else{void*_tmpC3=(void*)_exn_thrown;void*_tmpC7=_tmpC3;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpC7);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs);static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*
dirs){const char*_tmp46B;struct _dyneither_ptr tmp=(_tmp46B="",_tag_dyneither(
_tmp46B,sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr
_tmpC8=*((struct _dyneither_ptr*)dirs->hd);if((char*)_tmpC8.curr == (char*)(
_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC8)== 0)
continue;{const char*_tmp46C;_tmpC8=(struct _dyneither_ptr)Cyc_strconcat((struct
_dyneither_ptr)_tmpC8,((_tmp46C=":",_tag_dyneither(_tmp46C,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC8,(struct
_dyneither_ptr)tmp);}return tmp;}static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file);static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpCB=*((
struct _dyneither_ptr*)dirs->hd);if((char*)_tmpCB.curr == (char*)(_tag_dyneither(0,
0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCB)== 0)continue;{struct
_dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpCB,file);if(Cyc_file_exists(
s)){struct _dyneither_ptr*_tmp46D;return(_tmp46D=_cycalloc(sizeof(*_tmp46D)),((
_tmp46D[0]=s,_tmp46D)));}};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file);static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpCD=
Cyc_find(dirs,file);if(_tmpCD == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
const char*_tmp472;void*_tmp471[2];struct Cyc_String_pa_struct _tmp470;struct Cyc_String_pa_struct
_tmp46F;(_tmp46F.tag=0,((_tmp46F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file),((_tmp471[0]=& _tmp470,((_tmp471[1]=& _tmp46F,Cyc_fprintf(
Cyc_stderr,((_tmp472="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp472,sizeof(char),56))),_tag_dyneither(_tmp471,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp482;const char*_tmp481;void*_tmp480[2];struct Cyc_String_pa_struct _tmp47F;
struct Cyc_String_pa_struct _tmp47E;struct Cyc_Core_Failure_struct*_tmp47D;(int)
_throw((void*)((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D[0]=((_tmp482.tag=
Cyc_Core_Failure,((_tmp482.f1=(struct _dyneither_ptr)((_tmp47E.tag=0,((_tmp47E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp47F.tag=
0,((_tmp47F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp480[0]=&
_tmp47F,((_tmp480[1]=& _tmp47E,Cyc_aprintf(((_tmp481="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp481,sizeof(char),56))),_tag_dyneither(_tmp480,sizeof(void*),2)))))))))))))),
_tmp482)))),_tmp47D)))));};}return*_tmpCD;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir);static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*
_tmpD8=0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp485;struct Cyc_List_List*
_tmp484;_tmpD8=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->hd=((_tmp485=
_cycalloc(sizeof(*_tmp485)),((_tmp485[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp485)))),((_tmp484->tl=_tmpD8,_tmp484))))));}
_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD8);
return _tmpD8;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpDB=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp486;_tmpDB=((_tmp486=_cycalloc(sizeof(*
_tmp486)),((_tmp486->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp486->tl=_tmpDB,
_tmp486))))));}{struct _dyneither_ptr*_tmp489;struct Cyc_List_List*_tmp488;_tmpDB=((
_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=((_tmp489=_cycalloc(sizeof(*
_tmp489)),((_tmp489[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp489)))),((_tmp488->tl=_tmpDB,_tmp488))))));};}
_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDB);
return _tmpDB;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmp492(unsigned int*_tmp491,unsigned int*
_tmp490,char**_tmp48E){for(*_tmp491=0;*_tmp491 < *_tmp490;(*_tmp491)++){(*_tmp48E)[*
_tmp491]='\000';}}static struct _dyneither_ptr Cyc_sh_escape_string(struct
_dyneither_ptr s){unsigned long _tmpDF=Cyc_strlen((struct _dyneither_ptr)s);int
_tmpE0=0;int _tmpE1=0;{int i=0;for(0;i < _tmpDF;++ i){char _tmpE2=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpE2 == '\'')++ _tmpE0;else{if(
Cyc_is_other_special(_tmpE2))++ _tmpE1;}}}if(_tmpE0 == 0  && _tmpE1 == 0)return s;if(
_tmpE0 == 0){struct _dyneither_ptr*_tmp48C;struct _dyneither_ptr*_tmp48B[3];return(
struct _dyneither_ptr)Cyc_strconcat_l(((_tmp48B[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp48B[1]=((_tmp48C=
_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=(struct _dyneither_ptr)s,_tmp48C)))),((
_tmp48B[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp48B,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long
_tmpE7=(_tmpDF + _tmpE0)+ _tmpE1;unsigned int _tmp491;unsigned int _tmp490;struct
_dyneither_ptr _tmp48F;char*_tmp48E;unsigned int _tmp48D;struct _dyneither_ptr s2=(
_tmp48D=_tmpE7 + 1,((_tmp48E=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp48D + 1)),((_tmp48F=_tag_dyneither(_tmp48E,sizeof(char),_tmp48D + 1),((((
_tmp490=_tmp48D,((_tmp492(& _tmp491,& _tmp490,& _tmp48E),_tmp48E[_tmp490]=(char)0)))),
_tmp48F)))))));int _tmpE8=0;int _tmpE9=0;for(0;_tmpE8 < _tmpDF;++ _tmpE8){char _tmpEA=*((
const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE8));if(_tmpEA == '\''
 || Cyc_is_other_special(_tmpEA)){char _tmp495;char _tmp494;struct _dyneither_ptr
_tmp493;(_tmp493=_dyneither_ptr_plus(s2,sizeof(char),_tmpE9 ++),((_tmp494=*((char*)
_check_dyneither_subscript(_tmp493,sizeof(char),0)),((_tmp495='\\',((
_get_dyneither_size(_tmp493,sizeof(char))== 1  && (_tmp494 == '\000'  && _tmp495 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp493.curr)=_tmp495)))))));}{char _tmp498;char
_tmp497;struct _dyneither_ptr _tmp496;(_tmp496=_dyneither_ptr_plus(s2,sizeof(char),
_tmpE9 ++),((_tmp497=*((char*)_check_dyneither_subscript(_tmp496,sizeof(char),0)),((
_tmp498=_tmpEA,((_get_dyneither_size(_tmp496,sizeof(char))== 1  && (_tmp497 == '\000'
 && _tmp498 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp496.curr)=_tmp498)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp499;return(_tmp499=_cycalloc(
sizeof(*_tmp499)),((_tmp499[0]=Cyc_sh_escape_string(*sp),_tmp499)));}static void
Cyc_process_file(struct _dyneither_ptr filename);static void Cyc_process_file(struct
_dyneither_ptr filename){struct _dyneither_ptr _tmpF6=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp49A;struct _dyneither_ptr _tmpF7=Cyc_strconcat((
struct _dyneither_ptr)_tmpF6,((_tmp49A=".cyp",_tag_dyneither(_tmp49A,sizeof(char),
5))));const char*_tmp49B;struct _dyneither_ptr _tmpF8=Cyc_strconcat((struct
_dyneither_ptr)_tmpF6,((_tmp49B=".cyci",_tag_dyneither(_tmp49B,sizeof(char),6))));
const char*_tmp49C;struct _dyneither_ptr _tmpF9=Cyc_strconcat((struct _dyneither_ptr)
_tmpF6,((_tmp49C=".cycio",_tag_dyneither(_tmp49C,sizeof(char),7))));const char*
_tmp49D;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct _dyneither_ptr)_tmpF6,((
_tmp49D=".c",_tag_dyneither(_tmp49D,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp4A1;void*_tmp4A0[1];struct Cyc_String_pa_struct _tmp49F;(_tmp49F.tag=0,((
_tmp49F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4A0[0]=&
_tmp49F,Cyc_fprintf(Cyc_stderr,((_tmp4A1="Compiling %s\n",_tag_dyneither(_tmp4A1,
sizeof(char),14))),_tag_dyneither(_tmp4A0,sizeof(void*),1)))))));}{const char*
_tmp4A3;const char*_tmp4A2;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp4A2="r",_tag_dyneither(_tmp4A2,sizeof(char),2))),((_tmp4A3="input file",
_tag_dyneither(_tmp4A3,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp4AA;
struct _dyneither_ptr*_tmp4A9;const char*_tmp4A8;struct Cyc_List_List*_tmp4A7;
struct _dyneither_ptr _tmpFE=Cyc_str_sepstr(((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((
_tmp4A7->hd=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9[0]=(struct
_dyneither_ptr)((_tmp4A8="",_tag_dyneither(_tmp4A8,sizeof(char),1))),_tmp4A9)))),((
_tmp4A7->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp4A7)))))),((_tmp4AA=" ",_tag_dyneither(_tmp4AA,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpFF=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp4AB;_tmpFF=Cyc_add_subdir(_tmpFF,((_tmp4AB="include",
_tag_dyneither(_tmp4AB,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp4AE;struct Cyc_List_List*_tmp4AD;
_tmpFF=((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD->hd=((_tmp4AE=_cycalloc(
sizeof(*_tmp4AE)),((_tmp4AE[0]=def_inc_path,_tmp4AE)))),((_tmp4AD->tl=_tmpFF,
_tmp4AD))))));}{char*_tmp103=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmp103 != 0){char*_tmp4AF;_tmpFF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4AF=_tmp103,
_tag_dyneither(_tmp4AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4AF,1)))),':'),
_tmpFF);}{const char*_tmp4B6;struct _dyneither_ptr*_tmp4B5;const char*_tmp4B4;
struct Cyc_List_List*_tmp4B3;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->hd=((
_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5[0]=(struct _dyneither_ptr)((_tmp4B4="",
_tag_dyneither(_tmp4B4,sizeof(char),1))),_tmp4B5)))),((_tmp4B3->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpFF),_tmp4B3)))))),((_tmp4B6=" -I",_tag_dyneither(
_tmp4B6,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp4BA;void*_tmp4B9[1];struct Cyc_String_pa_struct
_tmp4B8;ofile_string=(struct _dyneither_ptr)((_tmp4B8.tag=0,((_tmp4B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp4B9[0]=& _tmp4B8,Cyc_aprintf(((_tmp4BA=" > %s",_tag_dyneither(_tmp4BA,sizeof(
char),6))),_tag_dyneither(_tmp4B9,sizeof(void*),1))))))));}else{const char*
_tmp4BB;ofile_string=((_tmp4BB="",_tag_dyneither(_tmp4BB,sizeof(char),1)));}}
else{const char*_tmp4BF;void*_tmp4BE[1];struct Cyc_String_pa_struct _tmp4BD;
ofile_string=(struct _dyneither_ptr)((_tmp4BD.tag=0,((_tmp4BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpF7)),((_tmp4BE[0]=& _tmp4BD,Cyc_aprintf(((_tmp4BF=" > %s",_tag_dyneither(
_tmp4BF,sizeof(char),6))),_tag_dyneither(_tmp4BE,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp4C0;fixup_string=((_tmp4C0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp4C0,sizeof(char),35)));}else{const char*_tmp4C4;void*_tmp4C3[1];
struct Cyc_String_pa_struct _tmp4C2;fixup_string=(struct _dyneither_ptr)((_tmp4C2.tag=
0,((_tmp4C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4C3[0]=& _tmp4C2,Cyc_aprintf(((
_tmp4C4=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4C4,sizeof(char),29))),
_tag_dyneither(_tmp4C3,sizeof(void*),1))))))));}}else{const char*_tmp4C5;
fixup_string=((_tmp4C5="",_tag_dyneither(_tmp4C5,sizeof(char),1)));}{const char*
_tmp4CE;void*_tmp4CD[6];struct Cyc_String_pa_struct _tmp4CC;struct Cyc_String_pa_struct
_tmp4CB;struct Cyc_String_pa_struct _tmp4CA;struct Cyc_String_pa_struct _tmp4C9;
struct Cyc_String_pa_struct _tmp4C8;struct Cyc_String_pa_struct _tmp4C7;struct
_dyneither_ptr _tmp111=(_tmp4C7.tag=0,((_tmp4C7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp4C8.tag=0,((_tmp4C8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp4C9.tag=0,((_tmp4C9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp4CA.tag=
0,((_tmp4CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp4CB.tag=0,((_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE),((
_tmp4CC.tag=0,((_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp4CD[0]=& _tmp4CC,((_tmp4CD[1]=& _tmp4CB,((_tmp4CD[2]=& _tmp4CA,((_tmp4CD[3]=&
_tmp4C9,((_tmp4CD[4]=& _tmp4C8,((_tmp4CD[5]=& _tmp4C7,Cyc_aprintf(((_tmp4CE="%s %s%s %s%s%s",
_tag_dyneither(_tmp4CE,sizeof(char),15))),_tag_dyneither(_tmp4CD,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp4D2;void*_tmp4D1[1];struct Cyc_String_pa_struct _tmp4D0;(
_tmp4D0.tag=0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp111),((
_tmp4D1[0]=& _tmp4D0,Cyc_fprintf(Cyc_stderr,((_tmp4D2="%s\n",_tag_dyneither(
_tmp4D2,sizeof(char),4))),_tag_dyneither(_tmp4D1,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp111,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp4D5;void*_tmp4D4;(_tmp4D4=0,Cyc_fprintf(Cyc_stderr,((_tmp4D5="\nError: preprocessing\n",
_tag_dyneither(_tmp4D5,sizeof(char),23))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpF7);{const char*_tmp4D7;const char*_tmp4D6;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF7,((_tmp4D6="r",
_tag_dyneither(_tmp4D6,sizeof(char),2))),((_tmp4D7="file",_tag_dyneither(_tmp4D7,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp117;_push_handler(& _tmp117);{int _tmp119=0;if(setjmp(
_tmp117.handler))_tmp119=1;if(!_tmp119){{const char*_tmp4D8;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4D8="parsing",_tag_dyneither(_tmp4D8,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp118=(
void*)_exn_thrown;void*_tmp11C=_tmp118;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpF7);(int)_throw(
_tmp11C);_LL29:;_LL2A:(void)_throw(_tmp11C);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF7);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp11D=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11D;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp11E=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple19 _tmp4D9;struct _tuple19 _tmp11F=(_tmp4D9.f1=
tc_rgn,((_tmp4D9.f2=_tmp11E,_tmp4D9)));{const char*_tmp4DA;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple19*,struct
Cyc_List_List*),struct _tuple19*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4DA="type checking",_tag_dyneither(_tmp4DA,sizeof(char),14))),Cyc_do_typecheck,&
_tmp11F,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpF7);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp4DB;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4DB="control-flow checking",_tag_dyneither(_tmp4DB,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF7);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpF8,sizeof(char),1)),(const char*)"r");
const char*_tmp4DD;const char*_tmp4DC;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpF9,((_tmp4DC="w",_tag_dyneither(_tmp4DC,sizeof(char),2))),((
_tmp4DD="interface object file",_tag_dyneither(_tmp4DD,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpF8);{struct _tuple20 _tmp4DE;struct _tuple20 _tmp122=(
_tmp4DE.f1=_tmp11E,((_tmp4DE.f2=inter_file,((_tmp4DE.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp4DE)))));{const char*_tmp4DF;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4DF="interface checking",_tag_dyneither(_tmp4DF,sizeof(char),19))),Cyc_do_interface,&
_tmp122,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp4E0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp4E0="translation to C",_tag_dyneither(_tmp4E0,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpF7);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpF7);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp4E1;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E1="post-pass to VC",_tag_dyneither(_tmp4E1,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp4E3;const char*_tmp4E2;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4E2="w",_tag_dyneither(
_tmp4E2,sizeof(char),2))),((_tmp4E3="output file",_tag_dyneither(_tmp4E3,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp4E5;const char*_tmp4E4;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4E4="w",_tag_dyneither(
_tmp4E4,sizeof(char),2))),((_tmp4E5="output file",_tag_dyneither(_tmp4E5,sizeof(
char),12))));}else{const char*_tmp4E7;const char*_tmp4E6;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFA,((_tmp4E6="w",_tag_dyneither(_tmp4E6,sizeof(char),2))),((
_tmp4E7="output file",_tag_dyneither(_tmp4E7,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp130;_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=
1;if(!_tmp132){if(!Cyc_noprint_r){const char*_tmp4E8;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E8="printing",_tag_dyneither(_tmp4E8,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp131=(void*)
_exn_thrown;void*_tmp135=_tmp131;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp4EB;struct Cyc_List_List*
_tmp4EA;Cyc_cfiles=((_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA->hd=((_tmp4EB=
_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB[0]=(struct _dyneither_ptr)_tmpFA,_tmp4EB)))),((
_tmp4EA->tl=Cyc_cfiles,_tmp4EA))))));}(int)_throw(_tmp135);_LL2E:;_LL2F:(void)
_throw(_tmp135);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp4EE;struct Cyc_List_List*_tmp4ED;Cyc_cfiles=((_tmp4ED=
_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED->hd=((_tmp4EE=_cycalloc(sizeof(*_tmp4EE)),((
_tmp4EE[0]=(struct _dyneither_ptr)_tmpFA,_tmp4EE)))),((_tmp4ED->tl=Cyc_cfiles,
_tmp4ED))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp152[8]="<final>";
static struct _dyneither_ptr Cyc_final_str={_tmp152,_tmp152,_tmp152 + 8};static
struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*
Cyc_read_cycio(struct _dyneither_ptr*n);static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _dyneither_ptr basename;{struct _handler_cons _tmp153;_push_handler(& _tmp153);{
int _tmp155=0;if(setjmp(_tmp153.handler))_tmp155=1;if(!_tmp155){basename=(struct
_dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp154=(void*)_exn_thrown;void*_tmp157=_tmp154;_LL31: {struct Cyc_Core_Invalid_argument_struct*
_tmp158=(struct Cyc_Core_Invalid_argument_struct*)_tmp157;if(_tmp158->tag != Cyc_Core_Invalid_argument)
goto _LL33;}_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmp157);_LL30:;}};}{
const char*_tmp4EF;struct _dyneither_ptr _tmp159=Cyc_strconcat((struct
_dyneither_ptr)basename,((_tmp4EF=".cycio",_tag_dyneither(_tmp4EF,sizeof(char),7))));
const char*_tmp4F1;const char*_tmp4F0;struct Cyc___cycFILE*_tmp15A=Cyc_try_file_open((
struct _dyneither_ptr)_tmp159,((_tmp4F0="rb",_tag_dyneither(_tmp4F0,sizeof(char),
3))),((_tmp4F1="interface object file",_tag_dyneither(_tmp4F1,sizeof(char),22))));
if(_tmp15A == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp159);{struct Cyc_Interface_I*_tmp15B=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp15A);Cyc_file_close((struct Cyc___cycFILE*)_tmp15A);
return _tmp15B;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n);static int Cyc_is_cfile(
struct _dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple21{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4F7(unsigned int*
_tmp4F6,unsigned int*_tmp4F5,int**_tmp4F3){for(*_tmp4F6=0;*_tmp4F6 < *_tmp4F5;(*
_tmp4F6)++){(*_tmp4F3)[*_tmp4F6]=0;}}static void _tmp4FF(unsigned int*_tmp4FE,
unsigned int*_tmp4FD,struct _dyneither_ptr**_tmp4FB){for(*_tmp4FE=0;*_tmp4FE < *
_tmp4FD;(*_tmp4FE)++){(*_tmp4FB)[*_tmp4FE]=(struct _dyneither_ptr)_tag_dyneither(
0,0,0);}}static void _tmp505(unsigned int*_tmp504,unsigned int*_tmp503,struct
_dyneither_ptr**_tmp501){for(*_tmp504=0;*_tmp504 < *_tmp503;(*_tmp504)++){(*
_tmp501)[*_tmp504]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp15F=
_new_region("r");struct _RegionHandle*r=& _tmp15F;_push_region(r);{unsigned int
_tmp4F6;unsigned int _tmp4F5;struct _dyneither_ptr _tmp4F4;int*_tmp4F3;unsigned int
_tmp4F2;struct _dyneither_ptr _tmp160=(_tmp4F2=(unsigned int)argc,((_tmp4F3=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp4F2)),((_tmp4F4=_tag_dyneither(
_tmp4F3,sizeof(int),_tmp4F2),((((_tmp4F5=_tmp4F2,_tmp4F7(& _tmp4F6,& _tmp4F5,&
_tmp4F3))),_tmp4F4)))))));int _tmp161=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp4F8;if(Cyc_strncmp(((_tmp4F8="-B",_tag_dyneither(_tmp4F8,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp160,sizeof(int),i))=1;++ _tmp161;}else{const char*_tmp4F9;if(Cyc_strcmp(((
_tmp4F9="-b",_tag_dyneither(_tmp4F9,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp160,sizeof(int),
i))=1;++ _tmp161;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp160,
sizeof(int),i))=1;++ _tmp161;}}}{unsigned int _tmp4FE;unsigned int _tmp4FD;struct
_dyneither_ptr _tmp4FC;struct _dyneither_ptr*_tmp4FB;unsigned int _tmp4FA;struct
_dyneither_ptr _tmp164=(_tmp4FA=(unsigned int)(_tmp161 + 1),((_tmp4FB=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4FA)),((
_tmp4FC=_tag_dyneither(_tmp4FB,sizeof(struct _dyneither_ptr),_tmp4FA),((((_tmp4FD=
_tmp4FA,_tmp4FF(& _tmp4FE,& _tmp4FD,& _tmp4FB))),_tmp4FC)))))));unsigned int _tmp504;
unsigned int _tmp503;struct _dyneither_ptr _tmp502;struct _dyneither_ptr*_tmp501;
unsigned int _tmp500;struct _dyneither_ptr _tmp165=(_tmp500=(unsigned int)(argc - 
_tmp161),((_tmp501=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp500)),((_tmp502=_tag_dyneither(_tmp501,sizeof(struct
_dyneither_ptr),_tmp500),((((_tmp503=_tmp500,_tmp505(& _tmp504,& _tmp503,& _tmp501))),
_tmp502)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp160,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple21*_tmpA04;const char*_tmpA03;const char*
_tmpA02;struct Cyc_Arg_Set_spec_struct _tmpA01;struct Cyc_Arg_Set_spec_struct*
_tmpA00;const char*_tmp9FF;struct _tuple21*_tmp9FE;const char*_tmp9FD;const char*
_tmp9FC;struct Cyc_Arg_Unit_spec_struct _tmp9FB;struct Cyc_Arg_Unit_spec_struct*
_tmp9FA;const char*_tmp9F9;struct _tuple21*_tmp9F8;const char*_tmp9F7;const char*
_tmp9F6;struct Cyc_Arg_String_spec_struct _tmp9F5;struct Cyc_Arg_String_spec_struct*
_tmp9F4;const char*_tmp9F3;struct _tuple21*_tmp9F2;const char*_tmp9F1;const char*
_tmp9F0;struct Cyc_Arg_Flag_spec_struct _tmp9EF;struct Cyc_Arg_Flag_spec_struct*
_tmp9EE;const char*_tmp9ED;struct _tuple21*_tmp9EC;const char*_tmp9EB;const char*
_tmp9EA;struct Cyc_Arg_Flag_spec_struct _tmp9E9;struct Cyc_Arg_Flag_spec_struct*
_tmp9E8;const char*_tmp9E7;struct _tuple21*_tmp9E6;const char*_tmp9E5;const char*
_tmp9E4;struct Cyc_Arg_Flag_spec_struct _tmp9E3;struct Cyc_Arg_Flag_spec_struct*
_tmp9E2;const char*_tmp9E1;struct _tuple21*_tmp9E0;const char*_tmp9DF;const char*
_tmp9DE;struct Cyc_Arg_Flag_spec_struct _tmp9DD;struct Cyc_Arg_Flag_spec_struct*
_tmp9DC;const char*_tmp9DB;struct _tuple21*_tmp9DA;const char*_tmp9D9;const char*
_tmp9D8;struct Cyc_Arg_Flag_spec_struct _tmp9D7;struct Cyc_Arg_Flag_spec_struct*
_tmp9D6;const char*_tmp9D5;struct _tuple21*_tmp9D4;const char*_tmp9D3;const char*
_tmp9D2;struct Cyc_Arg_Flag_spec_struct _tmp9D1;struct Cyc_Arg_Flag_spec_struct*
_tmp9D0;const char*_tmp9CF;struct _tuple21*_tmp9CE;const char*_tmp9CD;const char*
_tmp9CC;struct Cyc_Arg_Unit_spec_struct _tmp9CB;struct Cyc_Arg_Unit_spec_struct*
_tmp9CA;const char*_tmp9C9;struct _tuple21*_tmp9C8;const char*_tmp9C7;const char*
_tmp9C6;struct Cyc_Arg_String_spec_struct _tmp9C5;struct Cyc_Arg_String_spec_struct*
_tmp9C4;const char*_tmp9C3;struct _tuple21*_tmp9C2;const char*_tmp9C1;const char*
_tmp9C0;struct Cyc_Arg_Flag_spec_struct _tmp9BF;struct Cyc_Arg_Flag_spec_struct*
_tmp9BE;const char*_tmp9BD;struct _tuple21*_tmp9BC;const char*_tmp9BB;const char*
_tmp9BA;struct Cyc_Arg_Flag_spec_struct _tmp9B9;struct Cyc_Arg_Flag_spec_struct*
_tmp9B8;const char*_tmp9B7;struct _tuple21*_tmp9B6;const char*_tmp9B5;const char*
_tmp9B4;struct Cyc_Arg_Flag_spec_struct _tmp9B3;struct Cyc_Arg_Flag_spec_struct*
_tmp9B2;const char*_tmp9B1;struct _tuple21*_tmp9B0;const char*_tmp9AF;const char*
_tmp9AE;struct Cyc_Arg_Flag_spec_struct _tmp9AD;struct Cyc_Arg_Flag_spec_struct*
_tmp9AC;const char*_tmp9AB;struct _tuple21*_tmp9AA;const char*_tmp9A9;const char*
_tmp9A8;struct Cyc_Arg_Flag_spec_struct _tmp9A7;struct Cyc_Arg_Flag_spec_struct*
_tmp9A6;const char*_tmp9A5;struct _tuple21*_tmp9A4;const char*_tmp9A3;const char*
_tmp9A2;struct Cyc_Arg_Flag_spec_struct _tmp9A1;struct Cyc_Arg_Flag_spec_struct*
_tmp9A0;const char*_tmp99F;struct _tuple21*_tmp99E;const char*_tmp99D;const char*
_tmp99C;struct Cyc_Arg_Flag_spec_struct _tmp99B;struct Cyc_Arg_Flag_spec_struct*
_tmp99A;const char*_tmp999;struct _tuple21*_tmp998;const char*_tmp997;const char*
_tmp996;struct Cyc_Arg_Flag_spec_struct _tmp995;struct Cyc_Arg_Flag_spec_struct*
_tmp994;const char*_tmp993;struct _tuple21*_tmp992;const char*_tmp991;const char*
_tmp990;struct Cyc_Arg_Unit_spec_struct _tmp98F;struct Cyc_Arg_Unit_spec_struct*
_tmp98E;const char*_tmp98D;struct _tuple21*_tmp98C;const char*_tmp98B;const char*
_tmp98A;struct Cyc_Arg_Unit_spec_struct _tmp989;struct Cyc_Arg_Unit_spec_struct*
_tmp988;const char*_tmp987;struct _tuple21*_tmp986;const char*_tmp985;const char*
_tmp984;struct Cyc_Arg_Unit_spec_struct _tmp983;struct Cyc_Arg_Unit_spec_struct*
_tmp982;const char*_tmp981;struct _tuple21*_tmp980;const char*_tmp97F;const char*
_tmp97E;struct Cyc_Arg_Unit_spec_struct _tmp97D;struct Cyc_Arg_Unit_spec_struct*
_tmp97C;const char*_tmp97B;struct _tuple21*_tmp97A;const char*_tmp979;const char*
_tmp978;struct Cyc_Arg_Flag_spec_struct _tmp977;struct Cyc_Arg_Flag_spec_struct*
_tmp976;const char*_tmp975;struct _tuple21*_tmp974;const char*_tmp973;const char*
_tmp972;struct Cyc_Arg_String_spec_struct _tmp971;struct Cyc_Arg_String_spec_struct*
_tmp970;const char*_tmp96F;struct _tuple21*_tmp96E;const char*_tmp96D;const char*
_tmp96C;struct Cyc_Arg_String_spec_struct _tmp96B;struct Cyc_Arg_String_spec_struct*
_tmp96A;const char*_tmp969;struct _tuple21*_tmp968;const char*_tmp967;const char*
_tmp966;struct Cyc_Arg_Unit_spec_struct _tmp965;struct Cyc_Arg_Unit_spec_struct*
_tmp964;const char*_tmp963;struct _tuple21*_tmp962;const char*_tmp961;const char*
_tmp960;struct Cyc_Arg_Set_spec_struct _tmp95F;struct Cyc_Arg_Set_spec_struct*
_tmp95E;const char*_tmp95D;struct _tuple21*_tmp95C;const char*_tmp95B;const char*
_tmp95A;struct Cyc_Arg_Set_spec_struct _tmp959;struct Cyc_Arg_Set_spec_struct*
_tmp958;const char*_tmp957;struct _tuple21*_tmp956;const char*_tmp955;const char*
_tmp954;struct Cyc_Arg_Set_spec_struct _tmp953;struct Cyc_Arg_Set_spec_struct*
_tmp952;const char*_tmp951;struct _tuple21*_tmp950;const char*_tmp94F;const char*
_tmp94E;struct Cyc_Arg_Set_spec_struct _tmp94D;struct Cyc_Arg_Set_spec_struct*
_tmp94C;const char*_tmp94B;struct _tuple21*_tmp94A;const char*_tmp949;const char*
_tmp948;struct Cyc_Arg_Set_spec_struct _tmp947;struct Cyc_Arg_Set_spec_struct*
_tmp946;const char*_tmp945;struct _tuple21*_tmp944;const char*_tmp943;const char*
_tmp942;struct Cyc_Arg_Set_spec_struct _tmp941;struct Cyc_Arg_Set_spec_struct*
_tmp940;const char*_tmp93F;struct _tuple21*_tmp93E;const char*_tmp93D;const char*
_tmp93C;struct Cyc_Arg_Set_spec_struct _tmp93B;struct Cyc_Arg_Set_spec_struct*
_tmp93A;const char*_tmp939;struct _tuple21*_tmp938;const char*_tmp937;const char*
_tmp936;struct Cyc_Arg_Set_spec_struct _tmp935;struct Cyc_Arg_Set_spec_struct*
_tmp934;const char*_tmp933;struct _tuple21*_tmp932;const char*_tmp931;const char*
_tmp930;struct Cyc_Arg_Clear_spec_struct _tmp92F;struct Cyc_Arg_Clear_spec_struct*
_tmp92E;const char*_tmp92D;struct _tuple21*_tmp92C;const char*_tmp92B;const char*
_tmp92A;struct Cyc_Arg_Set_spec_struct _tmp929;struct Cyc_Arg_Set_spec_struct*
_tmp928;const char*_tmp927;struct _tuple21*_tmp926;const char*_tmp925;const char*
_tmp924;struct Cyc_Arg_Clear_spec_struct _tmp923;struct Cyc_Arg_Clear_spec_struct*
_tmp922;const char*_tmp921;struct _tuple21*_tmp920;const char*_tmp91F;const char*
_tmp91E;struct Cyc_Arg_Unit_spec_struct _tmp91D;struct Cyc_Arg_Unit_spec_struct*
_tmp91C;const char*_tmp91B;struct _tuple21*_tmp91A;const char*_tmp919;const char*
_tmp918;struct Cyc_Arg_String_spec_struct _tmp917;struct Cyc_Arg_String_spec_struct*
_tmp916;const char*_tmp915;struct _tuple21*_tmp914;const char*_tmp913;const char*
_tmp912;struct Cyc_Arg_Unit_spec_struct _tmp911;struct Cyc_Arg_Unit_spec_struct*
_tmp910;const char*_tmp90F;struct _tuple21*_tmp90E;const char*_tmp90D;const char*
_tmp90C;struct Cyc_Arg_Set_spec_struct _tmp90B;struct Cyc_Arg_Set_spec_struct*
_tmp90A;const char*_tmp909;struct _tuple21*_tmp908;const char*_tmp907;const char*
_tmp906;struct Cyc_Arg_Unit_spec_struct _tmp905;struct Cyc_Arg_Unit_spec_struct*
_tmp904;const char*_tmp903;struct _tuple21*_tmp902;const char*_tmp901;const char*
_tmp900;struct Cyc_Arg_Unit_spec_struct _tmp8FF;struct Cyc_Arg_Unit_spec_struct*
_tmp8FE;const char*_tmp8FD;struct _tuple21*_tmp8FC;const char*_tmp8FB;const char*
_tmp8FA;struct Cyc_Arg_Unit_spec_struct _tmp8F9;struct Cyc_Arg_Unit_spec_struct*
_tmp8F8;const char*_tmp8F7;struct _tuple21*_tmp8F6;const char*_tmp8F5;const char*
_tmp8F4;struct Cyc_Arg_Unit_spec_struct _tmp8F3;struct Cyc_Arg_Unit_spec_struct*
_tmp8F2;const char*_tmp8F1;struct _tuple21*_tmp8F0;const char*_tmp8EF;const char*
_tmp8EE;struct Cyc_Arg_Unit_spec_struct _tmp8ED;struct Cyc_Arg_Unit_spec_struct*
_tmp8EC;const char*_tmp8EB;struct _tuple21*_tmp8EA;const char*_tmp8E9;const char*
_tmp8E8;struct Cyc_Arg_String_spec_struct _tmp8E7;struct Cyc_Arg_String_spec_struct*
_tmp8E6;const char*_tmp8E5;struct _tuple21*_tmp8E4;const char*_tmp8E3;const char*
_tmp8E2;struct Cyc_Arg_Unit_spec_struct _tmp8E1;struct Cyc_Arg_Unit_spec_struct*
_tmp8E0;const char*_tmp8DF;struct _tuple21*_tmp8DE;const char*_tmp8DD;const char*
_tmp8DC;struct Cyc_Arg_Unit_spec_struct _tmp8DB;struct Cyc_Arg_Unit_spec_struct*
_tmp8DA;const char*_tmp8D9;struct _tuple21*_tmp8D8;const char*_tmp8D7;const char*
_tmp8D6;struct Cyc_Arg_Set_spec_struct _tmp8D5;struct Cyc_Arg_Set_spec_struct*
_tmp8D4;const char*_tmp8D3;struct _tuple21*_tmp8D2;const char*_tmp8D1;const char*
_tmp8D0;struct Cyc_Arg_Set_spec_struct _tmp8CF;struct Cyc_Arg_Set_spec_struct*
_tmp8CE;const char*_tmp8CD;struct _tuple21*_tmp8CC;const char*_tmp8CB;const char*
_tmp8CA;struct Cyc_Arg_Set_spec_struct _tmp8C9;struct Cyc_Arg_Set_spec_struct*
_tmp8C8;const char*_tmp8C7;struct _tuple21*_tmp8C6;const char*_tmp8C5;const char*
_tmp8C4;struct Cyc_Arg_Set_spec_struct _tmp8C3;struct Cyc_Arg_Set_spec_struct*
_tmp8C2;const char*_tmp8C1;struct _tuple21*_tmp8C0;const char*_tmp8BF;const char*
_tmp8BE;struct Cyc_Arg_Set_spec_struct _tmp8BD;struct Cyc_Arg_Set_spec_struct*
_tmp8BC;const char*_tmp8BB;struct _tuple21*_tmp8BA;const char*_tmp8B9;const char*
_tmp8B8;struct Cyc_Arg_Set_spec_struct _tmp8B7;struct Cyc_Arg_Set_spec_struct*
_tmp8B6;const char*_tmp8B5;struct _tuple21*_tmp8B4;const char*_tmp8B3;const char*
_tmp8B2;struct Cyc_Arg_Set_spec_struct _tmp8B1;struct Cyc_Arg_Set_spec_struct*
_tmp8B0;const char*_tmp8AF;struct _tuple21*_tmp8AE;const char*_tmp8AD;const char*
_tmp8AC;struct Cyc_Arg_Set_spec_struct _tmp8AB;struct Cyc_Arg_Set_spec_struct*
_tmp8AA;const char*_tmp8A9;struct _tuple21*_tmp8A8;const char*_tmp8A7;const char*
_tmp8A6;struct Cyc_Arg_Set_spec_struct _tmp8A5;struct Cyc_Arg_Set_spec_struct*
_tmp8A4;const char*_tmp8A3;struct _tuple21*_tmp8A2;const char*_tmp8A1;const char*
_tmp8A0;struct Cyc_Arg_String_spec_struct _tmp89F;struct Cyc_Arg_String_spec_struct*
_tmp89E;const char*_tmp89D;struct _tuple21*_tmp89C;const char*_tmp89B;const char*
_tmp89A;struct Cyc_Arg_Set_spec_struct _tmp899;struct Cyc_Arg_Set_spec_struct*
_tmp898;const char*_tmp897;struct _tuple21*_tmp896;const char*_tmp895;const char*
_tmp894;struct Cyc_Arg_Set_spec_struct _tmp893;struct Cyc_Arg_Set_spec_struct*
_tmp892;const char*_tmp891;struct _tuple21*_tmp890;const char*_tmp88F;const char*
_tmp88E;struct Cyc_Arg_Set_spec_struct _tmp88D;struct Cyc_Arg_Set_spec_struct*
_tmp88C;const char*_tmp88B;struct _tuple21*_tmp88A;const char*_tmp889;const char*
_tmp888;struct Cyc_Arg_Clear_spec_struct _tmp887;struct Cyc_Arg_Clear_spec_struct*
_tmp886;const char*_tmp885;struct _tuple21*_tmp884;const char*_tmp883;const char*
_tmp882;struct Cyc_Arg_Unit_spec_struct _tmp881;struct Cyc_Arg_Unit_spec_struct*
_tmp880;const char*_tmp87F;struct _tuple21*_tmp87E;const char*_tmp87D;const char*
_tmp87C;struct Cyc_Arg_Set_spec_struct _tmp87B;struct Cyc_Arg_Set_spec_struct*
_tmp87A;const char*_tmp879;struct _tuple21*_tmp878;const char*_tmp877;const char*
_tmp876;struct Cyc_Arg_Flag_spec_struct _tmp875;struct Cyc_Arg_Flag_spec_struct*
_tmp874;const char*_tmp873;struct _tuple21*_tmp872;const char*_tmp871;const char*
_tmp870;struct Cyc_Arg_Flag_spec_struct _tmp86F;struct Cyc_Arg_Flag_spec_struct*
_tmp86E;const char*_tmp86D;struct _tuple21*_tmp86C;const char*_tmp86B;const char*
_tmp86A;struct Cyc_Arg_Flag_spec_struct _tmp869;struct Cyc_Arg_Flag_spec_struct*
_tmp868;const char*_tmp867;struct _tuple21*_tmp866;const char*_tmp865;const char*
_tmp864;struct Cyc_Arg_Flag_spec_struct _tmp863;struct Cyc_Arg_Flag_spec_struct*
_tmp862;const char*_tmp861;struct _tuple21*_tmp860;const char*_tmp85F;const char*
_tmp85E;struct Cyc_Arg_Flag_spec_struct _tmp85D;struct Cyc_Arg_Flag_spec_struct*
_tmp85C;const char*_tmp85B;struct _tuple21*_tmp85A[71];struct Cyc_List_List*options=(
_tmp85A[70]=((_tmp860=_region_malloc(r,sizeof(*_tmp860)),((_tmp860->f1=((_tmp85F="-idirafter",
_tag_dyneither(_tmp85F,sizeof(char),11))),((_tmp860->f2=1,((_tmp860->f3=((
_tmp85E="<dir>",_tag_dyneither(_tmp85E,sizeof(char),6))),((_tmp860->f4=(void*)((
_tmp85C=_region_malloc(r,sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85D.tag=1,((
_tmp85D.f1=Cyc_add_idirafter,_tmp85D)))),_tmp85C)))),((_tmp860->f5=((_tmp85B="Add the directory to the second include path.",
_tag_dyneither(_tmp85B,sizeof(char),46))),_tmp860)))))))))))),((_tmp85A[69]=((
_tmp866=_region_malloc(r,sizeof(*_tmp866)),((_tmp866->f1=((_tmp865="-isystem",
_tag_dyneither(_tmp865,sizeof(char),9))),((_tmp866->f2=1,((_tmp866->f3=((_tmp864="<dir>",
_tag_dyneither(_tmp864,sizeof(char),6))),((_tmp866->f4=(void*)((_tmp862=
_region_malloc(r,sizeof(*_tmp862)),((_tmp862[0]=((_tmp863.tag=1,((_tmp863.f1=Cyc_add_isystem,
_tmp863)))),_tmp862)))),((_tmp866->f5=((_tmp861="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",
_tag_dyneither(_tmp861,sizeof(char),90))),_tmp866)))))))))))),((_tmp85A[68]=((
_tmp86C=_region_malloc(r,sizeof(*_tmp86C)),((_tmp86C->f1=((_tmp86B="-iwithprefixbefore",
_tag_dyneither(_tmp86B,sizeof(char),19))),((_tmp86C->f2=1,((_tmp86C->f3=((
_tmp86A="<dir>",_tag_dyneither(_tmp86A,sizeof(char),6))),((_tmp86C->f4=(void*)((
_tmp868=_region_malloc(r,sizeof(*_tmp868)),((_tmp868[0]=((_tmp869.tag=1,((
_tmp869.f1=Cyc_add_iwithprefixbefore,_tmp869)))),_tmp868)))),((_tmp86C->f5=((
_tmp867="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp867,
sizeof(char),45))),_tmp86C)))))))))))),((_tmp85A[67]=((_tmp872=_region_malloc(r,
sizeof(*_tmp872)),((_tmp872->f1=((_tmp871="-iwithprefix",_tag_dyneither(_tmp871,
sizeof(char),13))),((_tmp872->f2=1,((_tmp872->f3=((_tmp870="<dir>",
_tag_dyneither(_tmp870,sizeof(char),6))),((_tmp872->f4=(void*)((_tmp86E=
_region_malloc(r,sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=1,((_tmp86F.f1=Cyc_add_iwithprefix,
_tmp86F)))),_tmp86E)))),((_tmp872->f5=((_tmp86D="Add <prefix>/<dir> to the second include path.",
_tag_dyneither(_tmp86D,sizeof(char),47))),_tmp872)))))))))))),((_tmp85A[66]=((
_tmp878=_region_malloc(r,sizeof(*_tmp878)),((_tmp878->f1=((_tmp877="-iprefix",
_tag_dyneither(_tmp877,sizeof(char),9))),((_tmp878->f2=1,((_tmp878->f3=((_tmp876="<prefix>",
_tag_dyneither(_tmp876,sizeof(char),9))),((_tmp878->f4=(void*)((_tmp874=
_region_malloc(r,sizeof(*_tmp874)),((_tmp874[0]=((_tmp875.tag=1,((_tmp875.f1=Cyc_add_iprefix,
_tmp875)))),_tmp874)))),((_tmp878->f5=((_tmp873="Specify <prefix> as the prefix for subsequent -iwithprefix options",
_tag_dyneither(_tmp873,sizeof(char),67))),_tmp878)))))))))))),((_tmp85A[65]=((
_tmp87E=_region_malloc(r,sizeof(*_tmp87E)),((_tmp87E->f1=((_tmp87D="-noregions",
_tag_dyneither(_tmp87D,sizeof(char),11))),((_tmp87E->f2=0,((_tmp87E->f3=((
_tmp87C="",_tag_dyneither(_tmp87C,sizeof(char),1))),((_tmp87E->f4=(void*)((
_tmp87A=_region_malloc(r,sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87B.tag=3,((
_tmp87B.f1=& Cyc_Absyn_no_regions,_tmp87B)))),_tmp87A)))),((_tmp87E->f5=((_tmp879="Generate code that doesn't use regions",
_tag_dyneither(_tmp879,sizeof(char),39))),_tmp87E)))))))))))),((_tmp85A[64]=((
_tmp884=_region_malloc(r,sizeof(*_tmp884)),((_tmp884->f1=((_tmp883="-port",
_tag_dyneither(_tmp883,sizeof(char),6))),((_tmp884->f2=0,((_tmp884->f3=((_tmp882="",
_tag_dyneither(_tmp882,sizeof(char),1))),((_tmp884->f4=(void*)((_tmp880=
_region_malloc(r,sizeof(*_tmp880)),((_tmp880[0]=((_tmp881.tag=0,((_tmp881.f1=Cyc_set_port_c_code,
_tmp881)))),_tmp880)))),((_tmp884->f5=((_tmp87F="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp87F,sizeof(char),38))),_tmp884)))))))))))),((_tmp85A[63]=((
_tmp88A=_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A->f1=((_tmp889="-detailedlocation",
_tag_dyneither(_tmp889,sizeof(char),18))),((_tmp88A->f2=0,((_tmp88A->f3=((
_tmp888="",_tag_dyneither(_tmp888,sizeof(char),1))),((_tmp88A->f4=(void*)((
_tmp886=_region_malloc(r,sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=4,((
_tmp887.f1=& Cyc_Position_use_gcc_style_location,_tmp887)))),_tmp886)))),((
_tmp88A->f5=((_tmp885="Try to give more detailed location information for errors",
_tag_dyneither(_tmp885,sizeof(char),58))),_tmp88A)))))))))))),((_tmp85A[62]=((
_tmp890=_region_malloc(r,sizeof(*_tmp890)),((_tmp890->f1=((_tmp88F="-noregister",
_tag_dyneither(_tmp88F,sizeof(char),12))),((_tmp890->f2=0,((_tmp890->f3=((
_tmp88E="",_tag_dyneither(_tmp88E,sizeof(char),1))),((_tmp890->f4=(void*)((
_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88D.tag=3,((
_tmp88D.f1=& Cyc_Parse_no_register,_tmp88D)))),_tmp88C)))),((_tmp890->f5=((
_tmp88B="Treat register storage class as public",_tag_dyneither(_tmp88B,sizeof(
char),39))),_tmp890)))))))))))),((_tmp85A[61]=((_tmp896=_region_malloc(r,sizeof(*
_tmp896)),((_tmp896->f1=((_tmp895="-warnnullchecks",_tag_dyneither(_tmp895,
sizeof(char),16))),((_tmp896->f2=0,((_tmp896->f3=((_tmp894="",_tag_dyneither(
_tmp894,sizeof(char),1))),((_tmp896->f4=(void*)((_tmp892=_region_malloc(r,
sizeof(*_tmp892)),((_tmp892[0]=((_tmp893.tag=3,((_tmp893.f1=& Cyc_Toc_warn_all_null_deref,
_tmp893)))),_tmp892)))),((_tmp896->f5=((_tmp891="Warn when any null check can't be eliminated",
_tag_dyneither(_tmp891,sizeof(char),45))),_tmp896)))))))))))),((_tmp85A[60]=((
_tmp89C=_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C->f1=((_tmp89B="-warnboundschecks",
_tag_dyneither(_tmp89B,sizeof(char),18))),((_tmp89C->f2=0,((_tmp89C->f3=((
_tmp89A="",_tag_dyneither(_tmp89A,sizeof(char),1))),((_tmp89C->f4=(void*)((
_tmp898=_region_malloc(r,sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=3,((
_tmp899.f1=& Cyc_Toc_warn_bounds_checks,_tmp899)))),_tmp898)))),((_tmp89C->f5=((
_tmp897="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp897,
sizeof(char),44))),_tmp89C)))))))))))),((_tmp85A[59]=((_tmp8A2=_region_malloc(r,
sizeof(*_tmp8A2)),((_tmp8A2->f1=((_tmp8A1="-CI",_tag_dyneither(_tmp8A1,sizeof(
char),4))),((_tmp8A2->f2=0,((_tmp8A2->f3=((_tmp8A0=" <file>",_tag_dyneither(
_tmp8A0,sizeof(char),8))),((_tmp8A2->f4=(void*)((_tmp89E=_region_malloc(r,
sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=5,((_tmp89F.f1=Cyc_set_cyc_include,
_tmp89F)))),_tmp89E)))),((_tmp8A2->f5=((_tmp89D="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp89D,sizeof(char),31))),_tmp8A2)))))))))))),((_tmp85A[58]=((
_tmp8A8=_region_malloc(r,sizeof(*_tmp8A8)),((_tmp8A8->f1=((_tmp8A7="-compile-for-boot",
_tag_dyneither(_tmp8A7,sizeof(char),18))),((_tmp8A8->f2=0,((_tmp8A8->f3=((
_tmp8A6="",_tag_dyneither(_tmp8A6,sizeof(char),1))),((_tmp8A8->f4=(void*)((
_tmp8A4=_region_malloc(r,sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A5.tag=3,((
_tmp8A5.f1=& Cyc_compile_for_boot_r,_tmp8A5)))),_tmp8A4)))),((_tmp8A8->f5=((
_tmp8A3="Compile using the special boot library instead of the standard library",
_tag_dyneither(_tmp8A3,sizeof(char),71))),_tmp8A8)))))))))))),((_tmp85A[57]=((
_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE->f1=((_tmp8AD="-nocyc_setjmp",
_tag_dyneither(_tmp8AD,sizeof(char),14))),((_tmp8AE->f2=0,((_tmp8AE->f3=((
_tmp8AC="",_tag_dyneither(_tmp8AC,sizeof(char),1))),((_tmp8AE->f4=(void*)((
_tmp8AA=_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=3,((
_tmp8AB.f1=& Cyc_nocyc_setjmp_r,_tmp8AB)))),_tmp8AA)))),((_tmp8AE->f5=((_tmp8A9="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp8A9,sizeof(char),46))),_tmp8AE)))))))))))),((_tmp85A[56]=((
_tmp8B4=_region_malloc(r,sizeof(*_tmp8B4)),((_tmp8B4->f1=((_tmp8B3="-printalleffects",
_tag_dyneither(_tmp8B3,sizeof(char),17))),((_tmp8B4->f2=0,((_tmp8B4->f3=((
_tmp8B2="",_tag_dyneither(_tmp8B2,sizeof(char),1))),((_tmp8B4->f4=(void*)((
_tmp8B0=_region_malloc(r,sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=3,((
_tmp8B1.f1=& Cyc_print_all_effects_r,_tmp8B1)))),_tmp8B0)))),((_tmp8B4->f5=((
_tmp8AF="Print effects for functions (type debugging)",_tag_dyneither(_tmp8AF,
sizeof(char),45))),_tmp8B4)))))))))))),((_tmp85A[55]=((_tmp8BA=_region_malloc(r,
sizeof(*_tmp8BA)),((_tmp8BA->f1=((_tmp8B9="-printfullevars",_tag_dyneither(
_tmp8B9,sizeof(char),16))),((_tmp8BA->f2=0,((_tmp8BA->f3=((_tmp8B8="",
_tag_dyneither(_tmp8B8,sizeof(char),1))),((_tmp8BA->f4=(void*)((_tmp8B6=
_region_malloc(r,sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B7.tag=3,((_tmp8B7.f1=&
Cyc_print_full_evars_r,_tmp8B7)))),_tmp8B6)))),((_tmp8BA->f5=((_tmp8B5="Print full information for evars (type debugging)",
_tag_dyneither(_tmp8B5,sizeof(char),50))),_tmp8BA)))))))))))),((_tmp85A[54]=((
_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0->f1=((_tmp8BF="-printallkinds",
_tag_dyneither(_tmp8BF,sizeof(char),15))),((_tmp8C0->f2=0,((_tmp8C0->f3=((
_tmp8BE="",_tag_dyneither(_tmp8BE,sizeof(char),1))),((_tmp8C0->f4=(void*)((
_tmp8BC=_region_malloc(r,sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=3,((
_tmp8BD.f1=& Cyc_print_all_kinds_r,_tmp8BD)))),_tmp8BC)))),((_tmp8C0->f5=((
_tmp8BB="Always print kinds of type variables",_tag_dyneither(_tmp8BB,sizeof(
char),37))),_tmp8C0)))))))))))),((_tmp85A[53]=((_tmp8C6=_region_malloc(r,sizeof(*
_tmp8C6)),((_tmp8C6->f1=((_tmp8C5="-printalltvars",_tag_dyneither(_tmp8C5,
sizeof(char),15))),((_tmp8C6->f2=0,((_tmp8C6->f3=((_tmp8C4="",_tag_dyneither(
_tmp8C4,sizeof(char),1))),((_tmp8C6->f4=(void*)((_tmp8C2=_region_malloc(r,
sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=3,((_tmp8C3.f1=& Cyc_print_all_tvars_r,
_tmp8C3)))),_tmp8C2)))),((_tmp8C6->f5=((_tmp8C1="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp8C1,sizeof(char),57))),_tmp8C6)))))))))))),((_tmp85A[52]=((
_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC->f1=((_tmp8CB="-noexpandtypedefs",
_tag_dyneither(_tmp8CB,sizeof(char),18))),((_tmp8CC->f2=0,((_tmp8CC->f3=((
_tmp8CA="",_tag_dyneither(_tmp8CA,sizeof(char),1))),((_tmp8CC->f4=(void*)((
_tmp8C8=_region_malloc(r,sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=3,((
_tmp8C9.f1=& Cyc_noexpand_r,_tmp8C9)))),_tmp8C8)))),((_tmp8CC->f5=((_tmp8C7="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp8C7,sizeof(char),41))),_tmp8CC)))))))))))),((_tmp85A[51]=((
_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2->f1=((_tmp8D1="-noremoveunused",
_tag_dyneither(_tmp8D1,sizeof(char),16))),((_tmp8D2->f2=0,((_tmp8D2->f3=((
_tmp8D0="",_tag_dyneither(_tmp8D0,sizeof(char),1))),((_tmp8D2->f4=(void*)((
_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CF.tag=3,((
_tmp8CF.f1=& Cyc_noshake_r,_tmp8CF)))),_tmp8CE)))),((_tmp8D2->f5=((_tmp8CD="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp8CD,sizeof(char),49))),_tmp8D2)))))))))))),((_tmp85A[50]=((
_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8->f1=((_tmp8D7="-nogc",
_tag_dyneither(_tmp8D7,sizeof(char),6))),((_tmp8D8->f2=0,((_tmp8D8->f3=((_tmp8D6="",
_tag_dyneither(_tmp8D6,sizeof(char),1))),((_tmp8D8->f4=(void*)((_tmp8D4=
_region_malloc(r,sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D5.tag=3,((_tmp8D5.f1=&
Cyc_nogc_r,_tmp8D5)))),_tmp8D4)))),((_tmp8D8->f5=((_tmp8D3="Don't link in the garbage collector",
_tag_dyneither(_tmp8D3,sizeof(char),36))),_tmp8D8)))))))))))),((_tmp85A[49]=((
_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE->f1=((_tmp8DD="-nocyc",
_tag_dyneither(_tmp8DD,sizeof(char),7))),((_tmp8DE->f2=0,((_tmp8DE->f3=((_tmp8DC="",
_tag_dyneither(_tmp8DC,sizeof(char),1))),((_tmp8DE->f4=(void*)((_tmp8DA=
_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=0,((_tmp8DB.f1=Cyc_set_nocyc,
_tmp8DB)))),_tmp8DA)))),((_tmp8DE->f5=((_tmp8D9="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp8D9,sizeof(char),33))),_tmp8DE)))))))))))),((_tmp85A[48]=((
_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4->f1=((_tmp8E3="-no-cpp-precomp",
_tag_dyneither(_tmp8E3,sizeof(char),16))),((_tmp8E4->f2=0,((_tmp8E4->f3=((
_tmp8E2="",_tag_dyneither(_tmp8E2,sizeof(char),1))),((_tmp8E4->f4=(void*)((
_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=0,((
_tmp8E1.f1=Cyc_set_nocppprecomp,_tmp8E1)))),_tmp8E0)))),((_tmp8E4->f5=((_tmp8DF="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp8DF,sizeof(char),40))),_tmp8E4)))))))))))),((_tmp85A[47]=((
_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA->f1=((_tmp8E9="-use-cpp",
_tag_dyneither(_tmp8E9,sizeof(char),9))),((_tmp8EA->f2=0,((_tmp8EA->f3=((_tmp8E8="<path>",
_tag_dyneither(_tmp8E8,sizeof(char),7))),((_tmp8EA->f4=(void*)((_tmp8E6=
_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=5,((_tmp8E7.f1=Cyc_set_cpp,
_tmp8E7)))),_tmp8E6)))),((_tmp8EA->f5=((_tmp8E5="Indicate which preprocessor to use",
_tag_dyneither(_tmp8E5,sizeof(char),35))),_tmp8EA)))))))))))),((_tmp85A[46]=((
_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0->f1=((_tmp8EF="--inline-checks",
_tag_dyneither(_tmp8EF,sizeof(char),16))),((_tmp8F0->f2=0,((_tmp8F0->f3=((
_tmp8EE="",_tag_dyneither(_tmp8EE,sizeof(char),1))),((_tmp8F0->f4=(void*)((
_tmp8EC=_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=0,((
_tmp8ED.f1=Cyc_set_inline_functions,_tmp8ED)))),_tmp8EC)))),((_tmp8F0->f5=((
_tmp8EB="Inline bounds checks instead of #define",_tag_dyneither(_tmp8EB,sizeof(
char),40))),_tmp8F0)))))))))))),((_tmp85A[45]=((_tmp8F6=_region_malloc(r,sizeof(*
_tmp8F6)),((_tmp8F6->f1=((_tmp8F5="--noboundschecks",_tag_dyneither(_tmp8F5,
sizeof(char),17))),((_tmp8F6->f2=0,((_tmp8F6->f3=((_tmp8F4="",_tag_dyneither(
_tmp8F4,sizeof(char),1))),((_tmp8F6->f4=(void*)((_tmp8F2=_region_malloc(r,
sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=0,((_tmp8F3.f1=Cyc_set_noboundschecks,
_tmp8F3)))),_tmp8F2)))),((_tmp8F6->f5=((_tmp8F1="Disable bounds checks",
_tag_dyneither(_tmp8F1,sizeof(char),22))),_tmp8F6)))))))))))),((_tmp85A[44]=((
_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC->f1=((_tmp8FB="--nonullchecks",
_tag_dyneither(_tmp8FB,sizeof(char),15))),((_tmp8FC->f2=0,((_tmp8FC->f3=((
_tmp8FA="",_tag_dyneither(_tmp8FA,sizeof(char),1))),((_tmp8FC->f4=(void*)((
_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=0,((
_tmp8F9.f1=Cyc_set_nonullchecks,_tmp8F9)))),_tmp8F8)))),((_tmp8FC->f5=((_tmp8F7="Disable null checks",
_tag_dyneither(_tmp8F7,sizeof(char),20))),_tmp8FC)))))))))))),((_tmp85A[43]=((
_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902->f1=((_tmp901="--nochecks",
_tag_dyneither(_tmp901,sizeof(char),11))),((_tmp902->f2=0,((_tmp902->f3=((
_tmp900="",_tag_dyneither(_tmp900,sizeof(char),1))),((_tmp902->f4=(void*)((
_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=0,((
_tmp8FF.f1=Cyc_set_nochecks,_tmp8FF)))),_tmp8FE)))),((_tmp902->f5=((_tmp8FD="Disable bounds/null checks",
_tag_dyneither(_tmp8FD,sizeof(char),27))),_tmp902)))))))))))),((_tmp85A[42]=((
_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908->f1=((_tmp907="--lineno",
_tag_dyneither(_tmp907,sizeof(char),9))),((_tmp908->f2=0,((_tmp908->f3=((_tmp906="",
_tag_dyneither(_tmp906,sizeof(char),1))),((_tmp908->f4=(void*)((_tmp904=
_region_malloc(r,sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=0,((_tmp905.f1=Cyc_set_lineno,
_tmp905)))),_tmp904)))),((_tmp908->f5=((_tmp903="Generate line numbers for debugging",
_tag_dyneither(_tmp903,sizeof(char),36))),_tmp908)))))))))))),((_tmp85A[41]=((
_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E->f1=((_tmp90D="-save-c",
_tag_dyneither(_tmp90D,sizeof(char),8))),((_tmp90E->f2=0,((_tmp90E->f3=((_tmp90C="",
_tag_dyneither(_tmp90C,sizeof(char),1))),((_tmp90E->f4=(void*)((_tmp90A=
_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=3,((_tmp90B.f1=&
Cyc_save_c_r,_tmp90B)))),_tmp90A)))),((_tmp90E->f5=((_tmp909="Don't delete temporary C files",
_tag_dyneither(_tmp909,sizeof(char),31))),_tmp90E)))))))))))),((_tmp85A[40]=((
_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914->f1=((_tmp913="-save-temps",
_tag_dyneither(_tmp913,sizeof(char),12))),((_tmp914->f2=0,((_tmp914->f3=((
_tmp912="",_tag_dyneither(_tmp912,sizeof(char),1))),((_tmp914->f4=(void*)((
_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=0,((
_tmp911.f1=Cyc_set_save_temps,_tmp911)))),_tmp910)))),((_tmp914->f5=((_tmp90F="Don't delete temporary files",
_tag_dyneither(_tmp90F,sizeof(char),29))),_tmp914)))))))))))),((_tmp85A[39]=((
_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A->f1=((_tmp919="-c-comp",
_tag_dyneither(_tmp919,sizeof(char),8))),((_tmp91A->f2=0,((_tmp91A->f3=((_tmp918=" <compiler>",
_tag_dyneither(_tmp918,sizeof(char),12))),((_tmp91A->f4=(void*)((_tmp916=
_region_malloc(r,sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=5,((_tmp917.f1=Cyc_set_c_compiler,
_tmp917)))),_tmp916)))),((_tmp91A->f5=((_tmp915="Produce C output for the given compiler",
_tag_dyneither(_tmp915,sizeof(char),40))),_tmp91A)))))))))))),((_tmp85A[38]=((
_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920->f1=((_tmp91F="-un-gcc",
_tag_dyneither(_tmp91F,sizeof(char),8))),((_tmp920->f2=0,((_tmp920->f3=((_tmp91E="",
_tag_dyneither(_tmp91E,sizeof(char),1))),((_tmp920->f4=(void*)((_tmp91C=
_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=0,((_tmp91D.f1=Cyc_set_tovc,
_tmp91D)))),_tmp91C)))),((_tmp920->f5=((_tmp91B="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp91B,sizeof(char),57))),_tmp920)))))))))))),((_tmp85A[37]=((
_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926->f1=((_tmp925="-no-elim-statement-expressions",
_tag_dyneither(_tmp925,sizeof(char),31))),((_tmp926->f2=0,((_tmp926->f3=((
_tmp924="",_tag_dyneither(_tmp924,sizeof(char),1))),((_tmp926->f4=(void*)((
_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922[0]=((_tmp923.tag=4,((
_tmp923.f1=& Cyc_elim_se_r,_tmp923)))),_tmp922)))),((_tmp926->f5=((_tmp921="Do not avoid statement expressions in C output",
_tag_dyneither(_tmp921,sizeof(char),47))),_tmp926)))))))))))),((_tmp85A[36]=((
_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C->f1=((_tmp92B="-elim-statement-expressions",
_tag_dyneither(_tmp92B,sizeof(char),28))),((_tmp92C->f2=0,((_tmp92C->f3=((
_tmp92A="",_tag_dyneither(_tmp92A,sizeof(char),1))),((_tmp92C->f4=(void*)((
_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=((_tmp929.tag=3,((
_tmp929.f1=& Cyc_elim_se_r,_tmp929)))),_tmp928)))),((_tmp92C->f5=((_tmp927="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp927,sizeof(char),66))),_tmp92C)))))))))))),((_tmp85A[35]=((
_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932->f1=((_tmp931="-up",
_tag_dyneither(_tmp931,sizeof(char),4))),((_tmp932->f2=0,((_tmp932->f3=((_tmp930="",
_tag_dyneither(_tmp930,sizeof(char),1))),((_tmp932->f4=(void*)((_tmp92E=
_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=4,((_tmp92F.f1=&
Cyc_pp_r,_tmp92F)))),_tmp92E)))),((_tmp932->f5=((_tmp92D="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp92D,sizeof(char),55))),_tmp932)))))))))))),((_tmp85A[34]=((
_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938->f1=((_tmp937="-pp",
_tag_dyneither(_tmp937,sizeof(char),4))),((_tmp938->f2=0,((_tmp938->f3=((_tmp936="",
_tag_dyneither(_tmp936,sizeof(char),1))),((_tmp938->f4=(void*)((_tmp934=
_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=3,((_tmp935.f1=&
Cyc_pp_r,_tmp935)))),_tmp934)))),((_tmp938->f5=((_tmp933="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp933,sizeof(char),47))),_tmp938)))))))))))),((_tmp85A[33]=((
_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E->f1=((_tmp93D="-ic",
_tag_dyneither(_tmp93D,sizeof(char),4))),((_tmp93E->f2=0,((_tmp93E->f3=((_tmp93C="",
_tag_dyneither(_tmp93C,sizeof(char),1))),((_tmp93E->f4=(void*)((_tmp93A=
_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp93B.tag=3,((_tmp93B.f1=&
Cyc_ic_r,_tmp93B)))),_tmp93A)))),((_tmp93E->f5=((_tmp939="Activate the link-checker",
_tag_dyneither(_tmp939,sizeof(char),26))),_tmp93E)))))))))))),((_tmp85A[32]=((
_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944->f1=((_tmp943="-stopafter-toc",
_tag_dyneither(_tmp943,sizeof(char),15))),((_tmp944->f2=0,((_tmp944->f3=((
_tmp942="",_tag_dyneither(_tmp942,sizeof(char),1))),((_tmp944->f4=(void*)((
_tmp940=_region_malloc(r,sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=3,((
_tmp941.f1=& Cyc_toc_r,_tmp941)))),_tmp940)))),((_tmp944->f5=((_tmp93F="Stop after translation to C",
_tag_dyneither(_tmp93F,sizeof(char),28))),_tmp944)))))))))))),((_tmp85A[31]=((
_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A->f1=((_tmp949="-stopafter-cf",
_tag_dyneither(_tmp949,sizeof(char),14))),((_tmp94A->f2=0,((_tmp94A->f3=((
_tmp948="",_tag_dyneither(_tmp948,sizeof(char),1))),((_tmp94A->f4=(void*)((
_tmp946=_region_malloc(r,sizeof(*_tmp946)),((_tmp946[0]=((_tmp947.tag=3,((
_tmp947.f1=& Cyc_cf_r,_tmp947)))),_tmp946)))),((_tmp94A->f5=((_tmp945="Stop after control-flow checking",
_tag_dyneither(_tmp945,sizeof(char),33))),_tmp94A)))))))))))),((_tmp85A[30]=((
_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950->f1=((_tmp94F="-noprint",
_tag_dyneither(_tmp94F,sizeof(char),9))),((_tmp950->f2=0,((_tmp950->f3=((_tmp94E="",
_tag_dyneither(_tmp94E,sizeof(char),1))),((_tmp950->f4=(void*)((_tmp94C=
_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94D.tag=3,((_tmp94D.f1=&
Cyc_noprint_r,_tmp94D)))),_tmp94C)))),((_tmp950->f5=((_tmp94B="Do not print output (when stopping early)",
_tag_dyneither(_tmp94B,sizeof(char),42))),_tmp950)))))))))))),((_tmp85A[29]=((
_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956->f1=((_tmp955="-stopafter-tc",
_tag_dyneither(_tmp955,sizeof(char),14))),((_tmp956->f2=0,((_tmp956->f3=((
_tmp954="",_tag_dyneither(_tmp954,sizeof(char),1))),((_tmp956->f4=(void*)((
_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=3,((
_tmp953.f1=& Cyc_tc_r,_tmp953)))),_tmp952)))),((_tmp956->f5=((_tmp951="Stop after type checking",
_tag_dyneither(_tmp951,sizeof(char),25))),_tmp956)))))))))))),((_tmp85A[28]=((
_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C->f1=((_tmp95B="-stopafter-parse",
_tag_dyneither(_tmp95B,sizeof(char),17))),((_tmp95C->f2=0,((_tmp95C->f3=((
_tmp95A="",_tag_dyneither(_tmp95A,sizeof(char),1))),((_tmp95C->f4=(void*)((
_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=((_tmp959.tag=3,((
_tmp959.f1=& Cyc_parseonly_r,_tmp959)))),_tmp958)))),((_tmp95C->f5=((_tmp957="Stop after parsing",
_tag_dyneither(_tmp957,sizeof(char),19))),_tmp95C)))))))))))),((_tmp85A[27]=((
_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962->f1=((_tmp961="-E",
_tag_dyneither(_tmp961,sizeof(char),3))),((_tmp962->f2=0,((_tmp962->f3=((_tmp960="",
_tag_dyneither(_tmp960,sizeof(char),1))),((_tmp962->f4=(void*)((_tmp95E=
_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=3,((_tmp95F.f1=&
Cyc_stop_after_cpp_r,_tmp95F)))),_tmp95E)))),((_tmp962->f5=((_tmp95D="Stop after preprocessing",
_tag_dyneither(_tmp95D,sizeof(char),25))),_tmp962)))))))))))),((_tmp85A[26]=((
_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968->f1=((_tmp967="-Wall",
_tag_dyneither(_tmp967,sizeof(char),6))),((_tmp968->f2=0,((_tmp968->f3=((_tmp966="",
_tag_dyneither(_tmp966,sizeof(char),1))),((_tmp968->f4=(void*)((_tmp964=
_region_malloc(r,sizeof(*_tmp964)),((_tmp964[0]=((_tmp965.tag=0,((_tmp965.f1=Cyc_set_all_warnings,
_tmp965)))),_tmp964)))),((_tmp968->f5=((_tmp963="Turn on all warnings",
_tag_dyneither(_tmp963,sizeof(char),21))),_tmp968)))))))))))),((_tmp85A[25]=((
_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E->f1=((_tmp96D="-b",
_tag_dyneither(_tmp96D,sizeof(char),3))),((_tmp96E->f2=0,((_tmp96E->f3=((_tmp96C="<arch>",
_tag_dyneither(_tmp96C,sizeof(char),7))),((_tmp96E->f4=(void*)((_tmp96A=
_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96B.tag=5,((_tmp96B.f1=Cyc_set_target_arch,
_tmp96B)))),_tmp96A)))),((_tmp96E->f5=((_tmp969="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp969,sizeof(char),48))),_tmp96E)))))))))))),((_tmp85A[24]=((
_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974->f1=((_tmp973="-MT",
_tag_dyneither(_tmp973,sizeof(char),4))),((_tmp974->f2=0,((_tmp974->f3=((_tmp972=" <target>",
_tag_dyneither(_tmp972,sizeof(char),10))),((_tmp974->f4=(void*)((_tmp970=
_region_malloc(r,sizeof(*_tmp970)),((_tmp970[0]=((_tmp971.tag=5,((_tmp971.f1=Cyc_set_dependencies_target,
_tmp971)))),_tmp970)))),((_tmp974->f5=((_tmp96F="Give target for dependencies",
_tag_dyneither(_tmp96F,sizeof(char),29))),_tmp974)))))))))))),((_tmp85A[23]=((
_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A->f1=((_tmp979="-MG",
_tag_dyneither(_tmp979,sizeof(char),4))),((_tmp97A->f2=0,((_tmp97A->f3=((_tmp978="",
_tag_dyneither(_tmp978,sizeof(char),1))),((_tmp97A->f4=(void*)((_tmp976=
_region_malloc(r,sizeof(*_tmp976)),((_tmp976[0]=((_tmp977.tag=1,((_tmp977.f1=Cyc_add_cpparg,
_tmp977)))),_tmp976)))),((_tmp97A->f5=((_tmp975="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp975,sizeof(char),68))),_tmp97A)))))))))))),((_tmp85A[22]=((
_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980->f1=((_tmp97F="-M",
_tag_dyneither(_tmp97F,sizeof(char),3))),((_tmp980->f2=0,((_tmp980->f3=((_tmp97E="",
_tag_dyneither(_tmp97E,sizeof(char),1))),((_tmp980->f4=(void*)((_tmp97C=
_region_malloc(r,sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=0,((_tmp97D.f1=Cyc_set_produce_dependencies,
_tmp97D)))),_tmp97C)))),((_tmp980->f5=((_tmp97B="Produce dependencies",
_tag_dyneither(_tmp97B,sizeof(char),21))),_tmp980)))))))))))),((_tmp85A[21]=((
_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986->f1=((_tmp985="-S",
_tag_dyneither(_tmp985,sizeof(char),3))),((_tmp986->f2=0,((_tmp986->f3=((_tmp984="",
_tag_dyneither(_tmp984,sizeof(char),1))),((_tmp986->f4=(void*)((_tmp982=
_region_malloc(r,sizeof(*_tmp982)),((_tmp982[0]=((_tmp983.tag=0,((_tmp983.f1=Cyc_set_stop_after_asmfile,
_tmp983)))),_tmp982)))),((_tmp986->f5=((_tmp981="Stop after producing assembly code",
_tag_dyneither(_tmp981,sizeof(char),35))),_tmp986)))))))))))),((_tmp85A[20]=((
_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C->f1=((_tmp98B="-pa",
_tag_dyneither(_tmp98B,sizeof(char),4))),((_tmp98C->f2=0,((_tmp98C->f3=((_tmp98A="",
_tag_dyneither(_tmp98A,sizeof(char),1))),((_tmp98C->f4=(void*)((_tmp988=
_region_malloc(r,sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=0,((_tmp989.f1=Cyc_set_pa,
_tmp989)))),_tmp988)))),((_tmp98C->f5=((_tmp987="Compile for profiling with aprof",
_tag_dyneither(_tmp987,sizeof(char),33))),_tmp98C)))))))))))),((_tmp85A[19]=((
_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992->f1=((_tmp991="-pg",
_tag_dyneither(_tmp991,sizeof(char),4))),((_tmp992->f2=0,((_tmp992->f3=((_tmp990="",
_tag_dyneither(_tmp990,sizeof(char),1))),((_tmp992->f4=(void*)((_tmp98E=
_region_malloc(r,sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=0,((_tmp98F.f1=Cyc_set_pg,
_tmp98F)))),_tmp98E)))),((_tmp992->f5=((_tmp98D="Compile for profiling with gprof",
_tag_dyneither(_tmp98D,sizeof(char),33))),_tmp992)))))))))))),((_tmp85A[18]=((
_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998->f1=((_tmp997="-p",
_tag_dyneither(_tmp997,sizeof(char),3))),((_tmp998->f2=0,((_tmp998->f3=((_tmp996="",
_tag_dyneither(_tmp996,sizeof(char),1))),((_tmp998->f4=(void*)((_tmp994=
_region_malloc(r,sizeof(*_tmp994)),((_tmp994[0]=((_tmp995.tag=1,((_tmp995.f1=Cyc_add_ccarg,
_tmp995)))),_tmp994)))),((_tmp998->f5=((_tmp993="Compile for profiling with prof",
_tag_dyneither(_tmp993,sizeof(char),32))),_tmp998)))))))))))),((_tmp85A[17]=((
_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E->f1=((_tmp99D="-g",
_tag_dyneither(_tmp99D,sizeof(char),3))),((_tmp99E->f2=0,((_tmp99E->f3=((_tmp99C="",
_tag_dyneither(_tmp99C,sizeof(char),1))),((_tmp99E->f4=(void*)((_tmp99A=
_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=1,((_tmp99B.f1=Cyc_add_ccarg,
_tmp99B)))),_tmp99A)))),((_tmp99E->f5=((_tmp999="Compile for debugging",
_tag_dyneither(_tmp999,sizeof(char),22))),_tmp99E)))))))))))),((_tmp85A[16]=((
_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4->f1=((_tmp9A3="-fomit-frame-pointer",
_tag_dyneither(_tmp9A3,sizeof(char),21))),((_tmp9A4->f2=0,((_tmp9A4->f3=((
_tmp9A2="",_tag_dyneither(_tmp9A2,sizeof(char),1))),((_tmp9A4->f4=(void*)((
_tmp9A0=_region_malloc(r,sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=1,((
_tmp9A1.f1=Cyc_add_ccarg,_tmp9A1)))),_tmp9A0)))),((_tmp9A4->f5=((_tmp99F="Omit frame pointer",
_tag_dyneither(_tmp99F,sizeof(char),19))),_tmp9A4)))))))))))),((_tmp85A[15]=((
_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA->f1=((_tmp9A9="-O3",
_tag_dyneither(_tmp9A9,sizeof(char),4))),((_tmp9AA->f2=0,((_tmp9AA->f3=((_tmp9A8="",
_tag_dyneither(_tmp9A8,sizeof(char),1))),((_tmp9AA->f4=(void*)((_tmp9A6=
_region_malloc(r,sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A7.tag=1,((_tmp9A7.f1=Cyc_add_ccarg,
_tmp9A7)))),_tmp9A6)))),((_tmp9AA->f5=((_tmp9A5="Even more optimization",
_tag_dyneither(_tmp9A5,sizeof(char),23))),_tmp9AA)))))))))))),((_tmp85A[14]=((
_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0->f1=((_tmp9AF="-O2",
_tag_dyneither(_tmp9AF,sizeof(char),4))),((_tmp9B0->f2=0,((_tmp9B0->f3=((_tmp9AE="",
_tag_dyneither(_tmp9AE,sizeof(char),1))),((_tmp9B0->f4=(void*)((_tmp9AC=
_region_malloc(r,sizeof(*_tmp9AC)),((_tmp9AC[0]=((_tmp9AD.tag=1,((_tmp9AD.f1=Cyc_add_ccarg,
_tmp9AD)))),_tmp9AC)))),((_tmp9B0->f5=((_tmp9AB="A higher level of optimization",
_tag_dyneither(_tmp9AB,sizeof(char),31))),_tmp9B0)))))))))))),((_tmp85A[13]=((
_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6->f1=((_tmp9B5="-O",
_tag_dyneither(_tmp9B5,sizeof(char),3))),((_tmp9B6->f2=0,((_tmp9B6->f3=((_tmp9B4="",
_tag_dyneither(_tmp9B4,sizeof(char),1))),((_tmp9B6->f4=(void*)((_tmp9B2=
_region_malloc(r,sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B3.tag=1,((_tmp9B3.f1=Cyc_add_ccarg,
_tmp9B3)))),_tmp9B2)))),((_tmp9B6->f5=((_tmp9B1="Optimize",_tag_dyneither(
_tmp9B1,sizeof(char),9))),_tmp9B6)))))))))))),((_tmp85A[12]=((_tmp9BC=
_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC->f1=((_tmp9BB="-O0",_tag_dyneither(
_tmp9BB,sizeof(char),4))),((_tmp9BC->f2=0,((_tmp9BC->f3=((_tmp9BA="",
_tag_dyneither(_tmp9BA,sizeof(char),1))),((_tmp9BC->f4=(void*)((_tmp9B8=
_region_malloc(r,sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=1,((_tmp9B9.f1=Cyc_add_ccarg,
_tmp9B9)))),_tmp9B8)))),((_tmp9BC->f5=((_tmp9B7="Don't optimize",_tag_dyneither(
_tmp9B7,sizeof(char),15))),_tmp9BC)))))))))))),((_tmp85A[11]=((_tmp9C2=
_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2->f1=((_tmp9C1="-s",_tag_dyneither(
_tmp9C1,sizeof(char),3))),((_tmp9C2->f2=0,((_tmp9C2->f3=((_tmp9C0="",
_tag_dyneither(_tmp9C0,sizeof(char),1))),((_tmp9C2->f4=(void*)((_tmp9BE=
_region_malloc(r,sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=1,((_tmp9BF.f1=Cyc_add_ccarg,
_tmp9BF)))),_tmp9BE)))),((_tmp9C2->f5=((_tmp9BD="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp9BD,sizeof(char),60))),_tmp9C2)))))))))))),((_tmp85A[10]=((
_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8->f1=((_tmp9C7="-x",
_tag_dyneither(_tmp9C7,sizeof(char),3))),((_tmp9C8->f2=0,((_tmp9C8->f3=((_tmp9C6=" <language>",
_tag_dyneither(_tmp9C6,sizeof(char),12))),((_tmp9C8->f4=(void*)((_tmp9C4=
_region_malloc(r,sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=5,((_tmp9C5.f1=Cyc_set_inputtype,
_tmp9C5)))),_tmp9C4)))),((_tmp9C8->f5=((_tmp9C3="Specify <language> for the following input files",
_tag_dyneither(_tmp9C3,sizeof(char),49))),_tmp9C8)))))))))))),((_tmp85A[9]=((
_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE->f1=((_tmp9CD="-c",
_tag_dyneither(_tmp9CD,sizeof(char),3))),((_tmp9CE->f2=0,((_tmp9CE->f3=((_tmp9CC="",
_tag_dyneither(_tmp9CC,sizeof(char),1))),((_tmp9CE->f4=(void*)((_tmp9CA=
_region_malloc(r,sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=0,((_tmp9CB.f1=Cyc_set_stop_after_objectfile,
_tmp9CB)))),_tmp9CA)))),((_tmp9CE->f5=((_tmp9C9="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp9C9,sizeof(char),61))),_tmp9CE)))))))))))),((_tmp85A[8]=((
_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4->f1=((_tmp9D3="-m",
_tag_dyneither(_tmp9D3,sizeof(char),3))),((_tmp9D4->f2=1,((_tmp9D4->f3=((_tmp9D2="<option>",
_tag_dyneither(_tmp9D2,sizeof(char),9))),((_tmp9D4->f4=(void*)((_tmp9D0=
_region_malloc(r,sizeof(*_tmp9D0)),((_tmp9D0[0]=((_tmp9D1.tag=1,((_tmp9D1.f1=Cyc_add_marg,
_tmp9D1)))),_tmp9D0)))),((_tmp9D4->f5=((_tmp9CF="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmp9CF,sizeof(char),52))),_tmp9D4)))))))))))),((_tmp85A[7]=((
_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA->f1=((_tmp9D9="-l",
_tag_dyneither(_tmp9D9,sizeof(char),3))),((_tmp9DA->f2=1,((_tmp9DA->f3=((_tmp9D8="<libname>",
_tag_dyneither(_tmp9D8,sizeof(char),10))),((_tmp9DA->f4=(void*)((_tmp9D6=
_region_malloc(r,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=1,((_tmp9D7.f1=Cyc_add_libarg,
_tmp9D7)))),_tmp9D6)))),((_tmp9DA->f5=((_tmp9D5="Library file",_tag_dyneither(
_tmp9D5,sizeof(char),13))),_tmp9DA)))))))))))),((_tmp85A[6]=((_tmp9E0=
_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0->f1=((_tmp9DF="-L",_tag_dyneither(
_tmp9DF,sizeof(char),3))),((_tmp9E0->f2=1,((_tmp9E0->f3=((_tmp9DE="<dir>",
_tag_dyneither(_tmp9DE,sizeof(char),6))),((_tmp9E0->f4=(void*)((_tmp9DC=
_region_malloc(r,sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=1,((_tmp9DD.f1=Cyc_add_ccarg,
_tmp9DD)))),_tmp9DC)))),((_tmp9E0->f5=((_tmp9DB="Add to the list of directories for -l",
_tag_dyneither(_tmp9DB,sizeof(char),38))),_tmp9E0)))))))))))),((_tmp85A[5]=((
_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6->f1=((_tmp9E5="-I",
_tag_dyneither(_tmp9E5,sizeof(char),3))),((_tmp9E6->f2=1,((_tmp9E6->f3=((_tmp9E4="<dir>",
_tag_dyneither(_tmp9E4,sizeof(char),6))),((_tmp9E6->f4=(void*)((_tmp9E2=
_region_malloc(r,sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=1,((_tmp9E3.f1=Cyc_add_cpparg,
_tmp9E3)))),_tmp9E2)))),((_tmp9E6->f5=((_tmp9E1="Add to the list of directories to search for include files",
_tag_dyneither(_tmp9E1,sizeof(char),59))),_tmp9E6)))))))))))),((_tmp85A[4]=((
_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC->f1=((_tmp9EB="-B",
_tag_dyneither(_tmp9EB,sizeof(char),3))),((_tmp9EC->f2=1,((_tmp9EC->f3=((_tmp9EA="<file>",
_tag_dyneither(_tmp9EA,sizeof(char),7))),((_tmp9EC->f4=(void*)((_tmp9E8=
_region_malloc(r,sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=1,((_tmp9E9.f1=Cyc_add_cyclone_exec_path,
_tmp9E9)))),_tmp9E8)))),((_tmp9EC->f5=((_tmp9E7="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp9E7,sizeof(char),60))),_tmp9EC)))))))))))),((_tmp85A[3]=((
_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2->f1=((_tmp9F1="-D",
_tag_dyneither(_tmp9F1,sizeof(char),3))),((_tmp9F2->f2=1,((_tmp9F2->f3=((_tmp9F0="<name>[=<value>]",
_tag_dyneither(_tmp9F0,sizeof(char),17))),((_tmp9F2->f4=(void*)((_tmp9EE=
_region_malloc(r,sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=1,((_tmp9EF.f1=Cyc_add_cpparg,
_tmp9EF)))),_tmp9EE)))),((_tmp9F2->f5=((_tmp9ED="Pass definition to preprocessor",
_tag_dyneither(_tmp9ED,sizeof(char),32))),_tmp9F2)))))))))))),((_tmp85A[2]=((
_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8->f1=((_tmp9F7="-o",
_tag_dyneither(_tmp9F7,sizeof(char),3))),((_tmp9F8->f2=0,((_tmp9F8->f3=((_tmp9F6=" <file>",
_tag_dyneither(_tmp9F6,sizeof(char),8))),((_tmp9F8->f4=(void*)((_tmp9F4=
_region_malloc(r,sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=5,((_tmp9F5.f1=Cyc_set_output_file,
_tmp9F5)))),_tmp9F4)))),((_tmp9F8->f5=((_tmp9F3="Set the output file name to <file>",
_tag_dyneither(_tmp9F3,sizeof(char),35))),_tmp9F8)))))))))))),((_tmp85A[1]=((
_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE->f1=((_tmp9FD="--version",
_tag_dyneither(_tmp9FD,sizeof(char),10))),((_tmp9FE->f2=0,((_tmp9FE->f3=((
_tmp9FC="",_tag_dyneither(_tmp9FC,sizeof(char),1))),((_tmp9FE->f4=(void*)((
_tmp9FA=_region_malloc(r,sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=0,((
_tmp9FB.f1=Cyc_print_version,_tmp9FB)))),_tmp9FA)))),((_tmp9FE->f5=((_tmp9F9="Print version information and exit",
_tag_dyneither(_tmp9F9,sizeof(char),35))),_tmp9FE)))))))))))),((_tmp85A[0]=((
_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04->f1=((_tmpA03="-v",
_tag_dyneither(_tmpA03,sizeof(char),3))),((_tmpA04->f2=0,((_tmpA04->f3=((_tmpA02="",
_tag_dyneither(_tmpA02,sizeof(char),1))),((_tmpA04->f4=(void*)((_tmpA00=
_region_malloc(r,sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=3,((_tmpA01.f1=&
Cyc_v_r,_tmpA01)))),_tmpA00)))),((_tmpA04->f5=((_tmp9FF="Print compilation stages verbosely",
_tag_dyneither(_tmp9FF,sizeof(char),35))),_tmpA04)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp85A,sizeof(struct _tuple21*),71)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmpA05;struct _dyneither_ptr optstring=(_tmpA05="Options:",
_tag_dyneither(_tmpA05,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp164);if((char*)Cyc_target_arch.curr == (char*)(
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{char*_tmpA06;struct _dyneither_ptr cyclone_exec_prefix=(
_tmpA06=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA06,sizeof(
char),_get_zero_arr_size_char((void*)_tmpA06,1)));if((char*)cyclone_exec_prefix.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*_tmpA09;struct Cyc_List_List*
_tmpA08;Cyc_cyclone_exec_path=((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08->hd=((
_tmpA09=_cycalloc(sizeof(*_tmpA09)),((_tmpA09[0]=cyclone_exec_prefix,_tmpA09)))),((
_tmpA08->tl=Cyc_cyclone_exec_path,_tmpA08))))));}def_lib_path=(struct
_dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct
_dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmpA0F;const char*_tmpA0E;
struct Cyc_List_List*_tmpA0D;Cyc_cyclone_exec_path=((_tmpA0D=_cycalloc(sizeof(*
_tmpA0D)),((_tmpA0D->hd=((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=(
struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA0E="cyc-lib",
_tag_dyneither(_tmpA0E,sizeof(char),8)))),_tmpA0F)))),((_tmpA0D->tl=Cyc_cyclone_exec_path,
_tmpA0D))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmpA10;struct _dyneither_ptr
_tmp16B=Cyc_do_find(cyclone_arch_path,((_tmpA10="cycspecs",_tag_dyneither(
_tmpA10,sizeof(char),9))));if(Cyc_v_r){const char*_tmpA14;void*_tmpA13[1];struct
Cyc_String_pa_struct _tmpA12;(_tmpA12.tag=0,((_tmpA12.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp16B),((_tmpA13[0]=& _tmpA12,Cyc_fprintf(Cyc_stderr,((
_tmpA14="Reading from specs file %s\n",_tag_dyneither(_tmpA14,sizeof(char),28))),
_tag_dyneither(_tmpA13,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp16F=Cyc_read_specs(
_tmp16B);struct _dyneither_ptr _tmp170=_tag_dyneither(0,0,0);{struct _handler_cons
_tmp171;_push_handler(& _tmp171);{int _tmp173=0;if(setjmp(_tmp171.handler))_tmp173=
1;if(!_tmp173){{struct _dyneither_ptr _tmp174=*((struct _dyneither_ptr*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp16F,_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));_tmp170=Cyc_split_specs(
_tmp174);};_pop_handler();}else{void*_tmp172=(void*)_exn_thrown;void*_tmp177=
_tmp172;_LL36: {struct Cyc_Core_Not_found_struct*_tmp178=(struct Cyc_Core_Not_found_struct*)
_tmp177;if(_tmp178->tag != Cyc_Core_Not_found)goto _LL38;}_LL37: goto _LL35;_LL38:;
_LL39:(void)_throw(_tmp177);_LL35:;}};}if((struct _dyneither_ptr*)_tmp170.curr != (
struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp170);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp165);{const char*_tmpA15;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_cpp,((_tmpA15="",_tag_dyneither(_tmpA15,sizeof(char),1))))
== 0){const char*_tmp17A=Cyc_produce_dependencies?"":(const char*)" -E";const char*
_tmpA1D;void*_tmpA1C[3];struct Cyc_String_pa_struct _tmpA1B;const char*_tmpA1A;
struct Cyc_String_pa_struct _tmpA19;struct Cyc_String_pa_struct _tmpA18;Cyc_set_cpp((
struct _dyneither_ptr)((_tmpA18.tag=0,((_tmpA18.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp16B),((_tmpA19.tag=0,((_tmpA19.f1=(struct _dyneither_ptr)((
_tmpA1A=_tmp17A,_tag_dyneither(_tmpA1A,sizeof(char),_get_zero_arr_size_char((
void*)_tmpA1A,1)))),((_tmpA1B.tag=0,((_tmpA1B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmpA1C[0]=& _tmpA1B,((_tmpA1C[1]=& _tmpA19,((_tmpA1C[2]=&
_tmpA18,Cyc_aprintf(((_tmpA1D="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA1D,
sizeof(char),23))),_tag_dyneither(_tmpA1C,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmpA20;void*_tmpA1F;(
_tmpA1F=0,Cyc_fprintf(Cyc_stderr,((_tmpA20="missing file\n",_tag_dyneither(
_tmpA20,sizeof(char),14))),_tag_dyneither(_tmpA1F,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmpA21;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmpA21="cyc_setjmp.h",_tag_dyneither(_tmpA21,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmpA22;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmpA22="cyc_include.h",_tag_dyneither(_tmpA22,sizeof(
char),14))));}{struct Cyc_List_List*_tmp33F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp33F != 0;_tmp33F=_tmp33F->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp33F->hd));if(Cyc_compile_failure){int _tmp340=1;
_npop_handler(0);return _tmp340;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp341=0;_npop_handler(0);return _tmp341;}if(Cyc_ccargs
== 0){int _tmp342=0;_npop_handler(0);return _tmp342;}{const char*_tmpA23;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmpA23="-L",_tag_dyneither(_tmpA23,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmpA24;struct
_dyneither_ptr _tmp344=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA24=" ",_tag_dyneither(_tmpA24,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmpA26;struct Cyc_List_List*_tmpA25;struct _dyneither_ptr
_tmp345=Cyc_str_sepstr(((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmpA25->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmpA25)))))),((_tmpA26=" ",_tag_dyneither(_tmpA26,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmpA28;
const char*_tmpA27;int _tmp346=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmpA28=".a",_tag_dyneither(_tmpA28,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA27=".lib",_tag_dyneither(
_tmpA27,sizeof(char),5))));if(_tmp346){stdlib=0;{const char*_tmpA29;stdlib_string=(
struct _dyneither_ptr)((_tmpA29="",_tag_dyneither(_tmpA29,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmpA2A;libcyc_flag=((_tmpA2A="-lcyc_a",_tag_dyneither(_tmpA2A,sizeof(
char),8)));}{const char*_tmpA2B;nogc_filename=((_tmpA2B="nogc_a.a",_tag_dyneither(
_tmpA2B,sizeof(char),9)));}{const char*_tmpA2C;runtime_filename=((_tmpA2C="runtime_cyc_a.o",
_tag_dyneither(_tmpA2C,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmpA2D;libcyc_flag=((_tmpA2D="-lcyc_nocheck",_tag_dyneither(_tmpA2D,sizeof(char),
14)));}{const char*_tmpA2E;nogc_filename=((_tmpA2E="nogc.a",_tag_dyneither(
_tmpA2E,sizeof(char),7)));}{const char*_tmpA2F;runtime_filename=((_tmpA2F="runtime_cyc.o",
_tag_dyneither(_tmpA2F,sizeof(char),14)));};}else{if(Cyc_pg_r){{const char*
_tmpA30;libcyc_flag=((_tmpA30="-lcyc_pg",_tag_dyneither(_tmpA30,sizeof(char),9)));}{
const char*_tmpA31;runtime_filename=((_tmpA31="runtime_cyc_pg.o",_tag_dyneither(
_tmpA31,sizeof(char),17)));}{const char*_tmpA32;nogc_filename=((_tmpA32="nogc_pg.a",
_tag_dyneither(_tmpA32,sizeof(char),10)));};}else{if(Cyc_compile_for_boot_r){{
const char*_tmpA33;libcyc_flag=((_tmpA33="-lcycboot",_tag_dyneither(_tmpA33,
sizeof(char),10)));}{const char*_tmpA34;nogc_filename=((_tmpA34="nogc.a",
_tag_dyneither(_tmpA34,sizeof(char),7)));}{const char*_tmpA35;runtime_filename=((
_tmpA35="runtime_cyc.o",_tag_dyneither(_tmpA35,sizeof(char),14)));};}else{{const
char*_tmpA36;libcyc_flag=((_tmpA36="-lcyc",_tag_dyneither(_tmpA36,sizeof(char),6)));}{
const char*_tmpA37;nogc_filename=((_tmpA37="nogc.a",_tag_dyneither(_tmpA37,
sizeof(char),7)));}{const char*_tmpA38;runtime_filename=((_tmpA38="runtime_cyc.o",
_tag_dyneither(_tmpA38,sizeof(char),14)));};}}}}{const char*_tmpA39;gc_filename=((
_tmpA39="gc.a",_tag_dyneither(_tmpA39,sizeof(char),5)));}{struct _dyneither_ptr
_tmp358=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp359=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmpA3F;void*_tmpA3E[3];
struct Cyc_String_pa_struct _tmpA3D;struct Cyc_String_pa_struct _tmpA3C;struct Cyc_String_pa_struct
_tmpA3B;stdlib_string=(struct _dyneither_ptr)((_tmpA3B.tag=0,((_tmpA3B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp358),((_tmpA3C.tag=0,((_tmpA3C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA3D.tag=0,((
_tmpA3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp359),((_tmpA3E[0]=&
_tmpA3D,((_tmpA3E[1]=& _tmpA3C,((_tmpA3E[2]=& _tmpA3B,Cyc_aprintf(((_tmpA3F=" %s %s %s",
_tag_dyneither(_tmpA3F,sizeof(char),10))),_tag_dyneither(_tmpA3E,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp35F;_push_handler(& _tmp35F);{int _tmp361=0;
if(setjmp(_tmp35F.handler))_tmp361=1;if(!_tmp361){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp362=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp346){struct
Cyc_List_List*_tmpA40;_tmp362=((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->hd=
Cyc_final_strptr,((_tmpA40->tl=_tmp362,_tmpA40))))));}{struct Cyc_Interface_I*
_tmp364=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp362,_tmp362);if(_tmp364 == 0){{const char*_tmpA43;void*_tmpA42;(
_tmpA42=0,Cyc_fprintf(Cyc_stderr,((_tmpA43="Error: interfaces incompatible\n",
_tag_dyneither(_tmpA43,sizeof(char),32))),_tag_dyneither(_tmpA42,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp367=1;_npop_handler(1);return
_tmp367;};}if(_tmp346){if(Cyc_output_file != 0){const char*_tmpA47;void*_tmpA46[1];
struct Cyc_String_pa_struct _tmpA45;struct _dyneither_ptr _tmp368=(_tmpA45.tag=0,((
_tmpA45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA46[0]=& _tmpA45,Cyc_aprintf(((
_tmpA47="%s.cycio",_tag_dyneither(_tmpA47,sizeof(char),9))),_tag_dyneither(
_tmpA46,sizeof(void*),1)))))));const char*_tmpA49;const char*_tmpA48;struct Cyc___cycFILE*
_tmp369=Cyc_try_file_open((struct _dyneither_ptr)_tmp368,((_tmpA48="wb",
_tag_dyneither(_tmpA48,sizeof(char),3))),((_tmpA49="interface object file",
_tag_dyneither(_tmpA49,sizeof(char),22))));if(_tmp369 == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp36A=1;_npop_handler(1);return _tmp36A;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp364,(struct Cyc___cycFILE*)_tmp369);Cyc_file_close((
struct Cyc___cycFILE*)_tmp369);}}else{const char*_tmpA4E;const char*_tmpA4D;struct
_tuple17*_tmpA4C;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp364,((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->f1=((
_tmpA4D="empty interface",_tag_dyneither(_tmpA4D,sizeof(char),16))),((_tmpA4C->f2=((
_tmpA4E="global interface",_tag_dyneither(_tmpA4E,sizeof(char),17))),_tmpA4C)))))))){{
const char*_tmpA51;void*_tmpA50;(_tmpA50=0,Cyc_fprintf(Cyc_stderr,((_tmpA51="Error: some objects are still undefined\n",
_tag_dyneither(_tmpA51,sizeof(char),41))),_tag_dyneither(_tmpA50,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp375=1;_npop_handler(1);return
_tmp375;};}}};};;_pop_handler();}else{void*_tmp360=(void*)_exn_thrown;void*
_tmp377=_tmp360;_LL3B:;_LL3C:{void*_tmp378=_tmp377;struct _dyneither_ptr _tmp37A;
struct _dyneither_ptr _tmp37C;struct _dyneither_ptr _tmp37F;_LL40: {struct Cyc_Core_Failure_struct*
_tmp379=(struct Cyc_Core_Failure_struct*)_tmp378;if(_tmp379->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp37A=_tmp379->f1;}}_LL41:{const char*_tmpA55;void*_tmpA54[1];
struct Cyc_String_pa_struct _tmpA53;(_tmpA53.tag=0,((_tmpA53.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp37A),((_tmpA54[0]=& _tmpA53,Cyc_fprintf(
Cyc_stderr,((_tmpA55="Exception Core::Failure %s\n",_tag_dyneither(_tmpA55,
sizeof(char),28))),_tag_dyneither(_tmpA54,sizeof(void*),1)))))));}goto _LL3F;
_LL42: {struct Cyc_Core_Impossible_struct*_tmp37B=(struct Cyc_Core_Impossible_struct*)
_tmp378;if(_tmp37B->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp37C=_tmp37B->f1;}}
_LL43:{const char*_tmpA59;void*_tmpA58[1];struct Cyc_String_pa_struct _tmpA57;(
_tmpA57.tag=0,((_tmpA57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37C),((
_tmpA58[0]=& _tmpA57,Cyc_fprintf(Cyc_stderr,((_tmpA59="Exception Core::Impossible %s\n",
_tag_dyneither(_tmpA59,sizeof(char),31))),_tag_dyneither(_tmpA58,sizeof(void*),1)))))));}
goto _LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp37D=(struct Cyc_Dict_Absent_struct*)
_tmp378;if(_tmp37D->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:{const char*_tmpA5C;
void*_tmpA5B;(_tmpA5B=0,Cyc_fprintf(Cyc_stderr,((_tmpA5C="Exception Dict::Absent\n",
_tag_dyneither(_tmpA5C,sizeof(char),24))),_tag_dyneither(_tmpA5B,sizeof(void*),0)));}
goto _LL3F;_LL46: {struct Cyc_Core_Invalid_argument_struct*_tmp37E=(struct Cyc_Core_Invalid_argument_struct*)
_tmp378;if(_tmp37E->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp37F=
_tmp37E->f1;}}_LL47:{const char*_tmpA60;void*_tmpA5F[1];struct Cyc_String_pa_struct
_tmpA5E;(_tmpA5E.tag=0,((_tmpA5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp37F),((_tmpA5F[0]=& _tmpA5E,Cyc_fprintf(Cyc_stderr,((_tmpA60="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmpA60,sizeof(char),37))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))));}
goto _LL3F;_LL48:;_LL49:{const char*_tmpA63;void*_tmpA62;(_tmpA62=0,Cyc_fprintf(
Cyc_stderr,((_tmpA63="Uncaught exception\n",_tag_dyneither(_tmpA63,sizeof(char),
20))),_tag_dyneither(_tmpA62,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp38D=1;_npop_handler(0);return _tmp38D;};_LL3D:;
_LL3E:(void)_throw(_tmp377);_LL3A:;}};}{const char*_tmpA64;struct _dyneither_ptr
outfile_str=(_tmpA64="",_tag_dyneither(_tmpA64,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmpA68;void*_tmpA67[1];struct Cyc_String_pa_struct _tmpA66;
outfile_str=(struct _dyneither_ptr)((_tmpA66.tag=0,((_tmpA66.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA67[0]=& _tmpA66,Cyc_aprintf(((
_tmpA68=" -o %s",_tag_dyneither(_tmpA68,sizeof(char),7))),_tag_dyneither(_tmpA67,
sizeof(void*),1))))))));}{const char*_tmpA70;void*_tmpA6F[5];struct Cyc_String_pa_struct
_tmpA6E;struct Cyc_String_pa_struct _tmpA6D;struct Cyc_String_pa_struct _tmpA6C;
struct Cyc_String_pa_struct _tmpA6B;struct Cyc_String_pa_struct _tmpA6A;struct
_dyneither_ptr _tmp391=(_tmpA6A.tag=0,((_tmpA6A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp345),((_tmpA6B.tag=0,((_tmpA6B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmpA6C.tag=0,((_tmpA6C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp344),((_tmpA6D.tag=0,((_tmpA6D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpA6E.tag=0,((
_tmpA6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA6F[0]=&
_tmpA6E,((_tmpA6F[1]=& _tmpA6D,((_tmpA6F[2]=& _tmpA6C,((_tmpA6F[3]=& _tmpA6B,((
_tmpA6F[4]=& _tmpA6A,Cyc_aprintf(((_tmpA70="%s %s %s%s%s",_tag_dyneither(_tmpA70,
sizeof(char),13))),_tag_dyneither(_tmpA6F,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmpA74;void*_tmpA73[1];struct Cyc_String_pa_struct _tmpA72;(
_tmpA72.tag=0,((_tmpA72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp391),((
_tmpA73[0]=& _tmpA72,Cyc_fprintf(Cyc_stderr,((_tmpA74="%s\n",_tag_dyneither(
_tmpA74,sizeof(char),4))),_tag_dyneither(_tmpA73,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp391,sizeof(char),1))!= 0){{const char*_tmpA77;void*_tmpA76;(_tmpA76=0,Cyc_fprintf(
Cyc_stderr,((_tmpA77="Error: C compiler failed\n",_tag_dyneither(_tmpA77,sizeof(
char),26))),_tag_dyneither(_tmpA76,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp397=1;_npop_handler(0);return _tmp397;};}Cyc_remove_cfiles();{int _tmp398=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp398;};};};};};;_pop_region(r);};}
