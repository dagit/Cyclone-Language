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
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
void*Cyc_Core_snd(struct _tuple0*);int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*
hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*
val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_Position_error_p();struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
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
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{unsigned int f1;int f2;};struct
_tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[
7];struct Cyc_CfFlowInfo_IsZero_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[
8];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
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
set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12
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
f);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,
void*t,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{
char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct
_tmp6C9;struct Cyc_NewControlFlow_CFStmtAnnot _tmp6C8;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp6C7;enclosee->annot=(void*)((_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7[0]=((
_tmp6C9.tag=Cyc_NewControlFlow_CFAnnot,((_tmp6C9.f1=((_tmp6C8.encloser=encloser,((
_tmp6C8.visited=0,_tmp6C8)))),_tmp6C9)))),_tmp6C7))));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_AbsLVal f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,
struct Cyc_Absyn_Exp*);static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple15{
union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct
_tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)
goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)&
_tmp4->f1;}}_LL2: return _tmp6;_LL3:;_LL4: {struct Cyc_Core_Impossible_struct
_tmp6CF;const char*_tmp6CE;struct Cyc_Core_Impossible_struct*_tmp6CD;(int)_throw((
void*)((_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CF.tag=Cyc_Core_Impossible,((
_tmp6CF.f1=((_tmp6CE="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp6CE,sizeof(char),37))),_tmp6CF)))),_tmp6CD)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp6D0;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp6D0=_region_malloc(env->r,sizeof(*_tmp6D0)),((_tmp6D0[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp6D0)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp6D1;return(_tmp6D1.f1=_tmpB,((_tmp6D1.f2=
_tmpC,_tmp6D1)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow);static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow);}static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpE=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpF=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp10=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmpF);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp10,*_tmpF)){*_tmpF=
_tmp10;if(_tmpE->visited == env->iteration_num)env->iterate_again=1;}}static union
Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp11=inflow;int _tmp12;struct _tuple12 _tmp13;struct Cyc_Dict_Dict _tmp14;struct Cyc_List_List*
_tmp15;_LL6: if((_tmp11.BottomFL).tag != 1)goto _LL8;_tmp12=(int)(_tmp11.BottomFL).val;
_LL7: return Cyc_CfFlowInfo_BottomFL();_LL8: if((_tmp11.ReachableFL).tag != 2)goto
_LL5;_tmp13=(struct _tuple12)(_tmp11.ReachableFL).val;_tmp14=_tmp13.f1;_tmp15=
_tmp13.f2;_LL9: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp6D4;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp6D3;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp16=(_tmp6D3=_region_malloc(fenv->r,sizeof(*_tmp6D3)),((_tmp6D3[0]=((_tmp6D4.tag=
0,((_tmp6D4.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp6D4)))),_tmp6D3)));_tmp14=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp14,(void*)_tmp16,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp14,_tmp15);_LL5:;}
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp19=inflow;int _tmp1A;struct _tuple12 _tmp1B;struct
Cyc_Dict_Dict _tmp1C;struct Cyc_List_List*_tmp1D;_LLB: if((_tmp19.BottomFL).tag != 1)
goto _LLD;_tmp1A=(int)(_tmp19.BottomFL).val;_LLC: return Cyc_CfFlowInfo_BottomFL();
_LLD: if((_tmp19.ReachableFL).tag != 2)goto _LLA;_tmp1B=(struct _tuple12)(_tmp19.ReachableFL).val;
_tmp1C=_tmp1B.f1;_tmp1D=_tmp1B.f2;_LLE: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp1C,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp6D7;void*_tmp6D6;(_tmp6D6=0,Cyc_Tcutil_terr(
loc,((_tmp6D7="expression may not be fully initialized",_tag_dyneither(_tmp6D7,
sizeof(char),40))),_tag_dyneither(_tmp6D6,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp20=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp1C,env->all_changed,r);if(
_tmp1C.t == _tmp20.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp21=Cyc_CfFlowInfo_ReachableFL(
_tmp20,_tmp1D);Cyc_NewControlFlow_update_tryflow(env,_tmp21);return _tmp21;};};
_LLA:;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp6D8;struct
_tuple0 _tmp23=(_tmp6D8.f1=Cyc_Tcutil_compress(t),((_tmp6D8.f2=r,_tmp6D8)));void*
_tmp24;enum Cyc_CfFlowInfo_InitLevel _tmp26;void*_tmp27;void*_tmp29;void*_tmp2B;
struct Cyc_Absyn_DatatypeFieldInfo _tmp2D;union Cyc_Absyn_DatatypeFieldInfoU _tmp2E;
struct _tuple2 _tmp2F;struct Cyc_Absyn_Datatypefield*_tmp30;void*_tmp31;struct
_dyneither_ptr _tmp33;void*_tmp34;struct Cyc_List_List*_tmp36;void*_tmp37;struct
_dyneither_ptr _tmp39;void*_tmp3A;struct Cyc_Absyn_AggrInfo _tmp3C;union Cyc_Absyn_AggrInfoU
_tmp3D;struct Cyc_List_List*_tmp3E;void*_tmp3F;struct _dyneither_ptr _tmp41;void*
_tmp42;enum Cyc_Absyn_AggrKind _tmp44;struct Cyc_List_List*_tmp45;void*_tmp46;
struct _dyneither_ptr _tmp48;void*_tmp49;_LL10: _tmp24=_tmp23.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp25=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp24;if(_tmp25->tag != 3)goto
_LL12;else{_tmp26=_tmp25->f1;if(_tmp26 != Cyc_CfFlowInfo_NoneIL)goto _LL12;}};
_LL11: return;_LL12: _tmp27=_tmp23.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp28=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp27;if(_tmp28->tag != 0)goto _LL14;};_LL13:
return;_LL14: _tmp29=_tmp23.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp2A=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp29;if(_tmp2A->tag != 7)goto _LL16;};
_LL15: return;_LL16: _tmp2B=_tmp23.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp2C=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp2B;if(_tmp2C->tag != 4)goto
_LL18;else{_tmp2D=_tmp2C->f1;_tmp2E=_tmp2D.field_info;if((_tmp2E.KnownDatatypefield).tag
!= 2)goto _LL18;_tmp2F=(struct _tuple2)(_tmp2E.KnownDatatypefield).val;_tmp30=
_tmp2F.f2;}};_tmp31=_tmp23.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp32=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp31;if(_tmp32->tag != 6)goto _LL18;else{
_tmp33=_tmp32->f2;}};_LL17: if(_tmp30->typs == 0)return;_tmp36=_tmp30->typs;_tmp39=
_tmp33;goto _LL19;_LL18: _tmp34=_tmp23.f1;{struct Cyc_Absyn_TupleType_struct*_tmp35=(
struct Cyc_Absyn_TupleType_struct*)_tmp34;if(_tmp35->tag != 11)goto _LL1A;else{
_tmp36=_tmp35->f1;}};_tmp37=_tmp23.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp38=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp37;if(_tmp38->tag != 6)goto
_LL1A;else{_tmp39=_tmp38->f2;}};_LL19: {unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp36);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(
env,loc,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)).f2,*((
void**)_check_dyneither_subscript(_tmp39,sizeof(void*),i)));}}return;}_LL1A:
_tmp3A=_tmp23.f1;{struct Cyc_Absyn_AggrType_struct*_tmp3B=(struct Cyc_Absyn_AggrType_struct*)
_tmp3A;if(_tmp3B->tag != 12)goto _LL1C;else{_tmp3C=_tmp3B->f1;_tmp3D=_tmp3C.aggr_info;
_tmp3E=_tmp3C.targs;}};_tmp3F=_tmp23.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp40=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3F;if(_tmp40->tag != 6)goto
_LL1C;else{_tmp41=_tmp40->f2;}};_LL1B: {struct Cyc_Absyn_Aggrdecl*_tmp4B=Cyc_Absyn_get_known_aggrdecl(
_tmp3D);if(_tmp4B->impl == 0)return;{struct Cyc_List_List*_tmp4C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B->impl))->fields;struct _RegionHandle _tmp4D=_new_region("temp");
struct _RegionHandle*temp=& _tmp4D;_push_region(temp);{struct Cyc_List_List*_tmp4E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp4B->tvs,_tmp3E);{int i=0;
for(0;i < _get_dyneither_size(_tmp41,sizeof(void*));(i ++,_tmp4C=_tmp4C->tl)){void*
t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp4C))->hd)->type;
if(_tmp4E != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp4E,t);Cyc_NewControlFlow_check_nounique(
env,loc,t,((void**)_tmp41.curr)[i]);}}_npop_handler(0);return;};_pop_region(temp);};}
_LL1C: _tmp42=_tmp23.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp43=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42;if(_tmp43->tag != 13)goto _LL1E;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}};
_tmp46=_tmp23.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp47=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp46;if(_tmp47->tag != 6)goto _LL1E;else{_tmp48=_tmp47->f2;}};_LL1D:{int i=0;for(
0;i < _get_dyneither_size(_tmp48,sizeof(void*));(i ++,_tmp45=_tmp45->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp45))->hd)->type,((
void**)_tmp48.curr)[i]);}}return;_LL1E: _tmp49=_tmp23.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp4A=(struct Cyc_Absyn_PointerType_struct*)_tmp49;if(_tmp4A->tag != 5)goto _LL20;};
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL20;_LL1F:{const char*_tmp6DB;void*
_tmp6DA;(_tmp6DA=0,Cyc_Tcutil_warn(loc,((_tmp6DB="argument may still contain unique pointers",
_tag_dyneither(_tmp6DB,sizeof(char),43))),_tag_dyneither(_tmp6DA,sizeof(void*),0)));}
return;_LL20:;_LL21: return;_LLF:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp51=inflow;
int _tmp52;struct _tuple12 _tmp53;struct Cyc_Dict_Dict _tmp54;struct Cyc_List_List*
_tmp55;_LL23: if((_tmp51.BottomFL).tag != 1)goto _LL25;_tmp52=(int)(_tmp51.BottomFL).val;
_LL24: return Cyc_CfFlowInfo_BottomFL();_LL25: if((_tmp51.ReachableFL).tag != 2)goto
_LL22;_tmp53=(struct _tuple12)(_tmp51.ReachableFL).val;_tmp54=_tmp53.f1;_tmp55=
_tmp53.f2;_LL26: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp6DE;void*
_tmp6DD;(_tmp6DD=0,Cyc_Tcutil_terr(loc,((_tmp6DE="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp6DE,sizeof(char),47))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp58=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp59=r;struct Cyc_CfFlowInfo_Place*_tmp5B;_LL28: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp59;if(_tmp5A->tag != 5)goto
_LL2A;else{_tmp5B=_tmp5A->f1;}}_LL29: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp58,Cyc_CfFlowInfo_lookup_place(_tmp54,_tmp5B));goto _LL27;_LL2A:;_LL2B: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp58)){const char*_tmp6E1;void*_tmp6E0;(_tmp6E0=0,Cyc_Tcutil_warn(
loc,((_tmp6E1="argument may contain live unique pointers",_tag_dyneither(_tmp6E1,
sizeof(char),42))),_tag_dyneither(_tmp6E0,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL27:;}{struct Cyc_Dict_Dict _tmp5E=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp54,env->all_changed,r);if(_tmp54.t == _tmp5E.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp5F=Cyc_CfFlowInfo_ReachableFL(_tmp5E,_tmp55);Cyc_NewControlFlow_update_tryflow(
env,_tmp5F);return _tmp5F;};};};_LL22:;}struct _tuple18{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple18 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple18 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple18 _tmp6E2;return(
_tmp6E2.f1=inflow,((_tmp6E2.f2=0,_tmp6E2)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp62;void*_tmp63;struct _tuple13 _tmp61=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp62=_tmp61.f1;_tmp63=
_tmp61.f2;{struct Cyc_List_List*_tmp6E5;struct _tuple18 _tmp6E4;return(_tmp6E4.f1=
_tmp62,((_tmp6E4.f2=((_tmp6E5=_region_malloc(rgn,sizeof(*_tmp6E5)),((_tmp6E5->hd=
_tmp63,((_tmp6E5->tl=0,_tmp6E5)))))),_tmp6E4)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp6E6;env->all_changed=((
_tmp6E6=_region_malloc(env->r,sizeof(*_tmp6E6)),((_tmp6E6[0]=(env->fenv)->mt_place_set,
_tmp6E6))));}{union Cyc_CfFlowInfo_FlowInfo _tmp68;void*_tmp69;struct _tuple13
_tmp67=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;outflow=_tmp68;{struct Cyc_List_List*
_tmp6E7;rvals=((_tmp6E7=_region_malloc(rgn,sizeof(*_tmp6E7)),((_tmp6E7->hd=
_tmp69,((_tmp6E7->tl=0,_tmp6E7))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp6E8;env->all_changed=((_tmp6E8=_region_malloc(env->r,sizeof(*_tmp6E8)),((
_tmp6E8[0]=(env->fenv)->mt_place_set,_tmp6E8))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp6D;void*_tmp6E;struct _tuple13 _tmp6C=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp6D=_tmp6C.f1;_tmp6E=
_tmp6C.f2;{struct Cyc_List_List*_tmp6E9;rvals=((_tmp6E9=_region_malloc(rgn,
sizeof(*_tmp6E9)),((_tmp6E9->hd=_tmp6E,((_tmp6E9->tl=rvals,_tmp6E9))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp6D,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp6EA;env->all_changed=((
_tmp6EA=_region_malloc(env->r,sizeof(*_tmp6EA)),((_tmp6EA[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp6EA))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple18 _tmp6EB;return(_tmp6EB.f1=outflow,((_tmp6EB.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6EB)));};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);static struct
_tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct _RegionHandle*
_tmp72=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp74;struct Cyc_List_List*_tmp75;
struct _tuple18 _tmp73=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp72,env,inflow,
es,0,0);_tmp74=_tmp73.f1;_tmp75=_tmp73.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp76=
_tmp74;struct _tuple12 _tmp77;struct Cyc_Dict_Dict _tmp78;int _tmp79;_LL2D: if((_tmp76.ReachableFL).tag
!= 2)goto _LL2F;_tmp77=(struct _tuple12)(_tmp76.ReachableFL).val;_tmp78=_tmp77.f1;
_LL2E: for(0;_tmp75 != 0;(_tmp75=_tmp75->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp78,(void*)_tmp75->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6EE="expression may not be initialized",
_tag_dyneither(_tmp6EE,sizeof(char),34))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));}}
goto _LL2C;_LL2F: if((_tmp76.BottomFL).tag != 1)goto _LL2C;_tmp79=(int)(_tmp76.BottomFL).val;
_LL30: goto _LL2C;_LL2C:;}{struct _tuple13 _tmp6EF;return(_tmp6EF.f1=_tmp74,((
_tmp6EF.f2=(env->fenv)->unknown_all,_tmp6EF)));};}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel
il);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp7D=outflow;int _tmp7E;struct _tuple12 _tmp7F;struct Cyc_Dict_Dict _tmp80;struct
Cyc_List_List*_tmp81;_LL32: if((_tmp7D.BottomFL).tag != 1)goto _LL34;_tmp7E=(int)(
_tmp7D.BottomFL).val;_LL33: return outflow;_LL34: if((_tmp7D.ReachableFL).tag != 2)
goto _LL31;_tmp7F=(struct _tuple12)(_tmp7D.ReachableFL).val;_tmp80=_tmp7F.f1;
_tmp81=_tmp7F.f2;_LL35: {union Cyc_CfFlowInfo_AbsLVal _tmp82=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp83;struct Cyc_CfFlowInfo_Place*_tmp84;_LL37: if((_tmp82.UnknownL).tag
!= 2)goto _LL39;_tmp83=(int)(_tmp82.UnknownL).val;_LL38: return outflow;_LL39: if((
_tmp82.PlaceL).tag != 1)goto _LL36;_tmp84=(struct Cyc_CfFlowInfo_Place*)(_tmp82.PlaceL).val;
_LL3A: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmp85=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp80,env->all_changed,_tmp84,nzval),_tmp81);return _tmp85;}
_LL36:;}_LL31:;}static struct _tuple15 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il);static struct _tuple15 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp86=outflow;int _tmp87;struct _tuple12 _tmp88;struct Cyc_Dict_Dict _tmp89;struct
Cyc_List_List*_tmp8A;_LL3C: if((_tmp86.BottomFL).tag != 1)goto _LL3E;_tmp87=(int)(
_tmp86.BottomFL).val;_LL3D: {struct _tuple15 _tmp6F0;return(_tmp6F0.f1=outflow,((
_tmp6F0.f2=outflow,_tmp6F0)));}_LL3E: if((_tmp86.ReachableFL).tag != 2)goto _LL3B;
_tmp88=(struct _tuple12)(_tmp86.ReachableFL).val;_tmp89=_tmp88.f1;_tmp8A=_tmp88.f2;
_LL3F: {union Cyc_CfFlowInfo_AbsLVal _tmp8C=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmp8D;struct Cyc_CfFlowInfo_Place*_tmp8E;_LL41: if((_tmp8C.UnknownL).tag
!= 2)goto _LL43;_tmp8D=(int)(_tmp8C.UnknownL).val;_LL42: {struct _tuple15 _tmp6F1;
return(_tmp6F1.f1=outflow,((_tmp6F1.f2=outflow,_tmp6F1)));}_LL43: if((_tmp8C.PlaceL).tag
!= 1)goto _LL40;_tmp8E=(struct Cyc_CfFlowInfo_Place*)(_tmp8C.PlaceL).val;_LL44: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp6F2;return(_tmp6F2.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp89,env->all_changed,_tmp8E,nzval),_tmp8A),((_tmp6F2.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp89,env->all_changed,_tmp8E,(env->fenv)->zero),
_tmp8A),_tmp6F2)));}_LL40:;}_LL3B:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r);static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp93=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp95;void*
_tmp96;struct Cyc_Absyn_PtrAtts _tmp97;union Cyc_Absyn_Constraint*_tmp98;union Cyc_Absyn_Constraint*
_tmp99;_LL46: {struct Cyc_Absyn_PointerType_struct*_tmp94=(struct Cyc_Absyn_PointerType_struct*)
_tmp93;if(_tmp94->tag != 5)goto _LL48;else{_tmp95=_tmp94->f1;_tmp96=_tmp95.elt_typ;
_tmp97=_tmp95.ptr_atts;_tmp98=_tmp97.bounds;_tmp99=_tmp97.zero_term;}}_LL47: {
union Cyc_CfFlowInfo_FlowInfo _tmp9A=f;int _tmp9B;struct _tuple12 _tmp9C;struct Cyc_Dict_Dict
_tmp9D;struct Cyc_List_List*_tmp9E;_LL4B: if((_tmp9A.BottomFL).tag != 1)goto _LL4D;
_tmp9B=(int)(_tmp9A.BottomFL).val;_LL4C: {struct _tuple13 _tmp6F3;return(_tmp6F3.f1=
f,((_tmp6F3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp96,(env->fenv)->unknown_all),
_tmp6F3)));}_LL4D: if((_tmp9A.ReachableFL).tag != 2)goto _LL4A;_tmp9C=(struct
_tuple12)(_tmp9A.ReachableFL).val;_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;_LL4E: if(Cyc_Tcutil_is_bound_one(
_tmp98)){void*_tmpA0=r;struct Cyc_CfFlowInfo_Place*_tmpA4;enum Cyc_CfFlowInfo_InitLevel
_tmpA7;_LL50: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpA1=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpA0;if(_tmpA1->tag != 1)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpA2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpA0;if(_tmpA2->tag != 2)goto
_LL54;}_LL53:{void*_tmpA8=e->annot;struct Cyc_List_List*_tmpAA;_LL5D: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA9=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpA8;if(_tmpA9->tag != Cyc_CfFlowInfo_NotZero)
goto _LL5F;else{_tmpAA=_tmpA9->f1;}}_LL5E: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,
_tmpAA))goto _LL60;goto _LL5C;_LL5F:;_LL60:{void*_tmpAB=e->r;_LL62: {struct Cyc_Absyn_Subscript_e_struct*
_tmpAC=(struct Cyc_Absyn_Subscript_e_struct*)_tmpAB;if(_tmpAC->tag != 24)goto _LL64;}
_LL63:{struct Cyc_CfFlowInfo_NotZero_struct _tmp6F6;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp6F5;e->annot=(void*)((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5[0]=((
_tmp6F6.tag=Cyc_CfFlowInfo_NotZero,((_tmp6F6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9E),_tmp6F6)))),_tmp6F5))));}goto _LL61;_LL64:;_LL65: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL61;_LL61:;}goto _LL5C;_LL5C:;}goto _LL4F;_LL54: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpA3=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpA0;if(_tmpA3->tag != 5)goto
_LL56;else{_tmpA4=_tmpA3->f1;}}_LL55:{void*_tmpAF=e->annot;struct Cyc_List_List*
_tmpB1;_LL67: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpB0=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpAF;if(_tmpB0->tag != Cyc_CfFlowInfo_NotZero)goto _LL69;else{_tmpB1=_tmpB0->f1;}}
_LL68: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,_tmpB1))goto _LL6A;goto _LL66;_LL69:;
_LL6A:{void*_tmpB2=e->r;_LL6C: {struct Cyc_Absyn_Subscript_e_struct*_tmpB3=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpB2;if(_tmpB3->tag != 24)goto _LL6E;}_LL6D:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp6F9;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp6F8;e->annot=(void*)((_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8[0]=((
_tmp6F9.tag=Cyc_CfFlowInfo_NotZero,((_tmp6F9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9E),_tmp6F9)))),_tmp6F8))));}goto _LL6B;_LL6E:;_LL6F: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL6B;_LL6B:;}goto _LL66;_LL66:;}{struct _tuple13 _tmp6FA;return(_tmp6FA.f1=f,((
_tmp6FA.f2=Cyc_CfFlowInfo_lookup_place(_tmp9D,_tmpA4),_tmp6FA)));};_LL56: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpA5=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpA0;if(_tmpA5->tag != 0)goto _LL58;}_LL57: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp6FB;return(_tmp6FB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6FB.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp96,(env->fenv)->unknown_all),_tmp6FB)));};
_LL58: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpA6=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpA0;if(_tmpA6->tag != 3)goto _LL5A;else{_tmpA7=_tmpA6->f1;}}_LL59: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpA7);goto _LL5B;_LL5A:;_LL5B:{void*_tmpB8=e->r;_LL71: {struct
Cyc_Absyn_Subscript_e_struct*_tmpB9=(struct Cyc_Absyn_Subscript_e_struct*)_tmpB8;
if(_tmpB9->tag != 24)goto _LL73;}_LL72:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp6FE;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp6FD;e->annot=(void*)((_tmp6FD=
_cycalloc(sizeof(*_tmp6FD)),((_tmp6FD[0]=((_tmp6FE.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp6FE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E),_tmp6FE)))),
_tmp6FD))));}goto _LL70;_LL73:;_LL74: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL70;_LL70:;}goto _LL4F;_LL4F:;}else{void*_tmpBC=e->annot;struct Cyc_List_List*
_tmpBE;_LL76: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpBD=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpBC;if(_tmpBD->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL78;else{_tmpBE=_tmpBD->f1;}}
_LL77: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,_tmpBE))goto _LL79;goto _LL75;_LL78:;
_LL79:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp701;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp700;e->annot=(void*)((_tmp700=_cycalloc(sizeof(*_tmp700)),((_tmp700[0]=((
_tmp701.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp701.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9E),_tmp701)))),_tmp700))));}goto _LL75;_LL75:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp9D,r)){case Cyc_CfFlowInfo_NoneIL: _LL7A:{void*_tmpC1=r;_LL7D: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpC2=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpC1;if(_tmpC2->tag != 7)goto _LL7F;}_LL7E:{const char*_tmp704;void*_tmp703;(
_tmp703=0,Cyc_Tcutil_terr(e->loc,((_tmp704="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp704,sizeof(char),61))),_tag_dyneither(_tmp703,sizeof(void*),0)));}
goto _LL7C;_LL7F:;_LL80: {const char*_tmp707;void*_tmp706;(_tmp706=0,Cyc_Tcutil_terr(
e->loc,((_tmp707="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp707,sizeof(char),46))),_tag_dyneither(_tmp706,sizeof(void*),0)));}_LL7C:;}
goto _LL7B;case Cyc_CfFlowInfo_AllIL: _LL7B: {struct _tuple13 _tmp708;return(_tmp708.f1=
f,((_tmp708.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp96,(env->fenv)->unknown_all),
_tmp708)));}case Cyc_CfFlowInfo_ThisIL: _LL81: {struct _tuple13 _tmp709;return(
_tmp709.f1=f,((_tmp709.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp96,(env->fenv)->unknown_none),
_tmp709)));}}_LL4A:;}_LL48:;_LL49: {struct Cyc_Core_Impossible_struct _tmp70F;
const char*_tmp70E;struct Cyc_Core_Impossible_struct*_tmp70D;(int)_throw((void*)((
_tmp70D=_cycalloc(sizeof(*_tmp70D)),((_tmp70D[0]=((_tmp70F.tag=Cyc_Core_Impossible,((
_tmp70F.f1=((_tmp70E="right deref of non-pointer-type",_tag_dyneither(_tmp70E,
sizeof(char),32))),_tmp70F)))),_tmp70D)))));}_LL45:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpCC=e1->r;void*
_tmpCE;struct Cyc_Absyn_Vardecl*_tmpD0;void*_tmpD2;struct Cyc_Absyn_Vardecl*_tmpD4;
void*_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD8;void*_tmpDA;struct Cyc_Absyn_Vardecl*
_tmpDC;_LL84: {struct Cyc_Absyn_Var_e_struct*_tmpCD=(struct Cyc_Absyn_Var_e_struct*)
_tmpCC;if(_tmpCD->tag != 1)goto _LL86;else{_tmpCE=(void*)_tmpCD->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpCF=(struct Cyc_Absyn_Pat_b_struct*)_tmpCE;if(_tmpCF->tag != 5)goto _LL86;else{
_tmpD0=_tmpCF->f1;}};}}_LL85: _tmpD4=_tmpD0;goto _LL87;_LL86: {struct Cyc_Absyn_Var_e_struct*
_tmpD1=(struct Cyc_Absyn_Var_e_struct*)_tmpCC;if(_tmpD1->tag != 1)goto _LL88;else{
_tmpD2=(void*)_tmpD1->f2;{struct Cyc_Absyn_Local_b_struct*_tmpD3=(struct Cyc_Absyn_Local_b_struct*)
_tmpD2;if(_tmpD3->tag != 4)goto _LL88;else{_tmpD4=_tmpD3->f1;}};}}_LL87: _tmpD8=
_tmpD4;goto _LL89;_LL88: {struct Cyc_Absyn_Var_e_struct*_tmpD5=(struct Cyc_Absyn_Var_e_struct*)
_tmpCC;if(_tmpD5->tag != 1)goto _LL8A;else{_tmpD6=(void*)_tmpD5->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpD7=(struct Cyc_Absyn_Param_b_struct*)_tmpD6;if(_tmpD7->tag != 3)goto _LL8A;
else{_tmpD8=_tmpD7->f1;}};}}_LL89: _tmpDC=_tmpD8;goto _LL8B;_LL8A: {struct Cyc_Absyn_Var_e_struct*
_tmpD9=(struct Cyc_Absyn_Var_e_struct*)_tmpCC;if(_tmpD9->tag != 1)goto _LL8C;else{
_tmpDA=(void*)_tmpD9->f2;{struct Cyc_Absyn_Global_b_struct*_tmpDB=(struct Cyc_Absyn_Global_b_struct*)
_tmpDA;if(_tmpDB->tag != 1)goto _LL8C;else{_tmpDC=_tmpDB->f1;}};}}_LL8B: if(!_tmpDC->escapes){
void*_tmpDD=e2->r;void*_tmpDF;struct Cyc_Absyn_Vardecl*_tmpE1;void*_tmpE3;struct
Cyc_Absyn_Vardecl*_tmpE5;void*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpE9;void*_tmpEB;
struct Cyc_Absyn_Vardecl*_tmpED;_LL8F: {struct Cyc_Absyn_Var_e_struct*_tmpDE=(
struct Cyc_Absyn_Var_e_struct*)_tmpDD;if(_tmpDE->tag != 1)goto _LL91;else{_tmpDF=(
void*)_tmpDE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpE0=(struct Cyc_Absyn_Pat_b_struct*)
_tmpDF;if(_tmpE0->tag != 5)goto _LL91;else{_tmpE1=_tmpE0->f1;}};}}_LL90: _tmpE5=
_tmpE1;goto _LL92;_LL91: {struct Cyc_Absyn_Var_e_struct*_tmpE2=(struct Cyc_Absyn_Var_e_struct*)
_tmpDD;if(_tmpE2->tag != 1)goto _LL93;else{_tmpE3=(void*)_tmpE2->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpE4=(struct Cyc_Absyn_Local_b_struct*)_tmpE3;if(_tmpE4->tag != 4)goto _LL93;
else{_tmpE5=_tmpE4->f1;}};}}_LL92: _tmpE9=_tmpE5;goto _LL94;_LL93: {struct Cyc_Absyn_Var_e_struct*
_tmpE6=(struct Cyc_Absyn_Var_e_struct*)_tmpDD;if(_tmpE6->tag != 1)goto _LL95;else{
_tmpE7=(void*)_tmpE6->f2;{struct Cyc_Absyn_Param_b_struct*_tmpE8=(struct Cyc_Absyn_Param_b_struct*)
_tmpE7;if(_tmpE8->tag != 3)goto _LL95;else{_tmpE9=_tmpE8->f1;}};}}_LL94: _tmpED=
_tmpE9;goto _LL96;_LL95: {struct Cyc_Absyn_Var_e_struct*_tmpEA=(struct Cyc_Absyn_Var_e_struct*)
_tmpDD;if(_tmpEA->tag != 1)goto _LL97;else{_tmpEB=(void*)_tmpEA->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpEC=(struct Cyc_Absyn_Global_b_struct*)_tmpEB;if(_tmpEC->tag != 1)goto _LL97;
else{_tmpED=_tmpEC->f1;}};}}_LL96: if(!_tmpED->escapes){int found=0;{struct Cyc_List_List*
_tmpEE=relns;for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){struct Cyc_CfFlowInfo_Reln*
_tmpEF=(struct Cyc_CfFlowInfo_Reln*)_tmpEE->hd;if(_tmpEF->vd == _tmpED){union Cyc_CfFlowInfo_RelnOp
_tmpF0=_tmpEF->rop;struct Cyc_Absyn_Vardecl*_tmpF1;_LL9A: if((_tmpF0.LessNumelts).tag
!= 3)goto _LL9C;_tmpF1=(struct Cyc_Absyn_Vardecl*)(_tmpF0.LessNumelts).val;if(!(
_tmpF1 == _tmpDC))goto _LL9C;_LL9B: return relns;_LL9C:;_LL9D: continue;_LL99:;}}}if(
!found){struct Cyc_CfFlowInfo_Reln*_tmp712;struct Cyc_List_List*_tmp711;return(
_tmp711=_region_malloc(rgn,sizeof(*_tmp711)),((_tmp711->hd=((_tmp712=
_region_malloc(rgn,sizeof(*_tmp712)),((_tmp712->vd=_tmpED,((_tmp712->rop=Cyc_CfFlowInfo_LessNumelts(
_tmpDC),_tmp712)))))),((_tmp711->tl=relns,_tmp711)))));}}return relns;_LL97:;
_LL98: return relns;_LL8E:;}else{return relns;}_LL8C:;_LL8D: return relns;_LL83:;}
static struct _tuple13 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmpF4=outflow;int _tmpF5;struct _tuple12 _tmpF6;struct Cyc_Dict_Dict _tmpF7;struct
Cyc_List_List*_tmpF8;_LL9F: if((_tmpF4.BottomFL).tag != 1)goto _LLA1;_tmpF5=(int)(
_tmpF4.BottomFL).val;_LLA0: {struct _tuple13 _tmp713;return(_tmp713.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp713.f2=rval,_tmp713)));}_LLA1: if((_tmpF4.ReachableFL).tag != 2)goto _LL9E;
_tmpF6=(struct _tuple12)(_tmpF4.ReachableFL).val;_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;
_LLA2: {union Cyc_CfFlowInfo_AbsLVal _tmpFA=lval;struct Cyc_CfFlowInfo_Place*_tmpFB;
int _tmpFC;_LLA4: if((_tmpFA.PlaceL).tag != 1)goto _LLA6;_tmpFB=(struct Cyc_CfFlowInfo_Place*)(
_tmpFA.PlaceL).val;_LLA5: _tmpF7=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpF7,env->all_changed,
_tmpFB,rval);_tmpF8=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmpF8,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmpF7,_tmpF8);Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple13 _tmp714;return(_tmp714.f1=outflow,((_tmp714.f2=rval,
_tmp714)));};_LLA6: if((_tmpFA.UnknownL).tag != 2)goto _LLA3;_tmpFC=(int)(_tmpFA.UnknownL).val;
_LLA7: {struct _tuple13 _tmp715;return(_tmp715.f1=Cyc_NewControlFlow_use_Rval(env,
rexp->loc,outflow,rval),((_tmp715.f2=rval,_tmp715)));}_LLA3:;}_LL9E:;}static int
Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e);static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmpFF=e->r;void*_tmp101;void*_tmp104;void*_tmp107;
struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*
_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp112;_LLA9: {struct
Cyc_Absyn_Var_e_struct*_tmp100=(struct Cyc_Absyn_Var_e_struct*)_tmpFF;if(_tmp100->tag
!= 1)goto _LLAB;else{_tmp101=(void*)_tmp100->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp102=(struct Cyc_Absyn_Param_b_struct*)_tmp101;if(_tmp102->tag != 3)goto _LLAB;};}}
_LLAA: goto _LLAC;_LLAB: {struct Cyc_Absyn_Var_e_struct*_tmp103=(struct Cyc_Absyn_Var_e_struct*)
_tmpFF;if(_tmp103->tag != 1)goto _LLAD;else{_tmp104=(void*)_tmp103->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp105=(struct Cyc_Absyn_Local_b_struct*)_tmp104;if(_tmp105->tag != 4)goto _LLAD;};}}
_LLAC: goto _LLAE;_LLAD: {struct Cyc_Absyn_Var_e_struct*_tmp106=(struct Cyc_Absyn_Var_e_struct*)
_tmpFF;if(_tmp106->tag != 1)goto _LLAF;else{_tmp107=(void*)_tmp106->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp108=(struct Cyc_Absyn_Pat_b_struct*)_tmp107;if(_tmp108->tag != 5)goto _LLAF;};}}
_LLAE: return 1;_LLAF: {struct Cyc_Absyn_Deref_e_struct*_tmp109=(struct Cyc_Absyn_Deref_e_struct*)
_tmpFF;if(_tmp109->tag != 21)goto _LLB1;else{_tmp10A=_tmp109->f1;}}_LLB0: _tmp10C=
_tmp10A;goto _LLB2;_LLB1: {struct Cyc_Absyn_AggrMember_e_struct*_tmp10B=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpFF;if(_tmp10B->tag != 22)goto _LLB3;else{_tmp10C=_tmp10B->f1;}}_LLB2: _tmp10E=
_tmp10C;goto _LLB4;_LLB3: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp10D=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpFF;if(_tmp10D->tag != 23)goto _LLB5;else{_tmp10E=_tmp10D->f1;}}_LLB4: return Cyc_NewControlFlow_is_local_var_rooted_path(
_tmp10E);_LLB5: {struct Cyc_Absyn_Subscript_e_struct*_tmp10F=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpFF;if(_tmp10F->tag != 24)goto _LLB7;else{_tmp110=_tmp10F->f1;}}_LLB6: {void*
_tmp113=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp110->topt))->v);
_LLBC: {struct Cyc_Absyn_TupleType_struct*_tmp114=(struct Cyc_Absyn_TupleType_struct*)
_tmp113;if(_tmp114->tag != 11)goto _LLBE;}_LLBD: return Cyc_NewControlFlow_is_local_var_rooted_path(
_tmp110);_LLBE:;_LLBF: return 0;_LLBB:;}_LLB7: {struct Cyc_Absyn_Cast_e_struct*
_tmp111=(struct Cyc_Absyn_Cast_e_struct*)_tmpFF;if(_tmp111->tag != 15)goto _LLB9;
else{_tmp112=_tmp111->f2;}}_LLB8: return Cyc_NewControlFlow_is_local_var_rooted_path(
_tmp112);_LLB9:;_LLBA: return 0;_LLA8:;}struct _tuple19{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple20{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp7A4(unsigned int*_tmp7A3,unsigned int*_tmp7A2,void***_tmp7A0,
struct Cyc_List_List**_tmp2EF){for(*_tmp7A3=0;*_tmp7A3 < *_tmp7A2;(*_tmp7A3)++){
void*_tmp79E;(*_tmp7A0)[*_tmp7A3]=((_tmp79E=(void*)((struct Cyc_List_List*)
_check_null(*_tmp2EF))->hd,((*_tmp2EF=(*_tmp2EF)->tl,_tmp79E))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp115=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp116=inflow;int _tmp117;struct _tuple12 _tmp118;struct Cyc_Dict_Dict _tmp119;
struct Cyc_List_List*_tmp11A;_LLC1: if((_tmp116.BottomFL).tag != 1)goto _LLC3;
_tmp117=(int)(_tmp116.BottomFL).val;_LLC2: {struct _tuple13 _tmp716;return(_tmp716.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp716.f2=_tmp115->unknown_all,_tmp716)));}_LLC3:
if((_tmp116.ReachableFL).tag != 2)goto _LLC0;_tmp118=(struct _tuple12)(_tmp116.ReachableFL).val;
_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;_LLC4: d=_tmp119;relns=_tmp11A;_LLC0:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp11D;union Cyc_CfFlowInfo_AbsLVal _tmp11E;struct _tuple14 _tmp11C=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp11D=_tmp11C.f1;_tmp11E=_tmp11C.f2;{struct _tuple14 _tmp717;
struct _tuple14 _tmp120=(_tmp717.f1=_tmp11D,((_tmp717.f2=_tmp11E,_tmp717)));union
Cyc_CfFlowInfo_FlowInfo _tmp121;struct _tuple12 _tmp122;struct Cyc_Dict_Dict _tmp123;
struct Cyc_List_List*_tmp124;union Cyc_CfFlowInfo_AbsLVal _tmp125;struct Cyc_CfFlowInfo_Place*
_tmp126;_LLC6: _tmp121=_tmp120.f1;if((_tmp121.ReachableFL).tag != 2)goto _LLC8;
_tmp122=(struct _tuple12)(_tmp121.ReachableFL).val;_tmp123=_tmp122.f1;_tmp124=
_tmp122.f2;_tmp125=_tmp120.f2;if((_tmp125.PlaceL).tag != 1)goto _LLC8;_tmp126=(
struct Cyc_CfFlowInfo_Place*)(_tmp125.PlaceL).val;_LLC7: {void*_tmp127=Cyc_CfFlowInfo_lookup_place(
_tmp123,_tmp126);{void*_tmp128=_tmp127;struct Cyc_Absyn_Exp*_tmp12A;int _tmp12B;
void*_tmp12C;_LLCB: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp129=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp128;if(_tmp129->tag != 7)goto _LLCD;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;
_tmp12C=(void*)_tmp129->f3;}}_LLCC: if(_tmp12A == e  && _tmp12B == env->iteration_num){
struct _tuple13 _tmp718;return(_tmp718.f1=_tmp11D,((_tmp718.f2=_tmp12C,_tmp718)));}{
const char*_tmp71B;void*_tmp71A;(_tmp71A=0,Cyc_Tcutil_terr(e->loc,((_tmp71B="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp71B,sizeof(char),71))),_tag_dyneither(_tmp71A,sizeof(void*),0)));}{
struct _tuple13 _tmp71C;return(_tmp71C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71C.f2=
_tmp115->unknown_all,_tmp71C)));};_LLCD:;_LLCE: goto _LLCA;_LLCA:;}{void*_tmp131=
Cyc_CfFlowInfo_make_unique_consumed(_tmp115,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,e,env->iteration_num,_tmp127);struct Cyc_Dict_Dict
_tmp132=Cyc_CfFlowInfo_assign_place(_tmp115,e->loc,_tmp123,env->all_changed,
_tmp126,_tmp131);struct _tuple13 _tmp71D;return(_tmp71D.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp132,_tmp124),((_tmp71D.f2=_tmp127,_tmp71D)));};}_LLC8:;_LLC9: goto _LLC5;_LLC5:;};}{
void*_tmp134=e->r;struct Cyc_Absyn_Exp*_tmp136;enum Cyc_Absyn_Coercion _tmp137;
struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*
_tmp13D;union Cyc_Absyn_Cnst _tmp13F;int _tmp140;union Cyc_Absyn_Cnst _tmp142;struct
_tuple6 _tmp143;int _tmp144;union Cyc_Absyn_Cnst _tmp146;struct _tuple6 _tmp147;void*
_tmp149;struct Cyc_List_List*_tmp14C;void*_tmp154;void*_tmp157;struct Cyc_Absyn_Vardecl*
_tmp159;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15D;void*_tmp15F;struct Cyc_Absyn_Vardecl*
_tmp161;enum Cyc_Absyn_Primop _tmp163;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Exp*
_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Core_Opt*_tmp169;struct Cyc_Core_Opt
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Core_Opt*
_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_List_List*
_tmp177;struct Cyc_Absyn_MallocInfo _tmp179;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;
void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;
struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*
_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*
_tmp18A;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct
_dyneither_ptr*_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;
struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*
_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*
_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*
_tmp19E;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_Datatypedecl*_tmp1A1;struct
Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;
struct Cyc_Absyn_Aggrdecl*_tmp1A8;struct Cyc_Absyn_Aggrdecl _tmp1A9;enum Cyc_Absyn_AggrKind
_tmp1AA;struct Cyc_Absyn_AggrdeclImpl*_tmp1AB;struct Cyc_Absyn_AggrdeclImpl _tmp1AC;
struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1B0;struct Cyc_Absyn_Vardecl*
_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;int _tmp1B5;
struct Cyc_Absyn_Stmt*_tmp1B7;void*_tmp1B9;_LLD0: {struct Cyc_Absyn_Cast_e_struct*
_tmp135=(struct Cyc_Absyn_Cast_e_struct*)_tmp134;if(_tmp135->tag != 15)goto _LLD2;
else{_tmp136=_tmp135->f2;_tmp137=_tmp135->f4;if(_tmp137 != Cyc_Absyn_NonNull_to_Null)
goto _LLD2;}}_LLD1: {union Cyc_CfFlowInfo_FlowInfo _tmp1C1;void*_tmp1C2;struct
_tuple13 _tmp1C0=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp136);
_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1C4;void*
_tmp1C5;struct _tuple13 _tmp1C3=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1C1,
_tmp136,_tmp1C2);_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;{struct _tuple13 _tmp71E;
return(_tmp71E.f1=_tmp1C4,((_tmp71E.f2=_tmp1C2,_tmp71E)));};};}_LLD2: {struct Cyc_Absyn_Cast_e_struct*
_tmp138=(struct Cyc_Absyn_Cast_e_struct*)_tmp134;if(_tmp138->tag != 15)goto _LLD4;
else{_tmp139=_tmp138->f2;}}_LLD3: _tmp13B=_tmp139;goto _LLD5;_LLD4: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp13A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp134;if(_tmp13A->tag != 13)
goto _LLD6;else{_tmp13B=_tmp13A->f1;}}_LLD5: _tmp13D=_tmp13B;goto _LLD7;_LLD6: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp13C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp134;if(_tmp13C->tag != 14)goto _LLD8;else{_tmp13D=_tmp13C->f1;}}_LLD7: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp13D);_LLD8: {struct Cyc_Absyn_Const_e_struct*
_tmp13E=(struct Cyc_Absyn_Const_e_struct*)_tmp134;if(_tmp13E->tag != 0)goto _LLDA;
else{_tmp13F=_tmp13E->f1;if((_tmp13F.Null_c).tag != 1)goto _LLDA;_tmp140=(int)(
_tmp13F.Null_c).val;}}_LLD9: goto _LLDB;_LLDA: {struct Cyc_Absyn_Const_e_struct*
_tmp141=(struct Cyc_Absyn_Const_e_struct*)_tmp134;if(_tmp141->tag != 0)goto _LLDC;
else{_tmp142=_tmp141->f1;if((_tmp142.Int_c).tag != 4)goto _LLDC;_tmp143=(struct
_tuple6)(_tmp142.Int_c).val;_tmp144=_tmp143.f2;if(_tmp144 != 0)goto _LLDC;}}_LLDB: {
struct _tuple13 _tmp71F;return(_tmp71F.f1=inflow,((_tmp71F.f2=_tmp115->zero,
_tmp71F)));}_LLDC: {struct Cyc_Absyn_Const_e_struct*_tmp145=(struct Cyc_Absyn_Const_e_struct*)
_tmp134;if(_tmp145->tag != 0)goto _LLDE;else{_tmp146=_tmp145->f1;if((_tmp146.Int_c).tag
!= 4)goto _LLDE;_tmp147=(struct _tuple6)(_tmp146.Int_c).val;}}_LLDD: goto _LLDF;
_LLDE: {struct Cyc_Absyn_Var_e_struct*_tmp148=(struct Cyc_Absyn_Var_e_struct*)
_tmp134;if(_tmp148->tag != 1)goto _LLE0;else{_tmp149=(void*)_tmp148->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp14A=(struct Cyc_Absyn_Funname_b_struct*)_tmp149;if(_tmp14A->tag != 2)goto _LLE0;};}}
_LLDF: {struct _tuple13 _tmp720;return(_tmp720.f1=inflow,((_tmp720.f2=_tmp115->notzeroall,
_tmp720)));}_LLE0: {struct Cyc_Absyn_Datatype_e_struct*_tmp14B=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp134;if(_tmp14B->tag != 31)goto _LLE2;else{_tmp14C=_tmp14B->f1;if(_tmp14C != 0)
goto _LLE2;}}_LLE1: goto _LLE3;_LLE2: {struct Cyc_Absyn_Const_e_struct*_tmp14D=(
struct Cyc_Absyn_Const_e_struct*)_tmp134;if(_tmp14D->tag != 0)goto _LLE4;}_LLE3:
goto _LLE5;_LLE4: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp14E=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp134;if(_tmp14E->tag != 19)goto _LLE6;}_LLE5: goto _LLE7;_LLE6: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp14F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp134;if(_tmp14F->tag != 18)goto
_LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_Offsetof_e_struct*_tmp150=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp134;if(_tmp150->tag != 20)goto _LLEA;}_LLE9: goto
_LLEB;_LLEA: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp151=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp134;if(_tmp151->tag != 33)goto _LLEC;}_LLEB: goto _LLED;_LLEC: {struct Cyc_Absyn_Enum_e_struct*
_tmp152=(struct Cyc_Absyn_Enum_e_struct*)_tmp134;if(_tmp152->tag != 32)goto _LLEE;}
_LLED: {struct _tuple13 _tmp721;return(_tmp721.f1=inflow,((_tmp721.f2=_tmp115->unknown_all,
_tmp721)));}_LLEE: {struct Cyc_Absyn_Var_e_struct*_tmp153=(struct Cyc_Absyn_Var_e_struct*)
_tmp134;if(_tmp153->tag != 1)goto _LLF0;else{_tmp154=(void*)_tmp153->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp155=(struct Cyc_Absyn_Global_b_struct*)_tmp154;if(_tmp155->tag != 1)goto _LLF0;};}}
_LLEF: {struct _tuple13 _tmp722;return(_tmp722.f1=inflow,((_tmp722.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp115->unknown_all),
_tmp722)));}_LLF0: {struct Cyc_Absyn_Var_e_struct*_tmp156=(struct Cyc_Absyn_Var_e_struct*)
_tmp134;if(_tmp156->tag != 1)goto _LLF2;else{_tmp157=(void*)_tmp156->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp158=(struct Cyc_Absyn_Param_b_struct*)_tmp157;if(_tmp158->tag != 3)goto _LLF2;
else{_tmp159=_tmp158->f1;}};}}_LLF1: _tmp15D=_tmp159;goto _LLF3;_LLF2: {struct Cyc_Absyn_Var_e_struct*
_tmp15A=(struct Cyc_Absyn_Var_e_struct*)_tmp134;if(_tmp15A->tag != 1)goto _LLF4;
else{_tmp15B=(void*)_tmp15A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp15C=(struct
Cyc_Absyn_Local_b_struct*)_tmp15B;if(_tmp15C->tag != 4)goto _LLF4;else{_tmp15D=
_tmp15C->f1;}};}}_LLF3: _tmp161=_tmp15D;goto _LLF5;_LLF4: {struct Cyc_Absyn_Var_e_struct*
_tmp15E=(struct Cyc_Absyn_Var_e_struct*)_tmp134;if(_tmp15E->tag != 1)goto _LLF6;
else{_tmp15F=(void*)_tmp15E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp160=(struct Cyc_Absyn_Pat_b_struct*)
_tmp15F;if(_tmp160->tag != 5)goto _LLF6;else{_tmp161=_tmp160->f1;}};}}_LLF5: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp728;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp727;struct _tuple13 _tmp726;return(_tmp726.f1=inflow,((_tmp726.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp728=_region_malloc(
env->r,sizeof(*_tmp728)),((_tmp728[0]=((_tmp727.tag=0,((_tmp727.f1=_tmp161,
_tmp727)))),_tmp728))))),_tmp726)));}_LLF6: {struct Cyc_Absyn_Primop_e_struct*
_tmp162=(struct Cyc_Absyn_Primop_e_struct*)_tmp134;if(_tmp162->tag != 3)goto _LLF8;
else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;}}_LLF7: {union Cyc_CfFlowInfo_FlowInfo
_tmp1CF;void*_tmp1D0;struct _tuple13 _tmp1CE=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp164);_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;{struct _tuple13 _tmp729;
return(_tmp729.f1=_tmp1CF,((_tmp729.f2=_tmp1D0,_tmp729)));};}_LLF8: {struct Cyc_Absyn_Increment_e_struct*
_tmp165=(struct Cyc_Absyn_Increment_e_struct*)_tmp134;if(_tmp165->tag != 5)goto
_LLFA;else{_tmp166=_tmp165->f1;}}_LLF9: {struct Cyc_List_List _tmp72A;struct Cyc_List_List
_tmp1D2=(_tmp72A.hd=_tmp166,((_tmp72A.tl=0,_tmp72A)));union Cyc_CfFlowInfo_FlowInfo
_tmp1D4;struct _tuple13 _tmp1D3=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp1D2);_tmp1D4=_tmp1D3.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp1D6;struct _tuple14 _tmp1D5=Cyc_NewControlFlow_anal_Lexp(env,_tmp1D4,0,_tmp166);
_tmp1D6=_tmp1D5.f2;{struct _tuple19 _tmp72B;struct _tuple19 _tmp1D8=(_tmp72B.f1=
_tmp1D6,((_tmp72B.f2=_tmp1D4,_tmp72B)));union Cyc_CfFlowInfo_AbsLVal _tmp1D9;
struct Cyc_CfFlowInfo_Place*_tmp1DA;union Cyc_CfFlowInfo_FlowInfo _tmp1DB;struct
_tuple12 _tmp1DC;struct Cyc_Dict_Dict _tmp1DD;struct Cyc_List_List*_tmp1DE;_LL139:
_tmp1D9=_tmp1D8.f1;if((_tmp1D9.PlaceL).tag != 1)goto _LL13B;_tmp1DA=(struct Cyc_CfFlowInfo_Place*)(
_tmp1D9.PlaceL).val;_tmp1DB=_tmp1D8.f2;if((_tmp1DB.ReachableFL).tag != 2)goto
_LL13B;_tmp1DC=(struct _tuple12)(_tmp1DB.ReachableFL).val;_tmp1DD=_tmp1DC.f1;
_tmp1DE=_tmp1DC.f2;_LL13A: _tmp1DE=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,
_tmp1DE,_tmp166);_tmp1D4=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp115,_tmp166->loc,_tmp1DD,env->all_changed,_tmp1DA,_tmp115->unknown_all),
_tmp1DE);goto _LL138;_LL13B:;_LL13C: goto _LL138;_LL138:;}{struct _tuple13 _tmp72C;
return(_tmp72C.f1=_tmp1D4,((_tmp72C.f2=_tmp115->unknown_all,_tmp72C)));};};}
_LLFA: {struct Cyc_Absyn_AssignOp_e_struct*_tmp167=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp134;if(_tmp167->tag != 4)goto _LLFC;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;
if(_tmp169 == 0)goto _LLFC;_tmp16A=*_tmp169;_tmp16B=_tmp167->f3;}}_LLFB: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp72F;void*_tmp72E;(_tmp72E=0,Cyc_Tcutil_terr(
e->loc,((_tmp72F="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp72F,sizeof(char),58))),_tag_dyneither(_tmp72E,sizeof(void*),0)));}{
struct _tuple13 _tmp730;return(_tmp730.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp730.f2=
_tmp115->unknown_all,_tmp730)));};}{struct Cyc_List_List _tmp731;struct Cyc_List_List
_tmp1E4=(_tmp731.hd=_tmp16B,((_tmp731.tl=0,_tmp731)));struct Cyc_List_List _tmp732;
struct Cyc_List_List _tmp1E5=(_tmp732.hd=_tmp168,((_tmp732.tl=(struct Cyc_List_List*)&
_tmp1E4,_tmp732)));union Cyc_CfFlowInfo_FlowInfo _tmp1E7;struct _tuple13 _tmp1E6=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1E5);_tmp1E7=_tmp1E6.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp1E9;struct _tuple14 _tmp1E8=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E7,0,_tmp168);
_tmp1E9=_tmp1E8.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1EA=_tmp1E7;struct _tuple12
_tmp1EB;struct Cyc_Dict_Dict _tmp1EC;struct Cyc_List_List*_tmp1ED;_LL13E: if((
_tmp1EA.ReachableFL).tag != 2)goto _LL140;_tmp1EB=(struct _tuple12)(_tmp1EA.ReachableFL).val;
_tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;_LL13F:{union Cyc_CfFlowInfo_AbsLVal _tmp1EE=
_tmp1E9;struct Cyc_CfFlowInfo_Place*_tmp1EF;int _tmp1F0;_LL143: if((_tmp1EE.PlaceL).tag
!= 1)goto _LL145;_tmp1EF=(struct Cyc_CfFlowInfo_Place*)(_tmp1EE.PlaceL).val;_LL144:
_tmp1ED=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,_tmp1ED,_tmp168);_tmp1EC=Cyc_CfFlowInfo_assign_place(
_tmp115,_tmp168->loc,_tmp1EC,env->all_changed,_tmp1EF,_tmp115->unknown_all);
_tmp1E7=Cyc_CfFlowInfo_ReachableFL(_tmp1EC,_tmp1ED);goto _LL142;_LL145: if((
_tmp1EE.UnknownL).tag != 2)goto _LL142;_tmp1F0=(int)(_tmp1EE.UnknownL).val;_LL146:
goto _LL142;_LL142:;}goto _LL13D;_LL140:;_LL141: goto _LL13D;_LL13D:;}{struct
_tuple13 _tmp733;return(_tmp733.f1=_tmp1E7,((_tmp733.f2=_tmp115->unknown_all,
_tmp733)));};};};_LLFC: {struct Cyc_Absyn_AssignOp_e_struct*_tmp16C=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp134;if(_tmp16C->tag != 4)goto _LLFE;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;
if(_tmp16E != 0)goto _LLFE;_tmp16F=_tmp16C->f3;}}_LLFD: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp736;void*_tmp735;(_tmp735=0,Cyc_Tcutil_terr(e->loc,((_tmp736="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp736,sizeof(char),58))),_tag_dyneither(_tmp735,sizeof(void*),0)));}{
struct _tuple13 _tmp737;return(_tmp737.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp737.f2=
_tmp115->unknown_all,_tmp737)));};}{struct Cyc_Dict_Dict*_tmp1F7=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp738;env->all_changed=((_tmp738=_region_malloc(
env->r,sizeof(*_tmp738)),((_tmp738[0]=_tmp115->mt_place_set,_tmp738))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp1FA;union Cyc_CfFlowInfo_AbsLVal _tmp1FB;struct _tuple14
_tmp1F9=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp16D);_tmp1FA=_tmp1F9.f1;
_tmp1FB=_tmp1F9.f2;{struct Cyc_Dict_Dict _tmp1FC=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp739;env->all_changed=((
_tmp739=_region_malloc(env->r,sizeof(*_tmp739)),((_tmp739[0]=_tmp115->mt_place_set,
_tmp739))));}{union Cyc_CfFlowInfo_FlowInfo _tmp1FF;void*_tmp200;struct _tuple13
_tmp1FE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp16F);_tmp1FF=_tmp1FE.f1;
_tmp200=_tmp1FE.f2;{struct Cyc_Dict_Dict _tmp201=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp202=Cyc_CfFlowInfo_after_flow(
_tmp115,(struct Cyc_Dict_Dict*)& _tmp1FC,_tmp1FA,_tmp1FF,_tmp1FC,_tmp201);union Cyc_CfFlowInfo_FlowInfo
_tmp203=Cyc_CfFlowInfo_join_flow(_tmp115,_tmp1F7,inflow,_tmp202);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp203,inflow)){if(_tmp1F7 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1F7,Cyc_CfFlowInfo_union_place_set(
_tmp1FC,_tmp201,0),0);}return Cyc_NewControlFlow_do_assign(_tmp115,env,_tmp202,
_tmp16D,_tmp1FB,_tmp16F,_tmp200,e->loc);}inflow=_tmp203;};};};};}};_LLFE: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp170=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp134;if(_tmp170->tag != 9)goto _LL100;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}}
_LLFF: {union Cyc_CfFlowInfo_FlowInfo _tmp205;void*_tmp206;struct _tuple13 _tmp204=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp171);_tmp205=_tmp204.f1;_tmp206=
_tmp204.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp205,_tmp172);}
_LL100: {struct Cyc_Absyn_Throw_e_struct*_tmp173=(struct Cyc_Absyn_Throw_e_struct*)
_tmp134;if(_tmp173->tag != 12)goto _LL102;else{_tmp174=_tmp173->f1;}}_LL101: {
union Cyc_CfFlowInfo_FlowInfo _tmp208;void*_tmp209;struct _tuple13 _tmp207=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp174);_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp174->loc,_tmp208,_tmp209);{struct _tuple13 _tmp73A;return(_tmp73A.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp73A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp115,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp115->unknown_all),_tmp73A)));};}_LL102: {struct Cyc_Absyn_FnCall_e_struct*
_tmp175=(struct Cyc_Absyn_FnCall_e_struct*)_tmp134;if(_tmp175->tag != 11)goto
_LL104;else{_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;}}_LL103: {struct
_RegionHandle*_tmp20B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp20E;struct Cyc_List_List*
_tmp20F;struct Cyc_List_List*_tmp73B;struct _tuple18 _tmp20D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp20B,env,inflow,((_tmp73B=_region_malloc(_tmp20B,sizeof(*_tmp73B)),((_tmp73B->hd=
_tmp176,((_tmp73B->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp20B,_tmp177),_tmp73B)))))),0,1);_tmp20E=_tmp20D.f1;
_tmp20F=_tmp20D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp210=Cyc_NewControlFlow_use_Rval(
env,_tmp176->loc,_tmp20E,(void*)((struct Cyc_List_List*)_check_null(_tmp20F))->hd);
_tmp20F=_tmp20F->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp211=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp176->topt))->v);struct Cyc_Absyn_PtrInfo _tmp213;void*_tmp214;
_LL148: {struct Cyc_Absyn_PointerType_struct*_tmp212=(struct Cyc_Absyn_PointerType_struct*)
_tmp211;if(_tmp212->tag != 5)goto _LL14A;else{_tmp213=_tmp212->f1;_tmp214=_tmp213.elt_typ;}}
_LL149:{void*_tmp215=Cyc_Tcutil_compress(_tmp214);struct Cyc_Absyn_FnInfo _tmp217;
struct Cyc_List_List*_tmp218;_LL14D: {struct Cyc_Absyn_FnType_struct*_tmp216=(
struct Cyc_Absyn_FnType_struct*)_tmp215;if(_tmp216->tag != 10)goto _LL14F;else{
_tmp217=_tmp216->f1;_tmp218=_tmp217.attributes;}}_LL14E: for(0;_tmp218 != 0;
_tmp218=_tmp218->tl){void*_tmp219=(void*)_tmp218->hd;int _tmp21B;int _tmp21D;
_LL152: {struct Cyc_Absyn_Initializes_att_struct*_tmp21A=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp219;if(_tmp21A->tag != 20)goto _LL154;else{_tmp21B=_tmp21A->f1;}}_LL153:{
struct Cyc_List_List*_tmp73C;init_params=((_tmp73C=_region_malloc(_tmp20B,sizeof(*
_tmp73C)),((_tmp73C->hd=(void*)_tmp21B,((_tmp73C->tl=init_params,_tmp73C))))));}
goto _LL151;_LL154: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp21C=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp219;if(_tmp21C->tag != 21)goto _LL156;else{_tmp21D=_tmp21C->f1;}}_LL155:{
struct Cyc_List_List*_tmp73D;nolive_unique_params=((_tmp73D=_region_malloc(
_tmp20B,sizeof(*_tmp73D)),((_tmp73D->hd=(void*)_tmp21D,((_tmp73D->tl=
nolive_unique_params,_tmp73D))))));}goto _LL151;_LL156:;_LL157: goto _LL151;_LL151:;}
goto _LL14C;_LL14F:;_LL150: {const char*_tmp740;void*_tmp73F;(_tmp73F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp740="anal_Rexp: bad function type",
_tag_dyneither(_tmp740,sizeof(char),29))),_tag_dyneither(_tmp73F,sizeof(void*),0)));}
_LL14C:;}goto _LL147;_LL14A:;_LL14B: {const char*_tmp743;void*_tmp742;(_tmp742=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp743="anal_Rexp: bad function type",_tag_dyneither(_tmp743,sizeof(char),29))),
_tag_dyneither(_tmp742,sizeof(void*),0)));}_LL147:;}{int i=1;for(0;_tmp20F != 0;(((
_tmp20F=_tmp20F->tl,_tmp177=((struct Cyc_List_List*)_check_null(_tmp177))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp210=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp177))->hd)->loc,_tmp210,(void*)_tmp20F->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp210=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp177))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp177))->hd)->topt))->v,_tmp210,(void*)_tmp20F->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp224=_tmp20E;int _tmp225;struct _tuple12 _tmp226;
struct Cyc_Dict_Dict _tmp227;_LL159: if((_tmp224.BottomFL).tag != 1)goto _LL15B;
_tmp225=(int)(_tmp224.BottomFL).val;_LL15A: goto _LL158;_LL15B: if((_tmp224.ReachableFL).tag
!= 2)goto _LL158;_tmp226=(struct _tuple12)(_tmp224.ReachableFL).val;_tmp227=
_tmp226.f1;_LL15C: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp227,(void*)_tmp20F->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp746;void*_tmp745;(_tmp745=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp177))->hd)->loc,((
_tmp746="expression may not be initialized",_tag_dyneither(_tmp746,sizeof(char),
34))),_tag_dyneither(_tmp745,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp22A=_tmp210;int _tmp22B;struct _tuple12 _tmp22C;struct Cyc_Dict_Dict _tmp22D;
struct Cyc_List_List*_tmp22E;_LL15E: if((_tmp22A.BottomFL).tag != 1)goto _LL160;
_tmp22B=(int)(_tmp22A.BottomFL).val;_LL15F: goto _LL15D;_LL160: if((_tmp22A.ReachableFL).tag
!= 2)goto _LL15D;_tmp22C=(struct _tuple12)(_tmp22A.ReachableFL).val;_tmp22D=
_tmp22C.f1;_tmp22E=_tmp22C.f2;_LL161: {struct Cyc_Dict_Dict _tmp22F=Cyc_CfFlowInfo_escape_deref(
_tmp115,_tmp22D,env->all_changed,(void*)_tmp20F->hd);{void*_tmp230=(void*)
_tmp20F->hd;struct Cyc_CfFlowInfo_Place*_tmp232;_LL163: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp231=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp230;if(_tmp231->tag != 5)goto
_LL165;else{_tmp232=_tmp231->f1;}}_LL164:{void*_tmp233=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp177))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp235;void*
_tmp236;_LL168: {struct Cyc_Absyn_PointerType_struct*_tmp234=(struct Cyc_Absyn_PointerType_struct*)
_tmp233;if(_tmp234->tag != 5)goto _LL16A;else{_tmp235=_tmp234->f1;_tmp236=_tmp235.elt_typ;}}
_LL169: _tmp22F=Cyc_CfFlowInfo_assign_place(_tmp115,((struct Cyc_Absyn_Exp*)
_tmp177->hd)->loc,_tmp22F,env->all_changed,_tmp232,Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,_tmp236,_tmp115->esc_all));goto _LL167;_LL16A:;_LL16B: {const char*_tmp749;
void*_tmp748;(_tmp748=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp749="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp749,sizeof(char),39))),_tag_dyneither(_tmp748,sizeof(void*),0)));}
_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}_tmp210=Cyc_CfFlowInfo_ReachableFL(
_tmp22F,_tmp22E);goto _LL15D;}_LL15D:;}goto _LL158;_LL158:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp176->topt))->v)){struct _tuple13
_tmp74A;return(_tmp74A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp74A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp115->unknown_all),
_tmp74A)));}else{struct _tuple13 _tmp74B;return(_tmp74B.f1=_tmp210,((_tmp74B.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp115->unknown_all),_tmp74B)));}};};}_LL104: {struct Cyc_Absyn_Malloc_e_struct*
_tmp178=(struct Cyc_Absyn_Malloc_e_struct*)_tmp134;if(_tmp178->tag != 34)goto
_LL106;else{_tmp179=_tmp178->f1;_tmp17A=_tmp179.is_calloc;_tmp17B=_tmp179.rgn;
_tmp17C=_tmp179.elt_type;_tmp17D=_tmp179.num_elts;_tmp17E=_tmp179.fat_result;}}
_LL105: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp74E;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp74D;void*root=(void*)((_tmp74D=_region_malloc(_tmp115->r,sizeof(*_tmp74D)),((
_tmp74D[0]=((_tmp74E.tag=1,((_tmp74E.f1=_tmp17D,((_tmp74E.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp74E)))))),_tmp74D))));struct
Cyc_CfFlowInfo_Place*_tmp74F;struct Cyc_CfFlowInfo_Place*place=(_tmp74F=
_region_malloc(_tmp115->r,sizeof(*_tmp74F)),((_tmp74F->root=root,((_tmp74F->fields=
0,_tmp74F)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp752;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp751;void*rval=(void*)((_tmp751=_region_malloc(_tmp115->r,sizeof(*_tmp751)),((
_tmp751[0]=((_tmp752.tag=5,((_tmp752.f1=place,_tmp752)))),_tmp751))));void*
place_val=_tmp17E?_tmp115->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp115,*((
void**)_check_null(_tmp17C)),_tmp115->unknown_none);union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp17B != 0){struct _RegionHandle*_tmp23B=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp23E;struct Cyc_List_List*_tmp23F;struct Cyc_Absyn_Exp*_tmp753[2];struct
_tuple18 _tmp23D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp23B,env,inflow,((
_tmp753[1]=_tmp17D,((_tmp753[0]=(struct Cyc_Absyn_Exp*)_tmp17B,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp23B,
_tag_dyneither(_tmp753,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp23E=_tmp23D.f1;
_tmp23F=_tmp23D.f2;outflow=_tmp23E;}else{union Cyc_CfFlowInfo_FlowInfo _tmp241;
struct _tuple13 _tmp240=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17D);_tmp241=
_tmp240.f1;outflow=_tmp241;}{union Cyc_CfFlowInfo_FlowInfo _tmp242=outflow;int
_tmp243;struct _tuple12 _tmp244;struct Cyc_Dict_Dict _tmp245;struct Cyc_List_List*
_tmp246;_LL16D: if((_tmp242.BottomFL).tag != 1)goto _LL16F;_tmp243=(int)(_tmp242.BottomFL).val;
_LL16E: {struct _tuple13 _tmp754;return(_tmp754.f1=outflow,((_tmp754.f2=rval,
_tmp754)));}_LL16F: if((_tmp242.ReachableFL).tag != 2)goto _LL16C;_tmp244=(struct
_tuple12)(_tmp242.ReachableFL).val;_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;_LL170: {
struct _tuple13 _tmp755;return(_tmp755.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp245,root,place_val),
_tmp246),((_tmp755.f2=rval,_tmp755)));}_LL16C:;};}_LL106: {struct Cyc_Absyn_Swap_e_struct*
_tmp17F=(struct Cyc_Absyn_Swap_e_struct*)_tmp134;if(_tmp17F->tag != 35)goto _LL108;
else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LL107: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp24E=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp251;struct Cyc_List_List*_tmp252;struct Cyc_Absyn_Exp*
_tmp756[2];struct _tuple18 _tmp250=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp24E,
env,inflow,((_tmp756[1]=_tmp181,((_tmp756[0]=_tmp180,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp24E,
_tag_dyneither(_tmp756,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp251=_tmp250.f1;
_tmp252=_tmp250.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp252))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp252->tl))->hd;outflow=
_tmp251;}{union Cyc_CfFlowInfo_AbsLVal _tmp254;struct _tuple14 _tmp253=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp180);_tmp254=_tmp253.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp256;
struct _tuple14 _tmp255=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp181);
_tmp256=_tmp255.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp257=outflow;struct _tuple12
_tmp258;struct Cyc_Dict_Dict _tmp259;struct Cyc_List_List*_tmp25A;_LL172: if((
_tmp257.ReachableFL).tag != 2)goto _LL174;_tmp258=(struct _tuple12)(_tmp257.ReachableFL).val;
_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;_LL173:{union Cyc_CfFlowInfo_AbsLVal _tmp25B=
_tmp254;struct Cyc_CfFlowInfo_Place*_tmp25C;int _tmp25D;_LL177: if((_tmp25B.PlaceL).tag
!= 1)goto _LL179;_tmp25C=(struct Cyc_CfFlowInfo_Place*)(_tmp25B.PlaceL).val;_LL178:
_tmp259=Cyc_CfFlowInfo_assign_place(_tmp115,_tmp180->loc,_tmp259,env->all_changed,
_tmp25C,right_rval);goto _LL176;_LL179: if((_tmp25B.UnknownL).tag != 2)goto _LL176;
_tmp25D=(int)(_tmp25B.UnknownL).val;_LL17A: goto _LL176;_LL176:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp25E=_tmp256;struct Cyc_CfFlowInfo_Place*_tmp25F;int _tmp260;_LL17C: if((_tmp25E.PlaceL).tag
!= 1)goto _LL17E;_tmp25F=(struct Cyc_CfFlowInfo_Place*)(_tmp25E.PlaceL).val;_LL17D:
_tmp259=Cyc_CfFlowInfo_assign_place(_tmp115,_tmp181->loc,_tmp259,env->all_changed,
_tmp25F,left_rval);goto _LL17B;_LL17E: if((_tmp25E.UnknownL).tag != 2)goto _LL17B;
_tmp260=(int)(_tmp25E.UnknownL).val;_LL17F: goto _LL17B;_LL17B:;}_tmp25A=Cyc_CfFlowInfo_reln_kill_exp(
_tmp115->r,_tmp25A,_tmp180);_tmp25A=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,
_tmp25A,_tmp181);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp259,_tmp25A);goto _LL171;
_LL174:;_LL175: goto _LL171;_LL171:;}{struct _tuple13 _tmp757;return(_tmp757.f1=
outflow,((_tmp757.f2=_tmp115->unknown_all,_tmp757)));};};};}_LL108: {struct Cyc_Absyn_New_e_struct*
_tmp182=(struct Cyc_Absyn_New_e_struct*)_tmp134;if(_tmp182->tag != 17)goto _LL10A;
else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL109: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp75A;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp759;void*root=(void*)((_tmp759=
_region_malloc(_tmp115->r,sizeof(*_tmp759)),((_tmp759[0]=((_tmp75A.tag=1,((
_tmp75A.f1=_tmp184,((_tmp75A.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp75A)))))),_tmp759))));struct Cyc_CfFlowInfo_Place*_tmp75B;
struct Cyc_CfFlowInfo_Place*place=(_tmp75B=_region_malloc(_tmp115->r,sizeof(*
_tmp75B)),((_tmp75B->root=root,((_tmp75B->fields=0,_tmp75B)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp75E;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp75D;void*rval=(void*)((_tmp75D=
_region_malloc(_tmp115->r,sizeof(*_tmp75D)),((_tmp75D[0]=((_tmp75E.tag=5,((
_tmp75E.f1=place,_tmp75E)))),_tmp75D))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp183 != 0){struct _RegionHandle*_tmp262=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp265;struct Cyc_List_List*_tmp266;struct Cyc_Absyn_Exp*_tmp75F[2];struct
_tuple18 _tmp264=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp262,env,inflow,((
_tmp75F[1]=_tmp184,((_tmp75F[0]=(struct Cyc_Absyn_Exp*)_tmp183,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp262,
_tag_dyneither(_tmp75F,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp265=_tmp264.f1;
_tmp266=_tmp264.f2;outflow=_tmp265;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp266))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp268;void*_tmp269;struct _tuple13 _tmp267=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp184);_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;outflow=_tmp268;
place_val=_tmp269;}{union Cyc_CfFlowInfo_FlowInfo _tmp26A=outflow;int _tmp26B;
struct _tuple12 _tmp26C;struct Cyc_Dict_Dict _tmp26D;struct Cyc_List_List*_tmp26E;
_LL181: if((_tmp26A.BottomFL).tag != 1)goto _LL183;_tmp26B=(int)(_tmp26A.BottomFL).val;
_LL182: {struct _tuple13 _tmp760;return(_tmp760.f1=outflow,((_tmp760.f2=rval,
_tmp760)));}_LL183: if((_tmp26A.ReachableFL).tag != 2)goto _LL180;_tmp26C=(struct
_tuple12)(_tmp26A.ReachableFL).val;_tmp26D=_tmp26C.f1;_tmp26E=_tmp26C.f2;_LL184: {
struct _tuple13 _tmp761;return(_tmp761.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp26D,root,place_val),
_tmp26E),((_tmp761.f2=rval,_tmp761)));}_LL180:;};};}_LL10A: {struct Cyc_Absyn_Address_e_struct*
_tmp185=(struct Cyc_Absyn_Address_e_struct*)_tmp134;if(_tmp185->tag != 16)goto
_LL10C;else{_tmp186=_tmp185->f1;}}_LL10B: {union Cyc_CfFlowInfo_FlowInfo _tmp277;
union Cyc_CfFlowInfo_AbsLVal _tmp278;struct _tuple14 _tmp276=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp186);_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp279=_tmp278;int _tmp27A;struct Cyc_CfFlowInfo_Place*_tmp27B;_LL186: if((_tmp279.UnknownL).tag
!= 2)goto _LL188;_tmp27A=(int)(_tmp279.UnknownL).val;_LL187: {struct _tuple13
_tmp762;return(_tmp762.f1=_tmp277,((_tmp762.f2=_tmp115->notzeroall,_tmp762)));}
_LL188: if((_tmp279.PlaceL).tag != 1)goto _LL185;_tmp27B=(struct Cyc_CfFlowInfo_Place*)(
_tmp279.PlaceL).val;_LL189: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp768;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp767;struct _tuple13 _tmp766;return(
_tmp766.f1=_tmp277,((_tmp766.f2=(void*)((_tmp768=_region_malloc(env->r,sizeof(*
_tmp768)),((_tmp768[0]=((_tmp767.tag=5,((_tmp767.f1=_tmp27B,_tmp767)))),_tmp768)))),
_tmp766)));}_LL185:;};}_LL10C: {struct Cyc_Absyn_Deref_e_struct*_tmp187=(struct
Cyc_Absyn_Deref_e_struct*)_tmp134;if(_tmp187->tag != 21)goto _LL10E;else{_tmp188=
_tmp187->f1;}}_LL10D: {union Cyc_CfFlowInfo_FlowInfo _tmp281;void*_tmp282;struct
_tuple13 _tmp280=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp188);_tmp281=
_tmp280.f1;_tmp282=_tmp280.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp281,_tmp188,_tmp282);}_LL10E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp189=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp134;if(_tmp189->tag != 22)goto _LL110;
else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LL10F: {union Cyc_CfFlowInfo_FlowInfo
_tmp284;void*_tmp285;struct _tuple13 _tmp283=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp18A);_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp18A->topt))->v)){struct _tuple13
_tmp769;return(_tmp769.f1=_tmp284,((_tmp769.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp115->unknown_all),
_tmp769)));}{void*_tmp287=_tmp285;int _tmp289;struct _dyneither_ptr _tmp28A;_LL18B: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp288=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp287;if(_tmp288->tag != 6)goto _LL18D;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}
_LL18C: {int _tmp28B=Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp18A->topt))->v,_tmp18B);struct _tuple13 _tmp76A;return(_tmp76A.f1=
_tmp284,((_tmp76A.f2=*((void**)_check_dyneither_subscript(_tmp28A,sizeof(void*),
_tmp28B)),_tmp76A)));}_LL18D:;_LL18E: {struct Cyc_Core_Impossible_struct _tmp777;
const char*_tmp776;void*_tmp775[1];struct Cyc_String_pa_struct _tmp774;struct Cyc_Core_Impossible_struct*
_tmp773;(int)_throw((void*)((_tmp773=_cycalloc(sizeof(*_tmp773)),((_tmp773[0]=((
_tmp777.tag=Cyc_Core_Impossible,((_tmp777.f1=(struct _dyneither_ptr)((_tmp774.tag=
0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp775[0]=& _tmp774,Cyc_aprintf(((_tmp776="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp776,sizeof(char),26))),_tag_dyneither(_tmp775,sizeof(void*),1)))))))),
_tmp777)))),_tmp773)))));}_LL18A:;};}_LL110: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp18C=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp134;if(_tmp18C->tag != 38)goto
_LL112;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL111: {union Cyc_CfFlowInfo_FlowInfo
_tmp293;void*_tmp294;struct _tuple13 _tmp292=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp18D);_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;{struct _tuple13 _tmp778;
return(_tmp778.f1=_tmp293,((_tmp778.f2=_tmp115->unknown_all,_tmp778)));};}_LL112: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp18F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp134;if(_tmp18F->tag != 23)goto _LL114;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}
_LL113: {union Cyc_CfFlowInfo_FlowInfo _tmp297;void*_tmp298;struct _tuple13 _tmp296=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp190);_tmp297=_tmp296.f1;_tmp298=
_tmp296.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp29A;void*_tmp29B;struct _tuple13
_tmp299=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp297,_tmp190,_tmp298);
_tmp29A=_tmp299.f1;_tmp29B=_tmp299.f2;{void*_tmp29C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp190->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp29E;void*_tmp29F;_LL190: {struct Cyc_Absyn_PointerType_struct*_tmp29D=(struct
Cyc_Absyn_PointerType_struct*)_tmp29C;if(_tmp29D->tag != 5)goto _LL192;else{
_tmp29E=_tmp29D->f1;_tmp29F=_tmp29E.elt_typ;}}_LL191: if(Cyc_Absyn_is_nontagged_union_type(
_tmp29F)){struct _tuple13 _tmp779;return(_tmp779.f1=_tmp29A,((_tmp779.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp115->unknown_all),
_tmp779)));}{void*_tmp2A1=_tmp29B;int _tmp2A3;struct _dyneither_ptr _tmp2A4;_LL195: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2A2=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2A1;if(_tmp2A2->tag != 6)goto _LL197;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;}}
_LL196: {int _tmp2A5=Cyc_CfFlowInfo_get_field_index(_tmp29F,_tmp191);struct
_tuple13 _tmp77A;return(_tmp77A.f1=_tmp29A,((_tmp77A.f2=*((void**)
_check_dyneither_subscript(_tmp2A4,sizeof(void*),_tmp2A5)),_tmp77A)));}_LL197:;
_LL198: {struct Cyc_Core_Impossible_struct _tmp780;const char*_tmp77F;struct Cyc_Core_Impossible_struct*
_tmp77E;(int)_throw((void*)((_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E[0]=((
_tmp780.tag=Cyc_Core_Impossible,((_tmp780.f1=((_tmp77F="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp77F,sizeof(char),21))),_tmp780)))),_tmp77E)))));}_LL194:;};
_LL192:;_LL193: {struct Cyc_Core_Impossible_struct _tmp786;const char*_tmp785;
struct Cyc_Core_Impossible_struct*_tmp784;(int)_throw((void*)((_tmp784=_cycalloc(
sizeof(*_tmp784)),((_tmp784[0]=((_tmp786.tag=Cyc_Core_Impossible,((_tmp786.f1=((
_tmp785="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp785,sizeof(char),25))),
_tmp786)))),_tmp784)))));}_LL18F:;};};}_LL114: {struct Cyc_Absyn_Conditional_e_struct*
_tmp192=(struct Cyc_Absyn_Conditional_e_struct*)_tmp134;if(_tmp192->tag != 6)goto
_LL116;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;_tmp195=_tmp192->f3;}}_LL115: {
union Cyc_CfFlowInfo_FlowInfo _tmp2AE;union Cyc_CfFlowInfo_FlowInfo _tmp2AF;struct
_tuple15 _tmp2AD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp193);_tmp2AE=_tmp2AD.f1;
_tmp2AF=_tmp2AD.f2;{struct _tuple13 _tmp2B0=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2AE,_tmp194);struct _tuple13 _tmp2B1=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp2AF,_tmp195);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp115,
env->all_changed,_tmp2B0,_tmp2B1);};}_LL116: {struct Cyc_Absyn_And_e_struct*
_tmp196=(struct Cyc_Absyn_And_e_struct*)_tmp134;if(_tmp196->tag != 7)goto _LL118;
else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LL117: {union Cyc_CfFlowInfo_FlowInfo
_tmp2B3;union Cyc_CfFlowInfo_FlowInfo _tmp2B4;struct _tuple15 _tmp2B2=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp197);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp2B6;void*_tmp2B7;struct _tuple13 _tmp2B5=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2B3,_tmp198);_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;{struct _tuple13
_tmp787;struct _tuple13 _tmp2B8=(_tmp787.f1=_tmp2B6,((_tmp787.f2=_tmp2B7,_tmp787)));
struct _tuple13 _tmp788;struct _tuple13 _tmp2B9=(_tmp788.f1=_tmp2B4,((_tmp788.f2=
_tmp115->zero,_tmp788)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp115,env->all_changed,
_tmp2B8,_tmp2B9);};};}_LL118: {struct Cyc_Absyn_Or_e_struct*_tmp199=(struct Cyc_Absyn_Or_e_struct*)
_tmp134;if(_tmp199->tag != 8)goto _LL11A;else{_tmp19A=_tmp199->f1;_tmp19B=_tmp199->f2;}}
_LL119: {union Cyc_CfFlowInfo_FlowInfo _tmp2BD;union Cyc_CfFlowInfo_FlowInfo _tmp2BE;
struct _tuple15 _tmp2BC=Cyc_NewControlFlow_anal_test(env,inflow,_tmp19A);_tmp2BD=
_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2C0;void*_tmp2C1;
struct _tuple13 _tmp2BF=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2BE,_tmp19B);
_tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;{struct _tuple13 _tmp789;struct _tuple13
_tmp2C2=(_tmp789.f1=_tmp2C0,((_tmp789.f2=_tmp2C1,_tmp789)));struct _tuple13
_tmp78A;struct _tuple13 _tmp2C3=(_tmp78A.f1=_tmp2BD,((_tmp78A.f2=_tmp115->notzeroall,
_tmp78A)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp115,env->all_changed,
_tmp2C2,_tmp2C3);};};}_LL11A: {struct Cyc_Absyn_Subscript_e_struct*_tmp19C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp134;if(_tmp19C->tag != 24)goto _LL11C;
else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}_LL11B: {struct _RegionHandle*
_tmp2C6=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2C9;struct Cyc_List_List*_tmp2CA;
struct Cyc_Absyn_Exp*_tmp78B[2];struct _tuple18 _tmp2C8=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2C6,env,inflow,((_tmp78B[1]=_tmp19E,((_tmp78B[0]=_tmp19D,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2C6,
_tag_dyneither(_tmp78B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2C9=_tmp2C8.f1;
_tmp2CA=_tmp2C8.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2CB=_tmp2C9;{union Cyc_CfFlowInfo_FlowInfo
_tmp2CC=_tmp2C9;struct _tuple12 _tmp2CD;struct Cyc_Dict_Dict _tmp2CE;struct Cyc_List_List*
_tmp2CF;_LL19A: if((_tmp2CC.ReachableFL).tag != 2)goto _LL19C;_tmp2CD=(struct
_tuple12)(_tmp2CC.ReachableFL).val;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;_LL19B:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2CE,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2CA))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(_tmp19E->loc,((_tmp78E="expression may not be initialized",
_tag_dyneither(_tmp78E,sizeof(char),34))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2D2=Cyc_NewControlFlow_add_subscript_reln(_tmp115->r,
_tmp2CF,_tmp19D,_tmp19E);if(_tmp2CF != _tmp2D2)_tmp2CB=Cyc_CfFlowInfo_ReachableFL(
_tmp2CE,_tmp2D2);goto _LL199;};_LL19C:;_LL19D: goto _LL199;_LL199:;}{void*_tmp2D3=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19D->topt))->v);
struct Cyc_List_List*_tmp2D5;struct Cyc_Absyn_PtrInfo _tmp2D7;struct Cyc_Absyn_PtrAtts
_tmp2D8;union Cyc_Absyn_Constraint*_tmp2D9;_LL19F: {struct Cyc_Absyn_TupleType_struct*
_tmp2D4=(struct Cyc_Absyn_TupleType_struct*)_tmp2D3;if(_tmp2D4->tag != 11)goto
_LL1A1;else{_tmp2D5=_tmp2D4->f1;}}_LL1A0: {void*_tmp2DA=(void*)((struct Cyc_List_List*)
_check_null(_tmp2CA))->hd;int _tmp2DC;struct _dyneither_ptr _tmp2DD;_LL1A6: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp2DB=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2DA;if(_tmp2DB->tag != 6)goto _LL1A8;else{_tmp2DC=_tmp2DB->f1;_tmp2DD=_tmp2DB->f2;}}
_LL1A7: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp19E)).f1;struct
_tuple13 _tmp78F;return(_tmp78F.f1=_tmp2CB,((_tmp78F.f2=*((void**)
_check_dyneither_subscript(_tmp2DD,sizeof(void*),(int)i)),_tmp78F)));}_LL1A8:;
_LL1A9: {struct Cyc_Core_Impossible_struct _tmp795;const char*_tmp794;struct Cyc_Core_Impossible_struct*
_tmp793;(int)_throw((void*)((_tmp793=_cycalloc(sizeof(*_tmp793)),((_tmp793[0]=((
_tmp795.tag=Cyc_Core_Impossible,((_tmp795.f1=((_tmp794="anal_Rexp: Subscript",
_tag_dyneither(_tmp794,sizeof(char),21))),_tmp795)))),_tmp793)))));}_LL1A5:;}
_LL1A1: {struct Cyc_Absyn_PointerType_struct*_tmp2D6=(struct Cyc_Absyn_PointerType_struct*)
_tmp2D3;if(_tmp2D6->tag != 5)goto _LL1A3;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.ptr_atts;
_tmp2D9=_tmp2D8.bounds;}}_LL1A2: {union Cyc_CfFlowInfo_FlowInfo _tmp2E3;void*
_tmp2E4;struct _tuple13 _tmp2E2=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C9,
_tmp19D,(void*)((struct Cyc_List_List*)_check_null(_tmp2CA))->hd);_tmp2E3=_tmp2E2.f1;
_tmp2E4=_tmp2E2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2E5=_tmp2E3;int _tmp2E6;
_LL1AB: if((_tmp2E5.BottomFL).tag != 1)goto _LL1AD;_tmp2E6=(int)(_tmp2E5.BottomFL).val;
_LL1AC: {struct _tuple13 _tmp796;return(_tmp796.f1=_tmp2E3,((_tmp796.f2=_tmp2E4,
_tmp796)));}_LL1AD:;_LL1AE: {struct _tuple13 _tmp797;return(_tmp797.f1=_tmp2CB,((
_tmp797.f2=_tmp2E4,_tmp797)));}_LL1AA:;};}_LL1A3:;_LL1A4: {struct Cyc_Core_Impossible_struct
_tmp79D;const char*_tmp79C;struct Cyc_Core_Impossible_struct*_tmp79B;(int)_throw((
void*)((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B[0]=((_tmp79D.tag=Cyc_Core_Impossible,((
_tmp79D.f1=((_tmp79C="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp79C,
sizeof(char),33))),_tmp79D)))),_tmp79B)))));}_LL19E:;};};}_LL11C: {struct Cyc_Absyn_Datatype_e_struct*
_tmp19F=(struct Cyc_Absyn_Datatype_e_struct*)_tmp134;if(_tmp19F->tag != 31)goto
_LL11E;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL11D: _tmp1A3=_tmp1A0;
goto _LL11F;_LL11E: {struct Cyc_Absyn_Tuple_e_struct*_tmp1A2=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp134;if(_tmp1A2->tag != 25)goto _LL120;else{_tmp1A3=_tmp1A2->f1;}}_LL11F: {
struct _RegionHandle*_tmp2EC=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2EE;struct Cyc_List_List*
_tmp2EF;struct _tuple18 _tmp2ED=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2EC,
env,inflow,_tmp1A3,1,1);_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{unsigned int
_tmp7A3;unsigned int _tmp7A2;struct _dyneither_ptr _tmp7A1;void**_tmp7A0;
unsigned int _tmp79F;struct _dyneither_ptr aggrdict=(_tmp79F=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1A3),((_tmp7A0=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp79F)),((_tmp7A1=
_tag_dyneither(_tmp7A0,sizeof(void*),_tmp79F),((((_tmp7A2=_tmp79F,_tmp7A4(&
_tmp7A3,& _tmp7A2,& _tmp7A0,& _tmp2EF))),_tmp7A1)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp7AA;struct Cyc_CfFlowInfo_Aggregate_struct _tmp7A9;struct _tuple13 _tmp7A8;
return(_tmp7A8.f1=_tmp2EE,((_tmp7A8.f2=(void*)((_tmp7AA=_region_malloc(env->r,
sizeof(*_tmp7AA)),((_tmp7AA[0]=((_tmp7A9.tag=6,((_tmp7A9.f1=0,((_tmp7A9.f2=
aggrdict,_tmp7A9)))))),_tmp7AA)))),_tmp7A8)));};}_LL120: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1A4=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp134;if(_tmp1A4->tag != 30)goto
_LL122;else{_tmp1A5=_tmp1A4->f2;}}_LL121: {struct Cyc_List_List*fs;{void*_tmp2F8=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp2FA;_LL1B0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2F9=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2F8;if(_tmp2F9->tag != 13)goto _LL1B2;
else{_tmp2FA=_tmp2F9->f2;}}_LL1B1: fs=_tmp2FA;goto _LL1AF;_LL1B2:;_LL1B3: {struct
Cyc_Core_Impossible_struct _tmp7B0;const char*_tmp7AF;struct Cyc_Core_Impossible_struct*
_tmp7AE;(int)_throw((void*)((_tmp7AE=_cycalloc(sizeof(*_tmp7AE)),((_tmp7AE[0]=((
_tmp7B0.tag=Cyc_Core_Impossible,((_tmp7B0.f1=((_tmp7AF="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp7AF,sizeof(char),35))),_tmp7B0)))),_tmp7AE)))));}_LL1AF:;}
_tmp1A7=_tmp1A5;_tmp1AA=Cyc_Absyn_StructA;_tmp1AD=fs;goto _LL123;}_LL122: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1A6=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp134;if(_tmp1A6->tag != 29)goto _LL124;else{_tmp1A7=_tmp1A6->f3;_tmp1A8=_tmp1A6->f4;
if(_tmp1A8 == 0)goto _LL124;_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.kind;_tmp1AB=_tmp1A9.impl;
if(_tmp1AB == 0)goto _LL124;_tmp1AC=*_tmp1AB;_tmp1AD=_tmp1AC.fields;}}_LL123: {
struct _RegionHandle*_tmp2FE=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp300;struct Cyc_List_List*
_tmp301;struct _tuple18 _tmp2FF=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2FE,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2FE,(struct Cyc_Absyn_Exp*(*)(
struct _tuple20*))Cyc_Core_snd,_tmp1A7),1,1);_tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp115,
_tmp1AD,_tmp115->unknown_all);{int i=0;for(0;_tmp301 != 0;(((_tmp301=_tmp301->tl,
_tmp1A7=_tmp1A7->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple20*)((struct
Cyc_List_List*)_check_null(_tmp1A7))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp302=(void*)ds->hd;struct _dyneither_ptr*_tmp305;_LL1B5: {struct Cyc_Absyn_ArrayElement_struct*
_tmp303=(struct Cyc_Absyn_ArrayElement_struct*)_tmp302;if(_tmp303->tag != 0)goto
_LL1B7;}_LL1B6: {struct Cyc_Core_Impossible_struct _tmp7B6;const char*_tmp7B5;
struct Cyc_Core_Impossible_struct*_tmp7B4;(int)_throw((void*)((_tmp7B4=_cycalloc(
sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B6.tag=Cyc_Core_Impossible,((_tmp7B6.f1=((
_tmp7B5="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7B5,sizeof(char),22))),
_tmp7B6)))),_tmp7B4)))));}_LL1B7: {struct Cyc_Absyn_FieldName_struct*_tmp304=(
struct Cyc_Absyn_FieldName_struct*)_tmp302;if(_tmp304->tag != 1)goto _LL1B4;else{
_tmp305=_tmp304->f1;}}_LL1B8: {int _tmp309=Cyc_CfFlowInfo_get_field_index_fs(
_tmp1AD,_tmp305);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp309))=(void*)_tmp301->hd;if(_tmp1AA == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp30A=(*((struct _tuple20*)_tmp1A7->hd)).f2;_tmp300=Cyc_NewControlFlow_use_Rval(
env,_tmp30A->loc,_tmp300,(void*)_tmp301->hd);}}_LL1B4:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp7BC;struct Cyc_CfFlowInfo_Aggregate_struct _tmp7BB;struct _tuple13 _tmp7BA;
return(_tmp7BA.f1=_tmp300,((_tmp7BA.f2=(void*)((_tmp7BC=_region_malloc(env->r,
sizeof(*_tmp7BC)),((_tmp7BC[0]=((_tmp7BB.tag=6,((_tmp7BB.f1=_tmp1AA == Cyc_Absyn_UnionA,((
_tmp7BB.f2=aggrdict,_tmp7BB)))))),_tmp7BC)))),_tmp7BA)));};};}_LL124: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1AE=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp134;if(_tmp1AE->tag != 29)goto
_LL126;}_LL125: {struct Cyc_Core_Impossible_struct _tmp7C2;const char*_tmp7C1;
struct Cyc_Core_Impossible_struct*_tmp7C0;(int)_throw((void*)((_tmp7C0=_cycalloc(
sizeof(*_tmp7C0)),((_tmp7C0[0]=((_tmp7C2.tag=Cyc_Core_Impossible,((_tmp7C2.f1=((
_tmp7C1="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7C1,sizeof(char),31))),
_tmp7C2)))),_tmp7C0)))));}_LL126: {struct Cyc_Absyn_Array_e_struct*_tmp1AF=(
struct Cyc_Absyn_Array_e_struct*)_tmp134;if(_tmp1AF->tag != 27)goto _LL128;else{
_tmp1B0=_tmp1AF->f1;}}_LL127: {struct _RegionHandle*_tmp311=env->r;struct Cyc_List_List*
_tmp312=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple20*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp311,(struct Cyc_Absyn_Exp*(*)(
struct _tuple20*))Cyc_Core_snd,_tmp1B0);union Cyc_CfFlowInfo_FlowInfo _tmp314;
struct Cyc_List_List*_tmp315;struct _tuple18 _tmp313=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp311,env,inflow,_tmp312,1,1);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;for(0;
_tmp315 != 0;(_tmp315=_tmp315->tl,_tmp312=_tmp312->tl)){_tmp314=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp312))->hd)->loc,
_tmp314,(void*)_tmp315->hd);}{struct _tuple13 _tmp7C3;return(_tmp7C3.f1=_tmp314,((
_tmp7C3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp115,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp115->unknown_all),_tmp7C3)));};}_LL128: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1B1=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp134;if(_tmp1B1->tag != 28)
goto _LL12A;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;_tmp1B4=_tmp1B1->f3;
_tmp1B5=_tmp1B1->f4;}}_LL129: {union Cyc_CfFlowInfo_FlowInfo _tmp318;void*_tmp319;
struct _tuple13 _tmp317=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1B3);_tmp318=
_tmp317.f1;_tmp319=_tmp317.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp31A=_tmp318;int
_tmp31B;struct _tuple12 _tmp31C;struct Cyc_Dict_Dict _tmp31D;struct Cyc_List_List*
_tmp31E;_LL1BA: if((_tmp31A.BottomFL).tag != 1)goto _LL1BC;_tmp31B=(int)(_tmp31A.BottomFL).val;
_LL1BB: {struct _tuple13 _tmp7C4;return(_tmp7C4.f1=_tmp318,((_tmp7C4.f2=_tmp115->unknown_all,
_tmp7C4)));}_LL1BC: if((_tmp31A.ReachableFL).tag != 2)goto _LL1B9;_tmp31C=(struct
_tuple12)(_tmp31A.ReachableFL).val;_tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;_LL1BD:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp31D,_tmp319)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_terr(_tmp1B3->loc,((_tmp7C7="expression may not be initialized",
_tag_dyneither(_tmp7C7,sizeof(char),34))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp31E;comp_loop: {void*_tmp322=_tmp1B3->r;struct
Cyc_Absyn_Exp*_tmp324;void*_tmp326;struct Cyc_Absyn_Vardecl*_tmp328;void*_tmp32A;
struct Cyc_Absyn_Vardecl*_tmp32C;void*_tmp32E;struct Cyc_Absyn_Vardecl*_tmp330;
void*_tmp332;struct Cyc_Absyn_Vardecl*_tmp334;union Cyc_Absyn_Cnst _tmp336;struct
_tuple6 _tmp337;int _tmp338;enum Cyc_Absyn_Primop _tmp33A;struct Cyc_List_List*
_tmp33B;struct Cyc_List_List _tmp33C;struct Cyc_Absyn_Exp*_tmp33D;_LL1BF: {struct
Cyc_Absyn_Cast_e_struct*_tmp323=(struct Cyc_Absyn_Cast_e_struct*)_tmp322;if(
_tmp323->tag != 15)goto _LL1C1;else{_tmp324=_tmp323->f2;}}_LL1C0: _tmp1B3=_tmp324;
goto comp_loop;_LL1C1:{struct Cyc_Absyn_Var_e_struct*_tmp325=(struct Cyc_Absyn_Var_e_struct*)
_tmp322;if(_tmp325->tag != 1)goto _LL1C3;else{_tmp326=(void*)_tmp325->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp327=(struct Cyc_Absyn_Global_b_struct*)_tmp326;if(
_tmp327->tag != 1)goto _LL1C3;else{_tmp328=_tmp327->f1;}};}}if(!(!_tmp328->escapes))
goto _LL1C3;_LL1C2: _tmp32C=_tmp328;goto _LL1C4;_LL1C3:{struct Cyc_Absyn_Var_e_struct*
_tmp329=(struct Cyc_Absyn_Var_e_struct*)_tmp322;if(_tmp329->tag != 1)goto _LL1C5;
else{_tmp32A=(void*)_tmp329->f2;{struct Cyc_Absyn_Local_b_struct*_tmp32B=(struct
Cyc_Absyn_Local_b_struct*)_tmp32A;if(_tmp32B->tag != 4)goto _LL1C5;else{_tmp32C=
_tmp32B->f1;}};}}if(!(!_tmp32C->escapes))goto _LL1C5;_LL1C4: _tmp330=_tmp32C;goto
_LL1C6;_LL1C5:{struct Cyc_Absyn_Var_e_struct*_tmp32D=(struct Cyc_Absyn_Var_e_struct*)
_tmp322;if(_tmp32D->tag != 1)goto _LL1C7;else{_tmp32E=(void*)_tmp32D->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp32F=(struct Cyc_Absyn_Pat_b_struct*)_tmp32E;if(_tmp32F->tag
!= 5)goto _LL1C7;else{_tmp330=_tmp32F->f1;}};}}if(!(!_tmp330->escapes))goto _LL1C7;
_LL1C6: _tmp334=_tmp330;goto _LL1C8;_LL1C7:{struct Cyc_Absyn_Var_e_struct*_tmp331=(
struct Cyc_Absyn_Var_e_struct*)_tmp322;if(_tmp331->tag != 1)goto _LL1C9;else{
_tmp332=(void*)_tmp331->f2;{struct Cyc_Absyn_Param_b_struct*_tmp333=(struct Cyc_Absyn_Param_b_struct*)
_tmp332;if(_tmp333->tag != 3)goto _LL1C9;else{_tmp334=_tmp333->f1;}};}}if(!(!
_tmp334->escapes))goto _LL1C9;_LL1C8:{struct Cyc_CfFlowInfo_Reln*_tmp7CA;struct Cyc_List_List*
_tmp7C9;new_relns=((_tmp7C9=_region_malloc(env->r,sizeof(*_tmp7C9)),((_tmp7C9->hd=((
_tmp7CA=_region_malloc(env->r,sizeof(*_tmp7CA)),((_tmp7CA->vd=_tmp1B2,((_tmp7CA->rop=
Cyc_CfFlowInfo_LessVar(_tmp334,_tmp334->type),_tmp7CA)))))),((_tmp7C9->tl=
_tmp31E,_tmp7C9))))));}goto _LL1BE;_LL1C9: {struct Cyc_Absyn_Const_e_struct*
_tmp335=(struct Cyc_Absyn_Const_e_struct*)_tmp322;if(_tmp335->tag != 0)goto _LL1CB;
else{_tmp336=_tmp335->f1;if((_tmp336.Int_c).tag != 4)goto _LL1CB;_tmp337=(struct
_tuple6)(_tmp336.Int_c).val;_tmp338=_tmp337.f2;}}_LL1CA:{struct Cyc_CfFlowInfo_Reln*
_tmp7CD;struct Cyc_List_List*_tmp7CC;new_relns=((_tmp7CC=_region_malloc(env->r,
sizeof(*_tmp7CC)),((_tmp7CC->hd=((_tmp7CD=_region_malloc(env->r,sizeof(*_tmp7CD)),((
_tmp7CD->vd=_tmp1B2,((_tmp7CD->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp338),_tmp7CD)))))),((_tmp7CC->tl=_tmp31E,_tmp7CC))))));}goto _LL1BE;_LL1CB: {
struct Cyc_Absyn_Primop_e_struct*_tmp339=(struct Cyc_Absyn_Primop_e_struct*)
_tmp322;if(_tmp339->tag != 3)goto _LL1CD;else{_tmp33A=_tmp339->f1;_tmp33B=_tmp339->f2;
if(_tmp33B == 0)goto _LL1CD;_tmp33C=*_tmp33B;_tmp33D=(struct Cyc_Absyn_Exp*)_tmp33C.hd;}}
_LL1CC:{void*_tmp342=_tmp33D->r;void*_tmp344;struct Cyc_Absyn_Vardecl*_tmp346;
void*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34A;void*_tmp34C;struct Cyc_Absyn_Vardecl*
_tmp34E;void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;_LL1D0:{struct Cyc_Absyn_Var_e_struct*
_tmp343=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp343->tag != 1)goto _LL1D2;
else{_tmp344=(void*)_tmp343->f2;{struct Cyc_Absyn_Global_b_struct*_tmp345=(struct
Cyc_Absyn_Global_b_struct*)_tmp344;if(_tmp345->tag != 1)goto _LL1D2;else{_tmp346=
_tmp345->f1;}};}}if(!(!_tmp346->escapes))goto _LL1D2;_LL1D1: _tmp34A=_tmp346;goto
_LL1D3;_LL1D2:{struct Cyc_Absyn_Var_e_struct*_tmp347=(struct Cyc_Absyn_Var_e_struct*)
_tmp342;if(_tmp347->tag != 1)goto _LL1D4;else{_tmp348=(void*)_tmp347->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp349=(struct Cyc_Absyn_Local_b_struct*)_tmp348;if(
_tmp349->tag != 4)goto _LL1D4;else{_tmp34A=_tmp349->f1;}};}}if(!(!_tmp34A->escapes))
goto _LL1D4;_LL1D3: _tmp34E=_tmp34A;goto _LL1D5;_LL1D4:{struct Cyc_Absyn_Var_e_struct*
_tmp34B=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp34B->tag != 1)goto _LL1D6;
else{_tmp34C=(void*)_tmp34B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp34D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp34C;if(_tmp34D->tag != 5)goto _LL1D6;else{_tmp34E=_tmp34D->f1;}};}}if(!(!
_tmp34E->escapes))goto _LL1D6;_LL1D5: _tmp352=_tmp34E;goto _LL1D7;_LL1D6:{struct Cyc_Absyn_Var_e_struct*
_tmp34F=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp34F->tag != 1)goto _LL1D8;
else{_tmp350=(void*)_tmp34F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp351=(struct
Cyc_Absyn_Param_b_struct*)_tmp350;if(_tmp351->tag != 3)goto _LL1D8;else{_tmp352=
_tmp351->f1;}};}}if(!(!_tmp352->escapes))goto _LL1D8;_LL1D7:{struct Cyc_CfFlowInfo_Reln*
_tmp7D0;struct Cyc_List_List*_tmp7CF;new_relns=((_tmp7CF=_region_malloc(env->r,
sizeof(*_tmp7CF)),((_tmp7CF->hd=((_tmp7D0=_region_malloc(env->r,sizeof(*_tmp7D0)),((
_tmp7D0->vd=_tmp1B2,((_tmp7D0->rop=Cyc_CfFlowInfo_LessNumelts(_tmp352),_tmp7D0)))))),((
_tmp7CF->tl=_tmp31E,_tmp7CF))))));}goto _LL1CF;_LL1D8:;_LL1D9: goto _LL1CF;_LL1CF:;}
goto _LL1BE;_LL1CD:;_LL1CE: goto _LL1BE;_LL1BE:;}if(_tmp31E != new_relns)_tmp318=Cyc_CfFlowInfo_ReachableFL(
_tmp31D,new_relns);{void*_tmp355=_tmp319;_LL1DB: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp356=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp355;if(_tmp356->tag != 0)goto
_LL1DD;}_LL1DC: {struct _tuple13 _tmp7D1;return(_tmp7D1.f1=_tmp318,((_tmp7D1.f2=
_tmp115->unknown_all,_tmp7D1)));}_LL1DD: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp357=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp355;if(_tmp357->tag != 2)
goto _LL1DF;}_LL1DE: goto _LL1E0;_LL1DF: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp358=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp355;if(_tmp358->tag != 1)
goto _LL1E1;}_LL1E0: goto _LL1E2;_LL1E1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp359=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp355;if(_tmp359->tag != 5)goto
_LL1E3;}_LL1E2: {struct Cyc_List_List _tmp7D2;struct Cyc_List_List _tmp35B=(_tmp7D2.hd=
_tmp1B2,((_tmp7D2.tl=0,_tmp7D2)));_tmp318=Cyc_NewControlFlow_add_vars(_tmp115,
_tmp318,(struct Cyc_List_List*)& _tmp35B,_tmp115->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp35D;void*_tmp35E;struct _tuple13 _tmp35C=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp318,_tmp1B4);_tmp35D=_tmp35C.f1;_tmp35E=_tmp35C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp35F=_tmp35D;int _tmp360;struct _tuple12 _tmp361;struct Cyc_Dict_Dict _tmp362;
_LL1E6: if((_tmp35F.BottomFL).tag != 1)goto _LL1E8;_tmp360=(int)(_tmp35F.BottomFL).val;
_LL1E7: {struct _tuple13 _tmp7D3;return(_tmp7D3.f1=_tmp35D,((_tmp7D3.f2=_tmp115->unknown_all,
_tmp7D3)));}_LL1E8: if((_tmp35F.ReachableFL).tag != 2)goto _LL1E5;_tmp361=(struct
_tuple12)(_tmp35F.ReachableFL).val;_tmp362=_tmp361.f1;_LL1E9: if(Cyc_CfFlowInfo_initlevel(
_tmp115,_tmp362,_tmp35E)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp7D6;void*_tmp7D5;(
_tmp7D5=0,Cyc_Tcutil_terr(_tmp1B3->loc,((_tmp7D6="expression may not be initialized",
_tag_dyneither(_tmp7D6,sizeof(char),34))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}{
struct _tuple13 _tmp7D7;return(_tmp7D7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D7.f2=
_tmp115->unknown_all,_tmp7D7)));};}_LL1E5:;}_tmp318=_tmp35D;goto _LL1E4;};}_LL1E3:;
_LL1E4: while(1){struct Cyc_List_List _tmp7D8;struct Cyc_List_List _tmp368=(_tmp7D8.hd=
_tmp1B2,((_tmp7D8.tl=0,_tmp7D8)));_tmp318=Cyc_NewControlFlow_add_vars(_tmp115,
_tmp318,(struct Cyc_List_List*)& _tmp368,_tmp115->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp36A;void*_tmp36B;struct _tuple13 _tmp369=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp318,_tmp1B4);_tmp36A=_tmp369.f1;_tmp36B=_tmp369.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp36C=_tmp36A;int _tmp36D;struct _tuple12 _tmp36E;struct Cyc_Dict_Dict _tmp36F;
_LL1EB: if((_tmp36C.BottomFL).tag != 1)goto _LL1ED;_tmp36D=(int)(_tmp36C.BottomFL).val;
_LL1EC: goto _LL1EA;_LL1ED: if((_tmp36C.ReachableFL).tag != 2)goto _LL1EA;_tmp36E=(
struct _tuple12)(_tmp36C.ReachableFL).val;_tmp36F=_tmp36E.f1;_LL1EE: if(Cyc_CfFlowInfo_initlevel(
_tmp115,_tmp36F,_tmp36B)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp7DB;void*_tmp7DA;(
_tmp7DA=0,Cyc_Tcutil_terr(_tmp1B3->loc,((_tmp7DB="expression may not be initialized",
_tag_dyneither(_tmp7DB,sizeof(char),34))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}{
struct _tuple13 _tmp7DC;return(_tmp7DC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DC.f2=
_tmp115->unknown_all,_tmp7DC)));};}_LL1EA:;}{union Cyc_CfFlowInfo_FlowInfo _tmp373=
Cyc_CfFlowInfo_join_flow(_tmp115,env->all_changed,_tmp318,_tmp36A);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp373,_tmp318))break;_tmp318=_tmp373;};};}{struct _tuple13 _tmp7DD;return(
_tmp7DD.f1=_tmp318,((_tmp7DD.f2=_tmp115->unknown_all,_tmp7DD)));};_LL1DA:;};};
_LL1B9:;};}_LL12A: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1B6=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp134;if(_tmp1B6->tag != 37)goto _LL12C;else{_tmp1B7=_tmp1B6->f1;}}_LL12B: while(
1){union Cyc_CfFlowInfo_FlowInfo*_tmp377;struct _tuple16 _tmp376=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp1B7);_tmp377=_tmp376.f2;inflow=*_tmp377;{void*_tmp378=_tmp1B7->r;
struct Cyc_Absyn_Stmt*_tmp37A;struct Cyc_Absyn_Stmt*_tmp37B;struct Cyc_Absyn_Decl*
_tmp37D;struct Cyc_Absyn_Stmt*_tmp37E;struct Cyc_Absyn_Exp*_tmp380;_LL1F0: {struct
Cyc_Absyn_Seq_s_struct*_tmp379=(struct Cyc_Absyn_Seq_s_struct*)_tmp378;if(_tmp379->tag
!= 2)goto _LL1F2;else{_tmp37A=_tmp379->f1;_tmp37B=_tmp379->f2;}}_LL1F1: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp37A);_tmp1B7=_tmp37B;goto _LL1EF;_LL1F2: {struct Cyc_Absyn_Decl_s_struct*
_tmp37C=(struct Cyc_Absyn_Decl_s_struct*)_tmp378;if(_tmp37C->tag != 12)goto _LL1F4;
else{_tmp37D=_tmp37C->f1;_tmp37E=_tmp37C->f2;}}_LL1F3: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp37D);_tmp1B7=_tmp37E;goto _LL1EF;_LL1F4: {struct Cyc_Absyn_Exp_s_struct*
_tmp37F=(struct Cyc_Absyn_Exp_s_struct*)_tmp378;if(_tmp37F->tag != 1)goto _LL1F6;
else{_tmp380=_tmp37F->f1;}}_LL1F5: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp380);_LL1F6:;_LL1F7: {struct Cyc_Core_Impossible_struct
_tmp7E3;const char*_tmp7E2;struct Cyc_Core_Impossible_struct*_tmp7E1;(int)_throw((
void*)((_tmp7E1=_cycalloc(sizeof(*_tmp7E1)),((_tmp7E1[0]=((_tmp7E3.tag=Cyc_Core_Impossible,((
_tmp7E3.f1=((_tmp7E2="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7E2,
sizeof(char),33))),_tmp7E3)))),_tmp7E1)))));}_LL1EF:;};}_LL12C: {struct Cyc_Absyn_Var_e_struct*
_tmp1B8=(struct Cyc_Absyn_Var_e_struct*)_tmp134;if(_tmp1B8->tag != 1)goto _LL12E;
else{_tmp1B9=(void*)_tmp1B8->f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp1BA=(
struct Cyc_Absyn_Unresolved_b_struct*)_tmp1B9;if(_tmp1BA->tag != 0)goto _LL12E;};}}
_LL12D: goto _LL12F;_LL12E: {struct Cyc_Absyn_UnknownId_e_struct*_tmp1BB=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp134;if(_tmp1BB->tag != 2)goto _LL130;}_LL12F: goto _LL131;_LL130: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp1BC=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp134;if(_tmp1BC->tag != 10)goto
_LL132;}_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1BD=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp134;if(_tmp1BD->tag != 36)
goto _LL134;}_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1BE=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp134;if(_tmp1BE->tag != 26)goto
_LL136;}_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_Valueof_e_struct*_tmp1BF=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp134;if(_tmp1BF->tag != 39)goto _LLCF;}_LL137: {
struct Cyc_Core_Impossible_struct _tmp7E9;const char*_tmp7E8;struct Cyc_Core_Impossible_struct*
_tmp7E7;(int)_throw((void*)((_tmp7E7=_cycalloc(sizeof(*_tmp7E7)),((_tmp7E7[0]=((
_tmp7E9.tag=Cyc_Core_Impossible,((_tmp7E9.f1=((_tmp7E8="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp7E8,sizeof(char),31))),_tmp7E9)))),_tmp7E7)))));}_LLCF:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp387=env->fenv;void*_tmp388=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp38A;void*_tmp38B;struct Cyc_Absyn_PtrAtts _tmp38C;union Cyc_Absyn_Constraint*
_tmp38D;union Cyc_Absyn_Constraint*_tmp38E;_LL1F9: {struct Cyc_Absyn_PointerType_struct*
_tmp389=(struct Cyc_Absyn_PointerType_struct*)_tmp388;if(_tmp389->tag != 5)goto
_LL1FB;else{_tmp38A=_tmp389->f1;_tmp38B=_tmp38A.elt_typ;_tmp38C=_tmp38A.ptr_atts;
_tmp38D=_tmp38C.bounds;_tmp38E=_tmp38C.zero_term;}}_LL1FA: {union Cyc_CfFlowInfo_FlowInfo
_tmp38F=f;int _tmp390;struct _tuple12 _tmp391;struct Cyc_Dict_Dict _tmp392;struct Cyc_List_List*
_tmp393;_LL1FE: if((_tmp38F.BottomFL).tag != 1)goto _LL200;_tmp390=(int)(_tmp38F.BottomFL).val;
_LL1FF: {struct _tuple14 _tmp7EA;return(_tmp7EA.f1=f,((_tmp7EA.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp7EA)));}_LL200: if((_tmp38F.ReachableFL).tag != 2)goto _LL1FD;_tmp391=(struct
_tuple12)(_tmp38F.ReachableFL).val;_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_LL201:
if(Cyc_Tcutil_is_bound_one(_tmp38D)){void*_tmp395=r;struct Cyc_CfFlowInfo_Place*
_tmp399;struct Cyc_CfFlowInfo_Place _tmp39A;void*_tmp39B;struct Cyc_List_List*
_tmp39C;enum Cyc_CfFlowInfo_InitLevel _tmp39F;_LL203: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp396=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp395;if(_tmp396->tag != 1)
goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp397=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp395;if(_tmp397->tag != 2)
goto _LL207;}_LL206:{struct Cyc_CfFlowInfo_NotZero_struct _tmp7ED;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7EC;e->annot=(void*)((_tmp7EC=_cycalloc(sizeof(*_tmp7EC)),((_tmp7EC[0]=((
_tmp7ED.tag=Cyc_CfFlowInfo_NotZero,((_tmp7ED.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp393),_tmp7ED)))),_tmp7EC))));}goto _LL202;_LL207: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp398=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp395;if(_tmp398->tag != 5)goto
_LL209;else{_tmp399=_tmp398->f1;_tmp39A=*_tmp399;_tmp39B=_tmp39A.root;_tmp39C=
_tmp39A.fields;}}_LL208:{struct Cyc_CfFlowInfo_NotZero_struct _tmp7F0;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7EF;e->annot=(void*)((_tmp7EF=_cycalloc(sizeof(*_tmp7EF)),((_tmp7EF[0]=((
_tmp7F0.tag=Cyc_CfFlowInfo_NotZero,((_tmp7F0.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp393),_tmp7F0)))),_tmp7EF))));}{struct Cyc_CfFlowInfo_Place*_tmp7F3;struct
_tuple14 _tmp7F2;return(_tmp7F2.f1=f,((_tmp7F2.f2=Cyc_CfFlowInfo_PlaceL(((_tmp7F3=
_region_malloc(_tmp387->r,sizeof(*_tmp7F3)),((_tmp7F3->root=_tmp39B,((_tmp7F3->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp387->r,_tmp39C,flds),_tmp7F3))))))),_tmp7F2)));};_LL209: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp39D=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp395;if(_tmp39D->tag != 0)goto _LL20B;}_LL20A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp7F4;return(_tmp7F4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F4.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp7F4)));};_LL20B: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp39E=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp395;if(_tmp39E->tag != 3)goto
_LL20D;else{_tmp39F=_tmp39E->f1;}}_LL20C: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp39F);goto _LL20E;_LL20D:;_LL20E: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp7F7;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp7F6;e->annot=(void*)((_tmp7F6=
_cycalloc(sizeof(*_tmp7F6)),((_tmp7F6[0]=((_tmp7F7.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp7F7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp393),_tmp7F7)))),
_tmp7F6))));}_LL202:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp7FA;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp7F9;e->annot=(void*)((_tmp7F9=_cycalloc(sizeof(*_tmp7F9)),((_tmp7F9[0]=((
_tmp7FA.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7FA.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp393),_tmp7FA)))),_tmp7F9))));}if(Cyc_CfFlowInfo_initlevel(_tmp387,_tmp392,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7FD;void*_tmp7FC;(_tmp7FC=0,Cyc_Tcutil_terr(
e->loc,((_tmp7FD="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7FD,sizeof(char),46))),_tag_dyneither(_tmp7FC,sizeof(void*),0)));}{struct
_tuple14 _tmp7FE;return(_tmp7FE.f1=f,((_tmp7FE.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp7FE)));};_LL1FD:;}_LL1FB:;_LL1FC: {struct Cyc_Core_Impossible_struct _tmp804;
const char*_tmp803;struct Cyc_Core_Impossible_struct*_tmp802;(int)_throw((void*)((
_tmp802=_cycalloc(sizeof(*_tmp802)),((_tmp802[0]=((_tmp804.tag=Cyc_Core_Impossible,((
_tmp804.f1=((_tmp803="left deref of non-pointer-type",_tag_dyneither(_tmp803,
sizeof(char),31))),_tmp804)))),_tmp802)))));}_LL1F8:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp3B1=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp3B2=inflow;int _tmp3B3;struct _tuple12
_tmp3B4;struct Cyc_Dict_Dict _tmp3B5;struct Cyc_List_List*_tmp3B6;_LL210: if((
_tmp3B2.BottomFL).tag != 1)goto _LL212;_tmp3B3=(int)(_tmp3B2.BottomFL).val;_LL211: {
struct _tuple14 _tmp805;return(_tmp805.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp805.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp805)));}_LL212: if((_tmp3B2.ReachableFL).tag != 2)
goto _LL20F;_tmp3B4=(struct _tuple12)(_tmp3B2.ReachableFL).val;_tmp3B5=_tmp3B4.f1;
_tmp3B6=_tmp3B4.f2;_LL213: d=_tmp3B5;_LL20F:;}{void*_tmp3B8=e->r;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BE;void*_tmp3C0;
struct Cyc_Absyn_Vardecl*_tmp3C2;void*_tmp3C4;struct Cyc_Absyn_Vardecl*_tmp3C6;
void*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3CA;void*_tmp3CC;struct Cyc_Absyn_Vardecl*
_tmp3CE;struct Cyc_Absyn_Exp*_tmp3D0;struct _dyneither_ptr*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*
_tmp3D8;struct _dyneither_ptr*_tmp3D9;_LL215: {struct Cyc_Absyn_Cast_e_struct*
_tmp3B9=(struct Cyc_Absyn_Cast_e_struct*)_tmp3B8;if(_tmp3B9->tag != 15)goto _LL217;
else{_tmp3BA=_tmp3B9->f2;}}_LL216: _tmp3BC=_tmp3BA;goto _LL218;_LL217: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp3BB=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp3B8;if(_tmp3BB->tag != 13)
goto _LL219;else{_tmp3BC=_tmp3BB->f1;}}_LL218: _tmp3BE=_tmp3BC;goto _LL21A;_LL219: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp3BD=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp3B8;if(_tmp3BD->tag != 14)goto _LL21B;else{_tmp3BE=_tmp3BD->f1;}}_LL21A: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3BE,flds);_LL21B: {
struct Cyc_Absyn_Var_e_struct*_tmp3BF=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(
_tmp3BF->tag != 1)goto _LL21D;else{_tmp3C0=(void*)_tmp3BF->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp3C1=(struct Cyc_Absyn_Param_b_struct*)_tmp3C0;if(_tmp3C1->tag != 3)goto _LL21D;
else{_tmp3C2=_tmp3C1->f1;}};}}_LL21C: _tmp3C6=_tmp3C2;goto _LL21E;_LL21D: {struct
Cyc_Absyn_Var_e_struct*_tmp3C3=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C3->tag
!= 1)goto _LL21F;else{_tmp3C4=(void*)_tmp3C3->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp3C5=(struct Cyc_Absyn_Local_b_struct*)_tmp3C4;if(_tmp3C5->tag != 4)goto _LL21F;
else{_tmp3C6=_tmp3C5->f1;}};}}_LL21E: _tmp3CA=_tmp3C6;goto _LL220;_LL21F: {struct
Cyc_Absyn_Var_e_struct*_tmp3C7=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C7->tag
!= 1)goto _LL221;else{_tmp3C8=(void*)_tmp3C7->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp3C9=(struct Cyc_Absyn_Pat_b_struct*)_tmp3C8;if(_tmp3C9->tag != 5)goto _LL221;
else{_tmp3CA=_tmp3C9->f1;}};}}_LL220: {struct Cyc_CfFlowInfo_Place*_tmp80F;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp80E;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp80D;
struct _tuple14 _tmp80C;return(_tmp80C.f1=inflow,((_tmp80C.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp80F=_region_malloc(env->r,sizeof(*_tmp80F)),((_tmp80F->root=(void*)((_tmp80D=
_region_malloc(env->r,sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp80E.tag=0,((_tmp80E.f1=
_tmp3CA,_tmp80E)))),_tmp80D)))),((_tmp80F->fields=flds,_tmp80F))))))),_tmp80C)));}
_LL221: {struct Cyc_Absyn_Var_e_struct*_tmp3CB=(struct Cyc_Absyn_Var_e_struct*)
_tmp3B8;if(_tmp3CB->tag != 1)goto _LL223;else{_tmp3CC=(void*)_tmp3CB->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp3CD=(struct Cyc_Absyn_Global_b_struct*)_tmp3CC;if(
_tmp3CD->tag != 1)goto _LL223;else{_tmp3CE=_tmp3CD->f1;}};}}_LL222: {struct
_tuple14 _tmp810;return(_tmp810.f1=inflow,((_tmp810.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp810)));}_LL223: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp3CF=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3B8;if(_tmp3CF->tag != 23)goto _LL225;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;}}
_LL224:{void*_tmp3DF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3D0->topt))->v);struct Cyc_Absyn_PtrInfo _tmp3E1;void*_tmp3E2;
_LL22E: {struct Cyc_Absyn_PointerType_struct*_tmp3E0=(struct Cyc_Absyn_PointerType_struct*)
_tmp3DF;if(_tmp3E0->tag != 5)goto _LL230;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E1.elt_typ;}}
_LL22F: if(!Cyc_Absyn_is_nontagged_union_type(_tmp3E2)){struct Cyc_List_List*
_tmp811;flds=((_tmp811=_region_malloc(env->r,sizeof(*_tmp811)),((_tmp811->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp3E2,_tmp3D1),((_tmp811->tl=flds,_tmp811))))));}
goto _LL22D;_LL230:;_LL231: {struct Cyc_Core_Impossible_struct _tmp817;const char*
_tmp816;struct Cyc_Core_Impossible_struct*_tmp815;(int)_throw((void*)((_tmp815=
_cycalloc(sizeof(*_tmp815)),((_tmp815[0]=((_tmp817.tag=Cyc_Core_Impossible,((
_tmp817.f1=((_tmp816="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp816,sizeof(
char),25))),_tmp817)))),_tmp815)))));}_LL22D:;}_tmp3D3=_tmp3D0;goto _LL226;_LL225: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D2=(struct Cyc_Absyn_Deref_e_struct*)_tmp3B8;
if(_tmp3D2->tag != 21)goto _LL227;else{_tmp3D3=_tmp3D2->f1;}}_LL226: if(Cyc_Tcutil_is_noalias_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp3E8;union Cyc_CfFlowInfo_AbsLVal _tmp3E9;struct _tuple14 _tmp3E7=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,_tmp3D3);_tmp3E8=_tmp3E7.f1;_tmp3E9=_tmp3E7.f2;{struct _tuple14
_tmp818;struct _tuple14 _tmp3EB=(_tmp818.f1=_tmp3E8,((_tmp818.f2=_tmp3E9,_tmp818)));
union Cyc_CfFlowInfo_FlowInfo _tmp3EC;struct _tuple12 _tmp3ED;struct Cyc_Dict_Dict
_tmp3EE;struct Cyc_List_List*_tmp3EF;union Cyc_CfFlowInfo_AbsLVal _tmp3F0;struct Cyc_CfFlowInfo_Place*
_tmp3F1;_LL233: _tmp3EC=_tmp3EB.f1;if((_tmp3EC.ReachableFL).tag != 2)goto _LL235;
_tmp3ED=(struct _tuple12)(_tmp3EC.ReachableFL).val;_tmp3EE=_tmp3ED.f1;_tmp3EF=
_tmp3ED.f2;_tmp3F0=_tmp3EB.f2;if((_tmp3F0.PlaceL).tag != 1)goto _LL235;_tmp3F1=(
struct Cyc_CfFlowInfo_Place*)(_tmp3F0.PlaceL).val;_LL234: {void*_tmp3F2=Cyc_CfFlowInfo_lookup_place(
_tmp3EE,_tmp3F1);void*_tmp3F3=_tmp3F2;_LL238: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3F4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3F3;if(_tmp3F4->tag != 5)goto
_LL23A;}_LL239: return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3D3,_tmp3E8,
_tmp3F2,flds);_LL23A:;_LL23B: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp3B1,_tmp3EE,_tmp3F2);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3B1->unknown_all:
_tmp3B1->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B1,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3->topt))->v),leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp81B;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp81A;void*new_root=(void*)((
_tmp81A=_region_malloc(_tmp3B1->r,sizeof(*_tmp81A)),((_tmp81A[0]=((_tmp81B.tag=1,((
_tmp81B.f1=e,((_tmp81B.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp81B)))))),_tmp81A))));struct Cyc_CfFlowInfo_Place*_tmp81C;struct Cyc_CfFlowInfo_Place*
place=(_tmp81C=_region_malloc(_tmp3B1->r,sizeof(*_tmp81C)),((_tmp81C->root=
new_root,((_tmp81C->fields=0,_tmp81C)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp81F;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp81E;void*res=(void*)((_tmp81E=
_region_malloc(_tmp3B1->r,sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=5,((
_tmp81F.f1=place,_tmp81F)))),_tmp81E))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp3EE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp3EE,new_root,new_rval);_tmp3EE=Cyc_CfFlowInfo_assign_place(
_tmp3B1,e->loc,_tmp3EE,env->all_changed,_tmp3F1,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp3F5=Cyc_CfFlowInfo_ReachableFL(_tmp3EE,_tmp3EF);return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3D3,_tmp3F5,res,flds);};}_LL237:;}_LL235:;_LL236: goto _LL232;
_LL232:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp3FC;void*_tmp3FD;struct _tuple13
_tmp3FB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3D3);_tmp3FC=_tmp3FB.f1;
_tmp3FD=_tmp3FB.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3D3,
_tmp3FC,_tmp3FD,flds);};_LL227: {struct Cyc_Absyn_Subscript_e_struct*_tmp3D4=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp3B8;if(_tmp3D4->tag != 24)goto _LL229;
else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;}}_LL228: {void*_tmp3FE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D5->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp401;struct Cyc_Absyn_PtrAtts _tmp402;union Cyc_Absyn_Constraint*_tmp403;_LL23D: {
struct Cyc_Absyn_TupleType_struct*_tmp3FF=(struct Cyc_Absyn_TupleType_struct*)
_tmp3FE;if(_tmp3FF->tag != 11)goto _LL23F;}_LL23E: {unsigned int _tmp404=(Cyc_Evexp_eval_const_uint_exp(
_tmp3D6)).f1;struct Cyc_List_List*_tmp820;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp3D5,((_tmp820=_region_malloc(env->r,sizeof(*_tmp820)),((
_tmp820->hd=(void*)((int)_tmp404),((_tmp820->tl=flds,_tmp820)))))));}_LL23F: {
struct Cyc_Absyn_PointerType_struct*_tmp400=(struct Cyc_Absyn_PointerType_struct*)
_tmp3FE;if(_tmp400->tag != 5)goto _LL241;else{_tmp401=_tmp400->f1;_tmp402=_tmp401.ptr_atts;
_tmp403=_tmp402.bounds;}}_LL240: {struct _RegionHandle*_tmp406=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp409;struct Cyc_List_List*_tmp40A;struct Cyc_Absyn_Exp*_tmp821[2];struct
_tuple18 _tmp408=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp406,env,inflow,((
_tmp821[1]=_tmp3D6,((_tmp821[0]=_tmp3D5,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp406,_tag_dyneither(
_tmp821,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp409=_tmp408.f1;_tmp40A=
_tmp408.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp40B=_tmp409;{union Cyc_CfFlowInfo_FlowInfo
_tmp40C=_tmp409;struct _tuple12 _tmp40D;struct Cyc_Dict_Dict _tmp40E;struct Cyc_List_List*
_tmp40F;_LL244: if((_tmp40C.ReachableFL).tag != 2)goto _LL246;_tmp40D=(struct
_tuple12)(_tmp40C.ReachableFL).val;_tmp40E=_tmp40D.f1;_tmp40F=_tmp40D.f2;_LL245:
if(Cyc_CfFlowInfo_initlevel(_tmp3B1,_tmp40E,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp40A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp824;void*_tmp823;(_tmp823=0,Cyc_Tcutil_terr(_tmp3D6->loc,((_tmp824="expression may not be initialized",
_tag_dyneither(_tmp824,sizeof(char),34))),_tag_dyneither(_tmp823,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp412=Cyc_NewControlFlow_add_subscript_reln(_tmp3B1->r,
_tmp40F,_tmp3D5,_tmp3D6);if(_tmp40F != _tmp412)_tmp40B=Cyc_CfFlowInfo_ReachableFL(
_tmp40E,_tmp412);goto _LL243;};_LL246:;_LL247: goto _LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp414;union Cyc_CfFlowInfo_AbsLVal _tmp415;struct _tuple14 _tmp413=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3D5,_tmp409,(void*)((struct Cyc_List_List*)_check_null(_tmp40A))->hd,
flds);_tmp414=_tmp413.f1;_tmp415=_tmp413.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp416=
_tmp414;int _tmp417;_LL249: if((_tmp416.BottomFL).tag != 1)goto _LL24B;_tmp417=(int)(
_tmp416.BottomFL).val;_LL24A: {struct _tuple14 _tmp825;return(_tmp825.f1=_tmp414,((
_tmp825.f2=_tmp415,_tmp825)));}_LL24B:;_LL24C: {struct _tuple14 _tmp826;return(
_tmp826.f1=_tmp40B,((_tmp826.f2=_tmp415,_tmp826)));}_LL248:;};};};}_LL241:;
_LL242: {struct Cyc_Core_Impossible_struct _tmp82C;const char*_tmp82B;struct Cyc_Core_Impossible_struct*
_tmp82A;(int)_throw((void*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A[0]=((
_tmp82C.tag=Cyc_Core_Impossible,((_tmp82C.f1=((_tmp82B="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp82B,sizeof(char),33))),_tmp82C)))),_tmp82A)))));}_LL23C:;}
_LL229: {struct Cyc_Absyn_AggrMember_e_struct*_tmp3D7=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3B8;if(_tmp3D7->tag != 22)goto _LL22B;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=_tmp3D7->f2;}}
_LL22A: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3D8->topt))->v)){struct _tuple14 _tmp82D;return(_tmp82D.f1=inflow,((
_tmp82D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82D)));}{struct Cyc_List_List*_tmp82E;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3D8,((
_tmp82E=_region_malloc(env->r,sizeof(*_tmp82E)),((_tmp82E->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8->topt))->v,_tmp3D9),((_tmp82E->tl=
flds,_tmp82E)))))));};_LL22B:;_LL22C: {struct _tuple14 _tmp82F;return(_tmp82F.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp82F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82F)));}
_LL214:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp421;union Cyc_CfFlowInfo_AbsLVal _tmp422;struct
_tuple14 _tmp420=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp421=_tmp420.f1;_tmp422=_tmp420.f2;{struct _tuple14 _tmp830;return(_tmp830.f1=
_tmp421,((_tmp830.f2=_tmp422,_tmp830)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp424=env->fenv;{void*_tmp425=e->r;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*
_tmp428;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*
_tmp42C;struct Cyc_Absyn_Exp*_tmp42E;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_Absyn_Exp*
_tmp431;struct Cyc_Absyn_Exp*_tmp432;enum Cyc_Absyn_Primop _tmp434;struct Cyc_List_List*
_tmp435;struct Cyc_List_List _tmp436;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_List_List*
_tmp438;enum Cyc_Absyn_Primop _tmp43A;struct Cyc_List_List*_tmp43B;_LL24E: {struct
Cyc_Absyn_Conditional_e_struct*_tmp426=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp425;if(_tmp426->tag != 6)goto _LL250;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;
_tmp429=_tmp426->f3;}}_LL24F: {union Cyc_CfFlowInfo_FlowInfo _tmp43D;union Cyc_CfFlowInfo_FlowInfo
_tmp43E;struct _tuple15 _tmp43C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp427);
_tmp43D=_tmp43C.f1;_tmp43E=_tmp43C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp440;union
Cyc_CfFlowInfo_FlowInfo _tmp441;struct _tuple15 _tmp43F=Cyc_NewControlFlow_anal_test(
env,_tmp43D,_tmp428);_tmp440=_tmp43F.f1;_tmp441=_tmp43F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp443;union Cyc_CfFlowInfo_FlowInfo _tmp444;struct _tuple15 _tmp442=Cyc_NewControlFlow_anal_test(
env,_tmp43E,_tmp429);_tmp443=_tmp442.f1;_tmp444=_tmp442.f2;{struct _tuple15
_tmp831;return(_tmp831.f1=Cyc_CfFlowInfo_join_flow(_tmp424,env->all_changed,
_tmp440,_tmp443),((_tmp831.f2=Cyc_CfFlowInfo_join_flow(_tmp424,env->all_changed,
_tmp441,_tmp444),_tmp831)));};};};}_LL250: {struct Cyc_Absyn_And_e_struct*_tmp42A=(
struct Cyc_Absyn_And_e_struct*)_tmp425;if(_tmp42A->tag != 7)goto _LL252;else{
_tmp42B=_tmp42A->f1;_tmp42C=_tmp42A->f2;}}_LL251: {union Cyc_CfFlowInfo_FlowInfo
_tmp447;union Cyc_CfFlowInfo_FlowInfo _tmp448;struct _tuple15 _tmp446=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp42B);_tmp447=_tmp446.f1;_tmp448=_tmp446.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp44A;union Cyc_CfFlowInfo_FlowInfo _tmp44B;struct _tuple15 _tmp449=Cyc_NewControlFlow_anal_test(
env,_tmp447,_tmp42C);_tmp44A=_tmp449.f1;_tmp44B=_tmp449.f2;{struct _tuple15
_tmp832;return(_tmp832.f1=_tmp44A,((_tmp832.f2=Cyc_CfFlowInfo_join_flow(_tmp424,
env->all_changed,_tmp448,_tmp44B),_tmp832)));};};}_LL252: {struct Cyc_Absyn_Or_e_struct*
_tmp42D=(struct Cyc_Absyn_Or_e_struct*)_tmp425;if(_tmp42D->tag != 8)goto _LL254;
else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;}}_LL253: {union Cyc_CfFlowInfo_FlowInfo
_tmp44E;union Cyc_CfFlowInfo_FlowInfo _tmp44F;struct _tuple15 _tmp44D=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp42E);_tmp44E=_tmp44D.f1;_tmp44F=_tmp44D.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp451;union Cyc_CfFlowInfo_FlowInfo _tmp452;struct _tuple15 _tmp450=Cyc_NewControlFlow_anal_test(
env,_tmp44F,_tmp42F);_tmp451=_tmp450.f1;_tmp452=_tmp450.f2;{struct _tuple15
_tmp833;return(_tmp833.f1=Cyc_CfFlowInfo_join_flow(_tmp424,env->all_changed,
_tmp44E,_tmp451),((_tmp833.f2=_tmp452,_tmp833)));};};}_LL254: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp430=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp425;if(_tmp430->tag != 9)goto _LL256;
else{_tmp431=_tmp430->f1;_tmp432=_tmp430->f2;}}_LL255: {union Cyc_CfFlowInfo_FlowInfo
_tmp455;void*_tmp456;struct _tuple13 _tmp454=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp431);_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp455,_tmp432);}_LL256: {struct Cyc_Absyn_Primop_e_struct*_tmp433=(struct
Cyc_Absyn_Primop_e_struct*)_tmp425;if(_tmp433->tag != 3)goto _LL258;else{_tmp434=
_tmp433->f1;if(_tmp434 != Cyc_Absyn_Not)goto _LL258;_tmp435=_tmp433->f2;if(_tmp435
== 0)goto _LL258;_tmp436=*_tmp435;_tmp437=(struct Cyc_Absyn_Exp*)_tmp436.hd;
_tmp438=_tmp436.tl;if(_tmp438 != 0)goto _LL258;}}_LL257: {union Cyc_CfFlowInfo_FlowInfo
_tmp458;union Cyc_CfFlowInfo_FlowInfo _tmp459;struct _tuple15 _tmp457=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp437);_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;{struct _tuple15 _tmp834;
return(_tmp834.f1=_tmp459,((_tmp834.f2=_tmp458,_tmp834)));};}_LL258: {struct Cyc_Absyn_Primop_e_struct*
_tmp439=(struct Cyc_Absyn_Primop_e_struct*)_tmp425;if(_tmp439->tag != 3)goto _LL25A;
else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}_LL259: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp45B=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp45D;struct
Cyc_List_List*_tmp45E;struct _tuple18 _tmp45C=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp45B,env,inflow,_tmp43B,0,0);_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp45E))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp45E->tl))->hd;f=_tmp45D;}{union Cyc_CfFlowInfo_FlowInfo _tmp45F=f;
int _tmp460;struct _tuple12 _tmp461;struct Cyc_Dict_Dict _tmp462;struct Cyc_List_List*
_tmp463;_LL25D: if((_tmp45F.BottomFL).tag != 1)goto _LL25F;_tmp460=(int)(_tmp45F.BottomFL).val;
_LL25E: {struct _tuple15 _tmp835;return(_tmp835.f1=f,((_tmp835.f2=f,_tmp835)));}
_LL25F: if((_tmp45F.ReachableFL).tag != 2)goto _LL25C;_tmp461=(struct _tuple12)(
_tmp45F.ReachableFL).val;_tmp462=_tmp461.f1;_tmp463=_tmp461.f2;_LL260: {struct
Cyc_Absyn_Exp*_tmp465=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp43B))->hd;struct Cyc_Absyn_Exp*_tmp466=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp43B->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp462,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp838;void*_tmp837;(_tmp837=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp43B->hd)->loc,((_tmp838="expression may not be initialized",
_tag_dyneither(_tmp838,sizeof(char),34))),_tag_dyneither(_tmp837,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp462,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp83B;void*_tmp83A;(_tmp83A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp43B->tl))->hd)->loc,((_tmp83B="expression may not be initialized",
_tag_dyneither(_tmp83B,sizeof(char),34))),_tag_dyneither(_tmp83A,sizeof(void*),0)));}
if(_tmp43A == Cyc_Absyn_Eq  || _tmp43A == Cyc_Absyn_Neq){struct _tuple0 _tmp83C;
struct _tuple0 _tmp46C=(_tmp83C.f1=r1,((_tmp83C.f2=r2,_tmp83C)));void*_tmp46D;
enum Cyc_CfFlowInfo_InitLevel _tmp46F;void*_tmp470;void*_tmp472;void*_tmp474;
enum Cyc_CfFlowInfo_InitLevel _tmp476;void*_tmp477;void*_tmp479;void*_tmp47B;void*
_tmp47D;void*_tmp47F;void*_tmp481;void*_tmp483;void*_tmp485;void*_tmp487;void*
_tmp489;void*_tmp48B;void*_tmp48D;void*_tmp48F;void*_tmp491;_LL262: _tmp46D=
_tmp46C.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp46E=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp46D;if(_tmp46E->tag != 3)goto _LL264;else{_tmp46F=_tmp46E->f1;}};_tmp470=
_tmp46C.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp471=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp470;if(_tmp471->tag != 0)goto _LL264;};_LL263: {union Cyc_CfFlowInfo_FlowInfo
_tmp494;union Cyc_CfFlowInfo_FlowInfo _tmp495;struct _tuple15 _tmp493=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp465,_tmp46F);_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;switch(_tmp43A){
case Cyc_Absyn_Eq: _LL276: {struct _tuple15 _tmp83D;return(_tmp83D.f1=_tmp495,((
_tmp83D.f2=_tmp494,_tmp83D)));}case Cyc_Absyn_Neq: _LL277: {struct _tuple15 _tmp83E;
return(_tmp83E.f1=_tmp494,((_tmp83E.f2=_tmp495,_tmp83E)));}default: _LL278: {
struct Cyc_Core_Impossible_struct _tmp844;const char*_tmp843;struct Cyc_Core_Impossible_struct*
_tmp842;(int)_throw((void*)((_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842[0]=((
_tmp844.tag=Cyc_Core_Impossible,((_tmp844.f1=((_tmp843="anal_test, zero-split",
_tag_dyneither(_tmp843,sizeof(char),22))),_tmp844)))),_tmp842)))));}}}_LL264:
_tmp472=_tmp46C.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp473=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp472;if(_tmp473->tag != 0)goto _LL266;};_tmp474=_tmp46C.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp475=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp474;if(_tmp475->tag != 3)goto
_LL266;else{_tmp476=_tmp475->f1;}};_LL265: {union Cyc_CfFlowInfo_FlowInfo _tmp49C;
union Cyc_CfFlowInfo_FlowInfo _tmp49D;struct _tuple15 _tmp49B=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp466,_tmp476);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;switch(_tmp43A){
case Cyc_Absyn_Eq: _LL27A: {struct _tuple15 _tmp845;return(_tmp845.f1=_tmp49D,((
_tmp845.f2=_tmp49C,_tmp845)));}case Cyc_Absyn_Neq: _LL27B: {struct _tuple15 _tmp846;
return(_tmp846.f1=_tmp49C,((_tmp846.f2=_tmp49D,_tmp846)));}default: _LL27C: {
struct Cyc_Core_Impossible_struct _tmp84C;const char*_tmp84B;struct Cyc_Core_Impossible_struct*
_tmp84A;(int)_throw((void*)((_tmp84A=_cycalloc(sizeof(*_tmp84A)),((_tmp84A[0]=((
_tmp84C.tag=Cyc_Core_Impossible,((_tmp84C.f1=((_tmp84B="anal_test, zero-split",
_tag_dyneither(_tmp84B,sizeof(char),22))),_tmp84C)))),_tmp84A)))));}}}_LL266:
_tmp477=_tmp46C.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp478=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp477;if(_tmp478->tag != 0)goto _LL268;};_tmp479=_tmp46C.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp47A=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp479;if(_tmp47A->tag != 0)goto
_LL268;};_LL267: if(_tmp43A == Cyc_Absyn_Eq){struct _tuple15 _tmp84D;return(_tmp84D.f1=
f,((_tmp84D.f2=Cyc_CfFlowInfo_BottomFL(),_tmp84D)));}else{struct _tuple15 _tmp84E;
return(_tmp84E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84E.f2=f,_tmp84E)));}_LL268:
_tmp47B=_tmp46C.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp47C=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp47B;if(_tmp47C->tag != 0)goto _LL26A;};_tmp47D=_tmp46C.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp47E=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp47D;if(_tmp47E->tag != 1)
goto _LL26A;};_LL269: goto _LL26B;_LL26A: _tmp47F=_tmp46C.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp480=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp47F;if(_tmp480->tag != 0)goto
_LL26C;};_tmp481=_tmp46C.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp482=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp481;if(_tmp482->tag != 2)goto _LL26C;};
_LL26B: goto _LL26D;_LL26C: _tmp483=_tmp46C.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp484=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp483;if(_tmp484->tag != 0)goto
_LL26E;};_tmp485=_tmp46C.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp486=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp485;if(_tmp486->tag != 5)goto _LL26E;};
_LL26D: goto _LL26F;_LL26E: _tmp487=_tmp46C.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp488=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp487;if(_tmp488->tag != 1)
goto _LL270;};_tmp489=_tmp46C.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp48A=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp489;if(_tmp48A->tag != 0)goto _LL270;};
_LL26F: goto _LL271;_LL270: _tmp48B=_tmp46C.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp48C=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp48B;if(_tmp48C->tag != 2)
goto _LL272;};_tmp48D=_tmp46C.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp48E=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp48D;if(_tmp48E->tag != 0)goto _LL272;};
_LL271: goto _LL273;_LL272: _tmp48F=_tmp46C.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp490=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp48F;if(_tmp490->tag != 5)goto
_LL274;};_tmp491=_tmp46C.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp492=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp491;if(_tmp492->tag != 0)goto _LL274;};_LL273: if(_tmp43A == Cyc_Absyn_Neq){
struct _tuple15 _tmp84F;return(_tmp84F.f1=f,((_tmp84F.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp84F)));}else{struct _tuple15 _tmp850;return(_tmp850.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp850.f2=f,_tmp850)));}_LL274:;_LL275: goto _LL261;_LL261:;}{struct _tuple0
_tmp851;struct _tuple0 _tmp4A8=(_tmp851.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp465->topt))->v),((_tmp851.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp466->topt))->v),_tmp851)));void*_tmp4A9;enum Cyc_Absyn_Sign
_tmp4AB;void*_tmp4AC;enum Cyc_Absyn_Sign _tmp4AE;void*_tmp4AF;void*_tmp4B1;_LL27F:
_tmp4A9=_tmp4A8.f1;{struct Cyc_Absyn_IntType_struct*_tmp4AA=(struct Cyc_Absyn_IntType_struct*)
_tmp4A9;if(_tmp4AA->tag != 6)goto _LL281;else{_tmp4AB=_tmp4AA->f1;if(_tmp4AB != Cyc_Absyn_Unsigned)
goto _LL281;}};_LL280: goto _LL282;_LL281: _tmp4AC=_tmp4A8.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4AD=(struct Cyc_Absyn_IntType_struct*)_tmp4AC;if(_tmp4AD->tag != 6)goto _LL283;
else{_tmp4AE=_tmp4AD->f1;if(_tmp4AE != Cyc_Absyn_Unsigned)goto _LL283;}};_LL282:
goto _LL284;_LL283: _tmp4AF=_tmp4A8.f1;{struct Cyc_Absyn_TagType_struct*_tmp4B0=(
struct Cyc_Absyn_TagType_struct*)_tmp4AF;if(_tmp4B0->tag != 20)goto _LL285;};_LL284:
goto _LL286;_LL285: _tmp4B1=_tmp4A8.f2;{struct Cyc_Absyn_TagType_struct*_tmp4B2=(
struct Cyc_Absyn_TagType_struct*)_tmp4B1;if(_tmp4B2->tag != 20)goto _LL287;};_LL286:
goto _LL27E;_LL287:;_LL288: {struct _tuple15 _tmp852;return(_tmp852.f1=f,((_tmp852.f2=
f,_tmp852)));}_LL27E:;}switch(_tmp43A){case Cyc_Absyn_Eq: _LL289: goto _LL28A;case
Cyc_Absyn_Neq: _LL28A: goto _LL28B;case Cyc_Absyn_Gt: _LL28B: goto _LL28C;case Cyc_Absyn_Gte:
_LL28C: {struct _tuple15 _tmp853;return(_tmp853.f1=f,((_tmp853.f2=f,_tmp853)));}
case Cyc_Absyn_Lt: _LL28D: {union Cyc_CfFlowInfo_FlowInfo _tmp4B5=f;union Cyc_CfFlowInfo_FlowInfo
_tmp4B6=f;{union Cyc_CfFlowInfo_FlowInfo _tmp4B7=_tmp4B5;int _tmp4B8;struct _tuple12
_tmp4B9;struct Cyc_Dict_Dict _tmp4BA;_LL290: if((_tmp4B7.BottomFL).tag != 1)goto
_LL292;_tmp4B8=(int)(_tmp4B7.BottomFL).val;_LL291: {struct Cyc_Core_Impossible_struct
_tmp859;const char*_tmp858;struct Cyc_Core_Impossible_struct*_tmp857;(int)_throw((
void*)((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=((_tmp859.tag=Cyc_Core_Impossible,((
_tmp859.f1=((_tmp858="anal_test, Lt",_tag_dyneither(_tmp858,sizeof(char),14))),
_tmp859)))),_tmp857)))));}_LL292: if((_tmp4B7.ReachableFL).tag != 2)goto _LL28F;
_tmp4B9=(struct _tuple12)(_tmp4B7.ReachableFL).val;_tmp4BA=_tmp4B9.f1;_LL293:
_tmp462=_tmp4BA;_LL28F:;}{void*_tmp4BE=_tmp465->r;void*_tmp4C0;struct Cyc_Absyn_Vardecl*
_tmp4C2;void*_tmp4C4;struct Cyc_Absyn_Vardecl*_tmp4C6;void*_tmp4C8;struct Cyc_Absyn_Vardecl*
_tmp4CA;void*_tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4CE;_LL295:{struct Cyc_Absyn_Var_e_struct*
_tmp4BF=(struct Cyc_Absyn_Var_e_struct*)_tmp4BE;if(_tmp4BF->tag != 1)goto _LL297;
else{_tmp4C0=(void*)_tmp4BF->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4C1=(struct
Cyc_Absyn_Global_b_struct*)_tmp4C0;if(_tmp4C1->tag != 1)goto _LL297;else{_tmp4C2=
_tmp4C1->f1;}};}}if(!(!_tmp4C2->escapes))goto _LL297;_LL296: _tmp4C6=_tmp4C2;goto
_LL298;_LL297:{struct Cyc_Absyn_Var_e_struct*_tmp4C3=(struct Cyc_Absyn_Var_e_struct*)
_tmp4BE;if(_tmp4C3->tag != 1)goto _LL299;else{_tmp4C4=(void*)_tmp4C3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4C5=(struct Cyc_Absyn_Local_b_struct*)_tmp4C4;if(
_tmp4C5->tag != 4)goto _LL299;else{_tmp4C6=_tmp4C5->f1;}};}}if(!(!_tmp4C6->escapes))
goto _LL299;_LL298: _tmp4CA=_tmp4C6;goto _LL29A;_LL299:{struct Cyc_Absyn_Var_e_struct*
_tmp4C7=(struct Cyc_Absyn_Var_e_struct*)_tmp4BE;if(_tmp4C7->tag != 1)goto _LL29B;
else{_tmp4C8=(void*)_tmp4C7->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4C9=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4C8;if(_tmp4C9->tag != 5)goto _LL29B;else{_tmp4CA=_tmp4C9->f1;}};}}if(!(!
_tmp4CA->escapes))goto _LL29B;_LL29A: _tmp4CE=_tmp4CA;goto _LL29C;_LL29B:{struct Cyc_Absyn_Var_e_struct*
_tmp4CB=(struct Cyc_Absyn_Var_e_struct*)_tmp4BE;if(_tmp4CB->tag != 1)goto _LL29D;
else{_tmp4CC=(void*)_tmp4CB->f2;{struct Cyc_Absyn_Param_b_struct*_tmp4CD=(struct
Cyc_Absyn_Param_b_struct*)_tmp4CC;if(_tmp4CD->tag != 3)goto _LL29D;else{_tmp4CE=
_tmp4CD->f1;}};}}if(!(!_tmp4CE->escapes))goto _LL29D;_LL29C: {void*_tmp4CF=
_tmp466->r;void*_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D3;void*_tmp4D5;struct Cyc_Absyn_Vardecl*
_tmp4D7;void*_tmp4D9;struct Cyc_Absyn_Vardecl*_tmp4DB;void*_tmp4DD;struct Cyc_Absyn_Vardecl*
_tmp4DF;union Cyc_Absyn_Cnst _tmp4E1;struct _tuple6 _tmp4E2;int _tmp4E3;struct Cyc_Absyn_Exp*
_tmp4E5;struct Cyc_Absyn_Exp _tmp4E6;void*_tmp4E7;union Cyc_Absyn_Cnst _tmp4E9;
struct _tuple6 _tmp4EA;int _tmp4EB;enum Cyc_Absyn_Primop _tmp4ED;struct Cyc_List_List*
_tmp4EE;struct Cyc_List_List _tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;_LL2A0:{struct Cyc_Absyn_Var_e_struct*
_tmp4D0=(struct Cyc_Absyn_Var_e_struct*)_tmp4CF;if(_tmp4D0->tag != 1)goto _LL2A2;
else{_tmp4D1=(void*)_tmp4D0->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4D2=(struct
Cyc_Absyn_Global_b_struct*)_tmp4D1;if(_tmp4D2->tag != 1)goto _LL2A2;else{_tmp4D3=
_tmp4D2->f1;}};}}if(!(!_tmp4D3->escapes))goto _LL2A2;_LL2A1: _tmp4D7=_tmp4D3;goto
_LL2A3;_LL2A2:{struct Cyc_Absyn_Var_e_struct*_tmp4D4=(struct Cyc_Absyn_Var_e_struct*)
_tmp4CF;if(_tmp4D4->tag != 1)goto _LL2A4;else{_tmp4D5=(void*)_tmp4D4->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4D6=(struct Cyc_Absyn_Local_b_struct*)_tmp4D5;if(
_tmp4D6->tag != 4)goto _LL2A4;else{_tmp4D7=_tmp4D6->f1;}};}}if(!(!_tmp4D7->escapes))
goto _LL2A4;_LL2A3: _tmp4DB=_tmp4D7;goto _LL2A5;_LL2A4:{struct Cyc_Absyn_Var_e_struct*
_tmp4D8=(struct Cyc_Absyn_Var_e_struct*)_tmp4CF;if(_tmp4D8->tag != 1)goto _LL2A6;
else{_tmp4D9=(void*)_tmp4D8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4DA=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4D9;if(_tmp4DA->tag != 5)goto _LL2A6;else{_tmp4DB=_tmp4DA->f1;}};}}if(!(!
_tmp4DB->escapes))goto _LL2A6;_LL2A5: _tmp4DF=_tmp4DB;goto _LL2A7;_LL2A6:{struct Cyc_Absyn_Var_e_struct*
_tmp4DC=(struct Cyc_Absyn_Var_e_struct*)_tmp4CF;if(_tmp4DC->tag != 1)goto _LL2A8;
else{_tmp4DD=(void*)_tmp4DC->f2;{struct Cyc_Absyn_Param_b_struct*_tmp4DE=(struct
Cyc_Absyn_Param_b_struct*)_tmp4DD;if(_tmp4DE->tag != 3)goto _LL2A8;else{_tmp4DF=
_tmp4DE->f1;}};}}if(!(!_tmp4DF->escapes))goto _LL2A8;_LL2A7: {struct _RegionHandle*
_tmp4F1=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp85C;struct Cyc_List_List*
_tmp85B;struct Cyc_List_List*_tmp4F2=(_tmp85B=_region_malloc(_tmp4F1,sizeof(*
_tmp85B)),((_tmp85B->hd=((_tmp85C=_region_malloc(_tmp4F1,sizeof(*_tmp85C)),((
_tmp85C->vd=_tmp4CE,((_tmp85C->rop=Cyc_CfFlowInfo_LessVar(_tmp4DF,_tmp4DF->type),
_tmp85C)))))),((_tmp85B->tl=_tmp463,_tmp85B)))));struct _tuple15 _tmp85D;return(
_tmp85D.f1=Cyc_CfFlowInfo_ReachableFL(_tmp462,_tmp4F2),((_tmp85D.f2=_tmp4B6,
_tmp85D)));}_LL2A8: {struct Cyc_Absyn_Const_e_struct*_tmp4E0=(struct Cyc_Absyn_Const_e_struct*)
_tmp4CF;if(_tmp4E0->tag != 0)goto _LL2AA;else{_tmp4E1=_tmp4E0->f1;if((_tmp4E1.Int_c).tag
!= 4)goto _LL2AA;_tmp4E2=(struct _tuple6)(_tmp4E1.Int_c).val;_tmp4E3=_tmp4E2.f2;}}
_LL2A9: _tmp4EB=_tmp4E3;goto _LL2AB;_LL2AA: {struct Cyc_Absyn_Cast_e_struct*_tmp4E4=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4CF;if(_tmp4E4->tag != 15)goto _LL2AC;else{
_tmp4E5=_tmp4E4->f2;_tmp4E6=*_tmp4E5;_tmp4E7=_tmp4E6.r;{struct Cyc_Absyn_Const_e_struct*
_tmp4E8=(struct Cyc_Absyn_Const_e_struct*)_tmp4E7;if(_tmp4E8->tag != 0)goto _LL2AC;
else{_tmp4E9=_tmp4E8->f1;if((_tmp4E9.Int_c).tag != 4)goto _LL2AC;_tmp4EA=(struct
_tuple6)(_tmp4E9.Int_c).val;_tmp4EB=_tmp4EA.f2;}};}}_LL2AB: {struct _RegionHandle*
_tmp4F6=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp860;struct Cyc_List_List*
_tmp85F;struct Cyc_List_List*_tmp4F7=(_tmp85F=_region_malloc(_tmp4F6,sizeof(*
_tmp85F)),((_tmp85F->hd=((_tmp860=_region_malloc(_tmp4F6,sizeof(*_tmp860)),((
_tmp860->vd=_tmp4CE,((_tmp860->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp4EB),_tmp860)))))),((_tmp85F->tl=_tmp463,_tmp85F)))));struct _tuple15 _tmp861;
return(_tmp861.f1=Cyc_CfFlowInfo_ReachableFL(_tmp462,_tmp4F7),((_tmp861.f2=
_tmp4B6,_tmp861)));}_LL2AC: {struct Cyc_Absyn_Primop_e_struct*_tmp4EC=(struct Cyc_Absyn_Primop_e_struct*)
_tmp4CF;if(_tmp4EC->tag != 3)goto _LL2AE;else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;
if(_tmp4EE == 0)goto _LL2AE;_tmp4EF=*_tmp4EE;_tmp4F0=(struct Cyc_Absyn_Exp*)_tmp4EF.hd;}}
_LL2AD: {void*_tmp4FB=_tmp4F0->r;void*_tmp4FD;struct Cyc_Absyn_Vardecl*_tmp4FF;
void*_tmp501;struct Cyc_Absyn_Vardecl*_tmp503;void*_tmp505;struct Cyc_Absyn_Vardecl*
_tmp507;void*_tmp509;struct Cyc_Absyn_Vardecl*_tmp50B;_LL2B1:{struct Cyc_Absyn_Var_e_struct*
_tmp4FC=(struct Cyc_Absyn_Var_e_struct*)_tmp4FB;if(_tmp4FC->tag != 1)goto _LL2B3;
else{_tmp4FD=(void*)_tmp4FC->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4FE=(struct
Cyc_Absyn_Global_b_struct*)_tmp4FD;if(_tmp4FE->tag != 1)goto _LL2B3;else{_tmp4FF=
_tmp4FE->f1;}};}}if(!(!_tmp4FF->escapes))goto _LL2B3;_LL2B2: _tmp503=_tmp4FF;goto
_LL2B4;_LL2B3:{struct Cyc_Absyn_Var_e_struct*_tmp500=(struct Cyc_Absyn_Var_e_struct*)
_tmp4FB;if(_tmp500->tag != 1)goto _LL2B5;else{_tmp501=(void*)_tmp500->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp502=(struct Cyc_Absyn_Local_b_struct*)_tmp501;if(
_tmp502->tag != 4)goto _LL2B5;else{_tmp503=_tmp502->f1;}};}}if(!(!_tmp503->escapes))
goto _LL2B5;_LL2B4: _tmp507=_tmp503;goto _LL2B6;_LL2B5:{struct Cyc_Absyn_Var_e_struct*
_tmp504=(struct Cyc_Absyn_Var_e_struct*)_tmp4FB;if(_tmp504->tag != 1)goto _LL2B7;
else{_tmp505=(void*)_tmp504->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp506=(struct Cyc_Absyn_Pat_b_struct*)
_tmp505;if(_tmp506->tag != 5)goto _LL2B7;else{_tmp507=_tmp506->f1;}};}}if(!(!
_tmp507->escapes))goto _LL2B7;_LL2B6: _tmp50B=_tmp507;goto _LL2B8;_LL2B7:{struct Cyc_Absyn_Var_e_struct*
_tmp508=(struct Cyc_Absyn_Var_e_struct*)_tmp4FB;if(_tmp508->tag != 1)goto _LL2B9;
else{_tmp509=(void*)_tmp508->f2;{struct Cyc_Absyn_Param_b_struct*_tmp50A=(struct
Cyc_Absyn_Param_b_struct*)_tmp509;if(_tmp50A->tag != 3)goto _LL2B9;else{_tmp50B=
_tmp50A->f1;}};}}if(!(!_tmp50B->escapes))goto _LL2B9;_LL2B8: {struct _RegionHandle*
_tmp50C=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp864;struct Cyc_List_List*
_tmp863;struct Cyc_List_List*_tmp50D=(_tmp863=_region_malloc(_tmp50C,sizeof(*
_tmp863)),((_tmp863->hd=((_tmp864=_region_malloc(_tmp50C,sizeof(*_tmp864)),((
_tmp864->vd=_tmp4CE,((_tmp864->rop=Cyc_CfFlowInfo_LessNumelts(_tmp50B),_tmp864)))))),((
_tmp863->tl=_tmp463,_tmp863)))));struct _tuple15 _tmp865;return(_tmp865.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp462,_tmp50D),((_tmp865.f2=_tmp4B6,_tmp865)));}_LL2B9:;_LL2BA: {struct
_tuple15 _tmp866;return(_tmp866.f1=_tmp4B5,((_tmp866.f2=_tmp4B6,_tmp866)));}
_LL2B0:;}_LL2AE:;_LL2AF: {struct _tuple15 _tmp867;return(_tmp867.f1=_tmp4B5,((
_tmp867.f2=_tmp4B6,_tmp867)));}_LL29F:;}_LL29D:;_LL29E: {struct _tuple15 _tmp868;
return(_tmp868.f1=_tmp4B5,((_tmp868.f2=_tmp4B6,_tmp868)));}_LL294:;};}case Cyc_Absyn_Lte:
_LL28E: {union Cyc_CfFlowInfo_FlowInfo _tmp514=f;union Cyc_CfFlowInfo_FlowInfo
_tmp515=f;{union Cyc_CfFlowInfo_FlowInfo _tmp516=_tmp514;int _tmp517;struct _tuple12
_tmp518;struct Cyc_Dict_Dict _tmp519;_LL2BD: if((_tmp516.BottomFL).tag != 1)goto
_LL2BF;_tmp517=(int)(_tmp516.BottomFL).val;_LL2BE: {struct Cyc_Core_Impossible_struct
_tmp86E;const char*_tmp86D;struct Cyc_Core_Impossible_struct*_tmp86C;(int)_throw((
void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86E.tag=Cyc_Core_Impossible,((
_tmp86E.f1=((_tmp86D="anal_test, Lte",_tag_dyneither(_tmp86D,sizeof(char),15))),
_tmp86E)))),_tmp86C)))));}_LL2BF: if((_tmp516.ReachableFL).tag != 2)goto _LL2BC;
_tmp518=(struct _tuple12)(_tmp516.ReachableFL).val;_tmp519=_tmp518.f1;_LL2C0:
_tmp462=_tmp519;_LL2BC:;}{void*_tmp51D=_tmp465->r;void*_tmp51F;struct Cyc_Absyn_Vardecl*
_tmp521;void*_tmp523;struct Cyc_Absyn_Vardecl*_tmp525;void*_tmp527;struct Cyc_Absyn_Vardecl*
_tmp529;void*_tmp52B;struct Cyc_Absyn_Vardecl*_tmp52D;_LL2C2:{struct Cyc_Absyn_Var_e_struct*
_tmp51E=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp51E->tag != 1)goto _LL2C4;
else{_tmp51F=(void*)_tmp51E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp520=(struct
Cyc_Absyn_Global_b_struct*)_tmp51F;if(_tmp520->tag != 1)goto _LL2C4;else{_tmp521=
_tmp520->f1;}};}}if(!(!_tmp521->escapes))goto _LL2C4;_LL2C3: _tmp525=_tmp521;goto
_LL2C5;_LL2C4:{struct Cyc_Absyn_Var_e_struct*_tmp522=(struct Cyc_Absyn_Var_e_struct*)
_tmp51D;if(_tmp522->tag != 1)goto _LL2C6;else{_tmp523=(void*)_tmp522->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp524=(struct Cyc_Absyn_Local_b_struct*)_tmp523;if(
_tmp524->tag != 4)goto _LL2C6;else{_tmp525=_tmp524->f1;}};}}if(!(!_tmp525->escapes))
goto _LL2C6;_LL2C5: _tmp529=_tmp525;goto _LL2C7;_LL2C6:{struct Cyc_Absyn_Var_e_struct*
_tmp526=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp526->tag != 1)goto _LL2C8;
else{_tmp527=(void*)_tmp526->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp528=(struct Cyc_Absyn_Pat_b_struct*)
_tmp527;if(_tmp528->tag != 5)goto _LL2C8;else{_tmp529=_tmp528->f1;}};}}if(!(!
_tmp529->escapes))goto _LL2C8;_LL2C7: _tmp52D=_tmp529;goto _LL2C9;_LL2C8:{struct Cyc_Absyn_Var_e_struct*
_tmp52A=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp52A->tag != 1)goto _LL2CA;
else{_tmp52B=(void*)_tmp52A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52C=(struct
Cyc_Absyn_Param_b_struct*)_tmp52B;if(_tmp52C->tag != 3)goto _LL2CA;else{_tmp52D=
_tmp52C->f1;}};}}if(!(!_tmp52D->escapes))goto _LL2CA;_LL2C9: {void*_tmp52E=
_tmp466->r;union Cyc_Absyn_Cnst _tmp530;struct _tuple6 _tmp531;int _tmp532;struct Cyc_Absyn_Exp*
_tmp534;struct Cyc_Absyn_Exp _tmp535;void*_tmp536;union Cyc_Absyn_Cnst _tmp538;
struct _tuple6 _tmp539;int _tmp53A;enum Cyc_Absyn_Primop _tmp53C;struct Cyc_List_List*
_tmp53D;struct Cyc_List_List _tmp53E;struct Cyc_Absyn_Exp*_tmp53F;_LL2CD: {struct
Cyc_Absyn_Const_e_struct*_tmp52F=(struct Cyc_Absyn_Const_e_struct*)_tmp52E;if(
_tmp52F->tag != 0)goto _LL2CF;else{_tmp530=_tmp52F->f1;if((_tmp530.Int_c).tag != 4)
goto _LL2CF;_tmp531=(struct _tuple6)(_tmp530.Int_c).val;_tmp532=_tmp531.f2;}}
_LL2CE: _tmp53A=_tmp532;goto _LL2D0;_LL2CF: {struct Cyc_Absyn_Cast_e_struct*_tmp533=(
struct Cyc_Absyn_Cast_e_struct*)_tmp52E;if(_tmp533->tag != 15)goto _LL2D1;else{
_tmp534=_tmp533->f2;_tmp535=*_tmp534;_tmp536=_tmp535.r;{struct Cyc_Absyn_Const_e_struct*
_tmp537=(struct Cyc_Absyn_Const_e_struct*)_tmp536;if(_tmp537->tag != 0)goto _LL2D1;
else{_tmp538=_tmp537->f1;if((_tmp538.Int_c).tag != 4)goto _LL2D1;_tmp539=(struct
_tuple6)(_tmp538.Int_c).val;_tmp53A=_tmp539.f2;}};}}_LL2D0: {struct _RegionHandle*
_tmp540=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp871;struct Cyc_List_List*
_tmp870;struct Cyc_List_List*_tmp541=(_tmp870=_region_malloc(_tmp540,sizeof(*
_tmp870)),((_tmp870->hd=((_tmp871=_region_malloc(_tmp540,sizeof(*_tmp871)),((
_tmp871->vd=_tmp52D,((_tmp871->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp53A),_tmp871)))))),((_tmp870->tl=_tmp463,_tmp870)))));struct _tuple15 _tmp872;
return(_tmp872.f1=Cyc_CfFlowInfo_ReachableFL(_tmp462,_tmp541),((_tmp872.f2=
_tmp515,_tmp872)));}_LL2D1: {struct Cyc_Absyn_Primop_e_struct*_tmp53B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp52E;if(_tmp53B->tag != 3)goto _LL2D3;else{_tmp53C=_tmp53B->f1;_tmp53D=_tmp53B->f2;
if(_tmp53D == 0)goto _LL2D3;_tmp53E=*_tmp53D;_tmp53F=(struct Cyc_Absyn_Exp*)_tmp53E.hd;}}
_LL2D2: {void*_tmp545=_tmp53F->r;void*_tmp547;struct Cyc_Absyn_Vardecl*_tmp549;
void*_tmp54B;struct Cyc_Absyn_Vardecl*_tmp54D;void*_tmp54F;struct Cyc_Absyn_Vardecl*
_tmp551;void*_tmp553;struct Cyc_Absyn_Vardecl*_tmp555;_LL2D6:{struct Cyc_Absyn_Var_e_struct*
_tmp546=(struct Cyc_Absyn_Var_e_struct*)_tmp545;if(_tmp546->tag != 1)goto _LL2D8;
else{_tmp547=(void*)_tmp546->f2;{struct Cyc_Absyn_Global_b_struct*_tmp548=(struct
Cyc_Absyn_Global_b_struct*)_tmp547;if(_tmp548->tag != 1)goto _LL2D8;else{_tmp549=
_tmp548->f1;}};}}if(!(!_tmp549->escapes))goto _LL2D8;_LL2D7: _tmp54D=_tmp549;goto
_LL2D9;_LL2D8:{struct Cyc_Absyn_Var_e_struct*_tmp54A=(struct Cyc_Absyn_Var_e_struct*)
_tmp545;if(_tmp54A->tag != 1)goto _LL2DA;else{_tmp54B=(void*)_tmp54A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp54C=(struct Cyc_Absyn_Local_b_struct*)_tmp54B;if(
_tmp54C->tag != 4)goto _LL2DA;else{_tmp54D=_tmp54C->f1;}};}}if(!(!_tmp54D->escapes))
goto _LL2DA;_LL2D9: _tmp551=_tmp54D;goto _LL2DB;_LL2DA:{struct Cyc_Absyn_Var_e_struct*
_tmp54E=(struct Cyc_Absyn_Var_e_struct*)_tmp545;if(_tmp54E->tag != 1)goto _LL2DC;
else{_tmp54F=(void*)_tmp54E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp550=(struct Cyc_Absyn_Pat_b_struct*)
_tmp54F;if(_tmp550->tag != 5)goto _LL2DC;else{_tmp551=_tmp550->f1;}};}}if(!(!
_tmp551->escapes))goto _LL2DC;_LL2DB: _tmp555=_tmp551;goto _LL2DD;_LL2DC:{struct Cyc_Absyn_Var_e_struct*
_tmp552=(struct Cyc_Absyn_Var_e_struct*)_tmp545;if(_tmp552->tag != 1)goto _LL2DE;
else{_tmp553=(void*)_tmp552->f2;{struct Cyc_Absyn_Param_b_struct*_tmp554=(struct
Cyc_Absyn_Param_b_struct*)_tmp553;if(_tmp554->tag != 3)goto _LL2DE;else{_tmp555=
_tmp554->f1;}};}}if(!(!_tmp555->escapes))goto _LL2DE;_LL2DD: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp556=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp875;struct Cyc_List_List*_tmp874;
struct Cyc_List_List*_tmp557=(_tmp874=_region_malloc(_tmp556->r,sizeof(*_tmp874)),((
_tmp874->hd=((_tmp875=_region_malloc(_tmp556->r,sizeof(*_tmp875)),((_tmp875->vd=
_tmp52D,((_tmp875->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp555),_tmp875)))))),((
_tmp874->tl=_tmp463,_tmp874)))));struct _tuple15 _tmp876;return(_tmp876.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp462,_tmp557),((_tmp876.f2=_tmp515,_tmp876)));}_LL2DE:;_LL2DF: {struct
_tuple15 _tmp877;return(_tmp877.f1=_tmp514,((_tmp877.f2=_tmp515,_tmp877)));}
_LL2D5:;}_LL2D3:;_LL2D4: {struct _tuple15 _tmp878;return(_tmp878.f1=_tmp514,((
_tmp878.f2=_tmp515,_tmp878)));}_LL2CC:;}_LL2CA:;_LL2CB: {struct _tuple15 _tmp879;
return(_tmp879.f1=_tmp514,((_tmp879.f2=_tmp515,_tmp879)));}_LL2C1:;};}default:
_LL2BB: {struct _tuple15 _tmp87A;return(_tmp87A.f1=f,((_tmp87A.f2=f,_tmp87A)));}}}
_LL25C:;};}_LL25A:;_LL25B: goto _LL24D;_LL24D:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp560;void*_tmp561;struct _tuple13 _tmp55F=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp560=_tmp55F.f1;_tmp561=_tmp55F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp562=_tmp560;int _tmp563;struct _tuple12 _tmp564;struct Cyc_Dict_Dict _tmp565;
_LL2E2: if((_tmp562.BottomFL).tag != 1)goto _LL2E4;_tmp563=(int)(_tmp562.BottomFL).val;
_LL2E3: {struct _tuple15 _tmp87B;return(_tmp87B.f1=_tmp560,((_tmp87B.f2=_tmp560,
_tmp87B)));}_LL2E4: if((_tmp562.ReachableFL).tag != 2)goto _LL2E1;_tmp564=(struct
_tuple12)(_tmp562.ReachableFL).val;_tmp565=_tmp564.f1;_LL2E5: {void*_tmp567=
_tmp561;enum Cyc_CfFlowInfo_InitLevel _tmp56D;enum Cyc_CfFlowInfo_InitLevel
_tmp56F;enum Cyc_CfFlowInfo_InitLevel _tmp572;_LL2E7: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp568=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp567;if(_tmp568->tag != 0)goto
_LL2E9;}_LL2E8: {struct _tuple15 _tmp87C;return(_tmp87C.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp87C.f2=_tmp560,_tmp87C)));}_LL2E9: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp569=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp567;if(_tmp569->tag != 2)
goto _LL2EB;}_LL2EA: goto _LL2EC;_LL2EB: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp56A=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp567;if(_tmp56A->tag != 1)
goto _LL2ED;}_LL2EC: goto _LL2EE;_LL2ED: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp56B=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp567;if(_tmp56B->tag != 5)goto
_LL2EF;}_LL2EE: {struct _tuple15 _tmp87D;return(_tmp87D.f1=_tmp560,((_tmp87D.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp87D)));}_LL2EF: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp56C=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp567;if(_tmp56C->tag != 3)goto
_LL2F1;else{_tmp56D=_tmp56C->f1;if(_tmp56D != Cyc_CfFlowInfo_NoneIL)goto _LL2F1;}}
_LL2F0: goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_Esc_struct*_tmp56E=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp567;if(_tmp56E->tag != 4)goto _LL2F3;else{_tmp56F=_tmp56E->f1;if(_tmp56F != Cyc_CfFlowInfo_NoneIL)
goto _LL2F3;}}_LL2F2: goto _LL2F4;_LL2F3: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp570=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp567;if(_tmp570->tag != 7)goto
_LL2F5;}_LL2F4:{const char*_tmp880;void*_tmp87F;(_tmp87F=0,Cyc_Tcutil_terr(e->loc,((
_tmp880="expression may not be initialized",_tag_dyneither(_tmp880,sizeof(char),
34))),_tag_dyneither(_tmp87F,sizeof(void*),0)));}{struct _tuple15 _tmp881;return(
_tmp881.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp881.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp881)));};_LL2F5: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp571=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp567;if(_tmp571->tag != 3)goto _LL2F7;else{_tmp572=_tmp571->f1;}}_LL2F6: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp560,e,_tmp572);_LL2F7: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp573=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp567;if(_tmp573->tag != 4)goto _LL2F9;}
_LL2F8: {struct _tuple15 _tmp882;return(_tmp882.f1=_tmp560,((_tmp882.f2=_tmp560,
_tmp882)));}_LL2F9: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp574=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp567;if(_tmp574->tag != 6)goto _LL2E6;}_LL2FA: {struct Cyc_Core_Impossible_struct
_tmp888;const char*_tmp887;struct Cyc_Core_Impossible_struct*_tmp886;(int)_throw((
void*)((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886[0]=((_tmp888.tag=Cyc_Core_Impossible,((
_tmp888.f1=((_tmp887="anal_test",_tag_dyneither(_tmp887,sizeof(char),10))),
_tmp888)))),_tmp886)))));}_LL2E6:;}_LL2E1:;};};}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union
Cyc_CfFlowInfo_FlowInfo _tmp57E=flow;int _tmp57F;struct _tuple12 _tmp580;struct Cyc_Dict_Dict
_tmp581;_LL2FC: if((_tmp57E.BottomFL).tag != 1)goto _LL2FE;_tmp57F=(int)(_tmp57E.BottomFL).val;
_LL2FD: return;_LL2FE: if((_tmp57E.ReachableFL).tag != 2)goto _LL2FB;_tmp580=(struct
_tuple12)(_tmp57E.ReachableFL).val;_tmp581=_tmp580.f1;_LL2FF:{struct Cyc_List_List*
_tmp582=env->param_roots;for(0;_tmp582 != 0;_tmp582=_tmp582->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp581,Cyc_CfFlowInfo_lookup_place(_tmp581,(struct Cyc_CfFlowInfo_Place*)
_tmp582->hd))!= Cyc_CfFlowInfo_AllIL){const char*_tmp88B;void*_tmp88A;(_tmp88A=0,
Cyc_Tcutil_terr(loc,((_tmp88B="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp88B,sizeof(char),76))),_tag_dyneither(_tmp88A,sizeof(void*),0)));}}}
return;_LL2FB:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp585=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp587;struct Cyc_Core_Opt*_tmp588;
struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Position_Segment*
_tmp58B;struct Cyc_Absyn_Switch_clause*_tmp586=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp587=*_tmp586;_tmp588=_tmp587.pat_vars;_tmp589=_tmp587.where_clause;
_tmp58A=_tmp587.body;_tmp58B=_tmp587.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp585,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp588))->v,_tmp585->unknown_all,_tmp58B);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp589 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp589;union Cyc_CfFlowInfo_FlowInfo _tmp58D;union Cyc_CfFlowInfo_FlowInfo
_tmp58E;struct _tuple15 _tmp58C=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp58D=_tmp58C.f1;_tmp58E=_tmp58C.f2;inflow=_tmp58E;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp58D,_tmp58A);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp58A);}{union Cyc_CfFlowInfo_FlowInfo _tmp58F=clause_outflow;int
_tmp590;_LL301: if((_tmp58F.BottomFL).tag != 1)goto _LL303;_tmp590=(int)(_tmp58F.BottomFL).val;
_LL302: goto _LL300;_LL303:;_LL304: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp88E;void*_tmp88D;(_tmp88D=0,Cyc_Tcutil_terr(_tmp58A->loc,((_tmp88E="switch clause may implicitly fallthru",
_tag_dyneither(_tmp88E,sizeof(char),38))),_tag_dyneither(_tmp88D,sizeof(void*),0)));}
else{const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_Tcutil_warn(_tmp58A->loc,((
_tmp891="switch clause may implicitly fallthru",_tag_dyneither(_tmp891,sizeof(
char),38))),_tag_dyneither(_tmp890,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL300;_LL300:;};};}return Cyc_CfFlowInfo_BottomFL();}static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp596;union Cyc_CfFlowInfo_FlowInfo*_tmp597;struct _tuple16 _tmp595=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;inflow=*_tmp597;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp598=env->fenv;{void*_tmp599=s->r;struct Cyc_Absyn_Exp*_tmp59C;struct Cyc_Absyn_Exp*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;struct Cyc_Absyn_Stmt*_tmp5A2;struct Cyc_Absyn_Stmt*
_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Stmt*_tmp5A6;struct Cyc_Absyn_Stmt*
_tmp5A7;struct _tuple9 _tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Stmt*
_tmp5AB;struct Cyc_Absyn_Stmt*_tmp5AC;struct Cyc_Absyn_Stmt*_tmp5AE;struct _tuple9
_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Stmt*_tmp5B1;struct Cyc_Absyn_Exp*
_tmp5B3;struct _tuple9 _tmp5B4;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Stmt*
_tmp5B6;struct _tuple9 _tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Stmt*
_tmp5B9;struct Cyc_Absyn_Stmt*_tmp5BA;struct Cyc_Absyn_Stmt*_tmp5BC;struct Cyc_List_List*
_tmp5BE;struct Cyc_Absyn_Switch_clause**_tmp5BF;struct Cyc_Absyn_Switch_clause*
_tmp5C0;struct Cyc_Absyn_Stmt*_tmp5C2;struct Cyc_Absyn_Stmt*_tmp5C4;struct Cyc_Absyn_Stmt*
_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_List_List*_tmp5C9;struct Cyc_Absyn_Stmt*
_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_Absyn_Decl*_tmp5CE;struct Cyc_Absyn_Decl
_tmp5CF;void*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Tvar*_tmp5D3;
struct Cyc_Absyn_Vardecl*_tmp5D4;struct Cyc_Absyn_Stmt*_tmp5D5;struct Cyc_Absyn_Decl*
_tmp5D7;struct Cyc_Absyn_Stmt*_tmp5D8;struct Cyc_Absyn_Stmt*_tmp5DA;struct Cyc_Absyn_Exp*
_tmp5DC;_LL306: {struct Cyc_Absyn_Skip_s_struct*_tmp59A=(struct Cyc_Absyn_Skip_s_struct*)
_tmp599;if(_tmp59A->tag != 0)goto _LL308;}_LL307: return inflow;_LL308: {struct Cyc_Absyn_Return_s_struct*
_tmp59B=(struct Cyc_Absyn_Return_s_struct*)_tmp599;if(_tmp59B->tag != 3)goto _LL30A;
else{_tmp59C=_tmp59B->f1;if(_tmp59C != 0)goto _LL30A;}}_LL309: if(env->noreturn){
const char*_tmp894;void*_tmp893;(_tmp893=0,Cyc_Tcutil_terr(s->loc,((_tmp894="`noreturn' function might return",
_tag_dyneither(_tmp894,sizeof(char),33))),_tag_dyneither(_tmp893,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL30A: {struct Cyc_Absyn_Return_s_struct*_tmp59D=(struct Cyc_Absyn_Return_s_struct*)
_tmp599;if(_tmp59D->tag != 3)goto _LL30C;else{_tmp59E=_tmp59D->f1;}}_LL30B: if(env->noreturn){
const char*_tmp897;void*_tmp896;(_tmp896=0,Cyc_Tcutil_terr(s->loc,((_tmp897="`noreturn' function might return",
_tag_dyneither(_tmp897,sizeof(char),33))),_tag_dyneither(_tmp896,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp5E2;void*_tmp5E3;struct _tuple13 _tmp5E1=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp59E));_tmp5E2=_tmp5E1.f1;
_tmp5E3=_tmp5E1.f2;_tmp5E2=Cyc_NewControlFlow_use_Rval(env,_tmp59E->loc,_tmp5E2,
_tmp5E3);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5E2);return Cyc_CfFlowInfo_BottomFL();};
_LL30C: {struct Cyc_Absyn_Exp_s_struct*_tmp59F=(struct Cyc_Absyn_Exp_s_struct*)
_tmp599;if(_tmp59F->tag != 1)goto _LL30E;else{_tmp5A0=_tmp59F->f1;}}_LL30D: outflow=(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5A0)).f1;goto _LL305;_LL30E: {
struct Cyc_Absyn_Seq_s_struct*_tmp5A1=(struct Cyc_Absyn_Seq_s_struct*)_tmp599;if(
_tmp5A1->tag != 2)goto _LL310;else{_tmp5A2=_tmp5A1->f1;_tmp5A3=_tmp5A1->f2;}}
_LL30F: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp5A2),_tmp5A3);goto _LL305;_LL310: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp5A4=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp599;if(_tmp5A4->tag != 4)goto
_LL312;else{_tmp5A5=_tmp5A4->f1;_tmp5A6=_tmp5A4->f2;_tmp5A7=_tmp5A4->f3;}}_LL311: {
union Cyc_CfFlowInfo_FlowInfo _tmp5E5;union Cyc_CfFlowInfo_FlowInfo _tmp5E6;struct
_tuple15 _tmp5E4=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5A5);_tmp5E5=_tmp5E4.f1;
_tmp5E6=_tmp5E4.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5E7=Cyc_NewControlFlow_anal_stmt(
env,_tmp5E6,_tmp5A7);union Cyc_CfFlowInfo_FlowInfo _tmp5E8=Cyc_NewControlFlow_anal_stmt(
env,_tmp5E5,_tmp5A6);outflow=Cyc_CfFlowInfo_join_flow(_tmp598,env->all_changed,
_tmp5E8,_tmp5E7);goto _LL305;};}_LL312: {struct Cyc_Absyn_While_s_struct*_tmp5A8=(
struct Cyc_Absyn_While_s_struct*)_tmp599;if(_tmp5A8->tag != 5)goto _LL314;else{
_tmp5A9=_tmp5A8->f1;_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;_tmp5AC=_tmp5A8->f2;}}
_LL313: {union Cyc_CfFlowInfo_FlowInfo*_tmp5EA;struct _tuple16 _tmp5E9=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp5AB);_tmp5EA=_tmp5E9.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5EB=*
_tmp5EA;union Cyc_CfFlowInfo_FlowInfo _tmp5ED;union Cyc_CfFlowInfo_FlowInfo _tmp5EE;
struct _tuple15 _tmp5EC=Cyc_NewControlFlow_anal_test(env,_tmp5EB,_tmp5AA);_tmp5ED=
_tmp5EC.f1;_tmp5EE=_tmp5EC.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5EF=Cyc_NewControlFlow_anal_stmt(
env,_tmp5ED,_tmp5AC);Cyc_NewControlFlow_update_flow(env,_tmp5AB,_tmp5EF);outflow=
_tmp5EE;goto _LL305;};};}_LL314: {struct Cyc_Absyn_Do_s_struct*_tmp5AD=(struct Cyc_Absyn_Do_s_struct*)
_tmp599;if(_tmp5AD->tag != 14)goto _LL316;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AD->f2;
_tmp5B0=_tmp5AF.f1;_tmp5B1=_tmp5AF.f2;}}_LL315: {union Cyc_CfFlowInfo_FlowInfo
_tmp5F0=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5AE);union Cyc_CfFlowInfo_FlowInfo*
_tmp5F2;struct _tuple16 _tmp5F1=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5F0,
_tmp5B1);_tmp5F2=_tmp5F1.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5F3=*_tmp5F2;union
Cyc_CfFlowInfo_FlowInfo _tmp5F5;union Cyc_CfFlowInfo_FlowInfo _tmp5F6;struct
_tuple15 _tmp5F4=Cyc_NewControlFlow_anal_test(env,_tmp5F3,_tmp5B0);_tmp5F5=
_tmp5F4.f1;_tmp5F6=_tmp5F4.f2;Cyc_NewControlFlow_update_flow(env,_tmp5AE,_tmp5F5);
outflow=_tmp5F6;goto _LL305;};}_LL316: {struct Cyc_Absyn_For_s_struct*_tmp5B2=(
struct Cyc_Absyn_For_s_struct*)_tmp599;if(_tmp5B2->tag != 9)goto _LL318;else{
_tmp5B3=_tmp5B2->f1;_tmp5B4=_tmp5B2->f2;_tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;
_tmp5B7=_tmp5B2->f3;_tmp5B8=_tmp5B7.f1;_tmp5B9=_tmp5B7.f2;_tmp5BA=_tmp5B2->f4;}}
_LL317: {union Cyc_CfFlowInfo_FlowInfo _tmp5F7=(Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp5B3)).f1;union Cyc_CfFlowInfo_FlowInfo*_tmp5F9;struct _tuple16 _tmp5F8=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp5F7,_tmp5B6);_tmp5F9=_tmp5F8.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp5FA=*_tmp5F9;union Cyc_CfFlowInfo_FlowInfo _tmp5FC;
union Cyc_CfFlowInfo_FlowInfo _tmp5FD;struct _tuple15 _tmp5FB=Cyc_NewControlFlow_anal_test(
env,_tmp5FA,_tmp5B5);_tmp5FC=_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5FE=Cyc_NewControlFlow_anal_stmt(env,_tmp5FC,_tmp5BA);union Cyc_CfFlowInfo_FlowInfo*
_tmp600;struct _tuple16 _tmp5FF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5FE,
_tmp5B9);_tmp600=_tmp5FF.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp601=*_tmp600;union
Cyc_CfFlowInfo_FlowInfo _tmp602=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp601,
_tmp5B8)).f1;Cyc_NewControlFlow_update_flow(env,_tmp5B6,_tmp602);outflow=_tmp5FD;
goto _LL305;};};};}_LL318: {struct Cyc_Absyn_Break_s_struct*_tmp5BB=(struct Cyc_Absyn_Break_s_struct*)
_tmp599;if(_tmp5BB->tag != 6)goto _LL31A;else{_tmp5BC=_tmp5BB->f1;if(_tmp5BC != 0)
goto _LL31A;}}_LL319: return Cyc_CfFlowInfo_BottomFL();_LL31A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp5BD=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp599;if(_tmp5BD->tag != 11)goto
_LL31C;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;if(_tmp5BF == 0)goto _LL31C;
_tmp5C0=*_tmp5BF;}}_LL31B: {struct _RegionHandle*_tmp603=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp605;struct Cyc_List_List*_tmp606;struct _tuple18 _tmp604=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp603,env,inflow,_tmp5BE,1,1);_tmp605=_tmp604.f1;_tmp606=_tmp604.f2;for(0;
_tmp606 != 0;(_tmp606=_tmp606->tl,_tmp5BE=_tmp5BE->tl)){_tmp605=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BE))->hd)->loc,
_tmp605,(void*)_tmp606->hd);}_tmp605=Cyc_NewControlFlow_add_vars(_tmp598,_tmp605,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C0->pat_vars))->v,
_tmp598->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp5C0->body,_tmp605);return Cyc_CfFlowInfo_BottomFL();}_LL31C: {struct Cyc_Absyn_Break_s_struct*
_tmp5C1=(struct Cyc_Absyn_Break_s_struct*)_tmp599;if(_tmp5C1->tag != 6)goto _LL31E;
else{_tmp5C2=_tmp5C1->f1;}}_LL31D: _tmp5C4=_tmp5C2;goto _LL31F;_LL31E: {struct Cyc_Absyn_Continue_s_struct*
_tmp5C3=(struct Cyc_Absyn_Continue_s_struct*)_tmp599;if(_tmp5C3->tag != 7)goto
_LL320;else{_tmp5C4=_tmp5C3->f1;}}_LL31F: _tmp5C6=_tmp5C4;goto _LL321;_LL320: {
struct Cyc_Absyn_Goto_s_struct*_tmp5C5=(struct Cyc_Absyn_Goto_s_struct*)_tmp599;
if(_tmp5C5->tag != 8)goto _LL322;else{_tmp5C6=_tmp5C5->f2;}}_LL321: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp607=_tmp596->encloser;struct Cyc_Absyn_Stmt*
_tmp608=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp5C6)))->encloser;while(_tmp608 != _tmp607){struct Cyc_Absyn_Stmt*_tmp609=(Cyc_NewControlFlow_get_stmt_annot(
_tmp607))->encloser;if(_tmp609 == _tmp607){{const char*_tmp89A;void*_tmp899;(
_tmp899=0,Cyc_Tcutil_terr(s->loc,((_tmp89A="goto enters local scope or exception handler",
_tag_dyneither(_tmp89A,sizeof(char),45))),_tag_dyneither(_tmp899,sizeof(void*),0)));}
break;}_tmp607=_tmp609;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp5C6),inflow);return Cyc_CfFlowInfo_BottomFL();_LL322: {struct Cyc_Absyn_Switch_s_struct*
_tmp5C7=(struct Cyc_Absyn_Switch_s_struct*)_tmp599;if(_tmp5C7->tag != 10)goto
_LL324;else{_tmp5C8=_tmp5C7->f1;_tmp5C9=_tmp5C7->f2;}}_LL323: {union Cyc_CfFlowInfo_FlowInfo
_tmp60D;void*_tmp60E;struct _tuple13 _tmp60C=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp5C8);_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;_tmp60D=Cyc_NewControlFlow_use_Rval(
env,_tmp5C8->loc,_tmp60D,_tmp60E);outflow=Cyc_NewControlFlow_anal_scs(env,
_tmp60D,_tmp5C9);goto _LL305;}_LL324: {struct Cyc_Absyn_TryCatch_s_struct*_tmp5CA=(
struct Cyc_Absyn_TryCatch_s_struct*)_tmp599;if(_tmp5CA->tag != 15)goto _LL326;else{
_tmp5CB=_tmp5CA->f1;_tmp5CC=_tmp5CA->f2;}}_LL325: {int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;env->in_try=1;env->tryflow=
inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp5CB);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp5CC);{union Cyc_CfFlowInfo_FlowInfo _tmp60F=scs_outflow;int
_tmp610;_LL331: if((_tmp60F.BottomFL).tag != 1)goto _LL333;_tmp610=(int)(_tmp60F.BottomFL).val;
_LL332: goto _LL330;_LL333:;_LL334: {const char*_tmp89D;void*_tmp89C;(_tmp89C=0,Cyc_Tcutil_terr(
s->loc,((_tmp89D="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp89D,sizeof(char),42))),_tag_dyneither(_tmp89C,sizeof(void*),0)));}_LL330:;}
outflow=s1_outflow;goto _LL305;};};}_LL326: {struct Cyc_Absyn_Decl_s_struct*
_tmp5CD=(struct Cyc_Absyn_Decl_s_struct*)_tmp599;if(_tmp5CD->tag != 12)goto _LL328;
else{_tmp5CE=_tmp5CD->f1;_tmp5CF=*_tmp5CE;_tmp5D0=_tmp5CF.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp5D1=(struct Cyc_Absyn_Alias_d_struct*)_tmp5D0;if(_tmp5D1->tag != 5)goto _LL328;
else{_tmp5D2=_tmp5D1->f1;_tmp5D3=_tmp5D1->f2;_tmp5D4=_tmp5D1->f3;}};_tmp5D5=
_tmp5CD->f2;}}_LL327: {union Cyc_CfFlowInfo_FlowInfo _tmp614;union Cyc_CfFlowInfo_AbsLVal
_tmp615;struct _tuple14 _tmp613=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp5D2);
_tmp614=_tmp613.f1;_tmp615=_tmp613.f2;{struct _tuple14 _tmp89E;struct _tuple14
_tmp617=(_tmp89E.f1=_tmp614,((_tmp89E.f2=_tmp615,_tmp89E)));union Cyc_CfFlowInfo_FlowInfo
_tmp618;struct _tuple12 _tmp619;struct Cyc_Dict_Dict _tmp61A;struct Cyc_List_List*
_tmp61B;union Cyc_CfFlowInfo_AbsLVal _tmp61C;struct Cyc_CfFlowInfo_Place*_tmp61D;
union Cyc_CfFlowInfo_FlowInfo _tmp61E;int _tmp61F;_LL336: _tmp618=_tmp617.f1;if((
_tmp618.ReachableFL).tag != 2)goto _LL338;_tmp619=(struct _tuple12)(_tmp618.ReachableFL).val;
_tmp61A=_tmp619.f1;_tmp61B=_tmp619.f2;_tmp61C=_tmp617.f2;if((_tmp61C.PlaceL).tag
!= 1)goto _LL338;_tmp61D=(struct Cyc_CfFlowInfo_Place*)(_tmp61C.PlaceL).val;_LL337: {
void*_tmp620=Cyc_CfFlowInfo_lookup_place(_tmp61A,_tmp61D);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp5D2->topt))->v;{void*_tmp621=_tmp5D2->r;struct Cyc_Absyn_Exp*
_tmp623;_LL33D: {struct Cyc_Absyn_Cast_e_struct*_tmp622=(struct Cyc_Absyn_Cast_e_struct*)
_tmp621;if(_tmp622->tag != 15)goto _LL33F;else{_tmp623=_tmp622->f2;}}_LL33E: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp623->topt))->v;goto _LL33C;_LL33F:;
_LL340: goto _LL33C;_LL33C:;}{void*_tmp624=Cyc_CfFlowInfo_make_unique_consumed(
_tmp598,t,_tmp5D2,env->iteration_num,_tmp620);_tmp61A=Cyc_CfFlowInfo_assign_place(
_tmp598,_tmp5D2->loc,_tmp61A,env->all_changed,_tmp61D,_tmp624);_tmp614=Cyc_CfFlowInfo_ReachableFL(
_tmp61A,_tmp61B);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp598,_tmp61A,
_tmp620)){case Cyc_CfFlowInfo_AllIL: _LL341: leaf=_tmp598->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL342: leaf=_tmp598->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL343: leaf=_tmp598->unknown_this;break;}{void*_tmp625=Cyc_CfFlowInfo_typ_to_absrval(
_tmp598,t,leaf);_tmp614=Cyc_NewControlFlow_use_Rval(env,_tmp5D2->loc,_tmp614,
_tmp620);{struct Cyc_List_List _tmp89F;struct Cyc_List_List _tmp626=(_tmp89F.hd=
_tmp5D4,((_tmp89F.tl=0,_tmp89F)));_tmp614=Cyc_NewControlFlow_add_vars(_tmp598,
_tmp614,(struct Cyc_List_List*)& _tmp626,_tmp598->unknown_all,s->loc);_tmp614=Cyc_NewControlFlow_anal_stmt(
env,_tmp614,_tmp5D5);{union Cyc_CfFlowInfo_FlowInfo _tmp627=_tmp614;struct _tuple12
_tmp628;struct Cyc_Dict_Dict _tmp629;struct Cyc_List_List*_tmp62A;_LL346: if((
_tmp627.ReachableFL).tag != 2)goto _LL348;_tmp628=(struct _tuple12)(_tmp627.ReachableFL).val;
_tmp629=_tmp628.f1;_tmp62A=_tmp628.f2;_LL347: _tmp629=Cyc_CfFlowInfo_assign_place(
_tmp598,s->loc,_tmp629,env->all_changed,_tmp61D,_tmp625);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp629,_tmp62A);return outflow;_LL348:;_LL349: return _tmp614;_LL345:;};};};};};}
_LL338: _tmp61E=_tmp617.f1;if((_tmp61E.BottomFL).tag != 1)goto _LL33A;_tmp61F=(int)(
_tmp61E.BottomFL).val;_LL339: return _tmp614;_LL33A:;_LL33B:{const char*_tmp8A2;
void*_tmp8A1;(_tmp8A1=0,Cyc_Tcutil_terr(_tmp5D2->loc,((_tmp8A2="bad alias expression--no unique path found",
_tag_dyneither(_tmp8A2,sizeof(char),43))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL335:;};}_LL328: {struct Cyc_Absyn_Decl_s_struct*
_tmp5D6=(struct Cyc_Absyn_Decl_s_struct*)_tmp599;if(_tmp5D6->tag != 12)goto _LL32A;
else{_tmp5D7=_tmp5D6->f1;_tmp5D8=_tmp5D6->f2;}}_LL329: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5D7),_tmp5D8);goto _LL305;_LL32A: {
struct Cyc_Absyn_Label_s_struct*_tmp5D9=(struct Cyc_Absyn_Label_s_struct*)_tmp599;
if(_tmp5D9->tag != 13)goto _LL32C;else{_tmp5DA=_tmp5D9->f2;}}_LL32B: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp5DA);goto _LL305;_LL32C: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp5DB=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp599;if(_tmp5DB->tag != 16)goto
_LL32E;else{_tmp5DC=_tmp5DB->f1;}}_LL32D: {union Cyc_CfFlowInfo_FlowInfo _tmp62F;
void*_tmp630;struct _tuple13 _tmp62E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp5DC);_tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp631=Cyc_NewControlFlow_use_Rval(env,_tmp5DC->loc,_tmp62F,_tmp630);{void*
_tmp632=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp5DC->topt))->v);
void*_tmp634;_LL34B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp633=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp632;if(_tmp633->tag != 16)goto _LL34D;else{_tmp634=(void*)_tmp633->f1;}}_LL34C:
outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp598,_tmp62F,_tmp634);goto _LL34A;
_LL34D:;_LL34E: {struct Cyc_Core_Impossible_struct _tmp8A8;const char*_tmp8A7;
struct Cyc_Core_Impossible_struct*_tmp8A6;(int)_throw((void*)((_tmp8A6=_cycalloc(
sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A8.tag=Cyc_Core_Impossible,((_tmp8A8.f1=((
_tmp8A7="anal_stmt -- reset_region",_tag_dyneither(_tmp8A7,sizeof(char),26))),
_tmp8A8)))),_tmp8A6)))));}_LL34A:;}goto _LL305;};}_LL32E:;_LL32F: {struct Cyc_Core_Impossible_struct
_tmp8AE;const char*_tmp8AD;struct Cyc_Core_Impossible_struct*_tmp8AC;(int)_throw((
void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AE.tag=Cyc_Core_Impossible,((
_tmp8AE.f1=((_tmp8AD="anal_stmt -- bad stmt syntax or unimplemented stmt form",
_tag_dyneither(_tmp8AD,sizeof(char),56))),_tmp8AE)))),_tmp8AC)))));}_LL305:;}
return outflow;};}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp63B=env->fenv;void*_tmp63C=
decl->r;struct Cyc_Absyn_Vardecl*_tmp63E;struct Cyc_Core_Opt*_tmp640;struct Cyc_Core_Opt
_tmp641;struct Cyc_List_List*_tmp642;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_List_List*
_tmp645;struct Cyc_Absyn_Fndecl*_tmp647;struct Cyc_Absyn_Tvar*_tmp649;struct Cyc_Absyn_Vardecl*
_tmp64A;int _tmp64B;struct Cyc_Absyn_Exp*_tmp64C;_LL350: {struct Cyc_Absyn_Var_d_struct*
_tmp63D=(struct Cyc_Absyn_Var_d_struct*)_tmp63C;if(_tmp63D->tag != 0)goto _LL352;
else{_tmp63E=_tmp63D->f1;}}_LL351: {struct Cyc_List_List _tmp8AF;struct Cyc_List_List
_tmp64D=(_tmp8AF.hd=_tmp63E,((_tmp8AF.tl=0,_tmp8AF)));inflow=Cyc_NewControlFlow_add_vars(
_tmp63B,inflow,(struct Cyc_List_List*)& _tmp64D,_tmp63B->unknown_none,decl->loc);{
struct Cyc_Absyn_Exp*_tmp64E=_tmp63E->initializer;if(_tmp64E == 0)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp650;void*_tmp651;struct _tuple13 _tmp64F=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_tmp64E);_tmp650=_tmp64F.f1;_tmp651=_tmp64F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp652=_tmp650;int _tmp653;struct _tuple12 _tmp654;
struct Cyc_Dict_Dict _tmp655;struct Cyc_List_List*_tmp656;_LL35D: if((_tmp652.BottomFL).tag
!= 1)goto _LL35F;_tmp653=(int)(_tmp652.BottomFL).val;_LL35E: return Cyc_CfFlowInfo_BottomFL();
_LL35F: if((_tmp652.ReachableFL).tag != 2)goto _LL35C;_tmp654=(struct _tuple12)(
_tmp652.ReachableFL).val;_tmp655=_tmp654.f1;_tmp656=_tmp654.f2;_LL360:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp8B5;struct Cyc_CfFlowInfo_VarRoot_struct _tmp8B4;struct Cyc_CfFlowInfo_Place*
_tmp8B3;_tmp655=Cyc_CfFlowInfo_assign_place(_tmp63B,decl->loc,_tmp655,env->all_changed,((
_tmp8B3=_region_malloc(env->r,sizeof(*_tmp8B3)),((_tmp8B3->root=(void*)((_tmp8B5=
_region_malloc(env->r,sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B4.tag=0,((_tmp8B4.f1=
_tmp63E,_tmp8B4)))),_tmp8B5)))),((_tmp8B3->fields=0,_tmp8B3)))))),_tmp651);}
_tmp656=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp656,_tmp63E,(struct Cyc_Absyn_Exp*)
_check_null(_tmp63E->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp65A=Cyc_CfFlowInfo_ReachableFL(
_tmp655,_tmp656);Cyc_NewControlFlow_update_tryflow(env,_tmp65A);return _tmp65A;};
_LL35C:;};};};}_LL352: {struct Cyc_Absyn_Let_d_struct*_tmp63F=(struct Cyc_Absyn_Let_d_struct*)
_tmp63C;if(_tmp63F->tag != 2)goto _LL354;else{_tmp640=_tmp63F->f2;if(_tmp640 == 0)
goto _LL354;_tmp641=*_tmp640;_tmp642=(struct Cyc_List_List*)_tmp641.v;_tmp643=
_tmp63F->f3;}}_LL353: {union Cyc_CfFlowInfo_FlowInfo _tmp65D;void*_tmp65E;struct
_tuple13 _tmp65C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp643);_tmp65D=
_tmp65C.f1;_tmp65E=_tmp65C.f2;_tmp65D=Cyc_NewControlFlow_use_Rval(env,_tmp643->loc,
_tmp65D,_tmp65E);return Cyc_NewControlFlow_add_vars(_tmp63B,_tmp65D,_tmp642,
_tmp63B->unknown_all,decl->loc);}_LL354: {struct Cyc_Absyn_Letv_d_struct*_tmp644=(
struct Cyc_Absyn_Letv_d_struct*)_tmp63C;if(_tmp644->tag != 3)goto _LL356;else{
_tmp645=_tmp644->f1;}}_LL355: return Cyc_NewControlFlow_add_vars(_tmp63B,inflow,
_tmp645,_tmp63B->unknown_none,decl->loc);_LL356: {struct Cyc_Absyn_Fn_d_struct*
_tmp646=(struct Cyc_Absyn_Fn_d_struct*)_tmp63C;if(_tmp646->tag != 1)goto _LL358;
else{_tmp647=_tmp646->f1;}}_LL357: Cyc_NewControlFlow_check_nested_fun(_tmp63B,
inflow,_tmp647);{void*_tmp65F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp647->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp660=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp647->fn_vardecl);
struct Cyc_List_List*_tmp8B6;return Cyc_NewControlFlow_add_vars(_tmp63B,inflow,((
_tmp8B6=_region_malloc(env->r,sizeof(*_tmp8B6)),((_tmp8B6->hd=_tmp660,((_tmp8B6->tl=
0,_tmp8B6)))))),_tmp63B->unknown_all,decl->loc);};_LL358: {struct Cyc_Absyn_Region_d_struct*
_tmp648=(struct Cyc_Absyn_Region_d_struct*)_tmp63C;if(_tmp648->tag != 4)goto _LL35A;
else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;_tmp64B=_tmp648->f3;_tmp64C=_tmp648->f4;}}
_LL359: if(_tmp64C != 0){struct Cyc_Absyn_Exp*_tmp662=(struct Cyc_Absyn_Exp*)_tmp64C;
union Cyc_CfFlowInfo_FlowInfo _tmp664;void*_tmp665;struct _tuple13 _tmp663=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp662);_tmp664=_tmp663.f1;_tmp665=_tmp663.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp662->loc,_tmp664,_tmp665);}{struct Cyc_List_List _tmp8B7;struct Cyc_List_List
_tmp666=(_tmp8B7.hd=_tmp64A,((_tmp8B7.tl=0,_tmp8B7)));return Cyc_NewControlFlow_add_vars(
_tmp63B,inflow,(struct Cyc_List_List*)& _tmp666,_tmp63B->unknown_all,decl->loc);};
_LL35A:;_LL35B: {struct Cyc_Core_Impossible_struct _tmp8BD;const char*_tmp8BC;
struct Cyc_Core_Impossible_struct*_tmp8BB;(int)_throw((void*)((_tmp8BB=_cycalloc(
sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BD.tag=Cyc_Core_Impossible,((_tmp8BD.f1=((
_tmp8BC="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8BC,sizeof(char),
35))),_tmp8BD)))),_tmp8BB)))));}_LL34F:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp66B=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp66B;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp66C=fenv->r;struct Cyc_Position_Segment*_tmp66D=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp66D);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp66E=inflow;int _tmp66F;struct
_tuple12 _tmp670;struct Cyc_Dict_Dict _tmp671;struct Cyc_List_List*_tmp672;_LL362:
if((_tmp66E.BottomFL).tag != 1)goto _LL364;_tmp66F=(int)(_tmp66E.BottomFL).val;
_LL363: {const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8C0="check_fun",
_tag_dyneither(_tmp8C0,sizeof(char),10))),_tag_dyneither(_tmp8BF,sizeof(void*),0)));}
_LL364: if((_tmp66E.ReachableFL).tag != 2)goto _LL361;_tmp670=(struct _tuple12)(
_tmp66E.ReachableFL).val;_tmp671=_tmp670.f1;_tmp672=_tmp670.f2;_LL365: {struct
Cyc_List_List*atts;{void*_tmp675=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp677;struct Cyc_List_List*
_tmp678;_LL367: {struct Cyc_Absyn_FnType_struct*_tmp676=(struct Cyc_Absyn_FnType_struct*)
_tmp675;if(_tmp676->tag != 10)goto _LL369;else{_tmp677=_tmp676->f1;_tmp678=_tmp677.attributes;}}
_LL368: atts=_tmp678;goto _LL366;_LL369:;_LL36A: {const char*_tmp8C3;void*_tmp8C2;(
_tmp8C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8C3="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp8C3,sizeof(char),50))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}_LL366:;}
for(0;atts != 0;atts=atts->tl){void*_tmp67B=(void*)atts->hd;int _tmp67D;int _tmp67F;
_LL36C: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp67C=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp67B;if(_tmp67C->tag != 21)goto _LL36E;else{_tmp67D=_tmp67C->f1;}}_LL36D: {
unsigned int j=(unsigned int)_tmp67D;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp8C6;
void*_tmp8C5;(_tmp8C5=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8C6="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp8C6,sizeof(char),52))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp682=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp682->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp8C9;void*
_tmp8C8;(_tmp8C8=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8C9="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp8C9,sizeof(char),52))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp685=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp8CC;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp8CB;struct Cyc_CfFlowInfo_InitParam_struct*_tmp686=(_tmp8CB=_region_malloc(
_tmp66C,sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CC.tag=2,((_tmp8CC.f1=(int)j,((
_tmp8CC.f2=(void*)t,_tmp8CC)))))),_tmp8CB)));struct Cyc_CfFlowInfo_Place*_tmp8CD;
struct Cyc_CfFlowInfo_Place*_tmp687=(_tmp8CD=_region_malloc(_tmp66C,sizeof(*
_tmp8CD)),((_tmp8CD->root=(void*)_tmp686,((_tmp8CD->fields=0,_tmp8CD)))));
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp671,(void*)_tmp686,_tmp685);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp8D3;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8D2;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp8D0;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8CF;_tmp671=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp671,(void*)((_tmp8CF=
_region_malloc(_tmp66C,sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D0.tag=0,((_tmp8D0.f1=
_tmp682,_tmp8D0)))),_tmp8CF)))),(void*)((_tmp8D2=_region_malloc(_tmp66C,sizeof(*
_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=5,((_tmp8D3.f1=_tmp687,_tmp8D3)))),_tmp8D2)))));}
goto _LL36B;};};}_LL36E: {struct Cyc_Absyn_Initializes_att_struct*_tmp67E=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp67B;if(_tmp67E->tag != 20)goto _LL370;else{
_tmp67F=_tmp67E->f1;}}_LL36F: {unsigned int j=(unsigned int)_tmp67F;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,
Cyc_Tcutil_terr(_tmp66D,((_tmp8D6="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp8D6,sizeof(char),51))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp691=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp692=Cyc_Tcutil_compress(
_tmp691->type);struct Cyc_Absyn_PtrInfo _tmp694;void*_tmp695;struct Cyc_Absyn_PtrAtts
_tmp696;union Cyc_Absyn_Constraint*_tmp697;union Cyc_Absyn_Constraint*_tmp698;
union Cyc_Absyn_Constraint*_tmp699;_LL373: {struct Cyc_Absyn_PointerType_struct*
_tmp693=(struct Cyc_Absyn_PointerType_struct*)_tmp692;if(_tmp693->tag != 5)goto
_LL375;else{_tmp694=_tmp693->f1;_tmp695=_tmp694.elt_typ;_tmp696=_tmp694.ptr_atts;
_tmp697=_tmp696.nullable;_tmp698=_tmp696.bounds;_tmp699=_tmp696.zero_term;}}
_LL374: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp697)){
const char*_tmp8D9;void*_tmp8D8;(_tmp8D8=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8D9="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp8D9,sizeof(char),55))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp698)){const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,
Cyc_Tcutil_terr(_tmp66D,((_tmp8DC="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp8DC,sizeof(char),57))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp699)){
const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8DF="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp8DF,sizeof(char),77))),_tag_dyneither(_tmp8DE,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp8E2;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp8E1;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6A0=(_tmp8E1=_region_malloc(
_tmp66C,sizeof(*_tmp8E1)),((_tmp8E1[0]=((_tmp8E2.tag=2,((_tmp8E2.f1=(int)j,((
_tmp8E2.f2=(void*)_tmp695,_tmp8E2)))))),_tmp8E1)));struct Cyc_CfFlowInfo_Place*
_tmp8E3;struct Cyc_CfFlowInfo_Place*_tmp6A1=(_tmp8E3=_region_malloc(_tmp66C,
sizeof(*_tmp8E3)),((_tmp8E3->root=(void*)_tmp6A0,((_tmp8E3->fields=0,_tmp8E3)))));
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp671,(void*)_tmp6A0,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp695,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp8E9;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp8E8;struct Cyc_CfFlowInfo_VarRoot_struct _tmp8E6;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp8E5;_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp671,(void*)((_tmp8E5=_region_malloc(_tmp66C,sizeof(*_tmp8E5)),((_tmp8E5[0]=((
_tmp8E6.tag=0,((_tmp8E6.f1=_tmp691,_tmp8E6)))),_tmp8E5)))),(void*)((_tmp8E8=
_region_malloc(_tmp66C,sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8E9.tag=5,((_tmp8E9.f1=
_tmp6A1,_tmp8E9)))),_tmp8E8)))));}{struct Cyc_List_List*_tmp8EA;param_roots=((
_tmp8EA=_region_malloc(_tmp66C,sizeof(*_tmp8EA)),((_tmp8EA->hd=_tmp6A1,((_tmp8EA->tl=
param_roots,_tmp8EA))))));}goto _LL372;};_LL375:;_LL376: {const char*_tmp8ED;void*
_tmp8EC;(_tmp8EC=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8ED="initializes attribute on non-pointer",
_tag_dyneither(_tmp8ED,sizeof(char),37))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}
_LL372:;}goto _LL36B;};}_LL370:;_LL371: goto _LL36B;_LL36B:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp671,_tmp672);}_LL361:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp66C,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp8EE;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp8EE=_region_malloc(_tmp66C,sizeof(*_tmp8EE)),((
_tmp8EE->r=_tmp66C,((_tmp8EE->fenv=fenv,((_tmp8EE->iterate_again=1,((_tmp8EE->iteration_num=
0,((_tmp8EE->in_try=0,((_tmp8EE->tryflow=inflow,((_tmp8EE->all_changed=0,((
_tmp8EE->noreturn=noreturn,((_tmp8EE->param_roots=param_roots,((_tmp8EE->flow_table=
flow_table,_tmp8EE)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp6AC=outflow;int _tmp6AD;_LL378: if((_tmp6AC.BottomFL).tag
!= 1)goto _LL37A;_tmp6AD=(int)(_tmp6AC.BottomFL).val;_LL379: goto _LL377;_LL37A:;
_LL37B: Cyc_NewControlFlow_check_init_params(_tmp66D,env,outflow);if(noreturn){
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,Cyc_Tcutil_terr(_tmp66D,((_tmp8F1="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp8F1,sizeof(char),46))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}
else{void*_tmp6B0=Cyc_Tcutil_compress(fd->ret_type);_LL37D: {struct Cyc_Absyn_VoidType_struct*
_tmp6B1=(struct Cyc_Absyn_VoidType_struct*)_tmp6B0;if(_tmp6B1->tag != 0)goto _LL37F;}
_LL37E: goto _LL37C;_LL37F: {struct Cyc_Absyn_DoubleType_struct*_tmp6B2=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6B0;if(_tmp6B2->tag != 8)goto _LL381;}_LL380: goto _LL382;_LL381: {struct Cyc_Absyn_FloatType_struct*
_tmp6B3=(struct Cyc_Absyn_FloatType_struct*)_tmp6B0;if(_tmp6B3->tag != 7)goto
_LL383;}_LL382: goto _LL384;_LL383: {struct Cyc_Absyn_IntType_struct*_tmp6B4=(
struct Cyc_Absyn_IntType_struct*)_tmp6B0;if(_tmp6B4->tag != 6)goto _LL385;}_LL384:{
const char*_tmp8F4;void*_tmp8F3;(_tmp8F3=0,Cyc_Tcutil_warn(_tmp66D,((_tmp8F4="function may not return a value",
_tag_dyneither(_tmp8F4,sizeof(char),32))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}
goto _LL37C;_LL385:;_LL386:{const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,Cyc_Tcutil_terr(
_tmp66D,((_tmp8F7="function may not return a value",_tag_dyneither(_tmp8F7,
sizeof(char),32))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}goto _LL37C;_LL37C:;}
goto _LL377;_LL377:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp6BA=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp6BC;
struct Cyc_List_List*_tmp6BE;struct Cyc_List_List*_tmp6C0;struct Cyc_List_List*
_tmp6C2;_LL388: {struct Cyc_Absyn_Fn_d_struct*_tmp6BB=(struct Cyc_Absyn_Fn_d_struct*)
_tmp6BA;if(_tmp6BB->tag != 1)goto _LL38A;else{_tmp6BC=_tmp6BB->f1;}}_LL389: Cyc_NewControlFlow_check_fun(
_tmp6BC);goto _LL387;_LL38A: {struct Cyc_Absyn_ExternC_d_struct*_tmp6BD=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp6BA;if(_tmp6BD->tag != 12)goto _LL38C;else{_tmp6BE=_tmp6BD->f1;}}_LL38B:
_tmp6C0=_tmp6BE;goto _LL38D;_LL38C: {struct Cyc_Absyn_Using_d_struct*_tmp6BF=(
struct Cyc_Absyn_Using_d_struct*)_tmp6BA;if(_tmp6BF->tag != 11)goto _LL38E;else{
_tmp6C0=_tmp6BF->f2;}}_LL38D: _tmp6C2=_tmp6C0;goto _LL38F;_LL38E: {struct Cyc_Absyn_Namespace_d_struct*
_tmp6C1=(struct Cyc_Absyn_Namespace_d_struct*)_tmp6BA;if(_tmp6C1->tag != 10)goto
_LL390;else{_tmp6C2=_tmp6C1->f2;}}_LL38F: Cyc_NewControlFlow_cf_check(_tmp6C2);
goto _LL387;_LL390: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp6C3=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp6BA;if(_tmp6C3->tag != 13)goto _LL392;}_LL391: goto _LL387;_LL392:;_LL393: goto
_LL387;_LL387:;}}
