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
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict
d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*,struct Cyc_List_List*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,
enum Cyc_Absyn_Kind k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple9*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";static struct _dyneither_ptr
Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _dyneither_ptr*Cyc_Tc_tc_msg=(
struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple11{struct Cyc_Position_Segment*
f1;struct _tuple0*f2;int f3;};static int Cyc_Tc_export_member(struct _tuple0*x,struct
Cyc_List_List*exports);static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*
exports){for(0;exports != 0;exports=exports->tl){struct _tuple11*_tmp1=(struct
_tuple11*)exports->hd;if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){(*_tmp1).f3=1;
return 1;}}return 0;}struct _tuple12{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init,int ignore_init,struct Cyc_List_List**
exports);static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,
int ignore_init,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl _tmp3;enum 
Cyc_Absyn_Scope _tmp4;struct _tuple0*_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace
_tmp7;struct _dyneither_ptr*_tmp8;void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*
_tmpB;struct Cyc_Absyn_Vardecl*_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;
_tmp6=*_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;
_tmpB=_tmp3.attributes;{union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*
_tmpD;struct Cyc_List_List*_tmpE;_LL1: if((_tmpC.Rel_n).tag != 1)goto _LL3;_tmpD=(
struct Cyc_List_List*)(_tmpC.Rel_n).val;if(_tmpD != 0)goto _LL3;_LL2: goto _LL4;_LL3:
if((_tmpC.Abs_n).tag != 2)goto _LL5;_tmpE=(struct Cyc_List_List*)(_tmpC.Abs_n).val;
if(_tmpE != 0)goto _LL5;_LL4: goto _LL0;_LL5:;_LL6:{const char*_tmp262;void*_tmp261[1];
struct Cyc_String_pa_struct _tmp260;(_tmp260.tag=0,((_tmp260.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp261[0]=& _tmp260,Cyc_Tcutil_terr(loc,((_tmp262="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp262,sizeof(char),57))),_tag_dyneither(_tmp261,sizeof(void*),1)))))));}
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns);{void*_tmp12=Cyc_Tcutil_compress(
_tmp9);struct Cyc_Absyn_ArrayInfo _tmp14;void*_tmp15;struct Cyc_Absyn_Tqual _tmp16;
struct Cyc_Absyn_Exp*_tmp17;union Cyc_Absyn_Constraint*_tmp18;struct Cyc_Position_Segment*
_tmp19;_LL8:{struct Cyc_Absyn_ArrayType_struct*_tmp13=(struct Cyc_Absyn_ArrayType_struct*)
_tmp12;if(_tmp13->tag != 9)goto _LLA;else{_tmp14=_tmp13->f1;_tmp15=_tmp14.elt_type;
_tmp16=_tmp14.tq;_tmp17=_tmp14.num_elts;if(_tmp17 != 0)goto _LLA;_tmp18=_tmp14.zero_term;
_tmp19=_tmp14.zt_loc;}}if(!(_tmpA != 0))goto _LLA;_LL9:{void*_tmp1A=_tmpA->r;union
Cyc_Absyn_Cnst _tmp1C;struct _dyneither_ptr _tmp1D;struct Cyc_Absyn_Exp*_tmp1F;
struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp23;_LLD: {struct Cyc_Absyn_Const_e_struct*
_tmp1B=(struct Cyc_Absyn_Const_e_struct*)_tmp1A;if(_tmp1B->tag != 0)goto _LLF;else{
_tmp1C=_tmp1B->f1;if((_tmp1C.String_c).tag != 7)goto _LLF;_tmp1D=(struct
_dyneither_ptr)(_tmp1C.String_c).val;}}_LLE: _tmp9=(vd->type=Cyc_Absyn_array_typ(
_tmp15,_tmp16,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(
_tmp1D,sizeof(char)),0),_tmp18,_tmp19));goto _LLC;_LLF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A;if(_tmp1E->tag != 28)goto
_LL11;else{_tmp1F=_tmp1E->f2;}}_LL10: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp15,
_tmp16,(struct Cyc_Absyn_Exp*)_tmp1F,_tmp18,_tmp19));goto _LLC;_LL11: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp20=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1A;if(_tmp20->tag != 36)goto
_LL13;else{_tmp21=_tmp20->f2;}}_LL12: _tmp23=_tmp21;goto _LL14;_LL13: {struct Cyc_Absyn_Array_e_struct*
_tmp22=(struct Cyc_Absyn_Array_e_struct*)_tmp1A;if(_tmp22->tag != 27)goto _LL15;
else{_tmp23=_tmp22->f1;}}_LL14: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp15,_tmp16,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp23),0),_tmp18,_tmp19));goto _LLC;_LL15:;_LL16: goto _LLC;
_LLC:;}goto _LL7;_LLA:;_LLB: goto _LL7;_LL7:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,
_tmp9);{void*_tmp24=Cyc_Tcutil_compress(_tmp9);_LL18: {struct Cyc_Absyn_ArrayType_struct*
_tmp25=(struct Cyc_Absyn_ArrayType_struct*)_tmp24;if(_tmp25->tag != 9)goto _LL1A;}
_LL19: vd->escapes=0;goto _LL17;_LL1A:;_LL1B: vd->escapes=1;goto _LL17;_LL17:;}if(
Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(_tmp9,
_tmpB);if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){if(_tmpA != 0){
const char*_tmp265;void*_tmp264;(_tmp264=0,Cyc_Tcutil_terr(loc,((_tmp265="extern declaration should not have initializer",
_tag_dyneither(_tmp265,sizeof(char),47))),_tag_dyneither(_tmp264,sizeof(void*),0)));}}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp28=(void*)_tmpB->hd;_LL1D: {struct Cyc_Absyn_Aligned_att_struct*_tmp29=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp28;if(_tmp29->tag != 6)goto _LL1F;}_LL1E:
goto _LL20;_LL1F: {struct Cyc_Absyn_Section_att_struct*_tmp2A=(struct Cyc_Absyn_Section_att_struct*)
_tmp28;if(_tmp2A->tag != 8)goto _LL21;}_LL20: goto _LL22;_LL21: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp2B=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp28;if(_tmp2B->tag != 9)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Shared_att_struct*_tmp2C=(struct Cyc_Absyn_Shared_att_struct*)
_tmp28;if(_tmp2C->tag != 10)goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Unused_att_struct*
_tmp2D=(struct Cyc_Absyn_Unused_att_struct*)_tmp28;if(_tmp2D->tag != 11)goto _LL27;}
_LL26: goto _LL28;_LL27: {struct Cyc_Absyn_Weak_att_struct*_tmp2E=(struct Cyc_Absyn_Weak_att_struct*)
_tmp28;if(_tmp2E->tag != 12)goto _LL29;}_LL28: goto _LL2A;_LL29: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp2F=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp28;if(_tmp2F->tag != 13)goto
_LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Dllexport_att_struct*_tmp30=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp28;if(_tmp30->tag != 14)goto _LL2D;}
_LL2C: continue;_LL2D:;_LL2E:{const char*_tmp26A;void*_tmp269[2];struct Cyc_String_pa_struct
_tmp268;struct Cyc_String_pa_struct _tmp267;(_tmp267.tag=0,((_tmp267.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp268.tag=0,((_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB->hd)),((_tmp269[0]=& _tmp268,((_tmp269[1]=& _tmp267,Cyc_Tcutil_terr(
loc,((_tmp26A="bad attribute %s for variable %s",_tag_dyneither(_tmp26A,sizeof(
char),33))),_tag_dyneither(_tmp269,sizeof(void*),2)))))))))))));}goto _LL1C;_LL1C:;}
if(_tmpA == 0  || ignore_init){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp26F;void*_tmp26E[2];struct Cyc_String_pa_struct _tmp26D;
struct Cyc_String_pa_struct _tmp26C;(_tmp26C.tag=0,((_tmp26C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp26D.tag=
0,((_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp26E[0]=& _tmp26D,((_tmp26E[1]=& _tmp26C,Cyc_Tcutil_terr(loc,((
_tmp26F="initializer required for variable %s of type %s",_tag_dyneither(_tmp26F,
sizeof(char),48))),_tag_dyneither(_tmp26E,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp39=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp3A=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp39);if(!Cyc_Tcutil_coerce_assign(te,_tmp39,_tmp9)){{const
char*_tmp275;void*_tmp274[3];struct Cyc_String_pa_struct _tmp273;struct Cyc_String_pa_struct
_tmp272;struct Cyc_String_pa_struct _tmp271;(_tmp271.tag=0,((_tmp271.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3A)),((_tmp272.tag=
0,((_tmp272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9)),((_tmp273.tag=0,((_tmp273.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp274[0]=& _tmp273,((
_tmp274[1]=& _tmp272,((_tmp274[2]=& _tmp271,Cyc_Tcutil_terr(loc,((_tmp275="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp275,sizeof(char),57))),_tag_dyneither(_tmp274,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp39)){const char*
_tmp278;void*_tmp277;(_tmp277=0,Cyc_Tcutil_terr(loc,((_tmp278="initializer is not a constant expression",
_tag_dyneither(_tmp278,sizeof(char),41))),_tag_dyneither(_tmp277,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp42=(void*)_tmpB->hd;_LL30: {
struct Cyc_Absyn_Regparm_att_struct*_tmp43=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp42;if(_tmp43->tag != 0)goto _LL32;}_LL31: goto _LL33;_LL32: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp44=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp42;if(_tmp44->tag != 1)goto _LL34;}
_LL33: goto _LL35;_LL34: {struct Cyc_Absyn_Cdecl_att_struct*_tmp45=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp42;if(_tmp45->tag != 2)goto _LL36;}_LL35: goto _LL37;_LL36: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp46=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp42;if(_tmp46->tag != 3)goto _LL38;}
_LL37: goto _LL39;_LL38: {struct Cyc_Absyn_Noreturn_att_struct*_tmp47=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp42;if(_tmp47->tag != 4)goto _LL3A;}_LL39: goto _LL3B;_LL3A: {struct Cyc_Absyn_Format_att_struct*
_tmp48=(struct Cyc_Absyn_Format_att_struct*)_tmp42;if(_tmp48->tag != 19)goto _LL3C;}
_LL3B: goto _LL3D;_LL3C: {struct Cyc_Absyn_Initializes_att_struct*_tmp49=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp42;if(_tmp49->tag != 20)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Absyn_Pure_att_struct*
_tmp4A=(struct Cyc_Absyn_Pure_att_struct*)_tmp42;if(_tmp4A->tag != 21)goto _LL40;}
_LL3F: goto _LL41;_LL40: {struct Cyc_Absyn_Const_att_struct*_tmp4B=(struct Cyc_Absyn_Const_att_struct*)
_tmp42;if(_tmp4B->tag != 5)goto _LL42;}_LL41: {const char*_tmp27B;void*_tmp27A;(
_tmp27A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp27B="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp27B,
sizeof(char),45))),_tag_dyneither(_tmp27A,sizeof(void*),0)));}_LL42: {struct Cyc_Absyn_Aligned_att_struct*
_tmp4C=(struct Cyc_Absyn_Aligned_att_struct*)_tmp42;if(_tmp4C->tag != 6)goto _LL44;}
_LL43: goto _LL45;_LL44: {struct Cyc_Absyn_Packed_att_struct*_tmp4D=(struct Cyc_Absyn_Packed_att_struct*)
_tmp42;if(_tmp4D->tag != 7)goto _LL46;}_LL45:{const char*_tmp27F;void*_tmp27E[1];
struct Cyc_String_pa_struct _tmp27D;(_tmp27D.tag=0,((_tmp27D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp27E[0]=& _tmp27D,Cyc_Tcutil_terr(loc,((_tmp27F="bad attribute %s in function declaration",
_tag_dyneither(_tmp27F,sizeof(char),41))),_tag_dyneither(_tmp27E,sizeof(void*),1)))))));}
goto _LL2F;_LL46:;_LL47: continue;_LL2F:;}}}{struct _handler_cons _tmp53;
_push_handler(& _tmp53);{int _tmp55=0;if(setjmp(_tmp53.handler))_tmp55=1;if(!
_tmp55){{struct _tuple12*_tmp56=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp8);void*_tmp57=(*_tmp56).f1;
void*_tmp59;_LL49: {struct Cyc_Tcenv_VarRes_struct*_tmp58=(struct Cyc_Tcenv_VarRes_struct*)
_tmp57;if(_tmp58->tag != 0)goto _LL4B;else{_tmp59=(void*)_tmp58->f1;}}_LL4A: {
struct Cyc_Absyn_Global_b_struct _tmp282;struct Cyc_Absyn_Global_b_struct*_tmp281;
struct Cyc_Absyn_Global_b_struct*_tmp5E=(_tmp281=_cycalloc(sizeof(*_tmp281)),((
_tmp281[0]=((_tmp282.tag=1,((_tmp282.f1=vd,_tmp282)))),_tmp281)));void*_tmp5F=
Cyc_Tcdecl_merge_binding(_tmp59,(void*)_tmp5E,loc,Cyc_Tc_tc_msg);if(_tmp5F == (
void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}if(_tmp5F == _tmp59
 && (*_tmp56).f2){_npop_handler(0);return;}if(exports == 0  || Cyc_Tc_export_member(
vd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp288;struct Cyc_Tcenv_VarRes_struct
_tmp287;struct _tuple12*_tmp286;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp286=_cycalloc(sizeof(*_tmp286)),((_tmp286->f1=(void*)((
_tmp288=_cycalloc(sizeof(*_tmp288)),((_tmp288[0]=((_tmp287.tag=0,((_tmp287.f1=(
void*)_tmp5F,_tmp287)))),_tmp288)))),((_tmp286->f2=1,_tmp286)))))));}
_npop_handler(0);return;}_LL4B: {struct Cyc_Tcenv_AggrRes_struct*_tmp5A=(struct
Cyc_Tcenv_AggrRes_struct*)_tmp57;if(_tmp5A->tag != 1)goto _LL4D;}_LL4C:{const char*
_tmp28B;void*_tmp28A;(_tmp28A=0,Cyc_Tcutil_warn(loc,((_tmp28B="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp28B,sizeof(char),57))),_tag_dyneither(_tmp28A,sizeof(void*),0)));}
goto _LL48;_LL4D: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp5B=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp57;if(_tmp5B->tag != 2)goto _LL4F;}_LL4E:{const char*_tmp28E;void*_tmp28D;(
_tmp28D=0,Cyc_Tcutil_warn(loc,((_tmp28E="variable declaration shadows previous datatype constructor",
_tag_dyneither(_tmp28E,sizeof(char),59))),_tag_dyneither(_tmp28D,sizeof(void*),0)));}
goto _LL48;_LL4F: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp5C=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp57;if(_tmp5C->tag != 4)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Tcenv_EnumRes_struct*
_tmp5D=(struct Cyc_Tcenv_EnumRes_struct*)_tmp57;if(_tmp5D->tag != 3)goto _LL48;}
_LL52:{const char*_tmp291;void*_tmp290;(_tmp290=0,Cyc_Tcutil_warn(loc,((_tmp291="variable declaration shadows previous enum tag",
_tag_dyneither(_tmp291,sizeof(char),47))),_tag_dyneither(_tmp290,sizeof(void*),0)));}
goto _LL48;_LL48:;};_pop_handler();}else{void*_tmp54=(void*)_exn_thrown;void*
_tmp6C=_tmp54;_LL54: {struct Cyc_Dict_Absent_struct*_tmp6D=(struct Cyc_Dict_Absent_struct*)
_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL56;}_LL55: goto _LL53;_LL56:;_LL57:(
void)_throw(_tmp6C);_LL53:;}};}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2A0;struct Cyc_Absyn_Global_b_struct*
_tmp29F;struct Cyc_Absyn_Global_b_struct _tmp29E;struct Cyc_Tcenv_VarRes_struct
_tmp29D;struct _tuple12*_tmp29C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp29C=_cycalloc(sizeof(*_tmp29C)),((_tmp29C->f1=(void*)((
_tmp2A0=_cycalloc(sizeof(*_tmp2A0)),((_tmp2A0[0]=((_tmp29D.tag=0,((_tmp29D.f1=(
void*)((void*)((_tmp29F=_cycalloc(sizeof(*_tmp29F)),((_tmp29F[0]=((_tmp29E.tag=1,((
_tmp29E.f1=vd,_tmp29E)))),_tmp29F))))),_tmp29D)))),_tmp2A0)))),((_tmp29C->f2=0,
_tmp29C)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n);static int Cyc_Tc_is_main(
struct _tuple0*n){struct _tuple0 _tmp74;union Cyc_Absyn_Nmspace _tmp75;struct
_dyneither_ptr*_tmp76;struct _tuple0*_tmp73=n;_tmp74=*_tmp73;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;{union Cyc_Absyn_Nmspace _tmp77=_tmp75;struct Cyc_List_List*_tmp78;
_LL59: if((_tmp77.Abs_n).tag != 2)goto _LL5B;_tmp78=(struct Cyc_List_List*)(_tmp77.Abs_n).val;
if(_tmp78 != 0)goto _LL5B;_LL5A: {const char*_tmp2A1;return Cyc_strcmp((struct
_dyneither_ptr)*_tmp76,((_tmp2A1="main",_tag_dyneither(_tmp2A1,sizeof(char),5))))
== 0;}_LL5B:;_LL5C: return 0;_LL58:;};}struct _tuple13{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*
fd,int ignore_body,struct Cyc_List_List**exports);static void Cyc_Tc_tcFndecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Fndecl*fd,int ignore_body,struct Cyc_List_List**exports){struct
_dyneither_ptr*v=(*fd->name).f2;if(fd->sc == Cyc_Absyn_ExternC  && !ignore_body){
const char*_tmp2A4;void*_tmp2A3;(_tmp2A3=0,Cyc_Tcutil_terr(loc,((_tmp2A4="extern \"C\" functions cannot be implemented in Cyclone",
_tag_dyneither(_tmp2A4,sizeof(char),54))),_tag_dyneither(_tmp2A3,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace _tmp7C=(*fd->name).f1;struct Cyc_List_List*_tmp7D;struct Cyc_List_List*
_tmp7E;_LL5E: if((_tmp7C.Rel_n).tag != 1)goto _LL60;_tmp7D=(struct Cyc_List_List*)(
_tmp7C.Rel_n).val;if(_tmp7D != 0)goto _LL60;_LL5F: goto _LL5D;_LL60: if((_tmp7C.Abs_n).tag
!= 2)goto _LL62;_tmp7E=(struct Cyc_List_List*)(_tmp7C.Abs_n).val;_LL61: {const char*
_tmp2A7;void*_tmp2A6;(_tmp2A6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2A7="tc: Abs_n in tcFndecl",
_tag_dyneither(_tmp2A7,sizeof(char),22))),_tag_dyneither(_tmp2A6,sizeof(void*),0)));}
_LL62:;_LL63:{const char*_tmp2AB;void*_tmp2AA[1];struct Cyc_String_pa_struct
_tmp2A9;(_tmp2A9.tag=0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(fd->name)),((_tmp2AA[0]=& _tmp2A9,Cyc_Tcutil_terr(loc,((
_tmp2AB="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp2AB,sizeof(char),57))),_tag_dyneither(_tmp2AA,sizeof(void*),1)))))));}
return;_LL5D:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp84=(void*)atts->hd;_LL65: {struct Cyc_Absyn_Packed_att_struct*
_tmp85=(struct Cyc_Absyn_Packed_att_struct*)_tmp84;if(_tmp85->tag != 7)goto _LL67;}
_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Aligned_att_struct*_tmp86=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp84;if(_tmp86->tag != 6)goto _LL69;}_LL68:{const char*_tmp2AF;void*_tmp2AE[1];
struct Cyc_String_pa_struct _tmp2AD;(_tmp2AD.tag=0,((_tmp2AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((
_tmp2AE[0]=& _tmp2AD,Cyc_Tcutil_terr(loc,((_tmp2AF="bad attribute %s for function",
_tag_dyneither(_tmp2AF,sizeof(char),30))),_tag_dyneither(_tmp2AE,sizeof(void*),1)))))));}
goto _LL64;_LL69:;_LL6A: goto _LL64;_LL64:;}}{struct _handler_cons _tmp8A;
_push_handler(& _tmp8A);{int _tmp8C=0;if(setjmp(_tmp8A.handler))_tmp8C=1;if(!
_tmp8C){{struct _tuple12*_tmp8D=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp8E=(*_tmp8D).f1;
void*_tmp90;_LL6C: {struct Cyc_Tcenv_VarRes_struct*_tmp8F=(struct Cyc_Tcenv_VarRes_struct*)
_tmp8E;if(_tmp8F->tag != 0)goto _LL6E;else{_tmp90=(void*)_tmp8F->f1;}}_LL6D: {
struct Cyc_Absyn_Funname_b_struct _tmp2B2;struct Cyc_Absyn_Funname_b_struct*_tmp2B1;
struct Cyc_Absyn_Funname_b_struct*_tmp95=(_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((
_tmp2B1[0]=((_tmp2B2.tag=2,((_tmp2B2.f1=fd,_tmp2B2)))),_tmp2B1)));void*_tmp96=
Cyc_Tcdecl_merge_binding(_tmp90,(void*)_tmp95,loc,Cyc_Tc_tc_msg);if(_tmp96 == (
void*)& Cyc_Absyn_Unresolved_b_val)goto _LL6B;if(_tmp96 == _tmp90  && (*_tmp8D).f2)
goto _LL6B;if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*
_tmp2B8;struct Cyc_Tcenv_VarRes_struct _tmp2B7;struct _tuple12*_tmp2B6;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple12*)((_tmp2B6=
_cycalloc(sizeof(*_tmp2B6)),((_tmp2B6->f1=(void*)((_tmp2B8=_cycalloc(sizeof(*
_tmp2B8)),((_tmp2B8[0]=((_tmp2B7.tag=0,((_tmp2B7.f1=(void*)_tmp96,_tmp2B7)))),
_tmp2B8)))),((_tmp2B6->f2=1,_tmp2B6)))))));}goto _LL6B;}_LL6E: {struct Cyc_Tcenv_AggrRes_struct*
_tmp91=(struct Cyc_Tcenv_AggrRes_struct*)_tmp8E;if(_tmp91->tag != 1)goto _LL70;}
_LL6F:{const char*_tmp2BB;void*_tmp2BA;(_tmp2BA=0,Cyc_Tcutil_warn(loc,((_tmp2BB="function declaration shadows previous type declaration",
_tag_dyneither(_tmp2BB,sizeof(char),55))),_tag_dyneither(_tmp2BA,sizeof(void*),0)));}
goto _LL6B;_LL70: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp92=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp8E;if(_tmp92->tag != 2)goto _LL72;}_LL71:{const char*_tmp2BE;void*_tmp2BD;(
_tmp2BD=0,Cyc_Tcutil_warn(loc,((_tmp2BE="function declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2BE,sizeof(char),59))),_tag_dyneither(_tmp2BD,sizeof(void*),0)));}
goto _LL6B;_LL72: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp93=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp8E;if(_tmp93->tag != 4)goto _LL74;}_LL73: goto _LL75;_LL74: {struct Cyc_Tcenv_EnumRes_struct*
_tmp94=(struct Cyc_Tcenv_EnumRes_struct*)_tmp8E;if(_tmp94->tag != 3)goto _LL6B;}
_LL75:{const char*_tmp2C1;void*_tmp2C0;(_tmp2C0=0,Cyc_Tcutil_warn(loc,((_tmp2C1="function declaration shadows previous enum tag",
_tag_dyneither(_tmp2C1,sizeof(char),47))),_tag_dyneither(_tmp2C0,sizeof(void*),0)));}
goto _LL6B;_LL6B:;};_pop_handler();}else{void*_tmp8B=(void*)_exn_thrown;void*
_tmpA3=_tmp8B;_LL77: {struct Cyc_Dict_Absent_struct*_tmpA4=(struct Cyc_Dict_Absent_struct*)
_tmpA3;if(_tmpA4->tag != Cyc_Dict_Absent)goto _LL79;}_LL78: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2D0;struct Cyc_Absyn_Funname_b_struct*
_tmp2CF;struct Cyc_Absyn_Funname_b_struct _tmp2CE;struct Cyc_Tcenv_VarRes_struct
_tmp2CD;struct _tuple12*_tmp2CC;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple12*)((_tmp2CC=_cycalloc(sizeof(*_tmp2CC)),((_tmp2CC->f1=(void*)((
_tmp2D0=_cycalloc(sizeof(*_tmp2D0)),((_tmp2D0[0]=((_tmp2CD.tag=0,((_tmp2CD.f1=(
void*)((void*)((_tmp2CF=_cycalloc(sizeof(*_tmp2CF)),((_tmp2CF[0]=((_tmp2CE.tag=2,((
_tmp2CE.f1=fd,_tmp2CE)))),_tmp2CF))))),_tmp2CD)))),_tmp2D0)))),((_tmp2CC->f2=0,
_tmp2CC)))))));}goto _LL76;_LL79:;_LL7A:(void)_throw(_tmpA3);_LL76:;}};}if(
ignore_body)return;{struct _RegionHandle _tmpAA=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmpAA;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmpAB=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp2D1;struct Cyc_Tcenv_Tenv*_tmpAC=(_tmp2D1=
_region_malloc(fnrgn,sizeof(*_tmp2D1)),((_tmp2D1->ns=te->ns,((_tmp2D1->ae=te->ae,((
_tmp2D1->le=(struct Cyc_Tcenv_Fenv*)_tmpAB,((_tmp2D1->allow_valueof=0,_tmp2D1)))))))));
Cyc_Tcstmt_tcStmt(_tmpAC,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmpAC);Cyc_Tcenv_check_delayed_constraints(
_tmpAC);if(!Cyc_Tcenv_all_labels_resolved(_tmpAC)){const char*_tmp2D4;void*
_tmp2D3;(_tmp2D3=0,Cyc_Tcutil_terr(loc,((_tmp2D4="function has goto statements to undefined labels",
_tag_dyneither(_tmp2D4,sizeof(char),49))),_tag_dyneither(_tmp2D3,sizeof(void*),0)));}}
if(Cyc_Tc_is_main(fd->name)){{void*_tmpB0=Cyc_Tcutil_compress(fd->ret_type);
enum Cyc_Absyn_Size_of _tmpB3;_LL7C: {struct Cyc_Absyn_VoidType_struct*_tmpB1=(
struct Cyc_Absyn_VoidType_struct*)_tmpB0;if(_tmpB1->tag != 0)goto _LL7E;}_LL7D:{
const char*_tmp2D7;void*_tmp2D6;(_tmp2D6=0,Cyc_Tcutil_warn(loc,((_tmp2D7="main declared with return type void",
_tag_dyneither(_tmp2D7,sizeof(char),36))),_tag_dyneither(_tmp2D6,sizeof(void*),0)));}
goto _LL7B;_LL7E: {struct Cyc_Absyn_IntType_struct*_tmpB2=(struct Cyc_Absyn_IntType_struct*)
_tmpB0;if(_tmpB2->tag != 6)goto _LL80;else{_tmpB3=_tmpB2->f2;}}_LL7F: goto _LL7B;
_LL80:;_LL81:{const char*_tmp2DB;void*_tmp2DA[1];struct Cyc_String_pa_struct
_tmp2D9;(_tmp2D9.tag=0,((_tmp2D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp2DA[0]=& _tmp2D9,Cyc_Tcutil_terr(loc,((
_tmp2DB="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp2DB,sizeof(char),57))),_tag_dyneither(_tmp2DA,sizeof(void*),1)))))));}
goto _LL7B;_LL7B:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp2DE;
void*_tmp2DD;(_tmp2DD=0,Cyc_Tcutil_terr(loc,((_tmp2DE="main declared with varargs",
_tag_dyneither(_tmp2DE,sizeof(char),27))),_tag_dyneither(_tmp2DD,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpBB=fd->args;if(_tmpBB != 0){struct _tuple13 _tmpBD;void*
_tmpBE;struct _tuple13*_tmpBC=(struct _tuple13*)_tmpBB->hd;_tmpBD=*_tmpBC;_tmpBE=
_tmpBD.f3;{void*_tmpBF=Cyc_Tcutil_compress(_tmpBE);enum Cyc_Absyn_Size_of _tmpC1;
_LL83: {struct Cyc_Absyn_IntType_struct*_tmpC0=(struct Cyc_Absyn_IntType_struct*)
_tmpBF;if(_tmpC0->tag != 6)goto _LL85;else{_tmpC1=_tmpC0->f2;}}_LL84: goto _LL82;
_LL85:;_LL86:{const char*_tmp2E2;void*_tmp2E1[1];struct Cyc_String_pa_struct
_tmp2E0;(_tmp2E0.tag=0,((_tmp2E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpBE)),((_tmp2E1[0]=& _tmp2E0,Cyc_Tcutil_terr(loc,((
_tmp2E2="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp2E2,sizeof(char),60))),_tag_dyneither(_tmp2E1,sizeof(void*),1)))))));}
goto _LL82;_LL82:;}_tmpBB=_tmpBB->tl;if(_tmpBB != 0){struct _tuple13 _tmpC6;void*
_tmpC7;struct _tuple13*_tmpC5=(struct _tuple13*)_tmpBB->hd;_tmpC6=*_tmpC5;_tmpC7=
_tmpC6.f3;_tmpBB=_tmpBB->tl;if(_tmpBB != 0){const char*_tmp2E5;void*_tmp2E4;(
_tmp2E4=0,Cyc_Tcutil_terr(loc,((_tmp2E5="main declared with too many arguments",
_tag_dyneither(_tmp2E5,sizeof(char),38))),_tag_dyneither(_tmp2E4,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp2E6;struct Cyc_Core_Opt*tvs=(_tmp2E6=_cycalloc(sizeof(*
_tmp2E6)),((_tmp2E6->v=fd->tvs,_tmp2E6)));struct Cyc_Core_Opt*_tmp2EE;struct Cyc_Core_Opt*
_tmp2ED;struct Cyc_Core_Opt*_tmp2EC;struct Cyc_Core_Opt*_tmp2EB;struct Cyc_Core_Opt*
_tmp2EA;struct Cyc_Core_Opt*_tmp2E9;struct Cyc_Core_Opt*_tmp2E8;struct Cyc_Core_Opt*
_tmp2E7;if(((!Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp2ED=_cycalloc_atomic(sizeof(*_tmp2ED)),((_tmp2ED->v=(
void*)Cyc_Absyn_RgnKind,_tmp2ED)))),tvs)),Cyc_Absyn_new_evar(((_tmp2EE=
_cycalloc_atomic(sizeof(*_tmp2EE)),((_tmp2EE->v=(void*)Cyc_Absyn_RgnKind,_tmp2EE)))),
tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))
 && !Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(((_tmp2EB=_cycalloc_atomic(sizeof(*_tmp2EB)),((_tmp2EB->v=(
void*)Cyc_Absyn_RgnKind,_tmp2EB)))),tvs)),Cyc_Absyn_new_evar(((_tmp2EC=
_cycalloc_atomic(sizeof(*_tmp2EC)),((_tmp2EC->v=(void*)Cyc_Absyn_RgnKind,_tmp2EC)))),
tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((
_tmp2E9=_cycalloc_atomic(sizeof(*_tmp2E9)),((_tmp2E9->v=(void*)Cyc_Absyn_RgnKind,
_tmp2E9)))),tvs)),Cyc_Absyn_new_evar(((_tmp2EA=_cycalloc_atomic(sizeof(*_tmp2EA)),((
_tmp2EA->v=(void*)Cyc_Absyn_RgnKind,_tmp2EA)))),tvs),Cyc_Absyn_const_tqual(0),((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(
_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((
_tmp2E7=_cycalloc_atomic(sizeof(*_tmp2E7)),((_tmp2E7->v=(void*)Cyc_Absyn_RgnKind,
_tmp2E7)))),tvs)),Cyc_Absyn_new_evar(((_tmp2E8=_cycalloc_atomic(sizeof(*_tmp2E8)),((
_tmp2E8->v=(void*)Cyc_Absyn_RgnKind,_tmp2E8)))),tvs),Cyc_Absyn_const_tqual(0),((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){const char*_tmp2F2;
void*_tmp2F1[1];struct Cyc_String_pa_struct _tmp2F0;(_tmp2F0.tag=0,((_tmp2F0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpC7)),((
_tmp2F1[0]=& _tmp2F0,Cyc_Tcutil_terr(loc,((_tmp2F2="second argument of main has type %s instead of char??",
_tag_dyneither(_tmp2F2,sizeof(char),54))),_tag_dyneither(_tmp2F1,sizeof(void*),1)))))));}};}}};};
_pop_region(fnrgn);};};}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td);static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td){struct
_dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpD6=(*td->name).f1;
struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;_LL88: if((_tmpD6.Rel_n).tag
!= 1)goto _LL8A;_tmpD7=(struct Cyc_List_List*)(_tmpD6.Rel_n).val;if(_tmpD7 != 0)
goto _LL8A;_LL89: goto _LL8B;_LL8A: if((_tmpD6.Abs_n).tag != 2)goto _LL8C;_tmpD8=(
struct Cyc_List_List*)(_tmpD6.Abs_n).val;if(_tmpD8 != 0)goto _LL8C;_LL8B: goto _LL87;
_LL8C:;_LL8D:{const char*_tmp2F6;void*_tmp2F5[1];struct Cyc_String_pa_struct
_tmp2F4;(_tmp2F4.tag=0,((_tmp2F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(td->name)),((_tmp2F5[0]=& _tmp2F4,Cyc_Tcutil_terr(loc,((
_tmp2F6="qualified typedef declarations are not implemented (%s)",_tag_dyneither(
_tmp2F6,sizeof(char),56))),_tag_dyneither(_tmp2F5,sizeof(void*),1)))))));}
return;_LL87:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp2FA;void*_tmp2F9[1];struct Cyc_String_pa_struct
_tmp2F8;(_tmp2F8.tag=0,((_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp2F9[0]=& _tmp2F8,Cyc_Tcutil_terr(loc,((_tmp2FA="redeclaration of typedef %s",
_tag_dyneither(_tmp2FA,sizeof(char),28))),_tag_dyneither(_tmp2F9,sizeof(void*),1)))))));}
return;}(*td->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(loc,
td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,Cyc_Absyn_AnyKind,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{struct Cyc_List_List*tvs=
td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpDF=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt**_tmpE2;struct Cyc_Core_Opt*
_tmpE4;struct Cyc_Core_Opt**_tmpE5;enum Cyc_Absyn_Kind _tmpE6;_LL8F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpE0=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDF;if(_tmpE0->tag != 1)goto _LL91;
else{_tmpE1=_tmpE0->f1;_tmpE2=(struct Cyc_Core_Opt**)& _tmpE0->f1;}}_LL90: if(td->defn
!= 0){const char*_tmp2FE;void*_tmp2FD[1];struct Cyc_String_pa_struct _tmp2FC;(
_tmp2FC.tag=0,((_tmp2FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp2FD[0]=& _tmp2FC,Cyc_Tcutil_warn(loc,((
_tmp2FE="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp2FE,sizeof(char),51))),_tag_dyneither(_tmp2FD,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp2FF;*_tmpE2=((_tmp2FF=_cycalloc(sizeof(*_tmp2FF)),((
_tmp2FF->v=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_BoxKind),_tmp2FF))));}goto _LL8E;
_LL91: {struct Cyc_Absyn_Less_kb_struct*_tmpE3=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDF;if(_tmpE3->tag != 2)goto _LL93;else{_tmpE4=_tmpE3->f1;_tmpE5=(struct Cyc_Core_Opt**)&
_tmpE3->f1;_tmpE6=_tmpE3->f2;}}_LL92:{struct Cyc_Core_Opt*_tmp300;*_tmpE5=((
_tmp300=_cycalloc(sizeof(*_tmp300)),((_tmp300->v=Cyc_Tcutil_kind_to_bound(_tmpE6),
_tmp300))));}goto _LL8E;_LL93:;_LL94: continue;_LL8E:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct
Cyc_List_List*rpo,struct Cyc_List_List*fields);static void Cyc_Tc_tcAggrImpl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct
_RegionHandle _tmpEC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmpEC;_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmpEE;
void*_tmpEF;void*_tmpF0;struct _tuple9*_tmpED=(struct _tuple9*)rpo->hd;_tmpEE=*
_tmpED;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;Cyc_Tcutil_check_type(loc,te,tvs,Cyc_Absyn_EffKind,
0,_tmpEF);Cyc_Tcutil_check_type(loc,te,tvs,Cyc_Absyn_TopRgnKind,0,_tmpF0);}{
struct Cyc_List_List*prev_fields=0;struct Cyc_List_List*_tmpF1=fields;for(0;_tmpF1
!= 0;_tmpF1=_tmpF1->tl){struct Cyc_Absyn_Aggrfield _tmpF3;struct _dyneither_ptr*
_tmpF4;struct Cyc_Absyn_Tqual _tmpF5;void*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct
Cyc_List_List*_tmpF8;struct Cyc_Absyn_Aggrfield*_tmpF2=(struct Cyc_Absyn_Aggrfield*)
_tmpF1->hd;_tmpF3=*_tmpF2;_tmpF4=_tmpF3.name;_tmpF5=_tmpF3.tq;_tmpF6=_tmpF3.type;
_tmpF7=_tmpF3.width;_tmpF8=_tmpF3.attributes;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpF4)){const char*
_tmp304;void*_tmp303[1];struct Cyc_String_pa_struct _tmp302;(_tmp302.tag=0,((
_tmp302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF4),((_tmp303[0]=&
_tmp302,Cyc_Tcutil_terr(loc,((_tmp304="duplicate field %s",_tag_dyneither(
_tmp304,sizeof(char),19))),_tag_dyneither(_tmp303,sizeof(void*),1)))))));}{const
char*_tmp305;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpF4,((_tmp305="",
_tag_dyneither(_tmp305,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp306;
prev_fields=((_tmp306=_region_malloc(uprev_rgn,sizeof(*_tmp306)),((_tmp306->hd=
_tmpF4,((_tmp306->tl=prev_fields,_tmp306))))));}}Cyc_Tcutil_check_type(loc,te,
tvs,Cyc_Absyn_MemKind,0,_tmpF6);(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).print_const,
_tmpF6);Cyc_Tcutil_check_bitfield(loc,te,_tmpF6,_tmpF7,_tmpF4);}};;_pop_region(
uprev_rgn);}static void Cyc_Tc_rule_out_memkind(struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*n,struct Cyc_List_List*tvs);static void Cyc_Tc_rule_out_memkind(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpFE=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp100;struct Cyc_Core_Opt**
_tmp101;struct Cyc_Core_Opt*_tmp103;struct Cyc_Core_Opt**_tmp104;enum Cyc_Absyn_Kind
_tmp105;struct Cyc_Core_Opt*_tmp107;struct Cyc_Core_Opt**_tmp108;enum Cyc_Absyn_Kind
_tmp109;enum Cyc_Absyn_Kind _tmp10B;_LL96: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpFF=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpFE;if(_tmpFF->tag != 1)goto _LL98;
else{_tmp100=_tmpFF->f1;_tmp101=(struct Cyc_Core_Opt**)& _tmpFF->f1;}}_LL97:
_tmp104=_tmp101;goto _LL99;_LL98: {struct Cyc_Absyn_Less_kb_struct*_tmp102=(struct
Cyc_Absyn_Less_kb_struct*)_tmpFE;if(_tmp102->tag != 2)goto _LL9A;else{_tmp103=
_tmp102->f1;_tmp104=(struct Cyc_Core_Opt**)& _tmp102->f1;_tmp105=_tmp102->f2;if(
_tmp105 != Cyc_Absyn_MemKind)goto _LL9A;}}_LL99:{struct Cyc_Core_Opt*_tmp307;*
_tmp104=((_tmp307=_cycalloc(sizeof(*_tmp307)),((_tmp307->v=Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_BoxKind),_tmp307))));}continue;_LL9A: {struct Cyc_Absyn_Less_kb_struct*
_tmp106=(struct Cyc_Absyn_Less_kb_struct*)_tmpFE;if(_tmp106->tag != 2)goto _LL9C;
else{_tmp107=_tmp106->f1;_tmp108=(struct Cyc_Core_Opt**)& _tmp106->f1;_tmp109=
_tmp106->f2;}}_LL9B:{struct Cyc_Core_Opt*_tmp308;*_tmp108=((_tmp308=_cycalloc(
sizeof(*_tmp308)),((_tmp308->v=Cyc_Tcutil_kind_to_bound(_tmp109),_tmp308))));}
continue;_LL9C: {struct Cyc_Absyn_Eq_kb_struct*_tmp10A=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpFE;if(_tmp10A->tag != 0)goto _LL9E;else{_tmp10B=_tmp10A->f1;if(_tmp10B != Cyc_Absyn_MemKind)
goto _LL9E;}}_LL9D:{const char*_tmp30D;void*_tmp30C[2];struct Cyc_String_pa_struct
_tmp30B;struct Cyc_String_pa_struct _tmp30A;(_tmp30A.tag=0,((_tmp30A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp30B.tag=0,((_tmp30B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((
_tmp30C[0]=& _tmp30B,((_tmp30C[1]=& _tmp30A,Cyc_Tcutil_terr(loc,((_tmp30D="type %s attempts to abstract type variable %s of kind M",
_tag_dyneither(_tmp30D,sizeof(char),56))),_tag_dyneither(_tmp30C,sizeof(void*),2)))))))))))));}
continue;_LL9E:;_LL9F: continue;_LL95:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*
ad);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct
Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){struct _dyneither_ptr*
_tmp112=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;
atts=atts->tl){void*_tmp113=(void*)atts->hd;_LLA1: {struct Cyc_Absyn_Packed_att_struct*
_tmp114=(struct Cyc_Absyn_Packed_att_struct*)_tmp113;if(_tmp114->tag != 7)goto
_LLA3;}_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_Aligned_att_struct*_tmp115=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp113;if(_tmp115->tag != 6)goto _LLA5;}_LLA4:
continue;_LLA5:;_LLA6:{const char*_tmp312;void*_tmp311[2];struct Cyc_String_pa_struct
_tmp310;struct Cyc_String_pa_struct _tmp30F;(_tmp30F.tag=0,((_tmp30F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp112),((_tmp310.tag=0,((_tmp310.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp311[0]=& _tmp310,((_tmp311[1]=& _tmp30F,Cyc_Tcutil_terr(loc,((
_tmp312="bad attribute %s in  %s definition",_tag_dyneither(_tmp312,sizeof(char),
35))),_tag_dyneither(_tmp311,sizeof(void*),2)))))))))))));}goto _LLA0;_LLA0:;}}{
struct Cyc_List_List*_tmp11A=ad->tvs;{union Cyc_Absyn_Nmspace _tmp11B=(*ad->name).f1;
struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11D;_LLA8: if((_tmp11B.Rel_n).tag
!= 1)goto _LLAA;_tmp11C=(struct Cyc_List_List*)(_tmp11B.Rel_n).val;if(_tmp11C != 0)
goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((_tmp11B.Abs_n).tag != 2)goto _LLAC;_tmp11D=(
struct Cyc_List_List*)(_tmp11B.Abs_n).val;if(_tmp11D != 0)goto _LLAC;_LLAB: goto
_LLA7;_LLAC:;_LLAD:{const char*_tmp316;void*_tmp315[1];struct Cyc_String_pa_struct
_tmp314;(_tmp314.tag=0,((_tmp314.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ad->name)),((_tmp315[0]=& _tmp314,Cyc_Tcutil_terr(loc,((
_tmp316="qualified struct declarations are not implemented (%s)",_tag_dyneither(
_tmp316,sizeof(char),55))),_tag_dyneither(_tmp315,sizeof(void*),1)))))));}
return;_LLA7:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple14 _tmp317;
struct _tuple14 _tmp122=(_tmp317.f1=ad->impl,((_tmp317.f2=((struct Cyc_Absyn_Aggrdecl***(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,
_tmp112),_tmp317)));struct Cyc_Absyn_AggrdeclImpl*_tmp123;struct Cyc_Absyn_Aggrdecl***
_tmp124;struct Cyc_Absyn_AggrdeclImpl*_tmp125;struct Cyc_Absyn_AggrdeclImpl _tmp126;
struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_List_List*
_tmp129;int _tmp12A;struct Cyc_Absyn_Aggrdecl***_tmp12B;struct Cyc_Absyn_AggrdeclImpl*
_tmp12C;struct Cyc_Absyn_AggrdeclImpl _tmp12D;struct Cyc_List_List*_tmp12E;struct
Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp130;int _tmp131;struct Cyc_Absyn_Aggrdecl***
_tmp132;struct Cyc_Absyn_Aggrdecl**_tmp133;struct Cyc_Absyn_AggrdeclImpl*_tmp134;
struct Cyc_Absyn_Aggrdecl***_tmp135;struct Cyc_Absyn_Aggrdecl**_tmp136;_LLAF:
_tmp123=_tmp122.f1;if(_tmp123 != 0)goto _LLB1;_tmp124=_tmp122.f2;if(_tmp124 != 0)
goto _LLB1;_LLB0: Cyc_Tc_rule_out_memkind(loc,_tmp112,_tmp11A);{struct Cyc_Absyn_Aggrdecl**
_tmp318;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp112,((_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318[0]=ad,_tmp318)))));}goto
_LLAE;_LLB1: _tmp125=_tmp122.f1;if(_tmp125 == 0)goto _LLB3;_tmp126=*_tmp125;_tmp127=
_tmp126.exist_vars;_tmp128=_tmp126.rgn_po;_tmp129=_tmp126.fields;_tmp12A=_tmp126.tagged;
_tmp12B=_tmp122.f2;if(_tmp12B != 0)goto _LLB3;_LLB2: {struct Cyc_Absyn_Aggrdecl*
_tmp31B;struct Cyc_Absyn_Aggrdecl**_tmp31A;struct Cyc_Absyn_Aggrdecl**_tmp138=(
_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=((_tmp31B=_cycalloc(sizeof(*
_tmp31B)),((_tmp31B->kind=ad->kind,((_tmp31B->sc=Cyc_Absyn_Extern,((_tmp31B->name=
ad->name,((_tmp31B->tvs=_tmp11A,((_tmp31B->impl=0,((_tmp31B->attributes=ad->attributes,
_tmp31B)))))))))))))),_tmp31A)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmp112,_tmp138);Cyc_Tcutil_check_unique_tvars(loc,_tmp127);Cyc_Tcutil_add_tvar_identities(
_tmp127);if(_tmp12A  && ad->kind == Cyc_Absyn_StructA){const char*_tmp31E;void*
_tmp31D;(_tmp31D=0,Cyc_Tcutil_terr(loc,((_tmp31E="@tagged qualifier is only allowed on union declarations",
_tag_dyneither(_tmp31E,sizeof(char),56))),_tag_dyneither(_tmp31D,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp11A,_tmp127),_tmp128,_tmp129);Cyc_Tc_rule_out_memkind(
loc,_tmp112,_tmp11A);Cyc_Tc_rule_out_memkind(loc,_tmp112,_tmp127);if(ad->kind == 
Cyc_Absyn_UnionA  && !_tmp12A){struct Cyc_List_List*f=_tmp129;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){const char*
_tmp324;void*_tmp323[3];struct Cyc_String_pa_struct _tmp322;struct Cyc_String_pa_struct
_tmp321;struct Cyc_String_pa_struct _tmp320;(_tmp320.tag=0,((_tmp320.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp321.tag=0,((_tmp321.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp112),((_tmp322.tag=0,((_tmp322.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp323[0]=&
_tmp322,((_tmp323[1]=& _tmp321,((_tmp323[2]=& _tmp320,Cyc_Tcutil_warn(loc,((
_tmp324="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp324,sizeof(char),98))),_tag_dyneither(_tmp323,sizeof(void*),3)))))))))))))))))));}}}*
_tmp138=ad;goto _LLAE;}_LLB3: _tmp12C=_tmp122.f1;if(_tmp12C == 0)goto _LLB5;_tmp12D=*
_tmp12C;_tmp12E=_tmp12D.exist_vars;_tmp12F=_tmp12D.rgn_po;_tmp130=_tmp12D.fields;
_tmp131=_tmp12D.tagged;_tmp132=_tmp122.f2;if(_tmp132 == 0)goto _LLB5;_tmp133=*
_tmp132;_LLB4: if(ad->kind != (*_tmp133)->kind){const char*_tmp327;void*_tmp326;(
_tmp326=0,Cyc_Tcutil_terr(loc,((_tmp327="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp327,sizeof(char),52))),_tag_dyneither(_tmp326,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp144=*_tmp133;{struct Cyc_Absyn_Aggrdecl*_tmp328;*
_tmp133=((_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328->kind=ad->kind,((_tmp328->sc=
Cyc_Absyn_Extern,((_tmp328->name=ad->name,((_tmp328->tvs=_tmp11A,((_tmp328->impl=
0,((_tmp328->attributes=ad->attributes,_tmp328))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmp12E);Cyc_Tcutil_add_tvar_identities(_tmp12E);if(_tmp131  && ad->kind == 
Cyc_Absyn_StructA){const char*_tmp32B;void*_tmp32A;(_tmp32A=0,Cyc_Tcutil_terr(loc,((
_tmp32B="@tagged qualifier is only allowed on union declarations",_tag_dyneither(
_tmp32B,sizeof(char),56))),_tag_dyneither(_tmp32A,sizeof(void*),0)));}Cyc_Tc_tcAggrImpl(
te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp11A,_tmp12E),_tmp12F,_tmp130);*_tmp133=_tmp144;_tmp136=
_tmp133;goto _LLB6;};_LLB5: _tmp134=_tmp122.f1;if(_tmp134 != 0)goto _LLAE;_tmp135=
_tmp122.f2;if(_tmp135 == 0)goto _LLAE;_tmp136=*_tmp135;_LLB6: {struct Cyc_Absyn_Aggrdecl*
_tmp148=Cyc_Tcdecl_merge_aggrdecl(*_tmp136,ad,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_memkind(
loc,_tmp112,_tmp11A);if(ad->impl != 0)Cyc_Tc_rule_out_memkind(loc,_tmp112,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);if(_tmp148 == 0)
return;else{*_tmp136=(struct Cyc_Absyn_Aggrdecl*)_tmp148;ad=(struct Cyc_Absyn_Aggrdecl*)
_tmp148;goto _LLAE;}}_LLAE:;}{struct Cyc_Tcenv_AggrRes_struct*_tmp331;struct Cyc_Tcenv_AggrRes_struct
_tmp330;struct _tuple12*_tmp32F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp112,(struct _tuple12*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F->f1=(
void*)((_tmp331=_cycalloc(sizeof(*_tmp331)),((_tmp331[0]=((_tmp330.tag=1,((
_tmp330.f1=ad,_tmp330)))),_tmp331)))),((_tmp32F->f2=1,_tmp32F)))))));};};}static
void Cyc_Tc_rule_out_mem_and_unique(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*v,struct Cyc_List_List*tvs);static void Cyc_Tc_rule_out_mem_and_unique(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp14C=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp14E;struct Cyc_Core_Opt**
_tmp14F;struct Cyc_Core_Opt*_tmp151;struct Cyc_Core_Opt**_tmp152;enum Cyc_Absyn_Kind
_tmp153;struct Cyc_Core_Opt*_tmp155;struct Cyc_Core_Opt**_tmp156;enum Cyc_Absyn_Kind
_tmp157;struct Cyc_Core_Opt*_tmp159;struct Cyc_Core_Opt**_tmp15A;enum Cyc_Absyn_Kind
_tmp15B;enum Cyc_Absyn_Kind _tmp15D;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Core_Opt**
_tmp160;enum Cyc_Absyn_Kind _tmp161;enum Cyc_Absyn_Kind _tmp163;enum Cyc_Absyn_Kind
_tmp165;_LLB8: {struct Cyc_Absyn_Unknown_kb_struct*_tmp14D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp14C;if(_tmp14D->tag != 1)goto _LLBA;else{_tmp14E=_tmp14D->f1;_tmp14F=(struct
Cyc_Core_Opt**)& _tmp14D->f1;}}_LLB9: _tmp152=_tmp14F;goto _LLBB;_LLBA: {struct Cyc_Absyn_Less_kb_struct*
_tmp150=(struct Cyc_Absyn_Less_kb_struct*)_tmp14C;if(_tmp150->tag != 2)goto _LLBC;
else{_tmp151=_tmp150->f1;_tmp152=(struct Cyc_Core_Opt**)& _tmp150->f1;_tmp153=
_tmp150->f2;if(_tmp153 != Cyc_Absyn_MemKind)goto _LLBC;}}_LLBB:{struct Cyc_Core_Opt*
_tmp332;*_tmp152=((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332->v=Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_BoxKind),_tmp332))));}goto _LLB7;_LLBC: {struct Cyc_Absyn_Less_kb_struct*
_tmp154=(struct Cyc_Absyn_Less_kb_struct*)_tmp14C;if(_tmp154->tag != 2)goto _LLBE;
else{_tmp155=_tmp154->f1;_tmp156=(struct Cyc_Core_Opt**)& _tmp154->f1;_tmp157=
_tmp154->f2;if(_tmp157 != Cyc_Absyn_AnyKind)goto _LLBE;}}_LLBD:{struct Cyc_Core_Opt*
_tmp333;*_tmp156=((_tmp333=_cycalloc(sizeof(*_tmp333)),((_tmp333->v=Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_AnyKind),_tmp333))));}goto _LLB7;_LLBE: {struct Cyc_Absyn_Less_kb_struct*
_tmp158=(struct Cyc_Absyn_Less_kb_struct*)_tmp14C;if(_tmp158->tag != 2)goto _LLC0;
else{_tmp159=_tmp158->f1;_tmp15A=(struct Cyc_Core_Opt**)& _tmp158->f1;_tmp15B=
_tmp158->f2;if(_tmp15B != Cyc_Absyn_TopRgnKind)goto _LLC0;}}_LLBF:{struct Cyc_Core_Opt*
_tmp334;*_tmp15A=((_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334->v=Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_RgnKind),_tmp334))));}goto _LLB7;_LLC0: {struct Cyc_Absyn_Eq_kb_struct*
_tmp15C=(struct Cyc_Absyn_Eq_kb_struct*)_tmp14C;if(_tmp15C->tag != 0)goto _LLC2;
else{_tmp15D=_tmp15C->f1;if(_tmp15D != Cyc_Absyn_TopRgnKind)goto _LLC2;}}_LLC1:{
const char*_tmp339;void*_tmp338[2];struct Cyc_String_pa_struct _tmp337;struct Cyc_String_pa_struct
_tmp336;(_tmp336.tag=0,((_tmp336.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp337.tag=0,((_tmp337.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp338[0]=& _tmp337,((_tmp338[1]=&
_tmp336,Cyc_Tcutil_terr(loc,((_tmp339="type %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp339,sizeof(char),57))),_tag_dyneither(_tmp338,sizeof(void*),2)))))))))))));}
goto _LLB7;_LLC2: {struct Cyc_Absyn_Less_kb_struct*_tmp15E=(struct Cyc_Absyn_Less_kb_struct*)
_tmp14C;if(_tmp15E->tag != 2)goto _LLC4;else{_tmp15F=_tmp15E->f1;_tmp160=(struct
Cyc_Core_Opt**)& _tmp15E->f1;_tmp161=_tmp15E->f2;if(_tmp161 != Cyc_Absyn_UniqueRgnKind)
goto _LLC4;}}_LLC3: goto _LLC5;_LLC4: {struct Cyc_Absyn_Eq_kb_struct*_tmp162=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp14C;if(_tmp162->tag != 0)goto _LLC6;else{_tmp163=
_tmp162->f1;if(_tmp163 != Cyc_Absyn_UniqueRgnKind)goto _LLC6;}}_LLC5:{const char*
_tmp33E;void*_tmp33D[2];struct Cyc_String_pa_struct _tmp33C;struct Cyc_String_pa_struct
_tmp33B;(_tmp33B.tag=0,((_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp33C.tag=0,((_tmp33C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp33D[0]=& _tmp33C,((_tmp33D[1]=&
_tmp33B,Cyc_Tcutil_terr(loc,((_tmp33E="type %s attempts to abstract type variable %s of kind UR",
_tag_dyneither(_tmp33E,sizeof(char),57))),_tag_dyneither(_tmp33D,sizeof(void*),2)))))))))))));}
goto _LLB7;_LLC6: {struct Cyc_Absyn_Eq_kb_struct*_tmp164=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp14C;if(_tmp164->tag != 0)goto _LLC8;else{_tmp165=_tmp164->f1;if(_tmp165 != Cyc_Absyn_MemKind)
goto _LLC8;}}_LLC7:{const char*_tmp343;void*_tmp342[2];struct Cyc_String_pa_struct
_tmp341;struct Cyc_String_pa_struct _tmp340;(_tmp340.tag=0,((_tmp340.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp341.tag=0,((_tmp341.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp342[0]=& _tmp341,((_tmp342[1]=& _tmp340,Cyc_Tcutil_terr(loc,((_tmp343="type %s attempts to abstract type variable %s of kind M",
_tag_dyneither(_tmp343,sizeof(char),56))),_tag_dyneither(_tmp342,sizeof(void*),2)))))))))))));}
goto _LLB7;_LLC8:;_LLC9: goto _LLB7;_LLB7:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;
void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr
obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Datatypedecl*tudres);static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*
_tmp175=fields;for(0;_tmp175 != 0;_tmp175=_tmp175->tl){struct Cyc_Absyn_Datatypefield*
_tmp176=(struct Cyc_Absyn_Datatypefield*)_tmp175->hd;{struct Cyc_List_List*typs=
_tmp176->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp176->loc,
te,tvs,Cyc_Absyn_MemKind,0,(*((struct _tuple15*)typs->hd)).f2);{struct
_RegionHandle*_tmp177=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp177,(*((struct _tuple15*)typs->hd)).f2)){const char*_tmp347;void*_tmp346[1];
struct Cyc_String_pa_struct _tmp345;(_tmp345.tag=0,((_tmp345.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp176->name)),((
_tmp346[0]=& _tmp345,Cyc_Tcutil_terr(_tmp176->loc,((_tmp347="noalias pointers in datatypes are not allowed (%s)",
_tag_dyneither(_tmp347,sizeof(char),51))),_tag_dyneither(_tmp346,sizeof(void*),1)))))));}((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp176->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);};}}{union Cyc_Absyn_Nmspace _tmp17B=(*_tmp176->name).f1;struct Cyc_List_List*
_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17E;int _tmp17F;_LLCB:
if((_tmp17B.Rel_n).tag != 1)goto _LLCD;_tmp17C=(struct Cyc_List_List*)(_tmp17B.Rel_n).val;
if(_tmp17C != 0)goto _LLCD;_LLCC: if(is_extensible)(*_tmp176->name).f1=Cyc_Absyn_Abs_n(
te->ns);else{(*_tmp176->name).f1=(*name).f1;}goto _LLCA;_LLCD: if((_tmp17B.Rel_n).tag
!= 1)goto _LLCF;_tmp17D=(struct Cyc_List_List*)(_tmp17B.Rel_n).val;_LLCE:{const
char*_tmp34B;void*_tmp34A[1];struct Cyc_String_pa_struct _tmp349;(_tmp349.tag=0,((
_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp176->name)),((_tmp34A[0]=& _tmp349,Cyc_Tcutil_terr(_tmp176->loc,((_tmp34B="qualified datatypefield declarations are not allowed (%s)",
_tag_dyneither(_tmp34B,sizeof(char),58))),_tag_dyneither(_tmp34A,sizeof(void*),1)))))));}
goto _LLCA;_LLCF: if((_tmp17B.Abs_n).tag != 2)goto _LLD1;_tmp17E=(struct Cyc_List_List*)(
_tmp17B.Abs_n).val;_LLD0: goto _LLCA;_LLD1: if((_tmp17B.Loc_n).tag != 3)goto _LLCA;
_tmp17F=(int)(_tmp17B.Loc_n).val;_LLD2: {const char*_tmp34E;void*_tmp34D;(_tmp34D=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp34E="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp34E,sizeof(char),24))),
_tag_dyneither(_tmp34D,sizeof(void*),0)));}_LLCA:;};}}{struct Cyc_List_List*
fields2;if(is_extensible){int _tmp185=1;struct Cyc_List_List*_tmp186=Cyc_Tcdecl_sort_xdatatype_fields(
fields,& _tmp185,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmp185)fields2=_tmp186;else{
fields2=0;}}else{struct _RegionHandle _tmp187=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp187;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*
_tmp188=(struct Cyc_Absyn_Datatypefield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp188->name).f2)){
const char*_tmp353;void*_tmp352[2];struct Cyc_String_pa_struct _tmp351;struct Cyc_String_pa_struct
_tmp350;(_tmp350.tag=0,((_tmp350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
obj),((_tmp351.tag=0,((_tmp351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp188->name).f2),((_tmp352[0]=& _tmp351,((_tmp352[1]=& _tmp350,Cyc_Tcutil_terr(
_tmp188->loc,((_tmp353="duplicate field name %s in %s",_tag_dyneither(_tmp353,
sizeof(char),30))),_tag_dyneither(_tmp352,sizeof(void*),2)))))))))))));}else{
struct Cyc_List_List*_tmp354;prev_fields=((_tmp354=_region_malloc(uprev_rgn,
sizeof(*_tmp354)),((_tmp354->hd=(*_tmp188->name).f2,((_tmp354->tl=prev_fields,
_tmp354))))));}if(_tmp188->sc != Cyc_Absyn_Public){{const char*_tmp358;void*
_tmp357[1];struct Cyc_String_pa_struct _tmp356;(_tmp356.tag=0,((_tmp356.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp188->name).f2),((_tmp357[0]=&
_tmp356,Cyc_Tcutil_warn(loc,((_tmp358="ignoring scope of field %s",
_tag_dyneither(_tmp358,sizeof(char),27))),_tag_dyneither(_tmp357,sizeof(void*),1)))))));}
_tmp188->sc=Cyc_Absyn_Public;}}}fields2=fields;};_pop_region(uprev_rgn);}{struct
Cyc_List_List*_tmp191=fields;for(0;_tmp191 != 0;_tmp191=_tmp191->tl){struct Cyc_Absyn_Datatypefield*
_tmp192=(struct Cyc_Absyn_Datatypefield*)_tmp191->hd;struct Cyc_Tcenv_DatatypeRes_struct*
_tmp35E;struct Cyc_Tcenv_DatatypeRes_struct _tmp35D;struct _tuple12*_tmp35C;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp192->name).f2,(struct _tuple12*)((
_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C->f1=(void*)((_tmp35E=_cycalloc(
sizeof(*_tmp35E)),((_tmp35E[0]=((_tmp35D.tag=2,((_tmp35D.f1=tudres,((_tmp35D.f2=
_tmp192,_tmp35D)))))),_tmp35E)))),((_tmp35C->f2=1,_tmp35C)))))));}}return fields2;};}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Datatypedecl*tud);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl*
tud){struct _dyneither_ptr*v=(*tud->name).f2;const char*_tmp360;const char*_tmp35F;
struct _dyneither_ptr obj=tud->is_extensible?(_tmp360="@extensible datatype",
_tag_dyneither(_tmp360,sizeof(char),21)):((_tmp35F="datatype",_tag_dyneither(
_tmp35F,sizeof(char),9)));struct Cyc_List_List*tvs=tud->tvs;Cyc_Tcutil_check_unique_tvars(
loc,tvs);Cyc_Tcutil_add_tvar_identities(tvs);{struct _RegionHandle _tmp196=
_new_region("temp");struct _RegionHandle*temp=& _tmp196;_push_region(temp);{struct
Cyc_Absyn_Datatypedecl***tud_opt;{struct _handler_cons _tmp197;_push_handler(&
_tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){tud_opt=
Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);if(tud_opt == 0  && !tud->is_extensible){
struct Cyc_Dict_Absent_struct _tmp363;struct Cyc_Dict_Absent_struct*_tmp362;(int)
_throw((void*)((_tmp362=_cycalloc_atomic(sizeof(*_tmp362)),((_tmp362[0]=((
_tmp363.tag=Cyc_Dict_Absent,_tmp363)),_tmp362)))));}if(tud_opt != 0)tud->name=(*(*
tud_opt))->name;else{(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns);};_pop_handler();}
else{void*_tmp198=(void*)_exn_thrown;void*_tmp19D=_tmp198;_LLD4: {struct Cyc_Dict_Absent_struct*
_tmp19E=(struct Cyc_Dict_Absent_struct*)_tmp19D;if(_tmp19E->tag != Cyc_Dict_Absent)
goto _LLD6;}_LLD5:{union Cyc_Absyn_Nmspace _tmp19F=(*tud->name).f1;struct Cyc_List_List*
_tmp1A0;struct Cyc_List_List*_tmp1A1;_LLD9: if((_tmp19F.Rel_n).tag != 1)goto _LLDB;
_tmp1A0=(struct Cyc_List_List*)(_tmp19F.Rel_n).val;if(_tmp1A0 != 0)goto _LLDB;_LLDA:(*
tud->name).f1=Cyc_Absyn_Abs_n(te->ns);goto _LLD8;_LLDB: if((_tmp19F.Abs_n).tag != 2)
goto _LLDD;_tmp1A1=(struct Cyc_List_List*)(_tmp19F.Abs_n).val;_LLDC: goto _LLDE;
_LLDD:;_LLDE:{const char*_tmp367;void*_tmp366[1];struct Cyc_String_pa_struct
_tmp365;(_tmp365.tag=0,((_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name)),((_tmp366[0]=& _tmp365,Cyc_Tcutil_terr(loc,((
_tmp367="qualified (non-extensible) datatype declarations are not implemented (%s)",
_tag_dyneither(_tmp367,sizeof(char),74))),_tag_dyneither(_tmp366,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLD8:;}{struct Cyc_Absyn_Datatypedecl***_tmp1A5=((struct
Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->datatypedecls,v);{struct Cyc_Absyn_Datatypedecl***_tmp368;tud_opt=(
unsigned int)_tmp1A5?(_tmp368=_region_malloc(temp,sizeof(*_tmp368)),((_tmp368[0]=*
_tmp1A5,_tmp368))): 0;}goto _LLD3;};_LLD6:;_LLD7:(void)_throw(_tmp19D);_LLD3:;}};}{
struct _tuple16 _tmp369;struct _tuple16 _tmp1A8=(_tmp369.f1=tud->fields,((_tmp369.f2=
tud_opt,_tmp369)));struct Cyc_Core_Opt*_tmp1A9;struct Cyc_Absyn_Datatypedecl***
_tmp1AA;struct Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt _tmp1AC;struct Cyc_List_List*
_tmp1AD;struct Cyc_List_List**_tmp1AE;struct Cyc_Absyn_Datatypedecl***_tmp1AF;
struct Cyc_Core_Opt*_tmp1B0;struct Cyc_Core_Opt _tmp1B1;struct Cyc_List_List*_tmp1B2;
struct Cyc_List_List**_tmp1B3;struct Cyc_Absyn_Datatypedecl***_tmp1B4;struct Cyc_Absyn_Datatypedecl**
_tmp1B5;struct Cyc_Core_Opt*_tmp1B6;struct Cyc_Absyn_Datatypedecl***_tmp1B7;struct
Cyc_Absyn_Datatypedecl**_tmp1B8;_LLE0: _tmp1A9=_tmp1A8.f1;if(_tmp1A9 != 0)goto
_LLE2;_tmp1AA=_tmp1A8.f2;if(_tmp1AA != 0)goto _LLE2;_LLE1: Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);{struct Cyc_Absyn_Datatypedecl**_tmp36A;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=tud,
_tmp36A)))));}goto _LLDF;_LLE2: _tmp1AB=_tmp1A8.f1;if(_tmp1AB == 0)goto _LLE4;
_tmp1AC=*_tmp1AB;_tmp1AD=(struct Cyc_List_List*)_tmp1AC.v;_tmp1AE=(struct Cyc_List_List**)&(*
_tmp1A8.f1).v;_tmp1AF=_tmp1A8.f2;if(_tmp1AF != 0)goto _LLE4;_LLE3: {struct Cyc_Absyn_Datatypedecl*
_tmp36D;struct Cyc_Absyn_Datatypedecl**_tmp36C;struct Cyc_Absyn_Datatypedecl**
_tmp1BA=(_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=((_tmp36D=_cycalloc(
sizeof(*_tmp36D)),((_tmp36D->sc=Cyc_Absyn_Extern,((_tmp36D->name=tud->name,((
_tmp36D->tvs=tvs,((_tmp36D->fields=0,((_tmp36D->is_extensible=tud->is_extensible,
_tmp36D)))))))))))),_tmp36C)));ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,_tmp1BA);*_tmp1AE=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,
tud->name,*_tmp1AE,tvs,tud);Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);*_tmp1BA=
tud;goto _LLDF;}_LLE4: _tmp1B0=_tmp1A8.f1;if(_tmp1B0 == 0)goto _LLE6;_tmp1B1=*
_tmp1B0;_tmp1B2=(struct Cyc_List_List*)_tmp1B1.v;_tmp1B3=(struct Cyc_List_List**)&(*
_tmp1A8.f1).v;_tmp1B4=_tmp1A8.f2;if(_tmp1B4 == 0)goto _LLE6;_tmp1B5=*_tmp1B4;_LLE5: {
struct Cyc_Absyn_Datatypedecl*_tmp1BD=*_tmp1B5;if((!tud->is_extensible  && (
unsigned int)_tmp1BD) && _tmp1BD->is_extensible)tud->is_extensible=1;{struct Cyc_Absyn_Datatypedecl*
_tmp36E;*_tmp1B5=((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E->sc=Cyc_Absyn_Extern,((
_tmp36E->name=tud->name,((_tmp36E->tvs=tvs,((_tmp36E->fields=0,((_tmp36E->is_extensible=
tud->is_extensible,_tmp36E))))))))))));}*_tmp1B3=Cyc_Tc_tcDatatypeFields(te,ge,
loc,obj,tud->is_extensible,tud->name,*_tmp1B3,tvs,tud);*_tmp1B5=_tmp1BD;_tmp1B8=
_tmp1B5;goto _LLE7;}_LLE6: _tmp1B6=_tmp1A8.f1;if(_tmp1B6 != 0)goto _LLDF;_tmp1B7=
_tmp1A8.f2;if(_tmp1B7 == 0)goto _LLDF;_tmp1B8=*_tmp1B7;_LLE7: {struct Cyc_Absyn_Datatypedecl*
_tmp1BF=Cyc_Tcdecl_merge_datatypedecl(*_tmp1B8,tud,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);if(_tmp1BF == 0){_npop_handler(0);return;}else{*_tmp1B8=(struct Cyc_Absyn_Datatypedecl*)
_tmp1BF;goto _LLDF;}}_LLDF:;};};_pop_region(temp);};}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Enumdecl*ed);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed){struct
_dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace _tmp1C2=(*ed->name).f1;
struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C4;_LLE9: if((_tmp1C2.Rel_n).tag
!= 1)goto _LLEB;_tmp1C3=(struct Cyc_List_List*)(_tmp1C2.Rel_n).val;if(_tmp1C3 != 0)
goto _LLEB;_LLEA: goto _LLEC;_LLEB: if((_tmp1C2.Abs_n).tag != 2)goto _LLED;_tmp1C4=(
struct Cyc_List_List*)(_tmp1C2.Abs_n).val;if(_tmp1C4 != 0)goto _LLED;_LLEC: goto
_LLE8;_LLED:;_LLEE:{const char*_tmp372;void*_tmp371[1];struct Cyc_String_pa_struct
_tmp370;(_tmp370.tag=0,((_tmp370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ed->name)),((_tmp371[0]=& _tmp370,Cyc_Tcutil_terr(loc,((
_tmp372="qualified enum declarations are not implemented (%s)",_tag_dyneither(
_tmp372,sizeof(char),53))),_tag_dyneither(_tmp371,sizeof(void*),1)))))));}
return;_LLE8:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns);if(ed->fields != 0){struct
_RegionHandle _tmp1C8=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp1C8;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1C9=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1C9->name).f2)){
const char*_tmp376;void*_tmp375[1];struct Cyc_String_pa_struct _tmp374;(_tmp374.tag=
0,((_tmp374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C9->name).f2),((
_tmp375[0]=& _tmp374,Cyc_Tcutil_terr(_tmp1C9->loc,((_tmp376="duplicate field name %s",
_tag_dyneither(_tmp376,sizeof(char),24))),_tag_dyneither(_tmp375,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp377;prev_fields=((_tmp377=_region_malloc(uprev_rgn,
sizeof(*_tmp377)),((_tmp377->hd=(*_tmp1C9->name).f2,((_tmp377->tl=prev_fields,
_tmp377))))));}if(_tmp1C9->tag == 0)_tmp1C9->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp1C9->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp1C9->tag))){const char*_tmp37C;void*_tmp37B[2];struct Cyc_String_pa_struct
_tmp37A;struct Cyc_String_pa_struct _tmp379;(_tmp379.tag=0,((_tmp379.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C9->name).f2),((_tmp37A.tag=0,((
_tmp37A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp37B[0]=&
_tmp37A,((_tmp37B[1]=& _tmp379,Cyc_Tcutil_terr(loc,((_tmp37C="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp37C,sizeof(char),46))),_tag_dyneither(_tmp37B,sizeof(void*),2)))))))))))));}}{
unsigned int _tmp1D3;int _tmp1D4;struct _tuple10 _tmp1D2=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp1C9->tag));_tmp1D3=_tmp1D2.f1;_tmp1D4=
_tmp1D2.f2;if(!_tmp1D4){const char*_tmp37F;void*_tmp37E;(_tmp37E=0,Cyc_Tcutil_terr(
loc,((_tmp37F="Cyclone enum tags cannot use sizeof or offsetof",_tag_dyneither(
_tmp37F,sizeof(char),48))),_tag_dyneither(_tmp37E,sizeof(void*),0)));}tag_count=
_tmp1D3 + 1;(*_tmp1C9->name).f1=Cyc_Absyn_Abs_n(te->ns);};}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp1D7;_push_handler(& _tmp1D7);{int _tmp1D9=0;
if(setjmp(_tmp1D7.handler))_tmp1D9=1;if(!_tmp1D9){{struct Cyc_Absyn_Enumdecl**
_tmp1DA=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp1DB=Cyc_Tcdecl_merge_enumdecl(*_tmp1DA,ed,loc,Cyc_Tc_tc_msg);if(_tmp1DB == 0){
_npop_handler(0);return;}*_tmp1DA=(struct Cyc_Absyn_Enumdecl*)_tmp1DB;};
_pop_handler();}else{void*_tmp1D8=(void*)_exn_thrown;void*_tmp1DD=_tmp1D8;_LLF0: {
struct Cyc_Dict_Absent_struct*_tmp1DE=(struct Cyc_Dict_Absent_struct*)_tmp1DD;if(
_tmp1DE->tag != Cyc_Dict_Absent)goto _LLF2;}_LLF1: {struct Cyc_Absyn_Enumdecl**
_tmp380;struct Cyc_Absyn_Enumdecl**_tmp1DF=(_tmp380=_cycalloc(sizeof(*_tmp380)),((
_tmp380[0]=ed,_tmp380)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp1DF);goto _LLEF;}_LLF2:;_LLF3:(void)_throw(_tmp1DD);_LLEF:;}};}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1E1=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_struct*
_tmp386;struct Cyc_Tcenv_EnumRes_struct _tmp385;struct _tuple12*_tmp384;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1E1->name).f2,(struct _tuple12*)((
_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->f1=(void*)((_tmp386=_cycalloc(
sizeof(*_tmp386)),((_tmp386[0]=((_tmp385.tag=3,((_tmp385.f1=ed,((_tmp385.f2=
_tmp1E1,_tmp385)))))),_tmp386)))),((_tmp384->f2=1,_tmp384)))))));}}}static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc);static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc){switch(sc){case Cyc_Absyn_Static:
_LLF4:{const char*_tmp389;void*_tmp388;(_tmp388=0,Cyc_Tcutil_warn(loc,((_tmp389="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp389,sizeof(char),44))),_tag_dyneither(_tmp388,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LLF5:{const char*_tmp38C;void*_tmp38B;(_tmp38B=0,
Cyc_Tcutil_warn(loc,((_tmp38C="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp38C,sizeof(char),46))),_tag_dyneither(_tmp38B,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LLF6: goto _LLF7;case Cyc_Absyn_Register: _LLF7: goto
_LLF8;case Cyc_Absyn_Extern: _LLF8: return 1;case Cyc_Absyn_ExternC: _LLF9:{const char*
_tmp38F;void*_tmp38E;(_tmp38E=0,Cyc_Tcutil_warn(loc,((_tmp38F="nested extern \"C\" declaration",
_tag_dyneither(_tmp38F,sizeof(char),30))),_tag_dyneither(_tmp38E,sizeof(void*),0)));}
return 1;}}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,
struct _tuple11*exp);static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*
te,struct _tuple11*exp){struct Cyc_Position_Segment*_tmp1EC;struct _tuple0*_tmp1ED;
struct _tuple11 _tmp1EB=*exp;_tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;{struct _tuple0
_tmp1EF;union Cyc_Absyn_Nmspace _tmp1F0;struct _dyneither_ptr*_tmp1F1;struct _tuple0*
_tmp1EE=_tmp1ED;_tmp1EF=*_tmp1EE;_tmp1F0=_tmp1EF.f1;_tmp1F1=_tmp1EF.f2;{union Cyc_Absyn_Nmspace
_tmp1F2=_tmp1F0;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List*_tmp1F4;_LLFC:
if((_tmp1F2.Rel_n).tag != 1)goto _LLFE;_tmp1F3=(struct Cyc_List_List*)(_tmp1F2.Rel_n).val;
if(_tmp1F3 != 0)goto _LLFE;_LLFD: goto _LLFF;_LLFE: if((_tmp1F2.Abs_n).tag != 2)goto
_LL100;_tmp1F4=(struct Cyc_List_List*)(_tmp1F2.Abs_n).val;if(_tmp1F4 != 0)goto
_LL100;_LLFF: goto _LLFB;_LL100:;_LL101:{const char*_tmp393;void*_tmp392[1];struct
Cyc_String_pa_struct _tmp391;(_tmp391.tag=0,((_tmp391.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1ED)),((_tmp392[0]=& _tmp391,Cyc_Tcutil_terr(
_tmp1EC,((_tmp393="qualified export variables are not implemented (%s)",
_tag_dyneither(_tmp393,sizeof(char),52))),_tag_dyneither(_tmp392,sizeof(void*),1)))))));}
return;_LLFB:;}(*_tmp1ED).f1=Cyc_Absyn_Abs_n(te->ns);};}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports);static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*
grgn,struct Cyc_List_List**exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_List_List*_tmp1F8=ds0;for(0;_tmp1F8 != 0;(
last=_tmp1F8,_tmp1F8=_tmp1F8->tl)){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
_tmp1F8->hd;struct Cyc_Position_Segment*loc=d->loc;void*_tmp1F9=d->r;struct Cyc_Absyn_Vardecl*
_tmp1FF;struct Cyc_Absyn_Fndecl*_tmp201;struct Cyc_Absyn_Typedefdecl*_tmp203;
struct Cyc_Absyn_Aggrdecl*_tmp205;struct Cyc_Absyn_Datatypedecl*_tmp207;struct Cyc_Absyn_Enumdecl*
_tmp209;struct _dyneither_ptr*_tmp20D;struct Cyc_List_List*_tmp20E;struct _tuple0*
_tmp210;struct _tuple0 _tmp211;union Cyc_Absyn_Nmspace _tmp212;struct _dyneither_ptr*
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp216;struct Cyc_List_List*
_tmp218;struct Cyc_List_List*_tmp219;_LL103: {struct Cyc_Absyn_Let_d_struct*
_tmp1FA=(struct Cyc_Absyn_Let_d_struct*)_tmp1F9;if(_tmp1FA->tag != 2)goto _LL105;}
_LL104: goto _LL106;_LL105: {struct Cyc_Absyn_Letv_d_struct*_tmp1FB=(struct Cyc_Absyn_Letv_d_struct*)
_tmp1F9;if(_tmp1FB->tag != 3)goto _LL107;}_LL106:{const char*_tmp396;void*_tmp395;(
_tmp395=0,Cyc_Tcutil_terr(loc,((_tmp396="top level let-declarations are not implemented",
_tag_dyneither(_tmp396,sizeof(char),47))),_tag_dyneither(_tmp395,sizeof(void*),0)));}
goto _LL102;_LL107: {struct Cyc_Absyn_Region_d_struct*_tmp1FC=(struct Cyc_Absyn_Region_d_struct*)
_tmp1F9;if(_tmp1FC->tag != 4)goto _LL109;}_LL108:{const char*_tmp399;void*_tmp398;(
_tmp398=0,Cyc_Tcutil_terr(loc,((_tmp399="top level region declarations are not implemented",
_tag_dyneither(_tmp399,sizeof(char),50))),_tag_dyneither(_tmp398,sizeof(void*),0)));}
goto _LL102;_LL109: {struct Cyc_Absyn_Alias_d_struct*_tmp1FD=(struct Cyc_Absyn_Alias_d_struct*)
_tmp1F9;if(_tmp1FD->tag != 5)goto _LL10B;}_LL10A:{const char*_tmp39C;void*_tmp39B;(
_tmp39B=0,Cyc_Tcutil_terr(loc,((_tmp39C="top level alias declarations are not implemented",
_tag_dyneither(_tmp39C,sizeof(char),49))),_tag_dyneither(_tmp39B,sizeof(void*),0)));}
goto _LL102;_LL10B: {struct Cyc_Absyn_Var_d_struct*_tmp1FE=(struct Cyc_Absyn_Var_d_struct*)
_tmp1F9;if(_tmp1FE->tag != 0)goto _LL10D;else{_tmp1FF=_tmp1FE->f1;}}_LL10C: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1FF->sc))_tmp1FF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp1FF,check_var_init,in_externCinclude,exports);
goto _LL102;_LL10D: {struct Cyc_Absyn_Fn_d_struct*_tmp200=(struct Cyc_Absyn_Fn_d_struct*)
_tmp1F9;if(_tmp200->tag != 1)goto _LL10F;else{_tmp201=_tmp200->f1;}}_LL10E: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp201->sc))_tmp201->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp201,in_externCinclude,exports);goto _LL102;_LL10F: {
struct Cyc_Absyn_Typedef_d_struct*_tmp202=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp1F9;if(_tmp202->tag != 9)goto _LL111;else{_tmp203=_tmp202->f1;}}_LL110: Cyc_Tc_tcTypedefdecl(
te,ge,loc,_tmp203);goto _LL102;_LL111: {struct Cyc_Absyn_Aggr_d_struct*_tmp204=(
struct Cyc_Absyn_Aggr_d_struct*)_tmp1F9;if(_tmp204->tag != 6)goto _LL113;else{
_tmp205=_tmp204->f1;}}_LL112: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp205->sc))
_tmp205->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp205);goto _LL102;
_LL113: {struct Cyc_Absyn_Datatype_d_struct*_tmp206=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1F9;if(_tmp206->tag != 7)goto _LL115;else{_tmp207=_tmp206->f1;}}_LL114: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp207->sc))_tmp207->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp207);goto _LL102;_LL115: {struct Cyc_Absyn_Enum_d_struct*
_tmp208=(struct Cyc_Absyn_Enum_d_struct*)_tmp1F9;if(_tmp208->tag != 8)goto _LL117;
else{_tmp209=_tmp208->f1;}}_LL116: if(in_externC  && Cyc_Tc_okay_externC(d->loc,
_tmp209->sc))_tmp209->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp209);
goto _LL102;_LL117: {struct Cyc_Absyn_Porton_d_struct*_tmp20A=(struct Cyc_Absyn_Porton_d_struct*)
_tmp1F9;if(_tmp20A->tag != 14)goto _LL119;}_LL118:{const char*_tmp39F;void*_tmp39E;(
_tmp39E=0,Cyc_Tcutil_warn(d->loc,((_tmp39F="spurious __cyclone_port_on__",
_tag_dyneither(_tmp39F,sizeof(char),29))),_tag_dyneither(_tmp39E,sizeof(void*),0)));}
goto _LL102;_LL119: {struct Cyc_Absyn_Portoff_d_struct*_tmp20B=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp1F9;if(_tmp20B->tag != 15)goto _LL11B;}_LL11A: goto _LL102;_LL11B: {struct Cyc_Absyn_Namespace_d_struct*
_tmp20C=(struct Cyc_Absyn_Namespace_d_struct*)_tmp1F9;if(_tmp20C->tag != 10)goto
_LL11D;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;}}_LL11C: {struct Cyc_List_List*
_tmp222=te->ns;struct Cyc_List_List*_tmp3A0;struct Cyc_List_List*_tmp223=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp222,((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0->hd=_tmp20D,((_tmp3A0->tl=
0,_tmp3A0)))))));if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ge->namespaces,_tmp20D)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
grgn,ge->namespaces,_tmp20D);te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp223,
Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp223;Cyc_Tc_tc_decls(te,_tmp20E,
in_externC,in_externCinclude,check_var_init,grgn,exports);te->ns=_tmp222;goto
_LL102;}_LL11D: {struct Cyc_Absyn_Using_d_struct*_tmp20F=(struct Cyc_Absyn_Using_d_struct*)
_tmp1F9;if(_tmp20F->tag != 11)goto _LL11F;else{_tmp210=_tmp20F->f1;_tmp211=*
_tmp210;_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;_tmp214=_tmp20F->f2;}}_LL11E: {
struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union Cyc_Absyn_Nmspace
_tmp225=_tmp212;int _tmp226;struct Cyc_List_List*_tmp227;struct Cyc_List_List*
_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_List_List _tmp22A;struct
_dyneither_ptr*_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22D;
struct Cyc_List_List _tmp22E;struct _dyneither_ptr*_tmp22F;struct Cyc_List_List*
_tmp230;_LL124: if((_tmp225.Loc_n).tag != 3)goto _LL126;_tmp226=(int)(_tmp225.Loc_n).val;
_LL125: goto _LL127;_LL126: if((_tmp225.Rel_n).tag != 1)goto _LL128;_tmp227=(struct
Cyc_List_List*)(_tmp225.Rel_n).val;if(_tmp227 != 0)goto _LL128;_LL127: goto _LL129;
_LL128: if((_tmp225.Abs_n).tag != 2)goto _LL12A;_tmp228=(struct Cyc_List_List*)(
_tmp225.Abs_n).val;if(_tmp228 != 0)goto _LL12A;_LL129: first=_tmp213;rest=0;goto
_LL123;_LL12A: if((_tmp225.Rel_n).tag != 1)goto _LL12C;_tmp229=(struct Cyc_List_List*)(
_tmp225.Rel_n).val;if(_tmp229 == 0)goto _LL12C;_tmp22A=*_tmp229;_tmp22B=(struct
_dyneither_ptr*)_tmp22A.hd;_tmp22C=_tmp22A.tl;_LL12B: _tmp22F=_tmp22B;_tmp230=
_tmp22C;goto _LL12D;_LL12C: if((_tmp225.Abs_n).tag != 2)goto _LL123;_tmp22D=(struct
Cyc_List_List*)(_tmp225.Abs_n).val;if(_tmp22D == 0)goto _LL123;_tmp22E=*_tmp22D;
_tmp22F=(struct _dyneither_ptr*)_tmp22E.hd;_tmp230=_tmp22E.tl;_LL12D: first=
_tmp22F;{struct Cyc_List_List*_tmp3A1;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp230,((_tmp3A1=_cycalloc(sizeof(*
_tmp3A1)),((_tmp3A1->hd=_tmp213,((_tmp3A1->tl=0,_tmp3A1)))))));}goto _LL123;
_LL123:;}{struct Cyc_List_List*_tmp232=Cyc_Tcenv_resolve_namespace(te,loc,first,
rest);{struct Cyc_List_List*_tmp3A2;ge->availables=(struct Cyc_List_List*)((
_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2->hd=_tmp232,((_tmp3A2->tl=ge->availables,
_tmp3A2))))));}Cyc_Tc_tc_decls(te,_tmp214,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL102;};}_LL11F: {struct Cyc_Absyn_ExternC_d_struct*
_tmp215=(struct Cyc_Absyn_ExternC_d_struct*)_tmp1F9;if(_tmp215->tag != 12)goto
_LL121;else{_tmp216=_tmp215->f1;}}_LL120: Cyc_Tc_tc_decls(te,_tmp216,1,
in_externCinclude,check_var_init,grgn,exports);goto _LL102;_LL121: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp217=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1F9;if(_tmp217->tag != 13)
goto _LL102;else{_tmp218=_tmp217->f1;_tmp219=_tmp217->f2;}}_LL122:((void(*)(void(*
f)(struct Cyc_Tcenv_Tenv*,struct _tuple11*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp219);{struct Cyc_List_List*
newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp219,(unsigned int)exports?*exports: 0);Cyc_Tc_tc_decls(te,
_tmp218,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);for(0;_tmp219 != 
0;_tmp219=_tmp219->tl){struct _tuple11*_tmp234=(struct _tuple11*)_tmp219->hd;if(!(*
_tmp234).f3){const char*_tmp3A6;void*_tmp3A5[1];struct Cyc_String_pa_struct _tmp3A4;(
_tmp3A4.tag=0,((_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*
_tmp234).f2)),((_tmp3A5[0]=& _tmp3A4,Cyc_Tcutil_warn((*_tmp234).f1,((_tmp3A6="%s is exported but not defined",
_tag_dyneither(_tmp3A6,sizeof(char),31))),_tag_dyneither(_tmp3A5,sizeof(void*),1)))))));}}
goto _LL102;};_LL102:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds);void Cyc_Tc_tc(struct _RegionHandle*g,
struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(&
Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct
Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict
nsdict;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,
struct Cyc_List_List*ds);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,
struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(
Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*
f2;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d){void*_tmp238=d->r;struct Cyc_Absyn_Vardecl*_tmp23A;struct Cyc_List_List*_tmp23C;
struct Cyc_List_List**_tmp23D;struct Cyc_List_List*_tmp23F;struct Cyc_List_List**
_tmp240;struct Cyc_List_List*_tmp242;struct Cyc_List_List**_tmp243;struct Cyc_List_List*
_tmp245;struct Cyc_List_List**_tmp246;_LL12F: {struct Cyc_Absyn_Var_d_struct*
_tmp239=(struct Cyc_Absyn_Var_d_struct*)_tmp238;if(_tmp239->tag != 0)goto _LL131;
else{_tmp23A=_tmp239->f1;}}_LL130: if(env->in_cinclude  || _tmp23A->sc != Cyc_Absyn_Extern
 && _tmp23A->sc != Cyc_Absyn_ExternC)return 1;{struct _tuple0 _tmp248;union Cyc_Absyn_Nmspace
_tmp249;struct _dyneither_ptr*_tmp24A;struct _tuple0*_tmp247=_tmp23A->name;_tmp248=*
_tmp247;_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace
_tmp24B=_tmp249;int _tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*
_tmp24E;_LL13C: if((_tmp24B.Loc_n).tag != 3)goto _LL13E;_tmp24C=(int)(_tmp24B.Loc_n).val;
_LL13D: ns=0;goto _LL13B;_LL13E: if((_tmp24B.Rel_n).tag != 1)goto _LL140;_tmp24D=(
struct Cyc_List_List*)(_tmp24B.Rel_n).val;_LL13F: ns=_tmp24D;goto _LL13B;_LL140: if((
_tmp24B.Abs_n).tag != 2)goto _LL13B;_tmp24E=(struct Cyc_List_List*)(_tmp24B.Abs_n).val;
_LL141: ns=_tmp24E;goto _LL13B;_LL13B:;}{struct _tuple17*_tmp24F=((struct _tuple17*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp250=(*_tmp24F).f1;int _tmp251=(*((struct _tuple12*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp250->ordinaries,
_tmp24A)).f2;if(!_tmp251)(*_tmp24F).f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
env->rgn,(*_tmp24F).f2,_tmp24A);return _tmp251;};};};_LL131: {struct Cyc_Absyn_ExternC_d_struct*
_tmp23B=(struct Cyc_Absyn_ExternC_d_struct*)_tmp238;if(_tmp23B->tag != 12)goto
_LL133;else{_tmp23C=_tmp23B->f1;_tmp23D=(struct Cyc_List_List**)& _tmp23B->f1;}}
_LL132: _tmp240=_tmp23D;goto _LL134;_LL133: {struct Cyc_Absyn_Using_d_struct*
_tmp23E=(struct Cyc_Absyn_Using_d_struct*)_tmp238;if(_tmp23E->tag != 11)goto _LL135;
else{_tmp23F=_tmp23E->f2;_tmp240=(struct Cyc_List_List**)& _tmp23E->f2;}}_LL134:
_tmp243=_tmp240;goto _LL136;_LL135: {struct Cyc_Absyn_Namespace_d_struct*_tmp241=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp238;if(_tmp241->tag != 10)goto _LL137;
else{_tmp242=_tmp241->f2;_tmp243=(struct Cyc_List_List**)& _tmp241->f2;}}_LL136:*
_tmp243=Cyc_Tc_treeshake_f(env,*_tmp243);return 1;_LL137: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp244=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp238;if(_tmp244->tag != 13)
goto _LL139;else{_tmp245=_tmp244->f1;_tmp246=(struct Cyc_List_List**)& _tmp244->f1;}}
_LL138: {int in_cinclude=env->in_cinclude;env->in_cinclude=1;*_tmp246=Cyc_Tc_treeshake_f(
env,*_tmp246);env->in_cinclude=in_cinclude;return 1;}_LL139:;_LL13A: return 1;
_LL12E:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct
_dyneither_ptr*x,void*y);static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*
set,struct _dyneither_ptr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge);static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple17*_tmp3A7;return(
_tmp3A7=_region_malloc(rgn,sizeof(*_tmp3A7)),((_tmp3A7->f1=ge,((_tmp3A7->f2=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3A7)))));}struct
_tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct _RegionHandle _tmp253=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp253;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv
_tmp3A8;struct Cyc_Tc_TreeshakeEnv _tmp254=(_tmp3A8.rgn=rgn,((_tmp3A8.in_cinclude=
0,((_tmp3A8.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*
f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3A8)))));
struct Cyc_List_List*_tmp255=Cyc_Tc_treeshake_f(& _tmp254,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp254.nsdict)){struct Cyc_List_List*_tmp256=
_tmp255;_npop_handler(0);return _tmp256;}{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp254.nsdict);struct _tuple18 _tmp257=*((struct _tuple18*(*)(struct _RegionHandle*,
struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp254.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp257)){struct
_tuple17*_tmp259;struct _tuple17 _tmp25A;struct Cyc_Tcenv_Genv*_tmp25B;struct Cyc_Set_Set*
_tmp25C;struct _tuple18 _tmp258=_tmp257;_tmp259=_tmp258.f2;_tmp25A=*_tmp259;
_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;_tmp25B->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple12*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp25B->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct
_tuple12*y))Cyc_Tc_treeshake_remove_f,_tmp25C,_tmp25B->ordinaries);}{struct Cyc_List_List*
_tmp25D=_tmp255;_npop_handler(0);return _tmp25D;};};};_pop_region(rgn);}
