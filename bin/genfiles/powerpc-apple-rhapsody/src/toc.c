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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct
_dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr
elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
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
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(
enum Cyc_Absyn_Sign,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct
_dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
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
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;
struct Cyc_Position_Segment*loc;struct Cyc_List_List*non_local_preds;int try_depth;
void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;
struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*
body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*
fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};
struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct
Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
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
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple0*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);struct _tuple10{unsigned int
f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct _union_RelnOp_LessNumelts{
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
int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple13{
struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple13*
x,struct _tuple13*y){struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple13
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple13 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*
f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple14 _tmpC;void*_tmpD;
struct _tuple14*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp10="\n";_tag_dyneither(_tmp10,sizeof(char),2);}),_tag_dyneither(
_tmpF,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)
_throw((void*)({struct Cyc_Toc_Toc_Unimplemented_struct*_tmp11=_cycalloc_atomic(
sizeof(*_tmp11));_tmp11[0]=({struct Cyc_Toc_Toc_Unimplemented_struct _tmp12;_tmp12.tag=
Cyc_Toc_Toc_Unimplemented;_tmp12;});_tmp11;}));}static void*Cyc_Toc_toc_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp13=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14="\n";_tag_dyneither(
_tmp14,sizeof(char),2);}),_tag_dyneither(_tmp13,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)({struct Cyc_Toc_Toc_Impossible_struct*
_tmp15=_cycalloc_atomic(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Toc_Toc_Impossible_struct
_tmp16;_tmp16.tag=Cyc_Toc_Toc_Impossible;_tmp16;});_tmp15;}));}char Cyc_Toc_Match_error[
12]="Match_error";struct Cyc_Toc_Match_error_struct{char*tag;};static char _tmp17[5]="curr";
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp17,_tmp17,_tmp17 + 5};static
struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp18[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,_tmp18,_tmp18 + 4};static
struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp19[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,_tmp19,_tmp19 + 4};static
struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1A[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp1A,_tmp1A,_tmp1A + 14};
static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp1B[8]="handler";static struct _dyneither_ptr Cyc_Toc_handler_string={
_tmp1B,_tmp1B,_tmp1B + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp1C[14]="_RegionHandle";static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={
_tmp1C,_tmp1C,_tmp1C + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string;static char _tmp1D[17]="_DynRegionHandle";static
struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1D,_tmp1D,_tmp1D + 17};
static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1E[16]="_DynRegionFrame";static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1E,_tmp1E,_tmp1E + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=&
Cyc_Toc__DynRegionFrame_string;struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(
void*)0,(void*)(0 + 0)};static char _tmp1F[7]="_throw";static struct _dyneither_ptr
Cyc_Toc__throw_str={_tmp1F,_tmp1F,_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={
3,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={
1,& Cyc_Toc__throw_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static
struct _tuple0 Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)&
Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)&
Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp23[14]="_push_handler";static
struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp23,_tmp23,_tmp23 + 14};static
struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={
0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmp25[13]="_pop_handler";static struct _dyneither_ptr Cyc_Toc__pop_handler_str={
_tmp25,_tmp25,_tmp25 + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={3,
0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev;static char _tmp27[12]="_exn_thrown";static struct
_dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp27,_tmp27,_tmp27 + 12};static struct
_tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,0}},& Cyc_Toc__exn_thrown_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char
_tmp29[14]="_npop_handler";static struct _dyneither_ptr Cyc_Toc__npop_handler_str={
_tmp29,_tmp29,_tmp29 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp2B[12]="_check_null";static struct
_dyneither_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12};static struct
_tuple0 Cyc_Toc__check_null_pr={{.Loc_n={3,0}},& Cyc_Toc__check_null_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={
0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp2D[28]="_check_known_subscript_null";static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={
_tmp2D,_tmp2D,_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={3,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp37[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={3,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={
0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp3B[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp3F[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,
_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={3,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,
_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={3,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static
struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp51[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp55[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={
1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp59[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,
_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,
_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={3,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={3,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,
_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};
static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp89[18]="_cyccalloc_atomic";static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={
3,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmp8D[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={
3,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple0 Cyc_Toc__check_times_pr={{.Loc_n={3,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple0 Cyc_Toc__push_region_pr={{.Loc_n={3,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={
0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static
char _tmp99[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={
0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmp9D[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={
3,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static
struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={3,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static
struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={12,{{.UnknownAggr={1,{
Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=Cyc_Absyn_skip_stmt(0);_tmpA8;});
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpAA;_tmpAA.tag=15;_tmpAA.f1=(void*)t;_tmpAA.f2=e;_tmpAA.f3=0;_tmpAA.f4=Cyc_Absyn_No_coercion;
_tmpAA;});_tmpA9;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));
_tmpAB[0]=({struct Cyc_Absyn_Deref_e_struct _tmpAC;_tmpAC.tag=21;_tmpAC.f1=e;
_tmpAC;});_tmpAB;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpAE;
_tmpAE.tag=24;_tmpAE.f1=e1;_tmpAE.f2=e2;_tmpAE;});_tmpAD;});}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpB0;
_tmpB0.tag=37;_tmpB0.f1=s;_tmpB0;});_tmpAF;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpB1=_cycalloc(
sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB2;_tmpB2.tag=
18;_tmpB2.f1=(void*)t;_tmpB2;});_tmpB1;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpB4;_tmpB4.tag=11;_tmpB4.f1=e;_tmpB4.f2=es;_tmpB4.f3=0;_tmpB4;});_tmpB3;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Exp_s_struct _tmpB6;
_tmpB6.tag=1;_tmpB6.f1=e;_tmpB6;});_tmpB5;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Seq_s_struct _tmpB8;
_tmpB8.tag=2;_tmpB8.f1=s1;_tmpB8.f2=s2;_tmpB8;});_tmpB7;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));
_tmpB9[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpBA;_tmpBA.tag=6;_tmpBA.f1=e1;
_tmpBA.f2=e2;_tmpBA.f3=e3;_tmpBA;});_tmpB9;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpBC;_tmpBC.tag=22;_tmpBC.f1=e;_tmpBC.f2=n;_tmpBC.f3=0;_tmpBC.f4=0;_tmpBC;});
_tmpBB;});}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpBD=
_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpBE;
_tmpBE.tag=23;_tmpBE.f1=e;_tmpBE.f2=n;_tmpBE.f3=0;_tmpBE.f4=0;_tmpBE;});_tmpBD;});}
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*
ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpBF=_cycalloc(
sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmpC0;_tmpC0.tag=
36;_tmpC0.f1=tdopt;_tmpC0.f2=ds;_tmpC0;});_tmpBF;});}static void*Cyc_Toc_goto_stmt_r(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_Goto_s_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_Goto_s_struct
_tmpC2;_tmpC2.tag=8;_tmpC2.f1=v;_tmpC2.f2=s;_tmpC2;});_tmpC1;});}static struct Cyc_Absyn_Const_e_struct
Cyc_Toc_zero_exp={0,{.Int_c={4,{Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{
struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;
struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*
flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={&
Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,&
Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,&
Cyc_Toc__zero_arr_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={&
Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,&
Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=t;
enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Impossible_struct
_tmpCC;_tmpCC.tag=Cyc_Core_Impossible;_tmpCC.f1=({const char*_tmpCD="impossible IntType (not char, short or int)";
_tag_dyneither(_tmpCD,sizeof(char),44);});_tmpCC;});_tmpCB;}));}goto _LL0;_LL3: {
struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Core_Impossible_struct _tmpCF;_tmpCF.tag=
Cyc_Core_Impossible;_tmpCF.f1=({const char*_tmpD0="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmpD0,sizeof(char),64);});_tmpCF;});_tmpCE;}));_LL0:;}return
function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,
struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD3;void*_tmpD4;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD2=(struct Cyc_Absyn_PointerType_struct*)_tmpD1;if(_tmpD2->tag != 5)goto _LL16;
else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD3.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD4);_LL16:;_LL17:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Core_Impossible_struct
_tmpD6;_tmpD6.tag=Cyc_Core_Impossible;_tmpD6.f1=({const char*_tmpD7="impossible type (not pointer)";
_tag_dyneither(_tmpD7,sizeof(char),30);});_tmpD6;});_tmpD5;}));_LL13:;}struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpD8=e->r;union Cyc_Absyn_Cnst _tmpDA;struct _tuple3
_tmpDB;char _tmpDC;union Cyc_Absyn_Cnst _tmpDE;struct _tuple4 _tmpDF;short _tmpE0;
union Cyc_Absyn_Cnst _tmpE2;struct _tuple5 _tmpE3;int _tmpE4;union Cyc_Absyn_Cnst
_tmpE6;struct _tuple6 _tmpE7;long long _tmpE8;union Cyc_Absyn_Cnst _tmpEA;int _tmpEB;
struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpF1;
struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;
_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL1B;else{_tmpDA=_tmpD9->f1;if((_tmpDA.Char_c).tag
!= 2)goto _LL1B;_tmpDB=(struct _tuple3)(_tmpDA.Char_c).val;_tmpDC=_tmpDB.f2;}}
_LL1A: return _tmpDC == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(
struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpDD->tag != 0)goto _LL1D;else{_tmpDE=
_tmpDD->f1;if((_tmpDE.Short_c).tag != 3)goto _LL1D;_tmpDF=(struct _tuple4)(_tmpDE.Short_c).val;
_tmpE0=_tmpDF.f2;}}_LL1C: return _tmpE0 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE1=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE1->tag != 0)goto _LL1F;
else{_tmpE2=_tmpE1->f1;if((_tmpE2.Int_c).tag != 4)goto _LL1F;_tmpE3=(struct _tuple5)(
_tmpE2.Int_c).val;_tmpE4=_tmpE3.f2;}}_LL1E: return _tmpE4 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE5=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE5->tag != 0)goto _LL21;
else{_tmpE6=_tmpE5->f1;if((_tmpE6.LongLong_c).tag != 5)goto _LL21;_tmpE7=(struct
_tuple6)(_tmpE6.LongLong_c).val;_tmpE8=_tmpE7.f2;}}_LL20: return _tmpE8 == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpE9=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;
if(_tmpE9->tag != 0)goto _LL23;else{_tmpEA=_tmpE9->f1;if((_tmpEA.Null_c).tag != 1)
goto _LL23;_tmpEB=(int)(_tmpEA.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpEC=(struct Cyc_Absyn_Cast_e_struct*)_tmpD8;if(_tmpEC->tag != 15)goto _LL25;
else{_tmpED=_tmpEC->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpED);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpEE=(struct Cyc_Absyn_Tuple_e_struct*)_tmpD8;if(_tmpEE->tag != 25)goto _LL27;
else{_tmpEF=_tmpEE->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEF);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF0=(struct Cyc_Absyn_Array_e_struct*)_tmpD8;if(
_tmpF0->tag != 27)goto _LL29;else{_tmpF1=_tmpF0->f1;}}_LL28: _tmpF3=_tmpF1;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpD8;if(_tmpF2->tag != 29)goto _LL2B;else{_tmpF3=_tmpF2->f3;}}_LL2A: _tmpF5=
_tmpF3;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF4=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD8;if(_tmpF4->tag != 26)goto _LL2D;else{_tmpF5=_tmpF4->f2;}}_LL2C: _tmpF7=
_tmpF5;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF6=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD8;if(_tmpF6->tag != 36)goto _LL2F;else{
_tmpF7=_tmpF6->f2;}}_LL2E: for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpF7->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpF8=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpFA;struct Cyc_Absyn_PtrAtts _tmpFB;union Cyc_Absyn_Constraint*
_tmpFC;_LL32: {struct Cyc_Absyn_PointerType_struct*_tmpF9=(struct Cyc_Absyn_PointerType_struct*)
_tmpF8;if(_tmpF9->tag != 5)goto _LL34;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.ptr_atts;
_tmpFC=_tmpFB.nullable;}}_LL33: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpFC);_LL34:;_LL35:({void*_tmpFD=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpFE="is_nullable";
_tag_dyneither(_tmpFE,sizeof(char),12);}),_tag_dyneither(_tmpFD,sizeof(void*),0));});
_LL31:;}static char _tmp10B[1]="";static char _tmp10C[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag){
struct _DynRegionHandle*_tmp100;struct Cyc_Dict_Dict*_tmp101;struct Cyc_Toc_TocState
_tmpFF=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp100=_tmpFF.dyn;
_tmp101=_tmpFF.qvar_tags;{static struct _dyneither_ptr bogus_string={_tmp10C,
_tmp10C,_tmp10C + 8};static struct _tuple0 bogus_qvar={{.Loc_n={3,0}},& bogus_string};
static struct _tuple13 pair={& bogus_qvar,{_tmp10B,_tmp10B,_tmp10B + 1}};pair=({
struct _tuple13 _tmp102;_tmp102.f1=x;_tmp102.f2=tag;_tmp102;});{struct
_DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,_tmp100);{
struct _tuple0**_tmp104=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple13*
k))Cyc_Dict_lookup_opt)(*_tmp101,(struct _tuple13*)& pair);if(_tmp104 != 0){struct
_tuple0*_tmp105=*_tmp104;_npop_handler(0);return _tmp105;}{struct _tuple13*_tmp106=({
struct _tuple13*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=x;_tmp10A->f2=tag;
_tmp10A;});struct _tuple0*res=({struct _tuple0*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->f1=(*x).f1;_tmp108->f2=({struct _dyneither_ptr*_tmp109=_cycalloc(sizeof(*
_tmp109));_tmp109[0]=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*
x).f2,(struct _dyneither_ptr)tag);_tmp109;});_tmp108;});*_tmp101=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp101,(
struct _tuple13*)_tmp106,res);{struct _tuple0*_tmp107=res;_npop_handler(0);return
_tmp107;};};};_pop_dynregion(d);};};}struct _tuple16{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp10E;struct Cyc_List_List**_tmp10F;struct Cyc_Toc_TocState
_tmp10D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10E=
_tmp10D.dyn;_tmp10F=_tmp10D.tuple_types;{struct _DynRegionFrame _tmp110;struct
_RegionHandle*d=_open_dynregion(& _tmp110,_tmp10E);{struct Cyc_List_List*_tmp111=*
_tmp10F;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct _tuple16 _tmp113;void*
_tmp114;struct Cyc_List_List*_tmp115;struct _tuple16*_tmp112=(struct _tuple16*)
_tmp111->hd;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{struct Cyc_List_List*
_tmp116=tqs0;for(0;_tmp116 != 0  && _tmp115 != 0;(_tmp116=_tmp116->tl,_tmp115=
_tmp115->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp116->hd)).f2,(void*)
_tmp115->hd))break;}if(_tmp116 == 0  && _tmp115 == 0){void*_tmp117=_tmp114;
_npop_handler(0);return _tmp117;}};}}{struct _dyneither_ptr*xname=({struct
_dyneither_ptr*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp128;_tmp128.tag=1;_tmp128.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp126[1]={& _tmp128};Cyc_aprintf(({
const char*_tmp127="_tuple%d";_tag_dyneither(_tmp127,sizeof(char),9);}),
_tag_dyneither(_tmp126,sizeof(void*),1));});});_tmp125;});void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){_tmp118=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*
_tmp119));_tmp119->hd=({struct Cyc_Absyn_Aggrfield*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A->name=Cyc_Absyn_fieldname(i);_tmp11A->tq=Cyc_Toc_mt_tq;_tmp11A->type=(
void*)ts2->hd;_tmp11A->width=0;_tmp11A->attributes=0;_tmp11A;});_tmp119->tl=
_tmp118;_tmp119;});}}_tmp118=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp118);{struct Cyc_Absyn_Aggrdecl*_tmp11B=({struct Cyc_Absyn_Aggrdecl*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->kind=Cyc_Absyn_StructA;_tmp122->sc=
Cyc_Absyn_Public;_tmp122->name=({struct _tuple0*_tmp124=_cycalloc(sizeof(*_tmp124));
_tmp124->f1=Cyc_Absyn_Rel_n(0);_tmp124->f2=xname;_tmp124;});_tmp122->tvs=0;
_tmp122->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123->exist_vars=0;_tmp123->rgn_po=0;_tmp123->fields=_tmp118;_tmp123->tagged=
0;_tmp123;});_tmp122->attributes=0;_tmp122;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp11E;_tmp11E.tag=6;_tmp11E.f1=_tmp11B;_tmp11E;});
_tmp11D;}),0);_tmp11C->tl=Cyc_Toc_result_decls;_tmp11C;});*_tmp10F=({struct Cyc_List_List*
_tmp11F=_region_malloc(d,sizeof(*_tmp11F));_tmp11F->hd=({struct _tuple16*_tmp120=
_region_malloc(d,sizeof(*_tmp120));_tmp120->f1=x;_tmp120->f2=ts;_tmp120;});
_tmp11F->tl=*_tmp10F;_tmp11F;});{void*_tmp121=x;_npop_handler(0);return _tmp121;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp129=Cyc_Toc_temp_var_counter
++;struct _tuple0*res=({struct _tuple0*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->f1=
Cyc_Absyn_Loc_n;_tmp12A->f2=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*
_tmp12B));_tmp12B[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp12E;
_tmp12E.tag=1;_tmp12E.f1=(unsigned int)_tmp129;({void*_tmp12C[1]={& _tmp12E};Cyc_aprintf(({
const char*_tmp12D="_tmp%X";_tag_dyneither(_tmp12D,sizeof(char),7);}),
_tag_dyneither(_tmp12C,sizeof(void*),1));});});_tmp12B;});_tmp12A;});return res;}
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp130;
struct Cyc_Xarray_Xarray*_tmp131;struct Cyc_Toc_TocState _tmp12F=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp130=_tmp12F.dyn;_tmp131=_tmp12F.temp_labels;{
struct _DynRegionFrame _tmp132;struct _RegionHandle*d=_open_dynregion(& _tmp132,
_tmp130);{int _tmp133=Cyc_Toc_fresh_label_counter ++;if(_tmp133 < ((int(*)(struct
Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp131)){struct _dyneither_ptr*_tmp134=((
struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp131,
_tmp133);_npop_handler(0);return _tmp134;}{struct _dyneither_ptr*res=({struct
_dyneither_ptr*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp13B;_tmp13B.tag=1;_tmp13B.f1=(
unsigned int)_tmp133;({void*_tmp139[1]={& _tmp13B};Cyc_aprintf(({const char*
_tmp13A="_LL%X";_tag_dyneither(_tmp13A,sizeof(char),6);}),_tag_dyneither(_tmp139,
sizeof(void*),1));});});_tmp138;});if(((int(*)(struct Cyc_Xarray_Xarray*,struct
_dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp131,res)!= _tmp133)({void*_tmp135=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp136="fresh_label: add_ind returned bad index...";_tag_dyneither(
_tmp136,sizeof(char),43);}),_tag_dyneither(_tmp135,sizeof(void*),0));});{struct
_dyneither_ptr*_tmp137=res;_npop_handler(0);return _tmp137;};};};_pop_dynregion(d);};}
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,
struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp13C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13C))->hd)->name)
!= 0){++ ans;_tmp13C=_tmp13C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a){struct Cyc_Core_Opt*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;
struct _tuple7 _tmp13D=*a;_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;_tmp140=_tmp13D.f3;
return({struct _tuple7*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=_tmp13E;
_tmp141->f2=_tmp13F;_tmp141->f3=Cyc_Toc_typ_to_c(_tmp140);_tmp141;});}static
struct _tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp143;
void*_tmp144;struct _tuple9 _tmp142=*x;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;
return({struct _tuple9*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=_tmp143;
_tmp145->f2=Cyc_Toc_typ_to_c(_tmp144);_tmp145;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp146=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp148;
void*_tmp149;struct Cyc_Absyn_Tqual _tmp14A;struct Cyc_Absyn_Exp*_tmp14B;union Cyc_Absyn_Constraint*
_tmp14C;struct Cyc_Position_Segment*_tmp14D;struct Cyc_Core_Opt*_tmp14F;struct Cyc_Core_Opt
_tmp150;void*_tmp151;_LL37: {struct Cyc_Absyn_ArrayType_struct*_tmp147=(struct Cyc_Absyn_ArrayType_struct*)
_tmp146;if(_tmp147->tag != 9)goto _LL39;else{_tmp148=_tmp147->f1;_tmp149=_tmp148.elt_type;
_tmp14A=_tmp148.tq;_tmp14B=_tmp148.num_elts;_tmp14C=_tmp148.zero_term;_tmp14D=
_tmp148.zt_loc;}}_LL38: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp149),
_tmp14A,_tmp14B,Cyc_Absyn_false_conref,_tmp14D);_LL39: {struct Cyc_Absyn_Evar_struct*
_tmp14E=(struct Cyc_Absyn_Evar_struct*)_tmp146;if(_tmp14E->tag != 1)goto _LL3B;
else{_tmp14F=_tmp14E->f2;if(_tmp14F == 0)goto _LL3B;_tmp150=*_tmp14F;_tmp151=(void*)
_tmp150.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(_tmp151);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(
t);_LL36:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->name=
f->name;_tmp152->tq=Cyc_Toc_mt_tq;_tmp152->type=Cyc_Toc_typ_to_c(f->type);
_tmp152->width=f->width;_tmp152->attributes=f->attributes;_tmp152;});}static void
Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0)cs=({void**_tmp153=_cycalloc(sizeof(*_tmp153));
_tmp153[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp153;});return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);
_tmp154;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0)r=({
void**_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp155;});return*r;}static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp156=t;struct Cyc_Core_Opt*_tmp159;struct Cyc_Core_Opt*_tmp15B;
struct Cyc_Core_Opt _tmp15C;void*_tmp15D;struct Cyc_Absyn_Tvar*_tmp15F;struct Cyc_Absyn_DatatypeFieldInfo
_tmp162;union Cyc_Absyn_DatatypeFieldInfoU _tmp163;struct _tuple1 _tmp164;struct Cyc_Absyn_Datatypedecl*
_tmp165;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_Absyn_PtrInfo _tmp169;
void*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;struct Cyc_Absyn_PtrAtts _tmp16C;union
Cyc_Absyn_Constraint*_tmp16D;struct Cyc_Absyn_ArrayInfo _tmp172;void*_tmp173;
struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Position_Segment*
_tmp176;struct Cyc_Absyn_FnInfo _tmp178;void*_tmp179;struct Cyc_List_List*_tmp17A;
int _tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17D;struct
Cyc_List_List*_tmp17F;enum Cyc_Absyn_AggrKind _tmp181;struct Cyc_List_List*_tmp182;
struct Cyc_Absyn_AggrInfo _tmp184;union Cyc_Absyn_AggrInfoU _tmp185;struct Cyc_List_List*
_tmp186;struct _tuple0*_tmp188;struct Cyc_List_List*_tmp18A;struct _tuple0*_tmp18C;
struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Typedefdecl*_tmp18E;void**_tmp18F;
void*_tmp192;_LL3E: {struct Cyc_Absyn_VoidType_struct*_tmp157=(struct Cyc_Absyn_VoidType_struct*)
_tmp156;if(_tmp157->tag != 0)goto _LL40;}_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*
_tmp158=(struct Cyc_Absyn_Evar_struct*)_tmp156;if(_tmp158->tag != 1)goto _LL42;
else{_tmp159=_tmp158->f2;if(_tmp159 != 0)goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;
_LL42: {struct Cyc_Absyn_Evar_struct*_tmp15A=(struct Cyc_Absyn_Evar_struct*)
_tmp156;if(_tmp15A->tag != 1)goto _LL44;else{_tmp15B=_tmp15A->f2;if(_tmp15B == 0)
goto _LL44;_tmp15C=*_tmp15B;_tmp15D=(void*)_tmp15C.v;}}_LL43: return Cyc_Toc_typ_to_c(
_tmp15D);_LL44: {struct Cyc_Absyn_VarType_struct*_tmp15E=(struct Cyc_Absyn_VarType_struct*)
_tmp156;if(_tmp15E->tag != 2)goto _LL46;else{_tmp15F=_tmp15E->f1;}}_LL45: if(Cyc_Tcutil_tvar_kind(
_tmp15F,Cyc_Absyn_BoxKind)== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;
else{return Cyc_Absyn_void_star_typ();}_LL46: {struct Cyc_Absyn_DatatypeType_struct*
_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)_tmp156;if(_tmp160->tag != 3)goto
_LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp161=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp156;if(_tmp161->tag != 4)
goto _LL4A;else{_tmp162=_tmp161->f1;_tmp163=_tmp162.field_info;if((_tmp163.KnownDatatypefield).tag
!= 2)goto _LL4A;_tmp164=(struct _tuple1)(_tmp163.KnownDatatypefield).val;_tmp165=
_tmp164.f1;_tmp166=_tmp164.f2;}}_LL49: return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp166->name,({const char*_tmp19A="_struct";_tag_dyneither(_tmp19A,sizeof(char),
8);})));_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp167=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp156;if(_tmp167->tag != 4)goto _LL4C;}_LL4B:({void*_tmp19B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp19C="unresolved DatatypeFieldType";
_tag_dyneither(_tmp19C,sizeof(char),29);}),_tag_dyneither(_tmp19B,sizeof(void*),
0));});_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp168=(struct Cyc_Absyn_PointerType_struct*)
_tmp156;if(_tmp168->tag != 5)goto _LL4E;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.elt_typ;
_tmp16B=_tmp169.elt_tq;_tmp16C=_tmp169.ptr_atts;_tmp16D=_tmp16C.bounds;}}_LL4D:
_tmp16A=Cyc_Toc_typ_to_c_array(_tmp16A);{void*_tmp19D=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16D);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp19E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19D;if(_tmp19E->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16A,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16B,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp16E=(struct Cyc_Absyn_IntType_struct*)_tmp156;if(_tmp16E->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp16F=(struct Cyc_Absyn_FloatType_struct*)
_tmp156;if(_tmp16F->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp170=(struct Cyc_Absyn_DoubleType_struct*)_tmp156;if(_tmp170->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp171=(struct Cyc_Absyn_ArrayType_struct*)
_tmp156;if(_tmp171->tag != 9)goto _LL56;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.elt_type;
_tmp174=_tmp172.tq;_tmp175=_tmp172.num_elts;_tmp176=_tmp172.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp173),_tmp174,_tmp175,Cyc_Absyn_false_conref,
_tmp176);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp177=(struct Cyc_Absyn_FnType_struct*)
_tmp156;if(_tmp177->tag != 10)goto _LL58;else{_tmp178=_tmp177->f1;_tmp179=_tmp178.ret_typ;
_tmp17A=_tmp178.args;_tmp17B=_tmp178.c_varargs;_tmp17C=_tmp178.cyc_varargs;
_tmp17D=_tmp178.attributes;}}_LL57: {struct Cyc_List_List*_tmp19F=0;for(0;_tmp17D
!= 0;_tmp17D=_tmp17D->tl){void*_tmp1A0=(void*)_tmp17D->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A1=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A2=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A0;if(_tmp1A2->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A3=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A0;if(_tmp1A3->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1A4=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1A0;if(_tmp1A4->tag != 20)
goto _LL84;}_LL83: continue;_LL84:;_LL85: _tmp19F=({struct Cyc_List_List*_tmp1A5=
_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=(void*)_tmp17D->hd;_tmp1A5->tl=_tmp19F;
_tmp1A5;});goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp17A);if(_tmp17C != 0){void*_tmp1A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmp1A8=({struct _tuple7*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA->f1=_tmp17C->name;_tmp1AA->f2=_tmp17C->tq;_tmp1AA->f3=_tmp1A7;_tmp1AA;});
_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1A6,({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*
_tmp1A9));_tmp1A9->hd=_tmp1A8;_tmp1A9->tl=0;_tmp1A9;}));}return(void*)({struct
Cyc_Absyn_FnType_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct
Cyc_Absyn_FnType_struct _tmp1AC;_tmp1AC.tag=10;_tmp1AC.f1=({struct Cyc_Absyn_FnInfo
_tmp1AD;_tmp1AD.tvars=0;_tmp1AD.effect=0;_tmp1AD.ret_typ=Cyc_Toc_typ_to_c(
_tmp179);_tmp1AD.args=_tmp1A6;_tmp1AD.c_varargs=_tmp17B;_tmp1AD.cyc_varargs=0;
_tmp1AD.rgn_po=0;_tmp1AD.attributes=_tmp19F;_tmp1AD;});_tmp1AC;});_tmp1AB;});};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp17E=(struct Cyc_Absyn_TupleType_struct*)
_tmp156;if(_tmp17E->tag != 11)goto _LL5A;else{_tmp17F=_tmp17E->f1;}}_LL59: _tmp17F=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp17F);return Cyc_Toc_add_tuple_type(_tmp17F);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp180=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp156;if(_tmp180->tag != 13)goto _LL5C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}
_LL5B: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp1AE=_cycalloc(
sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp1AF;
_tmp1AF.tag=13;_tmp1AF.f1=_tmp181;_tmp1AF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp182);_tmp1AF;});_tmp1AE;});_LL5C: {struct Cyc_Absyn_AggrType_struct*_tmp183=(
struct Cyc_Absyn_AggrType_struct*)_tmp156;if(_tmp183->tag != 12)goto _LL5E;else{
_tmp184=_tmp183->f1;_tmp185=_tmp184.aggr_info;_tmp186=_tmp184.targs;}}_LL5D:{
union Cyc_Absyn_AggrInfoU _tmp1B0=_tmp185;struct _tuple2 _tmp1B1;_LL87: if((_tmp1B0.UnknownAggr).tag
!= 1)goto _LL89;_tmp1B1=(struct _tuple2)(_tmp1B0.UnknownAggr).val;_LL88: return t;
_LL89:;_LL8A: goto _LL86;_LL86:;}{struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(
_tmp185);if(_tmp1B2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B2->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_strctq);}};
_LL5E: {struct Cyc_Absyn_EnumType_struct*_tmp187=(struct Cyc_Absyn_EnumType_struct*)
_tmp156;if(_tmp187->tag != 14)goto _LL60;else{_tmp188=_tmp187->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp189=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp156;if(_tmp189->tag != 15)goto _LL62;else{_tmp18A=_tmp189->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18A);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp156;if(_tmp18B->tag != 18)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LL63: if(_tmp18F == 0  || Cyc_noexpand_r){
if(_tmp18D != 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp1B3=
_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1B4;_tmp1B4.tag=18;_tmp1B4.f1=_tmp18C;_tmp1B4.f2=0;_tmp1B4.f3=_tmp18E;
_tmp1B4.f4=0;_tmp1B4;});_tmp1B3;});else{return t;}}else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1B6;_tmp1B6.tag=18;_tmp1B6.f1=_tmp18C;_tmp1B6.f2=0;_tmp1B6.f3=_tmp18E;
_tmp1B6.f4=({void**_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=Cyc_Toc_typ_to_c_array(*
_tmp18F);_tmp1B7;});_tmp1B6;});_tmp1B5;});}_LL64: {struct Cyc_Absyn_TagType_struct*
_tmp190=(struct Cyc_Absyn_TagType_struct*)_tmp156;if(_tmp190->tag != 20)goto _LL66;}
_LL65: return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp191=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp156;if(_tmp191->tag != 16)goto
_LL68;else{_tmp192=(void*)_tmp191->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {
struct Cyc_Absyn_DynRgnType_struct*_tmp193=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp156;if(_tmp193->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {
struct Cyc_Absyn_HeapRgn_struct*_tmp194=(struct Cyc_Absyn_HeapRgn_struct*)_tmp156;
if(_tmp194->tag != 21)goto _LL6C;}_LL6B:({void*_tmp1B8=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1B9="Toc::typ_to_c: type translation passed the heap region";
_tag_dyneither(_tmp1B9,sizeof(char),55);}),_tag_dyneither(_tmp1B8,sizeof(void*),
0));});_LL6C: {struct Cyc_Absyn_UniqueRgn_struct*_tmp195=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp156;if(_tmp195->tag != 22)goto _LL6E;}_LL6D:({void*_tmp1BA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BB="Toc::typ_to_c: type translation passed the unique region";
_tag_dyneither(_tmp1BB,sizeof(char),57);}),_tag_dyneither(_tmp1BA,sizeof(void*),
0));});_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp196=(struct Cyc_Absyn_AccessEff_struct*)
_tmp156;if(_tmp196->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp197=(struct Cyc_Absyn_JoinEff_struct*)_tmp156;if(_tmp197->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp198=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp156;if(_tmp198->tag != 25)goto _LL74;}_LL73:({void*_tmp1BC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BD="Toc::typ_to_c: type translation passed an effect";
_tag_dyneither(_tmp1BD,sizeof(char),49);}),_tag_dyneither(_tmp1BC,sizeof(void*),
0));});_LL74: {struct Cyc_Absyn_ValueofType_struct*_tmp199=(struct Cyc_Absyn_ValueofType_struct*)
_tmp156;if(_tmp199->tag != 19)goto _LL3D;}_LL75:({void*_tmp1BE=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BF="Toc::typ_to_c: type translation passed a valueof_t";
_tag_dyneither(_tmp1BF,sizeof(char),51);}),_tag_dyneither(_tmp1BE,sizeof(void*),
0));});_LL3D:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C0=t;struct Cyc_Absyn_ArrayInfo
_tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;_LL8C: {struct Cyc_Absyn_ArrayType_struct*
_tmp1C1=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C0;if(_tmp1C1->tag != 9)goto _LL8E;
else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C2.elt_type;_tmp1C4=_tmp1C2.tq;}}_LL8D:
return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C3,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1C4,Cyc_Absyn_false_conref),e);_LL8E:;_LL8F: return Cyc_Toc_cast_it(t,e);_LL8B:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp1C5=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp1D0;void*_tmp1D1;struct Cyc_Absyn_AggrInfo _tmp1D3;union Cyc_Absyn_AggrInfoU
_tmp1D4;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1D8;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1D9;struct _tuple1 _tmp1DA;struct Cyc_Absyn_Datatypedecl*
_tmp1DB;struct Cyc_Absyn_Datatypefield*_tmp1DC;struct Cyc_List_List*_tmp1DE;_LL91: {
struct Cyc_Absyn_VoidType_struct*_tmp1C6=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C5;if(_tmp1C6->tag != 0)goto _LL93;}_LL92: return 1;_LL93: {struct Cyc_Absyn_VarType_struct*
_tmp1C7=(struct Cyc_Absyn_VarType_struct*)_tmp1C5;if(_tmp1C7->tag != 2)goto _LL95;}
_LL94: return 0;_LL95: {struct Cyc_Absyn_IntType_struct*_tmp1C8=(struct Cyc_Absyn_IntType_struct*)
_tmp1C5;if(_tmp1C8->tag != 6)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_EnumType_struct*
_tmp1C9=(struct Cyc_Absyn_EnumType_struct*)_tmp1C5;if(_tmp1C9->tag != 14)goto _LL99;}
_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CA=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C5;if(_tmp1CA->tag != 15)goto _LL9B;}_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_FloatType_struct*
_tmp1CB=(struct Cyc_Absyn_FloatType_struct*)_tmp1C5;if(_tmp1CB->tag != 7)goto _LL9D;}
_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_DoubleType_struct*_tmp1CC=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C5;if(_tmp1CC->tag != 8)goto _LL9F;}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_FnType_struct*
_tmp1CD=(struct Cyc_Absyn_FnType_struct*)_tmp1C5;if(_tmp1CD->tag != 10)goto _LLA1;}
_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_TagType_struct*_tmp1CE=(struct Cyc_Absyn_TagType_struct*)
_tmp1C5;if(_tmp1CE->tag != 20)goto _LLA3;}_LLA2: return 1;_LLA3: {struct Cyc_Absyn_ArrayType_struct*
_tmp1CF=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C5;if(_tmp1CF->tag != 9)goto _LLA5;
else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1D0.elt_type;}}_LLA4: return Cyc_Toc_atomic_typ(
_tmp1D1);_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp1D2=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C5;if(_tmp1D2->tag != 12)goto _LLA7;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.aggr_info;}}
_LLA6:{union Cyc_Absyn_AggrInfoU _tmp1E3=_tmp1D4;struct _tuple2 _tmp1E4;_LLB8: if((
_tmp1E3.UnknownAggr).tag != 1)goto _LLBA;_tmp1E4=(struct _tuple2)(_tmp1E3.UnknownAggr).val;
_LLB9: return 0;_LLBA:;_LLBB: goto _LLB7;_LLB7:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E5=
Cyc_Absyn_get_known_aggrdecl(_tmp1D4);if(_tmp1E5->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;for(
0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E6->hd)->type))return 0;}}return 1;};_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D5=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C5;if(_tmp1D5->tag != 13)goto
_LLA9;else{_tmp1D6=_tmp1D5->f2;}}_LLA8: for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D6->hd)->type))return 0;}
return 1;_LLA9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1D7=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C5;if(_tmp1D7->tag != 4)goto _LLAB;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.field_info;
if((_tmp1D9.KnownDatatypefield).tag != 2)goto _LLAB;_tmp1DA=(struct _tuple1)(
_tmp1D9.KnownDatatypefield).val;_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;}}_LLAA:
_tmp1DE=_tmp1DC->typs;goto _LLAC;_LLAB: {struct Cyc_Absyn_TupleType_struct*_tmp1DD=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C5;if(_tmp1DD->tag != 11)goto _LLAD;else{
_tmp1DE=_tmp1DD->f1;}}_LLAC: for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1DE->hd)).f2))return 0;}return 1;_LLAD: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1DF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C5;if(_tmp1DF->tag != 3)goto
_LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_PointerType_struct*_tmp1E0=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C5;if(_tmp1E0->tag != 5)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C5;if(_tmp1E1->tag != 17)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E2=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C5;if(_tmp1E2->tag != 16)goto
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6:({struct Cyc_String_pa_struct _tmp1E9;_tmp1E9.tag=
0;_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({void*_tmp1E7[1]={& _tmp1E9};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E8="atomic_typ:  bad type %s";
_tag_dyneither(_tmp1E8,sizeof(char),25);}),_tag_dyneither(_tmp1E7,sizeof(void*),
1));});});_LL90:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1EA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1EC;void*_tmp1ED;_LLBD: {struct Cyc_Absyn_PointerType_struct*
_tmp1EB=(struct Cyc_Absyn_PointerType_struct*)_tmp1EA;if(_tmp1EB->tag != 5)goto
_LLBF;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EC.elt_typ;}}_LLBE: {void*_tmp1EE=
Cyc_Tcutil_compress(_tmp1ED);_LLC2: {struct Cyc_Absyn_VoidType_struct*_tmp1EF=(
struct Cyc_Absyn_VoidType_struct*)_tmp1EE;if(_tmp1EF->tag != 0)goto _LLC4;}_LLC3:
return 1;_LLC4:;_LLC5: return 0;_LLC1:;}_LLBF:;_LLC0: return 0;_LLBC:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp1F0=Cyc_Tcutil_compress(t);_LLC7: {struct Cyc_Absyn_VarType_struct*
_tmp1F1=(struct Cyc_Absyn_VarType_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLC9;}
_LLC8: return 1;_LLC9:;_LLCA: return 0;_LLC6:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp1F2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F4;union Cyc_Absyn_AggrInfoU _tmp1F5;struct Cyc_List_List*_tmp1F7;_LLCC: {
struct Cyc_Absyn_AggrType_struct*_tmp1F3=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F2;if(_tmp1F3->tag != 12)goto _LLCE;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F4.aggr_info;}}
_LLCD: {struct Cyc_Absyn_Aggrdecl*_tmp1F8=Cyc_Absyn_get_known_aggrdecl(_tmp1F5);
if(_tmp1F8->impl == 0)({void*_tmp1F9=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1FA="is_poly_field: type missing fields";
_tag_dyneither(_tmp1FA,sizeof(char),35);}),_tag_dyneither(_tmp1F9,sizeof(void*),
0));});_tmp1F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F8->impl))->fields;
goto _LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F2;if(_tmp1F6->tag != 13)goto _LLD0;else{_tmp1F7=_tmp1F6->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FB=Cyc_Absyn_lookup_field(_tmp1F7,f);if(_tmp1FB == 0)({
struct Cyc_String_pa_struct _tmp1FE;_tmp1FE.tag=0;_tmp1FE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*f);({void*_tmp1FC[1]={& _tmp1FE};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1FD="is_poly_field: bad field %s";
_tag_dyneither(_tmp1FD,sizeof(char),28);}),_tag_dyneither(_tmp1FC,sizeof(void*),
1));});});return Cyc_Toc_is_void_star_or_tvar(_tmp1FB->type);}_LLD0:;_LLD1:({
struct Cyc_String_pa_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1FF[1]={& _tmp201};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp200="is_poly_field: bad type %s";_tag_dyneither(_tmp200,sizeof(
char),27);}),_tag_dyneither(_tmp1FF,sizeof(void*),1));});});_LLCB:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp202=e->r;struct Cyc_Absyn_Exp*_tmp204;struct
_dyneither_ptr*_tmp205;struct Cyc_Absyn_Exp*_tmp207;struct _dyneither_ptr*_tmp208;
_LLD3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp203=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp202;if(_tmp203->tag != 22)goto _LLD5;else{_tmp204=_tmp203->f1;_tmp205=_tmp203->f2;}}
_LLD4: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp204->topt))->v,_tmp205);_LLD5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp206=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp202;if(_tmp206->tag != 23)goto _LLD7;else{
_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;}}_LLD6: {void*_tmp209=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20B;void*_tmp20C;_LLDA: {struct Cyc_Absyn_PointerType_struct*_tmp20A=(struct
Cyc_Absyn_PointerType_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LLDC;else{_tmp20B=
_tmp20A->f1;_tmp20C=_tmp20B.elt_typ;}}_LLDB: return Cyc_Toc_is_poly_field(_tmp20C,
_tmp208);_LLDC:;_LLDD:({struct Cyc_String_pa_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v));({void*_tmp20D[1]={& _tmp20F};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp20E="is_poly_project: bad type %s";_tag_dyneither(_tmp20E,sizeof(
char),29);}),_tag_dyneither(_tmp20D,sizeof(void*),1));});});_LLD9:;}_LLD7:;_LLD8:
return 0;_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp210=
_cycalloc(sizeof(*_tmp210));_tmp210->hd=s;_tmp210->tl=0;_tmp210;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp211=_cycalloc(sizeof(*
_tmp211));_tmp211->hd=s;_tmp211->tl=0;_tmp211;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp212[2];_tmp212[1]=s;_tmp212[0]=
rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp212,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({
struct Cyc_Absyn_Exp*_tmp213[2];_tmp213[1]=n;_tmp213[0]=s;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp213,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*
_tmp214[2];_tmp214[1]=n;_tmp214[0]=s;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp214,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp215[3];_tmp215[2]=n;_tmp215[1]=s;_tmp215[0]=rgn;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp215,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->hd=e;_tmp216->tl=0;_tmp216;}),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**
throw_match_stmt_opt=0;if(throw_match_stmt_opt == 0)throw_match_stmt_opt=({struct
Cyc_Absyn_Stmt**_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);_tmp217;});return*
throw_match_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp218=e->r;union Cyc_Absyn_Cnst _tmp21A;struct _dyneither_ptr _tmp21B;_LLDF: {
struct Cyc_Absyn_Const_e_struct*_tmp219=(struct Cyc_Absyn_Const_e_struct*)_tmp218;
if(_tmp219->tag != 0)goto _LLE1;else{_tmp21A=_tmp219->f1;if((_tmp21A.String_c).tag
!= 7)goto _LLE1;_tmp21B=(struct _dyneither_ptr)(_tmp21A.String_c).val;}}_LLE0:
is_string=1;goto _LLDE;_LLE1:;_LLE2: goto _LLDE;_LLDE:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({
struct Cyc_Absyn_Var_d_struct _tmp21E;_tmp21E.tag=0;_tmp21E.f1=vd;_tmp21E;});
_tmp21D;}),0);_tmp21C->tl=Cyc_Toc_result_decls;_tmp21C;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple15*_tmp21F[3];_tmp21F[2]=({struct _tuple15*_tmp222=_cycalloc(sizeof(*
_tmp222));_tmp222->f1=0;_tmp222->f2=xplussz;_tmp222;});_tmp21F[1]=({struct
_tuple15*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->f1=0;_tmp221->f2=xexp;
_tmp221;});_tmp21F[0]=({struct _tuple15*_tmp220=_cycalloc(sizeof(*_tmp220));
_tmp220->f1=0;_tmp220->f2=xexp;_tmp220;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21F,sizeof(struct _tuple15*),3));}),
0);return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp224;int
_tmp225;struct Cyc_Toc_Env*_tmp223=nv;_tmp224=*_tmp223;_tmp225=_tmp224.toplevel;
return _tmp225;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp227;struct Cyc_Dict_Dict _tmp228;struct
Cyc_Toc_Env*_tmp226=nv;_tmp227=*_tmp226;_tmp228=_tmp227.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp228,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*
_tmp229=_region_malloc(r,sizeof(*_tmp229));_tmp229->break_lab=(struct
_dyneither_ptr**)0;_tmp229->continue_lab=(struct _dyneither_ptr**)0;_tmp229->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp229->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp229->toplevel=(int)1;_tmp229->rgn=(struct _RegionHandle*)
r;_tmp229;});}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp22B;struct _dyneither_ptr**_tmp22C;
struct _dyneither_ptr**_tmp22D;struct Cyc_Toc_FallthruInfo*_tmp22E;struct Cyc_Dict_Dict
_tmp22F;int _tmp230;struct Cyc_Toc_Env*_tmp22A=e;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.break_lab;
_tmp22D=_tmp22B.continue_lab;_tmp22E=_tmp22B.fallthru_info;_tmp22F=_tmp22B.varmap;
_tmp230=_tmp22B.toplevel;return({struct Cyc_Toc_Env*_tmp231=_region_malloc(r,
sizeof(*_tmp231));_tmp231->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp22C);_tmp231->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp22D);_tmp231->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22E;_tmp231->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22F);_tmp231->toplevel=(
int)_tmp230;_tmp231->rgn=(struct _RegionHandle*)r;_tmp231;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp233;struct _dyneither_ptr**_tmp234;struct _dyneither_ptr**_tmp235;struct Cyc_Toc_FallthruInfo*
_tmp236;struct Cyc_Dict_Dict _tmp237;int _tmp238;struct Cyc_Toc_Env*_tmp232=e;
_tmp233=*_tmp232;_tmp234=_tmp233.break_lab;_tmp235=_tmp233.continue_lab;_tmp236=
_tmp233.fallthru_info;_tmp237=_tmp233.varmap;_tmp238=_tmp233.toplevel;return({
struct Cyc_Toc_Env*_tmp239=_region_malloc(r,sizeof(*_tmp239));_tmp239->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp234);_tmp239->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp235);_tmp239->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp236;_tmp239->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp237);_tmp239->toplevel=(int)0;_tmp239->rgn=(struct _RegionHandle*)r;_tmp239;});}
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23B;struct _dyneither_ptr**_tmp23C;struct _dyneither_ptr**
_tmp23D;struct Cyc_Toc_FallthruInfo*_tmp23E;struct Cyc_Dict_Dict _tmp23F;int _tmp240;
struct Cyc_Toc_Env*_tmp23A=e;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.break_lab;_tmp23D=
_tmp23B.continue_lab;_tmp23E=_tmp23B.fallthru_info;_tmp23F=_tmp23B.varmap;
_tmp240=_tmp23B.toplevel;return({struct Cyc_Toc_Env*_tmp241=_region_malloc(r,
sizeof(*_tmp241));_tmp241->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp23C);_tmp241->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp23D);_tmp241->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23E;_tmp241->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23F);_tmp241->toplevel=(
int)1;_tmp241->rgn=(struct _RegionHandle*)r;_tmp241;});}static struct Cyc_Toc_Env*
Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,
struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp242=(*x).f1;struct Cyc_List_List*
_tmp243;_LLE4: if((_tmp242.Rel_n).tag != 1)goto _LLE6;_tmp243=(struct Cyc_List_List*)(
_tmp242.Rel_n).val;_LLE5:({struct Cyc_String_pa_struct _tmp246;_tmp246.tag=0;
_tmp246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x));({void*_tmp244[1]={& _tmp246};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp245="Toc::add_varmap on Rel_n: %s\n";
_tag_dyneither(_tmp245,sizeof(char),30);}),_tag_dyneither(_tmp244,sizeof(void*),
1));});});_LLE6:;_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp248;struct
_dyneither_ptr**_tmp249;struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*
_tmp24B;struct Cyc_Dict_Dict _tmp24C;int _tmp24D;struct Cyc_Toc_Env*_tmp247=e;
_tmp248=*_tmp247;_tmp249=_tmp248.break_lab;_tmp24A=_tmp248.continue_lab;_tmp24B=
_tmp248.fallthru_info;_tmp24C=_tmp248.varmap;_tmp24D=_tmp248.toplevel;{struct Cyc_Dict_Dict
_tmp24E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp24C),x,y);return({struct Cyc_Toc_Env*_tmp24F=
_region_malloc(r,sizeof(*_tmp24F));_tmp24F->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp249);_tmp24F->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24A);_tmp24F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24B;_tmp24F->varmap=(struct Cyc_Dict_Dict)_tmp24E;_tmp24F->toplevel=(int)
_tmp24D;_tmp24F->rgn=(struct _RegionHandle*)r;_tmp24F;});};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp251;struct _dyneither_ptr**_tmp252;struct _dyneither_ptr**_tmp253;struct Cyc_Toc_FallthruInfo*
_tmp254;struct Cyc_Dict_Dict _tmp255;int _tmp256;struct Cyc_Toc_Env*_tmp250=e;
_tmp251=*_tmp250;_tmp252=_tmp251.break_lab;_tmp253=_tmp251.continue_lab;_tmp254=
_tmp251.fallthru_info;_tmp255=_tmp251.varmap;_tmp256=_tmp251.toplevel;return({
struct Cyc_Toc_Env*_tmp257=_region_malloc(r,sizeof(*_tmp257));_tmp257->break_lab=(
struct _dyneither_ptr**)0;_tmp257->continue_lab=(struct _dyneither_ptr**)0;_tmp257->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp254;_tmp257->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp255);_tmp257->toplevel=(int)_tmp256;_tmp257->rgn=(struct _RegionHandle*)r;
_tmp257;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp258=_region_malloc(r,sizeof(*_tmp258));_tmp258->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp258->tl=fallthru_vars;_tmp258;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp25A;struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**
_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;
struct Cyc_Toc_Env*_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;_tmp25C=
_tmp25A.continue_lab;_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;
_tmp25F=_tmp25A.toplevel;{struct Cyc_Toc_Env _tmp261;struct Cyc_Dict_Dict _tmp262;
struct Cyc_Toc_Env*_tmp260=next_case_env;_tmp261=*_tmp260;_tmp262=_tmp261.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp265=
_region_malloc(r,sizeof(*_tmp265));_tmp265->label=fallthru_l;_tmp265->binders=
fallthru_vars;_tmp265->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262);_tmp265;});
return({struct Cyc_Toc_Env*_tmp263=_region_malloc(r,sizeof(*_tmp263));_tmp263->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp264=_region_malloc(r,sizeof(*
_tmp264));_tmp264[0]=break_l;_tmp264;});_tmp263->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp25C);_tmp263->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp263->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E);_tmp263->toplevel=(
int)_tmp25F;_tmp263->rgn=(struct _RegionHandle*)r;_tmp263;});};};};}static struct
Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp267;struct _dyneither_ptr**
_tmp268;struct _dyneither_ptr**_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct
Cyc_Dict_Dict _tmp26B;int _tmp26C;struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;
_tmp268=_tmp267.break_lab;_tmp269=_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;
_tmp26B=_tmp267.varmap;_tmp26C=_tmp267.toplevel;return({struct Cyc_Toc_Env*
_tmp26D=_region_malloc(r,sizeof(*_tmp26D));_tmp26D->break_lab=(struct
_dyneither_ptr**)({struct _dyneither_ptr**_tmp26E=_region_malloc(r,sizeof(*
_tmp26E));_tmp26E[0]=break_l;_tmp26E;});_tmp26D->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp269);_tmp26D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp26D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26B);_tmp26D->toplevel=(
int)_tmp26C;_tmp26D->rgn=(struct _RegionHandle*)r;_tmp26D;});}static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_dyneither_ptr*next_l){struct Cyc_Toc_Env _tmp270;struct _dyneither_ptr**_tmp271;
struct _dyneither_ptr**_tmp272;struct Cyc_Toc_FallthruInfo*_tmp273;struct Cyc_Dict_Dict
_tmp274;int _tmp275;struct Cyc_Toc_Env*_tmp26F=e;_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;
_tmp272=_tmp270.continue_lab;_tmp273=_tmp270.fallthru_info;_tmp274=_tmp270.varmap;
_tmp275=_tmp270.toplevel;return({struct Cyc_Toc_Env*_tmp276=_region_malloc(r,
sizeof(*_tmp276));_tmp276->break_lab=(struct _dyneither_ptr**)0;_tmp276->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp272);_tmp276->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp277=_region_malloc(
r,sizeof(*_tmp277));_tmp277->label=next_l;_tmp277->binders=0;_tmp277->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp277;});_tmp276->varmap=(
struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp274);_tmp276->toplevel=(int)_tmp275;_tmp276->rgn=(struct
_RegionHandle*)r;_tmp276;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*
_tmp278=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp279=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp278;if(_tmp279->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp27A=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp278;if(_tmp27A->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27B=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp278;if(_tmp27B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:({void*_tmp27E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp27F="dereference of NULL pointer";
_tag_dyneither(_tmp27F,sizeof(char),28);}),_tag_dyneither(_tmp27E,sizeof(void*),
0));});return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp27C=(struct Cyc_Absyn_EmptyAnnot_struct*)
_tmp278;if(_tmp27C->tag != Cyc_Absyn_EmptyAnnot)goto _LLF1;}_LLF0: return 0;_LLF1: {
struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27D=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp278;if(_tmp27D->tag != Cyc_CfFlowInfo_HasTagCmps)goto _LLF3;}_LLF2:({void*
_tmp280=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp281="compiler oddity: pointer compared to tag type";
_tag_dyneither(_tmp281,sizeof(char),46);}),_tag_dyneither(_tmp280,sizeof(void*),
0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF3:;_LLF4:({void*_tmp282=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp283="need_null_check";
_tag_dyneither(_tmp283,sizeof(char),16);}),_tag_dyneither(_tmp282,sizeof(void*),
0));});_LLE8:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp284=e->annot;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;
_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF8;else{_tmp286=_tmp285->f1;}}
_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp287=(struct
Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:({void*_tmp28C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp28D="dereference of NULL pointer";
_tag_dyneither(_tmp28D,sizeof(char),28);}),_tag_dyneither(_tmp28C,sizeof(void*),
0));});return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp28A=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp284;if(_tmp28A->tag != Cyc_CfFlowInfo_HasTagCmps)goto _LLFE;}_LLFD: goto _LLFF;
_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28B=(struct Cyc_Absyn_EmptyAnnot_struct*)
_tmp284;if(_tmp28B->tag != Cyc_Absyn_EmptyAnnot)goto _LL100;}_LLFF: return 0;_LL100:;
_LL101:({void*_tmp28E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp28F="get_relns";_tag_dyneither(_tmp28F,
sizeof(char),10);}),_tag_dyneither(_tmp28E,sizeof(void*),0));});_LLF5:;}static
int Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp290=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp292;struct Cyc_Absyn_PtrAtts _tmp293;union Cyc_Absyn_Constraint*
_tmp294;union Cyc_Absyn_Constraint*_tmp295;struct Cyc_Absyn_ArrayInfo _tmp297;
struct Cyc_Absyn_Exp*_tmp298;_LL103: {struct Cyc_Absyn_PointerType_struct*_tmp291=(
struct Cyc_Absyn_PointerType_struct*)_tmp290;if(_tmp291->tag != 5)goto _LL105;else{
_tmp292=_tmp291->f1;_tmp293=_tmp292.ptr_atts;_tmp294=_tmp293.bounds;_tmp295=
_tmp293.zero_term;}}_LL104: {void*_tmp299=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp294);struct Cyc_Absyn_Exp*
_tmp29C;_LL10A: {struct Cyc_Absyn_DynEither_b_struct*_tmp29A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp299;if(_tmp29A->tag != 0)goto _LL10C;}_LL10B: return 0;_LL10C: {struct Cyc_Absyn_Upper_b_struct*
_tmp29B=(struct Cyc_Absyn_Upper_b_struct*)_tmp299;if(_tmp29B->tag != 1)goto _LL109;
else{_tmp29C=_tmp29B->f1;}}_LL10D: {unsigned int _tmp29E;int _tmp29F;struct
_tuple10 _tmp29D=Cyc_Evexp_eval_const_uint_exp(_tmp29C);_tmp29E=_tmp29D.f1;
_tmp29F=_tmp29D.f2;return _tmp29F  && i <= _tmp29E;}_LL109:;}_LL105: {struct Cyc_Absyn_ArrayType_struct*
_tmp296=(struct Cyc_Absyn_ArrayType_struct*)_tmp290;if(_tmp296->tag != 9)goto
_LL107;else{_tmp297=_tmp296->f1;_tmp298=_tmp297.num_elts;}}_LL106: if(_tmp298 == 0)
return 0;{unsigned int _tmp2A1;int _tmp2A2;struct _tuple10 _tmp2A0=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp298);_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;return
_tmp2A2  && i <= _tmp2A1;};_LL107:;_LL108: return 0;_LL102:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp2A3=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp2A3->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2A4=_tmp2A3->rop;
struct Cyc_Absyn_Vardecl*_tmp2A5;struct Cyc_Absyn_Vardecl*_tmp2A6;_LL10F: if((
_tmp2A4.LessNumelts).tag != 3)goto _LL111;_tmp2A5=(struct Cyc_Absyn_Vardecl*)(
_tmp2A4.LessNumelts).val;_LL110: _tmp2A6=_tmp2A5;goto _LL112;_LL111: if((_tmp2A4.LessEqNumelts).tag
!= 5)goto _LL113;_tmp2A6=(struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessEqNumelts).val;
_LL112: if(_tmp2A6 == v)return 1;else{goto _LL10E;}_LL113:;_LL114: continue;_LL10E:;};}
return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp2A7=e->r;void*_tmp2A9;struct Cyc_Absyn_Vardecl*
_tmp2AB;void*_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AF;void*_tmp2B1;struct Cyc_Absyn_Vardecl*
_tmp2B3;void*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B7;enum Cyc_Absyn_Primop
_tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List _tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2A8->tag != 1)goto _LL118;else{_tmp2A9=(void*)_tmp2A8->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2AA=(struct Cyc_Absyn_Pat_b_struct*)_tmp2A9;if(_tmp2AA->tag
!= 5)goto _LL118;else{_tmp2AB=_tmp2AA->f1;}};}}_LL117: _tmp2AF=_tmp2AB;goto _LL119;
_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AC=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2AC->tag != 1)goto _LL11A;else{_tmp2AD=(void*)_tmp2AC->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2AE=(struct Cyc_Absyn_Local_b_struct*)_tmp2AD;if(
_tmp2AE->tag != 4)goto _LL11A;else{_tmp2AF=_tmp2AE->f1;}};}}_LL119: _tmp2B3=_tmp2AF;
goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B0=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2B0->tag != 1)goto _LL11C;else{_tmp2B1=(void*)_tmp2B0->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2B2=(struct Cyc_Absyn_Global_b_struct*)_tmp2B1;if(
_tmp2B2->tag != 1)goto _LL11C;else{_tmp2B3=_tmp2B2->f1;}};}}_LL11B: _tmp2B7=_tmp2B3;
goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2B4=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2B4->tag != 1)goto _LL11E;else{_tmp2B5=(void*)_tmp2B4->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2B6=(struct Cyc_Absyn_Param_b_struct*)_tmp2B5;if(
_tmp2B6->tag != 3)goto _LL11E;else{_tmp2B7=_tmp2B6->f1;}};}}_LL11D: if(_tmp2B7->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B7))return 1;goto _LL115;_LL11E: {
struct Cyc_Absyn_Primop_e_struct*_tmp2B8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A7;if(_tmp2B8->tag != 3)goto _LL120;else{_tmp2B9=_tmp2B8->f1;if(_tmp2B9 != Cyc_Absyn_Numelts)
goto _LL120;_tmp2BA=_tmp2B8->f2;if(_tmp2BA == 0)goto _LL120;_tmp2BB=*_tmp2BA;
_tmp2BC=(struct Cyc_Absyn_Exp*)_tmp2BB.hd;}}_LL11F:{void*_tmp2BD=_tmp2BC->r;void*
_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C1;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C5;void*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2C9;void*_tmp2CB;struct Cyc_Absyn_Vardecl*
_tmp2CD;_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2BE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2BE->tag != 1)goto _LL125;else{_tmp2BF=(void*)_tmp2BE->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2C0=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BF;if(_tmp2C0->tag
!= 5)goto _LL125;else{_tmp2C1=_tmp2C0->f1;}};}}_LL124: _tmp2C5=_tmp2C1;goto _LL126;
_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C2->tag != 1)goto _LL127;else{_tmp2C3=(void*)_tmp2C2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C4=(struct Cyc_Absyn_Local_b_struct*)_tmp2C3;if(
_tmp2C4->tag != 4)goto _LL127;else{_tmp2C5=_tmp2C4->f1;}};}}_LL126: _tmp2C9=_tmp2C5;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C6->tag != 1)goto _LL129;else{_tmp2C7=(void*)_tmp2C6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C8=(struct Cyc_Absyn_Global_b_struct*)_tmp2C7;if(
_tmp2C8->tag != 1)goto _LL129;else{_tmp2C9=_tmp2C8->f1;}};}}_LL128: _tmp2CD=_tmp2C9;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2CA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2CA->tag != 1)goto _LL12B;else{_tmp2CB=(void*)_tmp2CA->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_struct*)_tmp2CB;if(
_tmp2CC->tag != 3)goto _LL12B;else{_tmp2CD=_tmp2CC->f1;}};}}_LL12A: return _tmp2CD == 
v;_LL12B:;_LL12C: goto _LL122;_LL122:;}goto _LL115;_LL120:;_LL121: goto _LL115;_LL115:;}
return 0;}static char _tmp2DF[8]="*bogus*";struct _tuple17{void*f1;void*f2;};static
int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*_tmp2CE=a->r;void*
_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Vardecl*
_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*
_tmp2DE;_LL12E: {struct Cyc_Absyn_Var_e_struct*_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2CF->tag != 1)goto _LL130;else{_tmp2D0=(void*)_tmp2CF->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2D0;if(_tmp2D1->tag
!= 5)goto _LL130;else{_tmp2D2=_tmp2D1->f1;}};}}_LL12F: _tmp2D6=_tmp2D2;goto _LL131;
_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D3->tag != 1)goto _LL132;else{_tmp2D4=(void*)_tmp2D3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)_tmp2D4;if(
_tmp2D5->tag != 4)goto _LL132;else{_tmp2D6=_tmp2D5->f1;}};}}_LL131: _tmp2DA=_tmp2D6;
goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D7->tag != 1)goto _LL134;else{_tmp2D8=(void*)_tmp2D7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)_tmp2D8;if(
_tmp2D9->tag != 1)goto _LL134;else{_tmp2DA=_tmp2D9->f1;}};}}_LL133: _tmp2DE=_tmp2DA;
goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2DB->tag != 1)goto _LL136;else{_tmp2DC=(void*)_tmp2DB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)_tmp2DC;if(
_tmp2DD->tag != 3)goto _LL136;else{_tmp2DE=_tmp2DD->f1;}};}}_LL135: if(_tmp2DE->escapes)
goto _LL137;x=_tmp2DE;goto _LL12D;_LL136:;_LL137: {static struct _dyneither_ptr
bogus_string={_tmp2DF,_tmp2DF,_tmp2DF + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL12D:;}inner_loop: {void*_tmp2E0=i->r;void*_tmp2E2;struct Cyc_Absyn_Exp*
_tmp2E3;union Cyc_Absyn_Cnst _tmp2E5;struct _tuple5 _tmp2E6;enum Cyc_Absyn_Sign
_tmp2E7;int _tmp2E8;union Cyc_Absyn_Cnst _tmp2EA;struct _tuple5 _tmp2EB;enum Cyc_Absyn_Sign
_tmp2EC;int _tmp2ED;union Cyc_Absyn_Cnst _tmp2EF;struct _tuple5 _tmp2F0;enum Cyc_Absyn_Sign
_tmp2F1;int _tmp2F2;enum Cyc_Absyn_Primop _tmp2F4;struct Cyc_List_List*_tmp2F5;
struct Cyc_List_List _tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_List_List*
_tmp2F8;struct Cyc_List_List _tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;void*_tmp2FC;
struct Cyc_Absyn_Vardecl*_tmp2FE;void*_tmp300;struct Cyc_Absyn_Vardecl*_tmp302;
void*_tmp304;struct Cyc_Absyn_Vardecl*_tmp306;void*_tmp308;struct Cyc_Absyn_Vardecl*
_tmp30A;_LL139: {struct Cyc_Absyn_Cast_e_struct*_tmp2E1=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2E0;if(_tmp2E1->tag != 15)goto _LL13B;else{_tmp2E2=(void*)_tmp2E1->f1;_tmp2E3=
_tmp2E1->f2;}}_LL13A: i=_tmp2E3;goto inner_loop;_LL13B: {struct Cyc_Absyn_Const_e_struct*
_tmp2E4=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2E4->tag != 0)goto _LL13D;
else{_tmp2E5=_tmp2E4->f1;if((_tmp2E5.Int_c).tag != 4)goto _LL13D;_tmp2E6=(struct
_tuple5)(_tmp2E5.Int_c).val;_tmp2E7=_tmp2E6.f1;if(_tmp2E7 != Cyc_Absyn_None)goto
_LL13D;_tmp2E8=_tmp2E6.f2;}}_LL13C: _tmp2ED=_tmp2E8;goto _LL13E;_LL13D: {struct Cyc_Absyn_Const_e_struct*
_tmp2E9=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2E9->tag != 0)goto _LL13F;
else{_tmp2EA=_tmp2E9->f1;if((_tmp2EA.Int_c).tag != 4)goto _LL13F;_tmp2EB=(struct
_tuple5)(_tmp2EA.Int_c).val;_tmp2EC=_tmp2EB.f1;if(_tmp2EC != Cyc_Absyn_Signed)
goto _LL13F;_tmp2ED=_tmp2EB.f2;}}_LL13E: return _tmp2ED >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2ED + 1),x->type);_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2EE=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2EE->tag != 0)goto _LL141;
else{_tmp2EF=_tmp2EE->f1;if((_tmp2EF.Int_c).tag != 4)goto _LL141;_tmp2F0=(struct
_tuple5)(_tmp2EF.Int_c).val;_tmp2F1=_tmp2F0.f1;if(_tmp2F1 != Cyc_Absyn_Unsigned)
goto _LL141;_tmp2F2=_tmp2F0.f2;}}_LL140: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2F2 + 1),x->type);_LL141: {struct Cyc_Absyn_Primop_e_struct*
_tmp2F3=(struct Cyc_Absyn_Primop_e_struct*)_tmp2E0;if(_tmp2F3->tag != 3)goto _LL143;
else{_tmp2F4=_tmp2F3->f1;if(_tmp2F4 != Cyc_Absyn_Mod)goto _LL143;_tmp2F5=_tmp2F3->f2;
if(_tmp2F5 == 0)goto _LL143;_tmp2F6=*_tmp2F5;_tmp2F7=(struct Cyc_Absyn_Exp*)_tmp2F6.hd;
_tmp2F8=_tmp2F6.tl;if(_tmp2F8 == 0)goto _LL143;_tmp2F9=*_tmp2F8;_tmp2FA=(struct Cyc_Absyn_Exp*)
_tmp2F9.hd;}}_LL142: return Cyc_Toc_check_leq_size(relns,x,_tmp2FA);_LL143: {
struct Cyc_Absyn_Var_e_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(
_tmp2FB->tag != 1)goto _LL145;else{_tmp2FC=(void*)_tmp2FB->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2FD=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FC;if(_tmp2FD->tag != 5)goto _LL145;
else{_tmp2FE=_tmp2FD->f1;}};}}_LL144: _tmp302=_tmp2FE;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp2FF=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2FF->tag
!= 1)goto _LL147;else{_tmp300=(void*)_tmp2FF->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp301=(struct Cyc_Absyn_Local_b_struct*)_tmp300;if(_tmp301->tag != 4)goto _LL147;
else{_tmp302=_tmp301->f1;}};}}_LL146: _tmp306=_tmp302;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp303=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp303->tag
!= 1)goto _LL149;else{_tmp304=(void*)_tmp303->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp305=(struct Cyc_Absyn_Global_b_struct*)_tmp304;if(_tmp305->tag != 1)goto _LL149;
else{_tmp306=_tmp305->f1;}};}}_LL148: _tmp30A=_tmp306;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp307=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp307->tag
!= 1)goto _LL14B;else{_tmp308=(void*)_tmp307->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp309=(struct Cyc_Absyn_Param_b_struct*)_tmp308;if(_tmp309->tag != 3)goto _LL14B;
else{_tmp30A=_tmp309->f1;}};}}_LL14A: if(_tmp30A->escapes)return 0;{struct Cyc_List_List*
_tmp30B=relns;for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30C=(struct Cyc_CfFlowInfo_Reln*)_tmp30B->hd;if(_tmp30C->vd == _tmp30A){union
Cyc_CfFlowInfo_RelnOp _tmp30D=_tmp30C->rop;struct Cyc_Absyn_Vardecl*_tmp30E;struct
_tuple11 _tmp30F;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp311;unsigned int _tmp312;
_LL14E: if((_tmp30D.LessNumelts).tag != 3)goto _LL150;_tmp30E=(struct Cyc_Absyn_Vardecl*)(
_tmp30D.LessNumelts).val;_LL14F: if(x == _tmp30E)return 1;else{goto _LL14D;}_LL150:
if((_tmp30D.LessVar).tag != 2)goto _LL152;_tmp30F=(struct _tuple11)(_tmp30D.LessVar).val;
_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_LL151:{struct _tuple17 _tmp314=({struct
_tuple17 _tmp313;_tmp313.f1=Cyc_Tcutil_compress(_tmp311);_tmp313.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v);_tmp313;});void*_tmp315;
void*_tmp317;void*_tmp318;struct Cyc_Absyn_PtrInfo _tmp31A;struct Cyc_Absyn_PtrAtts
_tmp31B;union Cyc_Absyn_Constraint*_tmp31C;_LL157: _tmp315=_tmp314.f1;{struct Cyc_Absyn_TagType_struct*
_tmp316=(struct Cyc_Absyn_TagType_struct*)_tmp315;if(_tmp316->tag != 20)goto _LL159;
else{_tmp317=(void*)_tmp316->f1;}};_tmp318=_tmp314.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp319=(struct Cyc_Absyn_PointerType_struct*)_tmp318;if(_tmp319->tag != 5)goto
_LL159;else{_tmp31A=_tmp319->f1;_tmp31B=_tmp31A.ptr_atts;_tmp31C=_tmp31B.bounds;}};
_LL158:{void*_tmp31D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp31C);struct Cyc_Absyn_Exp*_tmp31F;_LL15C: {struct Cyc_Absyn_Upper_b_struct*
_tmp31E=(struct Cyc_Absyn_Upper_b_struct*)_tmp31D;if(_tmp31E->tag != 1)goto _LL15E;
else{_tmp31F=_tmp31E->f1;}}_LL15D: {struct Cyc_Absyn_Exp*_tmp320=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp317,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp320,_tmp31F))return 1;goto _LL15B;}_LL15E:;_LL15F:
goto _LL15B;_LL15B:;}goto _LL156;_LL159:;_LL15A: goto _LL156;_LL156:;}{struct Cyc_List_List*
_tmp321=relns;for(0;_tmp321 != 0;_tmp321=_tmp321->tl){struct Cyc_CfFlowInfo_Reln*
_tmp322=(struct Cyc_CfFlowInfo_Reln*)_tmp321->hd;if(_tmp322->vd == _tmp310){union
Cyc_CfFlowInfo_RelnOp _tmp323=_tmp322->rop;struct Cyc_Absyn_Vardecl*_tmp324;struct
Cyc_Absyn_Vardecl*_tmp325;unsigned int _tmp326;struct _tuple11 _tmp327;struct Cyc_Absyn_Vardecl*
_tmp328;_LL161: if((_tmp323.LessEqNumelts).tag != 5)goto _LL163;_tmp324=(struct Cyc_Absyn_Vardecl*)(
_tmp323.LessEqNumelts).val;_LL162: _tmp325=_tmp324;goto _LL164;_LL163: if((_tmp323.LessNumelts).tag
!= 3)goto _LL165;_tmp325=(struct Cyc_Absyn_Vardecl*)(_tmp323.LessNumelts).val;
_LL164: if(x == _tmp325)return 1;goto _LL160;_LL165: if((_tmp323.EqualConst).tag != 1)
goto _LL167;_tmp326=(unsigned int)(_tmp323.EqualConst).val;_LL166: return Cyc_Toc_check_const_array(
_tmp326,x->type);_LL167: if((_tmp323.LessVar).tag != 2)goto _LL169;_tmp327=(struct
_tuple11)(_tmp323.LessVar).val;_tmp328=_tmp327.f1;_LL168: if(Cyc_Toc_check_leq_size_var(
relns,x,_tmp328))return 1;goto _LL160;_LL169:;_LL16A: goto _LL160;_LL160:;}}}goto
_LL14D;_LL152: if((_tmp30D.LessConst).tag != 4)goto _LL154;_tmp312=(unsigned int)(
_tmp30D.LessConst).val;_LL153: return Cyc_Toc_check_const_array(_tmp312,x->type);
_LL154:;_LL155: goto _LL14D;_LL14D:;}}}goto _LL138;_LL14B:;_LL14C: goto _LL138;_LL138:;}
return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({
void*_tmp329=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp32A="Missing type in primop ";_tag_dyneither(_tmp32A,sizeof(char),
24);}),_tag_dyneither(_tmp329,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp32B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32C="Missing type in primop ";
_tag_dyneither(_tmp32C,sizeof(char),24);}),_tag_dyneither(_tmp32B,sizeof(void*),
0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple9*_tmp32D=
_cycalloc(sizeof(*_tmp32D));_tmp32D->f1=Cyc_Toc_mt_tq;_tmp32D->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp32D;});}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple15*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->f1=0;
_tmp32E->f2=e;_tmp32E;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp32F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp32F): Cyc_Toc_malloc_ptr(_tmp32F));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp32F);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp330=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp330 != 0;_tmp330=_tmp330->tl){struct _tuple15 _tmp332;struct Cyc_List_List*
_tmp333;struct Cyc_Absyn_Exp*_tmp334;struct _tuple15*_tmp331=(struct _tuple15*)
_tmp330->hd;_tmp332=*_tmp331;_tmp333=_tmp332.f1;_tmp334=_tmp332.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp333 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp333->tl != 0)({void*_tmp335=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp336="multiple designators in array";
_tag_dyneither(_tmp336,sizeof(char),30);}),_tag_dyneither(_tmp335,sizeof(void*),
0));});{void*_tmp337=(void*)_tmp333->hd;void*_tmp338=_tmp337;struct Cyc_Absyn_Exp*
_tmp33A;_LL16C: {struct Cyc_Absyn_ArrayElement_struct*_tmp339=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp338;if(_tmp339->tag != 0)goto _LL16E;else{_tmp33A=_tmp339->f1;}}_LL16D: Cyc_Toc_exp_to_c(
nv,_tmp33A);e_index=_tmp33A;goto _LL16B;_LL16E: {struct Cyc_Absyn_FieldName_struct*
_tmp33B=(struct Cyc_Absyn_FieldName_struct*)_tmp338;if(_tmp33B->tag != 1)goto
_LL16B;}_LL16F:({void*_tmp33C=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp33D="field name designators in array";
_tag_dyneither(_tmp33D,sizeof(char),32);}),_tag_dyneither(_tmp33C,sizeof(void*),
0));});_LL16B:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,
0);void*_tmp33E=_tmp334->r;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Vardecl*
_tmp342;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp344;int _tmp345;void*
_tmp347;struct Cyc_List_List*_tmp348;_LL171: {struct Cyc_Absyn_Array_e_struct*
_tmp33F=(struct Cyc_Absyn_Array_e_struct*)_tmp33E;if(_tmp33F->tag != 27)goto _LL173;
else{_tmp340=_tmp33F->f1;}}_LL172: s=Cyc_Toc_init_array(nv,lval,_tmp340,s);goto
_LL170;_LL173: {struct Cyc_Absyn_Comprehension_e_struct*_tmp341=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp33E;if(_tmp341->tag != 28)goto _LL175;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;
_tmp344=_tmp341->f3;_tmp345=_tmp341->f4;}}_LL174: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp342,_tmp343,_tmp344,_tmp345,s,0);goto _LL170;_LL175: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp346=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp33E;if(_tmp346->tag != 30)goto
_LL177;else{_tmp347=(void*)_tmp346->f1;_tmp348=_tmp346->f2;}}_LL176: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp347,_tmp348,s);goto _LL170;_LL177:;_LL178: Cyc_Toc_exp_to_c(nv,_tmp334);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp334,0),s,0);goto _LL170;_LL170:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp349=vd->name;void*_tmp34A=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34C;struct _RegionHandle*_tmp34D;
struct Cyc_Toc_Env*_tmp34B=nv;_tmp34C=*_tmp34B;_tmp34D=_tmp34C.rgn;{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(_tmp34D,nv,_tmp349,Cyc_Absyn_varb_exp(_tmp349,(void*)({
struct Cyc_Absyn_Local_b_struct*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359[0]=({
struct Cyc_Absyn_Local_b_struct _tmp35A;_tmp35A.tag=4;_tmp35A.f1=vd;_tmp35A;});
_tmp359;}),0));struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp349,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp349,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp349,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp349,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp34E=e2->r;struct Cyc_List_List*_tmp350;
struct Cyc_Absyn_Vardecl*_tmp352;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*
_tmp354;int _tmp355;void*_tmp357;struct Cyc_List_List*_tmp358;_LL17A: {struct Cyc_Absyn_Array_e_struct*
_tmp34F=(struct Cyc_Absyn_Array_e_struct*)_tmp34E;if(_tmp34F->tag != 27)goto _LL17C;
else{_tmp350=_tmp34F->f1;}}_LL17B: body=Cyc_Toc_init_array(nv2,lval,_tmp350,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL17C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp351=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34E;if(_tmp351->tag != 28)goto _LL17E;else{_tmp352=_tmp351->f1;_tmp353=_tmp351->f2;
_tmp354=_tmp351->f3;_tmp355=_tmp351->f4;}}_LL17D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp352,_tmp353,_tmp354,_tmp355,Cyc_Toc_skip_stmt_dl(),0);goto _LL179;
_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp356=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34E;if(_tmp356->tag != 30)goto _LL180;else{_tmp357=(void*)_tmp356->f1;_tmp358=
_tmp356->f2;}}_LL17F: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp357,_tmp358,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL180:;_LL181: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL179;_LL179:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp34A,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp349,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp35B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp35B != 0;_tmp35B=_tmp35B->tl){struct _tuple15 _tmp35D;
struct Cyc_List_List*_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct _tuple15*_tmp35C=(
struct _tuple15*)_tmp35B->hd;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;
if(_tmp35E == 0)({void*_tmp360=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp361="empty designator list";
_tag_dyneither(_tmp361,sizeof(char),22);}),_tag_dyneither(_tmp360,sizeof(void*),
0));});if(_tmp35E->tl != 0)({void*_tmp362=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp363="too many designators in anonymous struct";
_tag_dyneither(_tmp363,sizeof(char),41);}),_tag_dyneither(_tmp362,sizeof(void*),
0));});{void*_tmp364=(void*)_tmp35E->hd;struct _dyneither_ptr*_tmp366;_LL183: {
struct Cyc_Absyn_FieldName_struct*_tmp365=(struct Cyc_Absyn_FieldName_struct*)
_tmp364;if(_tmp365->tag != 1)goto _LL185;else{_tmp366=_tmp365->f1;}}_LL184: {
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp366,0);{void*_tmp367=
_tmp35F->r;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;int _tmp36E;void*_tmp370;struct Cyc_List_List*
_tmp371;_LL188: {struct Cyc_Absyn_Array_e_struct*_tmp368=(struct Cyc_Absyn_Array_e_struct*)
_tmp367;if(_tmp368->tag != 27)goto _LL18A;else{_tmp369=_tmp368->f1;}}_LL189: s=Cyc_Toc_init_array(
nv,lval,_tmp369,s);goto _LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp36A=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp367;if(_tmp36A->tag != 28)
goto _LL18C;else{_tmp36B=_tmp36A->f1;_tmp36C=_tmp36A->f2;_tmp36D=_tmp36A->f3;
_tmp36E=_tmp36A->f4;}}_LL18B: s=Cyc_Toc_init_comprehension(nv,lval,_tmp36B,
_tmp36C,_tmp36D,_tmp36E,s,0);goto _LL187;_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp36F=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp367;if(_tmp36F->tag != 30)goto
_LL18E;else{_tmp370=(void*)_tmp36F->f1;_tmp371=_tmp36F->f2;}}_LL18D: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp370,_tmp371,s);goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv,_tmp35F);
if(Cyc_Toc_is_poly_field(struct_type,_tmp366))_tmp35F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35F);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp35F,0),0),s,0);goto _LL187;_LL187:;}goto _LL182;}_LL185:;_LL186:({void*_tmp372=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp373="array designator in struct";_tag_dyneither(_tmp373,sizeof(
char),27);}),_tag_dyneither(_tmp372,sizeof(void*),0));});_LL182:;};}}return s;}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,
struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp375;
struct _RegionHandle*_tmp376;struct Cyc_Toc_Env*_tmp374=nv;_tmp375=*_tmp374;
_tmp376=_tmp375.rgn;{struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp376,Cyc_Toc_tup_to_c,es);void*_tmp378=Cyc_Toc_add_tuple_type(
_tmp377);struct _tuple0*_tmp379=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp37A=
Cyc_Absyn_var_exp(_tmp379,0);struct Cyc_Absyn_Stmt*_tmp37B=Cyc_Absyn_exp_stmt(
_tmp37A,0);struct Cyc_Absyn_Exp*(*_tmp37C)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp376,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp37D);for(0;_tmp37D != 0;(_tmp37D=
_tmp37D->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37D->hd;struct
Cyc_Absyn_Exp*lval=_tmp37C(_tmp37A,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp37E=e->r;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Vardecl*_tmp382;
struct Cyc_Absyn_Exp*_tmp383;struct Cyc_Absyn_Exp*_tmp384;int _tmp385;_LL191: {
struct Cyc_Absyn_Array_e_struct*_tmp37F=(struct Cyc_Absyn_Array_e_struct*)_tmp37E;
if(_tmp37F->tag != 27)goto _LL193;else{_tmp380=_tmp37F->f1;}}_LL192: _tmp37B=Cyc_Toc_init_array(
nv,lval,_tmp380,_tmp37B);goto _LL190;_LL193: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp381=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp37E;if(_tmp381->tag != 28)
goto _LL195;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;_tmp384=_tmp381->f3;
_tmp385=_tmp381->f4;}}_LL194: _tmp37B=Cyc_Toc_init_comprehension(nv,lval,_tmp382,
_tmp383,_tmp384,_tmp385,_tmp37B,0);goto _LL190;_LL195:;_LL196: Cyc_Toc_exp_to_c(nv,
e);_tmp37B=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37C(
_tmp37A,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp37B,0);goto _LL190;_LL190:;};}}
return Cyc_Toc_make_struct(nv,_tmp379,_tmp378,_tmp37B,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp386=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp386 != 0;_tmp386=_tmp386->tl){struct Cyc_Absyn_Aggrfield*
_tmp387=(struct Cyc_Absyn_Aggrfield*)_tmp386->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp387->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}({void*
_tmp388=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp38B;_tmp38B.tag=0;_tmp38B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp389[1]={& _tmp38B};Cyc_aprintf(({
const char*_tmp38A="get_member_offset %s failed";_tag_dyneither(_tmp38A,sizeof(
char),28);}),_tag_dyneither(_tmp389,sizeof(void*),1));});}),_tag_dyneither(
_tmp388,sizeof(void*),0));});}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp38C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38D=Cyc_Absyn_var_exp(_tmp38C,0);struct Cyc_Absyn_Stmt*
_tmp38E=Cyc_Absyn_exp_stmt(_tmp38D,0);struct Cyc_Absyn_Exp*(*_tmp38F)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp390=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp391=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp393;union Cyc_Absyn_AggrInfoU _tmp394;
_LL198: {struct Cyc_Absyn_AggrType_struct*_tmp392=(struct Cyc_Absyn_AggrType_struct*)
_tmp391;if(_tmp392->tag != 12)goto _LL19A;else{_tmp393=_tmp392->f1;_tmp394=_tmp393.aggr_info;}}
_LL199: ad=Cyc_Absyn_get_known_aggrdecl(_tmp394);goto _LL197;_LL19A:;_LL19B:({void*
_tmp395=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp396="init_struct: bad struct type";_tag_dyneither(_tmp396,sizeof(
char),29);}),_tag_dyneither(_tmp395,sizeof(void*),0));});_LL197:;}{int
is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp398;struct _RegionHandle*_tmp399;struct Cyc_Toc_Env*_tmp397=
nv;_tmp398=*_tmp397;_tmp399=_tmp398.rgn;{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp399,dles);for(0;
_tmp39A != 0;_tmp39A=_tmp39A->tl){struct _tuple15 _tmp39C;struct Cyc_List_List*
_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple15*_tmp39B=(struct _tuple15*)
_tmp39A->hd;_tmp39C=*_tmp39B;_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39E->topt))->v);
if(_tmp39D == 0)({void*_tmp39F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A0="empty designator list";
_tag_dyneither(_tmp3A0,sizeof(char),22);}),_tag_dyneither(_tmp39F,sizeof(void*),
0));});if(_tmp39D->tl != 0){struct _tuple0*_tmp3A1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A2=Cyc_Absyn_var_exp(_tmp3A1,0);for(0;_tmp39D != 0;_tmp39D=_tmp39D->tl){void*
_tmp3A3=(void*)_tmp39D->hd;struct _dyneither_ptr*_tmp3A5;_LL19D: {struct Cyc_Absyn_FieldName_struct*
_tmp3A4=(struct Cyc_Absyn_FieldName_struct*)_tmp3A3;if(_tmp3A4->tag != 1)goto
_LL19F;else{_tmp3A5=_tmp3A4->f1;}}_LL19E: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A5))_tmp3A2=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A2);_tmp38E=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38F(_tmp38D,_tmp3A5,0),_tmp3A2,0),0),
_tmp38E,0);goto _LL19C;_LL19F:;_LL1A0:({void*_tmp3A6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A7="array designator in struct";
_tag_dyneither(_tmp3A7,sizeof(char),27);}),_tag_dyneither(_tmp3A6,sizeof(void*),
0));});_LL19C:;}Cyc_Toc_exp_to_c(nv,_tmp39E);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp3A2,_tmp39E,0),0),_tmp38E,0);}else{void*_tmp3A8=(void*)
_tmp39D->hd;struct _dyneither_ptr*_tmp3AA;_LL1A2: {struct Cyc_Absyn_FieldName_struct*
_tmp3A9=(struct Cyc_Absyn_FieldName_struct*)_tmp3A8;if(_tmp3A9->tag != 1)goto
_LL1A4;else{_tmp3AA=_tmp3A9->f1;}}_LL1A3: {struct Cyc_Absyn_Exp*lval=_tmp38F(
_tmp38D,_tmp3AA,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3AA);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*
lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=
Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp38E,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp3AB=_tmp39E->r;struct Cyc_List_List*_tmp3AD;struct Cyc_Absyn_Vardecl*
_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;int _tmp3B2;void*
_tmp3B4;struct Cyc_List_List*_tmp3B5;_LL1A7: {struct Cyc_Absyn_Array_e_struct*
_tmp3AC=(struct Cyc_Absyn_Array_e_struct*)_tmp3AB;if(_tmp3AC->tag != 27)goto _LL1A9;
else{_tmp3AD=_tmp3AC->f1;}}_LL1A8: _tmp38E=Cyc_Toc_init_array(nv,lval,_tmp3AD,
_tmp38E);goto _LL1A6;_LL1A9: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3AE=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3AB;if(_tmp3AE->tag != 28)goto _LL1AB;
else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;_tmp3B1=_tmp3AE->f3;_tmp3B2=_tmp3AE->f4;}}
_LL1AA: _tmp38E=Cyc_Toc_init_comprehension(nv,lval,_tmp3AF,_tmp3B0,_tmp3B1,
_tmp3B2,_tmp38E,0);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B3=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3AB;if(_tmp3B3->tag != 30)goto
_LL1AD;else{_tmp3B4=(void*)_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;}}_LL1AC: _tmp38E=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B4,_tmp3B5,_tmp38E);goto _LL1A6;_LL1AD:;_LL1AE: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp39E->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp39E);{struct Cyc_Absyn_Aggrfield*_tmp3B6=Cyc_Absyn_lookup_decl_field(ad,
_tmp3AA);if(Cyc_Toc_is_poly_field(struct_type,_tmp3AA) && !was_ptr_type)_tmp39E=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39E);if(has_exists)_tmp39E=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B6))->type),
_tmp39E);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp39E,0),0),_tmp38E,0);goto _LL1A6;};}_LL1A6:;}goto _LL1A1;}_LL1A4:;_LL1A5:({
void*_tmp3B7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3B8="array designator in struct";_tag_dyneither(_tmp3B8,sizeof(
char),27);}),_tag_dyneither(_tmp3B7,sizeof(void*),0));});_LL1A1:;}}}return Cyc_Toc_make_struct(
nv,_tmp38C,_tmp390,_tmp38E,pointer,rgnopt,is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));
_tmp3B9[0]=({struct Cyc_Absyn_Increment_e_struct _tmp3BA;_tmp3BA.tag=5;_tmp3BA.f1=
e1;_tmp3BA.f2=incr;_tmp3BA;});_tmp3B9;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3BB=e1->r;struct Cyc_Absyn_Stmt*_tmp3BD;void*_tmp3BF;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C2;struct _dyneither_ptr*_tmp3C3;int _tmp3C4;int
_tmp3C5;_LL1B0: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3BC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3BB;if(_tmp3BC->tag != 37)goto _LL1B2;else{_tmp3BD=_tmp3BC->f1;}}_LL1B1: Cyc_Toc_lvalue_assign_stmt(
_tmp3BD,fs,f,f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp3BE=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3BB;if(_tmp3BE->tag != 15)goto _LL1B4;else{
_tmp3BF=(void*)_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;}}_LL1B3: Cyc_Toc_lvalue_assign(
_tmp3C0,fs,f,f_env);goto _LL1AF;_LL1B4: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3C1=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3BB;if(_tmp3C1->tag != 22)goto
_LL1B6;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;_tmp3C4=_tmp3C1->f3;_tmp3C5=
_tmp3C1->f4;}}_LL1B5: e1->r=_tmp3C2->r;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=_tmp3C3;
_tmp3C6->tl=fs;_tmp3C6;}),f,f_env);goto _LL1AF;_LL1B6:;_LL1B7: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;
goto _LL1AF;}_LL1AF:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env){void*_tmp3C7=s->r;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Decl*
_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CE;_LL1B9: {
struct Cyc_Absyn_Exp_s_struct*_tmp3C8=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C7;if(
_tmp3C8->tag != 1)goto _LL1BB;else{_tmp3C9=_tmp3C8->f1;}}_LL1BA: Cyc_Toc_lvalue_assign(
_tmp3C9,fs,f,f_env);goto _LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_struct*_tmp3CA=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3C7;if(_tmp3CA->tag != 12)goto _LL1BD;else{
_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL1BC: Cyc_Toc_lvalue_assign_stmt(
_tmp3CC,fs,f,f_env);goto _LL1B8;_LL1BD: {struct Cyc_Absyn_Seq_s_struct*_tmp3CD=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3C7;if(_tmp3CD->tag != 2)goto _LL1BF;else{
_tmp3CE=_tmp3CD->f2;}}_LL1BE: Cyc_Toc_lvalue_assign_stmt(_tmp3CE,fs,f,f_env);goto
_LL1B8;_LL1BF:;_LL1C0:({struct Cyc_String_pa_struct _tmp3D1;_tmp3D1.tag=0;_tmp3D1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*
_tmp3CF[1]={& _tmp3D1};Cyc_Toc_toc_impos(({const char*_tmp3D0="lvalue_assign_stmt: %s";
_tag_dyneither(_tmp3D0,sizeof(char),23);}),_tag_dyneither(_tmp3CF,sizeof(void*),
1));});});_LL1B8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*
e){void*_tmp3D2=e->r;void*_tmp3D4;void**_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;
struct Cyc_Absyn_Exp**_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D9;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D3=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D2;if(_tmp3D3->tag != 15)goto _LL1C4;
else{_tmp3D4=(void**)& _tmp3D3->f1;_tmp3D5=(void**)((void**)& _tmp3D3->f1);_tmp3D6=
_tmp3D3->f2;_tmp3D7=(struct Cyc_Absyn_Exp**)& _tmp3D3->f2;}}_LL1C3:*_tmp3D7=Cyc_Toc_push_address_exp(*
_tmp3D7);*_tmp3D5=Cyc_Absyn_cstar_typ(*_tmp3D5,Cyc_Toc_mt_tq);return e;_LL1C4: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D8=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D2;
if(_tmp3D8->tag != 21)goto _LL1C6;else{_tmp3D9=_tmp3D8->f1;}}_LL1C5: return _tmp3D9;
_LL1C6:;_LL1C7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);({
struct Cyc_String_pa_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3DA[1]={& _tmp3DC};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3DB="can't take & of %s";_tag_dyneither(_tmp3DB,sizeof(char),19);}),
_tag_dyneither(_tmp3DA,sizeof(void*),1));});});_LL1C1:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp3DD=_region_malloc(r2,sizeof(*_tmp3DD));_tmp3DD->hd=(
void*)f((void*)x->hd,env);_tmp3DD->tl=0;_tmp3DD;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp3DE=_region_malloc(r2,sizeof(*_tmp3DE));_tmp3DE->hd=(void*)f((void*)x->hd,
env);_tmp3DE->tl=0;_tmp3DE;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple15*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->f1=0;_tmp3DF->f2=e;_tmp3DF;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3E0=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E2;_LL1C9: {struct Cyc_Absyn_PointerType_struct*
_tmp3E1=(struct Cyc_Absyn_PointerType_struct*)_tmp3E0;if(_tmp3E1->tag != 5)goto
_LL1CB;else{_tmp3E2=_tmp3E1->f1;}}_LL1CA: return _tmp3E2;_LL1CB:;_LL1CC:({void*
_tmp3E3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3E4="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3E4,sizeof(
char),28);}),_tag_dyneither(_tmp3E3,sizeof(void*),0));});_LL1C8:;}static struct
Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*
_tmp3E5=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3E8;enum Cyc_Absyn_Size_of
_tmp3E9;enum Cyc_Absyn_Sign _tmp3EB;enum Cyc_Absyn_Size_of _tmp3EC;enum Cyc_Absyn_Sign
_tmp3F0;enum Cyc_Absyn_Size_of _tmp3F1;enum Cyc_Absyn_Sign _tmp3F3;enum Cyc_Absyn_Size_of
_tmp3F4;enum Cyc_Absyn_Sign _tmp3F6;enum Cyc_Absyn_Size_of _tmp3F7;_LL1CE: {struct
Cyc_Absyn_PointerType_struct*_tmp3E6=(struct Cyc_Absyn_PointerType_struct*)
_tmp3E5;if(_tmp3E6->tag != 5)goto _LL1D0;}_LL1CF: res=Cyc_Absyn_null_exp(0);goto
_LL1CD;_LL1D0: {struct Cyc_Absyn_IntType_struct*_tmp3E7=(struct Cyc_Absyn_IntType_struct*)
_tmp3E5;if(_tmp3E7->tag != 6)goto _LL1D2;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;
if(_tmp3E9 != Cyc_Absyn_Char_sz)goto _LL1D2;}}_LL1D1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp3E8,'\000'),0);goto _LL1CD;_LL1D2: {struct Cyc_Absyn_IntType_struct*_tmp3EA=(
struct Cyc_Absyn_IntType_struct*)_tmp3E5;if(_tmp3EA->tag != 6)goto _LL1D4;else{
_tmp3EB=_tmp3EA->f1;_tmp3EC=_tmp3EA->f2;if(_tmp3EC != Cyc_Absyn_Short_sz)goto
_LL1D4;}}_LL1D3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3EB,0),0);goto
_LL1CD;_LL1D4: {struct Cyc_Absyn_EnumType_struct*_tmp3ED=(struct Cyc_Absyn_EnumType_struct*)
_tmp3E5;if(_tmp3ED->tag != 14)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp3EE=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp3E5;if(_tmp3EE->tag != 15)goto
_LL1D8;}_LL1D7: _tmp3F0=Cyc_Absyn_Unsigned;goto _LL1D9;_LL1D8: {struct Cyc_Absyn_IntType_struct*
_tmp3EF=(struct Cyc_Absyn_IntType_struct*)_tmp3E5;if(_tmp3EF->tag != 6)goto _LL1DA;
else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;if(_tmp3F1 != Cyc_Absyn_Int_sz)goto
_LL1DA;}}_LL1D9: _tmp3F3=_tmp3F0;goto _LL1DB;_LL1DA: {struct Cyc_Absyn_IntType_struct*
_tmp3F2=(struct Cyc_Absyn_IntType_struct*)_tmp3E5;if(_tmp3F2->tag != 6)goto _LL1DC;
else{_tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;if(_tmp3F4 != Cyc_Absyn_Long_sz)goto
_LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3F3,0),0);goto _LL1CD;
_LL1DC: {struct Cyc_Absyn_IntType_struct*_tmp3F5=(struct Cyc_Absyn_IntType_struct*)
_tmp3E5;if(_tmp3F5->tag != 6)goto _LL1DE;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;
if(_tmp3F7 != Cyc_Absyn_LongLong_sz)goto _LL1DE;}}_LL1DD: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp3F6,(long long)0),0);goto _LL1CD;_LL1DE: {struct Cyc_Absyn_FloatType_struct*
_tmp3F8=(struct Cyc_Absyn_FloatType_struct*)_tmp3E5;if(_tmp3F8->tag != 7)goto
_LL1E0;}_LL1DF: goto _LL1E1;_LL1E0: {struct Cyc_Absyn_DoubleType_struct*_tmp3F9=(
struct Cyc_Absyn_DoubleType_struct*)_tmp3E5;if(_tmp3F9->tag != 8)goto _LL1E2;}
_LL1E1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3FA="0.0";
_tag_dyneither(_tmp3FA,sizeof(char),4);})),0);goto _LL1CD;_LL1E2:;_LL1E3:({struct
Cyc_String_pa_struct _tmp3FD;_tmp3FD.tag=0;_tmp3FD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp3FB[1]={& _tmp3FD};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3FC="found non-zero type %s in generate_zero";_tag_dyneither(
_tmp3FC,sizeof(char),40);}),_tag_dyneither(_tmp3FB,sizeof(void*),1));});});
_LL1CD:;}res->topt=({struct Cyc_Core_Opt*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));
_tmp3FE->v=t;_tmp3FE;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp3FF=Cyc_Toc_typ_to_c(elt_type);void*_tmp400=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp401=Cyc_Absyn_cstar_typ(_tmp3FF,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp402=({struct Cyc_Core_Opt*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->v=
_tmp401;_tmp443;});struct Cyc_Absyn_Exp*xinit;{void*_tmp403=e1->r;struct Cyc_Absyn_Exp*
_tmp405;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp408;_LL1E5: {struct
Cyc_Absyn_Deref_e_struct*_tmp404=(struct Cyc_Absyn_Deref_e_struct*)_tmp403;if(
_tmp404->tag != 21)goto _LL1E7;else{_tmp405=_tmp404->f1;}}_LL1E6: if(!is_dyneither){
_tmp405=Cyc_Toc_cast_it(fat_ptr_type,_tmp405);_tmp405->topt=({struct Cyc_Core_Opt*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->v=fat_ptr_type;_tmp409;});}Cyc_Toc_exp_to_c(
nv,_tmp405);xinit=_tmp405;goto _LL1E4;_LL1E7: {struct Cyc_Absyn_Subscript_e_struct*
_tmp406=(struct Cyc_Absyn_Subscript_e_struct*)_tmp403;if(_tmp406->tag != 24)goto
_LL1E9;else{_tmp407=_tmp406->f1;_tmp408=_tmp406->f2;}}_LL1E8: if(!is_dyneither){
_tmp407=Cyc_Toc_cast_it(fat_ptr_type,_tmp407);_tmp407->topt=({struct Cyc_Core_Opt*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->v=fat_ptr_type;_tmp40A;});}Cyc_Toc_exp_to_c(
nv,_tmp407);Cyc_Toc_exp_to_c(nv,_tmp408);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp40B[3];_tmp40B[2]=_tmp408;_tmp40B[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp40B[0]=_tmp407;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);goto _LL1E4;_LL1E9:;_LL1EA:({void*_tmp40C=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp40D="found bad lhs for zero-terminated pointer assignment";
_tag_dyneither(_tmp40D,sizeof(char),53);}),_tag_dyneither(_tmp40C,sizeof(void*),
0));});_LL1E4:;}{struct _tuple0*_tmp40E=Cyc_Toc_temp_var();struct Cyc_Toc_Env
_tmp410;struct _RegionHandle*_tmp411;struct Cyc_Toc_Env*_tmp40F=nv;_tmp410=*
_tmp40F;_tmp411=_tmp410.rgn;{struct Cyc_Toc_Env*_tmp412=Cyc_Toc_add_varmap(
_tmp411,nv,_tmp40E,Cyc_Absyn_var_exp(_tmp40E,0));struct Cyc_Absyn_Vardecl*_tmp413=({
struct Cyc_Absyn_Vardecl*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->sc=Cyc_Absyn_Public;
_tmp442->name=_tmp40E;_tmp442->tq=Cyc_Toc_mt_tq;_tmp442->type=_tmp400;_tmp442->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp442->rgn=0;_tmp442->attributes=0;_tmp442->escapes=
0;_tmp442;});struct Cyc_Absyn_Local_b_struct*_tmp414=({struct Cyc_Absyn_Local_b_struct*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_Absyn_Local_b_struct
_tmp441;_tmp441.tag=4;_tmp441.f1=_tmp413;_tmp441;});_tmp440;});struct Cyc_Absyn_Exp*
_tmp415=Cyc_Absyn_varb_exp(_tmp40E,(void*)_tmp414,0);_tmp415->topt=({struct Cyc_Core_Opt*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->v=fat_ptr_type;_tmp416;});{struct
Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_deref_exp(_tmp415,0);_tmp417->topt=({struct Cyc_Core_Opt*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->v=elt_type;_tmp418;});Cyc_Toc_exp_to_c(
_tmp412,_tmp417);{struct _tuple0*_tmp419=Cyc_Toc_temp_var();_tmp412=Cyc_Toc_add_varmap(
_tmp411,_tmp412,_tmp419,Cyc_Absyn_var_exp(_tmp419,0));{struct Cyc_Absyn_Vardecl*
_tmp41A=({struct Cyc_Absyn_Vardecl*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->sc=
Cyc_Absyn_Public;_tmp43F->name=_tmp419;_tmp43F->tq=Cyc_Toc_mt_tq;_tmp43F->type=
_tmp3FF;_tmp43F->initializer=(struct Cyc_Absyn_Exp*)_tmp417;_tmp43F->rgn=0;
_tmp43F->attributes=0;_tmp43F->escapes=0;_tmp43F;});struct Cyc_Absyn_Local_b_struct*
_tmp41B=({struct Cyc_Absyn_Local_b_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));
_tmp43D[0]=({struct Cyc_Absyn_Local_b_struct _tmp43E;_tmp43E.tag=4;_tmp43E.f1=
_tmp41A;_tmp43E;});_tmp43D;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct
Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41B,0);_tmp41C->topt=
_tmp417->topt;z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp41C,
e2,0);z_init->topt=_tmp41C->topt;}Cyc_Toc_exp_to_c(_tmp412,z_init);{struct
_tuple0*_tmp41D=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*_tmp41E=({struct Cyc_Absyn_Vardecl*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->sc=Cyc_Absyn_Public;_tmp43C->name=
_tmp41D;_tmp43C->tq=Cyc_Toc_mt_tq;_tmp43C->type=_tmp3FF;_tmp43C->initializer=(
struct Cyc_Absyn_Exp*)z_init;_tmp43C->rgn=0;_tmp43C->attributes=0;_tmp43C->escapes=
0;_tmp43C;});struct Cyc_Absyn_Local_b_struct*_tmp41F=({struct Cyc_Absyn_Local_b_struct*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({struct Cyc_Absyn_Local_b_struct
_tmp43B;_tmp43B.tag=4;_tmp43B.f1=_tmp41E;_tmp43B;});_tmp43A;});_tmp412=Cyc_Toc_add_varmap(
_tmp411,_tmp412,_tmp41D,Cyc_Absyn_var_exp(_tmp41D,0));{struct Cyc_Absyn_Exp*
_tmp420=Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41B,0);_tmp420->topt=_tmp417->topt;{
struct Cyc_Absyn_Exp*_tmp421=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp422=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp420,_tmp421,0);_tmp422->topt=({
struct Cyc_Core_Opt*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->v=Cyc_Absyn_sint_typ;
_tmp423;});Cyc_Toc_exp_to_c(_tmp412,_tmp422);{struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_varb_exp(
_tmp41D,(void*)_tmp41F,0);_tmp424->topt=_tmp417->topt;{struct Cyc_Absyn_Exp*
_tmp425=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Neq,_tmp424,_tmp425,0);_tmp426->topt=({struct Cyc_Core_Opt*_tmp427=
_cycalloc(sizeof(*_tmp427));_tmp427->v=Cyc_Absyn_sint_typ;_tmp427;});Cyc_Toc_exp_to_c(
_tmp412,_tmp426);{struct Cyc_List_List*_tmp428=({struct Cyc_Absyn_Exp*_tmp439[2];
_tmp439[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp439[0]=Cyc_Absyn_varb_exp(
_tmp40E,(void*)_tmp414,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp439,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp429=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp428,0),
_tmp429,0);{struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp422,_tmp426,0),0);struct Cyc_Absyn_Stmt*_tmp42B=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp40E,(void*)_tmp414,0),Cyc_Toc_curr_sp,0);_tmp42C=Cyc_Toc_cast_it(
_tmp401,_tmp42C);{struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_deref_exp(_tmp42C,0);
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_assign_exp(_tmp42D,Cyc_Absyn_var_exp(
_tmp41D,0),0);struct Cyc_Absyn_Stmt*_tmp42F=Cyc_Absyn_exp_stmt(_tmp42E,0);_tmp42F=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42A,_tmp42B,Cyc_Absyn_skip_stmt(
0),0),_tmp42F,0);_tmp42F=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp430=
_cycalloc(sizeof(*_tmp430));_tmp430->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_Absyn_Var_d_struct
_tmp432;_tmp432.tag=0;_tmp432.f1=_tmp41E;_tmp432;});_tmp431;});_tmp430->loc=0;
_tmp430;}),_tmp42F,0);_tmp42F=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp433=
_cycalloc(sizeof(*_tmp433));_tmp433->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434[0]=({struct Cyc_Absyn_Var_d_struct
_tmp435;_tmp435.tag=0;_tmp435.f1=_tmp41A;_tmp435;});_tmp434;});_tmp433->loc=0;
_tmp433;}),_tmp42F,0);_tmp42F=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp436=
_cycalloc(sizeof(*_tmp436));_tmp436->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct Cyc_Absyn_Var_d_struct
_tmp438;_tmp438.tag=0;_tmp438.f1=_tmp413;_tmp438;});_tmp437;});_tmp436->loc=0;
_tmp436;}),_tmp42F,0);e->r=Cyc_Toc_stmt_exp_r(_tmp42F);};};};};};};};};};};};};};}
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*
aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp444=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp446;union
Cyc_Absyn_AggrInfoU _tmp447;_LL1EC: {struct Cyc_Absyn_AggrType_struct*_tmp445=(
struct Cyc_Absyn_AggrType_struct*)_tmp444;if(_tmp445->tag != 12)goto _LL1EE;else{
_tmp446=_tmp445->f1;_tmp447=_tmp446.aggr_info;}}_LL1ED: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp447);goto _LL1EB;_LL1EE:;_LL1EF:({void*_tmp448=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp44B;_tmp44B.tag=0;_tmp44B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({
void*_tmp449[1]={& _tmp44B};Cyc_aprintf(({const char*_tmp44A="expecting union but found %s in check_tagged_union";
_tag_dyneither(_tmp44A,sizeof(char),51);}),_tag_dyneither(_tmp449,sizeof(void*),
1));});}),_tag_dyneither(_tmp448,sizeof(void*),0));});_LL1EB:;}{struct _tuple0*
_tmp44C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp44D=Cyc_Absyn_var_exp(_tmp44C,
0);struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44D,f,0),
Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
ad,f),0);struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_neq_exp(_tmp44E,_tmp44F,0);struct
Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44D,f,0),Cyc_Toc_val_sp,
0);struct Cyc_Absyn_Stmt*_tmp452=Cyc_Absyn_exp_stmt(_tmp451,0);struct Cyc_Absyn_Stmt*
_tmp453=Cyc_Absyn_ifthenelse_stmt(_tmp450,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp454=Cyc_Absyn_declare_stmt(_tmp44C,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp453,_tmp452,0),0);return Cyc_Toc_stmt_exp_r(
_tmp454);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp455=e->r;struct Cyc_Absyn_Exp*
_tmp457;struct Cyc_Absyn_Exp*_tmp459;struct _dyneither_ptr*_tmp45A;int _tmp45B;int*
_tmp45C;struct Cyc_Absyn_Exp*_tmp45E;struct _dyneither_ptr*_tmp45F;int _tmp460;int*
_tmp461;_LL1F1: {struct Cyc_Absyn_Cast_e_struct*_tmp456=(struct Cyc_Absyn_Cast_e_struct*)
_tmp455;if(_tmp456->tag != 15)goto _LL1F3;else{_tmp457=_tmp456->f2;}}_LL1F2:({void*
_tmp462=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp463="cast on lhs!";_tag_dyneither(_tmp463,sizeof(char),13);}),
_tag_dyneither(_tmp462,sizeof(void*),0));});_LL1F3: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp458=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp455;if(_tmp458->tag != 22)goto
_LL1F5;else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;_tmp45B=_tmp458->f4;_tmp45C=(
int*)& _tmp458->f4;}}_LL1F4: {void*_tmp464=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp459->topt))->v);struct Cyc_Absyn_AggrInfo _tmp466;union Cyc_Absyn_AggrInfoU
_tmp467;_LL1FA: {struct Cyc_Absyn_AggrType_struct*_tmp465=(struct Cyc_Absyn_AggrType_struct*)
_tmp464;if(_tmp465->tag != 12)goto _LL1FC;else{_tmp466=_tmp465->f1;_tmp467=_tmp466.aggr_info;}}
_LL1FB: {struct Cyc_Absyn_Aggrdecl*_tmp468=Cyc_Absyn_get_known_aggrdecl(_tmp467);*
f_tag=Cyc_Toc_get_member_offset(_tmp468,_tmp45A);{struct _dyneither_ptr str=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp46D;_tmp46D.tag=0;_tmp46D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp45A);({struct Cyc_String_pa_struct
_tmp46C;_tmp46C.tag=0;_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp468->name).f2);({void*_tmp46A[2]={& _tmp46C,& _tmp46D};Cyc_aprintf(({const char*
_tmp46B="_union_%s_%s";_tag_dyneither(_tmp46B,sizeof(char),13);}),_tag_dyneither(
_tmp46A,sizeof(void*),2));});});});*tagged_member_type=Cyc_Absyn_strct(({struct
_dyneither_ptr*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=str;_tmp469;}));
if(clear_read)*_tmp45C=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp468->impl))->tagged;};}_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1F5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp45D=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp455;if(_tmp45D->tag != 23)goto
_LL1F7;else{_tmp45E=_tmp45D->f1;_tmp45F=_tmp45D->f2;_tmp460=_tmp45D->f4;_tmp461=(
int*)& _tmp45D->f4;}}_LL1F6: {void*_tmp46E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp470;void*_tmp471;
_LL1FF: {struct Cyc_Absyn_PointerType_struct*_tmp46F=(struct Cyc_Absyn_PointerType_struct*)
_tmp46E;if(_tmp46F->tag != 5)goto _LL201;else{_tmp470=_tmp46F->f1;_tmp471=_tmp470.elt_typ;}}
_LL200: {void*_tmp472=Cyc_Tcutil_compress(_tmp471);struct Cyc_Absyn_AggrInfo
_tmp474;union Cyc_Absyn_AggrInfoU _tmp475;_LL204: {struct Cyc_Absyn_AggrType_struct*
_tmp473=(struct Cyc_Absyn_AggrType_struct*)_tmp472;if(_tmp473->tag != 12)goto
_LL206;else{_tmp474=_tmp473->f1;_tmp475=_tmp474.aggr_info;}}_LL205: {struct Cyc_Absyn_Aggrdecl*
_tmp476=Cyc_Absyn_get_known_aggrdecl(_tmp475);*f_tag=Cyc_Toc_get_member_offset(
_tmp476,_tmp45F);{struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp47B;_tmp47B.tag=0;_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp45F);({struct Cyc_String_pa_struct _tmp47A;_tmp47A.tag=0;_tmp47A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp476->name).f2);({void*_tmp478[2]={&
_tmp47A,& _tmp47B};Cyc_aprintf(({const char*_tmp479="_union_%s_%s";_tag_dyneither(
_tmp479,sizeof(char),13);}),_tag_dyneither(_tmp478,sizeof(void*),2));});});});*
tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp477=_cycalloc(
sizeof(*_tmp477));_tmp477[0]=str;_tmp477;}));if(clear_read)*_tmp461=0;return((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp476->impl))->tagged;};}_LL206:;
_LL207: return 0;_LL203:;}_LL201:;_LL202: return 0;_LL1FE:;}_LL1F7:;_LL1F8: return 0;
_LL1F0:;}static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*
e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int
tag_num,void*member_type){struct _tuple0*_tmp47C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp47C,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(
temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 0)s2=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*_tmp47D=Cyc_Absyn_neq_exp(
temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp47D,Cyc_Toc_throw_match_stmt(),
Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(
_tmp47C,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(
e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);return Cyc_Toc_stmt_exp_r(s1);};}struct
_tuple19{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple20{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp47E=e->r;if(e->topt == 0)({
struct Cyc_String_pa_struct _tmp481;_tmp481.tag=0;_tmp481.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp47F[1]={& _tmp481};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp480="exp_to_c: no type for %s";_tag_dyneither(_tmp480,sizeof(char),
25);}),_tag_dyneither(_tmp47F,sizeof(void*),1));});});{void*old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp482=_tmp47E;union Cyc_Absyn_Cnst
_tmp484;int _tmp485;struct _tuple0*_tmp488;void*_tmp489;struct _tuple0*_tmp48B;
enum Cyc_Absyn_Primop _tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_Exp*
_tmp490;enum Cyc_Absyn_Incrementor _tmp491;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Core_Opt*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*
_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*
_tmp49C;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_List_List*
_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*_tmp4A8;struct Cyc_Absyn_VarargCallInfo*
_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_VarargCallInfo*
_tmp4AD;struct Cyc_Absyn_VarargCallInfo _tmp4AE;int _tmp4AF;struct Cyc_List_List*
_tmp4B0;struct Cyc_Absyn_VarargInfo*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_List_List*_tmp4B8;void*_tmp4BA;
void**_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;int _tmp4BD;enum Cyc_Absyn_Coercion
_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C5;void*_tmp4C7;void*_tmp4C9;void*_tmp4CA;
struct _dyneither_ptr*_tmp4CC;void*_tmp4CE;void*_tmp4CF;unsigned int _tmp4D1;
struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct _dyneither_ptr*
_tmp4D6;int _tmp4D7;int _tmp4D8;struct Cyc_Absyn_Exp*_tmp4DA;struct _dyneither_ptr*
_tmp4DB;int _tmp4DC;int _tmp4DD;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*
_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*_tmp4E4;struct Cyc_Absyn_Vardecl*
_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;int _tmp4E9;
struct _tuple0*_tmp4EB;struct Cyc_List_List*_tmp4EC;struct Cyc_List_List*_tmp4ED;
struct Cyc_Absyn_Aggrdecl*_tmp4EE;void*_tmp4F0;struct Cyc_List_List*_tmp4F1;struct
Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Datatypedecl*_tmp4F4;struct Cyc_Absyn_Datatypefield*
_tmp4F5;struct Cyc_Absyn_MallocInfo _tmp4F9;int _tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;
void**_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;int _tmp4FE;struct Cyc_Absyn_Exp*_tmp500;
struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp503;struct _dyneither_ptr*
_tmp504;struct Cyc_Absyn_Stmt*_tmp506;_LL209: {struct Cyc_Absyn_Const_e_struct*
_tmp483=(struct Cyc_Absyn_Const_e_struct*)_tmp482;if(_tmp483->tag != 0)goto _LL20B;
else{_tmp484=_tmp483->f1;if((_tmp484.Null_c).tag != 1)goto _LL20B;_tmp485=(int)(
_tmp484.Null_c).val;}}_LL20A: {struct Cyc_Absyn_Exp*_tmp50A=Cyc_Absyn_uint_exp(0,
0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(Cyc_Toc_is_toplevel(nv))e->r=(
Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp50A,_tmp50A))->r;else{e->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp50B[3];_tmp50B[2]=_tmp50A;
_tmp50B[1]=_tmp50A;_tmp50B[0]=_tmp50A;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50B,sizeof(struct Cyc_Absyn_Exp*),
3));}));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL208;}_LL20B: {struct Cyc_Absyn_Const_e_struct*
_tmp486=(struct Cyc_Absyn_Const_e_struct*)_tmp482;if(_tmp486->tag != 0)goto _LL20D;}
_LL20C: goto _LL208;_LL20D: {struct Cyc_Absyn_Var_e_struct*_tmp487=(struct Cyc_Absyn_Var_e_struct*)
_tmp482;if(_tmp487->tag != 1)goto _LL20F;else{_tmp488=_tmp487->f1;_tmp489=(void*)
_tmp487->f2;}}_LL20E:{struct _handler_cons _tmp50C;_push_handler(& _tmp50C);{int
_tmp50E=0;if(setjmp(_tmp50C.handler))_tmp50E=1;if(!_tmp50E){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp488))->r;;_pop_handler();}else{void*_tmp50D=(void*)_exn_thrown;void*
_tmp510=_tmp50D;_LL260: {struct Cyc_Dict_Absent_struct*_tmp511=(struct Cyc_Dict_Absent_struct*)
_tmp510;if(_tmp511->tag != Cyc_Dict_Absent)goto _LL262;}_LL261:({struct Cyc_String_pa_struct
_tmp514;_tmp514.tag=0;_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp488));({void*_tmp512[1]={& _tmp514};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp513="Can't find %s in exp_to_c, Var\n";
_tag_dyneither(_tmp513,sizeof(char),32);}),_tag_dyneither(_tmp512,sizeof(void*),
1));});});_LL262:;_LL263:(void)_throw(_tmp510);_LL25F:;}};}goto _LL208;_LL20F: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp48A=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp482;if(_tmp48A->tag != 2)goto _LL211;else{_tmp48B=_tmp48A->f1;}}_LL210:({void*
_tmp515=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp516="unknownid";_tag_dyneither(_tmp516,sizeof(char),10);}),
_tag_dyneither(_tmp515,sizeof(void*),0));});_LL211: {struct Cyc_Absyn_Primop_e_struct*
_tmp48C=(struct Cyc_Absyn_Primop_e_struct*)_tmp482;if(_tmp48C->tag != 3)goto _LL213;
else{_tmp48D=_tmp48C->f1;_tmp48E=_tmp48C->f2;}}_LL212: {struct Cyc_List_List*
_tmp517=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp48E);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp48E);switch(_tmp48D){case Cyc_Absyn_Numelts: _LL264: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48E))->hd;{void*_tmp518=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp51A;struct Cyc_Absyn_Exp*
_tmp51B;struct Cyc_Absyn_PtrInfo _tmp51D;void*_tmp51E;struct Cyc_Absyn_PtrAtts
_tmp51F;union Cyc_Absyn_Constraint*_tmp520;union Cyc_Absyn_Constraint*_tmp521;
union Cyc_Absyn_Constraint*_tmp522;_LL267: {struct Cyc_Absyn_ArrayType_struct*
_tmp519=(struct Cyc_Absyn_ArrayType_struct*)_tmp518;if(_tmp519->tag != 9)goto
_LL269;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp51A.num_elts;}}_LL268: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp51B)))({void*_tmp523=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp524="can't calculate numelts";_tag_dyneither(_tmp524,sizeof(char),
24);}),_tag_dyneither(_tmp523,sizeof(void*),0));});e->r=_tmp51B->r;goto _LL266;
_LL269: {struct Cyc_Absyn_PointerType_struct*_tmp51C=(struct Cyc_Absyn_PointerType_struct*)
_tmp518;if(_tmp51C->tag != 5)goto _LL26B;else{_tmp51D=_tmp51C->f1;_tmp51E=_tmp51D.elt_typ;
_tmp51F=_tmp51D.ptr_atts;_tmp520=_tmp51F.nullable;_tmp521=_tmp51F.bounds;_tmp522=
_tmp51F.zero_term;}}_LL26A:{void*_tmp525=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp521);struct Cyc_Absyn_Exp*
_tmp528;_LL26E: {struct Cyc_Absyn_DynEither_b_struct*_tmp526=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp525;if(_tmp526->tag != 0)goto _LL270;}_LL26F: e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({
struct Cyc_Absyn_Exp*_tmp529[2];_tmp529[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp51E),0);_tmp529[0]=(struct Cyc_Absyn_Exp*)_tmp48E->hd;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp529,sizeof(struct Cyc_Absyn_Exp*),
2));}));goto _LL26D;_LL270: {struct Cyc_Absyn_Upper_b_struct*_tmp527=(struct Cyc_Absyn_Upper_b_struct*)
_tmp525;if(_tmp527->tag != 1)goto _LL26D;else{_tmp528=_tmp527->f1;}}_LL271: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp522)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp48E->hd);e->r=Cyc_Toc_fncall_exp_r(function_e,({struct
Cyc_Absyn_Exp*_tmp52A[2];_tmp52A[1]=_tmp528;_tmp52A[0]=(struct Cyc_Absyn_Exp*)
_tmp48E->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp52A,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp520)){if(!Cyc_Evexp_c_can_eval(
_tmp528))({void*_tmp52B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp52C="can't calculate numelts";
_tag_dyneither(_tmp52C,sizeof(char),24);}),_tag_dyneither(_tmp52B,sizeof(void*),
0));});e->r=Cyc_Toc_conditional_exp_r(arg,_tmp528,Cyc_Absyn_uint_exp(0,0));}
else{e->r=_tmp528->r;goto _LL26D;}}goto _LL26D;_LL26D:;}goto _LL266;_LL26B:;_LL26C:({
struct Cyc_String_pa_struct _tmp530;_tmp530.tag=0;_tmp530.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v));({struct Cyc_String_pa_struct _tmp52F;_tmp52F.tag=0;
_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));({void*_tmp52D[2]={&
_tmp52F,& _tmp530};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp52E="size primop applied to non-array %s (%s)";_tag_dyneither(
_tmp52E,sizeof(char),41);}),_tag_dyneither(_tmp52D,sizeof(void*),2));});});});
_LL266:;}break;}case Cyc_Absyn_Plus: _LL265:{void*_tmp531=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp517))->hd);struct Cyc_Absyn_PtrInfo
_tmp533;void*_tmp534;struct Cyc_Absyn_PtrAtts _tmp535;union Cyc_Absyn_Constraint*
_tmp536;union Cyc_Absyn_Constraint*_tmp537;_LL274: {struct Cyc_Absyn_PointerType_struct*
_tmp532=(struct Cyc_Absyn_PointerType_struct*)_tmp531;if(_tmp532->tag != 5)goto
_LL276;else{_tmp533=_tmp532->f1;_tmp534=_tmp533.elt_typ;_tmp535=_tmp533.ptr_atts;
_tmp536=_tmp535.bounds;_tmp537=_tmp535.zero_term;}}_LL275:{void*_tmp538=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp536);struct Cyc_Absyn_Exp*_tmp53B;_LL279: {struct Cyc_Absyn_DynEither_b_struct*
_tmp539=(struct Cyc_Absyn_DynEither_b_struct*)_tmp538;if(_tmp539->tag != 0)goto
_LL27B;}_LL27A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48E->tl))->hd;e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp53C[3];_tmp53C[2]=e2;_tmp53C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp534),0);_tmp53C[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp53C,sizeof(struct Cyc_Absyn_Exp*),
3));}));goto _LL278;}_LL27B: {struct Cyc_Absyn_Upper_b_struct*_tmp53A=(struct Cyc_Absyn_Upper_b_struct*)
_tmp538;if(_tmp53A->tag != 1)goto _LL278;else{_tmp53B=_tmp53A->f1;}}_LL27C: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp537)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48E->tl))->hd;e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp53D[3];_tmp53D[
2]=e2;_tmp53D[1]=_tmp53B;_tmp53D[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp53D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r;}goto _LL278;_LL278:;}goto _LL273;_LL276:;_LL277: goto _LL273;_LL273:;}
break;case Cyc_Absyn_Minus: _LL272: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp517->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp53E[3];_tmp53E[2]=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,
0);_tmp53E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp53E[0]=e1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp53E,sizeof(struct Cyc_Absyn_Exp*),3));}));}}break;}case Cyc_Absyn_Eq: _LL27D:
goto _LL27E;case Cyc_Absyn_Neq: _LL27E: goto _LL27F;case Cyc_Absyn_Gt: _LL27F: goto
_LL280;case Cyc_Absyn_Gte: _LL280: goto _LL281;case Cyc_Absyn_Lt: _LL281: goto _LL282;
case Cyc_Absyn_Lte: _LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp517->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
break;}default: _LL283: break;}goto _LL208;}_LL213: {struct Cyc_Absyn_Increment_e_struct*
_tmp48F=(struct Cyc_Absyn_Increment_e_struct*)_tmp482;if(_tmp48F->tag != 5)goto
_LL215;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;}}_LL214: {void*e2_cyc_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp490->topt))->v;void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;struct _dyneither_ptr incr_str=({const char*_tmp561="increment";
_tag_dyneither(_tmp561,sizeof(char),10);});if(_tmp491 == Cyc_Absyn_PreDec  || 
_tmp491 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp53F="decrement";
_tag_dyneither(_tmp53F,sizeof(char),10);});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp490,& ptr_type,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp542;
_tmp542.tag=0;_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({
void*_tmp540[1]={& _tmp542};Cyc_Tcutil_terr(e->loc,({const char*_tmp541="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dyneither(_tmp541,sizeof(char),74);}),_tag_dyneither(_tmp540,sizeof(void*),
1));});});({void*_tmp543=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp544="in-place inc/dec on zero-term";
_tag_dyneither(_tmp544,sizeof(char),30);}),_tag_dyneither(_tmp543,sizeof(void*),
0));});}{void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(
Cyc_Toc_is_tagged_union_project(_tmp490,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*
_tmp545=Cyc_Absyn_signed_int_exp(1,0);_tmp545->topt=({struct Cyc_Core_Opt*_tmp546=
_cycalloc(sizeof(*_tmp546));_tmp546->v=Cyc_Absyn_sint_typ;_tmp546;});switch(
_tmp491){case Cyc_Absyn_PreInc: _LL285: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*
_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_Absyn_AssignOp_e_struct
_tmp548;_tmp548.tag=4;_tmp548.f1=_tmp490;_tmp548.f2=({struct Cyc_Core_Opt*_tmp549=
_cycalloc_atomic(sizeof(*_tmp549));_tmp549->v=(void*)Cyc_Absyn_Plus;_tmp549;});
_tmp548.f3=_tmp545;_tmp548;});_tmp547;});Cyc_Toc_exp_to_c(nv,e);return;case Cyc_Absyn_PreDec:
_LL286: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmp54A=_cycalloc(sizeof(*
_tmp54A));_tmp54A[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmp54B;_tmp54B.tag=4;
_tmp54B.f1=_tmp490;_tmp54B.f2=({struct Cyc_Core_Opt*_tmp54C=_cycalloc_atomic(
sizeof(*_tmp54C));_tmp54C->v=(void*)Cyc_Absyn_Minus;_tmp54C;});_tmp54B.f3=
_tmp545;_tmp54B;});_tmp54A;});Cyc_Toc_exp_to_c(nv,e);return;default: _LL287:({
struct Cyc_String_pa_struct _tmp54F;_tmp54F.tag=0;_tmp54F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)incr_str);({void*_tmp54D[1]={& _tmp54F};Cyc_Tcutil_terr(e->loc,({
const char*_tmp54E="in-place post-%s is not supported on @tagged union members";
_tag_dyneither(_tmp54E,sizeof(char),59);}),_tag_dyneither(_tmp54D,sizeof(void*),
1));});});({void*_tmp550=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp551="in-place inc/dec on @tagged union";
_tag_dyneither(_tmp551,sizeof(char),34);}),_tag_dyneither(_tmp550,sizeof(void*),
0));});}}Cyc_Toc_exp_to_c(nv,_tmp490);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;int change=1;fn_e=(_tmp491 == Cyc_Absyn_PostInc  || _tmp491 == Cyc_Absyn_PostDec)?
Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp491 == Cyc_Absyn_PreDec  || _tmp491 == Cyc_Absyn_PostDec)change=- 1;e->r=Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp552[3];_tmp552[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp552[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp552[0]=
Cyc_Toc_push_address_exp(_tmp490);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp552,sizeof(struct Cyc_Absyn_Exp*),3));}));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp553=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp490->topt))->v);enum Cyc_Absyn_Size_of _tmp555;int _tmp558;_LL28A: {struct Cyc_Absyn_IntType_struct*
_tmp554=(struct Cyc_Absyn_IntType_struct*)_tmp553;if(_tmp554->tag != 6)goto _LL28C;
else{_tmp555=_tmp554->f2;}}_LL28B: switch(_tmp555){case Cyc_Absyn_Char_sz: _LL294:
fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
break;case Cyc_Absyn_Short_sz: _LL295: fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e:
Cyc_Toc__zero_arr_inplace_plus_short_e;break;case Cyc_Absyn_Int_sz: _LL296: fn_e=
_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
break;default: _LL297:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({struct Cyc_Core_Impossible_struct
_tmp55B;_tmp55B.tag=Cyc_Core_Impossible;_tmp55B.f1=({const char*_tmp55C="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp55C,sizeof(char),44);});_tmp55B;});_tmp55A;}));}goto _LL289;
_LL28C: {struct Cyc_Absyn_FloatType_struct*_tmp556=(struct Cyc_Absyn_FloatType_struct*)
_tmp553;if(_tmp556->tag != 7)goto _LL28E;}_LL28D: fn_e=_tmp491 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL289;_LL28E: {struct Cyc_Absyn_DoubleType_struct*_tmp557=(struct Cyc_Absyn_DoubleType_struct*)
_tmp553;if(_tmp557->tag != 8)goto _LL290;else{_tmp558=_tmp557->f1;}}_LL28F: switch(
_tmp558){case 1: _LL299: fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL29A: fn_e=_tmp491 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL289;_LL290: {struct Cyc_Absyn_PointerType_struct*_tmp559=(struct Cyc_Absyn_PointerType_struct*)
_tmp553;if(_tmp559->tag != 5)goto _LL292;}_LL291: fn_e=_tmp491 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL289;_LL292:;_LL293:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Core_Impossible_struct
_tmp55E;_tmp55E.tag=Cyc_Core_Impossible;_tmp55E.f1=({const char*_tmp55F="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp55F,sizeof(char),64);});_tmp55E;});_tmp55D;}));_LL289:;}e->r=
Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp560[2];_tmp560[1]=Cyc_Absyn_signed_int_exp(
1,0);_tmp560[0]=_tmp490;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp560,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp490)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp490,0,Cyc_Toc_incr_lvalue,_tmp491);e->r=_tmp490->r;}}}goto _LL208;};};}_LL215: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp492=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp482;if(_tmp492->tag != 4)goto _LL217;else{_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;
_tmp495=_tmp492->f3;}}_LL216: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp493->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp495->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp493,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp493);Cyc_Toc_exp_to_c(nv,
_tmp495);e->r=Cyc_Toc_tagged_union_assignop(_tmp493,e1_old_typ,_tmp494,_tmp495,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp493,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp493,_tmp494,_tmp495,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp493);Cyc_Toc_exp_to_c(nv,_tmp493);Cyc_Toc_exp_to_c(nv,_tmp495);{int done=0;
if(_tmp494 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp494->v){case Cyc_Absyn_Plus: _LL29C: change=_tmp495;break;case Cyc_Absyn_Minus:
_LL29D: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp495,0);break;default:
_LL29E:({void*_tmp562=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp563="bad t ? pointer arithmetic";
_tag_dyneither(_tmp563,sizeof(char),27);}),_tag_dyneither(_tmp562,sizeof(void*),
0));});}done=1;{struct Cyc_Absyn_Exp*_tmp564=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp564,({struct Cyc_Absyn_Exp*_tmp565[3];_tmp565[2]=
change;_tmp565[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp565[0]=
Cyc_Toc_push_address_exp(_tmp493);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp565,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)
_tmp494->v){case Cyc_Absyn_Plus: _LL2A0: done=1;e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp493),({struct Cyc_Absyn_Exp*
_tmp566[2];_tmp566[1]=_tmp495;_tmp566[0]=_tmp493;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp566,sizeof(struct Cyc_Absyn_Exp*),
2));}));break;default: _LL2A1:({void*_tmp567=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp568="bad zero-terminated pointer arithmetic";
_tag_dyneither(_tmp568,sizeof(char),39);}),_tag_dyneither(_tmp567,sizeof(void*),
0));});}}}if(!done){if(e1_poly)_tmp495->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp495->r,0));if(!Cyc_Absyn_is_lvalue(_tmp493)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple18*),struct _tuple18*f_env))Cyc_Toc_lvalue_assign)(_tmp493,0,Cyc_Toc_assignop_lvalue,({
struct _tuple18*_tmp569=_cycalloc(sizeof(struct _tuple18)* 1);_tmp569[0]=({struct
_tuple18 _tmp56A;_tmp56A.f1=_tmp494;_tmp56A.f2=_tmp495;_tmp56A;});_tmp569;}));e->r=
_tmp493->r;}}goto _LL208;};};};}_LL217: {struct Cyc_Absyn_Conditional_e_struct*
_tmp496=(struct Cyc_Absyn_Conditional_e_struct*)_tmp482;if(_tmp496->tag != 6)goto
_LL219;else{_tmp497=_tmp496->f1;_tmp498=_tmp496->f2;_tmp499=_tmp496->f3;}}_LL218:
Cyc_Toc_exp_to_c(nv,_tmp497);Cyc_Toc_exp_to_c(nv,_tmp498);Cyc_Toc_exp_to_c(nv,
_tmp499);goto _LL208;_LL219: {struct Cyc_Absyn_And_e_struct*_tmp49A=(struct Cyc_Absyn_And_e_struct*)
_tmp482;if(_tmp49A->tag != 7)goto _LL21B;else{_tmp49B=_tmp49A->f1;_tmp49C=_tmp49A->f2;}}
_LL21A: Cyc_Toc_exp_to_c(nv,_tmp49B);Cyc_Toc_exp_to_c(nv,_tmp49C);goto _LL208;
_LL21B: {struct Cyc_Absyn_Or_e_struct*_tmp49D=(struct Cyc_Absyn_Or_e_struct*)
_tmp482;if(_tmp49D->tag != 8)goto _LL21D;else{_tmp49E=_tmp49D->f1;_tmp49F=_tmp49D->f2;}}
_LL21C: Cyc_Toc_exp_to_c(nv,_tmp49E);Cyc_Toc_exp_to_c(nv,_tmp49F);goto _LL208;
_LL21D: {struct Cyc_Absyn_SeqExp_e_struct*_tmp4A0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp482;if(_tmp4A0->tag != 9)goto _LL21F;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}
_LL21E: Cyc_Toc_exp_to_c(nv,_tmp4A1);Cyc_Toc_exp_to_c(nv,_tmp4A2);goto _LL208;
_LL21F: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp4A3=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp482;if(_tmp4A3->tag != 10)goto _LL221;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}}
_LL220: _tmp4A7=_tmp4A4;_tmp4A8=_tmp4A5;goto _LL222;_LL221: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4A6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp482;if(_tmp4A6->tag != 11)goto
_LL223;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;_tmp4A9=_tmp4A6->f3;if(
_tmp4A9 != 0)goto _LL223;}}_LL222: Cyc_Toc_exp_to_c(nv,_tmp4A7);((void(*)(void(*f)(
struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4A8);goto _LL208;_LL223: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4AA=(struct Cyc_Absyn_FnCall_e_struct*)_tmp482;if(_tmp4AA->tag != 11)goto
_LL225;else{_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AA->f2;_tmp4AD=_tmp4AA->f3;if(
_tmp4AD == 0)goto _LL225;_tmp4AE=*_tmp4AD;_tmp4AF=_tmp4AE.num_varargs;_tmp4B0=
_tmp4AE.injectors;_tmp4B1=_tmp4AE.vai;}}_LL224: {struct Cyc_Toc_Env _tmp56C;struct
_RegionHandle*_tmp56D;struct Cyc_Toc_Env*_tmp56B=nv;_tmp56C=*_tmp56B;_tmp56D=
_tmp56C.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4AF,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4B1->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4AC);int num_normargs=num_args - _tmp4AF;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4AC=_tmp4AC->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AC))->hd);
new_args=({struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=(
struct Cyc_Absyn_Exp*)_tmp4AC->hd;_tmp56E->tl=new_args;_tmp56E;});}}new_args=({
struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp570[3];_tmp570[2]=
num_varargs_exp;_tmp570[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp570[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp570,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp56F->tl=
new_args;_tmp56F;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4AB);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AB,new_args,0),0);if(_tmp4B1->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp571=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4B1->type));struct Cyc_Absyn_DatatypeInfo _tmp573;union Cyc_Absyn_DatatypeInfoU
_tmp574;struct Cyc_Absyn_Datatypedecl**_tmp575;struct Cyc_Absyn_Datatypedecl*
_tmp576;_LL2A4: {struct Cyc_Absyn_DatatypeType_struct*_tmp572=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp571;if(_tmp572->tag != 3)goto _LL2A6;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.datatype_info;
if((_tmp574.KnownDatatype).tag != 2)goto _LL2A6;_tmp575=(struct Cyc_Absyn_Datatypedecl**)(
_tmp574.KnownDatatype).val;_tmp576=*_tmp575;}}_LL2A5: tud=_tmp576;goto _LL2A3;
_LL2A6:;_LL2A7:({void*_tmp577=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp578="toc: unknown datatype in vararg with inject";
_tag_dyneither(_tmp578,sizeof(char),44);}),_tag_dyneither(_tmp577,sizeof(void*),
0));});_LL2A3:;}{struct _dyneither_ptr vs=({unsigned int _tmp582=(unsigned int)
_tmp4AF;struct _tuple0**_tmp583=(struct _tuple0**)_region_malloc(_tmp56D,
_check_times(sizeof(struct _tuple0*),_tmp582));struct _dyneither_ptr _tmp585=
_tag_dyneither(_tmp583,sizeof(struct _tuple0*),_tmp582);{unsigned int _tmp584=
_tmp582;unsigned int i;for(i=0;i < _tmp584;i ++){_tmp583[i]=Cyc_Toc_temp_var();}}
_tmp585;});if(_tmp4AF != 0){struct Cyc_List_List*_tmp579=0;{int i=_tmp4AF - 1;for(0;
i >= 0;-- i){_tmp579=({struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));
_tmp57A->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0));
_tmp57A->tl=_tmp579;_tmp57A;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp579,0),s,0);{int i=0;for(0;
_tmp4AC != 0;(((_tmp4AC=_tmp4AC->tl,_tmp4B0=_tmp4B0->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp4AC->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple0*var=*((struct _tuple0**)
_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp57C;struct
_tuple0*_tmp57D;struct Cyc_List_List*_tmp57E;struct Cyc_Absyn_Datatypefield*
_tmp57B=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4B0))->hd;_tmp57C=*_tmp57B;_tmp57D=_tmp57C.name;_tmp57E=_tmp57C.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp57E))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp57D),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp57D,({const char*_tmp57F="_struct";_tag_dyneither(_tmp57F,sizeof(char),8);}))),
0,s,0);};}};}else{struct Cyc_List_List*_tmp580=({struct _tuple15*_tmp581[3];
_tmp581[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp581[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0));_tmp581[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp581,sizeof(struct _tuple15*),3));});s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4AC
!= 0;(_tmp4AC=_tmp4AC->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AC->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AC->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL208;}_LL225: {struct Cyc_Absyn_Throw_e_struct*_tmp4B2=(struct Cyc_Absyn_Throw_e_struct*)
_tmp482;if(_tmp4B2->tag != 12)goto _LL227;else{_tmp4B3=_tmp4B2->f1;}}_LL226: Cyc_Toc_exp_to_c(
nv,_tmp4B3);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4B3),0))->r;goto _LL208;_LL227: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4B4=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp482;if(_tmp4B4->tag != 13)
goto _LL229;else{_tmp4B5=_tmp4B4->f1;}}_LL228: Cyc_Toc_exp_to_c(nv,_tmp4B5);goto
_LL208;_LL229: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4B6=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp482;if(_tmp4B6->tag != 14)goto _LL22B;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;}}
_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B7);for(0;_tmp4B8 != 0;_tmp4B8=_tmp4B8->tl){enum 
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4B8->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp586=Cyc_Tcutil_compress((void*)_tmp4B8->hd);_LL2A9: {struct Cyc_Absyn_VarType_struct*
_tmp587=(struct Cyc_Absyn_VarType_struct*)_tmp586;if(_tmp587->tag != 2)goto _LL2AB;}
_LL2AA: goto _LL2AC;_LL2AB: {struct Cyc_Absyn_DatatypeType_struct*_tmp588=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp586;if(_tmp588->tag != 3)goto _LL2AD;}_LL2AC:
continue;_LL2AD:;_LL2AE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4B7,0))->r;goto _LL2A8;_LL2A8:;}
break;}}goto _LL208;_LL22B: {struct Cyc_Absyn_Cast_e_struct*_tmp4B9=(struct Cyc_Absyn_Cast_e_struct*)
_tmp482;if(_tmp4B9->tag != 15)goto _LL22D;else{_tmp4BA=(void**)& _tmp4B9->f1;
_tmp4BB=(void**)((void**)& _tmp4B9->f1);_tmp4BC=_tmp4B9->f2;_tmp4BD=_tmp4B9->f3;
_tmp4BE=_tmp4B9->f4;}}_LL22C: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BC->topt))->v;void*new_typ=*_tmp4BB;*_tmp4BB=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BC);{struct _tuple17 _tmp58A=({struct _tuple17
_tmp589;_tmp589.f1=Cyc_Tcutil_compress(old_t2);_tmp589.f2=Cyc_Tcutil_compress(
new_typ);_tmp589;});void*_tmp58B;struct Cyc_Absyn_PtrInfo _tmp58D;void*_tmp58E;
struct Cyc_Absyn_PtrInfo _tmp590;void*_tmp591;struct Cyc_Absyn_PtrInfo _tmp593;void*
_tmp594;_LL2B0: _tmp58B=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58C=(
struct Cyc_Absyn_PointerType_struct*)_tmp58B;if(_tmp58C->tag != 5)goto _LL2B2;else{
_tmp58D=_tmp58C->f1;}};_tmp58E=_tmp58A.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp58F=(struct Cyc_Absyn_PointerType_struct*)_tmp58E;if(_tmp58F->tag != 5)goto
_LL2B2;else{_tmp590=_tmp58F->f1;}};_LL2B1: {int _tmp596=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp58D.ptr_atts).nullable);int _tmp597=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).nullable);
void*_tmp598=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp58D.ptr_atts).bounds);void*_tmp599=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp590.ptr_atts).bounds);
int _tmp59A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).zero_term);int _tmp59B=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).zero_term);{struct _tuple17 _tmp59D=({
struct _tuple17 _tmp59C;_tmp59C.f1=_tmp598;_tmp59C.f2=_tmp599;_tmp59C;});void*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;
void*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AE;void*_tmp5B0;_LL2B7: _tmp59E=_tmp59D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_struct*)_tmp59E;
if(_tmp59F->tag != 1)goto _LL2B9;else{_tmp5A0=_tmp59F->f1;}};_tmp5A1=_tmp59D.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A1;
if(_tmp5A2->tag != 1)goto _LL2B9;else{_tmp5A3=_tmp5A2->f1;}};_LL2B8: if((!Cyc_Evexp_c_can_eval(
_tmp5A0) || !Cyc_Evexp_c_can_eval(_tmp5A3)) && !Cyc_Evexp_same_const_exp(_tmp5A0,
_tmp5A3))({void*_tmp5B2=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B3="can't validate cast due to potential size differences";
_tag_dyneither(_tmp5B3,sizeof(char),54);}),_tag_dyneither(_tmp5B2,sizeof(void*),
0));});if(_tmp596  && !_tmp597){if(Cyc_Toc_is_toplevel(nv))({void*_tmp5B4=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5B5="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp5B5,
sizeof(char),44);}),_tag_dyneither(_tmp5B4,sizeof(void*),0));});if(_tmp4BE != Cyc_Absyn_NonNull_to_Null)({
struct Cyc_String_pa_struct _tmp5B8;_tmp5B8.tag=0;_tmp5B8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5B6[1]={& _tmp5B8};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp5B7="null-check conversion mis-classified: %s";_tag_dyneither(_tmp5B7,
sizeof(char),41);}),_tag_dyneither(_tmp5B6,sizeof(void*),1));});});{int
do_null_check=Cyc_Toc_need_null_check(_tmp4BC);if(do_null_check){if(!_tmp4BD)({
void*_tmp5B9=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5BA="inserted null check due to implicit cast from * to @ type";
_tag_dyneither(_tmp5BA,sizeof(char),58);}),_tag_dyneither(_tmp5B9,sizeof(void*),
0));});e->r=Cyc_Toc_cast_it_r(*_tmp4BB,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=_tmp4BC;
_tmp5BB->tl=0;_tmp5BB;}),0));}};}goto _LL2B6;_LL2B9: _tmp5A4=_tmp59D.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp5A5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A4;if(_tmp5A5->tag != 1)goto _LL2BB;
else{_tmp5A6=_tmp5A5->f1;}};_tmp5A7=_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5A8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A7;if(_tmp5A8->tag != 0)goto
_LL2BB;};_LL2BA: if(!Cyc_Evexp_c_can_eval(_tmp5A6))({void*_tmp5BC=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp5BD="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp5BD,sizeof(char),71);}),_tag_dyneither(_tmp5BC,sizeof(void*),
0));});if(_tmp4BE == Cyc_Absyn_NonNull_to_Null)({struct Cyc_String_pa_struct
_tmp5C0;_tmp5C0.tag=0;_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp5BE[1]={& _tmp5C0};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5BF="conversion mis-classified as null-check: %s";
_tag_dyneither(_tmp5BF,sizeof(char),44);}),_tag_dyneither(_tmp5BE,sizeof(void*),
1));});});if(Cyc_Toc_is_toplevel(nv)){if((_tmp59A  && !(_tmp590.elt_tq).real_const)
 && !_tmp59B)_tmp5A6=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5A6,Cyc_Absyn_uint_exp(
1,0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5A6,_tmp4BC))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Toc__tag_dyneither_e;if(_tmp59A){struct _tuple0*
_tmp5C2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Absyn_var_exp(_tmp5C2,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5C4=_tmp4BC->r;union Cyc_Absyn_Cnst _tmp5C6;
struct _dyneither_ptr _tmp5C7;_LL2C0: {struct Cyc_Absyn_Const_e_struct*_tmp5C5=(
struct Cyc_Absyn_Const_e_struct*)_tmp5C4;if(_tmp5C5->tag != 0)goto _LL2C2;else{
_tmp5C6=_tmp5C5->f1;if((_tmp5C6.String_c).tag != 7)goto _LL2C2;_tmp5C7=(struct
_dyneither_ptr)(_tmp5C6.String_c).val;}}_LL2C1: arg3=_tmp5A6;goto _LL2BF;_LL2C2:;
_LL2C3: arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,
_tmp4BC),({struct Cyc_Absyn_Exp*_tmp5C8[2];_tmp5C8[1]=_tmp5A6;_tmp5C8[0]=_tmp5C3;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp5C8,sizeof(struct Cyc_Absyn_Exp*),2));}),0);goto _LL2BF;_LL2BF:;}if(!_tmp59B
 && !(_tmp590.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,
Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp590.elt_typ),0);struct Cyc_Absyn_Exp*_tmp5CA=Cyc_Absyn_fncall_exp(
_tmp5C1,({struct Cyc_Absyn_Exp*_tmp5CC[3];_tmp5CC[2]=arg3;_tmp5CC[1]=_tmp5C9;
_tmp5CC[0]=_tmp5C3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5CC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp5CB=Cyc_Absyn_exp_stmt(_tmp5CA,0);_tmp5CB=Cyc_Absyn_declare_stmt(_tmp5C2,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp4BC,_tmp5CB,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CB);};}
else{e->r=Cyc_Toc_fncall_exp_r(_tmp5C1,({struct Cyc_Absyn_Exp*_tmp5CD[3];_tmp5CD[
2]=_tmp5A6;_tmp5CD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp590.elt_typ),
0);_tmp5CD[0]=_tmp4BC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5CD,sizeof(struct Cyc_Absyn_Exp*),3));}));}}goto _LL2B6;_LL2BB:
_tmp5A9=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A9;if(_tmp5AA->tag != 0)goto _LL2BD;};_tmp5AB=_tmp59D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AC=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AB;if(_tmp5AC->tag != 1)goto _LL2BD;
else{_tmp5AD=_tmp5AC->f1;}};_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5AD))({void*
_tmp5CE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5CF="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp5CF,sizeof(char),71);}),_tag_dyneither(_tmp5CE,sizeof(void*),
0));});if(Cyc_Toc_is_toplevel(nv))({void*_tmp5D0=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5D1="can't coerce t? to t* or t@ at the top-level";
_tag_dyneither(_tmp5D1,sizeof(char),45);}),_tag_dyneither(_tmp5D0,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp5D2=_tmp5AD;if(_tmp59A  && !_tmp59B)_tmp5D2=Cyc_Absyn_add_exp(
_tmp5AD,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Absyn_fncall_exp(_tmp5D3,({struct Cyc_Absyn_Exp*
_tmp5D6[3];_tmp5D6[2]=_tmp5D2;_tmp5D6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58D.elt_typ),0);_tmp5D6[0]=_tmp4BC;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D6,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp597)_tmp5D4->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=Cyc_Absyn_copy_exp(
_tmp5D4);_tmp5D5->tl=0;_tmp5D5;}));e->r=Cyc_Toc_cast_it_r(*_tmp4BB,_tmp5D4);goto
_LL2B6;};};_LL2BD: _tmp5AE=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp5AE;if(_tmp5AF->tag != 0)goto _LL2B6;};
_tmp5B0=_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5B0;if(_tmp5B1->tag != 0)goto _LL2B6;};_LL2BE: DynCast: if((_tmp59A  && !_tmp59B)
 && !(_tmp590.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({void*_tmp5D7=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp5D8="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dyneither(_tmp5D8,sizeof(char),70);}),_tag_dyneither(_tmp5D7,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__dyneither_ptr_decrease_size_e;e->r=
Cyc_Toc_fncall_exp_r(_tmp5D9,({struct Cyc_Absyn_Exp*_tmp5DA[3];_tmp5DA[2]=Cyc_Absyn_uint_exp(
1,0);_tmp5DA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58D.elt_typ),0);
_tmp5DA[0]=_tmp4BC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5DA,sizeof(struct Cyc_Absyn_Exp*),3));}));};}goto _LL2B6;_LL2B6:;}
goto _LL2AF;}_LL2B2: _tmp591=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp592=(struct Cyc_Absyn_PointerType_struct*)_tmp591;if(_tmp592->tag != 5)goto
_LL2B4;else{_tmp593=_tmp592->f1;}};_tmp594=_tmp58A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp595=(struct Cyc_Absyn_IntType_struct*)_tmp594;if(_tmp595->tag != 6)goto _LL2B4;};
_LL2B3:{void*_tmp5DB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp593.ptr_atts).bounds);_LL2C5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5DC=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5DB;if(_tmp5DC->tag != 0)goto
_LL2C7;}_LL2C6: _tmp4BC->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BC->r,
_tmp4BC->loc),Cyc_Toc_curr_sp);goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}
goto _LL2AF;_LL2B4:;_LL2B5: goto _LL2AF;_LL2AF:;}goto _LL208;}_LL22D: {struct Cyc_Absyn_Address_e_struct*
_tmp4BF=(struct Cyc_Absyn_Address_e_struct*)_tmp482;if(_tmp4BF->tag != 16)goto
_LL22F;else{_tmp4C0=_tmp4BF->f1;}}_LL22E:{void*_tmp5DD=_tmp4C0->r;struct _tuple0*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*
_tmp5E3;_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5DE=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5DD;if(_tmp5DE->tag != 29)goto _LL2CC;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;
_tmp5E1=_tmp5DE->f3;}}_LL2CB: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp5E6;_tmp5E6.tag=0;_tmp5E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C0->loc));({void*_tmp5E4[1]={& _tmp5E6};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5E5="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5E5,
sizeof(char),42);}),_tag_dyneither(_tmp5E4,sizeof(void*),1));});});e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C0->topt))->v,_tmp5E0 != 0,1,0,
_tmp5E1,_tmp5DF))->r;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E2=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DD;if(_tmp5E2->tag != 25)goto _LL2CE;else{
_tmp5E3=_tmp5E2->f1;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp5E9;_tmp5E9.tag=0;_tmp5E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C0->loc));({void*_tmp5E7[1]={& _tmp5E9};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5E8="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5E8,
sizeof(char),42);}),_tag_dyneither(_tmp5E7,sizeof(void*),1));});});e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E3))->r;goto _LL2C9;_LL2CE:;_LL2CF: Cyc_Toc_exp_to_c(nv,_tmp4C0);if(!
Cyc_Absyn_is_lvalue(_tmp4C0)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4C0,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4C0);}goto _LL2C9;
_LL2C9:;}goto _LL208;_LL22F: {struct Cyc_Absyn_New_e_struct*_tmp4C1=(struct Cyc_Absyn_New_e_struct*)
_tmp482;if(_tmp4C1->tag != 17)goto _LL231;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;}}
_LL230: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp5EC;_tmp5EC.tag=
0;_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp4C3->loc));({void*_tmp5EA[1]={& _tmp5EC};((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5EB="%s: new at top-level";
_tag_dyneither(_tmp5EB,sizeof(char),21);}),_tag_dyneither(_tmp5EA,sizeof(void*),
1));});});{void*_tmp5ED=_tmp4C3->r;struct Cyc_List_List*_tmp5EF;struct Cyc_Absyn_Vardecl*
_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;int _tmp5F4;
struct _tuple0*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_List_List*_tmp5F8;
struct Cyc_Absyn_Aggrdecl*_tmp5F9;struct Cyc_List_List*_tmp5FB;_LL2D1: {struct Cyc_Absyn_Array_e_struct*
_tmp5EE=(struct Cyc_Absyn_Array_e_struct*)_tmp5ED;if(_tmp5EE->tag != 27)goto _LL2D3;
else{_tmp5EF=_tmp5EE->f1;}}_LL2D2: {struct _tuple0*_tmp5FC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Absyn_var_exp(_tmp5FC,0);struct Cyc_Absyn_Stmt*
_tmp5FE=Cyc_Toc_init_array(nv,_tmp5FD,_tmp5EF,Cyc_Absyn_exp_stmt(_tmp5FD,0));
void*old_elt_typ;{void*_tmp5FF=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp601;void*_tmp602;struct Cyc_Absyn_Tqual _tmp603;struct Cyc_Absyn_PtrAtts _tmp604;
union Cyc_Absyn_Constraint*_tmp605;_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp600=(struct Cyc_Absyn_PointerType_struct*)_tmp5FF;if(_tmp600->tag != 5)goto
_LL2DE;else{_tmp601=_tmp600->f1;_tmp602=_tmp601.elt_typ;_tmp603=_tmp601.elt_tq;
_tmp604=_tmp601.ptr_atts;_tmp605=_tmp604.zero_term;}}_LL2DD: old_elt_typ=_tmp602;
goto _LL2DB;_LL2DE:;_LL2DF: old_elt_typ=({void*_tmp606=0;((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp607="exp_to_c:new array expression doesn't have ptr type";
_tag_dyneither(_tmp607,sizeof(char),52);}),_tag_dyneither(_tmp606,sizeof(void*),
0));});_LL2DB:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp608=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5EF),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C2 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp609);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C2;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp609);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FC,_tmp608,(struct Cyc_Absyn_Exp*)e1,_tmp5FE,0));goto
_LL2D0;};}_LL2D3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F0=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5ED;if(_tmp5F0->tag != 28)goto _LL2D5;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;
_tmp5F3=_tmp5F0->f3;_tmp5F4=_tmp5F0->f4;}}_LL2D4: {int is_dyneither_ptr=0;{void*
_tmp60A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp60C;void*_tmp60D;
struct Cyc_Absyn_Tqual _tmp60E;struct Cyc_Absyn_PtrAtts _tmp60F;union Cyc_Absyn_Constraint*
_tmp610;union Cyc_Absyn_Constraint*_tmp611;_LL2E1: {struct Cyc_Absyn_PointerType_struct*
_tmp60B=(struct Cyc_Absyn_PointerType_struct*)_tmp60A;if(_tmp60B->tag != 5)goto
_LL2E3;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60C.elt_typ;_tmp60E=_tmp60C.elt_tq;
_tmp60F=_tmp60C.ptr_atts;_tmp610=_tmp60F.bounds;_tmp611=_tmp60F.zero_term;}}
_LL2E2: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp610)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E0;
_LL2E3:;_LL2E4:({void*_tmp612=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp613="exp_to_c: comprehension not an array type";
_tag_dyneither(_tmp613,sizeof(char),42);}),_tag_dyneither(_tmp612,sizeof(void*),
0));});_LL2E0:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F3->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F2);{struct Cyc_Absyn_Exp*_tmp614=
Cyc_Absyn_var_exp(max,0);if(_tmp5F4)_tmp614=Cyc_Absyn_add_exp(_tmp614,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F1,Cyc_Absyn_var_exp(max,0),_tmp5F3,_tmp5F4,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp616;struct _RegionHandle*_tmp617;struct Cyc_Toc_Env*_tmp615=
nv;_tmp616=*_tmp615;_tmp617=_tmp616.rgn;{struct Cyc_List_List*decls=({struct Cyc_List_List*
_tmp628=_region_malloc(_tmp617,sizeof(*_tmp628));_tmp628->hd=({struct _tuple19*
_tmp629=_region_malloc(_tmp617,sizeof(*_tmp629));_tmp629->f1=max;_tmp629->f2=Cyc_Absyn_uint_typ;
_tmp629->f3=(struct Cyc_Absyn_Exp*)_tmp5F2;_tmp629;});_tmp628->tl=0;_tmp628;});
struct Cyc_Absyn_Exp*ai;if(_tmp4C2 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp618[2];_tmp618[1]=_tmp614;_tmp618[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp618,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp4C2;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp619[2];_tmp619[1]=_tmp614;
_tmp619[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp619,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp61A=_region_malloc(_tmp617,sizeof(*_tmp61A));_tmp61A->hd=({
struct _tuple19*_tmp61B=_region_malloc(_tmp617,sizeof(*_tmp61B));_tmp61B->f1=a;
_tmp61B->f2=ptr_typ;_tmp61B->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp61B;});_tmp61A->tl=
decls;_tmp61A;});if(is_dyneither_ptr){struct _tuple0*_tmp61C=Cyc_Toc_temp_var();
void*_tmp61D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_fncall_exp(_tmp61E,({struct Cyc_Absyn_Exp*
_tmp622[3];_tmp622[2]=_tmp614;_tmp622[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);
_tmp622[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp622,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
decls=({struct Cyc_List_List*_tmp620=_region_malloc(_tmp617,sizeof(*_tmp620));
_tmp620->hd=({struct _tuple19*_tmp621=_region_malloc(_tmp617,sizeof(*_tmp621));
_tmp621->f1=_tmp61C;_tmp621->f2=_tmp61D;_tmp621->f3=(struct Cyc_Absyn_Exp*)
_tmp61F;_tmp621;});_tmp620->tl=decls;_tmp620;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp61C,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(a,0),0),0);}{struct Cyc_List_List*_tmp623=decls;for(0;_tmp623 != 
0;_tmp623=_tmp623->tl){struct _tuple0*_tmp625;void*_tmp626;struct Cyc_Absyn_Exp*
_tmp627;struct _tuple19 _tmp624=*((struct _tuple19*)_tmp623->hd);_tmp625=_tmp624.f1;
_tmp626=_tmp624.f2;_tmp627=_tmp624.f3;s=Cyc_Absyn_declare_stmt(_tmp625,_tmp626,
_tmp627,s,0);}}e->r=Cyc_Toc_stmt_exp_r(s);};}goto _LL2D0;};};};}_LL2D5: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp5F5=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5ED;if(_tmp5F5->tag != 29)goto _LL2D7;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;
_tmp5F8=_tmp5F5->f3;_tmp5F9=_tmp5F5->f4;}}_LL2D6: e->r=(Cyc_Toc_init_struct(nv,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C3->topt))->v,_tmp5F7 != 0,1,_tmp4C2,
_tmp5F8,_tmp5F6))->r;goto _LL2D0;_LL2D7: {struct Cyc_Absyn_Tuple_e_struct*_tmp5FA=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5ED;if(_tmp5FA->tag != 25)goto _LL2D9;else{
_tmp5FB=_tmp5FA->f1;}}_LL2D8: e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4C2,_tmp5FB))->r;
goto _LL2D0;_LL2D9:;_LL2DA: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C3->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple0*_tmp62A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp62B=Cyc_Absyn_var_exp(
_tmp62A,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp62B,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4C2 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C2;Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp62C=_tmp4C3->r;void*_tmp62E;struct Cyc_Absyn_Exp*_tmp62F;_LL2E6: {struct
Cyc_Absyn_Cast_e_struct*_tmp62D=(struct Cyc_Absyn_Cast_e_struct*)_tmp62C;if(
_tmp62D->tag != 15)goto _LL2E8;else{_tmp62E=(void*)_tmp62D->f1;_tmp62F=_tmp62D->f2;}}
_LL2E7:{struct _tuple17 _tmp631=({struct _tuple17 _tmp630;_tmp630.f1=Cyc_Tcutil_compress(
_tmp62E);_tmp630.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp62F->topt))->v);_tmp630;});void*_tmp632;struct Cyc_Absyn_PtrInfo _tmp634;void*
_tmp635;struct Cyc_Absyn_PtrAtts _tmp636;union Cyc_Absyn_Constraint*_tmp637;void*
_tmp638;struct Cyc_Absyn_PtrInfo _tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;union Cyc_Absyn_Constraint*
_tmp63C;_LL2EB: _tmp632=_tmp631.f1;{struct Cyc_Absyn_PointerType_struct*_tmp633=(
struct Cyc_Absyn_PointerType_struct*)_tmp632;if(_tmp633->tag != 5)goto _LL2ED;else{
_tmp634=_tmp633->f1;_tmp635=_tmp634.elt_typ;_tmp636=_tmp634.ptr_atts;_tmp637=
_tmp636.bounds;}};_tmp638=_tmp631.f2;{struct Cyc_Absyn_PointerType_struct*_tmp639=(
struct Cyc_Absyn_PointerType_struct*)_tmp638;if(_tmp639->tag != 5)goto _LL2ED;else{
_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.ptr_atts;_tmp63C=_tmp63B.bounds;}};_LL2EC:{
struct _tuple17 _tmp63E=({struct _tuple17 _tmp63D;_tmp63D.f1=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp637);
_tmp63D.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63C);_tmp63D;});void*_tmp63F;void*_tmp641;struct Cyc_Absyn_Exp*
_tmp643;_LL2F0: _tmp63F=_tmp63E.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp640=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp63F;if(_tmp640->tag != 0)goto _LL2F2;};
_tmp641=_tmp63E.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp642=(struct Cyc_Absyn_Upper_b_struct*)
_tmp641;if(_tmp642->tag != 1)goto _LL2F2;else{_tmp643=_tmp642->f1;}};_LL2F1: Cyc_Toc_exp_to_c(
nv,_tmp62F);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp644=Cyc_Toc__init_dyneither_ptr_e;e->r=Cyc_Toc_fncall_exp_r(_tmp644,({struct
Cyc_Absyn_Exp*_tmp645[4];_tmp645[3]=_tmp643;_tmp645[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp635),0);_tmp645[1]=_tmp62F;_tmp645[0]=mexp;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp645,sizeof(struct Cyc_Absyn_Exp*),
4));}));goto _LL2EF;};_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}goto _LL2EA;_LL2ED:;
_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2E5;_LL2E8:;_LL2E9: goto _LL2E5;_LL2E5:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_exp_stmt(_tmp62B,0);struct Cyc_Absyn_Exp*
_tmp647=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C3);_tmp646=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62B,_tmp647,0),_tmp4C3,0),
_tmp646,0);{void*_tmp648=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp62A,_tmp648,(struct Cyc_Absyn_Exp*)mexp,_tmp646,0));};}
goto _LL2D0;};}_LL2D0:;}goto _LL208;_LL231: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C4=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp482;if(_tmp4C4->tag != 19)goto
_LL233;else{_tmp4C5=_tmp4C4->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C5);goto _LL208;
_LL233: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C6=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp482;if(_tmp4C6->tag != 18)goto _LL235;else{_tmp4C7=(void*)_tmp4C6->f1;}}_LL234:
e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp649=_cycalloc(sizeof(*
_tmp649));_tmp649[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmp64A;_tmp64A.tag=18;
_tmp64A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C7);_tmp64A;});_tmp649;});goto
_LL208;_LL235: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4C8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp482;if(_tmp4C8->tag != 20)goto _LL237;else{_tmp4C9=(void*)_tmp4C8->f1;_tmp4CA=(
void*)_tmp4C8->f2;{struct Cyc_Absyn_StructField_struct*_tmp4CB=(struct Cyc_Absyn_StructField_struct*)
_tmp4CA;if(_tmp4CB->tag != 0)goto _LL237;else{_tmp4CC=_tmp4CB->f1;}};}}_LL236: e->r=(
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));
_tmp64B[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp64C;_tmp64C.tag=20;_tmp64C.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp4C9);_tmp64C.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_StructField_struct
_tmp64E;_tmp64E.tag=0;_tmp64E.f1=_tmp4CC;_tmp64E;});_tmp64D;}));_tmp64C;});
_tmp64B;});goto _LL208;_LL237: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4CD=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp482;if(_tmp4CD->tag != 20)goto _LL239;else{
_tmp4CE=(void*)_tmp4CD->f1;_tmp4CF=(void*)_tmp4CD->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp4D0=(struct Cyc_Absyn_TupleIndex_struct*)_tmp4CF;if(_tmp4D0->tag != 1)goto
_LL239;else{_tmp4D1=_tmp4D0->f1;}};}}_LL238:{void*_tmp64F=Cyc_Tcutil_compress(
_tmp4CE);struct Cyc_Absyn_AggrInfo _tmp651;union Cyc_Absyn_AggrInfoU _tmp652;struct
Cyc_List_List*_tmp654;_LL2F5: {struct Cyc_Absyn_AggrType_struct*_tmp650=(struct
Cyc_Absyn_AggrType_struct*)_tmp64F;if(_tmp650->tag != 12)goto _LL2F7;else{_tmp651=
_tmp650->f1;_tmp652=_tmp651.aggr_info;}}_LL2F6: {struct Cyc_Absyn_Aggrdecl*
_tmp657=Cyc_Absyn_get_known_aggrdecl(_tmp652);if(_tmp657->impl == 0)({void*
_tmp658=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp659="struct fields must be known";_tag_dyneither(_tmp659,sizeof(
char),28);}),_tag_dyneither(_tmp658,sizeof(void*),0));});_tmp654=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp657->impl))->fields;goto _LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp653=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp64F;if(_tmp653->tag != 13)goto
_LL2F9;else{_tmp654=_tmp653->f2;}}_LL2F8: {struct Cyc_Absyn_Aggrfield*_tmp65A=((
struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp654,(
int)_tmp4D1);e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp65B=_cycalloc(
sizeof(*_tmp65B));_tmp65B[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp65C;_tmp65C.tag=
20;_tmp65C.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CE);_tmp65C.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({
struct Cyc_Absyn_StructField_struct _tmp65E;_tmp65E.tag=0;_tmp65E.f1=_tmp65A->name;
_tmp65E;});_tmp65D;}));_tmp65C;});_tmp65B;});goto _LL2F4;}_LL2F9: {struct Cyc_Absyn_TupleType_struct*
_tmp655=(struct Cyc_Absyn_TupleType_struct*)_tmp64F;if(_tmp655->tag != 11)goto
_LL2FB;}_LL2FA: e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp65F=_cycalloc(
sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp660;_tmp660.tag=
20;_tmp660.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CE);_tmp660.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661[0]=({
struct Cyc_Absyn_StructField_struct _tmp662;_tmp662.tag=0;_tmp662.f1=Cyc_Absyn_fieldname((
int)(_tmp4D1 + 1));_tmp662;});_tmp661;}));_tmp660;});_tmp65F;});goto _LL2F4;_LL2FB: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp656=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp64F;if(_tmp656->tag != 4)goto _LL2FD;}_LL2FC: if(_tmp4D1 == 0)e->r=(void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp664;_tmp664.tag=20;_tmp664.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp4CE);_tmp664.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp665=
_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_Absyn_StructField_struct
_tmp666;_tmp666.tag=0;_tmp666.f1=Cyc_Toc_tag_sp;_tmp666;});_tmp665;}));_tmp664;});
_tmp663;});else{e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp667=
_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp668;_tmp668.tag=20;_tmp668.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CE);_tmp668.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp669=_cycalloc(sizeof(*
_tmp669));_tmp669[0]=({struct Cyc_Absyn_StructField_struct _tmp66A;_tmp66A.tag=0;
_tmp66A.f1=Cyc_Absyn_fieldname((int)_tmp4D1);_tmp66A;});_tmp669;}));_tmp668;});
_tmp667;});}goto _LL2F4;_LL2FD:;_LL2FE:({void*_tmp66B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp66C="impossible type for offsetof tuple index";
_tag_dyneither(_tmp66C,sizeof(char),41);}),_tag_dyneither(_tmp66B,sizeof(void*),
0));});_LL2F4:;}goto _LL208;_LL239: {struct Cyc_Absyn_Deref_e_struct*_tmp4D2=(
struct Cyc_Absyn_Deref_e_struct*)_tmp482;if(_tmp4D2->tag != 21)goto _LL23B;else{
_tmp4D3=_tmp4D2->f1;}}_LL23A: {void*_tmp66D=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D3->topt))->v);{void*_tmp66E=_tmp66D;struct Cyc_Absyn_PtrInfo
_tmp670;void*_tmp671;struct Cyc_Absyn_Tqual _tmp672;struct Cyc_Absyn_PtrAtts _tmp673;
void*_tmp674;union Cyc_Absyn_Constraint*_tmp675;union Cyc_Absyn_Constraint*_tmp676;
union Cyc_Absyn_Constraint*_tmp677;_LL300: {struct Cyc_Absyn_PointerType_struct*
_tmp66F=(struct Cyc_Absyn_PointerType_struct*)_tmp66E;if(_tmp66F->tag != 5)goto
_LL302;else{_tmp670=_tmp66F->f1;_tmp671=_tmp670.elt_typ;_tmp672=_tmp670.elt_tq;
_tmp673=_tmp670.ptr_atts;_tmp674=_tmp673.rgn;_tmp675=_tmp673.nullable;_tmp676=
_tmp673.bounds;_tmp677=_tmp673.zero_term;}}_LL301:{void*_tmp678=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp676);struct Cyc_Absyn_Exp*_tmp67A;_LL305: {struct Cyc_Absyn_Upper_b_struct*
_tmp679=(struct Cyc_Absyn_Upper_b_struct*)_tmp678;if(_tmp679->tag != 1)goto _LL307;
else{_tmp67A=_tmp679->f1;}}_LL306: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4D3);Cyc_Toc_exp_to_c(nv,_tmp4D3);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({
void*_tmp67C=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp67D="inserted null check due to dereference";
_tag_dyneither(_tmp67D,sizeof(char),39);}),_tag_dyneither(_tmp67C,sizeof(void*),
0));});_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp66D),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));
_tmp67E->hd=Cyc_Absyn_copy_exp(_tmp4D3);_tmp67E->tl=0;_tmp67E;}),0));}if(!((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp677)){unsigned int
_tmp680;int _tmp681;struct _tuple10 _tmp67F=Cyc_Evexp_eval_const_uint_exp(_tmp67A);
_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;if(!_tmp681  || _tmp680 <= 0)({void*_tmp682=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmp683="cannot determine dereference is in bounds";
_tag_dyneither(_tmp683,sizeof(char),42);}),_tag_dyneither(_tmp682,sizeof(void*),
0));});}goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_struct*_tmp67B=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp678;if(_tmp67B->tag != 0)goto _LL304;}_LL308: {
struct Cyc_Absyn_Exp*_tmp684=Cyc_Absyn_uint_exp(0,0);_tmp684->topt=({struct Cyc_Core_Opt*
_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->v=Cyc_Absyn_uint_typ;_tmp685;});e->r=
Cyc_Toc_subscript_exp_r(_tmp4D3,_tmp684);Cyc_Toc_exp_to_c(nv,e);goto _LL304;}
_LL304:;}goto _LL2FF;_LL302:;_LL303:({void*_tmp686=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp687="exp_to_c: Deref: non-pointer";
_tag_dyneither(_tmp687,sizeof(char),29);}),_tag_dyneither(_tmp686,sizeof(void*),
0));});_LL2FF:;}goto _LL208;}_LL23B: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D4=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp482;if(_tmp4D4->tag != 22)goto _LL23D;
else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;_tmp4D7=_tmp4D4->f3;_tmp4D8=_tmp4D4->f4;}}
_LL23C: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D5->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4D5);if(
_tmp4D7  && _tmp4D8)e->r=Cyc_Toc_check_tagged_union(_tmp4D5,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4D6,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;}_LL23D: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4D9=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp482;if(_tmp4D9->tag != 23)goto _LL23F;
else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;_tmp4DC=_tmp4D9->f3;_tmp4DD=_tmp4D9->f4;}}
_LL23E: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4DA->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4DA);Cyc_Toc_exp_to_c(
nv,_tmp4DA);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp689;struct Cyc_Absyn_Tqual
_tmp68A;struct Cyc_Absyn_PtrAtts _tmp68B;void*_tmp68C;union Cyc_Absyn_Constraint*
_tmp68D;union Cyc_Absyn_Constraint*_tmp68E;union Cyc_Absyn_Constraint*_tmp68F;
struct Cyc_Absyn_PtrInfo _tmp688=Cyc_Toc_get_ptr_type(e1typ);_tmp689=_tmp688.elt_typ;
_tmp68A=_tmp688.elt_tq;_tmp68B=_tmp688.ptr_atts;_tmp68C=_tmp68B.rgn;_tmp68D=
_tmp68B.nullable;_tmp68E=_tmp68B.bounds;_tmp68F=_tmp68B.zero_term;{void*_tmp690=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp68E);struct Cyc_Absyn_Exp*_tmp692;_LL30A: {struct Cyc_Absyn_Upper_b_struct*
_tmp691=(struct Cyc_Absyn_Upper_b_struct*)_tmp690;if(_tmp691->tag != 1)goto _LL30C;
else{_tmp692=_tmp691->f1;}}_LL30B: {unsigned int _tmp695;int _tmp696;struct
_tuple10 _tmp694=Cyc_Evexp_eval_const_uint_exp(_tmp692);_tmp695=_tmp694.f1;
_tmp696=_tmp694.f2;if(_tmp696){if(_tmp695 < 1)({void*_tmp697=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp698="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_dyneither(_tmp698,sizeof(char),44);}),_tag_dyneither(_tmp697,sizeof(void*),
0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp699=0;Cyc_Tcutil_warn(
e->loc,({const char*_tmp69A="inserted null check due to dereference";
_tag_dyneither(_tmp69A,sizeof(char),39);}),_tag_dyneither(_tmp699,sizeof(void*),
0));});_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp69B[1];_tmp69B[0]=Cyc_Absyn_new_exp(
_tmp4DA->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp69B,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{if(!Cyc_Evexp_c_can_eval(
_tmp692))({void*_tmp69C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp69D="cannot determine pointer dereference in bounds";
_tag_dyneither(_tmp69D,sizeof(char),47);}),_tag_dyneither(_tmp69C,sizeof(void*),
0));});_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp69E[4];_tmp69E[3]=
Cyc_Absyn_uint_exp(0,0);_tmp69E[2]=Cyc_Absyn_sizeoftyp_exp(_tmp689,0);_tmp69E[1]=
_tmp692;_tmp69E[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp69E,sizeof(struct Cyc_Absyn_Exp*),
4));}),0));}goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_struct*_tmp693=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp690;if(_tmp693->tag != 0)goto _LL309;}
_LL30D: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp689);_tmp4DA->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp68A),Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({
struct Cyc_Absyn_Exp*_tmp69F[3];_tmp69F[2]=Cyc_Absyn_uint_exp(0,0);_tmp69F[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp69F[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp69F,sizeof(struct Cyc_Absyn_Exp*),
3));}),0));goto _LL309;}_LL309:;}if(_tmp4DC  && _tmp4DD)e->r=Cyc_Toc_check_tagged_union(
_tmp4DA,Cyc_Toc_typ_to_c(e1typ),_tmp689,_tmp4DB,Cyc_Absyn_aggrarrow_exp);if(
is_poly  && _tmp4DD)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL208;};}_LL23F: {struct Cyc_Absyn_Subscript_e_struct*_tmp4DE=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp482;if(_tmp4DE->tag != 24)goto _LL241;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}
_LL240: {void*_tmp6A0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4DF->topt))->v);{void*_tmp6A1=_tmp6A0;struct Cyc_List_List*
_tmp6A3;struct Cyc_Absyn_PtrInfo _tmp6A5;void*_tmp6A6;struct Cyc_Absyn_Tqual _tmp6A7;
struct Cyc_Absyn_PtrAtts _tmp6A8;void*_tmp6A9;union Cyc_Absyn_Constraint*_tmp6AA;
union Cyc_Absyn_Constraint*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;_LL30F: {
struct Cyc_Absyn_TupleType_struct*_tmp6A2=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A1;if(_tmp6A2->tag != 11)goto _LL311;else{_tmp6A3=_tmp6A2->f1;}}_LL310: Cyc_Toc_exp_to_c(
nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);{unsigned int _tmp6AE;int _tmp6AF;struct
_tuple10 _tmp6AD=Cyc_Evexp_eval_const_uint_exp(_tmp4E0);_tmp6AE=_tmp6AD.f1;
_tmp6AF=_tmp6AD.f2;if(!_tmp6AF)({void*_tmp6B0=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6B1="unknown tuple subscript in translation to C";
_tag_dyneither(_tmp6B1,sizeof(char),44);}),_tag_dyneither(_tmp6B0,sizeof(void*),
0));});e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DF,Cyc_Absyn_fieldname((int)(_tmp6AE + 
1)));goto _LL30E;};_LL311: {struct Cyc_Absyn_PointerType_struct*_tmp6A4=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A1;if(_tmp6A4->tag != 5)goto _LL313;else{_tmp6A5=_tmp6A4->f1;_tmp6A6=_tmp6A5.elt_typ;
_tmp6A7=_tmp6A5.elt_tq;_tmp6A8=_tmp6A5.ptr_atts;_tmp6A9=_tmp6A8.rgn;_tmp6AA=
_tmp6A8.nullable;_tmp6AB=_tmp6A8.bounds;_tmp6AC=_tmp6A8.zero_term;}}_LL312: {
struct Cyc_List_List*_tmp6B2=Cyc_Toc_get_relns(_tmp4DF);int in_bnds=0;{void*
_tmp6B3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AB);_LL316:;_LL317: in_bnds=Cyc_Toc_check_bounds(_tmp6A0,_tmp6B2,_tmp4DF,
_tmp4E0);if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct
_tmp6B6;_tmp6B6.tag=0;_tmp6B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp6B4[1]={& _tmp6B6};Cyc_Tcutil_warn(e->loc,({
const char*_tmp6B5="bounds check necessary for %s";_tag_dyneither(_tmp6B5,sizeof(
char),30);}),_tag_dyneither(_tmp6B4,sizeof(void*),1));});});_LL315:;}Cyc_Toc_exp_to_c(
nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);++ Cyc_Toc_total_bounds_checks;{void*
_tmp6B7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AB);struct Cyc_Absyn_Exp*_tmp6B9;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp6B8=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B7;if(_tmp6B8->tag != 1)goto _LL31B;
else{_tmp6B9=_tmp6B8->f1;}}_LL31A: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AA);void*ta1=Cyc_Toc_typ_to_c(_tmp6A6);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A7);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AC)){if(!Cyc_Evexp_c_can_eval(_tmp6B9))({void*_tmp6BB=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp6BC="cannot determine subscript is in bounds";_tag_dyneither(
_tmp6BC,sizeof(char),40);}),_tag_dyneither(_tmp6BB,sizeof(void*),0));});{struct
Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4DF);e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp6BD[3];_tmp6BD[2]=_tmp4E0;_tmp6BD[1]=
_tmp6B9;_tmp6BD[0]=_tmp4DF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6BD,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6B9))({void*_tmp6BE=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp6BF="cannot determine subscript is in bounds";
_tag_dyneither(_tmp6BF,sizeof(char),40);}),_tag_dyneither(_tmp6BE,sizeof(void*),
0));});if(Cyc_Toc_warn_all_null_deref)({void*_tmp6C0=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp6C1="inserted null check due to dereference";_tag_dyneither(
_tmp6C1,sizeof(char),39);}),_tag_dyneither(_tmp6C0,sizeof(void*),0));});e->r=Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp6C2[4];_tmp6C2[3]=_tmp4E0;_tmp6C2[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6C2[1]=_tmp6B9;_tmp6C2[0]=_tmp4DF;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6C2,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)));}else{if(!Cyc_Evexp_c_can_eval(_tmp6B9))({void*_tmp6C3=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp6C4="cannot determine subscript is in bounds";
_tag_dyneither(_tmp6C4,sizeof(char),40);}),_tag_dyneither(_tmp6C3,sizeof(void*),
0));});_tmp4E0->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp6C5[2];_tmp6C5[1]=Cyc_Absyn_copy_exp(_tmp4E0);_tmp6C5[0]=
_tmp6B9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6C5,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}goto _LL318;}
_LL31B: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6B7;if(_tmp6BA->tag != 0)goto _LL318;}_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp6A6);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;e->r=Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A7),Cyc_Absyn_aggrmember_exp(
_tmp4DF,Cyc_Toc_curr_sp,0)),_tmp4E0);}else{struct Cyc_Absyn_Exp*_tmp6C6=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A7),Cyc_Absyn_fncall_exp(
_tmp6C6,({struct Cyc_Absyn_Exp*_tmp6C7[3];_tmp6C7[2]=_tmp4E0;_tmp6C7[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6C7[0]=_tmp4DF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6C7,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}goto _LL318;}
_LL318:;}goto _LL30E;}_LL313:;_LL314:({void*_tmp6C8=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6C9="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dyneither(_tmp6C9,sizeof(char),49);}),_tag_dyneither(_tmp6C8,sizeof(void*),
0));});_LL30E:;}goto _LL208;}_LL241: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E1=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp482;if(_tmp4E1->tag != 25)goto _LL243;else{
_tmp4E2=_tmp4E1->f1;}}_LL242: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp4E2))->r;else{struct Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp4E2);void*_tmp6CB=Cyc_Toc_add_tuple_type(_tmp6CA);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp4E2 != 0;(_tmp4E2=_tmp4E2->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp4E2->hd);dles=({struct Cyc_List_List*_tmp6CC=_cycalloc(
sizeof(*_tmp6CC));_tmp6CC->hd=({struct _tuple15*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));
_tmp6CD->f1=0;_tmp6CD->f2=(struct Cyc_Absyn_Exp*)_tmp4E2->hd;_tmp6CD;});_tmp6CC->tl=
dles;_tmp6CC;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL208;_LL243: {struct Cyc_Absyn_Array_e_struct*
_tmp4E3=(struct Cyc_Absyn_Array_e_struct*)_tmp482;if(_tmp4E3->tag != 27)goto _LL245;
else{_tmp4E4=_tmp4E3->f1;}}_LL244: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E4);{
struct Cyc_List_List*_tmp6CE=_tmp4E4;for(0;_tmp6CE != 0;_tmp6CE=_tmp6CE->tl){
struct _tuple15 _tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct _tuple15*_tmp6CF=(struct
_tuple15*)_tmp6CE->hd;_tmp6D0=*_tmp6CF;_tmp6D1=_tmp6D0.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D1);}}goto _LL208;_LL245: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E5=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp482;if(_tmp4E5->tag != 28)goto _LL247;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL246: {unsigned int _tmp6D3;int _tmp6D4;struct _tuple10 _tmp6D2=Cyc_Evexp_eval_const_uint_exp(
_tmp4E7);_tmp6D3=_tmp6D2.f1;_tmp6D4=_tmp6D2.f2;{void*_tmp6D5=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4E8);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4E8)){if(!_tmp6D4)({
void*_tmp6D6=0;Cyc_Tcutil_terr(_tmp4E7->loc,({const char*_tmp6D7="cannot determine value of constant";
_tag_dyneither(_tmp6D7,sizeof(char),35);}),_tag_dyneither(_tmp6D6,sizeof(void*),
0));});{unsigned int i=0;for(0;i < _tmp6D3;++ i){es=({struct Cyc_List_List*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=({struct _tuple15*_tmp6D9=_cycalloc(
sizeof(*_tmp6D9));_tmp6D9->f1=0;_tmp6D9->f2=_tmp4E8;_tmp6D9;});_tmp6D8->tl=es;
_tmp6D8;});}}if(_tmp4E9){struct Cyc_Absyn_Exp*_tmp6DA=Cyc_Toc_cast_it(_tmp6D5,Cyc_Absyn_uint_exp(
0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));
_tmp6DB->hd=({struct _tuple15*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->f1=0;
_tmp6DC->f2=_tmp6DA;_tmp6DC;});_tmp6DB->tl=0;_tmp6DB;}));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL208;};};}_LL247: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4EA=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp482;if(_tmp4EA->tag != 29)goto _LL249;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL248: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EC
!= 0,0,0,_tmp4ED,_tmp4EB))->r;else{if(_tmp4EE == 0)({void*_tmp6DD=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp6DE="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp6DE,sizeof(
char),38);}),_tag_dyneither(_tmp6DD,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4EE;struct Cyc_Toc_Env _tmp6E0;struct
_RegionHandle*_tmp6E1;struct Cyc_Toc_Env*_tmp6DF=nv;_tmp6E0=*_tmp6DF;_tmp6E1=
_tmp6E0.rgn;{struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E1,e->loc,_tmp4ED,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EE->impl))->tagged){struct
_tuple20 _tmp6E4;struct Cyc_Absyn_Aggrfield*_tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;
struct _tuple20*_tmp6E3=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E2))->hd;_tmp6E4=*_tmp6E3;_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{void*
_tmp6E7=_tmp6E5->type;Cyc_Toc_exp_to_c(nv,_tmp6E6);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E7))_tmp6E6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E6->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4EE,_tmp6E5->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct Cyc_List_List*_tmp6E8=({struct _tuple15*_tmp6EE[2];_tmp6EE[1]=({struct
_tuple15*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->f1=0;_tmp6F0->f2=_tmp6E6;
_tmp6F0;});_tmp6EE[0]=({struct _tuple15*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));
_tmp6EF->f1=0;_tmp6EF->f2=field_tag_exp;_tmp6EF;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6EE,sizeof(struct
_tuple15*),2));});struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6E8,
0);struct Cyc_List_List*ds=({void*_tmp6EB[1];_tmp6EB[0]=(void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6ED;_tmp6ED.tag=1;_tmp6ED.f1=_tmp6E5->name;_tmp6ED;});_tmp6EC;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6EB,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple15*_tmp6E9[1];_tmp6E9[0]=({struct _tuple15*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA->f1=ds;_tmp6EA->f2=umem;_tmp6EA;});((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp6E9,sizeof(struct _tuple15*),1));});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}
else{struct Cyc_List_List*_tmp6F1=0;struct Cyc_List_List*_tmp6F2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){struct Cyc_List_List*
_tmp6F3=_tmp6E2;for(0;_tmp6F3 != 0;_tmp6F3=_tmp6F3->tl){if((*((struct _tuple20*)
_tmp6F3->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F2->hd){struct _tuple20
_tmp6F5;struct Cyc_Absyn_Aggrfield*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F7;struct
_tuple20*_tmp6F4=(struct _tuple20*)_tmp6F3->hd;_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;
_tmp6F7=_tmp6F5.f2;{void*_tmp6F8=_tmp6F6->type;Cyc_Toc_exp_to_c(nv,_tmp6F7);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6F8))_tmp6F7->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F7->r,0));_tmp6F1=({struct Cyc_List_List*_tmp6F9=_cycalloc(
sizeof(*_tmp6F9));_tmp6F9->hd=({struct _tuple15*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));
_tmp6FA->f1=0;_tmp6FA->f2=_tmp6F7;_tmp6FA;});_tmp6F9->tl=_tmp6F1;_tmp6F9;});
break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F1));}};};}goto _LL208;_LL249: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4EF=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp482;if(_tmp4EF->tag != 30)goto
_LL24B;else{_tmp4F0=(void*)_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}_LL24A: {struct Cyc_List_List*
fs;{void*_tmp6FB=Cyc_Tcutil_compress(_tmp4F0);struct Cyc_List_List*_tmp6FD;_LL31E: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp6FC=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6FB;if(_tmp6FC->tag != 13)goto _LL320;else{_tmp6FD=_tmp6FC->f2;}}_LL31F: fs=
_tmp6FD;goto _LL31D;_LL320:;_LL321:({struct Cyc_String_pa_struct _tmp700;_tmp700.tag=
0;_tmp700.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4F0));({void*_tmp6FE[1]={& _tmp700};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6FF="anon struct has type %s";
_tag_dyneither(_tmp6FF,sizeof(char),24);}),_tag_dyneither(_tmp6FE,sizeof(void*),
1));});});_LL31D:;}{struct Cyc_Toc_Env _tmp702;struct _RegionHandle*_tmp703;struct
Cyc_Toc_Env*_tmp701=nv;_tmp702=*_tmp701;_tmp703=_tmp702.rgn;{struct Cyc_List_List*
_tmp704=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp703,e->loc,_tmp4F1,Cyc_Absyn_StructA,
fs);for(0;_tmp704 != 0;_tmp704=_tmp704->tl){struct _tuple20 _tmp706;struct Cyc_Absyn_Aggrfield*
_tmp707;struct Cyc_Absyn_Exp*_tmp708;struct _tuple20*_tmp705=(struct _tuple20*)
_tmp704->hd;_tmp706=*_tmp705;_tmp707=_tmp706.f1;_tmp708=_tmp706.f2;{void*_tmp709=
_tmp707->type;Cyc_Toc_exp_to_c(nv,_tmp708);if(Cyc_Toc_is_void_star_or_tvar(
_tmp709))_tmp708->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp708->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F1);}goto _LL208;};}
_LL24B: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F2=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp482;if(_tmp4F2->tag != 31)goto _LL24D;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;
_tmp4F5=_tmp4F2->f3;}}_LL24C: {void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp70A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp70B=Cyc_Absyn_var_exp(
_tmp70A,0);struct Cyc_Absyn_Exp*member_exp;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4F5->name,({const char*_tmp70C="_struct";_tag_dyneither(_tmp70C,sizeof(char),
8);})));tag_exp=_tmp4F4->is_extensible?Cyc_Absyn_var_exp(_tmp4F5->name,0): Cyc_Toc_datatype_tag(
_tmp4F4,_tmp4F5->name);member_exp=_tmp70B;{struct Cyc_List_List*_tmp70D=_tmp4F5->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F3 != 0;(
_tmp4F3=_tmp4F3->tl,_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->hd=({struct _tuple15*_tmp70F=
_cycalloc(sizeof(*_tmp70F));_tmp70F->f1=0;_tmp70F->f2=cur_e;_tmp70F;});_tmp70E->tl=
dles;_tmp70E;});}dles=({struct Cyc_List_List*_tmp710=_cycalloc(sizeof(*_tmp710));
_tmp710->hd=({struct _tuple15*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->f1=0;
_tmp711->f2=tag_exp;_tmp711;});_tmp710->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp710;});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
else{struct Cyc_List_List*_tmp712=({struct Cyc_List_List*_tmp718=_cycalloc(sizeof(*
_tmp718));_tmp718->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,
Cyc_Toc_tag_sp,0),tag_exp,0);_tmp718->tl=0;_tmp718;});{int i=1;for(0;_tmp4F3 != 0;(((
_tmp4F3=_tmp4F3->tl,i ++)),_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple9*)((struct Cyc_List_List*)_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(field_typ))cur_e=Cyc_Toc_cast_it(
field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp713=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp712=({struct Cyc_List_List*
_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714->hd=_tmp713;_tmp714->tl=_tmp712;
_tmp714;});};}}{struct Cyc_Absyn_Stmt*_tmp715=Cyc_Absyn_exp_stmt(_tmp70B,0);
struct Cyc_Absyn_Stmt*_tmp716=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp717=_cycalloc(
sizeof(*_tmp717));_tmp717->hd=_tmp715;_tmp717->tl=_tmp712;_tmp717;})),0);e->r=
Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp70A,datatype_ctype,0,_tmp716,0));};}
goto _LL208;};}_LL24D: {struct Cyc_Absyn_Enum_e_struct*_tmp4F6=(struct Cyc_Absyn_Enum_e_struct*)
_tmp482;if(_tmp4F6->tag != 32)goto _LL24F;}_LL24E: goto _LL250;_LL24F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp482;if(_tmp4F7->tag != 33)goto
_LL251;}_LL250: goto _LL208;_LL251: {struct Cyc_Absyn_Malloc_e_struct*_tmp4F8=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp482;if(_tmp4F8->tag != 34)goto _LL253;else{
_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.is_calloc;_tmp4FB=_tmp4F9.rgn;_tmp4FC=
_tmp4F9.elt_type;_tmp4FD=_tmp4F9.num_elts;_tmp4FE=_tmp4F9.fat_result;}}_LL252: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FC)));Cyc_Toc_exp_to_c(nv,
_tmp4FD);if(_tmp4FE){struct _tuple0*_tmp719=Cyc_Toc_temp_var();struct _tuple0*
_tmp71A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4FA){xexp=_tmp4FD;if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp719,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FC,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp719,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp71B[3];_tmp71B[2]=Cyc_Absyn_var_exp(_tmp719,0);_tmp71B[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp71B[0]=Cyc_Absyn_var_exp(_tmp71A,0);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp71B,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp4FD,0);if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp719,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp4FC,Cyc_Absyn_var_exp(
_tmp719,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp71C[3];_tmp71C[2]=Cyc_Absyn_var_exp(_tmp719,0);_tmp71C[1]=Cyc_Absyn_uint_exp(
1,0);_tmp71C[0]=Cyc_Absyn_var_exp(_tmp71A,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp71C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_declare_stmt(_tmp719,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp71A,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71D);};}else{struct Cyc_Absyn_Exp*_tmp71E=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp71F=_tmp4FD->r;union Cyc_Absyn_Cnst _tmp721;struct _tuple5 _tmp722;
int _tmp723;_LL323: {struct Cyc_Absyn_Const_e_struct*_tmp720=(struct Cyc_Absyn_Const_e_struct*)
_tmp71F;if(_tmp720->tag != 0)goto _LL325;else{_tmp721=_tmp720->f1;if((_tmp721.Int_c).tag
!= 4)goto _LL325;_tmp722=(struct _tuple5)(_tmp721.Int_c).val;_tmp723=_tmp722.f2;
if(_tmp723 != 1)goto _LL325;}}_LL324: goto _LL322;_LL325:;_LL326: _tmp71E=Cyc_Absyn_times_exp(
_tmp71E,_tmp4FD,0);goto _LL322;_LL322:;}if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp71E))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FC,
_tmp71E))->r;}}goto _LL208;}_LL253: {struct Cyc_Absyn_Swap_e_struct*_tmp4FF=(
struct Cyc_Absyn_Swap_e_struct*)_tmp482;if(_tmp4FF->tag != 35)goto _LL255;else{
_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;}}_LL254: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr))({void*_tmp724=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp725="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dyneither(_tmp725,sizeof(char),57);}),_tag_dyneither(_tmp724,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp500);Cyc_Toc_exp_to_c(
nv,_tmp501);e->r=Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp726[2];
_tmp726[1]=Cyc_Toc_push_address_exp(_tmp501);_tmp726[0]=Cyc_Toc_push_address_exp(
_tmp500);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp726,sizeof(struct Cyc_Absyn_Exp*),2));}));goto _LL208;};}_LL255: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp502=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp482;if(_tmp502->tag != 38)goto _LL257;else{_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;}}
_LL256: {void*_tmp727=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp503->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp503);{void*_tmp728=
_tmp727;struct Cyc_Absyn_AggrInfo _tmp72A;union Cyc_Absyn_AggrInfoU _tmp72B;struct
Cyc_Absyn_Aggrdecl**_tmp72C;struct Cyc_Absyn_Aggrdecl*_tmp72D;_LL328: {struct Cyc_Absyn_AggrType_struct*
_tmp729=(struct Cyc_Absyn_AggrType_struct*)_tmp728;if(_tmp729->tag != 12)goto
_LL32A;else{_tmp72A=_tmp729->f1;_tmp72B=_tmp72A.aggr_info;if((_tmp72B.KnownAggr).tag
!= 2)goto _LL32A;_tmp72C=(struct Cyc_Absyn_Aggrdecl**)(_tmp72B.KnownAggr).val;
_tmp72D=*_tmp72C;}}_LL329: {struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72D,_tmp504),0);struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_aggrmember_exp(
_tmp503,_tmp504,0);struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_aggrmember_exp(_tmp72F,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp730,_tmp72E,0))->r;goto _LL327;}
_LL32A:;_LL32B:({void*_tmp731=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp732="non-aggregate type in tagcheck";
_tag_dyneither(_tmp732,sizeof(char),31);}),_tag_dyneither(_tmp731,sizeof(void*),
0));});_LL327:;}goto _LL208;}_LL257: {struct Cyc_Absyn_StmtExp_e_struct*_tmp505=(
struct Cyc_Absyn_StmtExp_e_struct*)_tmp482;if(_tmp505->tag != 37)goto _LL259;else{
_tmp506=_tmp505->f1;}}_LL258: Cyc_Toc_stmt_to_c(nv,_tmp506);goto _LL208;_LL259: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp507=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp482;if(_tmp507->tag != 36)goto _LL25B;}_LL25A:({void*_tmp733=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp734="UnresolvedMem";
_tag_dyneither(_tmp734,sizeof(char),14);}),_tag_dyneither(_tmp733,sizeof(void*),
0));});_LL25B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp508=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp482;if(_tmp508->tag != 26)goto _LL25D;}_LL25C:({void*_tmp735=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp736="compoundlit";
_tag_dyneither(_tmp736,sizeof(char),12);}),_tag_dyneither(_tmp735,sizeof(void*),
0));});_LL25D: {struct Cyc_Absyn_Valueof_e_struct*_tmp509=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp482;if(_tmp509->tag != 39)goto _LL208;}_LL25E:({void*_tmp737=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp738="valueof(-)";
_tag_dyneither(_tmp738,sizeof(char),11);}),_tag_dyneither(_tmp737,sizeof(void*),
0));});_LL208:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp739=p->r;struct Cyc_Absyn_Vardecl*_tmp73B;struct
Cyc_Absyn_Vardecl _tmp73C;struct _tuple0*_tmp73D;struct Cyc_Absyn_Pat*_tmp73E;
struct Cyc_Absyn_Vardecl*_tmp740;struct Cyc_Absyn_Vardecl _tmp741;struct _tuple0*
_tmp742;struct Cyc_Absyn_Vardecl*_tmp745;struct Cyc_Absyn_Pat*_tmp746;enum Cyc_Absyn_Sign
_tmp749;int _tmp74A;char _tmp74C;struct _dyneither_ptr _tmp74E;struct Cyc_Absyn_Enumdecl*
_tmp750;struct Cyc_Absyn_Enumfield*_tmp751;void*_tmp753;struct Cyc_Absyn_Enumfield*
_tmp754;struct Cyc_Absyn_Pat*_tmp756;struct Cyc_Absyn_Pat _tmp757;void*_tmp758;
struct Cyc_Absyn_Datatypedecl*_tmp75A;struct Cyc_Absyn_Datatypefield*_tmp75B;
struct Cyc_List_List*_tmp75C;struct Cyc_List_List*_tmp75E;struct Cyc_List_List*
_tmp760;struct Cyc_Absyn_AggrInfo*_tmp762;struct Cyc_Absyn_AggrInfo*_tmp764;struct
Cyc_Absyn_AggrInfo _tmp765;union Cyc_Absyn_AggrInfoU _tmp766;struct Cyc_List_List*
_tmp767;struct Cyc_Absyn_Pat*_tmp769;_LL32D: {struct Cyc_Absyn_Var_p_struct*
_tmp73A=(struct Cyc_Absyn_Var_p_struct*)_tmp739;if(_tmp73A->tag != 1)goto _LL32F;
else{_tmp73B=_tmp73A->f1;_tmp73C=*_tmp73B;_tmp73D=_tmp73C.name;_tmp73E=_tmp73A->f2;}}
_LL32E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73D,r),rgn,t,r,path,
_tmp73E,fail_stmt,decls);_LL32F: {struct Cyc_Absyn_TagInt_p_struct*_tmp73F=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp739;if(_tmp73F->tag != 3)goto _LL331;else{
_tmp740=_tmp73F->f2;_tmp741=*_tmp740;_tmp742=_tmp741.name;}}_LL330: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp742,r);goto _LL332;_LL331: {struct Cyc_Absyn_Wild_p_struct*_tmp743=(
struct Cyc_Absyn_Wild_p_struct*)_tmp739;if(_tmp743->tag != 0)goto _LL333;}_LL332: s=
Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL333: {struct Cyc_Absyn_Reference_p_struct*
_tmp744=(struct Cyc_Absyn_Reference_p_struct*)_tmp739;if(_tmp744->tag != 2)goto
_LL335;else{_tmp745=_tmp744->f1;_tmp746=_tmp744->f2;}}_LL334: {struct _tuple0*
_tmp76D=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp76E=_region_malloc(
rgn,sizeof(*_tmp76E));_tmp76E->hd=({struct _tuple22*_tmp76F=_region_malloc(rgn,
sizeof(*_tmp76F));_tmp76F->f1=_tmp76D;_tmp76F->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq);_tmp76F;});_tmp76E->tl=decls;_tmp76E;});nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp745->name,Cyc_Absyn_var_exp(_tmp76D,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp76D,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp770=Cyc_Toc_xlate_pat(nv,
rgn,t,r,path,_tmp746,fail_stmt,decls);_tmp770.f3=Cyc_Absyn_seq_stmt(s,_tmp770.f3,
0);return _tmp770;};}_LL335: {struct Cyc_Absyn_Null_p_struct*_tmp747=(struct Cyc_Absyn_Null_p_struct*)
_tmp739;if(_tmp747->tag != 8)goto _LL337;}_LL336: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32C;_LL337: {struct Cyc_Absyn_Int_p_struct*_tmp748=(struct
Cyc_Absyn_Int_p_struct*)_tmp739;if(_tmp748->tag != 9)goto _LL339;else{_tmp749=
_tmp748->f1;_tmp74A=_tmp748->f2;}}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp749,_tmp74A,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Char_p_struct*
_tmp74B=(struct Cyc_Absyn_Char_p_struct*)_tmp739;if(_tmp74B->tag != 10)goto _LL33B;
else{_tmp74C=_tmp74B->f1;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74C,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Float_p_struct*_tmp74D=(
struct Cyc_Absyn_Float_p_struct*)_tmp739;if(_tmp74D->tag != 11)goto _LL33D;else{
_tmp74E=_tmp74D->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp74E,
0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Enum_p_struct*_tmp74F=(struct
Cyc_Absyn_Enum_p_struct*)_tmp739;if(_tmp74F->tag != 12)goto _LL33F;else{_tmp750=
_tmp74F->f1;_tmp751=_tmp74F->f2;}}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp771=_cycalloc(sizeof(*_tmp771));
_tmp771[0]=({struct Cyc_Absyn_Enum_e_struct _tmp772;_tmp772.tag=32;_tmp772.f1=
_tmp751->name;_tmp772.f2=(struct Cyc_Absyn_Enumdecl*)_tmp750;_tmp772.f3=(struct
Cyc_Absyn_Enumfield*)_tmp751;_tmp772;});_tmp771;}),0),fail_stmt);goto _LL32C;
_LL33F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp752=(struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp739;if(_tmp752->tag != 13)goto _LL341;else{_tmp753=(void*)_tmp752->f1;_tmp754=
_tmp752->f2;}}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_AnonEnum_e_struct*_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({
struct Cyc_Absyn_AnonEnum_e_struct _tmp774;_tmp774.tag=33;_tmp774.f1=_tmp754->name;
_tmp774.f2=(void*)_tmp753;_tmp774.f3=(struct Cyc_Absyn_Enumfield*)_tmp754;_tmp774;});
_tmp773;}),0),fail_stmt);goto _LL32C;_LL341: {struct Cyc_Absyn_Pointer_p_struct*
_tmp755=(struct Cyc_Absyn_Pointer_p_struct*)_tmp739;if(_tmp755->tag != 5)goto
_LL343;else{_tmp756=_tmp755->f1;_tmp757=*_tmp756;_tmp758=_tmp757.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp759=(struct Cyc_Absyn_Datatype_p_struct*)_tmp758;if(_tmp759->tag != 7)goto
_LL343;else{_tmp75A=_tmp759->f1;_tmp75B=_tmp759->f2;_tmp75C=_tmp759->f3;}};}}
_LL342: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp775=Cyc_Toc_temp_var();struct
_tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75B->name,({const char*_tmp783="_struct";
_tag_dyneither(_tmp783,sizeof(char),8);}));void*_tmp776=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(
tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp776,r);struct Cyc_List_List*_tmp777=_tmp75B->typs;for(0;_tmp75C != 0;(((
_tmp75C=_tmp75C->tl,_tmp777=((struct Cyc_List_List*)_check_null(_tmp777))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp778=(struct Cyc_Absyn_Pat*)_tmp75C->hd;if(_tmp778->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp779=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp777))->hd)).f2;struct _tuple0*_tmp77A=Cyc_Toc_temp_var();
void*_tmp77B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp778->topt))->v;void*
_tmp77C=Cyc_Toc_typ_to_c(_tmp77B);struct Cyc_Absyn_Exp*_tmp77D=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp775,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp779)))_tmp77D=Cyc_Toc_cast_it(_tmp77C,_tmp77D);decls=({
struct Cyc_List_List*_tmp77E=_region_malloc(rgn,sizeof(*_tmp77E));_tmp77E->hd=({
struct _tuple22*_tmp77F=_region_malloc(rgn,sizeof(*_tmp77F));_tmp77F->f1=_tmp77A;
_tmp77F->f2=_tmp77C;_tmp77F;});_tmp77E->tl=decls;_tmp77E;});{struct _tuple21
_tmp780=Cyc_Toc_xlate_pat(nv,rgn,_tmp77B,Cyc_Absyn_var_exp(_tmp77A,0),_tmp77D,
_tmp778,fail_stmt,decls);nv=_tmp780.f1;decls=_tmp780.f2;{struct Cyc_Absyn_Stmt*
_tmp781=_tmp780.f3;struct Cyc_Absyn_Stmt*_tmp782=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp77A,0),_tmp77D,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp782,_tmp781,
0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(
_tmp775,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=_tmp75A->is_extensible?
Cyc_Absyn_var_exp(_tmp75B->name,0): Cyc_Toc_datatype_tag(_tmp75A,_tmp75B->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(
_tmp775,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(
_tmp775,_tmp776,(struct Cyc_Absyn_Exp*)rcast,s,0);goto _LL32C;};};_LL343: {struct
Cyc_Absyn_Datatype_p_struct*_tmp75D=(struct Cyc_Absyn_Datatype_p_struct*)_tmp739;
if(_tmp75D->tag != 7)goto _LL345;else{_tmp75E=_tmp75D->f3;}}_LL344: _tmp760=_tmp75E;
goto _LL346;_LL345: {struct Cyc_Absyn_Tuple_p_struct*_tmp75F=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp739;if(_tmp75F->tag != 4)goto _LL347;else{_tmp760=_tmp75F->f1;}}_LL346: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp760 != 0;(_tmp760=_tmp760->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp784=(struct Cyc_Absyn_Pat*)_tmp760->hd;if(_tmp784->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{struct _tuple0*_tmp785=Cyc_Toc_temp_var();void*_tmp786=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp784->topt))->v;decls=({struct Cyc_List_List*_tmp787=
_region_malloc(rgn,sizeof(*_tmp787));_tmp787->hd=({struct _tuple22*_tmp788=
_region_malloc(rgn,sizeof(*_tmp788));_tmp788->f1=_tmp785;_tmp788->f2=Cyc_Toc_typ_to_c(
_tmp786);_tmp788;});_tmp787->tl=decls;_tmp787;});{struct _tuple21 _tmp789=Cyc_Toc_xlate_pat(
nv,rgn,_tmp786,Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp784,fail_stmt,decls);nv=_tmp789.f1;decls=_tmp789.f2;{struct Cyc_Absyn_Stmt*
_tmp78A=_tmp789.f3;struct Cyc_Absyn_Stmt*_tmp78B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp785,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp78B,_tmp78A,0),0);};};};}goto _LL32C;};_LL347: {struct
Cyc_Absyn_Aggr_p_struct*_tmp761=(struct Cyc_Absyn_Aggr_p_struct*)_tmp739;if(
_tmp761->tag != 6)goto _LL349;else{_tmp762=_tmp761->f1;if(_tmp762 != 0)goto _LL349;}}
_LL348:({void*_tmp78C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp78D="unresolved aggregate pattern!";
_tag_dyneither(_tmp78D,sizeof(char),30);}),_tag_dyneither(_tmp78C,sizeof(void*),
0));});_LL349: {struct Cyc_Absyn_Aggr_p_struct*_tmp763=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp739;if(_tmp763->tag != 6)goto _LL34B;else{_tmp764=_tmp763->f1;if(_tmp764 == 0)
goto _LL34B;_tmp765=*_tmp764;_tmp766=_tmp765.aggr_info;_tmp767=_tmp763->f3;}}
_LL34A: {struct Cyc_Absyn_Aggrdecl*_tmp78E=Cyc_Absyn_get_known_aggrdecl(_tmp766);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp78E->impl))->tagged){struct
_tuple23 _tmp790;struct Cyc_List_List*_tmp791;struct Cyc_Absyn_Pat*_tmp792;struct
_tuple23*_tmp78F=(struct _tuple23*)((struct Cyc_List_List*)_check_null(_tmp767))->hd;
_tmp790=*_tmp78F;_tmp791=_tmp790.f1;_tmp792=_tmp790.f2;{struct _dyneither_ptr*f;{
void*_tmp793=(void*)((struct Cyc_List_List*)_check_null(_tmp791))->hd;struct
_dyneither_ptr*_tmp795;_LL354: {struct Cyc_Absyn_FieldName_struct*_tmp794=(struct
Cyc_Absyn_FieldName_struct*)_tmp793;if(_tmp794->tag != 1)goto _LL356;else{_tmp795=
_tmp794->f1;}}_LL355: f=_tmp795;goto _LL353;_LL356:;_LL357:({void*_tmp796=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp797="no field name in tagged union pattern";_tag_dyneither(_tmp797,sizeof(
char),38);}),_tag_dyneither(_tmp796,sizeof(void*),0));});_LL353:;}{struct _tuple0*
_tmp798=Cyc_Toc_temp_var();void*_tmp799=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp792->topt))->v;void*_tmp79A=Cyc_Toc_typ_to_c(_tmp799);decls=({struct Cyc_List_List*
_tmp79B=_region_malloc(rgn,sizeof(*_tmp79B));_tmp79B->hd=({struct _tuple22*
_tmp79C=_region_malloc(rgn,sizeof(*_tmp79C));_tmp79C->f1=_tmp798;_tmp79C->f2=
_tmp79A;_tmp79C;});_tmp79B->tl=decls;_tmp79B;});{struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp79E=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp79E=Cyc_Toc_cast_it(_tmp79A,_tmp79E);_tmp79D=Cyc_Toc_cast_it(_tmp79A,_tmp79D);{
struct _tuple21 _tmp79F=Cyc_Toc_xlate_pat(nv,rgn,_tmp799,Cyc_Absyn_var_exp(_tmp798,
0),_tmp79D,_tmp792,fail_stmt,decls);nv=_tmp79F.f1;decls=_tmp79F.f2;{struct Cyc_Absyn_Stmt*
_tmp7A0=_tmp79F.f3;struct Cyc_Absyn_Stmt*_tmp7A1=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp78E,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp798,0),_tmp79E,0);s=Cyc_Absyn_seq_stmt(_tmp7A1,Cyc_Absyn_seq_stmt(_tmp7A2,
_tmp7A0,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp767 != 0;_tmp767=
_tmp767->tl){struct _tuple23*_tmp7A3=(struct _tuple23*)_tmp767->hd;struct Cyc_Absyn_Pat*
_tmp7A4=(*_tmp7A3).f2;if(_tmp7A4->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7A5=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A3).f1))->hd;struct _dyneither_ptr*_tmp7A7;_LL359: {struct Cyc_Absyn_FieldName_struct*
_tmp7A6=(struct Cyc_Absyn_FieldName_struct*)_tmp7A5;if(_tmp7A6->tag != 1)goto
_LL35B;else{_tmp7A7=_tmp7A6->f1;}}_LL35A: f=_tmp7A7;goto _LL358;_LL35B:;_LL35C:(
int)_throw((void*)({struct Cyc_Toc_Match_error_struct*_tmp7A8=_cycalloc_atomic(
sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Toc_Match_error_struct _tmp7A9;_tmp7A9.tag=
Cyc_Toc_Match_error;_tmp7A9;});_tmp7A8;}));_LL358:;}{struct _tuple0*_tmp7AA=Cyc_Toc_temp_var();
void*_tmp7AB=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7A4->topt))->v;void*
_tmp7AC=Cyc_Toc_typ_to_c(_tmp7AB);decls=({struct Cyc_List_List*_tmp7AD=
_region_malloc(rgn,sizeof(*_tmp7AD));_tmp7AD->hd=({struct _tuple22*_tmp7AE=
_region_malloc(rgn,sizeof(*_tmp7AE));_tmp7AE->f1=_tmp7AA;_tmp7AE->f2=_tmp7AC;
_tmp7AE;});_tmp7AD->tl=decls;_tmp7AD;});{struct _tuple21 _tmp7AF=Cyc_Toc_xlate_pat(
nv,rgn,_tmp7AB,Cyc_Absyn_var_exp(_tmp7AA,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp7A4,fail_stmt,decls);nv=_tmp7AF.f1;decls=_tmp7AF.f2;{struct Cyc_Absyn_Exp*
_tmp7B0=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star_or_tvar(((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp78E->impl))->fields,f)))->type))_tmp7B0=Cyc_Toc_cast_it(_tmp7AC,
_tmp7B0);{struct Cyc_Absyn_Stmt*_tmp7B1=_tmp7AF.f3;struct Cyc_Absyn_Stmt*_tmp7B2=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AA,0),_tmp7B0,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp7B2,_tmp7B1,0),0);};};};};};}}goto _LL32C;}_LL34B: {
struct Cyc_Absyn_Pointer_p_struct*_tmp768=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp739;if(_tmp768->tag != 5)goto _LL34D;else{_tmp769=_tmp768->f1;}}_LL34C: {
struct _tuple0*_tmp7B3=Cyc_Toc_temp_var();void*_tmp7B4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp769->topt))->v;decls=({struct Cyc_List_List*_tmp7B5=
_region_malloc(rgn,sizeof(*_tmp7B5));_tmp7B5->hd=({struct _tuple22*_tmp7B6=
_region_malloc(rgn,sizeof(*_tmp7B6));_tmp7B6->f1=_tmp7B3;_tmp7B6->f2=Cyc_Toc_typ_to_c(
_tmp7B4);_tmp7B6;});_tmp7B5->tl=decls;_tmp7B5;});{struct _tuple21 _tmp7B7=Cyc_Toc_xlate_pat(
nv,rgn,_tmp7B4,Cyc_Absyn_var_exp(_tmp7B3,0),Cyc_Absyn_deref_exp(path,0),_tmp769,
fail_stmt,decls);nv=_tmp7B7.f1;decls=_tmp7B7.f2;{struct Cyc_Absyn_Stmt*_tmp7B8=
_tmp7B7.f3;struct Cyc_Absyn_Stmt*_tmp7B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7B3,0),Cyc_Absyn_deref_exp(r,0),0),_tmp7B8,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp7B9,0);else{s=_tmp7B9;}goto
_LL32C;};};}_LL34D: {struct Cyc_Absyn_UnknownId_p_struct*_tmp76A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp739;if(_tmp76A->tag != 14)goto _LL34F;}_LL34E:({void*_tmp7BA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BB="unknownid pat";
_tag_dyneither(_tmp7BB,sizeof(char),14);}),_tag_dyneither(_tmp7BA,sizeof(void*),
0));});_LL34F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76B=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp739;if(_tmp76B->tag != 15)goto _LL351;}_LL350:({void*_tmp7BC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BD="unknowncall pat";
_tag_dyneither(_tmp7BD,sizeof(char),16);}),_tag_dyneither(_tmp7BC,sizeof(void*),
0));});_LL351: {struct Cyc_Absyn_Exp_p_struct*_tmp76C=(struct Cyc_Absyn_Exp_p_struct*)
_tmp739;if(_tmp76C->tag != 16)goto _LL32C;}_LL352:({void*_tmp7BE=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BF="exp pat";
_tag_dyneither(_tmp7BF,sizeof(char),8);}),_tag_dyneither(_tmp7BE,sizeof(void*),0));});
_LL32C:;}return({struct _tuple21 _tmp7C0;_tmp7C0.f1=nv;_tmp7C0.f2=decls;_tmp7C0.f3=
s;_tmp7C0;});}struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;
struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple24*_tmp7C1=
_region_malloc(r,sizeof(*_tmp7C1));_tmp7C1->f1=Cyc_Toc_fresh_label();_tmp7C1->f2=
Cyc_Toc_fresh_label();_tmp7C1->f3=sc;_tmp7C1;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp7C2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp7C3=Cyc_Tcutil_compress(
_tmp7C2);_LL35E: {struct Cyc_Absyn_IntType_struct*_tmp7C4=(struct Cyc_Absyn_IntType_struct*)
_tmp7C3;if(_tmp7C4->tag != 6)goto _LL360;}_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_EnumType_struct*
_tmp7C5=(struct Cyc_Absyn_EnumType_struct*)_tmp7C3;if(_tmp7C5->tag != 14)goto
_LL362;}_LL361: leave_as_switch=1;goto _LL35D;_LL362:;_LL363: leave_as_switch=0;
goto _LL35D;_LL35D:;}{struct Cyc_List_List*_tmp7C6=scs;for(0;_tmp7C6 != 0;_tmp7C6=
_tmp7C6->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp7C6->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp7C7=
scs;for(0;_tmp7C7 != 0;_tmp7C7=_tmp7C7->tl){struct Cyc_Absyn_Stmt*_tmp7C8=((struct
Cyc_Absyn_Switch_clause*)_tmp7C7->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C7->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7C8,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7CA;struct _RegionHandle*_tmp7CB;struct Cyc_Toc_Env*_tmp7C9=
nv;_tmp7CA=*_tmp7C9;_tmp7CB=_tmp7CA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CB,nv,next_l),_tmp7C8);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CD;
struct _RegionHandle*_tmp7CE;struct Cyc_Toc_Env*_tmp7CC=nv;_tmp7CD=*_tmp7CC;
_tmp7CE=_tmp7CD.rgn;{struct Cyc_Toc_Env*_tmp7CF=Cyc_Toc_share_env(_tmp7CE,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7CE,Cyc_Toc_gen_label,
_tmp7CE,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D0=lscs;for(0;_tmp7D0 != 0;
_tmp7D0=_tmp7D0->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D0->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D0->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D0->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D2;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_Stmt*_tmp7D4;struct _tuple21
_tmp7D1=Cyc_Toc_xlate_pat(_tmp7CF,_tmp7CE,_tmp7C2,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D2=_tmp7D1.f1;_tmp7D3=_tmp7D1.f2;_tmp7D4=_tmp7D1.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D5=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D2,_tmp7D5);_tmp7D4=Cyc_Absyn_seq_stmt(
_tmp7D4,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D5,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D3;nvs=({
struct Cyc_List_List*_tmp7D6=_region_malloc(_tmp7CE,sizeof(*_tmp7D6));_tmp7D6->hd=
_tmp7D2;_tmp7D6->tl=nvs;_tmp7D6;});test_stmts=({struct Cyc_List_List*_tmp7D7=
_region_malloc(_tmp7CE,sizeof(*_tmp7D7));_tmp7D7->hd=_tmp7D4;_tmp7D7->tl=
test_stmts;_tmp7D7;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple24 _tmp7D9;struct _dyneither_ptr*_tmp7DA;
struct _dyneither_ptr*_tmp7DB;struct Cyc_Absyn_Switch_clause*_tmp7DC;struct
_tuple24*_tmp7D8=(struct _tuple24*)lscs->hd;_tmp7D9=*_tmp7D8;_tmp7DA=_tmp7D9.f1;
_tmp7DB=_tmp7D9.f2;_tmp7DC=_tmp7D9.f3;{struct Cyc_Toc_Env*_tmp7DD=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DC->body;
struct Cyc_Toc_Env _tmp7DF;struct _RegionHandle*_tmp7E0;struct Cyc_Toc_Env*_tmp7DE=
_tmp7CF;_tmp7DF=*_tmp7DE;_tmp7E0=_tmp7DF.rgn;if(lscs->tl != 0){struct _tuple24
_tmp7E2;struct _dyneither_ptr*_tmp7E3;struct Cyc_Absyn_Switch_clause*_tmp7E4;
struct _tuple24*_tmp7E1=(struct _tuple24*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp7E2=*_tmp7E1;_tmp7E3=_tmp7E2.f2;_tmp7E4=_tmp7E2.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp7E0,_tmp7DD,end_l,_tmp7E3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp7E4->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
_tmp7E0,_tmp7DD,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7DA,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DB,s,0),0);stmts=({struct Cyc_List_List*_tmp7E5=_region_malloc(_tmp7CE,
sizeof(*_tmp7E5));_tmp7E5->hd=s;_tmp7E5->tl=stmts;_tmp7E5;});};}{struct Cyc_Absyn_Stmt*
res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22 _tmp7E7;struct _tuple0*
_tmp7E8;void*_tmp7E9;struct _tuple22*_tmp7E6=(struct _tuple22*)decls->hd;_tmp7E7=*
_tmp7E6;_tmp7E8=_tmp7E7.f1;_tmp7E9=_tmp7E7.f2;res=Cyc_Absyn_declare_stmt(_tmp7E8,
_tmp7E9,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(
int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({
struct Cyc_List_List*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0);_tmp7EA->tl=0;_tmp7EA;}),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp7EB=s->r;struct Cyc_Absyn_Exp*_tmp7EE;
struct Cyc_Absyn_Stmt*_tmp7F0;struct Cyc_Absyn_Stmt*_tmp7F1;struct Cyc_Absyn_Exp*
_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Stmt*_tmp7F6;struct Cyc_Absyn_Stmt*
_tmp7F7;struct _tuple8 _tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Stmt*
_tmp7FB;struct Cyc_Absyn_Stmt*_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FF;struct Cyc_Absyn_Stmt*
_tmp801;struct Cyc_Absyn_Exp*_tmp803;struct _tuple8 _tmp804;struct Cyc_Absyn_Exp*
_tmp805;struct _tuple8 _tmp806;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*
_tmp808;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_List_List*_tmp80B;struct Cyc_List_List*
_tmp80D;struct Cyc_Absyn_Switch_clause**_tmp80E;struct Cyc_Absyn_Decl*_tmp810;
struct Cyc_Absyn_Stmt*_tmp811;struct _dyneither_ptr*_tmp813;struct Cyc_Absyn_Stmt*
_tmp814;struct Cyc_Absyn_Stmt*_tmp816;struct _tuple8 _tmp817;struct Cyc_Absyn_Exp*
_tmp818;struct Cyc_Absyn_Stmt*_tmp81A;struct Cyc_List_List*_tmp81B;struct Cyc_Absyn_Exp*
_tmp81D;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp7EC=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7EB;if(_tmp7EC->tag != 0)goto _LL367;}_LL366: return;_LL367: {struct Cyc_Absyn_Exp_s_struct*
_tmp7ED=(struct Cyc_Absyn_Exp_s_struct*)_tmp7EB;if(_tmp7ED->tag != 1)goto _LL369;
else{_tmp7EE=_tmp7ED->f1;}}_LL368: Cyc_Toc_exp_to_c(nv,_tmp7EE);return;_LL369: {
struct Cyc_Absyn_Seq_s_struct*_tmp7EF=(struct Cyc_Absyn_Seq_s_struct*)_tmp7EB;if(
_tmp7EF->tag != 2)goto _LL36B;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7EF->f2;}}
_LL36A: Cyc_Toc_stmt_to_c(nv,_tmp7F0);s=_tmp7F1;continue;_LL36B: {struct Cyc_Absyn_Return_s_struct*
_tmp7F2=(struct Cyc_Absyn_Return_s_struct*)_tmp7EB;if(_tmp7F2->tag != 3)goto _LL36D;
else{_tmp7F3=_tmp7F2->f1;}}_LL36C: {struct Cyc_Core_Opt*topt=0;if(_tmp7F3 != 0){
topt=({struct Cyc_Core_Opt*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->v=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F3->topt))->v);_tmp81E;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp7F3);}if(s->try_depth > 0){if(topt != 0){struct
_tuple0*_tmp81F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp820=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp81F,0),0);s->r=(Cyc_Absyn_declare_stmt(
_tmp81F,(void*)topt->v,_tmp7F3,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),
_tmp820,0),0))->r;}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}return;}_LL36D: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F4=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp7EB;if(_tmp7F4->tag != 4)goto _LL36F;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F4->f2;
_tmp7F7=_tmp7F4->f3;}}_LL36E: Cyc_Toc_exp_to_c(nv,_tmp7F5);Cyc_Toc_stmt_to_c(nv,
_tmp7F6);s=_tmp7F7;continue;_LL36F: {struct Cyc_Absyn_While_s_struct*_tmp7F8=(
struct Cyc_Absyn_While_s_struct*)_tmp7EB;if(_tmp7F8->tag != 5)goto _LL371;else{
_tmp7F9=_tmp7F8->f1;_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F8->f2;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7FA);{struct Cyc_Toc_Env _tmp822;struct _RegionHandle*_tmp823;struct Cyc_Toc_Env*
_tmp821=nv;_tmp822=*_tmp821;_tmp823=_tmp822.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp823,nv),_tmp7FB);return;};_LL371: {struct Cyc_Absyn_Break_s_struct*_tmp7FC=(
struct Cyc_Absyn_Break_s_struct*)_tmp7EB;if(_tmp7FC->tag != 6)goto _LL373;else{
_tmp7FD=_tmp7FC->f1;}}_LL372: {struct Cyc_Toc_Env _tmp825;struct _dyneither_ptr**
_tmp826;struct Cyc_Toc_Env*_tmp824=nv;_tmp825=*_tmp824;_tmp826=_tmp825.break_lab;
if(_tmp826 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp826,0);{int dest_depth=_tmp7FD == 0?0:
_tmp7FD->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL373: {struct Cyc_Absyn_Continue_s_struct*_tmp7FE=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7EB;if(_tmp7FE->tag != 7)goto _LL375;else{_tmp7FF=_tmp7FE->f1;}}_LL374: {
struct Cyc_Toc_Env _tmp828;struct _dyneither_ptr**_tmp829;struct Cyc_Toc_Env*_tmp827=
nv;_tmp828=*_tmp827;_tmp829=_tmp828.continue_lab;if(_tmp829 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp829,0);_tmp801=_tmp7FF;goto _LL376;}_LL375: {struct Cyc_Absyn_Goto_s_struct*
_tmp800=(struct Cyc_Absyn_Goto_s_struct*)_tmp7EB;if(_tmp800->tag != 8)goto _LL377;
else{_tmp801=_tmp800->f2;}}_LL376: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp801))->try_depth,s);return;_LL377: {struct Cyc_Absyn_For_s_struct*
_tmp802=(struct Cyc_Absyn_For_s_struct*)_tmp7EB;if(_tmp802->tag != 9)goto _LL379;
else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f2;_tmp805=_tmp804.f1;_tmp806=_tmp802->f3;
_tmp807=_tmp806.f1;_tmp808=_tmp802->f4;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp803);Cyc_Toc_exp_to_c(
nv,_tmp805);Cyc_Toc_exp_to_c(nv,_tmp807);{struct Cyc_Toc_Env _tmp82B;struct
_RegionHandle*_tmp82C;struct Cyc_Toc_Env*_tmp82A=nv;_tmp82B=*_tmp82A;_tmp82C=
_tmp82B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82C,nv),_tmp808);return;};
_LL379: {struct Cyc_Absyn_Switch_s_struct*_tmp809=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7EB;if(_tmp809->tag != 10)goto _LL37B;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;}}
_LL37A: Cyc_Toc_xlate_switch(nv,s,_tmp80A,_tmp80B);return;_LL37B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7EB;if(_tmp80C->tag != 11)goto
_LL37D;else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80C->f2;}}_LL37C: {struct Cyc_Toc_Env
_tmp82E;struct Cyc_Toc_FallthruInfo*_tmp82F;struct Cyc_Toc_Env*_tmp82D=nv;_tmp82E=*
_tmp82D;_tmp82F=_tmp82E.fallthru_info;if(_tmp82F == 0)({void*_tmp830=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp831="fallthru in unexpected place";_tag_dyneither(_tmp831,sizeof(char),29);}),
_tag_dyneither(_tmp830,sizeof(void*),0));});{struct _dyneither_ptr*_tmp833;struct
Cyc_List_List*_tmp834;struct Cyc_Dict_Dict _tmp835;struct Cyc_Toc_FallthruInfo
_tmp832=*_tmp82F;_tmp833=_tmp832.label;_tmp834=_tmp832.binders;_tmp835=_tmp832.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp833,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp80E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp834);struct Cyc_List_List*_tmp837=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80D);for(0;_tmp836 != 0;(_tmp836=_tmp836->tl,
_tmp837=_tmp837->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp837))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp835,(
struct _tuple0*)_tmp836->hd),(struct Cyc_Absyn_Exp*)_tmp837->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37D: {struct Cyc_Absyn_Decl_s_struct*_tmp80F=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7EB;if(_tmp80F->tag != 12)goto _LL37F;else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;}}
_LL37E:{void*_tmp838=_tmp810->r;struct Cyc_Absyn_Vardecl*_tmp83A;struct Cyc_Absyn_Pat*
_tmp83C;struct Cyc_Absyn_Exp*_tmp83D;struct Cyc_List_List*_tmp83F;struct Cyc_Absyn_Fndecl*
_tmp841;struct Cyc_Absyn_Tvar*_tmp843;struct Cyc_Absyn_Vardecl*_tmp844;int _tmp845;
struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Tvar*
_tmp849;struct Cyc_Absyn_Vardecl*_tmp84A;_LL388: {struct Cyc_Absyn_Var_d_struct*
_tmp839=(struct Cyc_Absyn_Var_d_struct*)_tmp838;if(_tmp839->tag != 0)goto _LL38A;
else{_tmp83A=_tmp839->f1;}}_LL389: {struct Cyc_Toc_Env _tmp84C;struct _RegionHandle*
_tmp84D;struct Cyc_Toc_Env*_tmp84B=nv;_tmp84C=*_tmp84B;_tmp84D=_tmp84C.rgn;{
struct Cyc_Toc_Env*_tmp84E=Cyc_Toc_add_varmap(_tmp84D,nv,_tmp83A->name,Cyc_Absyn_varb_exp(
_tmp83A->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp84F=_cycalloc(sizeof(*
_tmp84F));_tmp84F[0]=({struct Cyc_Absyn_Local_b_struct _tmp850;_tmp850.tag=4;
_tmp850.f1=_tmp83A;_tmp850;});_tmp84F;}),0));Cyc_Toc_local_decl_to_c(_tmp84E,
_tmp84E,_tmp83A,_tmp811);}goto _LL387;}_LL38A: {struct Cyc_Absyn_Let_d_struct*
_tmp83B=(struct Cyc_Absyn_Let_d_struct*)_tmp838;if(_tmp83B->tag != 2)goto _LL38C;
else{_tmp83C=_tmp83B->f1;_tmp83D=_tmp83B->f3;}}_LL38B:{void*_tmp851=_tmp83C->r;
struct Cyc_Absyn_Vardecl*_tmp853;struct Cyc_Absyn_Pat*_tmp854;struct Cyc_Absyn_Pat
_tmp855;void*_tmp856;_LL397: {struct Cyc_Absyn_Var_p_struct*_tmp852=(struct Cyc_Absyn_Var_p_struct*)
_tmp851;if(_tmp852->tag != 1)goto _LL399;else{_tmp853=_tmp852->f1;_tmp854=_tmp852->f2;
_tmp855=*_tmp854;_tmp856=_tmp855.r;{struct Cyc_Absyn_Wild_p_struct*_tmp857=(
struct Cyc_Absyn_Wild_p_struct*)_tmp856;if(_tmp857->tag != 0)goto _LL399;};}}_LL398: {
struct _tuple0*old_name=_tmp853->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp853->name=new_name;_tmp853->initializer=(struct Cyc_Absyn_Exp*)_tmp83D;
_tmp810->r=(void*)({struct Cyc_Absyn_Var_d_struct*_tmp858=_cycalloc(sizeof(*
_tmp858));_tmp858[0]=({struct Cyc_Absyn_Var_d_struct _tmp859;_tmp859.tag=0;_tmp859.f1=
_tmp853;_tmp859;});_tmp858;});{struct Cyc_Toc_Env _tmp85B;struct _RegionHandle*
_tmp85C;struct Cyc_Toc_Env*_tmp85A=nv;_tmp85B=*_tmp85A;_tmp85C=_tmp85B.rgn;{
struct Cyc_Toc_Env*_tmp85D=Cyc_Toc_add_varmap(_tmp85C,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp85E=_cycalloc(sizeof(*
_tmp85E));_tmp85E[0]=({struct Cyc_Absyn_Local_b_struct _tmp85F;_tmp85F.tag=4;
_tmp85F.f1=_tmp853;_tmp85F;});_tmp85E;}),0));Cyc_Toc_local_decl_to_c(_tmp85D,nv,
_tmp853,_tmp811);}goto _LL396;};}_LL399:;_LL39A: s->r=(Cyc_Toc_letdecl_to_c(nv,
_tmp83C,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83D->topt))->v,_tmp83D,
_tmp811))->r;goto _LL396;_LL396:;}goto _LL387;_LL38C: {struct Cyc_Absyn_Letv_d_struct*
_tmp83E=(struct Cyc_Absyn_Letv_d_struct*)_tmp838;if(_tmp83E->tag != 3)goto _LL38E;
else{_tmp83F=_tmp83E->f1;}}_LL38D: {struct Cyc_List_List*_tmp860=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp83F);if(_tmp860 == 0)({void*_tmp861=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp862="empty Letv_d";_tag_dyneither(_tmp862,sizeof(char),13);}),
_tag_dyneither(_tmp861,sizeof(void*),0));});_tmp810->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863[0]=({struct Cyc_Absyn_Var_d_struct
_tmp864;_tmp864.tag=0;_tmp864.f1=(struct Cyc_Absyn_Vardecl*)_tmp860->hd;_tmp864;});
_tmp863;});_tmp860=_tmp860->tl;for(0;_tmp860 != 0;_tmp860=_tmp860->tl){struct Cyc_Absyn_Decl*
_tmp865=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Absyn_Var_d_struct _tmp867;
_tmp867.tag=0;_tmp867.f1=(struct Cyc_Absyn_Vardecl*)_tmp860->hd;_tmp867;});
_tmp866;}),0);s->r=(Cyc_Absyn_decl_stmt(_tmp865,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
Cyc_Toc_stmt_to_c(nv,s);goto _LL387;}_LL38E: {struct Cyc_Absyn_Fn_d_struct*_tmp840=(
struct Cyc_Absyn_Fn_d_struct*)_tmp838;if(_tmp840->tag != 1)goto _LL390;else{_tmp841=
_tmp840->f1;}}_LL38F: {struct _tuple0*_tmp868=_tmp841->name;struct Cyc_Toc_Env
_tmp86A;struct _RegionHandle*_tmp86B;struct Cyc_Toc_Env*_tmp869=nv;_tmp86A=*
_tmp869;_tmp86B=_tmp86A.rgn;{struct Cyc_Toc_Env*_tmp86C=Cyc_Toc_add_varmap(
_tmp86B,nv,_tmp841->name,Cyc_Absyn_var_exp(_tmp868,0));Cyc_Toc_fndecl_to_c(
_tmp86C,_tmp841,0);Cyc_Toc_stmt_to_c(_tmp86C,_tmp811);}goto _LL387;}_LL390: {
struct Cyc_Absyn_Region_d_struct*_tmp842=(struct Cyc_Absyn_Region_d_struct*)
_tmp838;if(_tmp842->tag != 4)goto _LL392;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;
_tmp845=_tmp842->f3;_tmp846=_tmp842->f4;}}_LL391: {struct Cyc_Absyn_Stmt*_tmp86D=
_tmp811;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp844->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp86F;struct _RegionHandle*_tmp870;struct Cyc_Toc_Env*
_tmp86E=nv;_tmp86F=*_tmp86E;_tmp870=_tmp86F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp870,nv,x_var,x_exp),_tmp86D);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86D,0))->r;
else{if(_tmp846 == 0)s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp871=
_cycalloc(sizeof(*_tmp871));_tmp871->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp871->tl=0;_tmp871;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp872[1];_tmp872[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp872,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp873[1];_tmp873[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp873,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0),0))->r;else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp846);s->r=(Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),
0,Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(
Cyc_Toc__open_dynregion_e,({struct Cyc_Absyn_Exp*_tmp874[2];_tmp874[1]=(struct Cyc_Absyn_Exp*)
_tmp846;_tmp874[0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp874,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp875[1];_tmp875[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp875,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r;}}return;}_LL392: {struct Cyc_Absyn_Alias_d_struct*
_tmp847=(struct Cyc_Absyn_Alias_d_struct*)_tmp838;if(_tmp847->tag != 5)goto _LL394;
else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;_tmp84A=_tmp847->f3;}}_LL393: {
struct _tuple0*old_name=_tmp84A->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp84A->name=new_name;_tmp84A->initializer=(struct Cyc_Absyn_Exp*)_tmp848;s->r=(
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp876=_cycalloc(sizeof(*_tmp876));
_tmp876[0]=({struct Cyc_Absyn_Decl_s_struct _tmp877;_tmp877.tag=12;_tmp877.f1=({
struct Cyc_Absyn_Decl*_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878->r=(void*)({
struct Cyc_Absyn_Var_d_struct*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({
struct Cyc_Absyn_Var_d_struct _tmp87A;_tmp87A.tag=0;_tmp87A.f1=_tmp84A;_tmp87A;});
_tmp879;});_tmp878->loc=0;_tmp878;});_tmp877.f2=_tmp811;_tmp877;});_tmp876;});{
struct Cyc_Toc_Env _tmp87C;struct _RegionHandle*_tmp87D;struct Cyc_Toc_Env*_tmp87B=
nv;_tmp87C=*_tmp87B;_tmp87D=_tmp87C.rgn;{struct Cyc_Toc_Env*_tmp87E=Cyc_Toc_add_varmap(
_tmp87D,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_Local_b_struct
_tmp880;_tmp880.tag=4;_tmp880.f1=_tmp84A;_tmp880;});_tmp87F;}),0));Cyc_Toc_local_decl_to_c(
_tmp87E,nv,_tmp84A,_tmp811);}return;};}_LL394:;_LL395:({void*_tmp881=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp882="bad nested declaration within function";_tag_dyneither(_tmp882,sizeof(
char),39);}),_tag_dyneither(_tmp881,sizeof(void*),0));});_LL387:;}return;_LL37F: {
struct Cyc_Absyn_Label_s_struct*_tmp812=(struct Cyc_Absyn_Label_s_struct*)_tmp7EB;
if(_tmp812->tag != 13)goto _LL381;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}
_LL380: s=_tmp814;continue;_LL381: {struct Cyc_Absyn_Do_s_struct*_tmp815=(struct
Cyc_Absyn_Do_s_struct*)_tmp7EB;if(_tmp815->tag != 14)goto _LL383;else{_tmp816=
_tmp815->f1;_tmp817=_tmp815->f2;_tmp818=_tmp817.f1;}}_LL382: {struct Cyc_Toc_Env
_tmp884;struct _RegionHandle*_tmp885;struct Cyc_Toc_Env*_tmp883=nv;_tmp884=*
_tmp883;_tmp885=_tmp884.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp885,nv),
_tmp816);Cyc_Toc_exp_to_c(nv,_tmp818);return;}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp819=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7EB;if(_tmp819->tag != 15)goto
_LL385;else{_tmp81A=_tmp819->f1;_tmp81B=_tmp819->f2;}}_LL384: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp886=_cycalloc(sizeof(*_tmp886));_tmp886->v=e_typ;_tmp886;});{struct Cyc_Toc_Env
_tmp888;struct _RegionHandle*_tmp889;struct Cyc_Toc_Env*_tmp887=nv;_tmp888=*
_tmp887;_tmp889=_tmp888.rgn;{struct Cyc_Toc_Env*_tmp88A=Cyc_Toc_add_varmap(
_tmp889,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp88A,_tmp81A);{struct Cyc_Absyn_Stmt*
_tmp88B=Cyc_Absyn_seq_stmt(_tmp81A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp88C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88D=
Cyc_Absyn_var_exp(_tmp88C,0);struct Cyc_Absyn_Vardecl*_tmp88E=Cyc_Absyn_new_vardecl(
_tmp88C,Cyc_Absyn_exn_typ(),0);_tmp88D->topt=({struct Cyc_Core_Opt*_tmp88F=
_cycalloc(sizeof(*_tmp88F));_tmp88F->v=Cyc_Absyn_exn_typ();_tmp88F;});{struct Cyc_Absyn_Pat*
_tmp890=({struct Cyc_Absyn_Pat*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->r=(
void*)({struct Cyc_Absyn_Var_p_struct*_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2[
0]=({struct Cyc_Absyn_Var_p_struct _tmp8A3;_tmp8A3.tag=1;_tmp8A3.f1=_tmp88E;
_tmp8A3.f2=({struct Cyc_Absyn_Pat*_tmp8A4=_cycalloc(sizeof(*_tmp8A4));_tmp8A4->r=(
void*)& Cyc_Absyn_Wild_p_val;_tmp8A4->topt=({struct Cyc_Core_Opt*_tmp8A5=_cycalloc(
sizeof(*_tmp8A5));_tmp8A5->v=Cyc_Absyn_exn_typ();_tmp8A5;});_tmp8A4->loc=0;
_tmp8A4;});_tmp8A3;});_tmp8A2;});_tmp8A0->topt=({struct Cyc_Core_Opt*_tmp8A1=
_cycalloc(sizeof(*_tmp8A1));_tmp8A1->v=Cyc_Absyn_exn_typ();_tmp8A1;});_tmp8A0->loc=
0;_tmp8A0;});struct Cyc_Absyn_Exp*_tmp891=Cyc_Absyn_throw_exp(_tmp88D,0);_tmp891->topt=({
struct Cyc_Core_Opt*_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892->v=(void*)& Cyc_Absyn_VoidType_val;
_tmp892;});{struct Cyc_Absyn_Stmt*_tmp893=Cyc_Absyn_exp_stmt(_tmp891,0);struct Cyc_Absyn_Switch_clause*
_tmp894=({struct Cyc_Absyn_Switch_clause*_tmp89D=_cycalloc(sizeof(*_tmp89D));
_tmp89D->pattern=_tmp890;_tmp89D->pat_vars=({struct Cyc_Core_Opt*_tmp89E=
_cycalloc(sizeof(*_tmp89E));_tmp89E->v=({struct Cyc_List_List*_tmp89F=_cycalloc(
sizeof(*_tmp89F));_tmp89F->hd=_tmp88E;_tmp89F->tl=0;_tmp89F;});_tmp89E;});
_tmp89D->where_clause=0;_tmp89D->body=_tmp893;_tmp89D->loc=0;_tmp89D;});struct
Cyc_Absyn_Stmt*_tmp895=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp81B,({struct
Cyc_List_List*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C->hd=_tmp894;_tmp89C->tl=
0;_tmp89C;})),0);Cyc_Toc_stmt_to_c(_tmp88A,_tmp895);{struct Cyc_Absyn_Exp*_tmp896=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp89B=_cycalloc(
sizeof(*_tmp89B));_tmp89B->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp89B->tl=0;_tmp89B;}),0);struct Cyc_Absyn_Stmt*_tmp897=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp89A=
_cycalloc(sizeof(*_tmp89A));_tmp89A->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp89A->tl=
0;_tmp89A;}),0),0);struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
0,0);struct Cyc_Absyn_Exp*_tmp899=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp897,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp898,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp896,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp899,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88B,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp895,0),0),0),0),0),0))->r;};};};};}
return;};}_LL385: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81C=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7EB;if(_tmp81C->tag != 16)goto _LL364;else{_tmp81D=_tmp81C->f1;}}_LL386: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81D);s->r=Cyc_Toc_exp_stmt_r(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp8A6=
_cycalloc(sizeof(*_tmp8A6));_tmp8A6->hd=_tmp81D;_tmp8A6->tl=0;_tmp8A6;}),0));}
return;_LL364:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{struct _RegionHandle
_tmp8A7=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A7;_push_region(frgn);{
struct Cyc_Toc_Env*_tmp8A8=Cyc_Toc_share_env(frgn,nv);{struct Cyc_List_List*
_tmp8A9=f->args;for(0;_tmp8A9 != 0;_tmp8A9=_tmp8A9->tl){struct _tuple0*_tmp8AA=({
struct _tuple0*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp8AB->f2=(*((struct _tuple25*)_tmp8A9->hd)).f1;_tmp8AB;});(*((
struct _tuple25*)_tmp8A9->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp8A9->hd)).f3);
_tmp8A8=Cyc_Toc_add_varmap(frgn,_tmp8A8,_tmp8AA,Cyc_Absyn_var_exp(_tmp8AA,0));}}
if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);_npop_handler(0);return;}if((
unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0){struct Cyc_Core_Opt*_tmp8AD;struct Cyc_Absyn_Tqual _tmp8AE;void*_tmp8AF;int
_tmp8B0;struct Cyc_Absyn_VarargInfo _tmp8AC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AD=_tmp8AC.name;_tmp8AE=_tmp8AC.tq;_tmp8AF=
_tmp8AC.type;_tmp8B0=_tmp8AC.inject;{void*_tmp8B1=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8AF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8AE,Cyc_Absyn_false_conref));struct
_tuple0*_tmp8B2=({struct _tuple0*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5->f1=(
union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n;_tmp8B5->f2=(struct _dyneither_ptr*)((
struct Cyc_Core_Opt*)_check_null(_tmp8AD))->v;_tmp8B5;});f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct
Cyc_List_List*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3->hd=({struct _tuple25*
_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4->f1=(struct _dyneither_ptr*)_tmp8AD->v;
_tmp8B4->f2=_tmp8AE;_tmp8B4->f3=_tmp8B1;_tmp8B4;});_tmp8B3->tl=0;_tmp8B3;}));
_tmp8A8=Cyc_Toc_add_varmap(frgn,_tmp8A8,_tmp8B2,Cyc_Absyn_var_exp(_tmp8B2,0));f->cyc_varargs=
0;};}{struct Cyc_List_List*_tmp8B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(f->param_vardecls))->v;for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){((
struct Cyc_Absyn_Vardecl*)_tmp8B6->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)
_tmp8B6->hd)->type);}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8A8),f->body);};
_pop_region(frgn);};}static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope
s){switch(s){case Cyc_Absyn_Abstract: _LL39B: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC:
_LL39C: return Cyc_Absyn_Extern;default: _LL39D: return s;}}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*_tmp8B7=ad->name;struct
_DynRegionHandle*_tmp8B9;struct Cyc_Dict_Dict*_tmp8BA;struct Cyc_Toc_TocState
_tmp8B8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B9=
_tmp8B8.dyn;_tmp8BA=_tmp8B8.aggrs_so_far;{struct _DynRegionFrame _tmp8BB;struct
_RegionHandle*d=_open_dynregion(& _tmp8BB,_tmp8B9);{int seen_defn_before;struct
_tuple14**_tmp8BC=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))
Cyc_Dict_lookup_opt)(*_tmp8BA,_tmp8B7);if(_tmp8BC == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA)v=({struct _tuple14*_tmp8BD=
_region_malloc(d,sizeof(*_tmp8BD));_tmp8BD->f1=ad;_tmp8BD->f2=Cyc_Absyn_strctq(
_tmp8B7);_tmp8BD;});else{v=({struct _tuple14*_tmp8BE=_region_malloc(d,sizeof(*
_tmp8BE));_tmp8BE->f1=ad;_tmp8BE->f2=Cyc_Absyn_unionq_typ(_tmp8B7);_tmp8BE;});}*
_tmp8BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct
_tuple14*v))Cyc_Dict_insert)(*_tmp8BA,_tmp8B7,v);};}else{struct _tuple14 _tmp8C0;
struct Cyc_Absyn_Aggrdecl*_tmp8C1;void*_tmp8C2;struct _tuple14*_tmp8BF=*_tmp8BC;
_tmp8C0=*_tmp8BF;_tmp8C1=_tmp8C0.f1;_tmp8C2=_tmp8C0.f2;if(_tmp8C1->impl == 0){*
_tmp8BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct
_tuple14*v))Cyc_Dict_insert)(*_tmp8BA,_tmp8B7,({struct _tuple14*_tmp8C3=
_region_malloc(d,sizeof(*_tmp8C3));_tmp8C3->f1=ad;_tmp8C3->f2=_tmp8C2;_tmp8C3;}));
seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*new_ad=({
struct Cyc_Absyn_Aggrdecl*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));_tmp8E2->kind=ad->kind;
_tmp8E2->sc=Cyc_Absyn_Public;_tmp8E2->name=ad->name;_tmp8E2->tvs=0;_tmp8E2->impl=
0;_tmp8E2->attributes=ad->attributes;_tmp8E2;});if(ad->impl != 0  && !
seen_defn_before){new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8C4=_cycalloc(
sizeof(*_tmp8C4));_tmp8C4->exist_vars=0;_tmp8C4->rgn_po=0;_tmp8C4->fields=0;
_tmp8C4->tagged=0;_tmp8C4;});{struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*
_tmp8C5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){struct Cyc_Absyn_Aggrfield*_tmp8C6=(struct Cyc_Absyn_Aggrfield*)
_tmp8C5->hd;struct Cyc_Absyn_Aggrfield*_tmp8C7=({struct Cyc_Absyn_Aggrfield*
_tmp8DD=_cycalloc(sizeof(*_tmp8DD));_tmp8DD->name=_tmp8C6->name;_tmp8DD->tq=Cyc_Toc_mt_tq;
_tmp8DD->type=Cyc_Toc_typ_to_c_array(_tmp8C6->type);_tmp8DD->width=_tmp8C6->width;
_tmp8DD->attributes=_tmp8C6->attributes;_tmp8DD;});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged){void*_tmp8C8=_tmp8C7->type;struct _dyneither_ptr*
_tmp8C9=_tmp8C7->name;struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp8DB;_tmp8DB.tag=0;_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp8C9);({struct Cyc_String_pa_struct _tmp8DA;_tmp8DA.tag=0;_tmp8DA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2);({void*_tmp8D8[2]={&
_tmp8DA,& _tmp8DB};Cyc_aprintf(({const char*_tmp8D9="_union_%s_%s";_tag_dyneither(
_tmp8D9,sizeof(char),13);}),_tag_dyneither(_tmp8D8,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp8D7=_cycalloc(sizeof(*
_tmp8D7));_tmp8D7[0]=s;_tmp8D7;});struct Cyc_Absyn_Aggrfield*_tmp8CA=({struct Cyc_Absyn_Aggrfield*
_tmp8D6=_cycalloc(sizeof(*_tmp8D6));_tmp8D6->name=Cyc_Toc_val_sp;_tmp8D6->tq=Cyc_Toc_mt_tq;
_tmp8D6->type=_tmp8C8;_tmp8D6->width=0;_tmp8D6->attributes=0;_tmp8D6;});struct
Cyc_Absyn_Aggrfield*_tmp8CB=({struct Cyc_Absyn_Aggrfield*_tmp8D5=_cycalloc(
sizeof(*_tmp8D5));_tmp8D5->name=Cyc_Toc_tag_sp;_tmp8D5->tq=Cyc_Toc_mt_tq;_tmp8D5->type=
Cyc_Absyn_sint_typ;_tmp8D5->width=0;_tmp8D5->attributes=0;_tmp8D5;});struct Cyc_List_List*
_tmp8CC=({struct Cyc_Absyn_Aggrfield*_tmp8D4[2];_tmp8D4[1]=_tmp8CA;_tmp8D4[0]=
_tmp8CB;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp8D4,sizeof(struct Cyc_Absyn_Aggrfield*),2));});struct Cyc_Absyn_Aggrdecl*
_tmp8CD=({struct Cyc_Absyn_Aggrdecl*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1->kind=
Cyc_Absyn_StructA;_tmp8D1->sc=Cyc_Absyn_Public;_tmp8D1->name=({struct _tuple0*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3->f1=Cyc_Absyn_Loc_n;_tmp8D3->f2=str;
_tmp8D3;});_tmp8D1->tvs=0;_tmp8D1->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8D2=
_cycalloc(sizeof(*_tmp8D2));_tmp8D2->exist_vars=0;_tmp8D2->rgn_po=0;_tmp8D2->fields=
_tmp8CC;_tmp8D2->tagged=0;_tmp8D2;});_tmp8D1->attributes=0;_tmp8D1;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));_tmp8CE->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));
_tmp8CF[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp8D0;_tmp8D0.tag=6;_tmp8D0.f1=
_tmp8CD;_tmp8D0;});_tmp8CF;}),0);_tmp8CE->tl=Cyc_Toc_result_decls;_tmp8CE;});
_tmp8C7->type=Cyc_Absyn_strct(str);}new_fields=({struct Cyc_List_List*_tmp8DC=
_cycalloc(sizeof(*_tmp8DC));_tmp8DC->hd=_tmp8C7;_tmp8DC->tl=new_fields;_tmp8DC;});}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8DE=_cycalloc(
sizeof(*_tmp8DE));_tmp8DE->hd=({struct Cyc_Absyn_Decl*_tmp8DF=_cycalloc(sizeof(*
_tmp8DF));_tmp8DF->r=(void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp8E0=_cycalloc(
sizeof(*_tmp8E0));_tmp8E0[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp8E1;_tmp8E1.tag=
6;_tmp8E1.f1=new_ad;_tmp8E1;});_tmp8E0;});_tmp8DF->loc=0;_tmp8DF;});_tmp8DE->tl=
Cyc_Toc_result_decls;_tmp8DE;});};};_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud){struct _DynRegionHandle*_tmp8E4;struct Cyc_Set_Set**
_tmp8E5;struct Cyc_Toc_TocState _tmp8E3=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8E4=_tmp8E3.dyn;_tmp8E5=_tmp8E3.datatypes_so_far;{struct _DynRegionFrame
_tmp8E6;struct _RegionHandle*d=_open_dynregion(& _tmp8E6,_tmp8E4);{struct _tuple0*
_tmp8E7=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct
_tuple0*elt))Cyc_Set_member)(*_tmp8E5,_tmp8E7)){_npop_handler(0);return;}*
_tmp8E5=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct
_tuple0*elt))Cyc_Set_rinsert)(d,*_tmp8E5,_tmp8E7);}{struct Cyc_List_List*_tmp8E8=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;
_tmp8E8 != 0;_tmp8E8=_tmp8E8->tl){struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)
_tmp8E8->hd;struct Cyc_List_List*_tmp8E9=0;int i=1;{struct Cyc_List_List*_tmp8EA=f->typs;
for(0;_tmp8EA != 0;(_tmp8EA=_tmp8EA->tl,i ++)){struct _dyneither_ptr*_tmp8EB=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp8EC=({struct Cyc_Absyn_Aggrfield*_tmp8EE=
_cycalloc(sizeof(*_tmp8EE));_tmp8EE->name=_tmp8EB;_tmp8EE->tq=(*((struct _tuple9*)
_tmp8EA->hd)).f1;_tmp8EE->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8EA->hd)).f2);
_tmp8EE->width=0;_tmp8EE->attributes=0;_tmp8EE;});_tmp8E9=({struct Cyc_List_List*
_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED->hd=_tmp8EC;_tmp8ED->tl=_tmp8E9;
_tmp8ED;});}}_tmp8E9=({struct Cyc_List_List*_tmp8EF=_cycalloc(sizeof(*_tmp8EF));
_tmp8EF->hd=({struct Cyc_Absyn_Aggrfield*_tmp8F0=_cycalloc(sizeof(*_tmp8F0));
_tmp8F0->name=Cyc_Toc_tag_sp;_tmp8F0->tq=Cyc_Toc_mt_tq;_tmp8F0->type=Cyc_Absyn_sint_typ;
_tmp8F0->width=0;_tmp8F0->attributes=0;_tmp8F0;});_tmp8EF->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8E9);_tmp8EF;});{struct Cyc_Absyn_Aggrdecl*
_tmp8F1=({struct Cyc_Absyn_Aggrdecl*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5->kind=
Cyc_Absyn_StructA;_tmp8F5->sc=Cyc_Absyn_Public;_tmp8F5->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp8F7="_struct";_tag_dyneither(_tmp8F7,sizeof(char),8);}));
_tmp8F5->tvs=0;_tmp8F5->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8F6=_cycalloc(
sizeof(*_tmp8F6));_tmp8F6->exist_vars=0;_tmp8F6->rgn_po=0;_tmp8F6->fields=
_tmp8E9;_tmp8F6->tagged=0;_tmp8F6;});_tmp8F5->attributes=0;_tmp8F5;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp8F3=_cycalloc(sizeof(*_tmp8F3));
_tmp8F3[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp8F4;_tmp8F4.tag=6;_tmp8F4.f1=
_tmp8F1;_tmp8F4;});_tmp8F3;}),0);_tmp8F2->tl=Cyc_Toc_result_decls;_tmp8F2;});};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp8F9;struct Cyc_Dict_Dict*
_tmp8FA;struct Cyc_Toc_TocState _tmp8F8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8F9=_tmp8F8.dyn;_tmp8FA=_tmp8F8.xdatatypes_so_far;{struct _DynRegionFrame
_tmp8FB;struct _RegionHandle*d=_open_dynregion(& _tmp8FB,_tmp8F9);{struct _tuple0*
_tmp8FC=xd->name;struct Cyc_List_List*_tmp8FD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FD->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp8FF=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp8FE,Cyc_Absyn_false_conref,0);int*_tmp900=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8FA,f->name);int
_tmp901;_LL3A0: if(_tmp900 != 0)goto _LL3A2;_LL3A1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp902=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,
initopt);_tmp902->sc=f->sc;Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp903=
_cycalloc(sizeof(*_tmp903));_tmp903->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904[0]=({struct Cyc_Absyn_Var_d_struct
_tmp905;_tmp905.tag=0;_tmp905.f1=_tmp902;_tmp905;});_tmp904;}),0);_tmp903->tl=
Cyc_Toc_result_decls;_tmp903;});*_tmp8FA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8FA,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*_tmp906=f->typs;for(0;
_tmp906 != 0;(_tmp906=_tmp906->tl,i ++)){struct _dyneither_ptr*_tmp907=({struct
_dyneither_ptr*_tmp90B=_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp90E;_tmp90E.tag=1;_tmp90E.f1=(
unsigned long)i;({void*_tmp90C[1]={& _tmp90E};Cyc_aprintf(({const char*_tmp90D="f%d";
_tag_dyneither(_tmp90D,sizeof(char),4);}),_tag_dyneither(_tmp90C,sizeof(void*),1));});});
_tmp90B;});struct Cyc_Absyn_Aggrfield*_tmp908=({struct Cyc_Absyn_Aggrfield*_tmp90A=
_cycalloc(sizeof(*_tmp90A));_tmp90A->name=_tmp907;_tmp90A->tq=(*((struct _tuple9*)
_tmp906->hd)).f1;_tmp90A->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp906->hd)).f2);
_tmp90A->width=0;_tmp90A->attributes=0;_tmp90A;});fields=({struct Cyc_List_List*
_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909->hd=_tmp908;_tmp909->tl=fields;
_tmp909;});}}fields=({struct Cyc_List_List*_tmp90F=_cycalloc(sizeof(*_tmp90F));
_tmp90F->hd=({struct Cyc_Absyn_Aggrfield*_tmp910=_cycalloc(sizeof(*_tmp910));
_tmp910->name=Cyc_Toc_tag_sp;_tmp910->tq=Cyc_Toc_mt_tq;_tmp910->type=Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp910->width=0;_tmp910->attributes=0;_tmp910;});
_tmp90F->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp90F;});{struct Cyc_Absyn_Aggrdecl*_tmp911=({struct Cyc_Absyn_Aggrdecl*
_tmp915=_cycalloc(sizeof(*_tmp915));_tmp915->kind=Cyc_Absyn_StructA;_tmp915->sc=
Cyc_Absyn_Public;_tmp915->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp917="_struct";_tag_dyneither(_tmp917,sizeof(char),8);}));_tmp915->tvs=0;
_tmp915->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp916=_cycalloc(sizeof(*_tmp916));
_tmp916->exist_vars=0;_tmp916->rgn_po=0;_tmp916->fields=fields;_tmp916->tagged=0;
_tmp916;});_tmp915->attributes=0;_tmp915;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp912=_cycalloc(sizeof(*_tmp912));_tmp912->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp913=_cycalloc(sizeof(*_tmp913));_tmp913[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp914;_tmp914.tag=6;_tmp914.f1=_tmp911;_tmp914;});
_tmp913;}),0);_tmp912->tl=Cyc_Toc_result_decls;_tmp912;});goto _LL39F;};};};}
_LL3A2: if(_tmp900 == 0)goto _LL3A4;_tmp901=*_tmp900;if(_tmp901 != 0)goto _LL3A4;
_LL3A3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp918=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp919=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,(
struct Cyc_Absyn_Exp*)_tmp918);_tmp919->sc=f->sc;Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp91A=_cycalloc(sizeof(*_tmp91A));_tmp91A->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp91B=_cycalloc(sizeof(*_tmp91B));_tmp91B[0]=({
struct Cyc_Absyn_Var_d_struct _tmp91C;_tmp91C.tag=0;_tmp91C.f1=_tmp919;_tmp91C;});
_tmp91B;}),0);_tmp91A->tl=Cyc_Toc_result_decls;_tmp91A;});*_tmp8FA=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8FA,f->name,1);}
goto _LL39F;_LL3A4:;_LL3A5: goto _LL39F;_LL39F:;}};_pop_dynregion(d);};};}static
void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){ed->sc=
Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp91D=init->r;struct Cyc_Absyn_Vardecl*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;
struct Cyc_Absyn_Exp*_tmp921;int _tmp922;_LL3A7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp91E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp91D;if(_tmp91E->tag != 28)
goto _LL3A9;else{_tmp91F=_tmp91E->f1;_tmp920=_tmp91E->f2;_tmp921=_tmp91E->f3;
_tmp922=_tmp91E->f4;}}_LL3A8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp91F,_tmp920,_tmp921,_tmp922,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A6;_LL3A9:;_LL3AA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp924;struct _RegionHandle*_tmp925;struct Cyc_Toc_Env*_tmp923=init_nv;_tmp924=*
_tmp923;_tmp925=_tmp924.rgn;{struct Cyc_Toc_Env*_tmp926=Cyc_Toc_set_toplevel(
_tmp925,init_nv);Cyc_Toc_exp_to_c(_tmp926,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A6;_LL3A6:;}else{void*_tmp927=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp929;void*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;union Cyc_Absyn_Constraint*
_tmp92C;_LL3AC:{struct Cyc_Absyn_ArrayType_struct*_tmp928=(struct Cyc_Absyn_ArrayType_struct*)
_tmp927;if(_tmp928->tag != 9)goto _LL3AE;else{_tmp929=_tmp928->f1;_tmp92A=_tmp929.elt_type;
_tmp92B=_tmp929.num_elts;_tmp92C=_tmp929.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92C))goto _LL3AE;_LL3AD: if(_tmp92B == 0)({void*
_tmp92D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp92E="can't initialize zero-terminated array -- size unknown";
_tag_dyneither(_tmp92E,sizeof(char),55);}),_tag_dyneither(_tmp92D,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92B;struct Cyc_Absyn_Exp*
_tmp92F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp930=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp92F,
_tmp930,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AB;};_LL3AE:;_LL3AF: goto _LL3AB;
_LL3AB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp932;struct
_RegionHandle*_tmp933;struct Cyc_Toc_Env*_tmp931=nv;_tmp932=*_tmp931;_tmp933=
_tmp932.rgn;{struct Cyc_Absyn_Stmt*_tmp934=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp935=Cyc_Toc_share_env(_tmp933,nv);struct Cyc_Toc_Env*_tmp937;struct Cyc_List_List*
_tmp938;struct Cyc_Absyn_Stmt*_tmp939;struct _tuple21 _tmp936=Cyc_Toc_xlate_pat(
_tmp935,_tmp933,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp937=_tmp936.f1;_tmp938=_tmp936.f2;_tmp939=_tmp936.f3;Cyc_Toc_stmt_to_c(
_tmp937,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp939,s,0),0);for(0;_tmp938 != 0;_tmp938=_tmp938->tl){
struct _tuple22 _tmp93B;struct _tuple0*_tmp93C;void*_tmp93D;struct _tuple22*_tmp93A=(
struct _tuple22*)_tmp938->hd;_tmp93B=*_tmp93A;_tmp93C=_tmp93B.f1;_tmp93D=_tmp93B.f2;
s=Cyc_Absyn_declare_stmt(_tmp93C,_tmp93D,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp93E=e->r;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_Absyn_Exp*
_tmp942;struct Cyc_Absyn_Exp*_tmp944;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*
_tmp948;struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*
_tmp94E;struct Cyc_List_List*_tmp950;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*
_tmp953;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*
_tmp958;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*
_tmp95C;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*
_tmp961;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*
_tmp965;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_List_List*
_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_List_List*_tmp96C;void*_tmp96E;
void**_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct _tuple7*_tmp972;struct _tuple7
_tmp973;void*_tmp974;void**_tmp975;struct Cyc_List_List*_tmp976;struct Cyc_List_List*
_tmp978;struct Cyc_List_List*_tmp97A;void*_tmp97C;void**_tmp97D;void*_tmp97F;void**
_tmp980;struct Cyc_Absyn_Stmt*_tmp982;struct Cyc_Absyn_MallocInfo _tmp984;struct Cyc_Absyn_MallocInfo*
_tmp985;_LL3B1: {struct Cyc_Absyn_Deref_e_struct*_tmp93F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp93E;if(_tmp93F->tag != 21)goto _LL3B3;else{_tmp940=_tmp93F->f1;}}_LL3B2:
_tmp942=_tmp940;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp941=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp93E;if(_tmp941->tag != 22)goto _LL3B5;
else{_tmp942=_tmp941->f1;}}_LL3B4: _tmp944=_tmp942;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp943=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp93E;if(_tmp943->tag != 23)goto
_LL3B7;else{_tmp944=_tmp943->f1;}}_LL3B6: _tmp946=_tmp944;goto _LL3B8;_LL3B7: {
struct Cyc_Absyn_Address_e_struct*_tmp945=(struct Cyc_Absyn_Address_e_struct*)
_tmp93E;if(_tmp945->tag != 16)goto _LL3B9;else{_tmp946=_tmp945->f1;}}_LL3B8:
_tmp948=_tmp946;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_struct*_tmp947=(
struct Cyc_Absyn_Throw_e_struct*)_tmp93E;if(_tmp947->tag != 12)goto _LL3BB;else{
_tmp948=_tmp947->f1;}}_LL3BA: _tmp94A=_tmp948;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp949=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp93E;if(_tmp949->tag != 13)
goto _LL3BD;else{_tmp94A=_tmp949->f1;}}_LL3BC: _tmp94C=_tmp94A;goto _LL3BE;_LL3BD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp93E;if(_tmp94B->tag != 19)goto _LL3BF;else{_tmp94C=_tmp94B->f1;}}_LL3BE:
_tmp94E=_tmp94C;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_struct*_tmp94D=(
struct Cyc_Absyn_Increment_e_struct*)_tmp93E;if(_tmp94D->tag != 5)goto _LL3C1;else{
_tmp94E=_tmp94D->f1;}}_LL3C0: Cyc_Toc_exptypes_to_c(_tmp94E);goto _LL3B0;_LL3C1: {
struct Cyc_Absyn_Primop_e_struct*_tmp94F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp93E;if(_tmp94F->tag != 3)goto _LL3C3;else{_tmp950=_tmp94F->f2;}}_LL3C2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp950);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_struct*_tmp951=(struct Cyc_Absyn_And_e_struct*)
_tmp93E;if(_tmp951->tag != 7)goto _LL3C5;else{_tmp952=_tmp951->f1;_tmp953=_tmp951->f2;}}
_LL3C4: _tmp955=_tmp952;_tmp956=_tmp953;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_struct*
_tmp954=(struct Cyc_Absyn_Or_e_struct*)_tmp93E;if(_tmp954->tag != 8)goto _LL3C7;
else{_tmp955=_tmp954->f1;_tmp956=_tmp954->f2;}}_LL3C6: _tmp958=_tmp955;_tmp959=
_tmp956;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp957=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp93E;if(_tmp957->tag != 9)goto _LL3C9;else{_tmp958=_tmp957->f1;_tmp959=_tmp957->f2;}}
_LL3C8: _tmp95B=_tmp958;_tmp95C=_tmp959;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp95A=(struct Cyc_Absyn_Subscript_e_struct*)_tmp93E;if(_tmp95A->tag != 24)goto
_LL3CB;else{_tmp95B=_tmp95A->f1;_tmp95C=_tmp95A->f2;}}_LL3CA: _tmp95E=_tmp95B;
_tmp95F=_tmp95C;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_struct*_tmp95D=(
struct Cyc_Absyn_Swap_e_struct*)_tmp93E;if(_tmp95D->tag != 35)goto _LL3CD;else{
_tmp95E=_tmp95D->f1;_tmp95F=_tmp95D->f2;}}_LL3CC: _tmp961=_tmp95E;_tmp962=_tmp95F;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp960=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp93E;if(_tmp960->tag != 4)goto _LL3CF;else{_tmp961=_tmp960->f1;_tmp962=_tmp960->f3;}}
_LL3CE: Cyc_Toc_exptypes_to_c(_tmp961);Cyc_Toc_exptypes_to_c(_tmp962);goto _LL3B0;
_LL3CF: {struct Cyc_Absyn_Conditional_e_struct*_tmp963=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp93E;if(_tmp963->tag != 6)goto _LL3D1;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;
_tmp966=_tmp963->f3;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp964);Cyc_Toc_exptypes_to_c(
_tmp965);Cyc_Toc_exptypes_to_c(_tmp966);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp967=(struct Cyc_Absyn_FnCall_e_struct*)_tmp93E;if(_tmp967->tag != 11)goto
_LL3D3;else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3D2: _tmp96B=_tmp968;
_tmp96C=_tmp969;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp96A=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp93E;if(_tmp96A->tag != 10)goto _LL3D5;
else{_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;}}_LL3D4: Cyc_Toc_exptypes_to_c(
_tmp96B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96C);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_Cast_e_struct*
_tmp96D=(struct Cyc_Absyn_Cast_e_struct*)_tmp93E;if(_tmp96D->tag != 15)goto _LL3D7;
else{_tmp96E=(void**)& _tmp96D->f1;_tmp96F=(void**)((void**)& _tmp96D->f1);_tmp970=
_tmp96D->f2;}}_LL3D6:*_tmp96F=Cyc_Toc_typ_to_c(*_tmp96F);Cyc_Toc_exptypes_to_c(
_tmp970);goto _LL3B0;_LL3D7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp971=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp93E;if(_tmp971->tag != 26)goto _LL3D9;
else{_tmp972=_tmp971->f1;_tmp973=*_tmp972;_tmp974=_tmp973.f3;_tmp975=(void**)&(*
_tmp971->f1).f3;_tmp976=_tmp971->f2;}}_LL3D8:*_tmp975=Cyc_Toc_typ_to_c(*_tmp975);
_tmp978=_tmp976;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp977=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp93E;if(_tmp977->tag != 36)
goto _LL3DB;else{_tmp978=_tmp977->f2;}}_LL3DA: _tmp97A=_tmp978;goto _LL3DC;_LL3DB: {
struct Cyc_Absyn_Array_e_struct*_tmp979=(struct Cyc_Absyn_Array_e_struct*)_tmp93E;
if(_tmp979->tag != 27)goto _LL3DD;else{_tmp97A=_tmp979->f1;}}_LL3DC: for(0;_tmp97A
!= 0;_tmp97A=_tmp97A->tl){struct Cyc_Absyn_Exp*_tmp995;struct _tuple15 _tmp994=*((
struct _tuple15*)_tmp97A->hd);_tmp995=_tmp994.f2;Cyc_Toc_exptypes_to_c(_tmp995);}
goto _LL3B0;_LL3DD: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97B=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp93E;if(_tmp97B->tag != 20)goto _LL3DF;else{_tmp97C=(void**)& _tmp97B->f1;
_tmp97D=(void**)((void**)& _tmp97B->f1);}}_LL3DE: _tmp980=_tmp97D;goto _LL3E0;
_LL3DF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp97E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp93E;if(_tmp97E->tag != 18)goto _LL3E1;else{_tmp97F=(void**)& _tmp97E->f1;
_tmp980=(void**)((void**)& _tmp97E->f1);}}_LL3E0:*_tmp980=Cyc_Toc_typ_to_c(*
_tmp980);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp981=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp93E;if(_tmp981->tag != 37)goto _LL3E3;else{_tmp982=_tmp981->f1;}}_LL3E2: Cyc_Toc_stmttypes_to_c(
_tmp982);goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Malloc_e_struct*_tmp983=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp93E;if(_tmp983->tag != 34)goto _LL3E5;else{_tmp984=_tmp983->f1;_tmp985=(struct
Cyc_Absyn_MallocInfo*)& _tmp983->f1;}}_LL3E4: if(_tmp985->elt_type != 0)_tmp985->elt_type=({
void**_tmp996=_cycalloc(sizeof(*_tmp996));_tmp996[0]=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp985->elt_type)));_tmp996;});Cyc_Toc_exptypes_to_c(_tmp985->num_elts);
goto _LL3B0;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp986=(struct Cyc_Absyn_Const_e_struct*)
_tmp93E;if(_tmp986->tag != 0)goto _LL3E7;}_LL3E6: goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Var_e_struct*
_tmp987=(struct Cyc_Absyn_Var_e_struct*)_tmp93E;if(_tmp987->tag != 1)goto _LL3E9;}
_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnknownId_e_struct*_tmp988=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp93E;if(_tmp988->tag != 2)goto _LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Enum_e_struct*
_tmp989=(struct Cyc_Absyn_Enum_e_struct*)_tmp93E;if(_tmp989->tag != 32)goto _LL3ED;}
_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp98A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp93E;if(_tmp98A->tag != 33)goto _LL3EF;}_LL3EE: goto _LL3B0;_LL3EF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp93E;if(_tmp98B->tag != 30)goto
_LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Datatype_e_struct*_tmp98C=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp93E;if(_tmp98C->tag != 31)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp93E;if(_tmp98D->tag != 29)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp98E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp93E;if(_tmp98E->tag != 28)
goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_struct*_tmp98F=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp93E;if(_tmp98F->tag != 25)goto _LL3F9;}_LL3F8:
goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_struct*_tmp990=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp93E;if(_tmp990->tag != 14)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_struct*
_tmp991=(struct Cyc_Absyn_New_e_struct*)_tmp93E;if(_tmp991->tag != 17)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_struct*_tmp992=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp93E;if(_tmp992->tag != 39)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp993=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp93E;if(_tmp993->tag != 38)goto
_LL3B0;}_LL400:({void*_tmp997=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp998="Cyclone expression within C code";
_tag_dyneither(_tmp998,sizeof(char),33);}),_tag_dyneither(_tmp997,sizeof(void*),
0));});goto _LL3B0;_LL3B0:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*
d){void*_tmp999=d->r;struct Cyc_Absyn_Vardecl*_tmp99B;struct Cyc_Absyn_Fndecl*
_tmp99D;struct Cyc_Absyn_Aggrdecl*_tmp99F;struct Cyc_Absyn_Enumdecl*_tmp9A1;struct
Cyc_Absyn_Typedefdecl*_tmp9A3;_LL402: {struct Cyc_Absyn_Var_d_struct*_tmp99A=(
struct Cyc_Absyn_Var_d_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL404;else{
_tmp99B=_tmp99A->f1;}}_LL403: _tmp99B->type=Cyc_Toc_typ_to_c(_tmp99B->type);if(
_tmp99B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(
_tmp99B->initializer));goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_struct*_tmp99C=(
struct Cyc_Absyn_Fn_d_struct*)_tmp999;if(_tmp99C->tag != 1)goto _LL406;else{_tmp99D=
_tmp99C->f1;}}_LL405: _tmp99D->ret_type=Cyc_Toc_typ_to_c(_tmp99D->ret_type);{
struct Cyc_List_List*_tmp9AF=_tmp99D->args;for(0;_tmp9AF != 0;_tmp9AF=_tmp9AF->tl){(*((
struct _tuple25*)_tmp9AF->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9AF->hd)).f3);}}
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_struct*_tmp99E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp999;if(_tmp99E->tag != 6)goto _LL408;else{_tmp99F=_tmp99E->f1;}}_LL407: Cyc_Toc_aggrdecl_to_c(
_tmp99F);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_struct*_tmp9A0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp999;if(_tmp9A0->tag != 8)goto _LL40A;else{_tmp9A1=_tmp9A0->f1;}}_LL409: if(
_tmp9A1->fields != 0){struct Cyc_List_List*_tmp9B0=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A1->fields))->v;for(0;_tmp9B0 != 0;_tmp9B0=_tmp9B0->tl){
struct Cyc_Absyn_Enumfield*_tmp9B1=(struct Cyc_Absyn_Enumfield*)_tmp9B0->hd;if(
_tmp9B1->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B1->tag));}}
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A2=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp999;if(_tmp9A2->tag != 9)goto _LL40C;else{_tmp9A3=_tmp9A2->f1;}}_LL40B: _tmp9A3->defn=({
struct Cyc_Core_Opt*_tmp9B2=_cycalloc(sizeof(*_tmp9B2));_tmp9B2->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp9A3->defn))->v);_tmp9B2;});goto
_LL401;_LL40C: {struct Cyc_Absyn_Let_d_struct*_tmp9A4=(struct Cyc_Absyn_Let_d_struct*)
_tmp999;if(_tmp9A4->tag != 2)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_struct*
_tmp9A5=(struct Cyc_Absyn_Letv_d_struct*)_tmp999;if(_tmp9A5->tag != 3)goto _LL410;}
_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_struct*_tmp9A6=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp999;if(_tmp9A6->tag != 7)goto _LL412;}_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_struct*
_tmp9A7=(struct Cyc_Absyn_Namespace_d_struct*)_tmp999;if(_tmp9A7->tag != 10)goto
_LL414;}_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_struct*_tmp9A8=(
struct Cyc_Absyn_Using_d_struct*)_tmp999;if(_tmp9A8->tag != 11)goto _LL416;}_LL415:
goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_struct*_tmp9A9=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp999;if(_tmp9A9->tag != 12)goto _LL418;}_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AA=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp999;if(_tmp9AA->tag != 13)
goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_struct*_tmp9AB=(
struct Cyc_Absyn_Region_d_struct*)_tmp999;if(_tmp9AB->tag != 4)goto _LL41C;}_LL41B:
goto _LL41D;_LL41C: {struct Cyc_Absyn_Alias_d_struct*_tmp9AC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp999;if(_tmp9AC->tag != 5)goto _LL41E;}_LL41D:({void*_tmp9B3=0;Cyc_Tcutil_terr(
d->loc,({const char*_tmp9B4="Cyclone declaration within C code";_tag_dyneither(
_tmp9B4,sizeof(char),34);}),_tag_dyneither(_tmp9B3,sizeof(void*),0));});goto
_LL401;_LL41E: {struct Cyc_Absyn_Porton_d_struct*_tmp9AD=(struct Cyc_Absyn_Porton_d_struct*)
_tmp999;if(_tmp9AD->tag != 14)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9AE=(struct Cyc_Absyn_Portoff_d_struct*)_tmp999;if(_tmp9AE->tag != 15)goto
_LL401;}_LL421: goto _LL401;_LL401:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9B5=s->r;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Stmt*_tmp9B9;
struct Cyc_Absyn_Stmt*_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_Absyn_Exp*
_tmp9BE;struct Cyc_Absyn_Stmt*_tmp9BF;struct Cyc_Absyn_Stmt*_tmp9C0;struct _tuple8
_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Stmt*_tmp9C4;struct Cyc_Absyn_Exp*
_tmp9C6;struct _tuple8 _tmp9C7;struct Cyc_Absyn_Exp*_tmp9C8;struct _tuple8 _tmp9C9;
struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Exp*
_tmp9CD;struct Cyc_List_List*_tmp9CE;struct Cyc_Absyn_Decl*_tmp9D0;struct Cyc_Absyn_Stmt*
_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D3;struct _tuple8 _tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;_LL423: {struct Cyc_Absyn_Exp_s_struct*_tmp9B6=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9B5;if(_tmp9B6->tag != 1)goto _LL425;else{_tmp9B7=_tmp9B6->f1;}}_LL424: Cyc_Toc_exptypes_to_c(
_tmp9B7);goto _LL422;_LL425: {struct Cyc_Absyn_Seq_s_struct*_tmp9B8=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B5;if(_tmp9B8->tag != 2)goto _LL427;else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B8->f2;}}
_LL426: Cyc_Toc_stmttypes_to_c(_tmp9B9);Cyc_Toc_stmttypes_to_c(_tmp9BA);goto
_LL422;_LL427: {struct Cyc_Absyn_Return_s_struct*_tmp9BB=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B5;if(_tmp9BB->tag != 3)goto _LL429;else{_tmp9BC=_tmp9BB->f1;}}_LL428: if(
_tmp9BC != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BC);goto _LL422;
_LL429: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9BD=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B5;if(_tmp9BD->tag != 4)goto _LL42B;else{_tmp9BE=_tmp9BD->f1;_tmp9BF=_tmp9BD->f2;
_tmp9C0=_tmp9BD->f3;}}_LL42A: Cyc_Toc_exptypes_to_c(_tmp9BE);Cyc_Toc_stmttypes_to_c(
_tmp9BF);Cyc_Toc_stmttypes_to_c(_tmp9C0);goto _LL422;_LL42B: {struct Cyc_Absyn_While_s_struct*
_tmp9C1=(struct Cyc_Absyn_While_s_struct*)_tmp9B5;if(_tmp9C1->tag != 5)goto _LL42D;
else{_tmp9C2=_tmp9C1->f1;_tmp9C3=_tmp9C2.f1;_tmp9C4=_tmp9C1->f2;}}_LL42C: Cyc_Toc_exptypes_to_c(
_tmp9C3);Cyc_Toc_stmttypes_to_c(_tmp9C4);goto _LL422;_LL42D: {struct Cyc_Absyn_For_s_struct*
_tmp9C5=(struct Cyc_Absyn_For_s_struct*)_tmp9B5;if(_tmp9C5->tag != 9)goto _LL42F;
else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f2;_tmp9C8=_tmp9C7.f1;_tmp9C9=_tmp9C5->f3;
_tmp9CA=_tmp9C9.f1;_tmp9CB=_tmp9C5->f4;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9C6);
Cyc_Toc_exptypes_to_c(_tmp9C8);Cyc_Toc_exptypes_to_c(_tmp9CA);Cyc_Toc_stmttypes_to_c(
_tmp9CB);goto _LL422;_LL42F: {struct Cyc_Absyn_Switch_s_struct*_tmp9CC=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B5;if(_tmp9CC->tag != 10)goto _LL431;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}
_LL430: Cyc_Toc_exptypes_to_c(_tmp9CD);for(0;_tmp9CE != 0;_tmp9CE=_tmp9CE->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9CE->hd)->body);}goto _LL422;_LL431: {struct
Cyc_Absyn_Decl_s_struct*_tmp9CF=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B5;if(
_tmp9CF->tag != 12)goto _LL433;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}
_LL432: Cyc_Toc_decltypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(_tmp9D1);goto
_LL422;_LL433: {struct Cyc_Absyn_Do_s_struct*_tmp9D2=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B5;if(_tmp9D2->tag != 14)goto _LL435;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;
_tmp9D5=_tmp9D4.f1;}}_LL434: Cyc_Toc_stmttypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(
_tmp9D5);goto _LL422;_LL435: {struct Cyc_Absyn_Skip_s_struct*_tmp9D6=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B5;if(_tmp9D6->tag != 0)goto _LL437;}_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_Break_s_struct*
_tmp9D7=(struct Cyc_Absyn_Break_s_struct*)_tmp9B5;if(_tmp9D7->tag != 6)goto _LL439;}
_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Continue_s_struct*_tmp9D8=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B5;if(_tmp9D8->tag != 7)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Goto_s_struct*
_tmp9D9=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B5;if(_tmp9D9->tag != 8)goto _LL43D;}
_LL43C: goto _LL422;_LL43D: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DA=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B5;if(_tmp9DA->tag != 11)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Label_s_struct*
_tmp9DB=(struct Cyc_Absyn_Label_s_struct*)_tmp9B5;if(_tmp9DB->tag != 13)goto _LL441;}
_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DC=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B5;if(_tmp9DC->tag != 15)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9DD=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B5;if(_tmp9DD->tag != 16)goto
_LL422;}_LL444:({void*_tmp9DE=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp9DF="Cyclone statement in C code";
_tag_dyneither(_tmp9DF,sizeof(char),28);}),_tag_dyneither(_tmp9DE,sizeof(void*),
0));});goto _LL422;_LL422:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({void*_tmp9E0=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9E1="decls_to_c: not at toplevel!";_tag_dyneither(_tmp9E1,sizeof(char),29);}),
_tag_dyneither(_tmp9E0,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E2=d->r;struct Cyc_Absyn_Vardecl*_tmp9E4;struct Cyc_Absyn_Fndecl*
_tmp9E6;struct Cyc_Absyn_Aggrdecl*_tmp9EC;struct Cyc_Absyn_Datatypedecl*_tmp9EE;
struct Cyc_Absyn_Enumdecl*_tmp9F0;struct Cyc_Absyn_Typedefdecl*_tmp9F2;struct Cyc_List_List*
_tmp9F6;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9FA;struct Cyc_List_List*
_tmp9FC;_LL446: {struct Cyc_Absyn_Var_d_struct*_tmp9E3=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E2;if(_tmp9E3->tag != 0)goto _LL448;else{_tmp9E4=_tmp9E3->f1;}}_LL447: {
struct _tuple0*_tmp9FD=_tmp9E4->name;if(_tmp9E4->sc == Cyc_Absyn_ExternC)_tmp9FD=({
struct _tuple0*_tmp9FE=_cycalloc(sizeof(*_tmp9FE));_tmp9FE->f1=Cyc_Absyn_Rel_n(0);
_tmp9FE->f2=(*_tmp9FD).f2;_tmp9FE;});if(_tmp9E4->initializer != 0){if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E4->name,Cyc_Absyn_varb_exp(_tmp9FD,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp9FF=_cycalloc(sizeof(*_tmp9FF));_tmp9FF[0]=({
struct Cyc_Absyn_Global_b_struct _tmpA00;_tmpA00.tag=1;_tmpA00.f1=_tmp9E4;_tmpA00;});
_tmp9FF;}),0));_tmp9E4->name=_tmp9FD;_tmp9E4->sc=Cyc_Toc_scope_to_c(_tmp9E4->sc);
_tmp9E4->type=Cyc_Toc_typ_to_c_array(_tmp9E4->type);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpA01=_cycalloc(sizeof(*_tmpA01));_tmpA01->hd=d;_tmpA01->tl=
Cyc_Toc_result_decls;_tmpA01;});goto _LL445;}_LL448: {struct Cyc_Absyn_Fn_d_struct*
_tmp9E5=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E2;if(_tmp9E5->tag != 1)goto _LL44A;
else{_tmp9E6=_tmp9E5->f1;}}_LL449: {struct _tuple0*_tmpA02=_tmp9E6->name;if(
_tmp9E6->sc == Cyc_Absyn_ExternC){_tmpA02=({struct _tuple0*_tmpA03=_cycalloc(
sizeof(*_tmpA03));_tmpA03->f1=Cyc_Absyn_Rel_n(0);_tmpA03->f2=(*_tmpA02).f2;
_tmpA03;});_tmp9E6->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,nv,_tmp9E6->name,
Cyc_Absyn_var_exp(_tmpA02,0));_tmp9E6->name=_tmpA02;Cyc_Toc_fndecl_to_c(nv,
_tmp9E6,cinclude);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpA04=_cycalloc(
sizeof(*_tmpA04));_tmpA04->hd=d;_tmpA04->tl=Cyc_Toc_result_decls;_tmpA04;});goto
_LL445;}_LL44A: {struct Cyc_Absyn_Let_d_struct*_tmp9E7=(struct Cyc_Absyn_Let_d_struct*)
_tmp9E2;if(_tmp9E7->tag != 2)goto _LL44C;}_LL44B: goto _LL44D;_LL44C: {struct Cyc_Absyn_Letv_d_struct*
_tmp9E8=(struct Cyc_Absyn_Letv_d_struct*)_tmp9E2;if(_tmp9E8->tag != 3)goto _LL44E;}
_LL44D:({void*_tmpA05=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmpA06="letdecl at toplevel";_tag_dyneither(
_tmpA06,sizeof(char),20);}),_tag_dyneither(_tmpA05,sizeof(void*),0));});_LL44E: {
struct Cyc_Absyn_Region_d_struct*_tmp9E9=(struct Cyc_Absyn_Region_d_struct*)
_tmp9E2;if(_tmp9E9->tag != 4)goto _LL450;}_LL44F:({void*_tmpA07=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpA08="region decl at toplevel";
_tag_dyneither(_tmpA08,sizeof(char),24);}),_tag_dyneither(_tmpA07,sizeof(void*),
0));});_LL450: {struct Cyc_Absyn_Alias_d_struct*_tmp9EA=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E2;if(_tmp9EA->tag != 5)goto _LL452;}_LL451:({void*_tmpA09=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpA0A="alias decl at toplevel";
_tag_dyneither(_tmpA0A,sizeof(char),23);}),_tag_dyneither(_tmpA09,sizeof(void*),
0));});_LL452: {struct Cyc_Absyn_Aggr_d_struct*_tmp9EB=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9E2;if(_tmp9EB->tag != 6)goto _LL454;else{_tmp9EC=_tmp9EB->f1;}}_LL453: Cyc_Toc_aggrdecl_to_c(
_tmp9EC);goto _LL445;_LL454: {struct Cyc_Absyn_Datatype_d_struct*_tmp9ED=(struct
Cyc_Absyn_Datatype_d_struct*)_tmp9E2;if(_tmp9ED->tag != 7)goto _LL456;else{_tmp9EE=
_tmp9ED->f1;}}_LL455: if(_tmp9EE->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmp9EE);else{Cyc_Toc_datatypedecl_to_c(_tmp9EE);}goto _LL445;_LL456: {struct Cyc_Absyn_Enum_d_struct*
_tmp9EF=(struct Cyc_Absyn_Enum_d_struct*)_tmp9E2;if(_tmp9EF->tag != 8)goto _LL458;
else{_tmp9F0=_tmp9EF->f1;}}_LL457: Cyc_Toc_enumdecl_to_c(nv,_tmp9F0);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpA0B=_cycalloc(sizeof(*_tmpA0B));_tmpA0B->hd=d;_tmpA0B->tl=
Cyc_Toc_result_decls;_tmpA0B;});goto _LL445;_LL458: {struct Cyc_Absyn_Typedef_d_struct*
_tmp9F1=(struct Cyc_Absyn_Typedef_d_struct*)_tmp9E2;if(_tmp9F1->tag != 9)goto
_LL45A;else{_tmp9F2=_tmp9F1->f1;}}_LL459: _tmp9F2->name=_tmp9F2->name;_tmp9F2->tvs=
0;if(_tmp9F2->defn != 0)_tmp9F2->defn=({struct Cyc_Core_Opt*_tmpA0C=_cycalloc(
sizeof(*_tmpA0C));_tmpA0C->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F2->defn))->v);_tmpA0C;});else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F2->kind))->v){case Cyc_Absyn_BoxKind: _LL466:
_tmp9F2->defn=({struct Cyc_Core_Opt*_tmpA0D=_cycalloc(sizeof(*_tmpA0D));_tmpA0D->v=
Cyc_Absyn_void_star_typ();_tmpA0D;});break;default: _LL467: _tmp9F2->defn=({struct
Cyc_Core_Opt*_tmpA0E=_cycalloc(sizeof(*_tmpA0E));_tmpA0E->v=(void*)& Cyc_Absyn_VoidType_val;
_tmpA0E;});break;}}Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpA0F=_cycalloc(
sizeof(*_tmpA0F));_tmpA0F->hd=d;_tmpA0F->tl=Cyc_Toc_result_decls;_tmpA0F;});goto
_LL445;_LL45A: {struct Cyc_Absyn_Porton_d_struct*_tmp9F3=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E2;if(_tmp9F3->tag != 14)goto _LL45C;}_LL45B: goto _LL45D;_LL45C: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F4=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E2;if(_tmp9F4->tag != 15)goto
_LL45E;}_LL45D: goto _LL445;_LL45E: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F5=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E2;if(_tmp9F5->tag != 10)goto _LL460;
else{_tmp9F6=_tmp9F5->f2;}}_LL45F: _tmp9F8=_tmp9F6;goto _LL461;_LL460: {struct Cyc_Absyn_Using_d_struct*
_tmp9F7=(struct Cyc_Absyn_Using_d_struct*)_tmp9E2;if(_tmp9F7->tag != 11)goto _LL462;
else{_tmp9F8=_tmp9F7->f2;}}_LL461: _tmp9FA=_tmp9F8;goto _LL463;_LL462: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F9=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E2;if(_tmp9F9->tag != 12)goto
_LL464;else{_tmp9FA=_tmp9F9->f1;}}_LL463: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FA,top,
cinclude);goto _LL445;_LL464: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FB=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E2;if(_tmp9FB->tag != 13)goto _LL445;
else{_tmp9FC=_tmp9FB->f1;}}_LL465: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FC,top,1);goto
_LL445;_LL445:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA11;struct Cyc_Core_NewRegion _tmpA10=Cyc_Core_new_dynregion();_tmpA11=_tmpA10.dynregion;{
struct _DynRegionFrame _tmpA12;struct _RegionHandle*d=_open_dynregion(& _tmpA12,
_tmpA11);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*_tmpA13=_cycalloc(sizeof(*
_tmpA13));_tmpA13->dyn=(struct _DynRegionHandle*)_tmpA11;_tmpA13->tuple_types=(
struct Cyc_List_List**)({struct Cyc_List_List**_tmpA18=_region_malloc(d,sizeof(*
_tmpA18));_tmpA18[0]=0;_tmpA18;});_tmpA13->aggrs_so_far=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmpA17=_region_malloc(d,sizeof(*_tmpA17));_tmpA17[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA17;});_tmpA13->datatypes_so_far=(
struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmpA16=_region_malloc(d,sizeof(*
_tmpA16));_tmpA16[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA16;});
_tmpA13->xdatatypes_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmpA15=
_region_malloc(d,sizeof(*_tmpA15));_tmpA15[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmpA15;});_tmpA13->qvar_tags=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmpA14=_region_malloc(d,sizeof(*_tmpA14));_tmpA14[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(
d,Cyc_Toc_qvar_tag_cmp);_tmpA14;});_tmpA13->temp_labels=(struct Cyc_Xarray_Xarray*)((
struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);
_tmpA13;});Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmpA19=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38);_tmpA19[0]=& Cyc_Toc__throw_str;_tmpA19[1]=&
Cyc_Toc_setjmp_str;_tmpA19[2]=& Cyc_Toc__push_handler_str;_tmpA19[3]=& Cyc_Toc__pop_handler_str;
_tmpA19[4]=& Cyc_Toc__exn_thrown_str;_tmpA19[5]=& Cyc_Toc__npop_handler_str;
_tmpA19[6]=& Cyc_Toc__check_null_str;_tmpA19[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmpA19[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmpA19[9]=& Cyc_Toc__check_dyneither_subscript_str;
_tmpA19[10]=& Cyc_Toc__dyneither_ptr_str;_tmpA19[11]=& Cyc_Toc__tag_dyneither_str;
_tmpA19[12]=& Cyc_Toc__init_dyneither_ptr_str;_tmpA19[13]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmpA19[14]=& Cyc_Toc__get_dyneither_size_str;_tmpA19[15]=& Cyc_Toc__get_zero_arr_size_str;
_tmpA19[16]=& Cyc_Toc__dyneither_ptr_plus_str;_tmpA19[17]=& Cyc_Toc__zero_arr_plus_str;
_tmpA19[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmpA19[19]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmpA19[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmpA19[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmpA19[22]=& Cyc_Toc__cycalloc_str;_tmpA19[23]=& Cyc_Toc__cyccalloc_str;_tmpA19[
24]=& Cyc_Toc__cycalloc_atomic_str;_tmpA19[25]=& Cyc_Toc__cyccalloc_atomic_str;
_tmpA19[26]=& Cyc_Toc__region_malloc_str;_tmpA19[27]=& Cyc_Toc__region_calloc_str;
_tmpA19[28]=& Cyc_Toc__check_times_str;_tmpA19[29]=& Cyc_Toc__new_region_str;
_tmpA19[30]=& Cyc_Toc__push_region_str;_tmpA19[31]=& Cyc_Toc__pop_region_str;
_tmpA19[32]=& Cyc_Toc__open_dynregion_str;_tmpA19[33]=& Cyc_Toc__push_dynregion_str;
_tmpA19[34]=& Cyc_Toc__pop_dynregion_str;_tmpA19[35]=& Cyc_Toc__reset_region_str;
_tmpA19[36]=& Cyc_Toc__throw_arraybounds_str;_tmpA19[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmpA19;}),sizeof(struct _dyneither_ptr*),38);;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA1A=
_new_region("start");struct _RegionHandle*start=& _tmpA1A;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA1C;struct Cyc_Toc_TocState
_tmpA1B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1C=
_tmpA1B.dyn;Cyc_Core_free_dynregion(_tmpA1C);};{struct Cyc_List_List*_tmpA1D=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA1D;};;_pop_region(start);};}
