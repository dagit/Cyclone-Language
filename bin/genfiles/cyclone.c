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
int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
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
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
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
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct
Cyc_Absyn_Exp*f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};
struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;
struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*
Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct _tuple17{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
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
void Cyc_add_ccarg(struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3AF;struct Cyc_List_List*
_tmp3AE;Cyc_ccargs=((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->hd=((_tmp3AF=
_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF[0]=s,_tmp3AF)))),((_tmp3AE->tl=Cyc_ccargs,
_tmp3AE))))));}void Cyc_set_c_compiler(struct _dyneither_ptr s);void Cyc_set_c_compiler(
struct _dyneither_ptr s){const char*_tmp3B0;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3B0="vc",_tag_dyneither(_tmp3B0,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp3B1;Cyc_add_ccarg(((_tmp3B1="-DVC_C",
_tag_dyneither(_tmp3B1,sizeof(char),7))));};}else{const char*_tmp3B2;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3B2="gcc",_tag_dyneither(_tmp3B2,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp3B3;Cyc_add_ccarg(((
_tmp3B3="-DGCC_C",_tag_dyneither(_tmp3B3,sizeof(char),8))));};}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dyneither_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dyneither_ptr s);static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3B4;Cyc_output_file=((_tmp3B4=
_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4[0]=s,_tmp3B4))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s);static void Cyc_set_cpp(struct
_dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dyneither_ptr s);static void Cyc_add_cpparg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp3B7;struct Cyc_List_List*_tmp3B6;Cyc_cppargs=((
_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->hd=((_tmp3B7=_cycalloc(sizeof(*
_tmp3B7)),((_tmp3B7[0]=s,_tmp3B7)))),((_tmp3B6->tl=Cyc_cppargs,_tmp3B6))))));}
static void Cyc_add_iprefix(struct _dyneither_ptr s);static void Cyc_add_iprefix(
struct _dyneither_ptr s){const char*_tmp3BB;void*_tmp3BA[1];struct Cyc_String_pa_struct
_tmp3B9;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3B9.tag=0,((_tmp3B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3BA[0]=& _tmp3B9,Cyc_aprintf(((
_tmp3BB="-iprefix %s",_tag_dyneither(_tmp3BB,sizeof(char),12))),_tag_dyneither(
_tmp3BA,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefix(struct
_dyneither_ptr s);static void Cyc_add_iwithprefix(struct _dyneither_ptr s){const char*
_tmp3BF;void*_tmp3BE[1];struct Cyc_String_pa_struct _tmp3BD;Cyc_add_cpparg((struct
_dyneither_ptr)((_tmp3BD.tag=0,((_tmp3BD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp3BE[0]=& _tmp3BD,Cyc_aprintf(((_tmp3BF="-iwithprefix %s",
_tag_dyneither(_tmp3BF,sizeof(char),16))),_tag_dyneither(_tmp3BE,sizeof(void*),1)))))))));}
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s);static void Cyc_add_iwithprefixbefore(
struct _dyneither_ptr s){const char*_tmp3C3;void*_tmp3C2[1];struct Cyc_String_pa_struct
_tmp3C1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C1.tag=0,((_tmp3C1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C2[0]=& _tmp3C1,Cyc_aprintf(((
_tmp3C3="-iwithprefixbefore %s",_tag_dyneither(_tmp3C3,sizeof(char),22))),
_tag_dyneither(_tmp3C2,sizeof(void*),1)))))))));}static void Cyc_add_isystem(
struct _dyneither_ptr s);static void Cyc_add_isystem(struct _dyneither_ptr s){const
char*_tmp3C7;void*_tmp3C6[1];struct Cyc_String_pa_struct _tmp3C5;Cyc_add_cpparg((
struct _dyneither_ptr)((_tmp3C5.tag=0,((_tmp3C5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp3C6[0]=& _tmp3C5,Cyc_aprintf(((_tmp3C7="-isystem %s",
_tag_dyneither(_tmp3C7,sizeof(char),12))),_tag_dyneither(_tmp3C6,sizeof(void*),1)))))))));}
static void Cyc_add_idirafter(struct _dyneither_ptr s);static void Cyc_add_idirafter(
struct _dyneither_ptr s){const char*_tmp3CB;void*_tmp3CA[1];struct Cyc_String_pa_struct
_tmp3C9;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C9.tag=0,((_tmp3C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3CA[0]=& _tmp3C9,Cyc_aprintf(((
_tmp3CB="-idirafter %s",_tag_dyneither(_tmp3CB,sizeof(char),14))),_tag_dyneither(
_tmp3CA,sizeof(void*),1)))))))));}static struct _dyneither_ptr Cyc_target_arch=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_target_arch(
struct _dyneither_ptr s);static void Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=
1;Cyc_target_arch=s;}static void Cyc_print_version();static void Cyc_print_version(){{
const char*_tmp3CF;void*_tmp3CE[1];struct Cyc_String_pa_struct _tmp3CD;(_tmp3CD.tag=
0,((_tmp3CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cversion)),((_tmp3CE[0]=& _tmp3CD,Cyc_printf(((_tmp3CF="The Cyclone compiler, version %s\n",
_tag_dyneither(_tmp3CF,sizeof(char),34))),_tag_dyneither(_tmp3CE,sizeof(void*),1)))))));}{
const char*_tmp3D4;void*_tmp3D3[2];struct Cyc_String_pa_struct _tmp3D2;struct Cyc_String_pa_struct
_tmp3D1;(_tmp3D1.tag=0,((_tmp3D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3D2.tag=0,((_tmp3D2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3D3[0]=& _tmp3D2,((_tmp3D3[1]=&
_tmp3D1,Cyc_printf(((_tmp3D4="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp3D4,sizeof(char),39))),_tag_dyneither(_tmp3D3,sizeof(void*),2)))))))))))));}{
const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_struct _tmp3D6;(_tmp3D6.tag=
0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp3D7[0]=& _tmp3D6,Cyc_printf(((_tmp3D8="Standard library directory: %s\n",
_tag_dyneither(_tmp3D8,sizeof(char),32))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))));}{
const char*_tmp3DC;void*_tmp3DB[1];struct Cyc_String_pa_struct _tmp3DA;(_tmp3DA.tag=
0,((_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp3DB[0]=& _tmp3DA,Cyc_printf(((_tmp3DC="Standard include directory: %s\n",
_tag_dyneither(_tmp3DC,sizeof(char),32))),_tag_dyneither(_tmp3DB,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s);static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp26 <= 4)return 0;else{const char*_tmp3DD;return Cyc_strcmp((struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp3DD=".cyc",
_tag_dyneither(_tmp3DD,sizeof(char),5))))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s);static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp28 - 2);struct _dyneither_ptr*_tmp3E0;struct Cyc_List_List*
_tmp3DF;Cyc_cyclone_exec_path=((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->hd=((
_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0[0]=_tmp29,_tmp3E0)))),((_tmp3DF->tl=
Cyc_cyclone_exec_path,_tmp3DF))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s);static void Cyc_add_libarg(struct
_dyneither_ptr s){const char*_tmp3E1;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3E1="-lxml",_tag_dyneither(_tmp3E1,sizeof(char),6))))== 0)Cyc_add_ccarg(s);
else{struct _dyneither_ptr*_tmp3E4;struct Cyc_List_List*_tmp3E3;Cyc_libargs=((
_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3->hd=((_tmp3E4=_cycalloc(sizeof(*
_tmp3E4)),((_tmp3E4[0]=s,_tmp3E4)))),((_tmp3E3->tl=Cyc_libargs,_tmp3E3))))));}}
static void Cyc_add_marg(struct _dyneither_ptr s);static void Cyc_add_marg(struct
_dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{const char*_tmp3E5;Cyc_add_ccarg(((_tmp3E5="-save-temps",
_tag_dyneither(_tmp3E5,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp3E6;Cyc_add_cpparg(((
_tmp3E6="-M",_tag_dyneither(_tmp3E6,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s);static void Cyc_set_dependencies_target(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp3E7;Cyc_dependencies_target=((_tmp3E7=
_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7[0]=s,_tmp3E7))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp3E8;Cyc_add_ccarg(((_tmp3E8="-c",_tag_dyneither(_tmp3E8,sizeof(char),3))));};}
static void Cyc_set_nocppprecomp();static void Cyc_set_nocppprecomp(){{const char*
_tmp3E9;Cyc_add_cpparg(((_tmp3E9="-no-cpp-precomp",_tag_dyneither(_tmp3E9,
sizeof(char),16))));}{const char*_tmp3EA;Cyc_add_ccarg(((_tmp3EA="-no-cpp-precomp",
_tag_dyneither(_tmp3EA,sizeof(char),16))));};}static void Cyc_set_lineno();static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp3EB;Cyc_add_ccarg(((_tmp3EB="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3EB,
sizeof(char),18))));}Cyc_set_elim_se();}static void Cyc_set_noboundschecks();
static void Cyc_set_noboundschecks(){const char*_tmp3EC;Cyc_add_ccarg(((_tmp3EC="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp3EC,sizeof(char),23))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp3ED;Cyc_add_ccarg(((_tmp3ED="-DNO_CYC_NULL_CHECKS",
_tag_dyneither(_tmp3ED,sizeof(char),21))));}static void Cyc_set_nochecks();static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;{
const char*_tmp3EE;Cyc_add_ccarg(((_tmp3EE="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp3EE,sizeof(char),16))));};}static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=
1;{const char*_tmp3EF;Cyc_add_ccarg(((_tmp3EF="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp3EF,sizeof(char),21))));};}static void Cyc_set_pg();static void
Cyc_set_pg(){Cyc_pg_r=1;{const char*_tmp3F0;Cyc_add_ccarg(((_tmp3F0="-pg",
_tag_dyneither(_tmp3F0,sizeof(char),4))));};}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp3F1;Cyc_add_ccarg(((_tmp3F1="-S",_tag_dyneither(_tmp3F1,sizeof(char),3))));};}
static void Cyc_set_all_warnings();static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;Cyc_NewControlFlow_warn_lose_unique=1;}enum Cyc_inputtype{
Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s);static void Cyc_set_inputtype(
struct _dyneither_ptr s){const char*_tmp3F2;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3F2="cyc",_tag_dyneither(_tmp3F2,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;
else{const char*_tmp3F3;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3F3="none",
_tag_dyneither(_tmp3F3,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp3F7;void*_tmp3F6[1];struct Cyc_String_pa_struct _tmp3F5;(_tmp3F5.tag=
0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F6[0]=&
_tmp3F5,Cyc_fprintf(Cyc_stderr,((_tmp3F7="Input type '%s' not supported\n",
_tag_dyneither(_tmp3F7,sizeof(char),31))),_tag_dyneither(_tmp3F6,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file);struct _dyneither_ptr Cyc_make_base_filename(struct
_dyneither_ptr s,struct _dyneither_ptr*output_file){struct _dyneither_ptr _tmp41=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp42=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp43=Cyc_strlen((
struct _dyneither_ptr)_tmp41)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp41,(
struct _dyneither_ptr)_tmp42): _tmp42;return(struct _dyneither_ptr)_tmp43;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s);static void Cyc_add_other(struct _dyneither_ptr s){if(Cyc_intype == 
Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{struct _dyneither_ptr*_tmp3FA;
struct Cyc_List_List*_tmp3F9;Cyc_cyclone_files=((_tmp3F9=_cycalloc(sizeof(*
_tmp3F9)),((_tmp3F9->hd=((_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA[0]=s,
_tmp3FA)))),((_tmp3F9->tl=Cyc_cyclone_files,_tmp3F9))))));}{struct _dyneither_ptr
_tmp46=Cyc_make_base_filename(s,Cyc_output_file);const char*_tmp3FB;struct
_dyneither_ptr _tmp47=Cyc_strconcat((struct _dyneither_ptr)_tmp46,((_tmp3FB=".c",
_tag_dyneither(_tmp3FB,sizeof(char),3))));Cyc_add_ccarg((struct _dyneither_ptr)
_tmp47);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(struct
_dyneither_ptr s);static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)
return;else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct
_tuple18{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*
Cyc_read_specs(struct _dyneither_ptr file);struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp49=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp3FF;void*_tmp3FE[1];struct Cyc_String_pa_struct _tmp3FD;(_tmp3FD.tag=
0,((_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3FE[0]=&
_tmp3FD,Cyc_fprintf(Cyc_stderr,((_tmp3FF="Error opening spec file %s\n",
_tag_dyneither(_tmp3FF,sizeof(char),28))),_tag_dyneither(_tmp3FE,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp404;void*_tmp403[2];struct Cyc_String_pa_struct _tmp402;struct
Cyc_Int_pa_struct _tmp401;(_tmp401.tag=1,((_tmp401.f1=(unsigned long)c,((_tmp402.tag=
0,((_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp403[0]=&
_tmp402,((_tmp403[1]=& _tmp401,Cyc_fprintf(Cyc_stderr,((_tmp404="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp404,sizeof(char),55))),_tag_dyneither(_tmp403,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp408;void*_tmp407[1];struct Cyc_String_pa_struct _tmp406;(
_tmp406.tag=0,((_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp407[0]=& _tmp406,Cyc_fprintf(Cyc_stderr,((_tmp408="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp408,sizeof(char),44))),_tag_dyneither(_tmp407,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp40C;void*_tmp40B[1];struct Cyc_String_pa_struct _tmp40A;(_tmp40A.tag=0,((
_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp40B[0]=&
_tmp40A,Cyc_fprintf(Cyc_stderr,((_tmp40C="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp40C,sizeof(char),50))),_tag_dyneither(_tmp40B,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple18*_tmp419;struct _dyneither_ptr*_tmp418;const char*_tmp417;struct
_dyneither_ptr*_tmp416;struct Cyc_List_List*_tmp415;_tmp49=((_tmp415=_cycalloc(
sizeof(*_tmp415)),((_tmp415->hd=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->f1=((
_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp418)))),((_tmp419->f2=((_tmp416=
_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp417="",_tag_dyneither(_tmp417,sizeof(char),1)))),_tmp416)))),_tmp419)))))),((
_tmp415->tl=_tmp49,_tmp415))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp41E;void*_tmp41D[2];struct Cyc_String_pa_struct _tmp41C;struct Cyc_String_pa_struct
_tmp41B;(_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp41D[0]=& _tmp41C,((_tmp41D[1]=& _tmp41B,Cyc_fprintf(Cyc_stderr,((
_tmp41E="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp41E,sizeof(char),50))),_tag_dyneither(_tmp41D,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple18*_tmp427;struct _dyneither_ptr*_tmp426;struct _dyneither_ptr*_tmp425;
struct Cyc_List_List*_tmp424;_tmp49=((_tmp424=_cycalloc(sizeof(*_tmp424)),((
_tmp424->hd=((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427->f1=((_tmp426=
_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp426)))),((_tmp427->f2=((_tmp425=
_cycalloc(sizeof(*_tmp425)),((_tmp425[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp425)))),_tmp427)))))),((_tmp424->tl=
_tmp49,_tmp424))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp49;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp431(struct _dyneither_ptr*_tmp6A,
unsigned int*_tmp430,unsigned int*_tmp42F,struct _dyneither_ptr**_tmp42D){for(*
_tmp430=0;*_tmp430 < *_tmp42F;(*_tmp430)++){(*_tmp42D)[*_tmp430]=*((struct
_dyneither_ptr**)(*_tmp6A).curr)[(int)*_tmp430];}}struct _dyneither_ptr Cyc_split_specs(
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
4096,j)]='\000';{struct _dyneither_ptr*_tmp42A;struct Cyc_List_List*_tmp429;_tmp65=((
_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->hd=((_tmp42A=_cycalloc(sizeof(*
_tmp42A)),((_tmp42A[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp42A)))),((_tmp429->tl=_tmp65,_tmp429))))));};}DONE:
_tmp65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp65);{
struct Cyc_List_List*_tmp42B;_tmp65=((_tmp42B=_cycalloc(sizeof(*_tmp42B)),((
_tmp42B->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp42B->tl=_tmp65,_tmp42B))))));}{struct _dyneither_ptr _tmp6A=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp65);
unsigned int _tmp430;unsigned int _tmp42F;struct _dyneither_ptr _tmp42E;struct
_dyneither_ptr*_tmp42D;unsigned int _tmp42C;struct _dyneither_ptr _tmp6B=(_tmp42C=
_get_dyneither_size(_tmp6A,sizeof(struct _dyneither_ptr*)),((_tmp42D=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp42C)),((
_tmp42E=_tag_dyneither(_tmp42D,sizeof(struct _dyneither_ptr),_tmp42C),((((_tmp42F=
_tmp42C,_tmp431(& _tmp6A,& _tmp430,& _tmp42F,& _tmp42D))),_tmp42E)))))));return
_tmp6B;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part);struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,
struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp70;
_push_handler(& _tmp70);{int _tmp72=0;if(setjmp(_tmp70.handler))_tmp72=1;if(!
_tmp72){{struct Cyc___cycFILE*_tmp73=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp73;};_pop_handler();}else{void*_tmp71=(void*)
_exn_thrown;void*_tmp75=_tmp71;_LL1:;_LL2: Cyc_compile_failure=1;{const char*
_tmp436;void*_tmp435[2];struct Cyc_String_pa_struct _tmp434;struct Cyc_String_pa_struct
_tmp433;(_tmp433.tag=0,((_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename),((_tmp434.tag=0,((_tmp434.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg_part),((_tmp435[0]=& _tmp434,((_tmp435[1]=& _tmp433,Cyc_fprintf(
Cyc_stderr,((_tmp436="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp436,
sizeof(char),29))),_tag_dyneither(_tmp435,sizeof(void*),2)))))))))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp75);_LL0:;}};}
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*
Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){const char*_tmp437;struct
_dyneither_ptr exn_string=(_tmp437="",_tag_dyneither(_tmp437,sizeof(char),1));
const char*_tmp438;struct _dyneither_ptr explain_string=(_tmp438="",_tag_dyneither(
_tmp438,sizeof(char),1));int other_exn=0;struct Cyc_Core_Impossible_struct _tmp43E;
const char*_tmp43D;struct Cyc_Core_Impossible_struct*_tmp43C;void*ex=(void*)((
_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp43E.tag=Cyc_Core_Impossible,((
_tmp43E.f1=((_tmp43D="",_tag_dyneither(_tmp43D,sizeof(char),1))),_tmp43E)))),
_tmp43C))));struct Cyc_List_List*_tmp7A=0;{struct _handler_cons _tmp7B;
_push_handler(& _tmp7B);{int _tmp7D=0;if(setjmp(_tmp7B.handler))_tmp7D=1;if(!
_tmp7D){_tmp7A=f(env,tds);;_pop_handler();}else{void*_tmp7C=(void*)_exn_thrown;
void*_tmp7F=_tmp7C;struct _dyneither_ptr _tmp81;struct _dyneither_ptr _tmp84;struct
_dyneither_ptr _tmp86;_LL6: {struct Cyc_Core_Impossible_struct*_tmp80=(struct Cyc_Core_Impossible_struct*)
_tmp7F;if(_tmp80->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp81=_tmp80->f1;}}
_LL7:{const char*_tmp43F;exn_string=((_tmp43F="Exception Core::Impossible",
_tag_dyneither(_tmp43F,sizeof(char),27)));}explain_string=_tmp81;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp82=(struct Cyc_Dict_Absent_struct*)_tmp7F;if(
_tmp82->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp440;exn_string=((
_tmp440="Exception Dict::Absent",_tag_dyneither(_tmp440,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp83=(struct Cyc_Core_Invalid_argument_struct*)
_tmp7F;if(_tmp83->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp84=_tmp83->f1;}}
_LLB:{const char*_tmp441;exn_string=((_tmp441="Exception Core::Invalid_argument",
_tag_dyneither(_tmp441,sizeof(char),33)));}explain_string=_tmp84;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp85=(struct Cyc_Core_Failure_struct*)_tmp7F;if(
_tmp85->tag != Cyc_Core_Failure)goto _LLE;else{_tmp86=_tmp85->f1;}}_LLD:{const char*
_tmp442;exn_string=((_tmp442="Exception Core::Failure",_tag_dyneither(_tmp442,
sizeof(char),24)));}explain_string=_tmp86;goto _LL5;_LLE:;_LLF: ex=_tmp7F;
other_exn=1;{const char*_tmp443;exn_string=((_tmp443="Uncaught exception",
_tag_dyneither(_tmp443,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp7F);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp444;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp444="",
_tag_dyneither(_tmp444,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp44A;void*_tmp449[3];struct Cyc_String_pa_struct _tmp448;struct Cyc_String_pa_struct
_tmp447;struct Cyc_String_pa_struct _tmp446;(_tmp446.tag=0,((_tmp446.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp447.tag=0,((_tmp447.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp448.tag=0,((
_tmp448.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp449[0]=&
_tmp448,((_tmp449[1]=& _tmp447,((_tmp449[2]=& _tmp446,Cyc_fprintf(Cyc_stderr,((
_tmp44A="\n%s thrown during %s: %s",_tag_dyneither(_tmp44A,sizeof(char),25))),
_tag_dyneither(_tmp449,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp44D;void*_tmp44C;(_tmp44C=0,Cyc_fprintf(Cyc_stderr,((_tmp44D="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp44D,sizeof(char),22))),_tag_dyneither(_tmp44C,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp7A;}else{if(Cyc_v_r){{const char*_tmp451;void*_tmp450[1];struct Cyc_String_pa_struct
_tmp44F;(_tmp44F.tag=0,((_tmp44F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp450[0]=& _tmp44F,Cyc_fprintf(Cyc_stderr,((_tmp451="%s completed.\n",
_tag_dyneither(_tmp451,sizeof(char),15))),_tag_dyneither(_tmp450,sizeof(void*),1)))))));}
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
_tmpA3);const char*_tmp456;const char*_tmp455;struct _tuple17*_tmp454;if(!Cyc_Interface_is_subinterface(
_tmpA6,_tmpA5,((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454->f1=((_tmp455="written interface",
_tag_dyneither(_tmp455,sizeof(char),18))),((_tmp454->f2=((_tmp456="maximal interface",
_tag_dyneither(_tmp456,sizeof(char),18))),_tmp454))))))))Cyc_compile_failure=1;
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
_tmp459;void*_tmp458;(_tmp458=0,Cyc_fprintf(Cyc_stderr,((_tmp459="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp459,sizeof(char),53))),_tag_dyneither(_tmp458,sizeof(void*),0)));}
return 1;}{const char*_tmp45B;const char*_tmp45A;struct Cyc___cycFILE*_tmpAC=Cyc_try_file_open(
file,((_tmp45A="r",_tag_dyneither(_tmp45A,sizeof(char),2))),((_tmp45B="internal compiler file",
_tag_dyneither(_tmp45B,sizeof(char),23))));if(_tmpAC == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpB3=1024;
unsigned int i;for(i=0;i < _tmpB3;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmpAC);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAC)){Cyc_compile_failure=
1;{const char*_tmp45F;void*_tmp45E[1];struct Cyc_String_pa_struct _tmp45D;(_tmp45D.tag=
0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp45E[0]=&
_tmp45D,Cyc_fprintf(Cyc_stderr,((_tmp45F="\nError: problem copying %s\n",
_tag_dyneither(_tmp45F,sizeof(char),28))),_tag_dyneither(_tmp45E,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp463;void*_tmp462[1];struct Cyc_String_pa_struct
_tmp461;(_tmp461.tag=0,((_tmp461.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp462[0]=& _tmp461,Cyc_fprintf(Cyc_stderr,((_tmp463="\nError: problem copying %s\n",
_tag_dyneither(_tmp463,sizeof(char),28))),_tag_dyneither(_tmp462,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmpAC);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpB6=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpB6.expand_typedefs=!Cyc_noexpand_r;_tmpB6.to_VC=Cyc_Cyclone_tovc_r;
_tmpB6.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpB6.generate_line_directives=
Cyc_generate_line_directives_r;_tmpB6.print_full_evars=Cyc_print_full_evars_r;
_tmpB6.print_all_tvars=Cyc_print_all_tvars_r;_tmpB6.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB6.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp466;void*_tmp465;(_tmp465=0,Cyc_fprintf(out_file,((_tmp466="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp466,sizeof(char),27))),_tag_dyneither(_tmp465,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r){
const char*_tmp469;void*_tmp468;(_tmp468=0,Cyc_fprintf(out_file,((_tmp469="#include <setjmp.h>\n",
_tag_dyneither(_tmp469,sizeof(char),21))),_tag_dyneither(_tmp468,sizeof(void*),0)));}
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
_tag_dyneither(0,0,0)).curr){{struct _dyneither_ptr*_tmp46C;struct Cyc_List_List*
_tmp46B;_tmpBB=((_tmp46B=_cycalloc(sizeof(*_tmp46B)),((_tmp46B->hd=((_tmp46C=
_cycalloc(sizeof(*_tmp46C)),((_tmp46C[0]=s,_tmp46C)))),((_tmp46B->tl=_tmpBB,
_tmp46B))))));}break;}else{{struct _dyneither_ptr*_tmp46F;struct Cyc_List_List*
_tmp46E;_tmpBB=((_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E->hd=((_tmp46F=
_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBD).curr - s.curr)
/ sizeof(char))),_tmp46F)))),((_tmp46E->tl=_tmpBB,_tmp46E))))));}_tmpBC -=(((
struct _dyneither_ptr)_tmpBD).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(
_tmpBD,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpBB);};}static int Cyc_file_exists(struct _dyneither_ptr file);
static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{
struct _handler_cons _tmpC2;_push_handler(& _tmpC2);{int _tmpC4=0;if(setjmp(_tmpC2.handler))
_tmpC4=1;if(!_tmpC4){{const char*_tmp470;f=(struct Cyc___cycFILE*)Cyc_file_open(
file,((_tmp470="r",_tag_dyneither(_tmp470,sizeof(char),2))));};_pop_handler();}
else{void*_tmpC3=(void*)_exn_thrown;void*_tmpC7=_tmpC3;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpC7);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs);static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*
dirs){const char*_tmp471;struct _dyneither_ptr tmp=(_tmp471="",_tag_dyneither(
_tmp471,sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr
_tmpC8=*((struct _dyneither_ptr*)dirs->hd);if((char*)_tmpC8.curr == (char*)(
_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC8)== 0)
continue;{const char*_tmp472;_tmpC8=(struct _dyneither_ptr)Cyc_strconcat((struct
_dyneither_ptr)_tmpC8,((_tmp472=":",_tag_dyneither(_tmp472,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC8,(struct
_dyneither_ptr)tmp);}return tmp;}static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file);static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpCB=*((
struct _dyneither_ptr*)dirs->hd);if((char*)_tmpCB.curr == (char*)(_tag_dyneither(0,
0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCB)== 0)continue;{struct
_dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpCB,file);if(Cyc_file_exists(
s)){struct _dyneither_ptr*_tmp473;return(_tmp473=_cycalloc(sizeof(*_tmp473)),((
_tmp473[0]=s,_tmp473)));}};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file);static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpCD=
Cyc_find(dirs,file);if(_tmpCD == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
const char*_tmp478;void*_tmp477[2];struct Cyc_String_pa_struct _tmp476;struct Cyc_String_pa_struct
_tmp475;(_tmp475.tag=0,((_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp476.tag=0,((_tmp476.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file),((_tmp477[0]=& _tmp476,((_tmp477[1]=& _tmp475,Cyc_fprintf(
Cyc_stderr,((_tmp478="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp478,sizeof(char),56))),_tag_dyneither(_tmp477,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp488;const char*_tmp487;void*_tmp486[2];struct Cyc_String_pa_struct _tmp485;
struct Cyc_String_pa_struct _tmp484;struct Cyc_Core_Failure_struct*_tmp483;(int)
_throw((void*)((_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=((_tmp488.tag=
Cyc_Core_Failure,((_tmp488.f1=(struct _dyneither_ptr)((_tmp484.tag=0,((_tmp484.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp485.tag=
0,((_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp486[0]=&
_tmp485,((_tmp486[1]=& _tmp484,Cyc_aprintf(((_tmp487="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp487,sizeof(char),56))),_tag_dyneither(_tmp486,sizeof(void*),2)))))))))))))),
_tmp488)))),_tmp483)))));};}return*_tmpCD;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir);static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*
_tmpD8=0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp48B;struct Cyc_List_List*
_tmp48A;_tmpD8=((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A->hd=((_tmp48B=
_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp48B)))),((_tmp48A->tl=_tmpD8,_tmp48A))))));}
_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD8);
return _tmpD8;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpDB=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp48C;_tmpDB=((_tmp48C=_cycalloc(sizeof(*
_tmp48C)),((_tmp48C->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp48C->tl=_tmpDB,
_tmp48C))))));}{struct _dyneither_ptr*_tmp48F;struct Cyc_List_List*_tmp48E;_tmpDB=((
_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E->hd=((_tmp48F=_cycalloc(sizeof(*
_tmp48F)),((_tmp48F[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp48F)))),((_tmp48E->tl=_tmpDB,_tmp48E))))));};}
_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDB);
return _tmpDB;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmp498(unsigned int*_tmp497,unsigned int*
_tmp496,char**_tmp494){for(*_tmp497=0;*_tmp497 < *_tmp496;(*_tmp497)++){(*_tmp494)[*
_tmp497]='\000';}}static struct _dyneither_ptr Cyc_sh_escape_string(struct
_dyneither_ptr s){unsigned long _tmpDF=Cyc_strlen((struct _dyneither_ptr)s);int
_tmpE0=0;int _tmpE1=0;{int i=0;for(0;i < _tmpDF;++ i){char _tmpE2=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpE2 == '\'')++ _tmpE0;else{if(
Cyc_is_other_special(_tmpE2))++ _tmpE1;}}}if(_tmpE0 == 0  && _tmpE1 == 0)return s;if(
_tmpE0 == 0){struct _dyneither_ptr*_tmp492;struct _dyneither_ptr*_tmp491[3];return(
struct _dyneither_ptr)Cyc_strconcat_l(((_tmp491[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp491[1]=((_tmp492=
_cycalloc(sizeof(*_tmp492)),((_tmp492[0]=(struct _dyneither_ptr)s,_tmp492)))),((
_tmp491[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp491,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long
_tmpE7=(_tmpDF + _tmpE0)+ _tmpE1;unsigned int _tmp497;unsigned int _tmp496;struct
_dyneither_ptr _tmp495;char*_tmp494;unsigned int _tmp493;struct _dyneither_ptr s2=(
_tmp493=_tmpE7 + 1,((_tmp494=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp493 + 1)),((_tmp495=_tag_dyneither(_tmp494,sizeof(char),_tmp493 + 1),((((
_tmp496=_tmp493,((_tmp498(& _tmp497,& _tmp496,& _tmp494),_tmp494[_tmp496]=(char)0)))),
_tmp495)))))));int _tmpE8=0;int _tmpE9=0;for(0;_tmpE8 < _tmpDF;++ _tmpE8){char _tmpEA=*((
const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE8));if(_tmpEA == '\''
 || Cyc_is_other_special(_tmpEA)){char _tmp49B;char _tmp49A;struct _dyneither_ptr
_tmp499;(_tmp499=_dyneither_ptr_plus(s2,sizeof(char),_tmpE9 ++),((_tmp49A=*((char*)
_check_dyneither_subscript(_tmp499,sizeof(char),0)),((_tmp49B='\\',((
_get_dyneither_size(_tmp499,sizeof(char))== 1  && (_tmp49A == '\000'  && _tmp49B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp499.curr)=_tmp49B)))))));}{char _tmp49E;char
_tmp49D;struct _dyneither_ptr _tmp49C;(_tmp49C=_dyneither_ptr_plus(s2,sizeof(char),
_tmpE9 ++),((_tmp49D=*((char*)_check_dyneither_subscript(_tmp49C,sizeof(char),0)),((
_tmp49E=_tmpEA,((_get_dyneither_size(_tmp49C,sizeof(char))== 1  && (_tmp49D == '\000'
 && _tmp49E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp49C.curr)=_tmp49E)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp49F;return(_tmp49F=_cycalloc(
sizeof(*_tmp49F)),((_tmp49F[0]=Cyc_sh_escape_string(*sp),_tmp49F)));}static void
Cyc_process_file(struct _dyneither_ptr filename);static void Cyc_process_file(struct
_dyneither_ptr filename){struct _dyneither_ptr _tmpF6=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp4A0;struct _dyneither_ptr _tmpF7=Cyc_strconcat((
struct _dyneither_ptr)_tmpF6,((_tmp4A0=".cyp",_tag_dyneither(_tmp4A0,sizeof(char),
5))));const char*_tmp4A1;struct _dyneither_ptr _tmpF8=Cyc_strconcat((struct
_dyneither_ptr)_tmpF6,((_tmp4A1=".cyci",_tag_dyneither(_tmp4A1,sizeof(char),6))));
const char*_tmp4A2;struct _dyneither_ptr _tmpF9=Cyc_strconcat((struct _dyneither_ptr)
_tmpF6,((_tmp4A2=".cycio",_tag_dyneither(_tmp4A2,sizeof(char),7))));const char*
_tmp4A3;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct _dyneither_ptr)_tmpF6,((
_tmp4A3=".c",_tag_dyneither(_tmp4A3,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp4A7;void*_tmp4A6[1];struct Cyc_String_pa_struct _tmp4A5;(_tmp4A5.tag=0,((
_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4A6[0]=&
_tmp4A5,Cyc_fprintf(Cyc_stderr,((_tmp4A7="Compiling %s\n",_tag_dyneither(_tmp4A7,
sizeof(char),14))),_tag_dyneither(_tmp4A6,sizeof(void*),1)))))));}{const char*
_tmp4A9;const char*_tmp4A8;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp4A8="r",_tag_dyneither(_tmp4A8,sizeof(char),2))),((_tmp4A9="input file",
_tag_dyneither(_tmp4A9,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp4B0;
struct _dyneither_ptr*_tmp4AF;const char*_tmp4AE;struct Cyc_List_List*_tmp4AD;
struct _dyneither_ptr _tmpFE=Cyc_str_sepstr(((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((
_tmp4AD->hd=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=(struct
_dyneither_ptr)((_tmp4AE="",_tag_dyneither(_tmp4AE,sizeof(char),1))),_tmp4AF)))),((
_tmp4AD->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp4AD)))))),((_tmp4B0=" ",_tag_dyneither(_tmp4B0,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpFF=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp4B1;_tmpFF=Cyc_add_subdir(_tmpFF,((_tmp4B1="include",
_tag_dyneither(_tmp4B1,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp4B4;struct Cyc_List_List*_tmp4B3;
_tmpFF=((_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->hd=((_tmp4B4=_cycalloc(
sizeof(*_tmp4B4)),((_tmp4B4[0]=def_inc_path,_tmp4B4)))),((_tmp4B3->tl=_tmpFF,
_tmp4B3))))));}{char*_tmp103=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmp103 != 0){char*_tmp4B5;_tmpFF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4B5=_tmp103,
_tag_dyneither(_tmp4B5,sizeof(char),_get_zero_arr_size_char((void*)_tmp4B5,1)))),':'),
_tmpFF);}{const char*_tmp4BC;struct _dyneither_ptr*_tmp4BB;const char*_tmp4BA;
struct Cyc_List_List*_tmp4B9;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9->hd=((
_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=(struct _dyneither_ptr)((_tmp4BA="",
_tag_dyneither(_tmp4BA,sizeof(char),1))),_tmp4BB)))),((_tmp4B9->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpFF),_tmp4B9)))))),((_tmp4BC=" -I",_tag_dyneither(
_tmp4BC,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp4C0;void*_tmp4BF[1];struct Cyc_String_pa_struct
_tmp4BE;ofile_string=(struct _dyneither_ptr)((_tmp4BE.tag=0,((_tmp4BE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp4BF[0]=& _tmp4BE,Cyc_aprintf(((_tmp4C0=" > %s",_tag_dyneither(_tmp4C0,sizeof(
char),6))),_tag_dyneither(_tmp4BF,sizeof(void*),1))))))));}else{const char*
_tmp4C1;ofile_string=((_tmp4C1="",_tag_dyneither(_tmp4C1,sizeof(char),1)));}}
else{const char*_tmp4C5;void*_tmp4C4[1];struct Cyc_String_pa_struct _tmp4C3;
ofile_string=(struct _dyneither_ptr)((_tmp4C3.tag=0,((_tmp4C3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpF7)),((_tmp4C4[0]=& _tmp4C3,Cyc_aprintf(((_tmp4C5=" > %s",_tag_dyneither(
_tmp4C5,sizeof(char),6))),_tag_dyneither(_tmp4C4,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp4C6;fixup_string=((_tmp4C6=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp4C6,sizeof(char),35)));}else{const char*_tmp4CA;void*_tmp4C9[1];
struct Cyc_String_pa_struct _tmp4C8;fixup_string=(struct _dyneither_ptr)((_tmp4C8.tag=
0,((_tmp4C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4C9[0]=& _tmp4C8,Cyc_aprintf(((
_tmp4CA=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4CA,sizeof(char),29))),
_tag_dyneither(_tmp4C9,sizeof(void*),1))))))));}}else{const char*_tmp4CB;
fixup_string=((_tmp4CB="",_tag_dyneither(_tmp4CB,sizeof(char),1)));}{const char*
_tmp4D4;void*_tmp4D3[6];struct Cyc_String_pa_struct _tmp4D2;struct Cyc_String_pa_struct
_tmp4D1;struct Cyc_String_pa_struct _tmp4D0;struct Cyc_String_pa_struct _tmp4CF;
struct Cyc_String_pa_struct _tmp4CE;struct Cyc_String_pa_struct _tmp4CD;struct
_dyneither_ptr _tmp111=(_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp4CE.tag=0,((_tmp4CE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp4CF.tag=0,((_tmp4CF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp4D0.tag=
0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp4D1.tag=0,((_tmp4D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE),((
_tmp4D2.tag=0,((_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp4D3[0]=& _tmp4D2,((_tmp4D3[1]=& _tmp4D1,((_tmp4D3[2]=& _tmp4D0,((_tmp4D3[3]=&
_tmp4CF,((_tmp4D3[4]=& _tmp4CE,((_tmp4D3[5]=& _tmp4CD,Cyc_aprintf(((_tmp4D4="%s %s%s %s%s%s",
_tag_dyneither(_tmp4D4,sizeof(char),15))),_tag_dyneither(_tmp4D3,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp4D8;void*_tmp4D7[1];struct Cyc_String_pa_struct _tmp4D6;(
_tmp4D6.tag=0,((_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp111),((
_tmp4D7[0]=& _tmp4D6,Cyc_fprintf(Cyc_stderr,((_tmp4D8="%s\n",_tag_dyneither(
_tmp4D8,sizeof(char),4))),_tag_dyneither(_tmp4D7,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp111,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp4DB;void*_tmp4DA;(_tmp4DA=0,Cyc_fprintf(Cyc_stderr,((_tmp4DB="\nError: preprocessing\n",
_tag_dyneither(_tmp4DB,sizeof(char),23))),_tag_dyneither(_tmp4DA,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpF7);{const char*_tmp4DD;const char*_tmp4DC;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF7,((_tmp4DC="r",
_tag_dyneither(_tmp4DC,sizeof(char),2))),((_tmp4DD="file",_tag_dyneither(_tmp4DD,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp117;_push_handler(& _tmp117);{int _tmp119=0;if(setjmp(
_tmp117.handler))_tmp119=1;if(!_tmp119){{const char*_tmp4DE;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4DE="parsing",_tag_dyneither(_tmp4DE,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp118=(
void*)_exn_thrown;void*_tmp11C=_tmp118;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpF7);(int)_throw(
_tmp11C);_LL29:;_LL2A:(void)_throw(_tmp11C);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF7);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp11D=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11D;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp11E=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple19 _tmp4DF;struct _tuple19 _tmp11F=(_tmp4DF.f1=
tc_rgn,((_tmp4DF.f2=_tmp11E,_tmp4DF)));{const char*_tmp4E0;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple19*,struct
Cyc_List_List*),struct _tuple19*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E0="type checking",_tag_dyneither(_tmp4E0,sizeof(char),14))),Cyc_do_typecheck,&
_tmp11F,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpF7);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp4E1;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E1="control-flow checking",_tag_dyneither(_tmp4E1,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF7);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpF8,sizeof(char),1)),(const char*)"r");
const char*_tmp4E3;const char*_tmp4E2;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpF9,((_tmp4E2="w",_tag_dyneither(_tmp4E2,sizeof(char),2))),((
_tmp4E3="interface object file",_tag_dyneither(_tmp4E3,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpF8);{struct _tuple20 _tmp4E4;struct _tuple20 _tmp122=(
_tmp4E4.f1=_tmp11E,((_tmp4E4.f2=inter_file,((_tmp4E4.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp4E4)))));{const char*_tmp4E5;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E5="interface checking",_tag_dyneither(_tmp4E5,sizeof(char),19))),Cyc_do_interface,&
_tmp122,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp4E6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp4E6="translation to C",_tag_dyneither(_tmp4E6,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpF7);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpF7);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp4E7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E7="post-pass to VC",_tag_dyneither(_tmp4E7,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp4E9;const char*_tmp4E8;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4E8="w",_tag_dyneither(
_tmp4E8,sizeof(char),2))),((_tmp4E9="output file",_tag_dyneither(_tmp4E9,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp4EB;const char*_tmp4EA;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4EA="w",_tag_dyneither(
_tmp4EA,sizeof(char),2))),((_tmp4EB="output file",_tag_dyneither(_tmp4EB,sizeof(
char),12))));}else{const char*_tmp4ED;const char*_tmp4EC;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFA,((_tmp4EC="w",_tag_dyneither(_tmp4EC,sizeof(char),2))),((
_tmp4ED="output file",_tag_dyneither(_tmp4ED,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp130;_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=
1;if(!_tmp132){if(!Cyc_noprint_r){const char*_tmp4EE;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4EE="printing",_tag_dyneither(_tmp4EE,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp131=(void*)
_exn_thrown;void*_tmp135=_tmp131;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp4F1;struct Cyc_List_List*
_tmp4F0;Cyc_cfiles=((_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0->hd=((_tmp4F1=
_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=(struct _dyneither_ptr)_tmpFA,_tmp4F1)))),((
_tmp4F0->tl=Cyc_cfiles,_tmp4F0))))));}(int)_throw(_tmp135);_LL2E:;_LL2F:(void)
_throw(_tmp135);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp4F4;struct Cyc_List_List*_tmp4F3;Cyc_cfiles=((_tmp4F3=
_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3->hd=((_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((
_tmp4F4[0]=(struct _dyneither_ptr)_tmpFA,_tmp4F4)))),((_tmp4F3->tl=Cyc_cfiles,
_tmp4F3))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp152[8]="<final>";
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
const char*_tmp4F5;struct _dyneither_ptr _tmp159=Cyc_strconcat((struct
_dyneither_ptr)basename,((_tmp4F5=".cycio",_tag_dyneither(_tmp4F5,sizeof(char),7))));
const char*_tmp4F7;const char*_tmp4F6;struct Cyc___cycFILE*_tmp15A=Cyc_try_file_open((
struct _dyneither_ptr)_tmp159,((_tmp4F6="rb",_tag_dyneither(_tmp4F6,sizeof(char),
3))),((_tmp4F7="interface object file",_tag_dyneither(_tmp4F7,sizeof(char),22))));
if(_tmp15A == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp159);{struct Cyc_Interface_I*_tmp15B=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp15A);Cyc_file_close((struct Cyc___cycFILE*)_tmp15A);
return _tmp15B;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n);static int Cyc_is_cfile(
struct _dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple21{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4FD(unsigned int*
_tmp4FC,unsigned int*_tmp4FB,int**_tmp4F9){for(*_tmp4FC=0;*_tmp4FC < *_tmp4FB;(*
_tmp4FC)++){(*_tmp4F9)[*_tmp4FC]=0;}}static void _tmp505(unsigned int*_tmp504,
unsigned int*_tmp503,struct _dyneither_ptr**_tmp501){for(*_tmp504=0;*_tmp504 < *
_tmp503;(*_tmp504)++){(*_tmp501)[*_tmp504]=(struct _dyneither_ptr)_tag_dyneither(
0,0,0);}}static void _tmp50B(unsigned int*_tmp50A,unsigned int*_tmp509,struct
_dyneither_ptr**_tmp507){for(*_tmp50A=0;*_tmp50A < *_tmp509;(*_tmp50A)++){(*
_tmp507)[*_tmp50A]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp15F=
_new_region("r");struct _RegionHandle*r=& _tmp15F;_push_region(r);{unsigned int
_tmp4FC;unsigned int _tmp4FB;struct _dyneither_ptr _tmp4FA;int*_tmp4F9;unsigned int
_tmp4F8;struct _dyneither_ptr _tmp160=(_tmp4F8=(unsigned int)argc,((_tmp4F9=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp4F8)),((_tmp4FA=_tag_dyneither(
_tmp4F9,sizeof(int),_tmp4F8),((((_tmp4FB=_tmp4F8,_tmp4FD(& _tmp4FC,& _tmp4FB,&
_tmp4F9))),_tmp4FA)))))));int _tmp161=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp4FE;if(Cyc_strncmp(((_tmp4FE="-B",_tag_dyneither(_tmp4FE,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp160,sizeof(int),i))=1;++ _tmp161;}else{const char*_tmp4FF;if(Cyc_strcmp(((
_tmp4FF="-b",_tag_dyneither(_tmp4FF,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp160,sizeof(int),
i))=1;++ _tmp161;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp160,
sizeof(int),i))=1;++ _tmp161;}}}{unsigned int _tmp504;unsigned int _tmp503;struct
_dyneither_ptr _tmp502;struct _dyneither_ptr*_tmp501;unsigned int _tmp500;struct
_dyneither_ptr _tmp164=(_tmp500=(unsigned int)(_tmp161 + 1),((_tmp501=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp500)),((
_tmp502=_tag_dyneither(_tmp501,sizeof(struct _dyneither_ptr),_tmp500),((((_tmp503=
_tmp500,_tmp505(& _tmp504,& _tmp503,& _tmp501))),_tmp502)))))));unsigned int _tmp50A;
unsigned int _tmp509;struct _dyneither_ptr _tmp508;struct _dyneither_ptr*_tmp507;
unsigned int _tmp506;struct _dyneither_ptr _tmp165=(_tmp506=(unsigned int)(argc - 
_tmp161),((_tmp507=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp506)),((_tmp508=_tag_dyneither(_tmp507,sizeof(struct
_dyneither_ptr),_tmp506),((((_tmp509=_tmp506,_tmp50B(& _tmp50A,& _tmp509,& _tmp507))),
_tmp508)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp160,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple21*_tmpA1C;const char*_tmpA1B;const char*
_tmpA1A;struct Cyc_Arg_Set_spec_struct _tmpA19;struct Cyc_Arg_Set_spec_struct*
_tmpA18;const char*_tmpA17;struct _tuple21*_tmpA16;const char*_tmpA15;const char*
_tmpA14;struct Cyc_Arg_Unit_spec_struct _tmpA13;struct Cyc_Arg_Unit_spec_struct*
_tmpA12;const char*_tmpA11;struct _tuple21*_tmpA10;const char*_tmpA0F;const char*
_tmpA0E;struct Cyc_Arg_String_spec_struct _tmpA0D;struct Cyc_Arg_String_spec_struct*
_tmpA0C;const char*_tmpA0B;struct _tuple21*_tmpA0A;const char*_tmpA09;const char*
_tmpA08;struct Cyc_Arg_Flag_spec_struct _tmpA07;struct Cyc_Arg_Flag_spec_struct*
_tmpA06;const char*_tmpA05;struct _tuple21*_tmpA04;const char*_tmpA03;const char*
_tmpA02;struct Cyc_Arg_Flag_spec_struct _tmpA01;struct Cyc_Arg_Flag_spec_struct*
_tmpA00;const char*_tmp9FF;struct _tuple21*_tmp9FE;const char*_tmp9FD;const char*
_tmp9FC;struct Cyc_Arg_Flag_spec_struct _tmp9FB;struct Cyc_Arg_Flag_spec_struct*
_tmp9FA;const char*_tmp9F9;struct _tuple21*_tmp9F8;const char*_tmp9F7;const char*
_tmp9F6;struct Cyc_Arg_Flag_spec_struct _tmp9F5;struct Cyc_Arg_Flag_spec_struct*
_tmp9F4;const char*_tmp9F3;struct _tuple21*_tmp9F2;const char*_tmp9F1;const char*
_tmp9F0;struct Cyc_Arg_Flag_spec_struct _tmp9EF;struct Cyc_Arg_Flag_spec_struct*
_tmp9EE;const char*_tmp9ED;struct _tuple21*_tmp9EC;const char*_tmp9EB;const char*
_tmp9EA;struct Cyc_Arg_Flag_spec_struct _tmp9E9;struct Cyc_Arg_Flag_spec_struct*
_tmp9E8;const char*_tmp9E7;struct _tuple21*_tmp9E6;const char*_tmp9E5;const char*
_tmp9E4;struct Cyc_Arg_Unit_spec_struct _tmp9E3;struct Cyc_Arg_Unit_spec_struct*
_tmp9E2;const char*_tmp9E1;struct _tuple21*_tmp9E0;const char*_tmp9DF;const char*
_tmp9DE;struct Cyc_Arg_String_spec_struct _tmp9DD;struct Cyc_Arg_String_spec_struct*
_tmp9DC;const char*_tmp9DB;struct _tuple21*_tmp9DA;const char*_tmp9D9;const char*
_tmp9D8;struct Cyc_Arg_Flag_spec_struct _tmp9D7;struct Cyc_Arg_Flag_spec_struct*
_tmp9D6;const char*_tmp9D5;struct _tuple21*_tmp9D4;const char*_tmp9D3;const char*
_tmp9D2;struct Cyc_Arg_Flag_spec_struct _tmp9D1;struct Cyc_Arg_Flag_spec_struct*
_tmp9D0;const char*_tmp9CF;struct _tuple21*_tmp9CE;const char*_tmp9CD;const char*
_tmp9CC;struct Cyc_Arg_Flag_spec_struct _tmp9CB;struct Cyc_Arg_Flag_spec_struct*
_tmp9CA;const char*_tmp9C9;struct _tuple21*_tmp9C8;const char*_tmp9C7;const char*
_tmp9C6;struct Cyc_Arg_Flag_spec_struct _tmp9C5;struct Cyc_Arg_Flag_spec_struct*
_tmp9C4;const char*_tmp9C3;struct _tuple21*_tmp9C2;const char*_tmp9C1;const char*
_tmp9C0;struct Cyc_Arg_Flag_spec_struct _tmp9BF;struct Cyc_Arg_Flag_spec_struct*
_tmp9BE;const char*_tmp9BD;struct _tuple21*_tmp9BC;const char*_tmp9BB;const char*
_tmp9BA;struct Cyc_Arg_Flag_spec_struct _tmp9B9;struct Cyc_Arg_Flag_spec_struct*
_tmp9B8;const char*_tmp9B7;struct _tuple21*_tmp9B6;const char*_tmp9B5;const char*
_tmp9B4;struct Cyc_Arg_Flag_spec_struct _tmp9B3;struct Cyc_Arg_Flag_spec_struct*
_tmp9B2;const char*_tmp9B1;struct _tuple21*_tmp9B0;const char*_tmp9AF;const char*
_tmp9AE;struct Cyc_Arg_Flag_spec_struct _tmp9AD;struct Cyc_Arg_Flag_spec_struct*
_tmp9AC;const char*_tmp9AB;struct _tuple21*_tmp9AA;const char*_tmp9A9;const char*
_tmp9A8;struct Cyc_Arg_Unit_spec_struct _tmp9A7;struct Cyc_Arg_Unit_spec_struct*
_tmp9A6;const char*_tmp9A5;struct _tuple21*_tmp9A4;const char*_tmp9A3;const char*
_tmp9A2;struct Cyc_Arg_Unit_spec_struct _tmp9A1;struct Cyc_Arg_Unit_spec_struct*
_tmp9A0;const char*_tmp99F;struct _tuple21*_tmp99E;const char*_tmp99D;const char*
_tmp99C;struct Cyc_Arg_Unit_spec_struct _tmp99B;struct Cyc_Arg_Unit_spec_struct*
_tmp99A;const char*_tmp999;struct _tuple21*_tmp998;const char*_tmp997;const char*
_tmp996;struct Cyc_Arg_Unit_spec_struct _tmp995;struct Cyc_Arg_Unit_spec_struct*
_tmp994;const char*_tmp993;struct _tuple21*_tmp992;const char*_tmp991;const char*
_tmp990;struct Cyc_Arg_Flag_spec_struct _tmp98F;struct Cyc_Arg_Flag_spec_struct*
_tmp98E;const char*_tmp98D;struct _tuple21*_tmp98C;const char*_tmp98B;const char*
_tmp98A;struct Cyc_Arg_String_spec_struct _tmp989;struct Cyc_Arg_String_spec_struct*
_tmp988;const char*_tmp987;struct _tuple21*_tmp986;const char*_tmp985;const char*
_tmp984;struct Cyc_Arg_String_spec_struct _tmp983;struct Cyc_Arg_String_spec_struct*
_tmp982;const char*_tmp981;struct _tuple21*_tmp980;const char*_tmp97F;const char*
_tmp97E;struct Cyc_Arg_Set_spec_struct _tmp97D;struct Cyc_Arg_Set_spec_struct*
_tmp97C;const char*_tmp97B;struct _tuple21*_tmp97A;const char*_tmp979;const char*
_tmp978;struct Cyc_Arg_Unit_spec_struct _tmp977;struct Cyc_Arg_Unit_spec_struct*
_tmp976;const char*_tmp975;struct _tuple21*_tmp974;const char*_tmp973;const char*
_tmp972;struct Cyc_Arg_Set_spec_struct _tmp971;struct Cyc_Arg_Set_spec_struct*
_tmp970;const char*_tmp96F;struct _tuple21*_tmp96E;const char*_tmp96D;const char*
_tmp96C;struct Cyc_Arg_Set_spec_struct _tmp96B;struct Cyc_Arg_Set_spec_struct*
_tmp96A;const char*_tmp969;struct _tuple21*_tmp968;const char*_tmp967;const char*
_tmp966;struct Cyc_Arg_Set_spec_struct _tmp965;struct Cyc_Arg_Set_spec_struct*
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
_tmp942;struct Cyc_Arg_Clear_spec_struct _tmp941;struct Cyc_Arg_Clear_spec_struct*
_tmp940;const char*_tmp93F;struct _tuple21*_tmp93E;const char*_tmp93D;const char*
_tmp93C;struct Cyc_Arg_Set_spec_struct _tmp93B;struct Cyc_Arg_Set_spec_struct*
_tmp93A;const char*_tmp939;struct _tuple21*_tmp938;const char*_tmp937;const char*
_tmp936;struct Cyc_Arg_Clear_spec_struct _tmp935;struct Cyc_Arg_Clear_spec_struct*
_tmp934;const char*_tmp933;struct _tuple21*_tmp932;const char*_tmp931;const char*
_tmp930;struct Cyc_Arg_Unit_spec_struct _tmp92F;struct Cyc_Arg_Unit_spec_struct*
_tmp92E;const char*_tmp92D;struct _tuple21*_tmp92C;const char*_tmp92B;const char*
_tmp92A;struct Cyc_Arg_String_spec_struct _tmp929;struct Cyc_Arg_String_spec_struct*
_tmp928;const char*_tmp927;struct _tuple21*_tmp926;const char*_tmp925;const char*
_tmp924;struct Cyc_Arg_Unit_spec_struct _tmp923;struct Cyc_Arg_Unit_spec_struct*
_tmp922;const char*_tmp921;struct _tuple21*_tmp920;const char*_tmp91F;const char*
_tmp91E;struct Cyc_Arg_Set_spec_struct _tmp91D;struct Cyc_Arg_Set_spec_struct*
_tmp91C;const char*_tmp91B;struct _tuple21*_tmp91A;const char*_tmp919;const char*
_tmp918;struct Cyc_Arg_Unit_spec_struct _tmp917;struct Cyc_Arg_Unit_spec_struct*
_tmp916;const char*_tmp915;struct _tuple21*_tmp914;const char*_tmp913;const char*
_tmp912;struct Cyc_Arg_Unit_spec_struct _tmp911;struct Cyc_Arg_Unit_spec_struct*
_tmp910;const char*_tmp90F;struct _tuple21*_tmp90E;const char*_tmp90D;const char*
_tmp90C;struct Cyc_Arg_Unit_spec_struct _tmp90B;struct Cyc_Arg_Unit_spec_struct*
_tmp90A;const char*_tmp909;struct _tuple21*_tmp908;const char*_tmp907;const char*
_tmp906;struct Cyc_Arg_Unit_spec_struct _tmp905;struct Cyc_Arg_Unit_spec_struct*
_tmp904;const char*_tmp903;struct _tuple21*_tmp902;const char*_tmp901;const char*
_tmp900;struct Cyc_Arg_Unit_spec_struct _tmp8FF;struct Cyc_Arg_Unit_spec_struct*
_tmp8FE;const char*_tmp8FD;struct _tuple21*_tmp8FC;const char*_tmp8FB;const char*
_tmp8FA;struct Cyc_Arg_String_spec_struct _tmp8F9;struct Cyc_Arg_String_spec_struct*
_tmp8F8;const char*_tmp8F7;struct _tuple21*_tmp8F6;const char*_tmp8F5;const char*
_tmp8F4;struct Cyc_Arg_Unit_spec_struct _tmp8F3;struct Cyc_Arg_Unit_spec_struct*
_tmp8F2;const char*_tmp8F1;struct _tuple21*_tmp8F0;const char*_tmp8EF;const char*
_tmp8EE;struct Cyc_Arg_Unit_spec_struct _tmp8ED;struct Cyc_Arg_Unit_spec_struct*
_tmp8EC;const char*_tmp8EB;struct _tuple21*_tmp8EA;const char*_tmp8E9;const char*
_tmp8E8;struct Cyc_Arg_Set_spec_struct _tmp8E7;struct Cyc_Arg_Set_spec_struct*
_tmp8E6;const char*_tmp8E5;struct _tuple21*_tmp8E4;const char*_tmp8E3;const char*
_tmp8E2;struct Cyc_Arg_Set_spec_struct _tmp8E1;struct Cyc_Arg_Set_spec_struct*
_tmp8E0;const char*_tmp8DF;struct _tuple21*_tmp8DE;const char*_tmp8DD;const char*
_tmp8DC;struct Cyc_Arg_Set_spec_struct _tmp8DB;struct Cyc_Arg_Set_spec_struct*
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
_tmp8B2;struct Cyc_Arg_String_spec_struct _tmp8B1;struct Cyc_Arg_String_spec_struct*
_tmp8B0;const char*_tmp8AF;struct _tuple21*_tmp8AE;const char*_tmp8AD;const char*
_tmp8AC;struct Cyc_Arg_Set_spec_struct _tmp8AB;struct Cyc_Arg_Set_spec_struct*
_tmp8AA;const char*_tmp8A9;struct _tuple21*_tmp8A8;const char*_tmp8A7;const char*
_tmp8A6;struct Cyc_Arg_Set_spec_struct _tmp8A5;struct Cyc_Arg_Set_spec_struct*
_tmp8A4;const char*_tmp8A3;struct _tuple21*_tmp8A2;const char*_tmp8A1;const char*
_tmp8A0;struct Cyc_Arg_Set_spec_struct _tmp89F;struct Cyc_Arg_Set_spec_struct*
_tmp89E;const char*_tmp89D;struct _tuple21*_tmp89C;const char*_tmp89B;const char*
_tmp89A;struct Cyc_Arg_Clear_spec_struct _tmp899;struct Cyc_Arg_Clear_spec_struct*
_tmp898;const char*_tmp897;struct _tuple21*_tmp896;const char*_tmp895;const char*
_tmp894;struct Cyc_Arg_Unit_spec_struct _tmp893;struct Cyc_Arg_Unit_spec_struct*
_tmp892;const char*_tmp891;struct _tuple21*_tmp890;const char*_tmp88F;const char*
_tmp88E;struct Cyc_Arg_Set_spec_struct _tmp88D;struct Cyc_Arg_Set_spec_struct*
_tmp88C;const char*_tmp88B;struct _tuple21*_tmp88A;const char*_tmp889;const char*
_tmp888;struct Cyc_Arg_Flag_spec_struct _tmp887;struct Cyc_Arg_Flag_spec_struct*
_tmp886;const char*_tmp885;struct _tuple21*_tmp884;const char*_tmp883;const char*
_tmp882;struct Cyc_Arg_Flag_spec_struct _tmp881;struct Cyc_Arg_Flag_spec_struct*
_tmp880;const char*_tmp87F;struct _tuple21*_tmp87E;const char*_tmp87D;const char*
_tmp87C;struct Cyc_Arg_Flag_spec_struct _tmp87B;struct Cyc_Arg_Flag_spec_struct*
_tmp87A;const char*_tmp879;struct _tuple21*_tmp878;const char*_tmp877;const char*
_tmp876;struct Cyc_Arg_Flag_spec_struct _tmp875;struct Cyc_Arg_Flag_spec_struct*
_tmp874;const char*_tmp873;struct _tuple21*_tmp872;const char*_tmp871;const char*
_tmp870;struct Cyc_Arg_Flag_spec_struct _tmp86F;struct Cyc_Arg_Flag_spec_struct*
_tmp86E;const char*_tmp86D;struct _tuple21*_tmp86C[72];struct Cyc_List_List*options=(
_tmp86C[71]=((_tmp872=_region_malloc(r,sizeof(*_tmp872)),((_tmp872->f1=((_tmp871="-idirafter",
_tag_dyneither(_tmp871,sizeof(char),11))),((_tmp872->f2=1,((_tmp872->f3=((
_tmp870="<dir>",_tag_dyneither(_tmp870,sizeof(char),6))),((_tmp872->f4=(void*)((
_tmp86E=_region_malloc(r,sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=1,((
_tmp86F.f1=Cyc_add_idirafter,_tmp86F)))),_tmp86E)))),((_tmp872->f5=((_tmp86D="Add the directory to the second include path.",
_tag_dyneither(_tmp86D,sizeof(char),46))),_tmp872)))))))))))),((_tmp86C[70]=((
_tmp878=_region_malloc(r,sizeof(*_tmp878)),((_tmp878->f1=((_tmp877="-isystem",
_tag_dyneither(_tmp877,sizeof(char),9))),((_tmp878->f2=1,((_tmp878->f3=((_tmp876="<dir>",
_tag_dyneither(_tmp876,sizeof(char),6))),((_tmp878->f4=(void*)((_tmp874=
_region_malloc(r,sizeof(*_tmp874)),((_tmp874[0]=((_tmp875.tag=1,((_tmp875.f1=Cyc_add_isystem,
_tmp875)))),_tmp874)))),((_tmp878->f5=((_tmp873="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",
_tag_dyneither(_tmp873,sizeof(char),90))),_tmp878)))))))))))),((_tmp86C[69]=((
_tmp87E=_region_malloc(r,sizeof(*_tmp87E)),((_tmp87E->f1=((_tmp87D="-iwithprefixbefore",
_tag_dyneither(_tmp87D,sizeof(char),19))),((_tmp87E->f2=1,((_tmp87E->f3=((
_tmp87C="<dir>",_tag_dyneither(_tmp87C,sizeof(char),6))),((_tmp87E->f4=(void*)((
_tmp87A=_region_malloc(r,sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87B.tag=1,((
_tmp87B.f1=Cyc_add_iwithprefixbefore,_tmp87B)))),_tmp87A)))),((_tmp87E->f5=((
_tmp879="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp879,
sizeof(char),45))),_tmp87E)))))))))))),((_tmp86C[68]=((_tmp884=_region_malloc(r,
sizeof(*_tmp884)),((_tmp884->f1=((_tmp883="-iwithprefix",_tag_dyneither(_tmp883,
sizeof(char),13))),((_tmp884->f2=1,((_tmp884->f3=((_tmp882="<dir>",
_tag_dyneither(_tmp882,sizeof(char),6))),((_tmp884->f4=(void*)((_tmp880=
_region_malloc(r,sizeof(*_tmp880)),((_tmp880[0]=((_tmp881.tag=1,((_tmp881.f1=Cyc_add_iwithprefix,
_tmp881)))),_tmp880)))),((_tmp884->f5=((_tmp87F="Add <prefix>/<dir> to the second include path.",
_tag_dyneither(_tmp87F,sizeof(char),47))),_tmp884)))))))))))),((_tmp86C[67]=((
_tmp88A=_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A->f1=((_tmp889="-iprefix",
_tag_dyneither(_tmp889,sizeof(char),9))),((_tmp88A->f2=1,((_tmp88A->f3=((_tmp888="<prefix>",
_tag_dyneither(_tmp888,sizeof(char),9))),((_tmp88A->f4=(void*)((_tmp886=
_region_malloc(r,sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=1,((_tmp887.f1=Cyc_add_iprefix,
_tmp887)))),_tmp886)))),((_tmp88A->f5=((_tmp885="Specify <prefix> as the prefix for subsequent -iwithprefix options",
_tag_dyneither(_tmp885,sizeof(char),67))),_tmp88A)))))))))))),((_tmp86C[66]=((
_tmp890=_region_malloc(r,sizeof(*_tmp890)),((_tmp890->f1=((_tmp88F="-noregions",
_tag_dyneither(_tmp88F,sizeof(char),11))),((_tmp890->f2=0,((_tmp890->f3=((
_tmp88E="",_tag_dyneither(_tmp88E,sizeof(char),1))),((_tmp890->f4=(void*)((
_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88D.tag=3,((
_tmp88D.f1=& Cyc_Absyn_no_regions,_tmp88D)))),_tmp88C)))),((_tmp890->f5=((_tmp88B="Generate code that doesn't use regions",
_tag_dyneither(_tmp88B,sizeof(char),39))),_tmp890)))))))))))),((_tmp86C[65]=((
_tmp896=_region_malloc(r,sizeof(*_tmp896)),((_tmp896->f1=((_tmp895="-port",
_tag_dyneither(_tmp895,sizeof(char),6))),((_tmp896->f2=0,((_tmp896->f3=((_tmp894="",
_tag_dyneither(_tmp894,sizeof(char),1))),((_tmp896->f4=(void*)((_tmp892=
_region_malloc(r,sizeof(*_tmp892)),((_tmp892[0]=((_tmp893.tag=0,((_tmp893.f1=Cyc_set_port_c_code,
_tmp893)))),_tmp892)))),((_tmp896->f5=((_tmp891="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp891,sizeof(char),38))),_tmp896)))))))))))),((_tmp86C[64]=((
_tmp89C=_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C->f1=((_tmp89B="-detailedlocation",
_tag_dyneither(_tmp89B,sizeof(char),18))),((_tmp89C->f2=0,((_tmp89C->f3=((
_tmp89A="",_tag_dyneither(_tmp89A,sizeof(char),1))),((_tmp89C->f4=(void*)((
_tmp898=_region_malloc(r,sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=4,((
_tmp899.f1=& Cyc_Position_use_gcc_style_location,_tmp899)))),_tmp898)))),((
_tmp89C->f5=((_tmp897="Try to give more detailed location information for errors",
_tag_dyneither(_tmp897,sizeof(char),58))),_tmp89C)))))))))))),((_tmp86C[63]=((
_tmp8A2=_region_malloc(r,sizeof(*_tmp8A2)),((_tmp8A2->f1=((_tmp8A1="-noregister",
_tag_dyneither(_tmp8A1,sizeof(char),12))),((_tmp8A2->f2=0,((_tmp8A2->f3=((
_tmp8A0="",_tag_dyneither(_tmp8A0,sizeof(char),1))),((_tmp8A2->f4=(void*)((
_tmp89E=_region_malloc(r,sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=3,((
_tmp89F.f1=& Cyc_Parse_no_register,_tmp89F)))),_tmp89E)))),((_tmp8A2->f5=((
_tmp89D="Treat register storage class as public",_tag_dyneither(_tmp89D,sizeof(
char),39))),_tmp8A2)))))))))))),((_tmp86C[62]=((_tmp8A8=_region_malloc(r,sizeof(*
_tmp8A8)),((_tmp8A8->f1=((_tmp8A7="-warnnullchecks",_tag_dyneither(_tmp8A7,
sizeof(char),16))),((_tmp8A8->f2=0,((_tmp8A8->f3=((_tmp8A6="",_tag_dyneither(
_tmp8A6,sizeof(char),1))),((_tmp8A8->f4=(void*)((_tmp8A4=_region_malloc(r,
sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A5.tag=3,((_tmp8A5.f1=& Cyc_Toc_warn_all_null_deref,
_tmp8A5)))),_tmp8A4)))),((_tmp8A8->f5=((_tmp8A3="Warn when any null check can't be eliminated",
_tag_dyneither(_tmp8A3,sizeof(char),45))),_tmp8A8)))))))))))),((_tmp86C[61]=((
_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE->f1=((_tmp8AD="-warnboundschecks",
_tag_dyneither(_tmp8AD,sizeof(char),18))),((_tmp8AE->f2=0,((_tmp8AE->f3=((
_tmp8AC="",_tag_dyneither(_tmp8AC,sizeof(char),1))),((_tmp8AE->f4=(void*)((
_tmp8AA=_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=3,((
_tmp8AB.f1=& Cyc_Toc_warn_bounds_checks,_tmp8AB)))),_tmp8AA)))),((_tmp8AE->f5=((
_tmp8A9="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp8A9,
sizeof(char),44))),_tmp8AE)))))))))))),((_tmp86C[60]=((_tmp8B4=_region_malloc(r,
sizeof(*_tmp8B4)),((_tmp8B4->f1=((_tmp8B3="-CI",_tag_dyneither(_tmp8B3,sizeof(
char),4))),((_tmp8B4->f2=0,((_tmp8B4->f3=((_tmp8B2=" <file>",_tag_dyneither(
_tmp8B2,sizeof(char),8))),((_tmp8B4->f4=(void*)((_tmp8B0=_region_malloc(r,
sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=5,((_tmp8B1.f1=Cyc_set_cyc_include,
_tmp8B1)))),_tmp8B0)))),((_tmp8B4->f5=((_tmp8AF="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp8AF,sizeof(char),31))),_tmp8B4)))))))))))),((_tmp86C[59]=((
_tmp8BA=_region_malloc(r,sizeof(*_tmp8BA)),((_tmp8BA->f1=((_tmp8B9="-compile-for-boot",
_tag_dyneither(_tmp8B9,sizeof(char),18))),((_tmp8BA->f2=0,((_tmp8BA->f3=((
_tmp8B8="",_tag_dyneither(_tmp8B8,sizeof(char),1))),((_tmp8BA->f4=(void*)((
_tmp8B6=_region_malloc(r,sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B7.tag=3,((
_tmp8B7.f1=& Cyc_compile_for_boot_r,_tmp8B7)))),_tmp8B6)))),((_tmp8BA->f5=((
_tmp8B5="Compile using the special boot library instead of the standard library",
_tag_dyneither(_tmp8B5,sizeof(char),71))),_tmp8BA)))))))))))),((_tmp86C[58]=((
_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0->f1=((_tmp8BF="-nocyc_setjmp",
_tag_dyneither(_tmp8BF,sizeof(char),14))),((_tmp8C0->f2=0,((_tmp8C0->f3=((
_tmp8BE="",_tag_dyneither(_tmp8BE,sizeof(char),1))),((_tmp8C0->f4=(void*)((
_tmp8BC=_region_malloc(r,sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=3,((
_tmp8BD.f1=& Cyc_nocyc_setjmp_r,_tmp8BD)))),_tmp8BC)))),((_tmp8C0->f5=((_tmp8BB="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp8BB,sizeof(char),46))),_tmp8C0)))))))))))),((_tmp86C[57]=((
_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6->f1=((_tmp8C5="-printalleffects",
_tag_dyneither(_tmp8C5,sizeof(char),17))),((_tmp8C6->f2=0,((_tmp8C6->f3=((
_tmp8C4="",_tag_dyneither(_tmp8C4,sizeof(char),1))),((_tmp8C6->f4=(void*)((
_tmp8C2=_region_malloc(r,sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=3,((
_tmp8C3.f1=& Cyc_print_all_effects_r,_tmp8C3)))),_tmp8C2)))),((_tmp8C6->f5=((
_tmp8C1="Print effects for functions (type debugging)",_tag_dyneither(_tmp8C1,
sizeof(char),45))),_tmp8C6)))))))))))),((_tmp86C[56]=((_tmp8CC=_region_malloc(r,
sizeof(*_tmp8CC)),((_tmp8CC->f1=((_tmp8CB="-printfullevars",_tag_dyneither(
_tmp8CB,sizeof(char),16))),((_tmp8CC->f2=0,((_tmp8CC->f3=((_tmp8CA="",
_tag_dyneither(_tmp8CA,sizeof(char),1))),((_tmp8CC->f4=(void*)((_tmp8C8=
_region_malloc(r,sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=3,((_tmp8C9.f1=&
Cyc_print_full_evars_r,_tmp8C9)))),_tmp8C8)))),((_tmp8CC->f5=((_tmp8C7="Print full information for evars (type debugging)",
_tag_dyneither(_tmp8C7,sizeof(char),50))),_tmp8CC)))))))))))),((_tmp86C[55]=((
_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2->f1=((_tmp8D1="-printallkinds",
_tag_dyneither(_tmp8D1,sizeof(char),15))),((_tmp8D2->f2=0,((_tmp8D2->f3=((
_tmp8D0="",_tag_dyneither(_tmp8D0,sizeof(char),1))),((_tmp8D2->f4=(void*)((
_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CF.tag=3,((
_tmp8CF.f1=& Cyc_print_all_kinds_r,_tmp8CF)))),_tmp8CE)))),((_tmp8D2->f5=((
_tmp8CD="Always print kinds of type variables",_tag_dyneither(_tmp8CD,sizeof(
char),37))),_tmp8D2)))))))))))),((_tmp86C[54]=((_tmp8D8=_region_malloc(r,sizeof(*
_tmp8D8)),((_tmp8D8->f1=((_tmp8D7="-printalltvars",_tag_dyneither(_tmp8D7,
sizeof(char),15))),((_tmp8D8->f2=0,((_tmp8D8->f3=((_tmp8D6="",_tag_dyneither(
_tmp8D6,sizeof(char),1))),((_tmp8D8->f4=(void*)((_tmp8D4=_region_malloc(r,
sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D5.tag=3,((_tmp8D5.f1=& Cyc_print_all_tvars_r,
_tmp8D5)))),_tmp8D4)))),((_tmp8D8->f5=((_tmp8D3="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp8D3,sizeof(char),57))),_tmp8D8)))))))))))),((_tmp86C[53]=((
_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE->f1=((_tmp8DD="-noexpandtypedefs",
_tag_dyneither(_tmp8DD,sizeof(char),18))),((_tmp8DE->f2=0,((_tmp8DE->f3=((
_tmp8DC="",_tag_dyneither(_tmp8DC,sizeof(char),1))),((_tmp8DE->f4=(void*)((
_tmp8DA=_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=3,((
_tmp8DB.f1=& Cyc_noexpand_r,_tmp8DB)))),_tmp8DA)))),((_tmp8DE->f5=((_tmp8D9="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp8D9,sizeof(char),41))),_tmp8DE)))))))))))),((_tmp86C[52]=((
_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4->f1=((_tmp8E3="-noremoveunused",
_tag_dyneither(_tmp8E3,sizeof(char),16))),((_tmp8E4->f2=0,((_tmp8E4->f3=((
_tmp8E2="",_tag_dyneither(_tmp8E2,sizeof(char),1))),((_tmp8E4->f4=(void*)((
_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=3,((
_tmp8E1.f1=& Cyc_noshake_r,_tmp8E1)))),_tmp8E0)))),((_tmp8E4->f5=((_tmp8DF="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp8DF,sizeof(char),49))),_tmp8E4)))))))))))),((_tmp86C[51]=((
_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA->f1=((_tmp8E9="-nogc",
_tag_dyneither(_tmp8E9,sizeof(char),6))),((_tmp8EA->f2=0,((_tmp8EA->f3=((_tmp8E8="",
_tag_dyneither(_tmp8E8,sizeof(char),1))),((_tmp8EA->f4=(void*)((_tmp8E6=
_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=3,((_tmp8E7.f1=&
Cyc_nogc_r,_tmp8E7)))),_tmp8E6)))),((_tmp8EA->f5=((_tmp8E5="Don't link in the garbage collector",
_tag_dyneither(_tmp8E5,sizeof(char),36))),_tmp8EA)))))))))))),((_tmp86C[50]=((
_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0->f1=((_tmp8EF="-nocyc",
_tag_dyneither(_tmp8EF,sizeof(char),7))),((_tmp8F0->f2=0,((_tmp8F0->f3=((_tmp8EE="",
_tag_dyneither(_tmp8EE,sizeof(char),1))),((_tmp8F0->f4=(void*)((_tmp8EC=
_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=0,((_tmp8ED.f1=Cyc_set_nocyc,
_tmp8ED)))),_tmp8EC)))),((_tmp8F0->f5=((_tmp8EB="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp8EB,sizeof(char),33))),_tmp8F0)))))))))))),((_tmp86C[49]=((
_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6->f1=((_tmp8F5="-no-cpp-precomp",
_tag_dyneither(_tmp8F5,sizeof(char),16))),((_tmp8F6->f2=0,((_tmp8F6->f3=((
_tmp8F4="",_tag_dyneither(_tmp8F4,sizeof(char),1))),((_tmp8F6->f4=(void*)((
_tmp8F2=_region_malloc(r,sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=0,((
_tmp8F3.f1=Cyc_set_nocppprecomp,_tmp8F3)))),_tmp8F2)))),((_tmp8F6->f5=((_tmp8F1="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp8F1,sizeof(char),40))),_tmp8F6)))))))))))),((_tmp86C[48]=((
_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC->f1=((_tmp8FB="-use-cpp",
_tag_dyneither(_tmp8FB,sizeof(char),9))),((_tmp8FC->f2=0,((_tmp8FC->f3=((_tmp8FA="<path>",
_tag_dyneither(_tmp8FA,sizeof(char),7))),((_tmp8FC->f4=(void*)((_tmp8F8=
_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=5,((_tmp8F9.f1=Cyc_set_cpp,
_tmp8F9)))),_tmp8F8)))),((_tmp8FC->f5=((_tmp8F7="Indicate which preprocessor to use",
_tag_dyneither(_tmp8F7,sizeof(char),35))),_tmp8FC)))))))))))),((_tmp86C[47]=((
_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902->f1=((_tmp901="--inline-checks",
_tag_dyneither(_tmp901,sizeof(char),16))),((_tmp902->f2=0,((_tmp902->f3=((
_tmp900="",_tag_dyneither(_tmp900,sizeof(char),1))),((_tmp902->f4=(void*)((
_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=0,((
_tmp8FF.f1=Cyc_set_inline_functions,_tmp8FF)))),_tmp8FE)))),((_tmp902->f5=((
_tmp8FD="Inline bounds checks instead of #define",_tag_dyneither(_tmp8FD,sizeof(
char),40))),_tmp902)))))))))))),((_tmp86C[46]=((_tmp908=_region_malloc(r,sizeof(*
_tmp908)),((_tmp908->f1=((_tmp907="--noboundschecks",_tag_dyneither(_tmp907,
sizeof(char),17))),((_tmp908->f2=0,((_tmp908->f3=((_tmp906="",_tag_dyneither(
_tmp906,sizeof(char),1))),((_tmp908->f4=(void*)((_tmp904=_region_malloc(r,
sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=0,((_tmp905.f1=Cyc_set_noboundschecks,
_tmp905)))),_tmp904)))),((_tmp908->f5=((_tmp903="Disable bounds checks",
_tag_dyneither(_tmp903,sizeof(char),22))),_tmp908)))))))))))),((_tmp86C[45]=((
_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E->f1=((_tmp90D="--nonullchecks",
_tag_dyneither(_tmp90D,sizeof(char),15))),((_tmp90E->f2=0,((_tmp90E->f3=((
_tmp90C="",_tag_dyneither(_tmp90C,sizeof(char),1))),((_tmp90E->f4=(void*)((
_tmp90A=_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=0,((
_tmp90B.f1=Cyc_set_nonullchecks,_tmp90B)))),_tmp90A)))),((_tmp90E->f5=((_tmp909="Disable null checks",
_tag_dyneither(_tmp909,sizeof(char),20))),_tmp90E)))))))))))),((_tmp86C[44]=((
_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914->f1=((_tmp913="--nochecks",
_tag_dyneither(_tmp913,sizeof(char),11))),((_tmp914->f2=0,((_tmp914->f3=((
_tmp912="",_tag_dyneither(_tmp912,sizeof(char),1))),((_tmp914->f4=(void*)((
_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=0,((
_tmp911.f1=Cyc_set_nochecks,_tmp911)))),_tmp910)))),((_tmp914->f5=((_tmp90F="Disable bounds/null checks",
_tag_dyneither(_tmp90F,sizeof(char),27))),_tmp914)))))))))))),((_tmp86C[43]=((
_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A->f1=((_tmp919="--lineno",
_tag_dyneither(_tmp919,sizeof(char),9))),((_tmp91A->f2=0,((_tmp91A->f3=((_tmp918="",
_tag_dyneither(_tmp918,sizeof(char),1))),((_tmp91A->f4=(void*)((_tmp916=
_region_malloc(r,sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=0,((_tmp917.f1=Cyc_set_lineno,
_tmp917)))),_tmp916)))),((_tmp91A->f5=((_tmp915="Generate line numbers for debugging",
_tag_dyneither(_tmp915,sizeof(char),36))),_tmp91A)))))))))))),((_tmp86C[42]=((
_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920->f1=((_tmp91F="-save-c",
_tag_dyneither(_tmp91F,sizeof(char),8))),((_tmp920->f2=0,((_tmp920->f3=((_tmp91E="",
_tag_dyneither(_tmp91E,sizeof(char),1))),((_tmp920->f4=(void*)((_tmp91C=
_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=3,((_tmp91D.f1=&
Cyc_save_c_r,_tmp91D)))),_tmp91C)))),((_tmp920->f5=((_tmp91B="Don't delete temporary C files",
_tag_dyneither(_tmp91B,sizeof(char),31))),_tmp920)))))))))))),((_tmp86C[41]=((
_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926->f1=((_tmp925="-save-temps",
_tag_dyneither(_tmp925,sizeof(char),12))),((_tmp926->f2=0,((_tmp926->f3=((
_tmp924="",_tag_dyneither(_tmp924,sizeof(char),1))),((_tmp926->f4=(void*)((
_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922[0]=((_tmp923.tag=0,((
_tmp923.f1=Cyc_set_save_temps,_tmp923)))),_tmp922)))),((_tmp926->f5=((_tmp921="Don't delete temporary files",
_tag_dyneither(_tmp921,sizeof(char),29))),_tmp926)))))))))))),((_tmp86C[40]=((
_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C->f1=((_tmp92B="-c-comp",
_tag_dyneither(_tmp92B,sizeof(char),8))),((_tmp92C->f2=0,((_tmp92C->f3=((_tmp92A=" <compiler>",
_tag_dyneither(_tmp92A,sizeof(char),12))),((_tmp92C->f4=(void*)((_tmp928=
_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=((_tmp929.tag=5,((_tmp929.f1=Cyc_set_c_compiler,
_tmp929)))),_tmp928)))),((_tmp92C->f5=((_tmp927="Produce C output for the given compiler",
_tag_dyneither(_tmp927,sizeof(char),40))),_tmp92C)))))))))))),((_tmp86C[39]=((
_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932->f1=((_tmp931="-un-gcc",
_tag_dyneither(_tmp931,sizeof(char),8))),((_tmp932->f2=0,((_tmp932->f3=((_tmp930="",
_tag_dyneither(_tmp930,sizeof(char),1))),((_tmp932->f4=(void*)((_tmp92E=
_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=0,((_tmp92F.f1=Cyc_set_tovc,
_tmp92F)))),_tmp92E)))),((_tmp932->f5=((_tmp92D="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp92D,sizeof(char),57))),_tmp932)))))))))))),((_tmp86C[38]=((
_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938->f1=((_tmp937="-no-elim-statement-expressions",
_tag_dyneither(_tmp937,sizeof(char),31))),((_tmp938->f2=0,((_tmp938->f3=((
_tmp936="",_tag_dyneither(_tmp936,sizeof(char),1))),((_tmp938->f4=(void*)((
_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=4,((
_tmp935.f1=& Cyc_elim_se_r,_tmp935)))),_tmp934)))),((_tmp938->f5=((_tmp933="Do not avoid statement expressions in C output",
_tag_dyneither(_tmp933,sizeof(char),47))),_tmp938)))))))))))),((_tmp86C[37]=((
_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E->f1=((_tmp93D="-elim-statement-expressions",
_tag_dyneither(_tmp93D,sizeof(char),28))),((_tmp93E->f2=0,((_tmp93E->f3=((
_tmp93C="",_tag_dyneither(_tmp93C,sizeof(char),1))),((_tmp93E->f4=(void*)((
_tmp93A=_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp93B.tag=3,((
_tmp93B.f1=& Cyc_elim_se_r,_tmp93B)))),_tmp93A)))),((_tmp93E->f5=((_tmp939="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp939,sizeof(char),66))),_tmp93E)))))))))))),((_tmp86C[36]=((
_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944->f1=((_tmp943="-up",
_tag_dyneither(_tmp943,sizeof(char),4))),((_tmp944->f2=0,((_tmp944->f3=((_tmp942="",
_tag_dyneither(_tmp942,sizeof(char),1))),((_tmp944->f4=(void*)((_tmp940=
_region_malloc(r,sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=4,((_tmp941.f1=&
Cyc_pp_r,_tmp941)))),_tmp940)))),((_tmp944->f5=((_tmp93F="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp93F,sizeof(char),55))),_tmp944)))))))))))),((_tmp86C[35]=((
_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A->f1=((_tmp949="-pp",
_tag_dyneither(_tmp949,sizeof(char),4))),((_tmp94A->f2=0,((_tmp94A->f3=((_tmp948="",
_tag_dyneither(_tmp948,sizeof(char),1))),((_tmp94A->f4=(void*)((_tmp946=
_region_malloc(r,sizeof(*_tmp946)),((_tmp946[0]=((_tmp947.tag=3,((_tmp947.f1=&
Cyc_pp_r,_tmp947)))),_tmp946)))),((_tmp94A->f5=((_tmp945="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp945,sizeof(char),47))),_tmp94A)))))))))))),((_tmp86C[34]=((
_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950->f1=((_tmp94F="-ic",
_tag_dyneither(_tmp94F,sizeof(char),4))),((_tmp950->f2=0,((_tmp950->f3=((_tmp94E="",
_tag_dyneither(_tmp94E,sizeof(char),1))),((_tmp950->f4=(void*)((_tmp94C=
_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94D.tag=3,((_tmp94D.f1=&
Cyc_ic_r,_tmp94D)))),_tmp94C)))),((_tmp950->f5=((_tmp94B="Activate the link-checker",
_tag_dyneither(_tmp94B,sizeof(char),26))),_tmp950)))))))))))),((_tmp86C[33]=((
_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956->f1=((_tmp955="-stopafter-toc",
_tag_dyneither(_tmp955,sizeof(char),15))),((_tmp956->f2=0,((_tmp956->f3=((
_tmp954="",_tag_dyneither(_tmp954,sizeof(char),1))),((_tmp956->f4=(void*)((
_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=3,((
_tmp953.f1=& Cyc_toc_r,_tmp953)))),_tmp952)))),((_tmp956->f5=((_tmp951="Stop after translation to C",
_tag_dyneither(_tmp951,sizeof(char),28))),_tmp956)))))))))))),((_tmp86C[32]=((
_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C->f1=((_tmp95B="-stopafter-cf",
_tag_dyneither(_tmp95B,sizeof(char),14))),((_tmp95C->f2=0,((_tmp95C->f3=((
_tmp95A="",_tag_dyneither(_tmp95A,sizeof(char),1))),((_tmp95C->f4=(void*)((
_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=((_tmp959.tag=3,((
_tmp959.f1=& Cyc_cf_r,_tmp959)))),_tmp958)))),((_tmp95C->f5=((_tmp957="Stop after control-flow checking",
_tag_dyneither(_tmp957,sizeof(char),33))),_tmp95C)))))))))))),((_tmp86C[31]=((
_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962->f1=((_tmp961="-noprint",
_tag_dyneither(_tmp961,sizeof(char),9))),((_tmp962->f2=0,((_tmp962->f3=((_tmp960="",
_tag_dyneither(_tmp960,sizeof(char),1))),((_tmp962->f4=(void*)((_tmp95E=
_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=3,((_tmp95F.f1=&
Cyc_noprint_r,_tmp95F)))),_tmp95E)))),((_tmp962->f5=((_tmp95D="Do not print output (when stopping early)",
_tag_dyneither(_tmp95D,sizeof(char),42))),_tmp962)))))))))))),((_tmp86C[30]=((
_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968->f1=((_tmp967="-stopafter-tc",
_tag_dyneither(_tmp967,sizeof(char),14))),((_tmp968->f2=0,((_tmp968->f3=((
_tmp966="",_tag_dyneither(_tmp966,sizeof(char),1))),((_tmp968->f4=(void*)((
_tmp964=_region_malloc(r,sizeof(*_tmp964)),((_tmp964[0]=((_tmp965.tag=3,((
_tmp965.f1=& Cyc_tc_r,_tmp965)))),_tmp964)))),((_tmp968->f5=((_tmp963="Stop after type checking",
_tag_dyneither(_tmp963,sizeof(char),25))),_tmp968)))))))))))),((_tmp86C[29]=((
_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E->f1=((_tmp96D="-stopafter-parse",
_tag_dyneither(_tmp96D,sizeof(char),17))),((_tmp96E->f2=0,((_tmp96E->f3=((
_tmp96C="",_tag_dyneither(_tmp96C,sizeof(char),1))),((_tmp96E->f4=(void*)((
_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96B.tag=3,((
_tmp96B.f1=& Cyc_parseonly_r,_tmp96B)))),_tmp96A)))),((_tmp96E->f5=((_tmp969="Stop after parsing",
_tag_dyneither(_tmp969,sizeof(char),19))),_tmp96E)))))))))))),((_tmp86C[28]=((
_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974->f1=((_tmp973="-E",
_tag_dyneither(_tmp973,sizeof(char),3))),((_tmp974->f2=0,((_tmp974->f3=((_tmp972="",
_tag_dyneither(_tmp972,sizeof(char),1))),((_tmp974->f4=(void*)((_tmp970=
_region_malloc(r,sizeof(*_tmp970)),((_tmp970[0]=((_tmp971.tag=3,((_tmp971.f1=&
Cyc_stop_after_cpp_r,_tmp971)))),_tmp970)))),((_tmp974->f5=((_tmp96F="Stop after preprocessing",
_tag_dyneither(_tmp96F,sizeof(char),25))),_tmp974)))))))))))),((_tmp86C[27]=((
_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A->f1=((_tmp979="-Wall",
_tag_dyneither(_tmp979,sizeof(char),6))),((_tmp97A->f2=0,((_tmp97A->f3=((_tmp978="",
_tag_dyneither(_tmp978,sizeof(char),1))),((_tmp97A->f4=(void*)((_tmp976=
_region_malloc(r,sizeof(*_tmp976)),((_tmp976[0]=((_tmp977.tag=0,((_tmp977.f1=Cyc_set_all_warnings,
_tmp977)))),_tmp976)))),((_tmp97A->f5=((_tmp975="Turn on all warnings",
_tag_dyneither(_tmp975,sizeof(char),21))),_tmp97A)))))))))))),((_tmp86C[26]=((
_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980->f1=((_tmp97F="-Wlose-unique",
_tag_dyneither(_tmp97F,sizeof(char),14))),((_tmp980->f2=0,((_tmp980->f3=((
_tmp97E="",_tag_dyneither(_tmp97E,sizeof(char),1))),((_tmp980->f4=(void*)((
_tmp97C=_region_malloc(r,sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=3,((
_tmp97D.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp97D)))),_tmp97C)))),((
_tmp980->f5=((_tmp97B="Try to warn when a unique pointer might get lost",
_tag_dyneither(_tmp97B,sizeof(char),49))),_tmp980)))))))))))),((_tmp86C[25]=((
_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986->f1=((_tmp985="-b",
_tag_dyneither(_tmp985,sizeof(char),3))),((_tmp986->f2=0,((_tmp986->f3=((_tmp984="<arch>",
_tag_dyneither(_tmp984,sizeof(char),7))),((_tmp986->f4=(void*)((_tmp982=
_region_malloc(r,sizeof(*_tmp982)),((_tmp982[0]=((_tmp983.tag=5,((_tmp983.f1=Cyc_set_target_arch,
_tmp983)))),_tmp982)))),((_tmp986->f5=((_tmp981="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp981,sizeof(char),48))),_tmp986)))))))))))),((_tmp86C[24]=((
_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C->f1=((_tmp98B="-MT",
_tag_dyneither(_tmp98B,sizeof(char),4))),((_tmp98C->f2=0,((_tmp98C->f3=((_tmp98A=" <target>",
_tag_dyneither(_tmp98A,sizeof(char),10))),((_tmp98C->f4=(void*)((_tmp988=
_region_malloc(r,sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=5,((_tmp989.f1=Cyc_set_dependencies_target,
_tmp989)))),_tmp988)))),((_tmp98C->f5=((_tmp987="Give target for dependencies",
_tag_dyneither(_tmp987,sizeof(char),29))),_tmp98C)))))))))))),((_tmp86C[23]=((
_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992->f1=((_tmp991="-MG",
_tag_dyneither(_tmp991,sizeof(char),4))),((_tmp992->f2=0,((_tmp992->f3=((_tmp990="",
_tag_dyneither(_tmp990,sizeof(char),1))),((_tmp992->f4=(void*)((_tmp98E=
_region_malloc(r,sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=1,((_tmp98F.f1=Cyc_add_cpparg,
_tmp98F)))),_tmp98E)))),((_tmp992->f5=((_tmp98D="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp98D,sizeof(char),68))),_tmp992)))))))))))),((_tmp86C[22]=((
_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998->f1=((_tmp997="-M",
_tag_dyneither(_tmp997,sizeof(char),3))),((_tmp998->f2=0,((_tmp998->f3=((_tmp996="",
_tag_dyneither(_tmp996,sizeof(char),1))),((_tmp998->f4=(void*)((_tmp994=
_region_malloc(r,sizeof(*_tmp994)),((_tmp994[0]=((_tmp995.tag=0,((_tmp995.f1=Cyc_set_produce_dependencies,
_tmp995)))),_tmp994)))),((_tmp998->f5=((_tmp993="Produce dependencies",
_tag_dyneither(_tmp993,sizeof(char),21))),_tmp998)))))))))))),((_tmp86C[21]=((
_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E->f1=((_tmp99D="-S",
_tag_dyneither(_tmp99D,sizeof(char),3))),((_tmp99E->f2=0,((_tmp99E->f3=((_tmp99C="",
_tag_dyneither(_tmp99C,sizeof(char),1))),((_tmp99E->f4=(void*)((_tmp99A=
_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=0,((_tmp99B.f1=Cyc_set_stop_after_asmfile,
_tmp99B)))),_tmp99A)))),((_tmp99E->f5=((_tmp999="Stop after producing assembly code",
_tag_dyneither(_tmp999,sizeof(char),35))),_tmp99E)))))))))))),((_tmp86C[20]=((
_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4->f1=((_tmp9A3="-pa",
_tag_dyneither(_tmp9A3,sizeof(char),4))),((_tmp9A4->f2=0,((_tmp9A4->f3=((_tmp9A2="",
_tag_dyneither(_tmp9A2,sizeof(char),1))),((_tmp9A4->f4=(void*)((_tmp9A0=
_region_malloc(r,sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=0,((_tmp9A1.f1=Cyc_set_pa,
_tmp9A1)))),_tmp9A0)))),((_tmp9A4->f5=((_tmp99F="Compile for profiling with aprof",
_tag_dyneither(_tmp99F,sizeof(char),33))),_tmp9A4)))))))))))),((_tmp86C[19]=((
_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA->f1=((_tmp9A9="-pg",
_tag_dyneither(_tmp9A9,sizeof(char),4))),((_tmp9AA->f2=0,((_tmp9AA->f3=((_tmp9A8="",
_tag_dyneither(_tmp9A8,sizeof(char),1))),((_tmp9AA->f4=(void*)((_tmp9A6=
_region_malloc(r,sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A7.tag=0,((_tmp9A7.f1=Cyc_set_pg,
_tmp9A7)))),_tmp9A6)))),((_tmp9AA->f5=((_tmp9A5="Compile for profiling with gprof",
_tag_dyneither(_tmp9A5,sizeof(char),33))),_tmp9AA)))))))))))),((_tmp86C[18]=((
_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0->f1=((_tmp9AF="-p",
_tag_dyneither(_tmp9AF,sizeof(char),3))),((_tmp9B0->f2=0,((_tmp9B0->f3=((_tmp9AE="",
_tag_dyneither(_tmp9AE,sizeof(char),1))),((_tmp9B0->f4=(void*)((_tmp9AC=
_region_malloc(r,sizeof(*_tmp9AC)),((_tmp9AC[0]=((_tmp9AD.tag=1,((_tmp9AD.f1=Cyc_add_ccarg,
_tmp9AD)))),_tmp9AC)))),((_tmp9B0->f5=((_tmp9AB="Compile for profiling with prof",
_tag_dyneither(_tmp9AB,sizeof(char),32))),_tmp9B0)))))))))))),((_tmp86C[17]=((
_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6->f1=((_tmp9B5="-g",
_tag_dyneither(_tmp9B5,sizeof(char),3))),((_tmp9B6->f2=0,((_tmp9B6->f3=((_tmp9B4="",
_tag_dyneither(_tmp9B4,sizeof(char),1))),((_tmp9B6->f4=(void*)((_tmp9B2=
_region_malloc(r,sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B3.tag=1,((_tmp9B3.f1=Cyc_add_ccarg,
_tmp9B3)))),_tmp9B2)))),((_tmp9B6->f5=((_tmp9B1="Compile for debugging",
_tag_dyneither(_tmp9B1,sizeof(char),22))),_tmp9B6)))))))))))),((_tmp86C[16]=((
_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC->f1=((_tmp9BB="-fomit-frame-pointer",
_tag_dyneither(_tmp9BB,sizeof(char),21))),((_tmp9BC->f2=0,((_tmp9BC->f3=((
_tmp9BA="",_tag_dyneither(_tmp9BA,sizeof(char),1))),((_tmp9BC->f4=(void*)((
_tmp9B8=_region_malloc(r,sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=1,((
_tmp9B9.f1=Cyc_add_ccarg,_tmp9B9)))),_tmp9B8)))),((_tmp9BC->f5=((_tmp9B7="Omit frame pointer",
_tag_dyneither(_tmp9B7,sizeof(char),19))),_tmp9BC)))))))))))),((_tmp86C[15]=((
_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2->f1=((_tmp9C1="-O3",
_tag_dyneither(_tmp9C1,sizeof(char),4))),((_tmp9C2->f2=0,((_tmp9C2->f3=((_tmp9C0="",
_tag_dyneither(_tmp9C0,sizeof(char),1))),((_tmp9C2->f4=(void*)((_tmp9BE=
_region_malloc(r,sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=1,((_tmp9BF.f1=Cyc_add_ccarg,
_tmp9BF)))),_tmp9BE)))),((_tmp9C2->f5=((_tmp9BD="Even more optimization",
_tag_dyneither(_tmp9BD,sizeof(char),23))),_tmp9C2)))))))))))),((_tmp86C[14]=((
_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8->f1=((_tmp9C7="-O2",
_tag_dyneither(_tmp9C7,sizeof(char),4))),((_tmp9C8->f2=0,((_tmp9C8->f3=((_tmp9C6="",
_tag_dyneither(_tmp9C6,sizeof(char),1))),((_tmp9C8->f4=(void*)((_tmp9C4=
_region_malloc(r,sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=1,((_tmp9C5.f1=Cyc_add_ccarg,
_tmp9C5)))),_tmp9C4)))),((_tmp9C8->f5=((_tmp9C3="A higher level of optimization",
_tag_dyneither(_tmp9C3,sizeof(char),31))),_tmp9C8)))))))))))),((_tmp86C[13]=((
_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE->f1=((_tmp9CD="-O",
_tag_dyneither(_tmp9CD,sizeof(char),3))),((_tmp9CE->f2=0,((_tmp9CE->f3=((_tmp9CC="",
_tag_dyneither(_tmp9CC,sizeof(char),1))),((_tmp9CE->f4=(void*)((_tmp9CA=
_region_malloc(r,sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=1,((_tmp9CB.f1=Cyc_add_ccarg,
_tmp9CB)))),_tmp9CA)))),((_tmp9CE->f5=((_tmp9C9="Optimize",_tag_dyneither(
_tmp9C9,sizeof(char),9))),_tmp9CE)))))))))))),((_tmp86C[12]=((_tmp9D4=
_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4->f1=((_tmp9D3="-O0",_tag_dyneither(
_tmp9D3,sizeof(char),4))),((_tmp9D4->f2=0,((_tmp9D4->f3=((_tmp9D2="",
_tag_dyneither(_tmp9D2,sizeof(char),1))),((_tmp9D4->f4=(void*)((_tmp9D0=
_region_malloc(r,sizeof(*_tmp9D0)),((_tmp9D0[0]=((_tmp9D1.tag=1,((_tmp9D1.f1=Cyc_add_ccarg,
_tmp9D1)))),_tmp9D0)))),((_tmp9D4->f5=((_tmp9CF="Don't optimize",_tag_dyneither(
_tmp9CF,sizeof(char),15))),_tmp9D4)))))))))))),((_tmp86C[11]=((_tmp9DA=
_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA->f1=((_tmp9D9="-s",_tag_dyneither(
_tmp9D9,sizeof(char),3))),((_tmp9DA->f2=0,((_tmp9DA->f3=((_tmp9D8="",
_tag_dyneither(_tmp9D8,sizeof(char),1))),((_tmp9DA->f4=(void*)((_tmp9D6=
_region_malloc(r,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=1,((_tmp9D7.f1=Cyc_add_ccarg,
_tmp9D7)))),_tmp9D6)))),((_tmp9DA->f5=((_tmp9D5="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp9D5,sizeof(char),60))),_tmp9DA)))))))))))),((_tmp86C[10]=((
_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0->f1=((_tmp9DF="-x",
_tag_dyneither(_tmp9DF,sizeof(char),3))),((_tmp9E0->f2=0,((_tmp9E0->f3=((_tmp9DE=" <language>",
_tag_dyneither(_tmp9DE,sizeof(char),12))),((_tmp9E0->f4=(void*)((_tmp9DC=
_region_malloc(r,sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=5,((_tmp9DD.f1=Cyc_set_inputtype,
_tmp9DD)))),_tmp9DC)))),((_tmp9E0->f5=((_tmp9DB="Specify <language> for the following input files",
_tag_dyneither(_tmp9DB,sizeof(char),49))),_tmp9E0)))))))))))),((_tmp86C[9]=((
_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6->f1=((_tmp9E5="-c",
_tag_dyneither(_tmp9E5,sizeof(char),3))),((_tmp9E6->f2=0,((_tmp9E6->f3=((_tmp9E4="",
_tag_dyneither(_tmp9E4,sizeof(char),1))),((_tmp9E6->f4=(void*)((_tmp9E2=
_region_malloc(r,sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=0,((_tmp9E3.f1=Cyc_set_stop_after_objectfile,
_tmp9E3)))),_tmp9E2)))),((_tmp9E6->f5=((_tmp9E1="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp9E1,sizeof(char),61))),_tmp9E6)))))))))))),((_tmp86C[8]=((
_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC->f1=((_tmp9EB="-m",
_tag_dyneither(_tmp9EB,sizeof(char),3))),((_tmp9EC->f2=1,((_tmp9EC->f3=((_tmp9EA="<option>",
_tag_dyneither(_tmp9EA,sizeof(char),9))),((_tmp9EC->f4=(void*)((_tmp9E8=
_region_malloc(r,sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=1,((_tmp9E9.f1=Cyc_add_marg,
_tmp9E9)))),_tmp9E8)))),((_tmp9EC->f5=((_tmp9E7="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmp9E7,sizeof(char),52))),_tmp9EC)))))))))))),((_tmp86C[7]=((
_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2->f1=((_tmp9F1="-l",
_tag_dyneither(_tmp9F1,sizeof(char),3))),((_tmp9F2->f2=1,((_tmp9F2->f3=((_tmp9F0="<libname>",
_tag_dyneither(_tmp9F0,sizeof(char),10))),((_tmp9F2->f4=(void*)((_tmp9EE=
_region_malloc(r,sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=1,((_tmp9EF.f1=Cyc_add_libarg,
_tmp9EF)))),_tmp9EE)))),((_tmp9F2->f5=((_tmp9ED="Library file",_tag_dyneither(
_tmp9ED,sizeof(char),13))),_tmp9F2)))))))))))),((_tmp86C[6]=((_tmp9F8=
_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8->f1=((_tmp9F7="-L",_tag_dyneither(
_tmp9F7,sizeof(char),3))),((_tmp9F8->f2=1,((_tmp9F8->f3=((_tmp9F6="<dir>",
_tag_dyneither(_tmp9F6,sizeof(char),6))),((_tmp9F8->f4=(void*)((_tmp9F4=
_region_malloc(r,sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=1,((_tmp9F5.f1=Cyc_add_ccarg,
_tmp9F5)))),_tmp9F4)))),((_tmp9F8->f5=((_tmp9F3="Add to the list of directories for -l",
_tag_dyneither(_tmp9F3,sizeof(char),38))),_tmp9F8)))))))))))),((_tmp86C[5]=((
_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE->f1=((_tmp9FD="-I",
_tag_dyneither(_tmp9FD,sizeof(char),3))),((_tmp9FE->f2=1,((_tmp9FE->f3=((_tmp9FC="<dir>",
_tag_dyneither(_tmp9FC,sizeof(char),6))),((_tmp9FE->f4=(void*)((_tmp9FA=
_region_malloc(r,sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=1,((_tmp9FB.f1=Cyc_add_cpparg,
_tmp9FB)))),_tmp9FA)))),((_tmp9FE->f5=((_tmp9F9="Add to the list of directories to search for include files",
_tag_dyneither(_tmp9F9,sizeof(char),59))),_tmp9FE)))))))))))),((_tmp86C[4]=((
_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04->f1=((_tmpA03="-B",
_tag_dyneither(_tmpA03,sizeof(char),3))),((_tmpA04->f2=1,((_tmpA04->f3=((_tmpA02="<file>",
_tag_dyneither(_tmpA02,sizeof(char),7))),((_tmpA04->f4=(void*)((_tmpA00=
_region_malloc(r,sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=1,((_tmpA01.f1=Cyc_add_cyclone_exec_path,
_tmpA01)))),_tmpA00)))),((_tmpA04->f5=((_tmp9FF="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp9FF,sizeof(char),60))),_tmpA04)))))))))))),((_tmp86C[3]=((
_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A->f1=((_tmpA09="-D",
_tag_dyneither(_tmpA09,sizeof(char),3))),((_tmpA0A->f2=1,((_tmpA0A->f3=((_tmpA08="<name>[=<value>]",
_tag_dyneither(_tmpA08,sizeof(char),17))),((_tmpA0A->f4=(void*)((_tmpA06=
_region_malloc(r,sizeof(*_tmpA06)),((_tmpA06[0]=((_tmpA07.tag=1,((_tmpA07.f1=Cyc_add_cpparg,
_tmpA07)))),_tmpA06)))),((_tmpA0A->f5=((_tmpA05="Pass definition to preprocessor",
_tag_dyneither(_tmpA05,sizeof(char),32))),_tmpA0A)))))))))))),((_tmp86C[2]=((
_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10->f1=((_tmpA0F="-o",
_tag_dyneither(_tmpA0F,sizeof(char),3))),((_tmpA10->f2=0,((_tmpA10->f3=((_tmpA0E=" <file>",
_tag_dyneither(_tmpA0E,sizeof(char),8))),((_tmpA10->f4=(void*)((_tmpA0C=
_region_malloc(r,sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0D.tag=5,((_tmpA0D.f1=Cyc_set_output_file,
_tmpA0D)))),_tmpA0C)))),((_tmpA10->f5=((_tmpA0B="Set the output file name to <file>",
_tag_dyneither(_tmpA0B,sizeof(char),35))),_tmpA10)))))))))))),((_tmp86C[1]=((
_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16->f1=((_tmpA15="--version",
_tag_dyneither(_tmpA15,sizeof(char),10))),((_tmpA16->f2=0,((_tmpA16->f3=((
_tmpA14="",_tag_dyneither(_tmpA14,sizeof(char),1))),((_tmpA16->f4=(void*)((
_tmpA12=_region_malloc(r,sizeof(*_tmpA12)),((_tmpA12[0]=((_tmpA13.tag=0,((
_tmpA13.f1=Cyc_print_version,_tmpA13)))),_tmpA12)))),((_tmpA16->f5=((_tmpA11="Print version information and exit",
_tag_dyneither(_tmpA11,sizeof(char),35))),_tmpA16)))))))))))),((_tmp86C[0]=((
_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C->f1=((_tmpA1B="-v",
_tag_dyneither(_tmpA1B,sizeof(char),3))),((_tmpA1C->f2=0,((_tmpA1C->f3=((_tmpA1A="",
_tag_dyneither(_tmpA1A,sizeof(char),1))),((_tmpA1C->f4=(void*)((_tmpA18=
_region_malloc(r,sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA19.tag=3,((_tmpA19.f1=&
Cyc_v_r,_tmpA19)))),_tmpA18)))),((_tmpA1C->f5=((_tmpA17="Print compilation stages verbosely",
_tag_dyneither(_tmpA17,sizeof(char),35))),_tmpA1C)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp86C,sizeof(struct _tuple21*),72)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmpA1D;struct _dyneither_ptr optstring=(_tmpA1D="Options:",
_tag_dyneither(_tmpA1D,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp164);if((char*)Cyc_target_arch.curr == (char*)(
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{char*_tmpA1E;struct _dyneither_ptr cyclone_exec_prefix=(
_tmpA1E=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA1E,sizeof(
char),_get_zero_arr_size_char((void*)_tmpA1E,1)));if((char*)cyclone_exec_prefix.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*_tmpA21;struct Cyc_List_List*
_tmpA20;Cyc_cyclone_exec_path=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->hd=((
_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21[0]=cyclone_exec_prefix,_tmpA21)))),((
_tmpA20->tl=Cyc_cyclone_exec_path,_tmpA20))))));}def_lib_path=(struct
_dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct
_dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmpA27;const char*_tmpA26;
struct Cyc_List_List*_tmpA25;Cyc_cyclone_exec_path=((_tmpA25=_cycalloc(sizeof(*
_tmpA25)),((_tmpA25->hd=((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=(
struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA26="cyc-lib",
_tag_dyneither(_tmpA26,sizeof(char),8)))),_tmpA27)))),((_tmpA25->tl=Cyc_cyclone_exec_path,
_tmpA25))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmpA28;struct _dyneither_ptr
_tmp16B=Cyc_do_find(cyclone_arch_path,((_tmpA28="cycspecs",_tag_dyneither(
_tmpA28,sizeof(char),9))));if(Cyc_v_r){const char*_tmpA2C;void*_tmpA2B[1];struct
Cyc_String_pa_struct _tmpA2A;(_tmpA2A.tag=0,((_tmpA2A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp16B),((_tmpA2B[0]=& _tmpA2A,Cyc_fprintf(Cyc_stderr,((
_tmpA2C="Reading from specs file %s\n",_tag_dyneither(_tmpA2C,sizeof(char),28))),
_tag_dyneither(_tmpA2B,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp16F=Cyc_read_specs(
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
options,Cyc_add_other,optstring,_tmp165);{const char*_tmpA2D;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_cpp,((_tmpA2D="",_tag_dyneither(_tmpA2D,sizeof(char),1))))
== 0){const char*_tmp17A=Cyc_produce_dependencies?"":(const char*)" -E";const char*
_tmpA35;void*_tmpA34[3];struct Cyc_String_pa_struct _tmpA33;const char*_tmpA32;
struct Cyc_String_pa_struct _tmpA31;struct Cyc_String_pa_struct _tmpA30;Cyc_set_cpp((
struct _dyneither_ptr)((_tmpA30.tag=0,((_tmpA30.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp16B),((_tmpA31.tag=0,((_tmpA31.f1=(struct _dyneither_ptr)((
_tmpA32=_tmp17A,_tag_dyneither(_tmpA32,sizeof(char),_get_zero_arr_size_char((
void*)_tmpA32,1)))),((_tmpA33.tag=0,((_tmpA33.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmpA34[0]=& _tmpA33,((_tmpA34[1]=& _tmpA31,((_tmpA34[2]=&
_tmpA30,Cyc_aprintf(((_tmpA35="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA35,
sizeof(char),23))),_tag_dyneither(_tmpA34,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmpA38;void*_tmpA37;(
_tmpA37=0,Cyc_fprintf(Cyc_stderr,((_tmpA38="missing file\n",_tag_dyneither(
_tmpA38,sizeof(char),14))),_tag_dyneither(_tmpA37,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmpA39;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmpA39="cyc_setjmp.h",_tag_dyneither(_tmpA39,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmpA3A;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmpA3A="cyc_include.h",_tag_dyneither(_tmpA3A,sizeof(
char),14))));}{struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp345 != 0;_tmp345=_tmp345->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp345->hd));if(Cyc_compile_failure){int _tmp346=1;
_npop_handler(0);return _tmp346;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp347=0;_npop_handler(0);return _tmp347;}if(Cyc_ccargs
== 0){int _tmp348=0;_npop_handler(0);return _tmp348;}{const char*_tmpA3B;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmpA3B="-L",_tag_dyneither(_tmpA3B,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmpA3C;struct
_dyneither_ptr _tmp34A=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA3C=" ",_tag_dyneither(_tmpA3C,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmpA3E;struct Cyc_List_List*_tmpA3D;struct _dyneither_ptr
_tmp34B=Cyc_str_sepstr(((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmpA3D->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmpA3D)))))),((_tmpA3E=" ",_tag_dyneither(_tmpA3E,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmpA40;
const char*_tmpA3F;int _tmp34C=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmpA40=".a",_tag_dyneither(_tmpA40,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA3F=".lib",_tag_dyneither(
_tmpA3F,sizeof(char),5))));if(_tmp34C){stdlib=0;{const char*_tmpA41;stdlib_string=(
struct _dyneither_ptr)((_tmpA41="",_tag_dyneither(_tmpA41,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmpA42;libcyc_flag=((_tmpA42="-lcyc_a",_tag_dyneither(_tmpA42,sizeof(
char),8)));}{const char*_tmpA43;nogc_filename=((_tmpA43="nogc_a.a",_tag_dyneither(
_tmpA43,sizeof(char),9)));}{const char*_tmpA44;runtime_filename=((_tmpA44="runtime_cyc_a.o",
_tag_dyneither(_tmpA44,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmpA45;libcyc_flag=((_tmpA45="-lcyc_nocheck",_tag_dyneither(_tmpA45,sizeof(char),
14)));}{const char*_tmpA46;nogc_filename=((_tmpA46="nogc.a",_tag_dyneither(
_tmpA46,sizeof(char),7)));}{const char*_tmpA47;runtime_filename=((_tmpA47="runtime_cyc.o",
_tag_dyneither(_tmpA47,sizeof(char),14)));};}else{if(Cyc_pg_r){{const char*
_tmpA48;libcyc_flag=((_tmpA48="-lcyc_pg",_tag_dyneither(_tmpA48,sizeof(char),9)));}{
const char*_tmpA49;runtime_filename=((_tmpA49="runtime_cyc_pg.o",_tag_dyneither(
_tmpA49,sizeof(char),17)));}{const char*_tmpA4A;nogc_filename=((_tmpA4A="nogc_pg.a",
_tag_dyneither(_tmpA4A,sizeof(char),10)));};}else{if(Cyc_compile_for_boot_r){{
const char*_tmpA4B;libcyc_flag=((_tmpA4B="-lcycboot",_tag_dyneither(_tmpA4B,
sizeof(char),10)));}{const char*_tmpA4C;nogc_filename=((_tmpA4C="nogc.a",
_tag_dyneither(_tmpA4C,sizeof(char),7)));}{const char*_tmpA4D;runtime_filename=((
_tmpA4D="runtime_cyc.o",_tag_dyneither(_tmpA4D,sizeof(char),14)));};}else{{const
char*_tmpA4E;libcyc_flag=((_tmpA4E="-lcyc",_tag_dyneither(_tmpA4E,sizeof(char),6)));}{
const char*_tmpA4F;nogc_filename=((_tmpA4F="nogc.a",_tag_dyneither(_tmpA4F,
sizeof(char),7)));}{const char*_tmpA50;runtime_filename=((_tmpA50="runtime_cyc.o",
_tag_dyneither(_tmpA50,sizeof(char),14)));};}}}}{const char*_tmpA51;gc_filename=((
_tmpA51="gc.a",_tag_dyneither(_tmpA51,sizeof(char),5)));}{struct _dyneither_ptr
_tmp35E=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp35F=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmpA57;void*_tmpA56[3];
struct Cyc_String_pa_struct _tmpA55;struct Cyc_String_pa_struct _tmpA54;struct Cyc_String_pa_struct
_tmpA53;stdlib_string=(struct _dyneither_ptr)((_tmpA53.tag=0,((_tmpA53.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp35E),((_tmpA54.tag=0,((_tmpA54.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA55.tag=0,((
_tmpA55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35F),((_tmpA56[0]=&
_tmpA55,((_tmpA56[1]=& _tmpA54,((_tmpA56[2]=& _tmpA53,Cyc_aprintf(((_tmpA57=" %s %s %s",
_tag_dyneither(_tmpA57,sizeof(char),10))),_tag_dyneither(_tmpA56,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp365;_push_handler(& _tmp365);{int _tmp367=0;
if(setjmp(_tmp365.handler))_tmp367=1;if(!_tmp367){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp368=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp34C){struct
Cyc_List_List*_tmpA58;_tmp368=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->hd=
Cyc_final_strptr,((_tmpA58->tl=_tmp368,_tmpA58))))));}{struct Cyc_Interface_I*
_tmp36A=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp368,_tmp368);if(_tmp36A == 0){{const char*_tmpA5B;void*_tmpA5A;(
_tmpA5A=0,Cyc_fprintf(Cyc_stderr,((_tmpA5B="Error: interfaces incompatible\n",
_tag_dyneither(_tmpA5B,sizeof(char),32))),_tag_dyneither(_tmpA5A,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp36D=1;_npop_handler(1);return
_tmp36D;};}if(_tmp34C){if(Cyc_output_file != 0){const char*_tmpA5F;void*_tmpA5E[1];
struct Cyc_String_pa_struct _tmpA5D;struct _dyneither_ptr _tmp36E=(_tmpA5D.tag=0,((
_tmpA5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA5E[0]=& _tmpA5D,Cyc_aprintf(((
_tmpA5F="%s.cycio",_tag_dyneither(_tmpA5F,sizeof(char),9))),_tag_dyneither(
_tmpA5E,sizeof(void*),1)))))));const char*_tmpA61;const char*_tmpA60;struct Cyc___cycFILE*
_tmp36F=Cyc_try_file_open((struct _dyneither_ptr)_tmp36E,((_tmpA60="wb",
_tag_dyneither(_tmpA60,sizeof(char),3))),((_tmpA61="interface object file",
_tag_dyneither(_tmpA61,sizeof(char),22))));if(_tmp36F == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp370=1;_npop_handler(1);return _tmp370;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp36A,(struct Cyc___cycFILE*)_tmp36F);Cyc_file_close((
struct Cyc___cycFILE*)_tmp36F);}}else{const char*_tmpA66;const char*_tmpA65;struct
_tuple17*_tmpA64;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp36A,((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->f1=((
_tmpA65="empty interface",_tag_dyneither(_tmpA65,sizeof(char),16))),((_tmpA64->f2=((
_tmpA66="global interface",_tag_dyneither(_tmpA66,sizeof(char),17))),_tmpA64)))))))){{
const char*_tmpA69;void*_tmpA68;(_tmpA68=0,Cyc_fprintf(Cyc_stderr,((_tmpA69="Error: some objects are still undefined\n",
_tag_dyneither(_tmpA69,sizeof(char),41))),_tag_dyneither(_tmpA68,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp37B=1;_npop_handler(1);return
_tmp37B;};}}};};;_pop_handler();}else{void*_tmp366=(void*)_exn_thrown;void*
_tmp37D=_tmp366;_LL3B:;_LL3C:{void*_tmp37E=_tmp37D;struct _dyneither_ptr _tmp380;
struct _dyneither_ptr _tmp382;struct _dyneither_ptr _tmp385;_LL40: {struct Cyc_Core_Failure_struct*
_tmp37F=(struct Cyc_Core_Failure_struct*)_tmp37E;if(_tmp37F->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp380=_tmp37F->f1;}}_LL41:{const char*_tmpA6D;void*_tmpA6C[1];
struct Cyc_String_pa_struct _tmpA6B;(_tmpA6B.tag=0,((_tmpA6B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp380),((_tmpA6C[0]=& _tmpA6B,Cyc_fprintf(
Cyc_stderr,((_tmpA6D="Exception Core::Failure %s\n",_tag_dyneither(_tmpA6D,
sizeof(char),28))),_tag_dyneither(_tmpA6C,sizeof(void*),1)))))));}goto _LL3F;
_LL42: {struct Cyc_Core_Impossible_struct*_tmp381=(struct Cyc_Core_Impossible_struct*)
_tmp37E;if(_tmp381->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp382=_tmp381->f1;}}
_LL43:{const char*_tmpA71;void*_tmpA70[1];struct Cyc_String_pa_struct _tmpA6F;(
_tmpA6F.tag=0,((_tmpA6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp382),((
_tmpA70[0]=& _tmpA6F,Cyc_fprintf(Cyc_stderr,((_tmpA71="Exception Core::Impossible %s\n",
_tag_dyneither(_tmpA71,sizeof(char),31))),_tag_dyneither(_tmpA70,sizeof(void*),1)))))));}
goto _LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp383=(struct Cyc_Dict_Absent_struct*)
_tmp37E;if(_tmp383->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:{const char*_tmpA74;
void*_tmpA73;(_tmpA73=0,Cyc_fprintf(Cyc_stderr,((_tmpA74="Exception Dict::Absent\n",
_tag_dyneither(_tmpA74,sizeof(char),24))),_tag_dyneither(_tmpA73,sizeof(void*),0)));}
goto _LL3F;_LL46: {struct Cyc_Core_Invalid_argument_struct*_tmp384=(struct Cyc_Core_Invalid_argument_struct*)
_tmp37E;if(_tmp384->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp385=
_tmp384->f1;}}_LL47:{const char*_tmpA78;void*_tmpA77[1];struct Cyc_String_pa_struct
_tmpA76;(_tmpA76.tag=0,((_tmpA76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp385),((_tmpA77[0]=& _tmpA76,Cyc_fprintf(Cyc_stderr,((_tmpA78="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmpA78,sizeof(char),37))),_tag_dyneither(_tmpA77,sizeof(void*),1)))))));}
goto _LL3F;_LL48:;_LL49:{const char*_tmpA7B;void*_tmpA7A;(_tmpA7A=0,Cyc_fprintf(
Cyc_stderr,((_tmpA7B="Uncaught exception\n",_tag_dyneither(_tmpA7B,sizeof(char),
20))),_tag_dyneither(_tmpA7A,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp393=1;_npop_handler(0);return _tmp393;};_LL3D:;
_LL3E:(void)_throw(_tmp37D);_LL3A:;}};}{const char*_tmpA7C;struct _dyneither_ptr
outfile_str=(_tmpA7C="",_tag_dyneither(_tmpA7C,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmpA80;void*_tmpA7F[1];struct Cyc_String_pa_struct _tmpA7E;
outfile_str=(struct _dyneither_ptr)((_tmpA7E.tag=0,((_tmpA7E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA7F[0]=& _tmpA7E,Cyc_aprintf(((
_tmpA80=" -o %s",_tag_dyneither(_tmpA80,sizeof(char),7))),_tag_dyneither(_tmpA7F,
sizeof(void*),1))))))));}{const char*_tmpA88;void*_tmpA87[5];struct Cyc_String_pa_struct
_tmpA86;struct Cyc_String_pa_struct _tmpA85;struct Cyc_String_pa_struct _tmpA84;
struct Cyc_String_pa_struct _tmpA83;struct Cyc_String_pa_struct _tmpA82;struct
_dyneither_ptr _tmp397=(_tmpA82.tag=0,((_tmpA82.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp34B),((_tmpA83.tag=0,((_tmpA83.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmpA84.tag=0,((_tmpA84.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp34A),((_tmpA85.tag=0,((_tmpA85.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpA86.tag=0,((
_tmpA86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA87[0]=&
_tmpA86,((_tmpA87[1]=& _tmpA85,((_tmpA87[2]=& _tmpA84,((_tmpA87[3]=& _tmpA83,((
_tmpA87[4]=& _tmpA82,Cyc_aprintf(((_tmpA88="%s %s %s%s%s",_tag_dyneither(_tmpA88,
sizeof(char),13))),_tag_dyneither(_tmpA87,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmpA8C;void*_tmpA8B[1];struct Cyc_String_pa_struct _tmpA8A;(
_tmpA8A.tag=0,((_tmpA8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp397),((
_tmpA8B[0]=& _tmpA8A,Cyc_fprintf(Cyc_stderr,((_tmpA8C="%s\n",_tag_dyneither(
_tmpA8C,sizeof(char),4))),_tag_dyneither(_tmpA8B,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp397,sizeof(char),1))!= 0){{const char*_tmpA8F;void*_tmpA8E;(_tmpA8E=0,Cyc_fprintf(
Cyc_stderr,((_tmpA8F="Error: C compiler failed\n",_tag_dyneither(_tmpA8F,sizeof(
char),26))),_tag_dyneither(_tmpA8E,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp39D=1;_npop_handler(0);return _tmp39D;};}Cyc_remove_cfiles();{int _tmp39E=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp39E;};};};};};;_pop_region(r);};}