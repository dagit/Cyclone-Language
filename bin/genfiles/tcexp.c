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
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;
void*f2;};void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct
_tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
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
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*
k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct
_tuple12{int f1;void*f2;};struct _tuple12 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct
Cyc_Absyn_Exp*f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};
struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp60E;(_tmp60E=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp60E,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q);static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp611;struct Cyc_Absyn_Var_e_struct*_tmp610;e->r=(void*)((_tmp610=_cycalloc(
sizeof(*_tmp610)),((_tmp610[0]=((_tmp611.tag=1,((_tmp611.f1=q,((_tmp611.f2=(void*)
_tmp7,_tmp611)))))),_tmp610))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*
_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{
_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp614;
struct Cyc_Absyn_Enum_e_struct*_tmp613;e->r=(void*)((_tmp613=_cycalloc(sizeof(*
_tmp613)),((_tmp613[0]=((_tmp614.tag=32,((_tmp614.f1=q,((_tmp614.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp614.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp614)))))))),_tmp613))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp617;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp616;e->r=(void*)((_tmp616=_cycalloc(sizeof(*_tmp616)),((_tmp616[0]=((_tmp617.tag=
33,((_tmp617.f1=q,((_tmp617.f2=(void*)_tmpC,((_tmp617.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp617)))))))),_tmp616))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp61A;struct Cyc_Absyn_Datatype_e_struct*_tmp619;e->r=(void*)((_tmp619=
_cycalloc(sizeof(*_tmp619)),((_tmp619[0]=((_tmp61A.tag=31,((_tmp61A.f1=0,((
_tmp61A.f2=_tmpF,((_tmp61A.f3=_tmp10,_tmp61A)))))))),_tmp619))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp61E;void*_tmp61D[1];struct Cyc_String_pa_struct
_tmp61C;(_tmp61C.tag=0,((_tmp61C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp61D[0]=& _tmp61C,Cyc_Tcutil_terr(e->loc,((
_tmp61E="bad occurrence of type name %s",_tag_dyneither(_tmp61E,sizeof(char),31))),
_tag_dyneither(_tmp61D,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp621;struct Cyc_Absyn_Var_e_struct*_tmp620;e->r=(void*)((_tmp620=_cycalloc(
sizeof(*_tmp620)),((_tmp620[0]=((_tmp621.tag=1,((_tmp621.f1=q,((_tmp621.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp621)))))),_tmp620))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp624;struct Cyc_Absyn_Var_e_struct*_tmp623;e->r=(void*)((_tmp623=_cycalloc(
sizeof(*_tmp623)),((_tmp623[0]=((_tmp624.tag=1,((_tmp624.f1=q,((_tmp624.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp624)))))),_tmp623))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple16{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple16*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e);static struct _tuple16*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct
_tuple16*_tmp625;return(_tmp625=_cycalloc(sizeof(*_tmp625)),((_tmp625->f1=0,((
_tmp625->f2=e,_tmp625)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es);static
void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp25=e1->r;struct _tuple1*
_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp25;if(_tmp26->tag != 2)goto _LL13;else{_tmp27=_tmp26->f1;}}_LL12: {struct
_handler_cons _tmp28;_push_handler(& _tmp28);{int _tmp2A=0;if(setjmp(_tmp28.handler))
_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*
_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*
_tmp30;struct Cyc_Absyn_Datatypefield*_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)
_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{_tmp2E=(void*)_tmp2D->f1;}}_LL17:{
struct Cyc_Absyn_FnCall_e_struct _tmp628;struct Cyc_Absyn_FnCall_e_struct*_tmp627;e->r=(
void*)((_tmp627=_cycalloc(sizeof(*_tmp627)),((_tmp627[0]=((_tmp628.tag=11,((
_tmp628.f1=e1,((_tmp628.f2=es,((_tmp628.f3=0,_tmp628)))))))),_tmp627))));}
_npop_handler(0);return;_LL18: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;if(_tmp2F->tag != 2)goto _LL1A;else{
_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19: if(_tmp31->typs == 0){const char*
_tmp62C;void*_tmp62B[1];struct Cyc_String_pa_struct _tmp62A;(_tmp62A.tag=0,((
_tmp62A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp31->name)),((_tmp62B[0]=& _tmp62A,Cyc_Tcutil_terr(e->loc,((_tmp62C="%s is a constant, not a function",
_tag_dyneither(_tmp62C,sizeof(char),33))),_tag_dyneither(_tmp62B,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Datatype_e_struct _tmp62F;struct Cyc_Absyn_Datatype_e_struct*
_tmp62E;e->r=(void*)((_tmp62E=_cycalloc(sizeof(*_tmp62E)),((_tmp62E[0]=((_tmp62F.tag=
31,((_tmp62F.f1=es,((_tmp62F.f2=_tmp30,((_tmp62F.f3=_tmp31,_tmp62F)))))))),
_tmp62E))));}_npop_handler(0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*
_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)_tmp2C;if(_tmp32->tag != 1)goto _LL1C;
else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct _tmp632;struct
Cyc_Absyn_Aggregate_e_struct*_tmp631;e->r=(void*)((_tmp631=_cycalloc(sizeof(*
_tmp631)),((_tmp631[0]=((_tmp632.tag=29,((_tmp632.f1=_tmp33->name,((_tmp632.f2=0,((
_tmp632.f3=_tmp3D,((_tmp632.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,_tmp632)))))))))),
_tmp631))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp636;void*_tmp635[1];
struct Cyc_String_pa_struct _tmp634;(_tmp634.tag=0,((_tmp634.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp635[
0]=& _tmp634,Cyc_Tcutil_terr(e->loc,((_tmp636="%s is an enum constructor, not a function",
_tag_dyneither(_tmp636,sizeof(char),42))),_tag_dyneither(_tmp635,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp639;struct Cyc_Absyn_FnCall_e_struct*
_tmp638;e->r=(void*)((_tmp638=_cycalloc(sizeof(*_tmp638)),((_tmp638[0]=((_tmp639.tag=
11,((_tmp639.f1=e1,((_tmp639.f2=es,((_tmp639.f3=0,_tmp639)))))))),_tmp638))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp63C;struct Cyc_Absyn_FnCall_e_struct*_tmp63B;e->r=(void*)((_tmp63B=_cycalloc(
sizeof(*_tmp63B)),((_tmp63B[0]=((_tmp63C.tag=11,((_tmp63C.f1=e1,((_tmp63C.f2=es,((
_tmp63C.f3=0,_tmp63C)))))))),_tmp63B))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp63F;struct Cyc_Absyn_Array_e_struct*
_tmp63E;e->r=(void*)((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E[0]=((_tmp63F.tag=
27,((_tmp63F.f1=des,_tmp63F)))),_tmp63E))));}return;}{void*t=*topt;void*_tmp4C=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp4E;union Cyc_Absyn_AggrInfoU
_tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*_tmp52;struct Cyc_Absyn_Tqual _tmp53;
_LL26: {struct Cyc_Absyn_AggrType_struct*_tmp4D=(struct Cyc_Absyn_AggrType_struct*)
_tmp4C;if(_tmp4D->tag != 12)goto _LL28;else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}
_LL27:{union Cyc_Absyn_AggrInfoU _tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**
_tmp57;struct Cyc_Absyn_Aggrdecl*_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)
goto _LL31;_tmp56=(struct _tuple3)(_tmp55.UnknownAggr).val;_LL30: {const char*
_tmp642;void*_tmp641;(_tmp641=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp642="struct type not properly set",
_tag_dyneither(_tmp642,sizeof(char),29))),_tag_dyneither(_tmp641,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp645;struct Cyc_Absyn_Aggregate_e_struct*_tmp644;e->r=(void*)((_tmp644=
_cycalloc(sizeof(*_tmp644)),((_tmp644[0]=((_tmp645.tag=29,((_tmp645.f1=_tmp58->name,((
_tmp645.f2=0,((_tmp645.f3=des,((_tmp645.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp645)))))))))),_tmp644))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp648;struct Cyc_Absyn_Array_e_struct*_tmp647;e->r=(
void*)((_tmp647=_cycalloc(sizeof(*_tmp647)),((_tmp647[0]=((_tmp648.tag=27,((
_tmp648.f1=des,_tmp648)))),_tmp647))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp64B;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp64A;e->r=(void*)((_tmp64A=_cycalloc(sizeof(*_tmp64A)),((_tmp64A[0]=((_tmp64B.tag=
30,((_tmp64B.f1=(void*)t,((_tmp64B.f2=des,_tmp64B)))))),_tmp64A))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp64E;struct Cyc_Absyn_Array_e_struct*
_tmp64D;e->r=(void*)((_tmp64D=_cycalloc(sizeof(*_tmp64D)),((_tmp64D[0]=((_tmp64E.tag=
27,((_tmp64E.f1=des,_tmp64E)))),_tmp64D))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){const char*
_tmp653;void*_tmp652[2];struct Cyc_String_pa_struct _tmp651;struct Cyc_String_pa_struct
_tmp650;(_tmp650.tag=0,((_tmp650.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp651.tag=0,((_tmp651.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((
_tmp652[0]=& _tmp651,((_tmp652[1]=& _tmp650,Cyc_Tcutil_terr(e->loc,((_tmp653="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp653,sizeof(char),53))),_tag_dyneither(_tmp652,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp654;struct
_tuple0 _tmp6C=(_tmp654.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp654.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp654)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp657;struct Cyc_Tcexp_TestEnv _tmp656;return(_tmp656.eq=((_tmp657=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp657)),((_tmp657->f1=_tmp6F,((
_tmp657->f2=_tmp72,_tmp657)))))),((_tmp656.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp656)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp658;return(_tmp658.eq=0,((_tmp658.isTrue=0,_tmp658)));};}
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)
_check_null(c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4
_tmp79;enum Cyc_Absyn_Sign _tmp7A;struct _tuple4 _tmp7B;enum Cyc_Absyn_Sign _tmp7C;
struct _tuple5 _tmp7D;enum Cyc_Absyn_Sign _tmp7E;struct _tuple7 _tmp7F;enum Cyc_Absyn_Sign
_tmp80;struct _dyneither_ptr _tmp81;struct _tuple6 _tmp82;enum Cyc_Absyn_Sign _tmp83;
int _tmp84;struct _dyneither_ptr _tmp85;int _tmp86;_LL3E: if((_tmp76.Char_c).tag != 2)
goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;_tmp78=_tmp77.f1;if(_tmp78 != 
Cyc_Absyn_Signed)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;goto _LL3D;_LL40: if((
_tmp76.Char_c).tag != 2)goto _LL42;_tmp79=(struct _tuple4)(_tmp76.Char_c).val;
_tmp7A=_tmp79.f1;if(_tmp7A != Cyc_Absyn_Unsigned)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;
goto _LL3D;_LL42: if((_tmp76.Char_c).tag != 2)goto _LL44;_tmp7B=(struct _tuple4)(
_tmp76.Char_c).val;_tmp7C=_tmp7B.f1;if(_tmp7C != Cyc_Absyn_None)goto _LL44;_LL43: t=
Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp76.Short_c).tag != 3)goto _LL46;_tmp7D=(
struct _tuple5)(_tmp76.Short_c).val;_tmp7E=_tmp7D.f1;_LL45: t=_tmp7E == Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL3D;_LL46: if((_tmp76.LongLong_c).tag
!= 5)goto _LL48;_tmp7F=(struct _tuple7)(_tmp76.LongLong_c).val;_tmp80=_tmp7F.f1;
_LL47: t=_tmp80 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL3D;_LL48: if((_tmp76.Float_c).tag != 6)goto _LL4A;_tmp81=(struct
_dyneither_ptr)(_tmp76.Float_c).val;_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A:
if((_tmp76.Int_c).tag != 4)goto _LL4C;_tmp82=(struct _tuple6)(_tmp76.Int_c).val;
_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_LL4B: if(topt == 0)t=_tmp83 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp87=Cyc_Tcutil_compress(*topt);
enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;
enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of
_tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;void*_tmp96;_LL51: {
struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp87;
if(_tmp88->tag != 6)goto _LL53;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;if(_tmp8A
!= Cyc_Absyn_Char_sz)goto _LL53;}}_LL52: switch(_tmp89){case Cyc_Absyn_Unsigned:
_LL5F: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL60: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL61: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp89,(char)_tmp84);goto _LL50;_LL53: {struct Cyc_Absyn_IntType_struct*_tmp8B=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8B->tag != 6)goto _LL55;else{_tmp8C=
_tmp8B->f1;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Short_sz)goto _LL55;}}_LL54: t=
_tmp8C == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8C,(short)_tmp84);goto _LL50;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8E=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8E->tag != 6)goto _LL57;else{_tmp8F=
_tmp8E->f1;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Int_sz)goto _LL57;}}_LL56: t=
_tmp8F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL57: {struct Cyc_Absyn_IntType_struct*_tmp91=(struct Cyc_Absyn_IntType_struct*)
_tmp87;if(_tmp91->tag != 6)goto _LL59;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;if(
_tmp93 != Cyc_Absyn_Long_sz)goto _LL59;}}_LL58: t=_tmp92 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59:{struct Cyc_Absyn_PointerType_struct*_tmp94=(
struct Cyc_Absyn_PointerType_struct*)_tmp87;if(_tmp94->tag != 5)goto _LL5B;}if(!(
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp65B;struct Cyc_Absyn_Const_e_struct*
_tmp65A;e->r=(void*)((_tmp65A=_cycalloc(sizeof(*_tmp65A)),((_tmp65A[0]=((_tmp65B.tag=
0,((_tmp65B.f1=Cyc_Absyn_Null_c,_tmp65B)))),_tmp65A))));}{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp66E;
struct Cyc_Absyn_PtrAtts _tmp66D;struct Cyc_Core_Opt*_tmp66C;struct Cyc_Core_Opt*
_tmp66B;struct Cyc_Absyn_PtrInfo _tmp66A;struct Cyc_Absyn_PointerType_struct*
_tmp669;t=(void*)((_tmp669=_cycalloc(sizeof(*_tmp669)),((_tmp669[0]=((_tmp66E.tag=
5,((_tmp66E.f1=((_tmp66A.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp66B=_cycalloc(sizeof(*_tmp66B)),((_tmp66B->v=_tmp99,_tmp66B))))),((_tmp66A.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp66A.ptr_atts=((_tmp66D.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp66C=_cycalloc(sizeof(*_tmp66C)),((
_tmp66C->v=_tmp99,_tmp66C))))),((_tmp66D.nullable=Cyc_Absyn_true_conref,((
_tmp66D.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp66D.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp66D.ptrloc=0,_tmp66D)))))))))),_tmp66A)))))),_tmp66E)))),_tmp669))));}goto
_LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)
_tmp87;if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {
struct Cyc_Absyn_ValueofType_struct _tmp671;struct Cyc_Absyn_ValueofType_struct*
_tmp670;struct Cyc_Absyn_ValueofType_struct*_tmpA0=(_tmp670=_cycalloc(sizeof(*
_tmp670)),((_tmp670[0]=((_tmp671.tag=19,((_tmp671.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0),_tmp671)))),_tmp670)));if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){{const char*_tmp676;void*_tmp675[2];struct Cyc_String_pa_struct _tmp674;
struct Cyc_String_pa_struct _tmp673;(_tmp673.tag=0,((_tmp673.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0)),((
_tmp674.tag=0,((_tmp674.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp96)),((_tmp675[0]=& _tmp674,((_tmp675[1]=& _tmp673,Cyc_Tcutil_terr(loc,((
_tmp676="expecting %s but found %s",_tag_dyneither(_tmp676,sizeof(char),26))),
_tag_dyneither(_tmp675,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp679;struct Cyc_Absyn_TagType_struct*_tmp678;t=(
void*)((_tmp678=_cycalloc(sizeof(*_tmp678)),((_tmp678[0]=((_tmp679.tag=20,((
_tmp679.f1=(void*)((void*)_tmpA0),_tmp679)))),_tmp678))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp67A;elen->topt=((
_tmp67A=_cycalloc(sizeof(*_tmp67A)),((_tmp67A->v=Cyc_Absyn_uint_typ,_tmp67A))));}{
struct Cyc_Absyn_Upper_b_struct _tmp67D;struct Cyc_Absyn_Upper_b_struct*_tmp67C;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=1,((
_tmp67D.f1=elen,_tmp67D)))),_tmp67C)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual
_tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAC;if(_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}
_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {
struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp67E;e->topt=((_tmp67E=_cycalloc(sizeof(*
_tmp67E)),((_tmp67E->v=t,_tmp67E))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp681;struct Cyc_Absyn_Upper_b_struct*
_tmp680;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp680=_cycalloc(sizeof(*_tmp680)),((_tmp680[0]=((_tmp681.tag=1,((
_tmp681.f1=elen,_tmp681)))),_tmp680)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp682;e->topt=((_tmp682=_cycalloc(sizeof(*_tmp682)),((_tmp682->v=t,_tmp682))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp695;struct Cyc_Absyn_PtrAtts _tmp694;
struct Cyc_Core_Opt*_tmp693;struct Cyc_Core_Opt*_tmp692;struct Cyc_Absyn_PtrInfo
_tmp691;struct Cyc_Absyn_PointerType_struct*_tmp690;t=(void*)((_tmp690=_cycalloc(
sizeof(*_tmp690)),((_tmp690[0]=((_tmp695.tag=5,((_tmp695.f1=((_tmp691.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp692=_cycalloc(
sizeof(*_tmp692)),((_tmp692->v=_tmpB5,_tmp692))))),((_tmp691.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp691.ptr_atts=((_tmp694.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp693=_cycalloc(sizeof(*_tmp693)),((_tmp693->v=_tmpB5,_tmp693))))),((_tmp694.nullable=
Cyc_Absyn_true_conref,((_tmp694.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp694.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp694.ptrloc=0,_tmp694)))))))))),_tmp691)))))),_tmp695)))),_tmp690))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){void*_tmpBC=b;struct Cyc_Absyn_Vardecl*
_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*
_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}
_LL6C: {const char*_tmp699;void*_tmp698[1];struct Cyc_String_pa_struct _tmp697;
return(_tmp697.tag=0,((_tmp697.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp698[0]=& _tmp697,Cyc_Tcexp_expr_err(te,loc,0,((
_tmp699="undeclared identifier: %s",_tag_dyneither(_tmp699,sizeof(char),26))),
_tag_dyneither(_tmp698,sizeof(void*),1)))))));}_LL6D: {struct Cyc_Absyn_Global_b_struct*
_tmpBE=(struct Cyc_Absyn_Global_b_struct*)_tmpBC;if(_tmpBE->tag != 1)goto _LL6F;
else{_tmpBF=_tmpBE->f1;}}_LL6E:*q=*_tmpBF->name;return _tmpBF->type;_LL6F: {
struct Cyc_Absyn_Funname_b_struct*_tmpC0=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBC;if(_tmpC0->tag != 2)goto _LL71;else{_tmpC1=_tmpC0->f1;}}_LL70:*q=*_tmpC1->name;
return Cyc_Tcutil_fndecl2typ(_tmpC1);_LL71: {struct Cyc_Absyn_Pat_b_struct*_tmpC2=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBC;if(_tmpC2->tag != 5)goto _LL73;else{_tmpC3=
_tmpC2->f1;}}_LL72: _tmpC5=_tmpC3;goto _LL74;_LL73: {struct Cyc_Absyn_Local_b_struct*
_tmpC4=(struct Cyc_Absyn_Local_b_struct*)_tmpBC;if(_tmpC4->tag != 4)goto _LL75;
else{_tmpC5=_tmpC4->f1;}}_LL74: _tmpC7=_tmpC5;goto _LL76;_LL75: {struct Cyc_Absyn_Param_b_struct*
_tmpC6=(struct Cyc_Absyn_Param_b_struct*)_tmpBC;if(_tmpC6->tag != 3)goto _LL6A;
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp69A;(*q).f1=(((_tmp69A.Loc_n).val=
0,(((_tmp69A.Loc_n).tag=3,_tmp69A))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp69D;struct Cyc_Absyn_Valueof_e_struct*
_tmp69C;e->r=(void*)((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C[0]=((_tmp69D.tag=
39,((_tmp69D.f1=(void*)_tmpCE,_tmp69D)))),_tmp69C))));}goto _LL77;_LL7A:;_LL7B:
goto _LL77;_LL77:;}return _tmpC7->type;_LL6A:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*));static
void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,
struct Cyc_Core_Opt*opt_args,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpD1=fmt->r;
union Cyc_Absyn_Cnst _tmpD3;struct _dyneither_ptr _tmpD4;struct Cyc_Absyn_Exp*_tmpD6;
struct Cyc_Absyn_Exp _tmpD7;void*_tmpD8;union Cyc_Absyn_Cnst _tmpDA;struct
_dyneither_ptr _tmpDB;_LL7D: {struct Cyc_Absyn_Const_e_struct*_tmpD2=(struct Cyc_Absyn_Const_e_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL7F;else{_tmpD3=_tmpD2->f1;if((_tmpD3.String_c).tag
!= 7)goto _LL7F;_tmpD4=(struct _dyneither_ptr)(_tmpD3.String_c).val;}}_LL7E: _tmpDB=
_tmpD4;goto _LL80;_LL7F: {struct Cyc_Absyn_Cast_e_struct*_tmpD5=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD1;if(_tmpD5->tag != 15)goto _LL81;else{_tmpD6=_tmpD5->f2;_tmpD7=*_tmpD6;
_tmpD8=_tmpD7.r;{struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL81;else{_tmpDA=_tmpD9->f1;if((_tmpDA.String_c).tag
!= 7)goto _LL81;_tmpDB=(struct _dyneither_ptr)(_tmpDA.String_c).val;}};}}_LL80:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDB,fmt->loc);goto _LL7C;
_LL81:;_LL82: if(opt_args != 0){struct Cyc_List_List*_tmpDC=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDC->hd);{struct _RegionHandle*_tmpDD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpDD,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpDD,(struct Cyc_Absyn_Exp*)_tmpDC->hd)){const
char*_tmp6A0;void*_tmp69F;(_tmp69F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp6A0="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6A0,sizeof(char),49))),_tag_dyneither(_tmp69F,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(desc_types=desc_types->tl,_tmpE0=
_tmpE0->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp6A5;void*_tmp6A4[2];struct Cyc_String_pa_struct _tmp6A3;struct Cyc_String_pa_struct
_tmp6A2;(_tmp6A2.tag=0,((_tmp6A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6A3.tag=0,((_tmp6A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6A4[0]=& _tmp6A3,((_tmp6A4[1]=& _tmp6A2,Cyc_Tcutil_terr(e->loc,((_tmp6A5="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp6A5,sizeof(char),51))),_tag_dyneither(_tmp6A4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpE5=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpE5,t) && !Cyc_Tcutil_is_noalias_path(
_tmpE5,e)){const char*_tmp6A8;void*_tmp6A7;(_tmp6A7=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpE0->hd)->loc,((_tmp6A8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6A8,sizeof(char),49))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}};}
if(desc_types != 0){const char*_tmp6AB;void*_tmp6AA;(_tmp6AA=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp6AB="too few arguments",_tag_dyneither(_tmp6AB,sizeof(char),18))),
_tag_dyneither(_tmp6AA,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp6AE;void*
_tmp6AD;(_tmp6AD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6AE="too many arguments",_tag_dyneither(_tmp6AE,sizeof(char),19))),
_tag_dyneither(_tmp6AD,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus: _LL84: if(!Cyc_Tcutil_is_numeric(
e)){const char*_tmp6B2;void*_tmp6B1[1];struct Cyc_String_pa_struct _tmp6B0;(_tmp6B0.tag=
0,((_tmp6B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6B1[0]=& _tmp6B0,Cyc_Tcutil_terr(loc,((_tmp6B2="expecting arithmetic type but found %s",
_tag_dyneither(_tmp6B2,sizeof(char),39))),_tag_dyneither(_tmp6B1,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6B6;void*_tmp6B5[1];struct Cyc_String_pa_struct _tmp6B4;(_tmp6B4.tag=
0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6B5[0]=& _tmp6B4,Cyc_Tcutil_terr(loc,((_tmp6B6="expecting integral or * type but found %s",
_tag_dyneither(_tmp6B6,sizeof(char),42))),_tag_dyneither(_tmp6B5,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6BA;void*_tmp6B9[1];struct Cyc_String_pa_struct _tmp6B8;(_tmp6B8.tag=
0,((_tmp6B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6B9[0]=& _tmp6B8,Cyc_Tcutil_terr(loc,((_tmp6BA="expecting integral type but found %s",
_tag_dyneither(_tmp6BA,sizeof(char),37))),_tag_dyneither(_tmp6B9,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts:
_LL87:{void*_tmpF5=t;struct Cyc_Absyn_PtrInfo _tmpF8;struct Cyc_Absyn_PtrAtts _tmpF9;
union Cyc_Absyn_Constraint*_tmpFA;_LL8A: {struct Cyc_Absyn_ArrayType_struct*_tmpF6=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF5;if(_tmpF6->tag != 9)goto _LL8C;}_LL8B:
goto _LL89;_LL8C: {struct Cyc_Absyn_PointerType_struct*_tmpF7=(struct Cyc_Absyn_PointerType_struct*)
_tmpF5;if(_tmpF7->tag != 5)goto _LL8E;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF8.ptr_atts;
_tmpFA=_tmpF9.bounds;}}_LL8D:{void*_tmpFB=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFA);struct Cyc_Absyn_Exp*
_tmpFE;_LL91: {struct Cyc_Absyn_DynEither_b_struct*_tmpFC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFB;if(_tmpFC->tag != 0)goto _LL93;}_LL92: goto _LL90;_LL93: {struct Cyc_Absyn_Upper_b_struct*
_tmpFD=(struct Cyc_Absyn_Upper_b_struct*)_tmpFB;if(_tmpFD->tag != 1)goto _LL90;
else{_tmpFE=_tmpFD->f1;}}_LL94: if(!Cyc_Evexp_c_can_eval(_tmpFE)){const char*
_tmp6BD;void*_tmp6BC;(_tmp6BC=0,Cyc_Tcutil_terr(loc,((_tmp6BD="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp6BD,sizeof(char),55))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp6C1;void*_tmp6C0[1];
struct Cyc_String_pa_struct _tmp6BF;(_tmp6BF.tag=0,((_tmp6BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6C0[0]=&
_tmp6BF,Cyc_Tcutil_terr(loc,((_tmp6C1="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp6C1,sizeof(char),47))),_tag_dyneither(_tmp6C0,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp6C4;void*_tmp6C3;(
_tmp6C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp6C4="Non-unary primop",_tag_dyneither(_tmp6C4,sizeof(char),17))),
_tag_dyneither(_tmp6C3,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp6C8;void*_tmp6C7[1];struct Cyc_String_pa_struct
_tmp6C6;(_tmp6C6.tag=0,((_tmp6C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp6C7[0]=& _tmp6C6,Cyc_Tcutil_terr(e1->loc,((_tmp6C8="type %s cannot be used here",
_tag_dyneither(_tmp6C8,sizeof(char),28))),_tag_dyneither(_tmp6C7,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp6CC;void*_tmp6CB[1];struct Cyc_String_pa_struct _tmp6CA;(_tmp6CA.tag=
0,((_tmp6CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp6CB[0]=& _tmp6CA,
Cyc_Tcutil_terr(e2->loc,((_tmp6CC="type %s cannot be used here",_tag_dyneither(
_tmp6CC,sizeof(char),28))),_tag_dyneither(_tmp6CB,sizeof(void*),1)))))));}return
Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);};}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmp10C=t1;struct Cyc_Absyn_PtrInfo
_tmp10E;void*_tmp10F;struct Cyc_Absyn_Tqual _tmp110;struct Cyc_Absyn_PtrAtts _tmp111;
void*_tmp112;union Cyc_Absyn_Constraint*_tmp113;union Cyc_Absyn_Constraint*_tmp114;
union Cyc_Absyn_Constraint*_tmp115;_LL97: {struct Cyc_Absyn_PointerType_struct*
_tmp10D=(struct Cyc_Absyn_PointerType_struct*)_tmp10C;if(_tmp10D->tag != 5)goto
_LL99;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10E.elt_typ;_tmp110=_tmp10E.elt_tq;
_tmp111=_tmp10E.ptr_atts;_tmp112=_tmp111.rgn;_tmp113=_tmp111.nullable;_tmp114=
_tmp111.bounds;_tmp115=_tmp111.zero_term;}}_LL98: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp10F),& Cyc_Tcutil_tmk)){const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_Tcutil_terr(
e1->loc,((_tmp6CF="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp6CF,sizeof(char),50))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp6D2;void*_tmp6D1;(_tmp6D1=0,
Cyc_Tcutil_terr(e1->loc,((_tmp6D2="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp6D2,sizeof(char),54))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp6D6;void*_tmp6D5[1];struct
Cyc_String_pa_struct _tmp6D4;(_tmp6D4.tag=0,((_tmp6D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6D5[0]=& _tmp6D4,Cyc_Tcutil_terr(
e2->loc,((_tmp6D6="expecting int but found %s",_tag_dyneither(_tmp6D6,sizeof(
char),27))),_tag_dyneither(_tmp6D5,sizeof(void*),1)))))));}{void*_tmp11D=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp114);struct Cyc_Absyn_Exp*_tmp120;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11D;if(_tmp11E->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp11F=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11D;if(_tmp11F->tag != 1)goto _LL9B;else{_tmp120=_tmp11F->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp115)){const char*
_tmp6D9;void*_tmp6D8;(_tmp6D8=0,Cyc_Tcutil_warn(e1->loc,((_tmp6D9="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp6D9,sizeof(char),70))),_tag_dyneither(_tmp6D8,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp6E3;struct Cyc_Absyn_PtrAtts _tmp6E2;struct
Cyc_Absyn_PtrInfo _tmp6E1;struct Cyc_Absyn_PointerType_struct*_tmp6E0;struct Cyc_Absyn_PointerType_struct*
_tmp123=(_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E3.tag=5,((
_tmp6E3.f1=((_tmp6E1.elt_typ=_tmp10F,((_tmp6E1.elt_tq=_tmp110,((_tmp6E1.ptr_atts=((
_tmp6E2.rgn=_tmp112,((_tmp6E2.nullable=Cyc_Absyn_true_conref,((_tmp6E2.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp6E2.zero_term=_tmp115,((_tmp6E2.ptrloc=0,
_tmp6E2)))))))))),_tmp6E1)))))),_tmp6E3)))),_tmp6E0)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp123,Cyc_Absyn_Other_coercion);return(void*)_tmp123;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{
const char*_tmp6E8;void*_tmp6E7[2];struct Cyc_String_pa_struct _tmp6E6;struct Cyc_String_pa_struct
_tmp6E5;(_tmp6E5.tag=0,((_tmp6E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp6E7[0]=& _tmp6E6,((
_tmp6E7[1]=& _tmp6E5,Cyc_Tcutil_terr(e1->loc,((_tmp6E8="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp6E8,sizeof(char),59))),_tag_dyneither(_tmp6E7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp6EB;void*_tmp6EA;(
_tmp6EA=0,Cyc_Tcutil_terr(e1->loc,((_tmp6EB="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp6EB,sizeof(char),50))),_tag_dyneither(_tmp6EA,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,
Cyc_Tcutil_terr(e1->loc,((_tmp6EE="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp6EE,sizeof(char),54))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp6F3;void*_tmp6F2[2];struct
Cyc_String_pa_struct _tmp6F1;struct Cyc_String_pa_struct _tmp6F0;(_tmp6F0.tag=0,((
_tmp6F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp6F1.tag=0,((_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp6F2[0]=& _tmp6F1,((_tmp6F2[1]=& _tmp6F0,Cyc_Tcutil_terr(
e2->loc,((_tmp6F3="expecting either %s or int but found %s",_tag_dyneither(
_tmp6F3,sizeof(char),40))),_tag_dyneither(_tmp6F2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp6F4;struct _tuple0
_tmp135=(_tmp6F4.f1=Cyc_Tcutil_compress(t1),((_tmp6F4.f2=Cyc_Tcutil_compress(t2),
_tmp6F4)));void*_tmp136;struct Cyc_Absyn_PtrInfo _tmp138;void*_tmp139;void*_tmp13A;
struct Cyc_Absyn_PtrInfo _tmp13C;void*_tmp13D;void*_tmp13E;void*_tmp140;_LLA1:
_tmp136=_tmp135.f1;{struct Cyc_Absyn_PointerType_struct*_tmp137=(struct Cyc_Absyn_PointerType_struct*)
_tmp136;if(_tmp137->tag != 5)goto _LLA3;else{_tmp138=_tmp137->f1;_tmp139=_tmp138.elt_typ;}};
_tmp13A=_tmp135.f2;{struct Cyc_Absyn_PointerType_struct*_tmp13B=(struct Cyc_Absyn_PointerType_struct*)
_tmp13A;if(_tmp13B->tag != 5)goto _LLA3;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13C.elt_typ;}};
_LLA2: if(Cyc_Tcutil_unify(_tmp139,_tmp13D))return Cyc_Absyn_sint_typ;goto _LLA0;
_LLA3: _tmp13E=_tmp135.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp13F=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp13E;if(_tmp13F->tag != 16)goto _LLA5;};_tmp140=
_tmp135.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp140;if(_tmp141->tag != 16)goto _LLA5;};_LLA4: return Cyc_Absyn_sint_typ;_LLA5:;
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp6F9;void*_tmp6F8[2];struct Cyc_String_pa_struct
_tmp6F7;struct Cyc_String_pa_struct _tmp6F6;(_tmp6F6.tag=0,((_tmp6F6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6F7.tag=
0,((_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp6F8[0]=& _tmp6F7,((_tmp6F8[1]=& _tmp6F6,Cyc_Tcutil_terr(loc,((_tmp6F9="comparison not allowed between %s and %s",
_tag_dyneither(_tmp6F9,sizeof(char),41))),_tag_dyneither(_tmp6F8,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcEqPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct _tuple0 _tmp6FA;struct _tuple0 _tmp147=(_tmp6FA.f1=
t1,((_tmp6FA.f2=t2,_tmp6FA)));void*_tmp148;void*_tmp14A;void*_tmp14B;void*
_tmp14D;_LLA8: _tmp148=_tmp147.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp149=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp148;if(_tmp149->tag != 16)goto _LLAA;
else{_tmp14A=(void*)_tmp149->f1;}};_tmp14B=_tmp147.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14B;if(_tmp14C->tag != 16)goto
_LLAA;else{_tmp14D=(void*)_tmp14C->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp6FF;void*_tmp6FE[2];struct Cyc_String_pa_struct
_tmp6FD;struct Cyc_String_pa_struct _tmp6FC;(_tmp6FC.tag=0,((_tmp6FC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6FD.tag=
0,((_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp6FE[0]=& _tmp6FD,((_tmp6FE[1]=& _tmp6FC,Cyc_Tcutil_terr(loc,((_tmp6FF="comparison not allowed between %s and %s",
_tag_dyneither(_tmp6FF,sizeof(char),41))),_tag_dyneither(_tmp6FE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){switch(p){case Cyc_Absyn_Plus: _LLAC: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLAE: goto _LLAF;case
Cyc_Absyn_Div: _LLAF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB0: goto _LLB1;case Cyc_Absyn_Bitand: _LLB1: goto _LLB2;case Cyc_Absyn_Bitor:
_LLB2: goto _LLB3;case Cyc_Absyn_Bitxor: _LLB3: goto _LLB4;case Cyc_Absyn_Bitlshift:
_LLB4: goto _LLB5;case Cyc_Absyn_Bitlrshift: _LLB5: goto _LLB6;case Cyc_Absyn_Bitarshift:
_LLB6: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLB7: goto _LLB8;case Cyc_Absyn_Neq: _LLB8: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLB9: goto _LLBA;case Cyc_Absyn_Lt: _LLBA: goto _LLBB;case Cyc_Absyn_Gte:
_LLBB: goto _LLBC;case Cyc_Absyn_Lte: _LLBC: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLBD: {const char*_tmp702;void*_tmp701;(_tmp701=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp702="bad binary primop",
_tag_dyneither(_tmp702,sizeof(char),18))),_tag_dyneither(_tmp701,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLBF: {const
char*_tmp705;void*_tmp704;return(_tmp704=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp705="primitive operator has 0 arguments",_tag_dyneither(_tmp705,sizeof(char),
35))),_tag_dyneither(_tmp704,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp708;void*_tmp707;return(_tmp707=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp708="primitive operator has > 2 arguments",_tag_dyneither(_tmp708,
sizeof(char),37))),_tag_dyneither(_tmp707,sizeof(void*),0)));}}return t;};}struct
_tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp158=e->r;void*_tmp15A;
struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15E;struct Cyc_Absyn_Vardecl*_tmp160;
void*_tmp162;struct Cyc_Absyn_Vardecl*_tmp164;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16D;struct _dyneither_ptr*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct
_dyneither_ptr*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;
struct Cyc_Absyn_Exp*_tmp177;_LLC5: {struct Cyc_Absyn_Var_e_struct*_tmp159=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp159->tag != 1)goto _LLC7;else{_tmp15A=(void*)
_tmp159->f2;{struct Cyc_Absyn_Param_b_struct*_tmp15B=(struct Cyc_Absyn_Param_b_struct*)
_tmp15A;if(_tmp15B->tag != 3)goto _LLC7;else{_tmp15C=_tmp15B->f1;}};}}_LLC6:
_tmp160=_tmp15C;goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp15D=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp15D->tag != 1)goto _LLC9;else{_tmp15E=(void*)
_tmp15D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp15F=(struct Cyc_Absyn_Local_b_struct*)
_tmp15E;if(_tmp15F->tag != 4)goto _LLC9;else{_tmp160=_tmp15F->f1;}};}}_LLC8:
_tmp164=_tmp160;goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp161=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp161->tag != 1)goto _LLCB;else{_tmp162=(void*)
_tmp161->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp163=(struct Cyc_Absyn_Pat_b_struct*)
_tmp162;if(_tmp163->tag != 5)goto _LLCB;else{_tmp164=_tmp163->f1;}};}}_LLCA:
_tmp168=_tmp164;goto _LLCC;_LLCB: {struct Cyc_Absyn_Var_e_struct*_tmp165=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp165->tag != 1)goto _LLCD;else{_tmp166=(void*)
_tmp165->f2;{struct Cyc_Absyn_Global_b_struct*_tmp167=(struct Cyc_Absyn_Global_b_struct*)
_tmp166;if(_tmp167->tag != 1)goto _LLCD;else{_tmp168=_tmp167->f1;}};}}_LLCC: if(!(
_tmp168->tq).real_const)return;goto _LLC4;_LLCD: {struct Cyc_Absyn_Subscript_e_struct*
_tmp169=(struct Cyc_Absyn_Subscript_e_struct*)_tmp158;if(_tmp169->tag != 24)goto
_LLCF;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLCE:{void*_tmp178=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_ArrayInfo _tmp17D;struct Cyc_Absyn_Tqual
_tmp17E;struct Cyc_List_List*_tmp180;_LLDC: {struct Cyc_Absyn_PointerType_struct*
_tmp179=(struct Cyc_Absyn_PointerType_struct*)_tmp178;if(_tmp179->tag != 5)goto
_LLDE;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.elt_tq;}}_LLDD: _tmp17E=_tmp17B;
goto _LLDF;_LLDE: {struct Cyc_Absyn_ArrayType_struct*_tmp17C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp178;if(_tmp17C->tag != 9)goto _LLE0;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17D.tq;}}
_LLDF: if(!_tmp17E.real_const)return;goto _LLDB;_LLE0: {struct Cyc_Absyn_TupleType_struct*
_tmp17F=(struct Cyc_Absyn_TupleType_struct*)_tmp178;if(_tmp17F->tag != 11)goto
_LLE2;else{_tmp180=_tmp17F->f1;}}_LLE1: {unsigned int _tmp182;int _tmp183;struct
_tuple13 _tmp181=Cyc_Evexp_eval_const_uint_exp(_tmp16B);_tmp182=_tmp181.f1;
_tmp183=_tmp181.f2;if(!_tmp183){{const char*_tmp70B;void*_tmp70A;(_tmp70A=0,Cyc_Tcutil_terr(
e->loc,((_tmp70B="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp70B,sizeof(char),47))),_tag_dyneither(_tmp70A,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp186;_push_handler(& _tmp186);{int _tmp188=0;if(setjmp(
_tmp186.handler))_tmp188=1;if(!_tmp188){{struct _tuple17 _tmp18A;struct Cyc_Absyn_Tqual
_tmp18B;struct _tuple17*_tmp189=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp180,(int)_tmp182);_tmp18A=*_tmp189;_tmp18B=_tmp18A.f1;if(!
_tmp18B.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp187=(
void*)_exn_thrown;void*_tmp18D=_tmp187;_LLE5: {struct Cyc_List_Nth_struct*_tmp18E=(
struct Cyc_List_Nth_struct*)_tmp18D;if(_tmp18E->tag != Cyc_List_Nth)goto _LLE7;}
_LLE6: return;_LLE7:;_LLE8:(void)_throw(_tmp18D);_LLE4:;}};}goto _LLDB;}_LLE2:;
_LLE3: goto _LLDB;_LLDB:;}goto _LLC4;_LLCF: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp16C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp158;if(_tmp16C->tag != 22)goto
_LLD1;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LLD0:{void*_tmp18F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16D->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp191;union Cyc_Absyn_AggrInfoU _tmp192;struct Cyc_Absyn_Aggrdecl**_tmp193;struct
Cyc_List_List*_tmp195;_LLEA: {struct Cyc_Absyn_AggrType_struct*_tmp190=(struct Cyc_Absyn_AggrType_struct*)
_tmp18F;if(_tmp190->tag != 12)goto _LLEC;else{_tmp191=_tmp190->f1;_tmp192=_tmp191.aggr_info;
if((_tmp192.KnownAggr).tag != 2)goto _LLEC;_tmp193=(struct Cyc_Absyn_Aggrdecl**)(
_tmp192.KnownAggr).val;}}_LLEB: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp193 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp193,_tmp16E);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp194=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp18F;if(_tmp194->tag != 13)goto _LLEE;else{_tmp195=_tmp194->f2;}}_LLED: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp195,_tmp16E);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp16F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp158;if(_tmp16F->tag != 23)goto
_LLD3;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}_LLD2:{void*_tmp196=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp170->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp198;void*_tmp199;_LLF1: {struct Cyc_Absyn_PointerType_struct*_tmp197=(struct
Cyc_Absyn_PointerType_struct*)_tmp196;if(_tmp197->tag != 5)goto _LLF3;else{_tmp198=
_tmp197->f1;_tmp199=_tmp198.elt_typ;}}_LLF2:{void*_tmp19A=Cyc_Tcutil_compress(
_tmp199);struct Cyc_Absyn_AggrInfo _tmp19C;union Cyc_Absyn_AggrInfoU _tmp19D;struct
Cyc_Absyn_Aggrdecl**_tmp19E;struct Cyc_List_List*_tmp1A0;_LLF6: {struct Cyc_Absyn_AggrType_struct*
_tmp19B=(struct Cyc_Absyn_AggrType_struct*)_tmp19A;if(_tmp19B->tag != 12)goto _LLF8;
else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.aggr_info;if((_tmp19D.KnownAggr).tag != 
2)goto _LLF8;_tmp19E=(struct Cyc_Absyn_Aggrdecl**)(_tmp19D.KnownAggr).val;}}_LLF7: {
struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp19E == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp19E,_tmp171);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF5;}_LLF8: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp19F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19A;if(_tmp19F->tag != 13)goto _LLFA;else{_tmp1A0=_tmp19F->f2;}}_LLF9: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1A0,_tmp171);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto
_LLF0;_LLF0:;}goto _LLC4;_LLD3: {struct Cyc_Absyn_Deref_e_struct*_tmp172=(struct
Cyc_Absyn_Deref_e_struct*)_tmp158;if(_tmp172->tag != 21)goto _LLD5;else{_tmp173=
_tmp172->f1;}}_LLD4:{void*_tmp1A1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp173->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1A3;struct Cyc_Absyn_Tqual
_tmp1A4;struct Cyc_Absyn_ArrayInfo _tmp1A6;struct Cyc_Absyn_Tqual _tmp1A7;_LLFD: {
struct Cyc_Absyn_PointerType_struct*_tmp1A2=(struct Cyc_Absyn_PointerType_struct*)
_tmp1A1;if(_tmp1A2->tag != 5)goto _LLFF;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A3.elt_tq;}}
_LLFE: _tmp1A7=_tmp1A4;goto _LL100;_LLFF: {struct Cyc_Absyn_ArrayType_struct*
_tmp1A5=(struct Cyc_Absyn_ArrayType_struct*)_tmp1A1;if(_tmp1A5->tag != 9)goto
_LL101;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.tq;}}_LL100: if(!_tmp1A7.real_const)
return;goto _LLFC;_LL101:;_LL102: goto _LLFC;_LLFC:;}goto _LLC4;_LLD5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp174=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp158;if(_tmp174->tag != 13)
goto _LLD7;else{_tmp175=_tmp174->f1;}}_LLD6: _tmp177=_tmp175;goto _LLD8;_LLD7: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp176=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp158;if(_tmp176->tag != 14)goto _LLD9;else{_tmp177=_tmp176->f1;}}_LLD8: Cyc_Tcexp_check_writable(
te,_tmp177);return;_LLD9:;_LLDA: goto _LLC4;_LLC4:;}{const char*_tmp70F;void*
_tmp70E[1];struct Cyc_String_pa_struct _tmp70D;(_tmp70D.tag=0,((_tmp70D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp70E[0]=&
_tmp70D,Cyc_Tcutil_terr(e->loc,((_tmp70F="attempt to write a const location: %s",
_tag_dyneither(_tmp70F,sizeof(char),38))),_tag_dyneither(_tmp70E,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp712;void*_tmp711;return(_tmp711=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp712="increment/decrement of non-lvalue",_tag_dyneither(_tmp712,
sizeof(char),34))),_tag_dyneither(_tmp711,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp715;void*_tmp714;(_tmp714=0,Cyc_Tcutil_terr(e->loc,((
_tmp715="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp715,sizeof(char),50))),_tag_dyneither(_tmp714,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp718;void*_tmp717;(_tmp717=0,Cyc_Tcutil_terr(e->loc,((_tmp718="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp718,sizeof(char),54))),_tag_dyneither(_tmp717,sizeof(void*),0)));}}
else{const char*_tmp71C;void*_tmp71B[1];struct Cyc_String_pa_struct _tmp71A;(
_tmp71A.tag=0,((_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp71B[0]=& _tmp71A,Cyc_Tcutil_terr(e->loc,((_tmp71C="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp71C,sizeof(char),44))),_tag_dyneither(_tmp71B,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1B6;int _tmp1B7;const char*_tmp71D;struct Cyc_Tcexp_TestEnv
_tmp1B5=Cyc_Tcexp_tcTest(te,e1,((_tmp71D="conditional expression",_tag_dyneither(
_tmp71D,sizeof(char),23))));_tmp1B6=_tmp1B5.eq;_tmp1B7=_tmp1B5.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp71E;
struct Cyc_List_List _tmp1B8=(_tmp71E.hd=e3,((_tmp71E.tl=0,_tmp71E)));struct Cyc_List_List
_tmp71F;struct Cyc_List_List _tmp1B9=(_tmp71F.hd=e2,((_tmp71F.tl=(struct Cyc_List_List*)&
_tmp1B8,_tmp71F)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1B9)){{const char*_tmp724;void*_tmp723[2];struct Cyc_String_pa_struct _tmp722;
struct Cyc_String_pa_struct _tmp721;(_tmp721.tag=0,((_tmp721.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp723[0]=& _tmp722,((_tmp723[1]=& _tmp721,Cyc_Tcutil_terr(
loc,((_tmp724="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp724,sizeof(char),48))),_tag_dyneither(_tmp723,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp725;Cyc_Tcexp_tcTest(
te,e1,((_tmp725="logical-and expression",_tag_dyneither(_tmp725,sizeof(char),23))));}{
const char*_tmp726;Cyc_Tcexp_tcTest(te,e2,((_tmp726="logical-and expression",
_tag_dyneither(_tmp726,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp727;Cyc_Tcexp_tcTest(te,e1,((_tmp727="logical-or expression",
_tag_dyneither(_tmp727,sizeof(char),22))));}{const char*_tmp728;Cyc_Tcexp_tcTest(
te,e2,((_tmp728="logical-or expression",_tag_dyneither(_tmp728,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1C4=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1C4,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1C5=Cyc_Tcutil_compress(
t1);_LL104: {struct Cyc_Absyn_ArrayType_struct*_tmp1C6=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C5;if(_tmp1C6->tag != 9)goto _LL106;}_LL105:{const char*_tmp72B;void*_tmp72A;(
_tmp72A=0,Cyc_Tcutil_terr(loc,((_tmp72B="cannot assign to an array",
_tag_dyneither(_tmp72B,sizeof(char),26))),_tag_dyneither(_tmp72A,sizeof(void*),0)));}
goto _LL103;_LL106:;_LL107: goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp72E;void*_tmp72D;(_tmp72D=0,Cyc_Tcutil_terr(
loc,((_tmp72E="type is abstract (can't determine size).",_tag_dyneither(_tmp72E,
sizeof(char),41))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp731;void*_tmp730;return(_tmp730=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp731="assignment to non-lvalue",_tag_dyneither(_tmp731,sizeof(char),25))),
_tag_dyneither(_tmp730,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp1CD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1CD,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1CD,e2)){const char*_tmp734;void*
_tmp733;(_tmp733=0,Cyc_Tcutil_terr(e2->loc,((_tmp734="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp734,sizeof(char),49))),_tag_dyneither(_tmp733,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp739;void*_tmp738[2];struct
Cyc_String_pa_struct _tmp737;struct Cyc_String_pa_struct _tmp736;void*_tmp1D0=(
_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp738[0]=& _tmp737,((_tmp738[1]=& _tmp736,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp739="type mismatch: %s != %s",_tag_dyneither(_tmp739,sizeof(
char),24))),_tag_dyneither(_tmp738,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1D0;}}else{enum Cyc_Absyn_Primop
_tmp1D5=(enum Cyc_Absyn_Primop)po->v;void*_tmp1D6=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1D5,e1,e2);if(!(Cyc_Tcutil_unify(_tmp1D6,t1) || Cyc_Tcutil_coerceable(
_tmp1D6) && Cyc_Tcutil_coerceable(t1))){const char*_tmp73E;void*_tmp73D[2];struct
Cyc_String_pa_struct _tmp73C;struct Cyc_String_pa_struct _tmp73B;void*_tmp1D7=(
_tmp73B.tag=0,((_tmp73B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp73D[0]=& _tmp73C,((_tmp73D[1]=& _tmp73B,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp73E="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp73E,sizeof(char),82))),_tag_dyneither(_tmp73D,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp1D6,t1);Cyc_Tcutil_explain_failure();return _tmp1D7;}return
_tmp1D6;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_List_List*fs);static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp1DC=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp1DF;enum Cyc_Absyn_Size_of _tmp1E1;_LL109: {struct
Cyc_Absyn_FloatType_struct*_tmp1DD=(struct Cyc_Absyn_FloatType_struct*)_tmp1DC;
if(_tmp1DD->tag != 7)goto _LL10B;}_LL10A: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL108;_LL10B: {struct Cyc_Absyn_IntType_struct*
_tmp1DE=(struct Cyc_Absyn_IntType_struct*)_tmp1DC;if(_tmp1DE->tag != 6)goto _LL10D;
else{_tmp1DF=_tmp1DE->f2;if(_tmp1DF != Cyc_Absyn_Char_sz)goto _LL10D;}}_LL10C: goto
_LL10E;_LL10D: {struct Cyc_Absyn_IntType_struct*_tmp1E0=(struct Cyc_Absyn_IntType_struct*)
_tmp1DC;if(_tmp1E0->tag != 6)goto _LL10F;else{_tmp1E1=_tmp1E0->f2;if(_tmp1E1 != Cyc_Absyn_Short_sz)
goto _LL10F;}}_LL10E: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;_LL108:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp1E3;struct Cyc_List_List*
_tmp1E4;struct Cyc_Position_Segment*_tmp1E5;enum Cyc_Absyn_Scope _tmp1E6;struct Cyc_Absyn_Datatypefield
_tmp1E2=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E3=_tmp1E2.name;
_tmp1E4=_tmp1E2.typs;_tmp1E5=_tmp1E2.loc;_tmp1E6=_tmp1E2.sc;if(_tmp1E4 == 0  || 
_tmp1E4->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple17*)_tmp1E4->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_Position_Segment*
_tmp1EA;enum Cyc_Absyn_Scope _tmp1EB;struct Cyc_Absyn_Datatypefield _tmp1E7=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E8=_tmp1E7.name;_tmp1E9=_tmp1E7.typs;
_tmp1EA=_tmp1E7.loc;_tmp1EB=_tmp1E7.sc;if(_tmp1E9 == 0  || _tmp1E9->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp1E9->hd)).f2);
if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Datatypefield*)((struct
Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp743;void*_tmp742[2];
struct Cyc_String_pa_struct _tmp741;struct Cyc_String_pa_struct _tmp740;(_tmp740.tag=
0,((_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp741.tag=0,((_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp742[0]=& _tmp741,((_tmp742[1]=& _tmp740,Cyc_Tcutil_terr(
e->loc,((_tmp743="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp743,sizeof(char),54))),_tag_dyneither(_tmp742,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info);static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1F1=args;struct _RegionHandle*_tmp1F2=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1F3=Cyc_Tcenv_new_block(_tmp1F2,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1F3,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1F4=t;struct Cyc_Absyn_PtrInfo _tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Tqual
_tmp1F8;struct Cyc_Absyn_PtrAtts _tmp1F9;void*_tmp1FA;union Cyc_Absyn_Constraint*
_tmp1FB;union Cyc_Absyn_Constraint*_tmp1FC;union Cyc_Absyn_Constraint*_tmp1FD;
_LL112: {struct Cyc_Absyn_PointerType_struct*_tmp1F5=(struct Cyc_Absyn_PointerType_struct*)
_tmp1F4;if(_tmp1F5->tag != 5)goto _LL114;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F6.elt_typ;
_tmp1F8=_tmp1F6.elt_tq;_tmp1F9=_tmp1F6.ptr_atts;_tmp1FA=_tmp1F9.rgn;_tmp1FB=
_tmp1F9.nullable;_tmp1FC=_tmp1F9.bounds;_tmp1FD=_tmp1F9.zero_term;}}_LL113: Cyc_Tcenv_check_rgn_accessible(
_tmp1F3,loc,_tmp1FA);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FC);{void*_tmp1FE=
Cyc_Tcutil_compress(_tmp1F7);struct Cyc_Absyn_FnInfo _tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_Core_Opt*_tmp202;void*_tmp203;struct Cyc_List_List*_tmp204;int
_tmp205;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*
_tmp208;_LL117: {struct Cyc_Absyn_FnType_struct*_tmp1FF=(struct Cyc_Absyn_FnType_struct*)
_tmp1FE;if(_tmp1FF->tag != 10)goto _LL119;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp200.tvars;
_tmp202=_tmp200.effect;_tmp203=_tmp200.ret_typ;_tmp204=_tmp200.args;_tmp205=
_tmp200.c_varargs;_tmp206=_tmp200.cyc_varargs;_tmp207=_tmp200.rgn_po;_tmp208=
_tmp200.attributes;}}_LL118: if(topt != 0)Cyc_Tcutil_unify(_tmp203,*topt);while(
_tmp1F1 != 0  && _tmp204 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F1->hd;
void*t2=(*((struct _tuple8*)_tmp204->hd)).f3;Cyc_Tcexp_tcExp(_tmp1F3,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp1F3,e1,t2)){{const char*_tmp748;void*_tmp747[2];
struct Cyc_String_pa_struct _tmp746;struct Cyc_String_pa_struct _tmp745;(_tmp745.tag=
0,((_tmp745.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp747[0]=& _tmp746,((_tmp747[1]=& _tmp745,Cyc_Tcutil_terr(e1->loc,((_tmp748="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp748,sizeof(char),57))),_tag_dyneither(_tmp747,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1F2,t2) && !Cyc_Tcutil_is_noalias_path(
_tmp1F2,e1)){const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_terr(e1->loc,((
_tmp74B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp74B,sizeof(char),49))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}_tmp1F1=
_tmp1F1->tl;_tmp204=_tmp204->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp208;for(0;a != 0;a=a->tl){void*_tmp20F=(void*)a->hd;enum Cyc_Absyn_Format_Type
_tmp211;int _tmp212;int _tmp213;_LL11C: {struct Cyc_Absyn_Format_att_struct*_tmp210=(
struct Cyc_Absyn_Format_att_struct*)_tmp20F;if(_tmp210->tag != 19)goto _LL11E;else{
_tmp211=_tmp210->f1;_tmp212=_tmp210->f2;_tmp213=_tmp210->f3;}}_LL11D:{struct
_handler_cons _tmp214;_push_handler(& _tmp214);{int _tmp216=0;if(setjmp(_tmp214.handler))
_tmp216=1;if(!_tmp216){{struct Cyc_Absyn_Exp*_tmp217=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp212 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp213 == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp74C;fmt_args=((
_tmp74C=_cycalloc(sizeof(*_tmp74C)),((_tmp74C->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp213 - 1),_tmp74C))));}
args_already_checked=1;{struct _RegionHandle*_tmp219=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp211){case Cyc_Absyn_Printf_ft: _LL120: Cyc_Tcexp_check_format_args(
_tmp1F3,_tmp217,fmt_args,_tmp219,Cyc_Formatstr_get_format_typs);break;case Cyc_Absyn_Scanf_ft:
_LL121: Cyc_Tcexp_check_format_args(_tmp1F3,_tmp217,fmt_args,_tmp219,Cyc_Formatstr_get_scanf_typs);
break;}};};_pop_handler();}else{void*_tmp215=(void*)_exn_thrown;void*_tmp21B=
_tmp215;_LL124: {struct Cyc_List_Nth_struct*_tmp21C=(struct Cyc_List_Nth_struct*)
_tmp21B;if(_tmp21C->tag != Cyc_List_Nth)goto _LL126;}_LL125:{const char*_tmp74F;
void*_tmp74E;(_tmp74E=0,Cyc_Tcutil_terr(loc,((_tmp74F="bad format arguments",
_tag_dyneither(_tmp74F,sizeof(char),21))),_tag_dyneither(_tmp74E,sizeof(void*),0)));}
goto _LL123;_LL126:;_LL127:(void)_throw(_tmp21B);_LL123:;}};}goto _LL11B;_LL11E:;
_LL11F: goto _LL11B;_LL11B:;}}if(_tmp204 != 0){const char*_tmp752;void*_tmp751;(
_tmp751=0,Cyc_Tcutil_terr(loc,((_tmp752="too few arguments for function",
_tag_dyneither(_tmp752,sizeof(char),31))),_tag_dyneither(_tmp751,sizeof(void*),0)));}
else{if((_tmp1F1 != 0  || _tmp205) || _tmp206 != 0){if(_tmp205)for(0;_tmp1F1 != 0;
_tmp1F1=_tmp1F1->tl){Cyc_Tcexp_tcExp(_tmp1F3,0,(struct Cyc_Absyn_Exp*)_tmp1F1->hd);}
else{if(_tmp206 == 0){const char*_tmp755;void*_tmp754;(_tmp754=0,Cyc_Tcutil_terr(
loc,((_tmp755="too many arguments for function",_tag_dyneither(_tmp755,sizeof(
char),32))),_tag_dyneither(_tmp754,sizeof(void*),0)));}else{void*_tmp224;int
_tmp225;struct Cyc_Absyn_VarargInfo _tmp223=*_tmp206;_tmp224=_tmp223.type;_tmp225=
_tmp223.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp756;struct Cyc_Absyn_VarargCallInfo*
_tmp226=(_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->num_varargs=0,((_tmp756->injectors=
0,((_tmp756->vai=(struct Cyc_Absyn_VarargInfo*)_tmp206,_tmp756)))))));*
vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp226;if(!_tmp225)for(0;
_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp1F1->hd;++ _tmp226->num_varargs;Cyc_Tcexp_tcExp(_tmp1F3,(void**)& _tmp224,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp1F3,e1,_tmp224)){{const char*_tmp75B;void*_tmp75A[2];
struct Cyc_String_pa_struct _tmp759;struct Cyc_String_pa_struct _tmp758;(_tmp758.tag=
0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp759.tag=0,((
_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp224)),((_tmp75A[0]=& _tmp759,((_tmp75A[1]=& _tmp758,Cyc_Tcutil_terr(loc,((
_tmp75B="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp75B,sizeof(char),49))),_tag_dyneither(_tmp75A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1F2,
_tmp224) && !Cyc_Tcutil_is_noalias_path(_tmp1F2,e1)){const char*_tmp75E;void*
_tmp75D;(_tmp75D=0,Cyc_Tcutil_terr(e1->loc,((_tmp75E="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp75E,sizeof(char),49))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}}
else{void*_tmp22D=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp224));
struct Cyc_Absyn_DatatypeInfo _tmp22F;union Cyc_Absyn_DatatypeInfoU _tmp230;struct
Cyc_Absyn_Datatypedecl**_tmp231;struct Cyc_Absyn_Datatypedecl*_tmp232;struct Cyc_List_List*
_tmp233;_LL129: {struct Cyc_Absyn_DatatypeType_struct*_tmp22E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp22D;if(_tmp22E->tag != 3)goto _LL12B;else{_tmp22F=_tmp22E->f1;_tmp230=_tmp22F.datatype_info;
if((_tmp230.KnownDatatype).tag != 2)goto _LL12B;_tmp231=(struct Cyc_Absyn_Datatypedecl**)(
_tmp230.KnownDatatype).val;_tmp232=*_tmp231;_tmp233=_tmp22F.targs;}}_LL12A: {
struct Cyc_Absyn_Datatypedecl*_tmp234=*Cyc_Tcenv_lookup_datatypedecl(_tmp1F3,loc,
_tmp232->name);struct Cyc_List_List*fields=0;if(_tmp234->fields == 0){const char*
_tmp762;void*_tmp761[1];struct Cyc_String_pa_struct _tmp760;(_tmp760.tag=0,((
_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp224)),((_tmp761[0]=& _tmp760,Cyc_Tcutil_terr(loc,((_tmp762="can't inject into abstract datatype %s",
_tag_dyneither(_tmp762,sizeof(char),39))),_tag_dyneither(_tmp761,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp234->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp224),Cyc_Tcenv_curr_rgn(
_tmp1F3))){const char*_tmp767;void*_tmp766[2];struct Cyc_String_pa_struct _tmp765;
struct Cyc_String_pa_struct _tmp764;(_tmp764.tag=0,((_tmp764.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp1F3))),((_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp224)),((_tmp766[0]=& _tmp765,((_tmp766[
1]=& _tmp764,Cyc_Tcutil_terr(loc,((_tmp767="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp767,sizeof(char),49))),_tag_dyneither(_tmp766,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp23C=Cyc_Tcenv_get_fnrgn(_tmp1F3);{struct Cyc_List_List*
_tmp23D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp23C,_tmp23C,
_tmp234->tvs,_tmp233);for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){++ _tmp226->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F1->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1F3,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp23C,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp23C,e1)){const char*_tmp76A;void*_tmp769;(_tmp769=0,Cyc_Tcutil_terr(e1->loc,((
_tmp76A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp76A,sizeof(char),49))),_tag_dyneither(_tmp769,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp240=Cyc_Tcexp_tcInjection(_tmp1F3,e1,Cyc_Tcutil_pointer_elt_type(
_tmp224),_tmp23C,_tmp23D,fields);if(_tmp240 != 0){struct Cyc_List_List*_tmp76B;
_tmp226->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp226->injectors,((_tmp76B=_cycalloc(sizeof(*_tmp76B)),((
_tmp76B->hd=(struct Cyc_Absyn_Datatypefield*)_tmp240,((_tmp76B->tl=0,_tmp76B)))))));}};};}}
goto _LL128;};}_LL12B:;_LL12C:{const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_terr(
loc,((_tmp76E="bad inject vararg type",_tag_dyneither(_tmp76E,sizeof(char),23))),
_tag_dyneither(_tmp76D,sizeof(void*),0)));}goto _LL128;_LL128:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp1F3,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp202))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1F3,loc,_tmp207);return _tmp203;};_LL119:;_LL11A: {const char*_tmp771;void*
_tmp770;return(_tmp770=0,Cyc_Tcexp_expr_err(_tmp1F3,loc,topt,((_tmp771="expected pointer to function",
_tag_dyneither(_tmp771,sizeof(char),29))),_tag_dyneither(_tmp770,sizeof(void*),0)));}
_LL116:;};_LL114:;_LL115: {const char*_tmp774;void*_tmp773;return(_tmp773=0,Cyc_Tcexp_expr_err(
_tmp1F3,loc,topt,((_tmp774="expected pointer to function",_tag_dyneither(_tmp774,
sizeof(char),29))),_tag_dyneither(_tmp773,sizeof(void*),0)));}_LL111:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){void*_tmp249=
Cyc_Absyn_exn_typ();Cyc_Tcexp_tcExp(te,(void**)& _tmp249,e);if(!Cyc_Tcutil_coerce_arg(
te,e,_tmp249)){const char*_tmp779;void*_tmp778[2];struct Cyc_String_pa_struct
_tmp777;struct Cyc_String_pa_struct _tmp776;(_tmp776.tag=0,((_tmp776.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp777.tag=0,((_tmp777.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp249)),((_tmp778[0]=& _tmp777,((
_tmp778[1]=& _tmp776,Cyc_Tcutil_terr(loc,((_tmp779="expected %s but found %s",
_tag_dyneither(_tmp779,sizeof(char),25))),_tag_dyneither(_tmp778,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts);static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp24E=t1;struct Cyc_Absyn_PtrInfo _tmp250;void*_tmp251;struct Cyc_Absyn_Tqual
_tmp252;struct Cyc_Absyn_PtrAtts _tmp253;void*_tmp254;union Cyc_Absyn_Constraint*
_tmp255;union Cyc_Absyn_Constraint*_tmp256;union Cyc_Absyn_Constraint*_tmp257;
_LL12E: {struct Cyc_Absyn_PointerType_struct*_tmp24F=(struct Cyc_Absyn_PointerType_struct*)
_tmp24E;if(_tmp24F->tag != 5)goto _LL130;else{_tmp250=_tmp24F->f1;_tmp251=_tmp250.elt_typ;
_tmp252=_tmp250.elt_tq;_tmp253=_tmp250.ptr_atts;_tmp254=_tmp253.rgn;_tmp255=
_tmp253.nullable;_tmp256=_tmp253.bounds;_tmp257=_tmp253.zero_term;}}_LL12F:{void*
_tmp258=Cyc_Tcutil_compress(_tmp251);struct Cyc_Absyn_FnInfo _tmp25A;struct Cyc_List_List*
_tmp25B;struct Cyc_Core_Opt*_tmp25C;void*_tmp25D;struct Cyc_List_List*_tmp25E;int
_tmp25F;struct Cyc_Absyn_VarargInfo*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*
_tmp262;_LL133: {struct Cyc_Absyn_FnType_struct*_tmp259=(struct Cyc_Absyn_FnType_struct*)
_tmp258;if(_tmp259->tag != 10)goto _LL135;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.tvars;
_tmp25C=_tmp25A.effect;_tmp25D=_tmp25A.ret_typ;_tmp25E=_tmp25A.args;_tmp25F=
_tmp25A.c_varargs;_tmp260=_tmp25A.cyc_varargs;_tmp261=_tmp25A.rgn_po;_tmp262=
_tmp25A.attributes;}}_LL134: {struct _RegionHandle*_tmp263=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp25B != 0;(ts=ts->tl,
_tmp25B=_tmp25B->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp25B->hd,& Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);{struct _tuple11*_tmp77C;struct Cyc_List_List*_tmp77B;
instantiation=((_tmp77B=_region_malloc(_tmp263,sizeof(*_tmp77B)),((_tmp77B->hd=((
_tmp77C=_region_malloc(_tmp263,sizeof(*_tmp77C)),((_tmp77C->f1=(struct Cyc_Absyn_Tvar*)
_tmp25B->hd,((_tmp77C->f2=(void*)ts->hd,_tmp77C)))))),((_tmp77B->tl=
instantiation,_tmp77B))))));};}if(ts != 0){const char*_tmp77F;void*_tmp77E;return(
_tmp77E=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp77F="too many type variables in instantiation",
_tag_dyneither(_tmp77F,sizeof(char),41))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp785;struct Cyc_Absyn_FnInfo _tmp784;struct Cyc_Absyn_FnType_struct*
_tmp783;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp263,instantiation,(void*)((
_tmp783=_cycalloc(sizeof(*_tmp783)),((_tmp783[0]=((_tmp785.tag=10,((_tmp785.f1=((
_tmp784.tvars=_tmp25B,((_tmp784.effect=_tmp25C,((_tmp784.ret_typ=_tmp25D,((
_tmp784.args=_tmp25E,((_tmp784.c_varargs=_tmp25F,((_tmp784.cyc_varargs=_tmp260,((
_tmp784.rgn_po=_tmp261,((_tmp784.attributes=_tmp262,_tmp784)))))))))))))))),
_tmp785)))),_tmp783)))));struct Cyc_Absyn_PointerType_struct _tmp78F;struct Cyc_Absyn_PtrAtts
_tmp78E;struct Cyc_Absyn_PtrInfo _tmp78D;struct Cyc_Absyn_PointerType_struct*
_tmp78C;return(void*)((_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C[0]=((
_tmp78F.tag=5,((_tmp78F.f1=((_tmp78D.elt_typ=new_fn_typ,((_tmp78D.elt_tq=_tmp252,((
_tmp78D.ptr_atts=((_tmp78E.rgn=_tmp254,((_tmp78E.nullable=_tmp255,((_tmp78E.bounds=
_tmp256,((_tmp78E.zero_term=_tmp257,((_tmp78E.ptrloc=0,_tmp78E)))))))))),_tmp78D)))))),
_tmp78F)))),_tmp78C))));};}_LL135:;_LL136: goto _LL132;_LL132:;}goto _LL12D;_LL130:;
_LL131: goto _LL12D;_LL12D:;}{const char*_tmp793;void*_tmp792[1];struct Cyc_String_pa_struct
_tmp791;return(_tmp791.tag=0,((_tmp791.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp792[0]=& _tmp791,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp793="expecting polymorphic type but found %s",_tag_dyneither(
_tmp793,sizeof(char),40))),_tag_dyneither(_tmp792,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c);static
void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(
Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(
c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,
loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp798;void*_tmp797[2];struct Cyc_String_pa_struct _tmp796;struct Cyc_String_pa_struct
_tmp795;void*_tmp272=(_tmp795.tag=0,((_tmp795.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp796.tag=0,((_tmp796.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp797[0]=&
_tmp796,((_tmp797[1]=& _tmp795,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp798="cannot cast %s to %s",
_tag_dyneither(_tmp798,sizeof(char),21))),_tag_dyneither(_tmp797,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp272;};}}}{struct _tuple0 _tmp799;struct
_tuple0 _tmp278=(_tmp799.f1=e->r,((_tmp799.f2=Cyc_Tcutil_compress(t),_tmp799)));
void*_tmp279;struct Cyc_Absyn_MallocInfo _tmp27B;int _tmp27C;void*_tmp27D;struct Cyc_Absyn_PtrInfo
_tmp27F;struct Cyc_Absyn_PtrAtts _tmp280;union Cyc_Absyn_Constraint*_tmp281;union
Cyc_Absyn_Constraint*_tmp282;union Cyc_Absyn_Constraint*_tmp283;_LL138: _tmp279=
_tmp278.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp27A=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp279;if(_tmp27A->tag != 34)goto _LL13A;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27B.fat_result;}};
_tmp27D=_tmp278.f2;{struct Cyc_Absyn_PointerType_struct*_tmp27E=(struct Cyc_Absyn_PointerType_struct*)
_tmp27D;if(_tmp27E->tag != 5)goto _LL13A;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27F.ptr_atts;
_tmp281=_tmp280.nullable;_tmp282=_tmp280.bounds;_tmp283=_tmp280.zero_term;}};
_LL139: if((_tmp27C  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp283)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp281)){void*_tmp284=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp282);struct Cyc_Absyn_Exp*_tmp286;_LL13D: {struct Cyc_Absyn_Upper_b_struct*
_tmp285=(struct Cyc_Absyn_Upper_b_struct*)_tmp284;if(_tmp285->tag != 1)goto _LL13F;
else{_tmp286=_tmp285->f1;}}_LL13E: if((Cyc_Evexp_eval_const_uint_exp(_tmp286)).f1
== 1){const char*_tmp79C;void*_tmp79B;(_tmp79B=0,Cyc_Tcutil_warn(loc,((_tmp79C="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp79C,sizeof(char),60))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
goto _LL13C;_LL13F:;_LL140: goto _LL13C;_LL13C:;}goto _LL137;_LL13A:;_LL13B: goto
_LL137;_LL137:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp289=0;
struct Cyc_Absyn_Tqual _tmp28A=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp28B=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp28D;void*_tmp28E;struct Cyc_Absyn_Tqual
_tmp28F;struct Cyc_Absyn_PtrAtts _tmp290;union Cyc_Absyn_Constraint*_tmp291;_LL142: {
struct Cyc_Absyn_PointerType_struct*_tmp28C=(struct Cyc_Absyn_PointerType_struct*)
_tmp28B;if(_tmp28C->tag != 5)goto _LL144;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28D.elt_typ;
_tmp28F=_tmp28D.elt_tq;_tmp290=_tmp28D.ptr_atts;_tmp291=_tmp290.zero_term;}}
_LL143:{void**_tmp79D;_tmp289=((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=
_tmp28E,_tmp79D))));}_tmp28A=_tmp28F;goto _LL141;_LL144:;_LL145: goto _LL141;_LL141:;}{
struct _RegionHandle*_tmp293=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp293,te),_tmp289,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp293,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*_tmp7A0;void*_tmp79F;(
_tmp79F=0,Cyc_Tcutil_terr(e->loc,((_tmp7A0="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp7A0,sizeof(char),46))),_tag_dyneither(_tmp79F,sizeof(void*),0)));}{
void*_tmp296=e->r;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp299;_LL147: {
struct Cyc_Absyn_Subscript_e_struct*_tmp297=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp296;if(_tmp297->tag != 24)goto _LL149;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}
_LL148:{void*_tmp29A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp298->topt))->v);_LL14C: {struct Cyc_Absyn_TupleType_struct*
_tmp29B=(struct Cyc_Absyn_TupleType_struct*)_tmp29A;if(_tmp29B->tag != 11)goto
_LL14E;}_LL14D: goto _LL14B;_LL14E:;_LL14F: e0->r=(Cyc_Absyn_add_exp(_tmp298,
_tmp299,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp298,_tmp299);_LL14B:;}goto _LL146;
_LL149:;_LL14A: goto _LL146;_LL146:;}{void*_tmp29C=e->r;int _tmp29E;int _tmp2A0;
_LL151: {struct Cyc_Absyn_AggrMember_e_struct*_tmp29D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp29C;if(_tmp29D->tag != 22)goto _LL153;else{_tmp29E=_tmp29D->f3;if(_tmp29E != 1)
goto _LL153;}}_LL152: goto _LL154;_LL153: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp29F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp29C;if(_tmp29F->tag != 23)goto
_LL155;else{_tmp2A0=_tmp29F->f3;if(_tmp2A0 != 1)goto _LL155;}}_LL154:{const char*
_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(e->loc,((_tmp7A3="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp7A3,sizeof(char),50))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}
goto _LL150;_LL155:;_LL156: goto _LL150;_LL150:;}{int _tmp2A4;void*_tmp2A5;struct
_tuple12 _tmp2A3=Cyc_Tcutil_addressof_props(te,e);_tmp2A4=_tmp2A3.f1;_tmp2A5=
_tmp2A3.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2A4){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2A5,tq,Cyc_Absyn_false_conref);return t;};};};};}
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(!Cyc_Evexp_okay_szofarg(t)){const char*_tmp7A7;void*_tmp7A6[1];struct Cyc_String_pa_struct
_tmp7A5;(_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7A6[0]=& _tmp7A5,Cyc_Tcutil_terr(loc,((_tmp7A7="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp7A7,sizeof(char),55))),_tag_dyneither(_tmp7A6,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2A9=Cyc_Tcutil_compress(*topt);void*_tmp2AB;_LL158: {
struct Cyc_Absyn_TagType_struct*_tmp2AA=(struct Cyc_Absyn_TagType_struct*)_tmp2A9;
if(_tmp2AA->tag != 20)goto _LL15A;else{_tmp2AB=(void*)_tmp2AA->f1;}}_LL159: {
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp7AA;struct Cyc_Absyn_ValueofType_struct*_tmp7A9;struct Cyc_Absyn_ValueofType_struct*
_tmp2AD=(_tmp7A9=_cycalloc(sizeof(*_tmp7A9)),((_tmp7A9[0]=((_tmp7AA.tag=19,((
_tmp7AA.f1=_tmp2AC,_tmp7AA)))),_tmp7A9)));if(Cyc_Tcutil_unify(_tmp2AB,(void*)
_tmp2AD))return _tmp2A9;goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf);int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n);static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);{void*_tmp2B0=n;
struct _dyneither_ptr*_tmp2B2;unsigned int _tmp2B4;_LL15D: {struct Cyc_Absyn_StructField_struct*
_tmp2B1=(struct Cyc_Absyn_StructField_struct*)_tmp2B0;if(_tmp2B1->tag != 0)goto
_LL15F;else{_tmp2B2=_tmp2B1->f1;}}_LL15E: {int bad_type=1;{void*_tmp2B5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2B7;union Cyc_Absyn_AggrInfoU _tmp2B8;struct Cyc_Absyn_Aggrdecl**
_tmp2B9;struct Cyc_List_List*_tmp2BB;_LL162: {struct Cyc_Absyn_AggrType_struct*
_tmp2B6=(struct Cyc_Absyn_AggrType_struct*)_tmp2B5;if(_tmp2B6->tag != 12)goto
_LL164;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.aggr_info;if((_tmp2B8.KnownAggr).tag
!= 2)goto _LL164;_tmp2B9=(struct Cyc_Absyn_Aggrdecl**)(_tmp2B8.KnownAggr).val;}}
_LL163: if((*_tmp2B9)->impl == 0)goto _LL161;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2B2,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2B9)->impl))->fields)){const char*_tmp7AE;void*_tmp7AD[1];
struct Cyc_String_pa_struct _tmp7AC;(_tmp7AC.tag=0,((_tmp7AC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2B2),((_tmp7AD[0]=& _tmp7AC,Cyc_Tcutil_terr(
loc,((_tmp7AE="no field of struct/union has name %s",_tag_dyneither(_tmp7AE,
sizeof(char),37))),_tag_dyneither(_tmp7AD,sizeof(void*),1)))))));}bad_type=0;
goto _LL161;_LL164: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2BA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2B5;if(_tmp2BA->tag != 13)goto _LL166;else{_tmp2BB=_tmp2BA->f2;}}_LL165: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2B2,_tmp2BB)){const char*_tmp7B2;void*_tmp7B1[1];struct Cyc_String_pa_struct
_tmp7B0;(_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2B2),((_tmp7B1[0]=& _tmp7B0,Cyc_Tcutil_terr(loc,((_tmp7B2="no field of struct/union has name %s",
_tag_dyneither(_tmp7B2,sizeof(char),37))),_tag_dyneither(_tmp7B1,sizeof(void*),1)))))));}
bad_type=0;goto _LL161;_LL166:;_LL167: goto _LL161;_LL161:;}if(bad_type){const char*
_tmp7B6;void*_tmp7B5[1];struct Cyc_String_pa_struct _tmp7B4;(_tmp7B4.tag=0,((
_tmp7B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp7B5[0]=& _tmp7B4,Cyc_Tcutil_terr(loc,((_tmp7B6="%s is not a known struct/union type",
_tag_dyneither(_tmp7B6,sizeof(char),36))),_tag_dyneither(_tmp7B5,sizeof(void*),1)))))));}
goto _LL15C;}_LL15F: {struct Cyc_Absyn_TupleIndex_struct*_tmp2B3=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2B0;if(_tmp2B3->tag != 1)goto _LL15C;else{_tmp2B4=_tmp2B3->f1;}}_LL160: {int
bad_type=1;{void*_tmp2C5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2C7;
union Cyc_Absyn_AggrInfoU _tmp2C8;struct Cyc_Absyn_Aggrdecl**_tmp2C9;struct Cyc_List_List*
_tmp2CB;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_DatatypeFieldInfo _tmp2CF;
union Cyc_Absyn_DatatypeFieldInfoU _tmp2D0;struct _tuple2 _tmp2D1;struct Cyc_Absyn_Datatypefield*
_tmp2D2;_LL169: {struct Cyc_Absyn_AggrType_struct*_tmp2C6=(struct Cyc_Absyn_AggrType_struct*)
_tmp2C5;if(_tmp2C6->tag != 12)goto _LL16B;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C7.aggr_info;
if((_tmp2C8.KnownAggr).tag != 2)goto _LL16B;_tmp2C9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2C8.KnownAggr).val;}}_LL16A: if((*_tmp2C9)->impl == 0)goto _LL168;_tmp2CB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2C9)->impl))->fields;goto _LL16C;
_LL16B: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2CA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C5;if(_tmp2CA->tag != 13)goto _LL16D;else{_tmp2CB=_tmp2CA->f2;}}_LL16C: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CB)<= _tmp2B4){const char*
_tmp7BB;void*_tmp7BA[2];struct Cyc_Int_pa_struct _tmp7B9;struct Cyc_Int_pa_struct
_tmp7B8;(_tmp7B8.tag=1,((_tmp7B8.f1=(unsigned long)((int)_tmp2B4),((_tmp7B9.tag=
1,((_tmp7B9.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2CB),((_tmp7BA[0]=& _tmp7B9,((_tmp7BA[1]=& _tmp7B8,Cyc_Tcutil_terr(loc,((
_tmp7BB="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp7BB,
sizeof(char),46))),_tag_dyneither(_tmp7BA,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL168;_LL16D: {struct Cyc_Absyn_TupleType_struct*_tmp2CC=(struct Cyc_Absyn_TupleType_struct*)
_tmp2C5;if(_tmp2CC->tag != 11)goto _LL16F;else{_tmp2CD=_tmp2CC->f1;}}_LL16E: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CD)<= _tmp2B4){const char*
_tmp7C0;void*_tmp7BF[2];struct Cyc_Int_pa_struct _tmp7BE;struct Cyc_Int_pa_struct
_tmp7BD;(_tmp7BD.tag=1,((_tmp7BD.f1=(unsigned long)((int)_tmp2B4),((_tmp7BE.tag=
1,((_tmp7BE.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2CD),((_tmp7BF[0]=& _tmp7BE,((_tmp7BF[1]=& _tmp7BD,Cyc_Tcutil_terr(loc,((
_tmp7C0="tuple has too few components: %d <= %d",_tag_dyneither(_tmp7C0,sizeof(
char),39))),_tag_dyneither(_tmp7BF,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL168;_LL16F: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2CE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2C5;if(_tmp2CE->tag != 4)goto _LL171;else{_tmp2CF=_tmp2CE->f1;_tmp2D0=_tmp2CF.field_info;
if((_tmp2D0.KnownDatatypefield).tag != 2)goto _LL171;_tmp2D1=(struct _tuple2)(
_tmp2D0.KnownDatatypefield).val;_tmp2D2=_tmp2D1.f2;}}_LL170: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2D2->typs)< _tmp2B4){const char*_tmp7C5;
void*_tmp7C4[2];struct Cyc_Int_pa_struct _tmp7C3;struct Cyc_Int_pa_struct _tmp7C2;(
_tmp7C2.tag=1,((_tmp7C2.f1=(unsigned long)((int)_tmp2B4),((_tmp7C3.tag=1,((
_tmp7C3.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2D2->typs),((_tmp7C4[0]=& _tmp7C3,((_tmp7C4[1]=& _tmp7C2,Cyc_Tcutil_terr(loc,((
_tmp7C5="datatype field has too few components: %d < %d",_tag_dyneither(_tmp7C5,
sizeof(char),47))),_tag_dyneither(_tmp7C4,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL168;_LL171:;_LL172: goto _LL168;_LL168:;}if(bad_type){const char*_tmp7C9;
void*_tmp7C8[1];struct Cyc_String_pa_struct _tmp7C7;(_tmp7C7.tag=0,((_tmp7C7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7C8[
0]=& _tmp7C7,Cyc_Tcutil_terr(loc,((_tmp7C9="%s is not a known type",
_tag_dyneither(_tmp7C9,sizeof(char),23))),_tag_dyneither(_tmp7C8,sizeof(void*),1)))))));}
goto _LL15C;}_LL15C:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp2E2=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp2E3=Cyc_Tcenv_clear_notreadctxt(_tmp2E2,
te_orig);Cyc_Tcexp_tcExp(_tmp2E3,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp2E4=t;struct Cyc_Absyn_PtrInfo
_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_PtrAtts _tmp2E9;void*_tmp2EA;union Cyc_Absyn_Constraint*
_tmp2EB;union Cyc_Absyn_Constraint*_tmp2EC;_LL174: {struct Cyc_Absyn_Evar_struct*
_tmp2E5=(struct Cyc_Absyn_Evar_struct*)_tmp2E4;if(_tmp2E5->tag != 1)goto _LL176;}
_LL175: {struct Cyc_List_List*_tmp2ED=Cyc_Tcenv_lookup_type_vars(_tmp2E3);struct
Cyc_Core_Opt*_tmp7CA;void*_tmp2EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp7CA=_cycalloc(sizeof(*_tmp7CA)),((_tmp7CA->v=_tmp2ED,_tmp7CA)))));struct Cyc_Core_Opt*
_tmp7CB;void*_tmp2EF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp7CB=_cycalloc(sizeof(*_tmp7CB)),((_tmp7CB->v=_tmp2ED,_tmp7CB)))));union Cyc_Absyn_Constraint*
_tmp2F0=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp2F1=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp7CC;struct Cyc_Absyn_PtrAtts _tmp2F2=(_tmp7CC.rgn=_tmp2EF,((_tmp7CC.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp7CC.bounds=
_tmp2F0,((_tmp7CC.zero_term=_tmp2F1,((_tmp7CC.ptrloc=0,_tmp7CC)))))))));struct
Cyc_Absyn_PointerType_struct _tmp7D2;struct Cyc_Absyn_PtrInfo _tmp7D1;struct Cyc_Absyn_PointerType_struct*
_tmp7D0;struct Cyc_Absyn_PointerType_struct*_tmp2F3=(_tmp7D0=_cycalloc(sizeof(*
_tmp7D0)),((_tmp7D0[0]=((_tmp7D2.tag=5,((_tmp7D2.f1=((_tmp7D1.elt_typ=_tmp2EE,((
_tmp7D1.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp7D1.ptr_atts=_tmp2F2,_tmp7D1)))))),
_tmp7D2)))),_tmp7D0)));Cyc_Tcutil_unify(t,(void*)_tmp2F3);_tmp2E8=_tmp2EE;
_tmp2EA=_tmp2EF;_tmp2EB=_tmp2F0;_tmp2EC=_tmp2F1;goto _LL177;}_LL176: {struct Cyc_Absyn_PointerType_struct*
_tmp2E6=(struct Cyc_Absyn_PointerType_struct*)_tmp2E4;if(_tmp2E6->tag != 5)goto
_LL178;else{_tmp2E7=_tmp2E6->f1;_tmp2E8=_tmp2E7.elt_typ;_tmp2E9=_tmp2E7.ptr_atts;
_tmp2EA=_tmp2E9.rgn;_tmp2EB=_tmp2E9.bounds;_tmp2EC=_tmp2E9.zero_term;}}_LL177:
Cyc_Tcenv_check_rgn_accessible(_tmp2E3,loc,_tmp2EA);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2EB);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2E8),& Cyc_Tcutil_tmk)){
const char*_tmp7D5;void*_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_terr(loc,((_tmp7D5="can't dereference abstract pointer type",
_tag_dyneither(_tmp7D5,sizeof(char),40))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}
return _tmp2E8;_LL178:;_LL179: {const char*_tmp7D9;void*_tmp7D8[1];struct Cyc_String_pa_struct
_tmp7D7;return(_tmp7D7.tag=0,((_tmp7D7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7D8[0]=& _tmp7D7,Cyc_Tcexp_expr_err(
_tmp2E3,loc,topt,((_tmp7D9="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp7D9,sizeof(char),39))),_tag_dyneither(_tmp7D8,sizeof(void*),1)))))));}
_LL173:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read);static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
Cyc_Tcexp_tcExpNoPromote(te,0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*
_tmp2FF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp301;union Cyc_Absyn_AggrInfoU _tmp302;struct Cyc_Absyn_Aggrdecl**
_tmp303;struct Cyc_Absyn_Aggrdecl*_tmp304;struct Cyc_List_List*_tmp305;enum Cyc_Absyn_AggrKind
_tmp307;struct Cyc_List_List*_tmp308;_LL17B: {struct Cyc_Absyn_AggrType_struct*
_tmp300=(struct Cyc_Absyn_AggrType_struct*)_tmp2FF;if(_tmp300->tag != 12)goto
_LL17D;else{_tmp301=_tmp300->f1;_tmp302=_tmp301.aggr_info;if((_tmp302.KnownAggr).tag
!= 2)goto _LL17D;_tmp303=(struct Cyc_Absyn_Aggrdecl**)(_tmp302.KnownAggr).val;
_tmp304=*_tmp303;_tmp305=_tmp301.targs;}}_LL17C: {struct Cyc_Absyn_Aggrfield*
_tmp30D=Cyc_Absyn_lookup_decl_field(_tmp304,f);if(_tmp30D == 0){const char*_tmp7DE;
void*_tmp7DD[2];struct Cyc_String_pa_struct _tmp7DC;struct Cyc_String_pa_struct
_tmp7DB;return(_tmp7DB.tag=0,((_tmp7DB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp304->name)),((_tmp7DD[0]=& _tmp7DC,((
_tmp7DD[1]=& _tmp7DB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DE="type %s has no %s field",
_tag_dyneither(_tmp7DE,sizeof(char),24))),_tag_dyneither(_tmp7DD,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp312=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp313=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp312,_tmp312,_tmp304->tvs,_tmp305);t2=Cyc_Tcutil_rsubstitute(_tmp312,_tmp313,
_tmp30D->type);if(((_tmp304->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp304->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp7E2;void*_tmp7E1[1];struct Cyc_String_pa_struct _tmp7E0;return(
_tmp7E0.tag=0,((_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp7E1[0]=& _tmp7E0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E2="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp7E2,sizeof(char),56))),_tag_dyneither(_tmp7E1,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp7E6;void*_tmp7E5[1];struct Cyc_String_pa_struct _tmp7E4;return(
_tmp7E4.tag=0,((_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp7E5[0]=& _tmp7E4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E6="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp7E6,sizeof(char),81))),_tag_dyneither(_tmp7E5,sizeof(void*),1)))))));}}
return t2;};}_LL17D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp306=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2FF;if(_tmp306->tag != 13)goto _LL17F;else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;}}
_LL17E: {struct Cyc_Absyn_Aggrfield*_tmp31A=Cyc_Absyn_lookup_field(_tmp308,f);if(
_tmp31A == 0){const char*_tmp7EA;void*_tmp7E9[1];struct Cyc_String_pa_struct _tmp7E8;
return(_tmp7E8.tag=0,((_tmp7E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp7E9[0]=& _tmp7E8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7EA="type has no %s field",
_tag_dyneither(_tmp7EA,sizeof(char),21))),_tag_dyneither(_tmp7E9,sizeof(void*),1)))))));}
if((_tmp307 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp31A->type)){const char*_tmp7EE;void*_tmp7ED[1];struct Cyc_String_pa_struct
_tmp7EC;return(_tmp7EC.tag=0,((_tmp7EC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp7ED[0]=& _tmp7EC,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp7EE="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp7EE,sizeof(char),56))),_tag_dyneither(_tmp7ED,sizeof(void*),1)))))));}return
_tmp31A->type;}_LL17F:{struct Cyc_Absyn_ArrayType_struct*_tmp309=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2FF;if(_tmp309->tag != 9)goto _LL181;}{const char*_tmp7EF;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp7EF="size",_tag_dyneither(_tmp7EF,sizeof(char),5))))
== 0))goto _LL181;};_LL180: goto _LL182;_LL181:{struct Cyc_Absyn_PointerType_struct*
_tmp30B=(struct Cyc_Absyn_PointerType_struct*)_tmp2FF;if(_tmp30B->tag != 5)goto
_LL183;}{const char*_tmp7F0;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp7F0="size",
_tag_dyneither(_tmp7F0,sizeof(char),5))))== 0))goto _LL183;};_LL182:{const char*
_tmp7F4;void*_tmp7F3[1];struct Cyc_String_pa_struct _tmp7F2;(_tmp7F2.tag=0,((
_tmp7F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp7F3[0]=& _tmp7F2,Cyc_Tcutil_warn(e->loc,((_tmp7F4="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp7F4,sizeof(char),54))),_tag_dyneither(_tmp7F3,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp7FA;struct Cyc_List_List*_tmp7F9;struct Cyc_Absyn_Primop_e_struct*
_tmp7F8;outer_e->r=(void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((
_tmp7FA.tag=3,((_tmp7FA.f1=Cyc_Absyn_Numelts,((_tmp7FA.f2=((_tmp7F9=_cycalloc(
sizeof(*_tmp7F9)),((_tmp7F9->hd=e,((_tmp7F9->tl=0,_tmp7F9)))))),_tmp7FA)))))),
_tmp7F8))));}return Cyc_Absyn_uint_typ;_LL183:;_LL184: {const char*_tmp7FB;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp7FB="size",_tag_dyneither(_tmp7FB,sizeof(char),5))))
== 0){const char*_tmp7FF;void*_tmp7FE[1];struct Cyc_String_pa_struct _tmp7FD;return(
_tmp7FD.tag=0,((_tmp7FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp7FE[0]=& _tmp7FD,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7FF="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp7FF,sizeof(char),44))),_tag_dyneither(_tmp7FE,sizeof(void*),1)))))));}
else{const char*_tmp803;void*_tmp802[1];struct Cyc_String_pa_struct _tmp801;return(
_tmp801.tag=0,((_tmp801.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp802[0]=& _tmp801,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp803="expecting struct or union, found %s",_tag_dyneither(
_tmp803,sizeof(char),36))),_tag_dyneither(_tmp802,sizeof(void*),1)))))));}}
_LL17A:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read);static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp32E=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp32E,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp32F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp331;void*_tmp332;struct Cyc_Absyn_PtrAtts _tmp333;void*_tmp334;union Cyc_Absyn_Constraint*
_tmp335;union Cyc_Absyn_Constraint*_tmp336;_LL186: {struct Cyc_Absyn_PointerType_struct*
_tmp330=(struct Cyc_Absyn_PointerType_struct*)_tmp32F;if(_tmp330->tag != 5)goto
_LL188;else{_tmp331=_tmp330->f1;_tmp332=_tmp331.elt_typ;_tmp333=_tmp331.ptr_atts;
_tmp334=_tmp333.rgn;_tmp335=_tmp333.bounds;_tmp336=_tmp333.zero_term;}}_LL187:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp335);{void*_tmp337=Cyc_Tcutil_compress(
_tmp332);struct Cyc_Absyn_AggrInfo _tmp339;union Cyc_Absyn_AggrInfoU _tmp33A;struct
Cyc_Absyn_Aggrdecl**_tmp33B;struct Cyc_Absyn_Aggrdecl*_tmp33C;struct Cyc_List_List*
_tmp33D;enum Cyc_Absyn_AggrKind _tmp33F;struct Cyc_List_List*_tmp340;_LL18B: {
struct Cyc_Absyn_AggrType_struct*_tmp338=(struct Cyc_Absyn_AggrType_struct*)
_tmp337;if(_tmp338->tag != 12)goto _LL18D;else{_tmp339=_tmp338->f1;_tmp33A=_tmp339.aggr_info;
if((_tmp33A.KnownAggr).tag != 2)goto _LL18D;_tmp33B=(struct Cyc_Absyn_Aggrdecl**)(
_tmp33A.KnownAggr).val;_tmp33C=*_tmp33B;_tmp33D=_tmp339.targs;}}_LL18C: {struct
Cyc_Absyn_Aggrfield*_tmp341=Cyc_Absyn_lookup_decl_field(_tmp33C,f);if(_tmp341 == 
0){const char*_tmp808;void*_tmp807[2];struct Cyc_String_pa_struct _tmp806;struct Cyc_String_pa_struct
_tmp805;return(_tmp805.tag=0,((_tmp805.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp33C->name)),((_tmp807[0]=& _tmp806,((
_tmp807[1]=& _tmp805,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp808="type %s has no %s field",
_tag_dyneither(_tmp808,sizeof(char),24))),_tag_dyneither(_tmp807,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33C->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp346=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp346,_tmp346,_tmp33C->tvs,_tmp33D);t3=Cyc_Tcutil_rsubstitute(_tmp346,_tmp347,
_tmp341->type);}if(((_tmp33C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp33C->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp80C;void*_tmp80B[1];struct Cyc_String_pa_struct _tmp80A;return(
_tmp80A.tag=0,((_tmp80A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp80B[0]=& _tmp80A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80C="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp80C,sizeof(char),56))),_tag_dyneither(_tmp80B,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33C->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp810;void*_tmp80F[1];struct Cyc_String_pa_struct _tmp80E;return(
_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp80F[0]=& _tmp80E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp810="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp810,sizeof(char),72))),_tag_dyneither(_tmp80F,sizeof(void*),1)))))));}}
return t3;};}_LL18D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp33E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp337;if(_tmp33E->tag != 13)goto _LL18F;else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33E->f2;}}
_LL18E: {struct Cyc_Absyn_Aggrfield*_tmp34E=Cyc_Absyn_lookup_field(_tmp340,f);if(
_tmp34E == 0){const char*_tmp814;void*_tmp813[1];struct Cyc_String_pa_struct _tmp812;
return(_tmp812.tag=0,((_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp813[0]=& _tmp812,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp814="type has no %s field",
_tag_dyneither(_tmp814,sizeof(char),21))),_tag_dyneither(_tmp813,sizeof(void*),1)))))));}
if((_tmp33F == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp34E->type)){const char*_tmp818;void*_tmp817[1];struct Cyc_String_pa_struct
_tmp816;return(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp817[0]=& _tmp816,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp818="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp818,sizeof(char),56))),_tag_dyneither(_tmp817,sizeof(void*),1)))))));}return
_tmp34E->type;}_LL18F:;_LL190: goto _LL18A;_LL18A:;}goto _LL185;_LL188:;_LL189: goto
_LL185;_LL185:;}{const char*_tmp81C;void*_tmp81B[1];struct Cyc_String_pa_struct
_tmp81A;return(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp81B[0]=& _tmp81A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp81C="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp81C,sizeof(char),44))),_tag_dyneither(_tmp81B,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp359;int _tmp35A;struct _tuple13 _tmp358=
Cyc_Evexp_eval_const_uint_exp(index);_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;if(!
_tmp35A){const char*_tmp81F;void*_tmp81E;return(_tmp81E=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp81F="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp81F,sizeof(char),47))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}{struct
_handler_cons _tmp35D;_push_handler(& _tmp35D);{int _tmp35F=0;if(setjmp(_tmp35D.handler))
_tmp35F=1;if(!_tmp35F){{void*_tmp360=(*((struct _tuple17*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(ts,(int)_tmp359)).f2;_npop_handler(0);return _tmp360;};
_pop_handler();}else{void*_tmp35E=(void*)_exn_thrown;void*_tmp362=_tmp35E;_LL192: {
struct Cyc_List_Nth_struct*_tmp363=(struct Cyc_List_Nth_struct*)_tmp362;if(_tmp363->tag
!= Cyc_List_Nth)goto _LL194;}_LL193: {const char*_tmp824;void*_tmp823[2];struct Cyc_Int_pa_struct
_tmp822;struct Cyc_Int_pa_struct _tmp821;return(_tmp821.tag=1,((_tmp821.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp822.tag=
1,((_tmp822.f1=(unsigned long)((int)_tmp359),((_tmp823[0]=& _tmp822,((_tmp823[1]=&
_tmp821,Cyc_Tcexp_expr_err(te,loc,0,((_tmp824="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp824,sizeof(char),41))),_tag_dyneither(_tmp823,sizeof(void*),2)))))))))))));}
_LL194:;_LL195:(void)_throw(_tmp362);_LL191:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp368=Cyc_Tcenv_get_fnrgn(te_orig);
struct Cyc_Tcenv_Tenv*_tmp369=Cyc_Tcenv_clear_notreadctxt(_tmp368,te_orig);Cyc_Tcexp_tcExp(
_tmp369,0,e1);Cyc_Tcexp_tcExp(_tmp369,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
_tmp369,e2)){const char*_tmp828;void*_tmp827[1];struct Cyc_String_pa_struct _tmp826;
return(_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp827[0]=& _tmp826,Cyc_Tcexp_expr_err(_tmp369,e2->loc,
topt,((_tmp828="expecting int subscript, found %s",_tag_dyneither(_tmp828,
sizeof(char),34))),_tag_dyneither(_tmp827,sizeof(void*),1)))))));}{void*_tmp36D=
t1;struct Cyc_Absyn_PtrInfo _tmp36F;void*_tmp370;struct Cyc_Absyn_Tqual _tmp371;
struct Cyc_Absyn_PtrAtts _tmp372;void*_tmp373;union Cyc_Absyn_Constraint*_tmp374;
union Cyc_Absyn_Constraint*_tmp375;struct Cyc_List_List*_tmp377;_LL197: {struct Cyc_Absyn_PointerType_struct*
_tmp36E=(struct Cyc_Absyn_PointerType_struct*)_tmp36D;if(_tmp36E->tag != 5)goto
_LL199;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_typ;_tmp371=_tmp36F.elt_tq;
_tmp372=_tmp36F.ptr_atts;_tmp373=_tmp372.rgn;_tmp374=_tmp372.bounds;_tmp375=
_tmp372.zero_term;}}_LL198: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp375)){int emit_warning=1;{void*_tmp378=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp374);struct Cyc_Absyn_Exp*
_tmp37A;_LL19E: {struct Cyc_Absyn_Upper_b_struct*_tmp379=(struct Cyc_Absyn_Upper_b_struct*)
_tmp378;if(_tmp379->tag != 1)goto _LL1A0;else{_tmp37A=_tmp379->f1;}}_LL19F: if(Cyc_Tcutil_is_const_exp(
_tmp369,e2)){unsigned int _tmp37D;int _tmp37E;struct _tuple13 _tmp37C=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;if(_tmp37E){unsigned int _tmp380;int
_tmp381;struct _tuple13 _tmp37F=Cyc_Evexp_eval_const_uint_exp(_tmp37A);_tmp380=
_tmp37F.f1;_tmp381=_tmp37F.f2;if(_tmp381  && _tmp380 > _tmp37D)emit_warning=0;}}
goto _LL19D;_LL1A0: {struct Cyc_Absyn_DynEither_b_struct*_tmp37B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp378;if(_tmp37B->tag != 0)goto _LL19D;}_LL1A1: emit_warning=0;goto _LL19D;_LL19D:;}
if(emit_warning){const char*_tmp82B;void*_tmp82A;(_tmp82A=0,Cyc_Tcutil_warn(e2->loc,((
_tmp82B="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp82B,sizeof(char),63))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp369,e2)){unsigned int _tmp385;int _tmp386;
struct _tuple13 _tmp384=Cyc_Evexp_eval_const_uint_exp(e2);_tmp385=_tmp384.f1;
_tmp386=_tmp384.f2;if(_tmp386)Cyc_Tcutil_check_bound(loc,_tmp385,_tmp374);}else{
if(Cyc_Tcutil_is_bound_one(_tmp374) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp375)){const char*_tmp82E;void*_tmp82D;(_tmp82D=0,
Cyc_Tcutil_warn(e1->loc,((_tmp82E="subscript applied to pointer to one object",
_tag_dyneither(_tmp82E,sizeof(char),43))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp374);}}Cyc_Tcenv_check_rgn_accessible(
_tmp369,loc,_tmp373);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp370),& Cyc_Tcutil_tmk)){
const char*_tmp831;void*_tmp830;(_tmp830=0,Cyc_Tcutil_terr(e1->loc,((_tmp831="can't subscript an abstract pointer",
_tag_dyneither(_tmp831,sizeof(char),36))),_tag_dyneither(_tmp830,sizeof(void*),0)));}
return _tmp370;_LL199: {struct Cyc_Absyn_TupleType_struct*_tmp376=(struct Cyc_Absyn_TupleType_struct*)
_tmp36D;if(_tmp376->tag != 11)goto _LL19B;else{_tmp377=_tmp376->f1;}}_LL19A: return
Cyc_Tcexp_ithTupleType(_tmp369,loc,_tmp377,e2);_LL19B:;_LL19C: {const char*
_tmp835;void*_tmp834[1];struct Cyc_String_pa_struct _tmp833;return(_tmp833.tag=0,((
_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp834[0]=& _tmp833,Cyc_Tcexp_expr_err(_tmp369,loc,topt,((_tmp835="subscript applied to %s",
_tag_dyneither(_tmp835,sizeof(char),24))),_tag_dyneither(_tmp834,sizeof(void*),1)))))));}
_LL196:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;
struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp38E=Cyc_Tcutil_compress(*topt);
struct Cyc_List_List*_tmp390;_LL1A3: {struct Cyc_Absyn_TupleType_struct*_tmp38F=(
struct Cyc_Absyn_TupleType_struct*)_tmp38E;if(_tmp38F->tag != 11)goto _LL1A5;else{
_tmp390=_tmp38F->f1;}}_LL1A4: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp390)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))goto _LL1A2;for(0;
es != 0;(es=es->tl,_tmp390=_tmp390->tl)){void*_tmp391=(*((struct _tuple17*)((
struct Cyc_List_List*)_check_null(_tmp390))->hd)).f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp391,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(te,(
struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp390->hd)).f2);{struct
_tuple17*_tmp838;struct Cyc_List_List*_tmp837;fields=((_tmp837=_cycalloc(sizeof(*
_tmp837)),((_tmp837->hd=((_tmp838=_cycalloc(sizeof(*_tmp838)),((_tmp838->f1=(*((
struct _tuple17*)_tmp390->hd)).f1,((_tmp838->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp838)))))),((_tmp837->tl=
fields,_tmp837))))));};}done=1;goto _LL1A2;_LL1A5:;_LL1A6: goto _LL1A2;_LL1A2:;}if(
!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple17*_tmp83B;struct Cyc_List_List*_tmp83A;fields=((_tmp83A=
_cycalloc(sizeof(*_tmp83A)),((_tmp83A->hd=((_tmp83B=_cycalloc(sizeof(*_tmp83B)),((
_tmp83B->f1=Cyc_Absyn_empty_tqual(0),((_tmp83B->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp83B)))))),((_tmp83A->tl=
fields,_tmp83A))))));};}{struct Cyc_Absyn_TupleType_struct _tmp83E;struct Cyc_Absyn_TupleType_struct*
_tmp83D;return(void*)((_tmp83D=_cycalloc(sizeof(*_tmp83D)),((_tmp83D[0]=((
_tmp83E.tag=11,((_tmp83E.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp83E)))),_tmp83D))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){const char*_tmp841;void*
_tmp840;return(_tmp840=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp841="tcCompoundLit",
_tag_dyneither(_tmp841,sizeof(char),14))),_tag_dyneither(_tmp840,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des);static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp39A=Cyc_Tcenv_get_fnrgn(
te);{int _tmp39B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp39A,(struct Cyc_Absyn_Exp*(*)(
struct _tuple16*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp844;struct Cyc_Absyn_Const_e_struct*_tmp843;struct Cyc_Absyn_Const_e_struct*
_tmp39C=(_tmp843=_cycalloc(sizeof(*_tmp843)),((_tmp843[0]=((_tmp844.tag=0,((
_tmp844.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp39B),_tmp844)))),_tmp843)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp39C,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp39D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp39B - 1);if(!Cyc_Tcutil_is_zero(_tmp39D)){const char*_tmp847;
void*_tmp846;(_tmp846=0,Cyc_Tcutil_terr(_tmp39D->loc,((_tmp847="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp847,sizeof(char),45))),_tag_dyneither(_tmp846,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp848;sz_exp->topt=((_tmp848=_cycalloc(sizeof(*_tmp848)),((
_tmp848->v=Cyc_Absyn_uint_typ,_tmp848))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp84C;void*_tmp84B[1];struct Cyc_String_pa_struct _tmp84A;(
_tmp84A.tag=0,((_tmp84A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp84B[0]=& _tmp84A,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp84C="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp84C,sizeof(char),53))),_tag_dyneither(_tmp84B,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple16*)des->hd)).f1;if(ds != 0){void*_tmp3A6=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3A9;_LL1A8: {struct Cyc_Absyn_FieldName_struct*_tmp3A7=(struct Cyc_Absyn_FieldName_struct*)
_tmp3A6;if(_tmp3A7->tag != 1)goto _LL1AA;}_LL1A9:{const char*_tmp84F;void*_tmp84E;(
_tmp84E=0,Cyc_Tcutil_terr(loc,((_tmp84F="only array index designators are supported",
_tag_dyneither(_tmp84F,sizeof(char),43))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}
goto _LL1A7;_LL1AA: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A8=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3A6;if(_tmp3A8->tag != 0)goto _LL1A7;else{_tmp3A9=_tmp3A8->f1;}}_LL1AB: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3A9);{unsigned int _tmp3AD;int _tmp3AE;struct _tuple13 _tmp3AC=Cyc_Evexp_eval_const_uint_exp(
_tmp3A9);_tmp3AD=_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;if(!_tmp3AE){const char*_tmp852;
void*_tmp851;(_tmp851=0,Cyc_Tcutil_terr(_tmp3A9->loc,((_tmp852="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp852,sizeof(char),47))),_tag_dyneither(_tmp851,sizeof(void*),0)));}
else{if(_tmp3AD != offset){const char*_tmp857;void*_tmp856[2];struct Cyc_Int_pa_struct
_tmp855;struct Cyc_Int_pa_struct _tmp854;(_tmp854.tag=1,((_tmp854.f1=(
unsigned long)((int)_tmp3AD),((_tmp855.tag=1,((_tmp855.f1=(unsigned long)offset,((
_tmp856[0]=& _tmp855,((_tmp856[1]=& _tmp854,Cyc_Tcutil_terr(_tmp3A9->loc,((_tmp857="expecting index designator %d but found %d",
_tag_dyneither(_tmp857,sizeof(char),43))),_tag_dyneither(_tmp856,sizeof(void*),2)))))))))))));}}
goto _LL1A7;};_LL1A7:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp3B5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1AD: {struct Cyc_Absyn_TagType_struct*
_tmp3B6=(struct Cyc_Absyn_TagType_struct*)_tmp3B5;if(_tmp3B6->tag != 20)goto _LL1AF;}
_LL1AE: goto _LL1AC;_LL1AF:;_LL1B0: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const
char*_tmp85B;void*_tmp85A[1];struct Cyc_String_pa_struct _tmp859;(_tmp859.tag=0,((
_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp85A[0]=& _tmp859,
Cyc_Tcutil_terr(bound->loc,((_tmp85B="expecting unsigned int, found %s",
_tag_dyneither(_tmp85B,sizeof(char),33))),_tag_dyneither(_tmp85A,sizeof(void*),1)))))));}
_LL1AC:;}if(!(vd->tq).real_const){const char*_tmp85E;void*_tmp85D;(_tmp85D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp85E="comprehension index variable is not declared const!",_tag_dyneither(
_tmp85E,sizeof(char),52))),_tag_dyneither(_tmp85D,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3BC=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3BC,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3BC,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3BC,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3BD=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp3BE=0;struct Cyc_Absyn_Tqual*_tmp3BF=0;union Cyc_Absyn_Constraint**
_tmp3C0=0;if(topt != 0){void*_tmp3C1=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3C3;struct Cyc_Absyn_ArrayInfo _tmp3C5;void*_tmp3C6;void**_tmp3C7;struct Cyc_Absyn_Tqual
_tmp3C8;struct Cyc_Absyn_Tqual*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3CA;union Cyc_Absyn_Constraint*
_tmp3CB;union Cyc_Absyn_Constraint**_tmp3CC;_LL1B2: {struct Cyc_Absyn_PointerType_struct*
_tmp3C2=(struct Cyc_Absyn_PointerType_struct*)_tmp3C1;if(_tmp3C2->tag != 5)goto
_LL1B4;else{_tmp3C3=_tmp3C2->f1;}}_LL1B3: pinfo=_tmp3C3;_tmp3BE=(void**)& pinfo.elt_typ;
_tmp3BF=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp3C0=(union Cyc_Absyn_Constraint**)&(
pinfo.ptr_atts).zero_term;goto _LL1B1;_LL1B4: {struct Cyc_Absyn_ArrayType_struct*
_tmp3C4=(struct Cyc_Absyn_ArrayType_struct*)_tmp3C1;if(_tmp3C4->tag != 9)goto
_LL1B6;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C5.elt_type;_tmp3C7=(void**)&(
_tmp3C4->f1).elt_type;_tmp3C8=_tmp3C5.tq;_tmp3C9=(struct Cyc_Absyn_Tqual*)&(
_tmp3C4->f1).tq;_tmp3CA=_tmp3C5.num_elts;_tmp3CB=_tmp3C5.zero_term;_tmp3CC=(
union Cyc_Absyn_Constraint**)&(_tmp3C4->f1).zero_term;}}_LL1B5: _tmp3BE=(void**)
_tmp3C7;_tmp3BF=(struct Cyc_Absyn_Tqual*)_tmp3C9;_tmp3C0=(union Cyc_Absyn_Constraint**)
_tmp3CC;goto _LL1B1;_LL1B6:;_LL1B7: goto _LL1B1;_LL1B1:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3BD,_tmp3BE,body);if(_tmp3BD->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3BD,
bound)){const char*_tmp861;void*_tmp860;(_tmp860=0,Cyc_Tcutil_terr(bound->loc,((
_tmp861="bound is not constant",_tag_dyneither(_tmp861,sizeof(char),22))),
_tag_dyneither(_tmp860,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp3BD,
body)){const char*_tmp864;void*_tmp863;(_tmp863=0,Cyc_Tcutil_terr(bound->loc,((
_tmp864="body is not constant",_tag_dyneither(_tmp864,sizeof(char),21))),
_tag_dyneither(_tmp863,sizeof(void*),0)));}}if(_tmp3C0 != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3C0)){struct Cyc_Absyn_Exp*
_tmp3D1=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp865;_tmp3D1->topt=((
_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865->v=Cyc_Absyn_uint_typ,_tmp865))));}
bound=Cyc_Absyn_add_exp(bound,_tmp3D1,0);{struct Cyc_Core_Opt*_tmp866;bound->topt=((
_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866->v=Cyc_Absyn_uint_typ,_tmp866))));}*
is_zero_term=1;}{void*_tmp3D4=Cyc_Absyn_array_typ(t,_tmp3BF == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3BF,(struct Cyc_Absyn_Exp*)bound,_tmp3C0 == 0?Cyc_Absyn_false_conref:*
_tmp3C0,0);return _tmp3D4;};};};};}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt);static void*Cyc_Tcexp_tcAggregate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp3D5;_push_handler(& _tmp3D5);{
int _tmp3D7=0;if(setjmp(_tmp3D5.handler))_tmp3D7=1;if(!_tmp3D7){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp3D6=(void*)_exn_thrown;void*_tmp3D9=
_tmp3D6;_LL1B9: {struct Cyc_Dict_Absent_struct*_tmp3DA=(struct Cyc_Dict_Absent_struct*)
_tmp3D9;if(_tmp3DA->tag != Cyc_Dict_Absent)goto _LL1BB;}_LL1BA:{const char*_tmp86A;
void*_tmp869[1];struct Cyc_String_pa_struct _tmp868;(_tmp868.tag=0,((_tmp868.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((
_tmp869[0]=& _tmp868,Cyc_Tcutil_terr(loc,((_tmp86A="unbound struct/union name %s",
_tag_dyneither(_tmp86A,sizeof(char),29))),_tag_dyneither(_tmp869,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1BB:;_LL1BC:(void)_throw(
_tmp3D9);_LL1B8:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp872;void*_tmp871[1];const char*_tmp870;const char*_tmp86F;
struct Cyc_String_pa_struct _tmp86E;(_tmp86E.tag=0,((_tmp86E.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp86F="struct",_tag_dyneither(
_tmp86F,sizeof(char),7)):((_tmp870="union",_tag_dyneither(_tmp870,sizeof(char),6)))),((
_tmp871[0]=& _tmp86E,Cyc_Tcutil_terr(loc,((_tmp872="can't construct abstract %s",
_tag_dyneither(_tmp872,sizeof(char),28))),_tag_dyneither(_tmp871,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp3E3=Cyc_Tcenv_get_fnrgn(te);struct _tuple10 _tmp873;struct
_tuple10 _tmp3E4=(_tmp873.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp873.f2=_tmp3E3,
_tmp873)));struct Cyc_List_List*_tmp3E5=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E3,Cyc_Tcutil_r_make_inst_var,&
_tmp3E4,ad->tvs);struct Cyc_List_List*_tmp3E6=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E3,Cyc_Tcutil_r_make_inst_var,&
_tmp3E4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3E5);struct Cyc_List_List*_tmp3E8=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3E6);struct Cyc_List_List*_tmp3E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp3E3,_tmp3E5,_tmp3E6);struct Cyc_Absyn_AggrType_struct _tmp87D;struct Cyc_Absyn_Aggrdecl**
_tmp87C;struct Cyc_Absyn_AggrInfo _tmp87B;struct Cyc_Absyn_AggrType_struct*_tmp87A;
struct Cyc_Absyn_AggrType_struct*_tmp3EA=(_tmp87A=_cycalloc(sizeof(*_tmp87A)),((
_tmp87A[0]=((_tmp87D.tag=12,((_tmp87D.f1=((_tmp87B.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=ad,_tmp87C))))),((_tmp87B.targs=
_tmp3E7,_tmp87B)))),_tmp87D)))),_tmp87A)));struct Cyc_List_List*_tmp3EB=*ts;
struct Cyc_List_List*_tmp3EC=_tmp3E8;while(_tmp3EB != 0  && _tmp3EC != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp3EB->hd);Cyc_Tcutil_unify((
void*)_tmp3EB->hd,(void*)_tmp3EC->hd);_tmp3EB=_tmp3EB->tl;_tmp3EC=_tmp3EC->tl;}
if(_tmp3EB != 0){const char*_tmp880;void*_tmp87F;(_tmp87F=0,Cyc_Tcutil_terr(loc,((
_tmp880="too many explicit witness types",_tag_dyneither(_tmp880,sizeof(char),32))),
_tag_dyneither(_tmp87F,sizeof(void*),0)));}*ts=_tmp3E8;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp3EA,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp3E3,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple18 _tmp3F0;struct Cyc_Absyn_Aggrfield*
_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;struct _tuple18*_tmp3EF=(struct _tuple18*)
fields->hd;_tmp3F0=*_tmp3EF;_tmp3F1=_tmp3F0.f1;_tmp3F2=_tmp3F0.f2;{void*_tmp3F3=
Cyc_Tcutil_rsubstitute(_tmp3E3,_tmp3E9,_tmp3F1->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp3F3,_tmp3F2);if(!Cyc_Tcutil_coerce_arg(te,_tmp3F2,_tmp3F3)){{
const char*_tmp88C;void*_tmp88B[5];struct Cyc_String_pa_struct _tmp88A;const char*
_tmp889;const char*_tmp888;struct Cyc_String_pa_struct _tmp887;struct Cyc_String_pa_struct
_tmp886;struct Cyc_String_pa_struct _tmp885;struct Cyc_String_pa_struct _tmp884;(
_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v)),((_tmp885.tag=0,((
_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp3F3)),((_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp887.tag=0,((_tmp887.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp888="struct",
_tag_dyneither(_tmp888,sizeof(char),7)):((_tmp889="union",_tag_dyneither(_tmp889,
sizeof(char),6)))),((_tmp88A.tag=0,((_tmp88A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3F1->name),((_tmp88B[0]=& _tmp88A,((_tmp88B[1]=& _tmp887,((
_tmp88B[2]=& _tmp886,((_tmp88B[3]=& _tmp885,((_tmp88B[4]=& _tmp884,Cyc_Tcutil_terr(
_tmp3F2->loc,((_tmp88C="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp88C,sizeof(char),40))),_tag_dyneither(_tmp88B,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp3FD=0;{struct Cyc_List_List*
_tmp3FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp3FE != 0;_tmp3FE=_tmp3FE->tl){struct _tuple0*_tmp88F;struct Cyc_List_List*
_tmp88E;_tmp3FD=((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E->hd=((_tmp88F=
_cycalloc(sizeof(*_tmp88F)),((_tmp88F->f1=Cyc_Tcutil_rsubstitute(_tmp3E3,_tmp3E9,(*((
struct _tuple0*)_tmp3FE->hd)).f1),((_tmp88F->f2=Cyc_Tcutil_rsubstitute(_tmp3E3,
_tmp3E9,(*((struct _tuple0*)_tmp3FE->hd)).f2),_tmp88F)))))),((_tmp88E->tl=_tmp3FD,
_tmp88E))))));}}_tmp3FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp3FD);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp3FD);return(void*)_tmp3EA;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args);static void*Cyc_Tcexp_tcAnonStruct(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){struct _RegionHandle*_tmp406=Cyc_Tcenv_get_fnrgn(te);{void*_tmp407=Cyc_Tcutil_compress(
ts);enum Cyc_Absyn_AggrKind _tmp409;struct Cyc_List_List*_tmp40A;_LL1BE: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp408=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp407;if(_tmp408->tag != 13)goto _LL1C0;else{_tmp409=_tmp408->f1;_tmp40A=_tmp408->f2;}}
_LL1BF: if(_tmp409 == Cyc_Absyn_UnionA){const char*_tmp892;void*_tmp891;(_tmp891=0,
Cyc_Tcutil_terr(loc,((_tmp892="expecting struct but found union",_tag_dyneither(
_tmp892,sizeof(char),33))),_tag_dyneither(_tmp891,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp406,loc,args,Cyc_Absyn_StructA,
_tmp40A);for(0;fields != 0;fields=fields->tl){struct _tuple18 _tmp40E;struct Cyc_Absyn_Aggrfield*
_tmp40F;struct Cyc_Absyn_Exp*_tmp410;struct _tuple18*_tmp40D=(struct _tuple18*)
fields->hd;_tmp40E=*_tmp40D;_tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp40F->type,_tmp410);if(!Cyc_Tcutil_coerce_arg(te,_tmp410,_tmp40F->type)){{
const char*_tmp898;void*_tmp897[3];struct Cyc_String_pa_struct _tmp896;struct Cyc_String_pa_struct
_tmp895;struct Cyc_String_pa_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp410->topt))->v)),((_tmp895.tag=0,((_tmp895.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp40F->type)),((
_tmp896.tag=0,((_tmp896.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp40F->name),((
_tmp897[0]=& _tmp896,((_tmp897[1]=& _tmp895,((_tmp897[2]=& _tmp894,Cyc_Tcutil_terr(
_tmp410->loc,((_tmp898="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp898,sizeof(char),41))),_tag_dyneither(_tmp897,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1BD;};_LL1C0:;_LL1C1: {const char*_tmp89B;
void*_tmp89A;(_tmp89A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp89B="tcAnonStruct: wrong type",_tag_dyneither(_tmp89B,
sizeof(char),25))),_tag_dyneither(_tmp89A,sizeof(void*),0)));}_LL1BD:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf);static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){struct
_RegionHandle*_tmp418=Cyc_Tcenv_get_fnrgn(te);struct _tuple10 _tmp89C;struct
_tuple10 _tmp419=(_tmp89C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp89C.f2=_tmp418,
_tmp89C)));struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp418,Cyc_Tcutil_r_make_inst_var,&
_tmp419,tud->tvs);struct Cyc_List_List*_tmp41B=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp41A);struct Cyc_Absyn_DatatypeFieldType_struct _tmp8A2;struct Cyc_Absyn_DatatypeFieldInfo
_tmp8A1;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp8A0;void*res=(void*)((
_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0[0]=((_tmp8A2.tag=4,((_tmp8A2.f1=((
_tmp8A1.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8A1.targs=_tmp41B,
_tmp8A1)))),_tmp8A2)))),_tmp8A0))));if(topt != 0){void*_tmp41C=Cyc_Tcutil_compress(*
topt);_LL1C3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp41D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp41C;if(_tmp41D->tag != 4)goto _LL1C5;}_LL1C4: Cyc_Tcutil_unify(*topt,res);goto
_LL1C2;_LL1C5:;_LL1C6: goto _LL1C2;_LL1C2:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp418,_tmp41A,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp8AA;void*_tmp8A9[3];struct Cyc_String_pa_struct _tmp8A8;struct Cyc_String_pa_struct
_tmp8A7;const char*_tmp8A6;struct Cyc_String_pa_struct _tmp8A5;(_tmp8A5.tag=0,((
_tmp8A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct
_dyneither_ptr)((_tmp8A6="?",_tag_dyneither(_tmp8A6,sizeof(char),2))): Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((_tmp8A7.tag=0,((
_tmp8A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp8A9[0]=& _tmp8A8,((_tmp8A9[1]=& _tmp8A7,((_tmp8A9[2]=& _tmp8A5,
Cyc_Tcutil_terr(e->loc,((_tmp8AA="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp8AA,sizeof(char),82))),_tag_dyneither(_tmp8A9,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp8AE;void*_tmp8AD[1];
struct Cyc_String_pa_struct _tmp8AC;return(_tmp8AC.tag=0,((_tmp8AC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp8AD[0]=& _tmp8AC,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp8AE="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp8AE,sizeof(char),47))),_tag_dyneither(_tmp8AD,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp8B2;void*_tmp8B1[1];struct Cyc_String_pa_struct _tmp8B0;return(
_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp8B1[0]=& _tmp8B0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B2="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp8B2,sizeof(char),46))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp42E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp435;
struct Cyc_Absyn_PtrAtts _tmp436;union Cyc_Absyn_Constraint*_tmp437;struct Cyc_Absyn_ArrayInfo
_tmp43C;void*_tmp43D;struct Cyc_List_List*_tmp440;struct Cyc_Absyn_AggrInfo _tmp442;
union Cyc_Absyn_AggrInfoU _tmp443;struct Cyc_List_List*_tmp444;struct Cyc_List_List*
_tmp448;_LL1C8: {struct Cyc_Absyn_VoidType_struct*_tmp42F=(struct Cyc_Absyn_VoidType_struct*)
_tmp42E;if(_tmp42F->tag != 0)goto _LL1CA;}_LL1C9: return 1;_LL1CA: {struct Cyc_Absyn_Evar_struct*
_tmp430=(struct Cyc_Absyn_Evar_struct*)_tmp42E;if(_tmp430->tag != 1)goto _LL1CC;}
_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_VarType_struct*_tmp431=(struct Cyc_Absyn_VarType_struct*)
_tmp42E;if(_tmp431->tag != 2)goto _LL1CE;}_LL1CD: goto _LL1CF;_LL1CE: {struct Cyc_Absyn_DatatypeType_struct*
_tmp432=(struct Cyc_Absyn_DatatypeType_struct*)_tmp42E;if(_tmp432->tag != 3)goto
_LL1D0;}_LL1CF: goto _LL1D1;_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp433=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp42E;if(_tmp433->tag != 4)
goto _LL1D2;}_LL1D1: return 0;_LL1D2: {struct Cyc_Absyn_PointerType_struct*_tmp434=(
struct Cyc_Absyn_PointerType_struct*)_tmp42E;if(_tmp434->tag != 5)goto _LL1D4;else{
_tmp435=_tmp434->f1;_tmp436=_tmp435.ptr_atts;_tmp437=_tmp436.nullable;}}_LL1D3:
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp437);
_LL1D4: {struct Cyc_Absyn_IntType_struct*_tmp438=(struct Cyc_Absyn_IntType_struct*)
_tmp42E;if(_tmp438->tag != 6)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_FloatType_struct*
_tmp439=(struct Cyc_Absyn_FloatType_struct*)_tmp42E;if(_tmp439->tag != 7)goto
_LL1D8;}_LL1D7: goto _LL1D9;_LL1D8: {struct Cyc_Absyn_DoubleType_struct*_tmp43A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp42E;if(_tmp43A->tag != 8)goto _LL1DA;}
_LL1D9: return 1;_LL1DA: {struct Cyc_Absyn_ArrayType_struct*_tmp43B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp42E;if(_tmp43B->tag != 9)goto _LL1DC;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43C.elt_type;}}
_LL1DB: return Cyc_Tcexp_zeroable_type(_tmp43D);_LL1DC: {struct Cyc_Absyn_FnType_struct*
_tmp43E=(struct Cyc_Absyn_FnType_struct*)_tmp42E;if(_tmp43E->tag != 10)goto _LL1DE;}
_LL1DD: return 0;_LL1DE: {struct Cyc_Absyn_TupleType_struct*_tmp43F=(struct Cyc_Absyn_TupleType_struct*)
_tmp42E;if(_tmp43F->tag != 11)goto _LL1E0;else{_tmp440=_tmp43F->f1;}}_LL1DF: for(0;(
unsigned int)_tmp440;_tmp440=_tmp440->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple17*)_tmp440->hd)).f2))return 0;}return 1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*
_tmp441=(struct Cyc_Absyn_AggrType_struct*)_tmp42E;if(_tmp441->tag != 12)goto
_LL1E2;else{_tmp442=_tmp441->f1;_tmp443=_tmp442.aggr_info;_tmp444=_tmp442.targs;}}
_LL1E1: {struct Cyc_Absyn_Aggrdecl*_tmp453=Cyc_Absyn_get_known_aggrdecl(_tmp443);
if(_tmp453->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp453->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp454=_new_region("r");struct _RegionHandle*r=&
_tmp454;_push_region(r);{struct Cyc_List_List*_tmp455=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp453->tvs,_tmp444);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp453->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp455,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp456=0;_npop_handler(0);return _tmp456;}}}{int _tmp457=1;_npop_handler(0);
return _tmp457;};};_pop_region(r);};}_LL1E2: {struct Cyc_Absyn_EnumType_struct*
_tmp445=(struct Cyc_Absyn_EnumType_struct*)_tmp42E;if(_tmp445->tag != 14)goto
_LL1E4;}_LL1E3: return 1;_LL1E4: {struct Cyc_Absyn_TagType_struct*_tmp446=(struct
Cyc_Absyn_TagType_struct*)_tmp42E;if(_tmp446->tag != 20)goto _LL1E6;}_LL1E5: return
1;_LL1E6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp447=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42E;if(_tmp447->tag != 13)goto _LL1E8;else{_tmp448=_tmp447->f2;}}_LL1E7: for(0;
_tmp448 != 0;_tmp448=_tmp448->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp448->hd)->type))return 0;}return 1;_LL1E8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp449=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp42E;if(_tmp449->tag != 15)goto
_LL1EA;}_LL1E9: return 1;_LL1EA: {struct Cyc_Absyn_TypedefType_struct*_tmp44A=(
struct Cyc_Absyn_TypedefType_struct*)_tmp42E;if(_tmp44A->tag != 18)goto _LL1EC;}
_LL1EB: return 0;_LL1EC: {struct Cyc_Absyn_DynRgnType_struct*_tmp44B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp42E;if(_tmp44B->tag != 17)goto _LL1EE;}_LL1ED: return 0;_LL1EE: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp44C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp42E;if(_tmp44C->tag != 16)goto
_LL1F0;}_LL1EF: return 0;_LL1F0: {struct Cyc_Absyn_ValueofType_struct*_tmp44D=(
struct Cyc_Absyn_ValueofType_struct*)_tmp42E;if(_tmp44D->tag != 19)goto _LL1F2;}
_LL1F1: goto _LL1F3;_LL1F2: {struct Cyc_Absyn_HeapRgn_struct*_tmp44E=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp42E;if(_tmp44E->tag != 21)goto _LL1F4;}_LL1F3: goto _LL1F5;_LL1F4: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp44F=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp42E;if(_tmp44F->tag != 22)goto
_LL1F6;}_LL1F5: goto _LL1F7;_LL1F6: {struct Cyc_Absyn_AccessEff_struct*_tmp450=(
struct Cyc_Absyn_AccessEff_struct*)_tmp42E;if(_tmp450->tag != 23)goto _LL1F8;}
_LL1F7: goto _LL1F9;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*_tmp451=(struct Cyc_Absyn_JoinEff_struct*)
_tmp42E;if(_tmp451->tag != 24)goto _LL1FA;}_LL1F9: goto _LL1FB;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct*
_tmp452=(struct Cyc_Absyn_RgnsEff_struct*)_tmp42E;if(_tmp452->tag != 25)goto _LL1C7;}
_LL1FB: {const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_String_pa_struct _tmp8B4;(
_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp8B5[0]=& _tmp8B4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp8B6="bad type `%s' in zeroable type",_tag_dyneither(
_tmp8B6,sizeof(char),31))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))));}_LL1C7:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp45B=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp45D;void*_tmp45E;_LL1FD: {struct Cyc_Absyn_PointerType_struct*
_tmp45C=(struct Cyc_Absyn_PointerType_struct*)_tmp45B;if(_tmp45C->tag != 5)goto
_LL1FF;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45D.elt_typ;}}_LL1FE: Cyc_Tcutil_unify(
_tmp45E,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))
return;goto _LL1FC;_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}{const char*_tmp8BF;void*
_tmp8BE[2];const char*_tmp8BD;const char*_tmp8BC;struct Cyc_String_pa_struct _tmp8BB;
struct Cyc_String_pa_struct _tmp8BA;(_tmp8BA.tag=0,((_tmp8BA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8BB.tag=0,((
_tmp8BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp8BC="calloc",_tag_dyneither(_tmp8BC,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp8BD="malloc",_tag_dyneither(_tmp8BD,sizeof(char),7))))),((
_tmp8BE[0]=& _tmp8BB,((_tmp8BE[1]=& _tmp8BA,Cyc_Tcutil_terr(loc,((_tmp8BF="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp8BF,sizeof(char),60))),_tag_dyneither(_tmp8BE,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn);static void*Cyc_Tcexp_mallocRgn(void*rgn){
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique:
_LL201: return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL202: return(void*)& Cyc_Absyn_HeapRgn_val;}}
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp8C2;struct Cyc_Absyn_RgnHandleType_struct*
_tmp8C1;void*expected_type=(void*)((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((
_tmp8C1[0]=((_tmp8C2.tag=16,((_tmp8C2.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp8C2)))),_tmp8C1))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp465=Cyc_Tcutil_compress(handle_type);void*_tmp467;_LL205: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp466=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp465;if(_tmp466->tag != 16)goto
_LL207;else{_tmp467=(void*)_tmp466->f1;}}_LL206: rgn=_tmp467;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL204;_LL207:;_LL208:{const char*_tmp8C6;void*_tmp8C5[1];struct
Cyc_String_pa_struct _tmp8C4;(_tmp8C4.tag=0,((_tmp8C4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp8C5[0]=& _tmp8C4,
Cyc_Tcutil_terr(ropt->loc,((_tmp8C6="expecting region_t type but found %s",
_tag_dyneither(_tmp8C6,sizeof(char),37))),_tag_dyneither(_tmp8C5,sizeof(void*),1)))))));}
goto _LL204;_LL204:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp8C9;void*_tmp8C8;(
_tmp8C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8C9="calloc with empty type",_tag_dyneither(_tmp8C9,sizeof(char),23))),
_tag_dyneither(_tmp8C8,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=0;}
else{void*_tmp46F=(*e)->r;void*_tmp471;enum Cyc_Absyn_Primop _tmp473;struct Cyc_List_List*
_tmp474;struct Cyc_List_List _tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_List_List*
_tmp477;struct Cyc_List_List _tmp478;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_List_List*
_tmp47A;_LL20A: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp470=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp46F;if(_tmp470->tag != 18)goto _LL20C;else{_tmp471=(void*)_tmp470->f1;}}_LL20B:
elt_type=_tmp471;{void**_tmp8CA;*t=(void**)((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((
_tmp8CA[0]=elt_type,_tmp8CA))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL209;_LL20C: {struct Cyc_Absyn_Primop_e_struct*
_tmp472=(struct Cyc_Absyn_Primop_e_struct*)_tmp46F;if(_tmp472->tag != 3)goto _LL20E;
else{_tmp473=_tmp472->f1;if(_tmp473 != Cyc_Absyn_Times)goto _LL20E;_tmp474=_tmp472->f2;
if(_tmp474 == 0)goto _LL20E;_tmp475=*_tmp474;_tmp476=(struct Cyc_Absyn_Exp*)_tmp475.hd;
_tmp477=_tmp475.tl;if(_tmp477 == 0)goto _LL20E;_tmp478=*_tmp477;_tmp479=(struct Cyc_Absyn_Exp*)
_tmp478.hd;_tmp47A=_tmp478.tl;if(_tmp47A != 0)goto _LL20E;}}_LL20D:{struct _tuple0
_tmp8CB;struct _tuple0 _tmp47D=(_tmp8CB.f1=_tmp476->r,((_tmp8CB.f2=_tmp479->r,
_tmp8CB)));void*_tmp47E;void*_tmp480;void*_tmp481;void*_tmp483;_LL211: _tmp47E=
_tmp47D.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp47F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp47E;if(_tmp47F->tag != 18)goto _LL213;else{_tmp480=(void*)_tmp47F->f1;}};
_LL212: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp480);elt_type=_tmp480;{void**
_tmp8CC;*t=(void**)((_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=elt_type,
_tmp8CC))));}num_elts=_tmp479;one_elt=0;goto _LL210;_LL213: _tmp481=_tmp47D.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp482=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp481;if(_tmp482->tag != 18)goto _LL215;else{_tmp483=(void*)_tmp482->f1;}};
_LL214: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp483);elt_type=_tmp483;{void**
_tmp8CD;*t=(void**)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=elt_type,
_tmp8CD))));}num_elts=_tmp476;one_elt=0;goto _LL210;_LL215:;_LL216: goto No_sizeof;
_LL210:;}goto _LL209;_LL20E:;_LL20F: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**
_tmp8CE;*t=(void**)((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=elt_type,
_tmp8CE))));}num_elts=*e;one_elt=0;goto _LL209;_LL209:;}*e=num_elts;*is_fat=!
one_elt;{void*_tmp487=elt_type;struct Cyc_Absyn_AggrInfo _tmp489;union Cyc_Absyn_AggrInfoU
_tmp48A;struct Cyc_Absyn_Aggrdecl**_tmp48B;struct Cyc_Absyn_Aggrdecl*_tmp48C;
_LL218: {struct Cyc_Absyn_AggrType_struct*_tmp488=(struct Cyc_Absyn_AggrType_struct*)
_tmp487;if(_tmp488->tag != 12)goto _LL21A;else{_tmp489=_tmp488->f1;_tmp48A=_tmp489.aggr_info;
if((_tmp48A.KnownAggr).tag != 2)goto _LL21A;_tmp48B=(struct Cyc_Absyn_Aggrdecl**)(
_tmp48A.KnownAggr).val;_tmp48C=*_tmp48B;}}_LL219: if(_tmp48C->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48C->impl))->exist_vars != 0){const
char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(loc,((_tmp8D1="malloc with existential types not yet implemented",
_tag_dyneither(_tmp8D1,sizeof(char),50))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}
goto _LL217;_LL21A:;_LL21B: goto _LL217;_LL217:;}{void*(*_tmp48F)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp490=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp491=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp493;struct Cyc_Absyn_PtrAtts
_tmp494;union Cyc_Absyn_Constraint*_tmp495;union Cyc_Absyn_Constraint*_tmp496;
union Cyc_Absyn_Constraint*_tmp497;_LL21D: {struct Cyc_Absyn_PointerType_struct*
_tmp492=(struct Cyc_Absyn_PointerType_struct*)_tmp491;if(_tmp492->tag != 5)goto
_LL21F;else{_tmp493=_tmp492->f1;_tmp494=_tmp493.ptr_atts;_tmp495=_tmp494.nullable;
_tmp496=_tmp494.bounds;_tmp497=_tmp494.zero_term;}}_LL21E: _tmp490=_tmp497;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp495))
_tmp48F=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp497) && !(*is_calloc)){{const char*_tmp8D4;void*_tmp8D3;(_tmp8D3=0,Cyc_Tcutil_warn(
loc,((_tmp8D4="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp8D4,sizeof(char),55))),_tag_dyneither(_tmp8D3,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp49A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp496);struct Cyc_Absyn_Exp*_tmp49D;_LL222: {struct Cyc_Absyn_DynEither_b_struct*
_tmp49B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp49A;if(_tmp49B->tag != 0)goto
_LL224;}_LL223: goto _LL221;_LL224:{struct Cyc_Absyn_Upper_b_struct*_tmp49C=(struct
Cyc_Absyn_Upper_b_struct*)_tmp49A;if(_tmp49C->tag != 1)goto _LL226;else{_tmp49D=
_tmp49C->f1;}}if(!(!one_elt))goto _LL226;_LL225: {int _tmp49E=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp49E  && Cyc_Evexp_same_const_exp(_tmp49D,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp496),_tmp490);}{void*_tmp49F=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4A1;_LL229: {struct Cyc_Absyn_TagType_struct*_tmp4A0=(struct Cyc_Absyn_TagType_struct*)
_tmp49F;if(_tmp4A0->tag != 20)goto _LL22B;else{_tmp4A1=(void*)_tmp4A0->f1;}}_LL22A: {
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4A1,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4A2,
_tmp49D)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp496),_tmp490);}
goto _LL228;}_LL22B:;_LL22C: goto _LL228;_LL228:;}goto _LL221;}_LL226:;_LL227: goto
_LL221;_LL221:;}goto _LL21C;_LL21F:;_LL220: goto _LL21C;_LL21C:;}if(!one_elt)
_tmp48F=Cyc_Absyn_dyneither_typ;return _tmp48F(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp490);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp4A3=Cyc_Tcutil_compress(t1);_LL22E: {struct
Cyc_Absyn_ArrayType_struct*_tmp4A4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4A3;
if(_tmp4A4->tag != 9)goto _LL230;}_LL22F:{const char*_tmp8D7;void*_tmp8D6;(_tmp8D6=
0,Cyc_Tcutil_terr(loc,((_tmp8D7="cannot assign to an array",_tag_dyneither(
_tmp8D7,sizeof(char),26))),_tag_dyneither(_tmp8D6,sizeof(void*),0)));}goto _LL22D;
_LL230:;_LL231: goto _LL22D;_LL22D:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp8DA;void*_tmp8D9;(_tmp8D9=0,Cyc_Tcutil_terr(loc,((
_tmp8DA="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp8DA,sizeof(char),58))),_tag_dyneither(_tmp8D9,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp8DD;void*_tmp8DC;return(_tmp8DC=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp8DD="swap non-lvalue",_tag_dyneither(_tmp8DD,sizeof(char),
16))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp8E0;void*_tmp8DF;return(_tmp8DF=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp8E0="swap non-lvalue",_tag_dyneither(_tmp8E0,sizeof(char),16))),
_tag_dyneither(_tmp8DF,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp8E3;void*_tmp8E2;return(_tmp8E2=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp8E3="swap value in zeroterm array",_tag_dyneither(_tmp8E3,
sizeof(char),29))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp8E6;void*_tmp8E5;return(_tmp8E5=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp8E6="swap value in zeroterm array",_tag_dyneither(_tmp8E6,
sizeof(char),29))),_tag_dyneither(_tmp8E5,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp8EB;void*_tmp8EA[2];struct Cyc_String_pa_struct _tmp8E9;struct Cyc_String_pa_struct
_tmp8E8;void*_tmp4B1=(_tmp8E8.tag=0,((_tmp8E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp8E9.tag=0,((_tmp8E9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8EA[0]=&
_tmp8E9,((_tmp8EA[1]=& _tmp8E8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8EB="type mismatch: %s != %s",
_tag_dyneither(_tmp8EB,sizeof(char),24))),_tag_dyneither(_tmp8EA,sizeof(void*),2)))))))))))));
return _tmp4B1;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*s){struct
_RegionHandle*_tmp4B6=Cyc_Tcenv_get_fnrgn(te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp4B6,te,s),s,1);Cyc_Tcexp_in_stmt_exp=
old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp4B7=s->r;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Stmt*
_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Absyn_Decl*_tmp4BE;struct Cyc_Absyn_Stmt*
_tmp4BF;_LL233: {struct Cyc_Absyn_Exp_s_struct*_tmp4B8=(struct Cyc_Absyn_Exp_s_struct*)
_tmp4B7;if(_tmp4B8->tag != 1)goto _LL235;else{_tmp4B9=_tmp4B8->f1;}}_LL234: {void*
_tmp4C0=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4B9->topt))->v;if(!Cyc_Tcutil_unify(
_tmp4C0,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*
_tmp8EF;void*_tmp8EE[1];struct Cyc_String_pa_struct _tmp8ED;(_tmp8ED.tag=0,((
_tmp8ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4C0)),((_tmp8EE[0]=& _tmp8ED,Cyc_Tcutil_terr(loc,((_tmp8EF="statement expression returns type %s",
_tag_dyneither(_tmp8EF,sizeof(char),37))),_tag_dyneither(_tmp8EE,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}return _tmp4C0;}_LL235: {struct Cyc_Absyn_Seq_s_struct*
_tmp4BA=(struct Cyc_Absyn_Seq_s_struct*)_tmp4B7;if(_tmp4BA->tag != 2)goto _LL237;
else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;}}_LL236: s=_tmp4BC;continue;_LL237: {
struct Cyc_Absyn_Decl_s_struct*_tmp4BD=(struct Cyc_Absyn_Decl_s_struct*)_tmp4B7;
if(_tmp4BD->tag != 12)goto _LL239;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BD->f2;}}
_LL238: s=_tmp4BF;continue;_LL239:;_LL23A: {const char*_tmp8F2;void*_tmp8F1;return(
_tmp8F1=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F2="statement expression must end with expression",
_tag_dyneither(_tmp8F2,sizeof(char),46))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}
_LL232:;}}static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f);static void*Cyc_Tcexp_tcTagcheck(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{
void*_tmp4C6=t;struct Cyc_Absyn_AggrInfo _tmp4C8;union Cyc_Absyn_AggrInfoU _tmp4C9;
struct Cyc_Absyn_Aggrdecl**_tmp4CA;struct Cyc_Absyn_Aggrdecl*_tmp4CB;_LL23C: {
struct Cyc_Absyn_AggrType_struct*_tmp4C7=(struct Cyc_Absyn_AggrType_struct*)
_tmp4C6;if(_tmp4C7->tag != 12)goto _LL23E;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C8.aggr_info;
if((_tmp4C9.KnownAggr).tag != 2)goto _LL23E;_tmp4CA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4C9.KnownAggr).val;_tmp4CB=*_tmp4CA;}}_LL23D: if((_tmp4CB->kind == Cyc_Absyn_UnionA
 && _tmp4CB->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->tagged)
goto _LL23B;goto _LL23F;_LL23E:;_LL23F:{const char*_tmp8F6;void*_tmp8F5[1];struct
Cyc_String_pa_struct _tmp8F4;(_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8F5[0]=& _tmp8F4,Cyc_Tcutil_terr(
loc,((_tmp8F6="expecting @tagged union but found %s",_tag_dyneither(_tmp8F6,
sizeof(char),37))),_tag_dyneither(_tmp8F5,sizeof(void*),1)))))));}goto _LL23B;
_LL23B:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1);static void*Cyc_Tcexp_tcNew(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp8F9;struct Cyc_Absyn_RgnHandleType_struct*
_tmp8F8;void*expected_type=(void*)((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((
_tmp8F8[0]=((_tmp8F9.tag=16,((_tmp8F9.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp8F9)))),_tmp8F8))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp4CF=Cyc_Tcutil_compress(handle_type);void*_tmp4D1;_LL241: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp4D0=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4CF;if(_tmp4D0->tag != 16)goto _LL243;else{_tmp4D1=(void*)_tmp4D0->f1;}}_LL242:
rgn=_tmp4D1;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL240;_LL243:;_LL244:{
const char*_tmp8FD;void*_tmp8FC[1];struct Cyc_String_pa_struct _tmp8FB;(_tmp8FB.tag=
0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp8FC[0]=& _tmp8FB,Cyc_Tcutil_terr(rgn_handle->loc,((_tmp8FD="expecting region_t type but found %s",
_tag_dyneither(_tmp8FD,sizeof(char),37))),_tag_dyneither(_tmp8FC,sizeof(void*),1)))))));}
goto _LL240;_LL240:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
void*_tmp4D7=e1->r;struct Cyc_Core_Opt*_tmp4DA;struct Cyc_List_List*_tmp4DB;struct
Cyc_List_List*_tmp4DD;union Cyc_Absyn_Cnst _tmp4DF;struct _dyneither_ptr _tmp4E0;
_LL246: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4D8=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4D7;if(_tmp4D8->tag != 28)goto _LL248;}_LL247: {void*_tmp4E1=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp4E2=Cyc_Tcutil_compress(_tmp4E1);struct Cyc_Absyn_ArrayInfo
_tmp4E4;void*_tmp4E5;struct Cyc_Absyn_Tqual _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;
union Cyc_Absyn_Constraint*_tmp4E8;_LL251: {struct Cyc_Absyn_ArrayType_struct*
_tmp4E3=(struct Cyc_Absyn_ArrayType_struct*)_tmp4E2;if(_tmp4E3->tag != 9)goto
_LL253;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E4.elt_type;_tmp4E6=_tmp4E4.tq;
_tmp4E7=_tmp4E4.num_elts;_tmp4E8=_tmp4E4.zero_term;}}_LL252: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp4E7);void*b;{void*_tmp4E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp4EB;_LL256: {
struct Cyc_Absyn_TagType_struct*_tmp4EA=(struct Cyc_Absyn_TagType_struct*)_tmp4E9;
if(_tmp4EA->tag != 20)goto _LL258;else{_tmp4EB=(void*)_tmp4EA->f1;}}_LL257:{struct
Cyc_Absyn_Upper_b_struct _tmp900;struct Cyc_Absyn_Upper_b_struct*_tmp8FF;b=(void*)((
_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=1,((_tmp900.f1=
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4EB,0),0,Cyc_Absyn_No_coercion,
0),_tmp900)))),_tmp8FF))));}goto _LL255;_LL258:;_LL259: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp903;struct Cyc_Absyn_Upper_b_struct*
_tmp902;b=(void*)((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=
1,((_tmp903.f1=bnd,_tmp903)))),_tmp902))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL255:;}{struct Cyc_Absyn_PointerType_struct _tmp90D;struct Cyc_Absyn_PtrAtts
_tmp90C;struct Cyc_Absyn_PtrInfo _tmp90B;struct Cyc_Absyn_PointerType_struct*
_tmp90A;void*res_typ=(void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((
_tmp90D.tag=5,((_tmp90D.f1=((_tmp90B.elt_typ=_tmp4E5,((_tmp90B.elt_tq=_tmp4E6,((
_tmp90B.ptr_atts=((_tmp90C.rgn=rgn,((_tmp90C.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp90C.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp90C.zero_term=_tmp4E8,((_tmp90C.ptrloc=0,
_tmp90C)))))))))),_tmp90B)))))),_tmp90D)))),_tmp90A))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp90E;e->topt=((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->v=res_typ,
_tmp90E))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL253:;_LL254: {const char*_tmp911;void*_tmp910;(
_tmp910=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp911="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp911,
sizeof(char),45))),_tag_dyneither(_tmp910,sizeof(void*),0)));}_LL250:;}_LL248: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp4D9=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp4D7;if(_tmp4D9->tag != 36)goto _LL24A;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}}
_LL249:{struct Cyc_Absyn_Array_e_struct _tmp914;struct Cyc_Absyn_Array_e_struct*
_tmp913;e1->r=(void*)((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913[0]=((
_tmp914.tag=27,((_tmp914.f1=_tmp4DB,_tmp914)))),_tmp913))));}_tmp4DD=_tmp4DB;
goto _LL24B;_LL24A: {struct Cyc_Absyn_Array_e_struct*_tmp4DC=(struct Cyc_Absyn_Array_e_struct*)
_tmp4D7;if(_tmp4DC->tag != 27)goto _LL24C;else{_tmp4DD=_tmp4DC->f1;}}_LL24B: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp4F9=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp4FB;void*_tmp4FC;void**_tmp4FD;struct Cyc_Absyn_Tqual
_tmp4FE;struct Cyc_Absyn_PtrAtts _tmp4FF;union Cyc_Absyn_Constraint*_tmp500;_LL25B: {
struct Cyc_Absyn_PointerType_struct*_tmp4FA=(struct Cyc_Absyn_PointerType_struct*)
_tmp4F9;if(_tmp4FA->tag != 5)goto _LL25D;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FB.elt_typ;
_tmp4FD=(void**)&(_tmp4FA->f1).elt_typ;_tmp4FE=_tmp4FB.elt_tq;_tmp4FF=_tmp4FB.ptr_atts;
_tmp500=_tmp4FF.zero_term;}}_LL25C: elt_typ_opt=(void**)_tmp4FD;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp500);goto _LL25A;
_LL25D:;_LL25E: goto _LL25A;_LL25A:;}{void*_tmp501=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp4DD);{struct Cyc_Core_Opt*_tmp915;e1->topt=((_tmp915=
_cycalloc(sizeof(*_tmp915)),((_tmp915->v=_tmp501,_tmp915))));}{void*res_typ;{
void*_tmp503=Cyc_Tcutil_compress(_tmp501);struct Cyc_Absyn_ArrayInfo _tmp505;void*
_tmp506;struct Cyc_Absyn_Tqual _tmp507;struct Cyc_Absyn_Exp*_tmp508;union Cyc_Absyn_Constraint*
_tmp509;_LL260: {struct Cyc_Absyn_ArrayType_struct*_tmp504=(struct Cyc_Absyn_ArrayType_struct*)
_tmp503;if(_tmp504->tag != 9)goto _LL262;else{_tmp505=_tmp504->f1;_tmp506=_tmp505.elt_type;
_tmp507=_tmp505.tq;_tmp508=_tmp505.num_elts;_tmp509=_tmp505.zero_term;}}_LL261:{
struct Cyc_Absyn_PointerType_struct _tmp92A;struct Cyc_Absyn_PtrAtts _tmp929;struct
Cyc_Absyn_Upper_b_struct _tmp928;struct Cyc_Absyn_Upper_b_struct*_tmp927;struct Cyc_Absyn_PtrInfo
_tmp926;struct Cyc_Absyn_PointerType_struct*_tmp925;res_typ=(void*)((_tmp925=
_cycalloc(sizeof(*_tmp925)),((_tmp925[0]=((_tmp92A.tag=5,((_tmp92A.f1=((_tmp926.elt_typ=
_tmp506,((_tmp926.elt_tq=_tmp507,((_tmp926.ptr_atts=((_tmp929.rgn=rgn,((_tmp929.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp929.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp927=
_cycalloc(sizeof(*_tmp927)),((_tmp927[0]=((_tmp928.tag=1,((_tmp928.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp508),_tmp928)))),_tmp927))))),((_tmp929.zero_term=_tmp509,((
_tmp929.ptrloc=0,_tmp929)))))))))),_tmp926)))))),_tmp92A)))),_tmp925))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp92B;e->topt=((_tmp92B=_cycalloc(
sizeof(*_tmp92B)),((_tmp92B->v=res_typ,_tmp92B))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL25F;_LL262:;_LL263: {
const char*_tmp92E;void*_tmp92D;(_tmp92D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp92E="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp92E,sizeof(char),50))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}
_LL25F:;}return res_typ;};};}_LL24C: {struct Cyc_Absyn_Const_e_struct*_tmp4DE=(
struct Cyc_Absyn_Const_e_struct*)_tmp4D7;if(_tmp4DE->tag != 0)goto _LL24E;else{
_tmp4DF=_tmp4DE->f1;if((_tmp4DF.String_c).tag != 7)goto _LL24E;_tmp4E0=(struct
_dyneither_ptr)(_tmp4DF.String_c).val;}}_LL24D: {void*_tmp513=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp514=Cyc_Tcexp_tcExp(te,(void**)& _tmp513,e1);
struct Cyc_Absyn_Upper_b_struct _tmp931;struct Cyc_Absyn_Upper_b_struct*_tmp930;
return Cyc_Absyn_atb_typ(_tmp514,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp930=
_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=1,((_tmp931.f1=Cyc_Absyn_uint_exp(
1,0),_tmp931)))),_tmp930)))),Cyc_Absyn_false_conref);}_LL24E:;_LL24F: {void**
topt2=0;if(topt != 0){void*_tmp517=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp519;void*_tmp51A;void**_tmp51B;struct Cyc_Absyn_Tqual _tmp51C;_LL265: {struct
Cyc_Absyn_PointerType_struct*_tmp518=(struct Cyc_Absyn_PointerType_struct*)
_tmp517;if(_tmp518->tag != 5)goto _LL267;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.elt_typ;
_tmp51B=(void**)&(_tmp518->f1).elt_typ;_tmp51C=_tmp519.elt_tq;}}_LL266: topt2=(
void**)_tmp51B;goto _LL264;_LL267: {struct Cyc_Absyn_DatatypeType_struct*_tmp51D=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp517;if(_tmp51D->tag != 3)goto _LL269;}
_LL268:{void**_tmp932;topt2=((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=*
topt,_tmp932))));}goto _LL264;_LL269:;_LL26A: goto _LL264;_LL264:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp93C;struct Cyc_Absyn_PtrAtts
_tmp93B;struct Cyc_Absyn_PtrInfo _tmp93A;struct Cyc_Absyn_PointerType_struct*
_tmp939;void*res_typ=(void*)((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[0]=((
_tmp93C.tag=5,((_tmp93C.f1=((_tmp93A.elt_typ=telt,((_tmp93A.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp93A.ptr_atts=((_tmp93B.rgn=rgn,((_tmp93B.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp93B.bounds=Cyc_Absyn_bounds_one_conref,((_tmp93B.zero_term=
Cyc_Absyn_false_conref,((_tmp93B.ptrloc=0,_tmp93B)))))))))),_tmp93A)))))),
_tmp93C)))),_tmp939))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp93D;e->topt=((_tmp93D=_cycalloc(
sizeof(*_tmp93D)),((_tmp93D->v=res_typ,_tmp93D))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL245:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp524=t;
struct Cyc_Absyn_ArrayInfo _tmp526;void*_tmp527;struct Cyc_Absyn_Tqual _tmp528;
struct Cyc_Absyn_Exp*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;_LL26C: {struct Cyc_Absyn_ArrayType_struct*
_tmp525=(struct Cyc_Absyn_ArrayType_struct*)_tmp524;if(_tmp525->tag != 9)goto
_LL26E;else{_tmp526=_tmp525->f1;_tmp527=_tmp526.elt_type;_tmp528=_tmp526.tq;
_tmp529=_tmp526.num_elts;_tmp52A=_tmp526.zero_term;}}_LL26D: {void*_tmp52C;
struct _tuple12 _tmp52B=Cyc_Tcutil_addressof_props(te,e);_tmp52C=_tmp52B.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp940;struct Cyc_Absyn_Upper_b_struct*_tmp93F;
void*_tmp52D=_tmp529 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp93F=
_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp940.tag=1,((_tmp940.f1=(struct Cyc_Absyn_Exp*)
_tmp529,_tmp940)))),_tmp93F))));t=Cyc_Absyn_atb_typ(_tmp527,_tmp52C,_tmp528,
_tmp52D,_tmp52A);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL26E:;_LL26F: return t;_LL26B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp530=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp530,t) && !Cyc_Tcutil_is_noalias_path(_tmp530,e)){{const char*_tmp948;void*
_tmp947[3];const char*_tmp946;struct Cyc_String_pa_struct _tmp945;struct Cyc_String_pa_struct
_tmp944;struct Cyc_String_pa_struct _tmp943;(_tmp943.tag=0,((_tmp943.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp944.tag=0,((
_tmp944.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp945.tag=0,((_tmp945.f1=(
struct _dyneither_ptr)(topt == 0?(_tmp946="NULL",_tag_dyneither(_tmp946,sizeof(
char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt)),((_tmp947[0]=&
_tmp945,((_tmp947[1]=& _tmp944,((_tmp947[2]=& _tmp943,Cyc_fprintf(Cyc_stderr,((
_tmp948="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp948,sizeof(char),30))),
_tag_dyneither(_tmp947,sizeof(void*),3)))))))))))))))))));}{const char*_tmp94B;
void*_tmp94A;(_tmp94A=0,Cyc_Tcutil_terr(e->loc,((_tmp94B="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp94B,sizeof(char),49))),_tag_dyneither(_tmp94A,sizeof(void*),0)));};}{
void*_tmp539=e->r;union Cyc_Absyn_Cnst _tmp53D;struct _dyneither_ptr _tmp53E;_LL271: {
struct Cyc_Absyn_Array_e_struct*_tmp53A=(struct Cyc_Absyn_Array_e_struct*)_tmp539;
if(_tmp53A->tag != 27)goto _LL273;}_LL272: goto _LL274;_LL273: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp53B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp539;if(_tmp53B->tag != 28)
goto _LL275;}_LL274: goto _LL276;_LL275: {struct Cyc_Absyn_Const_e_struct*_tmp53C=(
struct Cyc_Absyn_Const_e_struct*)_tmp539;if(_tmp53C->tag != 0)goto _LL277;else{
_tmp53D=_tmp53C->f1;if((_tmp53D.String_c).tag != 7)goto _LL277;_tmp53E=(struct
_dyneither_ptr)(_tmp53D.String_c).val;}}_LL276: return t;_LL277:;_LL278: t=Cyc_Tcutil_compress(
t);{void*_tmp53F=t;struct Cyc_Absyn_ArrayInfo _tmp541;void*_tmp542;struct Cyc_Absyn_Tqual
_tmp543;struct Cyc_Absyn_Exp*_tmp544;union Cyc_Absyn_Constraint*_tmp545;_LL27A: {
struct Cyc_Absyn_ArrayType_struct*_tmp540=(struct Cyc_Absyn_ArrayType_struct*)
_tmp53F;if(_tmp540->tag != 9)goto _LL27C;else{_tmp541=_tmp540->f1;_tmp542=_tmp541.elt_type;
_tmp543=_tmp541.tq;_tmp544=_tmp541.num_elts;_tmp545=_tmp541.zero_term;}}_LL27B: {
void*_tmp547;struct _tuple12 _tmp546=Cyc_Tcutil_addressof_props(te,e);_tmp547=
_tmp546.f2;{struct Cyc_Absyn_Upper_b_struct _tmp94E;struct Cyc_Absyn_Upper_b_struct*
_tmp94D;void*b=_tmp544 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp94D=
_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94E.tag=1,((_tmp94E.f1=(struct Cyc_Absyn_Exp*)
_tmp544,_tmp94E)))),_tmp94D))));t=Cyc_Absyn_atb_typ(_tmp542,_tmp547,_tmp543,b,
_tmp545);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL27C:;_LL27D: return t;_LL279:;};_LL270:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp54A=e->r;
struct Cyc_Absyn_Exp*_tmp54C;_LL27F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp54B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp54A;if(_tmp54B->tag != 13)
goto _LL281;else{_tmp54C=_tmp54B->f1;}}_LL280: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp54C);((struct Cyc_Core_Opt*)_check_null(_tmp54C->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp54C->topt))->v,0);e->topt=_tmp54C->topt;
goto _LL27E;_LL281:;_LL282: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp54D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp54F;void*_tmp550;struct Cyc_Absyn_Tqual
_tmp551;struct Cyc_Absyn_PtrAtts _tmp552;void*_tmp553;union Cyc_Absyn_Constraint*
_tmp554;union Cyc_Absyn_Constraint*_tmp555;union Cyc_Absyn_Constraint*_tmp556;
_LL284: {struct Cyc_Absyn_PointerType_struct*_tmp54E=(struct Cyc_Absyn_PointerType_struct*)
_tmp54D;if(_tmp54E->tag != 5)goto _LL286;else{_tmp54F=_tmp54E->f1;_tmp550=_tmp54F.elt_typ;
_tmp551=_tmp54F.elt_tq;_tmp552=_tmp54F.ptr_atts;_tmp553=_tmp552.rgn;_tmp554=
_tmp552.nullable;_tmp555=_tmp552.bounds;_tmp556=_tmp552.zero_term;}}_LL285:{void*
_tmp557=Cyc_Tcutil_compress(_tmp550);struct Cyc_Absyn_FnInfo _tmp559;struct Cyc_List_List*
_tmp55A;struct Cyc_Core_Opt*_tmp55B;void*_tmp55C;struct Cyc_List_List*_tmp55D;int
_tmp55E;struct Cyc_Absyn_VarargInfo*_tmp55F;struct Cyc_List_List*_tmp560;struct Cyc_List_List*
_tmp561;_LL289: {struct Cyc_Absyn_FnType_struct*_tmp558=(struct Cyc_Absyn_FnType_struct*)
_tmp557;if(_tmp558->tag != 10)goto _LL28B;else{_tmp559=_tmp558->f1;_tmp55A=_tmp559.tvars;
_tmp55B=_tmp559.effect;_tmp55C=_tmp559.ret_typ;_tmp55D=_tmp559.args;_tmp55E=
_tmp559.c_varargs;_tmp55F=_tmp559.cyc_varargs;_tmp560=_tmp559.rgn_po;_tmp561=
_tmp559.attributes;}}_LL28A: if(_tmp55A != 0){struct _RegionHandle*_tmp562=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp94F;struct _tuple10 _tmp563=(_tmp94F.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp94F.f2=_tmp562,_tmp94F)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp562,Cyc_Tcutil_r_make_inst_var,&
_tmp563,_tmp55A);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp955;struct Cyc_Absyn_FnInfo
_tmp954;struct Cyc_Absyn_FnType_struct*_tmp953;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp562,inst,(void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp955.tag=
10,((_tmp955.f1=((_tmp954.tvars=0,((_tmp954.effect=_tmp55B,((_tmp954.ret_typ=
_tmp55C,((_tmp954.args=_tmp55D,((_tmp954.c_varargs=_tmp55E,((_tmp954.cyc_varargs=
_tmp55F,((_tmp954.rgn_po=_tmp560,((_tmp954.attributes=_tmp561,_tmp954)))))))))))))))),
_tmp955)))),_tmp953)))));struct Cyc_Absyn_PointerType_struct _tmp95F;struct Cyc_Absyn_PtrAtts
_tmp95E;struct Cyc_Absyn_PtrInfo _tmp95D;struct Cyc_Absyn_PointerType_struct*
_tmp95C;struct Cyc_Absyn_PointerType_struct*_tmp564=(_tmp95C=_cycalloc(sizeof(*
_tmp95C)),((_tmp95C[0]=((_tmp95F.tag=5,((_tmp95F.f1=((_tmp95D.elt_typ=ftyp,((
_tmp95D.elt_tq=_tmp551,((_tmp95D.ptr_atts=((_tmp95E.rgn=_tmp553,((_tmp95E.nullable=
_tmp554,((_tmp95E.bounds=_tmp555,((_tmp95E.zero_term=_tmp556,((_tmp95E.ptrloc=0,
_tmp95E)))))))))),_tmp95D)))))),_tmp95F)))),_tmp95C)));struct Cyc_Absyn_Exp*
_tmp565=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp962;
struct Cyc_Absyn_Instantiate_e_struct*_tmp961;e->r=(void*)((_tmp961=_cycalloc(
sizeof(*_tmp961)),((_tmp961[0]=((_tmp962.tag=14,((_tmp962.f1=_tmp565,((_tmp962.f2=
ts,_tmp962)))))),_tmp961))));}{struct Cyc_Core_Opt*_tmp963;e->topt=((_tmp963=
_cycalloc(sizeof(*_tmp963)),((_tmp963->v=(void*)_tmp564,_tmp963))));};}goto
_LL288;_LL28B:;_LL28C: goto _LL288;_LL288:;}goto _LL283;_LL286:;_LL287: goto _LL283;
_LL283:;}goto _LL27E;_LL27E:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp571=e->r;struct Cyc_Absyn_Exp*
_tmp573;struct _tuple1*_tmp575;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_List_List*
_tmp578;struct Cyc_Core_Opt*_tmp57A;struct Cyc_List_List*_tmp57B;union Cyc_Absyn_Cnst
_tmp57D;union Cyc_Absyn_Cnst*_tmp57E;struct _tuple1*_tmp580;void*_tmp581;enum Cyc_Absyn_Primop
_tmp583;struct Cyc_List_List*_tmp584;struct Cyc_Absyn_Exp*_tmp586;enum Cyc_Absyn_Incrementor
_tmp587;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Core_Opt*_tmp58A;struct Cyc_Absyn_Exp*
_tmp58B;struct Cyc_Absyn_Exp*_tmp58D;struct Cyc_Absyn_Exp*_tmp58E;struct Cyc_Absyn_Exp*
_tmp58F;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Exp*_tmp592;struct Cyc_Absyn_Exp*
_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*
_tmp598;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_List_List*_tmp59B;struct Cyc_Absyn_VarargCallInfo*
_tmp59C;struct Cyc_Absyn_VarargCallInfo**_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;
struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_List_List*_tmp5A2;void*_tmp5A4;struct Cyc_Absyn_Exp*
_tmp5A5;enum Cyc_Absyn_Coercion _tmp5A6;enum Cyc_Absyn_Coercion*_tmp5A7;struct Cyc_Absyn_Exp*
_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*
_tmp5AE;void*_tmp5B0;void*_tmp5B2;void*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B5;
struct Cyc_Absyn_Exp*_tmp5B7;struct _dyneither_ptr*_tmp5B8;int _tmp5B9;int*_tmp5BA;
int _tmp5BB;int*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BE;struct _dyneither_ptr*_tmp5BF;
int _tmp5C0;int*_tmp5C1;int _tmp5C2;int*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C5;struct
Cyc_Absyn_Exp*_tmp5C6;struct Cyc_List_List*_tmp5C8;struct _tuple8*_tmp5CA;struct
Cyc_List_List*_tmp5CB;struct Cyc_List_List*_tmp5CD;struct Cyc_Absyn_Stmt*_tmp5CF;
struct Cyc_Absyn_Vardecl*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*
_tmp5D3;int _tmp5D4;int*_tmp5D5;struct _tuple1*_tmp5D7;struct _tuple1**_tmp5D8;
struct Cyc_List_List*_tmp5D9;struct Cyc_List_List**_tmp5DA;struct Cyc_List_List*
_tmp5DB;struct Cyc_Absyn_Aggrdecl*_tmp5DC;struct Cyc_Absyn_Aggrdecl**_tmp5DD;void*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E2;struct Cyc_Absyn_Datatypedecl*
_tmp5E3;struct Cyc_Absyn_Datatypefield*_tmp5E4;struct _tuple1*_tmp5E6;struct
_tuple1**_tmp5E7;struct Cyc_Absyn_Enumdecl*_tmp5E8;struct Cyc_Absyn_Enumfield*
_tmp5E9;struct _tuple1*_tmp5EB;struct _tuple1**_tmp5EC;void*_tmp5ED;struct Cyc_Absyn_Enumfield*
_tmp5EE;struct Cyc_Absyn_MallocInfo _tmp5F0;int _tmp5F1;int*_tmp5F2;struct Cyc_Absyn_Exp*
_tmp5F3;void**_tmp5F4;void***_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Exp**
_tmp5F7;int _tmp5F8;int*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Exp*
_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FE;struct _dyneither_ptr*_tmp5FF;void*_tmp601;
_LL28E: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp572=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp571;if(_tmp572->tag != 13)goto _LL290;else{_tmp573=_tmp572->f1;}}_LL28F: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp573);return;_LL290: {struct Cyc_Absyn_UnknownId_e_struct*_tmp574=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp571;if(_tmp574->tag != 2)goto _LL292;else{
_tmp575=_tmp574->f1;}}_LL291: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp575);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp576=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp571;if(_tmp576->tag != 10)goto _LL294;else{
_tmp577=_tmp576->f1;_tmp578=_tmp576->f2;}}_LL293: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp577,_tmp578);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL294: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp579=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp571;if(_tmp579->tag != 36)
goto _LL296;else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;}}_LL295: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp57B);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL296: {struct
Cyc_Absyn_Const_e_struct*_tmp57C=(struct Cyc_Absyn_Const_e_struct*)_tmp571;if(
_tmp57C->tag != 0)goto _LL298;else{_tmp57D=_tmp57C->f1;_tmp57E=(union Cyc_Absyn_Cnst*)&
_tmp57C->f1;}}_LL297: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp57E,e);goto _LL28D;_LL298: {struct Cyc_Absyn_Var_e_struct*_tmp57F=(struct Cyc_Absyn_Var_e_struct*)
_tmp571;if(_tmp57F->tag != 1)goto _LL29A;else{_tmp580=_tmp57F->f1;_tmp581=(void*)
_tmp57F->f2;}}_LL299: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp580,_tmp581);goto _LL28D;
_LL29A: {struct Cyc_Absyn_Primop_e_struct*_tmp582=(struct Cyc_Absyn_Primop_e_struct*)
_tmp571;if(_tmp582->tag != 3)goto _LL29C;else{_tmp583=_tmp582->f1;_tmp584=_tmp582->f2;}}
_LL29B: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp583,_tmp584);goto _LL28D;_LL29C: {
struct Cyc_Absyn_Increment_e_struct*_tmp585=(struct Cyc_Absyn_Increment_e_struct*)
_tmp571;if(_tmp585->tag != 5)goto _LL29E;else{_tmp586=_tmp585->f1;_tmp587=_tmp585->f2;}}
_LL29D: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp586,_tmp587);goto _LL28D;_LL29E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp588=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp571;if(_tmp588->tag != 4)goto _LL2A0;else{_tmp589=_tmp588->f1;_tmp58A=_tmp588->f2;
_tmp58B=_tmp588->f3;}}_LL29F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp589,_tmp58A,
_tmp58B);goto _LL28D;_LL2A0: {struct Cyc_Absyn_Conditional_e_struct*_tmp58C=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp571;if(_tmp58C->tag != 6)goto _LL2A2;
else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;_tmp58F=_tmp58C->f3;}}_LL2A1: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp58D,_tmp58E,_tmp58F);goto _LL28D;_LL2A2: {struct Cyc_Absyn_And_e_struct*
_tmp590=(struct Cyc_Absyn_And_e_struct*)_tmp571;if(_tmp590->tag != 7)goto _LL2A4;
else{_tmp591=_tmp590->f1;_tmp592=_tmp590->f2;}}_LL2A3: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp591,_tmp592);goto _LL28D;_LL2A4: {struct Cyc_Absyn_Or_e_struct*_tmp593=(struct
Cyc_Absyn_Or_e_struct*)_tmp571;if(_tmp593->tag != 8)goto _LL2A6;else{_tmp594=
_tmp593->f1;_tmp595=_tmp593->f2;}}_LL2A5: t=Cyc_Tcexp_tcOr(te,loc,_tmp594,_tmp595);
goto _LL28D;_LL2A6: {struct Cyc_Absyn_SeqExp_e_struct*_tmp596=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp571;if(_tmp596->tag != 9)goto _LL2A8;else{_tmp597=_tmp596->f1;_tmp598=_tmp596->f2;}}
_LL2A7: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp597,_tmp598);goto _LL28D;_LL2A8: {
struct Cyc_Absyn_FnCall_e_struct*_tmp599=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp571;if(_tmp599->tag != 11)goto _LL2AA;else{_tmp59A=_tmp599->f1;_tmp59B=_tmp599->f2;
_tmp59C=_tmp599->f3;_tmp59D=(struct Cyc_Absyn_VarargCallInfo**)& _tmp599->f3;}}
_LL2A9: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp59A,_tmp59B,_tmp59D);goto _LL28D;
_LL2AA: {struct Cyc_Absyn_Throw_e_struct*_tmp59E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp571;if(_tmp59E->tag != 12)goto _LL2AC;else{_tmp59F=_tmp59E->f1;}}_LL2AB: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp59F);goto _LL28D;_LL2AC: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5A0=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp571;if(_tmp5A0->tag != 14)goto
_LL2AE;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A0->f2;}}_LL2AD: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5A1,_tmp5A2);goto _LL28D;_LL2AE: {struct Cyc_Absyn_Cast_e_struct*
_tmp5A3=(struct Cyc_Absyn_Cast_e_struct*)_tmp571;if(_tmp5A3->tag != 15)goto _LL2B0;
else{_tmp5A4=(void*)_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;_tmp5A6=_tmp5A3->f4;_tmp5A7=(
enum Cyc_Absyn_Coercion*)& _tmp5A3->f4;}}_LL2AF: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5A4,_tmp5A5,(enum Cyc_Absyn_Coercion*)_tmp5A7);goto _LL28D;_LL2B0: {struct Cyc_Absyn_Address_e_struct*
_tmp5A8=(struct Cyc_Absyn_Address_e_struct*)_tmp571;if(_tmp5A8->tag != 16)goto
_LL2B2;else{_tmp5A9=_tmp5A8->f1;}}_LL2B1: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5A9);goto _LL28D;_LL2B2: {struct Cyc_Absyn_New_e_struct*_tmp5AA=(struct Cyc_Absyn_New_e_struct*)
_tmp571;if(_tmp5AA->tag != 17)goto _LL2B4;else{_tmp5AB=_tmp5AA->f1;_tmp5AC=_tmp5AA->f2;}}
_LL2B3: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5AB,e,_tmp5AC);goto _LL28D;_LL2B4: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp5AD=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp571;if(_tmp5AD->tag != 19)goto _LL2B6;else{_tmp5AE=_tmp5AD->f1;}}_LL2B5: {void*
_tmp602=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5AE);t=Cyc_Tcexp_tcSizeof(te,loc,topt,
_tmp602);goto _LL28D;}_LL2B6: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5AF=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp571;if(_tmp5AF->tag != 18)goto _LL2B8;else{
_tmp5B0=(void*)_tmp5AF->f1;}}_LL2B7: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5B0);
goto _LL28D;_LL2B8: {struct Cyc_Absyn_Offsetof_e_struct*_tmp5B1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp571;if(_tmp5B1->tag != 20)goto _LL2BA;else{_tmp5B2=(void*)_tmp5B1->f1;_tmp5B3=(
void*)_tmp5B1->f2;}}_LL2B9: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5B2,_tmp5B3);
goto _LL28D;_LL2BA: {struct Cyc_Absyn_Deref_e_struct*_tmp5B4=(struct Cyc_Absyn_Deref_e_struct*)
_tmp571;if(_tmp5B4->tag != 21)goto _LL2BC;else{_tmp5B5=_tmp5B4->f1;}}_LL2BB: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp5B5);goto _LL28D;_LL2BC: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp5B6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp571;if(_tmp5B6->tag != 22)goto
_LL2BE;else{_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;_tmp5B9=_tmp5B6->f3;_tmp5BA=(
int*)& _tmp5B6->f3;_tmp5BB=_tmp5B6->f4;_tmp5BC=(int*)& _tmp5B6->f4;}}_LL2BD: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp5B7,_tmp5B8,_tmp5BA,_tmp5BC);goto _LL28D;_LL2BE: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp5BD=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp571;if(_tmp5BD->tag != 23)goto
_LL2C0;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;_tmp5C0=_tmp5BD->f3;_tmp5C1=(
int*)& _tmp5BD->f3;_tmp5C2=_tmp5BD->f4;_tmp5C3=(int*)& _tmp5BD->f4;}}_LL2BF: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp5BE,_tmp5BF,_tmp5C1,_tmp5C3);goto _LL28D;_LL2C0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp5C4=(struct Cyc_Absyn_Subscript_e_struct*)_tmp571;if(_tmp5C4->tag != 24)goto
_LL2C2;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;}}_LL2C1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp5C5,_tmp5C6);goto _LL28D;_LL2C2: {struct Cyc_Absyn_Tuple_e_struct*
_tmp5C7=(struct Cyc_Absyn_Tuple_e_struct*)_tmp571;if(_tmp5C7->tag != 25)goto _LL2C4;
else{_tmp5C8=_tmp5C7->f1;}}_LL2C3: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5C8);goto
_LL28D;_LL2C4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp5C9=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp571;if(_tmp5C9->tag != 26)goto _LL2C6;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=_tmp5C9->f2;}}
_LL2C5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5CA,_tmp5CB);goto _LL28D;_LL2C6: {
struct Cyc_Absyn_Array_e_struct*_tmp5CC=(struct Cyc_Absyn_Array_e_struct*)_tmp571;
if(_tmp5CC->tag != 27)goto _LL2C8;else{_tmp5CD=_tmp5CC->f1;}}_LL2C7: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp603=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp605;void*_tmp606;void**_tmp607;union Cyc_Absyn_Constraint*
_tmp608;_LL2DF: {struct Cyc_Absyn_ArrayType_struct*_tmp604=(struct Cyc_Absyn_ArrayType_struct*)
_tmp603;if(_tmp604->tag != 9)goto _LL2E1;else{_tmp605=_tmp604->f1;_tmp606=_tmp605.elt_type;
_tmp607=(void**)&(_tmp604->f1).elt_type;_tmp608=_tmp605.zero_term;}}_LL2E0:
elt_topt=(void**)_tmp607;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp608);goto _LL2DE;_LL2E1:;_LL2E2: goto _LL2DE;_LL2DE:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp5CD);goto _LL28D;}_LL2C8: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp5CE=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp571;
if(_tmp5CE->tag != 37)goto _LL2CA;else{_tmp5CF=_tmp5CE->f1;}}_LL2C9: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp5CF);goto _LL28D;_LL2CA: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp5D0=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp571;if(_tmp5D0->tag != 28)
goto _LL2CC;else{_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;_tmp5D3=_tmp5D0->f3;
_tmp5D4=_tmp5D0->f4;_tmp5D5=(int*)& _tmp5D0->f4;}}_LL2CB: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp5D1,_tmp5D2,_tmp5D3,_tmp5D5);goto _LL28D;_LL2CC: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp5D6=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp571;if(_tmp5D6->tag != 29)goto
_LL2CE;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=(struct _tuple1**)& _tmp5D6->f1;_tmp5D9=
_tmp5D6->f2;_tmp5DA=(struct Cyc_List_List**)& _tmp5D6->f2;_tmp5DB=_tmp5D6->f3;
_tmp5DC=_tmp5D6->f4;_tmp5DD=(struct Cyc_Absyn_Aggrdecl**)& _tmp5D6->f4;}}_LL2CD: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp5D8,_tmp5DA,_tmp5DB,_tmp5DD);goto _LL28D;
_LL2CE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp5DE=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp571;if(_tmp5DE->tag != 30)goto _LL2D0;else{_tmp5DF=(void*)_tmp5DE->f1;_tmp5E0=
_tmp5DE->f2;}}_LL2CF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp5DF,_tmp5E0);goto _LL28D;
_LL2D0: {struct Cyc_Absyn_Datatype_e_struct*_tmp5E1=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp571;if(_tmp5E1->tag != 31)goto _LL2D2;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;
_tmp5E4=_tmp5E1->f3;}}_LL2D1: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp5E2,
_tmp5E3,_tmp5E4);goto _LL28D;_LL2D2: {struct Cyc_Absyn_Enum_e_struct*_tmp5E5=(
struct Cyc_Absyn_Enum_e_struct*)_tmp571;if(_tmp5E5->tag != 32)goto _LL2D4;else{
_tmp5E6=_tmp5E5->f1;_tmp5E7=(struct _tuple1**)& _tmp5E5->f1;_tmp5E8=_tmp5E5->f2;
_tmp5E9=_tmp5E5->f3;}}_LL2D3:*_tmp5E7=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp5E9))->name;{struct Cyc_Absyn_EnumType_struct _tmp966;struct Cyc_Absyn_EnumType_struct*
_tmp965;t=(void*)((_tmp965=_cycalloc(sizeof(*_tmp965)),((_tmp965[0]=((_tmp966.tag=
14,((_tmp966.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp5E8))->name,((
_tmp966.f2=_tmp5E8,_tmp966)))))),_tmp965))));}goto _LL28D;_LL2D4: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp5EA=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp571;if(_tmp5EA->tag != 33)goto
_LL2D6;else{_tmp5EB=_tmp5EA->f1;_tmp5EC=(struct _tuple1**)& _tmp5EA->f1;_tmp5ED=(
void*)_tmp5EA->f2;_tmp5EE=_tmp5EA->f3;}}_LL2D5:*_tmp5EC=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp5EE))->name;t=_tmp5ED;goto _LL28D;_LL2D6: {struct Cyc_Absyn_Malloc_e_struct*
_tmp5EF=(struct Cyc_Absyn_Malloc_e_struct*)_tmp571;if(_tmp5EF->tag != 34)goto
_LL2D8;else{_tmp5F0=_tmp5EF->f1;_tmp5F1=_tmp5F0.is_calloc;_tmp5F2=(int*)&(
_tmp5EF->f1).is_calloc;_tmp5F3=_tmp5F0.rgn;_tmp5F4=_tmp5F0.elt_type;_tmp5F5=(
void***)&(_tmp5EF->f1).elt_type;_tmp5F6=_tmp5F0.num_elts;_tmp5F7=(struct Cyc_Absyn_Exp**)&(
_tmp5EF->f1).num_elts;_tmp5F8=_tmp5F0.fat_result;_tmp5F9=(int*)&(_tmp5EF->f1).fat_result;}}
_LL2D7: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp5F3,_tmp5F5,_tmp5F7,_tmp5F2,_tmp5F9);
goto _LL28D;_LL2D8: {struct Cyc_Absyn_Swap_e_struct*_tmp5FA=(struct Cyc_Absyn_Swap_e_struct*)
_tmp571;if(_tmp5FA->tag != 35)goto _LL2DA;else{_tmp5FB=_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}
_LL2D9: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp5FB,_tmp5FC);goto _LL28D;_LL2DA: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp5FD=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp571;if(_tmp5FD->tag != 38)goto _LL2DC;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}
_LL2DB: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp5FE,_tmp5FF);goto _LL28D;_LL2DC: {
struct Cyc_Absyn_Valueof_e_struct*_tmp600=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp571;if(_tmp600->tag != 39)goto _LL28D;else{_tmp601=(void*)_tmp600->f1;}}_LL2DD:
if(!te->allow_valueof){const char*_tmp969;void*_tmp968;(_tmp968=0,Cyc_Tcutil_terr(
e->loc,((_tmp969="valueof(-) can only occur within types",_tag_dyneither(_tmp969,
sizeof(char),39))),_tag_dyneither(_tmp968,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL28D;_LL28D:;}{struct Cyc_Core_Opt*_tmp96A;e->topt=((_tmp96A=_cycalloc(
sizeof(*_tmp96A)),((_tmp96A->v=t,_tmp96A))));};}
