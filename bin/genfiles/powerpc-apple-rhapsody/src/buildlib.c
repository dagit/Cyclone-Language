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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};int Cyc_open(
const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};int isspace(int);int toupper(int);int system(const char*);
int mkdir(const char*pathname,unsigned short mode);int close(int);int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);struct
Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);extern char Cyc_Lexing_Error[6];
struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct
_dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int
lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dyneither_ptr*p);struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct
Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];
struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*
f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,
struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
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
char*tag;};struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct
_tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
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
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);void Cyc_Lex_lex_init(
int use_cyclone_keywords);extern char*Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*
Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_log(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_log_file == 0){{const char*
_tmp3B0;void*_tmp3AF;(_tmp3AF=0,Cyc_fprintf(Cyc_stderr,((_tmp3B0="Internal error: log file is NULL\n",
_tag_dyneither(_tmp3B0,sizeof(char),34))),_tag_dyneither(_tmp3AF,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;};}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr);static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp3B1;Cyc_current_targets=((_tmp3B1=_cycalloc(sizeof(*
_tmp3B1)),((_tmp3B1[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp3B1))));}struct _tuple16{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;}
;struct _tuple16*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple17{struct _dyneither_ptr f1;struct
_dyneither_ptr*f2;};struct _tuple17*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};struct _tuple18{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
struct _tuple19{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple19*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*
Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*
Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*
Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*
Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=0;int Cyc_numdef=0;
const int Cyc_lex_base[425]=(const int[425]){0,0,75,192,305,310,311,166,312,91,27,
384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,
- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,
74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,
525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,
684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,
244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,
1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,
300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,
422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,
449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,
53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,
659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,
714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,
772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,
875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,
1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,
2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,
1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,
1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,
1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,-
18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,
1171,1185,1255};const int Cyc_lex_backtrk[425]=(const int[425]){- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,
- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,
2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,
- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,-
1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,
9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
425]=(const int[425]){- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,
- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,
- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,
0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3481]=(const int[
3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,
0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,
415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,
218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,
121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,
34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,
331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,
139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,
21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,
163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,
154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,
161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,
113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,
104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,
185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,
199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,
206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,
33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,
99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,
236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,
245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,
137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,
262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,
288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,
44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,
- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,
344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,
79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,
287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,
275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,
100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,
277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,
21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,
300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,
412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,
0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,
97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,
98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,
358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,
108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,
0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,
0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,
109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,
125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,
127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,-
1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,
127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,
136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,
0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,
0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,
312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,
136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,
314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,
314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,
363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,
314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,
353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,
356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,
342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,
0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,
0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,
359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,
353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,
0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,
359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,
362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,
0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,
373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,
0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,
378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,
368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,
0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,
368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,
390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,
0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,
0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,
0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,
0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,
413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,
0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3481]=(const int[3481]){- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,-
1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,
1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,
7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,
47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,
94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,
34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,
150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,
142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,
11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,
173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,
73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,
195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,
199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,
32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,
125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,
32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,
237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,
15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,
15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,
267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,
137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,
42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,
294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,
329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,
273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,
327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,
391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,
275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,
365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,
275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,
309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,
- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,
315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,
90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,
328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,
354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,-
1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,
98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,
106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,
107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,-
1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,
- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,
113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,
123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,
126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,-
1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,
390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,
127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,
135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,
136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,
136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,
405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,
313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,
361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,
313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,
330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,
330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,
331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,
- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,
352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,
353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,
353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,
359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,
- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,
363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,-
1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,
- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,
378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,-
1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,
368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,
368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,
387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,-
1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,
400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,
- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,
401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,-
1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,
413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,
415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf);int Cyc_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(425,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct
_tmp3B7;const char*_tmp3B6;struct Cyc_Lexing_Error_struct*_tmp3B5;(int)_throw((
void*)((_tmp3B5=_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5[0]=((_tmp3B7.tag=Cyc_Lexing_Error,((
_tmp3B7.f1=((_tmp3B6="empty token",_tag_dyneither(_tmp3B6,sizeof(char),12))),
_tmp3B7)))),_tmp3B5)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple16*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);struct _tuple16*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct
Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**_tmp3B8;
Cyc_current_targets=((_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3B8))));}{struct
_tuple16*_tmp3B9;return(_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3B9->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp3B9)))));};case 1: _LL1: return Cyc_line(
lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3BF;const char*_tmp3BE;struct
Cyc_Lexing_Error_struct*_tmp3BD;(int)_throw((void*)((_tmp3BD=_cycalloc(sizeof(*
_tmp3BD)),((_tmp3BD[0]=((_tmp3BF.tag=Cyc_Lexing_Error,((_tmp3BF.f1=((_tmp3BE="some action didn't return!",
_tag_dyneither(_tmp3BE,sizeof(char),27))),_tmp3BF)))),_tmp3BD)))));};}struct
_tuple16*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple16*Cyc_line(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5:{struct _dyneither_ptr*_tmp3C0;Cyc_current_source=((_tmp3C0=_cycalloc(
sizeof(*_tmp3C0)),((_tmp3C0[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3C0))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3C1;Cyc_current_targets=((_tmp3C1=_cycalloc(sizeof(*
_tmp3C1)),((_tmp3C1[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C1))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp3C2;Cyc_current_source=((
_tmp3C2=_cycalloc(sizeof(*_tmp3C2)),((_tmp3C2[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3C2))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3C3;Cyc_current_targets=((_tmp3C3=_cycalloc(sizeof(*
_tmp3C3)),((_tmp3C3[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C3))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp3C4;Cyc_current_source=((
_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3C4))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp3C5;Cyc_current_targets=((
_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp3C5))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3CB;
const char*_tmp3CA;struct Cyc_Lexing_Error_struct*_tmp3C9;(int)_throw((void*)((
_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9[0]=((_tmp3CB.tag=Cyc_Lexing_Error,((
_tmp3CB.f1=((_tmp3CA="some action didn't return!",_tag_dyneither(_tmp3CA,sizeof(
char),27))),_tmp3CB)))),_tmp3C9)))));};}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(
lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_args_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA: {struct _dyneither_ptr*_tmp3CC;struct
_dyneither_ptr*_tmp15=(_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
2)),_tmp3CC)));{struct Cyc_List_List*_tmp3CD;Cyc_current_args=((_tmp3CD=_cycalloc(
sizeof(*_tmp3CD)),((_tmp3CD->hd=_tmp15,((_tmp3CD->tl=Cyc_current_args,_tmp3CD))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {struct _dyneither_ptr*_tmp3CE;struct
_dyneither_ptr*_tmp18=(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp3CE)));{struct Cyc_List_List*_tmp3CF;Cyc_current_args=((_tmp3CF=_cycalloc(
sizeof(*_tmp3CF)),((_tmp3CF->hd=_tmp18,((_tmp3CF->tl=Cyc_current_args,_tmp3CF))))));}
return Cyc_args(lexbuf);}case 2: _LLC: {struct _dyneither_ptr*_tmp3D0;struct
_dyneither_ptr*_tmp1B=(_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp3D0)));{struct Cyc_List_List*_tmp3D1;Cyc_current_args=((_tmp3D1=_cycalloc(
sizeof(*_tmp3D1)),((_tmp3D1->hd=_tmp1B,((_tmp3D1->tl=Cyc_current_args,_tmp3D1))))));}
return Cyc_token(lexbuf);}default: _LLD:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3D7;const char*_tmp3D6;struct
Cyc_Lexing_Error_struct*_tmp3D5;(int)_throw((void*)((_tmp3D5=_cycalloc(sizeof(*
_tmp3D5)),((_tmp3D5[0]=((_tmp3D7.tag=Cyc_Lexing_Error,((_tmp3D7.f1=((_tmp3D6="some action didn't return!",
_tag_dyneither(_tmp3D6,sizeof(char),27))),_tmp3D7)))),_tmp3D5)))));};}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLF:{struct
_dyneither_ptr*_tmp3D8;Cyc_add_target(((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((
_tmp3D8[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3D8)))));}return
Cyc_token(lexbuf);case 1: _LL10: return 0;case 2: _LL11: return Cyc_token(lexbuf);case 3:
_LL12: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL13: return Cyc_token(
lexbuf);case 5: _LL14: return Cyc_token(lexbuf);case 6: _LL15: return Cyc_token(lexbuf);
case 7: _LL16: return Cyc_token(lexbuf);case 8: _LL17: return Cyc_token(lexbuf);case 9:
_LL18: return Cyc_token(lexbuf);case 10: _LL19: return Cyc_token(lexbuf);case 11: _LL1A:
return Cyc_token(lexbuf);case 12: _LL1B: return Cyc_token(lexbuf);case 13: _LL1C: return
Cyc_token(lexbuf);case 14: _LL1D: return Cyc_token(lexbuf);case 15: _LL1E: return Cyc_token(
lexbuf);case 16: _LL1F: return Cyc_token(lexbuf);case 17: _LL20: return Cyc_token(lexbuf);
case 18: _LL21: return Cyc_token(lexbuf);case 19: _LL22: return Cyc_token(lexbuf);case 20:
_LL23: return Cyc_token(lexbuf);case 21: _LL24: return Cyc_token(lexbuf);case 22: _LL25:
return Cyc_token(lexbuf);case 23: _LL26: return Cyc_token(lexbuf);case 24: _LL27: return
Cyc_token(lexbuf);case 25: _LL28: return Cyc_token(lexbuf);case 26: _LL29: return Cyc_token(
lexbuf);case 27: _LL2A: return Cyc_token(lexbuf);case 28: _LL2B: return Cyc_token(lexbuf);
case 29: _LL2C: return Cyc_token(lexbuf);case 30: _LL2D: return Cyc_token(lexbuf);case 31:
_LL2E: return Cyc_token(lexbuf);case 32: _LL2F: return Cyc_token(lexbuf);case 33: _LL30:
return Cyc_token(lexbuf);case 34: _LL31: return Cyc_token(lexbuf);case 35: _LL32: return
Cyc_token(lexbuf);case 36: _LL33: return Cyc_token(lexbuf);case 37: _LL34: return Cyc_token(
lexbuf);case 38: _LL35: return Cyc_token(lexbuf);case 39: _LL36: return Cyc_token(lexbuf);
case 40: _LL37: return Cyc_token(lexbuf);case 41: _LL38: return Cyc_token(lexbuf);case 42:
_LL39: return Cyc_token(lexbuf);case 43: _LL3A: return Cyc_token(lexbuf);case 44: _LL3B:
return Cyc_token(lexbuf);case 45: _LL3C: return Cyc_token(lexbuf);case 46: _LL3D: return
Cyc_token(lexbuf);case 47: _LL3E: return Cyc_token(lexbuf);case 48: _LL3F: return Cyc_token(
lexbuf);default: _LL40:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp3DE;const char*_tmp3DD;struct Cyc_Lexing_Error_struct*
_tmp3DC;(int)_throw((void*)((_tmp3DC=_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC[0]=((
_tmp3DE.tag=Cyc_Lexing_Error,((_tmp3DE.f1=((_tmp3DD="some action didn't return!",
_tag_dyneither(_tmp3DD,sizeof(char),27))),_tmp3DE)))),_tmp3DC)))));};}int Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL42: return Cyc_string(
lexbuf);case 1: _LL43: return 0;case 2: _LL44: return Cyc_string(lexbuf);case 3: _LL45:
return Cyc_string(lexbuf);case 4: _LL46: return Cyc_string(lexbuf);case 5: _LL47: return
Cyc_string(lexbuf);case 6: _LL48: return Cyc_string(lexbuf);case 7: _LL49: return 0;case
8: _LL4A: return 0;case 9: _LL4B: return Cyc_string(lexbuf);default: _LL4C:(lexbuf->refill_buff)(
lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3E4;const char*_tmp3E3;struct Cyc_Lexing_Error_struct*_tmp3E2;(int)_throw((
void*)((_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2[0]=((_tmp3E4.tag=Cyc_Lexing_Error,((
_tmp3E4.f1=((_tmp3E3="some action didn't return!",_tag_dyneither(_tmp3E3,sizeof(
char),27))),_tmp3E4)))),_tmp3E2)))));};}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(
lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4E: return 0;case 1: _LL4F: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E7;void*_tmp3E6;(
_tmp3E6=0,Cyc_log(((_tmp3E7="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3E7,sizeof(char),44))),_tag_dyneither(_tmp3E6,sizeof(void*),0)));}
return 1;case 3: _LL51: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3EA;void*_tmp3E9;(
_tmp3E9=0,Cyc_log(((_tmp3EA="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3EA,sizeof(char),44))),_tag_dyneither(_tmp3E9,sizeof(void*),0)));}
return 1;case 4: _LL52: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3ED;void*_tmp3EC;(
_tmp3EC=0,Cyc_log(((_tmp3ED="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3ED,sizeof(char),44))),_tag_dyneither(_tmp3EC,sizeof(void*),0)));}
return 1;case 5: _LL53: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3F0;void*_tmp3EF;(
_tmp3EF=0,Cyc_log(((_tmp3F0="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3F0,sizeof(char),44))),_tag_dyneither(_tmp3EF,sizeof(void*),0)));}
return 1;case 6: _LL54: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp3F3;void*_tmp3F2;(_tmp3F2=0,Cyc_log(((
_tmp3F3="Warning: use of region sidestepped\n",_tag_dyneither(_tmp3F3,sizeof(
char),36))),_tag_dyneither(_tmp3F2,sizeof(void*),0)));}return 1;case 7: _LL55:{
const char*_tmp3F6;void*_tmp3F5;(_tmp3F5=0,Cyc_log(((_tmp3F6="Warning: use of __extension__ deleted\n",
_tag_dyneither(_tmp3F6,sizeof(char),39))),_tag_dyneither(_tmp3F5,sizeof(void*),0)));}
return 1;case 8: _LL56:{const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,Cyc_log(((_tmp3F9="Warning: use of mode HI deleted\n",
_tag_dyneither(_tmp3F9,sizeof(char),33))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}
return 1;case 9: _LL57:{const char*_tmp3FC;void*_tmp3FB;(_tmp3FB=0,Cyc_log(((_tmp3FC="Warning: use of mode SI deleted\n",
_tag_dyneither(_tmp3FC,sizeof(char),33))),_tag_dyneither(_tmp3FB,sizeof(void*),0)));}
return 1;case 10: _LL58:{const char*_tmp3FF;void*_tmp3FE;(_tmp3FE=0,Cyc_log(((
_tmp3FF="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp3FF,sizeof(char),
33))),_tag_dyneither(_tmp3FE,sizeof(void*),0)));}return 1;case 11: _LL59:{const char*
_tmp402;void*_tmp401;(_tmp401=0,Cyc_log(((_tmp402="Warning: use of mode DI deleted\n",
_tag_dyneither(_tmp402,sizeof(char),33))),_tag_dyneither(_tmp401,sizeof(void*),0)));}
return 1;case 12: _LL5A:{const char*_tmp405;void*_tmp404;(_tmp404=0,Cyc_log(((
_tmp405="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp405,sizeof(char),
33))),_tag_dyneither(_tmp404,sizeof(void*),0)));}return 1;case 13: _LL5B:{const char*
_tmp408;void*_tmp407;(_tmp407=0,Cyc_log(((_tmp408="Warning: use of mode word deleted\n",
_tag_dyneither(_tmp408,sizeof(char),35))),_tag_dyneither(_tmp407,sizeof(void*),0)));}
return 1;case 14: _LL5C: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5D: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 17: _LL5F: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL60: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 19: _LL61: return 1;case 20: _LL62: Cyc_parens_to_match=
1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_log(((
_tmp40B="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp40B,
sizeof(char),42))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}return 1;case 21:
_LL63: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL64:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp411;
const char*_tmp410;struct Cyc_Lexing_Error_struct*_tmp40F;(int)_throw((void*)((
_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=((_tmp411.tag=Cyc_Lexing_Error,((
_tmp411.f1=((_tmp410="some action didn't return!",_tag_dyneither(_tmp410,sizeof(
char),27))),_tmp411)))),_tmp40F)))));};}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,
5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL66: return 0;case 1: _LL67: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL68:{const char*
_tmp414;void*_tmp413;(_tmp413=0,Cyc_log(((_tmp414="Warning: unclosed string\n",
_tag_dyneither(_tmp414,sizeof(char),26))),_tag_dyneither(_tmp413,sizeof(void*),0)));}{
const char*_tmp418;void*_tmp417[1];struct Cyc_String_pa_struct _tmp416;(_tmp416.tag=
0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp417[0]=& _tmp416,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp418="%s",_tag_dyneither(_tmp418,sizeof(char),3))),_tag_dyneither(_tmp417,
sizeof(void*),1)))))));}return 1;case 3: _LL69:{const char*_tmp41C;void*_tmp41B[1];
struct Cyc_String_pa_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp41B[0]=&
_tmp41A,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp41C="%s",
_tag_dyneither(_tmp41C,sizeof(char),3))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}
return 1;case 4: _LL6A:{const char*_tmp420;void*_tmp41F[1];struct Cyc_String_pa_struct
_tmp41E;(_tmp41E.tag=0,((_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp41F[0]=& _tmp41E,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp420="%s",_tag_dyneither(_tmp420,sizeof(char),3))),
_tag_dyneither(_tmp41F,sizeof(void*),1)))))));}return 1;case 5: _LL6B:{const char*
_tmp424;void*_tmp423[1];struct Cyc_String_pa_struct _tmp422;(_tmp422.tag=0,((
_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp423[0]=& _tmp422,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp424="%s",_tag_dyneither(_tmp424,sizeof(char),3))),_tag_dyneither(_tmp423,
sizeof(void*),1)))))));}return 1;case 6: _LL6C:{const char*_tmp428;void*_tmp427[1];
struct Cyc_String_pa_struct _tmp426;(_tmp426.tag=0,((_tmp426.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp427[0]=&
_tmp426,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp428="%s",
_tag_dyneither(_tmp428,sizeof(char),3))),_tag_dyneither(_tmp427,sizeof(void*),1)))))));}
return 1;case 7: _LL6D:{const char*_tmp42C;void*_tmp42B[1];struct Cyc_String_pa_struct
_tmp42A;(_tmp42A.tag=0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp42B[0]=& _tmp42A,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp42C="%s",_tag_dyneither(_tmp42C,sizeof(char),3))),
_tag_dyneither(_tmp42B,sizeof(void*),1)))))));}return 1;case 8: _LL6E:{const char*
_tmp430;void*_tmp42F[1];struct Cyc_String_pa_struct _tmp42E;(_tmp42E.tag=0,((
_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp42F[0]=& _tmp42E,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp430="%s",_tag_dyneither(_tmp430,sizeof(char),3))),_tag_dyneither(_tmp42F,
sizeof(void*),1)))))));}return 1;default: _LL6F:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp436;const char*_tmp435;struct Cyc_Lexing_Error_struct*_tmp434;(int)_throw((
void*)((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=((_tmp436.tag=Cyc_Lexing_Error,((
_tmp436.f1=((_tmp435="some action didn't return!",_tag_dyneither(_tmp435,sizeof(
char),27))),_tmp436)))),_tmp434)))));};}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL71: return 0;case 1: _LL72: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL73: ++ Cyc_parens_to_match;
return 1;case 3: _LL74: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL75: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL76: return 1;case 6: _LL77: return 1;
default: _LL78:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp43C;const char*_tmp43B;struct Cyc_Lexing_Error_struct*
_tmp43A;(int)_throw((void*)((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((
_tmp43C.tag=Cyc_Lexing_Error,((_tmp43C.f1=((_tmp43B="some action didn't return!",
_tag_dyneither(_tmp43B,sizeof(char),27))),_tmp43C)))),_tmp43A)))));};}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL7A:{
const char*_tmp43F;void*_tmp43E;(_tmp43E=0,Cyc_log(((_tmp43F="Warning: unclosed string\n",
_tag_dyneither(_tmp43F,sizeof(char),26))),_tag_dyneither(_tmp43E,sizeof(void*),0)));}
return 0;case 1: _LL7B: return 0;case 2: _LL7C:{const char*_tmp442;void*_tmp441;(_tmp441=
0,Cyc_log(((_tmp442="Warning: unclosed string\n",_tag_dyneither(_tmp442,sizeof(
char),26))),_tag_dyneither(_tmp441,sizeof(void*),0)));}return 1;case 3: _LL7D:
return 1;case 4: _LL7E: return 1;case 5: _LL7F: return 1;case 6: _LL80: return 1;case 7: _LL81:
return 1;case 8: _LL82: return 1;default: _LL83:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp448;const
char*_tmp447;struct Cyc_Lexing_Error_struct*_tmp446;(int)_throw((void*)((_tmp446=
_cycalloc(sizeof(*_tmp446)),((_tmp446[0]=((_tmp448.tag=Cyc_Lexing_Error,((
_tmp448.f1=((_tmp447="some action didn't return!",_tag_dyneither(_tmp447,sizeof(
char),27))),_tmp448)))),_tmp446)))));};}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL85:{const char*_tmp44B;void*_tmp44A;(
_tmp44A=0,Cyc_log(((_tmp44B="Warning: unclosed comment\n",_tag_dyneither(_tmp44B,
sizeof(char),27))),_tag_dyneither(_tmp44A,sizeof(void*),0)));}return 0;case 1:
_LL86: return 0;case 2: _LL87: return 1;default: _LL88:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp451;const char*_tmp450;struct Cyc_Lexing_Error_struct*_tmp44F;(int)_throw((
void*)((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=((_tmp451.tag=Cyc_Lexing_Error,((
_tmp451.f1=((_tmp450="some action didn't return!",_tag_dyneither(_tmp450,sizeof(
char),27))),_tmp451)))),_tmp44F)))));};}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple17*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple17*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8A:{
const char*_tmp452;Cyc_current_line=((_tmp452="#define ",_tag_dyneither(_tmp452,
sizeof(char),9)));}Cyc_suck_macroname(lexbuf);{struct _tuple17*_tmp453;return(
_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453->f1=Cyc_current_line,((_tmp453->f2=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp453)))));};case 1: _LL8B:
return Cyc_suck_line(lexbuf);case 2: _LL8C: return 0;default: _LL8D:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp459;const char*_tmp458;struct Cyc_Lexing_Error_struct*_tmp457;(int)_throw((
void*)((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457[0]=((_tmp459.tag=Cyc_Lexing_Error,((
_tmp459.f1=((_tmp458="some action didn't return!",_tag_dyneither(_tmp458,sizeof(
char),27))),_tmp459)))),_tmp457)))));};}struct _tuple17*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf);struct _tuple17*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8F:{struct
_dyneither_ptr*_tmp45A;Cyc_current_source=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((
_tmp45A[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp45A))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL90:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp460;const char*_tmp45F;struct Cyc_Lexing_Error_struct*_tmp45E;(int)_throw((
void*)((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp460.tag=Cyc_Lexing_Error,((
_tmp460.f1=((_tmp45F="some action didn't return!",_tag_dyneither(_tmp45F,sizeof(
char),27))),_tmp460)))),_tmp45E)))));};}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL92: Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL93:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp466;const char*_tmp465;struct Cyc_Lexing_Error_struct*_tmp464;(int)_throw((
void*)((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464[0]=((_tmp466.tag=Cyc_Lexing_Error,((
_tmp466.f1=((_tmp465="some action didn't return!",_tag_dyneither(_tmp465,sizeof(
char),27))),_tmp466)))),_tmp464)))));};}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple19*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple19*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL95: return Cyc_spec(
lexbuf);case 1: _LL96: Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_cstubs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cycstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple19*_tmp467;return(_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->f1=
Cyc_current_headerfile,((_tmp467->f2=Cyc_current_symbols,((_tmp467->f3=Cyc_current_omit_symbols,((
_tmp467->f4=Cyc_current_hstubs,((_tmp467->f5=Cyc_current_cstubs,((_tmp467->f6=
Cyc_current_cycstubs,_tmp467)))))))))))));};case 2: _LL97: return Cyc_spec(lexbuf);
case 3: _LL98: return 0;case 4: _LL99:{const char*_tmp46B;void*_tmp46A[1];struct Cyc_Int_pa_struct
_tmp469;(_tmp469.tag=1,((_tmp469.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp46A[0]=& _tmp469,Cyc_fprintf(Cyc_stderr,((_tmp46B="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp46B,sizeof(char),67))),_tag_dyneither(_tmp46A,sizeof(void*),1)))))));}
return 0;default: _LL9A:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp471;const char*_tmp470;struct Cyc_Lexing_Error_struct*
_tmp46F;(int)_throw((void*)((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=((
_tmp471.tag=Cyc_Lexing_Error,((_tmp471.f1=((_tmp470="some action didn't return!",
_tag_dyneither(_tmp470,sizeof(char),27))),_tmp471)))),_tmp46F)))));};}struct
_tuple19*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple19*Cyc_spec(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9C: return 0;case 1: _LL9D: return 0;case 2: _LL9E: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9F: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA0: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple18*_tmp472;struct _tuple18*x=(_tmp472=_cycalloc(sizeof(*_tmp472)),((
_tmp472->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp472->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp472)))));{struct Cyc_List_List*_tmp473;Cyc_current_hstubs=((_tmp473=_cycalloc(
sizeof(*_tmp473)),((_tmp473->hd=x,((_tmp473->tl=Cyc_current_hstubs,_tmp473))))));}
return 1;};case 5: _LLA1: {struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp474;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((
_tmp474="hstub",_tag_dyneither(_tmp474,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp83;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp85=Cyc_substring((
struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*_tmp475;struct _tuple18*x=(_tmp475=
_cycalloc(sizeof(*_tmp475)),((_tmp475->f1=(struct _dyneither_ptr)_tmp85,((_tmp475->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp475)))));{struct Cyc_List_List*_tmp476;Cyc_current_hstubs=((_tmp476=_cycalloc(
sizeof(*_tmp476)),((_tmp476->hd=x,((_tmp476->tl=Cyc_current_hstubs,_tmp476))))));}
return 1;};};};}case 6: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple18*_tmp477;struct
_tuple18*x=(_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp477->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp477)))));{struct Cyc_List_List*
_tmp478;Cyc_current_cstubs=((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478->hd=x,((
_tmp478->tl=Cyc_current_cstubs,_tmp478))))));}return 1;};case 7: _LLA3: {struct
_dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp479;
_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((_tmp479="cstub",
_tag_dyneither(_tmp479,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp8A,sizeof(char),1);}{struct _dyneither_ptr t=_tmp8A;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp8C=Cyc_substring((
struct _dyneither_ptr)_tmp8A,0,(unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*_tmp47A;struct _tuple18*x=(_tmp47A=
_cycalloc(sizeof(*_tmp47A)),((_tmp47A->f1=(struct _dyneither_ptr)_tmp8C,((_tmp47A->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp47A)))));{struct Cyc_List_List*_tmp47B;Cyc_current_cstubs=((_tmp47B=_cycalloc(
sizeof(*_tmp47B)),((_tmp47B->hd=x,((_tmp47B->tl=Cyc_current_cstubs,_tmp47B))))));}
return 1;};};};}case 8: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple18*_tmp47C;struct
_tuple18*x=(_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp47C->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp47C)))));{struct Cyc_List_List*
_tmp47D;Cyc_current_cycstubs=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->hd=
x,((_tmp47D->tl=Cyc_current_cycstubs,_tmp47D))))));}return 1;};case 9: _LLA5: {
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp47E;
_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((_tmp47E="cycstub",
_tag_dyneither(_tmp47E,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp91,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp91,sizeof(char),1);}{struct _dyneither_ptr t=_tmp91;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp93=Cyc_substring((
struct _dyneither_ptr)_tmp91,0,(unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*_tmp47F;struct _tuple18*x=(_tmp47F=
_cycalloc(sizeof(*_tmp47F)),((_tmp47F->f1=(struct _dyneither_ptr)_tmp93,((_tmp47F->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp47F)))));{struct Cyc_List_List*_tmp480;Cyc_current_cycstubs=((_tmp480=
_cycalloc(sizeof(*_tmp480)),((_tmp480->hd=x,((_tmp480->tl=Cyc_current_cycstubs,
_tmp480))))));}return 1;};};};}case 10: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp481;struct _dyneither_ptr*x=(_tmp481=_cycalloc(sizeof(*_tmp481)),((
_tmp481[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp481)));{struct Cyc_List_List*_tmp482;Cyc_current_cpp=((
_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482->hd=x,((_tmp482->tl=Cyc_current_cpp,
_tmp482))))));}return 1;};case 11: _LLA7: return 1;case 12: _LLA8: return 1;case 13: _LLA9:{
const char*_tmp486;void*_tmp485[1];struct Cyc_Int_pa_struct _tmp484;(_tmp484.tag=1,((
_tmp484.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp485[0]=&
_tmp484,Cyc_fprintf(Cyc_stderr,((_tmp486="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp486,sizeof(char),58))),_tag_dyneither(_tmp485,sizeof(void*),1)))))));}
return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp48C;const char*_tmp48B;struct
Cyc_Lexing_Error_struct*_tmp48A;(int)_throw((void*)((_tmp48A=_cycalloc(sizeof(*
_tmp48A)),((_tmp48A[0]=((_tmp48C.tag=Cyc_Lexing_Error,((_tmp48C.f1=((_tmp48B="some action didn't return!",
_tag_dyneither(_tmp48B,sizeof(char),27))),_tmp48C)))),_tmp48A)))));};}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAC:{
struct _dyneither_ptr*_tmp48F;struct Cyc_List_List*_tmp48E;Cyc_snarfed_symbols=((
_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E->hd=((_tmp48F=_cycalloc(sizeof(*
_tmp48F)),((_tmp48F[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp48F)))),((
_tmp48E->tl=Cyc_snarfed_symbols,_tmp48E))))));}return 1;case 1: _LLAD: return 1;case 2:
_LLAE: return 0;case 3: _LLAF:{const char*_tmp492;void*_tmp491;(_tmp491=0,Cyc_fprintf(
Cyc_stderr,((_tmp492="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp492,sizeof(char),44))),_tag_dyneither(_tmp491,sizeof(void*),0)));}
return 0;case 4: _LLB0:{const char*_tmp496;void*_tmp495[1];struct Cyc_Int_pa_struct
_tmp494;(_tmp494.tag=1,((_tmp494.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp495[0]=& _tmp494,Cyc_fprintf(Cyc_stderr,((_tmp496="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp496,sizeof(char),57))),_tag_dyneither(_tmp495,sizeof(void*),1)))))));}
return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp49C;const char*_tmp49B;struct
Cyc_Lexing_Error_struct*_tmp49A;(int)_throw((void*)((_tmp49A=_cycalloc(sizeof(*
_tmp49A)),((_tmp49A[0]=((_tmp49C.tag=Cyc_Lexing_Error,((_tmp49C.f1=((_tmp49B="some action didn't return!",
_tag_dyneither(_tmp49B,sizeof(char),27))),_tmp49C)))),_tmp49A)))));};}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB3:{const char*
_tmp49F;void*_tmp49E;(_tmp49E=0,Cyc_log(((_tmp49F="Warning: unclosed brace\n",
_tag_dyneither(_tmp49F,sizeof(char),25))),_tag_dyneither(_tmp49E,sizeof(void*),0)));}
return 0;case 1: _LLB4: if(Cyc_braces_to_match == 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB5: ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;
case 3: _LLB6: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}return 1;case 4: _LLB7:{const char*_tmp4A0;Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp4A0="/*",_tag_dyneither(
_tmp4A0,sizeof(char),3))));}while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLB8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLBA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp4A6;const char*_tmp4A5;struct Cyc_Lexing_Error_struct*
_tmp4A4;(int)_throw((void*)((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((
_tmp4A6.tag=Cyc_Lexing_Error,((_tmp4A6.f1=((_tmp4A5="some action didn't return!",
_tag_dyneither(_tmp4A5,sizeof(char),27))),_tmp4A6)))),_tmp4A4)))));};}int Cyc_block(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBC:{
const char*_tmp4A9;void*_tmp4A8;(_tmp4A8=0,Cyc_log(((_tmp4A9="Warning: unclosed string\n",
_tag_dyneither(_tmp4A9,sizeof(char),26))),_tag_dyneither(_tmp4A8,sizeof(void*),0)));}
return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBE:{const char*_tmp4AC;void*_tmp4AB;(_tmp4AB=0,Cyc_log(((_tmp4AC="Warning: unclosed string\n",
_tag_dyneither(_tmp4AC,sizeof(char),26))),_tag_dyneither(_tmp4AB,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBF: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC5:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp4B2;const char*_tmp4B1;struct Cyc_Lexing_Error_struct*_tmp4B0;(int)_throw((
void*)((_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B2.tag=Cyc_Lexing_Error,((
_tmp4B2.f1=((_tmp4B1="some action didn't return!",_tag_dyneither(_tmp4B1,sizeof(
char),27))),_tmp4B2)))),_tmp4B0)))));};}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:{const char*_tmp4B5;
void*_tmp4B4;(_tmp4B4=0,Cyc_log(((_tmp4B5="Warning: unclosed comment\n",
_tag_dyneither(_tmp4B5,sizeof(char),27))),_tag_dyneither(_tmp4B4,sizeof(void*),0)));}
return 0;case 1: _LLC8:{const char*_tmp4B6;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp4B6="*/",_tag_dyneither(_tmp4B6,sizeof(char),3))));}
return 0;case 2: _LLC9: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp4BC;const char*_tmp4BB;struct Cyc_Lexing_Error_struct*_tmp4BA;(int)_throw((
void*)((_tmp4BA=_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA[0]=((_tmp4BC.tag=Cyc_Lexing_Error,((
_tmp4BC.f1=((_tmp4BB="some action didn't return!",_tag_dyneither(_tmp4BB,sizeof(
char),27))),_tmp4BC)))),_tmp4BA)))));};}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple20{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e);void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpBD;struct _tuple0 _tmpBE;struct _dyneither_ptr*_tmpBF;struct
_tuple0*_tmpC1;struct _tuple0 _tmpC2;struct _dyneither_ptr*_tmpC3;struct Cyc_List_List*
_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*
_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*
_tmpCE;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*
_tmpD4;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*
_tmpDD;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*
_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*
_tmpE6;void*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_MallocInfo _tmpEB;
int _tmpEC;struct Cyc_Absyn_Exp*_tmpED;void**_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF1;void*_tmpF3;void*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;
struct _dyneither_ptr*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct _dyneither_ptr*
_tmpFB;void*_tmpFD;void*_tmpFE;struct Cyc_List_List*_tmp101;_LLCD: {struct Cyc_Absyn_Var_e_struct*
_tmpBC=(struct Cyc_Absyn_Var_e_struct*)_tmpBB;if(_tmpBC->tag != 1)goto _LLCF;else{
_tmpBD=_tmpBC->f1;_tmpBE=*_tmpBD;_tmpBF=_tmpBE.f2;}}_LLCE: _tmpC3=_tmpBF;goto
_LLD0;_LLCF: {struct Cyc_Absyn_UnknownId_e_struct*_tmpC0=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmpBB;if(_tmpC0->tag != 2)goto _LLD1;else{_tmpC1=_tmpC0->f1;_tmpC2=*_tmpC1;_tmpC3=
_tmpC2.f2;}}_LLD0: Cyc_add_target(_tmpC3);return;_LLD1: {struct Cyc_Absyn_Primop_e_struct*
_tmpC4=(struct Cyc_Absyn_Primop_e_struct*)_tmpBB;if(_tmpC4->tag != 3)goto _LLD3;
else{_tmpC5=_tmpC4->f2;}}_LLD2: for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpC5->hd));}return;_LLD3: {struct
Cyc_Absyn_Subscript_e_struct*_tmpC6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpBB;
if(_tmpC6->tag != 24)goto _LLD5;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;}}_LLD4:
_tmpCA=_tmpC7;_tmpCB=_tmpC8;goto _LLD6;_LLD5: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpC9=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpBB;if(_tmpC9->tag != 9)goto _LLD7;
else{_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;}}_LLD6: _tmpCD=_tmpCA;_tmpCE=_tmpCB;
goto _LLD8;_LLD7: {struct Cyc_Absyn_AssignOp_e_struct*_tmpCC=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBB;if(_tmpCC->tag != 4)goto _LLD9;else{_tmpCD=_tmpCC->f1;_tmpCE=_tmpCC->f3;}}
_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCD);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpCE);return;_LLD9: {struct Cyc_Absyn_Deref_e_struct*_tmpCF=(struct Cyc_Absyn_Deref_e_struct*)
_tmpBB;if(_tmpCF->tag != 21)goto _LLDB;else{_tmpD0=_tmpCF->f1;}}_LLDA: _tmpD2=
_tmpD0;goto _LLDC;_LLDB: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpD1=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpBB;if(_tmpD1->tag != 19)goto _LLDD;else{_tmpD2=_tmpD1->f1;}}_LLDC: _tmpD4=
_tmpD2;goto _LLDE;_LLDD: {struct Cyc_Absyn_Address_e_struct*_tmpD3=(struct Cyc_Absyn_Address_e_struct*)
_tmpBB;if(_tmpD3->tag != 16)goto _LLDF;else{_tmpD4=_tmpD3->f1;}}_LLDE: _tmpD6=
_tmpD4;goto _LLE0;_LLDF: {struct Cyc_Absyn_Increment_e_struct*_tmpD5=(struct Cyc_Absyn_Increment_e_struct*)
_tmpBB;if(_tmpD5->tag != 5)goto _LLE1;else{_tmpD6=_tmpD5->f1;}}_LLE0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD6);return;_LLE1: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD7=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBB;if(_tmpD7->tag != 6)goto
_LLE3;else{_tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;_tmpDA=_tmpD7->f3;}}_LLE2: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD8);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD9);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDA);return;_LLE3: {struct Cyc_Absyn_And_e_struct*_tmpDB=(
struct Cyc_Absyn_And_e_struct*)_tmpBB;if(_tmpDB->tag != 7)goto _LLE5;else{_tmpDC=
_tmpDB->f1;_tmpDD=_tmpDB->f2;}}_LLE4: _tmpDF=_tmpDC;_tmpE0=_tmpDD;goto _LLE6;_LLE5: {
struct Cyc_Absyn_Or_e_struct*_tmpDE=(struct Cyc_Absyn_Or_e_struct*)_tmpBB;if(
_tmpDE->tag != 8)goto _LLE7;else{_tmpDF=_tmpDE->f1;_tmpE0=_tmpDE->f2;}}_LLE6: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE0);return;
_LLE7: {struct Cyc_Absyn_FnCall_e_struct*_tmpE1=(struct Cyc_Absyn_FnCall_e_struct*)
_tmpBB;if(_tmpE1->tag != 11)goto _LLE9;else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}
_LLE8: _tmpE5=_tmpE2;_tmpE6=_tmpE3;goto _LLEA;_LLE9: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmpE4=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBB;if(_tmpE4->tag != 10)goto
_LLEB;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;}}_LLEA: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpE5);for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpE6->hd));}return;_LLEB: {struct Cyc_Absyn_Cast_e_struct*
_tmpE7=(struct Cyc_Absyn_Cast_e_struct*)_tmpBB;if(_tmpE7->tag != 15)goto _LLED;
else{_tmpE8=(void*)_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LLEC: Cyc_scan_type(_tmpE8);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE9);return;_LLED: {struct Cyc_Absyn_Malloc_e_struct*
_tmpEA=(struct Cyc_Absyn_Malloc_e_struct*)_tmpBB;if(_tmpEA->tag != 34)goto _LLEF;
else{_tmpEB=_tmpEA->f1;_tmpEC=_tmpEB.is_calloc;_tmpED=_tmpEB.rgn;_tmpEE=_tmpEB.elt_type;
_tmpEF=_tmpEB.num_elts;}}_LLEE: if(_tmpED != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpED));if(_tmpEE != 0)Cyc_scan_type(*_tmpEE);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpEF);return;_LLEF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpF0=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmpBB;if(_tmpF0->tag != 38)goto _LLF1;
else{_tmpF1=_tmpF0->f1;}}_LLF0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpF1);
return;_LLF1: {struct Cyc_Absyn_Valueof_e_struct*_tmpF2=(struct Cyc_Absyn_Valueof_e_struct*)
_tmpBB;if(_tmpF2->tag != 39)goto _LLF3;else{_tmpF3=(void*)_tmpF2->f1;}}_LLF2:
_tmpF5=_tmpF3;goto _LLF4;_LLF3: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpF4=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBB;if(_tmpF4->tag != 18)goto _LLF5;else{
_tmpF5=(void*)_tmpF4->f1;}}_LLF4: Cyc_scan_type(_tmpF5);return;_LLF5: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpBB;if(_tmpF6->tag != 22)goto
_LLF7;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LLF6: _tmpFA=_tmpF7;_tmpFB=
_tmpF8;goto _LLF8;_LLF7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpBB;if(_tmpF9->tag != 23)goto _LLF9;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;}}
_LLF8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpFA);Cyc_add_target(_tmpFB);return;
_LLF9: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBB;if(_tmpFC->tag != 20)goto _LLFB;else{_tmpFD=(void*)_tmpFC->f1;_tmpFE=(void*)
_tmpFC->f2;}}_LLFA: Cyc_scan_type(_tmpFD);{void*_tmp111=_tmpFE;struct
_dyneither_ptr*_tmp113;_LL11E: {struct Cyc_Absyn_StructField_struct*_tmp112=(
struct Cyc_Absyn_StructField_struct*)_tmp111;if(_tmp112->tag != 0)goto _LL120;else{
_tmp113=_tmp112->f1;}}_LL11F: Cyc_add_target(_tmp113);goto _LL11D;_LL120: {struct
Cyc_Absyn_TupleIndex_struct*_tmp114=(struct Cyc_Absyn_TupleIndex_struct*)_tmp111;
if(_tmp114->tag != 1)goto _LL11D;}_LL121: goto _LL11D;_LL11D:;}return;_LLFB: {struct
Cyc_Absyn_Const_e_struct*_tmpFF=(struct Cyc_Absyn_Const_e_struct*)_tmpBB;if(
_tmpFF->tag != 0)goto _LLFD;}_LLFC: return;_LLFD: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp100=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBB;if(_tmp100->tag != 36)
goto _LLFF;else{_tmp101=_tmp100->f2;}}_LLFE: for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
struct _tuple20 _tmp116;struct Cyc_Absyn_Exp*_tmp117;struct _tuple20*_tmp115=(struct
_tuple20*)_tmp101->hd;_tmp116=*_tmp115;_tmp117=_tmp116.f2;Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmp117);}return;_LLFF: {struct Cyc_Absyn_Swap_e_struct*_tmp102=(
struct Cyc_Absyn_Swap_e_struct*)_tmpBB;if(_tmp102->tag != 35)goto _LL101;}_LL100:{
const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,Cyc_fprintf(Cyc_stderr,((_tmp4BF="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp4BF,sizeof(char),26))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}
exit(1);return;_LL101: {struct Cyc_Absyn_StmtExp_e_struct*_tmp103=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBB;if(_tmp103->tag != 37)goto _LL103;}_LL102:{const char*_tmp4C2;void*_tmp4C1;(
_tmp4C1=0,Cyc_fprintf(Cyc_stderr,((_tmp4C2="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp4C2,sizeof(char),26))),_tag_dyneither(_tmp4C1,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_Throw_e_struct*_tmp104=(struct Cyc_Absyn_Throw_e_struct*)
_tmpBB;if(_tmp104->tag != 12)goto _LL105;}_LL104:{const char*_tmp4C5;void*_tmp4C4;(
_tmp4C4=0,Cyc_fprintf(Cyc_stderr,((_tmp4C5="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp4C5,sizeof(char),27))),_tag_dyneither(_tmp4C4,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp105=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpBB;if(_tmp105->tag != 13)goto _LL107;}_LL106:{
const char*_tmp4C8;void*_tmp4C7;(_tmp4C7=0,Cyc_fprintf(Cyc_stderr,((_tmp4C8="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp4C8,sizeof(char),35))),_tag_dyneither(_tmp4C7,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_Instantiate_e_struct*_tmp106=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpBB;if(_tmp106->tag != 14)goto _LL109;}_LL108:{const char*_tmp4CB;void*_tmp4CA;(
_tmp4CA=0,Cyc_fprintf(Cyc_stderr,((_tmp4CB="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp4CB,sizeof(char),33))),_tag_dyneither(_tmp4CA,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_New_e_struct*_tmp107=(struct Cyc_Absyn_New_e_struct*)
_tmpBB;if(_tmp107->tag != 17)goto _LL10B;}_LL10A:{const char*_tmp4CE;void*_tmp4CD;(
_tmp4CD=0,Cyc_fprintf(Cyc_stderr,((_tmp4CE="Error: unexpected New_e\n",
_tag_dyneither(_tmp4CE,sizeof(char),25))),_tag_dyneither(_tmp4CD,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_Tuple_e_struct*_tmp108=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpBB;if(_tmp108->tag != 25)goto _LL10D;}_LL10C:{const char*_tmp4D1;void*_tmp4D0;(
_tmp4D0=0,Cyc_fprintf(Cyc_stderr,((_tmp4D1="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp4D1,sizeof(char),27))),_tag_dyneither(_tmp4D0,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp109=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBB;if(_tmp109->tag != 26)goto _LL10F;}_LL10E:{const char*_tmp4D4;void*_tmp4D3;(
_tmp4D3=0,Cyc_fprintf(Cyc_stderr,((_tmp4D4="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp4D4,sizeof(char),33))),_tag_dyneither(_tmp4D3,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_Array_e_struct*_tmp10A=(struct Cyc_Absyn_Array_e_struct*)
_tmpBB;if(_tmp10A->tag != 27)goto _LL111;}_LL110:{const char*_tmp4D7;void*_tmp4D6;(
_tmp4D6=0,Cyc_fprintf(Cyc_stderr,((_tmp4D7="Error: unexpected Array_e\n",
_tag_dyneither(_tmp4D7,sizeof(char),27))),_tag_dyneither(_tmp4D6,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_Comprehension_e_struct*_tmp10B=(struct
Cyc_Absyn_Comprehension_e_struct*)_tmpBB;if(_tmp10B->tag != 28)goto _LL113;}_LL112:{
const char*_tmp4DA;void*_tmp4D9;(_tmp4D9=0,Cyc_fprintf(Cyc_stderr,((_tmp4DA="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp4DA,sizeof(char),35))),_tag_dyneither(_tmp4D9,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_Aggregate_e_struct*_tmp10C=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpBB;if(_tmp10C->tag != 29)goto _LL115;}_LL114:{const char*_tmp4DD;void*_tmp4DC;(
_tmp4DC=0,Cyc_fprintf(Cyc_stderr,((_tmp4DD="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp4DD,sizeof(char),31))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp10D=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBB;if(_tmp10D->tag != 30)goto _LL117;}_LL116:{const char*_tmp4E0;void*_tmp4DF;(
_tmp4DF=0,Cyc_fprintf(Cyc_stderr,((_tmp4E0="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp4E0,sizeof(char),32))),_tag_dyneither(_tmp4DF,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_Datatype_e_struct*_tmp10E=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpBB;if(_tmp10E->tag != 31)goto _LL119;}_LL118:{const char*_tmp4E3;void*_tmp4E2;(
_tmp4E2=0,Cyc_fprintf(Cyc_stderr,((_tmp4E3="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp4E3,sizeof(char),30))),_tag_dyneither(_tmp4E2,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_Enum_e_struct*_tmp10F=(struct Cyc_Absyn_Enum_e_struct*)
_tmpBB;if(_tmp10F->tag != 32)goto _LL11B;}_LL11A:{const char*_tmp4E6;void*_tmp4E5;(
_tmp4E5=0,Cyc_fprintf(Cyc_stderr,((_tmp4E6="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp4E6,sizeof(char),26))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}
exit(1);return;_LL11B: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp110=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBB;if(_tmp110->tag != 33)goto _LLCC;}_LL11C:{const char*_tmp4E9;void*_tmp4E8;(
_tmp4E8=0,Cyc_fprintf(Cyc_stderr,((_tmp4E9="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp4E9,sizeof(char),30))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));}
exit(1);return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo);void Cyc_scan_exp_opt(
struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(
void*t);void Cyc_scan_type(void*t){void*_tmp136=t;struct Cyc_Absyn_PtrInfo _tmp13C;
struct Cyc_Absyn_ArrayInfo _tmp13E;void*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;union
Cyc_Absyn_Constraint*_tmp141;struct Cyc_Absyn_FnInfo _tmp143;struct Cyc_List_List*
_tmp145;struct Cyc_Absyn_AggrInfo _tmp148;union Cyc_Absyn_AggrInfoU _tmp149;struct
_tuple0*_tmp14B;struct _tuple0 _tmp14C;struct _dyneither_ptr*_tmp14D;struct _tuple0*
_tmp14F;struct _tuple0 _tmp150;struct _dyneither_ptr*_tmp151;_LL123: {struct Cyc_Absyn_VoidType_struct*
_tmp137=(struct Cyc_Absyn_VoidType_struct*)_tmp136;if(_tmp137->tag != 0)goto _LL125;}
_LL124: goto _LL126;_LL125: {struct Cyc_Absyn_IntType_struct*_tmp138=(struct Cyc_Absyn_IntType_struct*)
_tmp136;if(_tmp138->tag != 6)goto _LL127;}_LL126: goto _LL128;_LL127: {struct Cyc_Absyn_FloatType_struct*
_tmp139=(struct Cyc_Absyn_FloatType_struct*)_tmp136;if(_tmp139->tag != 7)goto
_LL129;}_LL128: goto _LL12A;_LL129: {struct Cyc_Absyn_DoubleType_struct*_tmp13A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp136;if(_tmp13A->tag != 8)goto _LL12B;}
_LL12A: return;_LL12B: {struct Cyc_Absyn_PointerType_struct*_tmp13B=(struct Cyc_Absyn_PointerType_struct*)
_tmp136;if(_tmp13B->tag != 5)goto _LL12D;else{_tmp13C=_tmp13B->f1;}}_LL12C: Cyc_scan_type(
_tmp13C.elt_typ);return;_LL12D: {struct Cyc_Absyn_ArrayType_struct*_tmp13D=(
struct Cyc_Absyn_ArrayType_struct*)_tmp136;if(_tmp13D->tag != 9)goto _LL12F;else{
_tmp13E=_tmp13D->f1;_tmp13F=_tmp13E.elt_type;_tmp140=_tmp13E.num_elts;_tmp141=
_tmp13E.zero_term;}}_LL12E: Cyc_scan_type(_tmp13F);Cyc_scan_exp_opt(_tmp140);
return;_LL12F: {struct Cyc_Absyn_FnType_struct*_tmp142=(struct Cyc_Absyn_FnType_struct*)
_tmp136;if(_tmp142->tag != 10)goto _LL131;else{_tmp143=_tmp142->f1;}}_LL130: Cyc_scan_type(
_tmp143.ret_typ);{struct Cyc_List_List*_tmp160=_tmp143.args;for(0;_tmp160 != 0;
_tmp160=_tmp160->tl){struct _tuple7 _tmp162;void*_tmp163;struct _tuple7*_tmp161=(
struct _tuple7*)_tmp160->hd;_tmp162=*_tmp161;_tmp163=_tmp162.f3;Cyc_scan_type(
_tmp163);}}if(_tmp143.cyc_varargs != 0)Cyc_scan_type((_tmp143.cyc_varargs)->type);
return;_LL131: {struct Cyc_Absyn_AnonAggrType_struct*_tmp144=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp136;if(_tmp144->tag != 13)goto _LL133;else{_tmp145=_tmp144->f2;}}_LL132: for(0;
_tmp145 != 0;_tmp145=_tmp145->tl){Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)
_tmp145->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp145->hd)->width);}
return;_LL133: {struct Cyc_Absyn_AnonEnumType_struct*_tmp146=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp136;if(_tmp146->tag != 15)goto _LL135;}_LL134: return;_LL135: {struct Cyc_Absyn_AggrType_struct*
_tmp147=(struct Cyc_Absyn_AggrType_struct*)_tmp136;if(_tmp147->tag != 12)goto
_LL137;else{_tmp148=_tmp147->f1;_tmp149=_tmp148.aggr_info;}}_LL136: {struct
_tuple0*_tmp165;struct _tuple0 _tmp166;struct _dyneither_ptr*_tmp167;struct _tuple9
_tmp164=Cyc_Absyn_aggr_kinded_name(_tmp149);_tmp165=_tmp164.f2;_tmp166=*_tmp165;
_tmp167=_tmp166.f2;_tmp14D=_tmp167;goto _LL138;}_LL137: {struct Cyc_Absyn_EnumType_struct*
_tmp14A=(struct Cyc_Absyn_EnumType_struct*)_tmp136;if(_tmp14A->tag != 14)goto
_LL139;else{_tmp14B=_tmp14A->f1;_tmp14C=*_tmp14B;_tmp14D=_tmp14C.f2;}}_LL138:
_tmp151=_tmp14D;goto _LL13A;_LL139: {struct Cyc_Absyn_TypedefType_struct*_tmp14E=(
struct Cyc_Absyn_TypedefType_struct*)_tmp136;if(_tmp14E->tag != 18)goto _LL13B;
else{_tmp14F=_tmp14E->f1;_tmp150=*_tmp14F;_tmp151=_tmp150.f2;}}_LL13A: Cyc_add_target(
_tmp151);return;_LL13B: {struct Cyc_Absyn_Evar_struct*_tmp152=(struct Cyc_Absyn_Evar_struct*)
_tmp136;if(_tmp152->tag != 1)goto _LL13D;}_LL13C:{const char*_tmp4EC;void*_tmp4EB;(
_tmp4EB=0,Cyc_fprintf(Cyc_stderr,((_tmp4EC="Error: unexpected Evar\n",
_tag_dyneither(_tmp4EC,sizeof(char),24))),_tag_dyneither(_tmp4EB,sizeof(void*),0)));}
exit(1);return;_LL13D: {struct Cyc_Absyn_VarType_struct*_tmp153=(struct Cyc_Absyn_VarType_struct*)
_tmp136;if(_tmp153->tag != 2)goto _LL13F;}_LL13E:{const char*_tmp4EF;void*_tmp4EE;(
_tmp4EE=0,Cyc_fprintf(Cyc_stderr,((_tmp4EF="Error: unexpected VarType\n",
_tag_dyneither(_tmp4EF,sizeof(char),27))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}
exit(1);return;_LL13F: {struct Cyc_Absyn_DatatypeType_struct*_tmp154=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp136;if(_tmp154->tag != 3)goto _LL141;}_LL140:{const char*_tmp4F2;void*_tmp4F1;(
_tmp4F1=0,Cyc_fprintf(Cyc_stderr,((_tmp4F2="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp4F2,sizeof(char),32))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}
exit(1);return;_LL141: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp155=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp136;if(_tmp155->tag != 4)goto _LL143;}
_LL142:{const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((
_tmp4F5="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp4F5,sizeof(
char),37))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}exit(1);return;_LL143: {
struct Cyc_Absyn_TupleType_struct*_tmp156=(struct Cyc_Absyn_TupleType_struct*)
_tmp136;if(_tmp156->tag != 11)goto _LL145;}_LL144:{const char*_tmp4F8;void*_tmp4F7;(
_tmp4F7=0,Cyc_fprintf(Cyc_stderr,((_tmp4F8="Error: unexpected TupleType\n",
_tag_dyneither(_tmp4F8,sizeof(char),29))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}
exit(1);return;_LL145: {struct Cyc_Absyn_RgnHandleType_struct*_tmp157=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp136;if(_tmp157->tag != 16)goto _LL147;}_LL146:{const char*_tmp4FB;void*_tmp4FA;(
_tmp4FA=0,Cyc_fprintf(Cyc_stderr,((_tmp4FB="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp4FB,sizeof(char),33))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_DynRgnType_struct*_tmp158=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp136;if(_tmp158->tag != 17)goto _LL149;}_LL148:{const char*_tmp4FE;void*_tmp4FD;(
_tmp4FD=0,Cyc_fprintf(Cyc_stderr,((_tmp4FE="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp4FE,sizeof(char),30))),_tag_dyneither(_tmp4FD,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_HeapRgn_struct*_tmp159=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp136;if(_tmp159->tag != 21)goto _LL14B;}_LL14A:{const char*_tmp501;void*_tmp500;(
_tmp500=0,Cyc_fprintf(Cyc_stderr,((_tmp501="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp501,sizeof(char),27))),_tag_dyneither(_tmp500,sizeof(void*),0)));}
exit(1);return;_LL14B: {struct Cyc_Absyn_UniqueRgn_struct*_tmp15A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp136;if(_tmp15A->tag != 22)goto _LL14D;}_LL14C:{const char*_tmp504;void*_tmp503;(
_tmp503=0,Cyc_fprintf(Cyc_stderr,((_tmp504="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp504,sizeof(char),29))),_tag_dyneither(_tmp503,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_AccessEff_struct*_tmp15B=(struct Cyc_Absyn_AccessEff_struct*)
_tmp136;if(_tmp15B->tag != 23)goto _LL14F;}_LL14E:{const char*_tmp507;void*_tmp506;(
_tmp506=0,Cyc_fprintf(Cyc_stderr,((_tmp507="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp507,sizeof(char),29))),_tag_dyneither(_tmp506,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_JoinEff_struct*_tmp15C=(struct Cyc_Absyn_JoinEff_struct*)
_tmp136;if(_tmp15C->tag != 24)goto _LL151;}_LL150:{const char*_tmp50A;void*_tmp509;(
_tmp509=0,Cyc_fprintf(Cyc_stderr,((_tmp50A="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp50A,sizeof(char),27))),_tag_dyneither(_tmp509,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_RgnsEff_struct*_tmp15D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp136;if(_tmp15D->tag != 25)goto _LL153;}_LL152:{const char*_tmp50D;void*_tmp50C;(
_tmp50C=0,Cyc_fprintf(Cyc_stderr,((_tmp50D="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp50D,sizeof(char),27))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_TagType_struct*_tmp15E=(struct Cyc_Absyn_TagType_struct*)
_tmp136;if(_tmp15E->tag != 20)goto _LL155;}_LL154:{const char*_tmp510;void*_tmp50F;(
_tmp50F=0,Cyc_fprintf(Cyc_stderr,((_tmp510="Error: unexpected tag_t\n",
_tag_dyneither(_tmp510,sizeof(char),25))),_tag_dyneither(_tmp50F,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_ValueofType_struct*_tmp15F=(struct Cyc_Absyn_ValueofType_struct*)
_tmp136;if(_tmp15F->tag != 19)goto _LL122;}_LL156:{const char*_tmp513;void*_tmp512;(
_tmp512=0,Cyc_fprintf(Cyc_stderr,((_tmp513="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp513,sizeof(char),29))),_tag_dyneither(_tmp512,sizeof(void*),0)));}
exit(1);return;_LL122:;}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple16*Cyc_scan_decl(struct Cyc_Absyn_Decl*d);struct _tuple16*
Cyc_scan_decl(struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**_tmp514;Cyc_current_targets=((
_tmp514=_cycalloc(sizeof(*_tmp514)),((_tmp514[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp514))));}{void*_tmp185=d->r;struct Cyc_Absyn_Vardecl*_tmp187;struct Cyc_Absyn_Fndecl*
_tmp189;struct Cyc_Absyn_Aggrdecl*_tmp18B;struct Cyc_Absyn_Enumdecl*_tmp18D;struct
Cyc_Absyn_Typedefdecl*_tmp18F;_LL158: {struct Cyc_Absyn_Var_d_struct*_tmp186=(
struct Cyc_Absyn_Var_d_struct*)_tmp185;if(_tmp186->tag != 0)goto _LL15A;else{
_tmp187=_tmp186->f1;}}_LL159: {struct _tuple0 _tmp19C;struct _dyneither_ptr*_tmp19D;
struct _tuple0*_tmp19B=_tmp187->name;_tmp19C=*_tmp19B;_tmp19D=_tmp19C.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp19D;Cyc_scan_type(_tmp187->type);Cyc_scan_exp_opt(
_tmp187->initializer);goto _LL157;}_LL15A: {struct Cyc_Absyn_Fn_d_struct*_tmp188=(
struct Cyc_Absyn_Fn_d_struct*)_tmp185;if(_tmp188->tag != 1)goto _LL15C;else{_tmp189=
_tmp188->f1;}}_LL15B: {struct _tuple0 _tmp19F;struct _dyneither_ptr*_tmp1A0;struct
_tuple0*_tmp19E=_tmp189->name;_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp1A0;Cyc_scan_type(_tmp189->ret_type);{struct Cyc_List_List*
_tmp1A1=_tmp189->args;for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){struct _tuple21
_tmp1A3;void*_tmp1A4;struct _tuple21*_tmp1A2=(struct _tuple21*)_tmp1A1->hd;_tmp1A3=*
_tmp1A2;_tmp1A4=_tmp1A3.f3;Cyc_scan_type(_tmp1A4);}}if(_tmp189->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp189->cyc_varargs))->type);
if(_tmp189->is_inline){const char*_tmp517;void*_tmp516;(_tmp516=0,Cyc_fprintf(Cyc_stderr,((
_tmp517="Warning: ignoring inline function\n",_tag_dyneither(_tmp517,sizeof(char),
35))),_tag_dyneither(_tmp516,sizeof(void*),0)));}goto _LL157;}_LL15C: {struct Cyc_Absyn_Aggr_d_struct*
_tmp18A=(struct Cyc_Absyn_Aggr_d_struct*)_tmp185;if(_tmp18A->tag != 6)goto _LL15E;
else{_tmp18B=_tmp18A->f1;}}_LL15D: {struct _tuple0 _tmp1A8;struct _dyneither_ptr*
_tmp1A9;struct _tuple0*_tmp1A7=_tmp18B->name;_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp1A9;if((unsigned int)_tmp18B->impl){{
struct Cyc_List_List*_tmp1AA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18B->impl))->fields;
for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){struct Cyc_Absyn_Aggrfield*_tmp1AB=(struct
Cyc_Absyn_Aggrfield*)_tmp1AA->hd;Cyc_scan_type(_tmp1AB->type);Cyc_scan_exp_opt(
_tmp1AB->width);}}{struct Cyc_List_List*_tmp1AC=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp18B->impl))->fields;for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){;}};}
goto _LL157;}_LL15E: {struct Cyc_Absyn_Enum_d_struct*_tmp18C=(struct Cyc_Absyn_Enum_d_struct*)
_tmp185;if(_tmp18C->tag != 8)goto _LL160;else{_tmp18D=_tmp18C->f1;}}_LL15F: {
struct _tuple0 _tmp1AE;struct _dyneither_ptr*_tmp1AF;struct _tuple0*_tmp1AD=_tmp18D->name;
_tmp1AE=*_tmp1AD;_tmp1AF=_tmp1AE.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1AF;if((unsigned int)_tmp18D->fields){{struct Cyc_List_List*_tmp1B0=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp18D->fields))->v;for(0;
_tmp1B0 != 0;_tmp1B0=_tmp1B0->tl){struct Cyc_Absyn_Enumfield*_tmp1B1=(struct Cyc_Absyn_Enumfield*)
_tmp1B0->hd;Cyc_scan_exp_opt(_tmp1B1->tag);}}{struct Cyc_List_List*_tmp1B2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp18D->fields))->v;for(
0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){;}};}goto _LL157;}_LL160: {struct Cyc_Absyn_Typedef_d_struct*
_tmp18E=(struct Cyc_Absyn_Typedef_d_struct*)_tmp185;if(_tmp18E->tag != 9)goto
_LL162;else{_tmp18F=_tmp18E->f1;}}_LL161: {struct _tuple0 _tmp1B4;struct
_dyneither_ptr*_tmp1B5;struct _tuple0*_tmp1B3=_tmp18F->name;_tmp1B4=*_tmp1B3;
_tmp1B5=_tmp1B4.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1B5;if((
unsigned int)_tmp18F->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp18F->defn))->v);goto _LL157;}_LL162: {struct Cyc_Absyn_Region_d_struct*
_tmp190=(struct Cyc_Absyn_Region_d_struct*)_tmp185;if(_tmp190->tag != 4)goto _LL164;}
_LL163:{const char*_tmp51A;void*_tmp519;(_tmp519=0,Cyc_fprintf(Cyc_stderr,((
_tmp51A="Error: unexpected region declaration",_tag_dyneither(_tmp51A,sizeof(
char),37))),_tag_dyneither(_tmp519,sizeof(void*),0)));}exit(1);_LL164: {struct
Cyc_Absyn_Alias_d_struct*_tmp191=(struct Cyc_Absyn_Alias_d_struct*)_tmp185;if(
_tmp191->tag != 5)goto _LL166;}_LL165:{const char*_tmp51D;void*_tmp51C;(_tmp51C=0,
Cyc_fprintf(Cyc_stderr,((_tmp51D="Error: unexpected alias declaration",
_tag_dyneither(_tmp51D,sizeof(char),36))),_tag_dyneither(_tmp51C,sizeof(void*),0)));}
exit(1);_LL166: {struct Cyc_Absyn_Porton_d_struct*_tmp192=(struct Cyc_Absyn_Porton_d_struct*)
_tmp185;if(_tmp192->tag != 14)goto _LL168;}_LL167:{const char*_tmp520;void*_tmp51F;(
_tmp51F=0,Cyc_fprintf(Cyc_stderr,((_tmp520="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp520,sizeof(char),38))),_tag_dyneither(_tmp51F,sizeof(void*),0)));}
exit(1);return 0;_LL168: {struct Cyc_Absyn_Portoff_d_struct*_tmp193=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp185;if(_tmp193->tag != 15)goto _LL16A;}_LL169:{const char*_tmp523;void*_tmp522;(
_tmp522=0,Cyc_fprintf(Cyc_stderr,((_tmp523="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp523,sizeof(char),39))),_tag_dyneither(_tmp522,sizeof(void*),0)));}
exit(1);return 0;_LL16A: {struct Cyc_Absyn_Let_d_struct*_tmp194=(struct Cyc_Absyn_Let_d_struct*)
_tmp185;if(_tmp194->tag != 2)goto _LL16C;}_LL16B:{const char*_tmp526;void*_tmp525;(
_tmp525=0,Cyc_fprintf(Cyc_stderr,((_tmp526="Error: unexpected let declaration\n",
_tag_dyneither(_tmp526,sizeof(char),35))),_tag_dyneither(_tmp525,sizeof(void*),0)));}
exit(1);return 0;_LL16C: {struct Cyc_Absyn_Datatype_d_struct*_tmp195=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp185;if(_tmp195->tag != 7)goto _LL16E;}_LL16D:{const char*_tmp529;void*_tmp528;(
_tmp528=0,Cyc_fprintf(Cyc_stderr,((_tmp529="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp529,sizeof(char),40))),_tag_dyneither(_tmp528,sizeof(void*),0)));}
exit(1);return 0;_LL16E: {struct Cyc_Absyn_Letv_d_struct*_tmp196=(struct Cyc_Absyn_Letv_d_struct*)
_tmp185;if(_tmp196->tag != 3)goto _LL170;}_LL16F:{const char*_tmp52C;void*_tmp52B;(
_tmp52B=0,Cyc_fprintf(Cyc_stderr,((_tmp52C="Error: unexpected let declaration\n",
_tag_dyneither(_tmp52C,sizeof(char),35))),_tag_dyneither(_tmp52B,sizeof(void*),0)));}
exit(1);return 0;_LL170: {struct Cyc_Absyn_Namespace_d_struct*_tmp197=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp185;if(_tmp197->tag != 10)goto _LL172;}_LL171:{const char*_tmp52F;void*_tmp52E;(
_tmp52E=0,Cyc_fprintf(Cyc_stderr,((_tmp52F="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp52F,sizeof(char),41))),_tag_dyneither(_tmp52E,sizeof(void*),0)));}
exit(1);return 0;_LL172: {struct Cyc_Absyn_Using_d_struct*_tmp198=(struct Cyc_Absyn_Using_d_struct*)
_tmp185;if(_tmp198->tag != 11)goto _LL174;}_LL173:{const char*_tmp532;void*_tmp531;(
_tmp531=0,Cyc_fprintf(Cyc_stderr,((_tmp532="Error: unexpected using declaration\n",
_tag_dyneither(_tmp532,sizeof(char),37))),_tag_dyneither(_tmp531,sizeof(void*),0)));}
exit(1);return 0;_LL174: {struct Cyc_Absyn_ExternC_d_struct*_tmp199=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp185;if(_tmp199->tag != 12)goto _LL176;}_LL175:{const char*_tmp535;void*_tmp534;(
_tmp534=0,Cyc_fprintf(Cyc_stderr,((_tmp535="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp535,sizeof(char),42))),_tag_dyneither(_tmp534,sizeof(void*),0)));}
exit(1);return 0;_LL176: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp19A=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp185;if(_tmp19A->tag != 13)goto _LL157;}
_LL177:{const char*_tmp538;void*_tmp537;(_tmp537=0,Cyc_fprintf(Cyc_stderr,((
_tmp538="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp538,sizeof(char),50))),_tag_dyneither(_tmp537,sizeof(void*),0)));}exit(1);
return 0;_LL157:;}{struct _tuple16*_tmp539;return(_tmp539=_cycalloc(sizeof(*
_tmp539)),((_tmp539->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp539->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp539)))));};}
struct Cyc_Hashtable_Table*Cyc_new_deps();struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x);struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){struct _handler_cons _tmp1CD;
_push_handler(& _tmp1CD);{int _tmp1CF=0;if(setjmp(_tmp1CD.handler))_tmp1CF=1;if(!
_tmp1CF){{struct Cyc_Set_Set*_tmp1D0=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return
_tmp1D0;};_pop_handler();}else{void*_tmp1CE=(void*)_exn_thrown;void*_tmp1D2=
_tmp1CE;_LL179: {struct Cyc_Core_Not_found_struct*_tmp1D3=(struct Cyc_Core_Not_found_struct*)
_tmp1D2;if(_tmp1D3->tag != Cyc_Core_Not_found)goto _LL17B;}_LL17A: return((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(
Cyc_strptrcmp);_LL17B:;_LL17C:(void)_throw(_tmp1D2);_LL178:;}};}struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t);struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*
emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);struct Cyc_Set_Set*curr;for(curr=
emptyset;init != 0;init=init->tl){curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1D4=curr;struct _dyneither_ptr*_tmp1D5=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);while(((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1D4)> 0){struct Cyc_Set_Set*_tmp1D6=
emptyset;struct Cyc_Iter_Iter _tmp1D7=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1D4);while(((
int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1D7,&
_tmp1D5)){_tmp1D6=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(_tmp1D6,Cyc_find(t,_tmp1D5));}_tmp1D4=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1D6,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp1D4);}return curr;};}enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 
1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering();static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER
 || Cyc_mode == Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int
Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp53C;void*_tmp53B;(_tmp53B=0,Cyc_fprintf(Cyc_stderr,((_tmp53C="Internal error: script file is NULL\n",
_tag_dyneither(_tmp53C,sizeof(char),37))),_tag_dyneither(_tmp53B,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d);int Cyc_force_directory(
struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT){const char*_tmp541;void*
_tmp540[2];struct Cyc_String_pa_struct _tmp53F;struct Cyc_String_pa_struct _tmp53E;(
_tmp53E.tag=0,((_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp53F.tag=0,((_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp540[0]=& _tmp53F,((_tmp540[1]=& _tmp53E,Cyc_prscript(((_tmp541="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp541,sizeof(char),36))),_tag_dyneither(_tmp540,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp542[0];int _tmp1DF=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp542,sizeof(
unsigned short),0));if(_tmp1DF == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp546;void*_tmp545[1];struct Cyc_String_pa_struct
_tmp544;(_tmp544.tag=0,((_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp545[0]=& _tmp544,Cyc_fprintf(Cyc_stderr,((_tmp546="Error: could not create directory %s\n",
_tag_dyneither(_tmp546,sizeof(char),38))),_tag_dyneither(_tmp545,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp1DF);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file);int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
struct _dyneither_ptr _tmp1E4=Cyc_strdup((struct _dyneither_ptr)file);struct Cyc_List_List*
_tmp1E5=0;while(1){_tmp1E4=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1E4);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1E4)== 0)break;{struct _dyneither_ptr*
_tmp549;struct Cyc_List_List*_tmp548;_tmp1E5=((_tmp548=_cycalloc(sizeof(*_tmp548)),((
_tmp548->hd=((_tmp549=_cycalloc(sizeof(*_tmp549)),((_tmp549[0]=(struct
_dyneither_ptr)_tmp1E4,_tmp549)))),((_tmp548->tl=_tmp1E5,_tmp548))))));};}for(0;
_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){if(Cyc_force_directory(*((struct _dyneither_ptr*)
_tmp1E5->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_struct{
char*tag;struct _dyneither_ptr f1;};struct _tuple22{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;};int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs);int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;{const char*_tmp54F;void*_tmp54E[1];const char*_tmp54D;
struct Cyc_String_pa_struct _tmp54C;(_tmp54C.tag=0,((_tmp54C.f1=(struct
_dyneither_ptr)((_tmp54D=filename,_tag_dyneither(_tmp54D,sizeof(char),
_get_zero_arr_size_char(_tmp54D,1)))),((_tmp54E[0]=& _tmp54C,Cyc_fprintf(Cyc_stderr,((
_tmp54F="********************************* %s...\n",_tag_dyneither(_tmp54F,
sizeof(char),41))),_tag_dyneither(_tmp54E,sizeof(void*),1)))))));}if(!Cyc_gathering()){
const char*_tmp555;void*_tmp554[1];const char*_tmp553;struct Cyc_String_pa_struct
_tmp552;(_tmp552.tag=0,((_tmp552.f1=(struct _dyneither_ptr)((_tmp553=filename,
_tag_dyneither(_tmp553,sizeof(char),_get_zero_arr_size_char(_tmp553,1)))),((
_tmp554[0]=& _tmp552,Cyc_log(((_tmp555="\n%s:\n",_tag_dyneither(_tmp555,sizeof(
char),6))),_tag_dyneither(_tmp554,sizeof(void*),1)))))));}{const char*_tmp556;
struct _dyneither_ptr _tmp1F0=Cyc_Filename_basename(((_tmp556=filename,
_tag_dyneither(_tmp556,sizeof(char),_get_zero_arr_size_char(_tmp556,1)))));const
char*_tmp557;struct _dyneither_ptr _tmp1F1=Cyc_Filename_dirname(((_tmp557=filename,
_tag_dyneither(_tmp557,sizeof(char),_get_zero_arr_size_char(_tmp557,1)))));
struct _dyneither_ptr _tmp1F2=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp1F0);const char*_tmp558;const char*_tmp1F3=(const char*)_untag_dyneither_ptr(
Cyc_strconcat((struct _dyneither_ptr)_tmp1F2,((_tmp558=".iA",_tag_dyneither(
_tmp558,sizeof(char),4)))),sizeof(char),1);const char*_tmp560;void*_tmp55F[1];
struct Cyc_String_pa_struct _tmp55E;const char*_tmp55C;void*_tmp55B[1];struct Cyc_String_pa_struct
_tmp55A;const char*_tmp1F4=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1F1,sizeof(char))== 0?(_tmp55E.tag=0,((_tmp55E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F2),((_tmp55F[0]=& _tmp55E,Cyc_aprintf(((_tmp560="%s.iB",
_tag_dyneither(_tmp560,sizeof(char),6))),_tag_dyneither(_tmp55F,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F1,(struct _dyneither_ptr)((
_tmp55A.tag=0,((_tmp55A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F2),((
_tmp55B[0]=& _tmp55A,Cyc_aprintf(((_tmp55C="%s.iB",_tag_dyneither(_tmp55C,sizeof(
char),6))),_tag_dyneither(_tmp55B,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp568;void*_tmp567[1];struct Cyc_String_pa_struct _tmp566;const char*_tmp564;
void*_tmp563[1];struct Cyc_String_pa_struct _tmp562;const char*_tmp1F5=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F1,sizeof(char))== 0?(_tmp566.tag=
0,((_tmp566.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F2),((_tmp567[0]=&
_tmp566,Cyc_aprintf(((_tmp568="%s.iC",_tag_dyneither(_tmp568,sizeof(char),6))),
_tag_dyneither(_tmp567,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp1F1,(struct _dyneither_ptr)((_tmp562.tag=0,((_tmp562.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F2),((_tmp563[0]=& _tmp562,Cyc_aprintf(((
_tmp564="%s.iC",_tag_dyneither(_tmp564,sizeof(char),6))),_tag_dyneither(_tmp563,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp570;void*_tmp56F[1];
struct Cyc_String_pa_struct _tmp56E;const char*_tmp56C;void*_tmp56B[1];struct Cyc_String_pa_struct
_tmp56A;const char*_tmp1F6=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1F1,sizeof(char))== 0?(_tmp56E.tag=0,((_tmp56E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F2),((_tmp56F[0]=& _tmp56E,Cyc_aprintf(((_tmp570="%s.iD",
_tag_dyneither(_tmp570,sizeof(char),6))),_tag_dyneither(_tmp56F,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F1,(struct _dyneither_ptr)((
_tmp56A.tag=0,((_tmp56A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F2),((
_tmp56B[0]=& _tmp56A,Cyc_aprintf(((_tmp56C="%s.iD",_tag_dyneither(_tmp56C,sizeof(
char),6))),_tag_dyneither(_tmp56B,sizeof(void*),1))))))))),sizeof(char),1);
struct _handler_cons _tmp1F7;_push_handler(& _tmp1F7);{int _tmp1F9=0;if(setjmp(
_tmp1F7.handler))_tmp1F9=1;if(!_tmp1F9){{const char*_tmp571;if(Cyc_force_directory_prefixes(((
_tmp571=filename,_tag_dyneither(_tmp571,sizeof(char),_get_zero_arr_size_char(
_tmp571,1)))))){int _tmp1FB=1;_npop_handler(0);return _tmp1FB;}}if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp577;void*_tmp576[
1];const char*_tmp575;struct Cyc_String_pa_struct _tmp574;(_tmp574.tag=0,((_tmp574.f1=(
struct _dyneither_ptr)((_tmp575=_tmp1F3,_tag_dyneither(_tmp575,sizeof(char),
_get_zero_arr_size_char(_tmp575,1)))),((_tmp576[0]=& _tmp574,Cyc_prscript(((
_tmp577="cat >%s <<XXX\n",_tag_dyneither(_tmp577,sizeof(char),15))),
_tag_dyneither(_tmp576,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp200=Cyc_current_cpp;
for(0;_tmp200 != 0;_tmp200=_tmp200->tl){const char*_tmp57B;void*_tmp57A[1];struct
Cyc_String_pa_struct _tmp579;(_tmp579.tag=0,((_tmp579.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp200->hd)),((_tmp57A[0]=&
_tmp579,Cyc_prscript(((_tmp57B="%s",_tag_dyneither(_tmp57B,sizeof(char),3))),
_tag_dyneither(_tmp57A,sizeof(void*),1)))))));}}{const char*_tmp581;void*_tmp580[
1];const char*_tmp57F;struct Cyc_String_pa_struct _tmp57E;(_tmp57E.tag=0,((_tmp57E.f1=(
struct _dyneither_ptr)((_tmp57F=filename,_tag_dyneither(_tmp57F,sizeof(char),
_get_zero_arr_size_char(_tmp57F,1)))),((_tmp580[0]=& _tmp57E,Cyc_prscript(((
_tmp581="#include <%s>\n",_tag_dyneither(_tmp581,sizeof(char),15))),
_tag_dyneither(_tmp580,sizeof(void*),1)))))));}{const char*_tmp584;void*_tmp583;(
_tmp583=0,Cyc_prscript(((_tmp584="XXX\n",_tag_dyneither(_tmp584,sizeof(char),5))),
_tag_dyneither(_tmp583,sizeof(void*),0)));}{const char*_tmp58D;void*_tmp58C[2];
const char*_tmp58B;struct Cyc_String_pa_struct _tmp58A;const char*_tmp589;struct Cyc_String_pa_struct
_tmp588;(_tmp588.tag=0,((_tmp588.f1=(struct _dyneither_ptr)((_tmp589=_tmp1F3,
_tag_dyneither(_tmp589,sizeof(char),_get_zero_arr_size_char(_tmp589,1)))),((
_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((_tmp58B=_tmp1F4,
_tag_dyneither(_tmp58B,sizeof(char),_get_zero_arr_size_char(_tmp58B,1)))),((
_tmp58C[0]=& _tmp58A,((_tmp58C[1]=& _tmp588,Cyc_prscript(((_tmp58D="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp58D,sizeof(char),32))),_tag_dyneither(_tmp58C,sizeof(void*),2)))))))))))));}{
const char*_tmp596;void*_tmp595[2];const char*_tmp594;struct Cyc_String_pa_struct
_tmp593;const char*_tmp592;struct Cyc_String_pa_struct _tmp591;(_tmp591.tag=0,((
_tmp591.f1=(struct _dyneither_ptr)((_tmp592=_tmp1F3,_tag_dyneither(_tmp592,
sizeof(char),_get_zero_arr_size_char(_tmp592,1)))),((_tmp593.tag=0,((_tmp593.f1=(
struct _dyneither_ptr)((_tmp594=_tmp1F5,_tag_dyneither(_tmp594,sizeof(char),
_get_zero_arr_size_char(_tmp594,1)))),((_tmp595[0]=& _tmp593,((_tmp595[1]=&
_tmp591,Cyc_prscript(((_tmp596="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(
_tmp596,sizeof(char),28))),_tag_dyneither(_tmp595,sizeof(void*),2)))))))))))));}{
const char*_tmp59C;void*_tmp59B[1];const char*_tmp59A;struct Cyc_String_pa_struct
_tmp599;(_tmp599.tag=0,((_tmp599.f1=(struct _dyneither_ptr)((_tmp59A=_tmp1F3,
_tag_dyneither(_tmp59A,sizeof(char),_get_zero_arr_size_char(_tmp59A,1)))),((
_tmp59B[0]=& _tmp599,Cyc_prscript(((_tmp59C="rm %s\n",_tag_dyneither(_tmp59C,
sizeof(char),7))),_tag_dyneither(_tmp59B,sizeof(void*),1)))))));};}else{maybe=
Cyc_fopen(_tmp1F3,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp5A2;
void*_tmp5A1[1];const char*_tmp5A0;struct Cyc_String_pa_struct _tmp59F;(_tmp59F.tag=
0,((_tmp59F.f1=(struct _dyneither_ptr)((_tmp5A0=_tmp1F3,_tag_dyneither(_tmp5A0,
sizeof(char),_get_zero_arr_size_char(_tmp5A0,1)))),((_tmp5A1[0]=& _tmp59F,Cyc_fprintf(
Cyc_stderr,((_tmp5A2="Error: could not create file %s\n",_tag_dyneither(_tmp5A2,
sizeof(char),33))),_tag_dyneither(_tmp5A1,sizeof(void*),1)))))));}{int _tmp21E=1;
_npop_handler(0);return _tmp21E;};}out_file=(struct Cyc___cycFILE*)maybe;{struct
Cyc_List_List*_tmp21F=Cyc_current_cpp;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){Cyc_fputs((
const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp21F->hd),sizeof(
char),1),out_file);}}{const char*_tmp5A8;void*_tmp5A7[1];const char*_tmp5A6;struct
Cyc_String_pa_struct _tmp5A5;(_tmp5A5.tag=0,((_tmp5A5.f1=(struct _dyneither_ptr)((
_tmp5A6=filename,_tag_dyneither(_tmp5A6,sizeof(char),_get_zero_arr_size_char(
_tmp5A6,1)))),((_tmp5A7[0]=& _tmp5A5,Cyc_fprintf(out_file,((_tmp5A8="#include <%s>\n",
_tag_dyneither(_tmp5A8,sizeof(char),15))),_tag_dyneither(_tmp5A7,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{struct _dyneither_ptr _tmp224=Cstring_to_string(Ccomp);const
char*_tmp5B2;void*_tmp5B1[3];struct Cyc_String_pa_struct _tmp5B0;const char*_tmp5AF;
struct Cyc_String_pa_struct _tmp5AE;const char*_tmp5AD;struct Cyc_String_pa_struct
_tmp5AC;char*cmd=(char*)_untag_dyneither_ptr(((_tmp5AC.tag=0,((_tmp5AC.f1=(
struct _dyneither_ptr)((_tmp5AD=_tmp1F3,_tag_dyneither(_tmp5AD,sizeof(char),
_get_zero_arr_size_char(_tmp5AD,1)))),((_tmp5AE.tag=0,((_tmp5AE.f1=(struct
_dyneither_ptr)((_tmp5AF=_tmp1F4,_tag_dyneither(_tmp5AF,sizeof(char),
_get_zero_arr_size_char(_tmp5AF,1)))),((_tmp5B0.tag=0,((_tmp5B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp224),((_tmp5B1[0]=& _tmp5B0,((_tmp5B1[1]=&
_tmp5AE,((_tmp5B1[2]=& _tmp5AC,Cyc_aprintf(((_tmp5B2="%s -E -dM -o %s -x c %s",
_tag_dyneither(_tmp5B2,sizeof(char),24))),_tag_dyneither(_tmp5B1,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp5BC;void*_tmp5BB[3];
struct Cyc_String_pa_struct _tmp5BA;const char*_tmp5B9;struct Cyc_String_pa_struct
_tmp5B8;const char*_tmp5B7;struct Cyc_String_pa_struct _tmp5B6;cmd=(char*)
_untag_dyneither_ptr(((_tmp5B6.tag=0,((_tmp5B6.f1=(struct _dyneither_ptr)((
_tmp5B7=_tmp1F3,_tag_dyneither(_tmp5B7,sizeof(char),_get_zero_arr_size_char(
_tmp5B7,1)))),((_tmp5B8.tag=0,((_tmp5B8.f1=(struct _dyneither_ptr)((_tmp5B9=
_tmp1F5,_tag_dyneither(_tmp5B9,sizeof(char),_get_zero_arr_size_char(_tmp5B9,1)))),((
_tmp5BA.tag=0,((_tmp5BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp224),((
_tmp5BB[0]=& _tmp5BA,((_tmp5BB[1]=& _tmp5B8,((_tmp5BB[2]=& _tmp5B6,Cyc_aprintf(((
_tmp5BC="%s -E -o %s -x c %s",_tag_dyneither(_tmp5BC,sizeof(char),20))),
_tag_dyneither(_tmp5BB,sizeof(void*),3)))))))))))))))))))),sizeof(char),1);}
system((const char*)cmd);}remove(_tmp1F3);};}}if(Cyc_gathering()){int _tmp233=0;
_npop_handler(0);return _tmp233;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1F4,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5CD;const char*_tmp5CC;void*_tmp5CB[1];const char*_tmp5CA;struct Cyc_String_pa_struct
_tmp5C9;struct Cyc_NO_SUPPORT_struct*_tmp5C8;(int)_throw((void*)((_tmp5C8=
_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8[0]=((_tmp5CD.tag=Cyc_NO_SUPPORT,((_tmp5CD.f1=(
struct _dyneither_ptr)((_tmp5C9.tag=0,((_tmp5C9.f1=(struct _dyneither_ptr)((
_tmp5CA=_tmp1F4,_tag_dyneither(_tmp5CA,sizeof(char),_get_zero_arr_size_char(
_tmp5CA,1)))),((_tmp5CB[0]=& _tmp5C9,Cyc_aprintf(((_tmp5CC="can't open macrosfile %s",
_tag_dyneither(_tmp5CC,sizeof(char),25))),_tag_dyneither(_tmp5CB,sizeof(void*),1)))))))),
_tmp5CD)))),_tmp5C8)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp23A=Cyc_Lexing_from_file(in_file);struct _tuple16*entry;while((entry=((struct
_tuple16*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp23A))!= 0){struct
_tuple16 _tmp23C;struct _dyneither_ptr*_tmp23D;struct Cyc_Set_Set*_tmp23E;struct
_tuple16*_tmp23B=(struct _tuple16*)_check_null(entry);_tmp23C=*_tmp23B;_tmp23D=
_tmp23C.f1;_tmp23E=_tmp23C.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp23D,
_tmp23E);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp1F5,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp5DE;const char*_tmp5DD;void*
_tmp5DC[1];const char*_tmp5DB;struct Cyc_String_pa_struct _tmp5DA;struct Cyc_NO_SUPPORT_struct*
_tmp5D9;(int)_throw((void*)((_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9[0]=((
_tmp5DE.tag=Cyc_NO_SUPPORT,((_tmp5DE.f1=(struct _dyneither_ptr)((_tmp5DA.tag=0,((
_tmp5DA.f1=(struct _dyneither_ptr)((_tmp5DB=_tmp1F5,_tag_dyneither(_tmp5DB,
sizeof(char),_get_zero_arr_size_char(_tmp5DB,1)))),((_tmp5DC[0]=& _tmp5DA,Cyc_aprintf(((
_tmp5DD="can't open declsfile %s",_tag_dyneither(_tmp5DD,sizeof(char),24))),
_tag_dyneither(_tmp5DC,sizeof(void*),1)))))))),_tmp5DE)))),_tmp5D9)))));}in_file=(
struct Cyc___cycFILE*)maybe;_tmp23A=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp1F6,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp245=
1;_npop_handler(0);return _tmp245;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp23A)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp1F5);maybe=Cyc_fopen(
_tmp1F6,(const char*)"r");if(!((unsigned int)maybe)){int _tmp246=1;_npop_handler(0);
return _tmp246;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5DF;Cyc_Position_reset_position(((
_tmp5DF=_tmp1F6,_tag_dyneither(_tmp5DF,sizeof(char),_get_zero_arr_size_char(
_tmp5DF,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp248=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp1F6);{struct Cyc_List_List*
_tmp249=_tmp248;for(0;_tmp249 != 0;_tmp249=_tmp249->tl){struct _tuple16*_tmp24A=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp249->hd);if(_tmp24A == 0)continue;{
struct _tuple16 _tmp24C;struct _dyneither_ptr*_tmp24D;struct Cyc_Set_Set*_tmp24E;
struct _tuple16*_tmp24B=(struct _tuple16*)_tmp24A;_tmp24C=*_tmp24B;_tmp24D=_tmp24C.f1;
_tmp24E=_tmp24C.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp24F;
_push_handler(& _tmp24F);{int _tmp251=0;if(setjmp(_tmp24F.handler))_tmp251=1;if(!
_tmp251){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp24D);;_pop_handler();}else{void*
_tmp250=(void*)_exn_thrown;void*_tmp253=_tmp250;_LL17E: {struct Cyc_Core_Not_found_struct*
_tmp254=(struct Cyc_Core_Not_found_struct*)_tmp253;if(_tmp254->tag != Cyc_Core_Not_found)
goto _LL180;}_LL17F: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17D;_LL180:;_LL181:(
void)_throw(_tmp253);_LL17D:;}};}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp24D,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp24E,old));};};}}{struct Cyc_Set_Set*_tmp255=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp256=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp257=_tmp248;for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
struct Cyc_Absyn_Decl*_tmp258=(struct Cyc_Absyn_Decl*)_tmp257->hd;struct
_dyneither_ptr*name;{void*_tmp259=_tmp258->r;struct Cyc_Absyn_Vardecl*_tmp25B;
struct Cyc_Absyn_Fndecl*_tmp25D;struct Cyc_Absyn_Aggrdecl*_tmp25F;struct Cyc_Absyn_Enumdecl*
_tmp261;struct Cyc_Absyn_Typedefdecl*_tmp263;_LL183: {struct Cyc_Absyn_Var_d_struct*
_tmp25A=(struct Cyc_Absyn_Var_d_struct*)_tmp259;if(_tmp25A->tag != 0)goto _LL185;
else{_tmp25B=_tmp25A->f1;}}_LL184: {struct _tuple0 _tmp270;struct _dyneither_ptr*
_tmp271;struct _tuple0*_tmp26F=_tmp25B->name;_tmp270=*_tmp26F;_tmp271=_tmp270.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp271);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp271))name=0;else{
name=(struct _dyneither_ptr*)_tmp271;}goto _LL182;}_LL185: {struct Cyc_Absyn_Fn_d_struct*
_tmp25C=(struct Cyc_Absyn_Fn_d_struct*)_tmp259;if(_tmp25C->tag != 1)goto _LL187;
else{_tmp25D=_tmp25C->f1;}}_LL186: {struct _tuple0 _tmp273;struct _dyneither_ptr*
_tmp274;struct _tuple0*_tmp272=_tmp25D->name;_tmp273=*_tmp272;_tmp274=_tmp273.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp274);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp274))name=0;else{
name=(struct _dyneither_ptr*)_tmp274;}goto _LL182;}_LL187: {struct Cyc_Absyn_Aggr_d_struct*
_tmp25E=(struct Cyc_Absyn_Aggr_d_struct*)_tmp259;if(_tmp25E->tag != 6)goto _LL189;
else{_tmp25F=_tmp25E->f1;}}_LL188: {struct _tuple0 _tmp276;struct _dyneither_ptr*
_tmp277;struct _tuple0*_tmp275=_tmp25F->name;_tmp276=*_tmp275;_tmp277=_tmp276.f2;
name=(struct _dyneither_ptr*)_tmp277;goto _LL182;}_LL189: {struct Cyc_Absyn_Enum_d_struct*
_tmp260=(struct Cyc_Absyn_Enum_d_struct*)_tmp259;if(_tmp260->tag != 8)goto _LL18B;
else{_tmp261=_tmp260->f1;}}_LL18A: {struct _tuple0 _tmp279;struct _dyneither_ptr*
_tmp27A;struct _tuple0*_tmp278=_tmp261->name;_tmp279=*_tmp278;_tmp27A=_tmp279.f2;
name=(struct _dyneither_ptr*)_tmp27A;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp255,(struct _dyneither_ptr*)name)){
struct Cyc_List_List*_tmp5E0;_tmp256=((_tmp5E0=_cycalloc(sizeof(*_tmp5E0)),((
_tmp5E0->hd=_tmp258,((_tmp5E0->tl=_tmp256,_tmp5E0))))));}else{if((unsigned int)
_tmp261->fields){struct Cyc_List_List*_tmp27C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp261->fields))->v;for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){struct
Cyc_Absyn_Enumfield*_tmp27D=(struct Cyc_Absyn_Enumfield*)_tmp27C->hd;struct
_tuple0 _tmp27F;struct _dyneither_ptr*_tmp280;struct _tuple0*_tmp27E=_tmp27D->name;
_tmp27F=*_tmp27E;_tmp280=_tmp27F.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp255,_tmp280)){{struct Cyc_List_List*
_tmp5E1;_tmp256=((_tmp5E1=_cycalloc(sizeof(*_tmp5E1)),((_tmp5E1->hd=_tmp258,((
_tmp5E1->tl=_tmp256,_tmp5E1))))));}break;}}}}name=0;goto _LL182;}_LL18B: {struct
Cyc_Absyn_Typedef_d_struct*_tmp262=(struct Cyc_Absyn_Typedef_d_struct*)_tmp259;
if(_tmp262->tag != 9)goto _LL18D;else{_tmp263=_tmp262->f1;}}_LL18C: {struct _tuple0
_tmp283;struct _dyneither_ptr*_tmp284;struct _tuple0*_tmp282=_tmp263->name;_tmp283=*
_tmp282;_tmp284=_tmp283.f2;name=(struct _dyneither_ptr*)_tmp284;goto _LL182;}
_LL18D: {struct Cyc_Absyn_Porton_d_struct*_tmp264=(struct Cyc_Absyn_Porton_d_struct*)
_tmp259;if(_tmp264->tag != 14)goto _LL18F;}_LL18E: goto _LL190;_LL18F: {struct Cyc_Absyn_Portoff_d_struct*
_tmp265=(struct Cyc_Absyn_Portoff_d_struct*)_tmp259;if(_tmp265->tag != 15)goto
_LL191;}_LL190: goto _LL192;_LL191: {struct Cyc_Absyn_Let_d_struct*_tmp266=(struct
Cyc_Absyn_Let_d_struct*)_tmp259;if(_tmp266->tag != 2)goto _LL193;}_LL192: goto
_LL194;_LL193: {struct Cyc_Absyn_Datatype_d_struct*_tmp267=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp259;if(_tmp267->tag != 7)goto _LL195;}_LL194: goto _LL196;_LL195: {struct Cyc_Absyn_Letv_d_struct*
_tmp268=(struct Cyc_Absyn_Letv_d_struct*)_tmp259;if(_tmp268->tag != 3)goto _LL197;}
_LL196: goto _LL198;_LL197: {struct Cyc_Absyn_Namespace_d_struct*_tmp269=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp259;if(_tmp269->tag != 10)goto _LL199;}_LL198: goto _LL19A;_LL199: {struct Cyc_Absyn_Using_d_struct*
_tmp26A=(struct Cyc_Absyn_Using_d_struct*)_tmp259;if(_tmp26A->tag != 11)goto _LL19B;}
_LL19A: goto _LL19C;_LL19B: {struct Cyc_Absyn_ExternC_d_struct*_tmp26B=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp259;if(_tmp26B->tag != 12)goto _LL19D;}_LL19C: goto _LL19E;_LL19D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp26C=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp259;if(_tmp26C->tag != 13)
goto _LL19F;}_LL19E: goto _LL1A0;_LL19F: {struct Cyc_Absyn_Region_d_struct*_tmp26D=(
struct Cyc_Absyn_Region_d_struct*)_tmp259;if(_tmp26D->tag != 4)goto _LL1A1;}_LL1A0:
goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Alias_d_struct*_tmp26E=(struct Cyc_Absyn_Alias_d_struct*)
_tmp259;if(_tmp26E->tag != 5)goto _LL182;}_LL1A2: name=0;goto _LL182;_LL182:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp255,(struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp5E2;_tmp256=((
_tmp5E2=_cycalloc(sizeof(*_tmp5E2)),((_tmp5E2->hd=_tmp258,((_tmp5E2->tl=_tmp256,
_tmp5E2))))));}}}if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){int _tmp286=1;_npop_handler(0);return _tmp286;}out_file=(
struct Cyc___cycFILE*)maybe;}else{out_file=Cyc_stdout;}{const char*_tmp5E8;void*
_tmp5E7[1];const char*_tmp5E6;struct Cyc_String_pa_struct _tmp5E5;struct
_dyneither_ptr ifdefmacro=(_tmp5E5.tag=0,((_tmp5E5.f1=(struct _dyneither_ptr)((
_tmp5E6=filename,_tag_dyneither(_tmp5E6,sizeof(char),_get_zero_arr_size_char(
_tmp5E6,1)))),((_tmp5E7[0]=& _tmp5E5,Cyc_aprintf(((_tmp5E8="_%s_",_tag_dyneither(
_tmp5E8,sizeof(char),5))),_tag_dyneither(_tmp5E7,sizeof(void*),1)))))));{int
_tmp287=0;for(0;_tmp287 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp287){
if(((char*)ifdefmacro.curr)[_tmp287]== '.'  || ((char*)ifdefmacro.curr)[_tmp287]
== '/'){char _tmp5EB;char _tmp5EA;struct _dyneither_ptr _tmp5E9;(_tmp5E9=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp287),((_tmp5EA=*((char*)
_check_dyneither_subscript(_tmp5E9,sizeof(char),0)),((_tmp5EB='_',((
_get_dyneither_size(_tmp5E9,sizeof(char))== 1  && (_tmp5EA == '\000'  && _tmp5EB != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5E9.curr)=_tmp5EB)))))));}else{if(((char*)
ifdefmacro.curr)[_tmp287]!= '_'  && ((char*)ifdefmacro.curr)[_tmp287]!= '/'){char
_tmp5EE;char _tmp5ED;struct _dyneither_ptr _tmp5EC;(_tmp5EC=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp287),((_tmp5ED=*((char*)_check_dyneither_subscript(
_tmp5EC,sizeof(char),0)),((_tmp5EE=(char)toupper((int)((char*)ifdefmacro.curr)[
_tmp287]),((_get_dyneither_size(_tmp5EC,sizeof(char))== 1  && (_tmp5ED == '\000'
 && _tmp5EE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EC.curr)=_tmp5EE)))))));}}}}{
const char*_tmp5F3;void*_tmp5F2[2];struct Cyc_String_pa_struct _tmp5F1;struct Cyc_String_pa_struct
_tmp5F0;(_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp5F2[0]=& _tmp5F1,((_tmp5F2[1]=& _tmp5F0,Cyc_fprintf(
out_file,((_tmp5F3="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp5F3,sizeof(char),
23))),_tag_dyneither(_tmp5F2,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp292=0;struct Cyc_List_List*_tmp293=0;{struct Cyc_List_List*_tmp294=_tmp256;
for(0;_tmp294 != 0;_tmp294=_tmp294->tl){struct Cyc_Absyn_Decl*_tmp295=(struct Cyc_Absyn_Decl*)
_tmp294->hd;int _tmp296=0;struct _dyneither_ptr*name;{void*_tmp297=_tmp295->r;
struct Cyc_Absyn_Vardecl*_tmp299;struct Cyc_Absyn_Fndecl*_tmp29B;struct Cyc_Absyn_Aggrdecl*
_tmp29D;struct Cyc_Absyn_Enumdecl*_tmp29F;struct Cyc_Absyn_Typedefdecl*_tmp2A1;
_LL1A4: {struct Cyc_Absyn_Var_d_struct*_tmp298=(struct Cyc_Absyn_Var_d_struct*)
_tmp297;if(_tmp298->tag != 0)goto _LL1A6;else{_tmp299=_tmp298->f1;}}_LL1A5: {
struct _tuple0 _tmp2AE;struct _dyneither_ptr*_tmp2AF;struct _tuple0*_tmp2AD=_tmp299->name;
_tmp2AE=*_tmp2AD;_tmp2AF=_tmp2AE.f2;name=(struct _dyneither_ptr*)_tmp2AF;goto
_LL1A3;}_LL1A6: {struct Cyc_Absyn_Fn_d_struct*_tmp29A=(struct Cyc_Absyn_Fn_d_struct*)
_tmp297;if(_tmp29A->tag != 1)goto _LL1A8;else{_tmp29B=_tmp29A->f1;}}_LL1A7: {
struct _tuple0 _tmp2B1;struct _dyneither_ptr*_tmp2B2;struct _tuple0*_tmp2B0=_tmp29B->name;
_tmp2B1=*_tmp2B0;_tmp2B2=_tmp2B1.f2;name=(struct _dyneither_ptr*)_tmp2B2;goto
_LL1A3;}_LL1A8: {struct Cyc_Absyn_Aggr_d_struct*_tmp29C=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp297;if(_tmp29C->tag != 6)goto _LL1AA;else{_tmp29D=_tmp29C->f1;}}_LL1A9: {
struct _tuple0 _tmp2B4;struct _dyneither_ptr*_tmp2B5;struct _tuple0*_tmp2B3=_tmp29D->name;
_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B4.f2;name=(struct _dyneither_ptr*)_tmp2B5;goto
_LL1A3;}_LL1AA: {struct Cyc_Absyn_Enum_d_struct*_tmp29E=(struct Cyc_Absyn_Enum_d_struct*)
_tmp297;if(_tmp29E->tag != 8)goto _LL1AC;else{_tmp29F=_tmp29E->f1;}}_LL1AB: {
struct _tuple0 _tmp2B7;struct _dyneither_ptr*_tmp2B8;struct _tuple0*_tmp2B6=_tmp29F->name;
_tmp2B7=*_tmp2B6;_tmp2B8=_tmp2B7.f2;name=(struct _dyneither_ptr*)_tmp2B8;goto
_LL1A3;}_LL1AC: {struct Cyc_Absyn_Typedef_d_struct*_tmp2A0=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp297;if(_tmp2A0->tag != 9)goto _LL1AE;else{_tmp2A1=_tmp2A0->f1;}}_LL1AD: {
struct _tuple0 _tmp2BA;struct _dyneither_ptr*_tmp2BB;struct _tuple0*_tmp2B9=_tmp2A1->name;
_tmp2BA=*_tmp2B9;_tmp2BB=_tmp2BA.f2;name=(struct _dyneither_ptr*)_tmp2BB;goto
_LL1A3;}_LL1AE: {struct Cyc_Absyn_Region_d_struct*_tmp2A2=(struct Cyc_Absyn_Region_d_struct*)
_tmp297;if(_tmp2A2->tag != 4)goto _LL1B0;}_LL1AF: goto _LL1B1;_LL1B0: {struct Cyc_Absyn_Alias_d_struct*
_tmp2A3=(struct Cyc_Absyn_Alias_d_struct*)_tmp297;if(_tmp2A3->tag != 5)goto _LL1B2;}
_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_Porton_d_struct*_tmp2A4=(struct Cyc_Absyn_Porton_d_struct*)
_tmp297;if(_tmp2A4->tag != 14)goto _LL1B4;}_LL1B3: goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Portoff_d_struct*
_tmp2A5=(struct Cyc_Absyn_Portoff_d_struct*)_tmp297;if(_tmp2A5->tag != 15)goto
_LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Let_d_struct*_tmp2A6=(struct
Cyc_Absyn_Let_d_struct*)_tmp297;if(_tmp2A6->tag != 2)goto _LL1B8;}_LL1B7: goto
_LL1B9;_LL1B8: {struct Cyc_Absyn_Datatype_d_struct*_tmp2A7=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp297;if(_tmp2A7->tag != 7)goto _LL1BA;}_LL1B9: goto _LL1BB;_LL1BA: {struct Cyc_Absyn_Letv_d_struct*
_tmp2A8=(struct Cyc_Absyn_Letv_d_struct*)_tmp297;if(_tmp2A8->tag != 3)goto _LL1BC;}
_LL1BB: goto _LL1BD;_LL1BC: {struct Cyc_Absyn_Namespace_d_struct*_tmp2A9=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp297;if(_tmp2A9->tag != 10)goto _LL1BE;}_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_Using_d_struct*
_tmp2AA=(struct Cyc_Absyn_Using_d_struct*)_tmp297;if(_tmp2AA->tag != 11)goto _LL1C0;}
_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_ExternC_d_struct*_tmp2AB=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp297;if(_tmp2AB->tag != 12)goto _LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp2AC=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp297;if(_tmp2AC->tag != 13)
goto _LL1A3;}_LL1C3: name=0;goto _LL1A3;_LL1A3:;}if(!((unsigned int)name) && !
_tmp296)continue;{struct Cyc_List_List*_tmp5F4;_tmp292=((_tmp5F4=_cycalloc(
sizeof(*_tmp5F4)),((_tmp5F4->hd=_tmp295,((_tmp5F4->tl=_tmp292,_tmp5F4))))));}{
struct Cyc_List_List*_tmp5F5;_tmp293=((_tmp5F5=_cycalloc(sizeof(*_tmp5F5)),((
_tmp5F5->hd=name,((_tmp5F5->tl=_tmp293,_tmp5F5))))));};}}{struct _RegionHandle
_tmp2BE=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2BE;_push_region(
tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2BF=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,
_tmp2BF,1,_tmp292);}{struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C3;
struct _tuple22 _tmp5F6;struct _tuple22 _tmp2C1=(_tmp5F6.f1=_tmp292,((_tmp5F6.f2=
_tmp293,_tmp5F6)));_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;for(0;_tmp2C2 != 0  && 
_tmp2C3 != 0;(_tmp2C2=_tmp2C2->tl,_tmp2C3=_tmp2C3->tl)){struct Cyc_Absyn_Decl*
_tmp2C4=(struct Cyc_Absyn_Decl*)_tmp2C2->hd;struct _dyneither_ptr*_tmp2C5=(struct
_dyneither_ptr*)_tmp2C3->hd;int _tmp2C6=0;if(!((unsigned int)_tmp2C5))_tmp2C6=1;
if((unsigned int)_tmp2C5){{const char*_tmp5FA;void*_tmp5F9[1];struct Cyc_String_pa_struct
_tmp5F8;ifdefmacro=((_tmp5F8.tag=0,((_tmp5F8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2C5),((_tmp5F9[0]=& _tmp5F8,Cyc_aprintf(((_tmp5FA="_%s_def_",
_tag_dyneither(_tmp5FA,sizeof(char),9))),_tag_dyneither(_tmp5F9,sizeof(void*),1))))))));}{
const char*_tmp5FE;void*_tmp5FD[1];struct Cyc_String_pa_struct _tmp5FC;(_tmp5FC.tag=
0,((_tmp5FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((
_tmp5FD[0]=& _tmp5FC,Cyc_fprintf(out_file,((_tmp5FE="#ifndef %s\n",_tag_dyneither(
_tmp5FE,sizeof(char),12))),_tag_dyneither(_tmp5FD,sizeof(void*),1)))))));}{const
char*_tmp602;void*_tmp601[1];struct Cyc_String_pa_struct _tmp600;(_tmp600.tag=0,((
_tmp600.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp601[0]=&
_tmp600,Cyc_fprintf(out_file,((_tmp602="#define %s\n",_tag_dyneither(_tmp602,
sizeof(char),12))),_tag_dyneither(_tmp601,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp603[1];Cyc_Absynpp_decllist2file(((_tmp603[0]=_tmp2C4,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp603,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp606;void*_tmp605;(_tmp605=0,Cyc_fprintf(out_file,((
_tmp606="#endif\n",_tag_dyneither(_tmp606,sizeof(char),8))),_tag_dyneither(
_tmp605,sizeof(void*),0)));};}else{struct Cyc_Absyn_Decl*_tmp607[1];Cyc_Absynpp_decllist2file(((
_tmp607[0]=_tmp2C4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp607,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp1F4,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp618;const char*_tmp617;void*_tmp616[1];const char*_tmp615;struct Cyc_String_pa_struct
_tmp614;struct Cyc_NO_SUPPORT_struct*_tmp613;(int)_throw((void*)((_tmp613=
_cycalloc(sizeof(*_tmp613)),((_tmp613[0]=((_tmp618.tag=Cyc_NO_SUPPORT,((_tmp618.f1=(
struct _dyneither_ptr)((_tmp614.tag=0,((_tmp614.f1=(struct _dyneither_ptr)((
_tmp615=_tmp1F4,_tag_dyneither(_tmp615,sizeof(char),_get_zero_arr_size_char(
_tmp615,1)))),((_tmp616[0]=& _tmp614,Cyc_aprintf(((_tmp617="can't open macrosfile %s",
_tag_dyneither(_tmp617,sizeof(char),25))),_tag_dyneither(_tmp616,sizeof(void*),1)))))))),
_tmp618)))),_tmp613)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp23A=Cyc_Lexing_from_file(
in_file);{struct _tuple17*entry2;while((entry2=((struct _tuple17*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp23A))!= 0){struct _tuple17 _tmp2DB;struct _dyneither_ptr
_tmp2DC;struct _dyneither_ptr*_tmp2DD;struct _tuple17*_tmp2DA=(struct _tuple17*)
_check_null(entry2);_tmp2DB=*_tmp2DA;_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp255,
_tmp2DD)){{const char*_tmp61C;void*_tmp61B[1];struct Cyc_String_pa_struct _tmp61A;(
_tmp61A.tag=0,((_tmp61A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DD),((
_tmp61B[0]=& _tmp61A,Cyc_fprintf(out_file,((_tmp61C="#ifndef %s\n",_tag_dyneither(
_tmp61C,sizeof(char),12))),_tag_dyneither(_tmp61B,sizeof(void*),1)))))));}{const
char*_tmp620;void*_tmp61F[1];struct Cyc_String_pa_struct _tmp61E;(_tmp61E.tag=0,((
_tmp61E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DC),((_tmp61F[0]=&
_tmp61E,Cyc_fprintf(out_file,((_tmp620="%s\n",_tag_dyneither(_tmp620,sizeof(char),
4))),_tag_dyneither(_tmp61F,sizeof(void*),1)))))));}{const char*_tmp623;void*
_tmp622;(_tmp622=0,Cyc_fprintf(out_file,((_tmp623="#endif\n",_tag_dyneither(
_tmp623,sizeof(char),8))),_tag_dyneither(_tmp622,sizeof(void*),0)));};}}Cyc_fclose(
in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp1F4);if(hstubs != 0){struct Cyc_List_List*
_tmp2E6=hstubs;for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){struct _tuple18 _tmp2E8;
struct _dyneither_ptr _tmp2E9;struct _dyneither_ptr _tmp2EA;struct _tuple18*_tmp2E7=(
struct _tuple18*)_tmp2E6->hd;_tmp2E8=*_tmp2E7;_tmp2E9=_tmp2E8.f1;_tmp2EA=_tmp2E8.f2;{
struct _dyneither_ptr*_tmp624;if(_tmp2EA.curr != (_tag_dyneither(0,0,0)).curr  && (
_tmp2E9.curr == (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp624=_cycalloc(
sizeof(*_tmp624)),((_tmp624[0]=_tmp2E9,_tmp624)))))))Cyc_fputs((const char*)
_untag_dyneither_ptr(_tmp2EA,sizeof(char),1),out_file);else{const char*_tmp628;
void*_tmp627[1];struct Cyc_String_pa_struct _tmp626;(_tmp626.tag=0,((_tmp626.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E9),((_tmp627[0]=& _tmp626,Cyc_log(((
_tmp628="%s is not supported on this platform\n",_tag_dyneither(_tmp628,sizeof(
char),38))),_tag_dyneither(_tmp627,sizeof(void*),1)))))));}};}}{const char*
_tmp62B;void*_tmp62A;(_tmp62A=0,Cyc_fprintf(out_file,((_tmp62B="#endif\n",
_tag_dyneither(_tmp62B,sizeof(char),8))),_tag_dyneither(_tmp62A,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp2F1=0;_npop_handler(1);return _tmp2F1;}else{Cyc_fclose(
out_file);}if(cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2F2=cstubs;for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){struct
_tuple18 _tmp2F4;struct _dyneither_ptr _tmp2F5;struct _dyneither_ptr _tmp2F6;struct
_tuple18*_tmp2F3=(struct _tuple18*)_tmp2F2->hd;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.f1;
_tmp2F6=_tmp2F4.f2;{struct _dyneither_ptr*_tmp62C;if(_tmp2F6.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2F5.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp62C=_cycalloc(sizeof(*_tmp62C)),((_tmp62C[0]=_tmp2F5,
_tmp62C)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2F6,sizeof(char),1),
out_file);};}};}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp632;void*_tmp631[1];const char*_tmp630;struct Cyc_String_pa_struct
_tmp62F;(_tmp62F.tag=0,((_tmp62F.f1=(struct _dyneither_ptr)((_tmp630=filename,
_tag_dyneither(_tmp630,sizeof(char),_get_zero_arr_size_char(_tmp630,1)))),((
_tmp631[0]=& _tmp62F,Cyc_fprintf(out_file,((_tmp632="#include <%s>\n\n",
_tag_dyneither(_tmp632,sizeof(char),16))),_tag_dyneither(_tmp631,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp2FC=cycstubs;for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
struct _tuple18 _tmp2FE;struct _dyneither_ptr _tmp2FF;struct _dyneither_ptr _tmp300;
struct _tuple18*_tmp2FD=(struct _tuple18*)_tmp2FC->hd;_tmp2FE=*_tmp2FD;_tmp2FF=
_tmp2FE.f1;_tmp300=_tmp2FE.f2;{struct _dyneither_ptr*_tmp633;if(_tmp300.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2FF.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp633=_cycalloc(sizeof(*_tmp633)),((_tmp633[0]=_tmp2FF,
_tmp633)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp300,sizeof(char),1),
out_file);};}}{const char*_tmp636;void*_tmp635;(_tmp635=0,Cyc_fprintf(out_file,((
_tmp636="\n",_tag_dyneither(_tmp636,sizeof(char),2))),_tag_dyneither(_tmp635,
sizeof(void*),0)));};}{int _tmp304=0;_npop_handler(1);return _tmp304;};};;
_pop_region(tc_rgn);};};};};};};};;_pop_handler();}else{void*_tmp1F8=(void*)
_exn_thrown;void*_tmp30A=_tmp1F8;struct _dyneither_ptr _tmp30C;_LL1C5: {struct Cyc_NO_SUPPORT_struct*
_tmp30B=(struct Cyc_NO_SUPPORT_struct*)_tmp30A;if(_tmp30B->tag != Cyc_NO_SUPPORT)
goto _LL1C7;else{_tmp30C=_tmp30B->f1;}}_LL1C6:{const char*_tmp63A;void*_tmp639[1];
struct Cyc_String_pa_struct _tmp638;(_tmp638.tag=0,((_tmp638.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp30C),((_tmp639[0]=& _tmp638,Cyc_fprintf(
Cyc_stderr,((_tmp63A="No support because %s\n",_tag_dyneither(_tmp63A,sizeof(
char),23))),_tag_dyneither(_tmp639,sizeof(void*),1)))))));}goto _LL1C8;_LL1C7:;
_LL1C8: maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)maybe)){{
const char*_tmp640;void*_tmp63F[1];const char*_tmp63E;struct Cyc_String_pa_struct
_tmp63D;(_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((_tmp63E=filename,
_tag_dyneither(_tmp63E,sizeof(char),_get_zero_arr_size_char(_tmp63E,1)))),((
_tmp63F[0]=& _tmp63D,Cyc_fprintf(Cyc_stderr,((_tmp640="Error: could not create file %s\n",
_tag_dyneither(_tmp640,sizeof(char),33))),_tag_dyneither(_tmp63F,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp646;void*_tmp645[1];
const char*_tmp644;struct Cyc_String_pa_struct _tmp643;(_tmp643.tag=0,((_tmp643.f1=(
struct _dyneither_ptr)((_tmp644=filename,_tag_dyneither(_tmp644,sizeof(char),
_get_zero_arr_size_char(_tmp644,1)))),((_tmp645[0]=& _tmp643,Cyc_fprintf(out_file,((
_tmp646="#error -- %s is not supported on this platform\n",_tag_dyneither(
_tmp646,sizeof(char),48))),_tag_dyneither(_tmp645,sizeof(void*),1)))))));}Cyc_fclose(
out_file);{const char*_tmp64C;void*_tmp64B[1];const char*_tmp64A;struct Cyc_String_pa_struct
_tmp649;(_tmp649.tag=0,((_tmp649.f1=(struct _dyneither_ptr)((_tmp64A=filename,
_tag_dyneither(_tmp64A,sizeof(char),_get_zero_arr_size_char(_tmp64A,1)))),((
_tmp64B[0]=& _tmp649,Cyc_fprintf(Cyc_stderr,((_tmp64C="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp64C,sizeof(char),52))),_tag_dyneither(_tmp64B,sizeof(void*),1)))))));}{
const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_log(((_tmp64F="Not supported on this platform\n",
_tag_dyneither(_tmp64F,sizeof(char),32))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}
remove(_tmp1F4);remove(_tmp1F5);remove(_tmp1F6);return 0;_LL1C9:;_LL1CA:(void)
_throw(_tmp30A);_LL1C4:;}};};}int Cyc_process_specfile(const char*file,const char*
dir);static void _tmp65A(unsigned int*_tmp659,unsigned int*_tmp658,char**_tmp657){
for(*_tmp659=0;*_tmp659 < *_tmp658;(*_tmp659)++){(*_tmp657)[*_tmp659]='\000';}}
int Cyc_process_specfile(const char*file,const char*dir){struct Cyc___cycFILE*
_tmp333=Cyc_fopen(file,(const char*)"r");if(!((unsigned int)_tmp333)){{const char*
_tmp655;void*_tmp654[1];const char*_tmp653;struct Cyc_String_pa_struct _tmp652;(
_tmp652.tag=0,((_tmp652.f1=(struct _dyneither_ptr)((_tmp653=file,_tag_dyneither(
_tmp653,sizeof(char),_get_zero_arr_size_char(_tmp653,1)))),((_tmp654[0]=& _tmp652,
Cyc_fprintf(Cyc_stderr,((_tmp655="Error: could not open %s\n",_tag_dyneither(
_tmp655,sizeof(char),26))),_tag_dyneither(_tmp654,sizeof(void*),1)))))));}return
1;}{struct Cyc___cycFILE*_tmp338=(struct Cyc___cycFILE*)_tmp333;char*_tmp65F;
unsigned int _tmp65E;char*_tmp65D;unsigned int _tmp65C;unsigned int _tmp65B;struct
_dyneither_ptr buf=(_tmp65F=((_tmp65E=(unsigned int)1024,((_tmp65D=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp65E + 1)),((((_tmp65C=_tmp65E,((
_tmp65A(& _tmp65B,& _tmp65C,& _tmp65D),_tmp65D[_tmp65C]=(char)0)))),_tmp65D)))))),
_tag_dyneither(_tmp65F,sizeof(char),_get_zero_arr_size_char(_tmp65F,(
unsigned int)1024 + 1)));struct _dyneither_ptr _tmp339=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir(
dir)){{const char*_tmp665;void*_tmp664[1];const char*_tmp663;struct Cyc_String_pa_struct
_tmp662;(_tmp662.tag=0,((_tmp662.f1=(struct _dyneither_ptr)((_tmp663=dir,
_tag_dyneither(_tmp663,sizeof(char),_get_zero_arr_size_char(_tmp663,1)))),((
_tmp664[0]=& _tmp662,Cyc_fprintf(Cyc_stderr,((_tmp665="Error: can't change directory to %s\n",
_tag_dyneither(_tmp665,sizeof(char),37))),_tag_dyneither(_tmp664,sizeof(void*),1)))))));}
return 1;}}if(Cyc_mode == Cyc_GATHER){struct _dyneither_ptr _tmp33E=
Cstring_to_string(Ccomp);const char*_tmp669;void*_tmp668[1];struct Cyc_String_pa_struct
_tmp667;system((const char*)_untag_dyneither_ptr(((_tmp667.tag=0,((_tmp667.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33E),((_tmp668[0]=& _tmp667,Cyc_aprintf(((
_tmp669="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp669,sizeof(
char),36))),_tag_dyneither(_tmp668,sizeof(void*),1)))))))),sizeof(char),1));}{
struct Cyc_Lexing_lexbuf*_tmp342=Cyc_Lexing_from_file(_tmp338);struct _tuple19*
entry;while((entry=((struct _tuple19*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp342))!= 0){struct _tuple19 _tmp344;struct _dyneither_ptr _tmp345;struct Cyc_List_List*
_tmp346;struct Cyc_List_List*_tmp347;struct Cyc_List_List*_tmp348;struct Cyc_List_List*
_tmp349;struct Cyc_List_List*_tmp34A;struct _tuple19*_tmp343=(struct _tuple19*)
_check_null(entry);_tmp344=*_tmp343;_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;
_tmp347=_tmp344.f3;_tmp348=_tmp344.f4;_tmp349=_tmp344.f5;_tmp34A=_tmp344.f6;if(
Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp345,sizeof(char),1),
_tmp346,_tmp347,_tmp348,_tmp349,_tmp34A))return 1;}Cyc_fclose(_tmp338);if(Cyc_mode
!= Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp339,
sizeof(char),1)))){{const char*_tmp66D;void*_tmp66C[1];struct Cyc_String_pa_struct
_tmp66B;(_tmp66B.tag=0,((_tmp66B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp339),((_tmp66C[0]=& _tmp66B,Cyc_fprintf(Cyc_stderr,((_tmp66D="Error: could not change directory to %s\n",
_tag_dyneither(_tmp66D,sizeof(char),41))),_tag_dyneither(_tmp66C,sizeof(void*),1)))))));}
return 1;}}return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp672(
unsigned int*_tmp671,unsigned int*_tmp670,char**_tmp66F){for(*_tmp671=0;*_tmp671
< *_tmp670;(*_tmp671)++){(*_tmp66F)[*_tmp671]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp677;unsigned int _tmp676;char*_tmp675;unsigned int _tmp674;
unsigned int _tmp673;struct _dyneither_ptr buf=(_tmp677=((_tmp676=(unsigned int)
1024,((_tmp675=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp676 + 1)),((((
_tmp674=_tmp676,((_tmp672(& _tmp673,& _tmp674,& _tmp675),_tmp675[_tmp674]=(char)0)))),
_tmp675)))))),_tag_dyneither(_tmp677,sizeof(char),_get_zero_arr_size_char(
_tmp677,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp352=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp67D;void*
_tmp67C[1];const char*_tmp67B;struct Cyc_String_pa_struct _tmp67A;(_tmp67A.tag=0,((
_tmp67A.f1=(struct _dyneither_ptr)((_tmp67B=dir,_tag_dyneither(_tmp67B,sizeof(
char),_get_zero_arr_size_char(_tmp67B,1)))),((_tmp67C[0]=& _tmp67A,Cyc_fprintf(
Cyc_stderr,((_tmp67D="Error: can't change directory to %s\n",_tag_dyneither(
_tmp67D,sizeof(char),37))),_tag_dyneither(_tmp67C,sizeof(void*),1)))))));}return
1;}{struct _tuple18*_tmp687;const char*_tmp686;const char*_tmp685;struct _tuple18*
_tmp684[1];struct _dyneither_ptr*_tmp67E[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp67E[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp67E,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp684[0]=((
_tmp687=_cycalloc(sizeof(*_tmp687)),((_tmp687->f1=((_tmp686="setjmp",
_tag_dyneither(_tmp686,sizeof(char),7))),((_tmp687->f2=((_tmp685="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp685,sizeof(char),29))),_tmp687)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp684,sizeof(struct
_tuple18*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp352,sizeof(char),1)))){{const char*_tmp68B;void*_tmp68A[1];struct Cyc_String_pa_struct
_tmp689;(_tmp689.tag=0,((_tmp689.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp352),((_tmp68A[0]=& _tmp689,Cyc_fprintf(Cyc_stderr,((_tmp68B="Error: could not change directory to %s\n",
_tag_dyneither(_tmp68B,sizeof(char),41))),_tag_dyneither(_tmp68A,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp364[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp364,_tmp364,_tmp364 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s);static void Cyc_set_output_dir(struct _dyneither_ptr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_dyneither_ptr s);static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*
_tmp68C;Cyc_spec_files=((_tmp68C=_cycalloc(sizeof(*_tmp68C)),((_tmp68C->hd=(
const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp68C->tl=Cyc_spec_files,
_tmp68C))))));}static void Cyc_set_GATHER();static void Cyc_set_GATHER(){Cyc_mode=
Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT();static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}static void Cyc_set_FINISH();static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}static int Cyc_badparse=0;static void Cyc_unsupported_option(
struct _dyneither_ptr s);static void Cyc_unsupported_option(struct _dyneither_ptr s){{
const char*_tmp690;void*_tmp68F[1];struct Cyc_String_pa_struct _tmp68E;(_tmp68E.tag=
0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp68F[0]=&
_tmp68E,Cyc_fprintf(Cyc_stderr,((_tmp690="Unsupported option %s\n",
_tag_dyneither(_tmp690,sizeof(char),23))),_tag_dyneither(_tmp68F,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple23{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct _tuple23*_tmp6FD;const char*
_tmp6FC;const char*_tmp6FB;struct Cyc_Arg_String_spec_struct _tmp6FA;struct Cyc_Arg_String_spec_struct*
_tmp6F9;const char*_tmp6F8;struct _tuple23*_tmp6F7;const char*_tmp6F6;const char*
_tmp6F5;struct Cyc_Arg_Unit_spec_struct _tmp6F4;struct Cyc_Arg_Unit_spec_struct*
_tmp6F3;const char*_tmp6F2;struct _tuple23*_tmp6F1;const char*_tmp6F0;const char*
_tmp6EF;struct Cyc_Arg_Unit_spec_struct _tmp6EE;struct Cyc_Arg_Unit_spec_struct*
_tmp6ED;const char*_tmp6EC;struct _tuple23*_tmp6EB;const char*_tmp6EA;const char*
_tmp6E9;struct Cyc_Arg_Unit_spec_struct _tmp6E8;struct Cyc_Arg_Unit_spec_struct*
_tmp6E7;const char*_tmp6E6;struct _tuple23*_tmp6E5;const char*_tmp6E4;const char*
_tmp6E3;struct Cyc_Arg_Set_spec_struct _tmp6E2;struct Cyc_Arg_Set_spec_struct*
_tmp6E1;const char*_tmp6E0;struct _tuple23*_tmp6DF;const char*_tmp6DE;const char*
_tmp6DD;struct Cyc_Arg_Flag_spec_struct _tmp6DC;struct Cyc_Arg_Flag_spec_struct*
_tmp6DB;const char*_tmp6DA;struct _tuple23*_tmp6D9[6];struct Cyc_List_List*options=(
_tmp6D9[5]=((_tmp6DF=_cycalloc(sizeof(*_tmp6DF)),((_tmp6DF->f1=((_tmp6DE="-",
_tag_dyneither(_tmp6DE,sizeof(char),2))),((_tmp6DF->f2=1,((_tmp6DF->f3=((_tmp6DD="",
_tag_dyneither(_tmp6DD,sizeof(char),1))),((_tmp6DF->f4=(void*)((_tmp6DB=
_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=1,((_tmp6DC.f1=Cyc_unsupported_option,
_tmp6DC)))),_tmp6DB)))),((_tmp6DF->f5=((_tmp6DA="",_tag_dyneither(_tmp6DA,
sizeof(char),1))),_tmp6DF)))))))))))),((_tmp6D9[4]=((_tmp6E5=_cycalloc(sizeof(*
_tmp6E5)),((_tmp6E5->f1=((_tmp6E4="-setjmp",_tag_dyneither(_tmp6E4,sizeof(char),
8))),((_tmp6E5->f2=0,((_tmp6E5->f3=((_tmp6E3="",_tag_dyneither(_tmp6E3,sizeof(
char),1))),((_tmp6E5->f4=(void*)((_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1[
0]=((_tmp6E2.tag=3,((_tmp6E2.f1=& Cyc_do_setjmp,_tmp6E2)))),_tmp6E1)))),((_tmp6E5->f5=((
_tmp6E0="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp6E0,sizeof(char),186))),_tmp6E5)))))))))))),((_tmp6D9[3]=((
_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB->f1=((_tmp6EA="-finish",
_tag_dyneither(_tmp6EA,sizeof(char),8))),((_tmp6EB->f2=0,((_tmp6EB->f3=((_tmp6E9="",
_tag_dyneither(_tmp6E9,sizeof(char),1))),((_tmp6EB->f4=(void*)((_tmp6E7=
_cycalloc(sizeof(*_tmp6E7)),((_tmp6E7[0]=((_tmp6E8.tag=0,((_tmp6E8.f1=Cyc_set_FINISH,
_tmp6E8)))),_tmp6E7)))),((_tmp6EB->f5=((_tmp6E6="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp6E6,sizeof(char),57))),_tmp6EB)))))))))))),((_tmp6D9[2]=((
_tmp6F1=_cycalloc(sizeof(*_tmp6F1)),((_tmp6F1->f1=((_tmp6F0="-gatherscript",
_tag_dyneither(_tmp6F0,sizeof(char),14))),((_tmp6F1->f2=0,((_tmp6F1->f3=((
_tmp6EF="",_tag_dyneither(_tmp6EF,sizeof(char),1))),((_tmp6F1->f4=(void*)((
_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED[0]=((_tmp6EE.tag=0,((_tmp6EE.f1=
Cyc_set_GATHERSCRIPT,_tmp6EE)))),_tmp6ED)))),((_tmp6F1->f5=((_tmp6EC="Produce a script to gather C library info",
_tag_dyneither(_tmp6EC,sizeof(char),42))),_tmp6F1)))))))))))),((_tmp6D9[1]=((
_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7->f1=((_tmp6F6="-gather",
_tag_dyneither(_tmp6F6,sizeof(char),8))),((_tmp6F7->f2=0,((_tmp6F7->f3=((_tmp6F5="",
_tag_dyneither(_tmp6F5,sizeof(char),1))),((_tmp6F7->f4=(void*)((_tmp6F3=
_cycalloc(sizeof(*_tmp6F3)),((_tmp6F3[0]=((_tmp6F4.tag=0,((_tmp6F4.f1=Cyc_set_GATHER,
_tmp6F4)))),_tmp6F3)))),((_tmp6F7->f5=((_tmp6F2="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp6F2,sizeof(char),56))),_tmp6F7)))))))))))),((_tmp6D9[0]=((
_tmp6FD=_cycalloc(sizeof(*_tmp6FD)),((_tmp6FD->f1=((_tmp6FC="-d",_tag_dyneither(
_tmp6FC,sizeof(char),3))),((_tmp6FD->f2=0,((_tmp6FD->f3=((_tmp6FB=" <file>",
_tag_dyneither(_tmp6FB,sizeof(char),8))),((_tmp6FD->f4=(void*)((_tmp6F9=
_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9[0]=((_tmp6FA.tag=5,((_tmp6FA.f1=Cyc_set_output_dir,
_tmp6FA)))),_tmp6F9)))),((_tmp6FD->f5=((_tmp6F8="Set the output directory to <file>",
_tag_dyneither(_tmp6F8,sizeof(char),35))),_tmp6FD)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6D9,sizeof(struct
_tuple23*),6)))))))))))));{const char*_tmp6FE;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp6FE="Options:",_tag_dyneither(_tmp6FE,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){{
const char*_tmp6FF;Cyc_Arg_usage(options,((_tmp6FF="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp6FF,sizeof(char),59))));}return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){{const char*_tmp702;void*_tmp701;(_tmp701=0,Cyc_fprintf(
Cyc_stderr,((_tmp702="Could not create file BUILDLIB.sh\n",_tag_dyneither(
_tmp702,sizeof(char),35))),_tag_dyneither(_tmp701,sizeof(void*),0)));}exit(1);}{
const char*_tmp705;void*_tmp704;(_tmp704=0,Cyc_prscript(((_tmp705="#!/bin/sh\n",
_tag_dyneither(_tmp705,sizeof(char),11))),_tag_dyneither(_tmp704,sizeof(void*),0)));}{
const char*_tmp708;void*_tmp707;(_tmp707=0,Cyc_prscript(((_tmp708="GCC=\"gcc\"\n",
_tag_dyneither(_tmp708,sizeof(char),11))),_tag_dyneither(_tmp707,sizeof(void*),0)));};}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp70C;void*_tmp70B[1];struct Cyc_String_pa_struct _tmp70A;(_tmp70A.tag=
0,((_tmp70A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp70B[0]=& _tmp70A,Cyc_fprintf(Cyc_stderr,((_tmp70C="Error: could not create directory %s\n",
_tag_dyneither(_tmp70C,sizeof(char),38))),_tag_dyneither(_tmp70B,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp710;void*_tmp70F[1];
struct Cyc_String_pa_struct _tmp70E;(_tmp70E.tag=0,((_tmp70E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp70F[0]=& _tmp70E,Cyc_prscript(((
_tmp710="cd %s\n",_tag_dyneither(_tmp710,sizeof(char),7))),_tag_dyneither(
_tmp70F,sizeof(void*),1)))))));}{const char*_tmp713;void*_tmp712;(_tmp712=0,Cyc_prscript(((
_tmp713="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp713,sizeof(
char),38))),_tag_dyneither(_tmp712,sizeof(void*),0)));};}if(!Cyc_gathering()){{
const char*_tmp714;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp714="BUILDLIB.LOG",_tag_dyneither(_tmp714,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp718;void*_tmp717[1];struct Cyc_String_pa_struct _tmp716;(_tmp716.tag=0,((
_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp717[0]=& _tmp716,Cyc_fprintf(Cyc_stderr,((_tmp718="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp718,sizeof(char),50))),_tag_dyneither(_tmp717,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp719;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp719="cstubs.c",
_tag_dyneither(_tmp719,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp71D;void*_tmp71C[1];struct Cyc_String_pa_struct
_tmp71B;(_tmp71B.tag=0,((_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp71C[0]=& _tmp71B,Cyc_fprintf(Cyc_stderr,((_tmp71D="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp71D,sizeof(char),50))),_tag_dyneither(_tmp71C,sizeof(void*),1)))))));}
return 1;}{const char*_tmp71E;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp71E="cycstubs.cyc",
_tag_dyneither(_tmp71E,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp722;void*_tmp721[1];struct Cyc_String_pa_struct
_tmp720;(_tmp720.tag=0,((_tmp720.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp721[0]=& _tmp720,Cyc_fprintf(Cyc_stderr,((_tmp722="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp722,sizeof(char),52))),_tag_dyneither(_tmp721,sizeof(void*),1)))))));}
return 1;}{const char*_tmp725;void*_tmp724;(_tmp724=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp725="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp725,sizeof(char),32))),_tag_dyneither(_tmp724,sizeof(void*),0)));};}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp728;void*_tmp727;(_tmp727=0,Cyc_fprintf(Cyc_stderr,((
_tmp728="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp728,sizeof(char),22))),
_tag_dyneither(_tmp727,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}return 0;};};}
