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
Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _dyneither_ptr s){Cyc_ccargs=({
struct Cyc_List_List*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->hd=({struct
_dyneither_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=s;_tmp1;});_tmp0->tl=Cyc_ccargs;
_tmp0;});}void Cyc_set_c_compiler(struct _dyneither_ptr s){if(Cyc_strcmp((struct
_dyneither_ptr)s,({const char*_tmp2="vc";_tag_dyneither(_tmp2,sizeof(char),3);}))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;Cyc_add_ccarg(({const char*_tmp3="-DVC_C";
_tag_dyneither(_tmp3,sizeof(char),7);}));}else{if(Cyc_strcmp((struct
_dyneither_ptr)s,({const char*_tmp4="gcc";_tag_dyneither(_tmp4,sizeof(char),4);}))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";
_tag_dyneither(_tmp5,sizeof(char),8);}));}}}void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=
1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct
_dyneither_ptr*Cyc_output_file=0;static void Cyc_set_output_file(struct
_dyneither_ptr s){Cyc_output_file=({struct _dyneither_ptr*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=s;_tmp6;});}extern char*Cdef_inc_path;extern char*Cdef_lib_path;
extern char*Carch;extern char*Ccomp;extern char*Cversion;static char _tmp7[1]="";
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};static void Cyc_set_cpp(
struct _dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static
void Cyc_add_cpparg(struct _dyneither_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=({struct _dyneither_ptr*_tmp9=_cycalloc(
sizeof(*_tmp9));_tmp9[0]=s;_tmp9;});_tmp8->tl=Cyc_cppargs;_tmp8;});}static struct
_dyneither_ptr Cyc_target_arch=(struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0
+ 0)};static void Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=
s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;
_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion));({
void*_tmpA[1]={& _tmpC};Cyc_printf(({const char*_tmpB="The Cyclone compiler, version %s\n";
_tag_dyneither(_tmpB,sizeof(char),34);}),_tag_dyneither(_tmpA,sizeof(void*),1));});});({
struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Ccomp));({struct Cyc_String_pa_struct _tmpF;
_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Carch));({void*_tmpD[2]={& _tmpF,& _tmp10};Cyc_printf(({const
char*_tmpE="Compiled for architecture: %s CC=\"%s\"\n";_tag_dyneither(_tmpE,
sizeof(char),39);}),_tag_dyneither(_tmpD,sizeof(void*),2));});});});({struct Cyc_String_pa_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Cdef_lib_path));({void*_tmp11[1]={& _tmp13};Cyc_printf(({const
char*_tmp12="Standard library directory: %s\n";_tag_dyneither(_tmp12,sizeof(char),
32);}),_tag_dyneither(_tmp11,sizeof(void*),1));});});({struct Cyc_String_pa_struct
_tmp16;_tmp16.tag=0;_tmp16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Cdef_inc_path));({void*_tmp14[1]={& _tmp16};Cyc_printf(({const
char*_tmp15="Standard include directory: %s\n";_tag_dyneither(_tmp15,sizeof(char),
32);}),_tag_dyneither(_tmp14,sizeof(void*),1));});});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp17=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp17 <= 4)return 0;else{return Cyc_strcmp((struct _dyneither_ptr)
_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),({const char*_tmp18=".cyc";
_tag_dyneither(_tmp18,sizeof(char),5);}))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){unsigned long _tmp19=
Cyc_strlen((struct _dyneither_ptr)s);if(_tmp19 <= 2)return;{struct _dyneither_ptr
_tmp1A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp19 - 2);
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B->hd=({struct _dyneither_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=
_tmp1A;_tmp1C;});_tmp1B->tl=Cyc_cyclone_exec_path;_tmp1B;});};}static struct Cyc_List_List*
Cyc_libargs=0;static void Cyc_add_libarg(struct _dyneither_ptr s){if(Cyc_strcmp((
struct _dyneither_ptr)s,({const char*_tmp1D="-lxml";_tag_dyneither(_tmp1D,sizeof(
char),6);}))== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1E=
_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=({struct _dyneither_ptr*_tmp1F=_cycalloc(
sizeof(*_tmp1F));_tmp1F[0]=s;_tmp1F;});_tmp1E->tl=Cyc_libargs;_tmp1E;});}}static
void Cyc_add_marg(struct _dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;Cyc_add_ccarg(({const char*_tmp20="-save-temps";_tag_dyneither(
_tmp20,sizeof(char),12);}));}static int Cyc_produce_dependencies=0;static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;Cyc_add_cpparg(({const char*
_tmp21="-M";_tag_dyneither(_tmp21,sizeof(char),3);}));}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s){Cyc_dependencies_target=({struct _dyneither_ptr*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=s;_tmp22;});}static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({const char*_tmp23="-c";
_tag_dyneither(_tmp23,sizeof(char),3);}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({
const char*_tmp24="-no-cpp-precomp";_tag_dyneither(_tmp24,sizeof(char),16);}));
Cyc_add_ccarg(({const char*_tmp25="-no-cpp-precomp";_tag_dyneither(_tmp25,sizeof(
char),16);}));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions(){Cyc_inline_functions_r=1;}static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;Cyc_set_inline_functions();}static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=
1;Cyc_add_ccarg(({const char*_tmp26="-DCYC_ANSI_OUTPUT";_tag_dyneither(_tmp26,
sizeof(char),18);}));Cyc_set_elim_se();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({
const char*_tmp27="-DNO_CYC_BOUNDS_CHECKS";_tag_dyneither(_tmp27,sizeof(char),23);}));}
static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({const char*_tmp28="-DNO_CYC_NULL_CHECKS";
_tag_dyneither(_tmp28,sizeof(char),21);}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp29="-DNO_CYC_PREFIX";_tag_dyneither(_tmp29,
sizeof(char),16);}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({const
char*_tmp2A="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp2A,sizeof(char),21);}));}
static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({const char*_tmp2B="-pg";
_tag_dyneither(_tmp2B,sizeof(char),4);}));}static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({const char*_tmp2C="-S";_tag_dyneither(
_tmp2C,sizeof(char),3);}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE
 = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(
struct _dyneither_ptr s){if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp2D="cyc";
_tag_dyneither(_tmp2D,sizeof(char),4);}))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp2E="none";_tag_dyneither(
_tmp2E,sizeof(char),5);}))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{({struct Cyc_String_pa_struct
_tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({
void*_tmp2F[1]={& _tmp31};Cyc_fprintf(Cyc_stderr,({const char*_tmp30="Input type '%s' not supported\n";
_tag_dyneither(_tmp30,sizeof(char),31);}),_tag_dyneither(_tmp2F,sizeof(void*),1));});});}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file){struct _dyneither_ptr _tmp32=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp33=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp34=Cyc_strlen((
struct _dyneither_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp32,(
struct _dyneither_ptr)_tmp33): _tmp33;return(struct _dyneither_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s){if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
Cyc_cyclone_files=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=({
struct _dyneither_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=s;_tmp36;});
_tmp35->tl=Cyc_cyclone_files;_tmp35;});{struct _dyneither_ptr _tmp37=Cyc_make_base_filename(
s,Cyc_output_file);struct _dyneither_ptr _tmp38=Cyc_strconcat((struct
_dyneither_ptr)_tmp37,({const char*_tmp39=".c";_tag_dyneither(_tmp39,sizeof(char),
3);}));Cyc_add_ccarg((struct _dyneither_ptr)_tmp38);};}else{Cyc_add_ccarg(s);}}
static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)return;
else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple18{
struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){({
struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file);({void*_tmp3B[1]={& _tmp3D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3C="Error opening spec file %s\n";_tag_dyneither(_tmp3C,sizeof(
char),28);}),_tag_dyneither(_tmp3B,sizeof(void*),1));});});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;if(c == '*')break;if(c != - 1){({struct Cyc_Int_pa_struct
_tmp41;_tmp41.tag=1;_tmp41.f1=(unsigned long)c;({struct Cyc_String_pa_struct
_tmp40;_tmp40.tag=0;_tmp40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({
void*_tmp3E[2]={& _tmp40,& _tmp41};Cyc_fprintf(Cyc_stderr,({const char*_tmp3F="Error reading spec file %s: unexpected character '%c'\n";
_tag_dyneither(_tmp3F,sizeof(char),55);}),_tag_dyneither(_tmp3E,sizeof(void*),2));});});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){({struct Cyc_String_pa_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp42[1]={& _tmp44};Cyc_fprintf(
Cyc_stderr,({const char*_tmp43="Error reading spec file %s: unexpected EOF\n";
_tag_dyneither(_tmp43,sizeof(char),44);}),_tag_dyneither(_tmp42,sizeof(void*),1));});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){({struct Cyc_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({
void*_tmp45[1]={& _tmp47};Cyc_fprintf(Cyc_stderr,({const char*_tmp46="Error reading spec file %s: string name too long\n";
_tag_dyneither(_tmp46,sizeof(char),50);}),_tag_dyneither(_tmp45,sizeof(void*),1));});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){
_tmp3A=({struct Cyc_List_List*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->hd=({
struct _tuple18*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->f1=({struct
_dyneither_ptr*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=(struct _dyneither_ptr)
Cyc_strdup(_tag_dyneither(strname,sizeof(char),256));_tmp4C;});_tmp49->f2=({
struct _dyneither_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=(struct
_dyneither_ptr)Cyc_strdup(({const char*_tmp4B="";_tag_dyneither(_tmp4B,sizeof(
char),1);}));_tmp4A;});_tmp49;});_tmp48->tl=_tmp3A;_tmp48;});goto JUST_AFTER_STAR;}
strvalue[0]=(char)c;i=1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(
4096,i)]='\000';break;}strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;if(i >= 4096){({struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(
struct _dyneither_ptr)_tag_dyneither(strname,sizeof(char),256);({struct Cyc_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({
void*_tmp4D[2]={& _tmp4F,& _tmp50};Cyc_fprintf(Cyc_stderr,({const char*_tmp4E="Error reading spec file %s: value of %s too long\n";
_tag_dyneither(_tmp4E,sizeof(char),50);}),_tag_dyneither(_tmp4D,sizeof(void*),2));});});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}_tmp3A=({
struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=({struct
_tuple18*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=({struct _dyneither_ptr*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256));_tmp54;});_tmp52->f2=({struct
_dyneither_ptr*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=(struct _dyneither_ptr)
Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096));_tmp53;});_tmp52;});
_tmp51->tl=_tmp3A;_tmp51;});if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN:
Cyc_fclose((struct Cyc___cycFILE*)spec_file);return _tmp3A;}struct _dyneither_ptr
Cyc_split_specs(struct _dyneither_ptr cmdline){if(cmdline.curr == (_tag_dyneither(0,
0,0)).curr)return _tag_dyneither(0,0,0);{unsigned long _tmp55=Cyc_strlen((struct
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
4096,j)]='\000';_tmp56=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));
_tmp57->hd=({struct _dyneither_ptr*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=(
struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096));_tmp58;});
_tmp57->tl=_tmp56;_tmp57;});}DONE: _tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp56);_tmp56=({struct Cyc_List_List*_tmp59=_cycalloc(
sizeof(*_tmp59));_tmp59->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1);_tmp59->tl=_tmp56;_tmp59;});{struct
_dyneither_ptr _tmp5B=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(
_tmp56);struct _dyneither_ptr _tmp5C=({unsigned int _tmp5D=_get_dyneither_size(
_tmp5B,sizeof(struct _dyneither_ptr*));struct _dyneither_ptr*_tmp5E=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp5D));
struct _dyneither_ptr _tmp60=_tag_dyneither(_tmp5E,sizeof(struct _dyneither_ptr),
_tmp5D);{unsigned int _tmp5F=_tmp5D;unsigned int k;for(k=0;k < _tmp5F;k ++){_tmp5E[k]=*((
struct _dyneither_ptr**)_tmp5B.curr)[(int)k];}}_tmp60;});return _tmp5C;};};}int Cyc_compile_failure=
0;struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct
_dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp61;
_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!
_tmp63){{struct Cyc___cycFILE*_tmp64=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp62=(void*)
_exn_thrown;void*_tmp66=_tmp62;_LL1:;_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct
_tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename);({struct Cyc_String_pa_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp67[2]={& _tmp69,&
_tmp6A};Cyc_fprintf(Cyc_stderr,({const char*_tmp68="\nError: couldn't open %s %s\n";
_tag_dyneither(_tmp68,sizeof(char),29);}),_tag_dyneither(_tmp67,sizeof(void*),2));});});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp66);_LL0:;}};}struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){struct _dyneither_ptr exn_string=({const char*_tmp8C="";
_tag_dyneither(_tmp8C,sizeof(char),1);});struct _dyneither_ptr explain_string=({
const char*_tmp8B="";_tag_dyneither(_tmp8B,sizeof(char),1);});int other_exn=0;void*
ex=(void*)({struct Cyc_Core_Impossible_struct*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88[0]=({struct Cyc_Core_Impossible_struct _tmp89;_tmp89.tag=Cyc_Core_Impossible;
_tmp89.f1=({const char*_tmp8A="";_tag_dyneither(_tmp8A,sizeof(char),1);});_tmp89;});
_tmp88;});struct Cyc_List_List*_tmp6B=0;{struct _handler_cons _tmp6C;_push_handler(&
_tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){_tmp6B=f(env,
tds);;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;void*_tmp70=_tmp6D;
struct _dyneither_ptr _tmp72;struct _dyneither_ptr _tmp75;struct _dyneither_ptr _tmp77;
_LL6: {struct Cyc_Core_Impossible_struct*_tmp71=(struct Cyc_Core_Impossible_struct*)
_tmp70;if(_tmp71->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp72=_tmp71->f1;}}
_LL7: exn_string=({const char*_tmp78="Exception Core::Impossible";_tag_dyneither(
_tmp78,sizeof(char),27);});explain_string=_tmp72;goto _LL5;_LL8: {struct Cyc_Dict_Absent_struct*
_tmp73=(struct Cyc_Dict_Absent_struct*)_tmp70;if(_tmp73->tag != Cyc_Dict_Absent)
goto _LLA;}_LL9: exn_string=({const char*_tmp79="Exception Dict::Absent";
_tag_dyneither(_tmp79,sizeof(char),23);});goto _LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*
_tmp74=(struct Cyc_Core_Invalid_argument_struct*)_tmp70;if(_tmp74->tag != Cyc_Core_Invalid_argument)
goto _LLC;else{_tmp75=_tmp74->f1;}}_LLB: exn_string=({const char*_tmp7A="Exception Core::Invalid_argument";
_tag_dyneither(_tmp7A,sizeof(char),33);});explain_string=_tmp75;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp76=(struct Cyc_Core_Failure_struct*)_tmp70;if(
_tmp76->tag != Cyc_Core_Failure)goto _LLE;else{_tmp77=_tmp76->f1;}}_LLD: exn_string=({
const char*_tmp7B="Exception Core::Failure";_tag_dyneither(_tmp7B,sizeof(char),24);});
explain_string=_tmp77;goto _LL5;_LLE:;_LLF: ex=_tmp70;other_exn=1;exn_string=({
const char*_tmp7C="Uncaught exception";_tag_dyneither(_tmp7C,sizeof(char),19);});
goto _LL5;_LL10:;_LL11:(void)_throw(_tmp70);_LL5:;}};}if(Cyc_Position_error_p())
Cyc_compile_failure=1;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,({const char*
_tmp7D="";_tag_dyneither(_tmp7D,sizeof(char),1);}))!= 0){Cyc_compile_failure=1;({
struct Cyc_String_pa_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)explain_string);({struct Cyc_String_pa_struct _tmp81;_tmp81.tag=
0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({struct Cyc_String_pa_struct
_tmp80;_tmp80.tag=0;_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
exn_string);({void*_tmp7E[3]={& _tmp80,& _tmp81,& _tmp82};Cyc_fprintf(Cyc_stderr,({
const char*_tmp7F="\n%s thrown during %s: %s";_tag_dyneither(_tmp7F,sizeof(char),
25);}),_tag_dyneither(_tmp7E,sizeof(void*),3));});});});});}if(Cyc_compile_failure){({
void*_tmp83=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp84="\nCOMPILATION FAILED!\n";
_tag_dyneither(_tmp84,sizeof(char),22);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({void*_tmp85[1]={&
_tmp87};Cyc_fprintf(Cyc_stderr,({const char*_tmp86="%s completed.\n";
_tag_dyneither(_tmp86,sizeof(char),15);}),_tag_dyneither(_tmp85,sizeof(void*),1));});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp8D=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);
return _tmp8D;};}struct _tuple19{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple19*tcenv,struct Cyc_List_List*
tds){struct _RegionHandle*_tmp8F;struct Cyc_Tcenv_Tenv*_tmp90;struct _tuple19 _tmp8E=*
tcenv;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;Cyc_Tc_tc(_tmp8F,_tmp90,1,tds);if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp90,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(
int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple20{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple20*params,struct Cyc_List_List*
tds){struct _tuple20 _tmp92;struct Cyc_Tcenv_Tenv*_tmp93;struct Cyc___cycFILE*_tmp94;
struct Cyc___cycFILE*_tmp95;struct _tuple20*_tmp91=params;_tmp92=*_tmp91;_tmp93=
_tmp92.f1;_tmp94=_tmp92.f2;_tmp95=_tmp92.f3;{struct Cyc_Interface_I*_tmp96=Cyc_Interface_extract(
_tmp93->ae);if(_tmp94 == 0)Cyc_Interface_save(_tmp96,_tmp95);else{struct Cyc_Interface_I*
_tmp97=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmp94);if(!Cyc_Interface_is_subinterface(
_tmp97,_tmp96,({struct _tuple17*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=({
const char*_tmp9A="written interface";_tag_dyneither(_tmp9A,sizeof(char),18);});
_tmp98->f2=({const char*_tmp99="maximal interface";_tag_dyneither(_tmp99,sizeof(
char),18);});_tmp98;})))Cyc_compile_failure=1;else{Cyc_Interface_save(_tmp97,
_tmp95);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*
tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*
tds){Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;return Cyc_Tovc_tovc(tds);}
static struct _dyneither_ptr Cyc_cyc_setjmp=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};static struct _dyneither_ptr Cyc_cyc_include=(struct _dyneither_ptr){(
void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_cyc_include(struct
_dyneither_ptr f){Cyc_cyc_include=f;}int Cyc_copy_internal_file(struct
_dyneither_ptr file,struct Cyc___cycFILE*out_file){if(file.curr == (_tag_dyneither(
0,0,0)).curr){({void*_tmp9B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp9C="Internal error: copy_internal_file called with NULL\n";
_tag_dyneither(_tmp9C,sizeof(char),53);}),_tag_dyneither(_tmp9B,sizeof(void*),0));});
return 1;}{struct Cyc___cycFILE*_tmp9D=Cyc_try_file_open(file,({const char*_tmpA5="r";
_tag_dyneither(_tmpA5,sizeof(char),2);}),({const char*_tmpA6="internal compiler file";
_tag_dyneither(_tmpA6,sizeof(char),23);}));if(_tmp9D == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpA4=1024;
unsigned int i;for(i=0;i < _tmpA4;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp9D);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp9D)){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp9E[1]={& _tmpA0};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9F="\nError: problem copying %s\n";_tag_dyneither(
_tmp9F,sizeof(char),28);}),_tag_dyneither(_tmp9E,sizeof(void*),1));});});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(_tag_dyneither(
buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpA1[1]={& _tmpA3};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA2="\nError: problem copying %s\n";_tag_dyneither(
_tmpA2,sizeof(char),28);}),_tag_dyneither(_tmpA1,sizeof(void*),1));});});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmp9D);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpA7=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpA7.expand_typedefs=!Cyc_noexpand_r;_tmpA7.to_VC=Cyc_Cyclone_tovc_r;
_tmpA7.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpA7.generate_line_directives=
Cyc_generate_line_directives_r;_tmpA7.print_full_evars=Cyc_print_full_evars_r;
_tmpA7.print_all_tvars=Cyc_print_all_tvars_r;_tmpA7.print_all_kinds=Cyc_print_all_kinds_r;
_tmpA7.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r)({
void*_tmpA8=0;Cyc_fprintf(out_file,({const char*_tmpA9="#define _INLINE_FUNCTIONS\n";
_tag_dyneither(_tmpA9,sizeof(char),27);}),_tag_dyneither(_tmpA8,sizeof(void*),0));});
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r)({
void*_tmpAA=0;Cyc_fprintf(out_file,({const char*_tmpAB="#include <setjmp.h>\n";
_tag_dyneither(_tmpAB,sizeof(char),21);}),_tag_dyneither(_tmpAA,sizeof(void*),0));});
else{if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}if(Cyc_copy_internal_file(
Cyc_cyc_include,out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpA7);
Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpA7);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dyneither_ptr s,char c){if(s.curr == (_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpAC=0;unsigned long _tmpAD=Cyc_strlen((struct
_dyneither_ptr)s);while(_tmpAD > 0){struct _dyneither_ptr _tmpAE=Cyc_strchr(s,c);
if(_tmpAE.curr == (_tag_dyneither(0,0,0)).curr){_tmpAC=({struct Cyc_List_List*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=({struct _dyneither_ptr*_tmpB0=
_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=s;_tmpB0;});_tmpAF->tl=_tmpAC;_tmpAF;});
break;}else{_tmpAC=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));
_tmpB1->hd=({struct _dyneither_ptr*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=(
struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((
struct _dyneither_ptr)_tmpAE).curr - s.curr)/ sizeof(char)));_tmpB2;});_tmpB1->tl=
_tmpAC;_tmpB1;});_tmpAD -=(((struct _dyneither_ptr)_tmpAE).curr - s.curr)/ sizeof(
char);s=_dyneither_ptr_plus(_tmpAE,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpAC);};}static int Cyc_file_exists(
struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpB3;
_push_handler(& _tmpB3);{int _tmpB5=0;if(setjmp(_tmpB3.handler))_tmpB5=1;if(!
_tmpB5){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmpB6="r";
_tag_dyneither(_tmpB6,sizeof(char),2);}));;_pop_handler();}else{void*_tmpB4=(
void*)_exn_thrown;void*_tmpB8=_tmpB4;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)
_throw(_tmpB8);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((struct Cyc___cycFILE*)
f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
struct _dyneither_ptr tmp=({const char*_tmpBB="";_tag_dyneither(_tmpBB,sizeof(char),
1);});for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpB9=*((struct
_dyneither_ptr*)dirs->hd);if(_tmpB9.curr == (_tag_dyneither(0,0,0)).curr  || Cyc_strlen((
struct _dyneither_ptr)_tmpB9)== 0)continue;_tmpB9=(struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)_tmpB9,({const char*_tmpBA=":";_tag_dyneither(_tmpBA,sizeof(
char),2);}));tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)
_tmpB9,(struct _dyneither_ptr)tmp);}return tmp;}static struct _dyneither_ptr*Cyc_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){if(file.curr == (
_tag_dyneither(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){struct
_dyneither_ptr _tmpBC=*((struct _dyneither_ptr*)dirs->hd);if(_tmpBC.curr == (
_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpBC)== 0)
continue;{struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpBC,
file);if(Cyc_file_exists(s))return({struct _dyneither_ptr*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=s;_tmpBD;});};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpBE=
Cyc_find(dirs,file);if(_tmpBE == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();({
struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_sprint_list(dirs));({struct Cyc_String_pa_struct _tmpC1;
_tmpC1.tag=0;_tmpC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*
_tmpBF[2]={& _tmpC1,& _tmpC2};Cyc_fprintf(Cyc_stderr,({const char*_tmpC0="Error: can't find internal compiler file %s in path %s\n";
_tag_dyneither(_tmpC0,sizeof(char),56);}),_tag_dyneither(_tmpBF,sizeof(void*),2));});});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Core_Failure_struct
_tmpC4;_tmpC4.tag=Cyc_Core_Failure;_tmpC4.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(
dirs));({struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmpC5[2]={& _tmpC7,& _tmpC8};
Cyc_aprintf(({const char*_tmpC6="Error: can't find internal compiler file %s in path %s\n";
_tag_dyneither(_tmpC6,sizeof(char),56);}),_tag_dyneither(_tmpC5,sizeof(void*),2));});});});
_tmpC4;});_tmpC3;}));}return*_tmpBE;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpC9=
0;for(0;dirs != 0;dirs=dirs->tl){_tmpC9=({struct Cyc_List_List*_tmpCA=_cycalloc(
sizeof(*_tmpCA));_tmpCA->hd=({struct _dyneither_ptr*_tmpCB=_cycalloc(sizeof(*
_tmpCB));_tmpCB[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir);_tmpCB;});_tmpCA->tl=_tmpC9;_tmpCA;});}_tmpC9=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC9);return
_tmpC9;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpCC=0;for(0;dirs != 0;dirs=
dirs->tl){_tmpCC=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(
struct _dyneither_ptr*)dirs->hd;_tmpCD->tl=_tmpCC;_tmpCD;});_tmpCC=({struct Cyc_List_List*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=({struct _dyneither_ptr*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir);_tmpCF;});_tmpCE->tl=_tmpCC;_tmpCE;});}
_tmpCC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCC);
return _tmpCC;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case ' ': _LL21:
goto _LL22;case '\n': _LL22: goto _LL23;case '\t': _LL23: return 1;default: _LL24: return 0;}}
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpD0=Cyc_strlen((struct _dyneither_ptr)s);int _tmpD1=0;int _tmpD2=0;{
int i=0;for(0;i < _tmpD0;++ i){char _tmpD3=*((const char*)_check_dyneither_subscript(
s,sizeof(char),i));if(_tmpD3 == '\'')++ _tmpD1;else{if(Cyc_is_other_special(_tmpD3))
++ _tmpD2;}}}if(_tmpD1 == 0  && _tmpD2 == 0)return s;if(_tmpD1 == 0)return(struct
_dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmpD4[3];_tmpD4[2]=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);
_tmpD4[1]=({struct _dyneither_ptr*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=(
struct _dyneither_ptr)s;_tmpD6;});_tmpD4[0]=_init_dyneither_ptr(_cycalloc(sizeof(
struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD4,sizeof(struct _dyneither_ptr*),
3));}));{unsigned long _tmpD8=(_tmpD0 + _tmpD1)+ _tmpD2;struct _dyneither_ptr s2=({
unsigned int _tmpE2=_tmpD8 + 1;char*_tmpE3=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmpE2 + 1));struct _dyneither_ptr _tmpE5=_tag_dyneither(_tmpE3,
sizeof(char),_tmpE2 + 1);{unsigned int _tmpE4=_tmpE2;unsigned int i;for(i=0;i < 
_tmpE4;i ++){_tmpE3[i]='\000';}_tmpE3[_tmpE4]=(char)0;}_tmpE5;});int _tmpD9=0;int
_tmpDA=0;for(0;_tmpD9 < _tmpD0;++ _tmpD9){char _tmpDB=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmpD9));if(_tmpDB == '\''  || Cyc_is_other_special(
_tmpDB))({struct _dyneither_ptr _tmpDC=_dyneither_ptr_plus(s2,sizeof(char),_tmpDA
++);char _tmpDD=*((char*)_check_dyneither_subscript(_tmpDC,sizeof(char),0));char
_tmpDE='\\';if(_get_dyneither_size(_tmpDC,sizeof(char))== 1  && (_tmpDD == '\000'
 && _tmpDE != '\000'))_throw_arraybounds();*((char*)_tmpDC.curr)=_tmpDE;});({
struct _dyneither_ptr _tmpDF=_dyneither_ptr_plus(s2,sizeof(char),_tmpDA ++);char
_tmpE0=*((char*)_check_dyneither_subscript(_tmpDF,sizeof(char),0));char _tmpE1=
_tmpDB;if(_get_dyneither_size(_tmpDF,sizeof(char))== 1  && (_tmpE0 == '\000'  && 
_tmpE1 != '\000'))_throw_arraybounds();*((char*)_tmpDF.curr)=_tmpE1;});}return(
struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){return({struct _dyneither_ptr*_tmpE6=_cycalloc(sizeof(*
_tmpE6));_tmpE6[0]=Cyc_sh_escape_string(*sp);_tmpE6;});}static void Cyc_process_file(
struct _dyneither_ptr filename){struct _dyneither_ptr _tmpE7=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dyneither_ptr _tmpE8=Cyc_strconcat((struct
_dyneither_ptr)_tmpE7,({const char*_tmp142=".cyp";_tag_dyneither(_tmp142,sizeof(
char),5);}));struct _dyneither_ptr _tmpE9=Cyc_strconcat((struct _dyneither_ptr)
_tmpE7,({const char*_tmp141=".cyci";_tag_dyneither(_tmp141,sizeof(char),6);}));
struct _dyneither_ptr _tmpEA=Cyc_strconcat((struct _dyneither_ptr)_tmpE7,({const
char*_tmp140=".cycio";_tag_dyneither(_tmp140,sizeof(char),7);}));struct
_dyneither_ptr _tmpEB=Cyc_strconcat((struct _dyneither_ptr)_tmpE7,({const char*
_tmp13F=".c";_tag_dyneither(_tmp13F,sizeof(char),3);}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename);({void*_tmpEC[1]={& _tmpEE};Cyc_fprintf(Cyc_stderr,({const char*_tmpED="Compiling %s\n";
_tag_dyneither(_tmpED,sizeof(char),14);}),_tag_dyneither(_tmpEC,sizeof(void*),1));});});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp13D="r";
_tag_dyneither(_tmp13D,sizeof(char),2);}),({const char*_tmp13E="input file";
_tag_dyneither(_tmp13E,sizeof(char),11);}));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct
_dyneither_ptr _tmpEF=Cyc_str_sepstr(({struct Cyc_List_List*_tmp139=_cycalloc(
sizeof(*_tmp139));_tmp139->hd=({struct _dyneither_ptr*_tmp13A=_cycalloc(sizeof(*
_tmp13A));_tmp13A[0]=(struct _dyneither_ptr)({const char*_tmp13B="";_tag_dyneither(
_tmp13B,sizeof(char),1);});_tmp13A;});_tmp139->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp139;}),({const char*_tmp13C=" ";_tag_dyneither(_tmp13C,sizeof(
char),2);}));struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)
Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpF0=Cyc_add_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);_tmpF0=Cyc_add_subdir(_tmpF0,({const char*_tmpF1="include";
_tag_dyneither(_tmpF1,sizeof(char),8);}));if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0)_tmpF0=({struct Cyc_List_List*_tmpF2=_cycalloc(sizeof(*_tmpF2));
_tmpF2->hd=({struct _dyneither_ptr*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=
def_inc_path;_tmpF3;});_tmpF2->tl=_tmpF0;_tmpF2;});{char*_tmpF4=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpF4 != 0)_tmpF0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpF5=_tmpF4;_tag_dyneither(_tmpF5,sizeof(char),_get_zero_arr_size_char(
_tmpF5,1));}),':'),_tmpF0);{struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*
_tmp135));_tmp135->hd=({struct _dyneither_ptr*_tmp136=_cycalloc(sizeof(*_tmp136));
_tmp136[0]=(struct _dyneither_ptr)({const char*_tmp137="";_tag_dyneither(_tmp137,
sizeof(char),1);});_tmp136;});_tmp135->tl=((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpF0);_tmp135;}),({const char*_tmp138=" -I";
_tag_dyneither(_tmp138,sizeof(char),4);}));struct _dyneither_ptr ofile_string;if(
Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)ofile_string=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file)));({
void*_tmpF6[1]={& _tmpF8};Cyc_aprintf(({const char*_tmpF7=" > %s";_tag_dyneither(
_tmpF7,sizeof(char),6);}),_tag_dyneither(_tmpF6,sizeof(void*),1));});});else{
ofile_string=({const char*_tmpF9="";_tag_dyneither(_tmpF9,sizeof(char),1);});}}
else{ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmpFC;
_tmpFC.tag=0;_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((
struct _dyneither_ptr)_tmpE8));({void*_tmpFA[1]={& _tmpFC};Cyc_aprintf(({const char*
_tmpFB=" > %s";_tag_dyneither(_tmpFB,sizeof(char),6);}),_tag_dyneither(_tmpFA,
sizeof(void*),1));});});}{struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0)fixup_string=({const char*_tmpFD=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_dyneither(_tmpFD,sizeof(char),35);});else{fixup_string=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp100;_tmp100.tag=0;_tmp100.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(
Cyc_dependencies_target)));({void*_tmpFE[1]={& _tmp100};Cyc_aprintf(({const char*
_tmpFF=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dyneither(_tmpFF,sizeof(char),29);}),
_tag_dyneither(_tmpFE,sizeof(void*),1));});});}}else{fixup_string=({const char*
_tmp101="";_tag_dyneither(_tmp101,sizeof(char),1);});}{struct _dyneither_ptr
_tmp102=({struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;_tmp134.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ofile_string);({struct Cyc_String_pa_struct
_tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
fixup_string);({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;_tmp132.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename));({
struct Cyc_String_pa_struct _tmp131;_tmp131.tag=0;_tmp131.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdinc_string);({struct Cyc_String_pa_struct _tmp130;_tmp130.tag=
0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEF);({struct Cyc_String_pa_struct
_tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_cpp);({void*_tmp12D[6]={& _tmp12F,& _tmp130,& _tmp131,& _tmp132,& _tmp133,&
_tmp134};Cyc_aprintf(({const char*_tmp12E="%s %s%s %s%s%s";_tag_dyneither(_tmp12E,
sizeof(char),15);}),_tag_dyneither(_tmp12D,sizeof(void*),6));});});});});});});});
if(Cyc_v_r)({struct Cyc_String_pa_struct _tmp105;_tmp105.tag=0;_tmp105.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp102);({void*_tmp103[1]={& _tmp105};Cyc_fprintf(
Cyc_stderr,({const char*_tmp104="%s\n";_tag_dyneither(_tmp104,sizeof(char),4);}),
_tag_dyneither(_tmp103,sizeof(void*),1));});});if(system((const char*)
_untag_dyneither_ptr(_tmp102,sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*
_tmp106=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp107="\nError: preprocessing\n";
_tag_dyneither(_tmp107,sizeof(char),23);}),_tag_dyneither(_tmp106,sizeof(void*),
0));});return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpE8);{struct Cyc___cycFILE*in_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE8,({const char*_tmp12B="r";_tag_dyneither(_tmp12B,
sizeof(char),2);}),({const char*_tmp12C="file";_tag_dyneither(_tmp12C,sizeof(char),
5);}));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct
_handler_cons _tmp108;_push_handler(& _tmp108);{int _tmp10A=0;if(setjmp(_tmp108.handler))
_tmp10A=1;if(!_tmp10A){tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr
stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),
struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmp10B="parsing";_tag_dyneither(_tmp10B,sizeof(char),8);}),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*_tmp109=(
void*)_exn_thrown;void*_tmp10D=_tmp109;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpE8);(int)_throw(
_tmp10D);_LL29:;_LL2A:(void)_throw(_tmp10D);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE8);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp10E=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp10E;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp10F=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple19 _tmp110=({struct _tuple19 _tmp118;
_tmp118.f1=tc_rgn;_tmp118.f2=_tmp10F;_tmp118;});tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple19*,struct
Cyc_List_List*),struct _tuple19*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp111="type checking";_tag_dyneither(_tmp111,sizeof(char),14);}),Cyc_do_typecheck,&
_tmp110,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpE8);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp112="control-flow checking";
_tag_dyneither(_tmp112,sizeof(char),22);}),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpE8);_npop_handler(0);return;}if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)
_untag_dyneither_ptr(_tmpE9,sizeof(char),1)),(const char*)"r");struct Cyc___cycFILE*
inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpEA,({const char*_tmp116="w";
_tag_dyneither(_tmp116,sizeof(char),2);}),({const char*_tmp117="interface object file";
_tag_dyneither(_tmp117,sizeof(char),22);}));if(inter_objfile == 0){Cyc_compile_failure=
1;_npop_handler(0);return;}Cyc_Position_reset_position((struct _dyneither_ptr)
_tmpE9);{struct _tuple20 _tmp113=({struct _tuple20 _tmp115;_tmp115.f1=_tmp10F;
_tmp115.f2=inter_file;_tmp115.f3=(struct Cyc___cycFILE*)inter_objfile;_tmp115;});
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(struct _tuple20*,struct Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp114="interface checking";_tag_dyneither(
_tmp114,sizeof(char),19);}),Cyc_do_interface,& _tmp113,tds);if(inter_file != 0)Cyc_file_close((
struct Cyc___cycFILE*)inter_file);Cyc_file_close((struct Cyc___cycFILE*)
inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct
_dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp119="translation to C";
_tag_dyneither(_tmp119,sizeof(char),17);}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpE8);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpE8);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r)tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp11A="post-pass to VC";
_tag_dyneither(_tmp11A,sizeof(char),16);}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure){
_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r
 || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp11B="w";
_tag_dyneither(_tmp11B,sizeof(char),2);}),({const char*_tmp11C="output file";
_tag_dyneither(_tmp11C,sizeof(char),12);}));else{out_file=(struct Cyc___cycFILE*)
Cyc_stdout;}}else{if(Cyc_toc_r  && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp11D="w";
_tag_dyneither(_tmp11D,sizeof(char),2);}),({const char*_tmp11E="output file";
_tag_dyneither(_tmp11E,sizeof(char),12);}));else{out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpEB,({const char*_tmp11F="w";_tag_dyneither(_tmp11F,
sizeof(char),2);}),({const char*_tmp120="output file";_tag_dyneither(_tmp120,
sizeof(char),12);}));}}if(Cyc_compile_failure  || !((unsigned int)out_file)){
_npop_handler(0);return;}{struct _handler_cons _tmp121;_push_handler(& _tmp121);{
int _tmp123=0;if(setjmp(_tmp121.handler))_tmp123=1;if(!_tmp123){if(!Cyc_noprint_r)
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp124="printing";_tag_dyneither(_tmp124,sizeof(
char),9);}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);;_pop_handler();}
else{void*_tmp122=(void*)_exn_thrown;void*_tmp126=_tmp122;_LL2C:;_LL2D: Cyc_compile_failure=
1;Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({struct Cyc_List_List*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->hd=({struct _dyneither_ptr*_tmp128=
_cycalloc(sizeof(*_tmp128));_tmp128[0]=(struct _dyneither_ptr)_tmpEB;_tmp128;});
_tmp127->tl=Cyc_cfiles;_tmp127;});(int)_throw(_tmp126);_LL2E:;_LL2F:(void)_throw(
_tmp126);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({
struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->hd=({struct
_dyneither_ptr*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=(struct
_dyneither_ptr)_tmpEB;_tmp12A;});_tmp129->tl=Cyc_cfiles;_tmp129;});};_pop_region(
tc_rgn);};};};};};};};};};}static char _tmp143[8]="<final>";static struct
_dyneither_ptr Cyc_final_str={_tmp143,_tmp143,_tmp143 + 8};static struct
_dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _dyneither_ptr basename;{struct _handler_cons _tmp144;_push_handler(& _tmp144);{
int _tmp146=0;if(setjmp(_tmp144.handler))_tmp146=1;if(!_tmp146){basename=(struct
_dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp145=(void*)_exn_thrown;void*_tmp148=_tmp145;_LL31: {struct Cyc_Core_Invalid_argument_struct*
_tmp149=(struct Cyc_Core_Invalid_argument_struct*)_tmp148;if(_tmp149->tag != Cyc_Core_Invalid_argument)
goto _LL33;}_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmp148);_LL30:;}};}{
struct _dyneither_ptr _tmp14A=Cyc_strconcat((struct _dyneither_ptr)basename,({const
char*_tmp14F=".cycio";_tag_dyneither(_tmp14F,sizeof(char),7);}));struct Cyc___cycFILE*
_tmp14B=Cyc_try_file_open((struct _dyneither_ptr)_tmp14A,({const char*_tmp14D="rb";
_tag_dyneither(_tmp14D,sizeof(char),3);}),({const char*_tmp14E="interface object file";
_tag_dyneither(_tmp14E,sizeof(char),22);}));if(_tmp14B == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((struct _dyneither_ptr)
_tmp14A);{struct Cyc_Interface_I*_tmp14C=Cyc_Interface_load((struct Cyc___cycFILE*)
_tmp14B);Cyc_file_close((struct Cyc___cycFILE*)_tmp14B);return _tmp14C;};};};}
static int Cyc_is_cfile(struct _dyneither_ptr*n){return*((const char*)
_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}void
GC_blacklist_warn_clear();struct _tuple21{struct _dyneither_ptr f1;int f2;struct
_dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp150=
_new_region("r");struct _RegionHandle*r=& _tmp150;_push_region(r);{struct
_dyneither_ptr _tmp151=({unsigned int _tmp304=(unsigned int)argc;int*_tmp305=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp304));struct _dyneither_ptr _tmp307=
_tag_dyneither(_tmp305,sizeof(int),_tmp304);{unsigned int _tmp306=_tmp304;
unsigned int i;for(i=0;i < _tmp306;i ++){_tmp305[i]=0;}}_tmp307;});int _tmp152=0;int
i;int j;int k;for(i=1;i < argc;++ i){if(Cyc_strncmp(({const char*_tmp153="-B";
_tag_dyneither(_tmp153,sizeof(char),3);}),(struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),
2)== 0){*((int*)_check_dyneither_subscript(_tmp151,sizeof(int),i))=1;++ _tmp152;}
else{if(Cyc_strcmp(({const char*_tmp154="-b";_tag_dyneither(_tmp154,sizeof(char),
3);}),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(
argv,sizeof(struct _dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(
_tmp151,sizeof(int),i))=1;++ _tmp152;++ i;if(i >= argc)break;*((int*)
_check_dyneither_subscript(_tmp151,sizeof(int),i))=1;++ _tmp152;}}}{struct
_dyneither_ptr _tmp155=({unsigned int _tmp300=(unsigned int)(_tmp152 + 1);struct
_dyneither_ptr*_tmp301=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(
struct _dyneither_ptr),_tmp300));struct _dyneither_ptr _tmp303=_tag_dyneither(
_tmp301,sizeof(struct _dyneither_ptr),_tmp300);{unsigned int _tmp302=_tmp300;
unsigned int n;for(n=0;n < _tmp302;n ++){_tmp301[n]=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);}}_tmp303;});struct _dyneither_ptr _tmp156=({unsigned int
_tmp2FC=(unsigned int)(argc - _tmp152);struct _dyneither_ptr*_tmp2FD=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp2FC));
struct _dyneither_ptr _tmp2FF=_tag_dyneither(_tmp2FD,sizeof(struct _dyneither_ptr),
_tmp2FC);{unsigned int _tmp2FE=_tmp2FC;unsigned int n;for(n=0;n < _tmp2FE;n ++){
_tmp2FD[n]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}_tmp2FF;});*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp155,sizeof(struct _dyneither_ptr),
0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp156,sizeof(struct
_dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),0)));for(i=(j=(k=1));i < argc;++ i){if(*((int*)
_check_dyneither_subscript(_tmp151,sizeof(int),i)))*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp155,sizeof(struct _dyneither_ptr),j ++))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));
else{*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp156,sizeof(struct
_dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i));}}{struct Cyc_List_List*options=({struct _tuple21*
_tmp175[65];_tmp175[64]=({struct _tuple21*_tmp2F6=_region_malloc(r,sizeof(*
_tmp2F6));_tmp2F6->f1=({const char*_tmp2FB="-noregions";_tag_dyneither(_tmp2FB,
sizeof(char),11);});_tmp2F6->f2=0;_tmp2F6->f3=({const char*_tmp2FA="";
_tag_dyneither(_tmp2FA,sizeof(char),1);});_tmp2F6->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2F8=_region_malloc(r,sizeof(*_tmp2F8));_tmp2F8[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2F9;_tmp2F9.tag=3;_tmp2F9.f1=& Cyc_Absyn_no_regions;_tmp2F9;});_tmp2F8;});
_tmp2F6->f5=({const char*_tmp2F7="Generate code that doesn't use regions";
_tag_dyneither(_tmp2F7,sizeof(char),39);});_tmp2F6;});_tmp175[63]=({struct
_tuple21*_tmp2F0=_region_malloc(r,sizeof(*_tmp2F0));_tmp2F0->f1=({const char*
_tmp2F5="-port";_tag_dyneither(_tmp2F5,sizeof(char),6);});_tmp2F0->f2=0;_tmp2F0->f3=({
const char*_tmp2F4="";_tag_dyneither(_tmp2F4,sizeof(char),1);});_tmp2F0->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp2F2=_region_malloc(r,sizeof(*_tmp2F2));
_tmp2F2[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=Cyc_set_port_c_code;
_tmp2F3;});_tmp2F2;});_tmp2F0->f5=({const char*_tmp2F1="Suggest how to port C code to Cyclone";
_tag_dyneither(_tmp2F1,sizeof(char),38);});_tmp2F0;});_tmp175[62]=({struct
_tuple21*_tmp2EA=_region_malloc(r,sizeof(*_tmp2EA));_tmp2EA->f1=({const char*
_tmp2EF="-detailedlocation";_tag_dyneither(_tmp2EF,sizeof(char),18);});_tmp2EA->f2=
0;_tmp2EA->f3=({const char*_tmp2EE="";_tag_dyneither(_tmp2EE,sizeof(char),1);});
_tmp2EA->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*_tmp2EC=_region_malloc(r,
sizeof(*_tmp2EC));_tmp2EC[0]=({struct Cyc_Arg_Clear_spec_struct _tmp2ED;_tmp2ED.tag=
4;_tmp2ED.f1=& Cyc_Position_use_gcc_style_location;_tmp2ED;});_tmp2EC;});_tmp2EA->f5=({
const char*_tmp2EB="Try to give more detailed location information for errors";
_tag_dyneither(_tmp2EB,sizeof(char),58);});_tmp2EA;});_tmp175[61]=({struct
_tuple21*_tmp2E4=_region_malloc(r,sizeof(*_tmp2E4));_tmp2E4->f1=({const char*
_tmp2E9="-noregister";_tag_dyneither(_tmp2E9,sizeof(char),12);});_tmp2E4->f2=0;
_tmp2E4->f3=({const char*_tmp2E8="";_tag_dyneither(_tmp2E8,sizeof(char),1);});
_tmp2E4->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2E6=_region_malloc(r,
sizeof(*_tmp2E6));_tmp2E6[0]=({struct Cyc_Arg_Set_spec_struct _tmp2E7;_tmp2E7.tag=
3;_tmp2E7.f1=& Cyc_Parse_no_register;_tmp2E7;});_tmp2E6;});_tmp2E4->f5=({const
char*_tmp2E5="Treat register storage class as public";_tag_dyneither(_tmp2E5,
sizeof(char),39);});_tmp2E4;});_tmp175[60]=({struct _tuple21*_tmp2DE=
_region_malloc(r,sizeof(*_tmp2DE));_tmp2DE->f1=({const char*_tmp2E3="-warnnullchecks";
_tag_dyneither(_tmp2E3,sizeof(char),16);});_tmp2DE->f2=0;_tmp2DE->f3=({const char*
_tmp2E2="";_tag_dyneither(_tmp2E2,sizeof(char),1);});_tmp2DE->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2E0=_region_malloc(r,sizeof(*_tmp2E0));_tmp2E0[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2E1;_tmp2E1.tag=3;_tmp2E1.f1=& Cyc_Toc_warn_all_null_deref;
_tmp2E1;});_tmp2E0;});_tmp2DE->f5=({const char*_tmp2DF="Warn when any null check can't be eliminated";
_tag_dyneither(_tmp2DF,sizeof(char),45);});_tmp2DE;});_tmp175[59]=({struct
_tuple21*_tmp2D8=_region_malloc(r,sizeof(*_tmp2D8));_tmp2D8->f1=({const char*
_tmp2DD="-warnboundschecks";_tag_dyneither(_tmp2DD,sizeof(char),18);});_tmp2D8->f2=
0;_tmp2D8->f3=({const char*_tmp2DC="";_tag_dyneither(_tmp2DC,sizeof(char),1);});
_tmp2D8->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2DA=_region_malloc(r,
sizeof(*_tmp2DA));_tmp2DA[0]=({struct Cyc_Arg_Set_spec_struct _tmp2DB;_tmp2DB.tag=
3;_tmp2DB.f1=& Cyc_Toc_warn_bounds_checks;_tmp2DB;});_tmp2DA;});_tmp2D8->f5=({
const char*_tmp2D9="Warn when bounds checks can't be eliminated";_tag_dyneither(
_tmp2D9,sizeof(char),44);});_tmp2D8;});_tmp175[58]=({struct _tuple21*_tmp2D2=
_region_malloc(r,sizeof(*_tmp2D2));_tmp2D2->f1=({const char*_tmp2D7="-CI";
_tag_dyneither(_tmp2D7,sizeof(char),4);});_tmp2D2->f2=0;_tmp2D2->f3=({const char*
_tmp2D6=" <file>";_tag_dyneither(_tmp2D6,sizeof(char),8);});_tmp2D2->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp2D4=_region_malloc(r,sizeof(*_tmp2D4));
_tmp2D4[0]=({struct Cyc_Arg_String_spec_struct _tmp2D5;_tmp2D5.tag=5;_tmp2D5.f1=
Cyc_set_cyc_include;_tmp2D5;});_tmp2D4;});_tmp2D2->f5=({const char*_tmp2D3="Set cyc_include.h to be <file>";
_tag_dyneither(_tmp2D3,sizeof(char),31);});_tmp2D2;});_tmp175[57]=({struct
_tuple21*_tmp2CC=_region_malloc(r,sizeof(*_tmp2CC));_tmp2CC->f1=({const char*
_tmp2D1="-compile-for-boot";_tag_dyneither(_tmp2D1,sizeof(char),18);});_tmp2CC->f2=
0;_tmp2CC->f3=({const char*_tmp2D0="";_tag_dyneither(_tmp2D0,sizeof(char),1);});
_tmp2CC->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2CE=_region_malloc(r,
sizeof(*_tmp2CE));_tmp2CE[0]=({struct Cyc_Arg_Set_spec_struct _tmp2CF;_tmp2CF.tag=
3;_tmp2CF.f1=& Cyc_compile_for_boot_r;_tmp2CF;});_tmp2CE;});_tmp2CC->f5=({const
char*_tmp2CD="Compile using the special boot library instead of the standard library";
_tag_dyneither(_tmp2CD,sizeof(char),71);});_tmp2CC;});_tmp175[56]=({struct
_tuple21*_tmp2C6=_region_malloc(r,sizeof(*_tmp2C6));_tmp2C6->f1=({const char*
_tmp2CB="-nocyc_setjmp";_tag_dyneither(_tmp2CB,sizeof(char),14);});_tmp2C6->f2=0;
_tmp2C6->f3=({const char*_tmp2CA="";_tag_dyneither(_tmp2CA,sizeof(char),1);});
_tmp2C6->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2C8=_region_malloc(r,
sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Arg_Set_spec_struct _tmp2C9;_tmp2C9.tag=
3;_tmp2C9.f1=& Cyc_nocyc_setjmp_r;_tmp2C9;});_tmp2C8;});_tmp2C6->f5=({const char*
_tmp2C7="Do not use compiler special file cyc_setjmp.h";_tag_dyneither(_tmp2C7,
sizeof(char),46);});_tmp2C6;});_tmp175[55]=({struct _tuple21*_tmp2C0=
_region_malloc(r,sizeof(*_tmp2C0));_tmp2C0->f1=({const char*_tmp2C5="-printalleffects";
_tag_dyneither(_tmp2C5,sizeof(char),17);});_tmp2C0->f2=0;_tmp2C0->f3=({const char*
_tmp2C4="";_tag_dyneither(_tmp2C4,sizeof(char),1);});_tmp2C0->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2C2=_region_malloc(r,sizeof(*_tmp2C2));_tmp2C2[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2C3;_tmp2C3.tag=3;_tmp2C3.f1=& Cyc_print_all_effects_r;
_tmp2C3;});_tmp2C2;});_tmp2C0->f5=({const char*_tmp2C1="Print effects for functions (type debugging)";
_tag_dyneither(_tmp2C1,sizeof(char),45);});_tmp2C0;});_tmp175[54]=({struct
_tuple21*_tmp2BA=_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA->f1=({const char*
_tmp2BF="-printfullevars";_tag_dyneither(_tmp2BF,sizeof(char),16);});_tmp2BA->f2=
0;_tmp2BA->f3=({const char*_tmp2BE="";_tag_dyneither(_tmp2BE,sizeof(char),1);});
_tmp2BA->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2BC=_region_malloc(r,
sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_Arg_Set_spec_struct _tmp2BD;_tmp2BD.tag=
3;_tmp2BD.f1=& Cyc_print_full_evars_r;_tmp2BD;});_tmp2BC;});_tmp2BA->f5=({const
char*_tmp2BB="Print full information for evars (type debugging)";_tag_dyneither(
_tmp2BB,sizeof(char),50);});_tmp2BA;});_tmp175[53]=({struct _tuple21*_tmp2B4=
_region_malloc(r,sizeof(*_tmp2B4));_tmp2B4->f1=({const char*_tmp2B9="-printallkinds";
_tag_dyneither(_tmp2B9,sizeof(char),15);});_tmp2B4->f2=0;_tmp2B4->f3=({const char*
_tmp2B8="";_tag_dyneither(_tmp2B8,sizeof(char),1);});_tmp2B4->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2B6=_region_malloc(r,sizeof(*_tmp2B6));_tmp2B6[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2B7;_tmp2B7.tag=3;_tmp2B7.f1=& Cyc_print_all_kinds_r;
_tmp2B7;});_tmp2B6;});_tmp2B4->f5=({const char*_tmp2B5="Always print kinds of type variables";
_tag_dyneither(_tmp2B5,sizeof(char),37);});_tmp2B4;});_tmp175[52]=({struct
_tuple21*_tmp2AE=_region_malloc(r,sizeof(*_tmp2AE));_tmp2AE->f1=({const char*
_tmp2B3="-printalltvars";_tag_dyneither(_tmp2B3,sizeof(char),15);});_tmp2AE->f2=
0;_tmp2AE->f3=({const char*_tmp2B2="";_tag_dyneither(_tmp2B2,sizeof(char),1);});
_tmp2AE->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B0=_region_malloc(r,
sizeof(*_tmp2B0));_tmp2B0[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B1;_tmp2B1.tag=
3;_tmp2B1.f1=& Cyc_print_all_tvars_r;_tmp2B1;});_tmp2B0;});_tmp2AE->f5=({const
char*_tmp2AF="Print all type variables (even implicit default effects)";
_tag_dyneither(_tmp2AF,sizeof(char),57);});_tmp2AE;});_tmp175[51]=({struct
_tuple21*_tmp2A8=_region_malloc(r,sizeof(*_tmp2A8));_tmp2A8->f1=({const char*
_tmp2AD="-noexpandtypedefs";_tag_dyneither(_tmp2AD,sizeof(char),18);});_tmp2A8->f2=
0;_tmp2A8->f3=({const char*_tmp2AC="";_tag_dyneither(_tmp2AC,sizeof(char),1);});
_tmp2A8->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2AA=_region_malloc(r,
sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Arg_Set_spec_struct _tmp2AB;_tmp2AB.tag=
3;_tmp2AB.f1=& Cyc_noexpand_r;_tmp2AB;});_tmp2AA;});_tmp2A8->f5=({const char*
_tmp2A9="Don't expand typedefs in pretty printing";_tag_dyneither(_tmp2A9,
sizeof(char),41);});_tmp2A8;});_tmp175[50]=({struct _tuple21*_tmp2A2=
_region_malloc(r,sizeof(*_tmp2A2));_tmp2A2->f1=({const char*_tmp2A7="-noremoveunused";
_tag_dyneither(_tmp2A7,sizeof(char),16);});_tmp2A2->f2=0;_tmp2A2->f3=({const char*
_tmp2A6="";_tag_dyneither(_tmp2A6,sizeof(char),1);});_tmp2A2->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2A4=_region_malloc(r,sizeof(*_tmp2A4));_tmp2A4[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2A5;_tmp2A5.tag=3;_tmp2A5.f1=& Cyc_noshake_r;
_tmp2A5;});_tmp2A4;});_tmp2A2->f5=({const char*_tmp2A3="Don't remove externed variables that aren't used";
_tag_dyneither(_tmp2A3,sizeof(char),49);});_tmp2A2;});_tmp175[49]=({struct
_tuple21*_tmp29C=_region_malloc(r,sizeof(*_tmp29C));_tmp29C->f1=({const char*
_tmp2A1="-nogc";_tag_dyneither(_tmp2A1,sizeof(char),6);});_tmp29C->f2=0;_tmp29C->f3=({
const char*_tmp2A0="";_tag_dyneither(_tmp2A0,sizeof(char),1);});_tmp29C->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp29E=_region_malloc(r,sizeof(*_tmp29E));_tmp29E[
0]=({struct Cyc_Arg_Set_spec_struct _tmp29F;_tmp29F.tag=3;_tmp29F.f1=& Cyc_nogc_r;
_tmp29F;});_tmp29E;});_tmp29C->f5=({const char*_tmp29D="Don't link in the garbage collector";
_tag_dyneither(_tmp29D,sizeof(char),36);});_tmp29C;});_tmp175[48]=({struct
_tuple21*_tmp296=_region_malloc(r,sizeof(*_tmp296));_tmp296->f1=({const char*
_tmp29B="-nocyc";_tag_dyneither(_tmp29B,sizeof(char),7);});_tmp296->f2=0;_tmp296->f3=({
const char*_tmp29A="";_tag_dyneither(_tmp29A,sizeof(char),1);});_tmp296->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp298=_region_malloc(r,sizeof(*_tmp298));
_tmp298[0]=({struct Cyc_Arg_Unit_spec_struct _tmp299;_tmp299.tag=0;_tmp299.f1=Cyc_set_nocyc;
_tmp299;});_tmp298;});_tmp296->f5=({const char*_tmp297="Don't add implicit namespace Cyc";
_tag_dyneither(_tmp297,sizeof(char),33);});_tmp296;});_tmp175[47]=({struct
_tuple21*_tmp290=_region_malloc(r,sizeof(*_tmp290));_tmp290->f1=({const char*
_tmp295="-no-cpp-precomp";_tag_dyneither(_tmp295,sizeof(char),16);});_tmp290->f2=
0;_tmp290->f3=({const char*_tmp294="";_tag_dyneither(_tmp294,sizeof(char),1);});
_tmp290->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp292=_region_malloc(r,
sizeof(*_tmp292));_tmp292[0]=({struct Cyc_Arg_Unit_spec_struct _tmp293;_tmp293.tag=
0;_tmp293.f1=Cyc_set_nocppprecomp;_tmp293;});_tmp292;});_tmp290->f5=({const char*
_tmp291="Don't do smart preprocessing (mac only)";_tag_dyneither(_tmp291,sizeof(
char),40);});_tmp290;});_tmp175[46]=({struct _tuple21*_tmp28A=_region_malloc(r,
sizeof(*_tmp28A));_tmp28A->f1=({const char*_tmp28F="-use-cpp";_tag_dyneither(
_tmp28F,sizeof(char),9);});_tmp28A->f2=0;_tmp28A->f3=({const char*_tmp28E="<path>";
_tag_dyneither(_tmp28E,sizeof(char),7);});_tmp28A->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp28C=_region_malloc(r,sizeof(*_tmp28C));_tmp28C[0]=({struct Cyc_Arg_String_spec_struct
_tmp28D;_tmp28D.tag=5;_tmp28D.f1=Cyc_set_cpp;_tmp28D;});_tmp28C;});_tmp28A->f5=({
const char*_tmp28B="Indicate which preprocessor to use";_tag_dyneither(_tmp28B,
sizeof(char),35);});_tmp28A;});_tmp175[45]=({struct _tuple21*_tmp284=
_region_malloc(r,sizeof(*_tmp284));_tmp284->f1=({const char*_tmp289="--inline-checks";
_tag_dyneither(_tmp289,sizeof(char),16);});_tmp284->f2=0;_tmp284->f3=({const char*
_tmp288="";_tag_dyneither(_tmp288,sizeof(char),1);});_tmp284->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp286=_region_malloc(r,sizeof(*_tmp286));_tmp286[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp287;_tmp287.tag=0;_tmp287.f1=Cyc_set_inline_functions;
_tmp287;});_tmp286;});_tmp284->f5=({const char*_tmp285="Inline bounds checks instead of #define";
_tag_dyneither(_tmp285,sizeof(char),40);});_tmp284;});_tmp175[44]=({struct
_tuple21*_tmp27E=_region_malloc(r,sizeof(*_tmp27E));_tmp27E->f1=({const char*
_tmp283="--noboundschecks";_tag_dyneither(_tmp283,sizeof(char),17);});_tmp27E->f2=
0;_tmp27E->f3=({const char*_tmp282="";_tag_dyneither(_tmp282,sizeof(char),1);});
_tmp27E->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp280=_region_malloc(r,
sizeof(*_tmp280));_tmp280[0]=({struct Cyc_Arg_Unit_spec_struct _tmp281;_tmp281.tag=
0;_tmp281.f1=Cyc_set_noboundschecks;_tmp281;});_tmp280;});_tmp27E->f5=({const
char*_tmp27F="Disable bounds checks";_tag_dyneither(_tmp27F,sizeof(char),22);});
_tmp27E;});_tmp175[43]=({struct _tuple21*_tmp278=_region_malloc(r,sizeof(*_tmp278));
_tmp278->f1=({const char*_tmp27D="--nonullchecks";_tag_dyneither(_tmp27D,sizeof(
char),15);});_tmp278->f2=0;_tmp278->f3=({const char*_tmp27C="";_tag_dyneither(
_tmp27C,sizeof(char),1);});_tmp278->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp27A=_region_malloc(r,sizeof(*_tmp27A));_tmp27A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp27B;_tmp27B.tag=0;_tmp27B.f1=Cyc_set_nonullchecks;_tmp27B;});_tmp27A;});
_tmp278->f5=({const char*_tmp279="Disable null checks";_tag_dyneither(_tmp279,
sizeof(char),20);});_tmp278;});_tmp175[42]=({struct _tuple21*_tmp272=
_region_malloc(r,sizeof(*_tmp272));_tmp272->f1=({const char*_tmp277="--nochecks";
_tag_dyneither(_tmp277,sizeof(char),11);});_tmp272->f2=0;_tmp272->f3=({const char*
_tmp276="";_tag_dyneither(_tmp276,sizeof(char),1);});_tmp272->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp274=_region_malloc(r,sizeof(*_tmp274));_tmp274[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp275;_tmp275.tag=0;_tmp275.f1=Cyc_set_nochecks;
_tmp275;});_tmp274;});_tmp272->f5=({const char*_tmp273="Disable bounds/null checks";
_tag_dyneither(_tmp273,sizeof(char),27);});_tmp272;});_tmp175[41]=({struct
_tuple21*_tmp26C=_region_malloc(r,sizeof(*_tmp26C));_tmp26C->f1=({const char*
_tmp271="--lineno";_tag_dyneither(_tmp271,sizeof(char),9);});_tmp26C->f2=0;
_tmp26C->f3=({const char*_tmp270="";_tag_dyneither(_tmp270,sizeof(char),1);});
_tmp26C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp26E=_region_malloc(r,
sizeof(*_tmp26E));_tmp26E[0]=({struct Cyc_Arg_Unit_spec_struct _tmp26F;_tmp26F.tag=
0;_tmp26F.f1=Cyc_set_lineno;_tmp26F;});_tmp26E;});_tmp26C->f5=({const char*
_tmp26D="Generate line numbers for debugging";_tag_dyneither(_tmp26D,sizeof(char),
36);});_tmp26C;});_tmp175[40]=({struct _tuple21*_tmp266=_region_malloc(r,sizeof(*
_tmp266));_tmp266->f1=({const char*_tmp26B="-save-c";_tag_dyneither(_tmp26B,
sizeof(char),8);});_tmp266->f2=0;_tmp266->f3=({const char*_tmp26A="";
_tag_dyneither(_tmp26A,sizeof(char),1);});_tmp266->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp268=_region_malloc(r,sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Arg_Set_spec_struct
_tmp269;_tmp269.tag=3;_tmp269.f1=& Cyc_save_c_r;_tmp269;});_tmp268;});_tmp266->f5=({
const char*_tmp267="Don't delete temporary C files";_tag_dyneither(_tmp267,
sizeof(char),31);});_tmp266;});_tmp175[39]=({struct _tuple21*_tmp260=
_region_malloc(r,sizeof(*_tmp260));_tmp260->f1=({const char*_tmp265="-save-temps";
_tag_dyneither(_tmp265,sizeof(char),12);});_tmp260->f2=0;_tmp260->f3=({const char*
_tmp264="";_tag_dyneither(_tmp264,sizeof(char),1);});_tmp260->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp262=_region_malloc(r,sizeof(*_tmp262));_tmp262[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp263;_tmp263.tag=0;_tmp263.f1=Cyc_set_save_temps;
_tmp263;});_tmp262;});_tmp260->f5=({const char*_tmp261="Don't delete temporary files";
_tag_dyneither(_tmp261,sizeof(char),29);});_tmp260;});_tmp175[38]=({struct
_tuple21*_tmp25A=_region_malloc(r,sizeof(*_tmp25A));_tmp25A->f1=({const char*
_tmp25F="-c-comp";_tag_dyneither(_tmp25F,sizeof(char),8);});_tmp25A->f2=0;
_tmp25A->f3=({const char*_tmp25E=" <compiler>";_tag_dyneither(_tmp25E,sizeof(char),
12);});_tmp25A->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp25C=
_region_malloc(r,sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Arg_String_spec_struct
_tmp25D;_tmp25D.tag=5;_tmp25D.f1=Cyc_set_c_compiler;_tmp25D;});_tmp25C;});
_tmp25A->f5=({const char*_tmp25B="Produce C output for the given compiler";
_tag_dyneither(_tmp25B,sizeof(char),40);});_tmp25A;});_tmp175[37]=({struct
_tuple21*_tmp254=_region_malloc(r,sizeof(*_tmp254));_tmp254->f1=({const char*
_tmp259="-un-gcc";_tag_dyneither(_tmp259,sizeof(char),8);});_tmp254->f2=0;
_tmp254->f3=({const char*_tmp258="";_tag_dyneither(_tmp258,sizeof(char),1);});
_tmp254->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp256=_region_malloc(r,
sizeof(*_tmp256));_tmp256[0]=({struct Cyc_Arg_Unit_spec_struct _tmp257;_tmp257.tag=
0;_tmp257.f1=Cyc_set_tovc;_tmp257;});_tmp256;});_tmp254->f5=({const char*_tmp255="Avoid gcc extensions in C output (except for attributes)";
_tag_dyneither(_tmp255,sizeof(char),57);});_tmp254;});_tmp175[36]=({struct
_tuple21*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=({const char*
_tmp253="-elim-statement-expressions";_tag_dyneither(_tmp253,sizeof(char),28);});
_tmp24E->f2=0;_tmp24E->f3=({const char*_tmp252="";_tag_dyneither(_tmp252,sizeof(
char),1);});_tmp24E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp250=
_region_malloc(r,sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Arg_Set_spec_struct
_tmp251;_tmp251.tag=3;_tmp251.f1=& Cyc_elim_se_r;_tmp251;});_tmp250;});_tmp24E->f5=({
const char*_tmp24F="Avoid statement expressions in C output (implies --inline-checks)";
_tag_dyneither(_tmp24F,sizeof(char),66);});_tmp24E;});_tmp175[35]=({struct
_tuple21*_tmp248=_region_malloc(r,sizeof(*_tmp248));_tmp248->f1=({const char*
_tmp24D="-up";_tag_dyneither(_tmp24D,sizeof(char),4);});_tmp248->f2=0;_tmp248->f3=({
const char*_tmp24C="";_tag_dyneither(_tmp24C,sizeof(char),1);});_tmp248->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp24A=_region_malloc(r,sizeof(*_tmp24A));
_tmp24A[0]=({struct Cyc_Arg_Clear_spec_struct _tmp24B;_tmp24B.tag=4;_tmp24B.f1=&
Cyc_pp_r;_tmp24B;});_tmp24A;});_tmp248->f5=({const char*_tmp249="Ugly print the C code that Cyclone generates (default)";
_tag_dyneither(_tmp249,sizeof(char),55);});_tmp248;});_tmp175[34]=({struct
_tuple21*_tmp242=_region_malloc(r,sizeof(*_tmp242));_tmp242->f1=({const char*
_tmp247="-pp";_tag_dyneither(_tmp247,sizeof(char),4);});_tmp242->f2=0;_tmp242->f3=({
const char*_tmp246="";_tag_dyneither(_tmp246,sizeof(char),1);});_tmp242->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp244=_region_malloc(r,sizeof(*_tmp244));_tmp244[
0]=({struct Cyc_Arg_Set_spec_struct _tmp245;_tmp245.tag=3;_tmp245.f1=& Cyc_pp_r;
_tmp245;});_tmp244;});_tmp242->f5=({const char*_tmp243="Pretty print the C code that Cyclone generates";
_tag_dyneither(_tmp243,sizeof(char),47);});_tmp242;});_tmp175[33]=({struct
_tuple21*_tmp23C=_region_malloc(r,sizeof(*_tmp23C));_tmp23C->f1=({const char*
_tmp241="-ic";_tag_dyneither(_tmp241,sizeof(char),4);});_tmp23C->f2=0;_tmp23C->f3=({
const char*_tmp240="";_tag_dyneither(_tmp240,sizeof(char),1);});_tmp23C->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp23E=_region_malloc(r,sizeof(*_tmp23E));_tmp23E[
0]=({struct Cyc_Arg_Set_spec_struct _tmp23F;_tmp23F.tag=3;_tmp23F.f1=& Cyc_ic_r;
_tmp23F;});_tmp23E;});_tmp23C->f5=({const char*_tmp23D="Activate the link-checker";
_tag_dyneither(_tmp23D,sizeof(char),26);});_tmp23C;});_tmp175[32]=({struct
_tuple21*_tmp236=_region_malloc(r,sizeof(*_tmp236));_tmp236->f1=({const char*
_tmp23B="-stopafter-toc";_tag_dyneither(_tmp23B,sizeof(char),15);});_tmp236->f2=
0;_tmp236->f3=({const char*_tmp23A="";_tag_dyneither(_tmp23A,sizeof(char),1);});
_tmp236->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp238=_region_malloc(r,
sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Arg_Set_spec_struct _tmp239;_tmp239.tag=
3;_tmp239.f1=& Cyc_toc_r;_tmp239;});_tmp238;});_tmp236->f5=({const char*_tmp237="Stop after translation to C";
_tag_dyneither(_tmp237,sizeof(char),28);});_tmp236;});_tmp175[31]=({struct
_tuple21*_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230->f1=({const char*
_tmp235="-stopafter-cf";_tag_dyneither(_tmp235,sizeof(char),14);});_tmp230->f2=0;
_tmp230->f3=({const char*_tmp234="";_tag_dyneither(_tmp234,sizeof(char),1);});
_tmp230->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp232=_region_malloc(r,
sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Arg_Set_spec_struct _tmp233;_tmp233.tag=
3;_tmp233.f1=& Cyc_cf_r;_tmp233;});_tmp232;});_tmp230->f5=({const char*_tmp231="Stop after control-flow checking";
_tag_dyneither(_tmp231,sizeof(char),33);});_tmp230;});_tmp175[30]=({struct
_tuple21*_tmp22A=_region_malloc(r,sizeof(*_tmp22A));_tmp22A->f1=({const char*
_tmp22F="-noprint";_tag_dyneither(_tmp22F,sizeof(char),9);});_tmp22A->f2=0;
_tmp22A->f3=({const char*_tmp22E="";_tag_dyneither(_tmp22E,sizeof(char),1);});
_tmp22A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22C=_region_malloc(r,
sizeof(*_tmp22C));_tmp22C[0]=({struct Cyc_Arg_Set_spec_struct _tmp22D;_tmp22D.tag=
3;_tmp22D.f1=& Cyc_noprint_r;_tmp22D;});_tmp22C;});_tmp22A->f5=({const char*
_tmp22B="Do not print output (when stopping early)";_tag_dyneither(_tmp22B,
sizeof(char),42);});_tmp22A;});_tmp175[29]=({struct _tuple21*_tmp224=
_region_malloc(r,sizeof(*_tmp224));_tmp224->f1=({const char*_tmp229="-stopafter-tc";
_tag_dyneither(_tmp229,sizeof(char),14);});_tmp224->f2=0;_tmp224->f3=({const char*
_tmp228="";_tag_dyneither(_tmp228,sizeof(char),1);});_tmp224->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp226=_region_malloc(r,sizeof(*_tmp226));_tmp226[0]=({
struct Cyc_Arg_Set_spec_struct _tmp227;_tmp227.tag=3;_tmp227.f1=& Cyc_tc_r;_tmp227;});
_tmp226;});_tmp224->f5=({const char*_tmp225="Stop after type checking";
_tag_dyneither(_tmp225,sizeof(char),25);});_tmp224;});_tmp175[28]=({struct
_tuple21*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));_tmp21E->f1=({const char*
_tmp223="-stopafter-parse";_tag_dyneither(_tmp223,sizeof(char),17);});_tmp21E->f2=
0;_tmp21E->f3=({const char*_tmp222="";_tag_dyneither(_tmp222,sizeof(char),1);});
_tmp21E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp220=_region_malloc(r,
sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Arg_Set_spec_struct _tmp221;_tmp221.tag=
3;_tmp221.f1=& Cyc_parseonly_r;_tmp221;});_tmp220;});_tmp21E->f5=({const char*
_tmp21F="Stop after parsing";_tag_dyneither(_tmp21F,sizeof(char),19);});_tmp21E;});
_tmp175[27]=({struct _tuple21*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->f1=({
const char*_tmp21D="-E";_tag_dyneither(_tmp21D,sizeof(char),3);});_tmp218->f2=0;
_tmp218->f3=({const char*_tmp21C="";_tag_dyneither(_tmp21C,sizeof(char),1);});
_tmp218->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp21A=_region_malloc(r,
sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Arg_Set_spec_struct _tmp21B;_tmp21B.tag=
3;_tmp21B.f1=& Cyc_stop_after_cpp_r;_tmp21B;});_tmp21A;});_tmp218->f5=({const char*
_tmp219="Stop after preprocessing";_tag_dyneither(_tmp219,sizeof(char),25);});
_tmp218;});_tmp175[26]=({struct _tuple21*_tmp212=_region_malloc(r,sizeof(*_tmp212));
_tmp212->f1=({const char*_tmp217="-Wall";_tag_dyneither(_tmp217,sizeof(char),6);});
_tmp212->f2=0;_tmp212->f3=({const char*_tmp216="";_tag_dyneither(_tmp216,sizeof(
char),1);});_tmp212->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp214=
_region_malloc(r,sizeof(*_tmp214));_tmp214[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp215;_tmp215.tag=0;_tmp215.f1=Cyc_set_all_warnings;_tmp215;});_tmp214;});
_tmp212->f5=({const char*_tmp213="Turn on all warnings";_tag_dyneither(_tmp213,
sizeof(char),21);});_tmp212;});_tmp175[25]=({struct _tuple21*_tmp20C=
_region_malloc(r,sizeof(*_tmp20C));_tmp20C->f1=({const char*_tmp211="-b";
_tag_dyneither(_tmp211,sizeof(char),3);});_tmp20C->f2=0;_tmp20C->f3=({const char*
_tmp210="<arch>";_tag_dyneither(_tmp210,sizeof(char),7);});_tmp20C->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp20E=_region_malloc(r,sizeof(*_tmp20E));
_tmp20E[0]=({struct Cyc_Arg_String_spec_struct _tmp20F;_tmp20F.tag=5;_tmp20F.f1=
Cyc_set_target_arch;_tmp20F;});_tmp20E;});_tmp20C->f5=({const char*_tmp20D="Set target architecture; implies -stopafter-toc";
_tag_dyneither(_tmp20D,sizeof(char),48);});_tmp20C;});_tmp175[24]=({struct
_tuple21*_tmp206=_region_malloc(r,sizeof(*_tmp206));_tmp206->f1=({const char*
_tmp20B="-MT";_tag_dyneither(_tmp20B,sizeof(char),4);});_tmp206->f2=0;_tmp206->f3=({
const char*_tmp20A=" <target>";_tag_dyneither(_tmp20A,sizeof(char),10);});_tmp206->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp208=_region_malloc(r,sizeof(*
_tmp208));_tmp208[0]=({struct Cyc_Arg_String_spec_struct _tmp209;_tmp209.tag=5;
_tmp209.f1=Cyc_set_dependencies_target;_tmp209;});_tmp208;});_tmp206->f5=({const
char*_tmp207="Give target for dependencies";_tag_dyneither(_tmp207,sizeof(char),
29);});_tmp206;});_tmp175[23]=({struct _tuple21*_tmp200=_region_malloc(r,sizeof(*
_tmp200));_tmp200->f1=({const char*_tmp205="-MG";_tag_dyneither(_tmp205,sizeof(
char),4);});_tmp200->f2=0;_tmp200->f3=({const char*_tmp204="";_tag_dyneither(
_tmp204,sizeof(char),1);});_tmp200->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp202=_region_malloc(r,sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp203;_tmp203.tag=1;_tmp203.f1=Cyc_add_cpparg;_tmp203;});_tmp202;});_tmp200->f5=({
const char*_tmp201="When producing dependencies assume that missing files are generated";
_tag_dyneither(_tmp201,sizeof(char),68);});_tmp200;});_tmp175[22]=({struct
_tuple21*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA->f1=({const char*
_tmp1FF="-M";_tag_dyneither(_tmp1FF,sizeof(char),3);});_tmp1FA->f2=0;_tmp1FA->f3=({
const char*_tmp1FE="";_tag_dyneither(_tmp1FE,sizeof(char),1);});_tmp1FA->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));
_tmp1FC[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=Cyc_set_produce_dependencies;
_tmp1FD;});_tmp1FC;});_tmp1FA->f5=({const char*_tmp1FB="Produce dependencies";
_tag_dyneither(_tmp1FB,sizeof(char),21);});_tmp1FA;});_tmp175[21]=({struct
_tuple21*_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->f1=({const char*
_tmp1F9="-S";_tag_dyneither(_tmp1F9,sizeof(char),3);});_tmp1F4->f2=0;_tmp1F4->f3=({
const char*_tmp1F8="";_tag_dyneither(_tmp1F8,sizeof(char),1);});_tmp1F4->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));
_tmp1F6[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=Cyc_set_stop_after_asmfile;
_tmp1F7;});_tmp1F6;});_tmp1F4->f5=({const char*_tmp1F5="Stop after producing assembly code";
_tag_dyneither(_tmp1F5,sizeof(char),35);});_tmp1F4;});_tmp175[20]=({struct
_tuple21*_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE->f1=({const char*
_tmp1F3="-pa";_tag_dyneither(_tmp1F3,sizeof(char),4);});_tmp1EE->f2=0;_tmp1EE->f3=({
const char*_tmp1F2="";_tag_dyneither(_tmp1F2,sizeof(char),1);});_tmp1EE->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));
_tmp1F0[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=Cyc_set_pa;
_tmp1F1;});_tmp1F0;});_tmp1EE->f5=({const char*_tmp1EF="Compile for profiling with aprof";
_tag_dyneither(_tmp1EF,sizeof(char),33);});_tmp1EE;});_tmp175[19]=({struct
_tuple21*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));_tmp1E8->f1=({const char*
_tmp1ED="-pg";_tag_dyneither(_tmp1ED,sizeof(char),4);});_tmp1E8->f2=0;_tmp1E8->f3=({
const char*_tmp1EC="";_tag_dyneither(_tmp1EC,sizeof(char),1);});_tmp1E8->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));
_tmp1EA[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1EB;_tmp1EB.tag=0;_tmp1EB.f1=Cyc_set_pg;
_tmp1EB;});_tmp1EA;});_tmp1E8->f5=({const char*_tmp1E9="Compile for profiling with gprof";
_tag_dyneither(_tmp1E9,sizeof(char),33);});_tmp1E8;});_tmp175[18]=({struct
_tuple21*_tmp1E2=_region_malloc(r,sizeof(*_tmp1E2));_tmp1E2->f1=({const char*
_tmp1E7="-p";_tag_dyneither(_tmp1E7,sizeof(char),3);});_tmp1E2->f2=0;_tmp1E2->f3=({
const char*_tmp1E6="";_tag_dyneither(_tmp1E6,sizeof(char),1);});_tmp1E2->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1E4=_region_malloc(r,sizeof(*_tmp1E4));
_tmp1E4[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1E5;_tmp1E5.tag=1;_tmp1E5.f1=Cyc_add_ccarg;
_tmp1E5;});_tmp1E4;});_tmp1E2->f5=({const char*_tmp1E3="Compile for profiling with prof";
_tag_dyneither(_tmp1E3,sizeof(char),32);});_tmp1E2;});_tmp175[17]=({struct
_tuple21*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));_tmp1DC->f1=({const char*
_tmp1E1="-g";_tag_dyneither(_tmp1E1,sizeof(char),3);});_tmp1DC->f2=0;_tmp1DC->f3=({
const char*_tmp1E0="";_tag_dyneither(_tmp1E0,sizeof(char),1);});_tmp1DC->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));
_tmp1DE[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1DF;_tmp1DF.tag=1;_tmp1DF.f1=Cyc_add_ccarg;
_tmp1DF;});_tmp1DE;});_tmp1DC->f5=({const char*_tmp1DD="Compile for debugging";
_tag_dyneither(_tmp1DD,sizeof(char),22);});_tmp1DC;});_tmp175[16]=({struct
_tuple21*_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));_tmp1D6->f1=({const char*
_tmp1DB="-fomit-frame-pointer";_tag_dyneither(_tmp1DB,sizeof(char),21);});
_tmp1D6->f2=0;_tmp1D6->f3=({const char*_tmp1DA="";_tag_dyneither(_tmp1DA,sizeof(
char),1);});_tmp1D6->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1D8=
_region_malloc(r,sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D9;_tmp1D9.tag=1;_tmp1D9.f1=Cyc_add_ccarg;_tmp1D9;});_tmp1D8;});_tmp1D6->f5=({
const char*_tmp1D7="Omit frame pointer";_tag_dyneither(_tmp1D7,sizeof(char),19);});
_tmp1D6;});_tmp175[15]=({struct _tuple21*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));
_tmp1D0->f1=({const char*_tmp1D5="-O3";_tag_dyneither(_tmp1D5,sizeof(char),4);});
_tmp1D0->f2=0;_tmp1D0->f3=({const char*_tmp1D4="";_tag_dyneither(_tmp1D4,sizeof(
char),1);});_tmp1D0->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1D2=
_region_malloc(r,sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D3;_tmp1D3.tag=1;_tmp1D3.f1=Cyc_add_ccarg;_tmp1D3;});_tmp1D2;});_tmp1D0->f5=({
const char*_tmp1D1="Even more optimization";_tag_dyneither(_tmp1D1,sizeof(char),
23);});_tmp1D0;});_tmp175[14]=({struct _tuple21*_tmp1CA=_region_malloc(r,sizeof(*
_tmp1CA));_tmp1CA->f1=({const char*_tmp1CF="-O2";_tag_dyneither(_tmp1CF,sizeof(
char),4);});_tmp1CA->f2=0;_tmp1CA->f3=({const char*_tmp1CE="";_tag_dyneither(
_tmp1CE,sizeof(char),1);});_tmp1CA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CC=_region_malloc(r,sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=Cyc_add_ccarg;_tmp1CD;});_tmp1CC;});_tmp1CA->f5=({
const char*_tmp1CB="A higher level of optimization";_tag_dyneither(_tmp1CB,
sizeof(char),31);});_tmp1CA;});_tmp175[13]=({struct _tuple21*_tmp1C4=
_region_malloc(r,sizeof(*_tmp1C4));_tmp1C4->f1=({const char*_tmp1C9="-O";
_tag_dyneither(_tmp1C9,sizeof(char),3);});_tmp1C4->f2=0;_tmp1C4->f3=({const char*
_tmp1C8="";_tag_dyneither(_tmp1C8,sizeof(char),1);});_tmp1C4->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6));_tmp1C6[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=Cyc_add_ccarg;
_tmp1C7;});_tmp1C6;});_tmp1C4->f5=({const char*_tmp1C5="Optimize";_tag_dyneither(
_tmp1C5,sizeof(char),9);});_tmp1C4;});_tmp175[12]=({struct _tuple21*_tmp1BE=
_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->f1=({const char*_tmp1C3="-O0";
_tag_dyneither(_tmp1C3,sizeof(char),4);});_tmp1BE->f2=0;_tmp1BE->f3=({const char*
_tmp1C2="";_tag_dyneither(_tmp1C2,sizeof(char),1);});_tmp1BE->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp1C0=_region_malloc(r,sizeof(*_tmp1C0));_tmp1C0[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1C1;_tmp1C1.tag=1;_tmp1C1.f1=Cyc_add_ccarg;
_tmp1C1;});_tmp1C0;});_tmp1BE->f5=({const char*_tmp1BF="Don't optimize";
_tag_dyneither(_tmp1BF,sizeof(char),15);});_tmp1BE;});_tmp175[11]=({struct
_tuple21*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));_tmp1B8->f1=({const char*
_tmp1BD="-s";_tag_dyneither(_tmp1BD,sizeof(char),3);});_tmp1B8->f2=0;_tmp1B8->f3=({
const char*_tmp1BC="";_tag_dyneither(_tmp1BC,sizeof(char),1);});_tmp1B8->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1BA=_region_malloc(r,sizeof(*_tmp1BA));
_tmp1BA[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=Cyc_add_ccarg;
_tmp1BB;});_tmp1BA;});_tmp1B8->f5=({const char*_tmp1B9="Remove all symbol table and relocation info from executable";
_tag_dyneither(_tmp1B9,sizeof(char),60);});_tmp1B8;});_tmp175[10]=({struct
_tuple21*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2->f1=({const char*
_tmp1B7="-x";_tag_dyneither(_tmp1B7,sizeof(char),3);});_tmp1B2->f2=0;_tmp1B2->f3=({
const char*_tmp1B6=" <language>";_tag_dyneither(_tmp1B6,sizeof(char),12);});
_tmp1B2->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp1B4=_region_malloc(r,
sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Arg_String_spec_struct _tmp1B5;_tmp1B5.tag=
5;_tmp1B5.f1=Cyc_set_inputtype;_tmp1B5;});_tmp1B4;});_tmp1B2->f5=({const char*
_tmp1B3="Specify <language> for the following input files";_tag_dyneither(
_tmp1B3,sizeof(char),49);});_tmp1B2;});_tmp175[9]=({struct _tuple21*_tmp1AC=
_region_malloc(r,sizeof(*_tmp1AC));_tmp1AC->f1=({const char*_tmp1B1="-c";
_tag_dyneither(_tmp1B1,sizeof(char),3);});_tmp1AC->f2=0;_tmp1AC->f3=({const char*
_tmp1B0="";_tag_dyneither(_tmp1B0,sizeof(char),1);});_tmp1AC->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp1AE=_region_malloc(r,sizeof(*_tmp1AE));_tmp1AE[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp1AF;_tmp1AF.tag=0;_tmp1AF.f1=Cyc_set_stop_after_objectfile;
_tmp1AF;});_tmp1AE;});_tmp1AC->f5=({const char*_tmp1AD="Produce an object file instead of an executable; do not link";
_tag_dyneither(_tmp1AD,sizeof(char),61);});_tmp1AC;});_tmp175[8]=({struct
_tuple21*_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6));_tmp1A6->f1=({const char*
_tmp1AB="-m";_tag_dyneither(_tmp1AB,sizeof(char),3);});_tmp1A6->f2=1;_tmp1A6->f3=({
const char*_tmp1AA="<option>";_tag_dyneither(_tmp1AA,sizeof(char),9);});_tmp1A6->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1A8=_region_malloc(r,sizeof(*_tmp1A8));
_tmp1A8[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A9;_tmp1A9.tag=1;_tmp1A9.f1=Cyc_add_marg;
_tmp1A9;});_tmp1A8;});_tmp1A6->f5=({const char*_tmp1A7="GCC specific: pass machine-dependent flag on to gcc";
_tag_dyneither(_tmp1A7,sizeof(char),52);});_tmp1A6;});_tmp175[7]=({struct
_tuple21*_tmp1A0=_region_malloc(r,sizeof(*_tmp1A0));_tmp1A0->f1=({const char*
_tmp1A5="-l";_tag_dyneither(_tmp1A5,sizeof(char),3);});_tmp1A0->f2=1;_tmp1A0->f3=({
const char*_tmp1A4="<libname>";_tag_dyneither(_tmp1A4,sizeof(char),10);});_tmp1A0->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1A2=_region_malloc(r,sizeof(*_tmp1A2));
_tmp1A2[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A3;_tmp1A3.tag=1;_tmp1A3.f1=Cyc_add_libarg;
_tmp1A3;});_tmp1A2;});_tmp1A0->f5=({const char*_tmp1A1="Library file";
_tag_dyneither(_tmp1A1,sizeof(char),13);});_tmp1A0;});_tmp175[6]=({struct
_tuple21*_tmp19A=_region_malloc(r,sizeof(*_tmp19A));_tmp19A->f1=({const char*
_tmp19F="-L";_tag_dyneither(_tmp19F,sizeof(char),3);});_tmp19A->f2=1;_tmp19A->f3=({
const char*_tmp19E="<dir>";_tag_dyneither(_tmp19E,sizeof(char),6);});_tmp19A->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp19C=_region_malloc(r,sizeof(*_tmp19C));
_tmp19C[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19D;_tmp19D.tag=1;_tmp19D.f1=Cyc_add_ccarg;
_tmp19D;});_tmp19C;});_tmp19A->f5=({const char*_tmp19B="Add to the list of directories for -l";
_tag_dyneither(_tmp19B,sizeof(char),38);});_tmp19A;});_tmp175[5]=({struct
_tuple21*_tmp194=_region_malloc(r,sizeof(*_tmp194));_tmp194->f1=({const char*
_tmp199="-I";_tag_dyneither(_tmp199,sizeof(char),3);});_tmp194->f2=1;_tmp194->f3=({
const char*_tmp198="<dir>";_tag_dyneither(_tmp198,sizeof(char),6);});_tmp194->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp196=_region_malloc(r,sizeof(*_tmp196));
_tmp196[0]=({struct Cyc_Arg_Flag_spec_struct _tmp197;_tmp197.tag=1;_tmp197.f1=Cyc_add_cpparg;
_tmp197;});_tmp196;});_tmp194->f5=({const char*_tmp195="Add to the list of directories to search for include files";
_tag_dyneither(_tmp195,sizeof(char),59);});_tmp194;});_tmp175[4]=({struct
_tuple21*_tmp18E=_region_malloc(r,sizeof(*_tmp18E));_tmp18E->f1=({const char*
_tmp193="-B";_tag_dyneither(_tmp193,sizeof(char),3);});_tmp18E->f2=1;_tmp18E->f3=({
const char*_tmp192="<file>";_tag_dyneither(_tmp192,sizeof(char),7);});_tmp18E->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp190=_region_malloc(r,sizeof(*_tmp190));
_tmp190[0]=({struct Cyc_Arg_Flag_spec_struct _tmp191;_tmp191.tag=1;_tmp191.f1=Cyc_add_cyclone_exec_path;
_tmp191;});_tmp190;});_tmp18E->f5=({const char*_tmp18F="Add to the list of directories to search for compiler files";
_tag_dyneither(_tmp18F,sizeof(char),60);});_tmp18E;});_tmp175[3]=({struct
_tuple21*_tmp188=_region_malloc(r,sizeof(*_tmp188));_tmp188->f1=({const char*
_tmp18D="-D";_tag_dyneither(_tmp18D,sizeof(char),3);});_tmp188->f2=1;_tmp188->f3=({
const char*_tmp18C="<name>[=<value>]";_tag_dyneither(_tmp18C,sizeof(char),17);});
_tmp188->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp18A=_region_malloc(r,
sizeof(*_tmp18A));_tmp18A[0]=({struct Cyc_Arg_Flag_spec_struct _tmp18B;_tmp18B.tag=
1;_tmp18B.f1=Cyc_add_cpparg;_tmp18B;});_tmp18A;});_tmp188->f5=({const char*
_tmp189="Pass definition to preprocessor";_tag_dyneither(_tmp189,sizeof(char),32);});
_tmp188;});_tmp175[2]=({struct _tuple21*_tmp182=_region_malloc(r,sizeof(*_tmp182));
_tmp182->f1=({const char*_tmp187="-o";_tag_dyneither(_tmp187,sizeof(char),3);});
_tmp182->f2=0;_tmp182->f3=({const char*_tmp186=" <file>";_tag_dyneither(_tmp186,
sizeof(char),8);});_tmp182->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp184=
_region_malloc(r,sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Arg_String_spec_struct
_tmp185;_tmp185.tag=5;_tmp185.f1=Cyc_set_output_file;_tmp185;});_tmp184;});
_tmp182->f5=({const char*_tmp183="Set the output file name to <file>";
_tag_dyneither(_tmp183,sizeof(char),35);});_tmp182;});_tmp175[1]=({struct
_tuple21*_tmp17C=_region_malloc(r,sizeof(*_tmp17C));_tmp17C->f1=({const char*
_tmp181="--version";_tag_dyneither(_tmp181,sizeof(char),10);});_tmp17C->f2=0;
_tmp17C->f3=({const char*_tmp180="";_tag_dyneither(_tmp180,sizeof(char),1);});
_tmp17C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp17E=_region_malloc(r,
sizeof(*_tmp17E));_tmp17E[0]=({struct Cyc_Arg_Unit_spec_struct _tmp17F;_tmp17F.tag=
0;_tmp17F.f1=Cyc_print_version;_tmp17F;});_tmp17E;});_tmp17C->f5=({const char*
_tmp17D="Print version information and exit";_tag_dyneither(_tmp17D,sizeof(char),
35);});_tmp17C;});_tmp175[0]=({struct _tuple21*_tmp176=_region_malloc(r,sizeof(*
_tmp176));_tmp176->f1=({const char*_tmp17B="-v";_tag_dyneither(_tmp17B,sizeof(
char),3);});_tmp176->f2=0;_tmp176->f3=({const char*_tmp17A="";_tag_dyneither(
_tmp17A,sizeof(char),1);});_tmp176->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp178=_region_malloc(r,sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Arg_Set_spec_struct
_tmp179;_tmp179.tag=3;_tmp179.f1=& Cyc_v_r;_tmp179;});_tmp178;});_tmp176->f5=({
const char*_tmp177="Print compilation stages verbosely";_tag_dyneither(_tmp177,
sizeof(char),35);});_tmp176;});((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp175,sizeof(struct
_tuple21*),65));});struct _dyneither_ptr optstring=({const char*_tmp174="Options:";
_tag_dyneither(_tmp174,sizeof(char),9);});Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp155);if(Cyc_target_arch.curr == (
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{struct _dyneither_ptr cyclone_exec_prefix=({char*_tmp173=
getenv((const char*)"CYCLONE_EXEC_PREFIX");_tag_dyneither(_tmp173,sizeof(char),
_get_zero_arr_size_char(_tmp173,1));});if(cyclone_exec_prefix.curr != (
_tag_dyneither(0,0,0)).curr)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp157=
_cycalloc(sizeof(*_tmp157));_tmp157->hd=({struct _dyneither_ptr*_tmp158=_cycalloc(
sizeof(*_tmp158));_tmp158[0]=cyclone_exec_prefix;_tmp158;});_tmp157->tl=Cyc_cyclone_exec_path;
_tmp157;});def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=({struct
_dyneither_ptr*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=(struct
_dyneither_ptr)Cyc_Filename_concat(def_lib_path,({const char*_tmp15B="cyc-lib";
_tag_dyneither(_tmp15B,sizeof(char),8);}));_tmp15A;});_tmp159->tl=Cyc_cyclone_exec_path;
_tmp159;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{struct _dyneither_ptr _tmp15C=Cyc_do_find(cyclone_arch_path,({
const char*_tmp172="cycspecs";_tag_dyneither(_tmp172,sizeof(char),9);}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp15C);({void*_tmp15D[1]={& _tmp15F};Cyc_fprintf(Cyc_stderr,({
const char*_tmp15E="Reading from specs file %s\n";_tag_dyneither(_tmp15E,sizeof(
char),28);}),_tag_dyneither(_tmp15D,sizeof(void*),1));});});{struct Cyc_List_List*
_tmp160=Cyc_read_specs(_tmp15C);struct _dyneither_ptr _tmp161=_tag_dyneither(0,0,0);{
struct _handler_cons _tmp162;_push_handler(& _tmp162);{int _tmp164=0;if(setjmp(
_tmp162.handler))_tmp164=1;if(!_tmp164){{struct _dyneither_ptr _tmp165=*((struct
_dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,
_tmp160,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",
sizeof(char),8));_tmp161=Cyc_split_specs(_tmp165);};_pop_handler();}else{void*
_tmp163=(void*)_exn_thrown;void*_tmp168=_tmp163;_LL36: {struct Cyc_Core_Not_found_struct*
_tmp169=(struct Cyc_Core_Not_found_struct*)_tmp168;if(_tmp169->tag != Cyc_Core_Not_found)
goto _LL38;}_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp168);_LL35:;}};}if(
_tmp161.curr != (_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp161);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp156);if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,({
const char*_tmp16A="";_tag_dyneither(_tmp16A,sizeof(char),1);}))== 0){const char*
_tmp16B=Cyc_produce_dependencies?"":(const char*)" -E";Cyc_set_cpp((struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15C);({struct Cyc_String_pa_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _dyneither_ptr)({const char*_tmp171=
_tmp16B;_tag_dyneither(_tmp171,sizeof(char),_get_zero_arr_size_char(_tmp171,1));});({
struct Cyc_String_pa_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)comp);({void*_tmp16C[3]={& _tmp16E,& _tmp16F,& _tmp170};Cyc_aprintf(({
const char*_tmp16D="%s -w -x c%s -specs %s";_tag_dyneither(_tmp16D,sizeof(char),
23);}),_tag_dyneither(_tmp16C,sizeof(void*),3));});});});}));}};};};};};}if(Cyc_cyclone_files
== 0  && Cyc_ccargs == 0){({void*_tmp308=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp309="missing file\n";_tag_dyneither(_tmp309,sizeof(char),14);}),
_tag_dyneither(_tmp308,sizeof(void*),0));});exit(1);}if(!Cyc_stop_after_cpp_r
 && !Cyc_nocyc_setjmp_r)Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,({const char*
_tmp30A="cyc_setjmp.h";_tag_dyneither(_tmp30A,sizeof(char),13);}));if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,({const char*_tmp30B="cyc_include.h";_tag_dyneither(_tmp30B,
sizeof(char),14);}));{struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp30C != 0;
_tmp30C=_tmp30C->tl){Cyc_process_file(*((struct _dyneither_ptr*)_tmp30C->hd));if(
Cyc_compile_failure){int _tmp30D=1;_npop_handler(0);return _tmp30D;}}}if(((Cyc_stop_after_cpp_r
 || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp30E=0;_npop_handler(0);
return _tmp30E;}if(Cyc_ccargs == 0){int _tmp30F=0;_npop_handler(0);return _tmp30F;}
Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp310="-L";
_tag_dyneither(_tmp310,sizeof(char),3);}),(struct _dyneither_ptr)def_lib_path));
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _dyneither_ptr _tmp311=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp373=" ";_tag_dyneither(
_tmp373,sizeof(char),2);}));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_libargs);{struct _dyneither_ptr _tmp312=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp370->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp370;}),({const char*_tmp372=" ";_tag_dyneither(_tmp372,sizeof(
char),2);}));struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;int
_tmp313=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || Cyc_output_file
!= 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({
const char*_tmp36E=".a";_tag_dyneither(_tmp36E,sizeof(char),3);}))) || Cyc_output_file
!= 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({
const char*_tmp36F=".lib";_tag_dyneither(_tmp36F,sizeof(char),5);}));if(_tmp313){
stdlib=0;stdlib_string=(struct _dyneither_ptr)({const char*_tmp314="";
_tag_dyneither(_tmp314,sizeof(char),1);});}else{struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;struct _dyneither_ptr runtime_filename;struct
_dyneither_ptr gc_filename;if(Cyc_pa_r){libcyc_flag=({const char*_tmp315="-lcyc_a";
_tag_dyneither(_tmp315,sizeof(char),8);});nogc_filename=({const char*_tmp316="nogc_a.a";
_tag_dyneither(_tmp316,sizeof(char),9);});runtime_filename=({const char*_tmp317="runtime_cyc_a.o";
_tag_dyneither(_tmp317,sizeof(char),16);});}else{if(Cyc_nocheck_r){libcyc_flag=({
const char*_tmp318="-lcyc_nocheck";_tag_dyneither(_tmp318,sizeof(char),14);});
nogc_filename=({const char*_tmp319="nogc_nocheck.a";_tag_dyneither(_tmp319,
sizeof(char),15);});runtime_filename=({const char*_tmp31A="runtime_cyc.o";
_tag_dyneither(_tmp31A,sizeof(char),14);});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp31B="-lcyc_pg";_tag_dyneither(_tmp31B,sizeof(char),9);});
runtime_filename=({const char*_tmp31C="runtime_cyc_pg.o";_tag_dyneither(_tmp31C,
sizeof(char),17);});nogc_filename=({const char*_tmp31D="nogc_pg.a";_tag_dyneither(
_tmp31D,sizeof(char),10);});}else{if(Cyc_compile_for_boot_r){libcyc_flag=({const
char*_tmp31E="-lcycboot";_tag_dyneither(_tmp31E,sizeof(char),10);});
nogc_filename=({const char*_tmp31F="nogc.a";_tag_dyneither(_tmp31F,sizeof(char),7);});
runtime_filename=({const char*_tmp320="runtime_cyc.o";_tag_dyneither(_tmp320,
sizeof(char),14);});}else{libcyc_flag=({const char*_tmp321="-lcyc";_tag_dyneither(
_tmp321,sizeof(char),6);});nogc_filename=({const char*_tmp322="nogc.a";
_tag_dyneither(_tmp322,sizeof(char),7);});runtime_filename=({const char*_tmp323="runtime_cyc.o";
_tag_dyneither(_tmp323,sizeof(char),14);});}}}}gc_filename=({const char*_tmp324="gc.a";
_tag_dyneither(_tmp324,sizeof(char),5);});{struct _dyneither_ptr _tmp325=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(cyclone_arch_path,
gc_filename);struct _dyneither_ptr _tmp326=Cyc_do_find(cyclone_arch_path,
runtime_filename);stdlib=0;stdlib_string=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp32B;_tmp32B.tag=0;_tmp32B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp325);({struct Cyc_String_pa_struct _tmp32A;_tmp32A.tag=0;_tmp32A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)libcyc_flag);({struct Cyc_String_pa_struct
_tmp329;_tmp329.tag=0;_tmp329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp326);({void*_tmp327[3]={& _tmp329,& _tmp32A,& _tmp32B};Cyc_aprintf(({const char*
_tmp328=" %s %s %s";_tag_dyneither(_tmp328,sizeof(char),10);}),_tag_dyneither(
_tmp327,sizeof(void*),3));});});});});};}if(Cyc_ic_r){struct _handler_cons _tmp32C;
_push_handler(& _tmp32C);{int _tmp32E=0;if(setjmp(_tmp32C.handler))_tmp32E=1;if(!
_tmp32E){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((
struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))
Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmp32F=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp313)_tmp32F=({struct Cyc_List_List*
_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->hd=Cyc_final_strptr;_tmp330->tl=
_tmp32F;_tmp330;});{struct Cyc_Interface_I*_tmp331=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,
struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,
_tmp32F,_tmp32F);if(_tmp331 == 0){({void*_tmp332=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp333="Error: interfaces incompatible\n";_tag_dyneither(_tmp333,sizeof(
char),32);}),_tag_dyneither(_tmp332,sizeof(void*),0));});Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp334=1;_npop_handler(1);return _tmp334;};}if(_tmp313){
if(Cyc_output_file != 0){struct _dyneither_ptr _tmp335=({struct Cyc_String_pa_struct
_tmp33C;_tmp33C.tag=0;_tmp33C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));({
void*_tmp33A[1]={& _tmp33C};Cyc_aprintf(({const char*_tmp33B="%s.cycio";
_tag_dyneither(_tmp33B,sizeof(char),9);}),_tag_dyneither(_tmp33A,sizeof(void*),1));});});
struct Cyc___cycFILE*_tmp336=Cyc_try_file_open((struct _dyneither_ptr)_tmp335,({
const char*_tmp338="wb";_tag_dyneither(_tmp338,sizeof(char),3);}),({const char*
_tmp339="interface object file";_tag_dyneither(_tmp339,sizeof(char),22);}));if(
_tmp336 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp337=1;
_npop_handler(1);return _tmp337;};}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp331,(struct Cyc___cycFILE*)_tmp336);Cyc_file_close((struct Cyc___cycFILE*)
_tmp336);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp331,({struct _tuple17*_tmp33D=_cycalloc(sizeof(*_tmp33D));
_tmp33D->f1=({const char*_tmp33F="empty interface";_tag_dyneither(_tmp33F,sizeof(
char),16);});_tmp33D->f2=({const char*_tmp33E="global interface";_tag_dyneither(
_tmp33E,sizeof(char),17);});_tmp33D;}))){({void*_tmp340=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp341="Error: some objects are still undefined\n";_tag_dyneither(
_tmp341,sizeof(char),41);}),_tag_dyneither(_tmp340,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp342=1;_npop_handler(1);return _tmp342;};}}};};;
_pop_handler();}else{void*_tmp32D=(void*)_exn_thrown;void*_tmp344=_tmp32D;_LL3B:;
_LL3C:{void*_tmp345=_tmp344;struct _dyneither_ptr _tmp347;struct _dyneither_ptr
_tmp349;struct _dyneither_ptr _tmp34C;_LL40: {struct Cyc_Core_Failure_struct*
_tmp346=(struct Cyc_Core_Failure_struct*)_tmp345;if(_tmp346->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp347=_tmp346->f1;}}_LL41:({struct Cyc_String_pa_struct _tmp34F;
_tmp34F.tag=0;_tmp34F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp347);({
void*_tmp34D[1]={& _tmp34F};Cyc_fprintf(Cyc_stderr,({const char*_tmp34E="Exception Core::Failure %s\n";
_tag_dyneither(_tmp34E,sizeof(char),28);}),_tag_dyneither(_tmp34D,sizeof(void*),
1));});});goto _LL3F;_LL42: {struct Cyc_Core_Impossible_struct*_tmp348=(struct Cyc_Core_Impossible_struct*)
_tmp345;if(_tmp348->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp349=_tmp348->f1;}}
_LL43:({struct Cyc_String_pa_struct _tmp352;_tmp352.tag=0;_tmp352.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp349);({void*_tmp350[1]={& _tmp352};Cyc_fprintf(
Cyc_stderr,({const char*_tmp351="Exception Core::Impossible %s\n";_tag_dyneither(
_tmp351,sizeof(char),31);}),_tag_dyneither(_tmp350,sizeof(void*),1));});});goto
_LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp34A=(struct Cyc_Dict_Absent_struct*)
_tmp345;if(_tmp34A->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:({void*_tmp353=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp354="Exception Dict::Absent\n";_tag_dyneither(_tmp354,
sizeof(char),24);}),_tag_dyneither(_tmp353,sizeof(void*),0));});goto _LL3F;_LL46: {
struct Cyc_Core_Invalid_argument_struct*_tmp34B=(struct Cyc_Core_Invalid_argument_struct*)
_tmp345;if(_tmp34B->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp34C=
_tmp34B->f1;}}_LL47:({struct Cyc_String_pa_struct _tmp357;_tmp357.tag=0;_tmp357.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34C);({void*_tmp355[1]={& _tmp357};
Cyc_fprintf(Cyc_stderr,({const char*_tmp356="Exception Core::Invalid_argument %s\n";
_tag_dyneither(_tmp356,sizeof(char),37);}),_tag_dyneither(_tmp355,sizeof(void*),
1));});});goto _LL3F;_LL48:;_LL49:({void*_tmp358=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp359="Uncaught exception\n";_tag_dyneither(_tmp359,sizeof(char),20);}),
_tag_dyneither(_tmp358,sizeof(void*),0));});goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp35A=1;_npop_handler(0);return _tmp35A;};_LL3D:;
_LL3E:(void)_throw(_tmp344);_LL3A:;}};}{struct _dyneither_ptr outfile_str=({const
char*_tmp36D="";_tag_dyneither(_tmp36D,sizeof(char),1);});if(Cyc_output_file != 0)
outfile_str=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp35D;_tmp35D.tag=
0;_tmp35D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file))));({void*_tmp35B[1]={&
_tmp35D};Cyc_aprintf(({const char*_tmp35C=" -o %s";_tag_dyneither(_tmp35C,sizeof(
char),7);}),_tag_dyneither(_tmp35B,sizeof(void*),1));});});{struct _dyneither_ptr
_tmp35E=({struct Cyc_String_pa_struct _tmp36C;_tmp36C.tag=0;_tmp36C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp312);({struct Cyc_String_pa_struct
_tmp36B;_tmp36B.tag=0;_tmp36B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stdlib_string);({struct Cyc_String_pa_struct _tmp36A;_tmp36A.tag=0;_tmp36A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp311);({struct Cyc_String_pa_struct
_tmp369;_tmp369.tag=0;_tmp369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
outfile_str);({struct Cyc_String_pa_struct _tmp368;_tmp368.tag=0;_tmp368.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)comp);({void*_tmp366[5]={& _tmp368,&
_tmp369,& _tmp36A,& _tmp36B,& _tmp36C};Cyc_aprintf(({const char*_tmp367="%s %s %s%s%s";
_tag_dyneither(_tmp367,sizeof(char),13);}),_tag_dyneither(_tmp366,sizeof(void*),
5));});});});});});});if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp361;_tmp361.tag=
0;_tmp361.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35E);({void*
_tmp35F[1]={& _tmp361};Cyc_fprintf(Cyc_stderr,({const char*_tmp360="%s\n";
_tag_dyneither(_tmp360,sizeof(char),4);}),_tag_dyneither(_tmp35F,sizeof(void*),1));});});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dyneither_ptr(_tmp35E,sizeof(char),1))!= 0){({void*_tmp362=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp363="Error: C compiler failed\n";_tag_dyneither(
_tmp363,sizeof(char),26);}),_tag_dyneither(_tmp362,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp364=1;_npop_handler(0);return _tmp364;};}Cyc_remove_cfiles();{
int _tmp365=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp365;};};};};};;
_pop_region(r);};}
