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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};void*Cyc_Core_identity(void*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_struct{
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct
Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct
Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*
atts);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d,void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Interface_I;struct Cyc_Interface_I*
Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();struct Cyc_Interface_I*
Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple9{struct _dyneither_ptr
f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple9*info);struct Cyc_Interface_I*Cyc_Interface_merge(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple9*info);struct Cyc_Interface_I*
Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo);
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*
get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);void Cyc_Interface_print(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_parse(
struct Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,
Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple13{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple15*val;};
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
timestamp;int first_line;int first_column;int last_line;int last_column;};void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_function_type(void*t);extern struct
Cyc_Absyn_Kind Cyc_Tcutil_bk;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{char*
tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,
struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*
d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;
struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{
struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};static struct Cyc_Interface_Ienv*
Cyc_Interface_new_ienv();static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
struct Cyc_Interface_Ienv*_tmp25C;return(_tmp25C=_cycalloc(sizeof(*_tmp25C)),((
_tmp25C->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25C->datatypedecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25C->enumdecls=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((
_tmp25C->typedefdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25C->vardecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25C->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),
_tmp25C)))))))))))));}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv();
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){static struct Cyc_Interface_Ienv*
ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();return(
struct Cyc_Interface_Ienv*)ie;}static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(
void*ignored);static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(void*ignored){
return Cyc_Interface_new_ienv();}static struct Cyc_Interface_I*Cyc_Interface_skel2i(
struct Cyc_Dict_Dict skel);static struct Cyc_Interface_I*Cyc_Interface_skel2i(struct
Cyc_Dict_Dict skel){struct Cyc_Interface_I*_tmp25D;return(_tmp25D=_cycalloc(
sizeof(*_tmp25D)),((_tmp25D->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*
f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),((
_tmp25D->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),
struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),_tmp25D)))));}
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_empty(){
struct Cyc_Interface_I*_tmp25E;return(_tmp25E=_cycalloc(sizeof(*_tmp25E)),((
_tmp25E->imports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct
Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),((_tmp25E->exports=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp),_tmp25E)))));}struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_final(){struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();
struct Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();struct Cyc_Absyn_Datatypedecl*
_tmp25F;struct Cyc_Absyn_Datatypedecl*_tmp5=(_tmp25F=_cycalloc(sizeof(*_tmp25F)),((
_tmp25F[0]=*Cyc_Absyn_exn_tud,_tmp25F)));_tmp5->sc=Cyc_Absyn_Public;_tmp4->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,_tmp5);{struct
Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){struct Cyc_Absyn_Datatypefield*_tmp260;struct Cyc_Absyn_Datatypefield*
_tmp7=(_tmp260=_cycalloc(sizeof(*_tmp260)),((_tmp260[0]=*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd),_tmp260)));_tmp7->sc=Cyc_Absyn_Public;{struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp261;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=(_tmp261=_cycalloc(sizeof(*
_tmp261)),((_tmp261->base=_tmp5,((_tmp261->field=_tmp7,_tmp261)))));_tmp4->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd)->name).f2,_tmp8);};}}_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(
_tmp3->exports,0,_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();
struct _dyneither_ptr*_tmp269;const char*_tmp268;union Cyc_Absyn_Nmspace _tmp267;
struct _tuple0*_tmp266;struct _tuple0*qmain=(_tmp266=_cycalloc(sizeof(*_tmp266)),((
_tmp266->f1=(((_tmp267.Abs_n).val=0,(((_tmp267.Abs_n).tag=2,_tmp267)))),((
_tmp266->f2=((_tmp269=_cycalloc(sizeof(*_tmp269)),((_tmp269[0]=(struct
_dyneither_ptr)((_tmp268="main",_tag_dyneither(_tmp268,sizeof(char),5))),_tmp269)))),
_tmp266)))));struct Cyc_List_List*_tmp271;struct _tuple7*_tmp270;struct _tuple7*
_tmp26F;struct Cyc_List_List*_tmp26E;struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(
qmain,Cyc_Absyn_function_typ(0,0,Cyc_Absyn_sint_typ,((_tmp26E=_cycalloc(sizeof(*
_tmp26E)),((_tmp26E->hd=((_tmp26F=_cycalloc(sizeof(*_tmp26F)),((_tmp26F->f1=0,((
_tmp26F->f2=Cyc_Absyn_empty_tqual(0),((_tmp26F->f3=Cyc_Absyn_uint_typ,_tmp26F)))))))),((
_tmp26E->tl=((_tmp271=_cycalloc(sizeof(*_tmp271)),((_tmp271->hd=((_tmp270=
_cycalloc(sizeof(*_tmp270)),((_tmp270->f1=0,((_tmp270->f2=Cyc_Absyn_empty_tqual(
0),((_tmp270->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp270)))))))),((_tmp271->tl=0,_tmp271)))))),_tmp26E)))))),0,0,0,0),0);_tmpC->sc=
Cyc_Absyn_Extern;_tmpB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*
qmain).f2,_tmpC);_tmp3->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmpB);return _tmp3;};}static void Cyc_Interface_err(struct _dyneither_ptr msg);
static void Cyc_Interface_err(struct _dyneither_ptr msg){void*_tmp272;(_tmp272=0,Cyc_Tcutil_terr(
0,msg,_tag_dyneither(_tmp272,sizeof(void*),0)));}static void*Cyc_Interface_invalid_arg(
struct _dyneither_ptr s);static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr
s){struct Cyc_Core_Invalid_argument_struct _tmp275;struct Cyc_Core_Invalid_argument_struct*
_tmp274;(int)_throw((void*)((_tmp274=_cycalloc(sizeof(*_tmp274)),((_tmp274[0]=((
_tmp275.tag=Cyc_Core_Invalid_argument,((_tmp275.f1=s,_tmp275)))),_tmp274)))));}
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,
struct _tuple0*n);static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,
struct _dyneither_ptr t,struct _tuple0*n){const char*_tmp27B;void*_tmp27A[3];struct
Cyc_String_pa_struct _tmp279;struct Cyc_String_pa_struct _tmp278;struct Cyc_String_pa_struct
_tmp277;Cyc_Interface_err((struct _dyneither_ptr)((_tmp277.tag=0,((_tmp277.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n)),((
_tmp278.tag=0,((_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp279.tag=0,((_tmp279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((
_tmp27A[0]=& _tmp279,((_tmp27A[1]=& _tmp278,((_tmp27A[2]=& _tmp277,Cyc_aprintf(((
_tmp27B="fields of %s %s %s have never been defined",_tag_dyneither(_tmp27B,
sizeof(char),43))),_tag_dyneither(_tmp27A,sizeof(void*),3)))))))))))))))))))));}
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n);static
void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){const char*
_tmp280;void*_tmp27F[2];struct Cyc_String_pa_struct _tmp27E;struct Cyc_String_pa_struct
_tmp27D;Cyc_Interface_err((struct _dyneither_ptr)((_tmp27D.tag=0,((_tmp27D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n)),((
_tmp27E.tag=0,((_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((
_tmp27F[0]=& _tmp27E,((_tmp27F[1]=& _tmp27D,Cyc_aprintf(((_tmp280="the body of %s function %s has never been defined",
_tag_dyneither(_tmp280,sizeof(char),50))),_tag_dyneither(_tmp27F,sizeof(void*),2)))))))))))))));}
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,
struct _dyneither_ptr obj2,struct _tuple0*name2);static void Cyc_Interface_static_err(
struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct
_tuple0*name2){if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){const
char*_tmp287;void*_tmp286[4];struct Cyc_String_pa_struct _tmp285;struct Cyc_String_pa_struct
_tmp284;struct Cyc_String_pa_struct _tmp283;struct Cyc_String_pa_struct _tmp282;Cyc_Interface_err((
struct _dyneither_ptr)((_tmp282.tag=0,((_tmp282.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp283.tag=0,((_tmp283.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp284.tag=0,((_tmp284.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp285.tag=0,((_tmp285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp286[0]=& _tmp285,((_tmp286[1]=& _tmp284,((_tmp286[2]=& _tmp283,((_tmp286[3]=&
_tmp282,Cyc_aprintf(((_tmp287="declaration of %s %s relies on static %s %s",
_tag_dyneither(_tmp287,sizeof(char),44))),_tag_dyneither(_tmp286,sizeof(void*),4)))))))))))))))))))))))))));}}
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*
name1,struct _dyneither_ptr obj2,struct _tuple0*name2);static void Cyc_Interface_abstract_err(
struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct
_tuple0*name2){if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){const
char*_tmp28E;void*_tmp28D[4];struct Cyc_String_pa_struct _tmp28C;struct Cyc_String_pa_struct
_tmp28B;struct Cyc_String_pa_struct _tmp28A;struct Cyc_String_pa_struct _tmp289;Cyc_Interface_err((
struct _dyneither_ptr)((_tmp289.tag=0,((_tmp289.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp28A.tag=0,((_tmp28A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp28B.tag=0,((_tmp28B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp28C.tag=0,((_tmp28C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp28D[0]=& _tmp28C,((_tmp28D[1]=& _tmp28B,((_tmp28D[2]=& _tmp28A,((_tmp28D[3]=&
_tmp289,Cyc_aprintf(((_tmp28E="declaration of %s %s relies on fields of abstract %s %s",
_tag_dyneither(_tmp28E,sizeof(char),56))),_tag_dyneither(_tmp28D,sizeof(void*),4)))))))))))))))))))))))))));}}
struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;}
;static struct Cyc_Interface_Seen*Cyc_Interface_new_seen();static struct Cyc_Interface_Seen*
Cyc_Interface_new_seen(){struct Cyc_Interface_Seen*_tmp28F;return(_tmp28F=
_cycalloc(sizeof(*_tmp28F)),((_tmp28F->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp28F->datatypes=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),_tmp28F)))));}static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(struct
Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct
_tuple0*name,void*(*f)(void*),struct Cyc_List_List*l);static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){int _tmp2F=1;for(0;l != 
0;l=l->tl){if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f((void*)l->hd)))
_tmp2F=0;}return _tmp2F;}static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d);static int Cyc_Interface_check_public_aggrdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{
struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))
_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};
_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp35=_tmp31;_LL1: {
struct Cyc_Dict_Absent_struct*_tmp36=(struct Cyc_Dict_Absent_struct*)_tmp35;if(
_tmp36->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(
_tmp35);_LL0:;}};}{int _tmp37=1;seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp37);if(d->impl
!= 0){struct Cyc_List_List*_tmp38=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;
for(0;_tmp38 != 0;_tmp38=_tmp38->tl){const char*_tmp290;if(!Cyc_Interface_check_public_type(
ae,seen,((_tmp290="type",_tag_dyneither(_tmp290,sizeof(char),5))),d->name,((
struct Cyc_Absyn_Aggrfield*)_tmp38->hd)->type))_tmp37=0;}}seen->aggrs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,
_tmp37);return _tmp37;};}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static
void*Cyc_Interface_get_type1(struct _tuple16*x);static void*Cyc_Interface_get_type1(
struct _tuple16*x){return(*x).f2;}static void*Cyc_Interface_get_type2(struct
_tuple7*x);static void*Cyc_Interface_get_type2(struct _tuple7*x){return(*x).f3;}
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Datatypedecl*d);static int Cyc_Interface_check_public_datatypedecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*
d){{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(
_tmp3A.handler))_tmp3C=1;if(!_tmp3C){{int _tmp3D=((int(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);
return _tmp3D;};_pop_handler();}else{void*_tmp3B=(void*)_exn_thrown;void*_tmp3F=
_tmp3B;_LL6: {struct Cyc_Dict_Absent_struct*_tmp40=(struct Cyc_Dict_Absent_struct*)
_tmp3F;if(_tmp40->tag != Cyc_Dict_Absent)goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)
_throw(_tmp3F);_LL5:;}};}{int _tmp41=1;seen->datatypes=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,
_tmp41);if(d->fields != 0){struct Cyc_List_List*_tmp42=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
const char*_tmp291;if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple16*),
struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,((_tmp291="datatype",
_tag_dyneither(_tmp291,sizeof(char),9))),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Datatypefield*)_tmp42->hd)->typs))_tmp41=0;}}seen->datatypes=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->datatypes,d->name,_tmp41);return _tmp41;};}static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d);
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Enumdecl*d){return 1;}static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*
d);static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,struct
Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){if(d->defn != 0)return Cyc_Interface_check_public_type(
ae,seen,(struct _dyneither_ptr)_tag_dyneither(0,0,0),d->name,(void*)((struct Cyc_Core_Opt*)
_check_null(d->defn))->v);else{return 1;}}static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d);
static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Vardecl*d){const char*_tmp292;return Cyc_Interface_check_public_type(
ae,seen,((_tmp292="variable",_tag_dyneither(_tmp292,sizeof(char),9))),d->name,d->type);}
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns);
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns){
union Cyc_Absyn_Nmspace _tmp45=ns;struct Cyc_List_List*_tmp46;_LLB: if((_tmp45.Abs_n).tag
!= 2)goto _LLD;_tmp46=(struct Cyc_List_List*)(_tmp45.Abs_n).val;_LLC: return _tmp46;
_LLD:;_LLE: {const char*_tmp293;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp293="get_abs_ns",_tag_dyneither(_tmp293,sizeof(char),11))));}_LLA:;}static
int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*t){void*_tmp48=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp4A;void*_tmp4B;struct Cyc_Absyn_ArrayInfo _tmp4D;void*_tmp4E;void**_tmp50;void*
_tmp51;struct Cyc_Absyn_FnInfo _tmp53;void*_tmp54;struct Cyc_List_List*_tmp55;
struct Cyc_Absyn_VarargInfo*_tmp56;struct Cyc_List_List*_tmp58;struct Cyc_Absyn_AggrInfo
_tmp5A;union Cyc_Absyn_AggrInfoU _tmp5B;struct Cyc_List_List*_tmp5C;struct _tuple0*
_tmp5E;struct Cyc_Absyn_DatatypeInfo _tmp60;union Cyc_Absyn_DatatypeInfoU _tmp61;
struct Cyc_Absyn_Datatypedecl**_tmp62;struct Cyc_Absyn_Datatypedecl*_tmp63;struct
Cyc_List_List*_tmp64;struct Cyc_Absyn_DatatypeFieldInfo _tmp66;union Cyc_Absyn_DatatypeFieldInfoU
_tmp67;struct _tuple1 _tmp68;struct Cyc_Absyn_Datatypedecl*_tmp69;struct Cyc_Absyn_Datatypefield*
_tmp6A;struct Cyc_List_List*_tmp6B;_LL10: {struct Cyc_Absyn_PointerType_struct*
_tmp49=(struct Cyc_Absyn_PointerType_struct*)_tmp48;if(_tmp49->tag != 5)goto _LL12;
else{_tmp4A=_tmp49->f1;_tmp4B=_tmp4A.elt_typ;}}_LL11: _tmp4E=_tmp4B;goto _LL13;
_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp4C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp48;if(_tmp4C->tag != 9)goto _LL14;else{_tmp4D=_tmp4C->f1;_tmp4E=_tmp4D.elt_type;}}
_LL13: _tmp51=_tmp4E;goto _LL15;_LL14: {struct Cyc_Absyn_TypedefType_struct*_tmp4F=(
struct Cyc_Absyn_TypedefType_struct*)_tmp48;if(_tmp4F->tag != 18)goto _LL16;else{
_tmp50=_tmp4F->f4;if(_tmp50 == 0)goto _LL16;_tmp51=*_tmp50;}}_LL15: return Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp51);_LL16: {struct Cyc_Absyn_FnType_struct*_tmp52=(struct Cyc_Absyn_FnType_struct*)
_tmp48;if(_tmp52->tag != 10)goto _LL18;else{_tmp53=_tmp52->f1;_tmp54=_tmp53.ret_typ;
_tmp55=_tmp53.args;_tmp56=_tmp53.cyc_varargs;}}_LL17: {int b=((int(*)(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(struct _tuple7*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type2,_tmp55) && Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp54);if(_tmp56 != 0){void*_tmp6D;struct Cyc_Absyn_VarargInfo
_tmp6C=*_tmp56;_tmp6D=_tmp6C.type;b=Cyc_Interface_check_public_type(ae,seen,obj,
name,_tmp6D);}return b;}_LL18: {struct Cyc_Absyn_TupleType_struct*_tmp57=(struct
Cyc_Absyn_TupleType_struct*)_tmp48;if(_tmp57->tag != 11)goto _LL1A;else{_tmp58=
_tmp57->f1;}}_LL19: return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple16*),
struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,
Cyc_Interface_get_type1,_tmp58);_LL1A: {struct Cyc_Absyn_AggrType_struct*_tmp59=(
struct Cyc_Absyn_AggrType_struct*)_tmp48;if(_tmp59->tag != 12)goto _LL1C;else{
_tmp5A=_tmp59->f1;_tmp5B=_tmp5A.aggr_info;_tmp5C=_tmp5A.targs;}}_LL1B: {struct
Cyc_Absyn_Aggrdecl*_tmp6E=Cyc_Absyn_get_known_aggrdecl(_tmp5B);if(_tmp6E->sc == 
Cyc_Absyn_Static){{const char*_tmp294;Cyc_Interface_static_err(obj,name,((_tmp294="type",
_tag_dyneither(_tmp294,sizeof(char),5))),_tmp6E->name);}return 0;}return((int(*)(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp5C) && Cyc_Interface_check_public_aggrdecl(
ae,seen,_tmp6E);}_LL1C: {struct Cyc_Absyn_EnumType_struct*_tmp5D=(struct Cyc_Absyn_EnumType_struct*)
_tmp48;if(_tmp5D->tag != 14)goto _LL1E;else{_tmp5E=_tmp5D->f1;}}_LL1D: {struct
_tuple0 _tmp71;union Cyc_Absyn_Nmspace _tmp72;struct _dyneither_ptr*_tmp73;struct
_tuple0*_tmp70=_tmp5E;_tmp71=*_tmp70;_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;{struct
Cyc_List_List*_tmp74=Cyc_Interface_get_abs_ns(_tmp72);struct Cyc_Absyn_Enumdecl*
ed;{struct _handler_cons _tmp75;_push_handler(& _tmp75);{int _tmp77=0;if(setjmp(
_tmp75.handler))_tmp77=1;if(!_tmp77){{struct Cyc_Tcenv_Genv*_tmp78=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp74);ed=*((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp78->enumdecls,_tmp73);};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;
void*_tmp7A=_tmp76;_LL25: {struct Cyc_Dict_Absent_struct*_tmp7B=(struct Cyc_Dict_Absent_struct*)
_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)goto _LL27;}_LL26: {const char*_tmp298;
void*_tmp297[1];struct Cyc_String_pa_struct _tmp296;((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp296.tag=0,((_tmp296.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5E)),((
_tmp297[0]=& _tmp296,Cyc_aprintf(((_tmp298="check_public_type (can't find enum %s)",
_tag_dyneither(_tmp298,sizeof(char),39))),_tag_dyneither(_tmp297,sizeof(void*),1)))))))));}
_LL27:;_LL28:(void)_throw(_tmp7A);_LL24:;}};}if(ed->sc == Cyc_Absyn_Static){{
const char*_tmp299;Cyc_Interface_static_err(obj,_tmp5E,((_tmp299="enum",
_tag_dyneither(_tmp299,sizeof(char),5))),ed->name);}return 0;}return 1;};}_LL1E: {
struct Cyc_Absyn_DatatypeType_struct*_tmp5F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp48;if(_tmp5F->tag != 3)goto _LL20;else{_tmp60=_tmp5F->f1;_tmp61=_tmp60.datatype_info;
if((_tmp61.KnownDatatype).tag != 2)goto _LL20;_tmp62=(struct Cyc_Absyn_Datatypedecl**)(
_tmp61.KnownDatatype).val;_tmp63=*_tmp62;_tmp64=_tmp60.targs;}}_LL1F: {struct
_tuple0 _tmp81;union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct
_tuple0*_tmp80=_tmp63->name;_tmp81=*_tmp80;_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;{
struct Cyc_List_List*_tmp84=Cyc_Interface_get_abs_ns(_tmp82);struct Cyc_Absyn_Datatypedecl*
tud;{struct _handler_cons _tmp85;_push_handler(& _tmp85);{int _tmp87=0;if(setjmp(
_tmp85.handler))_tmp87=1;if(!_tmp87){{struct Cyc_Tcenv_Genv*_tmp88=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp84);tud=*((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp88->datatypedecls,_tmp83);};_pop_handler();}else{void*
_tmp86=(void*)_exn_thrown;void*_tmp8A=_tmp86;_LL2A: {struct Cyc_Dict_Absent_struct*
_tmp8B=(struct Cyc_Dict_Absent_struct*)_tmp8A;if(_tmp8B->tag != Cyc_Dict_Absent)
goto _LL2C;}_LL2B: {const char*_tmp29D;void*_tmp29C[1];struct Cyc_String_pa_struct
_tmp29B;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp29B.tag=0,((_tmp29B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp63->name)),((_tmp29C[0]=& _tmp29B,Cyc_aprintf(((
_tmp29D="check_public_type (can't find datatype %s)",_tag_dyneither(_tmp29D,
sizeof(char),43))),_tag_dyneither(_tmp29C,sizeof(void*),1)))))))));}_LL2C:;_LL2D:(
void)_throw(_tmp8A);_LL29:;}};}if(tud->sc == Cyc_Absyn_Static){{const char*_tmp29E;
Cyc_Interface_static_err(obj,name,((_tmp29E="datatype",_tag_dyneither(_tmp29E,
sizeof(char),9))),tud->name);}return 0;}return((int(*)(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,
obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp64) && Cyc_Interface_check_public_datatypedecl(
ae,seen,tud);};}_LL20: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp65=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp48;if(_tmp65->tag != 4)goto _LL22;else{
_tmp66=_tmp65->f1;_tmp67=_tmp66.field_info;if((_tmp67.KnownDatatypefield).tag != 
2)goto _LL22;_tmp68=(struct _tuple1)(_tmp67.KnownDatatypefield).val;_tmp69=_tmp68.f1;
_tmp6A=_tmp68.f2;_tmp6B=_tmp66.targs;}}_LL21: {struct _tuple0 _tmp91;union Cyc_Absyn_Nmspace
_tmp92;struct _dyneither_ptr*_tmp93;struct _tuple0*_tmp90=_tmp69->name;_tmp91=*
_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{struct Cyc_List_List*_tmp94=Cyc_Interface_get_abs_ns(
_tmp92);struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _tmp95;
_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))_tmp97=1;if(!
_tmp97){{struct Cyc_Tcenv_Genv*_tmp98=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp94);tud=*((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp98->datatypedecls,
_tmp93);};_pop_handler();}else{void*_tmp96=(void*)_exn_thrown;void*_tmp9A=_tmp96;
_LL2F: {struct Cyc_Dict_Absent_struct*_tmp9B=(struct Cyc_Dict_Absent_struct*)
_tmp9A;if(_tmp9B->tag != Cyc_Dict_Absent)goto _LL31;}_LL30: {const char*_tmp2A2;
void*_tmp2A1[1];struct Cyc_String_pa_struct _tmp2A0;((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp2A0.tag=0,((_tmp2A0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69->name)),((
_tmp2A1[0]=& _tmp2A0,Cyc_aprintf(((_tmp2A2="check_public_type (can't find datatype %s and search its fields)",
_tag_dyneither(_tmp2A2,sizeof(char),65))),_tag_dyneither(_tmp2A1,sizeof(void*),1)))))))));}
_LL31:;_LL32:(void)_throw(_tmp9A);_LL2E:;}};}if(tud->fields == 0){const char*
_tmp2A6;void*_tmp2A5[1];struct Cyc_String_pa_struct _tmp2A4;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp2A4.tag=
0,((_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp2A5[0]=& _tmp2A4,Cyc_aprintf(((_tmp2A6="check_public_type (datatype %s has no fields)",
_tag_dyneither(_tmp2A6,sizeof(char),46))),_tag_dyneither(_tmp2A5,sizeof(void*),1)))))))));}{
struct Cyc_Absyn_Datatypefield*tuf1=0;{struct Cyc_List_List*_tmpA2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
if(Cyc_strptrcmp((*_tmp6A->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmpA2->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
_tmpA2->hd);break;}}}if(tuf1 == 0){const char*_tmp2AA;void*_tmp2A9[1];struct Cyc_String_pa_struct
_tmp2A8;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp2A8.tag=0,((_tmp2A8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6A->name)),((_tmp2A9[0]=& _tmp2A8,Cyc_aprintf(((
_tmp2AA="check_public_type (can't find datatypefield %s)",_tag_dyneither(_tmp2AA,
sizeof(char),48))),_tag_dyneither(_tmp2A9,sizeof(void*),1)))))))));}{struct Cyc_Absyn_Datatypefield*
tuf=(struct Cyc_Absyn_Datatypefield*)tuf1;if(tud->sc == Cyc_Absyn_Static){{const
char*_tmp2AB;Cyc_Interface_static_err(obj,name,((_tmp2AB="datatype",
_tag_dyneither(_tmp2AB,sizeof(char),9))),tud->name);}return 0;}if(tud->sc == Cyc_Absyn_Abstract){{
const char*_tmp2AC;Cyc_Interface_abstract_err(obj,name,((_tmp2AC="datatype",
_tag_dyneither(_tmp2AC,sizeof(char),9))),tud->name);}return 0;}if(tuf->sc == Cyc_Absyn_Static){{
const char*_tmp2B0;void*_tmp2AF[1];struct Cyc_String_pa_struct _tmp2AE;Cyc_Interface_static_err(
obj,name,(struct _dyneither_ptr)((_tmp2AE.tag=0,((_tmp2AE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp2AF[0]=& _tmp2AE,Cyc_aprintf(((_tmp2B0="field %s of",_tag_dyneither(_tmp2B0,
sizeof(char),12))),_tag_dyneither(_tmp2AF,sizeof(void*),1)))))))),tud->name);}
return 0;}return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,
struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,
_tmp6B) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};};}_LL22:;
_LL23: return 1;_LLF:;}struct _tuple17{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*
f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_aggrdecl(struct _tuple17*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp);static void Cyc_Interface_extract_aggrdecl(
struct _tuple17*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){struct
_tuple17 _tmpAC;struct Cyc_Interface_Ienv*_tmpAD;struct Cyc_Interface_Ienv*_tmpAE;
int _tmpAF;struct Cyc_Dict_Dict _tmpB0;struct Cyc_Interface_Seen*_tmpB1;struct
_tuple17*_tmpAB=env;_tmpAC=*_tmpAB;_tmpAD=_tmpAC.f1;_tmpAE=_tmpAC.f2;_tmpAF=
_tmpAC.f3;_tmpB0=_tmpAC.f4;_tmpB1=_tmpAC.f5;{struct Cyc_Absyn_Aggrdecl*_tmpB2=*dp;
switch(_tmpB2->sc){case Cyc_Absyn_Static: _LL33: if(_tmpAF  && _tmpB2->impl == 0){
const char*_tmp2B2;const char*_tmp2B1;Cyc_Interface_fields_err(((_tmp2B1="static",
_tag_dyneither(_tmp2B1,sizeof(char),7))),((_tmp2B2="struct/union",_tag_dyneither(
_tmp2B2,sizeof(char),13))),_tmpB2->name);}break;case Cyc_Absyn_Abstract: _LL34: if(
_tmpB2->impl == 0){if(_tmpAF){const char*_tmp2B4;const char*_tmp2B3;Cyc_Interface_fields_err(((
_tmp2B3="abstract",_tag_dyneither(_tmp2B3,sizeof(char),9))),((_tmp2B4="struct/union",
_tag_dyneither(_tmp2B4,sizeof(char),13))),_tmpB2->name);}}else{{struct Cyc_Absyn_Aggrdecl*
_tmp2B5;_tmpB2=((_tmp2B5=_cycalloc(sizeof(*_tmp2B5)),((_tmp2B5[0]=*_tmpB2,
_tmp2B5))));}_tmpB2->impl=0;}if(Cyc_Interface_check_public_aggrdecl(_tmpB0,
_tmpB1,_tmpB2))_tmpAE->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpAE->aggrdecls,
x,_tmpB2);break;case Cyc_Absyn_Public: _LL35: if(_tmpB2->impl == 0){{const char*
_tmp2B7;const char*_tmp2B6;Cyc_Interface_fields_err(((_tmp2B6="public",
_tag_dyneither(_tmp2B6,sizeof(char),7))),((_tmp2B7="struct/union",_tag_dyneither(
_tmp2B7,sizeof(char),13))),_tmpB2->name);}{struct Cyc_Absyn_Aggrdecl*_tmp2B8;
_tmpB2=((_tmp2B8=_cycalloc(sizeof(*_tmp2B8)),((_tmp2B8[0]=*_tmpB2,_tmp2B8))));}
_tmpB2->sc=Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_aggrdecl(_tmpB0,
_tmpB1,_tmpB2))_tmpAE->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpAE->aggrdecls,
x,_tmpB2);break;case Cyc_Absyn_ExternC: _LL36: goto _LL37;case Cyc_Absyn_Extern: _LL37:
if(Cyc_Interface_check_public_aggrdecl(_tmpB0,_tmpB1,_tmpB2))_tmpAD->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmpAD->aggrdecls,x,_tmpB2);break;case Cyc_Absyn_Register:
_LL38:{const char*_tmp2B9;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2B9="add_aggrdecl",_tag_dyneither(_tmp2B9,sizeof(char),13))));}break;}};}
static void Cyc_Interface_extract_enumdecl(struct _tuple17*env,struct _dyneither_ptr*
x,struct Cyc_Absyn_Enumdecl**dp);static void Cyc_Interface_extract_enumdecl(struct
_tuple17*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl**dp){struct _tuple17
_tmpBD;struct Cyc_Interface_Ienv*_tmpBE;struct Cyc_Interface_Ienv*_tmpBF;int _tmpC0;
struct Cyc_Dict_Dict _tmpC1;struct Cyc_Interface_Seen*_tmpC2;struct _tuple17*_tmpBC=
env;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;_tmpC0=_tmpBD.f3;_tmpC1=
_tmpBD.f4;_tmpC2=_tmpBD.f5;{struct Cyc_Absyn_Enumdecl*_tmpC3=*dp;switch(_tmpC3->sc){
case Cyc_Absyn_Static: _LL3A: if(_tmpC0  && _tmpC3->fields == 0){const char*_tmp2BB;
const char*_tmp2BA;Cyc_Interface_fields_err(((_tmp2BA="static",_tag_dyneither(
_tmp2BA,sizeof(char),7))),((_tmp2BB="enum",_tag_dyneither(_tmp2BB,sizeof(char),5))),
_tmpC3->name);}break;case Cyc_Absyn_Abstract: _LL3B: if(_tmpC3->fields == 0){if(
_tmpC0){const char*_tmp2BD;const char*_tmp2BC;Cyc_Interface_fields_err(((_tmp2BC="abstract",
_tag_dyneither(_tmp2BC,sizeof(char),9))),((_tmp2BD="enum",_tag_dyneither(_tmp2BD,
sizeof(char),5))),_tmpC3->name);}}else{{struct Cyc_Absyn_Enumdecl*_tmp2BE;_tmpC3=((
_tmp2BE=_cycalloc(sizeof(*_tmp2BE)),((_tmp2BE[0]=*_tmpC3,_tmp2BE))));}_tmpC3->fields=
0;}if(Cyc_Interface_check_public_enumdecl(_tmpC1,_tmpC2,_tmpC3))_tmpBF->enumdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpBF->enumdecls,x,_tmpC3);break;case Cyc_Absyn_Public: _LL3C:
if(_tmpC3->fields == 0){{const char*_tmp2C0;const char*_tmp2BF;Cyc_Interface_fields_err(((
_tmp2BF="public",_tag_dyneither(_tmp2BF,sizeof(char),7))),((_tmp2C0="enum",
_tag_dyneither(_tmp2C0,sizeof(char),5))),_tmpC3->name);}{struct Cyc_Absyn_Enumdecl*
_tmp2C1;_tmpC3=((_tmp2C1=_cycalloc(sizeof(*_tmp2C1)),((_tmp2C1[0]=*_tmpC3,
_tmp2C1))));}_tmpC3->sc=Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_enumdecl(
_tmpC1,_tmpC2,_tmpC3))_tmpBF->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpBF->enumdecls,
x,_tmpC3);break;case Cyc_Absyn_ExternC: _LL3D: goto _LL3E;case Cyc_Absyn_Extern: _LL3E:
if(Cyc_Interface_check_public_enumdecl(_tmpC1,_tmpC2,_tmpC3))_tmpBE->enumdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpBE->enumdecls,x,_tmpC3);break;case Cyc_Absyn_Register:
_LL3F:{const char*_tmp2C2;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2C2="add_enumdecl",_tag_dyneither(_tmp2C2,sizeof(char),13))));}break;}};}
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f);static void Cyc_Interface_extract_xdatatypefielddecl(
struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*
f){struct Cyc_List_List*_tmpCD=0;{union Cyc_Absyn_Nmspace _tmpCE=(*f->name).f1;
struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;_LL42: if((_tmpCE.Rel_n).tag
!= 1)goto _LL44;_tmpCF=(struct Cyc_List_List*)(_tmpCE.Rel_n).val;_LL43: _tmpD0=
_tmpCF;goto _LL45;_LL44: if((_tmpCE.Abs_n).tag != 2)goto _LL46;_tmpD0=(struct Cyc_List_List*)(
_tmpCE.Abs_n).val;_LL45: _tmpCD=_tmpD0;goto _LL41;_LL46:;_LL47: goto _LL41;_LL41:;}{
struct Cyc_Dict_Dict*dict;switch(f->sc){case Cyc_Absyn_Static: _LL48: return;case Cyc_Absyn_Extern:
_LL49: dict=& i->imports;break;case Cyc_Absyn_Public: _LL4A: dict=& i->exports;break;
default: _LL4B: {const char*_tmp2C3;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2C3="add_xdatatypefielddecl",_tag_dyneither(_tmp2C3,sizeof(char),23))));}}{
struct Cyc_Interface_Ienv*env;{struct _handler_cons _tmpD2;_push_handler(& _tmpD2);{
int _tmpD4=0;if(setjmp(_tmpD2.handler))_tmpD4=1;if(!_tmpD4){env=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpCD);;
_pop_handler();}else{void*_tmpD3=(void*)_exn_thrown;void*_tmpD6=_tmpD3;_LL4E: {
struct Cyc_Dict_Absent_struct*_tmpD7=(struct Cyc_Dict_Absent_struct*)_tmpD6;if(
_tmpD7->tag != Cyc_Dict_Absent)goto _LL50;}_LL4F: env=Cyc_Interface_new_ienv();*
dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct
Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*dict,_tmpCD,env);goto _LL4D;_LL50:;_LL51:(
void)_throw(_tmpD6);_LL4D:;}};}{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2C4;env->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,((_tmp2C4=_cycalloc(
sizeof(*_tmp2C4)),((_tmp2C4->base=d,((_tmp2C4->field=f,_tmp2C4)))))));};};};}
static void Cyc_Interface_extract_datatypedecl(struct _tuple17*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp);static void Cyc_Interface_extract_datatypedecl(
struct _tuple17*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){
struct _tuple17 _tmpDA;struct Cyc_Interface_Ienv*_tmpDB;struct Cyc_Interface_Ienv*
_tmpDC;int _tmpDD;struct Cyc_Dict_Dict _tmpDE;struct Cyc_Interface_Seen*_tmpDF;
struct Cyc_Interface_I*_tmpE0;struct _tuple17*_tmpD9=env;_tmpDA=*_tmpD9;_tmpDB=
_tmpDA.f1;_tmpDC=_tmpDA.f2;_tmpDD=_tmpDA.f3;_tmpDE=_tmpDA.f4;_tmpDF=_tmpDA.f5;
_tmpE0=_tmpDA.f6;{struct Cyc_Absyn_Datatypedecl*_tmpE1=*dp;switch(_tmpE1->sc){
case Cyc_Absyn_Static: _LL52: if((!_tmpE1->is_extensible  && _tmpDD) && _tmpE1->fields
== 0){const char*_tmp2C6;const char*_tmp2C5;Cyc_Interface_fields_err(((_tmp2C5="static",
_tag_dyneither(_tmp2C5,sizeof(char),7))),((_tmp2C6="datatype",_tag_dyneither(
_tmp2C6,sizeof(char),9))),_tmpE1->name);}break;case Cyc_Absyn_Abstract: _LL53: if(
_tmpE1->fields == 0){if(!_tmpE1->is_extensible  && _tmpDD){const char*_tmp2C8;const
char*_tmp2C7;Cyc_Interface_fields_err(((_tmp2C7="abstract",_tag_dyneither(
_tmp2C7,sizeof(char),9))),((_tmp2C8="datatype",_tag_dyneither(_tmp2C8,sizeof(
char),9))),_tmpE1->name);}}else{{struct Cyc_Absyn_Datatypedecl*_tmp2C9;_tmpE1=((
_tmp2C9=_cycalloc(sizeof(*_tmp2C9)),((_tmp2C9[0]=*_tmpE1,_tmp2C9))));}_tmpE1->fields=
0;}if(Cyc_Interface_check_public_datatypedecl(_tmpDE,_tmpDF,_tmpE1))_tmpDC->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpDC->datatypedecls,x,_tmpE1);break;case Cyc_Absyn_Public:
_LL54:{struct Cyc_Absyn_Datatypedecl*_tmp2CA;_tmpE1=((_tmp2CA=_cycalloc(sizeof(*
_tmp2CA)),((_tmp2CA[0]=*_tmpE1,_tmp2CA))));}if(!_tmpE1->is_extensible  && _tmpE1->fields
== 0){{const char*_tmp2CC;const char*_tmp2CB;Cyc_Interface_fields_err(((_tmp2CB="public",
_tag_dyneither(_tmp2CB,sizeof(char),7))),((_tmp2CC="datatype",_tag_dyneither(
_tmp2CC,sizeof(char),9))),_tmpE1->name);}_tmpE1->sc=Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_datatypedecl(
_tmpDE,_tmpDF,_tmpE1)){if(_tmpE1->is_extensible  && _tmpE1->fields != 0){struct Cyc_List_List*
_tmpEA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpE1->fields))->v;
_tmpE1->fields=0;{struct Cyc_List_List*_tmpEB=_tmpEA;for(0;_tmpEB != 0;_tmpEB=
_tmpEB->tl){Cyc_Interface_extract_xdatatypefielddecl(_tmpE0,_tmpE1,(struct Cyc_Absyn_Datatypefield*)
_tmpEB->hd);}};}_tmpDC->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpDC->datatypedecls,
x,_tmpE1);}break;case Cyc_Absyn_ExternC: _LL55: {const char*_tmp2CD;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2CD="extract_datatypedecl",
_tag_dyneither(_tmp2CD,sizeof(char),21))));}case Cyc_Absyn_Extern: _LL56: if(Cyc_Interface_check_public_datatypedecl(
_tmpDE,_tmpDF,_tmpE1)){if(_tmpE1->is_extensible  && _tmpE1->fields != 0){{struct
Cyc_Absyn_Datatypedecl*_tmp2CE;_tmpE1=((_tmp2CE=_cycalloc(sizeof(*_tmp2CE)),((
_tmp2CE[0]=*_tmpE1,_tmp2CE))));}{struct Cyc_List_List*_tmpEE=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpE1->fields))->v;_tmpE1->fields=0;{struct Cyc_List_List*
_tmpEF=_tmpEE;for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){Cyc_Interface_extract_xdatatypefielddecl(
_tmpE0,_tmpE1,(struct Cyc_Absyn_Datatypefield*)_tmpEF->hd);}};};}_tmpDB->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpDB->datatypedecls,x,_tmpE1);}break;case Cyc_Absyn_Register:
_LL57:{const char*_tmp2CF;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2CF="add_datatypedecl",_tag_dyneither(_tmp2CF,sizeof(char),17))));}break;}};}
static void Cyc_Interface_extract_typedef(struct _tuple17*env,struct _dyneither_ptr*
x,struct Cyc_Absyn_Typedefdecl*d);static void Cyc_Interface_extract_typedef(struct
_tuple17*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d){struct
_tuple17 _tmpF2;struct Cyc_Interface_Ienv*_tmpF3;struct Cyc_Dict_Dict _tmpF4;struct
Cyc_Interface_Seen*_tmpF5;struct _tuple17*_tmpF1=env;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f2;
_tmpF4=_tmpF2.f4;_tmpF5=_tmpF2.f5;if(Cyc_Interface_check_public_typedefdecl(
_tmpF4,_tmpF5,d))_tmpF3->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpF3->typedefdecls,
x,d);}struct _tuple18{void*f1;int f2;};static void Cyc_Interface_extract_ordinarie(
struct _tuple17*env,struct _dyneither_ptr*x,struct _tuple18*v);static void Cyc_Interface_extract_ordinarie(
struct _tuple17*env,struct _dyneither_ptr*x,struct _tuple18*v){struct _tuple17 _tmpF7;
struct Cyc_Interface_Ienv*_tmpF8;struct Cyc_Interface_Ienv*_tmpF9;int _tmpFA;struct
Cyc_Dict_Dict _tmpFB;struct Cyc_Interface_Seen*_tmpFC;struct _tuple17*_tmpF6=env;
_tmpF7=*_tmpF6;_tmpF8=_tmpF7.f1;_tmpF9=_tmpF7.f2;_tmpFA=_tmpF7.f3;_tmpFB=_tmpF7.f4;
_tmpFC=_tmpF7.f5;{void*_tmpFD=(*v).f1;void*_tmpFE=_tmpFD;void*_tmp100;_LL5A: {
struct Cyc_Tcenv_VarRes_struct*_tmpFF=(struct Cyc_Tcenv_VarRes_struct*)_tmpFE;if(
_tmpFF->tag != 0)goto _LL5C;else{_tmp100=(void*)_tmpFF->f1;}}_LL5B:{void*_tmp101=
_tmp100;struct Cyc_Absyn_Fndecl*_tmp103;struct Cyc_Absyn_Vardecl*_tmp105;_LL5F: {
struct Cyc_Absyn_Funname_b_struct*_tmp102=(struct Cyc_Absyn_Funname_b_struct*)
_tmp101;if(_tmp102->tag != 2)goto _LL61;else{_tmp103=_tmp102->f1;}}_LL60: {struct
Cyc_Absyn_Vardecl*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp106=(_tmp2D0=_cycalloc(
sizeof(*_tmp2D0)),((_tmp2D0->sc=_tmp103->sc,((_tmp2D0->name=_tmp103->name,((
_tmp2D0->tq=Cyc_Absyn_empty_tqual(0),((_tmp2D0->type=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp103->cached_typ))->v,((_tmp2D0->initializer=0,((_tmp2D0->rgn=0,((
_tmp2D0->attributes=0,((_tmp2D0->escapes=0,_tmp2D0)))))))))))))))));_tmpFA=0;
_tmp105=_tmp106;goto _LL62;}_LL61: {struct Cyc_Absyn_Global_b_struct*_tmp104=(
struct Cyc_Absyn_Global_b_struct*)_tmp101;if(_tmp104->tag != 1)goto _LL63;else{
_tmp105=_tmp104->f1;}}_LL62: if(_tmp105->initializer != 0){{struct Cyc_Absyn_Vardecl*
_tmp2D1;_tmp105=((_tmp2D1=_cycalloc(sizeof(*_tmp2D1)),((_tmp2D1[0]=*_tmp105,
_tmp2D1))));}_tmp105->initializer=0;}switch(_tmp105->sc){case Cyc_Absyn_Static:
_LL65: if(_tmpFA  && Cyc_Tcutil_is_function_type(_tmp105->type)){const char*_tmp2D2;
Cyc_Interface_body_err(((_tmp2D2="static",_tag_dyneither(_tmp2D2,sizeof(char),7))),
_tmp105->name);}break;case Cyc_Absyn_Register: _LL66: goto _LL67;case Cyc_Absyn_Abstract:
_LL67: {const char*_tmp2D3;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2D3="extract_ordinarie",_tag_dyneither(_tmp2D3,sizeof(char),18))));}case Cyc_Absyn_Public:
_LL68: if(_tmpFA  && Cyc_Tcutil_is_function_type(_tmp105->type)){const char*_tmp2D4;
Cyc_Interface_body_err(((_tmp2D4="public",_tag_dyneither(_tmp2D4,sizeof(char),7))),
_tmp105->name);}if(Cyc_Interface_check_public_vardecl(_tmpFB,_tmpFC,_tmp105))
_tmpF9->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpF9->vardecls,x,
_tmp105);break;case Cyc_Absyn_ExternC: _LL69: goto _LL6A;case Cyc_Absyn_Extern: _LL6A:
if(Cyc_Interface_check_public_vardecl(_tmpFB,_tmpFC,_tmp105))_tmpF8->vardecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpF8->vardecls,x,_tmp105);break;}goto _LL5E;_LL63:;_LL64:
goto _LL5E;_LL5E:;}goto _LL59;_LL5C:;_LL5D: goto _LL59;_LL59:;};}struct _tuple19{
struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;
};static void Cyc_Interface_extract_f(struct _tuple19*env_f,struct Cyc_List_List*ns,
struct Cyc_Tcenv_Genv*ge);static void Cyc_Interface_extract_f(struct _tuple19*env_f,
struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){struct _tuple19 _tmp10D;struct Cyc_Interface_I*
_tmp10E;int _tmp10F;struct Cyc_Dict_Dict _tmp110;struct Cyc_Interface_Seen*_tmp111;
struct _tuple19*_tmp10C=env_f;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;
_tmp110=_tmp10D.f3;_tmp111=_tmp10D.f4;{struct _tuple17 _tmp2D5;struct _tuple17
_tmp112=(_tmp2D5.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(_tmp10E->imports,ns),((_tmp2D5.f2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp10E->exports,ns),((
_tmp2D5.f3=_tmp10F,((_tmp2D5.f4=_tmp110,((_tmp2D5.f5=_tmp111,((_tmp2D5.f6=
_tmp10E,_tmp2D5)))))))))));((void(*)(void(*f)(struct _tuple17*,struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple17*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp112,ge->aggrdecls);((
void(*)(void(*f)(struct _tuple17*,struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl**),
struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,&
_tmp112,ge->datatypedecls);((void(*)(void(*f)(struct _tuple17*,struct
_dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple17*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmp112,ge->enumdecls);((
void(*)(void(*f)(struct _tuple17*,struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*),
struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,&
_tmp112,ge->typedefs);((void(*)(void(*f)(struct _tuple17*,struct _dyneither_ptr*,
struct _tuple18*),struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,&
_tmp112,ge->ordinaries);};}static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict ae,int check_complete_defs);static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict ae,int check_complete_defs){struct _tuple19 _tmp2D6;struct
_tuple19 env=(_tmp2D6.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(
ae),((_tmp2D6.f2=check_complete_defs,((_tmp2D6.f3=ae,((_tmp2D6.f4=Cyc_Interface_new_seen(),
_tmp2D6)))))));((void(*)(void(*f)(struct _tuple19*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),
struct _tuple19*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,&
env,ae);return env.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae){return Cyc_Interface_gen_extract(
ae,1);}static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct
_dyneither_ptr msg2);inline static void Cyc_Interface_check_err(struct _dyneither_ptr*
msg1,struct _dyneither_ptr msg2){const char*_tmp2DA;void*_tmp2D9[1];struct Cyc_String_pa_struct
_tmp2D8;(_tmp2D8.tag=0,((_tmp2D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg2),((_tmp2D9[0]=& _tmp2D8,Cyc_Tcdecl_merr(0,msg1,((_tmp2DA="%s",_tag_dyneither(
_tmp2DA,sizeof(char),3))),_tag_dyneither(_tmp2D9,sizeof(void*),1)))))));}struct
_tuple20{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);
struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};static void Cyc_Interface_incl_dict_f(
struct _tuple20*env,struct _dyneither_ptr*x,void*y1);static void Cyc_Interface_incl_dict_f(
struct _tuple20*env,struct _dyneither_ptr*x,void*y1){struct _tuple20 _tmp119;int
_tmp11A;int*_tmp11B;struct Cyc_Dict_Dict _tmp11C;int(*_tmp11D)(void*,void*,struct
_dyneither_ptr*);struct _dyneither_ptr _tmp11E;struct _dyneither_ptr*_tmp11F;struct
_tuple20*_tmp118=env;_tmp119=*_tmp118;_tmp11A=_tmp119.f1;_tmp11B=(int*)&(*
_tmp118).f1;_tmp11C=_tmp119.f2;_tmp11D=_tmp119.f3;_tmp11E=_tmp119.f4;_tmp11F=
_tmp119.f5;{struct _handler_cons _tmp120;_push_handler(& _tmp120);{int _tmp122=0;if(
setjmp(_tmp120.handler))_tmp122=1;if(!_tmp122){{void*_tmp123=((void*(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp11C,x);if(!_tmp11D(
y1,_tmp123,_tmp11F))*_tmp11B=0;};_pop_handler();}else{void*_tmp121=(void*)
_exn_thrown;void*_tmp125=_tmp121;_LL6D: {struct Cyc_Dict_Absent_struct*_tmp126=(
struct Cyc_Dict_Absent_struct*)_tmp125;if(_tmp126->tag != Cyc_Dict_Absent)goto
_LL6F;}_LL6E:{const char*_tmp2DF;void*_tmp2DE[2];struct Cyc_String_pa_struct
_tmp2DD;struct Cyc_String_pa_struct _tmp2DC;Cyc_Interface_check_err(_tmp11F,(
struct _dyneither_ptr)((_tmp2DC.tag=0,((_tmp2DC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*x),((_tmp2DD.tag=0,((_tmp2DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp11E),((_tmp2DE[0]=& _tmp2DD,((_tmp2DE[1]=& _tmp2DC,Cyc_aprintf(((
_tmp2DF="%s %s is missing",_tag_dyneither(_tmp2DF,sizeof(char),17))),
_tag_dyneither(_tmp2DE,sizeof(void*),2)))))))))))))));}*_tmp11B=0;goto _LL6C;
_LL6F:;_LL70:(void)_throw(_tmp125);_LL6C:;}};};}static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple20
_tmp2E0;struct _tuple20 _tmp12B=(_tmp2E0.f1=1,((_tmp2E0.f2=dic2,((_tmp2E0.f3=
incl_f,((_tmp2E0.f4=t,((_tmp2E0.f5=msg,_tmp2E0)))))))));((void(*)(void(*f)(
struct _tuple20*,struct _dyneither_ptr*,void*),struct _tuple20*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp12B,dic1);return _tmp12B.f1;}
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*
d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*
_tmp12D=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp12D == 0)return 0;if((struct
Cyc_Absyn_Aggrdecl*)d0 != _tmp12D){{const char*_tmp2E4;void*_tmp2E3[1];struct Cyc_String_pa_struct
_tmp2E2;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2E2.tag=0,((
_tmp2E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
d1->name)),((_tmp2E3[0]=& _tmp2E2,Cyc_aprintf(((_tmp2E4="declaration of type %s discloses too much information",
_tag_dyneither(_tmp2E4,sizeof(char),54))),_tag_dyneither(_tmp2E3,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct
_dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp131=Cyc_Tcdecl_merge_datatypedecl(
d0,d1,0,msg);if(_tmp131 == 0)return 0;if((struct Cyc_Absyn_Datatypedecl*)d0 != 
_tmp131){{const char*_tmp2E8;void*_tmp2E7[1];struct Cyc_String_pa_struct _tmp2E6;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2E6.tag=0,((_tmp2E6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((
_tmp2E7[0]=& _tmp2E6,Cyc_aprintf(((_tmp2E8="declaration of datatype %s discloses too much information",
_tag_dyneither(_tmp2E8,sizeof(char),58))),_tag_dyneither(_tmp2E7,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){
struct Cyc_Absyn_Enumdecl*_tmp135=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(
_tmp135 == 0)return 0;if((struct Cyc_Absyn_Enumdecl*)d0 != _tmp135){{const char*
_tmp2EC;void*_tmp2EB[1];struct Cyc_String_pa_struct _tmp2EA;Cyc_Interface_check_err(
msg,(struct _dyneither_ptr)((_tmp2EA.tag=0,((_tmp2EA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2EB[0]=& _tmp2EA,
Cyc_aprintf(((_tmp2EC="declaration of enum %s discloses too much information",
_tag_dyneither(_tmp2EC,sizeof(char),54))),_tag_dyneither(_tmp2EB,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){
struct Cyc_Absyn_Vardecl*_tmp139=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp139
== 0)return 0;if((struct Cyc_Absyn_Vardecl*)d0 != _tmp139){{const char*_tmp2F0;void*
_tmp2EF[1];struct Cyc_String_pa_struct _tmp2EE;Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)((_tmp2EE.tag=0,((_tmp2EE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2EF[0]=& _tmp2EE,Cyc_aprintf(((
_tmp2F0="declaration of variable %s discloses too much information",
_tag_dyneither(_tmp2F0,sizeof(char),58))),_tag_dyneither(_tmp2EF,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct
_dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp13D=Cyc_Tcdecl_merge_typedefdecl(
d0,d1,0,msg);if(_tmp13D == 0)return 0;if((struct Cyc_Absyn_Typedefdecl*)d0 != 
_tmp13D){{const char*_tmp2F4;void*_tmp2F3[1];struct Cyc_String_pa_struct _tmp2F2;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2F2.tag=0,((_tmp2F2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((
_tmp2F3[0]=& _tmp2F2,Cyc_aprintf(((_tmp2F4="declaration of typedef %s discloses too much information",
_tag_dyneither(_tmp2F4,sizeof(char),57))),_tag_dyneither(_tmp2F3,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp141=Cyc_Tcdecl_merge_xdatatypefielddecl(
d0,d1,0,msg);if(_tmp141 == 0)return 0;if((struct Cyc_Tcdecl_Xdatatypefielddecl*)d0
!= _tmp141){{const char*_tmp2F8;void*_tmp2F7[1];struct Cyc_String_pa_struct _tmp2F6;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2F6.tag=0,((_tmp2F6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name)),((
_tmp2F7[0]=& _tmp2F6,Cyc_aprintf(((_tmp2F8="declaration of xdatatypefield %s discloses too much information",
_tag_dyneither(_tmp2F8,sizeof(char),64))),_tag_dyneither(_tmp2F7,sizeof(void*),1)))))))));}
return 0;}return 1;}struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){const char*_tmp2F9;int _tmp145=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,((_tmp2F9="type",
_tag_dyneither(_tmp2F9,sizeof(char),5))),msg);const char*_tmp2FA;int _tmp146=((int(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,
struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,
Cyc_Interface_incl_datatypedecl,((_tmp2FA="datatype",_tag_dyneither(_tmp2FA,
sizeof(char),9))),msg);const char*_tmp2FB;int _tmp147=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,((_tmp2FB="enum",
_tag_dyneither(_tmp2FB,sizeof(char),5))),msg);const char*_tmp2FC;int _tmp148=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((
_tmp2FC="typedef",_tag_dyneither(_tmp2FC,sizeof(char),8))),msg)!= 0;const char*
_tmp2FD;int _tmp149=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*
incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,
ie2->vardecls,Cyc_Interface_incl_vardecl,((_tmp2FD="variable",_tag_dyneither(
_tmp2FD,sizeof(char),9))),msg);const char*_tmp2FE;int _tmp14A=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr
t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,((_tmp2FE="xdatatypefield",
_tag_dyneither(_tmp2FE,sizeof(char),15))),msg);return((((_tmp145  && _tmp146) && 
_tmp147) && _tmp148) && _tmp149) && _tmp14A;}struct _tuple21{int f1;struct Cyc_Dict_Dict
f2;struct _dyneither_ptr*f3;};static void Cyc_Interface_incl_ns_f(struct _tuple21*
env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1);static void Cyc_Interface_incl_ns_f(
struct _tuple21*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct
_tuple21 _tmp152;int _tmp153;int*_tmp154;struct Cyc_Dict_Dict _tmp155;struct
_dyneither_ptr*_tmp156;struct _tuple21*_tmp151=env;_tmp152=*_tmp151;_tmp153=
_tmp152.f1;_tmp154=(int*)&(*_tmp151).f1;_tmp155=_tmp152.f2;_tmp156=_tmp152.f3;{
struct Cyc_Interface_Ienv*ie2;{struct _handler_cons _tmp157;_push_handler(& _tmp157);{
int _tmp159=0;if(setjmp(_tmp157.handler))_tmp159=1;if(!_tmp159){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp155,ns);;
_pop_handler();}else{void*_tmp158=(void*)_exn_thrown;void*_tmp15B=_tmp158;_LL72: {
struct Cyc_Dict_Absent_struct*_tmp15C=(struct Cyc_Dict_Absent_struct*)_tmp15B;if(
_tmp15C->tag != Cyc_Dict_Absent)goto _LL74;}_LL73: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL71;_LL74:;_LL75:(void)_throw(_tmp15B);_LL71:;}};}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp156))*_tmp154=0;};}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct
_tuple21 _tmp2FF;struct _tuple21 _tmp15D=(_tmp2FF.f1=1,((_tmp2FF.f2=dic2,((_tmp2FF.f3=
msg,_tmp2FF)))));((void(*)(void(*f)(struct _tuple21*,struct Cyc_List_List*,struct
Cyc_Interface_Ienv*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _tmp15D,dic1);return _tmp15D.f1;}int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple9*info);int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple9*info){struct
_handler_cons _tmp15F;_push_handler(& _tmp15F);{int _tmp161=0;if(setjmp(_tmp15F.handler))
_tmp161=1;if(!_tmp161){{int _tmp162=1;struct _dyneither_ptr*msg=0;if(info != 0){
struct Cyc_String_pa_struct _tmp309;struct Cyc_String_pa_struct _tmp308;void*_tmp307[
2];const char*_tmp306;struct _dyneither_ptr*_tmp305;msg=((_tmp305=_cycalloc(
sizeof(*_tmp305)),((_tmp305[0]=(struct _dyneither_ptr)((_tmp309.tag=0,((_tmp309.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp308.tag=0,((
_tmp308.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp307[0]=&
_tmp308,((_tmp307[1]=& _tmp309,Cyc_aprintf(((_tmp306="checking inclusion of %s exports into %s exports,",
_tag_dyneither(_tmp306,sizeof(char),50))),_tag_dyneither(_tmp307,sizeof(void*),2)))))))))))))),
_tmp305))));}if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))_tmp162=0;
if(info != 0){struct Cyc_String_pa_struct _tmp313;struct Cyc_String_pa_struct _tmp312;
void*_tmp311[2];const char*_tmp310;struct _dyneither_ptr*_tmp30F;msg=((_tmp30F=
_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=(struct _dyneither_ptr)((_tmp313.tag=0,((
_tmp313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp312.tag=
0,((_tmp312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp311[0]=& _tmp312,((_tmp311[1]=& _tmp313,Cyc_aprintf(((_tmp310="checking inclusion of %s imports into %s imports,",
_tag_dyneither(_tmp310,sizeof(char),50))),_tag_dyneither(_tmp311,sizeof(void*),2)))))))))))))),
_tmp30F))));}if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))_tmp162=0;{
int _tmp16D=_tmp162;_npop_handler(0);return _tmp16D;};};_pop_handler();}else{void*
_tmp160=(void*)_exn_thrown;void*_tmp16F=_tmp160;_LL77: {struct Cyc_Tcdecl_Incompatible_struct*
_tmp170=(struct Cyc_Tcdecl_Incompatible_struct*)_tmp16F;if(_tmp170->tag != Cyc_Tcdecl_Incompatible)
goto _LL79;}_LL78: return 0;_LL79:;_LL7A:(void)_throw(_tmp16F);_LL76:;}};}struct
_tuple22{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
void*(*f5)(void*,void*,struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct
_dyneither_ptr f6;struct _dyneither_ptr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _dyneither_ptr*x,void*y2);void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _dyneither_ptr*x,void*y2){struct _tuple22 _tmp172;int
_tmp173;int*_tmp174;struct Cyc_Dict_Dict _tmp175;struct Cyc_Dict_Dict*_tmp176;
struct Cyc_Dict_Dict _tmp177;struct Cyc_Dict_Dict _tmp178;void*(*_tmp179)(void*,void*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp17A;
struct _dyneither_ptr*_tmp17B;struct _tuple22*_tmp171=env;_tmp172=*_tmp171;_tmp173=
_tmp172.f1;_tmp174=(int*)&(*_tmp171).f1;_tmp175=_tmp172.f2;_tmp176=(struct Cyc_Dict_Dict*)&(*
_tmp171).f2;_tmp177=_tmp172.f3;_tmp178=_tmp172.f4;_tmp179=_tmp172.f5;_tmp17A=
_tmp172.f6;_tmp17B=_tmp172.f7;{void*y;{struct _handler_cons _tmp17C;_push_handler(&
_tmp17C);{int _tmp17E=0;if(setjmp(_tmp17C.handler))_tmp17E=1;if(!_tmp17E){{void*
_tmp17F=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp177,x);void*_tmp180=_tmp179(_tmp17F,y2,0,_tmp17B);if(!((unsigned int)_tmp180)){*
_tmp174=0;_npop_handler(0);return;}y=(void*)_tmp180;};_pop_handler();}else{void*
_tmp17D=(void*)_exn_thrown;void*_tmp182=_tmp17D;_LL7C: {struct Cyc_Dict_Absent_struct*
_tmp183=(struct Cyc_Dict_Absent_struct*)_tmp182;if(_tmp183->tag != Cyc_Dict_Absent)
goto _LL7E;}_LL7D: y=y2;goto _LL7B;_LL7E:;_LL7F:(void)_throw(_tmp182);_LL7B:;}};}{
struct _handler_cons _tmp184;_push_handler(& _tmp184);{int _tmp186=0;if(setjmp(
_tmp184.handler))_tmp186=1;if(!_tmp186){{void*_tmp187=((void*(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp178,x);void*_tmp188=_tmp179(
_tmp187,y,0,_tmp17B);if(_tmp188 != (void*)_tmp187){if((unsigned int)_tmp188){
const char*_tmp318;void*_tmp317[2];struct Cyc_String_pa_struct _tmp316;struct Cyc_String_pa_struct
_tmp315;Cyc_Interface_check_err(_tmp17B,(struct _dyneither_ptr)((_tmp315.tag=0,((
_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp316.tag=0,((
_tmp316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp17A),((_tmp317[0]=&
_tmp316,((_tmp317[1]=& _tmp315,Cyc_aprintf(((_tmp318="abstract %s %s is being imported as non-abstract",
_tag_dyneither(_tmp318,sizeof(char),49))),_tag_dyneither(_tmp317,sizeof(void*),2)))))))))))))));}*
_tmp174=0;}};_pop_handler();}else{void*_tmp185=(void*)_exn_thrown;void*_tmp18E=
_tmp185;_LL81: {struct Cyc_Dict_Absent_struct*_tmp18F=(struct Cyc_Dict_Absent_struct*)
_tmp18E;if(_tmp18F->tag != Cyc_Dict_Absent)goto _LL83;}_LL82: if(*_tmp174)*_tmp176=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*
_tmp176,x,y);goto _LL80;_LL83:;_LL84:(void)_throw(_tmp18E);_LL80:;}};};};}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*
Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct
_tuple22 _tmp319;struct _tuple22 _tmp190=(_tmp319.f1=1,((_tmp319.f2=dic1,((_tmp319.f3=
dic1,((_tmp319.f4=excl,((_tmp319.f5=merge_f,((_tmp319.f6=t,((_tmp319.f7=msg,
_tmp319)))))))))))));((void(*)(void(*f)(struct _tuple22*,struct _dyneither_ptr*,
void*),struct _tuple22*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,&
_tmp190,dic2);if(_tmp190.f1){struct Cyc_Dict_Dict*_tmp31A;return(_tmp31A=
_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=_tmp190.f2,_tmp31A)));}return 0;}struct
Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*
msg);struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct
_dyneither_ptr*msg){const char*_tmp31B;struct Cyc_Dict_Dict*_tmp193=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,((_tmp31B="type",_tag_dyneither(
_tmp31B,sizeof(char),5))),msg);const char*_tmp31C;struct Cyc_Dict_Dict*_tmp194=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,((
_tmp31C="datatype",_tag_dyneither(_tmp31C,sizeof(char),9))),msg);const char*
_tmp31D;struct Cyc_Dict_Dict*_tmp195=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*
merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,((
_tmp31D="enum",_tag_dyneither(_tmp31D,sizeof(char),5))),msg);const char*_tmp31E;
struct Cyc_Dict_Dict*_tmp196=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,((
_tmp31E="typedef",_tag_dyneither(_tmp31E,sizeof(char),8))),msg);const char*
_tmp31F;struct Cyc_Dict_Dict*_tmp197=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*
merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,((_tmp31F="variable",
_tag_dyneither(_tmp31F,sizeof(char),9))),msg);const char*_tmp320;struct Cyc_Dict_Dict*
_tmp198=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct
Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,((
_tmp320="xdatatypefield",_tag_dyneither(_tmp320,sizeof(char),15))),msg);if(((((!((
unsigned int)_tmp193) || !((unsigned int)_tmp194)) || !((unsigned int)_tmp195))
 || !((unsigned int)_tmp196)) || !((unsigned int)_tmp197)) || !((unsigned int)
_tmp198))return 0;{struct Cyc_Interface_Ienv*_tmp321;return(_tmp321=_cycalloc(
sizeof(*_tmp321)),((_tmp321->aggrdecls=*_tmp193,((_tmp321->datatypedecls=*
_tmp194,((_tmp321->enumdecls=*_tmp195,((_tmp321->typedefdecls=*_tmp196,((_tmp321->vardecls=*
_tmp197,((_tmp321->xdatatypefielddecls=*_tmp198,_tmp321)))))))))))));};}struct
_tuple23{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
struct _dyneither_ptr*f5;};void Cyc_Interface_compat_merge_ns_f(struct _tuple23*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2);void Cyc_Interface_compat_merge_ns_f(
struct _tuple23*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple23 _tmp1A1;int _tmp1A2;int*_tmp1A3;struct Cyc_Dict_Dict _tmp1A4;struct Cyc_Dict_Dict*
_tmp1A5;struct Cyc_Dict_Dict _tmp1A6;struct Cyc_Dict_Dict _tmp1A7;struct
_dyneither_ptr*_tmp1A8;struct _tuple23*_tmp1A0=env;_tmp1A1=*_tmp1A0;_tmp1A2=
_tmp1A1.f1;_tmp1A3=(int*)&(*_tmp1A0).f1;_tmp1A4=_tmp1A1.f2;_tmp1A5=(struct Cyc_Dict_Dict*)&(*
_tmp1A0).f2;_tmp1A6=_tmp1A1.f3;_tmp1A7=_tmp1A1.f4;_tmp1A8=_tmp1A1.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp1A9;_push_handler(&
_tmp1A9);{int _tmp1AB=0;if(setjmp(_tmp1A9.handler))_tmp1AB=1;if(!_tmp1AB){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp1A7,ns);;_pop_handler();}else{void*_tmp1AA=(void*)_exn_thrown;void*_tmp1AD=
_tmp1AA;_LL86: {struct Cyc_Dict_Absent_struct*_tmp1AE=(struct Cyc_Dict_Absent_struct*)
_tmp1AD;if(_tmp1AE->tag != Cyc_Dict_Absent)goto _LL88;}_LL87: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LL85;_LL88:;_LL89:(void)_throw(_tmp1AD);_LL85:;}};}{struct _handler_cons
_tmp1AF;_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(_tmp1AF.handler))_tmp1B1=
1;if(!_tmp1B1){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp1A6,ns);{struct Cyc_Interface_Ienv*_tmp1B2=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp1A8);if(_tmp1B2 == 0){*_tmp1A3=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_tmp1B2;};;_pop_handler();}
else{void*_tmp1B0=(void*)_exn_thrown;void*_tmp1B4=_tmp1B0;_LL8B: {struct Cyc_Dict_Absent_struct*
_tmp1B5=(struct Cyc_Dict_Absent_struct*)_tmp1B4;if(_tmp1B5->tag != Cyc_Dict_Absent)
goto _LL8D;}_LL8C: ie1=Cyc_Interface_lazy_new_ienv();goto _LL8A;_LL8D:;_LL8E:(void)
_throw(_tmp1B4);_LL8A:;}};}{struct Cyc_Interface_Ienv*_tmp1B6=Cyc_Interface_compat_merge_ienv(
ie1,ie2,iexcl,_tmp1A8);if(_tmp1B6 == 0)*_tmp1A3=0;else{if(*_tmp1A3)*_tmp1A5=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp1A5,ns,(struct Cyc_Interface_Ienv*)_tmp1B6);}};};}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
_dyneither_ptr*msg){struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple23 _tmp322;struct _tuple23 _tmp1B7=(_tmp322.f1=1,((_tmp322.f2=d,((
_tmp322.f3=dic1,((_tmp322.f4=excl,((_tmp322.f5=msg,_tmp322)))))))));((void(*)(
void(*f)(struct _tuple23*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
_tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp1B7,dic2);if(_tmp1B7.f1){struct Cyc_Dict_Dict*_tmp323;return(_tmp323=
_cycalloc(sizeof(*_tmp323)),((_tmp323[0]=_tmp1B7.f2,_tmp323)));}return 0;}struct
_tuple24{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr
f4;struct _dyneither_ptr*f5;};void Cyc_Interface_disj_merge_dict_f(struct _tuple24*
env,struct _dyneither_ptr*x,void*y);void Cyc_Interface_disj_merge_dict_f(struct
_tuple24*env,struct _dyneither_ptr*x,void*y){struct _tuple24 _tmp1BB;int _tmp1BC;int*
_tmp1BD;struct Cyc_Dict_Dict _tmp1BE;struct Cyc_Dict_Dict*_tmp1BF;struct Cyc_Dict_Dict
_tmp1C0;struct _dyneither_ptr _tmp1C1;struct _dyneither_ptr*_tmp1C2;struct _tuple24*
_tmp1BA=env;_tmp1BB=*_tmp1BA;_tmp1BC=_tmp1BB.f1;_tmp1BD=(int*)&(*_tmp1BA).f1;
_tmp1BE=_tmp1BB.f2;_tmp1BF=(struct Cyc_Dict_Dict*)&(*_tmp1BA).f2;_tmp1C0=_tmp1BB.f3;
_tmp1C1=_tmp1BB.f4;_tmp1C2=_tmp1BB.f5;if(((int(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_member)(_tmp1C0,x)){{const char*_tmp328;void*_tmp327[2];
struct Cyc_String_pa_struct _tmp326;struct Cyc_String_pa_struct _tmp325;Cyc_Interface_check_err(
_tmp1C2,(struct _dyneither_ptr)((_tmp325.tag=0,((_tmp325.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*x),((_tmp326.tag=0,((_tmp326.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1C1),((_tmp327[0]=& _tmp326,((_tmp327[1]=& _tmp325,Cyc_aprintf(((
_tmp328="%s %s is exported more than once",_tag_dyneither(_tmp328,sizeof(char),
33))),_tag_dyneither(_tmp327,sizeof(void*),2)))))))))))))));}*_tmp1BD=0;}else{
if(*_tmp1BD)*_tmp1BF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp1BF,x,y);}}struct Cyc_Dict_Dict*
Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct _dyneither_ptr t,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg){struct _tuple24 _tmp329;struct _tuple24 _tmp1C7=(_tmp329.f1=1,((
_tmp329.f2=dic1,((_tmp329.f3=dic1,((_tmp329.f4=t,((_tmp329.f5=msg,_tmp329)))))))));((
void(*)(void(*f)(struct _tuple24*,struct _dyneither_ptr*,void*),struct _tuple24*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp1C7,
dic2);if(_tmp1C7.f1){struct Cyc_Dict_Dict*_tmp32A;return(_tmp32A=_cycalloc(
sizeof(*_tmp32A)),((_tmp32A[0]=_tmp1C7.f2,_tmp32A)));}return 0;}struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*
ie2,struct _dyneither_ptr*msg);struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){const char*_tmp32B;struct Cyc_Dict_Dict*_tmp1CA=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,((_tmp32B="type",
_tag_dyneither(_tmp32B,sizeof(char),5))),msg);const char*_tmp32C;struct Cyc_Dict_Dict*
_tmp1CB=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,((_tmp32C="datatype",_tag_dyneither(
_tmp32C,sizeof(char),9))),msg);const char*_tmp32D;struct Cyc_Dict_Dict*_tmp1CC=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,
ie2->enumdecls,((_tmp32D="enum",_tag_dyneither(_tmp32D,sizeof(char),5))),msg);
const char*_tmp32E;struct Cyc_Dict_Dict*_tmp1CD=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((
_tmp32E="typedef",_tag_dyneither(_tmp32E,sizeof(char),8))),msg);const char*
_tmp32F;struct Cyc_Dict_Dict*_tmp1CE=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->vardecls,ie2->vardecls,((_tmp32F="variable",_tag_dyneither(_tmp32F,sizeof(
char),9))),msg);const char*_tmp330;struct Cyc_Dict_Dict*_tmp1CF=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,((
_tmp330="xdatatypefield",_tag_dyneither(_tmp330,sizeof(char),15))),msg);if(((((!((
unsigned int)_tmp1CA) || !((unsigned int)_tmp1CB)) || !((unsigned int)_tmp1CC))
 || !((unsigned int)_tmp1CD)) || !((unsigned int)_tmp1CE)) || !((unsigned int)
_tmp1CF))return 0;{struct Cyc_Interface_Ienv*_tmp331;return(_tmp331=_cycalloc(
sizeof(*_tmp331)),((_tmp331->aggrdecls=*_tmp1CA,((_tmp331->datatypedecls=*
_tmp1CB,((_tmp331->enumdecls=*_tmp1CC,((_tmp331->typedefdecls=*_tmp1CD,((_tmp331->vardecls=*
_tmp1CE,((_tmp331->xdatatypefielddecls=*_tmp1CF,_tmp331)))))))))))));};}struct
_tuple25{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*
f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple25*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie2);void Cyc_Interface_disj_merge_ns_f(struct _tuple25*
env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct _tuple25 _tmp1D8;
int _tmp1D9;int*_tmp1DA;struct Cyc_Dict_Dict _tmp1DB;struct Cyc_Dict_Dict*_tmp1DC;
struct Cyc_Dict_Dict _tmp1DD;struct _dyneither_ptr*_tmp1DE;struct _tuple25*_tmp1D7=
env;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.f1;_tmp1DA=(int*)&(*_tmp1D7).f1;_tmp1DB=
_tmp1D8.f2;_tmp1DC=(struct Cyc_Dict_Dict*)&(*_tmp1D7).f2;_tmp1DD=_tmp1D8.f3;
_tmp1DE=_tmp1D8.f4;{struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1DF;
_push_handler(& _tmp1DF);{int _tmp1E1=0;if(setjmp(_tmp1DF.handler))_tmp1E1=1;if(!
_tmp1E1){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp1DD,ns);;_pop_handler();}else{void*_tmp1E0=(void*)
_exn_thrown;void*_tmp1E3=_tmp1E0;_LL90: {struct Cyc_Dict_Absent_struct*_tmp1E4=(
struct Cyc_Dict_Absent_struct*)_tmp1E3;if(_tmp1E4->tag != Cyc_Dict_Absent)goto
_LL92;}_LL91: ie1=Cyc_Interface_lazy_new_ienv();goto _LL8F;_LL92:;_LL93:(void)
_throw(_tmp1E3);_LL8F:;}};}{struct Cyc_Interface_Ienv*_tmp1E5=Cyc_Interface_disj_merge_ienv(
ie1,ie2,_tmp1DE);if(_tmp1E5 == 0)*_tmp1DA=0;else{if(*_tmp1DA)*_tmp1DC=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
_tmp1DC,ns,(struct Cyc_Interface_Ienv*)_tmp1E5);}};};}struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg);struct
Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct _dyneither_ptr*msg){struct _tuple25 _tmp332;struct _tuple25 _tmp1E6=(
_tmp332.f1=1,((_tmp332.f2=dic1,((_tmp332.f3=dic1,((_tmp332.f4=msg,_tmp332)))))));((
void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,&
_tmp1E6,dic2);if(_tmp1E6.f1){struct Cyc_Dict_Dict*_tmp333;return(_tmp333=
_cycalloc(sizeof(*_tmp333)),((_tmp333[0]=_tmp1E6.f2,_tmp333)));}return 0;}struct
Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple9*info);struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple9*info){struct _handler_cons _tmp1E9;
_push_handler(& _tmp1E9);{int _tmp1EB=0;if(setjmp(_tmp1E9.handler))_tmp1EB=1;if(!
_tmp1EB){{struct _dyneither_ptr*msg=0;if(info != 0){struct Cyc_String_pa_struct
_tmp33D;struct Cyc_String_pa_struct _tmp33C;void*_tmp33B[2];const char*_tmp33A;
struct _dyneither_ptr*_tmp339;msg=((_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339[
0]=(struct _dyneither_ptr)((_tmp33D.tag=0,((_tmp33D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(*info).f2),((_tmp33C.tag=0,((_tmp33C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp33B[0]=& _tmp33C,((
_tmp33B[1]=& _tmp33D,Cyc_aprintf(((_tmp33A="merging exports of %s and %s,",
_tag_dyneither(_tmp33A,sizeof(char),30))),_tag_dyneither(_tmp33B,sizeof(void*),2)))))))))))))),
_tmp339))));}{struct Cyc_Dict_Dict*_tmp1F1=Cyc_Interface_disj_merge_ns(i1->exports,
i2->exports,msg);if(_tmp1F1 == 0){struct Cyc_Interface_I*_tmp1F2=0;_npop_handler(0);
return _tmp1F2;}if(info != 0){struct Cyc_String_pa_struct _tmp347;struct Cyc_String_pa_struct
_tmp346;void*_tmp345[2];const char*_tmp344;struct _dyneither_ptr*_tmp343;msg=((
_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343[0]=(struct _dyneither_ptr)((_tmp347.tag=
0,((_tmp347.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp346.tag=0,((_tmp346.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((
_tmp345[0]=& _tmp346,((_tmp345[1]=& _tmp347,Cyc_aprintf(((_tmp344="merging imports of %s and %s,",
_tag_dyneither(_tmp344,sizeof(char),30))),_tag_dyneither(_tmp345,sizeof(void*),2)))))))))))))),
_tmp343))));}{struct Cyc_Dict_Dict*_tmp1F8=Cyc_Interface_compat_merge_ns(i1->imports,
i2->imports,*_tmp1F1,msg);if(_tmp1F8 == 0){struct Cyc_Interface_I*_tmp1F9=0;
_npop_handler(0);return _tmp1F9;}{struct Cyc_Interface_I*_tmp348;struct Cyc_Interface_I*
_tmp1FB=(_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348->imports=*_tmp1F8,((
_tmp348->exports=*_tmp1F1,_tmp348)))));_npop_handler(0);return _tmp1FB;};};};};
_pop_handler();}else{void*_tmp1EA=(void*)_exn_thrown;void*_tmp1FD=_tmp1EA;_LL95: {
struct Cyc_Tcdecl_Incompatible_struct*_tmp1FE=(struct Cyc_Tcdecl_Incompatible_struct*)
_tmp1FD;if(_tmp1FE->tag != Cyc_Tcdecl_Incompatible)goto _LL97;}_LL96: return 0;_LL97:;
_LL98:(void)_throw(_tmp1FD);_LL94:;}};}struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li == 0)return(struct Cyc_Interface_I*)
Cyc_Interface_empty();{struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)((struct
_dyneither_ptr*)linfo->hd): 0;li=li->tl;if(linfo != 0)linfo=linfo->tl;for(0;li != 0;
li=li->tl){struct _tuple9*_tmp349;struct Cyc_Interface_I*_tmp1FF=Cyc_Interface_merge(
curr_i,(struct Cyc_Interface_I*)li->hd,curr_info != 0  && linfo != 0?(_tmp349=
_cycalloc(sizeof(*_tmp349)),((_tmp349->f1=*curr_info,((_tmp349->f2=*((struct
_dyneither_ptr*)linfo->hd),_tmp349))))): 0);if(_tmp1FF == 0)return 0;curr_i=(struct
Cyc_Interface_I*)_tmp1FF;if(curr_info != 0){struct Cyc_String_pa_struct _tmp353;
struct Cyc_String_pa_struct _tmp352;void*_tmp351[2];const char*_tmp350;struct
_dyneither_ptr*_tmp34F;curr_info=linfo != 0?(_tmp34F=_cycalloc(sizeof(*_tmp34F)),((
_tmp34F[0]=(struct _dyneither_ptr)((_tmp353.tag=0,((_tmp353.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((
_tmp352.tag=0,((_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
curr_info),((_tmp351[0]=& _tmp352,((_tmp351[1]=& _tmp353,Cyc_aprintf(((_tmp350="%s+%s",
_tag_dyneither(_tmp350,sizeof(char),6))),_tag_dyneither(_tmp351,sizeof(void*),2)))))))))))))),
_tmp34F))): 0;}if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;};}
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*
get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo){if(la == 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{
struct Cyc_Interface_I*curr_i=get((void*)la->hd);struct _dyneither_ptr*curr_info=
linfo != 0?(struct _dyneither_ptr*)((struct _dyneither_ptr*)linfo->hd): 0;la=la->tl;
if(linfo != 0)linfo=linfo->tl;for(0;la != 0;la=la->tl){struct _tuple9*_tmp354;
struct Cyc_Interface_I*_tmp206=Cyc_Interface_merge(curr_i,get((void*)la->hd),
curr_info != 0  && linfo != 0?(_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354->f1=*
curr_info,((_tmp354->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp354))))): 0);if(
_tmp206 == 0)return 0;curr_i=(struct Cyc_Interface_I*)_tmp206;if(curr_info != 0){
struct Cyc_String_pa_struct _tmp35E;struct Cyc_String_pa_struct _tmp35D;void*_tmp35C[
2];const char*_tmp35B;struct _dyneither_ptr*_tmp35A;curr_info=linfo != 0?(_tmp35A=
_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=(struct _dyneither_ptr)((_tmp35E.tag=0,((
_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
linfo->hd)),((_tmp35D.tag=0,((_tmp35D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*curr_info),((_tmp35C[0]=& _tmp35D,((_tmp35C[1]=& _tmp35E,Cyc_aprintf(((
_tmp35B="%s+%s",_tag_dyneither(_tmp35B,sizeof(char),6))),_tag_dyneither(_tmp35C,
sizeof(void*),2)))))))))))))),_tmp35A))): 0;}if(linfo != 0)linfo=linfo->tl;}return(
struct Cyc_Interface_I*)curr_i;};}static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns);static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns){if(ns == 0)return tds;{struct Cyc_Absyn_Namespace_d_struct*
_tmp364;struct Cyc_Absyn_Namespace_d_struct _tmp363;struct Cyc_List_List*_tmp362;
return(_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362->hd=Cyc_Absyn_new_decl((
void*)((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364[0]=((_tmp363.tag=10,((
_tmp363.f1=(struct _dyneither_ptr*)ns->hd,((_tmp363.f2=Cyc_Interface_add_namespace(
tds,ns->tl),_tmp363)))))),_tmp364)))),0),((_tmp362->tl=0,_tmp362)))));};}static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*
d,struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Aggr_d_struct*_tmp36A;struct Cyc_Absyn_Aggr_d_struct _tmp369;
struct Cyc_List_List*_tmp368;return(_tmp368=_cycalloc(sizeof(*_tmp368)),((_tmp368->hd=
Cyc_Absyn_new_decl((void*)((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=((
_tmp369.tag=6,((_tmp369.f1=d,_tmp369)))),_tmp36A)))),0),((_tmp368->tl=tds,
_tmp368)))));}static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Aggrdecl*
_tmp36B;d=((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B[0]=*d,_tmp36B))));}d->impl=
0;if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Aggr_d_struct*
_tmp371;struct Cyc_Absyn_Aggr_d_struct _tmp370;struct Cyc_List_List*_tmp36F;return(
_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->hd=Cyc_Absyn_new_decl((void*)((
_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371[0]=((_tmp370.tag=6,((_tmp370.f1=d,
_tmp370)))),_tmp371)))),0),((_tmp36F->tl=tds,_tmp36F)))));};}static struct Cyc_List_List*
Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*
d,struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Datatype_d_struct*_tmp377;struct Cyc_Absyn_Datatype_d_struct
_tmp376;struct Cyc_List_List*_tmp375;return(_tmp375=_cycalloc(sizeof(*_tmp375)),((
_tmp375->hd=Cyc_Absyn_new_decl((void*)((_tmp377=_cycalloc(sizeof(*_tmp377)),((
_tmp377[0]=((_tmp376.tag=7,((_tmp376.f1=d,_tmp376)))),_tmp377)))),0),((_tmp375->tl=
tds,_tmp375)))));}static char _tmp21A[2]="_";static struct _dyneither_ptr Cyc_Interface_us={
_tmp21A,_tmp21A,_tmp21A + 2};static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
static struct _tuple16*Cyc_Interface_rewrite_datatypefield_type(struct _tuple16*x);
static struct _tuple16*Cyc_Interface_rewrite_datatypefield_type(struct _tuple16*x){
struct Cyc_Absyn_VarType_struct*_tmp381;struct Cyc_Absyn_Tvar*_tmp380;struct Cyc_Absyn_VarType_struct
_tmp37F;struct _tuple16*_tmp37E;return(_tmp37E=_cycalloc(sizeof(*_tmp37E)),((
_tmp37E->f1=Cyc_Absyn_empty_tqual(0),((_tmp37E->f2=(void*)((_tmp381=_cycalloc(
sizeof(*_tmp381)),((_tmp381[0]=((_tmp37F.tag=2,((_tmp37F.f1=((_tmp380=_cycalloc(
sizeof(*_tmp380)),((_tmp380->name=Cyc_Interface_us_p,((_tmp380->identity=- 1,((
_tmp380->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp380)))))))),_tmp37F)))),
_tmp381)))),_tmp37E)))));}static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(
struct Cyc_Absyn_Datatypefield*f);static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(
struct Cyc_Absyn_Datatypefield*f){{struct Cyc_Absyn_Datatypefield*_tmp382;f=((
_tmp382=_cycalloc(sizeof(*_tmp382)),((_tmp382[0]=*f,_tmp382))));}f->typs=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);return f;}
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Datatypedecl*
_tmp383;d=((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=*d,_tmp383))));}if(
d->fields != 0){struct Cyc_Core_Opt*_tmp384;d->fields=((_tmp384=_cycalloc(sizeof(*
_tmp384)),((_tmp384->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(
struct Cyc_List_List*)(d->fields)->v),_tmp384))));}if(d->sc != Cyc_Absyn_ExternC)d->sc=
Cyc_Absyn_Extern;{struct Cyc_Absyn_Datatype_d_struct*_tmp38A;struct Cyc_Absyn_Datatype_d_struct
_tmp389;struct Cyc_List_List*_tmp388;return(_tmp388=_cycalloc(sizeof(*_tmp388)),((
_tmp388->hd=Cyc_Absyn_new_decl((void*)((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((
_tmp38A[0]=((_tmp389.tag=7,((_tmp389.f1=d,_tmp389)))),_tmp38A)))),0),((_tmp388->tl=
tds,_tmp388)))));};}static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct
_dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Enum_d_struct*_tmp390;struct Cyc_Absyn_Enum_d_struct
_tmp38F;struct Cyc_List_List*_tmp38E;return(_tmp38E=_cycalloc(sizeof(*_tmp38E)),((
_tmp38E->hd=Cyc_Absyn_new_decl((void*)((_tmp390=_cycalloc(sizeof(*_tmp390)),((
_tmp390[0]=((_tmp38F.tag=8,((_tmp38F.f1=d,_tmp38F)))),_tmp390)))),0),((_tmp38E->tl=
tds,_tmp38E)))));}static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(
struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds);
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _dyneither_ptr*
x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Enumdecl*
_tmp391;d=((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=*d,_tmp391))));}d->fields=
0;if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Enum_d_struct*
_tmp397;struct Cyc_Absyn_Enum_d_struct _tmp396;struct Cyc_List_List*_tmp395;return(
_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395->hd=Cyc_Absyn_new_decl((void*)((
_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397[0]=((_tmp396.tag=8,((_tmp396.f1=d,
_tmp396)))),_tmp397)))),0),((_tmp395->tl=tds,_tmp395)))));};}static struct Cyc_List_List*
Cyc_Interface_add_typedef(struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,
struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_typedef(
struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Typedef_d_struct*_tmp39D;struct Cyc_Absyn_Typedef_d_struct _tmp39C;
struct Cyc_List_List*_tmp39B;return(_tmp39B=_cycalloc(sizeof(*_tmp39B)),((_tmp39B->hd=
Cyc_Absyn_new_decl((void*)((_tmp39D=_cycalloc(sizeof(*_tmp39D)),((_tmp39D[0]=((
_tmp39C.tag=9,((_tmp39C.f1=d,_tmp39C)))),_tmp39D)))),0),((_tmp39B->tl=tds,
_tmp39B)))));}static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct
_dyneither_ptr*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds);static struct
Cyc_List_List*Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Var_d_struct*_tmp3A3;struct Cyc_Absyn_Var_d_struct
_tmp3A2;struct Cyc_List_List*_tmp3A1;return(_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((
_tmp3A1->hd=Cyc_Absyn_new_decl((void*)((_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((
_tmp3A3[0]=((_tmp3A2.tag=0,((_tmp3A2.f1=d,_tmp3A2)))),_tmp3A3)))),0),((_tmp3A1->tl=
tds,_tmp3A1)))));}static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds);static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct
_dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xdatatypefielddecl _tmp233;struct Cyc_Absyn_Datatypedecl*_tmp234;
struct Cyc_Absyn_Datatypefield*_tmp235;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp232=d;_tmp233=*_tmp232;_tmp234=_tmp233.base;_tmp235=_tmp233.field;{struct Cyc_Absyn_Datatypedecl*
_tmp3A4;_tmp234=((_tmp3A4=_cycalloc(sizeof(*_tmp3A4)),((_tmp3A4[0]=*_tmp234,
_tmp3A4))));}{struct Cyc_List_List*_tmp3A7;struct Cyc_Core_Opt*_tmp3A6;_tmp234->fields=((
_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->v=((_tmp3A7=_cycalloc(sizeof(*
_tmp3A7)),((_tmp3A7->hd=_tmp235,((_tmp3A7->tl=0,_tmp3A7)))))),_tmp3A6))));}
_tmp234->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Datatype_d_struct*_tmp3AD;struct
Cyc_Absyn_Datatype_d_struct _tmp3AC;struct Cyc_List_List*_tmp3AB;return(_tmp3AB=
_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB->hd=Cyc_Absyn_new_decl((void*)((_tmp3AD=
_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD[0]=((_tmp3AC.tag=7,((_tmp3AC.f1=_tmp234,
_tmp3AC)))),_tmp3AD)))),0),((_tmp3AB->tl=tds,_tmp3AB)))));};}static struct Cyc_List_List*
Cyc_Interface_add_xdatatypefielddecl_header(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*
d,struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp23D;struct Cyc_Absyn_Datatypedecl*
_tmp23E;struct Cyc_Absyn_Datatypefield*_tmp23F;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp23C=d;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.base;_tmp23F=_tmp23D.field;{struct Cyc_Absyn_Datatypedecl*
_tmp3AE;_tmp23E=((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE[0]=*_tmp23E,
_tmp3AE))));}_tmp23F=Cyc_Interface_rewrite_datatypefield(_tmp23F);_tmp23F->sc=
Cyc_Absyn_Extern;{struct Cyc_List_List*_tmp3B1;struct Cyc_Core_Opt*_tmp3B0;_tmp23E->fields=((
_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->v=((_tmp3B1=_cycalloc(sizeof(*
_tmp3B1)),((_tmp3B1->hd=_tmp23F,((_tmp3B1->tl=0,_tmp3B1)))))),_tmp3B0))));}
_tmp23E->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Datatype_d_struct*_tmp3B7;struct
Cyc_Absyn_Datatype_d_struct _tmp3B6;struct Cyc_List_List*_tmp3B5;return(_tmp3B5=
_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5->hd=Cyc_Absyn_new_decl((void*)((_tmp3B7=
_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3B6.tag=7,((_tmp3B6.f1=_tmp23E,
_tmp3B6)))),_tmp3B7)))),0),((_tmp3B5->tl=tds,_tmp3B5)))));};}static void Cyc_Interface_print_ns_headers(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static
void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp246=0;_tmp246=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp246);_tmp246=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp246);_tmp246=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp246);if(_tmp246 != 0){_tmp246=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp246);
_tmp246=Cyc_Interface_add_namespace(_tmp246,ns);Cyc_Absynpp_decllist2file(
_tmp246,f);}}static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct
Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void
Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp247=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);if(
_tmp247 != 0){_tmp247=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp247);_tmp247=Cyc_Interface_add_namespace(_tmp247,ns);Cyc_Absynpp_decllist2file(
_tmp247,f);}}static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void Cyc_Interface_print_ns_typedefs(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp248=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp248 != 0){_tmp248=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp248);_tmp248=Cyc_Interface_add_namespace(_tmp248,ns);Cyc_Absynpp_decllist2file(
_tmp248,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void Cyc_Interface_print_ns_decls(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp249=0;_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,
ie->aggrdecls,_tmp249);_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,
ie->datatypedecls,_tmp249);_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,
ie->enumdecls,_tmp249);_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp249);_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,
ie->xdatatypefielddecls,_tmp249);if(_tmp249 != 0){_tmp249=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp249);_tmp249=Cyc_Interface_add_namespace(
_tmp249,ns);Cyc_Absynpp_decllist2file(_tmp249,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc___cycFILE*f);void Cyc_Interface_print(struct Cyc_Interface_I*
i,struct Cyc___cycFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);{
const char*_tmp3BA;void*_tmp3B9;(_tmp3B9=0,Cyc_fprintf(f,((_tmp3BA="/****** needed (headers) ******/\n",
_tag_dyneither(_tmp3BA,sizeof(char),34))),_tag_dyneither(_tmp3B9,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);{const char*_tmp3BD;void*_tmp3BC;(_tmp3BC=0,Cyc_fprintf(f,((_tmp3BD="\n/****** provided (headers) ******/\n",
_tag_dyneither(_tmp3BD,sizeof(char),37))),_tag_dyneither(_tmp3BC,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);{const char*_tmp3C0;void*_tmp3BF;(_tmp3BF=0,Cyc_fprintf(f,((_tmp3C0="\n/****** needed (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3C0,sizeof(char),58))),_tag_dyneither(_tmp3BF,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->imports);{const char*_tmp3C3;void*_tmp3C2;(_tmp3C2=0,Cyc_fprintf(f,((_tmp3C3="\n/****** provided (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3C3,sizeof(char),60))),_tag_dyneither(_tmp3C2,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->exports);{const char*_tmp3C6;void*_tmp3C5;(_tmp3C5=0,Cyc_fprintf(f,((_tmp3C6="\n/****** provided (typedefs) ******/\n",
_tag_dyneither(_tmp3C6,sizeof(char),38))),_tag_dyneither(_tmp3C5,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);{const char*_tmp3C9;void*_tmp3C8;(_tmp3C8=0,Cyc_fprintf(f,((_tmp3C9="\n/****** needed (declarations) ******/\n",
_tag_dyneither(_tmp3C9,sizeof(char),40))),_tag_dyneither(_tmp3C8,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);{const char*_tmp3CC;void*_tmp3CB;(_tmp3CB=0,Cyc_fprintf(f,((_tmp3CC="\n/****** provided (declarations) ******/\n",
_tag_dyneither(_tmp3CC,sizeof(char),42))),_tag_dyneither(_tmp3CB,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp258=Cyc_Parse_parse_file(f);struct _RegionHandle _tmp259=
_new_region("r");struct _RegionHandle*r=& _tmp259;_push_region(r);{struct Cyc_Tcenv_Tenv*
_tmp25A=Cyc_Tcenv_tc_init(r);Cyc_Tc_tc(r,_tmp25A,0,_tmp258);{struct Cyc_Interface_I*
_tmp25B=Cyc_Interface_gen_extract(_tmp25A->ae,0);_npop_handler(0);return _tmp25B;};};
_pop_region(r);};}void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*
f);void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){Cyc_Interface_print(
i,f);}struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f);struct Cyc_Interface_I*
Cyc_Interface_load(struct Cyc___cycFILE*f){return Cyc_Interface_parse(f);}
