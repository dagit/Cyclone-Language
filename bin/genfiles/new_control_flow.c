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
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
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
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp71D;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp71C;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp71B;enclosee->annot=(void*)((_tmp71B=_cycalloc(sizeof(*_tmp71B)),((_tmp71B[0]=((
_tmp71D.tag=Cyc_NewControlFlow_CFAnnot,((_tmp71D.f1=((_tmp71C.encloser=encloser,((
_tmp71C.visited=0,_tmp71C)))),_tmp71D)))),_tmp71B))));}struct Cyc_NewControlFlow_AnalEnv{
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
_tmp723;const char*_tmp722;struct Cyc_Core_Impossible_struct*_tmp721;(int)_throw((
void*)((_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721[0]=((_tmp723.tag=Cyc_Core_Impossible,((
_tmp723.f1=((_tmp722="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp722,sizeof(char),37))),_tmp723)))),_tmp721)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp724;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp724=_region_malloc(env->r,sizeof(*_tmp724)),((_tmp724[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp724)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp725;return(_tmp725.f1=_tmpB,((_tmp725.f2=
_tmpC,_tmp725)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow);static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow);}struct _tuple17{struct Cyc_NewControlFlow_AnalEnv*f1;
struct Cyc_Position_Segment*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_unique_root(
struct _tuple17*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_unique_root(
struct _tuple17*ckenv,void*root,void*rval){struct _tuple17 _tmpF;struct Cyc_NewControlFlow_AnalEnv*
_tmp10;struct Cyc_Position_Segment*_tmp11;struct Cyc_Dict_Dict _tmp12;struct
_tuple17*_tmpE=ckenv;_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{
void*_tmp13=root;struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp14=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL8;
else{_tmp15=_tmp14->f1;}}_LL7: if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))
Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp10->fenv)->r,_tmp15->type)){{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp19;_LLB: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp16;if(_tmp17->tag != 7)goto _LLD;}_LLC: goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp18=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp16;if(_tmp18->tag != 3)goto _LLF;
else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE: goto
_LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp16;if(_tmp1A->tag != 0)goto _LL11;}_LL10: goto _LLA;_LL11:;_LL12:{const char*
_tmp729;void*_tmp728[1];struct Cyc_String_pa_struct _tmp727;(_tmp727.tag=0,((
_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp728[0]=& _tmp727,Cyc_Tcutil_warn(_tmp11,((_tmp729="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp729,sizeof(char),60))),_tag_dyneither(_tmp728,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=
Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple15 _tmp72A;struct _tuple15 _tmp22=(_tmp72A.f1=flow,((_tmp72A.f2=_tmp20,
_tmp72A)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict
_tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict
_tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(
struct _tuple12)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((
_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;
_tmp28=_tmp27.f1;_LL15: {struct _tuple17 _tmp72B;struct _tuple17 _tmp29=(_tmp72B.f1=
env,((_tmp72B.f2=s->loc,((_tmp72B.f3=_tmp28,_tmp72B)))));((void(*)(void(*f)(
struct _tuple17*,void*,void*),struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_NewControlFlow_check_unique_root,& _tmp29,_tmp25);goto _LL13;}_LL16:;_LL17:
goto _LL13;_LL13:;}if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){*
_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=1;}}
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp2B=inflow;int _tmp2C;struct _tuple12 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*
_tmp2F;_LL19: if((_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;
_LL1A: return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto
_LL18;_tmp2D=(struct _tuple12)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=
_tmp2D.f2;_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp72E;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp72D;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp72D=_region_malloc(fenv->r,sizeof(*_tmp72D)),((_tmp72D[0]=((_tmp72E.tag=
0,((_tmp72E.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp72E)))),_tmp72D)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);_LL18:;}
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp33=inflow;int _tmp34;struct _tuple12 _tmp35;struct
Cyc_Dict_Dict _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 
1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;_LL1F: return Cyc_CfFlowInfo_BottomFL();
_LL20: if((_tmp33.ReachableFL).tag != 2)goto _LL1D;_tmp35=(struct _tuple12)(_tmp33.ReachableFL).val;
_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp731;void*_tmp730;(_tmp730=0,Cyc_Tcutil_terr(
loc,((_tmp731="expression may not be fully initialized",_tag_dyneither(_tmp731,
sizeof(char),40))),_tag_dyneither(_tmp730,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp732;struct
_tuple0 _tmp3D=(_tmp732.f1=Cyc_Tcutil_compress(t),((_tmp732.f2=r,_tmp732)));void*
_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*_tmp43;void*_tmp45;
struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU _tmp48;
struct _tuple2 _tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;void*_tmp4B;struct
_dyneither_ptr _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp50;void*_tmp51;struct
_dyneither_ptr _tmp53;void*_tmp54;struct Cyc_Absyn_AggrInfo _tmp56;union Cyc_Absyn_AggrInfoU
_tmp57;struct Cyc_List_List*_tmp58;void*_tmp59;struct _dyneither_ptr _tmp5B;void*
_tmp5C;enum Cyc_Absyn_AggrKind _tmp5E;struct Cyc_List_List*_tmp5F;void*_tmp60;
struct _dyneither_ptr _tmp62;void*_tmp63;_LL23: _tmp3E=_tmp3D.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E;if(_tmp3F->tag != 3)goto
_LL25;else{_tmp40=_tmp3F->f1;if(_tmp40 != Cyc_CfFlowInfo_NoneIL)goto _LL25;}};
_LL24: return;_LL25: _tmp41=_tmp3D.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp42=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp41;if(_tmp42->tag != 0)goto _LL27;};_LL26:
return;_LL27: _tmp43=_tmp3D.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp44=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp43;if(_tmp44->tag != 7)goto _LL29;};
_LL28: return;_LL29: _tmp45=_tmp3D.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp46=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp45;if(_tmp46->tag != 4)goto
_LL2B;else{_tmp47=_tmp46->f1;_tmp48=_tmp47.field_info;if((_tmp48.KnownDatatypefield).tag
!= 2)goto _LL2B;_tmp49=(struct _tuple2)(_tmp48.KnownDatatypefield).val;_tmp4A=
_tmp49.f2;}};_tmp4B=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4C=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp4B;if(_tmp4C->tag != 6)goto _LL2B;else{
_tmp4D=_tmp4C->f2;}};_LL2A: if(_tmp4A->typs == 0)return;_tmp50=_tmp4A->typs;_tmp53=
_tmp4D;goto _LL2C;_LL2B: _tmp4E=_tmp3D.f1;{struct Cyc_Absyn_TupleType_struct*_tmp4F=(
struct Cyc_Absyn_TupleType_struct*)_tmp4E;if(_tmp4F->tag != 11)goto _LL2D;else{
_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp52=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp51;if(_tmp52->tag != 6)goto
_LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp50);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(
env,loc,(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((
void**)_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}return;}_LL2D:
_tmp54=_tmp3D.f1;{struct Cyc_Absyn_AggrType_struct*_tmp55=(struct Cyc_Absyn_AggrType_struct*)
_tmp54;if(_tmp55->tag != 12)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;
_tmp58=_tmp56.targs;}};_tmp59=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp59;if(_tmp5A->tag != 6)goto
_LL2F;else{_tmp5B=_tmp5A->f2;}};_LL2E: {struct Cyc_Absyn_Aggrdecl*_tmp65=Cyc_Absyn_get_known_aggrdecl(
_tmp57);if(_tmp65->impl == 0)return;{struct Cyc_List_List*_tmp66=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->fields;struct _RegionHandle _tmp67=_new_region("temp");
struct _RegionHandle*temp=& _tmp67;_push_region(temp);{struct Cyc_List_List*_tmp68=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp65->tvs,_tmp58);{int i=0;
for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){void*
t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type;
if(_tmp68 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp68,t);Cyc_NewControlFlow_check_nounique(
env,loc,t,((void**)_tmp5B.curr)[i]);}}_npop_handler(0);return;};_pop_region(temp);};}
_LL2F: _tmp5C=_tmp3D.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp5D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5C;if(_tmp5D->tag != 13)goto _LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};
_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp61=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp60;if(_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:{int i=0;for(
0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((
void**)_tmp62.curr)[i]);}}return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64=(struct Cyc_Absyn_PointerType_struct*)_tmp63;if(_tmp64->tag != 5)goto _LL33;};
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp735;void*
_tmp734;(_tmp734=0,Cyc_Tcutil_warn(loc,((_tmp735="argument may still contain unique pointers",
_tag_dyneither(_tmp735,sizeof(char),43))),_tag_dyneither(_tmp734,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp738;void*
_tmp737;(_tmp737=0,Cyc_Tcutil_terr(loc,((_tmp738="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp738,sizeof(char),47))),_tag_dyneither(_tmp737,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp73B;void*_tmp73A;(_tmp73A=0,Cyc_Tcutil_warn(
loc,((_tmp73B="argument may contain live unique pointers",_tag_dyneither(_tmp73B,
sizeof(char),42))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple19 _tmp73C;return(
_tmp73C.f1=inflow,((_tmp73C.f2=0,_tmp73C)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp73F;struct _tuple19 _tmp73E;return(_tmp73E.f1=
_tmp7C,((_tmp73E.f2=((_tmp73F=_region_malloc(rgn,sizeof(*_tmp73F)),((_tmp73F->hd=
_tmp7D,((_tmp73F->tl=0,_tmp73F)))))),_tmp73E)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp740;env->all_changed=((
_tmp740=_region_malloc(env->r,sizeof(*_tmp740)),((_tmp740[0]=(env->fenv)->mt_place_set,
_tmp740))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp741;rvals=((_tmp741=_region_malloc(rgn,sizeof(*_tmp741)),((_tmp741->hd=
_tmp83,((_tmp741->tl=0,_tmp741))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp742;env->all_changed=((_tmp742=_region_malloc(env->r,sizeof(*_tmp742)),((
_tmp742[0]=(env->fenv)->mt_place_set,_tmp742))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp743;rvals=((_tmp743=_region_malloc(rgn,
sizeof(*_tmp743)),((_tmp743->hd=_tmp88,((_tmp743->tl=rvals,_tmp743))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp744;env->all_changed=((
_tmp744=_region_malloc(env->r,sizeof(*_tmp744)),((_tmp744[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp744))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp745;return(_tmp745.f1=outflow,((_tmp745.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp745)));};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);static struct
_tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct _RegionHandle*
_tmp8C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*_tmp8F;
struct _tuple19 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp8C,env,inflow,
es,0,0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp90=
_tmp8E;struct _tuple12 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((_tmp90.ReachableFL).tag
!= 2)goto _LL42;_tmp91=(struct _tuple12)(_tmp90.ReachableFL).val;_tmp92=_tmp91.f1;
_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp748;void*_tmp747;(_tmp747=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp748="expression may not be initialized",
_tag_dyneither(_tmp748,sizeof(char),34))),_tag_dyneither(_tmp747,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple13 _tmp749;return(_tmp749.f1=_tmp8E,((
_tmp749.f2=(env->fenv)->unknown_all,_tmp749)));};}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel
il);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp97=outflow;int _tmp98;struct _tuple12 _tmp99;struct Cyc_Dict_Dict _tmp9A;struct
Cyc_List_List*_tmp9B;_LL45: if((_tmp97.BottomFL).tag != 1)goto _LL47;_tmp98=(int)(
_tmp97.BottomFL).val;_LL46: return outflow;_LL47: if((_tmp97.ReachableFL).tag != 2)
goto _LL44;_tmp99=(struct _tuple12)(_tmp97.ReachableFL).val;_tmp9A=_tmp99.f1;
_tmp9B=_tmp99.f2;_LL48: {union Cyc_CfFlowInfo_AbsLVal _tmp9C=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp9D;struct Cyc_CfFlowInfo_Place*_tmp9E;_LL4A: if((_tmp9C.UnknownL).tag
!= 2)goto _LL4C;_tmp9D=(int)(_tmp9C.UnknownL).val;_LL4B: return outflow;_LL4C: if((
_tmp9C.PlaceL).tag != 1)goto _LL49;_tmp9E=(struct Cyc_CfFlowInfo_Place*)(_tmp9C.PlaceL).val;
_LL4D: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmp9F=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp9A,env->all_changed,_tmp9E,nzval),_tmp9B);return _tmp9F;}
_LL49:;}_LL44:;}static struct _tuple15 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il);static struct _tuple15 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmpA0=outflow;int _tmpA1;struct _tuple12 _tmpA2;struct Cyc_Dict_Dict _tmpA3;struct
Cyc_List_List*_tmpA4;_LL4F: if((_tmpA0.BottomFL).tag != 1)goto _LL51;_tmpA1=(int)(
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp74A;return(_tmp74A.f1=outflow,((
_tmp74A.f2=outflow,_tmp74A)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp74B;
return(_tmp74B.f1=outflow,((_tmp74B.f2=outflow,_tmp74B)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp74C;return(_tmp74C.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp74C.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp74C)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r);static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmpAD=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAF;void*
_tmpB0;struct Cyc_Absyn_PtrAtts _tmpB1;union Cyc_Absyn_Constraint*_tmpB2;union Cyc_Absyn_Constraint*
_tmpB3;_LL59: {struct Cyc_Absyn_PointerType_struct*_tmpAE=(struct Cyc_Absyn_PointerType_struct*)
_tmpAD;if(_tmpAE->tag != 5)goto _LL5B;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAF.elt_typ;
_tmpB1=_tmpAF.ptr_atts;_tmpB2=_tmpB1.bounds;_tmpB3=_tmpB1.zero_term;}}_LL5A: {
union Cyc_CfFlowInfo_FlowInfo _tmpB4=f;int _tmpB5;struct _tuple12 _tmpB6;struct Cyc_Dict_Dict
_tmpB7;struct Cyc_List_List*_tmpB8;_LL5E: if((_tmpB4.BottomFL).tag != 1)goto _LL60;
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp74D;return(_tmp74D.f1=
f,((_tmp74D.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp74D)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
_tuple12)(_tmpB4.ReachableFL).val;_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;_LL61: if(Cyc_Tcutil_is_bound_one(
_tmpB2)){void*_tmpBA=r;struct Cyc_CfFlowInfo_Place*_tmpBE;enum Cyc_CfFlowInfo_InitLevel
_tmpC1;_LL63: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpBB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpBA;if(_tmpBB->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpBC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpBA;if(_tmpBC->tag != 2)goto
_LL67;}_LL66:{void*_tmpC2=e->annot;struct Cyc_List_List*_tmpC4;_LL70: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpC3=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpC2;if(_tmpC3->tag != Cyc_CfFlowInfo_NotZero)
goto _LL72;else{_tmpC4=_tmpC3->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,
_tmpC4))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC5=e->r;_LL75: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC5;if(_tmpC6->tag != 24)goto _LL77;}
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp750;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp74F;e->annot=(void*)((_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((
_tmp750.tag=Cyc_CfFlowInfo_NotZero,((_tmp750.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp750)))),_tmp74F))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp753;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp752;e->annot=(void*)((_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752[0]=((
_tmp753.tag=Cyc_CfFlowInfo_NotZero,((_tmp753.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp753)))),_tmp752))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp754;return(_tmp754.f1=f,((
_tmp754.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp754)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp755;return(_tmp755.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp755.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),_tmp755)));};
_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp758;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp757;e->annot=(void*)((_tmp757=
_cycalloc(sizeof(*_tmp757)),((_tmp757[0]=((_tmp758.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp758.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp758)))),
_tmp757))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp75B;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp75A;e->annot=(void*)((_tmp75A=_cycalloc(sizeof(*_tmp75A)),((_tmp75A[0]=((
_tmp75B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp75B.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp75B)))),_tmp75A))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp75E;void*_tmp75D;(
_tmp75D=0,Cyc_Tcutil_terr(e->loc,((_tmp75E="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp75E,sizeof(char),61))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_Tcutil_terr(
e->loc,((_tmp761="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp761,sizeof(char),46))),_tag_dyneither(_tmp760,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp762;return(_tmp762.f1=
f,((_tmp762.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp762)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp763;return(
_tmp763.f1=f,((_tmp763.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_none),
_tmp763)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp769;
const char*_tmp768;struct Cyc_Core_Impossible_struct*_tmp767;(int)_throw((void*)((
_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((_tmp769.tag=Cyc_Core_Impossible,((
_tmp769.f1=((_tmp768="right deref of non-pointer-type",_tag_dyneither(_tmp768,
sizeof(char),32))),_tmp769)))),_tmp767)))));}_LL58:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpE6=e1->r;void*
_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEA;void*_tmpEC;struct Cyc_Absyn_Vardecl*_tmpEE;
void*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpF2;void*_tmpF4;struct Cyc_Absyn_Vardecl*
_tmpF6;_LL97: {struct Cyc_Absyn_Var_e_struct*_tmpE7=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpE7->tag != 1)goto _LL99;else{_tmpE8=(void*)_tmpE7->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpE9=(struct Cyc_Absyn_Pat_b_struct*)_tmpE8;if(_tmpE9->tag != 5)goto _LL99;else{
_tmpEA=_tmpE9->f1;}};}}_LL98: _tmpEE=_tmpEA;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*
_tmpEB=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpEB->tag != 1)goto _LL9B;else{
_tmpEC=(void*)_tmpEB->f2;{struct Cyc_Absyn_Local_b_struct*_tmpED=(struct Cyc_Absyn_Local_b_struct*)
_tmpEC;if(_tmpED->tag != 4)goto _LL9B;else{_tmpEE=_tmpED->f1;}};}}_LL9A: _tmpF2=
_tmpEE;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*_tmpEF=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpEF->tag != 1)goto _LL9D;else{_tmpF0=(void*)_tmpEF->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpF1=(struct Cyc_Absyn_Param_b_struct*)_tmpF0;if(_tmpF1->tag != 3)goto _LL9D;
else{_tmpF2=_tmpF1->f1;}};}}_LL9C: _tmpF6=_tmpF2;goto _LL9E;_LL9D: {struct Cyc_Absyn_Var_e_struct*
_tmpF3=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpF3->tag != 1)goto _LL9F;else{
_tmpF4=(void*)_tmpF3->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF5=(struct Cyc_Absyn_Global_b_struct*)
_tmpF4;if(_tmpF5->tag != 1)goto _LL9F;else{_tmpF6=_tmpF5->f1;}};}}_LL9E: if(!_tmpF6->escapes){
void*_tmpF7=e2->r;void*_tmpF9;struct Cyc_Absyn_Vardecl*_tmpFB;void*_tmpFD;struct
Cyc_Absyn_Vardecl*_tmpFF;void*_tmp101;struct Cyc_Absyn_Vardecl*_tmp103;void*
_tmp105;struct Cyc_Absyn_Vardecl*_tmp107;_LLA2: {struct Cyc_Absyn_Var_e_struct*
_tmpF8=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmpF8->tag != 1)goto _LLA4;else{
_tmpF9=(void*)_tmpF8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpFA=(struct Cyc_Absyn_Pat_b_struct*)
_tmpF9;if(_tmpFA->tag != 5)goto _LLA4;else{_tmpFB=_tmpFA->f1;}};}}_LLA3: _tmpFF=
_tmpFB;goto _LLA5;_LLA4: {struct Cyc_Absyn_Var_e_struct*_tmpFC=(struct Cyc_Absyn_Var_e_struct*)
_tmpF7;if(_tmpFC->tag != 1)goto _LLA6;else{_tmpFD=(void*)_tmpFC->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpFE=(struct Cyc_Absyn_Local_b_struct*)_tmpFD;if(_tmpFE->tag != 4)goto _LLA6;
else{_tmpFF=_tmpFE->f1;}};}}_LLA5: _tmp103=_tmpFF;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_struct*
_tmp100=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp100->tag != 1)goto _LLA8;
else{_tmp101=(void*)_tmp100->f2;{struct Cyc_Absyn_Param_b_struct*_tmp102=(struct
Cyc_Absyn_Param_b_struct*)_tmp101;if(_tmp102->tag != 3)goto _LLA8;else{_tmp103=
_tmp102->f1;}};}}_LLA7: _tmp107=_tmp103;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_struct*
_tmp104=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp104->tag != 1)goto _LLAA;
else{_tmp105=(void*)_tmp104->f2;{struct Cyc_Absyn_Global_b_struct*_tmp106=(struct
Cyc_Absyn_Global_b_struct*)_tmp105;if(_tmp106->tag != 1)goto _LLAA;else{_tmp107=
_tmp106->f1;}};}}_LLA9: if(!_tmp107->escapes){int found=0;{struct Cyc_List_List*
_tmp108=relns;for(0;_tmp108 != 0;_tmp108=_tmp108->tl){struct Cyc_CfFlowInfo_Reln*
_tmp109=(struct Cyc_CfFlowInfo_Reln*)_tmp108->hd;if(_tmp109->vd == _tmp107){union
Cyc_CfFlowInfo_RelnOp _tmp10A=_tmp109->rop;struct Cyc_Absyn_Vardecl*_tmp10B;_LLAD:
if((_tmp10A.LessNumelts).tag != 3)goto _LLAF;_tmp10B=(struct Cyc_Absyn_Vardecl*)(
_tmp10A.LessNumelts).val;if(!(_tmp10B == _tmpF6))goto _LLAF;_LLAE: return relns;
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp76C;
struct Cyc_List_List*_tmp76B;return(_tmp76B=_region_malloc(rgn,sizeof(*_tmp76B)),((
_tmp76B->hd=((_tmp76C=_region_malloc(rgn,sizeof(*_tmp76C)),((_tmp76C->vd=_tmp107,((
_tmp76C->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp76C)))))),((_tmp76B->tl=
relns,_tmp76B)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp76D;return(_tmp76D.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp76D.f2=rval,_tmp76D)));}_LLB4: if((_tmp10E.ReachableFL).tag
!= 2)goto _LLB1;_tmp110=(struct _tuple12)(_tmp10E.ReachableFL).val;_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp114=lval;struct Cyc_CfFlowInfo_Place*
_tmp115;int _tmp116;_LLB7: if((_tmp114.PlaceL).tag != 1)goto _LLB9;_tmp115=(struct
Cyc_CfFlowInfo_Place*)(_tmp114.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp117=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp111,env->all_changed,_tmp115,rval);
_tmp112=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp112,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp117,_tmp112);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)((struct Cyc_Core_Opt*)_check_null(lexp->topt))->v)){void*
_tmp118=Cyc_CfFlowInfo_lookup_place(_tmp111,_tmp115);enum Cyc_CfFlowInfo_InitLevel
_tmp11A;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp119=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp118;if(_tmp119->tag != 3)goto _LLBE;else{_tmp11A=_tmp119->f1;if(_tmp11A != Cyc_CfFlowInfo_NoneIL)
goto _LLBE;}}_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp11B=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp118;if(_tmp11B->tag != 7)goto _LLC0;}
_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_struct*_tmp11C=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp118;if(_tmp11C->tag != 0)goto _LLC2;}_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*
_tmp770;void*_tmp76F;(_tmp76F=0,Cyc_Tcutil_warn(lexp->loc,((_tmp770="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp770,sizeof(char),43))),_tag_dyneither(_tmp76F,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp771;return(_tmp771.f1=outflow,((_tmp771.f2=rval,_tmp771)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp772;return(_tmp772.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp772.f2=rval,_tmp772)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e);static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp121=e->r;void*_tmp123;void*_tmp126;void*_tmp129;
struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*
_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;_LLC5: {struct
Cyc_Absyn_Var_e_struct*_tmp122=(struct Cyc_Absyn_Var_e_struct*)_tmp121;if(_tmp122->tag
!= 1)goto _LLC7;else{_tmp123=(void*)_tmp122->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp124=(struct Cyc_Absyn_Param_b_struct*)_tmp123;if(_tmp124->tag != 3)goto _LLC7;};}}
_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp125=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp125->tag != 1)goto _LLC9;else{_tmp126=(void*)_tmp125->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp127=(struct Cyc_Absyn_Local_b_struct*)_tmp126;if(_tmp127->tag != 4)goto _LLC9;};}}
_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp128=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp128->tag != 1)goto _LLCB;else{_tmp129=(void*)_tmp128->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp12A=(struct Cyc_Absyn_Pat_b_struct*)_tmp129;if(_tmp12A->tag != 5)goto _LLCB;};}}
_LLCA: return 1;_LLCB: {struct Cyc_Absyn_Deref_e_struct*_tmp12B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp121;if(_tmp12B->tag != 21)goto _LLCD;else{_tmp12C=_tmp12B->f1;}}_LLCC: _tmp12E=
_tmp12C;goto _LLCE;_LLCD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp12D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp121;if(_tmp12D->tag != 22)goto _LLCF;else{_tmp12E=_tmp12D->f1;}}_LLCE: _tmp130=
_tmp12E;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp12F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp121;if(_tmp12F->tag != 23)goto _LLD1;else{_tmp130=_tmp12F->f1;}}_LLD0: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp130);_LLD1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp131=(struct Cyc_Absyn_Subscript_e_struct*)_tmp121;if(_tmp131->tag != 24)goto
_LLD3;else{_tmp132=_tmp131->f1;}}_LLD2: {void*_tmp135=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp132->topt))->v);_LLD8: {struct Cyc_Absyn_TupleType_struct*
_tmp136=(struct Cyc_Absyn_TupleType_struct*)_tmp135;if(_tmp136->tag != 11)goto
_LLDA;}_LLD9: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132);_LLDA:;
_LLDB: return 0;_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_struct*_tmp133=(struct Cyc_Absyn_Cast_e_struct*)
_tmp121;if(_tmp133->tag != 15)goto _LLD5;else{_tmp134=_tmp133->f2;}}_LLD4: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp134);_LLD5:;_LLD6: return 0;_LLC4:;}
struct _tuple20{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};
struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void
_tmp801(unsigned int*_tmp800,unsigned int*_tmp7FF,void***_tmp7FD,struct Cyc_List_List**
_tmp311){for(*_tmp800=0;*_tmp800 < *_tmp7FF;(*_tmp800)++){void*_tmp7FB;(*_tmp7FD)[*
_tmp800]=((_tmp7FB=(void*)((struct Cyc_List_List*)_check_null(*_tmp311))->hd,((*
_tmp311=(*_tmp311)->tl,_tmp7FB))));}}static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp137=env->fenv;
struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp138=inflow;int _tmp139;struct _tuple12 _tmp13A;struct Cyc_Dict_Dict _tmp13B;
struct Cyc_List_List*_tmp13C;_LLDD: if((_tmp138.BottomFL).tag != 1)goto _LLDF;
_tmp139=(int)(_tmp138.BottomFL).val;_LLDE: {struct _tuple13 _tmp773;return(_tmp773.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp773.f2=_tmp137->unknown_all,_tmp773)));}_LLDF:
if((_tmp138.ReachableFL).tag != 2)goto _LLDC;_tmp13A=(struct _tuple12)(_tmp138.ReachableFL).val;
_tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;_LLE0: d=_tmp13B;relns=_tmp13C;_LLDC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp13F;union Cyc_CfFlowInfo_AbsLVal _tmp140;struct _tuple14 _tmp13E=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;{struct _tuple14 _tmp774;
struct _tuple14 _tmp142=(_tmp774.f1=_tmp13F,((_tmp774.f2=_tmp140,_tmp774)));union
Cyc_CfFlowInfo_FlowInfo _tmp143;struct _tuple12 _tmp144;struct Cyc_Dict_Dict _tmp145;
struct Cyc_List_List*_tmp146;union Cyc_CfFlowInfo_AbsLVal _tmp147;struct Cyc_CfFlowInfo_Place*
_tmp148;_LLE2: _tmp143=_tmp142.f1;if((_tmp143.ReachableFL).tag != 2)goto _LLE4;
_tmp144=(struct _tuple12)(_tmp143.ReachableFL).val;_tmp145=_tmp144.f1;_tmp146=
_tmp144.f2;_tmp147=_tmp142.f2;if((_tmp147.PlaceL).tag != 1)goto _LLE4;_tmp148=(
struct Cyc_CfFlowInfo_Place*)(_tmp147.PlaceL).val;_LLE3: {void*_tmp149=Cyc_CfFlowInfo_lookup_place(
_tmp145,_tmp148);{void*_tmp14A=_tmp149;struct Cyc_Absyn_Exp*_tmp14C;int _tmp14D;
void*_tmp14E;_LLE7: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp14B=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp14A;if(_tmp14B->tag != 7)goto _LLE9;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;
_tmp14E=(void*)_tmp14B->f3;}}_LLE8: if(_tmp14C == e  && _tmp14D == env->iteration_num){
struct _tuple13 _tmp775;return(_tmp775.f1=_tmp13F,((_tmp775.f2=_tmp14E,_tmp775)));}{
const char*_tmp778;void*_tmp777;(_tmp777=0,Cyc_Tcutil_terr(e->loc,((_tmp778="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp778,sizeof(char),71))),_tag_dyneither(_tmp777,sizeof(void*),0)));}{
struct _tuple13 _tmp779;return(_tmp779.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp779.f2=
_tmp137->unknown_all,_tmp779)));};_LLE9:;_LLEA: goto _LLE6;_LLE6:;}{void*_tmp153=
Cyc_CfFlowInfo_make_unique_consumed(_tmp137,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,e,env->iteration_num,_tmp149);struct Cyc_Dict_Dict
_tmp154=Cyc_CfFlowInfo_assign_place(_tmp137,e->loc,_tmp145,env->all_changed,
_tmp148,_tmp153);struct _tuple13 _tmp77A;return(_tmp77A.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp154,_tmp146),((_tmp77A.f2=_tmp149,_tmp77A)));};}_LLE4:;_LLE5: goto _LLE1;_LLE1:;};}{
void*_tmp156=e->r;struct Cyc_Absyn_Exp*_tmp158;enum Cyc_Absyn_Coercion _tmp159;
struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*
_tmp15F;union Cyc_Absyn_Cnst _tmp161;int _tmp162;union Cyc_Absyn_Cnst _tmp164;struct
_tuple6 _tmp165;int _tmp166;union Cyc_Absyn_Cnst _tmp168;struct _tuple6 _tmp169;void*
_tmp16B;struct Cyc_List_List*_tmp16E;void*_tmp176;void*_tmp179;struct Cyc_Absyn_Vardecl*
_tmp17B;void*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp17F;void*_tmp181;struct Cyc_Absyn_Vardecl*
_tmp183;enum Cyc_Absyn_Primop _tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*
_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Core_Opt*_tmp18B;struct Cyc_Core_Opt
_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Core_Opt*
_tmp190;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*
_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_List_List*
_tmp199;struct Cyc_Absyn_MallocInfo _tmp19B;int _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;
void**_tmp19E;struct Cyc_Absyn_Exp*_tmp19F;int _tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;
struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*
_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*
_tmp1AC;struct _dyneither_ptr*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct
_dyneither_ptr*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct _dyneither_ptr*_tmp1B3;
struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*
_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*
_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*
_tmp1C0;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Datatypedecl*_tmp1C3;struct
Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C9;
struct Cyc_Absyn_Aggrdecl*_tmp1CA;struct Cyc_Absyn_Aggrdecl _tmp1CB;enum Cyc_Absyn_AggrKind
_tmp1CC;struct Cyc_Absyn_AggrdeclImpl*_tmp1CD;struct Cyc_Absyn_AggrdeclImpl _tmp1CE;
struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Vardecl*
_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;int _tmp1D7;
struct Cyc_Absyn_Stmt*_tmp1D9;void*_tmp1DB;_LLEC: {struct Cyc_Absyn_Cast_e_struct*
_tmp157=(struct Cyc_Absyn_Cast_e_struct*)_tmp156;if(_tmp157->tag != 15)goto _LLEE;
else{_tmp158=_tmp157->f2;_tmp159=_tmp157->f4;if(_tmp159 != Cyc_Absyn_NonNull_to_Null)
goto _LLEE;}}_LLED: {union Cyc_CfFlowInfo_FlowInfo _tmp1E3;void*_tmp1E4;struct
_tuple13 _tmp1E2=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp158);
_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1E6;void*
_tmp1E7;struct _tuple13 _tmp1E5=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1E3,
_tmp158,_tmp1E4);_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;{struct _tuple13 _tmp77B;
return(_tmp77B.f1=_tmp1E6,((_tmp77B.f2=_tmp1E4,_tmp77B)));};};}_LLEE: {struct Cyc_Absyn_Cast_e_struct*
_tmp15A=(struct Cyc_Absyn_Cast_e_struct*)_tmp156;if(_tmp15A->tag != 15)goto _LLF0;
else{_tmp15B=_tmp15A->f2;}}_LLEF: _tmp15D=_tmp15B;goto _LLF1;_LLF0: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp15C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp156;if(_tmp15C->tag != 13)
goto _LLF2;else{_tmp15D=_tmp15C->f1;}}_LLF1: _tmp15F=_tmp15D;goto _LLF3;_LLF2: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp15E=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp156;if(_tmp15E->tag != 14)goto _LLF4;else{_tmp15F=_tmp15E->f1;}}_LLF3: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15F);_LLF4: {struct Cyc_Absyn_Const_e_struct*
_tmp160=(struct Cyc_Absyn_Const_e_struct*)_tmp156;if(_tmp160->tag != 0)goto _LLF6;
else{_tmp161=_tmp160->f1;if((_tmp161.Null_c).tag != 1)goto _LLF6;_tmp162=(int)(
_tmp161.Null_c).val;}}_LLF5: goto _LLF7;_LLF6: {struct Cyc_Absyn_Const_e_struct*
_tmp163=(struct Cyc_Absyn_Const_e_struct*)_tmp156;if(_tmp163->tag != 0)goto _LLF8;
else{_tmp164=_tmp163->f1;if((_tmp164.Int_c).tag != 4)goto _LLF8;_tmp165=(struct
_tuple6)(_tmp164.Int_c).val;_tmp166=_tmp165.f2;if(_tmp166 != 0)goto _LLF8;}}_LLF7: {
struct _tuple13 _tmp77C;return(_tmp77C.f1=inflow,((_tmp77C.f2=_tmp137->zero,
_tmp77C)));}_LLF8: {struct Cyc_Absyn_Const_e_struct*_tmp167=(struct Cyc_Absyn_Const_e_struct*)
_tmp156;if(_tmp167->tag != 0)goto _LLFA;else{_tmp168=_tmp167->f1;if((_tmp168.Int_c).tag
!= 4)goto _LLFA;_tmp169=(struct _tuple6)(_tmp168.Int_c).val;}}_LLF9: goto _LLFB;
_LLFA: {struct Cyc_Absyn_Var_e_struct*_tmp16A=(struct Cyc_Absyn_Var_e_struct*)
_tmp156;if(_tmp16A->tag != 1)goto _LLFC;else{_tmp16B=(void*)_tmp16A->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp16C=(struct Cyc_Absyn_Funname_b_struct*)_tmp16B;if(_tmp16C->tag != 2)goto _LLFC;};}}
_LLFB: {struct _tuple13 _tmp77D;return(_tmp77D.f1=inflow,((_tmp77D.f2=_tmp137->notzeroall,
_tmp77D)));}_LLFC: {struct Cyc_Absyn_Datatype_e_struct*_tmp16D=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp156;if(_tmp16D->tag != 31)goto _LLFE;else{_tmp16E=_tmp16D->f1;if(_tmp16E != 0)
goto _LLFE;}}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_Const_e_struct*_tmp16F=(
struct Cyc_Absyn_Const_e_struct*)_tmp156;if(_tmp16F->tag != 0)goto _LL100;}_LLFF:
goto _LL101;_LL100: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp170=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp156;if(_tmp170->tag != 19)goto _LL102;}_LL101: goto _LL103;_LL102: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp171=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp156;if(_tmp171->tag != 18)goto
_LL104;}_LL103: goto _LL105;_LL104: {struct Cyc_Absyn_Offsetof_e_struct*_tmp172=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp156;if(_tmp172->tag != 20)goto _LL106;}
_LL105: goto _LL107;_LL106: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp173=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp156;if(_tmp173->tag != 33)goto _LL108;}_LL107: goto _LL109;_LL108: {struct Cyc_Absyn_Enum_e_struct*
_tmp174=(struct Cyc_Absyn_Enum_e_struct*)_tmp156;if(_tmp174->tag != 32)goto _LL10A;}
_LL109: {struct _tuple13 _tmp77E;return(_tmp77E.f1=inflow,((_tmp77E.f2=_tmp137->unknown_all,
_tmp77E)));}_LL10A: {struct Cyc_Absyn_Var_e_struct*_tmp175=(struct Cyc_Absyn_Var_e_struct*)
_tmp156;if(_tmp175->tag != 1)goto _LL10C;else{_tmp176=(void*)_tmp175->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp177=(struct Cyc_Absyn_Global_b_struct*)_tmp176;if(
_tmp177->tag != 1)goto _LL10C;};}}_LL10B: {struct _tuple13 _tmp77F;return(_tmp77F.f1=
inflow,((_tmp77F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp137->unknown_all),_tmp77F)));}_LL10C: {struct Cyc_Absyn_Var_e_struct*
_tmp178=(struct Cyc_Absyn_Var_e_struct*)_tmp156;if(_tmp178->tag != 1)goto _LL10E;
else{_tmp179=(void*)_tmp178->f2;{struct Cyc_Absyn_Param_b_struct*_tmp17A=(struct
Cyc_Absyn_Param_b_struct*)_tmp179;if(_tmp17A->tag != 3)goto _LL10E;else{_tmp17B=
_tmp17A->f1;}};}}_LL10D: _tmp17F=_tmp17B;goto _LL10F;_LL10E: {struct Cyc_Absyn_Var_e_struct*
_tmp17C=(struct Cyc_Absyn_Var_e_struct*)_tmp156;if(_tmp17C->tag != 1)goto _LL110;
else{_tmp17D=(void*)_tmp17C->f2;{struct Cyc_Absyn_Local_b_struct*_tmp17E=(struct
Cyc_Absyn_Local_b_struct*)_tmp17D;if(_tmp17E->tag != 4)goto _LL110;else{_tmp17F=
_tmp17E->f1;}};}}_LL10F: _tmp183=_tmp17F;goto _LL111;_LL110: {struct Cyc_Absyn_Var_e_struct*
_tmp180=(struct Cyc_Absyn_Var_e_struct*)_tmp156;if(_tmp180->tag != 1)goto _LL112;
else{_tmp181=(void*)_tmp180->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp182=(struct Cyc_Absyn_Pat_b_struct*)
_tmp181;if(_tmp182->tag != 5)goto _LL112;else{_tmp183=_tmp182->f1;}};}}_LL111: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp785;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp784;struct _tuple13 _tmp783;return(_tmp783.f1=inflow,((_tmp783.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp785=_region_malloc(
env->r,sizeof(*_tmp785)),((_tmp785[0]=((_tmp784.tag=0,((_tmp784.f1=_tmp183,
_tmp784)))),_tmp785))))),_tmp783)));}_LL112: {struct Cyc_Absyn_Primop_e_struct*
_tmp184=(struct Cyc_Absyn_Primop_e_struct*)_tmp156;if(_tmp184->tag != 3)goto _LL114;
else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}_LL113: {union Cyc_CfFlowInfo_FlowInfo
_tmp1F1;void*_tmp1F2;struct _tuple13 _tmp1F0=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp186);_tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;{struct _tuple13 _tmp786;
return(_tmp786.f1=_tmp1F1,((_tmp786.f2=_tmp1F2,_tmp786)));};}_LL114: {struct Cyc_Absyn_Increment_e_struct*
_tmp187=(struct Cyc_Absyn_Increment_e_struct*)_tmp156;if(_tmp187->tag != 5)goto
_LL116;else{_tmp188=_tmp187->f1;}}_LL115: {struct Cyc_List_List _tmp787;struct Cyc_List_List
_tmp1F4=(_tmp787.hd=_tmp188,((_tmp787.tl=0,_tmp787)));union Cyc_CfFlowInfo_FlowInfo
_tmp1F6;struct _tuple13 _tmp1F5=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp1F4);_tmp1F6=_tmp1F5.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp1F8;struct _tuple14 _tmp1F7=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F6,0,_tmp188);
_tmp1F8=_tmp1F7.f2;{struct _tuple20 _tmp788;struct _tuple20 _tmp1FA=(_tmp788.f1=
_tmp1F8,((_tmp788.f2=_tmp1F6,_tmp788)));union Cyc_CfFlowInfo_AbsLVal _tmp1FB;
struct Cyc_CfFlowInfo_Place*_tmp1FC;union Cyc_CfFlowInfo_FlowInfo _tmp1FD;struct
_tuple12 _tmp1FE;struct Cyc_Dict_Dict _tmp1FF;struct Cyc_List_List*_tmp200;_LL155:
_tmp1FB=_tmp1FA.f1;if((_tmp1FB.PlaceL).tag != 1)goto _LL157;_tmp1FC=(struct Cyc_CfFlowInfo_Place*)(
_tmp1FB.PlaceL).val;_tmp1FD=_tmp1FA.f2;if((_tmp1FD.ReachableFL).tag != 2)goto
_LL157;_tmp1FE=(struct _tuple12)(_tmp1FD.ReachableFL).val;_tmp1FF=_tmp1FE.f1;
_tmp200=_tmp1FE.f2;_LL156: _tmp200=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,
_tmp200,_tmp188);_tmp1F6=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp137,_tmp188->loc,_tmp1FF,env->all_changed,_tmp1FC,_tmp137->unknown_all),
_tmp200);goto _LL154;_LL157:;_LL158: goto _LL154;_LL154:;}{struct _tuple13 _tmp789;
return(_tmp789.f1=_tmp1F6,((_tmp789.f2=_tmp137->unknown_all,_tmp789)));};};}
_LL116: {struct Cyc_Absyn_AssignOp_e_struct*_tmp189=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp156;if(_tmp189->tag != 4)goto _LL118;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;
if(_tmp18B == 0)goto _LL118;_tmp18C=*_tmp18B;_tmp18D=_tmp189->f3;}}_LL117: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_Tcutil_terr(
e->loc,((_tmp78C="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp78C,sizeof(char),58))),_tag_dyneither(_tmp78B,sizeof(void*),0)));}{
struct _tuple13 _tmp78D;return(_tmp78D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp78D.f2=
_tmp137->unknown_all,_tmp78D)));};}{struct Cyc_List_List _tmp78E;struct Cyc_List_List
_tmp206=(_tmp78E.hd=_tmp18D,((_tmp78E.tl=0,_tmp78E)));struct Cyc_List_List _tmp78F;
struct Cyc_List_List _tmp207=(_tmp78F.hd=_tmp18A,((_tmp78F.tl=(struct Cyc_List_List*)&
_tmp206,_tmp78F)));union Cyc_CfFlowInfo_FlowInfo _tmp209;struct _tuple13 _tmp208=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp207);_tmp209=_tmp208.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp20B;struct _tuple14 _tmp20A=Cyc_NewControlFlow_anal_Lexp(env,_tmp209,0,_tmp18A);
_tmp20B=_tmp20A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp20C=_tmp209;struct _tuple12
_tmp20D;struct Cyc_Dict_Dict _tmp20E;struct Cyc_List_List*_tmp20F;_LL15A: if((
_tmp20C.ReachableFL).tag != 2)goto _LL15C;_tmp20D=(struct _tuple12)(_tmp20C.ReachableFL).val;
_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;_LL15B:{union Cyc_CfFlowInfo_AbsLVal _tmp210=
_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp211;int _tmp212;_LL15F: if((_tmp210.PlaceL).tag
!= 1)goto _LL161;_tmp211=(struct Cyc_CfFlowInfo_Place*)(_tmp210.PlaceL).val;_LL160:
_tmp20F=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,_tmp20F,_tmp18A);_tmp20E=Cyc_CfFlowInfo_assign_place(
_tmp137,_tmp18A->loc,_tmp20E,env->all_changed,_tmp211,_tmp137->unknown_all);
_tmp209=Cyc_CfFlowInfo_ReachableFL(_tmp20E,_tmp20F);goto _LL15E;_LL161: if((
_tmp210.UnknownL).tag != 2)goto _LL15E;_tmp212=(int)(_tmp210.UnknownL).val;_LL162:
goto _LL15E;_LL15E:;}goto _LL159;_LL15C:;_LL15D: goto _LL159;_LL159:;}{struct
_tuple13 _tmp790;return(_tmp790.f1=_tmp209,((_tmp790.f2=_tmp137->unknown_all,
_tmp790)));};};};_LL118: {struct Cyc_Absyn_AssignOp_e_struct*_tmp18E=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp156;if(_tmp18E->tag != 4)goto _LL11A;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;
if(_tmp190 != 0)goto _LL11A;_tmp191=_tmp18E->f3;}}_LL119: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp793;void*_tmp792;(_tmp792=0,Cyc_Tcutil_terr(e->loc,((_tmp793="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp793,sizeof(char),58))),_tag_dyneither(_tmp792,sizeof(void*),0)));}{
struct _tuple13 _tmp794;return(_tmp794.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp794.f2=
_tmp137->unknown_all,_tmp794)));};}{struct Cyc_Dict_Dict*_tmp219=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp795;env->all_changed=((_tmp795=_region_malloc(
env->r,sizeof(*_tmp795)),((_tmp795[0]=_tmp137->mt_place_set,_tmp795))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp21C;union Cyc_CfFlowInfo_AbsLVal _tmp21D;struct _tuple14
_tmp21B=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp18F);_tmp21C=_tmp21B.f1;
_tmp21D=_tmp21B.f2;{struct Cyc_Dict_Dict _tmp21E=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp796;env->all_changed=((
_tmp796=_region_malloc(env->r,sizeof(*_tmp796)),((_tmp796[0]=_tmp137->mt_place_set,
_tmp796))));}{union Cyc_CfFlowInfo_FlowInfo _tmp221;void*_tmp222;struct _tuple13
_tmp220=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp191);_tmp221=_tmp220.f1;
_tmp222=_tmp220.f2;{struct Cyc_Dict_Dict _tmp223=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp224=Cyc_CfFlowInfo_after_flow(
_tmp137,(struct Cyc_Dict_Dict*)& _tmp21E,_tmp21C,_tmp221,_tmp21E,_tmp223);union Cyc_CfFlowInfo_FlowInfo
_tmp225=Cyc_CfFlowInfo_join_flow(_tmp137,_tmp219,inflow,_tmp224);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp225,inflow)){if(_tmp219 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp219,Cyc_CfFlowInfo_union_place_set(
_tmp21E,_tmp223,0),0);}return Cyc_NewControlFlow_do_assign(_tmp137,env,_tmp224,
_tmp18F,_tmp21D,_tmp191,_tmp222,e->loc);}inflow=_tmp225;};};};};}};_LL11A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp192=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp156;if(_tmp192->tag != 9)goto _LL11C;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}
_LL11B: {union Cyc_CfFlowInfo_FlowInfo _tmp227;void*_tmp228;struct _tuple13 _tmp226=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp193);_tmp227=_tmp226.f1;_tmp228=
_tmp226.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp227,_tmp194);}
_LL11C: {struct Cyc_Absyn_Throw_e_struct*_tmp195=(struct Cyc_Absyn_Throw_e_struct*)
_tmp156;if(_tmp195->tag != 12)goto _LL11E;else{_tmp196=_tmp195->f1;}}_LL11D: {
union Cyc_CfFlowInfo_FlowInfo _tmp22A;void*_tmp22B;struct _tuple13 _tmp229=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp196);_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp196->loc,_tmp22A,_tmp22B);{struct _tuple13 _tmp797;return(_tmp797.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp797.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp137->unknown_all),_tmp797)));};}_LL11E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp197=(struct Cyc_Absyn_FnCall_e_struct*)_tmp156;if(_tmp197->tag != 11)goto
_LL120;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;}}_LL11F: {struct
_RegionHandle*_tmp22D=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp230;struct Cyc_List_List*
_tmp231;struct Cyc_List_List*_tmp798;struct _tuple19 _tmp22F=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp22D,env,inflow,((_tmp798=_region_malloc(_tmp22D,sizeof(*_tmp798)),((_tmp798->hd=
_tmp198,((_tmp798->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp22D,_tmp199),_tmp798)))))),0,1);_tmp230=_tmp22F.f1;
_tmp231=_tmp22F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp232=Cyc_NewControlFlow_use_Rval(
env,_tmp198->loc,_tmp230,(void*)((struct Cyc_List_List*)_check_null(_tmp231))->hd);
_tmp231=_tmp231->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp233=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp198->topt))->v);struct Cyc_Absyn_PtrInfo _tmp235;void*_tmp236;
_LL164: {struct Cyc_Absyn_PointerType_struct*_tmp234=(struct Cyc_Absyn_PointerType_struct*)
_tmp233;if(_tmp234->tag != 5)goto _LL166;else{_tmp235=_tmp234->f1;_tmp236=_tmp235.elt_typ;}}
_LL165:{void*_tmp237=Cyc_Tcutil_compress(_tmp236);struct Cyc_Absyn_FnInfo _tmp239;
struct Cyc_List_List*_tmp23A;_LL169: {struct Cyc_Absyn_FnType_struct*_tmp238=(
struct Cyc_Absyn_FnType_struct*)_tmp237;if(_tmp238->tag != 10)goto _LL16B;else{
_tmp239=_tmp238->f1;_tmp23A=_tmp239.attributes;}}_LL16A: for(0;_tmp23A != 0;
_tmp23A=_tmp23A->tl){void*_tmp23B=(void*)_tmp23A->hd;int _tmp23D;int _tmp23F;
_LL16E: {struct Cyc_Absyn_Initializes_att_struct*_tmp23C=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp23B;if(_tmp23C->tag != 20)goto _LL170;else{_tmp23D=_tmp23C->f1;}}_LL16F:{
struct Cyc_List_List*_tmp799;init_params=((_tmp799=_region_malloc(_tmp22D,sizeof(*
_tmp799)),((_tmp799->hd=(void*)_tmp23D,((_tmp799->tl=init_params,_tmp799))))));}
goto _LL16D;_LL170: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp23E=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp23B;if(_tmp23E->tag != 21)goto _LL172;else{_tmp23F=_tmp23E->f1;}}_LL171:{
struct Cyc_List_List*_tmp79A;nolive_unique_params=((_tmp79A=_region_malloc(
_tmp22D,sizeof(*_tmp79A)),((_tmp79A->hd=(void*)_tmp23F,((_tmp79A->tl=
nolive_unique_params,_tmp79A))))));}goto _LL16D;_LL172:;_LL173: goto _LL16D;_LL16D:;}
goto _LL168;_LL16B:;_LL16C: {const char*_tmp79D;void*_tmp79C;(_tmp79C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp79D="anal_Rexp: bad function type",
_tag_dyneither(_tmp79D,sizeof(char),29))),_tag_dyneither(_tmp79C,sizeof(void*),0)));}
_LL168:;}goto _LL163;_LL166:;_LL167: {const char*_tmp7A0;void*_tmp79F;(_tmp79F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7A0="anal_Rexp: bad function type",_tag_dyneither(_tmp7A0,sizeof(char),29))),
_tag_dyneither(_tmp79F,sizeof(void*),0)));}_LL163:;}{int i=1;for(0;_tmp231 != 0;(((
_tmp231=_tmp231->tl,_tmp199=((struct Cyc_List_List*)_check_null(_tmp199))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp232=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp199))->hd)->loc,_tmp232,(void*)_tmp231->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp232=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp199))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp199))->hd)->topt))->v,_tmp232,(void*)_tmp231->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp246=_tmp230;int _tmp247;struct _tuple12 _tmp248;
struct Cyc_Dict_Dict _tmp249;_LL175: if((_tmp246.BottomFL).tag != 1)goto _LL177;
_tmp247=(int)(_tmp246.BottomFL).val;_LL176: goto _LL174;_LL177: if((_tmp246.ReachableFL).tag
!= 2)goto _LL174;_tmp248=(struct _tuple12)(_tmp246.ReachableFL).val;_tmp249=
_tmp248.f1;_LL178: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp249,(void*)_tmp231->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp199))->hd)->loc,((
_tmp7A3="expression may not be initialized",_tag_dyneither(_tmp7A3,sizeof(char),
34))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp24C=_tmp232;int _tmp24D;struct _tuple12 _tmp24E;struct Cyc_Dict_Dict _tmp24F;
struct Cyc_List_List*_tmp250;_LL17A: if((_tmp24C.BottomFL).tag != 1)goto _LL17C;
_tmp24D=(int)(_tmp24C.BottomFL).val;_LL17B: goto _LL179;_LL17C: if((_tmp24C.ReachableFL).tag
!= 2)goto _LL179;_tmp24E=(struct _tuple12)(_tmp24C.ReachableFL).val;_tmp24F=
_tmp24E.f1;_tmp250=_tmp24E.f2;_LL17D: {struct Cyc_Dict_Dict _tmp251=Cyc_CfFlowInfo_escape_deref(
_tmp137,_tmp24F,env->all_changed,(void*)_tmp231->hd);{void*_tmp252=(void*)
_tmp231->hd;struct Cyc_CfFlowInfo_Place*_tmp254;_LL17F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp253=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp252;if(_tmp253->tag != 5)goto
_LL181;else{_tmp254=_tmp253->f1;}}_LL180:{void*_tmp255=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp199))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp257;void*
_tmp258;_LL184: {struct Cyc_Absyn_PointerType_struct*_tmp256=(struct Cyc_Absyn_PointerType_struct*)
_tmp255;if(_tmp256->tag != 5)goto _LL186;else{_tmp257=_tmp256->f1;_tmp258=_tmp257.elt_typ;}}
_LL185: _tmp251=Cyc_CfFlowInfo_assign_place(_tmp137,((struct Cyc_Absyn_Exp*)
_tmp199->hd)->loc,_tmp251,env->all_changed,_tmp254,Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,_tmp258,_tmp137->esc_all));goto _LL183;_LL186:;_LL187: {const char*_tmp7A6;
void*_tmp7A5;(_tmp7A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp7A6="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7A6,sizeof(char),39))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}
_LL183:;}goto _LL17E;_LL181:;_LL182: goto _LL17E;_LL17E:;}_tmp232=Cyc_CfFlowInfo_ReachableFL(
_tmp251,_tmp250);goto _LL179;}_LL179:;}goto _LL174;_LL174:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp198->topt))->v)){struct _tuple13
_tmp7A7;return(_tmp7A7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A7.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp137->unknown_all),
_tmp7A7)));}else{struct _tuple13 _tmp7A8;return(_tmp7A8.f1=_tmp232,((_tmp7A8.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp137->unknown_all),_tmp7A8)));}};};}_LL120: {struct Cyc_Absyn_Malloc_e_struct*
_tmp19A=(struct Cyc_Absyn_Malloc_e_struct*)_tmp156;if(_tmp19A->tag != 34)goto
_LL122;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19B.is_calloc;_tmp19D=_tmp19B.rgn;
_tmp19E=_tmp19B.elt_type;_tmp19F=_tmp19B.num_elts;_tmp1A0=_tmp19B.fat_result;}}
_LL121: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7AB;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7AA;void*root=(void*)((_tmp7AA=_region_malloc(_tmp137->r,sizeof(*_tmp7AA)),((
_tmp7AA[0]=((_tmp7AB.tag=1,((_tmp7AB.f1=_tmp19F,((_tmp7AB.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7AB)))))),_tmp7AA))));struct
Cyc_CfFlowInfo_Place*_tmp7AC;struct Cyc_CfFlowInfo_Place*place=(_tmp7AC=
_region_malloc(_tmp137->r,sizeof(*_tmp7AC)),((_tmp7AC->root=root,((_tmp7AC->fields=
0,_tmp7AC)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7AF;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7AE;void*rval=(void*)((_tmp7AE=_region_malloc(_tmp137->r,sizeof(*_tmp7AE)),((
_tmp7AE[0]=((_tmp7AF.tag=5,((_tmp7AF.f1=place,_tmp7AF)))),_tmp7AE))));void*
place_val=_tmp1A0?_tmp137->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp137,*((
void**)_check_null(_tmp19E)),_tmp137->unknown_none);union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp19D != 0){struct _RegionHandle*_tmp25D=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Exp*_tmp7B0[2];struct
_tuple19 _tmp25F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp25D,env,inflow,((
_tmp7B0[1]=_tmp19F,((_tmp7B0[0]=(struct Cyc_Absyn_Exp*)_tmp19D,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp25D,
_tag_dyneither(_tmp7B0,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp260=_tmp25F.f1;
_tmp261=_tmp25F.f2;outflow=_tmp260;}else{union Cyc_CfFlowInfo_FlowInfo _tmp263;
struct _tuple13 _tmp262=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19F);_tmp263=
_tmp262.f1;outflow=_tmp263;}{union Cyc_CfFlowInfo_FlowInfo _tmp264=outflow;int
_tmp265;struct _tuple12 _tmp266;struct Cyc_Dict_Dict _tmp267;struct Cyc_List_List*
_tmp268;_LL189: if((_tmp264.BottomFL).tag != 1)goto _LL18B;_tmp265=(int)(_tmp264.BottomFL).val;
_LL18A: {struct _tuple13 _tmp7B1;return(_tmp7B1.f1=outflow,((_tmp7B1.f2=rval,
_tmp7B1)));}_LL18B: if((_tmp264.ReachableFL).tag != 2)goto _LL188;_tmp266=(struct
_tuple12)(_tmp264.ReachableFL).val;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;_LL18C: {
struct _tuple13 _tmp7B2;return(_tmp7B2.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp267,root,place_val),
_tmp268),((_tmp7B2.f2=rval,_tmp7B2)));}_LL188:;};}_LL122: {struct Cyc_Absyn_Swap_e_struct*
_tmp1A1=(struct Cyc_Absyn_Swap_e_struct*)_tmp156;if(_tmp1A1->tag != 35)goto _LL124;
else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LL123: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp270=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp273;struct Cyc_List_List*_tmp274;struct Cyc_Absyn_Exp*
_tmp7B3[2];struct _tuple19 _tmp272=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp270,
env,inflow,((_tmp7B3[1]=_tmp1A3,((_tmp7B3[0]=_tmp1A2,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp270,
_tag_dyneither(_tmp7B3,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp273=_tmp272.f1;
_tmp274=_tmp272.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp274))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp274->tl))->hd;outflow=
_tmp273;}{union Cyc_CfFlowInfo_AbsLVal _tmp276;struct _tuple14 _tmp275=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1A2);_tmp276=_tmp275.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp278;
struct _tuple14 _tmp277=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1A3);
_tmp278=_tmp277.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp279=outflow;struct _tuple12
_tmp27A;struct Cyc_Dict_Dict _tmp27B;struct Cyc_List_List*_tmp27C;_LL18E: if((
_tmp279.ReachableFL).tag != 2)goto _LL190;_tmp27A=(struct _tuple12)(_tmp279.ReachableFL).val;
_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;_LL18F:{union Cyc_CfFlowInfo_AbsLVal _tmp27D=
_tmp276;struct Cyc_CfFlowInfo_Place*_tmp27E;int _tmp27F;_LL193: if((_tmp27D.PlaceL).tag
!= 1)goto _LL195;_tmp27E=(struct Cyc_CfFlowInfo_Place*)(_tmp27D.PlaceL).val;_LL194:
_tmp27B=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp1A2->loc,_tmp27B,env->all_changed,
_tmp27E,right_rval);goto _LL192;_LL195: if((_tmp27D.UnknownL).tag != 2)goto _LL192;
_tmp27F=(int)(_tmp27D.UnknownL).val;_LL196: goto _LL192;_LL192:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp280=_tmp278;struct Cyc_CfFlowInfo_Place*_tmp281;int _tmp282;_LL198: if((_tmp280.PlaceL).tag
!= 1)goto _LL19A;_tmp281=(struct Cyc_CfFlowInfo_Place*)(_tmp280.PlaceL).val;_LL199:
_tmp27B=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp1A3->loc,_tmp27B,env->all_changed,
_tmp281,left_rval);goto _LL197;_LL19A: if((_tmp280.UnknownL).tag != 2)goto _LL197;
_tmp282=(int)(_tmp280.UnknownL).val;_LL19B: goto _LL197;_LL197:;}_tmp27C=Cyc_CfFlowInfo_reln_kill_exp(
_tmp137->r,_tmp27C,_tmp1A2);_tmp27C=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,
_tmp27C,_tmp1A3);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp27B,_tmp27C);goto _LL18D;
_LL190:;_LL191: goto _LL18D;_LL18D:;}{struct _tuple13 _tmp7B4;return(_tmp7B4.f1=
outflow,((_tmp7B4.f2=_tmp137->unknown_all,_tmp7B4)));};};};}_LL124: {struct Cyc_Absyn_New_e_struct*
_tmp1A4=(struct Cyc_Absyn_New_e_struct*)_tmp156;if(_tmp1A4->tag != 17)goto _LL126;
else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LL125: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp7B7;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp7B6;void*root=(void*)((_tmp7B6=
_region_malloc(_tmp137->r,sizeof(*_tmp7B6)),((_tmp7B6[0]=((_tmp7B7.tag=1,((
_tmp7B7.f1=_tmp1A6,((_tmp7B7.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp7B7)))))),_tmp7B6))));struct Cyc_CfFlowInfo_Place*_tmp7B8;
struct Cyc_CfFlowInfo_Place*place=(_tmp7B8=_region_malloc(_tmp137->r,sizeof(*
_tmp7B8)),((_tmp7B8->root=root,((_tmp7B8->fields=0,_tmp7B8)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp7BB;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7BA;void*rval=(void*)((_tmp7BA=
_region_malloc(_tmp137->r,sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BB.tag=5,((
_tmp7BB.f1=place,_tmp7BB)))),_tmp7BA))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1A5 != 0){struct _RegionHandle*_tmp284=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp287;struct Cyc_List_List*_tmp288;struct Cyc_Absyn_Exp*_tmp7BC[2];struct
_tuple19 _tmp286=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp284,env,inflow,((
_tmp7BC[1]=_tmp1A6,((_tmp7BC[0]=(struct Cyc_Absyn_Exp*)_tmp1A5,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp284,
_tag_dyneither(_tmp7BC,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp287=_tmp286.f1;
_tmp288=_tmp286.f2;outflow=_tmp287;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp288))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp28A;void*_tmp28B;struct _tuple13 _tmp289=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1A6);_tmp28A=_tmp289.f1;_tmp28B=_tmp289.f2;outflow=_tmp28A;
place_val=_tmp28B;}{union Cyc_CfFlowInfo_FlowInfo _tmp28C=outflow;int _tmp28D;
struct _tuple12 _tmp28E;struct Cyc_Dict_Dict _tmp28F;struct Cyc_List_List*_tmp290;
_LL19D: if((_tmp28C.BottomFL).tag != 1)goto _LL19F;_tmp28D=(int)(_tmp28C.BottomFL).val;
_LL19E: {struct _tuple13 _tmp7BD;return(_tmp7BD.f1=outflow,((_tmp7BD.f2=rval,
_tmp7BD)));}_LL19F: if((_tmp28C.ReachableFL).tag != 2)goto _LL19C;_tmp28E=(struct
_tuple12)(_tmp28C.ReachableFL).val;_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;_LL1A0: {
struct _tuple13 _tmp7BE;return(_tmp7BE.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp28F,root,place_val),
_tmp290),((_tmp7BE.f2=rval,_tmp7BE)));}_LL19C:;};};}_LL126: {struct Cyc_Absyn_Address_e_struct*
_tmp1A7=(struct Cyc_Absyn_Address_e_struct*)_tmp156;if(_tmp1A7->tag != 16)goto
_LL128;else{_tmp1A8=_tmp1A7->f1;}}_LL127: {union Cyc_CfFlowInfo_FlowInfo _tmp299;
union Cyc_CfFlowInfo_AbsLVal _tmp29A;struct _tuple14 _tmp298=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1A8);_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp29B=_tmp29A;int _tmp29C;struct Cyc_CfFlowInfo_Place*_tmp29D;_LL1A2: if((_tmp29B.UnknownL).tag
!= 2)goto _LL1A4;_tmp29C=(int)(_tmp29B.UnknownL).val;_LL1A3: {struct _tuple13
_tmp7BF;return(_tmp7BF.f1=_tmp299,((_tmp7BF.f2=_tmp137->notzeroall,_tmp7BF)));}
_LL1A4: if((_tmp29B.PlaceL).tag != 1)goto _LL1A1;_tmp29D=(struct Cyc_CfFlowInfo_Place*)(
_tmp29B.PlaceL).val;_LL1A5: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7C5;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp7C4;struct _tuple13 _tmp7C3;return(
_tmp7C3.f1=_tmp299,((_tmp7C3.f2=(void*)((_tmp7C5=_region_malloc(env->r,sizeof(*
_tmp7C5)),((_tmp7C5[0]=((_tmp7C4.tag=5,((_tmp7C4.f1=_tmp29D,_tmp7C4)))),_tmp7C5)))),
_tmp7C3)));}_LL1A1:;};}_LL128: {struct Cyc_Absyn_Deref_e_struct*_tmp1A9=(struct
Cyc_Absyn_Deref_e_struct*)_tmp156;if(_tmp1A9->tag != 21)goto _LL12A;else{_tmp1AA=
_tmp1A9->f1;}}_LL129: {union Cyc_CfFlowInfo_FlowInfo _tmp2A3;void*_tmp2A4;struct
_tuple13 _tmp2A2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AA);_tmp2A3=
_tmp2A2.f1;_tmp2A4=_tmp2A2.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2A3,_tmp1AA,_tmp2A4);}_LL12A: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1AB=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp156;if(_tmp1AB->tag != 22)goto _LL12C;
else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL12B: {union Cyc_CfFlowInfo_FlowInfo
_tmp2A6;void*_tmp2A7;struct _tuple13 _tmp2A5=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1AC);_tmp2A6=_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1AC->topt))->v)){struct _tuple13
_tmp7C6;return(_tmp7C6.f1=_tmp2A6,((_tmp7C6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp137->unknown_all),
_tmp7C6)));}{void*_tmp2A9=_tmp2A7;int _tmp2AB;struct _dyneither_ptr _tmp2AC;_LL1A7: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2AA=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2A9;if(_tmp2AA->tag != 6)goto _LL1A9;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AA->f2;}}
_LL1A8: {int _tmp2AD=Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1AC->topt))->v,_tmp1AD);struct _tuple13 _tmp7C7;return(_tmp7C7.f1=
_tmp2A6,((_tmp7C7.f2=*((void**)_check_dyneither_subscript(_tmp2AC,sizeof(void*),
_tmp2AD)),_tmp7C7)));}_LL1A9:;_LL1AA: {struct Cyc_Core_Impossible_struct _tmp7D4;
const char*_tmp7D3;void*_tmp7D2[1];struct Cyc_String_pa_struct _tmp7D1;struct Cyc_Core_Impossible_struct*
_tmp7D0;(int)_throw((void*)((_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0[0]=((
_tmp7D4.tag=Cyc_Core_Impossible,((_tmp7D4.f1=(struct _dyneither_ptr)((_tmp7D1.tag=
0,((_tmp7D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp7D2[0]=& _tmp7D1,Cyc_aprintf(((_tmp7D3="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp7D3,sizeof(char),26))),_tag_dyneither(_tmp7D2,sizeof(void*),1)))))))),
_tmp7D4)))),_tmp7D0)))));}_LL1A6:;};}_LL12C: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1AE=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp156;if(_tmp1AE->tag != 38)goto
_LL12E;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LL12D: {union Cyc_CfFlowInfo_FlowInfo
_tmp2B5;void*_tmp2B6;struct _tuple13 _tmp2B4=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1AF);_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;{struct _tuple13 _tmp7D5;
return(_tmp7D5.f1=_tmp2B5,((_tmp7D5.f2=_tmp137->unknown_all,_tmp7D5)));};}_LL12E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp1B1=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp156;if(_tmp1B1->tag != 23)goto _LL130;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}
_LL12F: {union Cyc_CfFlowInfo_FlowInfo _tmp2B9;void*_tmp2BA;struct _tuple13 _tmp2B8=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1B2);_tmp2B9=_tmp2B8.f1;_tmp2BA=
_tmp2B8.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2BC;void*_tmp2BD;struct _tuple13
_tmp2BB=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2B9,_tmp1B2,_tmp2BA);
_tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;{void*_tmp2BE=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B2->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2C0;void*_tmp2C1;_LL1AC: {struct Cyc_Absyn_PointerType_struct*_tmp2BF=(struct
Cyc_Absyn_PointerType_struct*)_tmp2BE;if(_tmp2BF->tag != 5)goto _LL1AE;else{
_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2C0.elt_typ;}}_LL1AD: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2C1)){struct _tuple13 _tmp7D6;return(_tmp7D6.f1=_tmp2BC,((_tmp7D6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp137->unknown_all),
_tmp7D6)));}{void*_tmp2C3=_tmp2BD;int _tmp2C5;struct _dyneither_ptr _tmp2C6;_LL1B1: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2C4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2C3;if(_tmp2C4->tag != 6)goto _LL1B3;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;}}
_LL1B2: {int _tmp2C7=Cyc_CfFlowInfo_get_field_index(_tmp2C1,_tmp1B3);struct
_tuple13 _tmp7D7;return(_tmp7D7.f1=_tmp2BC,((_tmp7D7.f2=*((void**)
_check_dyneither_subscript(_tmp2C6,sizeof(void*),_tmp2C7)),_tmp7D7)));}_LL1B3:;
_LL1B4: {struct Cyc_Core_Impossible_struct _tmp7DD;const char*_tmp7DC;struct Cyc_Core_Impossible_struct*
_tmp7DB;(int)_throw((void*)((_tmp7DB=_cycalloc(sizeof(*_tmp7DB)),((_tmp7DB[0]=((
_tmp7DD.tag=Cyc_Core_Impossible,((_tmp7DD.f1=((_tmp7DC="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp7DC,sizeof(char),21))),_tmp7DD)))),_tmp7DB)))));}_LL1B0:;};
_LL1AE:;_LL1AF: {struct Cyc_Core_Impossible_struct _tmp7E3;const char*_tmp7E2;
struct Cyc_Core_Impossible_struct*_tmp7E1;(int)_throw((void*)((_tmp7E1=_cycalloc(
sizeof(*_tmp7E1)),((_tmp7E1[0]=((_tmp7E3.tag=Cyc_Core_Impossible,((_tmp7E3.f1=((
_tmp7E2="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7E2,sizeof(char),25))),
_tmp7E3)))),_tmp7E1)))));}_LL1AB:;};};}_LL130: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1B4=(struct Cyc_Absyn_Conditional_e_struct*)_tmp156;if(_tmp1B4->tag != 6)goto
_LL132;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;_tmp1B7=_tmp1B4->f3;}}_LL131: {
union Cyc_CfFlowInfo_FlowInfo _tmp2D0;union Cyc_CfFlowInfo_FlowInfo _tmp2D1;struct
_tuple15 _tmp2CF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B5);_tmp2D0=_tmp2CF.f1;
_tmp2D1=_tmp2CF.f2;{struct _tuple13 _tmp2D2=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2D0,_tmp1B6);struct _tuple13 _tmp2D3=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp2D1,_tmp1B7);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp137,
env->all_changed,_tmp2D2,_tmp2D3);};}_LL132: {struct Cyc_Absyn_And_e_struct*
_tmp1B8=(struct Cyc_Absyn_And_e_struct*)_tmp156;if(_tmp1B8->tag != 7)goto _LL134;
else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;}}_LL133: {union Cyc_CfFlowInfo_FlowInfo
_tmp2D5;union Cyc_CfFlowInfo_FlowInfo _tmp2D6;struct _tuple15 _tmp2D4=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1B9);_tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp2D8;void*_tmp2D9;struct _tuple13 _tmp2D7=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2D5,_tmp1BA);_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;{struct _tuple13
_tmp7E4;struct _tuple13 _tmp2DA=(_tmp7E4.f1=_tmp2D8,((_tmp7E4.f2=_tmp2D9,_tmp7E4)));
struct _tuple13 _tmp7E5;struct _tuple13 _tmp2DB=(_tmp7E5.f1=_tmp2D6,((_tmp7E5.f2=
_tmp137->zero,_tmp7E5)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp137,env->all_changed,
_tmp2DA,_tmp2DB);};};}_LL134: {struct Cyc_Absyn_Or_e_struct*_tmp1BB=(struct Cyc_Absyn_Or_e_struct*)
_tmp156;if(_tmp1BB->tag != 8)goto _LL136;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}
_LL135: {union Cyc_CfFlowInfo_FlowInfo _tmp2DF;union Cyc_CfFlowInfo_FlowInfo _tmp2E0;
struct _tuple15 _tmp2DE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1BC);_tmp2DF=
_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;
struct _tuple13 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E0,_tmp1BD);
_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{struct _tuple13 _tmp7E6;struct _tuple13
_tmp2E4=(_tmp7E6.f1=_tmp2E2,((_tmp7E6.f2=_tmp2E3,_tmp7E6)));struct _tuple13
_tmp7E7;struct _tuple13 _tmp2E5=(_tmp7E7.f1=_tmp2DF,((_tmp7E7.f2=_tmp137->notzeroall,
_tmp7E7)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp137,env->all_changed,
_tmp2E4,_tmp2E5);};};}_LL136: {struct Cyc_Absyn_Subscript_e_struct*_tmp1BE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp156;if(_tmp1BE->tag != 24)goto _LL138;
else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;}}_LL137: {struct _RegionHandle*
_tmp2E8=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2EB;struct Cyc_List_List*_tmp2EC;
struct Cyc_Absyn_Exp*_tmp7E8[2];struct _tuple19 _tmp2EA=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2E8,env,inflow,((_tmp7E8[1]=_tmp1C0,((_tmp7E8[0]=_tmp1BF,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2E8,
_tag_dyneither(_tmp7E8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2EB=_tmp2EA.f1;
_tmp2EC=_tmp2EA.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2ED=_tmp2EB;{union Cyc_CfFlowInfo_FlowInfo
_tmp2EE=_tmp2EB;struct _tuple12 _tmp2EF;struct Cyc_Dict_Dict _tmp2F0;struct Cyc_List_List*
_tmp2F1;_LL1B6: if((_tmp2EE.ReachableFL).tag != 2)goto _LL1B8;_tmp2EF=(struct
_tuple12)(_tmp2EE.ReachableFL).val;_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;_LL1B7:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2F0,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2EC))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7EB;void*_tmp7EA;(_tmp7EA=0,Cyc_Tcutil_terr(_tmp1C0->loc,((_tmp7EB="expression may not be initialized",
_tag_dyneither(_tmp7EB,sizeof(char),34))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2F4=Cyc_NewControlFlow_add_subscript_reln(_tmp137->r,
_tmp2F1,_tmp1BF,_tmp1C0);if(_tmp2F1 != _tmp2F4)_tmp2ED=Cyc_CfFlowInfo_ReachableFL(
_tmp2F0,_tmp2F4);goto _LL1B5;};_LL1B8:;_LL1B9: goto _LL1B5;_LL1B5:;}{void*_tmp2F5=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BF->topt))->v);
struct Cyc_List_List*_tmp2F7;struct Cyc_Absyn_PtrInfo _tmp2F9;struct Cyc_Absyn_PtrAtts
_tmp2FA;union Cyc_Absyn_Constraint*_tmp2FB;_LL1BB: {struct Cyc_Absyn_TupleType_struct*
_tmp2F6=(struct Cyc_Absyn_TupleType_struct*)_tmp2F5;if(_tmp2F6->tag != 11)goto
_LL1BD;else{_tmp2F7=_tmp2F6->f1;}}_LL1BC: {void*_tmp2FC=(void*)((struct Cyc_List_List*)
_check_null(_tmp2EC))->hd;int _tmp2FE;struct _dyneither_ptr _tmp2FF;_LL1C2: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp2FD=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2FC;if(_tmp2FD->tag != 6)goto _LL1C4;else{_tmp2FE=_tmp2FD->f1;_tmp2FF=_tmp2FD->f2;}}
_LL1C3: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1C0)).f1;struct
_tuple13 _tmp7EC;return(_tmp7EC.f1=_tmp2ED,((_tmp7EC.f2=*((void**)
_check_dyneither_subscript(_tmp2FF,sizeof(void*),(int)i)),_tmp7EC)));}_LL1C4:;
_LL1C5: {struct Cyc_Core_Impossible_struct _tmp7F2;const char*_tmp7F1;struct Cyc_Core_Impossible_struct*
_tmp7F0;(int)_throw((void*)((_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0[0]=((
_tmp7F2.tag=Cyc_Core_Impossible,((_tmp7F2.f1=((_tmp7F1="anal_Rexp: Subscript",
_tag_dyneither(_tmp7F1,sizeof(char),21))),_tmp7F2)))),_tmp7F0)))));}_LL1C1:;}
_LL1BD: {struct Cyc_Absyn_PointerType_struct*_tmp2F8=(struct Cyc_Absyn_PointerType_struct*)
_tmp2F5;if(_tmp2F8->tag != 5)goto _LL1BF;else{_tmp2F9=_tmp2F8->f1;_tmp2FA=_tmp2F9.ptr_atts;
_tmp2FB=_tmp2FA.bounds;}}_LL1BE: {union Cyc_CfFlowInfo_FlowInfo _tmp305;void*
_tmp306;struct _tuple13 _tmp304=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2EB,
_tmp1BF,(void*)((struct Cyc_List_List*)_check_null(_tmp2EC))->hd);_tmp305=_tmp304.f1;
_tmp306=_tmp304.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp307=_tmp305;int _tmp308;
_LL1C7: if((_tmp307.BottomFL).tag != 1)goto _LL1C9;_tmp308=(int)(_tmp307.BottomFL).val;
_LL1C8: {struct _tuple13 _tmp7F3;return(_tmp7F3.f1=_tmp305,((_tmp7F3.f2=_tmp306,
_tmp7F3)));}_LL1C9:;_LL1CA: {struct _tuple13 _tmp7F4;return(_tmp7F4.f1=_tmp2ED,((
_tmp7F4.f2=_tmp306,_tmp7F4)));}_LL1C6:;};}_LL1BF:;_LL1C0: {struct Cyc_Core_Impossible_struct
_tmp7FA;const char*_tmp7F9;struct Cyc_Core_Impossible_struct*_tmp7F8;(int)_throw((
void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7FA.tag=Cyc_Core_Impossible,((
_tmp7FA.f1=((_tmp7F9="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7F9,
sizeof(char),33))),_tmp7FA)))),_tmp7F8)))));}_LL1BA:;};};}_LL138: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1C1=(struct Cyc_Absyn_Datatype_e_struct*)_tmp156;if(_tmp1C1->tag != 31)goto
_LL13A;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;}}_LL139: _tmp1C5=_tmp1C2;
goto _LL13B;_LL13A: {struct Cyc_Absyn_Tuple_e_struct*_tmp1C4=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp156;if(_tmp1C4->tag != 25)goto _LL13C;else{_tmp1C5=_tmp1C4->f1;}}_LL13B: {
struct _RegionHandle*_tmp30E=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp310;struct Cyc_List_List*
_tmp311;struct _tuple19 _tmp30F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp30E,
env,inflow,_tmp1C5,1,1);_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{unsigned int
_tmp800;unsigned int _tmp7FF;struct _dyneither_ptr _tmp7FE;void**_tmp7FD;
unsigned int _tmp7FC;struct _dyneither_ptr aggrdict=(_tmp7FC=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1C5),((_tmp7FD=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp7FC)),((_tmp7FE=
_tag_dyneither(_tmp7FD,sizeof(void*),_tmp7FC),((((_tmp7FF=_tmp7FC,_tmp801(&
_tmp800,& _tmp7FF,& _tmp7FD,& _tmp311))),_tmp7FE)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp807;struct Cyc_CfFlowInfo_Aggregate_struct _tmp806;struct _tuple13 _tmp805;
return(_tmp805.f1=_tmp310,((_tmp805.f2=(void*)((_tmp807=_region_malloc(env->r,
sizeof(*_tmp807)),((_tmp807[0]=((_tmp806.tag=6,((_tmp806.f1=0,((_tmp806.f2=
aggrdict,_tmp806)))))),_tmp807)))),_tmp805)));};}_LL13C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1C6=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp156;if(_tmp1C6->tag != 30)goto
_LL13E;else{_tmp1C7=_tmp1C6->f2;}}_LL13D: {struct Cyc_List_List*fs;{void*_tmp31A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp31C;_LL1CC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp31B=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp31A;if(_tmp31B->tag != 13)goto _LL1CE;
else{_tmp31C=_tmp31B->f2;}}_LL1CD: fs=_tmp31C;goto _LL1CB;_LL1CE:;_LL1CF: {struct
Cyc_Core_Impossible_struct _tmp80D;const char*_tmp80C;struct Cyc_Core_Impossible_struct*
_tmp80B;(int)_throw((void*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B[0]=((
_tmp80D.tag=Cyc_Core_Impossible,((_tmp80D.f1=((_tmp80C="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp80C,sizeof(char),35))),_tmp80D)))),_tmp80B)))));}_LL1CB:;}
_tmp1C9=_tmp1C7;_tmp1CC=Cyc_Absyn_StructA;_tmp1CF=fs;goto _LL13F;}_LL13E: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1C8=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp156;if(_tmp1C8->tag != 29)goto _LL140;else{_tmp1C9=_tmp1C8->f3;_tmp1CA=_tmp1C8->f4;
if(_tmp1CA == 0)goto _LL140;_tmp1CB=*_tmp1CA;_tmp1CC=_tmp1CB.kind;_tmp1CD=_tmp1CB.impl;
if(_tmp1CD == 0)goto _LL140;_tmp1CE=*_tmp1CD;_tmp1CF=_tmp1CE.fields;}}_LL13F: {
struct _RegionHandle*_tmp320=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp322;struct Cyc_List_List*
_tmp323;struct _tuple19 _tmp321=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp320,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp320,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp1C9),1,1);_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp137,
_tmp1CF,_tmp137->unknown_all);{int i=0;for(0;_tmp323 != 0;(((_tmp323=_tmp323->tl,
_tmp1C9=_tmp1C9->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple21*)((struct
Cyc_List_List*)_check_null(_tmp1C9))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp324=(void*)ds->hd;struct _dyneither_ptr*_tmp327;_LL1D1: {struct Cyc_Absyn_ArrayElement_struct*
_tmp325=(struct Cyc_Absyn_ArrayElement_struct*)_tmp324;if(_tmp325->tag != 0)goto
_LL1D3;}_LL1D2: {struct Cyc_Core_Impossible_struct _tmp813;const char*_tmp812;
struct Cyc_Core_Impossible_struct*_tmp811;(int)_throw((void*)((_tmp811=_cycalloc(
sizeof(*_tmp811)),((_tmp811[0]=((_tmp813.tag=Cyc_Core_Impossible,((_tmp813.f1=((
_tmp812="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp812,sizeof(char),22))),
_tmp813)))),_tmp811)))));}_LL1D3: {struct Cyc_Absyn_FieldName_struct*_tmp326=(
struct Cyc_Absyn_FieldName_struct*)_tmp324;if(_tmp326->tag != 1)goto _LL1D0;else{
_tmp327=_tmp326->f1;}}_LL1D4: {int _tmp32B=Cyc_CfFlowInfo_get_field_index_fs(
_tmp1CF,_tmp327);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp32B))=(void*)_tmp323->hd;if(_tmp1CC == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp32C=(*((struct _tuple21*)_tmp1C9->hd)).f2;_tmp322=Cyc_NewControlFlow_use_Rval(
env,_tmp32C->loc,_tmp322,(void*)_tmp323->hd);}}_LL1D0:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp819;struct Cyc_CfFlowInfo_Aggregate_struct _tmp818;struct _tuple13 _tmp817;
return(_tmp817.f1=_tmp322,((_tmp817.f2=(void*)((_tmp819=_region_malloc(env->r,
sizeof(*_tmp819)),((_tmp819[0]=((_tmp818.tag=6,((_tmp818.f1=_tmp1CC == Cyc_Absyn_UnionA,((
_tmp818.f2=aggrdict,_tmp818)))))),_tmp819)))),_tmp817)));};};}_LL140: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1D0=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp156;if(_tmp1D0->tag != 29)goto
_LL142;}_LL141: {struct Cyc_Core_Impossible_struct _tmp81F;const char*_tmp81E;
struct Cyc_Core_Impossible_struct*_tmp81D;(int)_throw((void*)((_tmp81D=_cycalloc(
sizeof(*_tmp81D)),((_tmp81D[0]=((_tmp81F.tag=Cyc_Core_Impossible,((_tmp81F.f1=((
_tmp81E="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp81E,sizeof(char),31))),
_tmp81F)))),_tmp81D)))));}_LL142: {struct Cyc_Absyn_Array_e_struct*_tmp1D1=(
struct Cyc_Absyn_Array_e_struct*)_tmp156;if(_tmp1D1->tag != 27)goto _LL144;else{
_tmp1D2=_tmp1D1->f1;}}_LL143: {struct _RegionHandle*_tmp333=env->r;struct Cyc_List_List*
_tmp334=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp333,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp1D2);union Cyc_CfFlowInfo_FlowInfo _tmp336;
struct Cyc_List_List*_tmp337;struct _tuple19 _tmp335=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp333,env,inflow,_tmp334,1,1);_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;for(0;
_tmp337 != 0;(_tmp337=_tmp337->tl,_tmp334=_tmp334->tl)){_tmp336=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp334))->hd)->loc,
_tmp336,(void*)_tmp337->hd);}{struct _tuple13 _tmp820;return(_tmp820.f1=_tmp336,((
_tmp820.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp137->unknown_all),_tmp820)));};}_LL144: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1D3=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp156;if(_tmp1D3->tag != 28)
goto _LL146;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;_tmp1D6=_tmp1D3->f3;
_tmp1D7=_tmp1D3->f4;}}_LL145: {union Cyc_CfFlowInfo_FlowInfo _tmp33A;void*_tmp33B;
struct _tuple13 _tmp339=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D5);_tmp33A=
_tmp339.f1;_tmp33B=_tmp339.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp33C=_tmp33A;int
_tmp33D;struct _tuple12 _tmp33E;struct Cyc_Dict_Dict _tmp33F;struct Cyc_List_List*
_tmp340;_LL1D6: if((_tmp33C.BottomFL).tag != 1)goto _LL1D8;_tmp33D=(int)(_tmp33C.BottomFL).val;
_LL1D7: {struct _tuple13 _tmp821;return(_tmp821.f1=_tmp33A,((_tmp821.f2=_tmp137->unknown_all,
_tmp821)));}_LL1D8: if((_tmp33C.ReachableFL).tag != 2)goto _LL1D5;_tmp33E=(struct
_tuple12)(_tmp33C.ReachableFL).val;_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;_LL1D9:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33F,_tmp33B)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp824;void*_tmp823;(_tmp823=0,Cyc_Tcutil_terr(_tmp1D5->loc,((_tmp824="expression may not be initialized",
_tag_dyneither(_tmp824,sizeof(char),34))),_tag_dyneither(_tmp823,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp340;comp_loop: {void*_tmp344=_tmp1D5->r;struct
Cyc_Absyn_Exp*_tmp346;void*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34A;void*_tmp34C;
struct Cyc_Absyn_Vardecl*_tmp34E;void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;
void*_tmp354;struct Cyc_Absyn_Vardecl*_tmp356;union Cyc_Absyn_Cnst _tmp358;struct
_tuple6 _tmp359;int _tmp35A;enum Cyc_Absyn_Primop _tmp35C;struct Cyc_List_List*
_tmp35D;struct Cyc_List_List _tmp35E;struct Cyc_Absyn_Exp*_tmp35F;_LL1DB: {struct
Cyc_Absyn_Cast_e_struct*_tmp345=(struct Cyc_Absyn_Cast_e_struct*)_tmp344;if(
_tmp345->tag != 15)goto _LL1DD;else{_tmp346=_tmp345->f2;}}_LL1DC: _tmp1D5=_tmp346;
goto comp_loop;_LL1DD:{struct Cyc_Absyn_Var_e_struct*_tmp347=(struct Cyc_Absyn_Var_e_struct*)
_tmp344;if(_tmp347->tag != 1)goto _LL1DF;else{_tmp348=(void*)_tmp347->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp349=(struct Cyc_Absyn_Global_b_struct*)_tmp348;if(
_tmp349->tag != 1)goto _LL1DF;else{_tmp34A=_tmp349->f1;}};}}if(!(!_tmp34A->escapes))
goto _LL1DF;_LL1DE: _tmp34E=_tmp34A;goto _LL1E0;_LL1DF:{struct Cyc_Absyn_Var_e_struct*
_tmp34B=(struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp34B->tag != 1)goto _LL1E1;
else{_tmp34C=(void*)_tmp34B->f2;{struct Cyc_Absyn_Local_b_struct*_tmp34D=(struct
Cyc_Absyn_Local_b_struct*)_tmp34C;if(_tmp34D->tag != 4)goto _LL1E1;else{_tmp34E=
_tmp34D->f1;}};}}if(!(!_tmp34E->escapes))goto _LL1E1;_LL1E0: _tmp352=_tmp34E;goto
_LL1E2;_LL1E1:{struct Cyc_Absyn_Var_e_struct*_tmp34F=(struct Cyc_Absyn_Var_e_struct*)
_tmp344;if(_tmp34F->tag != 1)goto _LL1E3;else{_tmp350=(void*)_tmp34F->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp351=(struct Cyc_Absyn_Pat_b_struct*)_tmp350;if(_tmp351->tag
!= 5)goto _LL1E3;else{_tmp352=_tmp351->f1;}};}}if(!(!_tmp352->escapes))goto _LL1E3;
_LL1E2: _tmp356=_tmp352;goto _LL1E4;_LL1E3:{struct Cyc_Absyn_Var_e_struct*_tmp353=(
struct Cyc_Absyn_Var_e_struct*)_tmp344;if(_tmp353->tag != 1)goto _LL1E5;else{
_tmp354=(void*)_tmp353->f2;{struct Cyc_Absyn_Param_b_struct*_tmp355=(struct Cyc_Absyn_Param_b_struct*)
_tmp354;if(_tmp355->tag != 3)goto _LL1E5;else{_tmp356=_tmp355->f1;}};}}if(!(!
_tmp356->escapes))goto _LL1E5;_LL1E4:{struct Cyc_CfFlowInfo_Reln*_tmp827;struct Cyc_List_List*
_tmp826;new_relns=((_tmp826=_region_malloc(env->r,sizeof(*_tmp826)),((_tmp826->hd=((
_tmp827=_region_malloc(env->r,sizeof(*_tmp827)),((_tmp827->vd=_tmp1D4,((_tmp827->rop=
Cyc_CfFlowInfo_LessVar(_tmp356,_tmp356->type),_tmp827)))))),((_tmp826->tl=
_tmp340,_tmp826))))));}goto _LL1DA;_LL1E5: {struct Cyc_Absyn_Const_e_struct*
_tmp357=(struct Cyc_Absyn_Const_e_struct*)_tmp344;if(_tmp357->tag != 0)goto _LL1E7;
else{_tmp358=_tmp357->f1;if((_tmp358.Int_c).tag != 4)goto _LL1E7;_tmp359=(struct
_tuple6)(_tmp358.Int_c).val;_tmp35A=_tmp359.f2;}}_LL1E6:{struct Cyc_CfFlowInfo_Reln*
_tmp82A;struct Cyc_List_List*_tmp829;new_relns=((_tmp829=_region_malloc(env->r,
sizeof(*_tmp829)),((_tmp829->hd=((_tmp82A=_region_malloc(env->r,sizeof(*_tmp82A)),((
_tmp82A->vd=_tmp1D4,((_tmp82A->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp35A),_tmp82A)))))),((_tmp829->tl=_tmp340,_tmp829))))));}goto _LL1DA;_LL1E7: {
struct Cyc_Absyn_Primop_e_struct*_tmp35B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp344;if(_tmp35B->tag != 3)goto _LL1E9;else{_tmp35C=_tmp35B->f1;_tmp35D=_tmp35B->f2;
if(_tmp35D == 0)goto _LL1E9;_tmp35E=*_tmp35D;_tmp35F=(struct Cyc_Absyn_Exp*)_tmp35E.hd;}}
_LL1E8:{void*_tmp364=_tmp35F->r;void*_tmp366;struct Cyc_Absyn_Vardecl*_tmp368;
void*_tmp36A;struct Cyc_Absyn_Vardecl*_tmp36C;void*_tmp36E;struct Cyc_Absyn_Vardecl*
_tmp370;void*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;_LL1EC:{struct Cyc_Absyn_Var_e_struct*
_tmp365=(struct Cyc_Absyn_Var_e_struct*)_tmp364;if(_tmp365->tag != 1)goto _LL1EE;
else{_tmp366=(void*)_tmp365->f2;{struct Cyc_Absyn_Global_b_struct*_tmp367=(struct
Cyc_Absyn_Global_b_struct*)_tmp366;if(_tmp367->tag != 1)goto _LL1EE;else{_tmp368=
_tmp367->f1;}};}}if(!(!_tmp368->escapes))goto _LL1EE;_LL1ED: _tmp36C=_tmp368;goto
_LL1EF;_LL1EE:{struct Cyc_Absyn_Var_e_struct*_tmp369=(struct Cyc_Absyn_Var_e_struct*)
_tmp364;if(_tmp369->tag != 1)goto _LL1F0;else{_tmp36A=(void*)_tmp369->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp36B=(struct Cyc_Absyn_Local_b_struct*)_tmp36A;if(
_tmp36B->tag != 4)goto _LL1F0;else{_tmp36C=_tmp36B->f1;}};}}if(!(!_tmp36C->escapes))
goto _LL1F0;_LL1EF: _tmp370=_tmp36C;goto _LL1F1;_LL1F0:{struct Cyc_Absyn_Var_e_struct*
_tmp36D=(struct Cyc_Absyn_Var_e_struct*)_tmp364;if(_tmp36D->tag != 1)goto _LL1F2;
else{_tmp36E=(void*)_tmp36D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp36F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp36E;if(_tmp36F->tag != 5)goto _LL1F2;else{_tmp370=_tmp36F->f1;}};}}if(!(!
_tmp370->escapes))goto _LL1F2;_LL1F1: _tmp374=_tmp370;goto _LL1F3;_LL1F2:{struct Cyc_Absyn_Var_e_struct*
_tmp371=(struct Cyc_Absyn_Var_e_struct*)_tmp364;if(_tmp371->tag != 1)goto _LL1F4;
else{_tmp372=(void*)_tmp371->f2;{struct Cyc_Absyn_Param_b_struct*_tmp373=(struct
Cyc_Absyn_Param_b_struct*)_tmp372;if(_tmp373->tag != 3)goto _LL1F4;else{_tmp374=
_tmp373->f1;}};}}if(!(!_tmp374->escapes))goto _LL1F4;_LL1F3:{struct Cyc_CfFlowInfo_Reln*
_tmp82D;struct Cyc_List_List*_tmp82C;new_relns=((_tmp82C=_region_malloc(env->r,
sizeof(*_tmp82C)),((_tmp82C->hd=((_tmp82D=_region_malloc(env->r,sizeof(*_tmp82D)),((
_tmp82D->vd=_tmp1D4,((_tmp82D->rop=Cyc_CfFlowInfo_LessNumelts(_tmp374),_tmp82D)))))),((
_tmp82C->tl=_tmp340,_tmp82C))))));}goto _LL1EB;_LL1F4:;_LL1F5: goto _LL1EB;_LL1EB:;}
goto _LL1DA;_LL1E9:;_LL1EA: goto _LL1DA;_LL1DA:;}if(_tmp340 != new_relns)_tmp33A=Cyc_CfFlowInfo_ReachableFL(
_tmp33F,new_relns);{void*_tmp377=_tmp33B;_LL1F7: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp378=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp377;if(_tmp378->tag != 0)goto
_LL1F9;}_LL1F8: {struct _tuple13 _tmp82E;return(_tmp82E.f1=_tmp33A,((_tmp82E.f2=
_tmp137->unknown_all,_tmp82E)));}_LL1F9: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp379=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp377;if(_tmp379->tag != 2)
goto _LL1FB;}_LL1FA: goto _LL1FC;_LL1FB: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp37A=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp377;if(_tmp37A->tag != 1)
goto _LL1FD;}_LL1FC: goto _LL1FE;_LL1FD: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp37B=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp377;if(_tmp37B->tag != 5)goto
_LL1FF;}_LL1FE: {struct Cyc_List_List _tmp82F;struct Cyc_List_List _tmp37D=(_tmp82F.hd=
_tmp1D4,((_tmp82F.tl=0,_tmp82F)));_tmp33A=Cyc_NewControlFlow_add_vars(_tmp137,
_tmp33A,(struct Cyc_List_List*)& _tmp37D,_tmp137->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp37F;void*_tmp380;struct _tuple13 _tmp37E=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp33A,_tmp1D6);_tmp37F=_tmp37E.f1;_tmp380=_tmp37E.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp381=_tmp37F;int _tmp382;struct _tuple12 _tmp383;struct Cyc_Dict_Dict _tmp384;
_LL202: if((_tmp381.BottomFL).tag != 1)goto _LL204;_tmp382=(int)(_tmp381.BottomFL).val;
_LL203: {struct _tuple13 _tmp830;return(_tmp830.f1=_tmp37F,((_tmp830.f2=_tmp137->unknown_all,
_tmp830)));}_LL204: if((_tmp381.ReachableFL).tag != 2)goto _LL201;_tmp383=(struct
_tuple12)(_tmp381.ReachableFL).val;_tmp384=_tmp383.f1;_LL205: if(Cyc_CfFlowInfo_initlevel(
_tmp137,_tmp384,_tmp380)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp833;void*_tmp832;(
_tmp832=0,Cyc_Tcutil_terr(_tmp1D5->loc,((_tmp833="expression may not be initialized",
_tag_dyneither(_tmp833,sizeof(char),34))),_tag_dyneither(_tmp832,sizeof(void*),0)));}{
struct _tuple13 _tmp834;return(_tmp834.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp834.f2=
_tmp137->unknown_all,_tmp834)));};}_LL201:;}_tmp33A=_tmp37F;goto _LL200;};}_LL1FF:;
_LL200: while(1){struct Cyc_List_List _tmp835;struct Cyc_List_List _tmp38A=(_tmp835.hd=
_tmp1D4,((_tmp835.tl=0,_tmp835)));_tmp33A=Cyc_NewControlFlow_add_vars(_tmp137,
_tmp33A,(struct Cyc_List_List*)& _tmp38A,_tmp137->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp38C;void*_tmp38D;struct _tuple13 _tmp38B=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp33A,_tmp1D6);_tmp38C=_tmp38B.f1;_tmp38D=_tmp38B.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp38E=_tmp38C;int _tmp38F;struct _tuple12 _tmp390;struct Cyc_Dict_Dict _tmp391;
_LL207: if((_tmp38E.BottomFL).tag != 1)goto _LL209;_tmp38F=(int)(_tmp38E.BottomFL).val;
_LL208: goto _LL206;_LL209: if((_tmp38E.ReachableFL).tag != 2)goto _LL206;_tmp390=(
struct _tuple12)(_tmp38E.ReachableFL).val;_tmp391=_tmp390.f1;_LL20A: if(Cyc_CfFlowInfo_initlevel(
_tmp137,_tmp391,_tmp38D)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp838;void*_tmp837;(
_tmp837=0,Cyc_Tcutil_terr(_tmp1D5->loc,((_tmp838="expression may not be initialized",
_tag_dyneither(_tmp838,sizeof(char),34))),_tag_dyneither(_tmp837,sizeof(void*),0)));}{
struct _tuple13 _tmp839;return(_tmp839.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp839.f2=
_tmp137->unknown_all,_tmp839)));};}_LL206:;}{union Cyc_CfFlowInfo_FlowInfo _tmp395=
Cyc_CfFlowInfo_join_flow(_tmp137,env->all_changed,_tmp33A,_tmp38C);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp395,_tmp33A))break;_tmp33A=_tmp395;};};}{struct _tuple13 _tmp83A;return(
_tmp83A.f1=_tmp33A,((_tmp83A.f2=_tmp137->unknown_all,_tmp83A)));};_LL1F6:;};};
_LL1D5:;};}_LL146: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1D8=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp156;if(_tmp1D8->tag != 37)goto _LL148;else{_tmp1D9=_tmp1D8->f1;}}_LL147: while(
1){union Cyc_CfFlowInfo_FlowInfo*_tmp399;struct _tuple16 _tmp398=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp1D9);_tmp399=_tmp398.f2;inflow=*_tmp399;{void*_tmp39A=_tmp1D9->r;
struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39D;struct Cyc_Absyn_Decl*
_tmp39F;struct Cyc_Absyn_Stmt*_tmp3A0;struct Cyc_Absyn_Exp*_tmp3A2;_LL20C: {struct
Cyc_Absyn_Seq_s_struct*_tmp39B=(struct Cyc_Absyn_Seq_s_struct*)_tmp39A;if(_tmp39B->tag
!= 2)goto _LL20E;else{_tmp39C=_tmp39B->f1;_tmp39D=_tmp39B->f2;}}_LL20D: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp39C);_tmp1D9=_tmp39D;goto _LL20B;_LL20E: {struct Cyc_Absyn_Decl_s_struct*
_tmp39E=(struct Cyc_Absyn_Decl_s_struct*)_tmp39A;if(_tmp39E->tag != 12)goto _LL210;
else{_tmp39F=_tmp39E->f1;_tmp3A0=_tmp39E->f2;}}_LL20F: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp39F);_tmp1D9=_tmp3A0;goto _LL20B;_LL210: {struct Cyc_Absyn_Exp_s_struct*
_tmp3A1=(struct Cyc_Absyn_Exp_s_struct*)_tmp39A;if(_tmp3A1->tag != 1)goto _LL212;
else{_tmp3A2=_tmp3A1->f1;}}_LL211: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp3A2);_LL212:;_LL213: {struct Cyc_Core_Impossible_struct
_tmp840;const char*_tmp83F;struct Cyc_Core_Impossible_struct*_tmp83E;(int)_throw((
void*)((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp840.tag=Cyc_Core_Impossible,((
_tmp840.f1=((_tmp83F="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp83F,
sizeof(char),33))),_tmp840)))),_tmp83E)))));}_LL20B:;};}_LL148: {struct Cyc_Absyn_Var_e_struct*
_tmp1DA=(struct Cyc_Absyn_Var_e_struct*)_tmp156;if(_tmp1DA->tag != 1)goto _LL14A;
else{_tmp1DB=(void*)_tmp1DA->f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp1DC=(
struct Cyc_Absyn_Unresolved_b_struct*)_tmp1DB;if(_tmp1DC->tag != 0)goto _LL14A;};}}
_LL149: goto _LL14B;_LL14A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp1DD=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp156;if(_tmp1DD->tag != 2)goto _LL14C;}_LL14B: goto _LL14D;_LL14C: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp1DE=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp156;if(_tmp1DE->tag != 10)goto
_LL14E;}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1DF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp156;if(_tmp1DF->tag != 36)
goto _LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1E0=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp156;if(_tmp1E0->tag != 26)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_Valueof_e_struct*_tmp1E1=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp156;if(_tmp1E1->tag != 39)goto _LLEB;}_LL153: {
struct Cyc_Core_Impossible_struct _tmp846;const char*_tmp845;struct Cyc_Core_Impossible_struct*
_tmp844;(int)_throw((void*)((_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844[0]=((
_tmp846.tag=Cyc_Core_Impossible,((_tmp846.f1=((_tmp845="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp845,sizeof(char),31))),_tmp846)))),_tmp844)))));}_LLEB:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3A9=env->fenv;void*_tmp3AA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3AC;void*_tmp3AD;struct Cyc_Absyn_PtrAtts _tmp3AE;union Cyc_Absyn_Constraint*
_tmp3AF;union Cyc_Absyn_Constraint*_tmp3B0;_LL215: {struct Cyc_Absyn_PointerType_struct*
_tmp3AB=(struct Cyc_Absyn_PointerType_struct*)_tmp3AA;if(_tmp3AB->tag != 5)goto
_LL217;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AC.elt_typ;_tmp3AE=_tmp3AC.ptr_atts;
_tmp3AF=_tmp3AE.bounds;_tmp3B0=_tmp3AE.zero_term;}}_LL216: {union Cyc_CfFlowInfo_FlowInfo
_tmp3B1=f;int _tmp3B2;struct _tuple12 _tmp3B3;struct Cyc_Dict_Dict _tmp3B4;struct Cyc_List_List*
_tmp3B5;_LL21A: if((_tmp3B1.BottomFL).tag != 1)goto _LL21C;_tmp3B2=(int)(_tmp3B1.BottomFL).val;
_LL21B: {struct _tuple14 _tmp847;return(_tmp847.f1=f,((_tmp847.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp847)));}_LL21C: if((_tmp3B1.ReachableFL).tag != 2)goto _LL219;_tmp3B3=(struct
_tuple12)(_tmp3B1.ReachableFL).val;_tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;_LL21D:
if(Cyc_Tcutil_is_bound_one(_tmp3AF)){void*_tmp3B7=r;struct Cyc_CfFlowInfo_Place*
_tmp3BB;struct Cyc_CfFlowInfo_Place _tmp3BC;void*_tmp3BD;struct Cyc_List_List*
_tmp3BE;enum Cyc_CfFlowInfo_InitLevel _tmp3C1;_LL21F: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3B8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3B7;if(_tmp3B8->tag != 1)
goto _LL221;}_LL220: goto _LL222;_LL221: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3B9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3B7;if(_tmp3B9->tag != 2)
goto _LL223;}_LL222:{struct Cyc_CfFlowInfo_NotZero_struct _tmp84A;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp849;e->annot=(void*)((_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849[0]=((
_tmp84A.tag=Cyc_CfFlowInfo_NotZero,((_tmp84A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3B5),_tmp84A)))),_tmp849))));}goto _LL21E;_LL223: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3BA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3B7;if(_tmp3BA->tag != 5)goto
_LL225;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=*_tmp3BB;_tmp3BD=_tmp3BC.root;_tmp3BE=
_tmp3BC.fields;}}_LL224:{struct Cyc_CfFlowInfo_NotZero_struct _tmp84D;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp84C;e->annot=(void*)((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((
_tmp84D.tag=Cyc_CfFlowInfo_NotZero,((_tmp84D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3B5),_tmp84D)))),_tmp84C))));}{struct Cyc_CfFlowInfo_Place*_tmp850;struct
_tuple14 _tmp84F;return(_tmp84F.f1=f,((_tmp84F.f2=Cyc_CfFlowInfo_PlaceL(((_tmp850=
_region_malloc(_tmp3A9->r,sizeof(*_tmp850)),((_tmp850->root=_tmp3BD,((_tmp850->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3A9->r,_tmp3BE,flds),_tmp850))))))),_tmp84F)));};_LL225: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3BF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3B7;if(_tmp3BF->tag != 0)goto _LL227;}_LL226: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp851;return(_tmp851.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp851.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp851)));};_LL227: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3C0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3B7;if(_tmp3C0->tag != 3)goto
_LL229;else{_tmp3C1=_tmp3C0->f1;}}_LL228: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3C1);goto _LL22A;_LL229:;_LL22A: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp854;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp853;e->annot=(void*)((_tmp853=
_cycalloc(sizeof(*_tmp853)),((_tmp853[0]=((_tmp854.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp854.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3B5),_tmp854)))),
_tmp853))));}_LL21E:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp857;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp856;e->annot=(void*)((_tmp856=_cycalloc(sizeof(*_tmp856)),((_tmp856[0]=((
_tmp857.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp857.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3B5),_tmp857)))),_tmp856))));}if(Cyc_CfFlowInfo_initlevel(_tmp3A9,_tmp3B4,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_terr(
e->loc,((_tmp85A="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp85A,sizeof(char),46))),_tag_dyneither(_tmp859,sizeof(void*),0)));}{struct
_tuple14 _tmp85B;return(_tmp85B.f1=f,((_tmp85B.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp85B)));};_LL219:;}_LL217:;_LL218: {struct Cyc_Core_Impossible_struct _tmp861;
const char*_tmp860;struct Cyc_Core_Impossible_struct*_tmp85F;(int)_throw((void*)((
_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp861.tag=Cyc_Core_Impossible,((
_tmp861.f1=((_tmp860="left deref of non-pointer-type",_tag_dyneither(_tmp860,
sizeof(char),31))),_tmp861)))),_tmp85F)))));}_LL214:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp3D3=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp3D4=inflow;int _tmp3D5;struct _tuple12
_tmp3D6;struct Cyc_Dict_Dict _tmp3D7;struct Cyc_List_List*_tmp3D8;_LL22C: if((
_tmp3D4.BottomFL).tag != 1)goto _LL22E;_tmp3D5=(int)(_tmp3D4.BottomFL).val;_LL22D: {
struct _tuple14 _tmp862;return(_tmp862.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp862.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp862)));}_LL22E: if((_tmp3D4.ReachableFL).tag != 2)
goto _LL22B;_tmp3D6=(struct _tuple12)(_tmp3D4.ReachableFL).val;_tmp3D7=_tmp3D6.f1;
_tmp3D8=_tmp3D6.f2;_LL22F: d=_tmp3D7;_LL22B:;}{void*_tmp3DA=e->r;struct Cyc_Absyn_Exp*
_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Exp*_tmp3E0;void*_tmp3E2;
struct Cyc_Absyn_Vardecl*_tmp3E4;void*_tmp3E6;struct Cyc_Absyn_Vardecl*_tmp3E8;
void*_tmp3EA;struct Cyc_Absyn_Vardecl*_tmp3EC;void*_tmp3EE;struct Cyc_Absyn_Vardecl*
_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F2;struct _dyneither_ptr*_tmp3F3;struct Cyc_Absyn_Exp*
_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*
_tmp3FA;struct _dyneither_ptr*_tmp3FB;_LL231: {struct Cyc_Absyn_Cast_e_struct*
_tmp3DB=(struct Cyc_Absyn_Cast_e_struct*)_tmp3DA;if(_tmp3DB->tag != 15)goto _LL233;
else{_tmp3DC=_tmp3DB->f2;}}_LL232: _tmp3DE=_tmp3DC;goto _LL234;_LL233: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp3DD=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp3DA;if(_tmp3DD->tag != 13)
goto _LL235;else{_tmp3DE=_tmp3DD->f1;}}_LL234: _tmp3E0=_tmp3DE;goto _LL236;_LL235: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp3DF=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp3DA;if(_tmp3DF->tag != 14)goto _LL237;else{_tmp3E0=_tmp3DF->f1;}}_LL236: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E0,flds);_LL237: {
struct Cyc_Absyn_Var_e_struct*_tmp3E1=(struct Cyc_Absyn_Var_e_struct*)_tmp3DA;if(
_tmp3E1->tag != 1)goto _LL239;else{_tmp3E2=(void*)_tmp3E1->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp3E3=(struct Cyc_Absyn_Param_b_struct*)_tmp3E2;if(_tmp3E3->tag != 3)goto _LL239;
else{_tmp3E4=_tmp3E3->f1;}};}}_LL238: _tmp3E8=_tmp3E4;goto _LL23A;_LL239: {struct
Cyc_Absyn_Var_e_struct*_tmp3E5=(struct Cyc_Absyn_Var_e_struct*)_tmp3DA;if(_tmp3E5->tag
!= 1)goto _LL23B;else{_tmp3E6=(void*)_tmp3E5->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp3E7=(struct Cyc_Absyn_Local_b_struct*)_tmp3E6;if(_tmp3E7->tag != 4)goto _LL23B;
else{_tmp3E8=_tmp3E7->f1;}};}}_LL23A: _tmp3EC=_tmp3E8;goto _LL23C;_LL23B: {struct
Cyc_Absyn_Var_e_struct*_tmp3E9=(struct Cyc_Absyn_Var_e_struct*)_tmp3DA;if(_tmp3E9->tag
!= 1)goto _LL23D;else{_tmp3EA=(void*)_tmp3E9->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp3EB=(struct Cyc_Absyn_Pat_b_struct*)_tmp3EA;if(_tmp3EB->tag != 5)goto _LL23D;
else{_tmp3EC=_tmp3EB->f1;}};}}_LL23C: {struct Cyc_CfFlowInfo_Place*_tmp86C;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp86B;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp86A;
struct _tuple14 _tmp869;return(_tmp869.f1=inflow,((_tmp869.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp86C=_region_malloc(env->r,sizeof(*_tmp86C)),((_tmp86C->root=(void*)((_tmp86A=
_region_malloc(env->r,sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86B.tag=0,((_tmp86B.f1=
_tmp3EC,_tmp86B)))),_tmp86A)))),((_tmp86C->fields=flds,_tmp86C))))))),_tmp869)));}
_LL23D: {struct Cyc_Absyn_Var_e_struct*_tmp3ED=(struct Cyc_Absyn_Var_e_struct*)
_tmp3DA;if(_tmp3ED->tag != 1)goto _LL23F;else{_tmp3EE=(void*)_tmp3ED->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp3EF=(struct Cyc_Absyn_Global_b_struct*)_tmp3EE;if(
_tmp3EF->tag != 1)goto _LL23F;else{_tmp3F0=_tmp3EF->f1;}};}}_LL23E: {struct
_tuple14 _tmp86D;return(_tmp86D.f1=inflow,((_tmp86D.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp86D)));}_LL23F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp3F1=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3DA;if(_tmp3F1->tag != 23)goto _LL241;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;}}
_LL240:{void*_tmp401=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F2->topt))->v);struct Cyc_Absyn_PtrInfo _tmp403;void*_tmp404;
_LL24A: {struct Cyc_Absyn_PointerType_struct*_tmp402=(struct Cyc_Absyn_PointerType_struct*)
_tmp401;if(_tmp402->tag != 5)goto _LL24C;else{_tmp403=_tmp402->f1;_tmp404=_tmp403.elt_typ;}}
_LL24B: if(!Cyc_Absyn_is_nontagged_union_type(_tmp404)){struct Cyc_List_List*
_tmp86E;flds=((_tmp86E=_region_malloc(env->r,sizeof(*_tmp86E)),((_tmp86E->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp404,_tmp3F3),((_tmp86E->tl=flds,_tmp86E))))));}
goto _LL249;_LL24C:;_LL24D: {struct Cyc_Core_Impossible_struct _tmp874;const char*
_tmp873;struct Cyc_Core_Impossible_struct*_tmp872;(int)_throw((void*)((_tmp872=
_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp874.tag=Cyc_Core_Impossible,((
_tmp874.f1=((_tmp873="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp873,sizeof(
char),25))),_tmp874)))),_tmp872)))));}_LL249:;}_tmp3F5=_tmp3F2;goto _LL242;_LL241: {
struct Cyc_Absyn_Deref_e_struct*_tmp3F4=(struct Cyc_Absyn_Deref_e_struct*)_tmp3DA;
if(_tmp3F4->tag != 21)goto _LL243;else{_tmp3F5=_tmp3F4->f1;}}_LL242: if(Cyc_Tcutil_is_noalias_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp40A;union Cyc_CfFlowInfo_AbsLVal _tmp40B;struct _tuple14 _tmp409=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,_tmp3F5);_tmp40A=_tmp409.f1;_tmp40B=_tmp409.f2;{struct _tuple14
_tmp875;struct _tuple14 _tmp40D=(_tmp875.f1=_tmp40A,((_tmp875.f2=_tmp40B,_tmp875)));
union Cyc_CfFlowInfo_FlowInfo _tmp40E;struct _tuple12 _tmp40F;struct Cyc_Dict_Dict
_tmp410;struct Cyc_List_List*_tmp411;union Cyc_CfFlowInfo_AbsLVal _tmp412;struct Cyc_CfFlowInfo_Place*
_tmp413;_LL24F: _tmp40E=_tmp40D.f1;if((_tmp40E.ReachableFL).tag != 2)goto _LL251;
_tmp40F=(struct _tuple12)(_tmp40E.ReachableFL).val;_tmp410=_tmp40F.f1;_tmp411=
_tmp40F.f2;_tmp412=_tmp40D.f2;if((_tmp412.PlaceL).tag != 1)goto _LL251;_tmp413=(
struct Cyc_CfFlowInfo_Place*)(_tmp412.PlaceL).val;_LL250: {void*_tmp414=Cyc_CfFlowInfo_lookup_place(
_tmp410,_tmp413);void*_tmp415=_tmp414;_LL254: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp416=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp415;if(_tmp416->tag != 5)goto
_LL256;}_LL255: return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F5,_tmp40A,
_tmp414,flds);_LL256:;_LL257: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp3D3,_tmp410,_tmp414);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3D3->unknown_all:
_tmp3D3->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3D3,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5->topt))->v),leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp878;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp877;void*new_root=(void*)((
_tmp877=_region_malloc(_tmp3D3->r,sizeof(*_tmp877)),((_tmp877[0]=((_tmp878.tag=1,((
_tmp878.f1=e,((_tmp878.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp878)))))),_tmp877))));struct Cyc_CfFlowInfo_Place*_tmp879;struct Cyc_CfFlowInfo_Place*
place=(_tmp879=_region_malloc(_tmp3D3->r,sizeof(*_tmp879)),((_tmp879->root=
new_root,((_tmp879->fields=0,_tmp879)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp87C;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp87B;void*res=(void*)((_tmp87B=
_region_malloc(_tmp3D3->r,sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp87C.tag=5,((
_tmp87C.f1=place,_tmp87C)))),_tmp87B))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp410=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp410,new_root,new_rval);_tmp410=Cyc_CfFlowInfo_assign_place(
_tmp3D3,e->loc,_tmp410,env->all_changed,_tmp413,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp417=Cyc_CfFlowInfo_ReachableFL(_tmp410,_tmp411);return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3F5,_tmp417,res,flds);};}_LL253:;}_LL251:;_LL252: goto _LL24E;
_LL24E:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp41E;void*_tmp41F;struct _tuple13
_tmp41D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3F5);_tmp41E=_tmp41D.f1;
_tmp41F=_tmp41D.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F5,
_tmp41E,_tmp41F,flds);};_LL243: {struct Cyc_Absyn_Subscript_e_struct*_tmp3F6=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp3DA;if(_tmp3F6->tag != 24)goto _LL245;
else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;}}_LL244: {void*_tmp420=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F7->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp423;struct Cyc_Absyn_PtrAtts _tmp424;union Cyc_Absyn_Constraint*_tmp425;_LL259: {
struct Cyc_Absyn_TupleType_struct*_tmp421=(struct Cyc_Absyn_TupleType_struct*)
_tmp420;if(_tmp421->tag != 11)goto _LL25B;}_LL25A: {unsigned int _tmp426=(Cyc_Evexp_eval_const_uint_exp(
_tmp3F8)).f1;struct Cyc_List_List*_tmp87D;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp3F7,((_tmp87D=_region_malloc(env->r,sizeof(*_tmp87D)),((
_tmp87D->hd=(void*)((int)_tmp426),((_tmp87D->tl=flds,_tmp87D)))))));}_LL25B: {
struct Cyc_Absyn_PointerType_struct*_tmp422=(struct Cyc_Absyn_PointerType_struct*)
_tmp420;if(_tmp422->tag != 5)goto _LL25D;else{_tmp423=_tmp422->f1;_tmp424=_tmp423.ptr_atts;
_tmp425=_tmp424.bounds;}}_LL25C: {struct _RegionHandle*_tmp428=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp42B;struct Cyc_List_List*_tmp42C;struct Cyc_Absyn_Exp*_tmp87E[2];struct
_tuple19 _tmp42A=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp428,env,inflow,((
_tmp87E[1]=_tmp3F8,((_tmp87E[0]=_tmp3F7,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp428,_tag_dyneither(
_tmp87E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp42B=_tmp42A.f1;_tmp42C=
_tmp42A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp42D=_tmp42B;{union Cyc_CfFlowInfo_FlowInfo
_tmp42E=_tmp42B;struct _tuple12 _tmp42F;struct Cyc_Dict_Dict _tmp430;struct Cyc_List_List*
_tmp431;_LL260: if((_tmp42E.ReachableFL).tag != 2)goto _LL262;_tmp42F=(struct
_tuple12)(_tmp42E.ReachableFL).val;_tmp430=_tmp42F.f1;_tmp431=_tmp42F.f2;_LL261:
if(Cyc_CfFlowInfo_initlevel(_tmp3D3,_tmp430,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp42C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp881;void*_tmp880;(_tmp880=0,Cyc_Tcutil_terr(_tmp3F8->loc,((_tmp881="expression may not be initialized",
_tag_dyneither(_tmp881,sizeof(char),34))),_tag_dyneither(_tmp880,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp434=Cyc_NewControlFlow_add_subscript_reln(_tmp3D3->r,
_tmp431,_tmp3F7,_tmp3F8);if(_tmp431 != _tmp434)_tmp42D=Cyc_CfFlowInfo_ReachableFL(
_tmp430,_tmp434);goto _LL25F;};_LL262:;_LL263: goto _LL25F;_LL25F:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp436;union Cyc_CfFlowInfo_AbsLVal _tmp437;struct _tuple14 _tmp435=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3F7,_tmp42B,(void*)((struct Cyc_List_List*)_check_null(_tmp42C))->hd,
flds);_tmp436=_tmp435.f1;_tmp437=_tmp435.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp438=
_tmp436;int _tmp439;_LL265: if((_tmp438.BottomFL).tag != 1)goto _LL267;_tmp439=(int)(
_tmp438.BottomFL).val;_LL266: {struct _tuple14 _tmp882;return(_tmp882.f1=_tmp436,((
_tmp882.f2=_tmp437,_tmp882)));}_LL267:;_LL268: {struct _tuple14 _tmp883;return(
_tmp883.f1=_tmp42D,((_tmp883.f2=_tmp437,_tmp883)));}_LL264:;};};};}_LL25D:;
_LL25E: {struct Cyc_Core_Impossible_struct _tmp889;const char*_tmp888;struct Cyc_Core_Impossible_struct*
_tmp887;(int)_throw((void*)((_tmp887=_cycalloc(sizeof(*_tmp887)),((_tmp887[0]=((
_tmp889.tag=Cyc_Core_Impossible,((_tmp889.f1=((_tmp888="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp888,sizeof(char),33))),_tmp889)))),_tmp887)))));}_LL258:;}
_LL245: {struct Cyc_Absyn_AggrMember_e_struct*_tmp3F9=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3DA;if(_tmp3F9->tag != 22)goto _LL247;else{_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3F9->f2;}}
_LL246: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FA->topt))->v)){struct _tuple14 _tmp88A;return(_tmp88A.f1=inflow,((
_tmp88A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp88A)));}{struct Cyc_List_List*_tmp88B;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3FA,((
_tmp88B=_region_malloc(env->r,sizeof(*_tmp88B)),((_tmp88B->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3FA->topt))->v,_tmp3FB),((_tmp88B->tl=
flds,_tmp88B)))))));};_LL247:;_LL248: {struct _tuple14 _tmp88C;return(_tmp88C.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp88C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp88C)));}
_LL230:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp443;union Cyc_CfFlowInfo_AbsLVal _tmp444;struct
_tuple14 _tmp442=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp443=_tmp442.f1;_tmp444=_tmp442.f2;{struct _tuple14 _tmp88D;return(_tmp88D.f1=
_tmp443,((_tmp88D.f2=_tmp444,_tmp88D)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp446=env->fenv;{void*_tmp447=e->r;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Exp*
_tmp44A;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*
_tmp44E;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*
_tmp453;struct Cyc_Absyn_Exp*_tmp454;enum Cyc_Absyn_Primop _tmp456;struct Cyc_List_List*
_tmp457;struct Cyc_List_List _tmp458;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_List_List*
_tmp45A;enum Cyc_Absyn_Primop _tmp45C;struct Cyc_List_List*_tmp45D;_LL26A: {struct
Cyc_Absyn_Conditional_e_struct*_tmp448=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp447;if(_tmp448->tag != 6)goto _LL26C;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;
_tmp44B=_tmp448->f3;}}_LL26B: {union Cyc_CfFlowInfo_FlowInfo _tmp45F;union Cyc_CfFlowInfo_FlowInfo
_tmp460;struct _tuple15 _tmp45E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp449);
_tmp45F=_tmp45E.f1;_tmp460=_tmp45E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp462;union
Cyc_CfFlowInfo_FlowInfo _tmp463;struct _tuple15 _tmp461=Cyc_NewControlFlow_anal_test(
env,_tmp45F,_tmp44A);_tmp462=_tmp461.f1;_tmp463=_tmp461.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp465;union Cyc_CfFlowInfo_FlowInfo _tmp466;struct _tuple15 _tmp464=Cyc_NewControlFlow_anal_test(
env,_tmp460,_tmp44B);_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;{struct _tuple15
_tmp88E;return(_tmp88E.f1=Cyc_CfFlowInfo_join_flow(_tmp446,env->all_changed,
_tmp462,_tmp465),((_tmp88E.f2=Cyc_CfFlowInfo_join_flow(_tmp446,env->all_changed,
_tmp463,_tmp466),_tmp88E)));};};};}_LL26C: {struct Cyc_Absyn_And_e_struct*_tmp44C=(
struct Cyc_Absyn_And_e_struct*)_tmp447;if(_tmp44C->tag != 7)goto _LL26E;else{
_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}_LL26D: {union Cyc_CfFlowInfo_FlowInfo
_tmp469;union Cyc_CfFlowInfo_FlowInfo _tmp46A;struct _tuple15 _tmp468=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp44D);_tmp469=_tmp468.f1;_tmp46A=_tmp468.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp46C;union Cyc_CfFlowInfo_FlowInfo _tmp46D;struct _tuple15 _tmp46B=Cyc_NewControlFlow_anal_test(
env,_tmp469,_tmp44E);_tmp46C=_tmp46B.f1;_tmp46D=_tmp46B.f2;{struct _tuple15
_tmp88F;return(_tmp88F.f1=_tmp46C,((_tmp88F.f2=Cyc_CfFlowInfo_join_flow(_tmp446,
env->all_changed,_tmp46A,_tmp46D),_tmp88F)));};};}_LL26E: {struct Cyc_Absyn_Or_e_struct*
_tmp44F=(struct Cyc_Absyn_Or_e_struct*)_tmp447;if(_tmp44F->tag != 8)goto _LL270;
else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}_LL26F: {union Cyc_CfFlowInfo_FlowInfo
_tmp470;union Cyc_CfFlowInfo_FlowInfo _tmp471;struct _tuple15 _tmp46F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp450);_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp473;union Cyc_CfFlowInfo_FlowInfo _tmp474;struct _tuple15 _tmp472=Cyc_NewControlFlow_anal_test(
env,_tmp471,_tmp451);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;{struct _tuple15
_tmp890;return(_tmp890.f1=Cyc_CfFlowInfo_join_flow(_tmp446,env->all_changed,
_tmp470,_tmp473),((_tmp890.f2=_tmp474,_tmp890)));};};}_LL270: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp452=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp447;if(_tmp452->tag != 9)goto _LL272;
else{_tmp453=_tmp452->f1;_tmp454=_tmp452->f2;}}_LL271: {union Cyc_CfFlowInfo_FlowInfo
_tmp477;void*_tmp478;struct _tuple13 _tmp476=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp453);_tmp477=_tmp476.f1;_tmp478=_tmp476.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp477,_tmp454);}_LL272: {struct Cyc_Absyn_Primop_e_struct*_tmp455=(struct
Cyc_Absyn_Primop_e_struct*)_tmp447;if(_tmp455->tag != 3)goto _LL274;else{_tmp456=
_tmp455->f1;if(_tmp456 != Cyc_Absyn_Not)goto _LL274;_tmp457=_tmp455->f2;if(_tmp457
== 0)goto _LL274;_tmp458=*_tmp457;_tmp459=(struct Cyc_Absyn_Exp*)_tmp458.hd;
_tmp45A=_tmp458.tl;if(_tmp45A != 0)goto _LL274;}}_LL273: {union Cyc_CfFlowInfo_FlowInfo
_tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp47B;struct _tuple15 _tmp479=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp459);_tmp47A=_tmp479.f1;_tmp47B=_tmp479.f2;{struct _tuple15 _tmp891;
return(_tmp891.f1=_tmp47B,((_tmp891.f2=_tmp47A,_tmp891)));};}_LL274: {struct Cyc_Absyn_Primop_e_struct*
_tmp45B=(struct Cyc_Absyn_Primop_e_struct*)_tmp447;if(_tmp45B->tag != 3)goto _LL276;
else{_tmp45C=_tmp45B->f1;_tmp45D=_tmp45B->f2;}}_LL275: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp47D=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp47F;struct
Cyc_List_List*_tmp480;struct _tuple19 _tmp47E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp47D,env,inflow,_tmp45D,0,0);_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp480))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp480->tl))->hd;f=_tmp47F;}{union Cyc_CfFlowInfo_FlowInfo _tmp481=f;
int _tmp482;struct _tuple12 _tmp483;struct Cyc_Dict_Dict _tmp484;struct Cyc_List_List*
_tmp485;_LL279: if((_tmp481.BottomFL).tag != 1)goto _LL27B;_tmp482=(int)(_tmp481.BottomFL).val;
_LL27A: {struct _tuple15 _tmp892;return(_tmp892.f1=f,((_tmp892.f2=f,_tmp892)));}
_LL27B: if((_tmp481.ReachableFL).tag != 2)goto _LL278;_tmp483=(struct _tuple12)(
_tmp481.ReachableFL).val;_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;_LL27C: {struct
Cyc_Absyn_Exp*_tmp487=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp45D))->hd;struct Cyc_Absyn_Exp*_tmp488=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp45D->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp484,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp895;void*_tmp894;(_tmp894=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp45D->hd)->loc,((_tmp895="expression may not be initialized",
_tag_dyneither(_tmp895,sizeof(char),34))),_tag_dyneither(_tmp894,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp484,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp898;void*_tmp897;(_tmp897=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp45D->tl))->hd)->loc,((_tmp898="expression may not be initialized",
_tag_dyneither(_tmp898,sizeof(char),34))),_tag_dyneither(_tmp897,sizeof(void*),0)));}
if(_tmp45C == Cyc_Absyn_Eq  || _tmp45C == Cyc_Absyn_Neq){struct _tuple0 _tmp899;
struct _tuple0 _tmp48E=(_tmp899.f1=r1,((_tmp899.f2=r2,_tmp899)));void*_tmp48F;
enum Cyc_CfFlowInfo_InitLevel _tmp491;void*_tmp492;void*_tmp494;void*_tmp496;
enum Cyc_CfFlowInfo_InitLevel _tmp498;void*_tmp499;void*_tmp49B;void*_tmp49D;void*
_tmp49F;void*_tmp4A1;void*_tmp4A3;void*_tmp4A5;void*_tmp4A7;void*_tmp4A9;void*
_tmp4AB;void*_tmp4AD;void*_tmp4AF;void*_tmp4B1;void*_tmp4B3;_LL27E: _tmp48F=
_tmp48E.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp490=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp48F;if(_tmp490->tag != 3)goto _LL280;else{_tmp491=_tmp490->f1;}};_tmp492=
_tmp48E.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp493=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp492;if(_tmp493->tag != 0)goto _LL280;};_LL27F: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B6;union Cyc_CfFlowInfo_FlowInfo _tmp4B7;struct _tuple15 _tmp4B5=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp487,_tmp491);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;switch(_tmp45C){
case Cyc_Absyn_Eq: _LL292: {struct _tuple15 _tmp89A;return(_tmp89A.f1=_tmp4B7,((
_tmp89A.f2=_tmp4B6,_tmp89A)));}case Cyc_Absyn_Neq: _LL293: {struct _tuple15 _tmp89B;
return(_tmp89B.f1=_tmp4B6,((_tmp89B.f2=_tmp4B7,_tmp89B)));}default: _LL294: {
struct Cyc_Core_Impossible_struct _tmp8A1;const char*_tmp8A0;struct Cyc_Core_Impossible_struct*
_tmp89F;(int)_throw((void*)((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F[0]=((
_tmp8A1.tag=Cyc_Core_Impossible,((_tmp8A1.f1=((_tmp8A0="anal_test, zero-split",
_tag_dyneither(_tmp8A0,sizeof(char),22))),_tmp8A1)))),_tmp89F)))));}}}_LL280:
_tmp494=_tmp48E.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp495=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp494;if(_tmp495->tag != 0)goto _LL282;};_tmp496=_tmp48E.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp497=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp496;if(_tmp497->tag != 3)goto
_LL282;else{_tmp498=_tmp497->f1;}};_LL281: {union Cyc_CfFlowInfo_FlowInfo _tmp4BE;
union Cyc_CfFlowInfo_FlowInfo _tmp4BF;struct _tuple15 _tmp4BD=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp488,_tmp498);_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;switch(_tmp45C){
case Cyc_Absyn_Eq: _LL296: {struct _tuple15 _tmp8A2;return(_tmp8A2.f1=_tmp4BF,((
_tmp8A2.f2=_tmp4BE,_tmp8A2)));}case Cyc_Absyn_Neq: _LL297: {struct _tuple15 _tmp8A3;
return(_tmp8A3.f1=_tmp4BE,((_tmp8A3.f2=_tmp4BF,_tmp8A3)));}default: _LL298: {
struct Cyc_Core_Impossible_struct _tmp8A9;const char*_tmp8A8;struct Cyc_Core_Impossible_struct*
_tmp8A7;(int)_throw((void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((
_tmp8A9.tag=Cyc_Core_Impossible,((_tmp8A9.f1=((_tmp8A8="anal_test, zero-split",
_tag_dyneither(_tmp8A8,sizeof(char),22))),_tmp8A9)))),_tmp8A7)))));}}}_LL282:
_tmp499=_tmp48E.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp49A=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp499;if(_tmp49A->tag != 0)goto _LL284;};_tmp49B=_tmp48E.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp49C=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp49B;if(_tmp49C->tag != 0)goto
_LL284;};_LL283: if(_tmp45C == Cyc_Absyn_Eq){struct _tuple15 _tmp8AA;return(_tmp8AA.f1=
f,((_tmp8AA.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8AA)));}else{struct _tuple15 _tmp8AB;
return(_tmp8AB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8AB.f2=f,_tmp8AB)));}_LL284:
_tmp49D=_tmp48E.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp49E=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp49D;if(_tmp49E->tag != 0)goto _LL286;};_tmp49F=_tmp48E.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4A0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp49F;if(_tmp4A0->tag != 1)
goto _LL286;};_LL285: goto _LL287;_LL286: _tmp4A1=_tmp48E.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4A2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4A1;if(_tmp4A2->tag != 0)goto
_LL288;};_tmp4A3=_tmp48E.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4A4=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4A3;if(_tmp4A4->tag != 2)goto _LL288;};
_LL287: goto _LL289;_LL288: _tmp4A5=_tmp48E.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4A6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4A5;if(_tmp4A6->tag != 0)goto
_LL28A;};_tmp4A7=_tmp48E.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4A8=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4A7;if(_tmp4A8->tag != 5)goto _LL28A;};
_LL289: goto _LL28B;_LL28A: _tmp4A9=_tmp48E.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4AA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4A9;if(_tmp4AA->tag != 1)
goto _LL28C;};_tmp4AB=_tmp48E.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4AC=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4AB;if(_tmp4AC->tag != 0)goto _LL28C;};
_LL28B: goto _LL28D;_LL28C: _tmp4AD=_tmp48E.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4AE=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4AD;if(_tmp4AE->tag != 2)
goto _LL28E;};_tmp4AF=_tmp48E.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4B0=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4AF;if(_tmp4B0->tag != 0)goto _LL28E;};
_LL28D: goto _LL28F;_LL28E: _tmp4B1=_tmp48E.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4B2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4B1;if(_tmp4B2->tag != 5)goto
_LL290;};_tmp4B3=_tmp48E.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4B4=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4B3;if(_tmp4B4->tag != 0)goto _LL290;};_LL28F: if(_tmp45C == Cyc_Absyn_Neq){
struct _tuple15 _tmp8AC;return(_tmp8AC.f1=f,((_tmp8AC.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8AC)));}else{struct _tuple15 _tmp8AD;return(_tmp8AD.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8AD.f2=f,_tmp8AD)));}_LL290:;_LL291: goto _LL27D;_LL27D:;}{struct _tuple0
_tmp8AE;struct _tuple0 _tmp4CA=(_tmp8AE.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp487->topt))->v),((_tmp8AE.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp488->topt))->v),_tmp8AE)));void*_tmp4CB;enum Cyc_Absyn_Sign
_tmp4CD;void*_tmp4CE;enum Cyc_Absyn_Sign _tmp4D0;void*_tmp4D1;void*_tmp4D3;_LL29B:
_tmp4CB=_tmp4CA.f1;{struct Cyc_Absyn_IntType_struct*_tmp4CC=(struct Cyc_Absyn_IntType_struct*)
_tmp4CB;if(_tmp4CC->tag != 6)goto _LL29D;else{_tmp4CD=_tmp4CC->f1;if(_tmp4CD != Cyc_Absyn_Unsigned)
goto _LL29D;}};_LL29C: goto _LL29E;_LL29D: _tmp4CE=_tmp4CA.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4CF=(struct Cyc_Absyn_IntType_struct*)_tmp4CE;if(_tmp4CF->tag != 6)goto _LL29F;
else{_tmp4D0=_tmp4CF->f1;if(_tmp4D0 != Cyc_Absyn_Unsigned)goto _LL29F;}};_LL29E:
goto _LL2A0;_LL29F: _tmp4D1=_tmp4CA.f1;{struct Cyc_Absyn_TagType_struct*_tmp4D2=(
struct Cyc_Absyn_TagType_struct*)_tmp4D1;if(_tmp4D2->tag != 20)goto _LL2A1;};_LL2A0:
goto _LL2A2;_LL2A1: _tmp4D3=_tmp4CA.f2;{struct Cyc_Absyn_TagType_struct*_tmp4D4=(
struct Cyc_Absyn_TagType_struct*)_tmp4D3;if(_tmp4D4->tag != 20)goto _LL2A3;};_LL2A2:
goto _LL29A;_LL2A3:;_LL2A4: {struct _tuple15 _tmp8AF;return(_tmp8AF.f1=f,((_tmp8AF.f2=
f,_tmp8AF)));}_LL29A:;}switch(_tmp45C){case Cyc_Absyn_Eq: _LL2A5: goto _LL2A6;case
Cyc_Absyn_Neq: _LL2A6: goto _LL2A7;case Cyc_Absyn_Gt: _LL2A7: goto _LL2A8;case Cyc_Absyn_Gte:
_LL2A8: {struct _tuple15 _tmp8B0;return(_tmp8B0.f1=f,((_tmp8B0.f2=f,_tmp8B0)));}
case Cyc_Absyn_Lt: _LL2A9: {union Cyc_CfFlowInfo_FlowInfo _tmp4D7=f;union Cyc_CfFlowInfo_FlowInfo
_tmp4D8=f;{union Cyc_CfFlowInfo_FlowInfo _tmp4D9=_tmp4D7;int _tmp4DA;struct _tuple12
_tmp4DB;struct Cyc_Dict_Dict _tmp4DC;_LL2AC: if((_tmp4D9.BottomFL).tag != 1)goto
_LL2AE;_tmp4DA=(int)(_tmp4D9.BottomFL).val;_LL2AD: {struct Cyc_Core_Impossible_struct
_tmp8B6;const char*_tmp8B5;struct Cyc_Core_Impossible_struct*_tmp8B4;(int)_throw((
void*)((_tmp8B4=_cycalloc(sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B6.tag=Cyc_Core_Impossible,((
_tmp8B6.f1=((_tmp8B5="anal_test, Lt",_tag_dyneither(_tmp8B5,sizeof(char),14))),
_tmp8B6)))),_tmp8B4)))));}_LL2AE: if((_tmp4D9.ReachableFL).tag != 2)goto _LL2AB;
_tmp4DB=(struct _tuple12)(_tmp4D9.ReachableFL).val;_tmp4DC=_tmp4DB.f1;_LL2AF:
_tmp484=_tmp4DC;_LL2AB:;}{void*_tmp4E0=_tmp487->r;void*_tmp4E2;struct Cyc_Absyn_Vardecl*
_tmp4E4;void*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;void*_tmp4EA;struct Cyc_Absyn_Vardecl*
_tmp4EC;void*_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4F0;_LL2B1:{struct Cyc_Absyn_Var_e_struct*
_tmp4E1=(struct Cyc_Absyn_Var_e_struct*)_tmp4E0;if(_tmp4E1->tag != 1)goto _LL2B3;
else{_tmp4E2=(void*)_tmp4E1->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4E3=(struct
Cyc_Absyn_Global_b_struct*)_tmp4E2;if(_tmp4E3->tag != 1)goto _LL2B3;else{_tmp4E4=
_tmp4E3->f1;}};}}if(!(!_tmp4E4->escapes))goto _LL2B3;_LL2B2: _tmp4E8=_tmp4E4;goto
_LL2B4;_LL2B3:{struct Cyc_Absyn_Var_e_struct*_tmp4E5=(struct Cyc_Absyn_Var_e_struct*)
_tmp4E0;if(_tmp4E5->tag != 1)goto _LL2B5;else{_tmp4E6=(void*)_tmp4E5->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4E7=(struct Cyc_Absyn_Local_b_struct*)_tmp4E6;if(
_tmp4E7->tag != 4)goto _LL2B5;else{_tmp4E8=_tmp4E7->f1;}};}}if(!(!_tmp4E8->escapes))
goto _LL2B5;_LL2B4: _tmp4EC=_tmp4E8;goto _LL2B6;_LL2B5:{struct Cyc_Absyn_Var_e_struct*
_tmp4E9=(struct Cyc_Absyn_Var_e_struct*)_tmp4E0;if(_tmp4E9->tag != 1)goto _LL2B7;
else{_tmp4EA=(void*)_tmp4E9->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4EB=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4EA;if(_tmp4EB->tag != 5)goto _LL2B7;else{_tmp4EC=_tmp4EB->f1;}};}}if(!(!
_tmp4EC->escapes))goto _LL2B7;_LL2B6: _tmp4F0=_tmp4EC;goto _LL2B8;_LL2B7:{struct Cyc_Absyn_Var_e_struct*
_tmp4ED=(struct Cyc_Absyn_Var_e_struct*)_tmp4E0;if(_tmp4ED->tag != 1)goto _LL2B9;
else{_tmp4EE=(void*)_tmp4ED->f2;{struct Cyc_Absyn_Param_b_struct*_tmp4EF=(struct
Cyc_Absyn_Param_b_struct*)_tmp4EE;if(_tmp4EF->tag != 3)goto _LL2B9;else{_tmp4F0=
_tmp4EF->f1;}};}}if(!(!_tmp4F0->escapes))goto _LL2B9;_LL2B8: {void*_tmp4F1=
_tmp488->r;void*_tmp4F3;struct Cyc_Absyn_Vardecl*_tmp4F5;void*_tmp4F7;struct Cyc_Absyn_Vardecl*
_tmp4F9;void*_tmp4FB;struct Cyc_Absyn_Vardecl*_tmp4FD;void*_tmp4FF;struct Cyc_Absyn_Vardecl*
_tmp501;union Cyc_Absyn_Cnst _tmp503;struct _tuple6 _tmp504;int _tmp505;struct Cyc_Absyn_Exp*
_tmp507;struct Cyc_Absyn_Exp _tmp508;void*_tmp509;union Cyc_Absyn_Cnst _tmp50B;
struct _tuple6 _tmp50C;int _tmp50D;enum Cyc_Absyn_Primop _tmp50F;struct Cyc_List_List*
_tmp510;struct Cyc_List_List _tmp511;struct Cyc_Absyn_Exp*_tmp512;_LL2BC:{struct Cyc_Absyn_Var_e_struct*
_tmp4F2=(struct Cyc_Absyn_Var_e_struct*)_tmp4F1;if(_tmp4F2->tag != 1)goto _LL2BE;
else{_tmp4F3=(void*)_tmp4F2->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4F4=(struct
Cyc_Absyn_Global_b_struct*)_tmp4F3;if(_tmp4F4->tag != 1)goto _LL2BE;else{_tmp4F5=
_tmp4F4->f1;}};}}if(!(!_tmp4F5->escapes))goto _LL2BE;_LL2BD: _tmp4F9=_tmp4F5;goto
_LL2BF;_LL2BE:{struct Cyc_Absyn_Var_e_struct*_tmp4F6=(struct Cyc_Absyn_Var_e_struct*)
_tmp4F1;if(_tmp4F6->tag != 1)goto _LL2C0;else{_tmp4F7=(void*)_tmp4F6->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4F8=(struct Cyc_Absyn_Local_b_struct*)_tmp4F7;if(
_tmp4F8->tag != 4)goto _LL2C0;else{_tmp4F9=_tmp4F8->f1;}};}}if(!(!_tmp4F9->escapes))
goto _LL2C0;_LL2BF: _tmp4FD=_tmp4F9;goto _LL2C1;_LL2C0:{struct Cyc_Absyn_Var_e_struct*
_tmp4FA=(struct Cyc_Absyn_Var_e_struct*)_tmp4F1;if(_tmp4FA->tag != 1)goto _LL2C2;
else{_tmp4FB=(void*)_tmp4FA->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4FC=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4FB;if(_tmp4FC->tag != 5)goto _LL2C2;else{_tmp4FD=_tmp4FC->f1;}};}}if(!(!
_tmp4FD->escapes))goto _LL2C2;_LL2C1: _tmp501=_tmp4FD;goto _LL2C3;_LL2C2:{struct Cyc_Absyn_Var_e_struct*
_tmp4FE=(struct Cyc_Absyn_Var_e_struct*)_tmp4F1;if(_tmp4FE->tag != 1)goto _LL2C4;
else{_tmp4FF=(void*)_tmp4FE->f2;{struct Cyc_Absyn_Param_b_struct*_tmp500=(struct
Cyc_Absyn_Param_b_struct*)_tmp4FF;if(_tmp500->tag != 3)goto _LL2C4;else{_tmp501=
_tmp500->f1;}};}}if(!(!_tmp501->escapes))goto _LL2C4;_LL2C3: {struct _RegionHandle*
_tmp513=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8B9;struct Cyc_List_List*
_tmp8B8;struct Cyc_List_List*_tmp514=(_tmp8B8=_region_malloc(_tmp513,sizeof(*
_tmp8B8)),((_tmp8B8->hd=((_tmp8B9=_region_malloc(_tmp513,sizeof(*_tmp8B9)),((
_tmp8B9->vd=_tmp4F0,((_tmp8B9->rop=Cyc_CfFlowInfo_LessVar(_tmp501,_tmp501->type),
_tmp8B9)))))),((_tmp8B8->tl=_tmp485,_tmp8B8)))));struct _tuple15 _tmp8BA;return(
_tmp8BA.f1=Cyc_CfFlowInfo_ReachableFL(_tmp484,_tmp514),((_tmp8BA.f2=_tmp4D8,
_tmp8BA)));}_LL2C4: {struct Cyc_Absyn_Const_e_struct*_tmp502=(struct Cyc_Absyn_Const_e_struct*)
_tmp4F1;if(_tmp502->tag != 0)goto _LL2C6;else{_tmp503=_tmp502->f1;if((_tmp503.Int_c).tag
!= 4)goto _LL2C6;_tmp504=(struct _tuple6)(_tmp503.Int_c).val;_tmp505=_tmp504.f2;}}
_LL2C5: _tmp50D=_tmp505;goto _LL2C7;_LL2C6: {struct Cyc_Absyn_Cast_e_struct*_tmp506=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4F1;if(_tmp506->tag != 15)goto _LL2C8;else{
_tmp507=_tmp506->f2;_tmp508=*_tmp507;_tmp509=_tmp508.r;{struct Cyc_Absyn_Const_e_struct*
_tmp50A=(struct Cyc_Absyn_Const_e_struct*)_tmp509;if(_tmp50A->tag != 0)goto _LL2C8;
else{_tmp50B=_tmp50A->f1;if((_tmp50B.Int_c).tag != 4)goto _LL2C8;_tmp50C=(struct
_tuple6)(_tmp50B.Int_c).val;_tmp50D=_tmp50C.f2;}};}}_LL2C7: {struct _RegionHandle*
_tmp518=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8BD;struct Cyc_List_List*
_tmp8BC;struct Cyc_List_List*_tmp519=(_tmp8BC=_region_malloc(_tmp518,sizeof(*
_tmp8BC)),((_tmp8BC->hd=((_tmp8BD=_region_malloc(_tmp518,sizeof(*_tmp8BD)),((
_tmp8BD->vd=_tmp4F0,((_tmp8BD->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp50D),_tmp8BD)))))),((_tmp8BC->tl=_tmp485,_tmp8BC)))));struct _tuple15 _tmp8BE;
return(_tmp8BE.f1=Cyc_CfFlowInfo_ReachableFL(_tmp484,_tmp519),((_tmp8BE.f2=
_tmp4D8,_tmp8BE)));}_LL2C8: {struct Cyc_Absyn_Primop_e_struct*_tmp50E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp4F1;if(_tmp50E->tag != 3)goto _LL2CA;else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;
if(_tmp510 == 0)goto _LL2CA;_tmp511=*_tmp510;_tmp512=(struct Cyc_Absyn_Exp*)_tmp511.hd;}}
_LL2C9: {void*_tmp51D=_tmp512->r;void*_tmp51F;struct Cyc_Absyn_Vardecl*_tmp521;
void*_tmp523;struct Cyc_Absyn_Vardecl*_tmp525;void*_tmp527;struct Cyc_Absyn_Vardecl*
_tmp529;void*_tmp52B;struct Cyc_Absyn_Vardecl*_tmp52D;_LL2CD:{struct Cyc_Absyn_Var_e_struct*
_tmp51E=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp51E->tag != 1)goto _LL2CF;
else{_tmp51F=(void*)_tmp51E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp520=(struct
Cyc_Absyn_Global_b_struct*)_tmp51F;if(_tmp520->tag != 1)goto _LL2CF;else{_tmp521=
_tmp520->f1;}};}}if(!(!_tmp521->escapes))goto _LL2CF;_LL2CE: _tmp525=_tmp521;goto
_LL2D0;_LL2CF:{struct Cyc_Absyn_Var_e_struct*_tmp522=(struct Cyc_Absyn_Var_e_struct*)
_tmp51D;if(_tmp522->tag != 1)goto _LL2D1;else{_tmp523=(void*)_tmp522->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp524=(struct Cyc_Absyn_Local_b_struct*)_tmp523;if(
_tmp524->tag != 4)goto _LL2D1;else{_tmp525=_tmp524->f1;}};}}if(!(!_tmp525->escapes))
goto _LL2D1;_LL2D0: _tmp529=_tmp525;goto _LL2D2;_LL2D1:{struct Cyc_Absyn_Var_e_struct*
_tmp526=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp526->tag != 1)goto _LL2D3;
else{_tmp527=(void*)_tmp526->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp528=(struct Cyc_Absyn_Pat_b_struct*)
_tmp527;if(_tmp528->tag != 5)goto _LL2D3;else{_tmp529=_tmp528->f1;}};}}if(!(!
_tmp529->escapes))goto _LL2D3;_LL2D2: _tmp52D=_tmp529;goto _LL2D4;_LL2D3:{struct Cyc_Absyn_Var_e_struct*
_tmp52A=(struct Cyc_Absyn_Var_e_struct*)_tmp51D;if(_tmp52A->tag != 1)goto _LL2D5;
else{_tmp52B=(void*)_tmp52A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52C=(struct
Cyc_Absyn_Param_b_struct*)_tmp52B;if(_tmp52C->tag != 3)goto _LL2D5;else{_tmp52D=
_tmp52C->f1;}};}}if(!(!_tmp52D->escapes))goto _LL2D5;_LL2D4: {struct _RegionHandle*
_tmp52E=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8C1;struct Cyc_List_List*
_tmp8C0;struct Cyc_List_List*_tmp52F=(_tmp8C0=_region_malloc(_tmp52E,sizeof(*
_tmp8C0)),((_tmp8C0->hd=((_tmp8C1=_region_malloc(_tmp52E,sizeof(*_tmp8C1)),((
_tmp8C1->vd=_tmp4F0,((_tmp8C1->rop=Cyc_CfFlowInfo_LessNumelts(_tmp52D),_tmp8C1)))))),((
_tmp8C0->tl=_tmp485,_tmp8C0)))));struct _tuple15 _tmp8C2;return(_tmp8C2.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp484,_tmp52F),((_tmp8C2.f2=_tmp4D8,_tmp8C2)));}_LL2D5:;_LL2D6: {struct
_tuple15 _tmp8C3;return(_tmp8C3.f1=_tmp4D7,((_tmp8C3.f2=_tmp4D8,_tmp8C3)));}
_LL2CC:;}_LL2CA:;_LL2CB: {struct _tuple15 _tmp8C4;return(_tmp8C4.f1=_tmp4D7,((
_tmp8C4.f2=_tmp4D8,_tmp8C4)));}_LL2BB:;}_LL2B9:;_LL2BA: {struct _tuple15 _tmp8C5;
return(_tmp8C5.f1=_tmp4D7,((_tmp8C5.f2=_tmp4D8,_tmp8C5)));}_LL2B0:;};}case Cyc_Absyn_Lte:
_LL2AA: {union Cyc_CfFlowInfo_FlowInfo _tmp536=f;union Cyc_CfFlowInfo_FlowInfo
_tmp537=f;{union Cyc_CfFlowInfo_FlowInfo _tmp538=_tmp536;int _tmp539;struct _tuple12
_tmp53A;struct Cyc_Dict_Dict _tmp53B;_LL2D9: if((_tmp538.BottomFL).tag != 1)goto
_LL2DB;_tmp539=(int)(_tmp538.BottomFL).val;_LL2DA: {struct Cyc_Core_Impossible_struct
_tmp8CB;const char*_tmp8CA;struct Cyc_Core_Impossible_struct*_tmp8C9;(int)_throw((
void*)((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CB.tag=Cyc_Core_Impossible,((
_tmp8CB.f1=((_tmp8CA="anal_test, Lte",_tag_dyneither(_tmp8CA,sizeof(char),15))),
_tmp8CB)))),_tmp8C9)))));}_LL2DB: if((_tmp538.ReachableFL).tag != 2)goto _LL2D8;
_tmp53A=(struct _tuple12)(_tmp538.ReachableFL).val;_tmp53B=_tmp53A.f1;_LL2DC:
_tmp484=_tmp53B;_LL2D8:;}{void*_tmp53F=_tmp487->r;void*_tmp541;struct Cyc_Absyn_Vardecl*
_tmp543;void*_tmp545;struct Cyc_Absyn_Vardecl*_tmp547;void*_tmp549;struct Cyc_Absyn_Vardecl*
_tmp54B;void*_tmp54D;struct Cyc_Absyn_Vardecl*_tmp54F;_LL2DE:{struct Cyc_Absyn_Var_e_struct*
_tmp540=(struct Cyc_Absyn_Var_e_struct*)_tmp53F;if(_tmp540->tag != 1)goto _LL2E0;
else{_tmp541=(void*)_tmp540->f2;{struct Cyc_Absyn_Global_b_struct*_tmp542=(struct
Cyc_Absyn_Global_b_struct*)_tmp541;if(_tmp542->tag != 1)goto _LL2E0;else{_tmp543=
_tmp542->f1;}};}}if(!(!_tmp543->escapes))goto _LL2E0;_LL2DF: _tmp547=_tmp543;goto
_LL2E1;_LL2E0:{struct Cyc_Absyn_Var_e_struct*_tmp544=(struct Cyc_Absyn_Var_e_struct*)
_tmp53F;if(_tmp544->tag != 1)goto _LL2E2;else{_tmp545=(void*)_tmp544->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp546=(struct Cyc_Absyn_Local_b_struct*)_tmp545;if(
_tmp546->tag != 4)goto _LL2E2;else{_tmp547=_tmp546->f1;}};}}if(!(!_tmp547->escapes))
goto _LL2E2;_LL2E1: _tmp54B=_tmp547;goto _LL2E3;_LL2E2:{struct Cyc_Absyn_Var_e_struct*
_tmp548=(struct Cyc_Absyn_Var_e_struct*)_tmp53F;if(_tmp548->tag != 1)goto _LL2E4;
else{_tmp549=(void*)_tmp548->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp54A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp549;if(_tmp54A->tag != 5)goto _LL2E4;else{_tmp54B=_tmp54A->f1;}};}}if(!(!
_tmp54B->escapes))goto _LL2E4;_LL2E3: _tmp54F=_tmp54B;goto _LL2E5;_LL2E4:{struct Cyc_Absyn_Var_e_struct*
_tmp54C=(struct Cyc_Absyn_Var_e_struct*)_tmp53F;if(_tmp54C->tag != 1)goto _LL2E6;
else{_tmp54D=(void*)_tmp54C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp54E=(struct
Cyc_Absyn_Param_b_struct*)_tmp54D;if(_tmp54E->tag != 3)goto _LL2E6;else{_tmp54F=
_tmp54E->f1;}};}}if(!(!_tmp54F->escapes))goto _LL2E6;_LL2E5: {void*_tmp550=
_tmp488->r;union Cyc_Absyn_Cnst _tmp552;struct _tuple6 _tmp553;int _tmp554;struct Cyc_Absyn_Exp*
_tmp556;struct Cyc_Absyn_Exp _tmp557;void*_tmp558;union Cyc_Absyn_Cnst _tmp55A;
struct _tuple6 _tmp55B;int _tmp55C;enum Cyc_Absyn_Primop _tmp55E;struct Cyc_List_List*
_tmp55F;struct Cyc_List_List _tmp560;struct Cyc_Absyn_Exp*_tmp561;_LL2E9: {struct
Cyc_Absyn_Const_e_struct*_tmp551=(struct Cyc_Absyn_Const_e_struct*)_tmp550;if(
_tmp551->tag != 0)goto _LL2EB;else{_tmp552=_tmp551->f1;if((_tmp552.Int_c).tag != 4)
goto _LL2EB;_tmp553=(struct _tuple6)(_tmp552.Int_c).val;_tmp554=_tmp553.f2;}}
_LL2EA: _tmp55C=_tmp554;goto _LL2EC;_LL2EB: {struct Cyc_Absyn_Cast_e_struct*_tmp555=(
struct Cyc_Absyn_Cast_e_struct*)_tmp550;if(_tmp555->tag != 15)goto _LL2ED;else{
_tmp556=_tmp555->f2;_tmp557=*_tmp556;_tmp558=_tmp557.r;{struct Cyc_Absyn_Const_e_struct*
_tmp559=(struct Cyc_Absyn_Const_e_struct*)_tmp558;if(_tmp559->tag != 0)goto _LL2ED;
else{_tmp55A=_tmp559->f1;if((_tmp55A.Int_c).tag != 4)goto _LL2ED;_tmp55B=(struct
_tuple6)(_tmp55A.Int_c).val;_tmp55C=_tmp55B.f2;}};}}_LL2EC: {struct _RegionHandle*
_tmp562=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8CE;struct Cyc_List_List*
_tmp8CD;struct Cyc_List_List*_tmp563=(_tmp8CD=_region_malloc(_tmp562,sizeof(*
_tmp8CD)),((_tmp8CD->hd=((_tmp8CE=_region_malloc(_tmp562,sizeof(*_tmp8CE)),((
_tmp8CE->vd=_tmp54F,((_tmp8CE->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp55C),_tmp8CE)))))),((_tmp8CD->tl=_tmp485,_tmp8CD)))));struct _tuple15 _tmp8CF;
return(_tmp8CF.f1=Cyc_CfFlowInfo_ReachableFL(_tmp484,_tmp563),((_tmp8CF.f2=
_tmp537,_tmp8CF)));}_LL2ED: {struct Cyc_Absyn_Primop_e_struct*_tmp55D=(struct Cyc_Absyn_Primop_e_struct*)
_tmp550;if(_tmp55D->tag != 3)goto _LL2EF;else{_tmp55E=_tmp55D->f1;_tmp55F=_tmp55D->f2;
if(_tmp55F == 0)goto _LL2EF;_tmp560=*_tmp55F;_tmp561=(struct Cyc_Absyn_Exp*)_tmp560.hd;}}
_LL2EE: {void*_tmp567=_tmp561->r;void*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56B;
void*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56F;void*_tmp571;struct Cyc_Absyn_Vardecl*
_tmp573;void*_tmp575;struct Cyc_Absyn_Vardecl*_tmp577;_LL2F2:{struct Cyc_Absyn_Var_e_struct*
_tmp568=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp568->tag != 1)goto _LL2F4;
else{_tmp569=(void*)_tmp568->f2;{struct Cyc_Absyn_Global_b_struct*_tmp56A=(struct
Cyc_Absyn_Global_b_struct*)_tmp569;if(_tmp56A->tag != 1)goto _LL2F4;else{_tmp56B=
_tmp56A->f1;}};}}if(!(!_tmp56B->escapes))goto _LL2F4;_LL2F3: _tmp56F=_tmp56B;goto
_LL2F5;_LL2F4:{struct Cyc_Absyn_Var_e_struct*_tmp56C=(struct Cyc_Absyn_Var_e_struct*)
_tmp567;if(_tmp56C->tag != 1)goto _LL2F6;else{_tmp56D=(void*)_tmp56C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp56E=(struct Cyc_Absyn_Local_b_struct*)_tmp56D;if(
_tmp56E->tag != 4)goto _LL2F6;else{_tmp56F=_tmp56E->f1;}};}}if(!(!_tmp56F->escapes))
goto _LL2F6;_LL2F5: _tmp573=_tmp56F;goto _LL2F7;_LL2F6:{struct Cyc_Absyn_Var_e_struct*
_tmp570=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp570->tag != 1)goto _LL2F8;
else{_tmp571=(void*)_tmp570->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp572=(struct Cyc_Absyn_Pat_b_struct*)
_tmp571;if(_tmp572->tag != 5)goto _LL2F8;else{_tmp573=_tmp572->f1;}};}}if(!(!
_tmp573->escapes))goto _LL2F8;_LL2F7: _tmp577=_tmp573;goto _LL2F9;_LL2F8:{struct Cyc_Absyn_Var_e_struct*
_tmp574=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp574->tag != 1)goto _LL2FA;
else{_tmp575=(void*)_tmp574->f2;{struct Cyc_Absyn_Param_b_struct*_tmp576=(struct
Cyc_Absyn_Param_b_struct*)_tmp575;if(_tmp576->tag != 3)goto _LL2FA;else{_tmp577=
_tmp576->f1;}};}}if(!(!_tmp577->escapes))goto _LL2FA;_LL2F9: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp578=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp8D2;struct Cyc_List_List*_tmp8D1;
struct Cyc_List_List*_tmp579=(_tmp8D1=_region_malloc(_tmp578->r,sizeof(*_tmp8D1)),((
_tmp8D1->hd=((_tmp8D2=_region_malloc(_tmp578->r,sizeof(*_tmp8D2)),((_tmp8D2->vd=
_tmp54F,((_tmp8D2->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp577),_tmp8D2)))))),((
_tmp8D1->tl=_tmp485,_tmp8D1)))));struct _tuple15 _tmp8D3;return(_tmp8D3.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp484,_tmp579),((_tmp8D3.f2=_tmp537,_tmp8D3)));}_LL2FA:;_LL2FB: {struct
_tuple15 _tmp8D4;return(_tmp8D4.f1=_tmp536,((_tmp8D4.f2=_tmp537,_tmp8D4)));}
_LL2F1:;}_LL2EF:;_LL2F0: {struct _tuple15 _tmp8D5;return(_tmp8D5.f1=_tmp536,((
_tmp8D5.f2=_tmp537,_tmp8D5)));}_LL2E8:;}_LL2E6:;_LL2E7: {struct _tuple15 _tmp8D6;
return(_tmp8D6.f1=_tmp536,((_tmp8D6.f2=_tmp537,_tmp8D6)));}_LL2DD:;};}default:
_LL2D7: {struct _tuple15 _tmp8D7;return(_tmp8D7.f1=f,((_tmp8D7.f2=f,_tmp8D7)));}}}
_LL278:;};}_LL276:;_LL277: goto _LL269;_LL269:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp582;void*_tmp583;struct _tuple13 _tmp581=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp584=_tmp582;int _tmp585;struct _tuple12 _tmp586;struct Cyc_Dict_Dict _tmp587;
_LL2FE: if((_tmp584.BottomFL).tag != 1)goto _LL300;_tmp585=(int)(_tmp584.BottomFL).val;
_LL2FF: {struct _tuple15 _tmp8D8;return(_tmp8D8.f1=_tmp582,((_tmp8D8.f2=_tmp582,
_tmp8D8)));}_LL300: if((_tmp584.ReachableFL).tag != 2)goto _LL2FD;_tmp586=(struct
_tuple12)(_tmp584.ReachableFL).val;_tmp587=_tmp586.f1;_LL301: {void*_tmp589=
_tmp583;enum Cyc_CfFlowInfo_InitLevel _tmp58F;enum Cyc_CfFlowInfo_InitLevel
_tmp591;enum Cyc_CfFlowInfo_InitLevel _tmp594;_LL303: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp58A=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp589;if(_tmp58A->tag != 0)goto
_LL305;}_LL304: {struct _tuple15 _tmp8D9;return(_tmp8D9.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8D9.f2=_tmp582,_tmp8D9)));}_LL305: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp58B=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp589;if(_tmp58B->tag != 2)
goto _LL307;}_LL306: goto _LL308;_LL307: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp58C=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp589;if(_tmp58C->tag != 1)
goto _LL309;}_LL308: goto _LL30A;_LL309: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp58D=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp589;if(_tmp58D->tag != 5)goto
_LL30B;}_LL30A: {struct _tuple15 _tmp8DA;return(_tmp8DA.f1=_tmp582,((_tmp8DA.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp8DA)));}_LL30B: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp58E=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp589;if(_tmp58E->tag != 3)goto
_LL30D;else{_tmp58F=_tmp58E->f1;if(_tmp58F != Cyc_CfFlowInfo_NoneIL)goto _LL30D;}}
_LL30C: goto _LL30E;_LL30D: {struct Cyc_CfFlowInfo_Esc_struct*_tmp590=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp589;if(_tmp590->tag != 4)goto _LL30F;else{_tmp591=_tmp590->f1;if(_tmp591 != Cyc_CfFlowInfo_NoneIL)
goto _LL30F;}}_LL30E: goto _LL310;_LL30F: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp592=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp589;if(_tmp592->tag != 7)goto
_LL311;}_LL310:{const char*_tmp8DD;void*_tmp8DC;(_tmp8DC=0,Cyc_Tcutil_terr(e->loc,((
_tmp8DD="expression may not be initialized",_tag_dyneither(_tmp8DD,sizeof(char),
34))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}{struct _tuple15 _tmp8DE;return(
_tmp8DE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8DE.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8DE)));};_LL311: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp593=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp589;if(_tmp593->tag != 3)goto _LL313;else{_tmp594=_tmp593->f1;}}_LL312: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp582,e,_tmp594);_LL313: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp595=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp589;if(_tmp595->tag != 4)goto _LL315;}
_LL314: {struct _tuple15 _tmp8DF;return(_tmp8DF.f1=_tmp582,((_tmp8DF.f2=_tmp582,
_tmp8DF)));}_LL315: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp596=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp589;if(_tmp596->tag != 6)goto _LL302;}_LL316: {struct Cyc_Core_Impossible_struct
_tmp8E5;const char*_tmp8E4;struct Cyc_Core_Impossible_struct*_tmp8E3;(int)_throw((
void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E5.tag=Cyc_Core_Impossible,((
_tmp8E5.f1=((_tmp8E4="anal_test",_tag_dyneither(_tmp8E4,sizeof(char),10))),
_tmp8E5)))),_tmp8E3)))));}_LL302:;}_LL2FD:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5A1;struct Cyc_Position_Segment*
_tmp5A2;struct Cyc_NewControlFlow_AnalEnv*_tmp5A3;struct Cyc_Dict_Dict _tmp5A4;
struct _tuple22*_tmp5A0=ckenv;_tmp5A1=*_tmp5A0;_tmp5A2=_tmp5A1.f1;_tmp5A3=_tmp5A1.f2;
_tmp5A4=_tmp5A1.f3;{void*_tmp5A5=root;struct Cyc_Absyn_Vardecl*_tmp5A7;_LL318: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5A6=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5A5;if(_tmp5A6->tag != 0)goto _LL31A;else{_tmp5A7=_tmp5A6->f1;}}_LL319: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5A3->fenv)->r,_tmp5A7->type)){void*_tmp5A8=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5AC;_LL31D: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5A9=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5A8;if(_tmp5A9->tag != 7)goto _LL31F;}_LL31E: goto _LL320;_LL31F: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5AA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5A8;if(_tmp5AA->tag != 0)goto
_LL321;}_LL320: goto _LL322;_LL321: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5AB=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5A8;if(_tmp5AB->tag != 3)goto _LL323;
else{_tmp5AC=_tmp5AB->f1;if(_tmp5AC != Cyc_CfFlowInfo_NoneIL)goto _LL323;}}_LL322:
goto _LL31C;_LL323:;_LL324:{const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_String_pa_struct
_tmp8E7;(_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5A7->name)),((_tmp8E8[0]=& _tmp8E7,Cyc_Tcutil_warn(
_tmp5A2,((_tmp8E9="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp8E9,sizeof(char),57))),_tag_dyneither(_tmp8E8,sizeof(void*),1)))))));}
goto _LL31C;_LL31C:;}goto _LL317;_LL31A:;_LL31B: goto _LL317;_LL317:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow){union Cyc_CfFlowInfo_FlowInfo _tmp5B0=flow;int _tmp5B1;struct _tuple12 _tmp5B2;
struct Cyc_Dict_Dict _tmp5B3;_LL326: if((_tmp5B0.BottomFL).tag != 1)goto _LL328;
_tmp5B1=(int)(_tmp5B0.BottomFL).val;_LL327: return;_LL328: if((_tmp5B0.ReachableFL).tag
!= 2)goto _LL325;_tmp5B2=(struct _tuple12)(_tmp5B0.ReachableFL).val;_tmp5B3=
_tmp5B2.f1;_LL329:{struct Cyc_List_List*_tmp5B4=env->param_roots;for(0;_tmp5B4 != 
0;_tmp5B4=_tmp5B4->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5B3,Cyc_CfFlowInfo_lookup_place(
_tmp5B3,(struct Cyc_CfFlowInfo_Place*)_tmp5B4->hd))!= Cyc_CfFlowInfo_AllIL){const
char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_terr(loc,((_tmp8EC="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp8EC,sizeof(char),76))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp8ED;struct _tuple22
_tmp5B7=(_tmp8ED.f1=loc,((_tmp8ED.f2=env,((_tmp8ED.f3=_tmp5B3,_tmp8ED)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5B7,_tmp5B3);}
return;_LL325:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5B9=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5BB;struct Cyc_Core_Opt*_tmp5BC;
struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Stmt*_tmp5BE;struct Cyc_Position_Segment*
_tmp5BF;struct Cyc_Absyn_Switch_clause*_tmp5BA=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5BB=*_tmp5BA;_tmp5BC=_tmp5BB.pat_vars;_tmp5BD=_tmp5BB.where_clause;
_tmp5BE=_tmp5BB.body;_tmp5BF=_tmp5BB.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5B9,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5BC))->v,_tmp5B9->unknown_all,_tmp5BF);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5BD != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5BD;union Cyc_CfFlowInfo_FlowInfo _tmp5C1;union Cyc_CfFlowInfo_FlowInfo
_tmp5C2;struct _tuple15 _tmp5C0=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5C1=_tmp5C0.f1;_tmp5C2=_tmp5C0.f2;inflow=_tmp5C2;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp5C1,_tmp5BE);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp5BE);}{union Cyc_CfFlowInfo_FlowInfo _tmp5C3=clause_outflow;int
_tmp5C4;_LL32B: if((_tmp5C3.BottomFL).tag != 1)goto _LL32D;_tmp5C4=(int)(_tmp5C3.BottomFL).val;
_LL32C: goto _LL32A;_LL32D:;_LL32E: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_terr(_tmp5BE->loc,((_tmp8F0="switch clause may implicitly fallthru",
_tag_dyneither(_tmp8F0,sizeof(char),38))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
else{const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_Tcutil_warn(_tmp5BE->loc,((
_tmp8F3="switch clause may implicitly fallthru",_tag_dyneither(_tmp8F3,sizeof(
char),38))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL32A;_LL32A:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd);static void Cyc_NewControlFlow_check_dropped_unique_vd(
struct _tuple23*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp5CA;struct Cyc_NewControlFlow_AnalEnv*
_tmp5CB;struct Cyc_Dict_Dict _tmp5CC;struct Cyc_Position_Segment*_tmp5CD;struct
_tuple23*_tmp5C9=vdenv;_tmp5CA=*_tmp5C9;_tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;
_tmp5CD=_tmp5CA.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5CB->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp8F6;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp8F5;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5CE=(_tmp8F5=_region_malloc((
_tmp5CB->fenv)->r,sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F6.tag=0,((_tmp8F6.f1=vd,
_tmp8F6)))),_tmp8F5)));void*_tmp5CF=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp5CC,(void*)_tmp5CE);void*_tmp5D0=_tmp5CF;enum Cyc_CfFlowInfo_InitLevel
_tmp5D4;_LL330: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D1=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5D0;if(_tmp5D1->tag != 7)goto _LL332;}_LL331: goto _LL333;_LL332: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5D2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5D0;if(_tmp5D2->tag != 0)goto
_LL334;}_LL333: goto _LL335;_LL334: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5D3=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5D0;if(_tmp5D3->tag != 3)goto _LL336;
else{_tmp5D4=_tmp5D3->f1;if(_tmp5D4 != Cyc_CfFlowInfo_NoneIL)goto _LL336;}}_LL335:
goto _LL32F;_LL336:;_LL337:{const char*_tmp8FA;void*_tmp8F9[1];struct Cyc_String_pa_struct
_tmp8F8;(_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp8F9[0]=& _tmp8F8,Cyc_Tcutil_warn(_tmp5CD,((
_tmp8FA="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp8FA,sizeof(char),68))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))));}
goto _LL32F;_LL32F:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static void Cyc_NewControlFlow_check_dropped_unique(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo _tmp5DA=inflow;struct _tuple12
_tmp5DB;struct Cyc_Dict_Dict _tmp5DC;_LL339: if((_tmp5DA.ReachableFL).tag != 2)goto
_LL33B;_tmp5DB=(struct _tuple12)(_tmp5DA.ReachableFL).val;_tmp5DC=_tmp5DB.f1;
_LL33A: {struct _tuple23 _tmp8FB;struct _tuple23 _tmp5DD=(_tmp8FB.f1=env,((_tmp8FB.f2=
_tmp5DC,((_tmp8FB.f3=decl->loc,_tmp8FB)))));struct Cyc_CfFlowInfo_FlowEnv*_tmp5DE=
env->fenv;{void*_tmp5DF=decl->r;struct Cyc_Absyn_Vardecl*_tmp5E1;struct Cyc_Core_Opt*
_tmp5E3;struct Cyc_Core_Opt _tmp5E4;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*
_tmp5E7;_LL33E: {struct Cyc_Absyn_Var_d_struct*_tmp5E0=(struct Cyc_Absyn_Var_d_struct*)
_tmp5DF;if(_tmp5E0->tag != 0)goto _LL340;else{_tmp5E1=_tmp5E0->f1;}}_LL33F: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp5DD,_tmp5E1);goto _LL33D;_LL340: {struct Cyc_Absyn_Let_d_struct*_tmp5E2=(
struct Cyc_Absyn_Let_d_struct*)_tmp5DF;if(_tmp5E2->tag != 2)goto _LL342;else{
_tmp5E3=_tmp5E2->f2;if(_tmp5E3 == 0)goto _LL342;_tmp5E4=*_tmp5E3;_tmp5E5=(struct
Cyc_List_List*)_tmp5E4.v;}}_LL341: _tmp5E7=_tmp5E5;goto _LL343;_LL342: {struct Cyc_Absyn_Letv_d_struct*
_tmp5E6=(struct Cyc_Absyn_Letv_d_struct*)_tmp5DF;if(_tmp5E6->tag != 3)goto _LL344;
else{_tmp5E7=_tmp5E6->f1;}}_LL343:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp5DD,_tmp5E7);goto _LL33D;_LL344:;_LL345: goto _LL33D;_LL33D:;}goto _LL338;}
_LL33B:;_LL33C: goto _LL338;_LL338:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5EA;union Cyc_CfFlowInfo_FlowInfo*_tmp5EB;struct _tuple16 _tmp5E9=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;inflow=*_tmp5EB;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp5EC=env->fenv;void*_tmp5ED=s->r;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*
_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Stmt*
_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FA;struct Cyc_Absyn_Stmt*
_tmp5FB;struct _tuple9 _tmp5FD;struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Stmt*
_tmp5FF;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Stmt*_tmp602;struct _tuple9
_tmp603;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Exp*
_tmp607;struct _tuple9 _tmp608;struct Cyc_Absyn_Exp*_tmp609;struct Cyc_Absyn_Stmt*
_tmp60A;struct _tuple9 _tmp60B;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Stmt*
_tmp60D;struct Cyc_Absyn_Stmt*_tmp60E;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_List_List*
_tmp612;struct Cyc_Absyn_Switch_clause**_tmp613;struct Cyc_Absyn_Switch_clause*
_tmp614;struct Cyc_Absyn_Stmt*_tmp616;struct Cyc_Absyn_Stmt*_tmp618;struct Cyc_Absyn_Stmt*
_tmp61A;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_List_List*_tmp61D;struct Cyc_Absyn_Stmt*
_tmp61F;struct Cyc_List_List*_tmp620;struct Cyc_Absyn_Decl*_tmp622;struct Cyc_Absyn_Decl
_tmp623;void*_tmp624;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Tvar*_tmp627;
struct Cyc_Absyn_Vardecl*_tmp628;struct Cyc_Absyn_Stmt*_tmp629;struct Cyc_Absyn_Decl*
_tmp62B;struct Cyc_Absyn_Stmt*_tmp62C;struct Cyc_Absyn_Stmt*_tmp62E;struct Cyc_Absyn_Exp*
_tmp630;_LL347: {struct Cyc_Absyn_Skip_s_struct*_tmp5EE=(struct Cyc_Absyn_Skip_s_struct*)
_tmp5ED;if(_tmp5EE->tag != 0)goto _LL349;}_LL348: return inflow;_LL349: {struct Cyc_Absyn_Return_s_struct*
_tmp5EF=(struct Cyc_Absyn_Return_s_struct*)_tmp5ED;if(_tmp5EF->tag != 3)goto _LL34B;
else{_tmp5F0=_tmp5EF->f1;if(_tmp5F0 != 0)goto _LL34B;}}_LL34A: if(env->noreturn){
const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,Cyc_Tcutil_terr(s->loc,((_tmp8FE="`noreturn' function might return",
_tag_dyneither(_tmp8FE,sizeof(char),33))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL34B: {struct Cyc_Absyn_Return_s_struct*_tmp5F1=(struct Cyc_Absyn_Return_s_struct*)
_tmp5ED;if(_tmp5F1->tag != 3)goto _LL34D;else{_tmp5F2=_tmp5F1->f1;}}_LL34C: if(env->noreturn){
const char*_tmp901;void*_tmp900;(_tmp900=0,Cyc_Tcutil_terr(s->loc,((_tmp901="`noreturn' function might return",
_tag_dyneither(_tmp901,sizeof(char),33))),_tag_dyneither(_tmp900,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp636;void*_tmp637;struct _tuple13 _tmp635=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F2));_tmp636=_tmp635.f1;
_tmp637=_tmp635.f2;_tmp636=Cyc_NewControlFlow_use_Rval(env,_tmp5F2->loc,_tmp636,
_tmp637);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp636);return Cyc_CfFlowInfo_BottomFL();};
_LL34D: {struct Cyc_Absyn_Exp_s_struct*_tmp5F3=(struct Cyc_Absyn_Exp_s_struct*)
_tmp5ED;if(_tmp5F3->tag != 1)goto _LL34F;else{_tmp5F4=_tmp5F3->f1;}}_LL34E: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5F4)).f1;_LL34F: {struct Cyc_Absyn_Seq_s_struct*
_tmp5F5=(struct Cyc_Absyn_Seq_s_struct*)_tmp5ED;if(_tmp5F5->tag != 2)goto _LL351;
else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;}}_LL350: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5F6),_tmp5F7);_LL351: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp5F8=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5ED;if(_tmp5F8->tag != 4)goto
_LL353;else{_tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F8->f2;_tmp5FB=_tmp5F8->f3;}}_LL352: {
union Cyc_CfFlowInfo_FlowInfo _tmp639;union Cyc_CfFlowInfo_FlowInfo _tmp63A;struct
_tuple15 _tmp638=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5F9);_tmp639=_tmp638.f1;
_tmp63A=_tmp638.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp63B=Cyc_NewControlFlow_anal_stmt(
env,_tmp63A,_tmp5FB);union Cyc_CfFlowInfo_FlowInfo _tmp63C=Cyc_NewControlFlow_anal_stmt(
env,_tmp639,_tmp5FA);return Cyc_CfFlowInfo_join_flow(_tmp5EC,env->all_changed,
_tmp63C,_tmp63B);};}_LL353: {struct Cyc_Absyn_While_s_struct*_tmp5FC=(struct Cyc_Absyn_While_s_struct*)
_tmp5ED;if(_tmp5FC->tag != 5)goto _LL355;else{_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FD.f1;
_tmp5FF=_tmp5FD.f2;_tmp600=_tmp5FC->f2;}}_LL354: {union Cyc_CfFlowInfo_FlowInfo*
_tmp63E;struct _tuple16 _tmp63D=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp5FF);_tmp63E=_tmp63D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp63F=*_tmp63E;union
Cyc_CfFlowInfo_FlowInfo _tmp641;union Cyc_CfFlowInfo_FlowInfo _tmp642;struct
_tuple15 _tmp640=Cyc_NewControlFlow_anal_test(env,_tmp63F,_tmp5FE);_tmp641=
_tmp640.f1;_tmp642=_tmp640.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp643=Cyc_NewControlFlow_anal_stmt(
env,_tmp641,_tmp600);Cyc_NewControlFlow_update_flow(env,_tmp5FF,_tmp643);return
_tmp642;};};}_LL355: {struct Cyc_Absyn_Do_s_struct*_tmp601=(struct Cyc_Absyn_Do_s_struct*)
_tmp5ED;if(_tmp601->tag != 14)goto _LL357;else{_tmp602=_tmp601->f1;_tmp603=_tmp601->f2;
_tmp604=_tmp603.f1;_tmp605=_tmp603.f2;}}_LL356: {union Cyc_CfFlowInfo_FlowInfo
_tmp644=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp602);union Cyc_CfFlowInfo_FlowInfo*
_tmp646;struct _tuple16 _tmp645=Cyc_NewControlFlow_pre_stmt_check(env,_tmp644,
_tmp605);_tmp646=_tmp645.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp647=*_tmp646;union
Cyc_CfFlowInfo_FlowInfo _tmp649;union Cyc_CfFlowInfo_FlowInfo _tmp64A;struct
_tuple15 _tmp648=Cyc_NewControlFlow_anal_test(env,_tmp647,_tmp604);_tmp649=
_tmp648.f1;_tmp64A=_tmp648.f2;Cyc_NewControlFlow_update_flow(env,_tmp602,_tmp649);
return _tmp64A;};}_LL357: {struct Cyc_Absyn_For_s_struct*_tmp606=(struct Cyc_Absyn_For_s_struct*)
_tmp5ED;if(_tmp606->tag != 9)goto _LL359;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;
_tmp609=_tmp608.f1;_tmp60A=_tmp608.f2;_tmp60B=_tmp606->f3;_tmp60C=_tmp60B.f1;
_tmp60D=_tmp60B.f2;_tmp60E=_tmp606->f4;}}_LL358: {union Cyc_CfFlowInfo_FlowInfo
_tmp64B=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp607)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp64D;struct _tuple16 _tmp64C=Cyc_NewControlFlow_pre_stmt_check(env,_tmp64B,
_tmp60A);_tmp64D=_tmp64C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp64E=*_tmp64D;union
Cyc_CfFlowInfo_FlowInfo _tmp650;union Cyc_CfFlowInfo_FlowInfo _tmp651;struct
_tuple15 _tmp64F=Cyc_NewControlFlow_anal_test(env,_tmp64E,_tmp609);_tmp650=
_tmp64F.f1;_tmp651=_tmp64F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp652=Cyc_NewControlFlow_anal_stmt(
env,_tmp650,_tmp60E);union Cyc_CfFlowInfo_FlowInfo*_tmp654;struct _tuple16 _tmp653=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp652,_tmp60D);_tmp654=_tmp653.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp655=*_tmp654;union Cyc_CfFlowInfo_FlowInfo _tmp656=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp655,_tmp60C)).f1;Cyc_NewControlFlow_update_flow(env,_tmp60A,_tmp656);
return _tmp651;};};};}_LL359: {struct Cyc_Absyn_Break_s_struct*_tmp60F=(struct Cyc_Absyn_Break_s_struct*)
_tmp5ED;if(_tmp60F->tag != 6)goto _LL35B;else{_tmp610=_tmp60F->f1;if(_tmp610 != 0)
goto _LL35B;}}_LL35A: return Cyc_CfFlowInfo_BottomFL();_LL35B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp611=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp5ED;if(_tmp611->tag != 11)goto
_LL35D;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;if(_tmp613 == 0)goto _LL35D;
_tmp614=*_tmp613;}}_LL35C: {struct _RegionHandle*_tmp657=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp659;struct Cyc_List_List*_tmp65A;struct _tuple19 _tmp658=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp657,env,inflow,_tmp612,1,1);_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;for(0;
_tmp65A != 0;(_tmp65A=_tmp65A->tl,_tmp612=_tmp612->tl)){_tmp659=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp612))->hd)->loc,
_tmp659,(void*)_tmp65A->hd);}_tmp659=Cyc_NewControlFlow_add_vars(_tmp5EC,_tmp659,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp614->pat_vars))->v,
_tmp5EC->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp614->body,_tmp659);return Cyc_CfFlowInfo_BottomFL();}_LL35D: {struct Cyc_Absyn_Break_s_struct*
_tmp615=(struct Cyc_Absyn_Break_s_struct*)_tmp5ED;if(_tmp615->tag != 6)goto _LL35F;
else{_tmp616=_tmp615->f1;}}_LL35E: _tmp618=_tmp616;goto _LL360;_LL35F: {struct Cyc_Absyn_Continue_s_struct*
_tmp617=(struct Cyc_Absyn_Continue_s_struct*)_tmp5ED;if(_tmp617->tag != 7)goto
_LL361;else{_tmp618=_tmp617->f1;}}_LL360: _tmp61A=_tmp618;goto _LL362;_LL361: {
struct Cyc_Absyn_Goto_s_struct*_tmp619=(struct Cyc_Absyn_Goto_s_struct*)_tmp5ED;
if(_tmp619->tag != 8)goto _LL363;else{_tmp61A=_tmp619->f2;}}_LL362: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp65B=_tmp5EA->encloser;struct Cyc_Absyn_Stmt*
_tmp65C=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp61A)))->encloser;while(_tmp65C != _tmp65B){struct Cyc_Absyn_Stmt*_tmp65D=(Cyc_NewControlFlow_get_stmt_annot(
_tmp65B))->encloser;if(_tmp65D == _tmp65B){{const char*_tmp904;void*_tmp903;(
_tmp903=0,Cyc_Tcutil_terr(s->loc,((_tmp904="goto enters local scope or exception handler",
_tag_dyneither(_tmp904,sizeof(char),45))),_tag_dyneither(_tmp903,sizeof(void*),0)));}
break;}_tmp65B=_tmp65D;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp61A),inflow);return Cyc_CfFlowInfo_BottomFL();_LL363: {struct Cyc_Absyn_Switch_s_struct*
_tmp61B=(struct Cyc_Absyn_Switch_s_struct*)_tmp5ED;if(_tmp61B->tag != 10)goto
_LL365;else{_tmp61C=_tmp61B->f1;_tmp61D=_tmp61B->f2;}}_LL364: {union Cyc_CfFlowInfo_FlowInfo
_tmp661;void*_tmp662;struct _tuple13 _tmp660=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp61C);_tmp661=_tmp660.f1;_tmp662=_tmp660.f2;_tmp661=Cyc_NewControlFlow_use_Rval(
env,_tmp61C->loc,_tmp661,_tmp662);return Cyc_NewControlFlow_anal_scs(env,_tmp661,
_tmp61D);}_LL365: {struct Cyc_Absyn_TryCatch_s_struct*_tmp61E=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5ED;if(_tmp61E->tag != 15)goto _LL367;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61E->f2;}}
_LL366: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp61F);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp620);{union Cyc_CfFlowInfo_FlowInfo _tmp663=scs_outflow;int
_tmp664;_LL372: if((_tmp663.BottomFL).tag != 1)goto _LL374;_tmp664=(int)(_tmp663.BottomFL).val;
_LL373: goto _LL371;_LL374:;_LL375: {const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_Tcutil_terr(
s->loc,((_tmp907="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp907,sizeof(char),42))),_tag_dyneither(_tmp906,sizeof(void*),0)));}_LL371:;}
outflow=s1_outflow;return outflow;};};}_LL367: {struct Cyc_Absyn_Decl_s_struct*
_tmp621=(struct Cyc_Absyn_Decl_s_struct*)_tmp5ED;if(_tmp621->tag != 12)goto _LL369;
else{_tmp622=_tmp621->f1;_tmp623=*_tmp622;_tmp624=_tmp623.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp625=(struct Cyc_Absyn_Alias_d_struct*)_tmp624;if(_tmp625->tag != 5)goto _LL369;
else{_tmp626=_tmp625->f1;_tmp627=_tmp625->f2;_tmp628=_tmp625->f3;}};_tmp629=
_tmp621->f2;}}_LL368: {union Cyc_CfFlowInfo_FlowInfo _tmp668;union Cyc_CfFlowInfo_AbsLVal
_tmp669;struct _tuple14 _tmp667=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp626);
_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;{struct _tuple14 _tmp908;struct _tuple14
_tmp66B=(_tmp908.f1=_tmp668,((_tmp908.f2=_tmp669,_tmp908)));union Cyc_CfFlowInfo_FlowInfo
_tmp66C;struct _tuple12 _tmp66D;struct Cyc_Dict_Dict _tmp66E;struct Cyc_List_List*
_tmp66F;union Cyc_CfFlowInfo_AbsLVal _tmp670;struct Cyc_CfFlowInfo_Place*_tmp671;
union Cyc_CfFlowInfo_FlowInfo _tmp672;int _tmp673;_LL377: _tmp66C=_tmp66B.f1;if((
_tmp66C.ReachableFL).tag != 2)goto _LL379;_tmp66D=(struct _tuple12)(_tmp66C.ReachableFL).val;
_tmp66E=_tmp66D.f1;_tmp66F=_tmp66D.f2;_tmp670=_tmp66B.f2;if((_tmp670.PlaceL).tag
!= 1)goto _LL379;_tmp671=(struct Cyc_CfFlowInfo_Place*)(_tmp670.PlaceL).val;_LL378: {
void*_tmp674=Cyc_CfFlowInfo_lookup_place(_tmp66E,_tmp671);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp626->topt))->v;{void*_tmp675=_tmp626->r;struct Cyc_Absyn_Exp*
_tmp677;_LL37E: {struct Cyc_Absyn_Cast_e_struct*_tmp676=(struct Cyc_Absyn_Cast_e_struct*)
_tmp675;if(_tmp676->tag != 15)goto _LL380;else{_tmp677=_tmp676->f2;}}_LL37F: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp677->topt))->v;goto _LL37D;_LL380:;
_LL381: goto _LL37D;_LL37D:;}{void*_tmp678=Cyc_CfFlowInfo_make_unique_consumed(
_tmp5EC,t,_tmp626,env->iteration_num,_tmp674);_tmp66E=Cyc_CfFlowInfo_assign_place(
_tmp5EC,_tmp626->loc,_tmp66E,env->all_changed,_tmp671,_tmp678);_tmp668=Cyc_CfFlowInfo_ReachableFL(
_tmp66E,_tmp66F);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp5EC,_tmp66E,
_tmp674)){case Cyc_CfFlowInfo_AllIL: _LL382: leaf=_tmp5EC->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL383: leaf=_tmp5EC->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL384: leaf=_tmp5EC->unknown_this;break;}{void*_tmp679=Cyc_CfFlowInfo_typ_to_absrval(
_tmp5EC,t,leaf);_tmp668=Cyc_NewControlFlow_use_Rval(env,_tmp626->loc,_tmp668,
_tmp674);{struct Cyc_List_List _tmp909;struct Cyc_List_List _tmp67A=(_tmp909.hd=
_tmp628,((_tmp909.tl=0,_tmp909)));_tmp668=Cyc_NewControlFlow_add_vars(_tmp5EC,
_tmp668,(struct Cyc_List_List*)& _tmp67A,_tmp5EC->unknown_all,s->loc);_tmp668=Cyc_NewControlFlow_anal_stmt(
env,_tmp668,_tmp629);{union Cyc_CfFlowInfo_FlowInfo _tmp67B=_tmp668;struct _tuple12
_tmp67C;struct Cyc_Dict_Dict _tmp67D;struct Cyc_List_List*_tmp67E;_LL387: if((
_tmp67B.ReachableFL).tag != 2)goto _LL389;_tmp67C=(struct _tuple12)(_tmp67B.ReachableFL).val;
_tmp67D=_tmp67C.f1;_tmp67E=_tmp67C.f2;_LL388: _tmp67D=Cyc_CfFlowInfo_assign_place(
_tmp5EC,s->loc,_tmp67D,env->all_changed,_tmp671,_tmp679);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp67D,_tmp67E);return outflow;_LL389:;_LL38A: return _tmp668;_LL386:;};};};};};}
_LL379: _tmp672=_tmp66B.f1;if((_tmp672.BottomFL).tag != 1)goto _LL37B;_tmp673=(int)(
_tmp672.BottomFL).val;_LL37A: return _tmp668;_LL37B:;_LL37C:{const char*_tmp90C;
void*_tmp90B;(_tmp90B=0,Cyc_Tcutil_terr(_tmp626->loc,((_tmp90C="bad alias expression--no unique path found",
_tag_dyneither(_tmp90C,sizeof(char),43))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL376:;};}_LL369: {struct Cyc_Absyn_Decl_s_struct*
_tmp62A=(struct Cyc_Absyn_Decl_s_struct*)_tmp5ED;if(_tmp62A->tag != 12)goto _LL36B;
else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62A->f2;}}_LL36A: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp62B),_tmp62C);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp62B);return outflow;
_LL36B: {struct Cyc_Absyn_Label_s_struct*_tmp62D=(struct Cyc_Absyn_Label_s_struct*)
_tmp5ED;if(_tmp62D->tag != 13)goto _LL36D;else{_tmp62E=_tmp62D->f2;}}_LL36C: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp62E);_LL36D: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp62F=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5ED;if(_tmp62F->tag != 16)goto
_LL36F;else{_tmp630=_tmp62F->f1;}}_LL36E: {union Cyc_CfFlowInfo_FlowInfo _tmp683;
void*_tmp684;struct _tuple13 _tmp682=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp630);_tmp683=_tmp682.f1;_tmp684=_tmp682.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp685=Cyc_NewControlFlow_use_Rval(env,_tmp630->loc,_tmp683,_tmp684);void*
_tmp686=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp630->topt))->v);
void*_tmp688;_LL38C: {struct Cyc_Absyn_RgnHandleType_struct*_tmp687=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp686;if(_tmp687->tag != 16)goto _LL38E;else{_tmp688=(void*)_tmp687->f1;}}_LL38D:
return Cyc_CfFlowInfo_kill_flow_region(_tmp5EC,_tmp683,_tmp688);_LL38E:;_LL38F: {
struct Cyc_Core_Impossible_struct _tmp912;const char*_tmp911;struct Cyc_Core_Impossible_struct*
_tmp910;(int)_throw((void*)((_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910[0]=((
_tmp912.tag=Cyc_Core_Impossible,((_tmp912.f1=((_tmp911="anal_stmt -- reset_region",
_tag_dyneither(_tmp911,sizeof(char),26))),_tmp912)))),_tmp910)))));}_LL38B:;};}
_LL36F:;_LL370: {struct Cyc_Core_Impossible_struct _tmp918;const char*_tmp917;
struct Cyc_Core_Impossible_struct*_tmp916;(int)_throw((void*)((_tmp916=_cycalloc(
sizeof(*_tmp916)),((_tmp916[0]=((_tmp918.tag=Cyc_Core_Impossible,((_tmp918.f1=((
_tmp917="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp917,sizeof(char),56))),_tmp918)))),_tmp916)))));}_LL346:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp68F=env->fenv;
void*_tmp690=decl->r;struct Cyc_Absyn_Vardecl*_tmp692;struct Cyc_Core_Opt*_tmp694;
struct Cyc_Core_Opt _tmp695;struct Cyc_List_List*_tmp696;struct Cyc_Absyn_Exp*
_tmp697;struct Cyc_List_List*_tmp699;struct Cyc_Absyn_Fndecl*_tmp69B;struct Cyc_Absyn_Tvar*
_tmp69D;struct Cyc_Absyn_Vardecl*_tmp69E;int _tmp69F;struct Cyc_Absyn_Exp*_tmp6A0;
_LL391: {struct Cyc_Absyn_Var_d_struct*_tmp691=(struct Cyc_Absyn_Var_d_struct*)
_tmp690;if(_tmp691->tag != 0)goto _LL393;else{_tmp692=_tmp691->f1;}}_LL392: {
struct Cyc_List_List _tmp919;struct Cyc_List_List _tmp6A1=(_tmp919.hd=_tmp692,((
_tmp919.tl=0,_tmp919)));inflow=Cyc_NewControlFlow_add_vars(_tmp68F,inflow,(
struct Cyc_List_List*)& _tmp6A1,_tmp68F->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6A2=_tmp692->initializer;if(_tmp6A2 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6A4;void*_tmp6A5;struct _tuple13 _tmp6A3=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6A2);_tmp6A4=_tmp6A3.f1;_tmp6A5=_tmp6A3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A6=_tmp6A4;int _tmp6A7;struct _tuple12 _tmp6A8;
struct Cyc_Dict_Dict _tmp6A9;struct Cyc_List_List*_tmp6AA;_LL39E: if((_tmp6A6.BottomFL).tag
!= 1)goto _LL3A0;_tmp6A7=(int)(_tmp6A6.BottomFL).val;_LL39F: return Cyc_CfFlowInfo_BottomFL();
_LL3A0: if((_tmp6A6.ReachableFL).tag != 2)goto _LL39D;_tmp6A8=(struct _tuple12)(
_tmp6A6.ReachableFL).val;_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_LL3A1:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp91F;struct Cyc_CfFlowInfo_VarRoot_struct _tmp91E;struct Cyc_CfFlowInfo_Place*
_tmp91D;_tmp6A9=Cyc_CfFlowInfo_assign_place(_tmp68F,decl->loc,_tmp6A9,env->all_changed,((
_tmp91D=_region_malloc(env->r,sizeof(*_tmp91D)),((_tmp91D->root=(void*)((_tmp91F=
_region_malloc(env->r,sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp91E.tag=0,((_tmp91E.f1=
_tmp692,_tmp91E)))),_tmp91F)))),((_tmp91D->fields=0,_tmp91D)))))),_tmp6A5);}
_tmp6AA=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6AA,_tmp692,(struct Cyc_Absyn_Exp*)
_check_null(_tmp692->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6AE=Cyc_CfFlowInfo_ReachableFL(
_tmp6A9,_tmp6AA);Cyc_NewControlFlow_update_tryflow(env,_tmp6AE);return _tmp6AE;};
_LL39D:;};};};}_LL393: {struct Cyc_Absyn_Let_d_struct*_tmp693=(struct Cyc_Absyn_Let_d_struct*)
_tmp690;if(_tmp693->tag != 2)goto _LL395;else{_tmp694=_tmp693->f2;if(_tmp694 == 0)
goto _LL395;_tmp695=*_tmp694;_tmp696=(struct Cyc_List_List*)_tmp695.v;_tmp697=
_tmp693->f3;}}_LL394: {union Cyc_CfFlowInfo_FlowInfo _tmp6B1;void*_tmp6B2;struct
_tuple13 _tmp6B0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp697);_tmp6B1=
_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;_tmp6B1=Cyc_NewControlFlow_use_Rval(env,_tmp697->loc,
_tmp6B1,_tmp6B2);return Cyc_NewControlFlow_add_vars(_tmp68F,_tmp6B1,_tmp696,
_tmp68F->unknown_all,decl->loc);}_LL395: {struct Cyc_Absyn_Letv_d_struct*_tmp698=(
struct Cyc_Absyn_Letv_d_struct*)_tmp690;if(_tmp698->tag != 3)goto _LL397;else{
_tmp699=_tmp698->f1;}}_LL396: return Cyc_NewControlFlow_add_vars(_tmp68F,inflow,
_tmp699,_tmp68F->unknown_none,decl->loc);_LL397: {struct Cyc_Absyn_Fn_d_struct*
_tmp69A=(struct Cyc_Absyn_Fn_d_struct*)_tmp690;if(_tmp69A->tag != 1)goto _LL399;
else{_tmp69B=_tmp69A->f1;}}_LL398: Cyc_NewControlFlow_check_nested_fun(_tmp68F,
inflow,_tmp69B);{void*_tmp6B3=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp69B->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6B4=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp69B->fn_vardecl);
struct Cyc_List_List*_tmp920;return Cyc_NewControlFlow_add_vars(_tmp68F,inflow,((
_tmp920=_region_malloc(env->r,sizeof(*_tmp920)),((_tmp920->hd=_tmp6B4,((_tmp920->tl=
0,_tmp920)))))),_tmp68F->unknown_all,decl->loc);};_LL399: {struct Cyc_Absyn_Region_d_struct*
_tmp69C=(struct Cyc_Absyn_Region_d_struct*)_tmp690;if(_tmp69C->tag != 4)goto _LL39B;
else{_tmp69D=_tmp69C->f1;_tmp69E=_tmp69C->f2;_tmp69F=_tmp69C->f3;_tmp6A0=_tmp69C->f4;}}
_LL39A: if(_tmp6A0 != 0){struct Cyc_Absyn_Exp*_tmp6B6=(struct Cyc_Absyn_Exp*)_tmp6A0;
union Cyc_CfFlowInfo_FlowInfo _tmp6B8;void*_tmp6B9;struct _tuple13 _tmp6B7=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6B6);_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6B6->loc,_tmp6B8,_tmp6B9);}{struct Cyc_List_List _tmp921;struct Cyc_List_List
_tmp6BA=(_tmp921.hd=_tmp69E,((_tmp921.tl=0,_tmp921)));return Cyc_NewControlFlow_add_vars(
_tmp68F,inflow,(struct Cyc_List_List*)& _tmp6BA,_tmp68F->unknown_all,decl->loc);};
_LL39B:;_LL39C: {struct Cyc_Core_Impossible_struct _tmp927;const char*_tmp926;
struct Cyc_Core_Impossible_struct*_tmp925;(int)_throw((void*)((_tmp925=_cycalloc(
sizeof(*_tmp925)),((_tmp925[0]=((_tmp927.tag=Cyc_Core_Impossible,((_tmp927.f1=((
_tmp926="anal_decl: unexpected decl variant",_tag_dyneither(_tmp926,sizeof(char),
35))),_tmp927)))),_tmp925)))));}_LL390:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp6BF=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp6BF;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp6C0=fenv->r;struct Cyc_Position_Segment*_tmp6C1=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6C1);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp6C2=inflow;int _tmp6C3;struct
_tuple12 _tmp6C4;struct Cyc_Dict_Dict _tmp6C5;struct Cyc_List_List*_tmp6C6;_LL3A3:
if((_tmp6C2.BottomFL).tag != 1)goto _LL3A5;_tmp6C3=(int)(_tmp6C2.BottomFL).val;
_LL3A4: {const char*_tmp92A;void*_tmp929;(_tmp929=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp92A="check_fun",
_tag_dyneither(_tmp92A,sizeof(char),10))),_tag_dyneither(_tmp929,sizeof(void*),0)));}
_LL3A5: if((_tmp6C2.ReachableFL).tag != 2)goto _LL3A2;_tmp6C4=(struct _tuple12)(
_tmp6C2.ReachableFL).val;_tmp6C5=_tmp6C4.f1;_tmp6C6=_tmp6C4.f2;_LL3A6: {struct
Cyc_List_List*atts;{void*_tmp6C9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6CB;struct Cyc_List_List*
_tmp6CC;_LL3A8: {struct Cyc_Absyn_FnType_struct*_tmp6CA=(struct Cyc_Absyn_FnType_struct*)
_tmp6C9;if(_tmp6CA->tag != 10)goto _LL3AA;else{_tmp6CB=_tmp6CA->f1;_tmp6CC=_tmp6CB.attributes;}}
_LL3A9: atts=_tmp6CC;goto _LL3A7;_LL3AA:;_LL3AB: {const char*_tmp92D;void*_tmp92C;(
_tmp92C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp92D="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp92D,sizeof(char),50))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}_LL3A7:;}
for(0;atts != 0;atts=atts->tl){void*_tmp6CF=(void*)atts->hd;int _tmp6D1;int _tmp6D3;
_LL3AD: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp6D0=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp6CF;if(_tmp6D0->tag != 21)goto _LL3AF;else{_tmp6D1=_tmp6D0->f1;}}_LL3AE: {
unsigned int j=(unsigned int)_tmp6D1;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp930;
void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp930="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp930,sizeof(char),52))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp6D6=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp6D6->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp933;void*
_tmp932;(_tmp932=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp933="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp933,sizeof(char),52))),_tag_dyneither(_tmp932,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp6D9=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp936;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp935;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6DA=(_tmp935=_region_malloc(
_tmp6C0,sizeof(*_tmp935)),((_tmp935[0]=((_tmp936.tag=2,((_tmp936.f1=(int)j,((
_tmp936.f2=(void*)t,_tmp936)))))),_tmp935)));struct Cyc_CfFlowInfo_Place*_tmp937;
struct Cyc_CfFlowInfo_Place*_tmp6DB=(_tmp937=_region_malloc(_tmp6C0,sizeof(*
_tmp937)),((_tmp937->root=(void*)_tmp6DA,((_tmp937->fields=0,_tmp937)))));
_tmp6C5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6C5,(void*)_tmp6DA,_tmp6D9);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp93D;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp93C;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp93A;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp939;_tmp6C5=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6C5,(void*)((_tmp939=
_region_malloc(_tmp6C0,sizeof(*_tmp939)),((_tmp939[0]=((_tmp93A.tag=0,((_tmp93A.f1=
_tmp6D6,_tmp93A)))),_tmp939)))),(void*)((_tmp93C=_region_malloc(_tmp6C0,sizeof(*
_tmp93C)),((_tmp93C[0]=((_tmp93D.tag=5,((_tmp93D.f1=_tmp6DB,_tmp93D)))),_tmp93C)))));}
goto _LL3AC;};};}_LL3AF: {struct Cyc_Absyn_Initializes_att_struct*_tmp6D2=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp6CF;if(_tmp6D2->tag != 20)goto _LL3B1;else{
_tmp6D3=_tmp6D2->f1;}}_LL3B0: {unsigned int j=(unsigned int)_tmp6D3;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp940;void*_tmp93F;(_tmp93F=0,
Cyc_Tcutil_terr(_tmp6C1,((_tmp940="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp940,sizeof(char),51))),_tag_dyneither(_tmp93F,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp6E5=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp6E6=Cyc_Tcutil_compress(
_tmp6E5->type);struct Cyc_Absyn_PtrInfo _tmp6E8;void*_tmp6E9;struct Cyc_Absyn_PtrAtts
_tmp6EA;union Cyc_Absyn_Constraint*_tmp6EB;union Cyc_Absyn_Constraint*_tmp6EC;
union Cyc_Absyn_Constraint*_tmp6ED;_LL3B4: {struct Cyc_Absyn_PointerType_struct*
_tmp6E7=(struct Cyc_Absyn_PointerType_struct*)_tmp6E6;if(_tmp6E7->tag != 5)goto
_LL3B6;else{_tmp6E8=_tmp6E7->f1;_tmp6E9=_tmp6E8.elt_typ;_tmp6EA=_tmp6E8.ptr_atts;
_tmp6EB=_tmp6EA.nullable;_tmp6EC=_tmp6EA.bounds;_tmp6ED=_tmp6EA.zero_term;}}
_LL3B5: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6EB)){
const char*_tmp943;void*_tmp942;(_tmp942=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp943="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp943,sizeof(char),55))),_tag_dyneither(_tmp942,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp6EC)){const char*_tmp946;void*_tmp945;(_tmp945=0,
Cyc_Tcutil_terr(_tmp6C1,((_tmp946="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp946,sizeof(char),57))),_tag_dyneither(_tmp945,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6ED)){
const char*_tmp949;void*_tmp948;(_tmp948=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp949="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp949,sizeof(char),77))),_tag_dyneither(_tmp948,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp94C;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp94B;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6F4=(_tmp94B=_region_malloc(
_tmp6C0,sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94C.tag=2,((_tmp94C.f1=(int)j,((
_tmp94C.f2=(void*)_tmp6E9,_tmp94C)))))),_tmp94B)));struct Cyc_CfFlowInfo_Place*
_tmp94D;struct Cyc_CfFlowInfo_Place*_tmp6F5=(_tmp94D=_region_malloc(_tmp6C0,
sizeof(*_tmp94D)),((_tmp94D->root=(void*)_tmp6F4,((_tmp94D->fields=0,_tmp94D)))));
_tmp6C5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6C5,(void*)_tmp6F4,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6E9,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp953;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp952;struct Cyc_CfFlowInfo_VarRoot_struct _tmp950;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp94F;_tmp6C5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6C5,(void*)((_tmp94F=_region_malloc(_tmp6C0,sizeof(*_tmp94F)),((_tmp94F[0]=((
_tmp950.tag=0,((_tmp950.f1=_tmp6E5,_tmp950)))),_tmp94F)))),(void*)((_tmp952=
_region_malloc(_tmp6C0,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=5,((_tmp953.f1=
_tmp6F5,_tmp953)))),_tmp952)))));}{struct Cyc_List_List*_tmp954;param_roots=((
_tmp954=_region_malloc(_tmp6C0,sizeof(*_tmp954)),((_tmp954->hd=_tmp6F5,((_tmp954->tl=
param_roots,_tmp954))))));}goto _LL3B3;};_LL3B6:;_LL3B7: {const char*_tmp957;void*
_tmp956;(_tmp956=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp957="initializes attribute on non-pointer",
_tag_dyneither(_tmp957,sizeof(char),37))),_tag_dyneither(_tmp956,sizeof(void*),0)));}
_LL3B3:;}goto _LL3AC;};}_LL3B1:;_LL3B2: goto _LL3AC;_LL3AC:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6C5,_tmp6C6);}_LL3A2:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6C0,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp958;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp958=_region_malloc(_tmp6C0,sizeof(*_tmp958)),((
_tmp958->r=_tmp6C0,((_tmp958->fenv=fenv,((_tmp958->iterate_again=1,((_tmp958->iteration_num=
0,((_tmp958->in_try=0,((_tmp958->tryflow=inflow,((_tmp958->all_changed=0,((
_tmp958->noreturn=noreturn,((_tmp958->param_roots=param_roots,((_tmp958->flow_table=
flow_table,_tmp958)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp700=outflow;int _tmp701;_LL3B9: if((_tmp700.BottomFL).tag
!= 1)goto _LL3BB;_tmp701=(int)(_tmp700.BottomFL).val;_LL3BA: goto _LL3B8;_LL3BB:;
_LL3BC: Cyc_NewControlFlow_check_init_params(_tmp6C1,env,outflow);if(noreturn){
const char*_tmp95B;void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_terr(_tmp6C1,((_tmp95B="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp95B,sizeof(char),46))),_tag_dyneither(_tmp95A,sizeof(void*),0)));}
else{void*_tmp704=Cyc_Tcutil_compress(fd->ret_type);_LL3BE: {struct Cyc_Absyn_VoidType_struct*
_tmp705=(struct Cyc_Absyn_VoidType_struct*)_tmp704;if(_tmp705->tag != 0)goto _LL3C0;}
_LL3BF: goto _LL3BD;_LL3C0: {struct Cyc_Absyn_DoubleType_struct*_tmp706=(struct Cyc_Absyn_DoubleType_struct*)
_tmp704;if(_tmp706->tag != 8)goto _LL3C2;}_LL3C1: goto _LL3C3;_LL3C2: {struct Cyc_Absyn_FloatType_struct*
_tmp707=(struct Cyc_Absyn_FloatType_struct*)_tmp704;if(_tmp707->tag != 7)goto
_LL3C4;}_LL3C3: goto _LL3C5;_LL3C4: {struct Cyc_Absyn_IntType_struct*_tmp708=(
struct Cyc_Absyn_IntType_struct*)_tmp704;if(_tmp708->tag != 6)goto _LL3C6;}_LL3C5:{
const char*_tmp95E;void*_tmp95D;(_tmp95D=0,Cyc_Tcutil_warn(_tmp6C1,((_tmp95E="function may not return a value",
_tag_dyneither(_tmp95E,sizeof(char),32))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}
goto _LL3BD;_LL3C6:;_LL3C7:{const char*_tmp961;void*_tmp960;(_tmp960=0,Cyc_Tcutil_terr(
_tmp6C1,((_tmp961="function may not return a value",_tag_dyneither(_tmp961,
sizeof(char),32))),_tag_dyneither(_tmp960,sizeof(void*),0)));}goto _LL3BD;_LL3BD:;}
goto _LL3B8;_LL3B8:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp70E=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp710;
struct Cyc_List_List*_tmp712;struct Cyc_List_List*_tmp714;struct Cyc_List_List*
_tmp716;_LL3C9: {struct Cyc_Absyn_Fn_d_struct*_tmp70F=(struct Cyc_Absyn_Fn_d_struct*)
_tmp70E;if(_tmp70F->tag != 1)goto _LL3CB;else{_tmp710=_tmp70F->f1;}}_LL3CA: Cyc_NewControlFlow_check_fun(
_tmp710);goto _LL3C8;_LL3CB: {struct Cyc_Absyn_ExternC_d_struct*_tmp711=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp70E;if(_tmp711->tag != 12)goto _LL3CD;else{_tmp712=_tmp711->f1;}}_LL3CC:
_tmp714=_tmp712;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Using_d_struct*_tmp713=(
struct Cyc_Absyn_Using_d_struct*)_tmp70E;if(_tmp713->tag != 11)goto _LL3CF;else{
_tmp714=_tmp713->f2;}}_LL3CE: _tmp716=_tmp714;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Namespace_d_struct*
_tmp715=(struct Cyc_Absyn_Namespace_d_struct*)_tmp70E;if(_tmp715->tag != 10)goto
_LL3D1;else{_tmp716=_tmp715->f2;}}_LL3D0: Cyc_NewControlFlow_cf_check(_tmp716);
goto _LL3C8;_LL3D1: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp717=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp70E;if(_tmp717->tag != 13)goto _LL3D3;}_LL3D2: goto _LL3C8;_LL3D3:;_LL3D4: goto
_LL3C8;_LL3C8:;}}
