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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict
d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_struct{
char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_struct{
char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};
struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
int tag;struct Cyc_Absyn_Stmt*f1;};extern struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val;extern struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple0*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple9{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple9*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct
Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple10 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*
e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[
10]="Env_error";struct Cyc_Tcenv_Env_error_struct{char*tag;};struct Cyc_Tcenv_Env_error_struct
Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){{const char*
_tmp319;void*_tmp318[1];struct Cyc_String_pa_struct _tmp317;(_tmp317.tag=0,((
_tmp317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp318[0]=&
_tmp317,Cyc_fprintf(Cyc_stderr,((_tmp319="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp319,sizeof(char),29))),_tag_dyneither(_tmp318,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val={
0};struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val={1};struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val={2};struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;
void*continue_stmt;void*break_stmt;struct _tuple9*fallthru_clause;void*next_stmt;
int try_depth;};struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*
return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;
struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_struct{
char*tag;};struct Cyc_Tcenv_NoBinding_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v);
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}(
int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*r);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp31A;return(_tmp31A=_region_malloc(r,
sizeof(*_tmp31A)),((_tmp31A->grgn=r,((_tmp31A->namespaces=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp31A->aggrdecls=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp31A->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp31A->enumdecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp31A->typedefs=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp31A->ordinaries=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp31A->availables=0,_tmp31A)))))))))))))))));}struct
_tuple11{void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct
_RegionHandle*r);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){{
struct Cyc_Core_Opt*_tmp31B;Cyc_Tcutil_empty_var_set=((_tmp31B=_cycalloc(sizeof(*
_tmp31B)),((_tmp31B->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp31B))));}{struct Cyc_Tcenv_Genv*
_tmpA=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Datatypedecl**_tmp31C;_tmpA->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**
v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp31C=
_cycalloc(sizeof(*_tmp31C)),((_tmp31C[0]=Cyc_Absyn_exn_tud,_tmp31C)))));}{struct
Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmpC != 0;_tmpC=_tmpC->tl){struct Cyc_Tcenv_DatatypeRes_struct*_tmp322;
struct Cyc_Tcenv_DatatypeRes_struct _tmp321;struct _tuple11*_tmp320;_tmpA->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple11*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)
_tmpC->hd)->name).f2,((_tmp320=_region_malloc(r,sizeof(*_tmp320)),((_tmp320->f1=(
void*)((_tmp322=_cycalloc(sizeof(*_tmp322)),((_tmp322[0]=((_tmp321.tag=2,((
_tmp321.f1=Cyc_Absyn_exn_tud,((_tmp321.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,
_tmp321)))))),_tmp322)))),((_tmp320->f2=1,_tmp320)))))));}}{struct Cyc_Dict_Dict
ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmpA);struct Cyc_Tcenv_Tenv*_tmp323;return(_tmp323=_region_malloc(r,sizeof(*
_tmp323)),((_tmp323->ns=0,((_tmp323->ae=ae,((_tmp323->le=0,((_tmp323->allow_valueof=
0,_tmp323)))))))));};};}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns);static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns);static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){const char*_tmp324;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp324="outer_namespace",_tag_dyneither(
_tmp324,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2);static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss);static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*
nss){for(0;nss != 0;nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)
nss->hd)){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))
_tmp14=1;if(!_tmp14){lookup(ge2,v);{const char*_tmp328;void*_tmp327[1];struct Cyc_String_pa_struct
_tmp326;(_tmp326.tag=0,((_tmp326.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp327[0]=& _tmp326,Cyc_Tcutil_terr(loc,((_tmp328="%s is ambiguous",
_tag_dyneither(_tmp328,sizeof(char),16))),_tag_dyneither(_tmp327,sizeof(void*),1)))))));};;
_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;_LL1: {
struct Cyc_Dict_Absent_struct*_tmp1A=(struct Cyc_Dict_Absent_struct*)_tmp19;if(
_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(
_tmp19);_LL0:;}};}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v);static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp1B;
_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!
_tmp1D){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;};};_pop_handler();}else{void*
_tmp1C=(void*)_exn_thrown;void*_tmp20=_tmp1C;_LL6: {struct Cyc_Dict_Absent_struct*
_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)
goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp20);_LL5:;}};}{struct Cyc_List_List*
_tmp22=ge->availables;for(0;_tmp22 != 0;_tmp22=_tmp22->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp22->hd);struct _handler_cons _tmp23;_push_handler(&
_tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp22->hd,_tmp22->tl);{void*_tmp26=result;_npop_handler(0);return _tmp26;};};
_pop_handler();}else{void*_tmp24=(void*)_exn_thrown;void*_tmp28=_tmp24;_LLB: {
struct Cyc_Dict_Absent_struct*_tmp29=(struct Cyc_Dict_Absent_struct*)_tmp28;if(
_tmp29->tag != Cyc_Dict_Absent)goto _LLD;}_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(
_tmp28);_LLA:;}};}}if(cns == 0){struct Cyc_Dict_Absent_struct _tmp32B;struct Cyc_Dict_Absent_struct*
_tmp32A;(int)_throw((void*)((_tmp32A=_cycalloc_atomic(sizeof(*_tmp32A)),((
_tmp32A[0]=((_tmp32B.tag=Cyc_Dict_Absent,_tmp32B)),_tmp32A)))));}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v);static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;struct
_tuple11*ans=((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp2C,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);static
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp2E;union Cyc_Absyn_Nmspace _tmp2F;struct
_dyneither_ptr*_tmp30;struct _tuple0*_tmp2D=q;_tmp2E=*_tmp2D;_tmp2F=_tmp2E.f1;
_tmp30=_tmp2E.f2;{union Cyc_Absyn_Nmspace _tmp31=_tmp2F;int _tmp32;struct Cyc_List_List*
_tmp33;struct Cyc_List_List*_tmp34;struct Cyc_List_List _tmp35;struct _dyneither_ptr*
_tmp36;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp38;_LL10: if((_tmp31.Loc_n).tag
!= 3)goto _LL12;_tmp32=(int)(_tmp31.Loc_n).val;_LL11: goto _LL13;_LL12: if((_tmp31.Rel_n).tag
!= 1)goto _LL14;_tmp33=(struct Cyc_List_List*)(_tmp31.Rel_n).val;if(_tmp33 != 0)
goto _LL14;_LL13: return((void*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp30);_LL14: if((_tmp31.Rel_n).tag != 1)goto _LL16;_tmp34=(struct Cyc_List_List*)(
_tmp31.Rel_n).val;if(_tmp34 == 0)goto _LL16;_tmp35=*_tmp34;_tmp36=(struct
_dyneither_ptr*)_tmp35.hd;_tmp37=_tmp35.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp39=
Cyc_Tcenv_lookup_namespace(te,loc,_tmp36,_tmp37);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp39,_tmp30);}_LL16: if((_tmp31.Abs_n).tag != 2)goto _LLF;_tmp38=(struct Cyc_List_List*)(
_tmp31.Abs_n).val;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp38),
_tmp30);_LLF:;};}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){struct
Cyc_List_List*_tmp3A=te->ns;struct _RegionHandle*_tmp3B=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp3C=0;while(1){struct Cyc_Tcenv_Genv*_tmp3D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp3A);
struct Cyc_List_List*_tmp3E=_tmp3D->availables;struct Cyc_Set_Set*_tmp3F=_tmp3D->namespaces;{
struct Cyc_List_List*_tmp40=_tmp3E;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_Set_Set*
_tmp41=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp40->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp41,n)){struct
Cyc_List_List*_tmp32E;struct Cyc_List_List*_tmp32D;_tmp3C=((_tmp32D=
_region_malloc(_tmp3B,sizeof(*_tmp32D)),((_tmp32D->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp32E=
_cycalloc(sizeof(*_tmp32E)),((_tmp32E->hd=n,((_tmp32E->tl=ns,_tmp32E))))))),((
_tmp32D->tl=_tmp3C,_tmp32D))))));}}}if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp3F,n)){struct Cyc_List_List*_tmp331;
struct Cyc_List_List*_tmp330;_tmp3C=((_tmp330=_region_malloc(_tmp3B,sizeof(*
_tmp330)),((_tmp330->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp331=_cycalloc(sizeof(*_tmp331)),((
_tmp331->hd=n,((_tmp331->tl=ns,_tmp331))))))),((_tmp330->tl=_tmp3C,_tmp330))))));}
if(_tmp3C != 0){if(_tmp3C->tl != 0){const char*_tmp335;void*_tmp334[1];struct Cyc_String_pa_struct
_tmp333;(_tmp333.tag=0,((_tmp333.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
n),((_tmp334[0]=& _tmp333,Cyc_Tcutil_terr(loc,((_tmp335="%s is ambiguous",
_tag_dyneither(_tmp335,sizeof(char),16))),_tag_dyneither(_tmp334,sizeof(void*),1)))))));}
return(struct Cyc_List_List*)_tmp3C->hd;}if(_tmp3A == 0){struct Cyc_Dict_Absent_struct
_tmp338;struct Cyc_Dict_Absent_struct*_tmp337;(int)_throw((void*)((_tmp337=
_cycalloc_atomic(sizeof(*_tmp337)),((_tmp337[0]=((_tmp338.tag=Cyc_Dict_Absent,
_tmp338)),_tmp337)))));}_tmp3A=Cyc_Tcenv_outer_namespace(_tmp3A);}}static struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct
_dyneither_ptr*v);static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp4B,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp4D;union Cyc_Absyn_Nmspace _tmp4E;struct
_dyneither_ptr*_tmp4F;struct _tuple0*_tmp4C=q;_tmp4D=*_tmp4C;_tmp4E=_tmp4D.f1;
_tmp4F=_tmp4D.f2;{union Cyc_Absyn_Nmspace _tmp50=_tmp4E;int _tmp51;struct Cyc_List_List*
_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List
_tmp55;struct _dyneither_ptr*_tmp56;struct Cyc_List_List*_tmp57;_LL19: if((_tmp50.Loc_n).tag
!= 3)goto _LL1B;_tmp51=(int)(_tmp50.Loc_n).val;_LL1A: goto _LL1C;_LL1B: if((_tmp50.Rel_n).tag
!= 1)goto _LL1D;_tmp52=(struct Cyc_List_List*)(_tmp50.Rel_n).val;if(_tmp52 != 0)
goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp4F);_LL1D: if((_tmp50.Abs_n).tag != 2)goto _LL1F;
_tmp53=(struct Cyc_List_List*)(_tmp50.Abs_n).val;_LL1E: {struct Cyc_Dict_Dict
_tmp58=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp53))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp58,_tmp4F);}
_LL1F: if((_tmp50.Rel_n).tag != 1)goto _LL18;_tmp54=(struct Cyc_List_List*)(_tmp50.Rel_n).val;
if(_tmp54 == 0)goto _LL18;_tmp55=*_tmp54;_tmp56=(struct _dyneither_ptr*)_tmp55.hd;
_tmp57=_tmp55.tl;_LL20: {struct Cyc_Dict_Dict _tmp59=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp56,_tmp57))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp4F);}_LL18:;};}
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp5A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5A,v);}struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);struct
Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp5C;union Cyc_Absyn_Nmspace
_tmp5D;struct _dyneither_ptr*_tmp5E;struct _tuple0*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=
_tmp5C.f1;_tmp5E=_tmp5C.f2;{union Cyc_Absyn_Nmspace _tmp5F=_tmp5D;int _tmp60;struct
Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_List_List _tmp63;struct
_dyneither_ptr*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp66;
_LL22: if((_tmp5F.Loc_n).tag != 3)goto _LL24;_tmp60=(int)(_tmp5F.Loc_n).val;_LL23:
goto _LL25;_LL24: if((_tmp5F.Rel_n).tag != 1)goto _LL26;_tmp61=(struct Cyc_List_List*)(
_tmp5F.Rel_n).val;if(_tmp61 != 0)goto _LL26;_LL25: return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp5E);_LL26: if((_tmp5F.Rel_n).tag != 1)
goto _LL28;_tmp62=(struct Cyc_List_List*)(_tmp5F.Rel_n).val;if(_tmp62 == 0)goto
_LL28;_tmp63=*_tmp62;_tmp64=(struct _dyneither_ptr*)_tmp63.hd;_tmp65=_tmp63.tl;
_LL27: {struct Cyc_Dict_Dict _tmp67=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp64,
_tmp65))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp67,_tmp5E);}_LL28: if((_tmp5F.Abs_n).tag
!= 2)goto _LL21;_tmp66=(struct Cyc_List_List*)(_tmp5F.Abs_n).val;_LL29: {struct Cyc_Dict_Dict
_tmp68=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp66))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp68,_tmp5E);}
_LL21:;};}static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q);struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp6A;union Cyc_Absyn_Nmspace
_tmp6B;struct _dyneither_ptr*_tmp6C;struct _tuple0*_tmp69=q;_tmp6A=*_tmp69;_tmp6B=
_tmp6A.f1;_tmp6C=_tmp6A.f2;{union Cyc_Absyn_Nmspace _tmp6D=_tmp6B;struct Cyc_List_List*
_tmp6E;int _tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List _tmp71;struct
_dyneither_ptr*_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_List_List*_tmp74;
_LL2B: if((_tmp6D.Rel_n).tag != 1)goto _LL2D;_tmp6E=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;
if(_tmp6E != 0)goto _LL2D;_LL2C: {struct _handler_cons _tmp75;_push_handler(& _tmp75);{
int _tmp77=0;if(setjmp(_tmp75.handler))_tmp77=1;if(!_tmp77){{struct Cyc_Absyn_Datatypedecl***
_tmp339;struct Cyc_Absyn_Datatypedecl***_tmp79=(_tmp339=_region_malloc(r,sizeof(*
_tmp339)),((_tmp339[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp6C),_tmp339)));_npop_handler(0);
return _tmp79;};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;void*_tmp7B=
_tmp76;_LL34: {struct Cyc_Dict_Absent_struct*_tmp7C=(struct Cyc_Dict_Absent_struct*)
_tmp7B;if(_tmp7C->tag != Cyc_Dict_Absent)goto _LL36;}_LL35: return 0;_LL36:;_LL37:(
void)_throw(_tmp7B);_LL33:;}};}_LL2D: if((_tmp6D.Loc_n).tag != 3)goto _LL2F;_tmp6F=(
int)(_tmp6D.Loc_n).val;_LL2E:{const char*_tmp33C;void*_tmp33B;(_tmp33B=0,Cyc_Tcutil_terr(
loc,((_tmp33C="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp33C,sizeof(
char),33))),_tag_dyneither(_tmp33B,sizeof(void*),0)));}return 0;_LL2F: if((_tmp6D.Rel_n).tag
!= 1)goto _LL31;_tmp70=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;if(_tmp70 == 0)
goto _LL31;_tmp71=*_tmp70;_tmp72=(struct _dyneither_ptr*)_tmp71.hd;_tmp73=_tmp71.tl;
_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{
int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp72,_tmp73);;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*
_tmp83=_tmp80;_LL39: {struct Cyc_Dict_Absent_struct*_tmp84=(struct Cyc_Dict_Absent_struct*)
_tmp83;if(_tmp84->tag != Cyc_Dict_Absent)goto _LL3B;}_LL3A:{const char*_tmp33F;void*
_tmp33E;(_tmp33E=0,Cyc_Tcutil_terr(loc,((_tmp33F="bad qualified name for @extensible datatype",
_tag_dyneither(_tmp33F,sizeof(char),44))),_tag_dyneither(_tmp33E,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_struct _tmp342;struct Cyc_Dict_Absent_struct*_tmp341;(int)
_throw((void*)((_tmp341=_cycalloc_atomic(sizeof(*_tmp341)),((_tmp341[0]=((
_tmp342.tag=Cyc_Dict_Absent,_tmp342)),_tmp341)))));};_LL3B:;_LL3C:(void)_throw(
_tmp83);_LL38:;}};}{struct Cyc_Dict_Dict _tmp89=ge->datatypedecls;struct Cyc_Absyn_Datatypedecl***
_tmp343;return(_tmp343=_region_malloc(r,sizeof(*_tmp343)),((_tmp343[0]=((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp89,_tmp6C),_tmp343)));};}_LL31: if((_tmp6D.Abs_n).tag != 2)goto _LL2A;_tmp74=(
struct Cyc_List_List*)(_tmp6D.Abs_n).val;_LL32: {struct Cyc_Dict_Dict _tmp8B=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp74))->datatypedecls;struct Cyc_Absyn_Datatypedecl***_tmp344;return(
_tmp344=_region_malloc(r,sizeof(*_tmp344)),((_tmp344[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp6C),
_tmp344)));}_LL2A:;};}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp8D=ge->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,v);}struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0
_tmp8F;union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*
_tmp8E=q;_tmp8F=*_tmp8E;_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;{union Cyc_Absyn_Nmspace
_tmp92=_tmp90;int _tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;
struct Cyc_List_List _tmp96;struct _dyneither_ptr*_tmp97;struct Cyc_List_List*_tmp98;
struct Cyc_List_List*_tmp99;_LL3E: if((_tmp92.Loc_n).tag != 3)goto _LL40;_tmp93=(int)(
_tmp92.Loc_n).val;_LL3F: goto _LL41;_LL40: if((_tmp92.Rel_n).tag != 1)goto _LL42;
_tmp94=(struct Cyc_List_List*)(_tmp92.Rel_n).val;if(_tmp94 != 0)goto _LL42;_LL41:
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp91);_LL42: if((_tmp92.Rel_n).tag != 1)goto _LL44;_tmp95=(struct Cyc_List_List*)(
_tmp92.Rel_n).val;if(_tmp95 == 0)goto _LL44;_tmp96=*_tmp95;_tmp97=(struct
_dyneither_ptr*)_tmp96.hd;_tmp98=_tmp96.tl;_LL43: {struct Cyc_Dict_Dict _tmp9A=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp97,_tmp98))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9A,_tmp91);}
_LL44: if((_tmp92.Abs_n).tag != 2)goto _LL3D;_tmp99=(struct Cyc_List_List*)(_tmp92.Abs_n).val;
_LL45: {struct Cyc_Dict_Dict _tmp9B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp99))->enumdecls;return((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp9B,_tmp91);}_LL3D:;};}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp9C=ge->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9C,v);}struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct
_tuple0 _tmp9E;union Cyc_Absyn_Nmspace _tmp9F;struct _dyneither_ptr*_tmpA0;struct
_tuple0*_tmp9D=q;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;{union Cyc_Absyn_Nmspace
_tmpA1=_tmp9F;int _tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*_tmpA4;
struct Cyc_List_List _tmpA5;struct _dyneither_ptr*_tmpA6;struct Cyc_List_List*_tmpA7;
struct Cyc_List_List*_tmpA8;_LL47: if((_tmpA1.Loc_n).tag != 3)goto _LL49;_tmpA2=(int)(
_tmpA1.Loc_n).val;_LL48: goto _LL4A;_LL49: if((_tmpA1.Rel_n).tag != 1)goto _LL4B;
_tmpA3=(struct Cyc_List_List*)(_tmpA1.Rel_n).val;if(_tmpA3 != 0)goto _LL4B;_LL4A:
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmpA0);_LL4B: if((_tmpA1.Rel_n).tag != 1)goto _LL4D;_tmpA4=(struct Cyc_List_List*)(
_tmpA1.Rel_n).val;if(_tmpA4 == 0)goto _LL4D;_tmpA5=*_tmpA4;_tmpA6=(struct
_dyneither_ptr*)_tmpA5.hd;_tmpA7=_tmpA5.tl;_LL4C: {struct Cyc_Dict_Dict _tmpA9=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmpA6,_tmpA7))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpA9,_tmpA0);}
_LL4D: if((_tmpA1.Abs_n).tag != 2)goto _LL46;_tmpA8=(struct Cyc_List_List*)(_tmpA1.Abs_n).val;
_LL4E: {struct Cyc_Dict_Dict _tmpAA=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpA8))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmpAA,_tmpA0);}_LL46:;};}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct
Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmpAB=
te->le;if((struct Cyc_Tcenv_Tenv*)te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(
err_msg);return(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAB);}struct _RegionHandle*
Cyc_Tcenv_coerce_heap_region();struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpAC=te->le;if(_tmpAC != 0){struct Cyc_Tcenv_Fenv _tmpAE;
struct _RegionHandle*_tmpAF;struct Cyc_Tcenv_Fenv*_tmpAD=(struct Cyc_Tcenv_Fenv*)
_tmpAC;_tmpAE=*_tmpAD;_tmpAF=_tmpAE.fnrgn;return _tmpAF;}return Cyc_Tcenv_coerce_heap_region();}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Fenv*fe);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le == 0){
const char*_tmp345;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp345="put_fenv",
_tag_dyneither(_tmp345,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*_tmp346;return(
_tmp346=_region_malloc(l,sizeof(*_tmp346)),((_tmp346->ns=te->ns,((_tmp346->ae=te->ae,((
_tmp346->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp346->allow_valueof=te->allow_valueof,
_tmp346)))))))));};}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp347;
return(_tmp347=_region_malloc(l,sizeof(*_tmp347)),((_tmp347->ns=te->ns,((_tmp347->ae=
te->ae,((_tmp347->le=0,((_tmp347->allow_valueof=te->allow_valueof,_tmp347)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpB4;
struct Cyc_List_List*_tmpB5;struct Cyc_RgnOrder_RgnPO*_tmpB6;struct Cyc_Tcenv_Bindings*
_tmpB7;struct Cyc_Absyn_Stmt*_tmpB8;struct Cyc_Tcenv_CtrlEnv*_tmpB9;void*_tmpBA;
void*_tmpBB;int _tmpBC;struct _RegionHandle*_tmpBD;struct Cyc_Tcenv_Fenv _tmpB3=*f;
_tmpB4=_tmpB3.shared;_tmpB5=_tmpB3.type_vars;_tmpB6=_tmpB3.region_order;_tmpB7=
_tmpB3.locals;_tmpB8=_tmpB3.encloser;_tmpB9=_tmpB3.ctrl_env;_tmpBA=_tmpB3.capability;
_tmpBB=_tmpB3.curr_rgn;_tmpBC=_tmpB3.in_notreadctxt;_tmpBD=_tmpB3.fnrgn;{struct
Cyc_Tcenv_Fenv*_tmp348;return(_tmp348=_region_malloc(l,sizeof(*_tmp348)),((
_tmp348->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB4,((_tmp348->type_vars=(struct
Cyc_List_List*)_tmpB5,((_tmp348->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB6,((
_tmp348->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpB7),((
_tmp348->encloser=(struct Cyc_Absyn_Stmt*)_tmpB8,((_tmp348->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpB9),((_tmp348->capability=(void*)_tmpBA,((_tmp348->curr_rgn=(
void*)_tmpBB,((_tmp348->in_notreadctxt=(int)_tmpBC,((_tmp348->fnrgn=(struct
_RegionHandle*)l,_tmp348)))))))))))))))))))));};}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpC0;
struct Cyc_List_List*_tmpC1;struct Cyc_RgnOrder_RgnPO*_tmpC2;struct Cyc_Tcenv_Bindings*
_tmpC3;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Tcenv_CtrlEnv*_tmpC5;void*_tmpC6;
void*_tmpC7;int _tmpC8;struct Cyc_Tcenv_Fenv _tmpBF=*f;_tmpC0=_tmpBF.shared;_tmpC1=
_tmpBF.type_vars;_tmpC2=_tmpBF.region_order;_tmpC3=_tmpBF.locals;_tmpC4=_tmpBF.encloser;
_tmpC5=_tmpBF.ctrl_env;_tmpC6=_tmpBF.capability;_tmpC7=_tmpBF.curr_rgn;_tmpC8=
_tmpBF.in_notreadctxt;{struct _RegionHandle*_tmpCA;void*_tmpCB;void*_tmpCC;struct
_tuple9*_tmpCD;void*_tmpCE;int _tmpCF;struct Cyc_Tcenv_CtrlEnv _tmpC9=*_tmpC5;
_tmpCA=_tmpC9.ctrl_rgn;_tmpCB=_tmpC9.continue_stmt;_tmpCC=_tmpC9.break_stmt;
_tmpCD=_tmpC9.fallthru_clause;_tmpCE=_tmpC9.next_stmt;_tmpCF=_tmpC9.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp349;struct Cyc_Tcenv_CtrlEnv*_tmpD0=(_tmp349=
_region_malloc(l,sizeof(*_tmp349)),((_tmp349->ctrl_rgn=_tmpCA,((_tmp349->continue_stmt=
_tmpCB,((_tmp349->break_stmt=_tmpCC,((_tmp349->fallthru_clause=_tmpCD,((_tmp349->next_stmt=
_tmpCE,((_tmp349->try_depth=_tmpCF,_tmp349)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp34A;return(_tmp34A=_region_malloc(l,sizeof(*_tmp34A)),((_tmp34A->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpC0,((_tmp34A->type_vars=(struct Cyc_List_List*)
_tmpC1,((_tmp34A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC2,((_tmp34A->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpC3),((_tmp34A->encloser=(
struct Cyc_Absyn_Stmt*)_tmpC4,((_tmp34A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpD0,((_tmp34A->capability=(void*)_tmpC6,((_tmp34A->curr_rgn=(void*)_tmpC7,((
_tmp34A->in_notreadctxt=(int)_tmpC8,((_tmp34A->fnrgn=(struct _RegionHandle*)l,
_tmp34A)))))))))))))))))))));};};}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmpD5;struct Cyc_Tcenv_SharedFenv*_tmpD6;const char*_tmp34B;struct Cyc_Tcenv_Fenv*
_tmpD4=Cyc_Tcenv_get_fenv(te,((_tmp34B="return_typ",_tag_dyneither(_tmp34B,
sizeof(char),11))));_tmpD5=*_tmpD4;_tmpD6=_tmpD5.shared;{void*_tmpD8;struct Cyc_Tcenv_SharedFenv
_tmpD7=*_tmpD6;_tmpD8=_tmpD7.return_typ;return _tmpD8;};}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD9=te->le;if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpDB;struct Cyc_Tcenv_Fenv _tmpDA=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpD9));_tmpDB=_tmpDA.type_vars;return _tmpDB;};}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp34C;return(_tmp34C=_cycalloc(
sizeof(*_tmp34C)),((_tmp34C->v=Cyc_Tcenv_lookup_type_vars(te),_tmp34C)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpDF;struct Cyc_Tcenv_SharedFenv*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_RgnOrder_RgnPO*_tmpE2;struct Cyc_Tcenv_Bindings*
_tmpE3;struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Tcenv_CtrlEnv*_tmpE5;void*_tmpE6;
void*_tmpE7;int _tmpE8;const char*_tmp34D;struct Cyc_Tcenv_Fenv*_tmpDE=Cyc_Tcenv_get_fenv(
te,((_tmp34D="add_type_vars",_tag_dyneither(_tmp34D,sizeof(char),14))));_tmpDF=*
_tmpDE;_tmpE0=_tmpDF.shared;_tmpE1=_tmpDF.type_vars;_tmpE2=_tmpDF.region_order;
_tmpE3=_tmpDF.locals;_tmpE4=_tmpDF.encloser;_tmpE5=_tmpDF.ctrl_env;_tmpE6=_tmpDF.capability;
_tmpE7=_tmpDF.curr_rgn;_tmpE8=_tmpDF.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpE1);Cyc_Tcutil_check_unique_tvars(
loc,_tmpE9);{struct Cyc_Tcenv_Fenv*_tmp34E;struct Cyc_Tcenv_Fenv*_tmpEA=(_tmp34E=
_region_malloc(r,sizeof(*_tmp34E)),((_tmp34E->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpE0,((_tmp34E->type_vars=(struct Cyc_List_List*)_tmpE9,((_tmp34E->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpE2,((_tmp34E->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpE3),((_tmp34E->encloser=(struct Cyc_Absyn_Stmt*)
_tmpE4,((_tmp34E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpE5),((_tmp34E->capability=(void*)_tmpE6,((_tmp34E->curr_rgn=(void*)_tmpE7,((
_tmp34E->in_notreadctxt=(int)_tmpE8,((_tmp34E->fnrgn=(struct _RegionHandle*)r,
_tmp34E)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmpEA);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpEC=te->le;if(_tmpEC == 0)return Cyc_Tcenv_put_emptyfenv(
r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)
_tmpEC);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmpED=(*vd->name).f2;{union Cyc_Absyn_Nmspace _tmpEE=(*
vd->name).f1;int _tmpEF;_LL50: if((_tmpEE.Loc_n).tag != 3)goto _LL52;_tmpEF=(int)(
_tmpEE.Loc_n).val;_LL51: goto _LL4F;_LL52:;_LL53: {struct Cyc_Core_Impossible_struct
_tmp354;const char*_tmp353;struct Cyc_Core_Impossible_struct*_tmp352;(int)_throw((
void*)((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[0]=((_tmp354.tag=Cyc_Core_Impossible,((
_tmp354.f1=((_tmp353="add_local_var: called with Rel_n",_tag_dyneither(_tmp353,
sizeof(char),33))),_tmp354)))),_tmp352)))));}_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpF5;
struct Cyc_Tcenv_SharedFenv*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_RgnOrder_RgnPO*
_tmpF8;struct Cyc_Tcenv_Bindings*_tmpF9;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Tcenv_CtrlEnv*
_tmpFB;void*_tmpFC;void*_tmpFD;int _tmpFE;const char*_tmp355;struct Cyc_Tcenv_Fenv*
_tmpF4=Cyc_Tcenv_get_fenv(te,((_tmp355="add_local_var",_tag_dyneither(_tmp355,
sizeof(char),14))));_tmpF5=*_tmpF4;_tmpF6=_tmpF5.shared;_tmpF7=_tmpF5.type_vars;
_tmpF8=_tmpF5.region_order;_tmpF9=_tmpF5.locals;_tmpFA=_tmpF5.encloser;_tmpFB=
_tmpF5.ctrl_env;_tmpFC=_tmpF5.capability;_tmpFD=_tmpF5.curr_rgn;_tmpFE=_tmpF5.in_notreadctxt;{
struct Cyc_Absyn_Local_b_struct*_tmp35B;struct Cyc_Absyn_Local_b_struct _tmp35A;
struct Cyc_Tcenv_Bindings*_tmp359;struct Cyc_Tcenv_Bindings*_tmpFF=(_tmp359=
_region_malloc(r,sizeof(*_tmp359)),((_tmp359->v=_tmpED,((_tmp359->b=(void*)((
_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=((_tmp35A.tag=4,((_tmp35A.f1=vd,
_tmp35A)))),_tmp35B)))),((_tmp359->tl=(struct Cyc_Tcenv_Bindings*)_tmpF9,_tmp359)))))));
struct Cyc_Tcenv_Fenv*_tmp35C;struct Cyc_Tcenv_Fenv*_tmp100=(_tmp35C=
_region_malloc(r,sizeof(*_tmp35C)),((_tmp35C->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpF6,((_tmp35C->type_vars=(struct Cyc_List_List*)_tmpF7,((_tmp35C->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpF8,((_tmp35C->locals=(struct Cyc_Tcenv_Bindings*)
_tmpFF,((_tmp35C->encloser=(struct Cyc_Absyn_Stmt*)_tmpFA,((_tmp35C->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpFB),((_tmp35C->capability=(
void*)_tmpFC,((_tmp35C->curr_rgn=(void*)_tmpFD,((_tmp35C->in_notreadctxt=(int)
_tmpFE,((_tmp35C->fnrgn=(struct _RegionHandle*)r,_tmp35C)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp100);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp105=te->le;
if(_tmp105 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_RgnOrder_RgnPO*_tmp109;struct Cyc_Tcenv_Bindings*
_tmp10A;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Tcenv_CtrlEnv*_tmp10C;void*
_tmp10D;void*_tmp10E;int _tmp10F;struct Cyc_Tcenv_Fenv _tmp106=*_tmp105;_tmp107=
_tmp106.shared;_tmp108=_tmp106.type_vars;_tmp109=_tmp106.region_order;_tmp10A=
_tmp106.locals;_tmp10B=_tmp106.encloser;_tmp10C=_tmp106.ctrl_env;_tmp10D=_tmp106.capability;
_tmp10E=_tmp106.curr_rgn;_tmp10F=_tmp106.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp35D;struct Cyc_Tcenv_Fenv*_tmp110=(_tmp35D=_region_malloc(r,sizeof(*_tmp35D)),((
_tmp35D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp107,((_tmp35D->type_vars=(
struct Cyc_List_List*)_tmp108,((_tmp35D->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp109,((_tmp35D->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp10A),((_tmp35D->encloser=(struct Cyc_Absyn_Stmt*)_tmp10B,((_tmp35D->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp10C),((_tmp35D->capability=(
void*)_tmp10D,((_tmp35D->curr_rgn=(void*)_tmp10E,((_tmp35D->in_notreadctxt=(int)
1,((_tmp35D->fnrgn=(struct _RegionHandle*)r,_tmp35D)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp110);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp112=te->le;
if(_tmp112 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_RgnOrder_RgnPO*_tmp116;struct Cyc_Tcenv_Bindings*
_tmp117;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Tcenv_CtrlEnv*_tmp119;void*
_tmp11A;void*_tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp113=*_tmp112;_tmp114=
_tmp113.shared;_tmp115=_tmp113.type_vars;_tmp116=_tmp113.region_order;_tmp117=
_tmp113.locals;_tmp118=_tmp113.encloser;_tmp119=_tmp113.ctrl_env;_tmp11A=_tmp113.capability;
_tmp11B=_tmp113.curr_rgn;_tmp11C=_tmp113.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp35E;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp35E=_region_malloc(r,sizeof(*_tmp35E)),((
_tmp35E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114,((_tmp35E->type_vars=(
struct Cyc_List_List*)_tmp115,((_tmp35E->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp116,((_tmp35E->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp117),((_tmp35E->encloser=(struct Cyc_Absyn_Stmt*)_tmp118,((_tmp35E->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp119),((_tmp35E->capability=(
void*)_tmp11A,((_tmp35E->curr_rgn=(void*)_tmp11B,((_tmp35E->in_notreadctxt=(int)
0,((_tmp35E->fnrgn=(struct _RegionHandle*)r,_tmp35E)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp11F=te->le;if(_tmp11F == 0)return 0;{struct Cyc_Tcenv_Fenv _tmp121;int _tmp122;
struct Cyc_Tcenv_Fenv*_tmp120=(struct Cyc_Tcenv_Fenv*)_tmp11F;_tmp121=*_tmp120;
_tmp122=_tmp121.in_notreadctxt;return _tmp122;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmp123=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp126;
struct Cyc_Tcenv_SharedFenv*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_RgnOrder_RgnPO*
_tmp129;struct Cyc_Tcenv_Bindings*_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Tcenv_CtrlEnv*
_tmp12C;void*_tmp12D;void*_tmp12E;int _tmp12F;const char*_tmp35F;struct Cyc_Tcenv_Fenv*
_tmp125=Cyc_Tcenv_get_fenv(te,((_tmp35F="add_pat_var",_tag_dyneither(_tmp35F,
sizeof(char),12))));_tmp126=*_tmp125;_tmp127=_tmp126.shared;_tmp128=_tmp126.type_vars;
_tmp129=_tmp126.region_order;_tmp12A=_tmp126.locals;_tmp12B=_tmp126.encloser;
_tmp12C=_tmp126.ctrl_env;_tmp12D=_tmp126.capability;_tmp12E=_tmp126.curr_rgn;
_tmp12F=_tmp126.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*_tmp365;struct Cyc_Absyn_Pat_b_struct
_tmp364;struct Cyc_Tcenv_Bindings*_tmp363;struct Cyc_Tcenv_Bindings*_tmp130=(
_tmp363=_region_malloc(r,sizeof(*_tmp363)),((_tmp363->v=_tmp123,((_tmp363->b=(
void*)((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365[0]=((_tmp364.tag=5,((
_tmp364.f1=vd,_tmp364)))),_tmp365)))),((_tmp363->tl=(struct Cyc_Tcenv_Bindings*)
_tmp12A,_tmp363)))))));struct Cyc_Tcenv_Fenv*_tmp366;struct Cyc_Tcenv_Fenv*_tmp131=(
_tmp366=_region_malloc(r,sizeof(*_tmp366)),((_tmp366->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp127,((_tmp366->type_vars=(struct Cyc_List_List*)_tmp128,((_tmp366->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp129,((_tmp366->locals=(struct Cyc_Tcenv_Bindings*)
_tmp130,((_tmp366->encloser=(struct Cyc_Absyn_Stmt*)_tmp12B,((_tmp366->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp12C),((_tmp366->capability=(
void*)_tmp12D,((_tmp366->curr_rgn=(void*)_tmp12E,((_tmp366->in_notreadctxt=(int)
_tmp12F,((_tmp366->fnrgn=(struct _RegionHandle*)r,_tmp366)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp131);};}void*Cyc_Tcenv_lookup_ordinary(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct
_tuple0*q);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct Cyc_Tcenv_Fenv*_tmp136=
te->le;struct _tuple0 _tmp138;union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr*
_tmp13A;struct _tuple0*_tmp137=q;_tmp138=*_tmp137;_tmp139=_tmp138.f1;_tmp13A=
_tmp138.f2;{union Cyc_Absyn_Nmspace _tmp13B=_tmp139;int _tmp13C;struct Cyc_List_List*
_tmp13D;_LL55: if((_tmp13B.Loc_n).tag != 3)goto _LL57;_tmp13C=(int)(_tmp13B.Loc_n).val;
_LL56: if(_tmp136 == 0){struct Cyc_Dict_Absent_struct _tmp369;struct Cyc_Dict_Absent_struct*
_tmp368;(int)_throw((void*)((_tmp368=_cycalloc_atomic(sizeof(*_tmp368)),((
_tmp368[0]=((_tmp369.tag=Cyc_Dict_Absent,_tmp369)),_tmp368)))));}goto _LL58;_LL57:
if((_tmp13B.Rel_n).tag != 1)goto _LL59;_tmp13D=(struct Cyc_List_List*)(_tmp13B.Rel_n).val;
if(_tmp13D != 0)goto _LL59;if(!(_tmp136 != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv
_tmp141;struct Cyc_Tcenv_Bindings*_tmp142;struct Cyc_Tcenv_Fenv*_tmp140=(struct Cyc_Tcenv_Fenv*)
_tmp136;_tmp141=*_tmp140;_tmp142=_tmp141.locals;{struct _handler_cons _tmp143;
_push_handler(& _tmp143);{int _tmp145=0;if(setjmp(_tmp143.handler))_tmp145=1;if(!
_tmp145){{struct Cyc_Tcenv_VarRes_struct _tmp36C;struct Cyc_Tcenv_VarRes_struct*
_tmp36B;void*_tmp148=(void*)((_tmp36B=_region_malloc(r,sizeof(*_tmp36B)),((
_tmp36B[0]=((_tmp36C.tag=0,((_tmp36C.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp142,
_tmp13A),_tmp36C)))),_tmp36B))));_npop_handler(0);return _tmp148;};_pop_handler();}
else{void*_tmp144=(void*)_exn_thrown;void*_tmp14A=_tmp144;_LL5C: {struct Cyc_Tcenv_NoBinding_struct*
_tmp14B=(struct Cyc_Tcenv_NoBinding_struct*)_tmp14A;if(_tmp14B->tag != Cyc_Tcenv_NoBinding)
goto _LL5E;}_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL5E:;
_LL5F:(void)_throw(_tmp14A);_LL5B:;}};};}_LL59:;_LL5A: {struct _handler_cons
_tmp14C;_push_handler(& _tmp14C);{int _tmp14E=0;if(setjmp(_tmp14C.handler))_tmp14E=
1;if(!_tmp14E){{void*_tmp14F=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp14F;};_pop_handler();}else{void*_tmp14D=(void*)
_exn_thrown;void*_tmp151=_tmp14D;_LL61: {struct Cyc_Dict_Absent_struct*_tmp152=(
struct Cyc_Dict_Absent_struct*)_tmp151;if(_tmp152->tag != Cyc_Dict_Absent)goto
_LL63;}_LL62: {struct Cyc_Tcenv_VarRes_struct _tmp36F;struct Cyc_Tcenv_VarRes_struct*
_tmp36E;return(void*)((_tmp36E=_region_malloc(r,sizeof(*_tmp36E)),((_tmp36E[0]=((
_tmp36F.tag=0,((_tmp36F.f1=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp36F)))),
_tmp36E))));}_LL63:;_LL64:(void)_throw(_tmp151);_LL60:;}};}_LL54:;};}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp157;struct Cyc_Tcenv_CtrlEnv*
_tmp158;const char*_tmp370;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(te,((
_tmp370="process_continue",_tag_dyneither(_tmp370,sizeof(char),17))));_tmp157=*
_tmp156;_tmp158=_tmp157.ctrl_env;{void*_tmp159=_tmp158->continue_stmt;struct Cyc_Absyn_Stmt*
_tmp15B;_LL66: {struct Cyc_Tcenv_Stmt_j_struct*_tmp15A=(struct Cyc_Tcenv_Stmt_j_struct*)
_tmp159;if(_tmp15A->tag != 3)goto _LL68;else{_tmp15B=_tmp15A->f1;}}_LL67:{struct
Cyc_List_List*_tmp371;_tmp15B->non_local_preds=((_tmp371=_cycalloc(sizeof(*
_tmp371)),((_tmp371->hd=s,((_tmp371->tl=_tmp15B->non_local_preds,_tmp371))))));}*
sopt=(struct Cyc_Absyn_Stmt*)_tmp15B;return;_LL68: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp15C=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp159;if(_tmp15C->tag != 0)goto _LL6A;}
_LL69:{const char*_tmp374;void*_tmp373;(_tmp373=0,Cyc_Tcutil_terr(s->loc,((
_tmp374="continue not in a loop",_tag_dyneither(_tmp374,sizeof(char),23))),
_tag_dyneither(_tmp373,sizeof(void*),0)));}return;_LL6A: {struct Cyc_Tcenv_CaseEnd_j_struct*
_tmp15D=(struct Cyc_Tcenv_CaseEnd_j_struct*)_tmp159;if(_tmp15D->tag != 1)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Tcenv_FnEnd_j_struct*_tmp15E=(struct Cyc_Tcenv_FnEnd_j_struct*)
_tmp159;if(_tmp15E->tag != 2)goto _LL65;}_LL6D: {const char*_tmp375;((int(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp375="bad continue destination",
_tag_dyneither(_tmp375,sizeof(char),25))));}_LL65:;};}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp165;struct Cyc_Tcenv_CtrlEnv*_tmp166;struct Cyc_Tcenv_SharedFenv*
_tmp167;const char*_tmp376;struct Cyc_Tcenv_Fenv*_tmp164=Cyc_Tcenv_get_fenv(te,((
_tmp376="process_break",_tag_dyneither(_tmp376,sizeof(char),14))));_tmp165=*
_tmp164;_tmp166=_tmp165.ctrl_env;_tmp167=_tmp165.shared;{void*_tmp168=_tmp166->break_stmt;
struct Cyc_Absyn_Stmt*_tmp16A;_LL6F: {struct Cyc_Tcenv_Stmt_j_struct*_tmp169=(
struct Cyc_Tcenv_Stmt_j_struct*)_tmp168;if(_tmp169->tag != 3)goto _LL71;else{
_tmp16A=_tmp169->f1;}}_LL70:{struct Cyc_List_List*_tmp377;_tmp16A->non_local_preds=((
_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->hd=s,((_tmp377->tl=_tmp16A->non_local_preds,
_tmp377))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp16A;return;_LL71: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp16B=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp168;if(_tmp16B->tag != 0)goto _LL73;}
_LL72:{const char*_tmp37A;void*_tmp379;(_tmp379=0,Cyc_Tcutil_terr(s->loc,((
_tmp37A="break not in a loop or switch",_tag_dyneither(_tmp37A,sizeof(char),30))),
_tag_dyneither(_tmp379,sizeof(void*),0)));}return;_LL73: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp16C=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp168;if(_tmp16C->tag != 2)goto _LL75;}
_LL74: if(_tmp167->return_typ != (void*)& Cyc_Absyn_VoidType_val){const char*_tmp37D;
void*_tmp37C;(_tmp37C=0,Cyc_Tcutil_terr(s->loc,((_tmp37D="break causes function not to return a value",
_tag_dyneither(_tmp37D,sizeof(char),44))),_tag_dyneither(_tmp37C,sizeof(void*),0)));}
return;_LL75: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp16D=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp168;if(_tmp16D->tag != 1)goto _LL6E;}_LL76:{const char*_tmp380;void*_tmp37F;(
_tmp37F=0,Cyc_Tcutil_terr(s->loc,((_tmp380="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp380,sizeof(char),56))),_tag_dyneither(_tmp37F,sizeof(void*),0)));}
return;_LL6E:;};}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt);void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp177;struct Cyc_Tcenv_SharedFenv*_tmp178;const char*
_tmp381;struct Cyc_Tcenv_Fenv*_tmp176=Cyc_Tcenv_get_fenv(te,((_tmp381="process_goto",
_tag_dyneither(_tmp381,sizeof(char),13))));_tmp177=*_tmp176;_tmp178=_tmp177.shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp178->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict _tmp179=_tmp178->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
_tmp179,l);struct _RegionHandle*frgn=_tmp178->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp382;slopt=((_tmp382=_region_malloc(frgn,sizeof(*_tmp382)),((_tmp382[0]=0,
_tmp382))));}{struct Cyc_List_List*_tmp383;struct Cyc_List_List*new_needed=(
_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383->hd=s,((_tmp383->tl=*((struct Cyc_List_List**)
_check_null(slopt)),_tmp383)))));_tmp178->needed_labels=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(
_tmp179,l,new_needed);};}else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*
_tmp384;s->non_local_preds=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->hd=s,((
_tmp384->tl=s->non_local_preds,_tmp384))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}};}
struct _tuple9*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt);struct _tuple9*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt){struct Cyc_Tcenv_Fenv _tmp17F;struct Cyc_Tcenv_CtrlEnv*_tmp180;const char*
_tmp385;struct Cyc_Tcenv_Fenv*_tmp17E=Cyc_Tcenv_get_fenv(te,((_tmp385="process_fallthru",
_tag_dyneither(_tmp385,sizeof(char),17))));_tmp17F=*_tmp17E;_tmp180=_tmp17F.ctrl_env;{
struct _tuple9*_tmp181=(struct _tuple9*)_tmp180->fallthru_clause;if(_tmp181 != 0){{
struct Cyc_List_List*_tmp386;(((*_tmp181).f1)->body)->non_local_preds=((_tmp386=
_cycalloc(sizeof(*_tmp386)),((_tmp386->hd=s,((_tmp386->tl=(((*_tmp181).f1)->body)->non_local_preds,
_tmp386))))));}{struct Cyc_Absyn_Switch_clause**_tmp387;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387[0]=(*_tmp181).f1,_tmp387))));};}
return _tmp181;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv
_tmp186;struct Cyc_Tcenv_SharedFenv*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_RgnOrder_RgnPO*
_tmp189;struct Cyc_Tcenv_Bindings*_tmp18A;struct Cyc_Absyn_Stmt*_tmp18B;struct Cyc_Tcenv_CtrlEnv*
_tmp18C;void*_tmp18D;void*_tmp18E;int _tmp18F;const char*_tmp388;struct Cyc_Tcenv_Fenv*
_tmp185=Cyc_Tcenv_get_fenv(te,((_tmp388="set_fallthru",_tag_dyneither(_tmp388,
sizeof(char),13))));_tmp186=*_tmp185;_tmp187=_tmp186.shared;_tmp188=_tmp186.type_vars;
_tmp189=_tmp186.region_order;_tmp18A=_tmp186.locals;_tmp18B=_tmp186.encloser;
_tmp18C=_tmp186.ctrl_env;_tmp18D=_tmp186.capability;_tmp18E=_tmp186.curr_rgn;
_tmp18F=_tmp186.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp191;struct
_RegionHandle*_tmp192;void*_tmp193;void*_tmp194;struct _tuple9*_tmp195;void*
_tmp196;int _tmp197;struct Cyc_Tcenv_CtrlEnv*_tmp190=_tmp18C;_tmp191=*_tmp190;
_tmp192=_tmp191.ctrl_rgn;_tmp193=_tmp191.continue_stmt;_tmp194=_tmp191.break_stmt;
_tmp195=_tmp191.fallthru_clause;_tmp196=_tmp191.next_stmt;_tmp197=_tmp191.try_depth;{
struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){struct Cyc_List_List*
_tmp389;ft_typ=((_tmp389=_region_malloc(_tmp192,sizeof(*_tmp389)),((_tmp389->hd=((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp389->tl=ft_typ,_tmp389))))));}{
struct Cyc_Tcenv_CList*_tmp199=(struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);struct _tuple9*_tmp38C;struct Cyc_Tcenv_CtrlEnv*
_tmp38B;struct Cyc_Tcenv_CtrlEnv*_tmp19A=(_tmp38B=_region_malloc(r,sizeof(*
_tmp38B)),((_tmp38B->ctrl_rgn=_tmp192,((_tmp38B->continue_stmt=_tmp193,((_tmp38B->break_stmt=
_tmp194,((_tmp38B->fallthru_clause=((_tmp38C=_region_malloc(_tmp192,sizeof(*
_tmp38C)),((_tmp38C->f1=clause,((_tmp38C->f2=new_tvs,((_tmp38C->f3=_tmp199,
_tmp38C)))))))),((_tmp38B->next_stmt=_tmp196,((_tmp38B->try_depth=_tmp197,
_tmp38B)))))))))))));struct Cyc_Tcenv_Fenv*_tmp38D;struct Cyc_Tcenv_Fenv*_tmp19B=(
_tmp38D=_region_malloc(r,sizeof(*_tmp38D)),((_tmp38D->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp187,((_tmp38D->type_vars=(struct Cyc_List_List*)_tmp188,((_tmp38D->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp189,((_tmp38D->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp18A),((_tmp38D->encloser=(struct Cyc_Absyn_Stmt*)
_tmp18B,((_tmp38D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp19A,((_tmp38D->capability=(
void*)_tmp18D,((_tmp38D->curr_rgn=(void*)_tmp18E,((_tmp38D->in_notreadctxt=(int)
_tmp18F,((_tmp38D->fnrgn=(struct _RegionHandle*)r,_tmp38D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp19B);};};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp38E;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp38E="clear_fallthru",
_tag_dyneither(_tmp38E,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp1A0;struct
Cyc_Tcenv_CtrlEnv*_tmp1A1;struct Cyc_Tcenv_Fenv*_tmp19F=fe;_tmp1A0=*_tmp19F;
_tmp1A1=_tmp1A0.ctrl_env;_tmp1A1->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,
te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp1A5;struct Cyc_Tcenv_SharedFenv*_tmp1A6;struct Cyc_List_List*
_tmp1A7;struct Cyc_RgnOrder_RgnPO*_tmp1A8;struct Cyc_Tcenv_Bindings*_tmp1A9;struct
Cyc_Absyn_Stmt*_tmp1AA;struct Cyc_Tcenv_CtrlEnv*_tmp1AB;void*_tmp1AC;void*_tmp1AD;
int _tmp1AE;const char*_tmp38F;struct Cyc_Tcenv_Fenv*_tmp1A4=Cyc_Tcenv_get_fenv(te,((
_tmp38F="set_in_loop",_tag_dyneither(_tmp38F,sizeof(char),12))));_tmp1A5=*
_tmp1A4;_tmp1A6=_tmp1A5.shared;_tmp1A7=_tmp1A5.type_vars;_tmp1A8=_tmp1A5.region_order;
_tmp1A9=_tmp1A5.locals;_tmp1AA=_tmp1A5.encloser;_tmp1AB=_tmp1A5.ctrl_env;_tmp1AC=
_tmp1A5.capability;_tmp1AD=_tmp1A5.curr_rgn;_tmp1AE=_tmp1A5.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp39A;struct Cyc_Tcenv_Stmt_j_struct _tmp399;
struct Cyc_Tcenv_Stmt_j_struct*_tmp398;struct Cyc_Tcenv_Stmt_j_struct _tmp397;
struct Cyc_Tcenv_CtrlEnv*_tmp396;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp396=
_region_malloc(r,sizeof(*_tmp396)),((_tmp396->ctrl_rgn=r,((_tmp396->continue_stmt=(
void*)((_tmp398=_region_malloc(r,sizeof(*_tmp398)),((_tmp398[0]=((_tmp397.tag=3,((
_tmp397.f1=continue_dest,_tmp397)))),_tmp398)))),((_tmp396->break_stmt=(void*)
_tmp1AB->next_stmt,((_tmp396->next_stmt=(void*)((_tmp39A=_region_malloc(r,
sizeof(*_tmp39A)),((_tmp39A[0]=((_tmp399.tag=3,((_tmp399.f1=continue_dest,
_tmp399)))),_tmp39A)))),((_tmp396->fallthru_clause=(struct _tuple9*)_tmp1AB->fallthru_clause,((
_tmp396->try_depth=_tmp1AB->try_depth,_tmp396)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp39B;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp39B=_region_malloc(r,sizeof(*_tmp39B)),((
_tmp39B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1A6,((_tmp39B->type_vars=(
struct Cyc_List_List*)_tmp1A7,((_tmp39B->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1A8,((_tmp39B->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1A9),((_tmp39B->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AA,((_tmp39B->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp39B->capability=(void*)_tmp1AC,((_tmp39B->curr_rgn=(
void*)_tmp1AD,((_tmp39B->in_notreadctxt=(int)_tmp1AE,((_tmp39B->fnrgn=(struct
_RegionHandle*)r,_tmp39B)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp39C;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp39C="enter_try",
_tag_dyneither(_tmp39C,sizeof(char),10)))));struct Cyc_Tcenv_Fenv _tmp1B6;struct
Cyc_Tcenv_CtrlEnv*_tmp1B7;struct Cyc_Tcenv_Fenv*_tmp1B5=fe;_tmp1B6=*_tmp1B5;
_tmp1B7=_tmp1B6.ctrl_env;++ _tmp1B7->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1BB;struct Cyc_Tcenv_CtrlEnv*
_tmp1BC;const char*_tmp39D;struct Cyc_Tcenv_Fenv*_tmp1BA=Cyc_Tcenv_get_fenv(te,((
_tmp39D="get_try_depth",_tag_dyneither(_tmp39D,sizeof(char),14))));_tmp1BB=*
_tmp1BA;_tmp1BC=_tmp1BB.ctrl_env;return _tmp1BC->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*
_tmp39E;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp39E="set_in_switch",_tag_dyneither(_tmp39E,sizeof(char),14)))));struct
Cyc_Tcenv_Fenv _tmp1BE;struct Cyc_Tcenv_CtrlEnv*_tmp1BF;struct Cyc_Tcenv_Fenv*
_tmp1BD=fe;_tmp1BE=*_tmp1BD;_tmp1BF=_tmp1BE.ctrl_env;_tmp1BF->break_stmt=_tmp1BF->next_stmt;
_tmp1BF->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;return Cyc_Tcenv_put_fenv(r,te,
fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*j);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_RgnOrder_RgnPO*_tmp1C5;struct Cyc_Tcenv_Bindings*_tmp1C6;struct
Cyc_Absyn_Stmt*_tmp1C7;struct Cyc_Tcenv_CtrlEnv*_tmp1C8;void*_tmp1C9;void*_tmp1CA;
int _tmp1CB;const char*_tmp39F;struct Cyc_Tcenv_Fenv _tmp1C2=*Cyc_Tcenv_get_fenv(te,((
_tmp39F="set_next",_tag_dyneither(_tmp39F,sizeof(char),9))));_tmp1C3=_tmp1C2.shared;
_tmp1C4=_tmp1C2.type_vars;_tmp1C5=_tmp1C2.region_order;_tmp1C6=_tmp1C2.locals;
_tmp1C7=_tmp1C2.encloser;_tmp1C8=_tmp1C2.ctrl_env;_tmp1C9=_tmp1C2.capability;
_tmp1CA=_tmp1C2.curr_rgn;_tmp1CB=_tmp1C2.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
_tmp3A0;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp3A0=_region_malloc(r,sizeof(*
_tmp3A0)),((_tmp3A0->ctrl_rgn=r,((_tmp3A0->continue_stmt=(void*)_tmp1C8->continue_stmt,((
_tmp3A0->break_stmt=(void*)_tmp1C8->break_stmt,((_tmp3A0->next_stmt=j,((_tmp3A0->fallthru_clause=(
struct _tuple9*)_tmp1C8->fallthru_clause,((_tmp3A0->try_depth=_tmp1C8->try_depth,
_tmp3A0)))))))))))));struct Cyc_Tcenv_Fenv*_tmp3A1;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp3A1=_region_malloc(r,sizeof(*_tmp3A1)),((_tmp3A1->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1C3,((_tmp3A1->type_vars=(struct Cyc_List_List*)_tmp1C4,((_tmp3A1->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1C5,((_tmp3A1->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1C6),((_tmp3A1->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1C7,((_tmp3A1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3A1->capability=(
void*)_tmp1C9,((_tmp3A1->curr_rgn=(void*)_tmp1CA,((_tmp3A1->in_notreadctxt=(int)
_tmp1CB,((_tmp3A1->fnrgn=(struct _RegionHandle*)r,_tmp3A1)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1D0;struct Cyc_Tcenv_SharedFenv*_tmp1D1;const char*
_tmp3A2;struct Cyc_Tcenv_Fenv*_tmp1CF=Cyc_Tcenv_get_fenv(te,((_tmp3A2="add_label",
_tag_dyneither(_tmp3A2,sizeof(char),10))));_tmp1D0=*_tmp1CF;_tmp1D1=_tmp1D0.shared;{
struct Cyc_Dict_Dict needed=_tmp1D1->needed_labels;struct Cyc_List_List**sl_opt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
needed,v);struct _RegionHandle*frgn=_tmp1D1->frgn;if(sl_opt != 0){_tmp1D1->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1D2=*
sl_opt;s->non_local_preds=_tmp1D2;for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){void*
_tmp1D3=((struct Cyc_Absyn_Stmt*)_tmp1D2->hd)->r;struct Cyc_Absyn_Stmt*_tmp1D5;
struct Cyc_Absyn_Stmt**_tmp1D6;_LL78: {struct Cyc_Absyn_Goto_s_struct*_tmp1D4=(
struct Cyc_Absyn_Goto_s_struct*)_tmp1D3;if(_tmp1D4->tag != 8)goto _LL7A;else{
_tmp1D5=_tmp1D4->f2;_tmp1D6=(struct Cyc_Absyn_Stmt**)& _tmp1D4->f2;}}_LL79:*
_tmp1D6=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:{const char*_tmp3A3;((int(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3A3="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp3A3,sizeof(char),42))));}goto _LL77;_LL77:;}};}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1D1->seen_labels,
v)){const char*_tmp3A7;void*_tmp3A6[1];struct Cyc_String_pa_struct _tmp3A5;(_tmp3A5.tag=
0,((_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3A6[0]=&
_tmp3A5,Cyc_Tcutil_terr(s->loc,((_tmp3A7="Repeated label: %s",_tag_dyneither(
_tmp3A7,sizeof(char),19))),_tag_dyneither(_tmp3A6,sizeof(void*),1)))))));}
_tmp1D1->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1D1->seen_labels,v,
s);return te;};}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1DD;struct Cyc_Tcenv_SharedFenv*
_tmp1DE;const char*_tmp3A8;struct Cyc_Tcenv_Fenv*_tmp1DC=Cyc_Tcenv_get_fenv(te,((
_tmp3A8="all_labels_resolved",_tag_dyneither(_tmp3A8,sizeof(char),20))));_tmp1DD=*
_tmp1DC;_tmp1DE=_tmp1DD.shared;return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(
_tmp1DE->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1E1;struct Cyc_Absyn_Stmt*_tmp1E2;const char*_tmp3A9;struct Cyc_Tcenv_Fenv*
_tmp1E0=Cyc_Tcenv_get_fenv(te,((_tmp3A9="get_encloser",_tag_dyneither(_tmp3A9,
sizeof(char),13))));_tmp1E1=*_tmp1E0;_tmp1E2=_tmp1E1.encloser;return _tmp1E2;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct
Cyc_Tcenv_Tenv*te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp3AA;return(_tmp3AA=
_region_malloc(r2,sizeof(*_tmp3AA)),((_tmp3AA->ns=te->ns,((_tmp3AA->ae=te->ae,((
_tmp3AA->le=0,((_tmp3AA->allow_valueof=1,_tmp3AA)))))))));}{struct Cyc_Tcenv_SharedFenv*
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_RgnOrder_RgnPO*_tmp1E7;struct Cyc_Tcenv_Bindings*
_tmp1E8;struct Cyc_Absyn_Stmt*_tmp1E9;struct Cyc_Tcenv_CtrlEnv*_tmp1EA;void*
_tmp1EB;void*_tmp1EC;int _tmp1ED;struct Cyc_Tcenv_Fenv _tmp1E4=*((struct Cyc_Tcenv_Fenv*)
_check_null(te->le));_tmp1E5=_tmp1E4.shared;_tmp1E6=_tmp1E4.type_vars;_tmp1E7=
_tmp1E4.region_order;_tmp1E8=_tmp1E4.locals;_tmp1E9=_tmp1E4.encloser;_tmp1EA=
_tmp1E4.ctrl_env;_tmp1EB=_tmp1E4.capability;_tmp1EC=_tmp1E4.curr_rgn;_tmp1ED=
_tmp1E4.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp3AB;struct Cyc_Tcenv_Fenv*
_tmp1EE=(_tmp3AB=_region_malloc(r2,sizeof(*_tmp3AB)),((_tmp3AB->shared=(struct
Cyc_Tcenv_SharedFenv*)_tmp1E5,((_tmp3AB->type_vars=(struct Cyc_List_List*)_tmp1E6,((
_tmp3AB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E7,((_tmp3AB->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1E8),((_tmp3AB->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1E9,((_tmp3AB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1EA),((_tmp3AB->capability=(void*)_tmp1EB,((_tmp3AB->curr_rgn=(
void*)_tmp1EC,((_tmp3AB->in_notreadctxt=(int)_tmp1ED,((_tmp3AB->fnrgn=(struct
_RegionHandle*)r2,_tmp3AB)))))))))))))))))))));struct Cyc_Tcenv_Tenv*_tmp3AC;
return(_tmp3AC=_region_malloc(r2,sizeof(*_tmp3AC)),((_tmp3AC->ns=te->ns,((
_tmp3AC->ae=te->ae,((_tmp3AC->le=_tmp1EE,((_tmp3AC->allow_valueof=1,_tmp3AC)))))))));};};}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1F3;struct Cyc_List_List*_tmp1F4;struct Cyc_RgnOrder_RgnPO*_tmp1F5;struct Cyc_Tcenv_Bindings*
_tmp1F6;struct Cyc_Absyn_Stmt*_tmp1F7;struct Cyc_Tcenv_CtrlEnv*_tmp1F8;void*
_tmp1F9;void*_tmp1FA;int _tmp1FB;const char*_tmp3AD;struct Cyc_Tcenv_Fenv _tmp1F2=*
Cyc_Tcenv_get_fenv(te,((_tmp3AD="set_encloser",_tag_dyneither(_tmp3AD,sizeof(
char),13))));_tmp1F3=_tmp1F2.shared;_tmp1F4=_tmp1F2.type_vars;_tmp1F5=_tmp1F2.region_order;
_tmp1F6=_tmp1F2.locals;_tmp1F7=_tmp1F2.encloser;_tmp1F8=_tmp1F2.ctrl_env;_tmp1F9=
_tmp1F2.capability;_tmp1FA=_tmp1F2.curr_rgn;_tmp1FB=_tmp1F2.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp3AE;struct Cyc_Tcenv_Fenv*_tmp1FC=(_tmp3AE=
_region_malloc(r,sizeof(*_tmp3AE)),((_tmp3AE->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1F3,((_tmp3AE->type_vars=(struct Cyc_List_List*)_tmp1F4,((_tmp3AE->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1F5,((_tmp3AE->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1F6),((_tmp3AE->encloser=(struct Cyc_Absyn_Stmt*)s,((
_tmp3AE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1F8),((
_tmp3AE->capability=(void*)_tmp1F9,((_tmp3AE->curr_rgn=(void*)_tmp1FA,((_tmp3AE->in_notreadctxt=(
int)_tmp1FB,((_tmp3AE->fnrgn=(struct _RegionHandle*)r,_tmp3AE)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1FC);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*rgn,int resetable,int opened){struct Cyc_Absyn_Tvar*tv;{void*_tmp1FE=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp200;_LL7D: {struct Cyc_Absyn_VarType_struct*_tmp1FF=(
struct Cyc_Absyn_VarType_struct*)_tmp1FE;if(_tmp1FF->tag != 2)goto _LL7F;else{
_tmp200=_tmp1FF->f1;}}_LL7E: tv=_tmp200;goto _LL7C;_LL7F:;_LL80:{const char*_tmp3AF;
tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp3AF="bad add region",_tag_dyneither(_tmp3AF,sizeof(char),15))));}goto _LL7C;
_LL7C:;}{struct Cyc_Tcenv_SharedFenv*_tmp204;struct Cyc_List_List*_tmp205;struct
Cyc_RgnOrder_RgnPO*_tmp206;struct Cyc_Tcenv_Bindings*_tmp207;struct Cyc_Absyn_Stmt*
_tmp208;struct Cyc_Tcenv_CtrlEnv*_tmp209;void*_tmp20A;void*_tmp20B;int _tmp20C;
const char*_tmp3B0;struct Cyc_Tcenv_Fenv _tmp203=*Cyc_Tcenv_get_fenv(te,((_tmp3B0="add_region",
_tag_dyneither(_tmp3B0,sizeof(char),11))));_tmp204=_tmp203.shared;_tmp205=
_tmp203.type_vars;_tmp206=_tmp203.region_order;_tmp207=_tmp203.locals;_tmp208=
_tmp203.encloser;_tmp209=_tmp203.ctrl_env;_tmp20A=_tmp203.capability;_tmp20B=
_tmp203.curr_rgn;_tmp20C=_tmp203.in_notreadctxt;_tmp206=Cyc_RgnOrder_add_youngest(
_tmp204->frgn,_tmp206,tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct
_tmp3C3;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_AccessEff_struct*_tmp3C1;
struct Cyc_Absyn_AccessEff_struct _tmp3C0;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_JoinEff_struct*
_tmp3BE;_tmp20A=(void*)((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((
_tmp3C3.tag=24,((_tmp3C3.f1=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF->hd=(
void*)((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C0.tag=23,((
_tmp3C0.f1=(void*)rgn,_tmp3C0)))),_tmp3C1)))),((_tmp3BF->tl=((_tmp3C2=_cycalloc(
sizeof(*_tmp3C2)),((_tmp3C2->hd=_tmp20A,((_tmp3C2->tl=0,_tmp3C2)))))),_tmp3BF)))))),
_tmp3C3)))),_tmp3BE))));}{struct Cyc_Tcenv_Fenv*_tmp3C4;struct Cyc_Tcenv_Fenv*
_tmp213=(_tmp3C4=_region_malloc(r,sizeof(*_tmp3C4)),((_tmp3C4->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp204,((_tmp3C4->type_vars=(struct Cyc_List_List*)_tmp205,((_tmp3C4->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp206,((_tmp3C4->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp207),((_tmp3C4->encloser=(struct Cyc_Absyn_Stmt*)
_tmp208,((_tmp3C4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp209),((_tmp3C4->capability=(void*)_tmp20A,((_tmp3C4->curr_rgn=(void*)_tmp20B,((
_tmp3C4->in_notreadctxt=(int)_tmp20C,((_tmp3C4->fnrgn=(struct _RegionHandle*)r,
_tmp3C4)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp213);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*
_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_RgnOrder_RgnPO*_tmp219;struct Cyc_Tcenv_Bindings*
_tmp21A;struct Cyc_Absyn_Stmt*_tmp21B;struct Cyc_Tcenv_CtrlEnv*_tmp21C;void*
_tmp21D;void*_tmp21E;int _tmp21F;const char*_tmp3C5;struct Cyc_Tcenv_Fenv _tmp216=*
Cyc_Tcenv_get_fenv(te,((_tmp3C5="new_named_block",_tag_dyneither(_tmp3C5,sizeof(
char),16))));_tmp217=_tmp216.shared;_tmp218=_tmp216.type_vars;_tmp219=_tmp216.region_order;
_tmp21A=_tmp216.locals;_tmp21B=_tmp216.encloser;_tmp21C=_tmp216.ctrl_env;_tmp21D=
_tmp216.capability;_tmp21E=_tmp216.curr_rgn;_tmp21F=_tmp216.in_notreadctxt;{
const char*_tmp3C6;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp3C6="new_block",_tag_dyneither(_tmp3C6,sizeof(char),10)))));struct Cyc_Absyn_VarType_struct
_tmp3C9;struct Cyc_Absyn_VarType_struct*_tmp3C8;void*block_typ=(void*)((_tmp3C8=
_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8[0]=((_tmp3C9.tag=2,((_tmp3C9.f1=block_rgn,
_tmp3C9)))),_tmp3C8))));{struct Cyc_List_List*_tmp3CA;_tmp218=((_tmp3CA=_cycalloc(
sizeof(*_tmp3CA)),((_tmp3CA->hd=block_rgn,((_tmp3CA->tl=_tmp218,_tmp3CA))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp218);_tmp219=Cyc_RgnOrder_add_youngest(
_tmp217->frgn,_tmp219,block_rgn,0,0);{struct Cyc_Absyn_JoinEff_struct _tmp3DD;
struct Cyc_List_List*_tmp3DC;struct Cyc_Absyn_AccessEff_struct*_tmp3DB;struct Cyc_Absyn_AccessEff_struct
_tmp3DA;struct Cyc_List_List*_tmp3D9;struct Cyc_Absyn_JoinEff_struct*_tmp3D8;
_tmp21D=(void*)((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=((_tmp3DD.tag=
24,((_tmp3DD.f1=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->hd=(void*)((
_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB[0]=((_tmp3DA.tag=23,((_tmp3DA.f1=(
void*)block_typ,_tmp3DA)))),_tmp3DB)))),((_tmp3D9->tl=((_tmp3DC=_cycalloc(
sizeof(*_tmp3DC)),((_tmp3DC->hd=_tmp21D,((_tmp3DC->tl=0,_tmp3DC)))))),_tmp3D9)))))),
_tmp3DD)))),_tmp3D8))));}_tmp21E=block_typ;{struct Cyc_Tcenv_Fenv*_tmp3DE;struct
Cyc_Tcenv_Fenv*_tmp227=(_tmp3DE=_region_malloc(r,sizeof(*_tmp3DE)),((_tmp3DE->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp217,((_tmp3DE->type_vars=(struct Cyc_List_List*)
_tmp218,((_tmp3DE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp219,((_tmp3DE->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp21A),((_tmp3DE->encloser=(
struct Cyc_Absyn_Stmt*)_tmp21B,((_tmp3DE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp21C),((_tmp3DE->capability=(void*)_tmp21D,((_tmp3DE->curr_rgn=(
void*)_tmp21E,((_tmp3DE->in_notreadctxt=(int)_tmp21F,((_tmp3DE->fnrgn=(struct
_RegionHandle*)r,_tmp3DE)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp227);};};}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct
Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple12{void*f1;void*f2;}
;struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_RgnOrder_RgnPO*
_tmp231;struct Cyc_Tcenv_Bindings*_tmp232;struct Cyc_Absyn_Stmt*_tmp233;struct Cyc_Tcenv_CtrlEnv*
_tmp234;void*_tmp235;void*_tmp236;int _tmp237;const char*_tmp3DF;struct Cyc_Tcenv_Fenv
_tmp22E=*Cyc_Tcenv_get_fenv(te,((_tmp3DF="new_outlives_constraints",
_tag_dyneither(_tmp3DF,sizeof(char),25))));_tmp22F=_tmp22E.shared;_tmp230=
_tmp22E.type_vars;_tmp231=_tmp22E.region_order;_tmp232=_tmp22E.locals;_tmp233=
_tmp22E.encloser;_tmp234=_tmp22E.ctrl_env;_tmp235=_tmp22E.capability;_tmp236=
_tmp22E.curr_rgn;_tmp237=_tmp22E.in_notreadctxt;for(0;cs != 0;cs=cs->tl){_tmp231=
Cyc_RgnOrder_add_outlives_constraint(_tmp22F->frgn,_tmp231,(*((struct _tuple12*)
cs->hd)).f1,(*((struct _tuple12*)cs->hd)).f2,loc);}{struct Cyc_Tcenv_Fenv*_tmp3E0;
struct Cyc_Tcenv_Fenv*_tmp238=(_tmp3E0=_region_malloc(r,sizeof(*_tmp3E0)),((
_tmp3E0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp22F,((_tmp3E0->type_vars=(
struct Cyc_List_List*)_tmp230,((_tmp3E0->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp231,((_tmp3E0->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp232),((_tmp3E0->encloser=(struct Cyc_Absyn_Stmt*)_tmp233,((_tmp3E0->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp234),((_tmp3E0->capability=(
void*)_tmp235,((_tmp3E0->curr_rgn=(void*)_tmp236,((_tmp3E0->in_notreadctxt=(int)
_tmp237,((_tmp3E0->fnrgn=(struct _RegionHandle*)r,_tmp3E0)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp238);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc){void*_tmp23A=Cyc_Tcutil_compress(r1);void*
_tmp23B=Cyc_Tcutil_compress(r2);struct Cyc_Absyn_Kind*_tmp23C=Cyc_Tcutil_typ_kind(
_tmp23A);struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_typ_kind(_tmp23B);int r1_le_r2=
Cyc_Tcutil_kind_leq(_tmp23C,_tmp23D);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp23D,
_tmp23C);if(!r1_le_r2  && !r2_le_r1){{const char*_tmp3E7;void*_tmp3E6[4];struct Cyc_String_pa_struct
_tmp3E5;struct Cyc_String_pa_struct _tmp3E4;struct Cyc_String_pa_struct _tmp3E3;
struct Cyc_String_pa_struct _tmp3E2;(_tmp3E2.tag=0,((_tmp3E2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp23D)),((
_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp23C)),((_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23B)),((_tmp3E5.tag=0,((_tmp3E5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23A)),((
_tmp3E6[0]=& _tmp3E5,((_tmp3E6[1]=& _tmp3E4,((_tmp3E6[2]=& _tmp3E3,((_tmp3E6[3]=&
_tmp3E2,Cyc_Tcutil_terr(loc,((_tmp3E7="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp3E7,sizeof(char),82))),_tag_dyneither(_tmp3E6,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp245;void*_tmp246;struct _tuple10 _tmp244=Cyc_Tcutil_swap_kind(
_tmp23B,Cyc_Tcutil_kind_to_bound(_tmp23C));_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;{
struct _tuple10*_tmp3E8;*oldtv=((_tmp3E8=_region_malloc(r,sizeof(*_tmp3E8)),((
_tmp3E8->f1=_tmp245,((_tmp3E8->f2=_tmp246,_tmp3E8))))));};}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp249;void*_tmp24A;struct _tuple10 _tmp248=Cyc_Tcutil_swap_kind(
_tmp23A,Cyc_Tcutil_kind_to_bound(_tmp23D));_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{
struct _tuple10*_tmp3E9;*oldtv=((_tmp3E9=_region_malloc(r,sizeof(*_tmp3E9)),((
_tmp3E9->f1=_tmp249,((_tmp3E9->f2=_tmp24A,_tmp3E9))))));};}}}{struct
_RegionHandle*_tmp24C=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp24D=0;if(
_tmp23A != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp23A != (void*)& Cyc_Absyn_UniqueRgn_val){
struct Cyc_Absyn_AccessEff_struct _tmp3EC;struct Cyc_Absyn_AccessEff_struct*_tmp3EB;
void*eff1=(void*)((_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=((_tmp3EC.tag=
23,((_tmp3EC.f1=(void*)_tmp23A,_tmp3EC)))),_tmp3EB))));struct _tuple12*_tmp3EF;
struct Cyc_List_List*_tmp3EE;_tmp24D=((_tmp3EE=_region_malloc(_tmp24C,sizeof(*
_tmp3EE)),((_tmp3EE->hd=((_tmp3EF=_region_malloc(_tmp24C,sizeof(*_tmp3EF)),((
_tmp3EF->f1=eff1,((_tmp3EF->f2=_tmp23B,_tmp3EF)))))),((_tmp3EE->tl=_tmp24D,
_tmp3EE))))));}if(_tmp23B != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp23B != (void*)&
Cyc_Absyn_UniqueRgn_val){struct Cyc_Absyn_AccessEff_struct _tmp3F2;struct Cyc_Absyn_AccessEff_struct*
_tmp3F1;void*eff2=(void*)((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=((
_tmp3F2.tag=23,((_tmp3F2.f1=(void*)_tmp23B,_tmp3F2)))),_tmp3F1))));struct
_tuple12*_tmp3F5;struct Cyc_List_List*_tmp3F4;_tmp24D=((_tmp3F4=_region_malloc(
_tmp24C,sizeof(*_tmp3F4)),((_tmp3F4->hd=((_tmp3F5=_region_malloc(_tmp24C,sizeof(*
_tmp3F5)),((_tmp3F5->f1=eff2,((_tmp3F5->f2=_tmp23A,_tmp3F5)))))),((_tmp3F4->tl=
_tmp24D,_tmp3F4))))));}return Cyc_Tcenv_new_outlives_constraints((struct
_RegionHandle*)_tmp24C,te,_tmp24D,loc);};}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp256=te->le;if(_tmp256 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp256;struct Cyc_Tcenv_Fenv _tmp258;void*_tmp259;
struct Cyc_Tcenv_Fenv*_tmp257=fe;_tmp258=*_tmp257;_tmp259=_tmp258.curr_rgn;return
_tmp259;};}void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){const char*_tmp3F6;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(
te,((_tmp3F6="check_rgn_accessible",_tag_dyneither(_tmp3F6,sizeof(char),21))));
struct Cyc_Tcenv_Fenv _tmp25B;void*_tmp25C;struct Cyc_RgnOrder_RgnPO*_tmp25D;struct
Cyc_Tcenv_Fenv*_tmp25A=fe;_tmp25B=*_tmp25A;_tmp25C=_tmp25B.capability;_tmp25D=
_tmp25B.region_order;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp25C) || Cyc_Tcutil_region_in_effect(
1,rgn,_tmp25C))return;{struct Cyc_Absyn_AccessEff_struct _tmp3F9;struct Cyc_Absyn_AccessEff_struct*
_tmp3F8;if(Cyc_RgnOrder_eff_outlives_eff(_tmp25D,(void*)((_tmp3F8=_cycalloc(
sizeof(*_tmp3F8)),((_tmp3F8[0]=((_tmp3F9.tag=23,((_tmp3F9.f1=(void*)rgn,_tmp3F9)))),
_tmp3F8)))),_tmp25C))return;}{const char*_tmp3FD;void*_tmp3FC[1];struct Cyc_String_pa_struct
_tmp3FB;(_tmp3FB.tag=0,((_tmp3FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp3FC[0]=& _tmp3FB,Cyc_Tcutil_terr(loc,((_tmp3FD="Expression accesses unavailable region %s",
_tag_dyneither(_tmp3FD,sizeof(char),42))),_tag_dyneither(_tmp3FC,sizeof(void*),1)))))));};}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv _tmp266;struct Cyc_RgnOrder_RgnPO*_tmp267;const char*_tmp3FE;
struct Cyc_Tcenv_Fenv*_tmp265=Cyc_Tcenv_get_fenv(te,((_tmp3FE="check_rgn_resetable",
_tag_dyneither(_tmp3FE,sizeof(char),20))));_tmp266=*_tmp265;_tmp267=_tmp266.region_order;{
void*_tmp268=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp26A;_LL82: {
struct Cyc_Absyn_VarType_struct*_tmp269=(struct Cyc_Absyn_VarType_struct*)_tmp268;
if(_tmp269->tag != 2)goto _LL84;else{_tmp26A=_tmp269->f1;}}_LL83: if(!Cyc_RgnOrder_is_region_resetable(
_tmp267,_tmp26A)){const char*_tmp402;void*_tmp401[1];struct Cyc_String_pa_struct
_tmp400;(_tmp400.tag=0,((_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp401[0]=& _tmp400,Cyc_Tcutil_terr(loc,((_tmp402="Region %s is not resetable",
_tag_dyneither(_tmp402,sizeof(char),27))),_tag_dyneither(_tmp401,sizeof(void*),1)))))));}
return;_LL84:;_LL85: {const char*_tmp405;void*_tmp404;(_tmp404=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp405="check_rgn_resetable",
_tag_dyneither(_tmp405,sizeof(char),20))),_tag_dyneither(_tmp404,sizeof(void*),0)));}
_LL81:;};};}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp270=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(
rt_b);if(_tmp270 == 0)return rt_a == (void*)& Cyc_Absyn_UniqueRgn_val  || rt_a == (
void*)& Cyc_Absyn_HeapRgn_val  && rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;{struct
Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp270;struct Cyc_Tcenv_Fenv _tmp272;
struct Cyc_RgnOrder_RgnPO*_tmp273;struct Cyc_Tcenv_Fenv*_tmp271=fe;_tmp272=*
_tmp271;_tmp273=_tmp272.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp408;
struct Cyc_Absyn_AccessEff_struct*_tmp407;int _tmp274=Cyc_RgnOrder_effect_outlives(
_tmp273,(void*)((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407[0]=((_tmp408.tag=
23,((_tmp408.f1=(void*)rt_a,_tmp408)))),_tmp407)))),rt_b);return _tmp274;};};}
struct _tuple13{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv _tmp279;void*
_tmp27A;struct Cyc_RgnOrder_RgnPO*_tmp27B;struct Cyc_Tcenv_SharedFenv*_tmp27C;
const char*_tmp409;struct Cyc_Tcenv_Fenv*_tmp278=Cyc_Tcenv_get_fenv(te,((_tmp409="check_effect_accessible",
_tag_dyneither(_tmp409,sizeof(char),24))));_tmp279=*_tmp278;_tmp27A=_tmp279.capability;
_tmp27B=_tmp279.region_order;_tmp27C=_tmp279.shared;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp27A))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp27B,eff,_tmp27A))
return;{struct _RegionHandle*frgn=_tmp27C->frgn;struct _tuple13*_tmp40C;struct Cyc_List_List*
_tmp40B;_tmp27C->delayed_effect_checks=((_tmp40B=_region_malloc(frgn,sizeof(*
_tmp40B)),((_tmp40B->hd=((_tmp40C=_region_malloc(frgn,sizeof(*_tmp40C)),((
_tmp40C->f1=_tmp27A,((_tmp40C->f2=eff,((_tmp40C->f3=_tmp27B,((_tmp40C->f4=loc,
_tmp40C)))))))))),((_tmp40B->tl=_tmp27C->delayed_effect_checks,_tmp40B))))));};}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp281;struct Cyc_Tcenv_SharedFenv*
_tmp282;const char*_tmp40D;struct Cyc_Tcenv_Fenv*_tmp280=Cyc_Tcenv_get_fenv(te,((
_tmp40D="check_delayed_effects",_tag_dyneither(_tmp40D,sizeof(char),22))));
_tmp281=*_tmp280;_tmp282=_tmp281.shared;{struct Cyc_List_List*_tmp283=_tmp282->delayed_effect_checks;
for(0;_tmp283 != 0;_tmp283=_tmp283->tl){struct _tuple13 _tmp285;void*_tmp286;void*
_tmp287;struct Cyc_RgnOrder_RgnPO*_tmp288;struct Cyc_Position_Segment*_tmp289;
struct _tuple13*_tmp284=(struct _tuple13*)_tmp283->hd;_tmp285=*_tmp284;_tmp286=
_tmp285.f1;_tmp287=_tmp285.f2;_tmp288=_tmp285.f3;_tmp289=_tmp285.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp287,_tmp286))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp288,_tmp287,
_tmp286))continue;{const char*_tmp412;void*_tmp411[2];struct Cyc_String_pa_struct
_tmp410;struct Cyc_String_pa_struct _tmp40F;(_tmp40F.tag=0,((_tmp40F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp287)),((_tmp410.tag=
0,((_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp286)),((_tmp411[0]=& _tmp410,((_tmp411[1]=& _tmp40F,Cyc_Tcutil_terr(_tmp289,((
_tmp412="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(
_tmp412,sizeof(char),51))),_tag_dyneither(_tmp411,sizeof(void*),2)))))))))))));};}};}
struct _tuple14{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp28E=te->le;if(_tmp28E == 0){for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct
_tmp415;struct Cyc_Absyn_AccessEff_struct*_tmp414;if(!Cyc_Tcutil_subset_effect(1,(*((
struct _tuple12*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)((_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414[0]=((_tmp415.tag=23,((
_tmp415.f1=(void*)(*((struct _tuple12*)po->hd)).f2,_tmp415)))),_tmp414)))),Cyc_Absyn_empty_effect)){
const char*_tmp418;void*_tmp417;(_tmp417=0,Cyc_Tcutil_terr(loc,((_tmp418="the required region ordering is not satisfied here",
_tag_dyneither(_tmp418,sizeof(char),51))),_tag_dyneither(_tmp417,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp294;struct Cyc_RgnOrder_RgnPO*_tmp295;struct Cyc_Tcenv_SharedFenv*
_tmp296;struct Cyc_Tcenv_Fenv*_tmp293=(struct Cyc_Tcenv_Fenv*)_tmp28E;_tmp294=*
_tmp293;_tmp295=_tmp294.region_order;_tmp296=_tmp294.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp295,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){struct _tuple14*_tmp41B;struct Cyc_List_List*
_tmp41A;_tmp296->delayed_constraint_checks=((_tmp41A=_region_malloc(_tmp296->frgn,
sizeof(*_tmp41A)),((_tmp41A->hd=((_tmp41B=_region_malloc(_tmp296->frgn,sizeof(*
_tmp41B)),((_tmp41B->f1=_tmp295,((_tmp41B->f2=po,((_tmp41B->f3=loc,_tmp41B)))))))),((
_tmp41A->tl=_tmp296->delayed_constraint_checks,_tmp41A))))));}};}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp29B;struct Cyc_Tcenv_SharedFenv*_tmp29C;const char*
_tmp41C;struct Cyc_Tcenv_Fenv*_tmp29A=Cyc_Tcenv_get_fenv(te,((_tmp41C="check_delayed_constraints",
_tag_dyneither(_tmp41C,sizeof(char),26))));_tmp29B=*_tmp29A;_tmp29C=_tmp29B.shared;{
struct Cyc_List_List*_tmp29D=_tmp29C->delayed_constraint_checks;for(0;_tmp29D != 0;
_tmp29D=_tmp29D->tl){struct _tuple14 _tmp29F;struct Cyc_RgnOrder_RgnPO*_tmp2A0;
struct Cyc_List_List*_tmp2A1;struct Cyc_Position_Segment*_tmp2A2;struct _tuple14*
_tmp29E=(struct _tuple14*)_tmp29D->hd;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.f1;_tmp2A1=
_tmp29F.f2;_tmp2A2=_tmp29F.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp2A0,
_tmp2A1,(void*)& Cyc_Absyn_HeapRgn_val,1)){const char*_tmp41F;void*_tmp41E;(
_tmp41E=0,Cyc_Tcutil_terr(_tmp2A2,((_tmp41F="the required region ordering is not satisfied here",
_tag_dyneither(_tmp41F,sizeof(char),51))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}}};}
struct _tuple15{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct
Cyc_Tcenv_Bindings*locals=0;struct _dyneither_ptr*_tmp42C;const char*_tmp42B;void*
_tmp42A[1];struct Cyc_String_pa_struct _tmp429;struct Cyc_Absyn_Tvar*_tmp428;struct
Cyc_Absyn_Tvar*rgn0=(_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428->name=((
_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=(struct _dyneither_ptr)((_tmp429.tag=
0,((_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp42A[0]=& _tmp429,Cyc_aprintf(((_tmp42B="`%s",_tag_dyneither(_tmp42B,sizeof(
char),4))),_tag_dyneither(_tmp42A,sizeof(void*),1)))))))),_tmp42C)))),((_tmp428->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp428->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp428)))))));
struct Cyc_List_List*_tmp42D;struct Cyc_List_List*_tmp2A5=(_tmp42D=_cycalloc(
sizeof(*_tmp42D)),((_tmp42D->hd=rgn0,((_tmp42D->tl=fd->tvs,_tmp42D)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2A5);{struct Cyc_RgnOrder_RgnPO*_tmp2A6=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
struct Cyc_Absyn_VarType_struct _tmp430;struct Cyc_Absyn_VarType_struct*_tmp42F;
void*param_rgn=(void*)((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=((
_tmp430.tag=2,((_tmp430.f1=rgn0,_tmp430)))),_tmp42F))));struct Cyc_Core_Opt*
_tmp431;struct Cyc_Core_Opt*param_rgn_opt=(_tmp431=_cycalloc(sizeof(*_tmp431)),((
_tmp431->v=param_rgn,_tmp431)));struct Cyc_List_List*_tmp2A7=0;{struct Cyc_List_List*
_tmp2A8=fd->args;for(0;_tmp2A8 != 0;_tmp2A8=_tmp2A8->tl){struct Cyc_Absyn_Vardecl
_tmp437;struct _tuple0*_tmp436;struct Cyc_Absyn_Vardecl*_tmp435;struct Cyc_Absyn_Vardecl*
_tmp2A9=(_tmp435=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp435[0]=((
_tmp437.sc=Cyc_Absyn_Public,((_tmp437.name=((_tmp436=_cycalloc(sizeof(*_tmp436)),((
_tmp436->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp436->f2=(*((struct
_tuple15*)_tmp2A8->hd)).f1,_tmp436)))))),((_tmp437.tq=(*((struct _tuple15*)
_tmp2A8->hd)).f2,((_tmp437.type=(*((struct _tuple15*)_tmp2A8->hd)).f3,((_tmp437.initializer=
0,((_tmp437.rgn=param_rgn_opt,((_tmp437.attributes=0,((_tmp437.escapes=0,_tmp437)))))))))))))))),
_tmp435)));{struct Cyc_List_List _tmp43A;struct Cyc_List_List*_tmp439;_tmp2A7=((
_tmp439=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp439[0]=((_tmp43A.hd=
_tmp2A9,((_tmp43A.tl=_tmp2A7,_tmp43A)))),_tmp439))));}{struct Cyc_Absyn_Param_b_struct*
_tmp440;struct Cyc_Absyn_Param_b_struct _tmp43F;struct Cyc_Tcenv_Bindings*_tmp43E;
locals=((_tmp43E=_region_malloc(r,sizeof(*_tmp43E)),((_tmp43E->v=(*((struct
_tuple15*)_tmp2A8->hd)).f1,((_tmp43E->b=(void*)((_tmp440=_cycalloc(sizeof(*
_tmp440)),((_tmp440[0]=((_tmp43F.tag=3,((_tmp43F.f1=_tmp2A9,_tmp43F)))),_tmp440)))),((
_tmp43E->tl=locals,_tmp43E))))))));};}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*
_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B4;void*_tmp2B5;int _tmp2B6;struct Cyc_Absyn_VarargInfo
_tmp2B2=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2B3=
_tmp2B2.name;_tmp2B4=_tmp2B2.tq;_tmp2B5=_tmp2B2.type;_tmp2B6=_tmp2B2.inject;if(
_tmp2B3 != 0){void*_tmp2B7=Cyc_Absyn_dyneither_typ(_tmp2B5,param_rgn,_tmp2B4,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp446;struct _tuple0*_tmp445;struct Cyc_Absyn_Vardecl*
_tmp444;struct Cyc_Absyn_Vardecl*_tmp2B8=(_tmp444=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp444[0]=((_tmp446.sc=Cyc_Absyn_Public,((_tmp446.name=((_tmp445=
_cycalloc(sizeof(*_tmp445)),((_tmp445->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((
_tmp445->f2=(struct _dyneither_ptr*)_tmp2B3->v,_tmp445)))))),((_tmp446.tq=Cyc_Absyn_empty_tqual(
0),((_tmp446.type=_tmp2B7,((_tmp446.initializer=0,((_tmp446.rgn=param_rgn_opt,((
_tmp446.attributes=0,((_tmp446.escapes=0,_tmp446)))))))))))))))),_tmp444)));{
struct Cyc_List_List*_tmp447;_tmp2A7=((_tmp447=_cycalloc(sizeof(*_tmp447)),((
_tmp447->hd=_tmp2B8,((_tmp447->tl=_tmp2A7,_tmp447))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp44D;struct Cyc_Absyn_Param_b_struct _tmp44C;struct Cyc_Tcenv_Bindings*_tmp44B;
locals=((_tmp44B=_region_malloc(r,sizeof(*_tmp44B)),((_tmp44B->v=(struct
_dyneither_ptr*)_tmp2B3->v,((_tmp44B->b=(void*)((_tmp44D=_cycalloc(sizeof(*
_tmp44D)),((_tmp44D[0]=((_tmp44C.tag=3,((_tmp44C.f1=_tmp2B8,_tmp44C)))),_tmp44D)))),((
_tmp44B->tl=locals,_tmp44B))))))));};}else{const char*_tmp450;void*_tmp44F;(
_tmp44F=0,Cyc_Tcutil_terr(loc,((_tmp450="missing name for varargs",
_tag_dyneither(_tmp450,sizeof(char),25))),_tag_dyneither(_tmp44F,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp453;struct Cyc_Core_Opt*_tmp452;fd->param_vardecls=((
_tmp452=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp452[0]=((_tmp453.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2A7),
_tmp453)),_tmp452))));}{struct Cyc_Absyn_JoinEff_struct*_tmp471;struct Cyc_List_List*
_tmp470;struct Cyc_Absyn_AccessEff_struct _tmp46F;struct Cyc_Absyn_AccessEff_struct*
_tmp46E;struct Cyc_List_List*_tmp46D;struct Cyc_Absyn_JoinEff_struct _tmp46C;struct
Cyc_Tcenv_CtrlEnv*_tmp46B;struct Cyc_Tcenv_SharedFenv*_tmp46A;struct Cyc_Tcenv_Fenv*
_tmp469;return(_tmp469=_region_malloc(r,sizeof(*_tmp469)),((_tmp469->shared=(
struct Cyc_Tcenv_SharedFenv*)((_tmp46A=_region_malloc(r,sizeof(*_tmp46A)),((
_tmp46A->frgn=r,((_tmp46A->return_typ=fd->ret_type,((_tmp46A->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp46A->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp46A->delayed_effect_checks=
0,((_tmp46A->delayed_constraint_checks=0,_tmp46A)))))))))))))),((_tmp469->type_vars=(
struct Cyc_List_List*)_tmp2A5,((_tmp469->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp2A6,((_tmp469->locals=(struct Cyc_Tcenv_Bindings*)locals,((_tmp469->encloser=(
struct Cyc_Absyn_Stmt*)fd->body,((_tmp469->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
_tmp46B=_region_malloc(r,sizeof(*_tmp46B)),((_tmp46B->ctrl_rgn=r,((_tmp46B->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp46B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp46B->fallthru_clause=0,((_tmp46B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp46B->try_depth=0,_tmp46B)))))))))))))),((_tmp469->capability=(void*)((void*)((
_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=((_tmp46C.tag=24,((_tmp46C.f1=((
_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470->hd=(void*)((_tmp46E=_cycalloc(
sizeof(*_tmp46E)),((_tmp46E[0]=((_tmp46F.tag=23,((_tmp46F.f1=(void*)param_rgn,
_tmp46F)))),_tmp46E)))),((_tmp470->tl=((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((
_tmp46D->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp46D->tl=
0,_tmp46D)))))),_tmp470)))))),_tmp46C)))),_tmp471))))),((_tmp469->curr_rgn=(void*)
param_rgn,((_tmp469->in_notreadctxt=(int)0,((_tmp469->fnrgn=(struct _RegionHandle*)
r,_tmp469)))))))))))))))))))));};};}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv
_tmp2D7;struct Cyc_Tcenv_Bindings*_tmp2D8;struct Cyc_RgnOrder_RgnPO*_tmp2D9;struct
Cyc_List_List*_tmp2DA;struct Cyc_Tcenv_SharedFenv*_tmp2DB;struct _RegionHandle*
_tmp2DC;struct Cyc_Tcenv_Fenv*_tmp2D6=old_fenv;_tmp2D7=*_tmp2D6;_tmp2D8=_tmp2D7.locals;
_tmp2D9=_tmp2D7.region_order;_tmp2DA=_tmp2D7.type_vars;_tmp2DB=_tmp2D7.shared;
_tmp2DC=_tmp2D7.fnrgn;{struct _RegionHandle*_tmp2DD=_tmp2DB->frgn;struct Cyc_Tcenv_Bindings*
_tmp2DE=(struct Cyc_Tcenv_Bindings*)_tmp2D8;struct _dyneither_ptr*_tmp47E;const
char*_tmp47D;void*_tmp47C[1];struct Cyc_String_pa_struct _tmp47B;struct Cyc_Absyn_Tvar*
_tmp47A;struct Cyc_Absyn_Tvar*rgn0=(_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->name=((
_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E[0]=(struct _dyneither_ptr)((_tmp47B.tag=
0,((_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp47C[0]=& _tmp47B,Cyc_aprintf(((_tmp47D="`%s",_tag_dyneither(_tmp47D,sizeof(
char),4))),_tag_dyneither(_tmp47C,sizeof(void*),1)))))))),_tmp47E)))),((_tmp47A->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp47A->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp47A)))))));{
struct Cyc_List_List*_tmp2DF=fd->tvs;for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){
struct Cyc_Absyn_Kind _tmp2E1;enum Cyc_Absyn_KindQual _tmp2E2;enum Cyc_Absyn_AliasQual
_tmp2E3;struct Cyc_Absyn_Kind*_tmp2E0=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2DF->hd,& Cyc_Tcutil_bk);_tmp2E1=*_tmp2E0;_tmp2E2=_tmp2E1.kind;_tmp2E3=
_tmp2E1.aliasqual;if(_tmp2E2 == Cyc_Absyn_RgnKind){if(_tmp2E3 == Cyc_Absyn_Aliasable)
_tmp2D9=Cyc_RgnOrder_add_youngest(_tmp2DD,_tmp2D9,(struct Cyc_Absyn_Tvar*)_tmp2DF->hd,
0,0);else{const char*_tmp481;void*_tmp480;(_tmp480=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp481="non-intuitionistic tvar in nested_fenv",
_tag_dyneither(_tmp481,sizeof(char),39))),_tag_dyneither(_tmp480,sizeof(void*),0)));}}}}
_tmp2D9=Cyc_RgnOrder_add_youngest(_tmp2DD,_tmp2D9,rgn0,0,0);{struct Cyc_List_List*
_tmp482;struct Cyc_List_List*_tmp2E6=(_tmp482=_cycalloc(sizeof(*_tmp482)),((
_tmp482->hd=rgn0,((_tmp482->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2DA),_tmp482)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2E6);{struct Cyc_Absyn_VarType_struct _tmp485;struct Cyc_Absyn_VarType_struct*
_tmp484;void*param_rgn=(void*)((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=((
_tmp485.tag=2,((_tmp485.f1=rgn0,_tmp485)))),_tmp484))));struct Cyc_Core_Opt*
_tmp486;struct Cyc_Core_Opt*param_rgn_opt=(_tmp486=_cycalloc(sizeof(*_tmp486)),((
_tmp486->v=param_rgn,_tmp486)));struct Cyc_List_List*_tmp2E7=0;{struct Cyc_List_List*
_tmp2E8=fd->args;for(0;_tmp2E8 != 0;_tmp2E8=_tmp2E8->tl){struct Cyc_Absyn_Vardecl
_tmp48C;struct _tuple0*_tmp48B;struct Cyc_Absyn_Vardecl*_tmp48A;struct Cyc_Absyn_Vardecl*
_tmp2E9=(_tmp48A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp48A[0]=((
_tmp48C.sc=Cyc_Absyn_Public,((_tmp48C.name=((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((
_tmp48B->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp48B->f2=(*((struct
_tuple15*)_tmp2E8->hd)).f1,_tmp48B)))))),((_tmp48C.tq=(*((struct _tuple15*)
_tmp2E8->hd)).f2,((_tmp48C.type=(*((struct _tuple15*)_tmp2E8->hd)).f3,((_tmp48C.initializer=
0,((_tmp48C.rgn=param_rgn_opt,((_tmp48C.attributes=0,((_tmp48C.escapes=0,_tmp48C)))))))))))))))),
_tmp48A)));{struct Cyc_List_List _tmp48F;struct Cyc_List_List*_tmp48E;_tmp2E7=((
_tmp48E=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp48E[0]=((_tmp48F.hd=
_tmp2E9,((_tmp48F.tl=_tmp2E7,_tmp48F)))),_tmp48E))));}{struct Cyc_Absyn_Param_b_struct*
_tmp495;struct Cyc_Absyn_Param_b_struct _tmp494;struct Cyc_Tcenv_Bindings*_tmp493;
_tmp2DE=(struct Cyc_Tcenv_Bindings*)((_tmp493=_region_malloc(_tmp2DD,sizeof(*
_tmp493)),((_tmp493->v=(*((struct _tuple15*)_tmp2E8->hd)).f1,((_tmp493->b=(void*)((
_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495[0]=((_tmp494.tag=3,((_tmp494.f1=
_tmp2E9,_tmp494)))),_tmp495)))),((_tmp493->tl=_tmp2DE,_tmp493))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2F3;struct Cyc_Absyn_Tqual _tmp2F4;void*_tmp2F5;int
_tmp2F6;struct Cyc_Absyn_VarargInfo _tmp2F2=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2F3=_tmp2F2.name;_tmp2F4=_tmp2F2.tq;_tmp2F5=
_tmp2F2.type;_tmp2F6=_tmp2F2.inject;if(_tmp2F3 != 0){void*_tmp2F7=Cyc_Absyn_dyneither_typ(
_tmp2F5,param_rgn,_tmp2F4,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp49B;
struct _tuple0*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp499;struct Cyc_Absyn_Vardecl*
_tmp2F8=(_tmp499=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp499[0]=((
_tmp49B.sc=Cyc_Absyn_Public,((_tmp49B.name=((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((
_tmp49A->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp49A->f2=(struct
_dyneither_ptr*)_tmp2F3->v,_tmp49A)))))),((_tmp49B.tq=Cyc_Absyn_empty_tqual(0),((
_tmp49B.type=_tmp2F7,((_tmp49B.initializer=0,((_tmp49B.rgn=param_rgn_opt,((
_tmp49B.attributes=0,((_tmp49B.escapes=0,_tmp49B)))))))))))))))),_tmp499)));{
struct Cyc_List_List*_tmp49C;_tmp2E7=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((
_tmp49C->hd=_tmp2F8,((_tmp49C->tl=_tmp2E7,_tmp49C))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4A2;struct Cyc_Absyn_Param_b_struct _tmp4A1;struct Cyc_Tcenv_Bindings*_tmp4A0;
_tmp2DE=(struct Cyc_Tcenv_Bindings*)((_tmp4A0=_region_malloc(_tmp2DD,sizeof(*
_tmp4A0)),((_tmp4A0->v=(struct _dyneither_ptr*)_tmp2F3->v,((_tmp4A0->b=(void*)((
_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A1.tag=3,((_tmp4A1.f1=
_tmp2F8,_tmp4A1)))),_tmp4A2)))),((_tmp4A0->tl=_tmp2DE,_tmp4A0))))))));};}else{
const char*_tmp4A5;void*_tmp4A4;(_tmp4A4=0,Cyc_Tcutil_terr(loc,((_tmp4A5="missing name for varargs",
_tag_dyneither(_tmp4A5,sizeof(char),25))),_tag_dyneither(_tmp4A4,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp4A8;struct Cyc_Core_Opt*_tmp4A7;fd->param_vardecls=((
_tmp4A7=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4A7[0]=((_tmp4A8.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2E7),
_tmp4A8)),_tmp4A7))));}{struct Cyc_Absyn_JoinEff_struct*_tmp4C6;struct Cyc_List_List*
_tmp4C5;struct Cyc_Absyn_AccessEff_struct _tmp4C4;struct Cyc_Absyn_AccessEff_struct*
_tmp4C3;struct Cyc_List_List*_tmp4C2;struct Cyc_Absyn_JoinEff_struct _tmp4C1;struct
Cyc_Tcenv_CtrlEnv*_tmp4C0;struct Cyc_Tcenv_SharedFenv*_tmp4BF;struct Cyc_Tcenv_Fenv*
_tmp4BE;return(struct Cyc_Tcenv_Fenv*)((_tmp4BE=_region_malloc(_tmp2DD,sizeof(*
_tmp4BE)),((_tmp4BE->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp4BF=
_region_malloc(_tmp2DD,sizeof(*_tmp4BF)),((_tmp4BF->frgn=_tmp2DD,((_tmp4BF->return_typ=
fd->ret_type,((_tmp4BF->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2DD,
Cyc_strptrcmp),((_tmp4BF->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp2DD,Cyc_strptrcmp),((_tmp4BF->delayed_effect_checks=0,((_tmp4BF->delayed_constraint_checks=
0,_tmp4BF)))))))))))))),((_tmp4BE->type_vars=(struct Cyc_List_List*)_tmp2E6,((
_tmp4BE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2D9,((_tmp4BE->locals=(
struct Cyc_Tcenv_Bindings*)_tmp2DE,((_tmp4BE->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((
_tmp4BE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)((_tmp4C0=
_region_malloc(_tmp2DD,sizeof(*_tmp4C0)),((_tmp4C0->ctrl_rgn=_tmp2DD,((_tmp4C0->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4C0->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp4C0->fallthru_clause=0,((_tmp4C0->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp4C0->try_depth=0,_tmp4C0))))))))))))))),((_tmp4BE->capability=(void*)((void*)((
_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4C1.tag=24,((_tmp4C1.f1=((
_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5->hd=(void*)((_tmp4C3=_cycalloc(
sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C4.tag=23,((_tmp4C4.f1=(void*)param_rgn,
_tmp4C4)))),_tmp4C3)))),((_tmp4C5->tl=((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((
_tmp4C2->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp4C2->tl=
0,_tmp4C2)))))),_tmp4C5)))))),_tmp4C1)))),_tmp4C6))))),((_tmp4BE->curr_rgn=(void*)
param_rgn,((_tmp4BE->in_notreadctxt=(int)0,((_tmp4BE->fnrgn=(struct _RegionHandle*)
_tmp2DC,_tmp4BE))))))))))))))))))))));};};};};}
