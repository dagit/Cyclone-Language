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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_sscanf(struct _dyneither_ptr,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};int isspace(int);struct Cyc_Core_NewRegion{struct
_DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};extern char Cyc_List_Nth[4];
struct Cyc_List_Nth_struct{char*tag;};unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr
f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*
anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr
args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*,struct _dyneither_ptr);void*Cyc___assert_fail(struct
_dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[
4]="Bad";char Cyc_Arg_Error[6]="Error";struct Cyc_Arg_Prefix_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Arg_Exact_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Arg_Unknown_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Missing_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Message_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Arg_Wrong_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple0{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _dyneither_ptr x){while(l != 
0){struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;unsigned long _tmp1=
Cyc_strlen((struct _dyneither_ptr)_tmp0);if(_tmp1 > 0  && (*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)
l->hd)).f1,_tmp1)== 0)return(*((struct _tuple0*)l->hd)).f4;}else{if(Cyc_strcmp((
struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}l=l->tl;}(int)_throw((void*)({struct Cyc_Core_Not_found_struct*
_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Core_Not_found_struct
_tmp3;_tmp3.tag=Cyc_Core_Not_found;_tmp3;});_tmp2;}));}static struct
_dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct
_dyneither_ptr s){if(s.curr == (_tag_dyneither(0,0,0)).curr)return(struct
_dyneither_ptr)_tag_dyneither(0,0,0);if(howmuch < 0)howmuch=0;{unsigned long _tmp4=
Cyc_strlen((struct _dyneither_ptr)s);_tmp4 <= _get_dyneither_size(s,sizeof(char))?
0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int
line))Cyc___assert_fail)(({const char*_tmp5="len <= numelts(s)";_tag_dyneither(
_tmp5,sizeof(char),18);}),({const char*_tmp6="arg.cyc";_tag_dyneither(_tmp6,
sizeof(char),8);}),93);if(howmuch > _tmp4){Cyc_Buffer_add_string(b,s);return(
struct _dyneither_ptr)_tag_dyneither(0,0,0);}{int i;for(i=howmuch - 1;i >= 0  && !
isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));-- i){;}
if(i < 0)for(i=howmuch?howmuch - 1: 0;(i < _tmp4  && (int)((const char*)s.curr)[i])
 && !isspace((int)((const char*)s.curr)[i]);++ i){;}Cyc_Buffer_add_substring(b,s,0,
i);{struct _dyneither_ptr whatsleft=(struct _dyneither_ptr)_tag_dyneither(0,0,0);
for(0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && isspace((int)((const char*)s.curr)[
i]);++ i){;}if(i < _tmp4  && (int)((const char*)s.curr)[i])whatsleft=
_dyneither_ptr_plus(s,sizeof(char),i);return whatsleft;};};};}void Cyc_Arg_Justify_justify_b(
struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct
_dyneither_ptr desc){if(item.curr != (_tag_dyneither(0,0,0)).curr)Cyc_Buffer_add_string(
b,item);if(desc.curr == (_tag_dyneither(0,0,0)).curr)return;if(indent < 0)indent=0;
if(margin < 0)margin=0;{struct _dyneither_ptr indentstr=({unsigned int _tmp13=(
unsigned int)(indent + 2);char*_tmp14=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmp13 + 1));struct _dyneither_ptr _tmp16=_tag_dyneither(_tmp14,
sizeof(char),_tmp13 + 1);{unsigned int _tmp15=_tmp13;unsigned int i;for(i=0;i < 
_tmp15;i ++){_tmp14[i]='\000';}_tmp14[_tmp15]=(char)0;}_tmp16;});{unsigned int i=0;
for(0;i < indent + 1;++ i){({struct _dyneither_ptr _tmp7=_dyneither_ptr_plus(
indentstr,sizeof(char),(int)i);char _tmp8=*((char*)_check_dyneither_subscript(
_tmp7,sizeof(char),0));char _tmp9=i == 0?'\n':' ';if(_get_dyneither_size(_tmp7,
sizeof(char))== 1  && (_tmp8 == '\000'  && _tmp9 != '\000'))_throw_arraybounds();*((
char*)_tmp7.curr)=_tmp9;});}}{unsigned long _tmpA=Cyc_strlen((struct
_dyneither_ptr)item);struct _dyneither_ptr itemsep;if(Cyc_strlen((struct
_dyneither_ptr)desc)> 0){if(_tmpA + 1 > indent)itemsep=indentstr;else{struct
_dyneither_ptr temp=({unsigned int _tmpE=(indent - _tmpA)+ 1;char*_tmpF=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmpE + 1));struct _dyneither_ptr _tmp11=
_tag_dyneither(_tmpF,sizeof(char),_tmpE + 1);{unsigned int _tmp10=_tmpE;
unsigned int i;for(i=0;i < _tmp10;i ++){_tmpF[i]='\000';}_tmpF[_tmp10]=(char)0;}
_tmp11;});{unsigned int i=0;for(0;i < indent - _tmpA;++ i){({struct _dyneither_ptr
_tmpB=_dyneither_ptr_plus(temp,sizeof(char),(int)i);char _tmpC=*((char*)
_check_dyneither_subscript(_tmpB,sizeof(char),0));char _tmpD=' ';if(
_get_dyneither_size(_tmpB,sizeof(char))== 1  && (_tmpC == '\000'  && _tmpD != '\000'))
_throw_arraybounds();*((char*)_tmpB.curr)=_tmpD;});}}itemsep=temp;}}else{return;}
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);while(desc.curr != (
_tag_dyneither(0,0,0)).curr){desc=Cyc_Arg_Justify_break_line(b,margin - indent,
desc);if(desc.curr != (_tag_dyneither(0,0,0)).curr)Cyc_Buffer_add_string(b,(
struct _dyneither_ptr)indentstr);else{Cyc_Buffer_add_string(b,({const char*_tmp12="\n";
_tag_dyneither(_tmp12,sizeof(char),2);}));}}return;};};}void Cyc_Arg_usage(struct
Cyc_List_List*speclist,struct _dyneither_ptr errmsg){({struct Cyc_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
errmsg);({void*_tmp17[1]={& _tmp19};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="%s\n";
_tag_dyneither(_tmp18,sizeof(char),4);}),_tag_dyneither(_tmp17,sizeof(void*),1));});});{
struct Cyc_Buffer_t*_tmp1A=Cyc_Buffer_create(1024);while(speclist != 0){Cyc_Arg_Justify_justify_b(
_tmp1A,12,72,(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)(*((
struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)
speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);speclist=speclist->tl;}({
struct Cyc_String_pa_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Buffer_contents(_tmp1A));({void*_tmp1B[1]={& _tmp1D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3);}),
_tag_dyneither(_tmp1B,sizeof(void*),1));});});};}int Cyc_Arg_current=0;static
struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_Arg_stop(
int prog_pos,void*e,struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
struct _dyneither_ptr progname=prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(
struct _dyneither_ptr))?*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,
sizeof(struct _dyneither_ptr),prog_pos)):({const char*_tmp3C="(?)";_tag_dyneither(
_tmp3C,sizeof(char),4);});{void*_tmp1E=e;struct _dyneither_ptr _tmp20;struct
_dyneither_ptr _tmp22;struct _dyneither_ptr _tmp24;struct _dyneither_ptr _tmp25;
struct _dyneither_ptr _tmp26;struct _dyneither_ptr _tmp28;_LL1: {struct Cyc_Arg_Unknown_struct*
_tmp1F=(struct Cyc_Arg_Unknown_struct*)_tmp1E;if(_tmp1F->tag != 0)goto _LL3;else{
_tmp20=_tmp1F->f1;}}_LL2: if(Cyc_strcmp((struct _dyneither_ptr)_tmp20,({const char*
_tmp29="-help";_tag_dyneither(_tmp29,sizeof(char),6);}))!= 0)({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp20);({struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp2A[2]={& _tmp2C,&
_tmp2D};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B="%s: unknown option `%s'.\n";
_tag_dyneither(_tmp2B,sizeof(char),26);}),_tag_dyneither(_tmp2A,sizeof(void*),2));});});});
goto _LL0;_LL3: {struct Cyc_Arg_Missing_struct*_tmp21=(struct Cyc_Arg_Missing_struct*)
_tmp1E;if(_tmp21->tag != 1)goto _LL5;else{_tmp22=_tmp21->f1;}}_LL4:({struct Cyc_String_pa_struct
_tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp22);({struct Cyc_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp2E[2]={& _tmp30,&
_tmp31};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="%s: option `%s' needs an argument.\n";
_tag_dyneither(_tmp2F,sizeof(char),36);}),_tag_dyneither(_tmp2E,sizeof(void*),2));});});});
goto _LL0;_LL5: {struct Cyc_Arg_Wrong_struct*_tmp23=(struct Cyc_Arg_Wrong_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL7;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;
_tmp26=_tmp23->f3;}}_LL6:({struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26);({struct Cyc_String_pa_struct
_tmp36;_tmp36.tag=0;_tmp36.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp24);({struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp25);({struct Cyc_String_pa_struct _tmp34;
_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);({
void*_tmp32[4]={& _tmp34,& _tmp35,& _tmp36,& _tmp37};Cyc_fprintf(Cyc_stderr,({const
char*_tmp33="%s: wrong argument `%s'; option `%s' expects %s.\n";_tag_dyneither(
_tmp33,sizeof(char),50);}),_tag_dyneither(_tmp32,sizeof(void*),4));});});});});});
goto _LL0;_LL7: {struct Cyc_Arg_Message_struct*_tmp27=(struct Cyc_Arg_Message_struct*)
_tmp1E;if(_tmp27->tag != 2)goto _LL0;else{_tmp28=_tmp27->f1;}}_LL8:({struct Cyc_String_pa_struct
_tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp28);({struct Cyc_String_pa_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)progname);({void*_tmp38[2]={& _tmp3A,&
_tmp3B};Cyc_fprintf(Cyc_stderr,({const char*_tmp39="%s: %s.\n";_tag_dyneither(
_tmp39,sizeof(char),9);}),_tag_dyneither(_tmp38,sizeof(void*),2));});});});goto
_LL0;_LL0:;}Cyc_Arg_usage(speclist,errmsg);Cyc_Arg_current=(int)
_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));}void Cyc_Arg_parse(
struct Cyc_List_List*speclist,void(*anonfun)(struct _dyneither_ptr),struct
_dyneither_ptr errmsg,struct _dyneither_ptr orig_args){Cyc_Arg_args=orig_args;{int
initpos=Cyc_Arg_current;unsigned int l=_get_dyneither_size(Cyc_Arg_args,sizeof(
struct _dyneither_ptr));if((*((struct _dyneither_ptr*)_check_dyneither_subscript(
Cyc_Arg_args,sizeof(struct _dyneither_ptr),(int)(l - 1)))).curr == (_tag_dyneither(
0,0,0)).curr)l=l - 1;++ Cyc_Arg_current;while(Cyc_Arg_current < l){struct
_dyneither_ptr s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,
sizeof(struct _dyneither_ptr),Cyc_Arg_current));if((s.curr != (_tag_dyneither(0,0,
0)).curr  && _get_dyneither_size(s,sizeof(char))>= 1) && *((const char*)
_check_dyneither_subscript(s,sizeof(char),0))== '-'){void*action;{struct
_handler_cons _tmp3D;_push_handler(& _tmp3D);{int _tmp3F=0;if(setjmp(_tmp3D.handler))
_tmp3F=1;if(!_tmp3F){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{
void*_tmp3E=(void*)_exn_thrown;void*_tmp41=_tmp3E;_LLA: {struct Cyc_Core_Not_found_struct*
_tmp42=(struct Cyc_Core_Not_found_struct*)_tmp41;if(_tmp42->tag != Cyc_Core_Not_found)
goto _LLC;}_LLB: Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Unknown_struct*_tmp43=
_cycalloc(sizeof(*_tmp43));_tmp43[0]=({struct Cyc_Arg_Unknown_struct _tmp44;_tmp44.tag=
0;_tmp44.f1=s;_tmp44;});_tmp43;}),speclist,errmsg);return;_LLC:;_LLD:(void)
_throw(_tmp41);_LL9:;}};}{struct _handler_cons _tmp45;_push_handler(& _tmp45);{int
_tmp47=0;if(setjmp(_tmp45.handler))_tmp47=1;if(!_tmp47){{void*_tmp48=action;void(*
_tmp4A)();void(*_tmp4C)(struct _dyneither_ptr);int*_tmp4E;int*_tmp50;void(*_tmp52)(
struct _dyneither_ptr,struct _dyneither_ptr);void(*_tmp54)(struct _dyneither_ptr);
void(*_tmp56)(int);void(*_tmp58)(struct _dyneither_ptr);_LLF: {struct Cyc_Arg_Unit_spec_struct*
_tmp49=(struct Cyc_Arg_Unit_spec_struct*)_tmp48;if(_tmp49->tag != 0)goto _LL11;
else{_tmp4A=_tmp49->f1;}}_LL10: _tmp4A();goto _LLE;_LL11: {struct Cyc_Arg_Flag_spec_struct*
_tmp4B=(struct Cyc_Arg_Flag_spec_struct*)_tmp48;if(_tmp4B->tag != 1)goto _LL13;
else{_tmp4C=_tmp4B->f1;}}_LL12: _tmp4C(s);goto _LLE;_LL13: {struct Cyc_Arg_Set_spec_struct*
_tmp4D=(struct Cyc_Arg_Set_spec_struct*)_tmp48;if(_tmp4D->tag != 3)goto _LL15;else{
_tmp4E=_tmp4D->f1;}}_LL14:*_tmp4E=1;goto _LLE;_LL15: {struct Cyc_Arg_Clear_spec_struct*
_tmp4F=(struct Cyc_Arg_Clear_spec_struct*)_tmp48;if(_tmp4F->tag != 4)goto _LL17;
else{_tmp50=_tmp4F->f1;}}_LL16:*_tmp50=0;goto _LLE;_LL17: {struct Cyc_Arg_FlagString_spec_struct*
_tmp51=(struct Cyc_Arg_FlagString_spec_struct*)_tmp48;if(_tmp51->tag != 2)goto
_LL19;else{_tmp52=_tmp51->f1;}}_LL18: if(Cyc_Arg_current + 1 < l){_tmp52(s,*((
struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct
_dyneither_ptr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(
initpos,(void*)({struct Cyc_Arg_Missing_struct*_tmp59=_cycalloc(sizeof(*_tmp59));
_tmp59[0]=({struct Cyc_Arg_Missing_struct _tmp5A;_tmp5A.tag=1;_tmp5A.f1=s;_tmp5A;});
_tmp59;}),speclist,errmsg);}goto _LLE;_LL19: {struct Cyc_Arg_String_spec_struct*
_tmp53=(struct Cyc_Arg_String_spec_struct*)_tmp48;if(_tmp53->tag != 5)goto _LL1B;
else{_tmp54=_tmp53->f1;}}_LL1A: if(Cyc_Arg_current + 1 < l){_tmp54(*((struct
_dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct
_dyneither_ptr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(
initpos,(void*)({struct Cyc_Arg_Missing_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B[0]=({struct Cyc_Arg_Missing_struct _tmp5C;_tmp5C.tag=1;_tmp5C.f1=s;_tmp5C;});
_tmp5B;}),speclist,errmsg);}goto _LLE;_LL1B: {struct Cyc_Arg_Int_spec_struct*
_tmp55=(struct Cyc_Arg_Int_spec_struct*)_tmp48;if(_tmp55->tag != 6)goto _LL1D;else{
_tmp56=_tmp55->f1;}}_LL1C: {struct _dyneither_ptr arg=*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1));int n=0;if(({struct Cyc_IntPtr_sa_struct _tmp5F;_tmp5F.tag=2;_tmp5F.f1=& n;({
void*_tmp5D[1]={& _tmp5F};Cyc_sscanf(arg,({const char*_tmp5E="%d";_tag_dyneither(
_tmp5E,sizeof(char),3);}),_tag_dyneither(_tmp5D,sizeof(void*),1));});})!= 1){Cyc_Arg_stop(
initpos,(void*)({struct Cyc_Arg_Wrong_struct*_tmp60=_cycalloc(sizeof(*_tmp60));
_tmp60[0]=({struct Cyc_Arg_Wrong_struct _tmp61;_tmp61.tag=3;_tmp61.f1=s;_tmp61.f2=
arg;_tmp61.f3=({const char*_tmp62="an integer";_tag_dyneither(_tmp62,sizeof(char),
11);});_tmp61;});_tmp60;}),speclist,errmsg);_npop_handler(0);return;}_tmp56(n);
++ Cyc_Arg_current;goto _LLE;}_LL1D: {struct Cyc_Arg_Rest_spec_struct*_tmp57=(
struct Cyc_Arg_Rest_spec_struct*)_tmp48;if(_tmp57->tag != 7)goto _LLE;else{_tmp58=
_tmp57->f1;}}_LL1E: while(Cyc_Arg_current < l - 1){_tmp58(*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}goto _LLE;_LLE:;};_pop_handler();}else{void*_tmp46=(
void*)_exn_thrown;void*_tmp64=_tmp46;struct _dyneither_ptr _tmp66;_LL20: {struct
Cyc_Arg_Bad_struct*_tmp65=(struct Cyc_Arg_Bad_struct*)_tmp64;if(_tmp65->tag != Cyc_Arg_Bad)
goto _LL22;else{_tmp66=_tmp65->f1;}}_LL21: Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Message_struct*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Arg_Message_struct _tmp68;
_tmp68.tag=2;_tmp68.f1=_tmp66;_tmp68;});_tmp67;}),speclist,errmsg);goto _LL1F;
_LL22:;_LL23:(void)_throw(_tmp64);_LL1F:;}};}++ Cyc_Arg_current;}else{{struct
_handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))
_tmp6B=1;if(!_tmp6B){anonfun(s);;_pop_handler();}else{void*_tmp6A=(void*)
_exn_thrown;void*_tmp6D=_tmp6A;struct _dyneither_ptr _tmp6F;_LL25: {struct Cyc_Arg_Bad_struct*
_tmp6E=(struct Cyc_Arg_Bad_struct*)_tmp6D;if(_tmp6E->tag != Cyc_Arg_Bad)goto _LL27;
else{_tmp6F=_tmp6E->f1;}}_LL26: Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Message_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Arg_Message_struct _tmp71;
_tmp71.tag=2;_tmp71.f1=_tmp6F;_tmp71;});_tmp70;}),speclist,errmsg);goto _LL24;
_LL27:;_LL28:(void)_throw(_tmp6D);_LL24:;}};}++ Cyc_Arg_current;}}};}
