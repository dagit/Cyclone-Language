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
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp715;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp714;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp713;enclosee->annot=(void*)((_tmp713=_cycalloc(sizeof(*_tmp713)),((_tmp713[0]=((
_tmp715.tag=Cyc_NewControlFlow_CFAnnot,((_tmp715.f1=((_tmp714.encloser=encloser,((
_tmp714.visited=0,_tmp714)))),_tmp715)))),_tmp713))));}struct Cyc_NewControlFlow_AnalEnv{
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
_tmp71B;const char*_tmp71A;struct Cyc_Core_Impossible_struct*_tmp719;(int)_throw((
void*)((_tmp719=_cycalloc(sizeof(*_tmp719)),((_tmp719[0]=((_tmp71B.tag=Cyc_Core_Impossible,((
_tmp71B.f1=((_tmp71A="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp71A,sizeof(char),37))),_tmp71B)))),_tmp719)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp71C;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp71C=_region_malloc(env->r,sizeof(*_tmp71C)),((_tmp71C[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp71C)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp71D;return(_tmp71D.f1=_tmpB,((_tmp71D.f2=
_tmpC,_tmp71D)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp721;void*_tmp720[1];struct Cyc_String_pa_struct _tmp71F;(_tmp71F.tag=0,((
_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp720[0]=& _tmp71F,Cyc_Tcutil_warn(_tmp11,((_tmp721="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp721,sizeof(char),60))),_tag_dyneither(_tmp720,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=
Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple15 _tmp722;struct _tuple15 _tmp22=(_tmp722.f1=flow,((_tmp722.f2=_tmp20,
_tmp722)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict
_tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict
_tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(
struct _tuple12)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((
_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;
_tmp28=_tmp27.f1;_LL15: {struct _tuple17 _tmp723;struct _tuple17 _tmp29=(_tmp723.f1=
env,((_tmp723.f2=s->loc,((_tmp723.f3=_tmp28,_tmp723)))));((void(*)(void(*f)(
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
_tmp726;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp725;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp725=_region_malloc(fenv->r,sizeof(*_tmp725)),((_tmp725[0]=((_tmp726.tag=
0,((_tmp726.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp726)))),_tmp725)));_tmp2E=((
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
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp729;void*_tmp728;(_tmp728=0,Cyc_Tcutil_terr(
loc,((_tmp729="expression may not be fully initialized",_tag_dyneither(_tmp729,
sizeof(char),40))),_tag_dyneither(_tmp728,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp72A;struct
_tuple0 _tmp3D=(_tmp72A.f1=Cyc_Tcutil_compress(t),((_tmp72A.f2=r,_tmp72A)));void*
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
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp72D;void*
_tmp72C;(_tmp72C=0,Cyc_Tcutil_warn(loc,((_tmp72D="argument may still contain unique pointers",
_tag_dyneither(_tmp72D,sizeof(char),43))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp730;void*
_tmp72F;(_tmp72F=0,Cyc_Tcutil_terr(loc,((_tmp730="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp730,sizeof(char),47))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_warn(
loc,((_tmp733="argument may contain live unique pointers",_tag_dyneither(_tmp733,
sizeof(char),42))),_tag_dyneither(_tmp732,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple19 _tmp734;return(
_tmp734.f1=inflow,((_tmp734.f2=0,_tmp734)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp737;struct _tuple19 _tmp736;return(_tmp736.f1=
_tmp7C,((_tmp736.f2=((_tmp737=_region_malloc(rgn,sizeof(*_tmp737)),((_tmp737->hd=
_tmp7D,((_tmp737->tl=0,_tmp737)))))),_tmp736)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp738;env->all_changed=((
_tmp738=_region_malloc(env->r,sizeof(*_tmp738)),((_tmp738[0]=(env->fenv)->mt_place_set,
_tmp738))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp739;rvals=((_tmp739=_region_malloc(rgn,sizeof(*_tmp739)),((_tmp739->hd=
_tmp83,((_tmp739->tl=0,_tmp739))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp73A;env->all_changed=((_tmp73A=_region_malloc(env->r,sizeof(*_tmp73A)),((
_tmp73A[0]=(env->fenv)->mt_place_set,_tmp73A))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp73B;rvals=((_tmp73B=_region_malloc(rgn,
sizeof(*_tmp73B)),((_tmp73B->hd=_tmp88,((_tmp73B->tl=rvals,_tmp73B))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp73C;env->all_changed=((
_tmp73C=_region_malloc(env->r,sizeof(*_tmp73C)),((_tmp73C[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp73C))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp73D;return(_tmp73D.f1=outflow,((_tmp73D.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp73D)));};};}
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
const char*_tmp740;void*_tmp73F;(_tmp73F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp740="expression may not be initialized",
_tag_dyneither(_tmp740,sizeof(char),34))),_tag_dyneither(_tmp73F,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple13 _tmp741;return(_tmp741.f1=_tmp8E,((
_tmp741.f2=(env->fenv)->unknown_all,_tmp741)));};}static union Cyc_CfFlowInfo_FlowInfo
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
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp742;return(_tmp742.f1=outflow,((
_tmp742.f2=outflow,_tmp742)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp743;
return(_tmp743.f1=outflow,((_tmp743.f2=outflow,_tmp743)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp744;return(_tmp744.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp744.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp744)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
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
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp745;return(_tmp745.f1=
f,((_tmp745.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp745)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
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
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp748;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp747;e->annot=(void*)((_tmp747=_cycalloc(sizeof(*_tmp747)),((_tmp747[0]=((
_tmp748.tag=Cyc_CfFlowInfo_NotZero,((_tmp748.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp748)))),_tmp747))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp74B;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp74A;e->annot=(void*)((_tmp74A=_cycalloc(sizeof(*_tmp74A)),((_tmp74A[0]=((
_tmp74B.tag=Cyc_CfFlowInfo_NotZero,((_tmp74B.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp74B)))),_tmp74A))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp74C;return(_tmp74C.f1=f,((
_tmp74C.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp74C)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp74D;return(_tmp74D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp74D.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),_tmp74D)));};
_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp750;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp74F;e->annot=(void*)((_tmp74F=
_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((_tmp750.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp750.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp750)))),
_tmp74F))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp753;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp752;e->annot=(void*)((_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752[0]=((
_tmp753.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp753.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp753)))),_tmp752))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp756;void*_tmp755;(
_tmp755=0,Cyc_Tcutil_terr(e->loc,((_tmp756="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp756,sizeof(char),61))),_tag_dyneither(_tmp755,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp759;void*_tmp758;(_tmp758=0,Cyc_Tcutil_terr(
e->loc,((_tmp759="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp759,sizeof(char),46))),_tag_dyneither(_tmp758,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp75A;return(_tmp75A.f1=
f,((_tmp75A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp75A)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp75B;return(
_tmp75B.f1=f,((_tmp75B.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_none),
_tmp75B)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp761;
const char*_tmp760;struct Cyc_Core_Impossible_struct*_tmp75F;(int)_throw((void*)((
_tmp75F=_cycalloc(sizeof(*_tmp75F)),((_tmp75F[0]=((_tmp761.tag=Cyc_Core_Impossible,((
_tmp761.f1=((_tmp760="right deref of non-pointer-type",_tag_dyneither(_tmp760,
sizeof(char),32))),_tmp761)))),_tmp75F)))));}_LL58:;}static struct Cyc_List_List*
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
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp764;
struct Cyc_List_List*_tmp763;return(_tmp763=_region_malloc(rgn,sizeof(*_tmp763)),((
_tmp763->hd=((_tmp764=_region_malloc(rgn,sizeof(*_tmp764)),((_tmp764->vd=_tmp107,((
_tmp764->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp764)))))),((_tmp763->tl=
relns,_tmp763)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp765;return(_tmp765.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp765.f2=rval,_tmp765)));}_LLB4: if((_tmp10E.ReachableFL).tag
!= 2)goto _LLB1;_tmp110=(struct _tuple12)(_tmp10E.ReachableFL).val;_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp114=lval;struct Cyc_CfFlowInfo_Place*
_tmp115;int _tmp116;_LLB7: if((_tmp114.PlaceL).tag != 1)goto _LLB9;_tmp115=(struct
Cyc_CfFlowInfo_Place*)(_tmp114.PlaceL).val;_LLB8: _tmp111=Cyc_CfFlowInfo_assign_place(
fenv,loc,_tmp111,env->all_changed,_tmp115,rval);_tmp112=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp112,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp111,_tmp112);
Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13 _tmp766;return(
_tmp766.f1=outflow,((_tmp766.f2=rval,_tmp766)));};_LLB9: if((_tmp114.UnknownL).tag
!= 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {struct _tuple13 _tmp767;
return(_tmp767.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((
_tmp767.f2=rval,_tmp767)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e);static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp119=e->r;void*_tmp11B;void*_tmp11E;void*_tmp121;
struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*
_tmp128;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;_LLBC: {struct
Cyc_Absyn_Var_e_struct*_tmp11A=(struct Cyc_Absyn_Var_e_struct*)_tmp119;if(_tmp11A->tag
!= 1)goto _LLBE;else{_tmp11B=(void*)_tmp11A->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp11C=(struct Cyc_Absyn_Param_b_struct*)_tmp11B;if(_tmp11C->tag != 3)goto _LLBE;};}}
_LLBD: goto _LLBF;_LLBE: {struct Cyc_Absyn_Var_e_struct*_tmp11D=(struct Cyc_Absyn_Var_e_struct*)
_tmp119;if(_tmp11D->tag != 1)goto _LLC0;else{_tmp11E=(void*)_tmp11D->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp11F=(struct Cyc_Absyn_Local_b_struct*)_tmp11E;if(_tmp11F->tag != 4)goto _LLC0;};}}
_LLBF: goto _LLC1;_LLC0: {struct Cyc_Absyn_Var_e_struct*_tmp120=(struct Cyc_Absyn_Var_e_struct*)
_tmp119;if(_tmp120->tag != 1)goto _LLC2;else{_tmp121=(void*)_tmp120->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp122=(struct Cyc_Absyn_Pat_b_struct*)_tmp121;if(_tmp122->tag != 5)goto _LLC2;};}}
_LLC1: return 1;_LLC2: {struct Cyc_Absyn_Deref_e_struct*_tmp123=(struct Cyc_Absyn_Deref_e_struct*)
_tmp119;if(_tmp123->tag != 21)goto _LLC4;else{_tmp124=_tmp123->f1;}}_LLC3: _tmp126=
_tmp124;goto _LLC5;_LLC4: {struct Cyc_Absyn_AggrMember_e_struct*_tmp125=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp119;if(_tmp125->tag != 22)goto _LLC6;else{_tmp126=_tmp125->f1;}}_LLC5: _tmp128=
_tmp126;goto _LLC7;_LLC6: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp127=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp119;if(_tmp127->tag != 23)goto _LLC8;else{_tmp128=_tmp127->f1;}}_LLC7: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp128);_LLC8: {struct Cyc_Absyn_Subscript_e_struct*
_tmp129=(struct Cyc_Absyn_Subscript_e_struct*)_tmp119;if(_tmp129->tag != 24)goto
_LLCA;else{_tmp12A=_tmp129->f1;}}_LLC9: {void*_tmp12D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp12A->topt))->v);_LLCF: {struct Cyc_Absyn_TupleType_struct*
_tmp12E=(struct Cyc_Absyn_TupleType_struct*)_tmp12D;if(_tmp12E->tag != 11)goto
_LLD1;}_LLD0: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp12A);_LLD1:;
_LLD2: return 0;_LLCE:;}_LLCA: {struct Cyc_Absyn_Cast_e_struct*_tmp12B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp119;if(_tmp12B->tag != 15)goto _LLCC;else{_tmp12C=_tmp12B->f2;}}_LLCB: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp12C);_LLCC:;_LLCD: return 0;_LLBB:;}
struct _tuple20{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};
struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void
_tmp7F6(unsigned int*_tmp7F5,unsigned int*_tmp7F4,void***_tmp7F2,struct Cyc_List_List**
_tmp309){for(*_tmp7F5=0;*_tmp7F5 < *_tmp7F4;(*_tmp7F5)++){void*_tmp7F0;(*_tmp7F2)[*
_tmp7F5]=((_tmp7F0=(void*)((struct Cyc_List_List*)_check_null(*_tmp309))->hd,((*
_tmp309=(*_tmp309)->tl,_tmp7F0))));}}static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp12F=env->fenv;
struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp130=inflow;int _tmp131;struct _tuple12 _tmp132;struct Cyc_Dict_Dict _tmp133;
struct Cyc_List_List*_tmp134;_LLD4: if((_tmp130.BottomFL).tag != 1)goto _LLD6;
_tmp131=(int)(_tmp130.BottomFL).val;_LLD5: {struct _tuple13 _tmp768;return(_tmp768.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp768.f2=_tmp12F->unknown_all,_tmp768)));}_LLD6:
if((_tmp130.ReachableFL).tag != 2)goto _LLD3;_tmp132=(struct _tuple12)(_tmp130.ReachableFL).val;
_tmp133=_tmp132.f1;_tmp134=_tmp132.f2;_LLD7: d=_tmp133;relns=_tmp134;_LLD3:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp137;union Cyc_CfFlowInfo_AbsLVal _tmp138;struct _tuple14 _tmp136=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp137=_tmp136.f1;_tmp138=_tmp136.f2;{struct _tuple14 _tmp769;
struct _tuple14 _tmp13A=(_tmp769.f1=_tmp137,((_tmp769.f2=_tmp138,_tmp769)));union
Cyc_CfFlowInfo_FlowInfo _tmp13B;struct _tuple12 _tmp13C;struct Cyc_Dict_Dict _tmp13D;
struct Cyc_List_List*_tmp13E;union Cyc_CfFlowInfo_AbsLVal _tmp13F;struct Cyc_CfFlowInfo_Place*
_tmp140;_LLD9: _tmp13B=_tmp13A.f1;if((_tmp13B.ReachableFL).tag != 2)goto _LLDB;
_tmp13C=(struct _tuple12)(_tmp13B.ReachableFL).val;_tmp13D=_tmp13C.f1;_tmp13E=
_tmp13C.f2;_tmp13F=_tmp13A.f2;if((_tmp13F.PlaceL).tag != 1)goto _LLDB;_tmp140=(
struct Cyc_CfFlowInfo_Place*)(_tmp13F.PlaceL).val;_LLDA: {void*_tmp141=Cyc_CfFlowInfo_lookup_place(
_tmp13D,_tmp140);{void*_tmp142=_tmp141;struct Cyc_Absyn_Exp*_tmp144;int _tmp145;
void*_tmp146;_LLDE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp143=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp142;if(_tmp143->tag != 7)goto _LLE0;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;
_tmp146=(void*)_tmp143->f3;}}_LLDF: if(_tmp144 == e  && _tmp145 == env->iteration_num){
struct _tuple13 _tmp76A;return(_tmp76A.f1=_tmp137,((_tmp76A.f2=_tmp146,_tmp76A)));}{
const char*_tmp76D;void*_tmp76C;(_tmp76C=0,Cyc_Tcutil_terr(e->loc,((_tmp76D="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp76D,sizeof(char),71))),_tag_dyneither(_tmp76C,sizeof(void*),0)));}{
struct _tuple13 _tmp76E;return(_tmp76E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp76E.f2=
_tmp12F->unknown_all,_tmp76E)));};_LLE0:;_LLE1: goto _LLDD;_LLDD:;}{void*_tmp14B=
Cyc_CfFlowInfo_make_unique_consumed(_tmp12F,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,e,env->iteration_num,_tmp141);struct Cyc_Dict_Dict
_tmp14C=Cyc_CfFlowInfo_assign_place(_tmp12F,e->loc,_tmp13D,env->all_changed,
_tmp140,_tmp14B);struct _tuple13 _tmp76F;return(_tmp76F.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp14C,_tmp13E),((_tmp76F.f2=_tmp141,_tmp76F)));};}_LLDB:;_LLDC: goto _LLD8;_LLD8:;};}{
void*_tmp14E=e->r;struct Cyc_Absyn_Exp*_tmp150;enum Cyc_Absyn_Coercion _tmp151;
struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*
_tmp157;union Cyc_Absyn_Cnst _tmp159;int _tmp15A;union Cyc_Absyn_Cnst _tmp15C;struct
_tuple6 _tmp15D;int _tmp15E;union Cyc_Absyn_Cnst _tmp160;struct _tuple6 _tmp161;void*
_tmp163;struct Cyc_List_List*_tmp166;void*_tmp16E;void*_tmp171;struct Cyc_Absyn_Vardecl*
_tmp173;void*_tmp175;struct Cyc_Absyn_Vardecl*_tmp177;void*_tmp179;struct Cyc_Absyn_Vardecl*
_tmp17B;enum Cyc_Absyn_Primop _tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*
_tmp180;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Core_Opt*_tmp183;struct Cyc_Core_Opt
_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Core_Opt*
_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*
_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_List_List*
_tmp191;struct Cyc_Absyn_MallocInfo _tmp193;int _tmp194;struct Cyc_Absyn_Exp*_tmp195;
void**_tmp196;struct Cyc_Absyn_Exp*_tmp197;int _tmp198;struct Cyc_Absyn_Exp*_tmp19A;
struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*
_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*
_tmp1A4;struct _dyneither_ptr*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A7;struct
_dyneither_ptr*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct _dyneither_ptr*_tmp1AB;
struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*
_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*
_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*
_tmp1B8;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Datatypedecl*_tmp1BB;struct
Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C1;
struct Cyc_Absyn_Aggrdecl*_tmp1C2;struct Cyc_Absyn_Aggrdecl _tmp1C3;enum Cyc_Absyn_AggrKind
_tmp1C4;struct Cyc_Absyn_AggrdeclImpl*_tmp1C5;struct Cyc_Absyn_AggrdeclImpl _tmp1C6;
struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Vardecl*
_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;int _tmp1CF;
struct Cyc_Absyn_Stmt*_tmp1D1;void*_tmp1D3;_LLE3: {struct Cyc_Absyn_Cast_e_struct*
_tmp14F=(struct Cyc_Absyn_Cast_e_struct*)_tmp14E;if(_tmp14F->tag != 15)goto _LLE5;
else{_tmp150=_tmp14F->f2;_tmp151=_tmp14F->f4;if(_tmp151 != Cyc_Absyn_NonNull_to_Null)
goto _LLE5;}}_LLE4: {union Cyc_CfFlowInfo_FlowInfo _tmp1DB;void*_tmp1DC;struct
_tuple13 _tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp150);
_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1DE;void*
_tmp1DF;struct _tuple13 _tmp1DD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1DB,
_tmp150,_tmp1DC);_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;{struct _tuple13 _tmp770;
return(_tmp770.f1=_tmp1DE,((_tmp770.f2=_tmp1DC,_tmp770)));};};}_LLE5: {struct Cyc_Absyn_Cast_e_struct*
_tmp152=(struct Cyc_Absyn_Cast_e_struct*)_tmp14E;if(_tmp152->tag != 15)goto _LLE7;
else{_tmp153=_tmp152->f2;}}_LLE6: _tmp155=_tmp153;goto _LLE8;_LLE7: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp154=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp14E;if(_tmp154->tag != 13)
goto _LLE9;else{_tmp155=_tmp154->f1;}}_LLE8: _tmp157=_tmp155;goto _LLEA;_LLE9: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp156=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp14E;if(_tmp156->tag != 14)goto _LLEB;else{_tmp157=_tmp156->f1;}}_LLEA: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp157);_LLEB: {struct Cyc_Absyn_Const_e_struct*
_tmp158=(struct Cyc_Absyn_Const_e_struct*)_tmp14E;if(_tmp158->tag != 0)goto _LLED;
else{_tmp159=_tmp158->f1;if((_tmp159.Null_c).tag != 1)goto _LLED;_tmp15A=(int)(
_tmp159.Null_c).val;}}_LLEC: goto _LLEE;_LLED: {struct Cyc_Absyn_Const_e_struct*
_tmp15B=(struct Cyc_Absyn_Const_e_struct*)_tmp14E;if(_tmp15B->tag != 0)goto _LLEF;
else{_tmp15C=_tmp15B->f1;if((_tmp15C.Int_c).tag != 4)goto _LLEF;_tmp15D=(struct
_tuple6)(_tmp15C.Int_c).val;_tmp15E=_tmp15D.f2;if(_tmp15E != 0)goto _LLEF;}}_LLEE: {
struct _tuple13 _tmp771;return(_tmp771.f1=inflow,((_tmp771.f2=_tmp12F->zero,
_tmp771)));}_LLEF: {struct Cyc_Absyn_Const_e_struct*_tmp15F=(struct Cyc_Absyn_Const_e_struct*)
_tmp14E;if(_tmp15F->tag != 0)goto _LLF1;else{_tmp160=_tmp15F->f1;if((_tmp160.Int_c).tag
!= 4)goto _LLF1;_tmp161=(struct _tuple6)(_tmp160.Int_c).val;}}_LLF0: goto _LLF2;
_LLF1: {struct Cyc_Absyn_Var_e_struct*_tmp162=(struct Cyc_Absyn_Var_e_struct*)
_tmp14E;if(_tmp162->tag != 1)goto _LLF3;else{_tmp163=(void*)_tmp162->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp164=(struct Cyc_Absyn_Funname_b_struct*)_tmp163;if(_tmp164->tag != 2)goto _LLF3;};}}
_LLF2: {struct _tuple13 _tmp772;return(_tmp772.f1=inflow,((_tmp772.f2=_tmp12F->notzeroall,
_tmp772)));}_LLF3: {struct Cyc_Absyn_Datatype_e_struct*_tmp165=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp14E;if(_tmp165->tag != 31)goto _LLF5;else{_tmp166=_tmp165->f1;if(_tmp166 != 0)
goto _LLF5;}}_LLF4: goto _LLF6;_LLF5: {struct Cyc_Absyn_Const_e_struct*_tmp167=(
struct Cyc_Absyn_Const_e_struct*)_tmp14E;if(_tmp167->tag != 0)goto _LLF7;}_LLF6:
goto _LLF8;_LLF7: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp168=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp14E;if(_tmp168->tag != 19)goto _LLF9;}_LLF8: goto _LLFA;_LLF9: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp169=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp14E;if(_tmp169->tag != 18)goto
_LLFB;}_LLFA: goto _LLFC;_LLFB: {struct Cyc_Absyn_Offsetof_e_struct*_tmp16A=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp14E;if(_tmp16A->tag != 20)goto _LLFD;}_LLFC: goto
_LLFE;_LLFD: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp16B=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp14E;if(_tmp16B->tag != 33)goto _LLFF;}_LLFE: goto _LL100;_LLFF: {struct Cyc_Absyn_Enum_e_struct*
_tmp16C=(struct Cyc_Absyn_Enum_e_struct*)_tmp14E;if(_tmp16C->tag != 32)goto _LL101;}
_LL100: {struct _tuple13 _tmp773;return(_tmp773.f1=inflow,((_tmp773.f2=_tmp12F->unknown_all,
_tmp773)));}_LL101: {struct Cyc_Absyn_Var_e_struct*_tmp16D=(struct Cyc_Absyn_Var_e_struct*)
_tmp14E;if(_tmp16D->tag != 1)goto _LL103;else{_tmp16E=(void*)_tmp16D->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp16F=(struct Cyc_Absyn_Global_b_struct*)_tmp16E;if(
_tmp16F->tag != 1)goto _LL103;};}}_LL102: {struct _tuple13 _tmp774;return(_tmp774.f1=
inflow,((_tmp774.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp12F,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp12F->unknown_all),_tmp774)));}_LL103: {struct Cyc_Absyn_Var_e_struct*
_tmp170=(struct Cyc_Absyn_Var_e_struct*)_tmp14E;if(_tmp170->tag != 1)goto _LL105;
else{_tmp171=(void*)_tmp170->f2;{struct Cyc_Absyn_Param_b_struct*_tmp172=(struct
Cyc_Absyn_Param_b_struct*)_tmp171;if(_tmp172->tag != 3)goto _LL105;else{_tmp173=
_tmp172->f1;}};}}_LL104: _tmp177=_tmp173;goto _LL106;_LL105: {struct Cyc_Absyn_Var_e_struct*
_tmp174=(struct Cyc_Absyn_Var_e_struct*)_tmp14E;if(_tmp174->tag != 1)goto _LL107;
else{_tmp175=(void*)_tmp174->f2;{struct Cyc_Absyn_Local_b_struct*_tmp176=(struct
Cyc_Absyn_Local_b_struct*)_tmp175;if(_tmp176->tag != 4)goto _LL107;else{_tmp177=
_tmp176->f1;}};}}_LL106: _tmp17B=_tmp177;goto _LL108;_LL107: {struct Cyc_Absyn_Var_e_struct*
_tmp178=(struct Cyc_Absyn_Var_e_struct*)_tmp14E;if(_tmp178->tag != 1)goto _LL109;
else{_tmp179=(void*)_tmp178->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp17A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp179;if(_tmp17A->tag != 5)goto _LL109;else{_tmp17B=_tmp17A->f1;}};}}_LL108: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp77A;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp779;struct _tuple13 _tmp778;return(_tmp778.f1=inflow,((_tmp778.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp77A=_region_malloc(
env->r,sizeof(*_tmp77A)),((_tmp77A[0]=((_tmp779.tag=0,((_tmp779.f1=_tmp17B,
_tmp779)))),_tmp77A))))),_tmp778)));}_LL109: {struct Cyc_Absyn_Primop_e_struct*
_tmp17C=(struct Cyc_Absyn_Primop_e_struct*)_tmp14E;if(_tmp17C->tag != 3)goto _LL10B;
else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LL10A: {union Cyc_CfFlowInfo_FlowInfo
_tmp1E9;void*_tmp1EA;struct _tuple13 _tmp1E8=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp17E);_tmp1E9=_tmp1E8.f1;_tmp1EA=_tmp1E8.f2;{struct _tuple13 _tmp77B;
return(_tmp77B.f1=_tmp1E9,((_tmp77B.f2=_tmp1EA,_tmp77B)));};}_LL10B: {struct Cyc_Absyn_Increment_e_struct*
_tmp17F=(struct Cyc_Absyn_Increment_e_struct*)_tmp14E;if(_tmp17F->tag != 5)goto
_LL10D;else{_tmp180=_tmp17F->f1;}}_LL10C: {struct Cyc_List_List _tmp77C;struct Cyc_List_List
_tmp1EC=(_tmp77C.hd=_tmp180,((_tmp77C.tl=0,_tmp77C)));union Cyc_CfFlowInfo_FlowInfo
_tmp1EE;struct _tuple13 _tmp1ED=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp1EC);_tmp1EE=_tmp1ED.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp1F0;struct _tuple14 _tmp1EF=Cyc_NewControlFlow_anal_Lexp(env,_tmp1EE,0,_tmp180);
_tmp1F0=_tmp1EF.f2;{struct _tuple20 _tmp77D;struct _tuple20 _tmp1F2=(_tmp77D.f1=
_tmp1F0,((_tmp77D.f2=_tmp1EE,_tmp77D)));union Cyc_CfFlowInfo_AbsLVal _tmp1F3;
struct Cyc_CfFlowInfo_Place*_tmp1F4;union Cyc_CfFlowInfo_FlowInfo _tmp1F5;struct
_tuple12 _tmp1F6;struct Cyc_Dict_Dict _tmp1F7;struct Cyc_List_List*_tmp1F8;_LL14C:
_tmp1F3=_tmp1F2.f1;if((_tmp1F3.PlaceL).tag != 1)goto _LL14E;_tmp1F4=(struct Cyc_CfFlowInfo_Place*)(
_tmp1F3.PlaceL).val;_tmp1F5=_tmp1F2.f2;if((_tmp1F5.ReachableFL).tag != 2)goto
_LL14E;_tmp1F6=(struct _tuple12)(_tmp1F5.ReachableFL).val;_tmp1F7=_tmp1F6.f1;
_tmp1F8=_tmp1F6.f2;_LL14D: _tmp1F8=Cyc_CfFlowInfo_reln_kill_exp(_tmp12F->r,
_tmp1F8,_tmp180);_tmp1EE=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp12F,_tmp180->loc,_tmp1F7,env->all_changed,_tmp1F4,_tmp12F->unknown_all),
_tmp1F8);goto _LL14B;_LL14E:;_LL14F: goto _LL14B;_LL14B:;}{struct _tuple13 _tmp77E;
return(_tmp77E.f1=_tmp1EE,((_tmp77E.f2=_tmp12F->unknown_all,_tmp77E)));};};}
_LL10D: {struct Cyc_Absyn_AssignOp_e_struct*_tmp181=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp14E;if(_tmp181->tag != 4)goto _LL10F;else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;
if(_tmp183 == 0)goto _LL10F;_tmp184=*_tmp183;_tmp185=_tmp181->f3;}}_LL10E: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp781;void*_tmp780;(_tmp780=0,Cyc_Tcutil_terr(
e->loc,((_tmp781="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp781,sizeof(char),58))),_tag_dyneither(_tmp780,sizeof(void*),0)));}{
struct _tuple13 _tmp782;return(_tmp782.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp782.f2=
_tmp12F->unknown_all,_tmp782)));};}{struct Cyc_List_List _tmp783;struct Cyc_List_List
_tmp1FE=(_tmp783.hd=_tmp185,((_tmp783.tl=0,_tmp783)));struct Cyc_List_List _tmp784;
struct Cyc_List_List _tmp1FF=(_tmp784.hd=_tmp182,((_tmp784.tl=(struct Cyc_List_List*)&
_tmp1FE,_tmp784)));union Cyc_CfFlowInfo_FlowInfo _tmp201;struct _tuple13 _tmp200=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1FF);_tmp201=_tmp200.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp203;struct _tuple14 _tmp202=Cyc_NewControlFlow_anal_Lexp(env,_tmp201,0,_tmp182);
_tmp203=_tmp202.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp204=_tmp201;struct _tuple12
_tmp205;struct Cyc_Dict_Dict _tmp206;struct Cyc_List_List*_tmp207;_LL151: if((
_tmp204.ReachableFL).tag != 2)goto _LL153;_tmp205=(struct _tuple12)(_tmp204.ReachableFL).val;
_tmp206=_tmp205.f1;_tmp207=_tmp205.f2;_LL152:{union Cyc_CfFlowInfo_AbsLVal _tmp208=
_tmp203;struct Cyc_CfFlowInfo_Place*_tmp209;int _tmp20A;_LL156: if((_tmp208.PlaceL).tag
!= 1)goto _LL158;_tmp209=(struct Cyc_CfFlowInfo_Place*)(_tmp208.PlaceL).val;_LL157:
_tmp207=Cyc_CfFlowInfo_reln_kill_exp(_tmp12F->r,_tmp207,_tmp182);_tmp206=Cyc_CfFlowInfo_assign_place(
_tmp12F,_tmp182->loc,_tmp206,env->all_changed,_tmp209,_tmp12F->unknown_all);
_tmp201=Cyc_CfFlowInfo_ReachableFL(_tmp206,_tmp207);goto _LL155;_LL158: if((
_tmp208.UnknownL).tag != 2)goto _LL155;_tmp20A=(int)(_tmp208.UnknownL).val;_LL159:
goto _LL155;_LL155:;}goto _LL150;_LL153:;_LL154: goto _LL150;_LL150:;}{struct
_tuple13 _tmp785;return(_tmp785.f1=_tmp201,((_tmp785.f2=_tmp12F->unknown_all,
_tmp785)));};};};_LL10F: {struct Cyc_Absyn_AssignOp_e_struct*_tmp186=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp14E;if(_tmp186->tag != 4)goto _LL111;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;
if(_tmp188 != 0)goto _LL111;_tmp189=_tmp186->f3;}}_LL110: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp788;void*_tmp787;(_tmp787=0,Cyc_Tcutil_terr(e->loc,((_tmp788="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp788,sizeof(char),58))),_tag_dyneither(_tmp787,sizeof(void*),0)));}{
struct _tuple13 _tmp789;return(_tmp789.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp789.f2=
_tmp12F->unknown_all,_tmp789)));};}{struct Cyc_Dict_Dict*_tmp211=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp78A;env->all_changed=((_tmp78A=_region_malloc(
env->r,sizeof(*_tmp78A)),((_tmp78A[0]=_tmp12F->mt_place_set,_tmp78A))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp214;union Cyc_CfFlowInfo_AbsLVal _tmp215;struct _tuple14
_tmp213=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp187);_tmp214=_tmp213.f1;
_tmp215=_tmp213.f2;{struct Cyc_Dict_Dict _tmp216=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp78B;env->all_changed=((
_tmp78B=_region_malloc(env->r,sizeof(*_tmp78B)),((_tmp78B[0]=_tmp12F->mt_place_set,
_tmp78B))));}{union Cyc_CfFlowInfo_FlowInfo _tmp219;void*_tmp21A;struct _tuple13
_tmp218=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp189);_tmp219=_tmp218.f1;
_tmp21A=_tmp218.f2;{struct Cyc_Dict_Dict _tmp21B=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp21C=Cyc_CfFlowInfo_after_flow(
_tmp12F,(struct Cyc_Dict_Dict*)& _tmp216,_tmp214,_tmp219,_tmp216,_tmp21B);union Cyc_CfFlowInfo_FlowInfo
_tmp21D=Cyc_CfFlowInfo_join_flow(_tmp12F,_tmp211,inflow,_tmp21C);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp21D,inflow)){if(_tmp211 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp211,Cyc_CfFlowInfo_union_place_set(
_tmp216,_tmp21B,0),0);}return Cyc_NewControlFlow_do_assign(_tmp12F,env,_tmp21C,
_tmp187,_tmp215,_tmp189,_tmp21A,e->loc);}inflow=_tmp21D;};};};};}};_LL111: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp18A=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp14E;if(_tmp18A->tag != 9)goto _LL113;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}
_LL112: {union Cyc_CfFlowInfo_FlowInfo _tmp21F;void*_tmp220;struct _tuple13 _tmp21E=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp18B);_tmp21F=_tmp21E.f1;_tmp220=
_tmp21E.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp21F,_tmp18C);}
_LL113: {struct Cyc_Absyn_Throw_e_struct*_tmp18D=(struct Cyc_Absyn_Throw_e_struct*)
_tmp14E;if(_tmp18D->tag != 12)goto _LL115;else{_tmp18E=_tmp18D->f1;}}_LL114: {
union Cyc_CfFlowInfo_FlowInfo _tmp222;void*_tmp223;struct _tuple13 _tmp221=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp18E);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp18E->loc,_tmp222,_tmp223);{struct _tuple13 _tmp78C;return(_tmp78C.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp78C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp12F,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp12F->unknown_all),_tmp78C)));};}_LL115: {struct Cyc_Absyn_FnCall_e_struct*
_tmp18F=(struct Cyc_Absyn_FnCall_e_struct*)_tmp14E;if(_tmp18F->tag != 11)goto
_LL117;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LL116: {struct
_RegionHandle*_tmp225=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp228;struct Cyc_List_List*
_tmp229;struct Cyc_List_List*_tmp78D;struct _tuple19 _tmp227=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp225,env,inflow,((_tmp78D=_region_malloc(_tmp225,sizeof(*_tmp78D)),((_tmp78D->hd=
_tmp190,((_tmp78D->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp225,_tmp191),_tmp78D)))))),0,1);_tmp228=_tmp227.f1;
_tmp229=_tmp227.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp22A=Cyc_NewControlFlow_use_Rval(
env,_tmp190->loc,_tmp228,(void*)((struct Cyc_List_List*)_check_null(_tmp229))->hd);
_tmp229=_tmp229->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp22B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp190->topt))->v);struct Cyc_Absyn_PtrInfo _tmp22D;void*_tmp22E;
_LL15B: {struct Cyc_Absyn_PointerType_struct*_tmp22C=(struct Cyc_Absyn_PointerType_struct*)
_tmp22B;if(_tmp22C->tag != 5)goto _LL15D;else{_tmp22D=_tmp22C->f1;_tmp22E=_tmp22D.elt_typ;}}
_LL15C:{void*_tmp22F=Cyc_Tcutil_compress(_tmp22E);struct Cyc_Absyn_FnInfo _tmp231;
struct Cyc_List_List*_tmp232;_LL160: {struct Cyc_Absyn_FnType_struct*_tmp230=(
struct Cyc_Absyn_FnType_struct*)_tmp22F;if(_tmp230->tag != 10)goto _LL162;else{
_tmp231=_tmp230->f1;_tmp232=_tmp231.attributes;}}_LL161: for(0;_tmp232 != 0;
_tmp232=_tmp232->tl){void*_tmp233=(void*)_tmp232->hd;int _tmp235;int _tmp237;
_LL165: {struct Cyc_Absyn_Initializes_att_struct*_tmp234=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp233;if(_tmp234->tag != 20)goto _LL167;else{_tmp235=_tmp234->f1;}}_LL166:{
struct Cyc_List_List*_tmp78E;init_params=((_tmp78E=_region_malloc(_tmp225,sizeof(*
_tmp78E)),((_tmp78E->hd=(void*)_tmp235,((_tmp78E->tl=init_params,_tmp78E))))));}
goto _LL164;_LL167: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp236=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp233;if(_tmp236->tag != 21)goto _LL169;else{_tmp237=_tmp236->f1;}}_LL168:{
struct Cyc_List_List*_tmp78F;nolive_unique_params=((_tmp78F=_region_malloc(
_tmp225,sizeof(*_tmp78F)),((_tmp78F->hd=(void*)_tmp237,((_tmp78F->tl=
nolive_unique_params,_tmp78F))))));}goto _LL164;_LL169:;_LL16A: goto _LL164;_LL164:;}
goto _LL15F;_LL162:;_LL163: {const char*_tmp792;void*_tmp791;(_tmp791=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp792="anal_Rexp: bad function type",
_tag_dyneither(_tmp792,sizeof(char),29))),_tag_dyneither(_tmp791,sizeof(void*),0)));}
_LL15F:;}goto _LL15A;_LL15D:;_LL15E: {const char*_tmp795;void*_tmp794;(_tmp794=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp795="anal_Rexp: bad function type",_tag_dyneither(_tmp795,sizeof(char),29))),
_tag_dyneither(_tmp794,sizeof(void*),0)));}_LL15A:;}{int i=1;for(0;_tmp229 != 0;(((
_tmp229=_tmp229->tl,_tmp191=((struct Cyc_List_List*)_check_null(_tmp191))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp22A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp191))->hd)->loc,_tmp22A,(void*)_tmp229->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp22A=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp191))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp191))->hd)->topt))->v,_tmp22A,(void*)_tmp229->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp23E=_tmp228;int _tmp23F;struct _tuple12 _tmp240;
struct Cyc_Dict_Dict _tmp241;_LL16C: if((_tmp23E.BottomFL).tag != 1)goto _LL16E;
_tmp23F=(int)(_tmp23E.BottomFL).val;_LL16D: goto _LL16B;_LL16E: if((_tmp23E.ReachableFL).tag
!= 2)goto _LL16B;_tmp240=(struct _tuple12)(_tmp23E.ReachableFL).val;_tmp241=
_tmp240.f1;_LL16F: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp241,(void*)_tmp229->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp191))->hd)->loc,((
_tmp798="expression may not be initialized",_tag_dyneither(_tmp798,sizeof(char),
34))),_tag_dyneither(_tmp797,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp244=_tmp22A;int _tmp245;struct _tuple12 _tmp246;struct Cyc_Dict_Dict _tmp247;
struct Cyc_List_List*_tmp248;_LL171: if((_tmp244.BottomFL).tag != 1)goto _LL173;
_tmp245=(int)(_tmp244.BottomFL).val;_LL172: goto _LL170;_LL173: if((_tmp244.ReachableFL).tag
!= 2)goto _LL170;_tmp246=(struct _tuple12)(_tmp244.ReachableFL).val;_tmp247=
_tmp246.f1;_tmp248=_tmp246.f2;_LL174: {struct Cyc_Dict_Dict _tmp249=Cyc_CfFlowInfo_escape_deref(
_tmp12F,_tmp247,env->all_changed,(void*)_tmp229->hd);{void*_tmp24A=(void*)
_tmp229->hd;struct Cyc_CfFlowInfo_Place*_tmp24C;_LL176: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp24B=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp24A;if(_tmp24B->tag != 5)goto
_LL178;else{_tmp24C=_tmp24B->f1;}}_LL177:{void*_tmp24D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp191))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp24F;void*
_tmp250;_LL17B: {struct Cyc_Absyn_PointerType_struct*_tmp24E=(struct Cyc_Absyn_PointerType_struct*)
_tmp24D;if(_tmp24E->tag != 5)goto _LL17D;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24F.elt_typ;}}
_LL17C: _tmp249=Cyc_CfFlowInfo_assign_place(_tmp12F,((struct Cyc_Absyn_Exp*)
_tmp191->hd)->loc,_tmp249,env->all_changed,_tmp24C,Cyc_CfFlowInfo_typ_to_absrval(
_tmp12F,_tmp250,_tmp12F->esc_all));goto _LL17A;_LL17D:;_LL17E: {const char*_tmp79B;
void*_tmp79A;(_tmp79A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp79B="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp79B,sizeof(char),39))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}
_LL17A:;}goto _LL175;_LL178:;_LL179: goto _LL175;_LL175:;}_tmp22A=Cyc_CfFlowInfo_ReachableFL(
_tmp249,_tmp248);goto _LL170;}_LL170:;}goto _LL16B;_LL16B:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp190->topt))->v)){struct _tuple13
_tmp79C;return(_tmp79C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp79C.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp12F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp12F->unknown_all),
_tmp79C)));}else{struct _tuple13 _tmp79D;return(_tmp79D.f1=_tmp22A,((_tmp79D.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp12F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp12F->unknown_all),_tmp79D)));}};};}_LL117: {struct Cyc_Absyn_Malloc_e_struct*
_tmp192=(struct Cyc_Absyn_Malloc_e_struct*)_tmp14E;if(_tmp192->tag != 34)goto
_LL119;else{_tmp193=_tmp192->f1;_tmp194=_tmp193.is_calloc;_tmp195=_tmp193.rgn;
_tmp196=_tmp193.elt_type;_tmp197=_tmp193.num_elts;_tmp198=_tmp193.fat_result;}}
_LL118: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7A0;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp79F;void*root=(void*)((_tmp79F=_region_malloc(_tmp12F->r,sizeof(*_tmp79F)),((
_tmp79F[0]=((_tmp7A0.tag=1,((_tmp7A0.f1=_tmp197,((_tmp7A0.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7A0)))))),_tmp79F))));struct
Cyc_CfFlowInfo_Place*_tmp7A1;struct Cyc_CfFlowInfo_Place*place=(_tmp7A1=
_region_malloc(_tmp12F->r,sizeof(*_tmp7A1)),((_tmp7A1->root=root,((_tmp7A1->fields=
0,_tmp7A1)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7A4;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7A3;void*rval=(void*)((_tmp7A3=_region_malloc(_tmp12F->r,sizeof(*_tmp7A3)),((
_tmp7A3[0]=((_tmp7A4.tag=5,((_tmp7A4.f1=place,_tmp7A4)))),_tmp7A3))));void*
place_val=_tmp198?_tmp12F->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp12F,*((
void**)_check_null(_tmp196)),_tmp12F->unknown_none);union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp195 != 0){struct _RegionHandle*_tmp255=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Exp*_tmp7A5[2];struct
_tuple19 _tmp257=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp255,env,inflow,((
_tmp7A5[1]=_tmp197,((_tmp7A5[0]=(struct Cyc_Absyn_Exp*)_tmp195,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp255,
_tag_dyneither(_tmp7A5,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp258=_tmp257.f1;
_tmp259=_tmp257.f2;outflow=_tmp258;}else{union Cyc_CfFlowInfo_FlowInfo _tmp25B;
struct _tuple13 _tmp25A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp197);_tmp25B=
_tmp25A.f1;outflow=_tmp25B;}{union Cyc_CfFlowInfo_FlowInfo _tmp25C=outflow;int
_tmp25D;struct _tuple12 _tmp25E;struct Cyc_Dict_Dict _tmp25F;struct Cyc_List_List*
_tmp260;_LL180: if((_tmp25C.BottomFL).tag != 1)goto _LL182;_tmp25D=(int)(_tmp25C.BottomFL).val;
_LL181: {struct _tuple13 _tmp7A6;return(_tmp7A6.f1=outflow,((_tmp7A6.f2=rval,
_tmp7A6)));}_LL182: if((_tmp25C.ReachableFL).tag != 2)goto _LL17F;_tmp25E=(struct
_tuple12)(_tmp25C.ReachableFL).val;_tmp25F=_tmp25E.f1;_tmp260=_tmp25E.f2;_LL183: {
struct _tuple13 _tmp7A7;return(_tmp7A7.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25F,root,place_val),
_tmp260),((_tmp7A7.f2=rval,_tmp7A7)));}_LL17F:;};}_LL119: {struct Cyc_Absyn_Swap_e_struct*
_tmp199=(struct Cyc_Absyn_Swap_e_struct*)_tmp14E;if(_tmp199->tag != 35)goto _LL11B;
else{_tmp19A=_tmp199->f1;_tmp19B=_tmp199->f2;}}_LL11A: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp268=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*
_tmp7A8[2];struct _tuple19 _tmp26A=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp268,
env,inflow,((_tmp7A8[1]=_tmp19B,((_tmp7A8[0]=_tmp19A,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp268,
_tag_dyneither(_tmp7A8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp26B=_tmp26A.f1;
_tmp26C=_tmp26A.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C->tl))->hd;outflow=
_tmp26B;}{union Cyc_CfFlowInfo_AbsLVal _tmp26E;struct _tuple14 _tmp26D=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp19A);_tmp26E=_tmp26D.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp270;
struct _tuple14 _tmp26F=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19B);
_tmp270=_tmp26F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp271=outflow;struct _tuple12
_tmp272;struct Cyc_Dict_Dict _tmp273;struct Cyc_List_List*_tmp274;_LL185: if((
_tmp271.ReachableFL).tag != 2)goto _LL187;_tmp272=(struct _tuple12)(_tmp271.ReachableFL).val;
_tmp273=_tmp272.f1;_tmp274=_tmp272.f2;_LL186:{union Cyc_CfFlowInfo_AbsLVal _tmp275=
_tmp26E;struct Cyc_CfFlowInfo_Place*_tmp276;int _tmp277;_LL18A: if((_tmp275.PlaceL).tag
!= 1)goto _LL18C;_tmp276=(struct Cyc_CfFlowInfo_Place*)(_tmp275.PlaceL).val;_LL18B:
_tmp273=Cyc_CfFlowInfo_assign_place(_tmp12F,_tmp19A->loc,_tmp273,env->all_changed,
_tmp276,right_rval);goto _LL189;_LL18C: if((_tmp275.UnknownL).tag != 2)goto _LL189;
_tmp277=(int)(_tmp275.UnknownL).val;_LL18D: goto _LL189;_LL189:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp278=_tmp270;struct Cyc_CfFlowInfo_Place*_tmp279;int _tmp27A;_LL18F: if((_tmp278.PlaceL).tag
!= 1)goto _LL191;_tmp279=(struct Cyc_CfFlowInfo_Place*)(_tmp278.PlaceL).val;_LL190:
_tmp273=Cyc_CfFlowInfo_assign_place(_tmp12F,_tmp19B->loc,_tmp273,env->all_changed,
_tmp279,left_rval);goto _LL18E;_LL191: if((_tmp278.UnknownL).tag != 2)goto _LL18E;
_tmp27A=(int)(_tmp278.UnknownL).val;_LL192: goto _LL18E;_LL18E:;}_tmp274=Cyc_CfFlowInfo_reln_kill_exp(
_tmp12F->r,_tmp274,_tmp19A);_tmp274=Cyc_CfFlowInfo_reln_kill_exp(_tmp12F->r,
_tmp274,_tmp19B);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp273,_tmp274);goto _LL184;
_LL187:;_LL188: goto _LL184;_LL184:;}{struct _tuple13 _tmp7A9;return(_tmp7A9.f1=
outflow,((_tmp7A9.f2=_tmp12F->unknown_all,_tmp7A9)));};};};}_LL11B: {struct Cyc_Absyn_New_e_struct*
_tmp19C=(struct Cyc_Absyn_New_e_struct*)_tmp14E;if(_tmp19C->tag != 17)goto _LL11D;
else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}_LL11C: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp7AC;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp7AB;void*root=(void*)((_tmp7AB=
_region_malloc(_tmp12F->r,sizeof(*_tmp7AB)),((_tmp7AB[0]=((_tmp7AC.tag=1,((
_tmp7AC.f1=_tmp19E,((_tmp7AC.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp7AC)))))),_tmp7AB))));struct Cyc_CfFlowInfo_Place*_tmp7AD;
struct Cyc_CfFlowInfo_Place*place=(_tmp7AD=_region_malloc(_tmp12F->r,sizeof(*
_tmp7AD)),((_tmp7AD->root=root,((_tmp7AD->fields=0,_tmp7AD)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp7B0;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7AF;void*rval=(void*)((_tmp7AF=
_region_malloc(_tmp12F->r,sizeof(*_tmp7AF)),((_tmp7AF[0]=((_tmp7B0.tag=5,((
_tmp7B0.f1=place,_tmp7B0)))),_tmp7AF))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp19D != 0){struct _RegionHandle*_tmp27C=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_Absyn_Exp*_tmp7B1[2];struct
_tuple19 _tmp27E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp27C,env,inflow,((
_tmp7B1[1]=_tmp19E,((_tmp7B1[0]=(struct Cyc_Absyn_Exp*)_tmp19D,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp27C,
_tag_dyneither(_tmp7B1,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp27F=_tmp27E.f1;
_tmp280=_tmp27E.f2;outflow=_tmp27F;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp280))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp282;void*_tmp283;struct _tuple13 _tmp281=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp19E);_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;outflow=_tmp282;
place_val=_tmp283;}{union Cyc_CfFlowInfo_FlowInfo _tmp284=outflow;int _tmp285;
struct _tuple12 _tmp286;struct Cyc_Dict_Dict _tmp287;struct Cyc_List_List*_tmp288;
_LL194: if((_tmp284.BottomFL).tag != 1)goto _LL196;_tmp285=(int)(_tmp284.BottomFL).val;
_LL195: {struct _tuple13 _tmp7B2;return(_tmp7B2.f1=outflow,((_tmp7B2.f2=rval,
_tmp7B2)));}_LL196: if((_tmp284.ReachableFL).tag != 2)goto _LL193;_tmp286=(struct
_tuple12)(_tmp284.ReachableFL).val;_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;_LL197: {
struct _tuple13 _tmp7B3;return(_tmp7B3.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp287,root,place_val),
_tmp288),((_tmp7B3.f2=rval,_tmp7B3)));}_LL193:;};};}_LL11D: {struct Cyc_Absyn_Address_e_struct*
_tmp19F=(struct Cyc_Absyn_Address_e_struct*)_tmp14E;if(_tmp19F->tag != 16)goto
_LL11F;else{_tmp1A0=_tmp19F->f1;}}_LL11E: {union Cyc_CfFlowInfo_FlowInfo _tmp291;
union Cyc_CfFlowInfo_AbsLVal _tmp292;struct _tuple14 _tmp290=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1A0);_tmp291=_tmp290.f1;_tmp292=_tmp290.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp293=_tmp292;int _tmp294;struct Cyc_CfFlowInfo_Place*_tmp295;_LL199: if((_tmp293.UnknownL).tag
!= 2)goto _LL19B;_tmp294=(int)(_tmp293.UnknownL).val;_LL19A: {struct _tuple13
_tmp7B4;return(_tmp7B4.f1=_tmp291,((_tmp7B4.f2=_tmp12F->notzeroall,_tmp7B4)));}
_LL19B: if((_tmp293.PlaceL).tag != 1)goto _LL198;_tmp295=(struct Cyc_CfFlowInfo_Place*)(
_tmp293.PlaceL).val;_LL19C: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7BA;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp7B9;struct _tuple13 _tmp7B8;return(
_tmp7B8.f1=_tmp291,((_tmp7B8.f2=(void*)((_tmp7BA=_region_malloc(env->r,sizeof(*
_tmp7BA)),((_tmp7BA[0]=((_tmp7B9.tag=5,((_tmp7B9.f1=_tmp295,_tmp7B9)))),_tmp7BA)))),
_tmp7B8)));}_LL198:;};}_LL11F: {struct Cyc_Absyn_Deref_e_struct*_tmp1A1=(struct
Cyc_Absyn_Deref_e_struct*)_tmp14E;if(_tmp1A1->tag != 21)goto _LL121;else{_tmp1A2=
_tmp1A1->f1;}}_LL120: {union Cyc_CfFlowInfo_FlowInfo _tmp29B;void*_tmp29C;struct
_tuple13 _tmp29A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A2);_tmp29B=
_tmp29A.f1;_tmp29C=_tmp29A.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp29B,_tmp1A2,_tmp29C);}_LL121: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1A3=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp14E;if(_tmp1A3->tag != 22)goto _LL123;
else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;}}_LL122: {union Cyc_CfFlowInfo_FlowInfo
_tmp29E;void*_tmp29F;struct _tuple13 _tmp29D=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1A4);_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1A4->topt))->v)){struct _tuple13
_tmp7BB;return(_tmp7BB.f1=_tmp29E,((_tmp7BB.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp12F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp12F->unknown_all),
_tmp7BB)));}{void*_tmp2A1=_tmp29F;int _tmp2A3;struct _dyneither_ptr _tmp2A4;_LL19E: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2A2=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2A1;if(_tmp2A2->tag != 6)goto _LL1A0;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;}}
_LL19F: {int _tmp2A5=Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A4->topt))->v,_tmp1A5);struct _tuple13 _tmp7BC;return(_tmp7BC.f1=
_tmp29E,((_tmp7BC.f2=*((void**)_check_dyneither_subscript(_tmp2A4,sizeof(void*),
_tmp2A5)),_tmp7BC)));}_LL1A0:;_LL1A1: {struct Cyc_Core_Impossible_struct _tmp7C9;
const char*_tmp7C8;void*_tmp7C7[1];struct Cyc_String_pa_struct _tmp7C6;struct Cyc_Core_Impossible_struct*
_tmp7C5;(int)_throw((void*)((_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5[0]=((
_tmp7C9.tag=Cyc_Core_Impossible,((_tmp7C9.f1=(struct _dyneither_ptr)((_tmp7C6.tag=
0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp7C7[0]=& _tmp7C6,Cyc_aprintf(((_tmp7C8="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp7C8,sizeof(char),26))),_tag_dyneither(_tmp7C7,sizeof(void*),1)))))))),
_tmp7C9)))),_tmp7C5)))));}_LL19D:;};}_LL123: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1A6=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp14E;if(_tmp1A6->tag != 38)goto
_LL125;else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A6->f2;}}_LL124: {union Cyc_CfFlowInfo_FlowInfo
_tmp2AD;void*_tmp2AE;struct _tuple13 _tmp2AC=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1A7);_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;{struct _tuple13 _tmp7CA;
return(_tmp7CA.f1=_tmp2AD,((_tmp7CA.f2=_tmp12F->unknown_all,_tmp7CA)));};}_LL125: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp1A9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp14E;if(_tmp1A9->tag != 23)goto _LL127;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}
_LL126: {union Cyc_CfFlowInfo_FlowInfo _tmp2B1;void*_tmp2B2;struct _tuple13 _tmp2B0=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AA);_tmp2B1=_tmp2B0.f1;_tmp2B2=
_tmp2B0.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B4;void*_tmp2B5;struct _tuple13
_tmp2B3=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2B1,_tmp1AA,_tmp2B2);
_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;{void*_tmp2B6=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1AA->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2B8;void*_tmp2B9;_LL1A3: {struct Cyc_Absyn_PointerType_struct*_tmp2B7=(struct
Cyc_Absyn_PointerType_struct*)_tmp2B6;if(_tmp2B7->tag != 5)goto _LL1A5;else{
_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B8.elt_typ;}}_LL1A4: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2B9)){struct _tuple13 _tmp7CB;return(_tmp7CB.f1=_tmp2B4,((_tmp7CB.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp12F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp12F->unknown_all),
_tmp7CB)));}{void*_tmp2BB=_tmp2B5;int _tmp2BD;struct _dyneither_ptr _tmp2BE;_LL1A8: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2BC=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2BB;if(_tmp2BC->tag != 6)goto _LL1AA;else{_tmp2BD=_tmp2BC->f1;_tmp2BE=_tmp2BC->f2;}}
_LL1A9: {int _tmp2BF=Cyc_CfFlowInfo_get_field_index(_tmp2B9,_tmp1AB);struct
_tuple13 _tmp7CC;return(_tmp7CC.f1=_tmp2B4,((_tmp7CC.f2=*((void**)
_check_dyneither_subscript(_tmp2BE,sizeof(void*),_tmp2BF)),_tmp7CC)));}_LL1AA:;
_LL1AB: {struct Cyc_Core_Impossible_struct _tmp7D2;const char*_tmp7D1;struct Cyc_Core_Impossible_struct*
_tmp7D0;(int)_throw((void*)((_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0[0]=((
_tmp7D2.tag=Cyc_Core_Impossible,((_tmp7D2.f1=((_tmp7D1="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp7D1,sizeof(char),21))),_tmp7D2)))),_tmp7D0)))));}_LL1A7:;};
_LL1A5:;_LL1A6: {struct Cyc_Core_Impossible_struct _tmp7D8;const char*_tmp7D7;
struct Cyc_Core_Impossible_struct*_tmp7D6;(int)_throw((void*)((_tmp7D6=_cycalloc(
sizeof(*_tmp7D6)),((_tmp7D6[0]=((_tmp7D8.tag=Cyc_Core_Impossible,((_tmp7D8.f1=((
_tmp7D7="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7D7,sizeof(char),25))),
_tmp7D8)))),_tmp7D6)))));}_LL1A2:;};};}_LL127: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1AC=(struct Cyc_Absyn_Conditional_e_struct*)_tmp14E;if(_tmp1AC->tag != 6)goto
_LL129;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;_tmp1AF=_tmp1AC->f3;}}_LL128: {
union Cyc_CfFlowInfo_FlowInfo _tmp2C8;union Cyc_CfFlowInfo_FlowInfo _tmp2C9;struct
_tuple15 _tmp2C7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AD);_tmp2C8=_tmp2C7.f1;
_tmp2C9=_tmp2C7.f2;{struct _tuple13 _tmp2CA=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2C8,_tmp1AE);struct _tuple13 _tmp2CB=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp2C9,_tmp1AF);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp12F,
env->all_changed,_tmp2CA,_tmp2CB);};}_LL129: {struct Cyc_Absyn_And_e_struct*
_tmp1B0=(struct Cyc_Absyn_And_e_struct*)_tmp14E;if(_tmp1B0->tag != 7)goto _LL12B;
else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL12A: {union Cyc_CfFlowInfo_FlowInfo
_tmp2CD;union Cyc_CfFlowInfo_FlowInfo _tmp2CE;struct _tuple15 _tmp2CC=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1B1);_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp2D0;void*_tmp2D1;struct _tuple13 _tmp2CF=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp2CD,_tmp1B2);_tmp2D0=_tmp2CF.f1;_tmp2D1=_tmp2CF.f2;{struct _tuple13
_tmp7D9;struct _tuple13 _tmp2D2=(_tmp7D9.f1=_tmp2D0,((_tmp7D9.f2=_tmp2D1,_tmp7D9)));
struct _tuple13 _tmp7DA;struct _tuple13 _tmp2D3=(_tmp7DA.f1=_tmp2CE,((_tmp7DA.f2=
_tmp12F->zero,_tmp7DA)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp12F,env->all_changed,
_tmp2D2,_tmp2D3);};};}_LL12B: {struct Cyc_Absyn_Or_e_struct*_tmp1B3=(struct Cyc_Absyn_Or_e_struct*)
_tmp14E;if(_tmp1B3->tag != 8)goto _LL12D;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B3->f2;}}
_LL12C: {union Cyc_CfFlowInfo_FlowInfo _tmp2D7;union Cyc_CfFlowInfo_FlowInfo _tmp2D8;
struct _tuple15 _tmp2D6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B4);_tmp2D7=
_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2DA;void*_tmp2DB;
struct _tuple13 _tmp2D9=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D8,_tmp1B5);
_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;{struct _tuple13 _tmp7DB;struct _tuple13
_tmp2DC=(_tmp7DB.f1=_tmp2DA,((_tmp7DB.f2=_tmp2DB,_tmp7DB)));struct _tuple13
_tmp7DC;struct _tuple13 _tmp2DD=(_tmp7DC.f1=_tmp2D7,((_tmp7DC.f2=_tmp12F->notzeroall,
_tmp7DC)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp12F,env->all_changed,
_tmp2DC,_tmp2DD);};};}_LL12D: {struct Cyc_Absyn_Subscript_e_struct*_tmp1B6=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp14E;if(_tmp1B6->tag != 24)goto _LL12F;
else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LL12E: {struct _RegionHandle*
_tmp2E0=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2E3;struct Cyc_List_List*_tmp2E4;
struct Cyc_Absyn_Exp*_tmp7DD[2];struct _tuple19 _tmp2E2=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2E0,env,inflow,((_tmp7DD[1]=_tmp1B8,((_tmp7DD[0]=_tmp1B7,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2E0,
_tag_dyneither(_tmp7DD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2E3=_tmp2E2.f1;
_tmp2E4=_tmp2E2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2E5=_tmp2E3;{union Cyc_CfFlowInfo_FlowInfo
_tmp2E6=_tmp2E3;struct _tuple12 _tmp2E7;struct Cyc_Dict_Dict _tmp2E8;struct Cyc_List_List*
_tmp2E9;_LL1AD: if((_tmp2E6.ReachableFL).tag != 2)goto _LL1AF;_tmp2E7=(struct
_tuple12)(_tmp2E6.ReachableFL).val;_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;_LL1AE:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2E8,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2E4))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7E0;void*_tmp7DF;(_tmp7DF=0,Cyc_Tcutil_terr(_tmp1B8->loc,((_tmp7E0="expression may not be initialized",
_tag_dyneither(_tmp7E0,sizeof(char),34))),_tag_dyneither(_tmp7DF,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2EC=Cyc_NewControlFlow_add_subscript_reln(_tmp12F->r,
_tmp2E9,_tmp1B7,_tmp1B8);if(_tmp2E9 != _tmp2EC)_tmp2E5=Cyc_CfFlowInfo_ReachableFL(
_tmp2E8,_tmp2EC);goto _LL1AC;};_LL1AF:;_LL1B0: goto _LL1AC;_LL1AC:;}{void*_tmp2ED=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B7->topt))->v);
struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_PtrInfo _tmp2F1;struct Cyc_Absyn_PtrAtts
_tmp2F2;union Cyc_Absyn_Constraint*_tmp2F3;_LL1B2: {struct Cyc_Absyn_TupleType_struct*
_tmp2EE=(struct Cyc_Absyn_TupleType_struct*)_tmp2ED;if(_tmp2EE->tag != 11)goto
_LL1B4;else{_tmp2EF=_tmp2EE->f1;}}_LL1B3: {void*_tmp2F4=(void*)((struct Cyc_List_List*)
_check_null(_tmp2E4))->hd;int _tmp2F6;struct _dyneither_ptr _tmp2F7;_LL1B9: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp2F5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2F4;if(_tmp2F5->tag != 6)goto _LL1BB;else{_tmp2F6=_tmp2F5->f1;_tmp2F7=_tmp2F5->f2;}}
_LL1BA: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1B8)).f1;struct
_tuple13 _tmp7E1;return(_tmp7E1.f1=_tmp2E5,((_tmp7E1.f2=*((void**)
_check_dyneither_subscript(_tmp2F7,sizeof(void*),(int)i)),_tmp7E1)));}_LL1BB:;
_LL1BC: {struct Cyc_Core_Impossible_struct _tmp7E7;const char*_tmp7E6;struct Cyc_Core_Impossible_struct*
_tmp7E5;(int)_throw((void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((
_tmp7E7.tag=Cyc_Core_Impossible,((_tmp7E7.f1=((_tmp7E6="anal_Rexp: Subscript",
_tag_dyneither(_tmp7E6,sizeof(char),21))),_tmp7E7)))),_tmp7E5)))));}_LL1B8:;}
_LL1B4: {struct Cyc_Absyn_PointerType_struct*_tmp2F0=(struct Cyc_Absyn_PointerType_struct*)
_tmp2ED;if(_tmp2F0->tag != 5)goto _LL1B6;else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F1.ptr_atts;
_tmp2F3=_tmp2F2.bounds;}}_LL1B5: {union Cyc_CfFlowInfo_FlowInfo _tmp2FD;void*
_tmp2FE;struct _tuple13 _tmp2FC=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E3,
_tmp1B7,(void*)((struct Cyc_List_List*)_check_null(_tmp2E4))->hd);_tmp2FD=_tmp2FC.f1;
_tmp2FE=_tmp2FC.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2FF=_tmp2FD;int _tmp300;
_LL1BE: if((_tmp2FF.BottomFL).tag != 1)goto _LL1C0;_tmp300=(int)(_tmp2FF.BottomFL).val;
_LL1BF: {struct _tuple13 _tmp7E8;return(_tmp7E8.f1=_tmp2FD,((_tmp7E8.f2=_tmp2FE,
_tmp7E8)));}_LL1C0:;_LL1C1: {struct _tuple13 _tmp7E9;return(_tmp7E9.f1=_tmp2E5,((
_tmp7E9.f2=_tmp2FE,_tmp7E9)));}_LL1BD:;};}_LL1B6:;_LL1B7: {struct Cyc_Core_Impossible_struct
_tmp7EF;const char*_tmp7EE;struct Cyc_Core_Impossible_struct*_tmp7ED;(int)_throw((
void*)((_tmp7ED=_cycalloc(sizeof(*_tmp7ED)),((_tmp7ED[0]=((_tmp7EF.tag=Cyc_Core_Impossible,((
_tmp7EF.f1=((_tmp7EE="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7EE,
sizeof(char),33))),_tmp7EF)))),_tmp7ED)))));}_LL1B1:;};};}_LL12F: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1B9=(struct Cyc_Absyn_Datatype_e_struct*)_tmp14E;if(_tmp1B9->tag != 31)goto
_LL131;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LL130: _tmp1BD=_tmp1BA;
goto _LL132;_LL131: {struct Cyc_Absyn_Tuple_e_struct*_tmp1BC=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp14E;if(_tmp1BC->tag != 25)goto _LL133;else{_tmp1BD=_tmp1BC->f1;}}_LL132: {
struct _RegionHandle*_tmp306=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp308;struct Cyc_List_List*
_tmp309;struct _tuple19 _tmp307=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp306,
env,inflow,_tmp1BD,1,1);_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;{unsigned int
_tmp7F5;unsigned int _tmp7F4;struct _dyneither_ptr _tmp7F3;void**_tmp7F2;
unsigned int _tmp7F1;struct _dyneither_ptr aggrdict=(_tmp7F1=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1BD),((_tmp7F2=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp7F1)),((_tmp7F3=
_tag_dyneither(_tmp7F2,sizeof(void*),_tmp7F1),((((_tmp7F4=_tmp7F1,_tmp7F6(&
_tmp7F5,& _tmp7F4,& _tmp7F2,& _tmp309))),_tmp7F3)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp7FC;struct Cyc_CfFlowInfo_Aggregate_struct _tmp7FB;struct _tuple13 _tmp7FA;
return(_tmp7FA.f1=_tmp308,((_tmp7FA.f2=(void*)((_tmp7FC=_region_malloc(env->r,
sizeof(*_tmp7FC)),((_tmp7FC[0]=((_tmp7FB.tag=6,((_tmp7FB.f1=0,((_tmp7FB.f2=
aggrdict,_tmp7FB)))))),_tmp7FC)))),_tmp7FA)));};}_LL133: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1BE=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp14E;if(_tmp1BE->tag != 30)goto
_LL135;else{_tmp1BF=_tmp1BE->f2;}}_LL134: {struct Cyc_List_List*fs;{void*_tmp312=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp314;_LL1C3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp313=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp312;if(_tmp313->tag != 13)goto _LL1C5;
else{_tmp314=_tmp313->f2;}}_LL1C4: fs=_tmp314;goto _LL1C2;_LL1C5:;_LL1C6: {struct
Cyc_Core_Impossible_struct _tmp802;const char*_tmp801;struct Cyc_Core_Impossible_struct*
_tmp800;(int)_throw((void*)((_tmp800=_cycalloc(sizeof(*_tmp800)),((_tmp800[0]=((
_tmp802.tag=Cyc_Core_Impossible,((_tmp802.f1=((_tmp801="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp801,sizeof(char),35))),_tmp802)))),_tmp800)))));}_LL1C2:;}
_tmp1C1=_tmp1BF;_tmp1C4=Cyc_Absyn_StructA;_tmp1C7=fs;goto _LL136;}_LL135: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1C0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp14E;if(_tmp1C0->tag != 29)goto _LL137;else{_tmp1C1=_tmp1C0->f3;_tmp1C2=_tmp1C0->f4;
if(_tmp1C2 == 0)goto _LL137;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.kind;_tmp1C5=_tmp1C3.impl;
if(_tmp1C5 == 0)goto _LL137;_tmp1C6=*_tmp1C5;_tmp1C7=_tmp1C6.fields;}}_LL136: {
struct _RegionHandle*_tmp318=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp31A;struct Cyc_List_List*
_tmp31B;struct _tuple19 _tmp319=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp318,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp318,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp1C1),1,1);_tmp31A=_tmp319.f1;_tmp31B=_tmp319.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp12F,
_tmp1C7,_tmp12F->unknown_all);{int i=0;for(0;_tmp31B != 0;(((_tmp31B=_tmp31B->tl,
_tmp1C1=_tmp1C1->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple21*)((struct
Cyc_List_List*)_check_null(_tmp1C1))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp31C=(void*)ds->hd;struct _dyneither_ptr*_tmp31F;_LL1C8: {struct Cyc_Absyn_ArrayElement_struct*
_tmp31D=(struct Cyc_Absyn_ArrayElement_struct*)_tmp31C;if(_tmp31D->tag != 0)goto
_LL1CA;}_LL1C9: {struct Cyc_Core_Impossible_struct _tmp808;const char*_tmp807;
struct Cyc_Core_Impossible_struct*_tmp806;(int)_throw((void*)((_tmp806=_cycalloc(
sizeof(*_tmp806)),((_tmp806[0]=((_tmp808.tag=Cyc_Core_Impossible,((_tmp808.f1=((
_tmp807="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp807,sizeof(char),22))),
_tmp808)))),_tmp806)))));}_LL1CA: {struct Cyc_Absyn_FieldName_struct*_tmp31E=(
struct Cyc_Absyn_FieldName_struct*)_tmp31C;if(_tmp31E->tag != 1)goto _LL1C7;else{
_tmp31F=_tmp31E->f1;}}_LL1CB: {int _tmp323=Cyc_CfFlowInfo_get_field_index_fs(
_tmp1C7,_tmp31F);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp323))=(void*)_tmp31B->hd;if(_tmp1C4 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp324=(*((struct _tuple21*)_tmp1C1->hd)).f2;_tmp31A=Cyc_NewControlFlow_use_Rval(
env,_tmp324->loc,_tmp31A,(void*)_tmp31B->hd);}}_LL1C7:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp80E;struct Cyc_CfFlowInfo_Aggregate_struct _tmp80D;struct _tuple13 _tmp80C;
return(_tmp80C.f1=_tmp31A,((_tmp80C.f2=(void*)((_tmp80E=_region_malloc(env->r,
sizeof(*_tmp80E)),((_tmp80E[0]=((_tmp80D.tag=6,((_tmp80D.f1=_tmp1C4 == Cyc_Absyn_UnionA,((
_tmp80D.f2=aggrdict,_tmp80D)))))),_tmp80E)))),_tmp80C)));};};}_LL137: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1C8=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp14E;if(_tmp1C8->tag != 29)goto
_LL139;}_LL138: {struct Cyc_Core_Impossible_struct _tmp814;const char*_tmp813;
struct Cyc_Core_Impossible_struct*_tmp812;(int)_throw((void*)((_tmp812=_cycalloc(
sizeof(*_tmp812)),((_tmp812[0]=((_tmp814.tag=Cyc_Core_Impossible,((_tmp814.f1=((
_tmp813="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp813,sizeof(char),31))),
_tmp814)))),_tmp812)))));}_LL139: {struct Cyc_Absyn_Array_e_struct*_tmp1C9=(
struct Cyc_Absyn_Array_e_struct*)_tmp14E;if(_tmp1C9->tag != 27)goto _LL13B;else{
_tmp1CA=_tmp1C9->f1;}}_LL13A: {struct _RegionHandle*_tmp32B=env->r;struct Cyc_List_List*
_tmp32C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp32B,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp1CA);union Cyc_CfFlowInfo_FlowInfo _tmp32E;
struct Cyc_List_List*_tmp32F;struct _tuple19 _tmp32D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp32B,env,inflow,_tmp32C,1,1);_tmp32E=_tmp32D.f1;_tmp32F=_tmp32D.f2;for(0;
_tmp32F != 0;(_tmp32F=_tmp32F->tl,_tmp32C=_tmp32C->tl)){_tmp32E=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp32C))->hd)->loc,
_tmp32E,(void*)_tmp32F->hd);}{struct _tuple13 _tmp815;return(_tmp815.f1=_tmp32E,((
_tmp815.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp12F,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp12F->unknown_all),_tmp815)));};}_LL13B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1CB=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp14E;if(_tmp1CB->tag != 28)
goto _LL13D;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CB->f2;_tmp1CE=_tmp1CB->f3;
_tmp1CF=_tmp1CB->f4;}}_LL13C: {union Cyc_CfFlowInfo_FlowInfo _tmp332;void*_tmp333;
struct _tuple13 _tmp331=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CD);_tmp332=
_tmp331.f1;_tmp333=_tmp331.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp334=_tmp332;int
_tmp335;struct _tuple12 _tmp336;struct Cyc_Dict_Dict _tmp337;struct Cyc_List_List*
_tmp338;_LL1CD: if((_tmp334.BottomFL).tag != 1)goto _LL1CF;_tmp335=(int)(_tmp334.BottomFL).val;
_LL1CE: {struct _tuple13 _tmp816;return(_tmp816.f1=_tmp332,((_tmp816.f2=_tmp12F->unknown_all,
_tmp816)));}_LL1CF: if((_tmp334.ReachableFL).tag != 2)goto _LL1CC;_tmp336=(struct
_tuple12)(_tmp334.ReachableFL).val;_tmp337=_tmp336.f1;_tmp338=_tmp336.f2;_LL1D0:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp337,_tmp333)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp819="expression may not be initialized",
_tag_dyneither(_tmp819,sizeof(char),34))),_tag_dyneither(_tmp818,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp338;comp_loop: {void*_tmp33C=_tmp1CD->r;struct
Cyc_Absyn_Exp*_tmp33E;void*_tmp340;struct Cyc_Absyn_Vardecl*_tmp342;void*_tmp344;
struct Cyc_Absyn_Vardecl*_tmp346;void*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34A;
void*_tmp34C;struct Cyc_Absyn_Vardecl*_tmp34E;union Cyc_Absyn_Cnst _tmp350;struct
_tuple6 _tmp351;int _tmp352;enum Cyc_Absyn_Primop _tmp354;struct Cyc_List_List*
_tmp355;struct Cyc_List_List _tmp356;struct Cyc_Absyn_Exp*_tmp357;_LL1D2: {struct
Cyc_Absyn_Cast_e_struct*_tmp33D=(struct Cyc_Absyn_Cast_e_struct*)_tmp33C;if(
_tmp33D->tag != 15)goto _LL1D4;else{_tmp33E=_tmp33D->f2;}}_LL1D3: _tmp1CD=_tmp33E;
goto comp_loop;_LL1D4:{struct Cyc_Absyn_Var_e_struct*_tmp33F=(struct Cyc_Absyn_Var_e_struct*)
_tmp33C;if(_tmp33F->tag != 1)goto _LL1D6;else{_tmp340=(void*)_tmp33F->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp341=(struct Cyc_Absyn_Global_b_struct*)_tmp340;if(
_tmp341->tag != 1)goto _LL1D6;else{_tmp342=_tmp341->f1;}};}}if(!(!_tmp342->escapes))
goto _LL1D6;_LL1D5: _tmp346=_tmp342;goto _LL1D7;_LL1D6:{struct Cyc_Absyn_Var_e_struct*
_tmp343=(struct Cyc_Absyn_Var_e_struct*)_tmp33C;if(_tmp343->tag != 1)goto _LL1D8;
else{_tmp344=(void*)_tmp343->f2;{struct Cyc_Absyn_Local_b_struct*_tmp345=(struct
Cyc_Absyn_Local_b_struct*)_tmp344;if(_tmp345->tag != 4)goto _LL1D8;else{_tmp346=
_tmp345->f1;}};}}if(!(!_tmp346->escapes))goto _LL1D8;_LL1D7: _tmp34A=_tmp346;goto
_LL1D9;_LL1D8:{struct Cyc_Absyn_Var_e_struct*_tmp347=(struct Cyc_Absyn_Var_e_struct*)
_tmp33C;if(_tmp347->tag != 1)goto _LL1DA;else{_tmp348=(void*)_tmp347->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp349=(struct Cyc_Absyn_Pat_b_struct*)_tmp348;if(_tmp349->tag
!= 5)goto _LL1DA;else{_tmp34A=_tmp349->f1;}};}}if(!(!_tmp34A->escapes))goto _LL1DA;
_LL1D9: _tmp34E=_tmp34A;goto _LL1DB;_LL1DA:{struct Cyc_Absyn_Var_e_struct*_tmp34B=(
struct Cyc_Absyn_Var_e_struct*)_tmp33C;if(_tmp34B->tag != 1)goto _LL1DC;else{
_tmp34C=(void*)_tmp34B->f2;{struct Cyc_Absyn_Param_b_struct*_tmp34D=(struct Cyc_Absyn_Param_b_struct*)
_tmp34C;if(_tmp34D->tag != 3)goto _LL1DC;else{_tmp34E=_tmp34D->f1;}};}}if(!(!
_tmp34E->escapes))goto _LL1DC;_LL1DB:{struct Cyc_CfFlowInfo_Reln*_tmp81C;struct Cyc_List_List*
_tmp81B;new_relns=((_tmp81B=_region_malloc(env->r,sizeof(*_tmp81B)),((_tmp81B->hd=((
_tmp81C=_region_malloc(env->r,sizeof(*_tmp81C)),((_tmp81C->vd=_tmp1CC,((_tmp81C->rop=
Cyc_CfFlowInfo_LessVar(_tmp34E,_tmp34E->type),_tmp81C)))))),((_tmp81B->tl=
_tmp338,_tmp81B))))));}goto _LL1D1;_LL1DC: {struct Cyc_Absyn_Const_e_struct*
_tmp34F=(struct Cyc_Absyn_Const_e_struct*)_tmp33C;if(_tmp34F->tag != 0)goto _LL1DE;
else{_tmp350=_tmp34F->f1;if((_tmp350.Int_c).tag != 4)goto _LL1DE;_tmp351=(struct
_tuple6)(_tmp350.Int_c).val;_tmp352=_tmp351.f2;}}_LL1DD:{struct Cyc_CfFlowInfo_Reln*
_tmp81F;struct Cyc_List_List*_tmp81E;new_relns=((_tmp81E=_region_malloc(env->r,
sizeof(*_tmp81E)),((_tmp81E->hd=((_tmp81F=_region_malloc(env->r,sizeof(*_tmp81F)),((
_tmp81F->vd=_tmp1CC,((_tmp81F->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp352),_tmp81F)))))),((_tmp81E->tl=_tmp338,_tmp81E))))));}goto _LL1D1;_LL1DE: {
struct Cyc_Absyn_Primop_e_struct*_tmp353=(struct Cyc_Absyn_Primop_e_struct*)
_tmp33C;if(_tmp353->tag != 3)goto _LL1E0;else{_tmp354=_tmp353->f1;_tmp355=_tmp353->f2;
if(_tmp355 == 0)goto _LL1E0;_tmp356=*_tmp355;_tmp357=(struct Cyc_Absyn_Exp*)_tmp356.hd;}}
_LL1DF:{void*_tmp35C=_tmp357->r;void*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp360;
void*_tmp362;struct Cyc_Absyn_Vardecl*_tmp364;void*_tmp366;struct Cyc_Absyn_Vardecl*
_tmp368;void*_tmp36A;struct Cyc_Absyn_Vardecl*_tmp36C;_LL1E3:{struct Cyc_Absyn_Var_e_struct*
_tmp35D=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp35D->tag != 1)goto _LL1E5;
else{_tmp35E=(void*)_tmp35D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp35F=(struct
Cyc_Absyn_Global_b_struct*)_tmp35E;if(_tmp35F->tag != 1)goto _LL1E5;else{_tmp360=
_tmp35F->f1;}};}}if(!(!_tmp360->escapes))goto _LL1E5;_LL1E4: _tmp364=_tmp360;goto
_LL1E6;_LL1E5:{struct Cyc_Absyn_Var_e_struct*_tmp361=(struct Cyc_Absyn_Var_e_struct*)
_tmp35C;if(_tmp361->tag != 1)goto _LL1E7;else{_tmp362=(void*)_tmp361->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp363=(struct Cyc_Absyn_Local_b_struct*)_tmp362;if(
_tmp363->tag != 4)goto _LL1E7;else{_tmp364=_tmp363->f1;}};}}if(!(!_tmp364->escapes))
goto _LL1E7;_LL1E6: _tmp368=_tmp364;goto _LL1E8;_LL1E7:{struct Cyc_Absyn_Var_e_struct*
_tmp365=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp365->tag != 1)goto _LL1E9;
else{_tmp366=(void*)_tmp365->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp367=(struct Cyc_Absyn_Pat_b_struct*)
_tmp366;if(_tmp367->tag != 5)goto _LL1E9;else{_tmp368=_tmp367->f1;}};}}if(!(!
_tmp368->escapes))goto _LL1E9;_LL1E8: _tmp36C=_tmp368;goto _LL1EA;_LL1E9:{struct Cyc_Absyn_Var_e_struct*
_tmp369=(struct Cyc_Absyn_Var_e_struct*)_tmp35C;if(_tmp369->tag != 1)goto _LL1EB;
else{_tmp36A=(void*)_tmp369->f2;{struct Cyc_Absyn_Param_b_struct*_tmp36B=(struct
Cyc_Absyn_Param_b_struct*)_tmp36A;if(_tmp36B->tag != 3)goto _LL1EB;else{_tmp36C=
_tmp36B->f1;}};}}if(!(!_tmp36C->escapes))goto _LL1EB;_LL1EA:{struct Cyc_CfFlowInfo_Reln*
_tmp822;struct Cyc_List_List*_tmp821;new_relns=((_tmp821=_region_malloc(env->r,
sizeof(*_tmp821)),((_tmp821->hd=((_tmp822=_region_malloc(env->r,sizeof(*_tmp822)),((
_tmp822->vd=_tmp1CC,((_tmp822->rop=Cyc_CfFlowInfo_LessNumelts(_tmp36C),_tmp822)))))),((
_tmp821->tl=_tmp338,_tmp821))))));}goto _LL1E2;_LL1EB:;_LL1EC: goto _LL1E2;_LL1E2:;}
goto _LL1D1;_LL1E0:;_LL1E1: goto _LL1D1;_LL1D1:;}if(_tmp338 != new_relns)_tmp332=Cyc_CfFlowInfo_ReachableFL(
_tmp337,new_relns);{void*_tmp36F=_tmp333;_LL1EE: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp370=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp36F;if(_tmp370->tag != 0)goto
_LL1F0;}_LL1EF: {struct _tuple13 _tmp823;return(_tmp823.f1=_tmp332,((_tmp823.f2=
_tmp12F->unknown_all,_tmp823)));}_LL1F0: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp371=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp36F;if(_tmp371->tag != 2)
goto _LL1F2;}_LL1F1: goto _LL1F3;_LL1F2: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp372=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp36F;if(_tmp372->tag != 1)
goto _LL1F4;}_LL1F3: goto _LL1F5;_LL1F4: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp373=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp36F;if(_tmp373->tag != 5)goto
_LL1F6;}_LL1F5: {struct Cyc_List_List _tmp824;struct Cyc_List_List _tmp375=(_tmp824.hd=
_tmp1CC,((_tmp824.tl=0,_tmp824)));_tmp332=Cyc_NewControlFlow_add_vars(_tmp12F,
_tmp332,(struct Cyc_List_List*)& _tmp375,_tmp12F->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp377;void*_tmp378;struct _tuple13 _tmp376=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp332,_tmp1CE);_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp379=_tmp377;int _tmp37A;struct _tuple12 _tmp37B;struct Cyc_Dict_Dict _tmp37C;
_LL1F9: if((_tmp379.BottomFL).tag != 1)goto _LL1FB;_tmp37A=(int)(_tmp379.BottomFL).val;
_LL1FA: {struct _tuple13 _tmp825;return(_tmp825.f1=_tmp377,((_tmp825.f2=_tmp12F->unknown_all,
_tmp825)));}_LL1FB: if((_tmp379.ReachableFL).tag != 2)goto _LL1F8;_tmp37B=(struct
_tuple12)(_tmp379.ReachableFL).val;_tmp37C=_tmp37B.f1;_LL1FC: if(Cyc_CfFlowInfo_initlevel(
_tmp12F,_tmp37C,_tmp378)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp828;void*_tmp827;(
_tmp827=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp828="expression may not be initialized",
_tag_dyneither(_tmp828,sizeof(char),34))),_tag_dyneither(_tmp827,sizeof(void*),0)));}{
struct _tuple13 _tmp829;return(_tmp829.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp829.f2=
_tmp12F->unknown_all,_tmp829)));};}_LL1F8:;}_tmp332=_tmp377;goto _LL1F7;};}_LL1F6:;
_LL1F7: while(1){struct Cyc_List_List _tmp82A;struct Cyc_List_List _tmp382=(_tmp82A.hd=
_tmp1CC,((_tmp82A.tl=0,_tmp82A)));_tmp332=Cyc_NewControlFlow_add_vars(_tmp12F,
_tmp332,(struct Cyc_List_List*)& _tmp382,_tmp12F->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp384;void*_tmp385;struct _tuple13 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp332,_tmp1CE);_tmp384=_tmp383.f1;_tmp385=_tmp383.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp386=_tmp384;int _tmp387;struct _tuple12 _tmp388;struct Cyc_Dict_Dict _tmp389;
_LL1FE: if((_tmp386.BottomFL).tag != 1)goto _LL200;_tmp387=(int)(_tmp386.BottomFL).val;
_LL1FF: goto _LL1FD;_LL200: if((_tmp386.ReachableFL).tag != 2)goto _LL1FD;_tmp388=(
struct _tuple12)(_tmp386.ReachableFL).val;_tmp389=_tmp388.f1;_LL201: if(Cyc_CfFlowInfo_initlevel(
_tmp12F,_tmp389,_tmp385)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp82D;void*_tmp82C;(
_tmp82C=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp82D="expression may not be initialized",
_tag_dyneither(_tmp82D,sizeof(char),34))),_tag_dyneither(_tmp82C,sizeof(void*),0)));}{
struct _tuple13 _tmp82E;return(_tmp82E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp82E.f2=
_tmp12F->unknown_all,_tmp82E)));};}_LL1FD:;}{union Cyc_CfFlowInfo_FlowInfo _tmp38D=
Cyc_CfFlowInfo_join_flow(_tmp12F,env->all_changed,_tmp332,_tmp384);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp38D,_tmp332))break;_tmp332=_tmp38D;};};}{struct _tuple13 _tmp82F;return(
_tmp82F.f1=_tmp332,((_tmp82F.f2=_tmp12F->unknown_all,_tmp82F)));};_LL1ED:;};};
_LL1CC:;};}_LL13D: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1D0=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp14E;if(_tmp1D0->tag != 37)goto _LL13F;else{_tmp1D1=_tmp1D0->f1;}}_LL13E: while(
1){union Cyc_CfFlowInfo_FlowInfo*_tmp391;struct _tuple16 _tmp390=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp1D1);_tmp391=_tmp390.f2;inflow=*_tmp391;{void*_tmp392=_tmp1D1->r;
struct Cyc_Absyn_Stmt*_tmp394;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Decl*
_tmp397;struct Cyc_Absyn_Stmt*_tmp398;struct Cyc_Absyn_Exp*_tmp39A;_LL203: {struct
Cyc_Absyn_Seq_s_struct*_tmp393=(struct Cyc_Absyn_Seq_s_struct*)_tmp392;if(_tmp393->tag
!= 2)goto _LL205;else{_tmp394=_tmp393->f1;_tmp395=_tmp393->f2;}}_LL204: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp394);_tmp1D1=_tmp395;goto _LL202;_LL205: {struct Cyc_Absyn_Decl_s_struct*
_tmp396=(struct Cyc_Absyn_Decl_s_struct*)_tmp392;if(_tmp396->tag != 12)goto _LL207;
else{_tmp397=_tmp396->f1;_tmp398=_tmp396->f2;}}_LL206: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp397);_tmp1D1=_tmp398;goto _LL202;_LL207: {struct Cyc_Absyn_Exp_s_struct*
_tmp399=(struct Cyc_Absyn_Exp_s_struct*)_tmp392;if(_tmp399->tag != 1)goto _LL209;
else{_tmp39A=_tmp399->f1;}}_LL208: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp39A);_LL209:;_LL20A: {struct Cyc_Core_Impossible_struct
_tmp835;const char*_tmp834;struct Cyc_Core_Impossible_struct*_tmp833;(int)_throw((
void*)((_tmp833=_cycalloc(sizeof(*_tmp833)),((_tmp833[0]=((_tmp835.tag=Cyc_Core_Impossible,((
_tmp835.f1=((_tmp834="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp834,
sizeof(char),33))),_tmp835)))),_tmp833)))));}_LL202:;};}_LL13F: {struct Cyc_Absyn_Var_e_struct*
_tmp1D2=(struct Cyc_Absyn_Var_e_struct*)_tmp14E;if(_tmp1D2->tag != 1)goto _LL141;
else{_tmp1D3=(void*)_tmp1D2->f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp1D4=(
struct Cyc_Absyn_Unresolved_b_struct*)_tmp1D3;if(_tmp1D4->tag != 0)goto _LL141;};}}
_LL140: goto _LL142;_LL141: {struct Cyc_Absyn_UnknownId_e_struct*_tmp1D5=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp14E;if(_tmp1D5->tag != 2)goto _LL143;}_LL142: goto _LL144;_LL143: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp1D6=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp14E;if(_tmp1D6->tag != 10)goto
_LL145;}_LL144: goto _LL146;_LL145: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1D7=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp14E;if(_tmp1D7->tag != 36)
goto _LL147;}_LL146: goto _LL148;_LL147: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1D8=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp14E;if(_tmp1D8->tag != 26)goto
_LL149;}_LL148: goto _LL14A;_LL149: {struct Cyc_Absyn_Valueof_e_struct*_tmp1D9=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp14E;if(_tmp1D9->tag != 39)goto _LLE2;}_LL14A: {
struct Cyc_Core_Impossible_struct _tmp83B;const char*_tmp83A;struct Cyc_Core_Impossible_struct*
_tmp839;(int)_throw((void*)((_tmp839=_cycalloc(sizeof(*_tmp839)),((_tmp839[0]=((
_tmp83B.tag=Cyc_Core_Impossible,((_tmp83B.f1=((_tmp83A="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp83A,sizeof(char),31))),_tmp83B)))),_tmp839)))));}_LLE2:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3A1=env->fenv;void*_tmp3A2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_PtrAtts _tmp3A6;union Cyc_Absyn_Constraint*
_tmp3A7;union Cyc_Absyn_Constraint*_tmp3A8;_LL20C: {struct Cyc_Absyn_PointerType_struct*
_tmp3A3=(struct Cyc_Absyn_PointerType_struct*)_tmp3A2;if(_tmp3A3->tag != 5)goto
_LL20E;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A4.elt_typ;_tmp3A6=_tmp3A4.ptr_atts;
_tmp3A7=_tmp3A6.bounds;_tmp3A8=_tmp3A6.zero_term;}}_LL20D: {union Cyc_CfFlowInfo_FlowInfo
_tmp3A9=f;int _tmp3AA;struct _tuple12 _tmp3AB;struct Cyc_Dict_Dict _tmp3AC;struct Cyc_List_List*
_tmp3AD;_LL211: if((_tmp3A9.BottomFL).tag != 1)goto _LL213;_tmp3AA=(int)(_tmp3A9.BottomFL).val;
_LL212: {struct _tuple14 _tmp83C;return(_tmp83C.f1=f,((_tmp83C.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp83C)));}_LL213: if((_tmp3A9.ReachableFL).tag != 2)goto _LL210;_tmp3AB=(struct
_tuple12)(_tmp3A9.ReachableFL).val;_tmp3AC=_tmp3AB.f1;_tmp3AD=_tmp3AB.f2;_LL214:
if(Cyc_Tcutil_is_bound_one(_tmp3A7)){void*_tmp3AF=r;struct Cyc_CfFlowInfo_Place*
_tmp3B3;struct Cyc_CfFlowInfo_Place _tmp3B4;void*_tmp3B5;struct Cyc_List_List*
_tmp3B6;enum Cyc_CfFlowInfo_InitLevel _tmp3B9;_LL216: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3B0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3AF;if(_tmp3B0->tag != 1)
goto _LL218;}_LL217: goto _LL219;_LL218: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3B1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3AF;if(_tmp3B1->tag != 2)
goto _LL21A;}_LL219:{struct Cyc_CfFlowInfo_NotZero_struct _tmp83F;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp83E;e->annot=(void*)((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E[0]=((
_tmp83F.tag=Cyc_CfFlowInfo_NotZero,((_tmp83F.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3AD),_tmp83F)))),_tmp83E))));}goto _LL215;_LL21A: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3B2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3AF;if(_tmp3B2->tag != 5)goto
_LL21C;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=*_tmp3B3;_tmp3B5=_tmp3B4.root;_tmp3B6=
_tmp3B4.fields;}}_LL21B:{struct Cyc_CfFlowInfo_NotZero_struct _tmp842;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp841;e->annot=(void*)((_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841[0]=((
_tmp842.tag=Cyc_CfFlowInfo_NotZero,((_tmp842.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3AD),_tmp842)))),_tmp841))));}{struct Cyc_CfFlowInfo_Place*_tmp845;struct
_tuple14 _tmp844;return(_tmp844.f1=f,((_tmp844.f2=Cyc_CfFlowInfo_PlaceL(((_tmp845=
_region_malloc(_tmp3A1->r,sizeof(*_tmp845)),((_tmp845->root=_tmp3B5,((_tmp845->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3A1->r,_tmp3B6,flds),_tmp845))))))),_tmp844)));};_LL21C: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3B7=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3AF;if(_tmp3B7->tag != 0)goto _LL21E;}_LL21D: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp846;return(_tmp846.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp846.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp846)));};_LL21E: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3B8=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3AF;if(_tmp3B8->tag != 3)goto
_LL220;else{_tmp3B9=_tmp3B8->f1;}}_LL21F: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3B9);goto _LL221;_LL220:;_LL221: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp849;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp848;e->annot=(void*)((_tmp848=
_cycalloc(sizeof(*_tmp848)),((_tmp848[0]=((_tmp849.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp849.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3AD),_tmp849)))),
_tmp848))));}_LL215:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp84C;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp84B;e->annot=(void*)((_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((
_tmp84C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp84C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3AD),_tmp84C)))),_tmp84B))));}if(Cyc_CfFlowInfo_initlevel(_tmp3A1,_tmp3AC,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp84F;void*_tmp84E;(_tmp84E=0,Cyc_Tcutil_terr(
e->loc,((_tmp84F="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp84F,sizeof(char),46))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}{struct
_tuple14 _tmp850;return(_tmp850.f1=f,((_tmp850.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp850)));};_LL210:;}_LL20E:;_LL20F: {struct Cyc_Core_Impossible_struct _tmp856;
const char*_tmp855;struct Cyc_Core_Impossible_struct*_tmp854;(int)_throw((void*)((
_tmp854=_cycalloc(sizeof(*_tmp854)),((_tmp854[0]=((_tmp856.tag=Cyc_Core_Impossible,((
_tmp856.f1=((_tmp855="left deref of non-pointer-type",_tag_dyneither(_tmp855,
sizeof(char),31))),_tmp856)))),_tmp854)))));}_LL20B:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp3CB=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp3CC=inflow;int _tmp3CD;struct _tuple12
_tmp3CE;struct Cyc_Dict_Dict _tmp3CF;struct Cyc_List_List*_tmp3D0;_LL223: if((
_tmp3CC.BottomFL).tag != 1)goto _LL225;_tmp3CD=(int)(_tmp3CC.BottomFL).val;_LL224: {
struct _tuple14 _tmp857;return(_tmp857.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp857.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp857)));}_LL225: if((_tmp3CC.ReachableFL).tag != 2)
goto _LL222;_tmp3CE=(struct _tuple12)(_tmp3CC.ReachableFL).val;_tmp3CF=_tmp3CE.f1;
_tmp3D0=_tmp3CE.f2;_LL226: d=_tmp3CF;_LL222:;}{void*_tmp3D2=e->r;struct Cyc_Absyn_Exp*
_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D8;void*_tmp3DA;
struct Cyc_Absyn_Vardecl*_tmp3DC;void*_tmp3DE;struct Cyc_Absyn_Vardecl*_tmp3E0;
void*_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E4;void*_tmp3E6;struct Cyc_Absyn_Vardecl*
_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;struct _dyneither_ptr*_tmp3EB;struct Cyc_Absyn_Exp*
_tmp3ED;struct Cyc_Absyn_Exp*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Exp*
_tmp3F2;struct _dyneither_ptr*_tmp3F3;_LL228: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D3=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D2;if(_tmp3D3->tag != 15)goto _LL22A;
else{_tmp3D4=_tmp3D3->f2;}}_LL229: _tmp3D6=_tmp3D4;goto _LL22B;_LL22A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp3D5=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp3D2;if(_tmp3D5->tag != 13)
goto _LL22C;else{_tmp3D6=_tmp3D5->f1;}}_LL22B: _tmp3D8=_tmp3D6;goto _LL22D;_LL22C: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp3D7=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp3D2;if(_tmp3D7->tag != 14)goto _LL22E;else{_tmp3D8=_tmp3D7->f1;}}_LL22D: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3D8,flds);_LL22E: {
struct Cyc_Absyn_Var_e_struct*_tmp3D9=(struct Cyc_Absyn_Var_e_struct*)_tmp3D2;if(
_tmp3D9->tag != 1)goto _LL230;else{_tmp3DA=(void*)_tmp3D9->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp3DB=(struct Cyc_Absyn_Param_b_struct*)_tmp3DA;if(_tmp3DB->tag != 3)goto _LL230;
else{_tmp3DC=_tmp3DB->f1;}};}}_LL22F: _tmp3E0=_tmp3DC;goto _LL231;_LL230: {struct
Cyc_Absyn_Var_e_struct*_tmp3DD=(struct Cyc_Absyn_Var_e_struct*)_tmp3D2;if(_tmp3DD->tag
!= 1)goto _LL232;else{_tmp3DE=(void*)_tmp3DD->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp3DF=(struct Cyc_Absyn_Local_b_struct*)_tmp3DE;if(_tmp3DF->tag != 4)goto _LL232;
else{_tmp3E0=_tmp3DF->f1;}};}}_LL231: _tmp3E4=_tmp3E0;goto _LL233;_LL232: {struct
Cyc_Absyn_Var_e_struct*_tmp3E1=(struct Cyc_Absyn_Var_e_struct*)_tmp3D2;if(_tmp3E1->tag
!= 1)goto _LL234;else{_tmp3E2=(void*)_tmp3E1->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp3E3=(struct Cyc_Absyn_Pat_b_struct*)_tmp3E2;if(_tmp3E3->tag != 5)goto _LL234;
else{_tmp3E4=_tmp3E3->f1;}};}}_LL233: {struct Cyc_CfFlowInfo_Place*_tmp861;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp860;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp85F;
struct _tuple14 _tmp85E;return(_tmp85E.f1=inflow,((_tmp85E.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp861=_region_malloc(env->r,sizeof(*_tmp861)),((_tmp861->root=(void*)((_tmp85F=
_region_malloc(env->r,sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp860.tag=0,((_tmp860.f1=
_tmp3E4,_tmp860)))),_tmp85F)))),((_tmp861->fields=flds,_tmp861))))))),_tmp85E)));}
_LL234: {struct Cyc_Absyn_Var_e_struct*_tmp3E5=(struct Cyc_Absyn_Var_e_struct*)
_tmp3D2;if(_tmp3E5->tag != 1)goto _LL236;else{_tmp3E6=(void*)_tmp3E5->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp3E7=(struct Cyc_Absyn_Global_b_struct*)_tmp3E6;if(
_tmp3E7->tag != 1)goto _LL236;else{_tmp3E8=_tmp3E7->f1;}};}}_LL235: {struct
_tuple14 _tmp862;return(_tmp862.f1=inflow,((_tmp862.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp862)));}_LL236: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp3E9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3D2;if(_tmp3E9->tag != 23)goto _LL238;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3E9->f2;}}
_LL237:{void*_tmp3F9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EA->topt))->v);struct Cyc_Absyn_PtrInfo _tmp3FB;void*_tmp3FC;
_LL241: {struct Cyc_Absyn_PointerType_struct*_tmp3FA=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F9;if(_tmp3FA->tag != 5)goto _LL243;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FB.elt_typ;}}
_LL242: if(!Cyc_Absyn_is_nontagged_union_type(_tmp3FC)){struct Cyc_List_List*
_tmp863;flds=((_tmp863=_region_malloc(env->r,sizeof(*_tmp863)),((_tmp863->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp3FC,_tmp3EB),((_tmp863->tl=flds,_tmp863))))));}
goto _LL240;_LL243:;_LL244: {struct Cyc_Core_Impossible_struct _tmp869;const char*
_tmp868;struct Cyc_Core_Impossible_struct*_tmp867;(int)_throw((void*)((_tmp867=
_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((_tmp869.tag=Cyc_Core_Impossible,((
_tmp869.f1=((_tmp868="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp868,sizeof(
char),25))),_tmp869)))),_tmp867)))));}_LL240:;}_tmp3ED=_tmp3EA;goto _LL239;_LL238: {
struct Cyc_Absyn_Deref_e_struct*_tmp3EC=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D2;
if(_tmp3EC->tag != 21)goto _LL23A;else{_tmp3ED=_tmp3EC->f1;}}_LL239: if(Cyc_Tcutil_is_noalias_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3ED->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp402;union Cyc_CfFlowInfo_AbsLVal _tmp403;struct _tuple14 _tmp401=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,_tmp3ED);_tmp402=_tmp401.f1;_tmp403=_tmp401.f2;{struct _tuple14
_tmp86A;struct _tuple14 _tmp405=(_tmp86A.f1=_tmp402,((_tmp86A.f2=_tmp403,_tmp86A)));
union Cyc_CfFlowInfo_FlowInfo _tmp406;struct _tuple12 _tmp407;struct Cyc_Dict_Dict
_tmp408;struct Cyc_List_List*_tmp409;union Cyc_CfFlowInfo_AbsLVal _tmp40A;struct Cyc_CfFlowInfo_Place*
_tmp40B;_LL246: _tmp406=_tmp405.f1;if((_tmp406.ReachableFL).tag != 2)goto _LL248;
_tmp407=(struct _tuple12)(_tmp406.ReachableFL).val;_tmp408=_tmp407.f1;_tmp409=
_tmp407.f2;_tmp40A=_tmp405.f2;if((_tmp40A.PlaceL).tag != 1)goto _LL248;_tmp40B=(
struct Cyc_CfFlowInfo_Place*)(_tmp40A.PlaceL).val;_LL247: {void*_tmp40C=Cyc_CfFlowInfo_lookup_place(
_tmp408,_tmp40B);void*_tmp40D=_tmp40C;_LL24B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp40E=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp40D;if(_tmp40E->tag != 5)goto
_LL24D;}_LL24C: return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3ED,_tmp402,
_tmp40C,flds);_LL24D:;_LL24E: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp3CB,_tmp408,_tmp40C);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3CB->unknown_all:
_tmp3CB->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3CB,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3ED->topt))->v),leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp86D;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp86C;void*new_root=(void*)((
_tmp86C=_region_malloc(_tmp3CB->r,sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=1,((
_tmp86D.f1=e,((_tmp86D.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp86D)))))),_tmp86C))));struct Cyc_CfFlowInfo_Place*_tmp86E;struct Cyc_CfFlowInfo_Place*
place=(_tmp86E=_region_malloc(_tmp3CB->r,sizeof(*_tmp86E)),((_tmp86E->root=
new_root,((_tmp86E->fields=0,_tmp86E)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp871;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp870;void*res=(void*)((_tmp870=
_region_malloc(_tmp3CB->r,sizeof(*_tmp870)),((_tmp870[0]=((_tmp871.tag=5,((
_tmp871.f1=place,_tmp871)))),_tmp870))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp408=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp408,new_root,new_rval);_tmp408=Cyc_CfFlowInfo_assign_place(
_tmp3CB,e->loc,_tmp408,env->all_changed,_tmp40B,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp40F=Cyc_CfFlowInfo_ReachableFL(_tmp408,_tmp409);return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3ED,_tmp40F,res,flds);};}_LL24A:;}_LL248:;_LL249: goto _LL245;
_LL245:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp416;void*_tmp417;struct _tuple13
_tmp415=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3ED);_tmp416=_tmp415.f1;
_tmp417=_tmp415.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3ED,
_tmp416,_tmp417,flds);};_LL23A: {struct Cyc_Absyn_Subscript_e_struct*_tmp3EE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp3D2;if(_tmp3EE->tag != 24)goto _LL23C;
else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;}}_LL23B: {void*_tmp418=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp41B;struct Cyc_Absyn_PtrAtts _tmp41C;union Cyc_Absyn_Constraint*_tmp41D;_LL250: {
struct Cyc_Absyn_TupleType_struct*_tmp419=(struct Cyc_Absyn_TupleType_struct*)
_tmp418;if(_tmp419->tag != 11)goto _LL252;}_LL251: {unsigned int _tmp41E=(Cyc_Evexp_eval_const_uint_exp(
_tmp3F0)).f1;struct Cyc_List_List*_tmp872;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp3EF,((_tmp872=_region_malloc(env->r,sizeof(*_tmp872)),((
_tmp872->hd=(void*)((int)_tmp41E),((_tmp872->tl=flds,_tmp872)))))));}_LL252: {
struct Cyc_Absyn_PointerType_struct*_tmp41A=(struct Cyc_Absyn_PointerType_struct*)
_tmp418;if(_tmp41A->tag != 5)goto _LL254;else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41B.ptr_atts;
_tmp41D=_tmp41C.bounds;}}_LL253: {struct _RegionHandle*_tmp420=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp423;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Exp*_tmp873[2];struct
_tuple19 _tmp422=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp420,env,inflow,((
_tmp873[1]=_tmp3F0,((_tmp873[0]=_tmp3EF,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp420,_tag_dyneither(
_tmp873,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp423=_tmp422.f1;_tmp424=
_tmp422.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp425=_tmp423;{union Cyc_CfFlowInfo_FlowInfo
_tmp426=_tmp423;struct _tuple12 _tmp427;struct Cyc_Dict_Dict _tmp428;struct Cyc_List_List*
_tmp429;_LL257: if((_tmp426.ReachableFL).tag != 2)goto _LL259;_tmp427=(struct
_tuple12)(_tmp426.ReachableFL).val;_tmp428=_tmp427.f1;_tmp429=_tmp427.f2;_LL258:
if(Cyc_CfFlowInfo_initlevel(_tmp3CB,_tmp428,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp424))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_terr(_tmp3F0->loc,((_tmp876="expression may not be initialized",
_tag_dyneither(_tmp876,sizeof(char),34))),_tag_dyneither(_tmp875,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp42C=Cyc_NewControlFlow_add_subscript_reln(_tmp3CB->r,
_tmp429,_tmp3EF,_tmp3F0);if(_tmp429 != _tmp42C)_tmp425=Cyc_CfFlowInfo_ReachableFL(
_tmp428,_tmp42C);goto _LL256;};_LL259:;_LL25A: goto _LL256;_LL256:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp42E;union Cyc_CfFlowInfo_AbsLVal _tmp42F;struct _tuple14 _tmp42D=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp3EF,_tmp423,(void*)((struct Cyc_List_List*)_check_null(_tmp424))->hd,
flds);_tmp42E=_tmp42D.f1;_tmp42F=_tmp42D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp430=
_tmp42E;int _tmp431;_LL25C: if((_tmp430.BottomFL).tag != 1)goto _LL25E;_tmp431=(int)(
_tmp430.BottomFL).val;_LL25D: {struct _tuple14 _tmp877;return(_tmp877.f1=_tmp42E,((
_tmp877.f2=_tmp42F,_tmp877)));}_LL25E:;_LL25F: {struct _tuple14 _tmp878;return(
_tmp878.f1=_tmp425,((_tmp878.f2=_tmp42F,_tmp878)));}_LL25B:;};};};}_LL254:;
_LL255: {struct Cyc_Core_Impossible_struct _tmp87E;const char*_tmp87D;struct Cyc_Core_Impossible_struct*
_tmp87C;(int)_throw((void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((
_tmp87E.tag=Cyc_Core_Impossible,((_tmp87E.f1=((_tmp87D="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp87D,sizeof(char),33))),_tmp87E)))),_tmp87C)))));}_LL24F:;}
_LL23C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp3F1=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3D2;if(_tmp3F1->tag != 22)goto _LL23E;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;}}
_LL23D: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F2->topt))->v)){struct _tuple14 _tmp87F;return(_tmp87F.f1=inflow,((
_tmp87F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp87F)));}{struct Cyc_List_List*_tmp880;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3F2,((
_tmp880=_region_malloc(env->r,sizeof(*_tmp880)),((_tmp880->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v,_tmp3F3),((_tmp880->tl=
flds,_tmp880)))))));};_LL23E:;_LL23F: {struct _tuple14 _tmp881;return(_tmp881.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp881.f2=Cyc_CfFlowInfo_UnknownL(),_tmp881)));}
_LL227:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp43B;union Cyc_CfFlowInfo_AbsLVal _tmp43C;struct
_tuple14 _tmp43A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp43B=_tmp43A.f1;_tmp43C=_tmp43A.f2;{struct _tuple14 _tmp882;return(_tmp882.f1=
_tmp43B,((_tmp882.f2=_tmp43C,_tmp882)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp43E=env->fenv;{void*_tmp43F=e->r;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*
_tmp442;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Exp*
_tmp446;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Exp*
_tmp44B;struct Cyc_Absyn_Exp*_tmp44C;enum Cyc_Absyn_Primop _tmp44E;struct Cyc_List_List*
_tmp44F;struct Cyc_List_List _tmp450;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_List_List*
_tmp452;enum Cyc_Absyn_Primop _tmp454;struct Cyc_List_List*_tmp455;_LL261: {struct
Cyc_Absyn_Conditional_e_struct*_tmp440=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp43F;if(_tmp440->tag != 6)goto _LL263;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;
_tmp443=_tmp440->f3;}}_LL262: {union Cyc_CfFlowInfo_FlowInfo _tmp457;union Cyc_CfFlowInfo_FlowInfo
_tmp458;struct _tuple15 _tmp456=Cyc_NewControlFlow_anal_test(env,inflow,_tmp441);
_tmp457=_tmp456.f1;_tmp458=_tmp456.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp45A;union
Cyc_CfFlowInfo_FlowInfo _tmp45B;struct _tuple15 _tmp459=Cyc_NewControlFlow_anal_test(
env,_tmp457,_tmp442);_tmp45A=_tmp459.f1;_tmp45B=_tmp459.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp45D;union Cyc_CfFlowInfo_FlowInfo _tmp45E;struct _tuple15 _tmp45C=Cyc_NewControlFlow_anal_test(
env,_tmp458,_tmp443);_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;{struct _tuple15
_tmp883;return(_tmp883.f1=Cyc_CfFlowInfo_join_flow(_tmp43E,env->all_changed,
_tmp45A,_tmp45D),((_tmp883.f2=Cyc_CfFlowInfo_join_flow(_tmp43E,env->all_changed,
_tmp45B,_tmp45E),_tmp883)));};};};}_LL263: {struct Cyc_Absyn_And_e_struct*_tmp444=(
struct Cyc_Absyn_And_e_struct*)_tmp43F;if(_tmp444->tag != 7)goto _LL265;else{
_tmp445=_tmp444->f1;_tmp446=_tmp444->f2;}}_LL264: {union Cyc_CfFlowInfo_FlowInfo
_tmp461;union Cyc_CfFlowInfo_FlowInfo _tmp462;struct _tuple15 _tmp460=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp445);_tmp461=_tmp460.f1;_tmp462=_tmp460.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp464;union Cyc_CfFlowInfo_FlowInfo _tmp465;struct _tuple15 _tmp463=Cyc_NewControlFlow_anal_test(
env,_tmp461,_tmp446);_tmp464=_tmp463.f1;_tmp465=_tmp463.f2;{struct _tuple15
_tmp884;return(_tmp884.f1=_tmp464,((_tmp884.f2=Cyc_CfFlowInfo_join_flow(_tmp43E,
env->all_changed,_tmp462,_tmp465),_tmp884)));};};}_LL265: {struct Cyc_Absyn_Or_e_struct*
_tmp447=(struct Cyc_Absyn_Or_e_struct*)_tmp43F;if(_tmp447->tag != 8)goto _LL267;
else{_tmp448=_tmp447->f1;_tmp449=_tmp447->f2;}}_LL266: {union Cyc_CfFlowInfo_FlowInfo
_tmp468;union Cyc_CfFlowInfo_FlowInfo _tmp469;struct _tuple15 _tmp467=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp448);_tmp468=_tmp467.f1;_tmp469=_tmp467.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp46B;union Cyc_CfFlowInfo_FlowInfo _tmp46C;struct _tuple15 _tmp46A=Cyc_NewControlFlow_anal_test(
env,_tmp469,_tmp449);_tmp46B=_tmp46A.f1;_tmp46C=_tmp46A.f2;{struct _tuple15
_tmp885;return(_tmp885.f1=Cyc_CfFlowInfo_join_flow(_tmp43E,env->all_changed,
_tmp468,_tmp46B),((_tmp885.f2=_tmp46C,_tmp885)));};};}_LL267: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp44A=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp43F;if(_tmp44A->tag != 9)goto _LL269;
else{_tmp44B=_tmp44A->f1;_tmp44C=_tmp44A->f2;}}_LL268: {union Cyc_CfFlowInfo_FlowInfo
_tmp46F;void*_tmp470;struct _tuple13 _tmp46E=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp44B);_tmp46F=_tmp46E.f1;_tmp470=_tmp46E.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp46F,_tmp44C);}_LL269: {struct Cyc_Absyn_Primop_e_struct*_tmp44D=(struct
Cyc_Absyn_Primop_e_struct*)_tmp43F;if(_tmp44D->tag != 3)goto _LL26B;else{_tmp44E=
_tmp44D->f1;if(_tmp44E != Cyc_Absyn_Not)goto _LL26B;_tmp44F=_tmp44D->f2;if(_tmp44F
== 0)goto _LL26B;_tmp450=*_tmp44F;_tmp451=(struct Cyc_Absyn_Exp*)_tmp450.hd;
_tmp452=_tmp450.tl;if(_tmp452 != 0)goto _LL26B;}}_LL26A: {union Cyc_CfFlowInfo_FlowInfo
_tmp472;union Cyc_CfFlowInfo_FlowInfo _tmp473;struct _tuple15 _tmp471=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp451);_tmp472=_tmp471.f1;_tmp473=_tmp471.f2;{struct _tuple15 _tmp886;
return(_tmp886.f1=_tmp473,((_tmp886.f2=_tmp472,_tmp886)));};}_LL26B: {struct Cyc_Absyn_Primop_e_struct*
_tmp453=(struct Cyc_Absyn_Primop_e_struct*)_tmp43F;if(_tmp453->tag != 3)goto _LL26D;
else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}}_LL26C: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp475=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp477;struct
Cyc_List_List*_tmp478;struct _tuple19 _tmp476=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp475,env,inflow,_tmp455,0,0);_tmp477=_tmp476.f1;_tmp478=_tmp476.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp478))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp478->tl))->hd;f=_tmp477;}{union Cyc_CfFlowInfo_FlowInfo _tmp479=f;
int _tmp47A;struct _tuple12 _tmp47B;struct Cyc_Dict_Dict _tmp47C;struct Cyc_List_List*
_tmp47D;_LL270: if((_tmp479.BottomFL).tag != 1)goto _LL272;_tmp47A=(int)(_tmp479.BottomFL).val;
_LL271: {struct _tuple15 _tmp887;return(_tmp887.f1=f,((_tmp887.f2=f,_tmp887)));}
_LL272: if((_tmp479.ReachableFL).tag != 2)goto _LL26F;_tmp47B=(struct _tuple12)(
_tmp479.ReachableFL).val;_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;_LL273: {struct
Cyc_Absyn_Exp*_tmp47F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp455))->hd;struct Cyc_Absyn_Exp*_tmp480=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp455->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp47C,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp88A;void*_tmp889;(_tmp889=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp455->hd)->loc,((_tmp88A="expression may not be initialized",
_tag_dyneither(_tmp88A,sizeof(char),34))),_tag_dyneither(_tmp889,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp47C,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp88D;void*_tmp88C;(_tmp88C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp455->tl))->hd)->loc,((_tmp88D="expression may not be initialized",
_tag_dyneither(_tmp88D,sizeof(char),34))),_tag_dyneither(_tmp88C,sizeof(void*),0)));}
if(_tmp454 == Cyc_Absyn_Eq  || _tmp454 == Cyc_Absyn_Neq){struct _tuple0 _tmp88E;
struct _tuple0 _tmp486=(_tmp88E.f1=r1,((_tmp88E.f2=r2,_tmp88E)));void*_tmp487;
enum Cyc_CfFlowInfo_InitLevel _tmp489;void*_tmp48A;void*_tmp48C;void*_tmp48E;
enum Cyc_CfFlowInfo_InitLevel _tmp490;void*_tmp491;void*_tmp493;void*_tmp495;void*
_tmp497;void*_tmp499;void*_tmp49B;void*_tmp49D;void*_tmp49F;void*_tmp4A1;void*
_tmp4A3;void*_tmp4A5;void*_tmp4A7;void*_tmp4A9;void*_tmp4AB;_LL275: _tmp487=
_tmp486.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp488=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp487;if(_tmp488->tag != 3)goto _LL277;else{_tmp489=_tmp488->f1;}};_tmp48A=
_tmp486.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp48B=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp48A;if(_tmp48B->tag != 0)goto _LL277;};_LL276: {union Cyc_CfFlowInfo_FlowInfo
_tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;struct _tuple15 _tmp4AD=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp47F,_tmp489);_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;switch(_tmp454){
case Cyc_Absyn_Eq: _LL289: {struct _tuple15 _tmp88F;return(_tmp88F.f1=_tmp4AF,((
_tmp88F.f2=_tmp4AE,_tmp88F)));}case Cyc_Absyn_Neq: _LL28A: {struct _tuple15 _tmp890;
return(_tmp890.f1=_tmp4AE,((_tmp890.f2=_tmp4AF,_tmp890)));}default: _LL28B: {
struct Cyc_Core_Impossible_struct _tmp896;const char*_tmp895;struct Cyc_Core_Impossible_struct*
_tmp894;(int)_throw((void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((
_tmp896.tag=Cyc_Core_Impossible,((_tmp896.f1=((_tmp895="anal_test, zero-split",
_tag_dyneither(_tmp895,sizeof(char),22))),_tmp896)))),_tmp894)))));}}}_LL277:
_tmp48C=_tmp486.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp48D=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp48C;if(_tmp48D->tag != 0)goto _LL279;};_tmp48E=_tmp486.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp48F=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp48E;if(_tmp48F->tag != 3)goto
_LL279;else{_tmp490=_tmp48F->f1;}};_LL278: {union Cyc_CfFlowInfo_FlowInfo _tmp4B6;
union Cyc_CfFlowInfo_FlowInfo _tmp4B7;struct _tuple15 _tmp4B5=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp480,_tmp490);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;switch(_tmp454){
case Cyc_Absyn_Eq: _LL28D: {struct _tuple15 _tmp897;return(_tmp897.f1=_tmp4B7,((
_tmp897.f2=_tmp4B6,_tmp897)));}case Cyc_Absyn_Neq: _LL28E: {struct _tuple15 _tmp898;
return(_tmp898.f1=_tmp4B6,((_tmp898.f2=_tmp4B7,_tmp898)));}default: _LL28F: {
struct Cyc_Core_Impossible_struct _tmp89E;const char*_tmp89D;struct Cyc_Core_Impossible_struct*
_tmp89C;(int)_throw((void*)((_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C[0]=((
_tmp89E.tag=Cyc_Core_Impossible,((_tmp89E.f1=((_tmp89D="anal_test, zero-split",
_tag_dyneither(_tmp89D,sizeof(char),22))),_tmp89E)))),_tmp89C)))));}}}_LL279:
_tmp491=_tmp486.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp492=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp491;if(_tmp492->tag != 0)goto _LL27B;};_tmp493=_tmp486.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp494=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp493;if(_tmp494->tag != 0)goto
_LL27B;};_LL27A: if(_tmp454 == Cyc_Absyn_Eq){struct _tuple15 _tmp89F;return(_tmp89F.f1=
f,((_tmp89F.f2=Cyc_CfFlowInfo_BottomFL(),_tmp89F)));}else{struct _tuple15 _tmp8A0;
return(_tmp8A0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A0.f2=f,_tmp8A0)));}_LL27B:
_tmp495=_tmp486.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp496=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp495;if(_tmp496->tag != 0)goto _LL27D;};_tmp497=_tmp486.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp498=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp497;if(_tmp498->tag != 1)
goto _LL27D;};_LL27C: goto _LL27E;_LL27D: _tmp499=_tmp486.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp49A=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp499;if(_tmp49A->tag != 0)goto
_LL27F;};_tmp49B=_tmp486.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp49C=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp49B;if(_tmp49C->tag != 2)goto _LL27F;};
_LL27E: goto _LL280;_LL27F: _tmp49D=_tmp486.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp49E=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp49D;if(_tmp49E->tag != 0)goto
_LL281;};_tmp49F=_tmp486.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4A0=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp49F;if(_tmp4A0->tag != 5)goto _LL281;};
_LL280: goto _LL282;_LL281: _tmp4A1=_tmp486.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4A2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4A1;if(_tmp4A2->tag != 1)
goto _LL283;};_tmp4A3=_tmp486.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4A4=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4A3;if(_tmp4A4->tag != 0)goto _LL283;};
_LL282: goto _LL284;_LL283: _tmp4A5=_tmp486.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4A6=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4A5;if(_tmp4A6->tag != 2)
goto _LL285;};_tmp4A7=_tmp486.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4A8=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4A7;if(_tmp4A8->tag != 0)goto _LL285;};
_LL284: goto _LL286;_LL285: _tmp4A9=_tmp486.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4AA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4A9;if(_tmp4AA->tag != 5)goto
_LL287;};_tmp4AB=_tmp486.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4AC=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4AB;if(_tmp4AC->tag != 0)goto _LL287;};_LL286: if(_tmp454 == Cyc_Absyn_Neq){
struct _tuple15 _tmp8A1;return(_tmp8A1.f1=f,((_tmp8A1.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8A1)));}else{struct _tuple15 _tmp8A2;return(_tmp8A2.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8A2.f2=f,_tmp8A2)));}_LL287:;_LL288: goto _LL274;_LL274:;}{struct _tuple0
_tmp8A3;struct _tuple0 _tmp4C2=(_tmp8A3.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp47F->topt))->v),((_tmp8A3.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp480->topt))->v),_tmp8A3)));void*_tmp4C3;enum Cyc_Absyn_Sign
_tmp4C5;void*_tmp4C6;enum Cyc_Absyn_Sign _tmp4C8;void*_tmp4C9;void*_tmp4CB;_LL292:
_tmp4C3=_tmp4C2.f1;{struct Cyc_Absyn_IntType_struct*_tmp4C4=(struct Cyc_Absyn_IntType_struct*)
_tmp4C3;if(_tmp4C4->tag != 6)goto _LL294;else{_tmp4C5=_tmp4C4->f1;if(_tmp4C5 != Cyc_Absyn_Unsigned)
goto _LL294;}};_LL293: goto _LL295;_LL294: _tmp4C6=_tmp4C2.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4C7=(struct Cyc_Absyn_IntType_struct*)_tmp4C6;if(_tmp4C7->tag != 6)goto _LL296;
else{_tmp4C8=_tmp4C7->f1;if(_tmp4C8 != Cyc_Absyn_Unsigned)goto _LL296;}};_LL295:
goto _LL297;_LL296: _tmp4C9=_tmp4C2.f1;{struct Cyc_Absyn_TagType_struct*_tmp4CA=(
struct Cyc_Absyn_TagType_struct*)_tmp4C9;if(_tmp4CA->tag != 20)goto _LL298;};_LL297:
goto _LL299;_LL298: _tmp4CB=_tmp4C2.f2;{struct Cyc_Absyn_TagType_struct*_tmp4CC=(
struct Cyc_Absyn_TagType_struct*)_tmp4CB;if(_tmp4CC->tag != 20)goto _LL29A;};_LL299:
goto _LL291;_LL29A:;_LL29B: {struct _tuple15 _tmp8A4;return(_tmp8A4.f1=f,((_tmp8A4.f2=
f,_tmp8A4)));}_LL291:;}switch(_tmp454){case Cyc_Absyn_Eq: _LL29C: goto _LL29D;case
Cyc_Absyn_Neq: _LL29D: goto _LL29E;case Cyc_Absyn_Gt: _LL29E: goto _LL29F;case Cyc_Absyn_Gte:
_LL29F: {struct _tuple15 _tmp8A5;return(_tmp8A5.f1=f,((_tmp8A5.f2=f,_tmp8A5)));}
case Cyc_Absyn_Lt: _LL2A0: {union Cyc_CfFlowInfo_FlowInfo _tmp4CF=f;union Cyc_CfFlowInfo_FlowInfo
_tmp4D0=f;{union Cyc_CfFlowInfo_FlowInfo _tmp4D1=_tmp4CF;int _tmp4D2;struct _tuple12
_tmp4D3;struct Cyc_Dict_Dict _tmp4D4;_LL2A3: if((_tmp4D1.BottomFL).tag != 1)goto
_LL2A5;_tmp4D2=(int)(_tmp4D1.BottomFL).val;_LL2A4: {struct Cyc_Core_Impossible_struct
_tmp8AB;const char*_tmp8AA;struct Cyc_Core_Impossible_struct*_tmp8A9;(int)_throw((
void*)((_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=((_tmp8AB.tag=Cyc_Core_Impossible,((
_tmp8AB.f1=((_tmp8AA="anal_test, Lt",_tag_dyneither(_tmp8AA,sizeof(char),14))),
_tmp8AB)))),_tmp8A9)))));}_LL2A5: if((_tmp4D1.ReachableFL).tag != 2)goto _LL2A2;
_tmp4D3=(struct _tuple12)(_tmp4D1.ReachableFL).val;_tmp4D4=_tmp4D3.f1;_LL2A6:
_tmp47C=_tmp4D4;_LL2A2:;}{void*_tmp4D8=_tmp47F->r;void*_tmp4DA;struct Cyc_Absyn_Vardecl*
_tmp4DC;void*_tmp4DE;struct Cyc_Absyn_Vardecl*_tmp4E0;void*_tmp4E2;struct Cyc_Absyn_Vardecl*
_tmp4E4;void*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;_LL2A8:{struct Cyc_Absyn_Var_e_struct*
_tmp4D9=(struct Cyc_Absyn_Var_e_struct*)_tmp4D8;if(_tmp4D9->tag != 1)goto _LL2AA;
else{_tmp4DA=(void*)_tmp4D9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4DB=(struct
Cyc_Absyn_Global_b_struct*)_tmp4DA;if(_tmp4DB->tag != 1)goto _LL2AA;else{_tmp4DC=
_tmp4DB->f1;}};}}if(!(!_tmp4DC->escapes))goto _LL2AA;_LL2A9: _tmp4E0=_tmp4DC;goto
_LL2AB;_LL2AA:{struct Cyc_Absyn_Var_e_struct*_tmp4DD=(struct Cyc_Absyn_Var_e_struct*)
_tmp4D8;if(_tmp4DD->tag != 1)goto _LL2AC;else{_tmp4DE=(void*)_tmp4DD->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4DF=(struct Cyc_Absyn_Local_b_struct*)_tmp4DE;if(
_tmp4DF->tag != 4)goto _LL2AC;else{_tmp4E0=_tmp4DF->f1;}};}}if(!(!_tmp4E0->escapes))
goto _LL2AC;_LL2AB: _tmp4E4=_tmp4E0;goto _LL2AD;_LL2AC:{struct Cyc_Absyn_Var_e_struct*
_tmp4E1=(struct Cyc_Absyn_Var_e_struct*)_tmp4D8;if(_tmp4E1->tag != 1)goto _LL2AE;
else{_tmp4E2=(void*)_tmp4E1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4E3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4E2;if(_tmp4E3->tag != 5)goto _LL2AE;else{_tmp4E4=_tmp4E3->f1;}};}}if(!(!
_tmp4E4->escapes))goto _LL2AE;_LL2AD: _tmp4E8=_tmp4E4;goto _LL2AF;_LL2AE:{struct Cyc_Absyn_Var_e_struct*
_tmp4E5=(struct Cyc_Absyn_Var_e_struct*)_tmp4D8;if(_tmp4E5->tag != 1)goto _LL2B0;
else{_tmp4E6=(void*)_tmp4E5->f2;{struct Cyc_Absyn_Param_b_struct*_tmp4E7=(struct
Cyc_Absyn_Param_b_struct*)_tmp4E6;if(_tmp4E7->tag != 3)goto _LL2B0;else{_tmp4E8=
_tmp4E7->f1;}};}}if(!(!_tmp4E8->escapes))goto _LL2B0;_LL2AF: {void*_tmp4E9=
_tmp480->r;void*_tmp4EB;struct Cyc_Absyn_Vardecl*_tmp4ED;void*_tmp4EF;struct Cyc_Absyn_Vardecl*
_tmp4F1;void*_tmp4F3;struct Cyc_Absyn_Vardecl*_tmp4F5;void*_tmp4F7;struct Cyc_Absyn_Vardecl*
_tmp4F9;union Cyc_Absyn_Cnst _tmp4FB;struct _tuple6 _tmp4FC;int _tmp4FD;struct Cyc_Absyn_Exp*
_tmp4FF;struct Cyc_Absyn_Exp _tmp500;void*_tmp501;union Cyc_Absyn_Cnst _tmp503;
struct _tuple6 _tmp504;int _tmp505;enum Cyc_Absyn_Primop _tmp507;struct Cyc_List_List*
_tmp508;struct Cyc_List_List _tmp509;struct Cyc_Absyn_Exp*_tmp50A;_LL2B3:{struct Cyc_Absyn_Var_e_struct*
_tmp4EA=(struct Cyc_Absyn_Var_e_struct*)_tmp4E9;if(_tmp4EA->tag != 1)goto _LL2B5;
else{_tmp4EB=(void*)_tmp4EA->f2;{struct Cyc_Absyn_Global_b_struct*_tmp4EC=(struct
Cyc_Absyn_Global_b_struct*)_tmp4EB;if(_tmp4EC->tag != 1)goto _LL2B5;else{_tmp4ED=
_tmp4EC->f1;}};}}if(!(!_tmp4ED->escapes))goto _LL2B5;_LL2B4: _tmp4F1=_tmp4ED;goto
_LL2B6;_LL2B5:{struct Cyc_Absyn_Var_e_struct*_tmp4EE=(struct Cyc_Absyn_Var_e_struct*)
_tmp4E9;if(_tmp4EE->tag != 1)goto _LL2B7;else{_tmp4EF=(void*)_tmp4EE->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp4F0=(struct Cyc_Absyn_Local_b_struct*)_tmp4EF;if(
_tmp4F0->tag != 4)goto _LL2B7;else{_tmp4F1=_tmp4F0->f1;}};}}if(!(!_tmp4F1->escapes))
goto _LL2B7;_LL2B6: _tmp4F5=_tmp4F1;goto _LL2B8;_LL2B7:{struct Cyc_Absyn_Var_e_struct*
_tmp4F2=(struct Cyc_Absyn_Var_e_struct*)_tmp4E9;if(_tmp4F2->tag != 1)goto _LL2B9;
else{_tmp4F3=(void*)_tmp4F2->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp4F4=(struct Cyc_Absyn_Pat_b_struct*)
_tmp4F3;if(_tmp4F4->tag != 5)goto _LL2B9;else{_tmp4F5=_tmp4F4->f1;}};}}if(!(!
_tmp4F5->escapes))goto _LL2B9;_LL2B8: _tmp4F9=_tmp4F5;goto _LL2BA;_LL2B9:{struct Cyc_Absyn_Var_e_struct*
_tmp4F6=(struct Cyc_Absyn_Var_e_struct*)_tmp4E9;if(_tmp4F6->tag != 1)goto _LL2BB;
else{_tmp4F7=(void*)_tmp4F6->f2;{struct Cyc_Absyn_Param_b_struct*_tmp4F8=(struct
Cyc_Absyn_Param_b_struct*)_tmp4F7;if(_tmp4F8->tag != 3)goto _LL2BB;else{_tmp4F9=
_tmp4F8->f1;}};}}if(!(!_tmp4F9->escapes))goto _LL2BB;_LL2BA: {struct _RegionHandle*
_tmp50B=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8AE;struct Cyc_List_List*
_tmp8AD;struct Cyc_List_List*_tmp50C=(_tmp8AD=_region_malloc(_tmp50B,sizeof(*
_tmp8AD)),((_tmp8AD->hd=((_tmp8AE=_region_malloc(_tmp50B,sizeof(*_tmp8AE)),((
_tmp8AE->vd=_tmp4E8,((_tmp8AE->rop=Cyc_CfFlowInfo_LessVar(_tmp4F9,_tmp4F9->type),
_tmp8AE)))))),((_tmp8AD->tl=_tmp47D,_tmp8AD)))));struct _tuple15 _tmp8AF;return(
_tmp8AF.f1=Cyc_CfFlowInfo_ReachableFL(_tmp47C,_tmp50C),((_tmp8AF.f2=_tmp4D0,
_tmp8AF)));}_LL2BB: {struct Cyc_Absyn_Const_e_struct*_tmp4FA=(struct Cyc_Absyn_Const_e_struct*)
_tmp4E9;if(_tmp4FA->tag != 0)goto _LL2BD;else{_tmp4FB=_tmp4FA->f1;if((_tmp4FB.Int_c).tag
!= 4)goto _LL2BD;_tmp4FC=(struct _tuple6)(_tmp4FB.Int_c).val;_tmp4FD=_tmp4FC.f2;}}
_LL2BC: _tmp505=_tmp4FD;goto _LL2BE;_LL2BD: {struct Cyc_Absyn_Cast_e_struct*_tmp4FE=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4E9;if(_tmp4FE->tag != 15)goto _LL2BF;else{
_tmp4FF=_tmp4FE->f2;_tmp500=*_tmp4FF;_tmp501=_tmp500.r;{struct Cyc_Absyn_Const_e_struct*
_tmp502=(struct Cyc_Absyn_Const_e_struct*)_tmp501;if(_tmp502->tag != 0)goto _LL2BF;
else{_tmp503=_tmp502->f1;if((_tmp503.Int_c).tag != 4)goto _LL2BF;_tmp504=(struct
_tuple6)(_tmp503.Int_c).val;_tmp505=_tmp504.f2;}};}}_LL2BE: {struct _RegionHandle*
_tmp510=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8B2;struct Cyc_List_List*
_tmp8B1;struct Cyc_List_List*_tmp511=(_tmp8B1=_region_malloc(_tmp510,sizeof(*
_tmp8B1)),((_tmp8B1->hd=((_tmp8B2=_region_malloc(_tmp510,sizeof(*_tmp8B2)),((
_tmp8B2->vd=_tmp4E8,((_tmp8B2->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp505),_tmp8B2)))))),((_tmp8B1->tl=_tmp47D,_tmp8B1)))));struct _tuple15 _tmp8B3;
return(_tmp8B3.f1=Cyc_CfFlowInfo_ReachableFL(_tmp47C,_tmp511),((_tmp8B3.f2=
_tmp4D0,_tmp8B3)));}_LL2BF: {struct Cyc_Absyn_Primop_e_struct*_tmp506=(struct Cyc_Absyn_Primop_e_struct*)
_tmp4E9;if(_tmp506->tag != 3)goto _LL2C1;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;
if(_tmp508 == 0)goto _LL2C1;_tmp509=*_tmp508;_tmp50A=(struct Cyc_Absyn_Exp*)_tmp509.hd;}}
_LL2C0: {void*_tmp515=_tmp50A->r;void*_tmp517;struct Cyc_Absyn_Vardecl*_tmp519;
void*_tmp51B;struct Cyc_Absyn_Vardecl*_tmp51D;void*_tmp51F;struct Cyc_Absyn_Vardecl*
_tmp521;void*_tmp523;struct Cyc_Absyn_Vardecl*_tmp525;_LL2C4:{struct Cyc_Absyn_Var_e_struct*
_tmp516=(struct Cyc_Absyn_Var_e_struct*)_tmp515;if(_tmp516->tag != 1)goto _LL2C6;
else{_tmp517=(void*)_tmp516->f2;{struct Cyc_Absyn_Global_b_struct*_tmp518=(struct
Cyc_Absyn_Global_b_struct*)_tmp517;if(_tmp518->tag != 1)goto _LL2C6;else{_tmp519=
_tmp518->f1;}};}}if(!(!_tmp519->escapes))goto _LL2C6;_LL2C5: _tmp51D=_tmp519;goto
_LL2C7;_LL2C6:{struct Cyc_Absyn_Var_e_struct*_tmp51A=(struct Cyc_Absyn_Var_e_struct*)
_tmp515;if(_tmp51A->tag != 1)goto _LL2C8;else{_tmp51B=(void*)_tmp51A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp51C=(struct Cyc_Absyn_Local_b_struct*)_tmp51B;if(
_tmp51C->tag != 4)goto _LL2C8;else{_tmp51D=_tmp51C->f1;}};}}if(!(!_tmp51D->escapes))
goto _LL2C8;_LL2C7: _tmp521=_tmp51D;goto _LL2C9;_LL2C8:{struct Cyc_Absyn_Var_e_struct*
_tmp51E=(struct Cyc_Absyn_Var_e_struct*)_tmp515;if(_tmp51E->tag != 1)goto _LL2CA;
else{_tmp51F=(void*)_tmp51E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp520=(struct Cyc_Absyn_Pat_b_struct*)
_tmp51F;if(_tmp520->tag != 5)goto _LL2CA;else{_tmp521=_tmp520->f1;}};}}if(!(!
_tmp521->escapes))goto _LL2CA;_LL2C9: _tmp525=_tmp521;goto _LL2CB;_LL2CA:{struct Cyc_Absyn_Var_e_struct*
_tmp522=(struct Cyc_Absyn_Var_e_struct*)_tmp515;if(_tmp522->tag != 1)goto _LL2CC;
else{_tmp523=(void*)_tmp522->f2;{struct Cyc_Absyn_Param_b_struct*_tmp524=(struct
Cyc_Absyn_Param_b_struct*)_tmp523;if(_tmp524->tag != 3)goto _LL2CC;else{_tmp525=
_tmp524->f1;}};}}if(!(!_tmp525->escapes))goto _LL2CC;_LL2CB: {struct _RegionHandle*
_tmp526=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8B6;struct Cyc_List_List*
_tmp8B5;struct Cyc_List_List*_tmp527=(_tmp8B5=_region_malloc(_tmp526,sizeof(*
_tmp8B5)),((_tmp8B5->hd=((_tmp8B6=_region_malloc(_tmp526,sizeof(*_tmp8B6)),((
_tmp8B6->vd=_tmp4E8,((_tmp8B6->rop=Cyc_CfFlowInfo_LessNumelts(_tmp525),_tmp8B6)))))),((
_tmp8B5->tl=_tmp47D,_tmp8B5)))));struct _tuple15 _tmp8B7;return(_tmp8B7.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp47C,_tmp527),((_tmp8B7.f2=_tmp4D0,_tmp8B7)));}_LL2CC:;_LL2CD: {struct
_tuple15 _tmp8B8;return(_tmp8B8.f1=_tmp4CF,((_tmp8B8.f2=_tmp4D0,_tmp8B8)));}
_LL2C3:;}_LL2C1:;_LL2C2: {struct _tuple15 _tmp8B9;return(_tmp8B9.f1=_tmp4CF,((
_tmp8B9.f2=_tmp4D0,_tmp8B9)));}_LL2B2:;}_LL2B0:;_LL2B1: {struct _tuple15 _tmp8BA;
return(_tmp8BA.f1=_tmp4CF,((_tmp8BA.f2=_tmp4D0,_tmp8BA)));}_LL2A7:;};}case Cyc_Absyn_Lte:
_LL2A1: {union Cyc_CfFlowInfo_FlowInfo _tmp52E=f;union Cyc_CfFlowInfo_FlowInfo
_tmp52F=f;{union Cyc_CfFlowInfo_FlowInfo _tmp530=_tmp52E;int _tmp531;struct _tuple12
_tmp532;struct Cyc_Dict_Dict _tmp533;_LL2D0: if((_tmp530.BottomFL).tag != 1)goto
_LL2D2;_tmp531=(int)(_tmp530.BottomFL).val;_LL2D1: {struct Cyc_Core_Impossible_struct
_tmp8C0;const char*_tmp8BF;struct Cyc_Core_Impossible_struct*_tmp8BE;(int)_throw((
void*)((_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8C0.tag=Cyc_Core_Impossible,((
_tmp8C0.f1=((_tmp8BF="anal_test, Lte",_tag_dyneither(_tmp8BF,sizeof(char),15))),
_tmp8C0)))),_tmp8BE)))));}_LL2D2: if((_tmp530.ReachableFL).tag != 2)goto _LL2CF;
_tmp532=(struct _tuple12)(_tmp530.ReachableFL).val;_tmp533=_tmp532.f1;_LL2D3:
_tmp47C=_tmp533;_LL2CF:;}{void*_tmp537=_tmp47F->r;void*_tmp539;struct Cyc_Absyn_Vardecl*
_tmp53B;void*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp53F;void*_tmp541;struct Cyc_Absyn_Vardecl*
_tmp543;void*_tmp545;struct Cyc_Absyn_Vardecl*_tmp547;_LL2D5:{struct Cyc_Absyn_Var_e_struct*
_tmp538=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp538->tag != 1)goto _LL2D7;
else{_tmp539=(void*)_tmp538->f2;{struct Cyc_Absyn_Global_b_struct*_tmp53A=(struct
Cyc_Absyn_Global_b_struct*)_tmp539;if(_tmp53A->tag != 1)goto _LL2D7;else{_tmp53B=
_tmp53A->f1;}};}}if(!(!_tmp53B->escapes))goto _LL2D7;_LL2D6: _tmp53F=_tmp53B;goto
_LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*_tmp53C=(struct Cyc_Absyn_Var_e_struct*)
_tmp537;if(_tmp53C->tag != 1)goto _LL2D9;else{_tmp53D=(void*)_tmp53C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp53E=(struct Cyc_Absyn_Local_b_struct*)_tmp53D;if(
_tmp53E->tag != 4)goto _LL2D9;else{_tmp53F=_tmp53E->f1;}};}}if(!(!_tmp53F->escapes))
goto _LL2D9;_LL2D8: _tmp543=_tmp53F;goto _LL2DA;_LL2D9:{struct Cyc_Absyn_Var_e_struct*
_tmp540=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp540->tag != 1)goto _LL2DB;
else{_tmp541=(void*)_tmp540->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp542=(struct Cyc_Absyn_Pat_b_struct*)
_tmp541;if(_tmp542->tag != 5)goto _LL2DB;else{_tmp543=_tmp542->f1;}};}}if(!(!
_tmp543->escapes))goto _LL2DB;_LL2DA: _tmp547=_tmp543;goto _LL2DC;_LL2DB:{struct Cyc_Absyn_Var_e_struct*
_tmp544=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp544->tag != 1)goto _LL2DD;
else{_tmp545=(void*)_tmp544->f2;{struct Cyc_Absyn_Param_b_struct*_tmp546=(struct
Cyc_Absyn_Param_b_struct*)_tmp545;if(_tmp546->tag != 3)goto _LL2DD;else{_tmp547=
_tmp546->f1;}};}}if(!(!_tmp547->escapes))goto _LL2DD;_LL2DC: {void*_tmp548=
_tmp480->r;union Cyc_Absyn_Cnst _tmp54A;struct _tuple6 _tmp54B;int _tmp54C;struct Cyc_Absyn_Exp*
_tmp54E;struct Cyc_Absyn_Exp _tmp54F;void*_tmp550;union Cyc_Absyn_Cnst _tmp552;
struct _tuple6 _tmp553;int _tmp554;enum Cyc_Absyn_Primop _tmp556;struct Cyc_List_List*
_tmp557;struct Cyc_List_List _tmp558;struct Cyc_Absyn_Exp*_tmp559;_LL2E0: {struct
Cyc_Absyn_Const_e_struct*_tmp549=(struct Cyc_Absyn_Const_e_struct*)_tmp548;if(
_tmp549->tag != 0)goto _LL2E2;else{_tmp54A=_tmp549->f1;if((_tmp54A.Int_c).tag != 4)
goto _LL2E2;_tmp54B=(struct _tuple6)(_tmp54A.Int_c).val;_tmp54C=_tmp54B.f2;}}
_LL2E1: _tmp554=_tmp54C;goto _LL2E3;_LL2E2: {struct Cyc_Absyn_Cast_e_struct*_tmp54D=(
struct Cyc_Absyn_Cast_e_struct*)_tmp548;if(_tmp54D->tag != 15)goto _LL2E4;else{
_tmp54E=_tmp54D->f2;_tmp54F=*_tmp54E;_tmp550=_tmp54F.r;{struct Cyc_Absyn_Const_e_struct*
_tmp551=(struct Cyc_Absyn_Const_e_struct*)_tmp550;if(_tmp551->tag != 0)goto _LL2E4;
else{_tmp552=_tmp551->f1;if((_tmp552.Int_c).tag != 4)goto _LL2E4;_tmp553=(struct
_tuple6)(_tmp552.Int_c).val;_tmp554=_tmp553.f2;}};}}_LL2E3: {struct _RegionHandle*
_tmp55A=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8C3;struct Cyc_List_List*
_tmp8C2;struct Cyc_List_List*_tmp55B=(_tmp8C2=_region_malloc(_tmp55A,sizeof(*
_tmp8C2)),((_tmp8C2->hd=((_tmp8C3=_region_malloc(_tmp55A,sizeof(*_tmp8C3)),((
_tmp8C3->vd=_tmp547,((_tmp8C3->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp554),_tmp8C3)))))),((_tmp8C2->tl=_tmp47D,_tmp8C2)))));struct _tuple15 _tmp8C4;
return(_tmp8C4.f1=Cyc_CfFlowInfo_ReachableFL(_tmp47C,_tmp55B),((_tmp8C4.f2=
_tmp52F,_tmp8C4)));}_LL2E4: {struct Cyc_Absyn_Primop_e_struct*_tmp555=(struct Cyc_Absyn_Primop_e_struct*)
_tmp548;if(_tmp555->tag != 3)goto _LL2E6;else{_tmp556=_tmp555->f1;_tmp557=_tmp555->f2;
if(_tmp557 == 0)goto _LL2E6;_tmp558=*_tmp557;_tmp559=(struct Cyc_Absyn_Exp*)_tmp558.hd;}}
_LL2E5: {void*_tmp55F=_tmp559->r;void*_tmp561;struct Cyc_Absyn_Vardecl*_tmp563;
void*_tmp565;struct Cyc_Absyn_Vardecl*_tmp567;void*_tmp569;struct Cyc_Absyn_Vardecl*
_tmp56B;void*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56F;_LL2E9:{struct Cyc_Absyn_Var_e_struct*
_tmp560=(struct Cyc_Absyn_Var_e_struct*)_tmp55F;if(_tmp560->tag != 1)goto _LL2EB;
else{_tmp561=(void*)_tmp560->f2;{struct Cyc_Absyn_Global_b_struct*_tmp562=(struct
Cyc_Absyn_Global_b_struct*)_tmp561;if(_tmp562->tag != 1)goto _LL2EB;else{_tmp563=
_tmp562->f1;}};}}if(!(!_tmp563->escapes))goto _LL2EB;_LL2EA: _tmp567=_tmp563;goto
_LL2EC;_LL2EB:{struct Cyc_Absyn_Var_e_struct*_tmp564=(struct Cyc_Absyn_Var_e_struct*)
_tmp55F;if(_tmp564->tag != 1)goto _LL2ED;else{_tmp565=(void*)_tmp564->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp566=(struct Cyc_Absyn_Local_b_struct*)_tmp565;if(
_tmp566->tag != 4)goto _LL2ED;else{_tmp567=_tmp566->f1;}};}}if(!(!_tmp567->escapes))
goto _LL2ED;_LL2EC: _tmp56B=_tmp567;goto _LL2EE;_LL2ED:{struct Cyc_Absyn_Var_e_struct*
_tmp568=(struct Cyc_Absyn_Var_e_struct*)_tmp55F;if(_tmp568->tag != 1)goto _LL2EF;
else{_tmp569=(void*)_tmp568->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp56A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp569;if(_tmp56A->tag != 5)goto _LL2EF;else{_tmp56B=_tmp56A->f1;}};}}if(!(!
_tmp56B->escapes))goto _LL2EF;_LL2EE: _tmp56F=_tmp56B;goto _LL2F0;_LL2EF:{struct Cyc_Absyn_Var_e_struct*
_tmp56C=(struct Cyc_Absyn_Var_e_struct*)_tmp55F;if(_tmp56C->tag != 1)goto _LL2F1;
else{_tmp56D=(void*)_tmp56C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp56E=(struct
Cyc_Absyn_Param_b_struct*)_tmp56D;if(_tmp56E->tag != 3)goto _LL2F1;else{_tmp56F=
_tmp56E->f1;}};}}if(!(!_tmp56F->escapes))goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp570=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp8C7;struct Cyc_List_List*_tmp8C6;
struct Cyc_List_List*_tmp571=(_tmp8C6=_region_malloc(_tmp570->r,sizeof(*_tmp8C6)),((
_tmp8C6->hd=((_tmp8C7=_region_malloc(_tmp570->r,sizeof(*_tmp8C7)),((_tmp8C7->vd=
_tmp547,((_tmp8C7->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp56F),_tmp8C7)))))),((
_tmp8C6->tl=_tmp47D,_tmp8C6)))));struct _tuple15 _tmp8C8;return(_tmp8C8.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp47C,_tmp571),((_tmp8C8.f2=_tmp52F,_tmp8C8)));}_LL2F1:;_LL2F2: {struct
_tuple15 _tmp8C9;return(_tmp8C9.f1=_tmp52E,((_tmp8C9.f2=_tmp52F,_tmp8C9)));}
_LL2E8:;}_LL2E6:;_LL2E7: {struct _tuple15 _tmp8CA;return(_tmp8CA.f1=_tmp52E,((
_tmp8CA.f2=_tmp52F,_tmp8CA)));}_LL2DF:;}_LL2DD:;_LL2DE: {struct _tuple15 _tmp8CB;
return(_tmp8CB.f1=_tmp52E,((_tmp8CB.f2=_tmp52F,_tmp8CB)));}_LL2D4:;};}default:
_LL2CE: {struct _tuple15 _tmp8CC;return(_tmp8CC.f1=f,((_tmp8CC.f2=f,_tmp8CC)));}}}
_LL26F:;};}_LL26D:;_LL26E: goto _LL260;_LL260:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp57A;void*_tmp57B;struct _tuple13 _tmp579=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp57A=_tmp579.f1;_tmp57B=_tmp579.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp57C=_tmp57A;int _tmp57D;struct _tuple12 _tmp57E;struct Cyc_Dict_Dict _tmp57F;
_LL2F5: if((_tmp57C.BottomFL).tag != 1)goto _LL2F7;_tmp57D=(int)(_tmp57C.BottomFL).val;
_LL2F6: {struct _tuple15 _tmp8CD;return(_tmp8CD.f1=_tmp57A,((_tmp8CD.f2=_tmp57A,
_tmp8CD)));}_LL2F7: if((_tmp57C.ReachableFL).tag != 2)goto _LL2F4;_tmp57E=(struct
_tuple12)(_tmp57C.ReachableFL).val;_tmp57F=_tmp57E.f1;_LL2F8: {void*_tmp581=
_tmp57B;enum Cyc_CfFlowInfo_InitLevel _tmp587;enum Cyc_CfFlowInfo_InitLevel
_tmp589;enum Cyc_CfFlowInfo_InitLevel _tmp58C;_LL2FA: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp582=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp581;if(_tmp582->tag != 0)goto
_LL2FC;}_LL2FB: {struct _tuple15 _tmp8CE;return(_tmp8CE.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8CE.f2=_tmp57A,_tmp8CE)));}_LL2FC: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp583=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp581;if(_tmp583->tag != 2)
goto _LL2FE;}_LL2FD: goto _LL2FF;_LL2FE: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp584=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp581;if(_tmp584->tag != 1)
goto _LL300;}_LL2FF: goto _LL301;_LL300: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp585=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp581;if(_tmp585->tag != 5)goto
_LL302;}_LL301: {struct _tuple15 _tmp8CF;return(_tmp8CF.f1=_tmp57A,((_tmp8CF.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp8CF)));}_LL302: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp586=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp581;if(_tmp586->tag != 3)goto
_LL304;else{_tmp587=_tmp586->f1;if(_tmp587 != Cyc_CfFlowInfo_NoneIL)goto _LL304;}}
_LL303: goto _LL305;_LL304: {struct Cyc_CfFlowInfo_Esc_struct*_tmp588=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp581;if(_tmp588->tag != 4)goto _LL306;else{_tmp589=_tmp588->f1;if(_tmp589 != Cyc_CfFlowInfo_NoneIL)
goto _LL306;}}_LL305: goto _LL307;_LL306: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp58A=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp581;if(_tmp58A->tag != 7)goto
_LL308;}_LL307:{const char*_tmp8D2;void*_tmp8D1;(_tmp8D1=0,Cyc_Tcutil_terr(e->loc,((
_tmp8D2="expression may not be initialized",_tag_dyneither(_tmp8D2,sizeof(char),
34))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}{struct _tuple15 _tmp8D3;return(
_tmp8D3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D3.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8D3)));};_LL308: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp58B=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp581;if(_tmp58B->tag != 3)goto _LL30A;else{_tmp58C=_tmp58B->f1;}}_LL309: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp57A,e,_tmp58C);_LL30A: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp58D=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp581;if(_tmp58D->tag != 4)goto _LL30C;}
_LL30B: {struct _tuple15 _tmp8D4;return(_tmp8D4.f1=_tmp57A,((_tmp8D4.f2=_tmp57A,
_tmp8D4)));}_LL30C: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp58E=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp581;if(_tmp58E->tag != 6)goto _LL2F9;}_LL30D: {struct Cyc_Core_Impossible_struct
_tmp8DA;const char*_tmp8D9;struct Cyc_Core_Impossible_struct*_tmp8D8;(int)_throw((
void*)((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8DA.tag=Cyc_Core_Impossible,((
_tmp8DA.f1=((_tmp8D9="anal_test",_tag_dyneither(_tmp8D9,sizeof(char),10))),
_tmp8DA)))),_tmp8D8)))));}_LL2F9:;}_LL2F4:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp599;struct Cyc_Position_Segment*
_tmp59A;struct Cyc_NewControlFlow_AnalEnv*_tmp59B;struct Cyc_Dict_Dict _tmp59C;
struct _tuple22*_tmp598=ckenv;_tmp599=*_tmp598;_tmp59A=_tmp599.f1;_tmp59B=_tmp599.f2;
_tmp59C=_tmp599.f3;{void*_tmp59D=root;struct Cyc_Absyn_Vardecl*_tmp59F;_LL30F: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp59E=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp59D;if(_tmp59E->tag != 0)goto _LL311;else{_tmp59F=_tmp59E->f1;}}_LL310: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp59B->fenv)->r,_tmp59F->type)){void*_tmp5A0=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5A4;_LL314: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5A1=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5A0;if(_tmp5A1->tag != 7)goto _LL316;}_LL315: goto _LL317;_LL316: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5A2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5A0;if(_tmp5A2->tag != 0)goto
_LL318;}_LL317: goto _LL319;_LL318: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5A3=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5A0;if(_tmp5A3->tag != 3)goto _LL31A;
else{_tmp5A4=_tmp5A3->f1;if(_tmp5A4 != Cyc_CfFlowInfo_NoneIL)goto _LL31A;}}_LL319:
goto _LL313;_LL31A:;_LL31B:{const char*_tmp8DE;void*_tmp8DD[1];struct Cyc_String_pa_struct
_tmp8DC;(_tmp8DC.tag=0,((_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp59F->name)),((_tmp8DD[0]=& _tmp8DC,Cyc_Tcutil_warn(
_tmp59A,((_tmp8DE="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp8DE,sizeof(char),57))),_tag_dyneither(_tmp8DD,sizeof(void*),1)))))));}
goto _LL313;_LL313:;}goto _LL30E;_LL311:;_LL312: goto _LL30E;_LL30E:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow){union Cyc_CfFlowInfo_FlowInfo _tmp5A8=flow;int _tmp5A9;struct _tuple12 _tmp5AA;
struct Cyc_Dict_Dict _tmp5AB;_LL31D: if((_tmp5A8.BottomFL).tag != 1)goto _LL31F;
_tmp5A9=(int)(_tmp5A8.BottomFL).val;_LL31E: return;_LL31F: if((_tmp5A8.ReachableFL).tag
!= 2)goto _LL31C;_tmp5AA=(struct _tuple12)(_tmp5A8.ReachableFL).val;_tmp5AB=
_tmp5AA.f1;_LL320:{struct Cyc_List_List*_tmp5AC=env->param_roots;for(0;_tmp5AC != 
0;_tmp5AC=_tmp5AC->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5AB,Cyc_CfFlowInfo_lookup_place(
_tmp5AB,(struct Cyc_CfFlowInfo_Place*)_tmp5AC->hd))!= Cyc_CfFlowInfo_AllIL){const
char*_tmp8E1;void*_tmp8E0;(_tmp8E0=0,Cyc_Tcutil_terr(loc,((_tmp8E1="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp8E1,sizeof(char),76))),_tag_dyneither(_tmp8E0,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp8E2;struct _tuple22
_tmp5AF=(_tmp8E2.f1=loc,((_tmp8E2.f2=env,((_tmp8E2.f3=_tmp5AB,_tmp8E2)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5AF,_tmp5AB);}
return;_LL31C:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5B1=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5B3;struct Cyc_Core_Opt*_tmp5B4;
struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Stmt*_tmp5B6;struct Cyc_Position_Segment*
_tmp5B7;struct Cyc_Absyn_Switch_clause*_tmp5B2=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5B3=*_tmp5B2;_tmp5B4=_tmp5B3.pat_vars;_tmp5B5=_tmp5B3.where_clause;
_tmp5B6=_tmp5B3.body;_tmp5B7=_tmp5B3.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5B1,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5B4))->v,_tmp5B1->unknown_all,_tmp5B7);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5B5 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5B5;union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_FlowInfo
_tmp5BA;struct _tuple15 _tmp5B8=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;inflow=_tmp5BA;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp5B9,_tmp5B6);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp5B6);}{union Cyc_CfFlowInfo_FlowInfo _tmp5BB=clause_outflow;int
_tmp5BC;_LL322: if((_tmp5BB.BottomFL).tag != 1)goto _LL324;_tmp5BC=(int)(_tmp5BB.BottomFL).val;
_LL323: goto _LL321;_LL324:;_LL325: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp8E5;void*_tmp8E4;(_tmp8E4=0,Cyc_Tcutil_terr(_tmp5B6->loc,((_tmp8E5="switch clause may implicitly fallthru",
_tag_dyneither(_tmp8E5,sizeof(char),38))),_tag_dyneither(_tmp8E4,sizeof(void*),0)));}
else{const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,Cyc_Tcutil_warn(_tmp5B6->loc,((
_tmp8E8="switch clause may implicitly fallthru",_tag_dyneither(_tmp8E8,sizeof(
char),38))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL321;_LL321:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd);static void Cyc_NewControlFlow_check_dropped_unique_vd(
struct _tuple23*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp5C2;struct Cyc_NewControlFlow_AnalEnv*
_tmp5C3;struct Cyc_Dict_Dict _tmp5C4;struct Cyc_Position_Segment*_tmp5C5;struct
_tuple23*_tmp5C1=vdenv;_tmp5C2=*_tmp5C1;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;
_tmp5C5=_tmp5C2.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5C3->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp8EB;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp8EA;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5C6=(_tmp8EA=_region_malloc((
_tmp5C3->fenv)->r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=0,((_tmp8EB.f1=vd,
_tmp8EB)))),_tmp8EA)));void*_tmp5C7=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp5C4,(void*)_tmp5C6);void*_tmp5C8=_tmp5C7;enum Cyc_CfFlowInfo_InitLevel
_tmp5CC;_LL327: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5C9=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5C8;if(_tmp5C9->tag != 7)goto _LL329;}_LL328: goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5CA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5C8;if(_tmp5CA->tag != 0)goto
_LL32B;}_LL32A: goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5CB=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5C8;if(_tmp5CB->tag != 3)goto _LL32D;
else{_tmp5CC=_tmp5CB->f1;if(_tmp5CC != Cyc_CfFlowInfo_NoneIL)goto _LL32D;}}_LL32C:
goto _LL326;_LL32D:;_LL32E:{const char*_tmp8EF;void*_tmp8EE[1];struct Cyc_String_pa_struct
_tmp8ED;(_tmp8ED.tag=0,((_tmp8ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp8EE[0]=& _tmp8ED,Cyc_Tcutil_warn(_tmp5C5,((
_tmp8EF="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp8EF,sizeof(char),68))),_tag_dyneither(_tmp8EE,sizeof(void*),1)))))));}
goto _LL326;_LL326:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static void Cyc_NewControlFlow_check_dropped_unique(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo _tmp5D2=inflow;struct _tuple12
_tmp5D3;struct Cyc_Dict_Dict _tmp5D4;_LL330: if((_tmp5D2.ReachableFL).tag != 2)goto
_LL332;_tmp5D3=(struct _tuple12)(_tmp5D2.ReachableFL).val;_tmp5D4=_tmp5D3.f1;
_LL331: {struct _tuple23 _tmp8F0;struct _tuple23 _tmp5D5=(_tmp8F0.f1=env,((_tmp8F0.f2=
_tmp5D4,((_tmp8F0.f3=decl->loc,_tmp8F0)))));struct Cyc_CfFlowInfo_FlowEnv*_tmp5D6=
env->fenv;{void*_tmp5D7=decl->r;struct Cyc_Absyn_Vardecl*_tmp5D9;struct Cyc_Core_Opt*
_tmp5DB;struct Cyc_Core_Opt _tmp5DC;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*
_tmp5DF;_LL335: {struct Cyc_Absyn_Var_d_struct*_tmp5D8=(struct Cyc_Absyn_Var_d_struct*)
_tmp5D7;if(_tmp5D8->tag != 0)goto _LL337;else{_tmp5D9=_tmp5D8->f1;}}_LL336: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp5D5,_tmp5D9);goto _LL334;_LL337: {struct Cyc_Absyn_Let_d_struct*_tmp5DA=(
struct Cyc_Absyn_Let_d_struct*)_tmp5D7;if(_tmp5DA->tag != 2)goto _LL339;else{
_tmp5DB=_tmp5DA->f2;if(_tmp5DB == 0)goto _LL339;_tmp5DC=*_tmp5DB;_tmp5DD=(struct
Cyc_List_List*)_tmp5DC.v;}}_LL338: _tmp5DF=_tmp5DD;goto _LL33A;_LL339: {struct Cyc_Absyn_Letv_d_struct*
_tmp5DE=(struct Cyc_Absyn_Letv_d_struct*)_tmp5D7;if(_tmp5DE->tag != 3)goto _LL33B;
else{_tmp5DF=_tmp5DE->f1;}}_LL33A:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp5D5,_tmp5DF);goto _LL334;_LL33B:;_LL33C: goto _LL334;_LL334:;}goto _LL32F;}
_LL332:;_LL333: goto _LL32F;_LL32F:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5E2;union Cyc_CfFlowInfo_FlowInfo*_tmp5E3;struct _tuple16 _tmp5E1=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp5E2=_tmp5E1.f1;_tmp5E3=_tmp5E1.f2;inflow=*_tmp5E3;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp5E4=env->fenv;void*_tmp5E5=s->r;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*
_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_Absyn_Stmt*
_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Stmt*
_tmp5F3;struct _tuple9 _tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Stmt*
_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5FA;struct _tuple9
_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Exp*
_tmp5FF;struct _tuple9 _tmp600;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Stmt*
_tmp602;struct _tuple9 _tmp603;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Stmt*
_tmp605;struct Cyc_Absyn_Stmt*_tmp606;struct Cyc_Absyn_Stmt*_tmp608;struct Cyc_List_List*
_tmp60A;struct Cyc_Absyn_Switch_clause**_tmp60B;struct Cyc_Absyn_Switch_clause*
_tmp60C;struct Cyc_Absyn_Stmt*_tmp60E;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_Absyn_Stmt*
_tmp612;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_List_List*_tmp615;struct Cyc_Absyn_Stmt*
_tmp617;struct Cyc_List_List*_tmp618;struct Cyc_Absyn_Decl*_tmp61A;struct Cyc_Absyn_Decl
_tmp61B;void*_tmp61C;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Tvar*_tmp61F;
struct Cyc_Absyn_Vardecl*_tmp620;struct Cyc_Absyn_Stmt*_tmp621;struct Cyc_Absyn_Decl*
_tmp623;struct Cyc_Absyn_Stmt*_tmp624;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Exp*
_tmp628;_LL33E: {struct Cyc_Absyn_Skip_s_struct*_tmp5E6=(struct Cyc_Absyn_Skip_s_struct*)
_tmp5E5;if(_tmp5E6->tag != 0)goto _LL340;}_LL33F: return inflow;_LL340: {struct Cyc_Absyn_Return_s_struct*
_tmp5E7=(struct Cyc_Absyn_Return_s_struct*)_tmp5E5;if(_tmp5E7->tag != 3)goto _LL342;
else{_tmp5E8=_tmp5E7->f1;if(_tmp5E8 != 0)goto _LL342;}}_LL341: if(env->noreturn){
const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_Tcutil_terr(s->loc,((_tmp8F3="`noreturn' function might return",
_tag_dyneither(_tmp8F3,sizeof(char),33))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL342: {struct Cyc_Absyn_Return_s_struct*_tmp5E9=(struct Cyc_Absyn_Return_s_struct*)
_tmp5E5;if(_tmp5E9->tag != 3)goto _LL344;else{_tmp5EA=_tmp5E9->f1;}}_LL343: if(env->noreturn){
const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_terr(s->loc,((_tmp8F6="`noreturn' function might return",
_tag_dyneither(_tmp8F6,sizeof(char),33))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp62E;void*_tmp62F;struct _tuple13 _tmp62D=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5EA));_tmp62E=_tmp62D.f1;
_tmp62F=_tmp62D.f2;_tmp62E=Cyc_NewControlFlow_use_Rval(env,_tmp5EA->loc,_tmp62E,
_tmp62F);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp62E);return Cyc_CfFlowInfo_BottomFL();};
_LL344: {struct Cyc_Absyn_Exp_s_struct*_tmp5EB=(struct Cyc_Absyn_Exp_s_struct*)
_tmp5E5;if(_tmp5EB->tag != 1)goto _LL346;else{_tmp5EC=_tmp5EB->f1;}}_LL345: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5EC)).f1;_LL346: {struct Cyc_Absyn_Seq_s_struct*
_tmp5ED=(struct Cyc_Absyn_Seq_s_struct*)_tmp5E5;if(_tmp5ED->tag != 2)goto _LL348;
else{_tmp5EE=_tmp5ED->f1;_tmp5EF=_tmp5ED->f2;}}_LL347: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5EE),_tmp5EF);_LL348: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp5F0=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5E5;if(_tmp5F0->tag != 4)goto
_LL34A;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;_tmp5F3=_tmp5F0->f3;}}_LL349: {
union Cyc_CfFlowInfo_FlowInfo _tmp631;union Cyc_CfFlowInfo_FlowInfo _tmp632;struct
_tuple15 _tmp630=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5F1);_tmp631=_tmp630.f1;
_tmp632=_tmp630.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp633=Cyc_NewControlFlow_anal_stmt(
env,_tmp632,_tmp5F3);union Cyc_CfFlowInfo_FlowInfo _tmp634=Cyc_NewControlFlow_anal_stmt(
env,_tmp631,_tmp5F2);return Cyc_CfFlowInfo_join_flow(_tmp5E4,env->all_changed,
_tmp634,_tmp633);};}_LL34A: {struct Cyc_Absyn_While_s_struct*_tmp5F4=(struct Cyc_Absyn_While_s_struct*)
_tmp5E5;if(_tmp5F4->tag != 5)goto _LL34C;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F5.f1;
_tmp5F7=_tmp5F5.f2;_tmp5F8=_tmp5F4->f2;}}_LL34B: {union Cyc_CfFlowInfo_FlowInfo*
_tmp636;struct _tuple16 _tmp635=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp5F7);_tmp636=_tmp635.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp637=*_tmp636;union
Cyc_CfFlowInfo_FlowInfo _tmp639;union Cyc_CfFlowInfo_FlowInfo _tmp63A;struct
_tuple15 _tmp638=Cyc_NewControlFlow_anal_test(env,_tmp637,_tmp5F6);_tmp639=
_tmp638.f1;_tmp63A=_tmp638.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp63B=Cyc_NewControlFlow_anal_stmt(
env,_tmp639,_tmp5F8);Cyc_NewControlFlow_update_flow(env,_tmp5F7,_tmp63B);return
_tmp63A;};};}_LL34C: {struct Cyc_Absyn_Do_s_struct*_tmp5F9=(struct Cyc_Absyn_Do_s_struct*)
_tmp5E5;if(_tmp5F9->tag != 14)goto _LL34E;else{_tmp5FA=_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;
_tmp5FC=_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;}}_LL34D: {union Cyc_CfFlowInfo_FlowInfo
_tmp63C=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5FA);union Cyc_CfFlowInfo_FlowInfo*
_tmp63E;struct _tuple16 _tmp63D=Cyc_NewControlFlow_pre_stmt_check(env,_tmp63C,
_tmp5FD);_tmp63E=_tmp63D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp63F=*_tmp63E;union
Cyc_CfFlowInfo_FlowInfo _tmp641;union Cyc_CfFlowInfo_FlowInfo _tmp642;struct
_tuple15 _tmp640=Cyc_NewControlFlow_anal_test(env,_tmp63F,_tmp5FC);_tmp641=
_tmp640.f1;_tmp642=_tmp640.f2;Cyc_NewControlFlow_update_flow(env,_tmp5FA,_tmp641);
return _tmp642;};}_LL34E: {struct Cyc_Absyn_For_s_struct*_tmp5FE=(struct Cyc_Absyn_For_s_struct*)
_tmp5E5;if(_tmp5FE->tag != 9)goto _LL350;else{_tmp5FF=_tmp5FE->f1;_tmp600=_tmp5FE->f2;
_tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_tmp603=_tmp5FE->f3;_tmp604=_tmp603.f1;
_tmp605=_tmp603.f2;_tmp606=_tmp5FE->f4;}}_LL34F: {union Cyc_CfFlowInfo_FlowInfo
_tmp643=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5FF)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp645;struct _tuple16 _tmp644=Cyc_NewControlFlow_pre_stmt_check(env,_tmp643,
_tmp602);_tmp645=_tmp644.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp646=*_tmp645;union
Cyc_CfFlowInfo_FlowInfo _tmp648;union Cyc_CfFlowInfo_FlowInfo _tmp649;struct
_tuple15 _tmp647=Cyc_NewControlFlow_anal_test(env,_tmp646,_tmp601);_tmp648=
_tmp647.f1;_tmp649=_tmp647.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp64A=Cyc_NewControlFlow_anal_stmt(
env,_tmp648,_tmp606);union Cyc_CfFlowInfo_FlowInfo*_tmp64C;struct _tuple16 _tmp64B=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp64A,_tmp605);_tmp64C=_tmp64B.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp64D=*_tmp64C;union Cyc_CfFlowInfo_FlowInfo _tmp64E=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp64D,_tmp604)).f1;Cyc_NewControlFlow_update_flow(env,_tmp602,_tmp64E);
return _tmp649;};};};}_LL350: {struct Cyc_Absyn_Break_s_struct*_tmp607=(struct Cyc_Absyn_Break_s_struct*)
_tmp5E5;if(_tmp607->tag != 6)goto _LL352;else{_tmp608=_tmp607->f1;if(_tmp608 != 0)
goto _LL352;}}_LL351: return Cyc_CfFlowInfo_BottomFL();_LL352: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp609=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp5E5;if(_tmp609->tag != 11)goto
_LL354;else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;if(_tmp60B == 0)goto _LL354;
_tmp60C=*_tmp60B;}}_LL353: {struct _RegionHandle*_tmp64F=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp651;struct Cyc_List_List*_tmp652;struct _tuple19 _tmp650=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp64F,env,inflow,_tmp60A,1,1);_tmp651=_tmp650.f1;_tmp652=_tmp650.f2;for(0;
_tmp652 != 0;(_tmp652=_tmp652->tl,_tmp60A=_tmp60A->tl)){_tmp651=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60A))->hd)->loc,
_tmp651,(void*)_tmp652->hd);}_tmp651=Cyc_NewControlFlow_add_vars(_tmp5E4,_tmp651,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60C->pat_vars))->v,
_tmp5E4->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp60C->body,_tmp651);return Cyc_CfFlowInfo_BottomFL();}_LL354: {struct Cyc_Absyn_Break_s_struct*
_tmp60D=(struct Cyc_Absyn_Break_s_struct*)_tmp5E5;if(_tmp60D->tag != 6)goto _LL356;
else{_tmp60E=_tmp60D->f1;}}_LL355: _tmp610=_tmp60E;goto _LL357;_LL356: {struct Cyc_Absyn_Continue_s_struct*
_tmp60F=(struct Cyc_Absyn_Continue_s_struct*)_tmp5E5;if(_tmp60F->tag != 7)goto
_LL358;else{_tmp610=_tmp60F->f1;}}_LL357: _tmp612=_tmp610;goto _LL359;_LL358: {
struct Cyc_Absyn_Goto_s_struct*_tmp611=(struct Cyc_Absyn_Goto_s_struct*)_tmp5E5;
if(_tmp611->tag != 8)goto _LL35A;else{_tmp612=_tmp611->f2;}}_LL359: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp653=_tmp5E2->encloser;struct Cyc_Absyn_Stmt*
_tmp654=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp612)))->encloser;while(_tmp654 != _tmp653){struct Cyc_Absyn_Stmt*_tmp655=(Cyc_NewControlFlow_get_stmt_annot(
_tmp653))->encloser;if(_tmp655 == _tmp653){{const char*_tmp8F9;void*_tmp8F8;(
_tmp8F8=0,Cyc_Tcutil_terr(s->loc,((_tmp8F9="goto enters local scope or exception handler",
_tag_dyneither(_tmp8F9,sizeof(char),45))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}
break;}_tmp653=_tmp655;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp612),inflow);return Cyc_CfFlowInfo_BottomFL();_LL35A: {struct Cyc_Absyn_Switch_s_struct*
_tmp613=(struct Cyc_Absyn_Switch_s_struct*)_tmp5E5;if(_tmp613->tag != 10)goto
_LL35C;else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;}}_LL35B: {union Cyc_CfFlowInfo_FlowInfo
_tmp659;void*_tmp65A;struct _tuple13 _tmp658=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp614);_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;_tmp659=Cyc_NewControlFlow_use_Rval(
env,_tmp614->loc,_tmp659,_tmp65A);return Cyc_NewControlFlow_anal_scs(env,_tmp659,
_tmp615);}_LL35C: {struct Cyc_Absyn_TryCatch_s_struct*_tmp616=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5E5;if(_tmp616->tag != 15)goto _LL35E;else{_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;}}
_LL35D: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp617);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp618);{union Cyc_CfFlowInfo_FlowInfo _tmp65B=scs_outflow;int
_tmp65C;_LL369: if((_tmp65B.BottomFL).tag != 1)goto _LL36B;_tmp65C=(int)(_tmp65B.BottomFL).val;
_LL36A: goto _LL368;_LL36B:;_LL36C: {const char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,Cyc_Tcutil_terr(
s->loc,((_tmp8FC="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp8FC,sizeof(char),42))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}_LL368:;}
outflow=s1_outflow;return outflow;};};}_LL35E: {struct Cyc_Absyn_Decl_s_struct*
_tmp619=(struct Cyc_Absyn_Decl_s_struct*)_tmp5E5;if(_tmp619->tag != 12)goto _LL360;
else{_tmp61A=_tmp619->f1;_tmp61B=*_tmp61A;_tmp61C=_tmp61B.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp61D=(struct Cyc_Absyn_Alias_d_struct*)_tmp61C;if(_tmp61D->tag != 5)goto _LL360;
else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;_tmp620=_tmp61D->f3;}};_tmp621=
_tmp619->f2;}}_LL35F: {union Cyc_CfFlowInfo_FlowInfo _tmp660;union Cyc_CfFlowInfo_AbsLVal
_tmp661;struct _tuple14 _tmp65F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp61E);
_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;{struct _tuple14 _tmp8FD;struct _tuple14
_tmp663=(_tmp8FD.f1=_tmp660,((_tmp8FD.f2=_tmp661,_tmp8FD)));union Cyc_CfFlowInfo_FlowInfo
_tmp664;struct _tuple12 _tmp665;struct Cyc_Dict_Dict _tmp666;struct Cyc_List_List*
_tmp667;union Cyc_CfFlowInfo_AbsLVal _tmp668;struct Cyc_CfFlowInfo_Place*_tmp669;
union Cyc_CfFlowInfo_FlowInfo _tmp66A;int _tmp66B;_LL36E: _tmp664=_tmp663.f1;if((
_tmp664.ReachableFL).tag != 2)goto _LL370;_tmp665=(struct _tuple12)(_tmp664.ReachableFL).val;
_tmp666=_tmp665.f1;_tmp667=_tmp665.f2;_tmp668=_tmp663.f2;if((_tmp668.PlaceL).tag
!= 1)goto _LL370;_tmp669=(struct Cyc_CfFlowInfo_Place*)(_tmp668.PlaceL).val;_LL36F: {
void*_tmp66C=Cyc_CfFlowInfo_lookup_place(_tmp666,_tmp669);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp61E->topt))->v;{void*_tmp66D=_tmp61E->r;struct Cyc_Absyn_Exp*
_tmp66F;_LL375: {struct Cyc_Absyn_Cast_e_struct*_tmp66E=(struct Cyc_Absyn_Cast_e_struct*)
_tmp66D;if(_tmp66E->tag != 15)goto _LL377;else{_tmp66F=_tmp66E->f2;}}_LL376: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp66F->topt))->v;goto _LL374;_LL377:;
_LL378: goto _LL374;_LL374:;}{void*_tmp670=Cyc_CfFlowInfo_make_unique_consumed(
_tmp5E4,t,_tmp61E,env->iteration_num,_tmp66C);_tmp666=Cyc_CfFlowInfo_assign_place(
_tmp5E4,_tmp61E->loc,_tmp666,env->all_changed,_tmp669,_tmp670);_tmp660=Cyc_CfFlowInfo_ReachableFL(
_tmp666,_tmp667);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp5E4,_tmp666,
_tmp66C)){case Cyc_CfFlowInfo_AllIL: _LL379: leaf=_tmp5E4->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL37A: leaf=_tmp5E4->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL37B: leaf=_tmp5E4->unknown_this;break;}{void*_tmp671=Cyc_CfFlowInfo_typ_to_absrval(
_tmp5E4,t,leaf);_tmp660=Cyc_NewControlFlow_use_Rval(env,_tmp61E->loc,_tmp660,
_tmp66C);{struct Cyc_List_List _tmp8FE;struct Cyc_List_List _tmp672=(_tmp8FE.hd=
_tmp620,((_tmp8FE.tl=0,_tmp8FE)));_tmp660=Cyc_NewControlFlow_add_vars(_tmp5E4,
_tmp660,(struct Cyc_List_List*)& _tmp672,_tmp5E4->unknown_all,s->loc);_tmp660=Cyc_NewControlFlow_anal_stmt(
env,_tmp660,_tmp621);{union Cyc_CfFlowInfo_FlowInfo _tmp673=_tmp660;struct _tuple12
_tmp674;struct Cyc_Dict_Dict _tmp675;struct Cyc_List_List*_tmp676;_LL37E: if((
_tmp673.ReachableFL).tag != 2)goto _LL380;_tmp674=(struct _tuple12)(_tmp673.ReachableFL).val;
_tmp675=_tmp674.f1;_tmp676=_tmp674.f2;_LL37F: _tmp675=Cyc_CfFlowInfo_assign_place(
_tmp5E4,s->loc,_tmp675,env->all_changed,_tmp669,_tmp671);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp675,_tmp676);return outflow;_LL380:;_LL381: return _tmp660;_LL37D:;};};};};};}
_LL370: _tmp66A=_tmp663.f1;if((_tmp66A.BottomFL).tag != 1)goto _LL372;_tmp66B=(int)(
_tmp66A.BottomFL).val;_LL371: return _tmp660;_LL372:;_LL373:{const char*_tmp901;
void*_tmp900;(_tmp900=0,Cyc_Tcutil_terr(_tmp61E->loc,((_tmp901="bad alias expression--no unique path found",
_tag_dyneither(_tmp901,sizeof(char),43))),_tag_dyneither(_tmp900,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL36D:;};}_LL360: {struct Cyc_Absyn_Decl_s_struct*
_tmp622=(struct Cyc_Absyn_Decl_s_struct*)_tmp5E5;if(_tmp622->tag != 12)goto _LL362;
else{_tmp623=_tmp622->f1;_tmp624=_tmp622->f2;}}_LL361: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp623),_tmp624);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp623);return outflow;
_LL362: {struct Cyc_Absyn_Label_s_struct*_tmp625=(struct Cyc_Absyn_Label_s_struct*)
_tmp5E5;if(_tmp625->tag != 13)goto _LL364;else{_tmp626=_tmp625->f2;}}_LL363: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp626);_LL364: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp627=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5E5;if(_tmp627->tag != 16)goto
_LL366;else{_tmp628=_tmp627->f1;}}_LL365: {union Cyc_CfFlowInfo_FlowInfo _tmp67B;
void*_tmp67C;struct _tuple13 _tmp67A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp628);_tmp67B=_tmp67A.f1;_tmp67C=_tmp67A.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp67D=Cyc_NewControlFlow_use_Rval(env,_tmp628->loc,_tmp67B,_tmp67C);void*
_tmp67E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp628->topt))->v);
void*_tmp680;_LL383: {struct Cyc_Absyn_RgnHandleType_struct*_tmp67F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp67E;if(_tmp67F->tag != 16)goto _LL385;else{_tmp680=(void*)_tmp67F->f1;}}_LL384:
return Cyc_CfFlowInfo_kill_flow_region(_tmp5E4,_tmp67B,_tmp680);_LL385:;_LL386: {
struct Cyc_Core_Impossible_struct _tmp907;const char*_tmp906;struct Cyc_Core_Impossible_struct*
_tmp905;(int)_throw((void*)((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905[0]=((
_tmp907.tag=Cyc_Core_Impossible,((_tmp907.f1=((_tmp906="anal_stmt -- reset_region",
_tag_dyneither(_tmp906,sizeof(char),26))),_tmp907)))),_tmp905)))));}_LL382:;};}
_LL366:;_LL367: {struct Cyc_Core_Impossible_struct _tmp90D;const char*_tmp90C;
struct Cyc_Core_Impossible_struct*_tmp90B;(int)_throw((void*)((_tmp90B=_cycalloc(
sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=Cyc_Core_Impossible,((_tmp90D.f1=((
_tmp90C="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp90C,sizeof(char),56))),_tmp90D)))),_tmp90B)))));}_LL33D:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp687=env->fenv;
void*_tmp688=decl->r;struct Cyc_Absyn_Vardecl*_tmp68A;struct Cyc_Core_Opt*_tmp68C;
struct Cyc_Core_Opt _tmp68D;struct Cyc_List_List*_tmp68E;struct Cyc_Absyn_Exp*
_tmp68F;struct Cyc_List_List*_tmp691;struct Cyc_Absyn_Fndecl*_tmp693;struct Cyc_Absyn_Tvar*
_tmp695;struct Cyc_Absyn_Vardecl*_tmp696;int _tmp697;struct Cyc_Absyn_Exp*_tmp698;
_LL388: {struct Cyc_Absyn_Var_d_struct*_tmp689=(struct Cyc_Absyn_Var_d_struct*)
_tmp688;if(_tmp689->tag != 0)goto _LL38A;else{_tmp68A=_tmp689->f1;}}_LL389: {
struct Cyc_List_List _tmp90E;struct Cyc_List_List _tmp699=(_tmp90E.hd=_tmp68A,((
_tmp90E.tl=0,_tmp90E)));inflow=Cyc_NewControlFlow_add_vars(_tmp687,inflow,(
struct Cyc_List_List*)& _tmp699,_tmp687->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp69A=_tmp68A->initializer;if(_tmp69A == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp69C;void*_tmp69D;struct _tuple13 _tmp69B=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp69A);_tmp69C=_tmp69B.f1;_tmp69D=_tmp69B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp69E=_tmp69C;int _tmp69F;struct _tuple12 _tmp6A0;
struct Cyc_Dict_Dict _tmp6A1;struct Cyc_List_List*_tmp6A2;_LL395: if((_tmp69E.BottomFL).tag
!= 1)goto _LL397;_tmp69F=(int)(_tmp69E.BottomFL).val;_LL396: return Cyc_CfFlowInfo_BottomFL();
_LL397: if((_tmp69E.ReachableFL).tag != 2)goto _LL394;_tmp6A0=(struct _tuple12)(
_tmp69E.ReachableFL).val;_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;_LL398:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp914;struct Cyc_CfFlowInfo_VarRoot_struct _tmp913;struct Cyc_CfFlowInfo_Place*
_tmp912;_tmp6A1=Cyc_CfFlowInfo_assign_place(_tmp687,decl->loc,_tmp6A1,env->all_changed,((
_tmp912=_region_malloc(env->r,sizeof(*_tmp912)),((_tmp912->root=(void*)((_tmp914=
_region_malloc(env->r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp913.tag=0,((_tmp913.f1=
_tmp68A,_tmp913)))),_tmp914)))),((_tmp912->fields=0,_tmp912)))))),_tmp69D);}
_tmp6A2=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6A2,_tmp68A,(struct Cyc_Absyn_Exp*)
_check_null(_tmp68A->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6A6=Cyc_CfFlowInfo_ReachableFL(
_tmp6A1,_tmp6A2);Cyc_NewControlFlow_update_tryflow(env,_tmp6A6);return _tmp6A6;};
_LL394:;};};};}_LL38A: {struct Cyc_Absyn_Let_d_struct*_tmp68B=(struct Cyc_Absyn_Let_d_struct*)
_tmp688;if(_tmp68B->tag != 2)goto _LL38C;else{_tmp68C=_tmp68B->f2;if(_tmp68C == 0)
goto _LL38C;_tmp68D=*_tmp68C;_tmp68E=(struct Cyc_List_List*)_tmp68D.v;_tmp68F=
_tmp68B->f3;}}_LL38B: {union Cyc_CfFlowInfo_FlowInfo _tmp6A9;void*_tmp6AA;struct
_tuple13 _tmp6A8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp68F);_tmp6A9=
_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_tmp6A9=Cyc_NewControlFlow_use_Rval(env,_tmp68F->loc,
_tmp6A9,_tmp6AA);return Cyc_NewControlFlow_add_vars(_tmp687,_tmp6A9,_tmp68E,
_tmp687->unknown_all,decl->loc);}_LL38C: {struct Cyc_Absyn_Letv_d_struct*_tmp690=(
struct Cyc_Absyn_Letv_d_struct*)_tmp688;if(_tmp690->tag != 3)goto _LL38E;else{
_tmp691=_tmp690->f1;}}_LL38D: return Cyc_NewControlFlow_add_vars(_tmp687,inflow,
_tmp691,_tmp687->unknown_none,decl->loc);_LL38E: {struct Cyc_Absyn_Fn_d_struct*
_tmp692=(struct Cyc_Absyn_Fn_d_struct*)_tmp688;if(_tmp692->tag != 1)goto _LL390;
else{_tmp693=_tmp692->f1;}}_LL38F: Cyc_NewControlFlow_check_nested_fun(_tmp687,
inflow,_tmp693);{void*_tmp6AB=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp693->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6AC=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp693->fn_vardecl);
struct Cyc_List_List*_tmp915;return Cyc_NewControlFlow_add_vars(_tmp687,inflow,((
_tmp915=_region_malloc(env->r,sizeof(*_tmp915)),((_tmp915->hd=_tmp6AC,((_tmp915->tl=
0,_tmp915)))))),_tmp687->unknown_all,decl->loc);};_LL390: {struct Cyc_Absyn_Region_d_struct*
_tmp694=(struct Cyc_Absyn_Region_d_struct*)_tmp688;if(_tmp694->tag != 4)goto _LL392;
else{_tmp695=_tmp694->f1;_tmp696=_tmp694->f2;_tmp697=_tmp694->f3;_tmp698=_tmp694->f4;}}
_LL391: if(_tmp698 != 0){struct Cyc_Absyn_Exp*_tmp6AE=(struct Cyc_Absyn_Exp*)_tmp698;
union Cyc_CfFlowInfo_FlowInfo _tmp6B0;void*_tmp6B1;struct _tuple13 _tmp6AF=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6AE);_tmp6B0=_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6AE->loc,_tmp6B0,_tmp6B1);}{struct Cyc_List_List _tmp916;struct Cyc_List_List
_tmp6B2=(_tmp916.hd=_tmp696,((_tmp916.tl=0,_tmp916)));return Cyc_NewControlFlow_add_vars(
_tmp687,inflow,(struct Cyc_List_List*)& _tmp6B2,_tmp687->unknown_all,decl->loc);};
_LL392:;_LL393: {struct Cyc_Core_Impossible_struct _tmp91C;const char*_tmp91B;
struct Cyc_Core_Impossible_struct*_tmp91A;(int)_throw((void*)((_tmp91A=_cycalloc(
sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91C.tag=Cyc_Core_Impossible,((_tmp91C.f1=((
_tmp91B="anal_decl: unexpected decl variant",_tag_dyneither(_tmp91B,sizeof(char),
35))),_tmp91C)))),_tmp91A)))));}_LL387:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp6B7=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp6B7;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp6B8=fenv->r;struct Cyc_Position_Segment*_tmp6B9=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6B9);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp6BA=inflow;int _tmp6BB;struct
_tuple12 _tmp6BC;struct Cyc_Dict_Dict _tmp6BD;struct Cyc_List_List*_tmp6BE;_LL39A:
if((_tmp6BA.BottomFL).tag != 1)goto _LL39C;_tmp6BB=(int)(_tmp6BA.BottomFL).val;
_LL39B: {const char*_tmp91F;void*_tmp91E;(_tmp91E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp91F="check_fun",
_tag_dyneither(_tmp91F,sizeof(char),10))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
_LL39C: if((_tmp6BA.ReachableFL).tag != 2)goto _LL399;_tmp6BC=(struct _tuple12)(
_tmp6BA.ReachableFL).val;_tmp6BD=_tmp6BC.f1;_tmp6BE=_tmp6BC.f2;_LL39D: {struct
Cyc_List_List*atts;{void*_tmp6C1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6C3;struct Cyc_List_List*
_tmp6C4;_LL39F: {struct Cyc_Absyn_FnType_struct*_tmp6C2=(struct Cyc_Absyn_FnType_struct*)
_tmp6C1;if(_tmp6C2->tag != 10)goto _LL3A1;else{_tmp6C3=_tmp6C2->f1;_tmp6C4=_tmp6C3.attributes;}}
_LL3A0: atts=_tmp6C4;goto _LL39E;_LL3A1:;_LL3A2: {const char*_tmp922;void*_tmp921;(
_tmp921=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp922="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp922,sizeof(char),50))),_tag_dyneither(_tmp921,sizeof(void*),0)));}_LL39E:;}
for(0;atts != 0;atts=atts->tl){void*_tmp6C7=(void*)atts->hd;int _tmp6C9;int _tmp6CB;
_LL3A4: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp6C8=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp6C7;if(_tmp6C8->tag != 21)goto _LL3A6;else{_tmp6C9=_tmp6C8->f1;}}_LL3A5: {
unsigned int j=(unsigned int)_tmp6C9;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp925;
void*_tmp924;(_tmp924=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp925="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp925,sizeof(char),52))),_tag_dyneither(_tmp924,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp6CE=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp6CE->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp928;void*
_tmp927;(_tmp927=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp928="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp928,sizeof(char),52))),_tag_dyneither(_tmp927,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp6D1=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp92B;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp92A;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6D2=(_tmp92A=_region_malloc(
_tmp6B8,sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=2,((_tmp92B.f1=(int)j,((
_tmp92B.f2=(void*)t,_tmp92B)))))),_tmp92A)));struct Cyc_CfFlowInfo_Place*_tmp92C;
struct Cyc_CfFlowInfo_Place*_tmp6D3=(_tmp92C=_region_malloc(_tmp6B8,sizeof(*
_tmp92C)),((_tmp92C->root=(void*)_tmp6D2,((_tmp92C->fields=0,_tmp92C)))));
_tmp6BD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6BD,(void*)_tmp6D2,_tmp6D1);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp932;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp931;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp92F;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp92E;_tmp6BD=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6BD,(void*)((_tmp92E=
_region_malloc(_tmp6B8,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=0,((_tmp92F.f1=
_tmp6CE,_tmp92F)))),_tmp92E)))),(void*)((_tmp931=_region_malloc(_tmp6B8,sizeof(*
_tmp931)),((_tmp931[0]=((_tmp932.tag=5,((_tmp932.f1=_tmp6D3,_tmp932)))),_tmp931)))));}
goto _LL3A3;};};}_LL3A6: {struct Cyc_Absyn_Initializes_att_struct*_tmp6CA=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp6C7;if(_tmp6CA->tag != 20)goto _LL3A8;else{
_tmp6CB=_tmp6CA->f1;}}_LL3A7: {unsigned int j=(unsigned int)_tmp6CB;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp935;void*_tmp934;(_tmp934=0,
Cyc_Tcutil_terr(_tmp6B9,((_tmp935="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp935,sizeof(char),51))),_tag_dyneither(_tmp934,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp6DD=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp6DE=Cyc_Tcutil_compress(
_tmp6DD->type);struct Cyc_Absyn_PtrInfo _tmp6E0;void*_tmp6E1;struct Cyc_Absyn_PtrAtts
_tmp6E2;union Cyc_Absyn_Constraint*_tmp6E3;union Cyc_Absyn_Constraint*_tmp6E4;
union Cyc_Absyn_Constraint*_tmp6E5;_LL3AB: {struct Cyc_Absyn_PointerType_struct*
_tmp6DF=(struct Cyc_Absyn_PointerType_struct*)_tmp6DE;if(_tmp6DF->tag != 5)goto
_LL3AD;else{_tmp6E0=_tmp6DF->f1;_tmp6E1=_tmp6E0.elt_typ;_tmp6E2=_tmp6E0.ptr_atts;
_tmp6E3=_tmp6E2.nullable;_tmp6E4=_tmp6E2.bounds;_tmp6E5=_tmp6E2.zero_term;}}
_LL3AC: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6E3)){
const char*_tmp938;void*_tmp937;(_tmp937=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp938="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp938,sizeof(char),55))),_tag_dyneither(_tmp937,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp6E4)){const char*_tmp93B;void*_tmp93A;(_tmp93A=0,
Cyc_Tcutil_terr(_tmp6B9,((_tmp93B="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp93B,sizeof(char),57))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6E5)){
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp93E="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp93E,sizeof(char),77))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp941;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp940;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6EC=(_tmp940=_region_malloc(
_tmp6B8,sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=2,((_tmp941.f1=(int)j,((
_tmp941.f2=(void*)_tmp6E1,_tmp941)))))),_tmp940)));struct Cyc_CfFlowInfo_Place*
_tmp942;struct Cyc_CfFlowInfo_Place*_tmp6ED=(_tmp942=_region_malloc(_tmp6B8,
sizeof(*_tmp942)),((_tmp942->root=(void*)_tmp6EC,((_tmp942->fields=0,_tmp942)))));
_tmp6BD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6BD,(void*)_tmp6EC,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6E1,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp948;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp947;struct Cyc_CfFlowInfo_VarRoot_struct _tmp945;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp944;_tmp6BD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6BD,(void*)((_tmp944=_region_malloc(_tmp6B8,sizeof(*_tmp944)),((_tmp944[0]=((
_tmp945.tag=0,((_tmp945.f1=_tmp6DD,_tmp945)))),_tmp944)))),(void*)((_tmp947=
_region_malloc(_tmp6B8,sizeof(*_tmp947)),((_tmp947[0]=((_tmp948.tag=5,((_tmp948.f1=
_tmp6ED,_tmp948)))),_tmp947)))));}{struct Cyc_List_List*_tmp949;param_roots=((
_tmp949=_region_malloc(_tmp6B8,sizeof(*_tmp949)),((_tmp949->hd=_tmp6ED,((_tmp949->tl=
param_roots,_tmp949))))));}goto _LL3AA;};_LL3AD:;_LL3AE: {const char*_tmp94C;void*
_tmp94B;(_tmp94B=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp94C="initializes attribute on non-pointer",
_tag_dyneither(_tmp94C,sizeof(char),37))),_tag_dyneither(_tmp94B,sizeof(void*),0)));}
_LL3AA:;}goto _LL3A3;};}_LL3A8:;_LL3A9: goto _LL3A3;_LL3A3:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6BD,_tmp6BE);}_LL399:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6B8,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp94D;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp94D=_region_malloc(_tmp6B8,sizeof(*_tmp94D)),((
_tmp94D->r=_tmp6B8,((_tmp94D->fenv=fenv,((_tmp94D->iterate_again=1,((_tmp94D->iteration_num=
0,((_tmp94D->in_try=0,((_tmp94D->tryflow=inflow,((_tmp94D->all_changed=0,((
_tmp94D->noreturn=noreturn,((_tmp94D->param_roots=param_roots,((_tmp94D->flow_table=
flow_table,_tmp94D)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp6F8=outflow;int _tmp6F9;_LL3B0: if((_tmp6F8.BottomFL).tag
!= 1)goto _LL3B2;_tmp6F9=(int)(_tmp6F8.BottomFL).val;_LL3B1: goto _LL3AF;_LL3B2:;
_LL3B3: Cyc_NewControlFlow_check_init_params(_tmp6B9,env,outflow);if(noreturn){
const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_terr(_tmp6B9,((_tmp950="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp950,sizeof(char),46))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}
else{void*_tmp6FC=Cyc_Tcutil_compress(fd->ret_type);_LL3B5: {struct Cyc_Absyn_VoidType_struct*
_tmp6FD=(struct Cyc_Absyn_VoidType_struct*)_tmp6FC;if(_tmp6FD->tag != 0)goto _LL3B7;}
_LL3B6: goto _LL3B4;_LL3B7: {struct Cyc_Absyn_DoubleType_struct*_tmp6FE=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6FC;if(_tmp6FE->tag != 8)goto _LL3B9;}_LL3B8: goto _LL3BA;_LL3B9: {struct Cyc_Absyn_FloatType_struct*
_tmp6FF=(struct Cyc_Absyn_FloatType_struct*)_tmp6FC;if(_tmp6FF->tag != 7)goto
_LL3BB;}_LL3BA: goto _LL3BC;_LL3BB: {struct Cyc_Absyn_IntType_struct*_tmp700=(
struct Cyc_Absyn_IntType_struct*)_tmp6FC;if(_tmp700->tag != 6)goto _LL3BD;}_LL3BC:{
const char*_tmp953;void*_tmp952;(_tmp952=0,Cyc_Tcutil_warn(_tmp6B9,((_tmp953="function may not return a value",
_tag_dyneither(_tmp953,sizeof(char),32))),_tag_dyneither(_tmp952,sizeof(void*),0)));}
goto _LL3B4;_LL3BD:;_LL3BE:{const char*_tmp956;void*_tmp955;(_tmp955=0,Cyc_Tcutil_terr(
_tmp6B9,((_tmp956="function may not return a value",_tag_dyneither(_tmp956,
sizeof(char),32))),_tag_dyneither(_tmp955,sizeof(void*),0)));}goto _LL3B4;_LL3B4:;}
goto _LL3AF;_LL3AF:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp706=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp708;
struct Cyc_List_List*_tmp70A;struct Cyc_List_List*_tmp70C;struct Cyc_List_List*
_tmp70E;_LL3C0: {struct Cyc_Absyn_Fn_d_struct*_tmp707=(struct Cyc_Absyn_Fn_d_struct*)
_tmp706;if(_tmp707->tag != 1)goto _LL3C2;else{_tmp708=_tmp707->f1;}}_LL3C1: Cyc_NewControlFlow_check_fun(
_tmp708);goto _LL3BF;_LL3C2: {struct Cyc_Absyn_ExternC_d_struct*_tmp709=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp706;if(_tmp709->tag != 12)goto _LL3C4;else{_tmp70A=_tmp709->f1;}}_LL3C3:
_tmp70C=_tmp70A;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Using_d_struct*_tmp70B=(
struct Cyc_Absyn_Using_d_struct*)_tmp706;if(_tmp70B->tag != 11)goto _LL3C6;else{
_tmp70C=_tmp70B->f2;}}_LL3C5: _tmp70E=_tmp70C;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Namespace_d_struct*
_tmp70D=(struct Cyc_Absyn_Namespace_d_struct*)_tmp706;if(_tmp70D->tag != 10)goto
_LL3C8;else{_tmp70E=_tmp70D->f2;}}_LL3C7: Cyc_NewControlFlow_cf_check(_tmp70E);
goto _LL3BF;_LL3C8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp70F=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp706;if(_tmp70F->tag != 13)goto _LL3CA;}_LL3C9: goto _LL3BF;_LL3CA:;_LL3CB: goto
_LL3BF;_LL3BF:;}}
