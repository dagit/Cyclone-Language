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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;enum Cyc_Absyn_Kind val;};
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
val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct
_union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct
_union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;
};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};extern char
Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
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
void Cyc_add_ccarg(struct _dyneither_ptr s){struct _dyneither_ptr*_tmp37C;struct Cyc_List_List*
_tmp37B;Cyc_ccargs=((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B->hd=((_tmp37C=
_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=s,_tmp37C)))),((_tmp37B->tl=Cyc_ccargs,
_tmp37B))))));}void Cyc_set_c_compiler(struct _dyneither_ptr s);void Cyc_set_c_compiler(
struct _dyneither_ptr s){const char*_tmp37D;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp37D="vc",_tag_dyneither(_tmp37D,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp37E;Cyc_add_ccarg(((_tmp37E="-DVC_C",
_tag_dyneither(_tmp37E,sizeof(char),7))));};}else{const char*_tmp37F;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp37F="gcc",_tag_dyneither(_tmp37F,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp380;Cyc_add_ccarg(((
_tmp380="-DGCC_C",_tag_dyneither(_tmp380,sizeof(char),8))));};}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dyneither_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dyneither_ptr s);static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp381;Cyc_output_file=((_tmp381=
_cycalloc(sizeof(*_tmp381)),((_tmp381[0]=s,_tmp381))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s);static void Cyc_set_cpp(struct
_dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dyneither_ptr s);static void Cyc_add_cpparg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp384;struct Cyc_List_List*_tmp383;Cyc_cppargs=((
_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383->hd=((_tmp384=_cycalloc(sizeof(*
_tmp384)),((_tmp384[0]=s,_tmp384)))),((_tmp383->tl=Cyc_cppargs,_tmp383))))));}
static struct _dyneither_ptr Cyc_target_arch=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};static void Cyc_set_target_arch(struct _dyneither_ptr s);static void
Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}static
void Cyc_print_version();static void Cyc_print_version(){{const char*_tmp388;void*
_tmp387[1];struct Cyc_String_pa_struct _tmp386;(_tmp386.tag=0,((_tmp386.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp387[0]=&
_tmp386,Cyc_printf(((_tmp388="The Cyclone compiler, version %s\n",_tag_dyneither(
_tmp388,sizeof(char),34))),_tag_dyneither(_tmp387,sizeof(void*),1)))))));}{const
char*_tmp38D;void*_tmp38C[2];struct Cyc_String_pa_struct _tmp38B;struct Cyc_String_pa_struct
_tmp38A;(_tmp38A.tag=0,((_tmp38A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp38B.tag=0,((_tmp38B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp38C[0]=& _tmp38B,((_tmp38C[1]=&
_tmp38A,Cyc_printf(((_tmp38D="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp38D,sizeof(char),39))),_tag_dyneither(_tmp38C,sizeof(void*),2)))))))))))));}{
const char*_tmp391;void*_tmp390[1];struct Cyc_String_pa_struct _tmp38F;(_tmp38F.tag=
0,((_tmp38F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp390[0]=& _tmp38F,Cyc_printf(((_tmp391="Standard library directory: %s\n",
_tag_dyneither(_tmp391,sizeof(char),32))),_tag_dyneither(_tmp390,sizeof(void*),1)))))));}{
const char*_tmp395;void*_tmp394[1];struct Cyc_String_pa_struct _tmp393;(_tmp393.tag=
0,((_tmp393.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp394[0]=& _tmp393,Cyc_printf(((_tmp395="Standard include directory: %s\n",
_tag_dyneither(_tmp395,sizeof(char),32))),_tag_dyneither(_tmp394,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s);static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp17=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp17 <= 4)return 0;else{const char*_tmp396;return Cyc_strcmp((struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),((_tmp396=".cyc",
_tag_dyneither(_tmp396,sizeof(char),5))))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s);static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp19=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp19 <= 2)return;{struct _dyneither_ptr _tmp1A=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp19 - 2);struct _dyneither_ptr*_tmp399;struct Cyc_List_List*
_tmp398;Cyc_cyclone_exec_path=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398->hd=((
_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=_tmp1A,_tmp399)))),((_tmp398->tl=
Cyc_cyclone_exec_path,_tmp398))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s);static void Cyc_add_libarg(struct
_dyneither_ptr s){const char*_tmp39A;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp39A="-lxml",_tag_dyneither(_tmp39A,sizeof(char),6))))== 0)Cyc_add_ccarg(s);
else{struct _dyneither_ptr*_tmp39D;struct Cyc_List_List*_tmp39C;Cyc_libargs=((
_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->hd=((_tmp39D=_cycalloc(sizeof(*
_tmp39D)),((_tmp39D[0]=s,_tmp39D)))),((_tmp39C->tl=Cyc_libargs,_tmp39C))))));}}
static void Cyc_add_marg(struct _dyneither_ptr s);static void Cyc_add_marg(struct
_dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{const char*_tmp39E;Cyc_add_ccarg(((_tmp39E="-save-temps",
_tag_dyneither(_tmp39E,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp39F;Cyc_add_cpparg(((
_tmp39F="-M",_tag_dyneither(_tmp39F,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s);static void Cyc_set_dependencies_target(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp3A0;Cyc_dependencies_target=((_tmp3A0=
_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0[0]=s,_tmp3A0))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp3A1;Cyc_add_ccarg(((_tmp3A1="-c",_tag_dyneither(_tmp3A1,sizeof(char),3))));};}
static void Cyc_set_nocppprecomp();static void Cyc_set_nocppprecomp(){{const char*
_tmp3A2;Cyc_add_cpparg(((_tmp3A2="-no-cpp-precomp",_tag_dyneither(_tmp3A2,
sizeof(char),16))));}{const char*_tmp3A3;Cyc_add_ccarg(((_tmp3A3="-no-cpp-precomp",
_tag_dyneither(_tmp3A3,sizeof(char),16))));};}static void Cyc_set_lineno();static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp3A4;Cyc_add_ccarg(((_tmp3A4="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3A4,
sizeof(char),18))));}Cyc_set_elim_se();}static void Cyc_set_noboundschecks();
static void Cyc_set_noboundschecks(){const char*_tmp3A5;Cyc_add_ccarg(((_tmp3A5="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp3A5,sizeof(char),23))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp3A6;Cyc_add_ccarg(((_tmp3A6="-DNO_CYC_NULL_CHECKS",
_tag_dyneither(_tmp3A6,sizeof(char),21))));}static void Cyc_set_nochecks();static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;{
const char*_tmp3A7;Cyc_add_ccarg(((_tmp3A7="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp3A7,sizeof(char),16))));};}static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=
1;{const char*_tmp3A8;Cyc_add_ccarg(((_tmp3A8="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp3A8,sizeof(char),21))));};}static void Cyc_set_pg();static void
Cyc_set_pg(){Cyc_pg_r=1;{const char*_tmp3A9;Cyc_add_ccarg(((_tmp3A9="-pg",
_tag_dyneither(_tmp3A9,sizeof(char),4))));};}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp3AA;Cyc_add_ccarg(((_tmp3AA="-S",_tag_dyneither(_tmp3AA,sizeof(char),3))));};}
static void Cyc_set_all_warnings();static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE
 = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(
struct _dyneither_ptr s);static void Cyc_set_inputtype(struct _dyneither_ptr s){const
char*_tmp3AB;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3AB="cyc",
_tag_dyneither(_tmp3AB,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp3AC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3AC="none",
_tag_dyneither(_tmp3AC,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp3B0;void*_tmp3AF[1];struct Cyc_String_pa_struct _tmp3AE;(_tmp3AE.tag=
0,((_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3AF[0]=&
_tmp3AE,Cyc_fprintf(Cyc_stderr,((_tmp3B0="Input type '%s' not supported\n",
_tag_dyneither(_tmp3B0,sizeof(char),31))),_tag_dyneither(_tmp3AF,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file);struct _dyneither_ptr Cyc_make_base_filename(struct
_dyneither_ptr s,struct _dyneither_ptr*output_file){struct _dyneither_ptr _tmp32=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp33=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp34=Cyc_strlen((
struct _dyneither_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp32,(
struct _dyneither_ptr)_tmp33): _tmp33;return(struct _dyneither_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s);static void Cyc_add_other(struct _dyneither_ptr s){if(Cyc_intype == 
Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{struct _dyneither_ptr*_tmp3B3;
struct Cyc_List_List*_tmp3B2;Cyc_cyclone_files=((_tmp3B2=_cycalloc(sizeof(*
_tmp3B2)),((_tmp3B2->hd=((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3[0]=s,
_tmp3B3)))),((_tmp3B2->tl=Cyc_cyclone_files,_tmp3B2))))));}{struct _dyneither_ptr
_tmp37=Cyc_make_base_filename(s,Cyc_output_file);const char*_tmp3B4;struct
_dyneither_ptr _tmp38=Cyc_strconcat((struct _dyneither_ptr)_tmp37,((_tmp3B4=".c",
_tag_dyneither(_tmp3B4,sizeof(char),3))));Cyc_add_ccarg((struct _dyneither_ptr)
_tmp38);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(struct
_dyneither_ptr s);static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)
return;else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct
_tuple18{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*
Cyc_read_specs(struct _dyneither_ptr file);struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp3B8;void*_tmp3B7[1];struct Cyc_String_pa_struct _tmp3B6;(_tmp3B6.tag=
0,((_tmp3B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3B7[0]=&
_tmp3B6,Cyc_fprintf(Cyc_stderr,((_tmp3B8="Error opening spec file %s\n",
_tag_dyneither(_tmp3B8,sizeof(char),28))),_tag_dyneither(_tmp3B7,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp3BD;void*_tmp3BC[2];struct Cyc_String_pa_struct _tmp3BB;struct
Cyc_Int_pa_struct _tmp3BA;(_tmp3BA.tag=1,((_tmp3BA.f1=(unsigned long)c,((_tmp3BB.tag=
0,((_tmp3BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3BC[0]=&
_tmp3BB,((_tmp3BC[1]=& _tmp3BA,Cyc_fprintf(Cyc_stderr,((_tmp3BD="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp3BD,sizeof(char),55))),_tag_dyneither(_tmp3BC,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp3C1;void*_tmp3C0[1];struct Cyc_String_pa_struct _tmp3BF;(
_tmp3BF.tag=0,((_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp3C0[0]=& _tmp3BF,Cyc_fprintf(Cyc_stderr,((_tmp3C1="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp3C1,sizeof(char),44))),_tag_dyneither(_tmp3C0,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp3C5;void*_tmp3C4[1];struct Cyc_String_pa_struct _tmp3C3;(_tmp3C3.tag=0,((
_tmp3C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3C4[0]=&
_tmp3C3,Cyc_fprintf(Cyc_stderr,((_tmp3C5="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp3C5,sizeof(char),50))),_tag_dyneither(_tmp3C4,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple18*_tmp3D2;struct _dyneither_ptr*_tmp3D1;const char*_tmp3D0;struct
_dyneither_ptr*_tmp3CF;struct Cyc_List_List*_tmp3CE;_tmp3A=((_tmp3CE=_cycalloc(
sizeof(*_tmp3CE)),((_tmp3CE->hd=((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->f1=((
_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3D1)))),((_tmp3D2->f2=((_tmp3CF=
_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp3D0="",_tag_dyneither(_tmp3D0,sizeof(char),1)))),_tmp3CF)))),_tmp3D2)))))),((
_tmp3CE->tl=_tmp3A,_tmp3CE))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp3D7;void*_tmp3D6[2];struct Cyc_String_pa_struct _tmp3D5;struct Cyc_String_pa_struct
_tmp3D4;(_tmp3D4.tag=0,((_tmp3D4.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp3D5.tag=0,((_tmp3D5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp3D6[0]=& _tmp3D5,((_tmp3D6[1]=& _tmp3D4,Cyc_fprintf(Cyc_stderr,((
_tmp3D7="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp3D7,sizeof(char),50))),_tag_dyneither(_tmp3D6,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple18*_tmp3E0;struct _dyneither_ptr*_tmp3DF;struct _dyneither_ptr*_tmp3DE;
struct Cyc_List_List*_tmp3DD;_tmp3A=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((
_tmp3DD->hd=((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0->f1=((_tmp3DF=
_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3DF)))),((_tmp3E0->f2=((_tmp3DE=
_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp3DE)))),_tmp3E0)))))),((_tmp3DD->tl=
_tmp3A,_tmp3DD))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp3A;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp3EA(struct _dyneither_ptr*_tmp5B,
unsigned int*_tmp3E9,unsigned int*_tmp3E8,struct _dyneither_ptr**_tmp3E6){for(*
_tmp3E9=0;*_tmp3E9 < *_tmp3E8;(*_tmp3E9)++){(*_tmp3E6)[*_tmp3E9]=*((struct
_dyneither_ptr**)(*_tmp5B).curr)[(int)*_tmp3E9];}}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline){if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return _tag_dyneither(0,0,0);{unsigned long _tmp55=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp56=0;char buf[4096];int i=0;int j=0;
if(_tmp55 > 4096)goto DONE;while(1){while(1){if(i >= _tmp55)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp55)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp55)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';{struct _dyneither_ptr*_tmp3E3;struct Cyc_List_List*_tmp3E2;_tmp56=((
_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2->hd=((_tmp3E3=_cycalloc(sizeof(*
_tmp3E3)),((_tmp3E3[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp3E3)))),((_tmp3E2->tl=_tmp56,_tmp3E2))))));};}DONE:
_tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp56);{
struct Cyc_List_List*_tmp3E4;_tmp56=((_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((
_tmp3E4->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp3E4->tl=_tmp56,_tmp3E4))))));}{struct _dyneither_ptr _tmp5B=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp56);
unsigned int _tmp3E9;unsigned int _tmp3E8;struct _dyneither_ptr _tmp3E7;struct
_dyneither_ptr*_tmp3E6;unsigned int _tmp3E5;struct _dyneither_ptr _tmp5C=(_tmp3E5=
_get_dyneither_size(_tmp5B,sizeof(struct _dyneither_ptr*)),((_tmp3E6=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp3E5)),((
_tmp3E7=_tag_dyneither(_tmp3E6,sizeof(struct _dyneither_ptr),_tmp3E5),((((_tmp3E8=
_tmp3E5,_tmp3EA(& _tmp5B,& _tmp3E9,& _tmp3E8,& _tmp3E6))),_tmp3E7)))))));return
_tmp5C;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part);struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,
struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp61;
_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!
_tmp63){{struct Cyc___cycFILE*_tmp64=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp62=(void*)
_exn_thrown;void*_tmp66=_tmp62;_LL1:;_LL2: Cyc_compile_failure=1;{const char*
_tmp3EF;void*_tmp3EE[2];struct Cyc_String_pa_struct _tmp3ED;struct Cyc_String_pa_struct
_tmp3EC;(_tmp3EC.tag=0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename),((_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg_part),((_tmp3EE[0]=& _tmp3ED,((_tmp3EE[1]=& _tmp3EC,Cyc_fprintf(
Cyc_stderr,((_tmp3EF="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp3EF,
sizeof(char),29))),_tag_dyneither(_tmp3EE,sizeof(void*),2)))))))))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp66);_LL0:;}};}
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*
Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){const char*_tmp3F0;struct
_dyneither_ptr exn_string=(_tmp3F0="",_tag_dyneither(_tmp3F0,sizeof(char),1));
const char*_tmp3F1;struct _dyneither_ptr explain_string=(_tmp3F1="",_tag_dyneither(
_tmp3F1,sizeof(char),1));int other_exn=0;struct Cyc_Core_Impossible_struct _tmp3F7;
const char*_tmp3F6;struct Cyc_Core_Impossible_struct*_tmp3F5;void*ex=(void*)((
_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F7.tag=Cyc_Core_Impossible,((
_tmp3F7.f1=((_tmp3F6="",_tag_dyneither(_tmp3F6,sizeof(char),1))),_tmp3F7)))),
_tmp3F5))));struct Cyc_List_List*_tmp6B=0;{struct _handler_cons _tmp6C;
_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!
_tmp6E){_tmp6B=f(env,tds);;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;
void*_tmp70=_tmp6D;struct _dyneither_ptr _tmp72;struct _dyneither_ptr _tmp75;struct
_dyneither_ptr _tmp77;_LL6: {struct Cyc_Core_Impossible_struct*_tmp71=(struct Cyc_Core_Impossible_struct*)
_tmp70;if(_tmp71->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp72=_tmp71->f1;}}
_LL7:{const char*_tmp3F8;exn_string=((_tmp3F8="Exception Core::Impossible",
_tag_dyneither(_tmp3F8,sizeof(char),27)));}explain_string=_tmp72;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp73=(struct Cyc_Dict_Absent_struct*)_tmp70;if(
_tmp73->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp3F9;exn_string=((
_tmp3F9="Exception Dict::Absent",_tag_dyneither(_tmp3F9,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp74=(struct Cyc_Core_Invalid_argument_struct*)
_tmp70;if(_tmp74->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp75=_tmp74->f1;}}
_LLB:{const char*_tmp3FA;exn_string=((_tmp3FA="Exception Core::Invalid_argument",
_tag_dyneither(_tmp3FA,sizeof(char),33)));}explain_string=_tmp75;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp76=(struct Cyc_Core_Failure_struct*)_tmp70;if(
_tmp76->tag != Cyc_Core_Failure)goto _LLE;else{_tmp77=_tmp76->f1;}}_LLD:{const char*
_tmp3FB;exn_string=((_tmp3FB="Exception Core::Failure",_tag_dyneither(_tmp3FB,
sizeof(char),24)));}explain_string=_tmp77;goto _LL5;_LLE:;_LLF: ex=_tmp70;
other_exn=1;{const char*_tmp3FC;exn_string=((_tmp3FC="Uncaught exception",
_tag_dyneither(_tmp3FC,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp70);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp3FD;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp3FD="",
_tag_dyneither(_tmp3FD,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp403;void*_tmp402[3];struct Cyc_String_pa_struct _tmp401;struct Cyc_String_pa_struct
_tmp400;struct Cyc_String_pa_struct _tmp3FF;(_tmp3FF.tag=0,((_tmp3FF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp400.tag=0,((_tmp400.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp401.tag=0,((
_tmp401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp402[0]=&
_tmp401,((_tmp402[1]=& _tmp400,((_tmp402[2]=& _tmp3FF,Cyc_fprintf(Cyc_stderr,((
_tmp403="\n%s thrown during %s: %s",_tag_dyneither(_tmp403,sizeof(char),25))),
_tag_dyneither(_tmp402,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp406;void*_tmp405;(_tmp405=0,Cyc_fprintf(Cyc_stderr,((_tmp406="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp406,sizeof(char),22))),_tag_dyneither(_tmp405,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){{const char*_tmp40A;void*_tmp409[1];struct Cyc_String_pa_struct
_tmp408;(_tmp408.tag=0,((_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp409[0]=& _tmp408,Cyc_fprintf(Cyc_stderr,((_tmp40A="%s completed.\n",
_tag_dyneither(_tmp40A,sizeof(char),15))),_tag_dyneither(_tmp409,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore);
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*
ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*_tmp8D=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);return _tmp8D;};}struct _tuple19{struct _RegionHandle*f1;struct
Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(struct _tuple19*tcenv,
struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_typecheck(struct _tuple19*
tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp8F;struct Cyc_Tcenv_Tenv*
_tmp90;struct _tuple19 _tmp8E=*tcenv;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;Cyc_Tc_tc(
_tmp8F,_tmp90,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp90,tds);return
tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds);
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple20{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;
struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple20*
params,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_interface(struct
_tuple20*params,struct Cyc_List_List*tds){struct _tuple20 _tmp92;struct Cyc_Tcenv_Tenv*
_tmp93;struct Cyc___cycFILE*_tmp94;struct Cyc___cycFILE*_tmp95;struct _tuple20*
_tmp91=params;_tmp92=*_tmp91;_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;_tmp95=_tmp92.f3;{
struct Cyc_Interface_I*_tmp96=Cyc_Interface_extract(_tmp93->ae);if(_tmp94 == 0)Cyc_Interface_save(
_tmp96,_tmp95);else{struct Cyc_Interface_I*_tmp97=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp94);const char*_tmp40F;const char*_tmp40E;struct _tuple17*_tmp40D;if(!Cyc_Interface_is_subinterface(
_tmp97,_tmp96,((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->f1=((_tmp40E="written interface",
_tag_dyneither(_tmp40E,sizeof(char),18))),((_tmp40D->f2=((_tmp40F="maximal interface",
_tag_dyneither(_tmp40F,sizeof(char),18))),_tmp40D))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmp97,_tmp95);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(
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
_tmp412;void*_tmp411;(_tmp411=0,Cyc_fprintf(Cyc_stderr,((_tmp412="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp412,sizeof(char),53))),_tag_dyneither(_tmp411,sizeof(void*),0)));}
return 1;}{const char*_tmp414;const char*_tmp413;struct Cyc___cycFILE*_tmp9D=Cyc_try_file_open(
file,((_tmp413="r",_tag_dyneither(_tmp413,sizeof(char),2))),((_tmp414="internal compiler file",
_tag_dyneither(_tmp414,sizeof(char),23))));if(_tmp9D == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpA4=1024;
unsigned int i;for(i=0;i < _tmpA4;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp9D);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp9D)){Cyc_compile_failure=
1;{const char*_tmp418;void*_tmp417[1];struct Cyc_String_pa_struct _tmp416;(_tmp416.tag=
0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp417[0]=&
_tmp416,Cyc_fprintf(Cyc_stderr,((_tmp418="\nError: problem copying %s\n",
_tag_dyneither(_tmp418,sizeof(char),28))),_tag_dyneither(_tmp417,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp41C;void*_tmp41B[1];struct Cyc_String_pa_struct
_tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp41B[0]=& _tmp41A,Cyc_fprintf(Cyc_stderr,((_tmp41C="\nError: problem copying %s\n",
_tag_dyneither(_tmp41C,sizeof(char),28))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmp9D);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpA7=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpA7.expand_typedefs=!Cyc_noexpand_r;_tmpA7.to_VC=Cyc_Cyclone_tovc_r;
_tmpA7.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpA7.generate_line_directives=
Cyc_generate_line_directives_r;_tmpA7.print_full_evars=Cyc_print_full_evars_r;
_tmpA7.print_all_tvars=Cyc_print_all_tvars_r;_tmpA7.print_all_kinds=Cyc_print_all_kinds_r;
_tmpA7.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_fprintf(out_file,((_tmp41F="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp41F,sizeof(char),27))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r){
const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_fprintf(out_file,((_tmp422="#include <setjmp.h>\n",
_tag_dyneither(_tmp422,sizeof(char),21))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
else{if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}if(Cyc_copy_internal_file(
Cyc_cyc_include,out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpA7);
Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpA7);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles();
static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((
struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static
struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c);static struct
Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){if((char*)s.curr == (
char*)(_tag_dyneither(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmpAC=0;
unsigned long _tmpAD=Cyc_strlen((struct _dyneither_ptr)s);while(_tmpAD > 0){struct
_dyneither_ptr _tmpAE=Cyc_strchr(s,c);if((char*)_tmpAE.curr == (char*)(
_tag_dyneither(0,0,0)).curr){{struct _dyneither_ptr*_tmp425;struct Cyc_List_List*
_tmp424;_tmpAC=((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->hd=((_tmp425=
_cycalloc(sizeof(*_tmp425)),((_tmp425[0]=s,_tmp425)))),((_tmp424->tl=_tmpAC,
_tmp424))))));}break;}else{{struct _dyneither_ptr*_tmp428;struct Cyc_List_List*
_tmp427;_tmpAC=((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427->hd=((_tmp428=
_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpAE).curr - s.curr)
/ sizeof(char))),_tmp428)))),((_tmp427->tl=_tmpAC,_tmp427))))));}_tmpAD -=(((
struct _dyneither_ptr)_tmpAE).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(
_tmpAE,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpAC);};}static int Cyc_file_exists(struct _dyneither_ptr file);
static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{
struct _handler_cons _tmpB3;_push_handler(& _tmpB3);{int _tmpB5=0;if(setjmp(_tmpB3.handler))
_tmpB5=1;if(!_tmpB5){{const char*_tmp429;f=(struct Cyc___cycFILE*)Cyc_file_open(
file,((_tmp429="r",_tag_dyneither(_tmp429,sizeof(char),2))));};_pop_handler();}
else{void*_tmpB4=(void*)_exn_thrown;void*_tmpB8=_tmpB4;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpB8);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs);static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*
dirs){const char*_tmp42A;struct _dyneither_ptr tmp=(_tmp42A="",_tag_dyneither(
_tmp42A,sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr
_tmpB9=*((struct _dyneither_ptr*)dirs->hd);if((char*)_tmpB9.curr == (char*)(
_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpB9)== 0)
continue;{const char*_tmp42B;_tmpB9=(struct _dyneither_ptr)Cyc_strconcat((struct
_dyneither_ptr)_tmpB9,((_tmp42B=":",_tag_dyneither(_tmp42B,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpB9,(struct
_dyneither_ptr)tmp);}return tmp;}static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file);static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpBC=*((
struct _dyneither_ptr*)dirs->hd);if((char*)_tmpBC.curr == (char*)(_tag_dyneither(0,
0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpBC)== 0)continue;{struct
_dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpBC,file);if(Cyc_file_exists(
s)){struct _dyneither_ptr*_tmp42C;return(_tmp42C=_cycalloc(sizeof(*_tmp42C)),((
_tmp42C[0]=s,_tmp42C)));}};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file);static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpBE=
Cyc_find(dirs,file);if(_tmpBE == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
const char*_tmp431;void*_tmp430[2];struct Cyc_String_pa_struct _tmp42F;struct Cyc_String_pa_struct
_tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp42F.tag=0,((_tmp42F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file),((_tmp430[0]=& _tmp42F,((_tmp430[1]=& _tmp42E,Cyc_fprintf(
Cyc_stderr,((_tmp431="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp431,sizeof(char),56))),_tag_dyneither(_tmp430,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp441;const char*_tmp440;void*_tmp43F[2];struct Cyc_String_pa_struct _tmp43E;
struct Cyc_String_pa_struct _tmp43D;struct Cyc_Core_Failure_struct*_tmp43C;(int)
_throw((void*)((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp441.tag=
Cyc_Core_Failure,((_tmp441.f1=(struct _dyneither_ptr)((_tmp43D.tag=0,((_tmp43D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp43E.tag=
0,((_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp43F[0]=&
_tmp43E,((_tmp43F[1]=& _tmp43D,Cyc_aprintf(((_tmp440="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp440,sizeof(char),56))),_tag_dyneither(_tmp43F,sizeof(void*),2)))))))))))))),
_tmp441)))),_tmp43C)))));};}return*_tmpBE;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir);static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*
_tmpC9=0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp444;struct Cyc_List_List*
_tmp443;_tmpC9=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->hd=((_tmp444=
_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp444)))),((_tmp443->tl=_tmpC9,_tmp443))))));}
_tmpC9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC9);
return _tmpC9;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpCC=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp445;_tmpCC=((_tmp445=_cycalloc(sizeof(*
_tmp445)),((_tmp445->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp445->tl=_tmpCC,
_tmp445))))));}{struct _dyneither_ptr*_tmp448;struct Cyc_List_List*_tmp447;_tmpCC=((
_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->hd=((_tmp448=_cycalloc(sizeof(*
_tmp448)),((_tmp448[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp448)))),((_tmp447->tl=_tmpCC,_tmp447))))));};}
_tmpCC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCC);
return _tmpCC;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmp451(unsigned int*_tmp450,unsigned int*
_tmp44F,char**_tmp44D){for(*_tmp450=0;*_tmp450 < *_tmp44F;(*_tmp450)++){(*_tmp44D)[*
_tmp450]='\000';}}static struct _dyneither_ptr Cyc_sh_escape_string(struct
_dyneither_ptr s){unsigned long _tmpD0=Cyc_strlen((struct _dyneither_ptr)s);int
_tmpD1=0;int _tmpD2=0;{int i=0;for(0;i < _tmpD0;++ i){char _tmpD3=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpD3 == '\'')++ _tmpD1;else{if(
Cyc_is_other_special(_tmpD3))++ _tmpD2;}}}if(_tmpD1 == 0  && _tmpD2 == 0)return s;if(
_tmpD1 == 0){struct _dyneither_ptr*_tmp44B;struct _dyneither_ptr*_tmp44A[3];return(
struct _dyneither_ptr)Cyc_strconcat_l(((_tmp44A[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp44A[1]=((_tmp44B=
_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=(struct _dyneither_ptr)s,_tmp44B)))),((
_tmp44A[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp44A,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long
_tmpD8=(_tmpD0 + _tmpD1)+ _tmpD2;unsigned int _tmp450;unsigned int _tmp44F;struct
_dyneither_ptr _tmp44E;char*_tmp44D;unsigned int _tmp44C;struct _dyneither_ptr s2=(
_tmp44C=_tmpD8 + 1,((_tmp44D=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp44C + 1)),((_tmp44E=_tag_dyneither(_tmp44D,sizeof(char),_tmp44C + 1),((((
_tmp44F=_tmp44C,((_tmp451(& _tmp450,& _tmp44F,& _tmp44D),_tmp44D[_tmp44F]=(char)0)))),
_tmp44E)))))));int _tmpD9=0;int _tmpDA=0;for(0;_tmpD9 < _tmpD0;++ _tmpD9){char _tmpDB=*((
const char*)_check_dyneither_subscript(s,sizeof(char),_tmpD9));if(_tmpDB == '\''
 || Cyc_is_other_special(_tmpDB)){char _tmp454;char _tmp453;struct _dyneither_ptr
_tmp452;(_tmp452=_dyneither_ptr_plus(s2,sizeof(char),_tmpDA ++),((_tmp453=*((char*)
_check_dyneither_subscript(_tmp452,sizeof(char),0)),((_tmp454='\\',((
_get_dyneither_size(_tmp452,sizeof(char))== 1  && (_tmp453 == '\000'  && _tmp454 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp452.curr)=_tmp454)))))));}{char _tmp457;char
_tmp456;struct _dyneither_ptr _tmp455;(_tmp455=_dyneither_ptr_plus(s2,sizeof(char),
_tmpDA ++),((_tmp456=*((char*)_check_dyneither_subscript(_tmp455,sizeof(char),0)),((
_tmp457=_tmpDB,((_get_dyneither_size(_tmp455,sizeof(char))== 1  && (_tmp456 == '\000'
 && _tmp457 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp455.curr)=_tmp457)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp458;return(_tmp458=_cycalloc(
sizeof(*_tmp458)),((_tmp458[0]=Cyc_sh_escape_string(*sp),_tmp458)));}static void
Cyc_process_file(struct _dyneither_ptr filename);static void Cyc_process_file(struct
_dyneither_ptr filename){struct _dyneither_ptr _tmpE7=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp459;struct _dyneither_ptr _tmpE8=Cyc_strconcat((
struct _dyneither_ptr)_tmpE7,((_tmp459=".cyp",_tag_dyneither(_tmp459,sizeof(char),
5))));const char*_tmp45A;struct _dyneither_ptr _tmpE9=Cyc_strconcat((struct
_dyneither_ptr)_tmpE7,((_tmp45A=".cyci",_tag_dyneither(_tmp45A,sizeof(char),6))));
const char*_tmp45B;struct _dyneither_ptr _tmpEA=Cyc_strconcat((struct _dyneither_ptr)
_tmpE7,((_tmp45B=".cycio",_tag_dyneither(_tmp45B,sizeof(char),7))));const char*
_tmp45C;struct _dyneither_ptr _tmpEB=Cyc_strconcat((struct _dyneither_ptr)_tmpE7,((
_tmp45C=".c",_tag_dyneither(_tmp45C,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp460;void*_tmp45F[1];struct Cyc_String_pa_struct _tmp45E;(_tmp45E.tag=0,((
_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp45F[0]=&
_tmp45E,Cyc_fprintf(Cyc_stderr,((_tmp460="Compiling %s\n",_tag_dyneither(_tmp460,
sizeof(char),14))),_tag_dyneither(_tmp45F,sizeof(void*),1)))))));}{const char*
_tmp462;const char*_tmp461;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp461="r",_tag_dyneither(_tmp461,sizeof(char),2))),((_tmp462="input file",
_tag_dyneither(_tmp462,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp469;
struct _dyneither_ptr*_tmp468;const char*_tmp467;struct Cyc_List_List*_tmp466;
struct _dyneither_ptr _tmpEF=Cyc_str_sepstr(((_tmp466=_cycalloc(sizeof(*_tmp466)),((
_tmp466->hd=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468[0]=(struct
_dyneither_ptr)((_tmp467="",_tag_dyneither(_tmp467,sizeof(char),1))),_tmp468)))),((
_tmp466->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp466)))))),((_tmp469=" ",_tag_dyneither(_tmp469,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpF0=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp46A;_tmpF0=Cyc_add_subdir(_tmpF0,((_tmp46A="include",
_tag_dyneither(_tmp46A,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp46D;struct Cyc_List_List*_tmp46C;
_tmpF0=((_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C->hd=((_tmp46D=_cycalloc(
sizeof(*_tmp46D)),((_tmp46D[0]=def_inc_path,_tmp46D)))),((_tmp46C->tl=_tmpF0,
_tmp46C))))));}{char*_tmpF4=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmpF4 != 0){char*_tmp46E;_tmpF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp46E=_tmpF4,
_tag_dyneither(_tmp46E,sizeof(char),_get_zero_arr_size_char(_tmp46E,1)))),':'),
_tmpF0);}{const char*_tmp475;struct _dyneither_ptr*_tmp474;const char*_tmp473;
struct Cyc_List_List*_tmp472;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472->hd=((
_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[0]=(struct _dyneither_ptr)((_tmp473="",
_tag_dyneither(_tmp473,sizeof(char),1))),_tmp474)))),((_tmp472->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpF0),_tmp472)))))),((_tmp475=" -I",_tag_dyneither(
_tmp475,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp479;void*_tmp478[1];struct Cyc_String_pa_struct
_tmp477;ofile_string=(struct _dyneither_ptr)((_tmp477.tag=0,((_tmp477.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp478[0]=& _tmp477,Cyc_aprintf(((_tmp479=" > %s",_tag_dyneither(_tmp479,sizeof(
char),6))),_tag_dyneither(_tmp478,sizeof(void*),1))))))));}else{const char*
_tmp47A;ofile_string=((_tmp47A="",_tag_dyneither(_tmp47A,sizeof(char),1)));}}
else{const char*_tmp47E;void*_tmp47D[1];struct Cyc_String_pa_struct _tmp47C;
ofile_string=(struct _dyneither_ptr)((_tmp47C.tag=0,((_tmp47C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpE8)),((_tmp47D[0]=& _tmp47C,Cyc_aprintf(((_tmp47E=" > %s",_tag_dyneither(
_tmp47E,sizeof(char),6))),_tag_dyneither(_tmp47D,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp47F;fixup_string=((_tmp47F=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp47F,sizeof(char),35)));}else{const char*_tmp483;void*_tmp482[1];
struct Cyc_String_pa_struct _tmp481;fixup_string=(struct _dyneither_ptr)((_tmp481.tag=
0,((_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp482[0]=& _tmp481,Cyc_aprintf(((
_tmp483=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp483,sizeof(char),29))),
_tag_dyneither(_tmp482,sizeof(void*),1))))))));}}else{const char*_tmp484;
fixup_string=((_tmp484="",_tag_dyneither(_tmp484,sizeof(char),1)));}{const char*
_tmp48D;void*_tmp48C[6];struct Cyc_String_pa_struct _tmp48B;struct Cyc_String_pa_struct
_tmp48A;struct Cyc_String_pa_struct _tmp489;struct Cyc_String_pa_struct _tmp488;
struct Cyc_String_pa_struct _tmp487;struct Cyc_String_pa_struct _tmp486;struct
_dyneither_ptr _tmp102=(_tmp486.tag=0,((_tmp486.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp487.tag=0,((_tmp487.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp488.tag=0,((_tmp488.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp489.tag=
0,((_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEF),((
_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp48C[0]=& _tmp48B,((_tmp48C[1]=& _tmp48A,((_tmp48C[2]=& _tmp489,((_tmp48C[3]=&
_tmp488,((_tmp48C[4]=& _tmp487,((_tmp48C[5]=& _tmp486,Cyc_aprintf(((_tmp48D="%s %s%s %s%s%s",
_tag_dyneither(_tmp48D,sizeof(char),15))),_tag_dyneither(_tmp48C,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp491;void*_tmp490[1];struct Cyc_String_pa_struct _tmp48F;(
_tmp48F.tag=0,((_tmp48F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp102),((
_tmp490[0]=& _tmp48F,Cyc_fprintf(Cyc_stderr,((_tmp491="%s\n",_tag_dyneither(
_tmp491,sizeof(char),4))),_tag_dyneither(_tmp490,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp102,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp494;void*_tmp493;(_tmp493=0,Cyc_fprintf(Cyc_stderr,((_tmp494="\nError: preprocessing\n",
_tag_dyneither(_tmp494,sizeof(char),23))),_tag_dyneither(_tmp493,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpE8);{const char*_tmp496;const char*_tmp495;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpE8,((_tmp495="r",
_tag_dyneither(_tmp495,sizeof(char),2))),((_tmp496="file",_tag_dyneither(_tmp496,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp108;_push_handler(& _tmp108);{int _tmp10A=0;if(setjmp(
_tmp108.handler))_tmp10A=1;if(!_tmp10A){{const char*_tmp497;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp497="parsing",_tag_dyneither(_tmp497,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp109=(
void*)_exn_thrown;void*_tmp10D=_tmp109;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpE8);(int)_throw(
_tmp10D);_LL29:;_LL2A:(void)_throw(_tmp10D);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE8);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp10E=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp10E;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp10F=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple19 _tmp498;struct _tuple19 _tmp110=(_tmp498.f1=
tc_rgn,((_tmp498.f2=_tmp10F,_tmp498)));{const char*_tmp499;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple19*,struct
Cyc_List_List*),struct _tuple19*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp499="type checking",_tag_dyneither(_tmp499,sizeof(char),14))),Cyc_do_typecheck,&
_tmp110,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpE8);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp49A;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp49A="control-flow checking",_tag_dyneither(_tmp49A,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE8);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpE9,sizeof(char),1)),(const char*)"r");
const char*_tmp49C;const char*_tmp49B;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpEA,((_tmp49B="w",_tag_dyneither(_tmp49B,sizeof(char),2))),((
_tmp49C="interface object file",_tag_dyneither(_tmp49C,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpE9);{struct _tuple20 _tmp49D;struct _tuple20 _tmp113=(
_tmp49D.f1=_tmp10F,((_tmp49D.f2=inter_file,((_tmp49D.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp49D)))));{const char*_tmp49E;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp49E="interface checking",_tag_dyneither(_tmp49E,sizeof(char),19))),Cyc_do_interface,&
_tmp113,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp49F;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp49F="translation to C",_tag_dyneither(_tmp49F,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpE8);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpE8);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp4A0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4A0="post-pass to VC",_tag_dyneither(_tmp4A0,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp4A2;const char*_tmp4A1;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4A1="w",_tag_dyneither(
_tmp4A1,sizeof(char),2))),((_tmp4A2="output file",_tag_dyneither(_tmp4A2,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp4A4;const char*_tmp4A3;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4A3="w",_tag_dyneither(
_tmp4A3,sizeof(char),2))),((_tmp4A4="output file",_tag_dyneither(_tmp4A4,sizeof(
char),12))));}else{const char*_tmp4A6;const char*_tmp4A5;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpEB,((_tmp4A5="w",_tag_dyneither(_tmp4A5,sizeof(char),2))),((
_tmp4A6="output file",_tag_dyneither(_tmp4A6,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp121;_push_handler(& _tmp121);{int _tmp123=0;if(setjmp(_tmp121.handler))_tmp123=
1;if(!_tmp123){if(!Cyc_noprint_r){const char*_tmp4A7;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4A7="printing",_tag_dyneither(_tmp4A7,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp122=(void*)
_exn_thrown;void*_tmp126=_tmp122;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp4AA;struct Cyc_List_List*
_tmp4A9;Cyc_cfiles=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9->hd=((_tmp4AA=
_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA[0]=(struct _dyneither_ptr)_tmpEB,_tmp4AA)))),((
_tmp4A9->tl=Cyc_cfiles,_tmp4A9))))));}(int)_throw(_tmp126);_LL2E:;_LL2F:(void)
_throw(_tmp126);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp4AD;struct Cyc_List_List*_tmp4AC;Cyc_cfiles=((_tmp4AC=
_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC->hd=((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((
_tmp4AD[0]=(struct _dyneither_ptr)_tmpEB,_tmp4AD)))),((_tmp4AC->tl=Cyc_cfiles,
_tmp4AC))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp143[8]="<final>";
static struct _dyneither_ptr Cyc_final_str={_tmp143,_tmp143,_tmp143 + 8};static
struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*
Cyc_read_cycio(struct _dyneither_ptr*n);static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _dyneither_ptr basename;{struct _handler_cons _tmp144;_push_handler(& _tmp144);{
int _tmp146=0;if(setjmp(_tmp144.handler))_tmp146=1;if(!_tmp146){basename=(struct
_dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp145=(void*)_exn_thrown;void*_tmp148=_tmp145;_LL31: {struct Cyc_Core_Invalid_argument_struct*
_tmp149=(struct Cyc_Core_Invalid_argument_struct*)_tmp148;if(_tmp149->tag != Cyc_Core_Invalid_argument)
goto _LL33;}_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmp148);_LL30:;}};}{
const char*_tmp4AE;struct _dyneither_ptr _tmp14A=Cyc_strconcat((struct
_dyneither_ptr)basename,((_tmp4AE=".cycio",_tag_dyneither(_tmp4AE,sizeof(char),7))));
const char*_tmp4B0;const char*_tmp4AF;struct Cyc___cycFILE*_tmp14B=Cyc_try_file_open((
struct _dyneither_ptr)_tmp14A,((_tmp4AF="rb",_tag_dyneither(_tmp4AF,sizeof(char),
3))),((_tmp4B0="interface object file",_tag_dyneither(_tmp4B0,sizeof(char),22))));
if(_tmp14B == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp14A);{struct Cyc_Interface_I*_tmp14C=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp14B);Cyc_file_close((struct Cyc___cycFILE*)_tmp14B);
return _tmp14C;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n);static int Cyc_is_cfile(
struct _dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple21{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4B6(unsigned int*
_tmp4B5,unsigned int*_tmp4B4,int**_tmp4B2){for(*_tmp4B5=0;*_tmp4B5 < *_tmp4B4;(*
_tmp4B5)++){(*_tmp4B2)[*_tmp4B5]=0;}}static void _tmp4BE(unsigned int*_tmp4BD,
unsigned int*_tmp4BC,struct _dyneither_ptr**_tmp4BA){for(*_tmp4BD=0;*_tmp4BD < *
_tmp4BC;(*_tmp4BD)++){(*_tmp4BA)[*_tmp4BD]=(struct _dyneither_ptr)_tag_dyneither(
0,0,0);}}static void _tmp4C4(unsigned int*_tmp4C3,unsigned int*_tmp4C2,struct
_dyneither_ptr**_tmp4C0){for(*_tmp4C3=0;*_tmp4C3 < *_tmp4C2;(*_tmp4C3)++){(*
_tmp4C0)[*_tmp4C3]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp150=
_new_region("r");struct _RegionHandle*r=& _tmp150;_push_region(r);{unsigned int
_tmp4B5;unsigned int _tmp4B4;struct _dyneither_ptr _tmp4B3;int*_tmp4B2;unsigned int
_tmp4B1;struct _dyneither_ptr _tmp151=(_tmp4B1=(unsigned int)argc,((_tmp4B2=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp4B1)),((_tmp4B3=_tag_dyneither(
_tmp4B2,sizeof(int),_tmp4B1),((((_tmp4B4=_tmp4B1,_tmp4B6(& _tmp4B5,& _tmp4B4,&
_tmp4B2))),_tmp4B3)))))));int _tmp152=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp4B7;if(Cyc_strncmp(((_tmp4B7="-B",_tag_dyneither(_tmp4B7,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp151,sizeof(int),i))=1;++ _tmp152;}else{const char*_tmp4B8;if(Cyc_strcmp(((
_tmp4B8="-b",_tag_dyneither(_tmp4B8,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp151,sizeof(int),
i))=1;++ _tmp152;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp151,
sizeof(int),i))=1;++ _tmp152;}}}{unsigned int _tmp4BD;unsigned int _tmp4BC;struct
_dyneither_ptr _tmp4BB;struct _dyneither_ptr*_tmp4BA;unsigned int _tmp4B9;struct
_dyneither_ptr _tmp155=(_tmp4B9=(unsigned int)(_tmp152 + 1),((_tmp4BA=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4B9)),((
_tmp4BB=_tag_dyneither(_tmp4BA,sizeof(struct _dyneither_ptr),_tmp4B9),((((_tmp4BC=
_tmp4B9,_tmp4BE(& _tmp4BD,& _tmp4BC,& _tmp4BA))),_tmp4BB)))))));unsigned int _tmp4C3;
unsigned int _tmp4C2;struct _dyneither_ptr _tmp4C1;struct _dyneither_ptr*_tmp4C0;
unsigned int _tmp4BF;struct _dyneither_ptr _tmp156=(_tmp4BF=(unsigned int)(argc - 
_tmp152),((_tmp4C0=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp4BF)),((_tmp4C1=_tag_dyneither(_tmp4C0,sizeof(struct
_dyneither_ptr),_tmp4BF),((((_tmp4C2=_tmp4BF,_tmp4C4(& _tmp4C3,& _tmp4C2,& _tmp4C0))),
_tmp4C1)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp155,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp156,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp151,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp155,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp156,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple21*_tmp969;const char*_tmp968;const char*
_tmp967;struct Cyc_Arg_Set_spec_struct _tmp966;struct Cyc_Arg_Set_spec_struct*
_tmp965;const char*_tmp964;struct _tuple21*_tmp963;const char*_tmp962;const char*
_tmp961;struct Cyc_Arg_Unit_spec_struct _tmp960;struct Cyc_Arg_Unit_spec_struct*
_tmp95F;const char*_tmp95E;struct _tuple21*_tmp95D;const char*_tmp95C;const char*
_tmp95B;struct Cyc_Arg_String_spec_struct _tmp95A;struct Cyc_Arg_String_spec_struct*
_tmp959;const char*_tmp958;struct _tuple21*_tmp957;const char*_tmp956;const char*
_tmp955;struct Cyc_Arg_Flag_spec_struct _tmp954;struct Cyc_Arg_Flag_spec_struct*
_tmp953;const char*_tmp952;struct _tuple21*_tmp951;const char*_tmp950;const char*
_tmp94F;struct Cyc_Arg_Flag_spec_struct _tmp94E;struct Cyc_Arg_Flag_spec_struct*
_tmp94D;const char*_tmp94C;struct _tuple21*_tmp94B;const char*_tmp94A;const char*
_tmp949;struct Cyc_Arg_Flag_spec_struct _tmp948;struct Cyc_Arg_Flag_spec_struct*
_tmp947;const char*_tmp946;struct _tuple21*_tmp945;const char*_tmp944;const char*
_tmp943;struct Cyc_Arg_Flag_spec_struct _tmp942;struct Cyc_Arg_Flag_spec_struct*
_tmp941;const char*_tmp940;struct _tuple21*_tmp93F;const char*_tmp93E;const char*
_tmp93D;struct Cyc_Arg_Flag_spec_struct _tmp93C;struct Cyc_Arg_Flag_spec_struct*
_tmp93B;const char*_tmp93A;struct _tuple21*_tmp939;const char*_tmp938;const char*
_tmp937;struct Cyc_Arg_Flag_spec_struct _tmp936;struct Cyc_Arg_Flag_spec_struct*
_tmp935;const char*_tmp934;struct _tuple21*_tmp933;const char*_tmp932;const char*
_tmp931;struct Cyc_Arg_Unit_spec_struct _tmp930;struct Cyc_Arg_Unit_spec_struct*
_tmp92F;const char*_tmp92E;struct _tuple21*_tmp92D;const char*_tmp92C;const char*
_tmp92B;struct Cyc_Arg_String_spec_struct _tmp92A;struct Cyc_Arg_String_spec_struct*
_tmp929;const char*_tmp928;struct _tuple21*_tmp927;const char*_tmp926;const char*
_tmp925;struct Cyc_Arg_Flag_spec_struct _tmp924;struct Cyc_Arg_Flag_spec_struct*
_tmp923;const char*_tmp922;struct _tuple21*_tmp921;const char*_tmp920;const char*
_tmp91F;struct Cyc_Arg_Flag_spec_struct _tmp91E;struct Cyc_Arg_Flag_spec_struct*
_tmp91D;const char*_tmp91C;struct _tuple21*_tmp91B;const char*_tmp91A;const char*
_tmp919;struct Cyc_Arg_Flag_spec_struct _tmp918;struct Cyc_Arg_Flag_spec_struct*
_tmp917;const char*_tmp916;struct _tuple21*_tmp915;const char*_tmp914;const char*
_tmp913;struct Cyc_Arg_Flag_spec_struct _tmp912;struct Cyc_Arg_Flag_spec_struct*
_tmp911;const char*_tmp910;struct _tuple21*_tmp90F;const char*_tmp90E;const char*
_tmp90D;struct Cyc_Arg_Flag_spec_struct _tmp90C;struct Cyc_Arg_Flag_spec_struct*
_tmp90B;const char*_tmp90A;struct _tuple21*_tmp909;const char*_tmp908;const char*
_tmp907;struct Cyc_Arg_Flag_spec_struct _tmp906;struct Cyc_Arg_Flag_spec_struct*
_tmp905;const char*_tmp904;struct _tuple21*_tmp903;const char*_tmp902;const char*
_tmp901;struct Cyc_Arg_Flag_spec_struct _tmp900;struct Cyc_Arg_Flag_spec_struct*
_tmp8FF;const char*_tmp8FE;struct _tuple21*_tmp8FD;const char*_tmp8FC;const char*
_tmp8FB;struct Cyc_Arg_Flag_spec_struct _tmp8FA;struct Cyc_Arg_Flag_spec_struct*
_tmp8F9;const char*_tmp8F8;struct _tuple21*_tmp8F7;const char*_tmp8F6;const char*
_tmp8F5;struct Cyc_Arg_Unit_spec_struct _tmp8F4;struct Cyc_Arg_Unit_spec_struct*
_tmp8F3;const char*_tmp8F2;struct _tuple21*_tmp8F1;const char*_tmp8F0;const char*
_tmp8EF;struct Cyc_Arg_Unit_spec_struct _tmp8EE;struct Cyc_Arg_Unit_spec_struct*
_tmp8ED;const char*_tmp8EC;struct _tuple21*_tmp8EB;const char*_tmp8EA;const char*
_tmp8E9;struct Cyc_Arg_Unit_spec_struct _tmp8E8;struct Cyc_Arg_Unit_spec_struct*
_tmp8E7;const char*_tmp8E6;struct _tuple21*_tmp8E5;const char*_tmp8E4;const char*
_tmp8E3;struct Cyc_Arg_Unit_spec_struct _tmp8E2;struct Cyc_Arg_Unit_spec_struct*
_tmp8E1;const char*_tmp8E0;struct _tuple21*_tmp8DF;const char*_tmp8DE;const char*
_tmp8DD;struct Cyc_Arg_Flag_spec_struct _tmp8DC;struct Cyc_Arg_Flag_spec_struct*
_tmp8DB;const char*_tmp8DA;struct _tuple21*_tmp8D9;const char*_tmp8D8;const char*
_tmp8D7;struct Cyc_Arg_String_spec_struct _tmp8D6;struct Cyc_Arg_String_spec_struct*
_tmp8D5;const char*_tmp8D4;struct _tuple21*_tmp8D3;const char*_tmp8D2;const char*
_tmp8D1;struct Cyc_Arg_String_spec_struct _tmp8D0;struct Cyc_Arg_String_spec_struct*
_tmp8CF;const char*_tmp8CE;struct _tuple21*_tmp8CD;const char*_tmp8CC;const char*
_tmp8CB;struct Cyc_Arg_Unit_spec_struct _tmp8CA;struct Cyc_Arg_Unit_spec_struct*
_tmp8C9;const char*_tmp8C8;struct _tuple21*_tmp8C7;const char*_tmp8C6;const char*
_tmp8C5;struct Cyc_Arg_Set_spec_struct _tmp8C4;struct Cyc_Arg_Set_spec_struct*
_tmp8C3;const char*_tmp8C2;struct _tuple21*_tmp8C1;const char*_tmp8C0;const char*
_tmp8BF;struct Cyc_Arg_Set_spec_struct _tmp8BE;struct Cyc_Arg_Set_spec_struct*
_tmp8BD;const char*_tmp8BC;struct _tuple21*_tmp8BB;const char*_tmp8BA;const char*
_tmp8B9;struct Cyc_Arg_Set_spec_struct _tmp8B8;struct Cyc_Arg_Set_spec_struct*
_tmp8B7;const char*_tmp8B6;struct _tuple21*_tmp8B5;const char*_tmp8B4;const char*
_tmp8B3;struct Cyc_Arg_Set_spec_struct _tmp8B2;struct Cyc_Arg_Set_spec_struct*
_tmp8B1;const char*_tmp8B0;struct _tuple21*_tmp8AF;const char*_tmp8AE;const char*
_tmp8AD;struct Cyc_Arg_Set_spec_struct _tmp8AC;struct Cyc_Arg_Set_spec_struct*
_tmp8AB;const char*_tmp8AA;struct _tuple21*_tmp8A9;const char*_tmp8A8;const char*
_tmp8A7;struct Cyc_Arg_Set_spec_struct _tmp8A6;struct Cyc_Arg_Set_spec_struct*
_tmp8A5;const char*_tmp8A4;struct _tuple21*_tmp8A3;const char*_tmp8A2;const char*
_tmp8A1;struct Cyc_Arg_Set_spec_struct _tmp8A0;struct Cyc_Arg_Set_spec_struct*
_tmp89F;const char*_tmp89E;struct _tuple21*_tmp89D;const char*_tmp89C;const char*
_tmp89B;struct Cyc_Arg_Set_spec_struct _tmp89A;struct Cyc_Arg_Set_spec_struct*
_tmp899;const char*_tmp898;struct _tuple21*_tmp897;const char*_tmp896;const char*
_tmp895;struct Cyc_Arg_Clear_spec_struct _tmp894;struct Cyc_Arg_Clear_spec_struct*
_tmp893;const char*_tmp892;struct _tuple21*_tmp891;const char*_tmp890;const char*
_tmp88F;struct Cyc_Arg_Set_spec_struct _tmp88E;struct Cyc_Arg_Set_spec_struct*
_tmp88D;const char*_tmp88C;struct _tuple21*_tmp88B;const char*_tmp88A;const char*
_tmp889;struct Cyc_Arg_Clear_spec_struct _tmp888;struct Cyc_Arg_Clear_spec_struct*
_tmp887;const char*_tmp886;struct _tuple21*_tmp885;const char*_tmp884;const char*
_tmp883;struct Cyc_Arg_Unit_spec_struct _tmp882;struct Cyc_Arg_Unit_spec_struct*
_tmp881;const char*_tmp880;struct _tuple21*_tmp87F;const char*_tmp87E;const char*
_tmp87D;struct Cyc_Arg_String_spec_struct _tmp87C;struct Cyc_Arg_String_spec_struct*
_tmp87B;const char*_tmp87A;struct _tuple21*_tmp879;const char*_tmp878;const char*
_tmp877;struct Cyc_Arg_Unit_spec_struct _tmp876;struct Cyc_Arg_Unit_spec_struct*
_tmp875;const char*_tmp874;struct _tuple21*_tmp873;const char*_tmp872;const char*
_tmp871;struct Cyc_Arg_Set_spec_struct _tmp870;struct Cyc_Arg_Set_spec_struct*
_tmp86F;const char*_tmp86E;struct _tuple21*_tmp86D;const char*_tmp86C;const char*
_tmp86B;struct Cyc_Arg_Unit_spec_struct _tmp86A;struct Cyc_Arg_Unit_spec_struct*
_tmp869;const char*_tmp868;struct _tuple21*_tmp867;const char*_tmp866;const char*
_tmp865;struct Cyc_Arg_Unit_spec_struct _tmp864;struct Cyc_Arg_Unit_spec_struct*
_tmp863;const char*_tmp862;struct _tuple21*_tmp861;const char*_tmp860;const char*
_tmp85F;struct Cyc_Arg_Unit_spec_struct _tmp85E;struct Cyc_Arg_Unit_spec_struct*
_tmp85D;const char*_tmp85C;struct _tuple21*_tmp85B;const char*_tmp85A;const char*
_tmp859;struct Cyc_Arg_Unit_spec_struct _tmp858;struct Cyc_Arg_Unit_spec_struct*
_tmp857;const char*_tmp856;struct _tuple21*_tmp855;const char*_tmp854;const char*
_tmp853;struct Cyc_Arg_Unit_spec_struct _tmp852;struct Cyc_Arg_Unit_spec_struct*
_tmp851;const char*_tmp850;struct _tuple21*_tmp84F;const char*_tmp84E;const char*
_tmp84D;struct Cyc_Arg_String_spec_struct _tmp84C;struct Cyc_Arg_String_spec_struct*
_tmp84B;const char*_tmp84A;struct _tuple21*_tmp849;const char*_tmp848;const char*
_tmp847;struct Cyc_Arg_Unit_spec_struct _tmp846;struct Cyc_Arg_Unit_spec_struct*
_tmp845;const char*_tmp844;struct _tuple21*_tmp843;const char*_tmp842;const char*
_tmp841;struct Cyc_Arg_Unit_spec_struct _tmp840;struct Cyc_Arg_Unit_spec_struct*
_tmp83F;const char*_tmp83E;struct _tuple21*_tmp83D;const char*_tmp83C;const char*
_tmp83B;struct Cyc_Arg_Set_spec_struct _tmp83A;struct Cyc_Arg_Set_spec_struct*
_tmp839;const char*_tmp838;struct _tuple21*_tmp837;const char*_tmp836;const char*
_tmp835;struct Cyc_Arg_Set_spec_struct _tmp834;struct Cyc_Arg_Set_spec_struct*
_tmp833;const char*_tmp832;struct _tuple21*_tmp831;const char*_tmp830;const char*
_tmp82F;struct Cyc_Arg_Set_spec_struct _tmp82E;struct Cyc_Arg_Set_spec_struct*
_tmp82D;const char*_tmp82C;struct _tuple21*_tmp82B;const char*_tmp82A;const char*
_tmp829;struct Cyc_Arg_Set_spec_struct _tmp828;struct Cyc_Arg_Set_spec_struct*
_tmp827;const char*_tmp826;struct _tuple21*_tmp825;const char*_tmp824;const char*
_tmp823;struct Cyc_Arg_Set_spec_struct _tmp822;struct Cyc_Arg_Set_spec_struct*
_tmp821;const char*_tmp820;struct _tuple21*_tmp81F;const char*_tmp81E;const char*
_tmp81D;struct Cyc_Arg_Set_spec_struct _tmp81C;struct Cyc_Arg_Set_spec_struct*
_tmp81B;const char*_tmp81A;struct _tuple21*_tmp819;const char*_tmp818;const char*
_tmp817;struct Cyc_Arg_Set_spec_struct _tmp816;struct Cyc_Arg_Set_spec_struct*
_tmp815;const char*_tmp814;struct _tuple21*_tmp813;const char*_tmp812;const char*
_tmp811;struct Cyc_Arg_Set_spec_struct _tmp810;struct Cyc_Arg_Set_spec_struct*
_tmp80F;const char*_tmp80E;struct _tuple21*_tmp80D;const char*_tmp80C;const char*
_tmp80B;struct Cyc_Arg_Set_spec_struct _tmp80A;struct Cyc_Arg_Set_spec_struct*
_tmp809;const char*_tmp808;struct _tuple21*_tmp807;const char*_tmp806;const char*
_tmp805;struct Cyc_Arg_String_spec_struct _tmp804;struct Cyc_Arg_String_spec_struct*
_tmp803;const char*_tmp802;struct _tuple21*_tmp801;const char*_tmp800;const char*
_tmp7FF;struct Cyc_Arg_Set_spec_struct _tmp7FE;struct Cyc_Arg_Set_spec_struct*
_tmp7FD;const char*_tmp7FC;struct _tuple21*_tmp7FB;const char*_tmp7FA;const char*
_tmp7F9;struct Cyc_Arg_Set_spec_struct _tmp7F8;struct Cyc_Arg_Set_spec_struct*
_tmp7F7;const char*_tmp7F6;struct _tuple21*_tmp7F5;const char*_tmp7F4;const char*
_tmp7F3;struct Cyc_Arg_Set_spec_struct _tmp7F2;struct Cyc_Arg_Set_spec_struct*
_tmp7F1;const char*_tmp7F0;struct _tuple21*_tmp7EF;const char*_tmp7EE;const char*
_tmp7ED;struct Cyc_Arg_Clear_spec_struct _tmp7EC;struct Cyc_Arg_Clear_spec_struct*
_tmp7EB;const char*_tmp7EA;struct _tuple21*_tmp7E9;const char*_tmp7E8;const char*
_tmp7E7;struct Cyc_Arg_Unit_spec_struct _tmp7E6;struct Cyc_Arg_Unit_spec_struct*
_tmp7E5;const char*_tmp7E4;struct _tuple21*_tmp7E3;const char*_tmp7E2;const char*
_tmp7E1;struct Cyc_Arg_Set_spec_struct _tmp7E0;struct Cyc_Arg_Set_spec_struct*
_tmp7DF;const char*_tmp7DE;struct _tuple21*_tmp7DD[66];struct Cyc_List_List*options=(
_tmp7DD[65]=((_tmp7E3=_region_malloc(r,sizeof(*_tmp7E3)),((_tmp7E3->f1=((_tmp7E2="-noregions",
_tag_dyneither(_tmp7E2,sizeof(char),11))),((_tmp7E3->f2=0,((_tmp7E3->f3=((
_tmp7E1="",_tag_dyneither(_tmp7E1,sizeof(char),1))),((_tmp7E3->f4=(void*)((
_tmp7DF=_region_malloc(r,sizeof(*_tmp7DF)),((_tmp7DF[0]=((_tmp7E0.tag=3,((
_tmp7E0.f1=& Cyc_Absyn_no_regions,_tmp7E0)))),_tmp7DF)))),((_tmp7E3->f5=((_tmp7DE="Generate code that doesn't use regions",
_tag_dyneither(_tmp7DE,sizeof(char),39))),_tmp7E3)))))))))))),((_tmp7DD[64]=((
_tmp7E9=_region_malloc(r,sizeof(*_tmp7E9)),((_tmp7E9->f1=((_tmp7E8="-port",
_tag_dyneither(_tmp7E8,sizeof(char),6))),((_tmp7E9->f2=0,((_tmp7E9->f3=((_tmp7E7="",
_tag_dyneither(_tmp7E7,sizeof(char),1))),((_tmp7E9->f4=(void*)((_tmp7E5=
_region_malloc(r,sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E6.tag=0,((_tmp7E6.f1=Cyc_set_port_c_code,
_tmp7E6)))),_tmp7E5)))),((_tmp7E9->f5=((_tmp7E4="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp7E4,sizeof(char),38))),_tmp7E9)))))))))))),((_tmp7DD[63]=((
_tmp7EF=_region_malloc(r,sizeof(*_tmp7EF)),((_tmp7EF->f1=((_tmp7EE="-detailedlocation",
_tag_dyneither(_tmp7EE,sizeof(char),18))),((_tmp7EF->f2=0,((_tmp7EF->f3=((
_tmp7ED="",_tag_dyneither(_tmp7ED,sizeof(char),1))),((_tmp7EF->f4=(void*)((
_tmp7EB=_region_malloc(r,sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7EC.tag=4,((
_tmp7EC.f1=& Cyc_Position_use_gcc_style_location,_tmp7EC)))),_tmp7EB)))),((
_tmp7EF->f5=((_tmp7EA="Try to give more detailed location information for errors",
_tag_dyneither(_tmp7EA,sizeof(char),58))),_tmp7EF)))))))))))),((_tmp7DD[62]=((
_tmp7F5=_region_malloc(r,sizeof(*_tmp7F5)),((_tmp7F5->f1=((_tmp7F4="-noregister",
_tag_dyneither(_tmp7F4,sizeof(char),12))),((_tmp7F5->f2=0,((_tmp7F5->f3=((
_tmp7F3="",_tag_dyneither(_tmp7F3,sizeof(char),1))),((_tmp7F5->f4=(void*)((
_tmp7F1=_region_malloc(r,sizeof(*_tmp7F1)),((_tmp7F1[0]=((_tmp7F2.tag=3,((
_tmp7F2.f1=& Cyc_Parse_no_register,_tmp7F2)))),_tmp7F1)))),((_tmp7F5->f5=((
_tmp7F0="Treat register storage class as public",_tag_dyneither(_tmp7F0,sizeof(
char),39))),_tmp7F5)))))))))))),((_tmp7DD[61]=((_tmp7FB=_region_malloc(r,sizeof(*
_tmp7FB)),((_tmp7FB->f1=((_tmp7FA="-warnnullchecks",_tag_dyneither(_tmp7FA,
sizeof(char),16))),((_tmp7FB->f2=0,((_tmp7FB->f3=((_tmp7F9="",_tag_dyneither(
_tmp7F9,sizeof(char),1))),((_tmp7FB->f4=(void*)((_tmp7F7=_region_malloc(r,
sizeof(*_tmp7F7)),((_tmp7F7[0]=((_tmp7F8.tag=3,((_tmp7F8.f1=& Cyc_Toc_warn_all_null_deref,
_tmp7F8)))),_tmp7F7)))),((_tmp7FB->f5=((_tmp7F6="Warn when any null check can't be eliminated",
_tag_dyneither(_tmp7F6,sizeof(char),45))),_tmp7FB)))))))))))),((_tmp7DD[60]=((
_tmp801=_region_malloc(r,sizeof(*_tmp801)),((_tmp801->f1=((_tmp800="-warnboundschecks",
_tag_dyneither(_tmp800,sizeof(char),18))),((_tmp801->f2=0,((_tmp801->f3=((
_tmp7FF="",_tag_dyneither(_tmp7FF,sizeof(char),1))),((_tmp801->f4=(void*)((
_tmp7FD=_region_malloc(r,sizeof(*_tmp7FD)),((_tmp7FD[0]=((_tmp7FE.tag=3,((
_tmp7FE.f1=& Cyc_Toc_warn_bounds_checks,_tmp7FE)))),_tmp7FD)))),((_tmp801->f5=((
_tmp7FC="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp7FC,
sizeof(char),44))),_tmp801)))))))))))),((_tmp7DD[59]=((_tmp807=_region_malloc(r,
sizeof(*_tmp807)),((_tmp807->f1=((_tmp806="-CI",_tag_dyneither(_tmp806,sizeof(
char),4))),((_tmp807->f2=0,((_tmp807->f3=((_tmp805=" <file>",_tag_dyneither(
_tmp805,sizeof(char),8))),((_tmp807->f4=(void*)((_tmp803=_region_malloc(r,
sizeof(*_tmp803)),((_tmp803[0]=((_tmp804.tag=5,((_tmp804.f1=Cyc_set_cyc_include,
_tmp804)))),_tmp803)))),((_tmp807->f5=((_tmp802="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp802,sizeof(char),31))),_tmp807)))))))))))),((_tmp7DD[58]=((
_tmp80D=_region_malloc(r,sizeof(*_tmp80D)),((_tmp80D->f1=((_tmp80C="-compile-for-boot",
_tag_dyneither(_tmp80C,sizeof(char),18))),((_tmp80D->f2=0,((_tmp80D->f3=((
_tmp80B="",_tag_dyneither(_tmp80B,sizeof(char),1))),((_tmp80D->f4=(void*)((
_tmp809=_region_malloc(r,sizeof(*_tmp809)),((_tmp809[0]=((_tmp80A.tag=3,((
_tmp80A.f1=& Cyc_compile_for_boot_r,_tmp80A)))),_tmp809)))),((_tmp80D->f5=((
_tmp808="Compile using the special boot library instead of the standard library",
_tag_dyneither(_tmp808,sizeof(char),71))),_tmp80D)))))))))))),((_tmp7DD[57]=((
_tmp813=_region_malloc(r,sizeof(*_tmp813)),((_tmp813->f1=((_tmp812="-nocyc_setjmp",
_tag_dyneither(_tmp812,sizeof(char),14))),((_tmp813->f2=0,((_tmp813->f3=((
_tmp811="",_tag_dyneither(_tmp811,sizeof(char),1))),((_tmp813->f4=(void*)((
_tmp80F=_region_malloc(r,sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=3,((
_tmp810.f1=& Cyc_nocyc_setjmp_r,_tmp810)))),_tmp80F)))),((_tmp813->f5=((_tmp80E="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp80E,sizeof(char),46))),_tmp813)))))))))))),((_tmp7DD[56]=((
_tmp819=_region_malloc(r,sizeof(*_tmp819)),((_tmp819->f1=((_tmp818="-printalleffects",
_tag_dyneither(_tmp818,sizeof(char),17))),((_tmp819->f2=0,((_tmp819->f3=((
_tmp817="",_tag_dyneither(_tmp817,sizeof(char),1))),((_tmp819->f4=(void*)((
_tmp815=_region_malloc(r,sizeof(*_tmp815)),((_tmp815[0]=((_tmp816.tag=3,((
_tmp816.f1=& Cyc_print_all_effects_r,_tmp816)))),_tmp815)))),((_tmp819->f5=((
_tmp814="Print effects for functions (type debugging)",_tag_dyneither(_tmp814,
sizeof(char),45))),_tmp819)))))))))))),((_tmp7DD[55]=((_tmp81F=_region_malloc(r,
sizeof(*_tmp81F)),((_tmp81F->f1=((_tmp81E="-printfullevars",_tag_dyneither(
_tmp81E,sizeof(char),16))),((_tmp81F->f2=0,((_tmp81F->f3=((_tmp81D="",
_tag_dyneither(_tmp81D,sizeof(char),1))),((_tmp81F->f4=(void*)((_tmp81B=
_region_malloc(r,sizeof(*_tmp81B)),((_tmp81B[0]=((_tmp81C.tag=3,((_tmp81C.f1=&
Cyc_print_full_evars_r,_tmp81C)))),_tmp81B)))),((_tmp81F->f5=((_tmp81A="Print full information for evars (type debugging)",
_tag_dyneither(_tmp81A,sizeof(char),50))),_tmp81F)))))))))))),((_tmp7DD[54]=((
_tmp825=_region_malloc(r,sizeof(*_tmp825)),((_tmp825->f1=((_tmp824="-printallkinds",
_tag_dyneither(_tmp824,sizeof(char),15))),((_tmp825->f2=0,((_tmp825->f3=((
_tmp823="",_tag_dyneither(_tmp823,sizeof(char),1))),((_tmp825->f4=(void*)((
_tmp821=_region_malloc(r,sizeof(*_tmp821)),((_tmp821[0]=((_tmp822.tag=3,((
_tmp822.f1=& Cyc_print_all_kinds_r,_tmp822)))),_tmp821)))),((_tmp825->f5=((
_tmp820="Always print kinds of type variables",_tag_dyneither(_tmp820,sizeof(
char),37))),_tmp825)))))))))))),((_tmp7DD[53]=((_tmp82B=_region_malloc(r,sizeof(*
_tmp82B)),((_tmp82B->f1=((_tmp82A="-printalltvars",_tag_dyneither(_tmp82A,
sizeof(char),15))),((_tmp82B->f2=0,((_tmp82B->f3=((_tmp829="",_tag_dyneither(
_tmp829,sizeof(char),1))),((_tmp82B->f4=(void*)((_tmp827=_region_malloc(r,
sizeof(*_tmp827)),((_tmp827[0]=((_tmp828.tag=3,((_tmp828.f1=& Cyc_print_all_tvars_r,
_tmp828)))),_tmp827)))),((_tmp82B->f5=((_tmp826="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp826,sizeof(char),57))),_tmp82B)))))))))))),((_tmp7DD[52]=((
_tmp831=_region_malloc(r,sizeof(*_tmp831)),((_tmp831->f1=((_tmp830="-noexpandtypedefs",
_tag_dyneither(_tmp830,sizeof(char),18))),((_tmp831->f2=0,((_tmp831->f3=((
_tmp82F="",_tag_dyneither(_tmp82F,sizeof(char),1))),((_tmp831->f4=(void*)((
_tmp82D=_region_malloc(r,sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82E.tag=3,((
_tmp82E.f1=& Cyc_noexpand_r,_tmp82E)))),_tmp82D)))),((_tmp831->f5=((_tmp82C="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp82C,sizeof(char),41))),_tmp831)))))))))))),((_tmp7DD[51]=((
_tmp837=_region_malloc(r,sizeof(*_tmp837)),((_tmp837->f1=((_tmp836="-noremoveunused",
_tag_dyneither(_tmp836,sizeof(char),16))),((_tmp837->f2=0,((_tmp837->f3=((
_tmp835="",_tag_dyneither(_tmp835,sizeof(char),1))),((_tmp837->f4=(void*)((
_tmp833=_region_malloc(r,sizeof(*_tmp833)),((_tmp833[0]=((_tmp834.tag=3,((
_tmp834.f1=& Cyc_noshake_r,_tmp834)))),_tmp833)))),((_tmp837->f5=((_tmp832="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp832,sizeof(char),49))),_tmp837)))))))))))),((_tmp7DD[50]=((
_tmp83D=_region_malloc(r,sizeof(*_tmp83D)),((_tmp83D->f1=((_tmp83C="-nogc",
_tag_dyneither(_tmp83C,sizeof(char),6))),((_tmp83D->f2=0,((_tmp83D->f3=((_tmp83B="",
_tag_dyneither(_tmp83B,sizeof(char),1))),((_tmp83D->f4=(void*)((_tmp839=
_region_malloc(r,sizeof(*_tmp839)),((_tmp839[0]=((_tmp83A.tag=3,((_tmp83A.f1=&
Cyc_nogc_r,_tmp83A)))),_tmp839)))),((_tmp83D->f5=((_tmp838="Don't link in the garbage collector",
_tag_dyneither(_tmp838,sizeof(char),36))),_tmp83D)))))))))))),((_tmp7DD[49]=((
_tmp843=_region_malloc(r,sizeof(*_tmp843)),((_tmp843->f1=((_tmp842="-nocyc",
_tag_dyneither(_tmp842,sizeof(char),7))),((_tmp843->f2=0,((_tmp843->f3=((_tmp841="",
_tag_dyneither(_tmp841,sizeof(char),1))),((_tmp843->f4=(void*)((_tmp83F=
_region_malloc(r,sizeof(*_tmp83F)),((_tmp83F[0]=((_tmp840.tag=0,((_tmp840.f1=Cyc_set_nocyc,
_tmp840)))),_tmp83F)))),((_tmp843->f5=((_tmp83E="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp83E,sizeof(char),33))),_tmp843)))))))))))),((_tmp7DD[48]=((
_tmp849=_region_malloc(r,sizeof(*_tmp849)),((_tmp849->f1=((_tmp848="-no-cpp-precomp",
_tag_dyneither(_tmp848,sizeof(char),16))),((_tmp849->f2=0,((_tmp849->f3=((
_tmp847="",_tag_dyneither(_tmp847,sizeof(char),1))),((_tmp849->f4=(void*)((
_tmp845=_region_malloc(r,sizeof(*_tmp845)),((_tmp845[0]=((_tmp846.tag=0,((
_tmp846.f1=Cyc_set_nocppprecomp,_tmp846)))),_tmp845)))),((_tmp849->f5=((_tmp844="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp844,sizeof(char),40))),_tmp849)))))))))))),((_tmp7DD[47]=((
_tmp84F=_region_malloc(r,sizeof(*_tmp84F)),((_tmp84F->f1=((_tmp84E="-use-cpp",
_tag_dyneither(_tmp84E,sizeof(char),9))),((_tmp84F->f2=0,((_tmp84F->f3=((_tmp84D="<path>",
_tag_dyneither(_tmp84D,sizeof(char),7))),((_tmp84F->f4=(void*)((_tmp84B=
_region_malloc(r,sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84C.tag=5,((_tmp84C.f1=Cyc_set_cpp,
_tmp84C)))),_tmp84B)))),((_tmp84F->f5=((_tmp84A="Indicate which preprocessor to use",
_tag_dyneither(_tmp84A,sizeof(char),35))),_tmp84F)))))))))))),((_tmp7DD[46]=((
_tmp855=_region_malloc(r,sizeof(*_tmp855)),((_tmp855->f1=((_tmp854="--inline-checks",
_tag_dyneither(_tmp854,sizeof(char),16))),((_tmp855->f2=0,((_tmp855->f3=((
_tmp853="",_tag_dyneither(_tmp853,sizeof(char),1))),((_tmp855->f4=(void*)((
_tmp851=_region_malloc(r,sizeof(*_tmp851)),((_tmp851[0]=((_tmp852.tag=0,((
_tmp852.f1=Cyc_set_inline_functions,_tmp852)))),_tmp851)))),((_tmp855->f5=((
_tmp850="Inline bounds checks instead of #define",_tag_dyneither(_tmp850,sizeof(
char),40))),_tmp855)))))))))))),((_tmp7DD[45]=((_tmp85B=_region_malloc(r,sizeof(*
_tmp85B)),((_tmp85B->f1=((_tmp85A="--noboundschecks",_tag_dyneither(_tmp85A,
sizeof(char),17))),((_tmp85B->f2=0,((_tmp85B->f3=((_tmp859="",_tag_dyneither(
_tmp859,sizeof(char),1))),((_tmp85B->f4=(void*)((_tmp857=_region_malloc(r,
sizeof(*_tmp857)),((_tmp857[0]=((_tmp858.tag=0,((_tmp858.f1=Cyc_set_noboundschecks,
_tmp858)))),_tmp857)))),((_tmp85B->f5=((_tmp856="Disable bounds checks",
_tag_dyneither(_tmp856,sizeof(char),22))),_tmp85B)))))))))))),((_tmp7DD[44]=((
_tmp861=_region_malloc(r,sizeof(*_tmp861)),((_tmp861->f1=((_tmp860="--nonullchecks",
_tag_dyneither(_tmp860,sizeof(char),15))),((_tmp861->f2=0,((_tmp861->f3=((
_tmp85F="",_tag_dyneither(_tmp85F,sizeof(char),1))),((_tmp861->f4=(void*)((
_tmp85D=_region_malloc(r,sizeof(*_tmp85D)),((_tmp85D[0]=((_tmp85E.tag=0,((
_tmp85E.f1=Cyc_set_nonullchecks,_tmp85E)))),_tmp85D)))),((_tmp861->f5=((_tmp85C="Disable null checks",
_tag_dyneither(_tmp85C,sizeof(char),20))),_tmp861)))))))))))),((_tmp7DD[43]=((
_tmp867=_region_malloc(r,sizeof(*_tmp867)),((_tmp867->f1=((_tmp866="--nochecks",
_tag_dyneither(_tmp866,sizeof(char),11))),((_tmp867->f2=0,((_tmp867->f3=((
_tmp865="",_tag_dyneither(_tmp865,sizeof(char),1))),((_tmp867->f4=(void*)((
_tmp863=_region_malloc(r,sizeof(*_tmp863)),((_tmp863[0]=((_tmp864.tag=0,((
_tmp864.f1=Cyc_set_nochecks,_tmp864)))),_tmp863)))),((_tmp867->f5=((_tmp862="Disable bounds/null checks",
_tag_dyneither(_tmp862,sizeof(char),27))),_tmp867)))))))))))),((_tmp7DD[42]=((
_tmp86D=_region_malloc(r,sizeof(*_tmp86D)),((_tmp86D->f1=((_tmp86C="--lineno",
_tag_dyneither(_tmp86C,sizeof(char),9))),((_tmp86D->f2=0,((_tmp86D->f3=((_tmp86B="",
_tag_dyneither(_tmp86B,sizeof(char),1))),((_tmp86D->f4=(void*)((_tmp869=
_region_malloc(r,sizeof(*_tmp869)),((_tmp869[0]=((_tmp86A.tag=0,((_tmp86A.f1=Cyc_set_lineno,
_tmp86A)))),_tmp869)))),((_tmp86D->f5=((_tmp868="Generate line numbers for debugging",
_tag_dyneither(_tmp868,sizeof(char),36))),_tmp86D)))))))))))),((_tmp7DD[41]=((
_tmp873=_region_malloc(r,sizeof(*_tmp873)),((_tmp873->f1=((_tmp872="-save-c",
_tag_dyneither(_tmp872,sizeof(char),8))),((_tmp873->f2=0,((_tmp873->f3=((_tmp871="",
_tag_dyneither(_tmp871,sizeof(char),1))),((_tmp873->f4=(void*)((_tmp86F=
_region_malloc(r,sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp870.tag=3,((_tmp870.f1=&
Cyc_save_c_r,_tmp870)))),_tmp86F)))),((_tmp873->f5=((_tmp86E="Don't delete temporary C files",
_tag_dyneither(_tmp86E,sizeof(char),31))),_tmp873)))))))))))),((_tmp7DD[40]=((
_tmp879=_region_malloc(r,sizeof(*_tmp879)),((_tmp879->f1=((_tmp878="-save-temps",
_tag_dyneither(_tmp878,sizeof(char),12))),((_tmp879->f2=0,((_tmp879->f3=((
_tmp877="",_tag_dyneither(_tmp877,sizeof(char),1))),((_tmp879->f4=(void*)((
_tmp875=_region_malloc(r,sizeof(*_tmp875)),((_tmp875[0]=((_tmp876.tag=0,((
_tmp876.f1=Cyc_set_save_temps,_tmp876)))),_tmp875)))),((_tmp879->f5=((_tmp874="Don't delete temporary files",
_tag_dyneither(_tmp874,sizeof(char),29))),_tmp879)))))))))))),((_tmp7DD[39]=((
_tmp87F=_region_malloc(r,sizeof(*_tmp87F)),((_tmp87F->f1=((_tmp87E="-c-comp",
_tag_dyneither(_tmp87E,sizeof(char),8))),((_tmp87F->f2=0,((_tmp87F->f3=((_tmp87D=" <compiler>",
_tag_dyneither(_tmp87D,sizeof(char),12))),((_tmp87F->f4=(void*)((_tmp87B=
_region_malloc(r,sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp87C.tag=5,((_tmp87C.f1=Cyc_set_c_compiler,
_tmp87C)))),_tmp87B)))),((_tmp87F->f5=((_tmp87A="Produce C output for the given compiler",
_tag_dyneither(_tmp87A,sizeof(char),40))),_tmp87F)))))))))))),((_tmp7DD[38]=((
_tmp885=_region_malloc(r,sizeof(*_tmp885)),((_tmp885->f1=((_tmp884="-un-gcc",
_tag_dyneither(_tmp884,sizeof(char),8))),((_tmp885->f2=0,((_tmp885->f3=((_tmp883="",
_tag_dyneither(_tmp883,sizeof(char),1))),((_tmp885->f4=(void*)((_tmp881=
_region_malloc(r,sizeof(*_tmp881)),((_tmp881[0]=((_tmp882.tag=0,((_tmp882.f1=Cyc_set_tovc,
_tmp882)))),_tmp881)))),((_tmp885->f5=((_tmp880="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp880,sizeof(char),57))),_tmp885)))))))))))),((_tmp7DD[37]=((
_tmp88B=_region_malloc(r,sizeof(*_tmp88B)),((_tmp88B->f1=((_tmp88A="-no-elim-statement-expressions",
_tag_dyneither(_tmp88A,sizeof(char),31))),((_tmp88B->f2=0,((_tmp88B->f3=((
_tmp889="",_tag_dyneither(_tmp889,sizeof(char),1))),((_tmp88B->f4=(void*)((
_tmp887=_region_malloc(r,sizeof(*_tmp887)),((_tmp887[0]=((_tmp888.tag=4,((
_tmp888.f1=& Cyc_elim_se_r,_tmp888)))),_tmp887)))),((_tmp88B->f5=((_tmp886="Do not avoid statement expressions in C output",
_tag_dyneither(_tmp886,sizeof(char),47))),_tmp88B)))))))))))),((_tmp7DD[36]=((
_tmp891=_region_malloc(r,sizeof(*_tmp891)),((_tmp891->f1=((_tmp890="-elim-statement-expressions",
_tag_dyneither(_tmp890,sizeof(char),28))),((_tmp891->f2=0,((_tmp891->f3=((
_tmp88F="",_tag_dyneither(_tmp88F,sizeof(char),1))),((_tmp891->f4=(void*)((
_tmp88D=_region_malloc(r,sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88E.tag=3,((
_tmp88E.f1=& Cyc_elim_se_r,_tmp88E)))),_tmp88D)))),((_tmp891->f5=((_tmp88C="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp88C,sizeof(char),66))),_tmp891)))))))))))),((_tmp7DD[35]=((
_tmp897=_region_malloc(r,sizeof(*_tmp897)),((_tmp897->f1=((_tmp896="-up",
_tag_dyneither(_tmp896,sizeof(char),4))),((_tmp897->f2=0,((_tmp897->f3=((_tmp895="",
_tag_dyneither(_tmp895,sizeof(char),1))),((_tmp897->f4=(void*)((_tmp893=
_region_malloc(r,sizeof(*_tmp893)),((_tmp893[0]=((_tmp894.tag=4,((_tmp894.f1=&
Cyc_pp_r,_tmp894)))),_tmp893)))),((_tmp897->f5=((_tmp892="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp892,sizeof(char),55))),_tmp897)))))))))))),((_tmp7DD[34]=((
_tmp89D=_region_malloc(r,sizeof(*_tmp89D)),((_tmp89D->f1=((_tmp89C="-pp",
_tag_dyneither(_tmp89C,sizeof(char),4))),((_tmp89D->f2=0,((_tmp89D->f3=((_tmp89B="",
_tag_dyneither(_tmp89B,sizeof(char),1))),((_tmp89D->f4=(void*)((_tmp899=
_region_malloc(r,sizeof(*_tmp899)),((_tmp899[0]=((_tmp89A.tag=3,((_tmp89A.f1=&
Cyc_pp_r,_tmp89A)))),_tmp899)))),((_tmp89D->f5=((_tmp898="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp898,sizeof(char),47))),_tmp89D)))))))))))),((_tmp7DD[33]=((
_tmp8A3=_region_malloc(r,sizeof(*_tmp8A3)),((_tmp8A3->f1=((_tmp8A2="-ic",
_tag_dyneither(_tmp8A2,sizeof(char),4))),((_tmp8A3->f2=0,((_tmp8A3->f3=((_tmp8A1="",
_tag_dyneither(_tmp8A1,sizeof(char),1))),((_tmp8A3->f4=(void*)((_tmp89F=
_region_malloc(r,sizeof(*_tmp89F)),((_tmp89F[0]=((_tmp8A0.tag=3,((_tmp8A0.f1=&
Cyc_ic_r,_tmp8A0)))),_tmp89F)))),((_tmp8A3->f5=((_tmp89E="Activate the link-checker",
_tag_dyneither(_tmp89E,sizeof(char),26))),_tmp8A3)))))))))))),((_tmp7DD[32]=((
_tmp8A9=_region_malloc(r,sizeof(*_tmp8A9)),((_tmp8A9->f1=((_tmp8A8="-stopafter-toc",
_tag_dyneither(_tmp8A8,sizeof(char),15))),((_tmp8A9->f2=0,((_tmp8A9->f3=((
_tmp8A7="",_tag_dyneither(_tmp8A7,sizeof(char),1))),((_tmp8A9->f4=(void*)((
_tmp8A5=_region_malloc(r,sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A6.tag=3,((
_tmp8A6.f1=& Cyc_toc_r,_tmp8A6)))),_tmp8A5)))),((_tmp8A9->f5=((_tmp8A4="Stop after translation to C",
_tag_dyneither(_tmp8A4,sizeof(char),28))),_tmp8A9)))))))))))),((_tmp7DD[31]=((
_tmp8AF=_region_malloc(r,sizeof(*_tmp8AF)),((_tmp8AF->f1=((_tmp8AE="-stopafter-cf",
_tag_dyneither(_tmp8AE,sizeof(char),14))),((_tmp8AF->f2=0,((_tmp8AF->f3=((
_tmp8AD="",_tag_dyneither(_tmp8AD,sizeof(char),1))),((_tmp8AF->f4=(void*)((
_tmp8AB=_region_malloc(r,sizeof(*_tmp8AB)),((_tmp8AB[0]=((_tmp8AC.tag=3,((
_tmp8AC.f1=& Cyc_cf_r,_tmp8AC)))),_tmp8AB)))),((_tmp8AF->f5=((_tmp8AA="Stop after control-flow checking",
_tag_dyneither(_tmp8AA,sizeof(char),33))),_tmp8AF)))))))))))),((_tmp7DD[30]=((
_tmp8B5=_region_malloc(r,sizeof(*_tmp8B5)),((_tmp8B5->f1=((_tmp8B4="-noprint",
_tag_dyneither(_tmp8B4,sizeof(char),9))),((_tmp8B5->f2=0,((_tmp8B5->f3=((_tmp8B3="",
_tag_dyneither(_tmp8B3,sizeof(char),1))),((_tmp8B5->f4=(void*)((_tmp8B1=
_region_malloc(r,sizeof(*_tmp8B1)),((_tmp8B1[0]=((_tmp8B2.tag=3,((_tmp8B2.f1=&
Cyc_noprint_r,_tmp8B2)))),_tmp8B1)))),((_tmp8B5->f5=((_tmp8B0="Do not print output (when stopping early)",
_tag_dyneither(_tmp8B0,sizeof(char),42))),_tmp8B5)))))))))))),((_tmp7DD[29]=((
_tmp8BB=_region_malloc(r,sizeof(*_tmp8BB)),((_tmp8BB->f1=((_tmp8BA="-stopafter-tc",
_tag_dyneither(_tmp8BA,sizeof(char),14))),((_tmp8BB->f2=0,((_tmp8BB->f3=((
_tmp8B9="",_tag_dyneither(_tmp8B9,sizeof(char),1))),((_tmp8BB->f4=(void*)((
_tmp8B7=_region_malloc(r,sizeof(*_tmp8B7)),((_tmp8B7[0]=((_tmp8B8.tag=3,((
_tmp8B8.f1=& Cyc_tc_r,_tmp8B8)))),_tmp8B7)))),((_tmp8BB->f5=((_tmp8B6="Stop after type checking",
_tag_dyneither(_tmp8B6,sizeof(char),25))),_tmp8BB)))))))))))),((_tmp7DD[28]=((
_tmp8C1=_region_malloc(r,sizeof(*_tmp8C1)),((_tmp8C1->f1=((_tmp8C0="-stopafter-parse",
_tag_dyneither(_tmp8C0,sizeof(char),17))),((_tmp8C1->f2=0,((_tmp8C1->f3=((
_tmp8BF="",_tag_dyneither(_tmp8BF,sizeof(char),1))),((_tmp8C1->f4=(void*)((
_tmp8BD=_region_malloc(r,sizeof(*_tmp8BD)),((_tmp8BD[0]=((_tmp8BE.tag=3,((
_tmp8BE.f1=& Cyc_parseonly_r,_tmp8BE)))),_tmp8BD)))),((_tmp8C1->f5=((_tmp8BC="Stop after parsing",
_tag_dyneither(_tmp8BC,sizeof(char),19))),_tmp8C1)))))))))))),((_tmp7DD[27]=((
_tmp8C7=_region_malloc(r,sizeof(*_tmp8C7)),((_tmp8C7->f1=((_tmp8C6="-E",
_tag_dyneither(_tmp8C6,sizeof(char),3))),((_tmp8C7->f2=0,((_tmp8C7->f3=((_tmp8C5="",
_tag_dyneither(_tmp8C5,sizeof(char),1))),((_tmp8C7->f4=(void*)((_tmp8C3=
_region_malloc(r,sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C4.tag=3,((_tmp8C4.f1=&
Cyc_stop_after_cpp_r,_tmp8C4)))),_tmp8C3)))),((_tmp8C7->f5=((_tmp8C2="Stop after preprocessing",
_tag_dyneither(_tmp8C2,sizeof(char),25))),_tmp8C7)))))))))))),((_tmp7DD[26]=((
_tmp8CD=_region_malloc(r,sizeof(*_tmp8CD)),((_tmp8CD->f1=((_tmp8CC="-Wall",
_tag_dyneither(_tmp8CC,sizeof(char),6))),((_tmp8CD->f2=0,((_tmp8CD->f3=((_tmp8CB="",
_tag_dyneither(_tmp8CB,sizeof(char),1))),((_tmp8CD->f4=(void*)((_tmp8C9=
_region_malloc(r,sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CA.tag=0,((_tmp8CA.f1=Cyc_set_all_warnings,
_tmp8CA)))),_tmp8C9)))),((_tmp8CD->f5=((_tmp8C8="Turn on all warnings",
_tag_dyneither(_tmp8C8,sizeof(char),21))),_tmp8CD)))))))))))),((_tmp7DD[25]=((
_tmp8D3=_region_malloc(r,sizeof(*_tmp8D3)),((_tmp8D3->f1=((_tmp8D2="-b",
_tag_dyneither(_tmp8D2,sizeof(char),3))),((_tmp8D3->f2=0,((_tmp8D3->f3=((_tmp8D1="<arch>",
_tag_dyneither(_tmp8D1,sizeof(char),7))),((_tmp8D3->f4=(void*)((_tmp8CF=
_region_malloc(r,sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D0.tag=5,((_tmp8D0.f1=Cyc_set_target_arch,
_tmp8D0)))),_tmp8CF)))),((_tmp8D3->f5=((_tmp8CE="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp8CE,sizeof(char),48))),_tmp8D3)))))))))))),((_tmp7DD[24]=((
_tmp8D9=_region_malloc(r,sizeof(*_tmp8D9)),((_tmp8D9->f1=((_tmp8D8="-MT",
_tag_dyneither(_tmp8D8,sizeof(char),4))),((_tmp8D9->f2=0,((_tmp8D9->f3=((_tmp8D7=" <target>",
_tag_dyneither(_tmp8D7,sizeof(char),10))),((_tmp8D9->f4=(void*)((_tmp8D5=
_region_malloc(r,sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D6.tag=5,((_tmp8D6.f1=Cyc_set_dependencies_target,
_tmp8D6)))),_tmp8D5)))),((_tmp8D9->f5=((_tmp8D4="Give target for dependencies",
_tag_dyneither(_tmp8D4,sizeof(char),29))),_tmp8D9)))))))))))),((_tmp7DD[23]=((
_tmp8DF=_region_malloc(r,sizeof(*_tmp8DF)),((_tmp8DF->f1=((_tmp8DE="-MG",
_tag_dyneither(_tmp8DE,sizeof(char),4))),((_tmp8DF->f2=0,((_tmp8DF->f3=((_tmp8DD="",
_tag_dyneither(_tmp8DD,sizeof(char),1))),((_tmp8DF->f4=(void*)((_tmp8DB=
_region_malloc(r,sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DC.tag=1,((_tmp8DC.f1=Cyc_add_cpparg,
_tmp8DC)))),_tmp8DB)))),((_tmp8DF->f5=((_tmp8DA="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp8DA,sizeof(char),68))),_tmp8DF)))))))))))),((_tmp7DD[22]=((
_tmp8E5=_region_malloc(r,sizeof(*_tmp8E5)),((_tmp8E5->f1=((_tmp8E4="-M",
_tag_dyneither(_tmp8E4,sizeof(char),3))),((_tmp8E5->f2=0,((_tmp8E5->f3=((_tmp8E3="",
_tag_dyneither(_tmp8E3,sizeof(char),1))),((_tmp8E5->f4=(void*)((_tmp8E1=
_region_malloc(r,sizeof(*_tmp8E1)),((_tmp8E1[0]=((_tmp8E2.tag=0,((_tmp8E2.f1=Cyc_set_produce_dependencies,
_tmp8E2)))),_tmp8E1)))),((_tmp8E5->f5=((_tmp8E0="Produce dependencies",
_tag_dyneither(_tmp8E0,sizeof(char),21))),_tmp8E5)))))))))))),((_tmp7DD[21]=((
_tmp8EB=_region_malloc(r,sizeof(*_tmp8EB)),((_tmp8EB->f1=((_tmp8EA="-S",
_tag_dyneither(_tmp8EA,sizeof(char),3))),((_tmp8EB->f2=0,((_tmp8EB->f3=((_tmp8E9="",
_tag_dyneither(_tmp8E9,sizeof(char),1))),((_tmp8EB->f4=(void*)((_tmp8E7=
_region_malloc(r,sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=0,((_tmp8E8.f1=Cyc_set_stop_after_asmfile,
_tmp8E8)))),_tmp8E7)))),((_tmp8EB->f5=((_tmp8E6="Stop after producing assembly code",
_tag_dyneither(_tmp8E6,sizeof(char),35))),_tmp8EB)))))))))))),((_tmp7DD[20]=((
_tmp8F1=_region_malloc(r,sizeof(*_tmp8F1)),((_tmp8F1->f1=((_tmp8F0="-pa",
_tag_dyneither(_tmp8F0,sizeof(char),4))),((_tmp8F1->f2=0,((_tmp8F1->f3=((_tmp8EF="",
_tag_dyneither(_tmp8EF,sizeof(char),1))),((_tmp8F1->f4=(void*)((_tmp8ED=
_region_malloc(r,sizeof(*_tmp8ED)),((_tmp8ED[0]=((_tmp8EE.tag=0,((_tmp8EE.f1=Cyc_set_pa,
_tmp8EE)))),_tmp8ED)))),((_tmp8F1->f5=((_tmp8EC="Compile for profiling with aprof",
_tag_dyneither(_tmp8EC,sizeof(char),33))),_tmp8F1)))))))))))),((_tmp7DD[19]=((
_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7->f1=((_tmp8F6="-pg",
_tag_dyneither(_tmp8F6,sizeof(char),4))),((_tmp8F7->f2=0,((_tmp8F7->f3=((_tmp8F5="",
_tag_dyneither(_tmp8F5,sizeof(char),1))),((_tmp8F7->f4=(void*)((_tmp8F3=
_region_malloc(r,sizeof(*_tmp8F3)),((_tmp8F3[0]=((_tmp8F4.tag=0,((_tmp8F4.f1=Cyc_set_pg,
_tmp8F4)))),_tmp8F3)))),((_tmp8F7->f5=((_tmp8F2="Compile for profiling with gprof",
_tag_dyneither(_tmp8F2,sizeof(char),33))),_tmp8F7)))))))))))),((_tmp7DD[18]=((
_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD->f1=((_tmp8FC="-p",
_tag_dyneither(_tmp8FC,sizeof(char),3))),((_tmp8FD->f2=0,((_tmp8FD->f3=((_tmp8FB="",
_tag_dyneither(_tmp8FB,sizeof(char),1))),((_tmp8FD->f4=(void*)((_tmp8F9=
_region_malloc(r,sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FA.tag=1,((_tmp8FA.f1=Cyc_add_ccarg,
_tmp8FA)))),_tmp8F9)))),((_tmp8FD->f5=((_tmp8F8="Compile for profiling with prof",
_tag_dyneither(_tmp8F8,sizeof(char),32))),_tmp8FD)))))))))))),((_tmp7DD[17]=((
_tmp903=_region_malloc(r,sizeof(*_tmp903)),((_tmp903->f1=((_tmp902="-g",
_tag_dyneither(_tmp902,sizeof(char),3))),((_tmp903->f2=0,((_tmp903->f3=((_tmp901="",
_tag_dyneither(_tmp901,sizeof(char),1))),((_tmp903->f4=(void*)((_tmp8FF=
_region_malloc(r,sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=1,((_tmp900.f1=Cyc_add_ccarg,
_tmp900)))),_tmp8FF)))),((_tmp903->f5=((_tmp8FE="Compile for debugging",
_tag_dyneither(_tmp8FE,sizeof(char),22))),_tmp903)))))))))))),((_tmp7DD[16]=((
_tmp909=_region_malloc(r,sizeof(*_tmp909)),((_tmp909->f1=((_tmp908="-fomit-frame-pointer",
_tag_dyneither(_tmp908,sizeof(char),21))),((_tmp909->f2=0,((_tmp909->f3=((
_tmp907="",_tag_dyneither(_tmp907,sizeof(char),1))),((_tmp909->f4=(void*)((
_tmp905=_region_malloc(r,sizeof(*_tmp905)),((_tmp905[0]=((_tmp906.tag=1,((
_tmp906.f1=Cyc_add_ccarg,_tmp906)))),_tmp905)))),((_tmp909->f5=((_tmp904="Omit frame pointer",
_tag_dyneither(_tmp904,sizeof(char),19))),_tmp909)))))))))))),((_tmp7DD[15]=((
_tmp90F=_region_malloc(r,sizeof(*_tmp90F)),((_tmp90F->f1=((_tmp90E="-O3",
_tag_dyneither(_tmp90E,sizeof(char),4))),((_tmp90F->f2=0,((_tmp90F->f3=((_tmp90D="",
_tag_dyneither(_tmp90D,sizeof(char),1))),((_tmp90F->f4=(void*)((_tmp90B=
_region_malloc(r,sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90C.tag=1,((_tmp90C.f1=Cyc_add_ccarg,
_tmp90C)))),_tmp90B)))),((_tmp90F->f5=((_tmp90A="Even more optimization",
_tag_dyneither(_tmp90A,sizeof(char),23))),_tmp90F)))))))))))),((_tmp7DD[14]=((
_tmp915=_region_malloc(r,sizeof(*_tmp915)),((_tmp915->f1=((_tmp914="-O2",
_tag_dyneither(_tmp914,sizeof(char),4))),((_tmp915->f2=0,((_tmp915->f3=((_tmp913="",
_tag_dyneither(_tmp913,sizeof(char),1))),((_tmp915->f4=(void*)((_tmp911=
_region_malloc(r,sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=1,((_tmp912.f1=Cyc_add_ccarg,
_tmp912)))),_tmp911)))),((_tmp915->f5=((_tmp910="A higher level of optimization",
_tag_dyneither(_tmp910,sizeof(char),31))),_tmp915)))))))))))),((_tmp7DD[13]=((
_tmp91B=_region_malloc(r,sizeof(*_tmp91B)),((_tmp91B->f1=((_tmp91A="-O",
_tag_dyneither(_tmp91A,sizeof(char),3))),((_tmp91B->f2=0,((_tmp91B->f3=((_tmp919="",
_tag_dyneither(_tmp919,sizeof(char),1))),((_tmp91B->f4=(void*)((_tmp917=
_region_malloc(r,sizeof(*_tmp917)),((_tmp917[0]=((_tmp918.tag=1,((_tmp918.f1=Cyc_add_ccarg,
_tmp918)))),_tmp917)))),((_tmp91B->f5=((_tmp916="Optimize",_tag_dyneither(
_tmp916,sizeof(char),9))),_tmp91B)))))))))))),((_tmp7DD[12]=((_tmp921=
_region_malloc(r,sizeof(*_tmp921)),((_tmp921->f1=((_tmp920="-O0",_tag_dyneither(
_tmp920,sizeof(char),4))),((_tmp921->f2=0,((_tmp921->f3=((_tmp91F="",
_tag_dyneither(_tmp91F,sizeof(char),1))),((_tmp921->f4=(void*)((_tmp91D=
_region_malloc(r,sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91E.tag=1,((_tmp91E.f1=Cyc_add_ccarg,
_tmp91E)))),_tmp91D)))),((_tmp921->f5=((_tmp91C="Don't optimize",_tag_dyneither(
_tmp91C,sizeof(char),15))),_tmp921)))))))))))),((_tmp7DD[11]=((_tmp927=
_region_malloc(r,sizeof(*_tmp927)),((_tmp927->f1=((_tmp926="-s",_tag_dyneither(
_tmp926,sizeof(char),3))),((_tmp927->f2=0,((_tmp927->f3=((_tmp925="",
_tag_dyneither(_tmp925,sizeof(char),1))),((_tmp927->f4=(void*)((_tmp923=
_region_malloc(r,sizeof(*_tmp923)),((_tmp923[0]=((_tmp924.tag=1,((_tmp924.f1=Cyc_add_ccarg,
_tmp924)))),_tmp923)))),((_tmp927->f5=((_tmp922="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp922,sizeof(char),60))),_tmp927)))))))))))),((_tmp7DD[10]=((
_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D->f1=((_tmp92C="-x",
_tag_dyneither(_tmp92C,sizeof(char),3))),((_tmp92D->f2=0,((_tmp92D->f3=((_tmp92B=" <language>",
_tag_dyneither(_tmp92B,sizeof(char),12))),((_tmp92D->f4=(void*)((_tmp929=
_region_malloc(r,sizeof(*_tmp929)),((_tmp929[0]=((_tmp92A.tag=5,((_tmp92A.f1=Cyc_set_inputtype,
_tmp92A)))),_tmp929)))),((_tmp92D->f5=((_tmp928="Specify <language> for the following input files",
_tag_dyneither(_tmp928,sizeof(char),49))),_tmp92D)))))))))))),((_tmp7DD[9]=((
_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933->f1=((_tmp932="-c",
_tag_dyneither(_tmp932,sizeof(char),3))),((_tmp933->f2=0,((_tmp933->f3=((_tmp931="",
_tag_dyneither(_tmp931,sizeof(char),1))),((_tmp933->f4=(void*)((_tmp92F=
_region_malloc(r,sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp930.tag=0,((_tmp930.f1=Cyc_set_stop_after_objectfile,
_tmp930)))),_tmp92F)))),((_tmp933->f5=((_tmp92E="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp92E,sizeof(char),61))),_tmp933)))))))))))),((_tmp7DD[8]=((
_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939->f1=((_tmp938="-m",
_tag_dyneither(_tmp938,sizeof(char),3))),((_tmp939->f2=1,((_tmp939->f3=((_tmp937="<option>",
_tag_dyneither(_tmp937,sizeof(char),9))),((_tmp939->f4=(void*)((_tmp935=
_region_malloc(r,sizeof(*_tmp935)),((_tmp935[0]=((_tmp936.tag=1,((_tmp936.f1=Cyc_add_marg,
_tmp936)))),_tmp935)))),((_tmp939->f5=((_tmp934="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmp934,sizeof(char),52))),_tmp939)))))))))))),((_tmp7DD[7]=((
_tmp93F=_region_malloc(r,sizeof(*_tmp93F)),((_tmp93F->f1=((_tmp93E="-l",
_tag_dyneither(_tmp93E,sizeof(char),3))),((_tmp93F->f2=1,((_tmp93F->f3=((_tmp93D="<libname>",
_tag_dyneither(_tmp93D,sizeof(char),10))),((_tmp93F->f4=(void*)((_tmp93B=
_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=1,((_tmp93C.f1=Cyc_add_libarg,
_tmp93C)))),_tmp93B)))),((_tmp93F->f5=((_tmp93A="Library file",_tag_dyneither(
_tmp93A,sizeof(char),13))),_tmp93F)))))))))))),((_tmp7DD[6]=((_tmp945=
_region_malloc(r,sizeof(*_tmp945)),((_tmp945->f1=((_tmp944="-L",_tag_dyneither(
_tmp944,sizeof(char),3))),((_tmp945->f2=1,((_tmp945->f3=((_tmp943="<dir>",
_tag_dyneither(_tmp943,sizeof(char),6))),((_tmp945->f4=(void*)((_tmp941=
_region_malloc(r,sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=1,((_tmp942.f1=Cyc_add_ccarg,
_tmp942)))),_tmp941)))),((_tmp945->f5=((_tmp940="Add to the list of directories for -l",
_tag_dyneither(_tmp940,sizeof(char),38))),_tmp945)))))))))))),((_tmp7DD[5]=((
_tmp94B=_region_malloc(r,sizeof(*_tmp94B)),((_tmp94B->f1=((_tmp94A="-I",
_tag_dyneither(_tmp94A,sizeof(char),3))),((_tmp94B->f2=1,((_tmp94B->f3=((_tmp949="<dir>",
_tag_dyneither(_tmp949,sizeof(char),6))),((_tmp94B->f4=(void*)((_tmp947=
_region_malloc(r,sizeof(*_tmp947)),((_tmp947[0]=((_tmp948.tag=1,((_tmp948.f1=Cyc_add_cpparg,
_tmp948)))),_tmp947)))),((_tmp94B->f5=((_tmp946="Add to the list of directories to search for include files",
_tag_dyneither(_tmp946,sizeof(char),59))),_tmp94B)))))))))))),((_tmp7DD[4]=((
_tmp951=_region_malloc(r,sizeof(*_tmp951)),((_tmp951->f1=((_tmp950="-B",
_tag_dyneither(_tmp950,sizeof(char),3))),((_tmp951->f2=1,((_tmp951->f3=((_tmp94F="<file>",
_tag_dyneither(_tmp94F,sizeof(char),7))),((_tmp951->f4=(void*)((_tmp94D=
_region_malloc(r,sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94E.tag=1,((_tmp94E.f1=Cyc_add_cyclone_exec_path,
_tmp94E)))),_tmp94D)))),((_tmp951->f5=((_tmp94C="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp94C,sizeof(char),60))),_tmp951)))))))))))),((_tmp7DD[3]=((
_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957->f1=((_tmp956="-D",
_tag_dyneither(_tmp956,sizeof(char),3))),((_tmp957->f2=1,((_tmp957->f3=((_tmp955="<name>[=<value>]",
_tag_dyneither(_tmp955,sizeof(char),17))),((_tmp957->f4=(void*)((_tmp953=
_region_malloc(r,sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=1,((_tmp954.f1=Cyc_add_cpparg,
_tmp954)))),_tmp953)))),((_tmp957->f5=((_tmp952="Pass definition to preprocessor",
_tag_dyneither(_tmp952,sizeof(char),32))),_tmp957)))))))))))),((_tmp7DD[2]=((
_tmp95D=_region_malloc(r,sizeof(*_tmp95D)),((_tmp95D->f1=((_tmp95C="-o",
_tag_dyneither(_tmp95C,sizeof(char),3))),((_tmp95D->f2=0,((_tmp95D->f3=((_tmp95B=" <file>",
_tag_dyneither(_tmp95B,sizeof(char),8))),((_tmp95D->f4=(void*)((_tmp959=
_region_malloc(r,sizeof(*_tmp959)),((_tmp959[0]=((_tmp95A.tag=5,((_tmp95A.f1=Cyc_set_output_file,
_tmp95A)))),_tmp959)))),((_tmp95D->f5=((_tmp958="Set the output file name to <file>",
_tag_dyneither(_tmp958,sizeof(char),35))),_tmp95D)))))))))))),((_tmp7DD[1]=((
_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963->f1=((_tmp962="--version",
_tag_dyneither(_tmp962,sizeof(char),10))),((_tmp963->f2=0,((_tmp963->f3=((
_tmp961="",_tag_dyneither(_tmp961,sizeof(char),1))),((_tmp963->f4=(void*)((
_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F[0]=((_tmp960.tag=0,((
_tmp960.f1=Cyc_print_version,_tmp960)))),_tmp95F)))),((_tmp963->f5=((_tmp95E="Print version information and exit",
_tag_dyneither(_tmp95E,sizeof(char),35))),_tmp963)))))))))))),((_tmp7DD[0]=((
_tmp969=_region_malloc(r,sizeof(*_tmp969)),((_tmp969->f1=((_tmp968="-v",
_tag_dyneither(_tmp968,sizeof(char),3))),((_tmp969->f2=0,((_tmp969->f3=((_tmp967="",
_tag_dyneither(_tmp967,sizeof(char),1))),((_tmp969->f4=(void*)((_tmp965=
_region_malloc(r,sizeof(*_tmp965)),((_tmp965[0]=((_tmp966.tag=3,((_tmp966.f1=&
Cyc_v_r,_tmp966)))),_tmp965)))),((_tmp969->f5=((_tmp964="Print compilation stages verbosely",
_tag_dyneither(_tmp964,sizeof(char),35))),_tmp969)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp7DD,sizeof(struct _tuple21*),66)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmp96A;struct _dyneither_ptr optstring=(_tmp96A="Options:",
_tag_dyneither(_tmp96A,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp155);if((char*)Cyc_target_arch.curr == (char*)(
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{char*_tmp96B;struct _dyneither_ptr cyclone_exec_prefix=(
_tmp96B=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmp96B,sizeof(
char),_get_zero_arr_size_char(_tmp96B,1)));if((char*)cyclone_exec_prefix.curr != (
char*)(_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*_tmp96E;struct Cyc_List_List*
_tmp96D;Cyc_cyclone_exec_path=((_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D->hd=((
_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E[0]=cyclone_exec_prefix,_tmp96E)))),((
_tmp96D->tl=Cyc_cyclone_exec_path,_tmp96D))))));}def_lib_path=(struct
_dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct
_dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmp974;const char*_tmp973;
struct Cyc_List_List*_tmp972;Cyc_cyclone_exec_path=((_tmp972=_cycalloc(sizeof(*
_tmp972)),((_tmp972->hd=((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=(
struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmp973="cyc-lib",
_tag_dyneither(_tmp973,sizeof(char),8)))),_tmp974)))),((_tmp972->tl=Cyc_cyclone_exec_path,
_tmp972))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmp975;struct _dyneither_ptr
_tmp15C=Cyc_do_find(cyclone_arch_path,((_tmp975="cycspecs",_tag_dyneither(
_tmp975,sizeof(char),9))));if(Cyc_v_r){const char*_tmp979;void*_tmp978[1];struct
Cyc_String_pa_struct _tmp977;(_tmp977.tag=0,((_tmp977.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp15C),((_tmp978[0]=& _tmp977,Cyc_fprintf(Cyc_stderr,((
_tmp979="Reading from specs file %s\n",_tag_dyneither(_tmp979,sizeof(char),28))),
_tag_dyneither(_tmp978,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp160=Cyc_read_specs(
_tmp15C);struct _dyneither_ptr _tmp161=_tag_dyneither(0,0,0);{struct _handler_cons
_tmp162;_push_handler(& _tmp162);{int _tmp164=0;if(setjmp(_tmp162.handler))_tmp164=
1;if(!_tmp164){{struct _dyneither_ptr _tmp165=*((struct _dyneither_ptr*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp160,_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));_tmp161=Cyc_split_specs(
_tmp165);};_pop_handler();}else{void*_tmp163=(void*)_exn_thrown;void*_tmp168=
_tmp163;_LL36: {struct Cyc_Core_Not_found_struct*_tmp169=(struct Cyc_Core_Not_found_struct*)
_tmp168;if(_tmp169->tag != Cyc_Core_Not_found)goto _LL38;}_LL37: goto _LL35;_LL38:;
_LL39:(void)_throw(_tmp168);_LL35:;}};}if((struct _dyneither_ptr*)_tmp161.curr != (
struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp161);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp156);{const char*_tmp97A;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_cpp,((_tmp97A="",_tag_dyneither(_tmp97A,sizeof(char),1))))
== 0){const char*_tmp16B=Cyc_produce_dependencies?"":(const char*)" -E";const char*
_tmp982;void*_tmp981[3];struct Cyc_String_pa_struct _tmp980;const char*_tmp97F;
struct Cyc_String_pa_struct _tmp97E;struct Cyc_String_pa_struct _tmp97D;Cyc_set_cpp((
struct _dyneither_ptr)((_tmp97D.tag=0,((_tmp97D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp15C),((_tmp97E.tag=0,((_tmp97E.f1=(struct _dyneither_ptr)((
_tmp97F=_tmp16B,_tag_dyneither(_tmp97F,sizeof(char),_get_zero_arr_size_char(
_tmp97F,1)))),((_tmp980.tag=0,((_tmp980.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmp981[0]=& _tmp980,((_tmp981[1]=& _tmp97E,((_tmp981[2]=&
_tmp97D,Cyc_aprintf(((_tmp982="%s -w -x c%s -specs %s",_tag_dyneither(_tmp982,
sizeof(char),23))),_tag_dyneither(_tmp981,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmp985;void*_tmp984;(
_tmp984=0,Cyc_fprintf(Cyc_stderr,((_tmp985="missing file\n",_tag_dyneither(
_tmp985,sizeof(char),14))),_tag_dyneither(_tmp984,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmp986;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmp986="cyc_setjmp.h",_tag_dyneither(_tmp986,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmp987;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmp987="cyc_include.h",_tag_dyneither(_tmp987,sizeof(
char),14))));}{struct Cyc_List_List*_tmp312=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp312 != 0;_tmp312=_tmp312->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp312->hd));if(Cyc_compile_failure){int _tmp313=1;
_npop_handler(0);return _tmp313;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp314=0;_npop_handler(0);return _tmp314;}if(Cyc_ccargs
== 0){int _tmp315=0;_npop_handler(0);return _tmp315;}{const char*_tmp988;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmp988="-L",_tag_dyneither(_tmp988,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmp989;struct
_dyneither_ptr _tmp317=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmp989=" ",_tag_dyneither(_tmp989,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmp98B;struct Cyc_List_List*_tmp98A;struct _dyneither_ptr
_tmp318=Cyc_str_sepstr(((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmp98A->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmp98A)))))),((_tmp98B=" ",_tag_dyneither(_tmp98B,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmp98D;
const char*_tmp98C;int _tmp319=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmp98D=".a",_tag_dyneither(_tmp98D,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp98C=".lib",_tag_dyneither(
_tmp98C,sizeof(char),5))));if(_tmp319){stdlib=0;{const char*_tmp98E;stdlib_string=(
struct _dyneither_ptr)((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmp98F;libcyc_flag=((_tmp98F="-lcyc_a",_tag_dyneither(_tmp98F,sizeof(
char),8)));}{const char*_tmp990;nogc_filename=((_tmp990="nogc_a.a",_tag_dyneither(
_tmp990,sizeof(char),9)));}{const char*_tmp991;runtime_filename=((_tmp991="runtime_cyc_a.o",
_tag_dyneither(_tmp991,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmp992;libcyc_flag=((_tmp992="-lcyc_nocheck",_tag_dyneither(_tmp992,sizeof(char),
14)));}{const char*_tmp993;nogc_filename=((_tmp993="nogc.a",_tag_dyneither(
_tmp993,sizeof(char),7)));}{const char*_tmp994;runtime_filename=((_tmp994="runtime_cyc.o",
_tag_dyneither(_tmp994,sizeof(char),14)));};}else{if(Cyc_pg_r){{const char*
_tmp995;libcyc_flag=((_tmp995="-lcyc_pg",_tag_dyneither(_tmp995,sizeof(char),9)));}{
const char*_tmp996;runtime_filename=((_tmp996="runtime_cyc_pg.o",_tag_dyneither(
_tmp996,sizeof(char),17)));}{const char*_tmp997;nogc_filename=((_tmp997="nogc_pg.a",
_tag_dyneither(_tmp997,sizeof(char),10)));};}else{if(Cyc_compile_for_boot_r){{
const char*_tmp998;libcyc_flag=((_tmp998="-lcycboot",_tag_dyneither(_tmp998,
sizeof(char),10)));}{const char*_tmp999;nogc_filename=((_tmp999="nogc.a",
_tag_dyneither(_tmp999,sizeof(char),7)));}{const char*_tmp99A;runtime_filename=((
_tmp99A="runtime_cyc.o",_tag_dyneither(_tmp99A,sizeof(char),14)));};}else{{const
char*_tmp99B;libcyc_flag=((_tmp99B="-lcyc",_tag_dyneither(_tmp99B,sizeof(char),6)));}{
const char*_tmp99C;nogc_filename=((_tmp99C="nogc.a",_tag_dyneither(_tmp99C,
sizeof(char),7)));}{const char*_tmp99D;runtime_filename=((_tmp99D="runtime_cyc.o",
_tag_dyneither(_tmp99D,sizeof(char),14)));};}}}}{const char*_tmp99E;gc_filename=((
_tmp99E="gc.a",_tag_dyneither(_tmp99E,sizeof(char),5)));}{struct _dyneither_ptr
_tmp32B=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp32C=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmp9A4;void*_tmp9A3[3];
struct Cyc_String_pa_struct _tmp9A2;struct Cyc_String_pa_struct _tmp9A1;struct Cyc_String_pa_struct
_tmp9A0;stdlib_string=(struct _dyneither_ptr)((_tmp9A0.tag=0,((_tmp9A0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp32B),((_tmp9A1.tag=0,((_tmp9A1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmp9A2.tag=0,((
_tmp9A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32C),((_tmp9A3[0]=&
_tmp9A2,((_tmp9A3[1]=& _tmp9A1,((_tmp9A3[2]=& _tmp9A0,Cyc_aprintf(((_tmp9A4=" %s %s %s",
_tag_dyneither(_tmp9A4,sizeof(char),10))),_tag_dyneither(_tmp9A3,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp332;_push_handler(& _tmp332);{int _tmp334=0;
if(setjmp(_tmp332.handler))_tmp334=1;if(!_tmp334){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp335=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp319){struct
Cyc_List_List*_tmp9A5;_tmp335=((_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5->hd=
Cyc_final_strptr,((_tmp9A5->tl=_tmp335,_tmp9A5))))));}{struct Cyc_Interface_I*
_tmp337=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp335,_tmp335);if(_tmp337 == 0){{const char*_tmp9A8;void*_tmp9A7;(
_tmp9A7=0,Cyc_fprintf(Cyc_stderr,((_tmp9A8="Error: interfaces incompatible\n",
_tag_dyneither(_tmp9A8,sizeof(char),32))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp33A=1;_npop_handler(1);return
_tmp33A;};}if(_tmp319){if(Cyc_output_file != 0){const char*_tmp9AC;void*_tmp9AB[1];
struct Cyc_String_pa_struct _tmp9AA;struct _dyneither_ptr _tmp33B=(_tmp9AA.tag=0,((
_tmp9AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp9AB[0]=& _tmp9AA,Cyc_aprintf(((
_tmp9AC="%s.cycio",_tag_dyneither(_tmp9AC,sizeof(char),9))),_tag_dyneither(
_tmp9AB,sizeof(void*),1)))))));const char*_tmp9AE;const char*_tmp9AD;struct Cyc___cycFILE*
_tmp33C=Cyc_try_file_open((struct _dyneither_ptr)_tmp33B,((_tmp9AD="wb",
_tag_dyneither(_tmp9AD,sizeof(char),3))),((_tmp9AE="interface object file",
_tag_dyneither(_tmp9AE,sizeof(char),22))));if(_tmp33C == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp33D=1;_npop_handler(1);return _tmp33D;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp337,(struct Cyc___cycFILE*)_tmp33C);Cyc_file_close((
struct Cyc___cycFILE*)_tmp33C);}}else{const char*_tmp9B3;const char*_tmp9B2;struct
_tuple17*_tmp9B1;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp337,((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->f1=((
_tmp9B2="empty interface",_tag_dyneither(_tmp9B2,sizeof(char),16))),((_tmp9B1->f2=((
_tmp9B3="global interface",_tag_dyneither(_tmp9B3,sizeof(char),17))),_tmp9B1)))))))){{
const char*_tmp9B6;void*_tmp9B5;(_tmp9B5=0,Cyc_fprintf(Cyc_stderr,((_tmp9B6="Error: some objects are still undefined\n",
_tag_dyneither(_tmp9B6,sizeof(char),41))),_tag_dyneither(_tmp9B5,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp348=1;_npop_handler(1);return
_tmp348;};}}};};;_pop_handler();}else{void*_tmp333=(void*)_exn_thrown;void*
_tmp34A=_tmp333;_LL3B:;_LL3C:{void*_tmp34B=_tmp34A;struct _dyneither_ptr _tmp34D;
struct _dyneither_ptr _tmp34F;struct _dyneither_ptr _tmp352;_LL40: {struct Cyc_Core_Failure_struct*
_tmp34C=(struct Cyc_Core_Failure_struct*)_tmp34B;if(_tmp34C->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp34D=_tmp34C->f1;}}_LL41:{const char*_tmp9BA;void*_tmp9B9[1];
struct Cyc_String_pa_struct _tmp9B8;(_tmp9B8.tag=0,((_tmp9B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp34D),((_tmp9B9[0]=& _tmp9B8,Cyc_fprintf(
Cyc_stderr,((_tmp9BA="Exception Core::Failure %s\n",_tag_dyneither(_tmp9BA,
sizeof(char),28))),_tag_dyneither(_tmp9B9,sizeof(void*),1)))))));}goto _LL3F;
_LL42: {struct Cyc_Core_Impossible_struct*_tmp34E=(struct Cyc_Core_Impossible_struct*)
_tmp34B;if(_tmp34E->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp34F=_tmp34E->f1;}}
_LL43:{const char*_tmp9BE;void*_tmp9BD[1];struct Cyc_String_pa_struct _tmp9BC;(
_tmp9BC.tag=0,((_tmp9BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34F),((
_tmp9BD[0]=& _tmp9BC,Cyc_fprintf(Cyc_stderr,((_tmp9BE="Exception Core::Impossible %s\n",
_tag_dyneither(_tmp9BE,sizeof(char),31))),_tag_dyneither(_tmp9BD,sizeof(void*),1)))))));}
goto _LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp350=(struct Cyc_Dict_Absent_struct*)
_tmp34B;if(_tmp350->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:{const char*_tmp9C1;
void*_tmp9C0;(_tmp9C0=0,Cyc_fprintf(Cyc_stderr,((_tmp9C1="Exception Dict::Absent\n",
_tag_dyneither(_tmp9C1,sizeof(char),24))),_tag_dyneither(_tmp9C0,sizeof(void*),0)));}
goto _LL3F;_LL46: {struct Cyc_Core_Invalid_argument_struct*_tmp351=(struct Cyc_Core_Invalid_argument_struct*)
_tmp34B;if(_tmp351->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp352=
_tmp351->f1;}}_LL47:{const char*_tmp9C5;void*_tmp9C4[1];struct Cyc_String_pa_struct
_tmp9C3;(_tmp9C3.tag=0,((_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp352),((_tmp9C4[0]=& _tmp9C3,Cyc_fprintf(Cyc_stderr,((_tmp9C5="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmp9C5,sizeof(char),37))),_tag_dyneither(_tmp9C4,sizeof(void*),1)))))));}
goto _LL3F;_LL48:;_LL49:{const char*_tmp9C8;void*_tmp9C7;(_tmp9C7=0,Cyc_fprintf(
Cyc_stderr,((_tmp9C8="Uncaught exception\n",_tag_dyneither(_tmp9C8,sizeof(char),
20))),_tag_dyneither(_tmp9C7,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp360=1;_npop_handler(0);return _tmp360;};_LL3D:;
_LL3E:(void)_throw(_tmp34A);_LL3A:;}};}{const char*_tmp9C9;struct _dyneither_ptr
outfile_str=(_tmp9C9="",_tag_dyneither(_tmp9C9,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmp9CD;void*_tmp9CC[1];struct Cyc_String_pa_struct _tmp9CB;
outfile_str=(struct _dyneither_ptr)((_tmp9CB.tag=0,((_tmp9CB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp9CC[0]=& _tmp9CB,Cyc_aprintf(((
_tmp9CD=" -o %s",_tag_dyneither(_tmp9CD,sizeof(char),7))),_tag_dyneither(_tmp9CC,
sizeof(void*),1))))))));}{const char*_tmp9D5;void*_tmp9D4[5];struct Cyc_String_pa_struct
_tmp9D3;struct Cyc_String_pa_struct _tmp9D2;struct Cyc_String_pa_struct _tmp9D1;
struct Cyc_String_pa_struct _tmp9D0;struct Cyc_String_pa_struct _tmp9CF;struct
_dyneither_ptr _tmp364=(_tmp9CF.tag=0,((_tmp9CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp318),((_tmp9D0.tag=0,((_tmp9D0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmp9D1.tag=0,((_tmp9D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp317),((_tmp9D2.tag=0,((_tmp9D2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmp9D3.tag=0,((
_tmp9D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmp9D4[0]=&
_tmp9D3,((_tmp9D4[1]=& _tmp9D2,((_tmp9D4[2]=& _tmp9D1,((_tmp9D4[3]=& _tmp9D0,((
_tmp9D4[4]=& _tmp9CF,Cyc_aprintf(((_tmp9D5="%s %s %s%s%s",_tag_dyneither(_tmp9D5,
sizeof(char),13))),_tag_dyneither(_tmp9D4,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmp9D9;void*_tmp9D8[1];struct Cyc_String_pa_struct _tmp9D7;(
_tmp9D7.tag=0,((_tmp9D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp364),((
_tmp9D8[0]=& _tmp9D7,Cyc_fprintf(Cyc_stderr,((_tmp9D9="%s\n",_tag_dyneither(
_tmp9D9,sizeof(char),4))),_tag_dyneither(_tmp9D8,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp364,sizeof(char),1))!= 0){{const char*_tmp9DC;void*_tmp9DB;(_tmp9DB=0,Cyc_fprintf(
Cyc_stderr,((_tmp9DC="Error: C compiler failed\n",_tag_dyneither(_tmp9DC,sizeof(
char),26))),_tag_dyneither(_tmp9DB,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp36A=1;_npop_handler(0);return _tmp36A;};}Cyc_remove_cfiles();{int _tmp36B=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp36B;};};};};};;_pop_region(r);};}
