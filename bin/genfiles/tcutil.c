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
tag;struct _dyneither_ptr f1;};int Cyc_printf(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;
void*f2;};void*Cyc_Core_fst(struct _tuple0*);int Cyc_Core_intcmp(int,int);extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_copy(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(struct
Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple2{union
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};
struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple8 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div
 = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt
 = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 
12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift
 = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);
struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct _tuple12 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);int Cyc_Tcutil_kind_eq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,
enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);void*Cyc_Tcutil_pointer_region(void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple14{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*f1;struct _RegionHandle*
f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,
struct Cyc_Absyn_Kind*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr
err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple16{int f1;void*f2;
};struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();
void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpED2;void*
_tmpED1[2];const char*_tmpED0;const char*_tmpECF;struct Cyc_String_pa_struct _tmpECE;
struct Cyc_String_pa_struct _tmpECD;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpECD.tag=0,((_tmpECD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpECE.tag=0,((_tmpECE.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpECF="const ",_tag_dyneither(_tmpECF,sizeof(char),7)):((_tmpED0="",
_tag_dyneither(_tmpED0,sizeof(char),1)))),((_tmpED1[0]=& _tmpECE,((_tmpED1[1]=&
_tmpECD,Cyc_aprintf(((_tmpED2="%s%s",_tag_dyneither(_tmpED2,sizeof(char),5))),
_tag_dyneither(_tmpED1,sizeof(void*),2))))))))))))));const char*_tmpEDB;void*
_tmpEDA[2];const char*_tmpED9;const char*_tmpED8;struct Cyc_String_pa_struct _tmpED7;
struct Cyc_String_pa_struct _tmpED6;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpED6.tag=0,((_tmpED6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpED7.tag=0,((_tmpED7.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpED8="const ",_tag_dyneither(_tmpED8,sizeof(char),7)):((_tmpED9="",
_tag_dyneither(_tmpED9,sizeof(char),1)))),((_tmpEDA[0]=& _tmpED7,((_tmpEDA[1]=&
_tmpED6,Cyc_aprintf(((_tmpEDB="%s%s",_tag_dyneither(_tmpEDB,sizeof(char),5))),
_tag_dyneither(_tmpEDA,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpEDF;
void*_tmpEDE[1];struct Cyc_String_pa_struct _tmpEDD;(_tmpEDD.tag=0,((_tmpEDD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEDE[0]=& _tmpEDD,Cyc_fprintf(
Cyc_stderr,((_tmpEDF="  %s",_tag_dyneither(_tmpEDF,sizeof(char),5))),
_tag_dyneither(_tmpEDE,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpEE2;void*_tmpEE1;(_tmpEE1=0,Cyc_fprintf(
Cyc_stderr,((_tmpEE2="\n\t",_tag_dyneither(_tmpEE2,sizeof(char),3))),
_tag_dyneither(_tmpEE1,sizeof(void*),0)));}pos=8;}else{{const char*_tmpEE5;void*
_tmpEE4;(_tmpEE4=0,Cyc_fprintf(Cyc_stderr,((_tmpEE5=" ",_tag_dyneither(_tmpEE5,
sizeof(char),2))),_tag_dyneither(_tmpEE4,sizeof(void*),0)));}++ pos;}{const char*
_tmpEE8;void*_tmpEE7;(_tmpEE7=0,Cyc_fprintf(Cyc_stderr,((_tmpEE8="and ",
_tag_dyneither(_tmpEE8,sizeof(char),5))),_tag_dyneither(_tmpEE7,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpEEB;
void*_tmpEEA;(_tmpEEA=0,Cyc_fprintf(Cyc_stderr,((_tmpEEB="\n\t",_tag_dyneither(
_tmpEEB,sizeof(char),3))),_tag_dyneither(_tmpEEA,sizeof(void*),0)));}pos=8;}{
const char*_tmpEEF;void*_tmpEEE[1];struct Cyc_String_pa_struct _tmpEED;(_tmpEED.tag=
0,((_tmpEED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpEEE[0]=&
_tmpEED,Cyc_fprintf(Cyc_stderr,((_tmpEEF="%s ",_tag_dyneither(_tmpEEF,sizeof(
char),4))),_tag_dyneither(_tmpEEE,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpEF2;void*
_tmpEF1;(_tmpEF1=0,Cyc_fprintf(Cyc_stderr,((_tmpEF2="\n\t",_tag_dyneither(
_tmpEF2,sizeof(char),3))),_tag_dyneither(_tmpEF1,sizeof(void*),0)));}pos=8;}{
const char*_tmpEF5;void*_tmpEF4;(_tmpEF4=0,Cyc_fprintf(Cyc_stderr,((_tmpEF5="are not compatible. ",
_tag_dyneither(_tmpEF5,sizeof(char),21))),_tag_dyneither(_tmpEF4,sizeof(void*),0)));}
pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const
char*_tmpEF8;void*_tmpEF7;(_tmpEF7=0,Cyc_fprintf(Cyc_stderr,((_tmpEF8="\n\t",
_tag_dyneither(_tmpEF8,sizeof(char),3))),_tag_dyneither(_tmpEF7,sizeof(void*),0)));}{
const char*_tmpEFC;void*_tmpEFB[1];struct Cyc_String_pa_struct _tmpEFA;(_tmpEFA.tag=
0,((_tmpEFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpEFB[0]=& _tmpEFA,Cyc_fprintf(Cyc_stderr,((_tmpEFC="%s",_tag_dyneither(_tmpEFC,
sizeof(char),3))),_tag_dyneither(_tmpEFB,sizeof(void*),1)))))));};}{const char*
_tmpEFF;void*_tmpEFE;(_tmpEFE=0,Cyc_fprintf(Cyc_stderr,((_tmpEFF="\n",
_tag_dyneither(_tmpEFF,sizeof(char),2))),_tag_dyneither(_tmpEFE,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpF03;void*_tmpF02[1];struct Cyc_String_pa_struct _tmpF01;(_tmpF01.tag=
0,((_tmpF01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpF02[0]=&
_tmpF01,Cyc_fprintf(Cyc_stderr,((_tmpF03="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpF03,sizeof(char),36))),_tag_dyneither(_tmpF02,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpF06;struct Cyc_Core_Impossible_struct*_tmpF05;(int)_throw((void*)((_tmpF05=
_cycalloc(sizeof(*_tmpF05)),((_tmpF05[0]=((_tmpF06.tag=Cyc_Core_Impossible,((
_tmpF06.f1=msg,_tmpF06)))),_tmpF05)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpF0B;void*_tmpF0A[2];struct Cyc_String_pa_struct _tmpF09;struct Cyc_String_pa_struct
_tmpF08;(_tmpF08.tag=0,((_tmpF08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpF09.tag=
0,((_tmpF09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpF0A[0]=& _tmpF09,((_tmpF0A[1]=& _tmpF08,Cyc_fprintf(
Cyc_stderr,((_tmpF0B="%s::%s ",_tag_dyneither(_tmpF0B,sizeof(char),8))),
_tag_dyneither(_tmpF0A,sizeof(void*),2)))))))))))));}{const char*_tmpF0E;void*
_tmpF0D;(_tmpF0D=0,Cyc_fprintf(Cyc_stderr,((_tmpF0E="\n",_tag_dyneither(_tmpF0E,
sizeof(char),2))),_tag_dyneither(_tmpF0D,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpF0F;Cyc_Tcutil_warning_segs=((_tmpF0F=_cycalloc(
sizeof(*_tmpF0F)),((_tmpF0F->hd=sg,((_tmpF0F->tl=Cyc_Tcutil_warning_segs,_tmpF0F))))));}{
struct _dyneither_ptr*_tmpF12;struct Cyc_List_List*_tmpF11;Cyc_Tcutil_warning_msgs=((
_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->hd=((_tmpF12=_cycalloc(sizeof(*
_tmpF12)),((_tmpF12[0]=msg,_tmpF12)))),((_tmpF11->tl=Cyc_Tcutil_warning_msgs,
_tmpF11))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpF15;void*_tmpF14;(_tmpF14=0,
Cyc_fprintf(Cyc_stderr,((_tmpF15="***Warnings***\n",_tag_dyneither(_tmpF15,
sizeof(char),16))),_tag_dyneither(_tmpF14,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpF1A;
void*_tmpF19[2];struct Cyc_String_pa_struct _tmpF18;struct Cyc_String_pa_struct
_tmpF17;(_tmpF17.tag=0,((_tmpF17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpF18.tag=0,((_tmpF18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmpF19[0]=& _tmpF18,((_tmpF19[1]=& _tmpF17,Cyc_fprintf(Cyc_stderr,((_tmpF1A="%s: %s\n",
_tag_dyneither(_tmpF1A,sizeof(char),8))),_tag_dyneither(_tmpF19,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpF1D;void*_tmpF1C;(_tmpF1C=0,Cyc_fprintf(Cyc_stderr,((_tmpF1D="**************\n",
_tag_dyneither(_tmpF1D,sizeof(char),16))),_tag_dyneither(_tmpF1C,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2);static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_compress(void*t){void*_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;
void**_tmp43;void***_tmp44;struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;
struct Cyc_Absyn_Exp*_tmp49;_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}
_LL2: goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmpF1E;*_tmp44=((_tmpF1E=
_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=t2,_tmpF1E))));}return t2;}_LL7: {struct
Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(_tmp45->tag
!= 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)& _tmp45->f2;}}
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp47))->v){
struct Cyc_Core_Opt*_tmpF1F;*_tmp47=((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((
_tmpF1F->v=t2,_tmpF1F))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4C=
_tmp49->r;void*_tmp4E;_LLE: {struct Cyc_Absyn_Valueof_e_struct*_tmp4D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4C;if(_tmp4D->tag != 39)goto _LL10;else{_tmp4E=(void*)_tmp4D->f1;}}_LLF: return
_tmp4E;_LL10:;_LL11: return t;_LLD:;};_LLB:;_LLC: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr);static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp4F=cptr;union Cyc_Absyn_Constraint
_tmp50;int _tmp51;union Cyc_Absyn_Constraint _tmp52;void*_tmp53;union Cyc_Absyn_Constraint
_tmp54;union Cyc_Absyn_Constraint*_tmp55;_LL13: _tmp50=*_tmp4F;if((_tmp50.No_constr).tag
!= 3)goto _LL15;_tmp51=(int)(_tmp50.No_constr).val;_LL14: return Cyc_Absyn_empty_conref();
_LL15: _tmp52=*_tmp4F;if((_tmp52.Eq_constr).tag != 1)goto _LL17;_tmp53=(void*)(
_tmp52.Eq_constr).val;_LL16: return Cyc_Absyn_new_conref(_tmp53);_LL17: _tmp54=*
_tmp4F;if((_tmp54.Forward_constr).tag != 2)goto _LL12;_tmp55=(union Cyc_Absyn_Constraint*)(
_tmp54.Forward_constr).val;_LL18: return Cyc_Tcutil_copy_conref(_tmp55);_LL12:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb);static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp56=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp59;_LL1A: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1C;}_LL1B: {struct Cyc_Absyn_Unknown_kb_struct
_tmpF22;struct Cyc_Absyn_Unknown_kb_struct*_tmpF21;return(void*)((_tmpF21=
_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF22.tag=1,((_tmpF22.f1=0,_tmpF22)))),
_tmpF21))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct Cyc_Absyn_Less_kb_struct*)
_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}_LL1D: {struct Cyc_Absyn_Less_kb_struct
_tmpF25;struct Cyc_Absyn_Less_kb_struct*_tmpF24;return(void*)((_tmpF24=_cycalloc(
sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF25.tag=2,((_tmpF25.f1=0,((_tmpF25.f2=
_tmp59,_tmpF25)))))),_tmpF24))));}_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpF26;return(_tmpF26=_cycalloc(
sizeof(*_tmpF26)),((_tmpF26->name=tv->name,((_tmpF26->identity=- 1,((_tmpF26->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmpF26)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmpF27;return(_tmpF27=_cycalloc(sizeof(*_tmpF27)),((
_tmpF27->f1=_tmp61,((_tmpF27->f2=_tmp62,((_tmpF27->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmpF27)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmpF28;return(_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28->f1=_tmp67,((
_tmpF28->f2=Cyc_Tcutil_copy_type(_tmp68),_tmpF28)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpF29;return(_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29->name=f->name,((
_tmpF29->tq=f->tq,((_tmpF29->type=Cyc_Tcutil_copy_type(f->type),((_tmpF29->width=
f->width,((_tmpF29->attributes=f->attributes,_tmpF29)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*_tmpF2A;
return(_tmpF2A=_cycalloc(sizeof(*_tmpF2A)),((_tmpF2A->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmpF2A->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmpF2A)))));};}static struct
Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmpF2B;return(_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((
_tmpF2B->name=f->name,((_tmpF2B->tag=f->tag,((_tmpF2B->loc=f->loc,_tmpF2B)))))));}
void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(void*t){void*_tmp71=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp75;struct Cyc_Absyn_DatatypeInfo
_tmp77;union Cyc_Absyn_DatatypeInfoU _tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7B;union Cyc_Absyn_DatatypeFieldInfoU _tmp7C;struct Cyc_List_List*_tmp7D;struct
Cyc_Absyn_PtrInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;struct Cyc_Absyn_PtrAtts
_tmp82;void*_tmp83;union Cyc_Absyn_Constraint*_tmp84;union Cyc_Absyn_Constraint*
_tmp85;union Cyc_Absyn_Constraint*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp87;struct Cyc_Absyn_ArrayInfo
_tmp8C;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;struct Cyc_Absyn_Exp*_tmp8F;union
Cyc_Absyn_Constraint*_tmp90;struct Cyc_Position_Segment*_tmp91;struct Cyc_Absyn_FnInfo
_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Core_Opt*_tmp95;void*_tmp96;struct
Cyc_List_List*_tmp97;int _tmp98;struct Cyc_Absyn_VarargInfo*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9D;struct Cyc_Absyn_AggrInfo
_tmp9F;union Cyc_Absyn_AggrInfoU _tmpA0;struct _tuple4 _tmpA1;enum Cyc_Absyn_AggrKind
_tmpA2;struct _tuple2*_tmpA3;struct Cyc_Core_Opt*_tmpA4;struct Cyc_List_List*_tmpA5;
struct Cyc_Absyn_AggrInfo _tmpA7;union Cyc_Absyn_AggrInfoU _tmpA8;struct Cyc_Absyn_Aggrdecl**
_tmpA9;struct Cyc_List_List*_tmpAA;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*
_tmpAD;struct _tuple2*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_List_List*
_tmpB2;void*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB8;void*_tmpBA;void*
_tmpBB;struct _tuple2*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Typedefdecl*
_tmpBF;void*_tmpC3;struct Cyc_List_List*_tmpC5;void*_tmpC7;_LL21: {struct Cyc_Absyn_VoidType_struct*
_tmp72=(struct Cyc_Absyn_VoidType_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Evar_struct*_tmp73=(struct Cyc_Absyn_Evar_struct*)
_tmp71;if(_tmp73->tag != 1)goto _LL25;}_LL24: return t;_LL25: {struct Cyc_Absyn_VarType_struct*
_tmp74=(struct Cyc_Absyn_VarType_struct*)_tmp71;if(_tmp74->tag != 2)goto _LL27;
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmpF2E;struct Cyc_Absyn_VarType_struct*
_tmpF2D;return(void*)((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D[0]=((
_tmpF2E.tag=2,((_tmpF2E.f1=Cyc_Tcutil_copy_tvar(_tmp75),_tmpF2E)))),_tmpF2D))));}
_LL27: {struct Cyc_Absyn_DatatypeType_struct*_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp71;if(_tmp76->tag != 3)goto _LL29;else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;
_tmp79=_tmp77.targs;}}_LL28: {struct Cyc_Absyn_DatatypeType_struct _tmpF34;struct
Cyc_Absyn_DatatypeInfo _tmpF33;struct Cyc_Absyn_DatatypeType_struct*_tmpF32;return(
void*)((_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32[0]=((_tmpF34.tag=3,((
_tmpF34.f1=((_tmpF33.datatype_info=_tmp78,((_tmpF33.targs=Cyc_Tcutil_copy_types(
_tmp79),_tmpF33)))),_tmpF34)))),_tmpF32))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp71;if(_tmp7A->tag != 4)goto
_LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}
_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpF3A;struct Cyc_Absyn_DatatypeFieldInfo
_tmpF39;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF38;return(void*)((_tmpF38=
_cycalloc(sizeof(*_tmpF38)),((_tmpF38[0]=((_tmpF3A.tag=4,((_tmpF3A.f1=((_tmpF39.field_info=
_tmp7C,((_tmpF39.targs=Cyc_Tcutil_copy_types(_tmp7D),_tmpF39)))),_tmpF3A)))),
_tmpF38))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*_tmp7E=(struct Cyc_Absyn_PointerType_struct*)
_tmp71;if(_tmp7E->tag != 5)goto _LL2D;else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;
_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;
_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;_tmp87=_tmp82.ptrloc;}}_LL2C: {void*
_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*_tmpD1=Cyc_Tcutil_copy_type(_tmp83);
union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual _tmpD3=_tmp81;union
Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*_tmpD5=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmpF44;struct Cyc_Absyn_PtrAtts _tmpF43;
struct Cyc_Absyn_PtrInfo _tmpF42;struct Cyc_Absyn_PointerType_struct*_tmpF41;return(
void*)((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=((_tmpF44.tag=5,((
_tmpF44.f1=((_tmpF42.elt_typ=_tmpD0,((_tmpF42.elt_tq=_tmpD3,((_tmpF42.ptr_atts=((
_tmpF43.rgn=_tmpD1,((_tmpF43.nullable=_tmpD2,((_tmpF43.bounds=_tmpD4,((_tmpF43.zero_term=
_tmpD5,((_tmpF43.ptrloc=_tmp87,_tmpF43)))))))))),_tmpF42)))))),_tmpF44)))),
_tmpF41))));}_LL2D: {struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)
_tmp71;if(_tmp88->tag != 6)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*
_tmp89=(struct Cyc_Absyn_FloatType_struct*)_tmp71;if(_tmp89->tag != 7)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp71;if(_tmp8A->tag != 8)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)_tmp71;if(_tmp8B->tag != 9)goto _LL35;
else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;
_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmpF4A;struct Cyc_Absyn_ArrayInfo _tmpF49;struct Cyc_Absyn_ArrayType_struct*
_tmpF48;return(void*)((_tmpF48=_cycalloc(sizeof(*_tmpF48)),((_tmpF48[0]=((
_tmpF4A.tag=9,((_tmpF4A.f1=((_tmpF49.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((
_tmpF49.tq=_tmp8E,((_tmpF49.num_elts=_tmp8F,((_tmpF49.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmpF49.zt_loc=
_tmp91,_tmpF49)))))))))),_tmpF4A)))),_tmpF48))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp92=(struct Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;
else{_tmp93=_tmp92->f1;_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;
_tmp97=_tmp93.args;_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=
_tmp93.rgn_po;_tmp9B=_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*
_tmpF4B;struct Cyc_Core_Opt*_tmpDE=_tmp95 == 0?0:((_tmpF4B=_cycalloc(sizeof(*
_tmpF4B)),((_tmpF4B->v=Cyc_Tcutil_copy_type((void*)_tmp95->v),_tmpF4B))));void*
_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp99 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp99;struct Cyc_Absyn_VarargInfo*
_tmpF4C;cyc_varargs2=((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C->name=cv->name,((
_tmpF4C->tq=cv->tq,((_tmpF4C->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF4C->inject=
cv->inject,_tmpF4C))))))))));}{struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp9A);struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmpF52;
struct Cyc_Absyn_FnInfo _tmpF51;struct Cyc_Absyn_FnType_struct*_tmpF50;return(void*)((
_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50[0]=((_tmpF52.tag=10,((_tmpF52.f1=((
_tmpF51.tvars=_tmpDD,((_tmpF51.effect=_tmpDE,((_tmpF51.ret_typ=_tmpDF,((_tmpF51.args=
_tmpE0,((_tmpF51.c_varargs=_tmpE1,((_tmpF51.cyc_varargs=cyc_varargs2,((_tmpF51.rgn_po=
_tmpE3,((_tmpF51.attributes=_tmpE4,_tmpF51)))))))))))))))),_tmpF52)))),_tmpF50))));};}
_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(struct Cyc_Absyn_TupleType_struct*)
_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmpF55;struct Cyc_Absyn_TupleType_struct*_tmpF54;return(void*)((_tmpF54=
_cycalloc(sizeof(*_tmpF54)),((_tmpF54[0]=((_tmpF55.tag=11,((_tmpF55.f1=((struct
Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),_tmpF55)))),_tmpF54))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmp9E=(struct Cyc_Absyn_AggrType_struct*)_tmp71;
if(_tmp9E->tag != 12)goto _LL3B;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((
_tmpA0.UnknownAggr).tag != 1)goto _LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;
_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmpF5B;struct Cyc_Absyn_AggrInfo _tmpF5A;struct Cyc_Absyn_AggrType_struct*
_tmpF59;return(void*)((_tmpF59=_cycalloc(sizeof(*_tmpF59)),((_tmpF59[0]=((
_tmpF5B.tag=12,((_tmpF5B.f1=((_tmpF5A.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmpF5A.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmpF5A)))),_tmpF5B)))),
_tmpF59))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(struct Cyc_Absyn_AggrType_struct*)
_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;
if((_tmpA8.KnownAggr).tag != 2)goto _LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmpF61;struct Cyc_Absyn_AggrInfo _tmpF60;struct Cyc_Absyn_AggrType_struct*_tmpF5F;
return(void*)((_tmpF5F=_cycalloc(sizeof(*_tmpF5F)),((_tmpF5F[0]=((_tmpF61.tag=12,((
_tmpF61.f1=((_tmpF60.aggr_info=Cyc_Absyn_KnownAggr(_tmpA9),((_tmpF60.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmpF60)))),_tmpF61)))),_tmpF5F))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmpF64;struct Cyc_Absyn_AnonAggrType_struct*_tmpF63;return(void*)((_tmpF63=
_cycalloc(sizeof(*_tmpF63)),((_tmpF63[0]=((_tmpF64.tag=13,((_tmpF64.f1=_tmpAC,((
_tmpF64.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpAD),_tmpF64)))))),
_tmpF63))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmpAE=(struct Cyc_Absyn_EnumType_struct*)
_tmp71;if(_tmpAE->tag != 14)goto _LL41;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}
_LL40: {struct Cyc_Absyn_EnumType_struct _tmpF67;struct Cyc_Absyn_EnumType_struct*
_tmpF66;return(void*)((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66[0]=((
_tmpF67.tag=14,((_tmpF67.f1=_tmpAF,((_tmpF67.f2=_tmpB0,_tmpF67)))))),_tmpF66))));}
_LL41: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp71;if(_tmpB1->tag != 15)goto _LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmpF6A;struct Cyc_Absyn_AnonEnumType_struct*_tmpF69;return(void*)((_tmpF69=
_cycalloc(sizeof(*_tmpF69)),((_tmpF69[0]=((_tmpF6A.tag=15,((_tmpF6A.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmpF6A)))),
_tmpF69))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmpF6D;struct Cyc_Absyn_TagType_struct*_tmpF6C;
return(void*)((_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6D.tag=20,((
_tmpF6D.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmpF6D)))),_tmpF6C))));}_LL45: {
struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmpF70;struct Cyc_Absyn_ValueofType_struct*_tmpF6F;return(void*)((_tmpF6F=
_cycalloc(sizeof(*_tmpF6F)),((_tmpF6F[0]=((_tmpF70.tag=19,((_tmpF70.f1=_tmpB6,
_tmpF70)))),_tmpF6F))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto _LL49;else{
_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct _tmpF73;
struct Cyc_Absyn_RgnHandleType_struct*_tmpF72;return(void*)((_tmpF72=_cycalloc(
sizeof(*_tmpF72)),((_tmpF72[0]=((_tmpF73.tag=16,((_tmpF73.f1=(void*)Cyc_Tcutil_copy_type(
_tmpB8),_tmpF73)))),_tmpF72))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmpF76;struct Cyc_Absyn_DynRgnType_struct*_tmpF75;return(void*)((_tmpF75=
_cycalloc(sizeof(*_tmpF75)),((_tmpF75[0]=((_tmpF76.tag=17,((_tmpF76.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmpF76.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmpF76)))))),_tmpF75))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmpF79;struct Cyc_Absyn_TypedefType_struct*_tmpF78;return(void*)((_tmpF78=
_cycalloc(sizeof(*_tmpF78)),((_tmpF78[0]=((_tmpF79.tag=18,((_tmpF79.f1=_tmpBD,((
_tmpF79.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmpF79.f3=_tmpBF,((_tmpF79.f4=0,
_tmpF79)))))))))),_tmpF78))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*_tmpC0=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}_LL4E:
goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmpF7C;
struct Cyc_Absyn_AccessEff_struct*_tmpF7B;return(void*)((_tmpF7B=_cycalloc(
sizeof(*_tmpF7B)),((_tmpF7B[0]=((_tmpF7C.tag=23,((_tmpF7C.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmpF7C)))),_tmpF7B))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmpF7F;struct Cyc_Absyn_JoinEff_struct*
_tmpF7E;return(void*)((_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((_tmpF7E[0]=((
_tmpF7F.tag=24,((_tmpF7F.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmpF7F)))),_tmpF7E))));}
_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmpF82;struct Cyc_Absyn_RgnsEff_struct*_tmpF81;
return(void*)((_tmpF81=_cycalloc(sizeof(*_tmpF81)),((_tmpF81[0]=((_tmpF82.tag=25,((
_tmpF82.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmpF82)))),_tmpF81))));}_LL20:;}
struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct
_tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2);int Cyc_Tcutil_kind_leq(struct
Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp108;enum 
Cyc_Absyn_KindQual _tmp109;enum Cyc_Absyn_AliasQual _tmp10A;struct Cyc_Absyn_Kind*
_tmp107=ka1;_tmp108=*_tmp107;_tmp109=_tmp108.kind;_tmp10A=_tmp108.aliasqual;{
struct Cyc_Absyn_Kind _tmp10C;enum Cyc_Absyn_KindQual _tmp10D;enum Cyc_Absyn_AliasQual
_tmp10E;struct Cyc_Absyn_Kind*_tmp10B=ka2;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.kind;
_tmp10E=_tmp10C.aliasqual;if(_tmp109 != _tmp10D){struct _tuple17 _tmpF83;struct
_tuple17 _tmp110=(_tmpF83.f1=_tmp109,((_tmpF83.f2=_tmp10D,_tmpF83)));enum Cyc_Absyn_KindQual
_tmp111;enum Cyc_Absyn_KindQual _tmp112;enum Cyc_Absyn_KindQual _tmp113;enum Cyc_Absyn_KindQual
_tmp114;enum Cyc_Absyn_KindQual _tmp115;enum Cyc_Absyn_KindQual _tmp116;_LL58:
_tmp111=_tmp110.f1;if(_tmp111 != Cyc_Absyn_BoxKind)goto _LL5A;_tmp112=_tmp110.f2;
if(_tmp112 != Cyc_Absyn_MemKind)goto _LL5A;_LL59: goto _LL5B;_LL5A: _tmp113=_tmp110.f1;
if(_tmp113 != Cyc_Absyn_BoxKind)goto _LL5C;_tmp114=_tmp110.f2;if(_tmp114 != Cyc_Absyn_AnyKind)
goto _LL5C;_LL5B: goto _LL5D;_LL5C: _tmp115=_tmp110.f1;if(_tmp115 != Cyc_Absyn_MemKind)
goto _LL5E;_tmp116=_tmp110.f2;if(_tmp116 != Cyc_Absyn_AnyKind)goto _LL5E;_LL5D: goto
_LL57;_LL5E:;_LL5F: return 0;_LL57:;}if(_tmp10A != _tmp10E){struct _tuple18 _tmpF84;
struct _tuple18 _tmp118=(_tmpF84.f1=_tmp10A,((_tmpF84.f2=_tmp10E,_tmpF84)));enum 
Cyc_Absyn_AliasQual _tmp119;enum Cyc_Absyn_AliasQual _tmp11A;enum Cyc_Absyn_AliasQual
_tmp11B;enum Cyc_Absyn_AliasQual _tmp11C;_LL61: _tmp119=_tmp118.f1;if(_tmp119 != 
Cyc_Absyn_Aliasable)goto _LL63;_tmp11A=_tmp118.f2;if(_tmp11A != Cyc_Absyn_Top)goto
_LL63;_LL62: goto _LL64;_LL63: _tmp11B=_tmp118.f1;if(_tmp11B != Cyc_Absyn_Unique)
goto _LL65;_tmp11C=_tmp118.f2;if(_tmp11C != Cyc_Absyn_Top)goto _LL65;_LL64: return 1;
_LL65:;_LL66: return 0;_LL60:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp11D=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp11F;struct Cyc_Absyn_Kind*_tmp121;_LL68: {
struct Cyc_Absyn_Eq_kb_struct*_tmp11E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11D;if(
_tmp11E->tag != 0)goto _LL6A;else{_tmp11F=_tmp11E->f1;}}_LL69: return _tmp11F;_LL6A: {
struct Cyc_Absyn_Less_kb_struct*_tmp120=(struct Cyc_Absyn_Less_kb_struct*)_tmp11D;
if(_tmp120->tag != 2)goto _LL6C;else{_tmp121=_tmp120->f2;}}_LL6B: return _tmp121;
_LL6C:;_LL6D:{struct Cyc_Absyn_Less_kb_struct _tmpF87;struct Cyc_Absyn_Less_kb_struct*
_tmpF86;tv->kind=(void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((
_tmpF87.tag=2,((_tmpF87.f1=0,((_tmpF87.f2=def,_tmpF87)))))),_tmpF86))));}return
def;_LL67:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2){struct _tuple0 _tmpF88;struct _tuple0 _tmp125=(_tmpF88.f1=Cyc_Absyn_compress_kb(
kb1),((_tmpF88.f2=Cyc_Absyn_compress_kb(kb2),_tmpF88)));void*_tmp126;struct Cyc_Absyn_Kind*
_tmp128;void*_tmp129;struct Cyc_Absyn_Kind*_tmp12B;void*_tmp12C;struct Cyc_Core_Opt*
_tmp12E;struct Cyc_Core_Opt**_tmp12F;struct Cyc_Absyn_Kind*_tmp130;void*_tmp131;
struct Cyc_Absyn_Kind*_tmp133;void*_tmp134;struct Cyc_Absyn_Kind*_tmp136;void*
_tmp137;struct Cyc_Core_Opt*_tmp139;struct Cyc_Core_Opt**_tmp13A;struct Cyc_Absyn_Kind*
_tmp13B;void*_tmp13C;struct Cyc_Core_Opt*_tmp13E;struct Cyc_Core_Opt**_tmp13F;
struct Cyc_Absyn_Kind*_tmp140;void*_tmp141;struct Cyc_Core_Opt*_tmp143;struct Cyc_Core_Opt**
_tmp144;struct Cyc_Absyn_Kind*_tmp145;void*_tmp146;struct Cyc_Core_Opt*_tmp148;
struct Cyc_Core_Opt**_tmp149;void*_tmp14A;void*_tmp14B;void*_tmp14C;struct Cyc_Core_Opt*
_tmp14E;struct Cyc_Core_Opt**_tmp14F;_LL6F: _tmp126=_tmp125.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp127=(struct Cyc_Absyn_Eq_kb_struct*)_tmp126;if(_tmp127->tag != 0)goto _LL71;
else{_tmp128=_tmp127->f1;}};_tmp129=_tmp125.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp12A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp129;if(_tmp12A->tag != 0)goto _LL71;
else{_tmp12B=_tmp12A->f1;}};_LL70: return _tmp128 == _tmp12B;_LL71: _tmp12C=_tmp125.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp12D=(struct Cyc_Absyn_Less_kb_struct*)_tmp12C;
if(_tmp12D->tag != 2)goto _LL73;else{_tmp12E=_tmp12D->f1;_tmp12F=(struct Cyc_Core_Opt**)&
_tmp12D->f1;_tmp130=_tmp12D->f2;}};_tmp131=_tmp125.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp132=(struct Cyc_Absyn_Eq_kb_struct*)_tmp131;if(_tmp132->tag != 0)goto _LL73;
else{_tmp133=_tmp132->f1;}};_LL72: if(Cyc_Tcutil_kind_leq(_tmp133,_tmp130)){{void**
_tmpF89;*_tmp12F=(struct Cyc_Core_Opt*)((_tmpF89=_cycalloc(sizeof(*_tmpF89)),((
_tmpF89[0]=kb2,_tmpF89))));}return 1;}else{return 0;}_LL73: _tmp134=_tmp125.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp135=(struct Cyc_Absyn_Eq_kb_struct*)_tmp134;if(
_tmp135->tag != 0)goto _LL75;else{_tmp136=_tmp135->f1;}};_tmp137=_tmp125.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp138=(struct Cyc_Absyn_Less_kb_struct*)_tmp137;
if(_tmp138->tag != 2)goto _LL75;else{_tmp139=_tmp138->f1;_tmp13A=(struct Cyc_Core_Opt**)&
_tmp138->f1;_tmp13B=_tmp138->f2;}};_LL74: if(Cyc_Tcutil_kind_leq(_tmp136,_tmp13B)){{
void**_tmpF8A;*_tmp13A=(struct Cyc_Core_Opt*)((_tmpF8A=_cycalloc(sizeof(*_tmpF8A)),((
_tmpF8A[0]=kb1,_tmpF8A))));}return 1;}else{return 0;}_LL75: _tmp13C=_tmp125.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp13D=(struct Cyc_Absyn_Less_kb_struct*)_tmp13C;
if(_tmp13D->tag != 2)goto _LL77;else{_tmp13E=_tmp13D->f1;_tmp13F=(struct Cyc_Core_Opt**)&
_tmp13D->f1;_tmp140=_tmp13D->f2;}};_tmp141=_tmp125.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp142=(struct Cyc_Absyn_Less_kb_struct*)_tmp141;if(_tmp142->tag != 2)goto _LL77;
else{_tmp143=_tmp142->f1;_tmp144=(struct Cyc_Core_Opt**)& _tmp142->f1;_tmp145=
_tmp142->f2;}};_LL76: if(Cyc_Tcutil_kind_leq(_tmp140,_tmp145)){{void**_tmpF8B;*
_tmp144=(struct Cyc_Core_Opt*)((_tmpF8B=_cycalloc(sizeof(*_tmpF8B)),((_tmpF8B[0]=
kb1,_tmpF8B))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp145,_tmp140)){{void**
_tmpF8C;*_tmp13F=(struct Cyc_Core_Opt*)((_tmpF8C=_cycalloc(sizeof(*_tmpF8C)),((
_tmpF8C[0]=kb2,_tmpF8C))));}return 1;}else{return 0;}}_LL77: _tmp146=_tmp125.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp147=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp146;if(_tmp147->tag != 1)goto _LL79;else{_tmp148=_tmp147->f1;_tmp149=(struct
Cyc_Core_Opt**)& _tmp147->f1;}};_tmp14A=_tmp125.f2;_LL78: _tmp14B=_tmp14A;_tmp14F=
_tmp149;goto _LL7A;_LL79: _tmp14B=_tmp125.f1;_tmp14C=_tmp125.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp14D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp14C;if(_tmp14D->tag != 1)goto
_LL6E;else{_tmp14E=_tmp14D->f1;_tmp14F=(struct Cyc_Core_Opt**)& _tmp14D->f1;}};
_LL7A:{void**_tmpF8D;*_tmp14F=(struct Cyc_Core_Opt*)((_tmpF8D=_cycalloc(sizeof(*
_tmpF8D)),((_tmpF8D[0]=_tmp14B,_tmpF8D))));}return 1;_LL6E:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp155=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp157;_LL7C: {struct Cyc_Absyn_VarType_struct*
_tmp156=(struct Cyc_Absyn_VarType_struct*)_tmp155;if(_tmp156->tag != 2)goto _LL7E;
else{_tmp157=_tmp156->f1;}}_LL7D: {void*_tmp158=_tmp157->kind;_tmp157->kind=kb;{
struct _tuple14 _tmpF8E;return(_tmpF8E.f1=_tmp157,((_tmpF8E.f2=_tmp158,_tmpF8E)));};}
_LL7E:;_LL7F: {const char*_tmpF92;void*_tmpF91[1];struct Cyc_String_pa_struct
_tmpF90;(_tmpF90.tag=0,((_tmpF90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF91[0]=& _tmpF90,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF92="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpF92,sizeof(char),40))),_tag_dyneither(_tmpF91,sizeof(void*),1)))))));}
_LL7B:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);struct Cyc_Absyn_Kind*
Cyc_Tcutil_typ_kind(void*t){void*_tmp15D=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;enum Cyc_Absyn_Size_of _tmp164;struct Cyc_Absyn_DatatypeFieldInfo
_tmp170;union Cyc_Absyn_DatatypeFieldInfoU _tmp171;struct _tuple3 _tmp172;struct Cyc_Absyn_Datatypedecl*
_tmp173;struct Cyc_Absyn_Datatypefield*_tmp174;struct Cyc_Absyn_DatatypeFieldInfo
_tmp176;union Cyc_Absyn_DatatypeFieldInfoU _tmp177;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp178;struct Cyc_Absyn_AggrInfo _tmp17A;union Cyc_Absyn_AggrInfoU _tmp17B;struct
_tuple4 _tmp17C;struct Cyc_Absyn_AggrInfo _tmp17E;union Cyc_Absyn_AggrInfoU _tmp17F;
struct Cyc_Absyn_Aggrdecl**_tmp180;struct Cyc_Absyn_Aggrdecl*_tmp181;struct Cyc_Absyn_Aggrdecl
_tmp182;struct Cyc_Absyn_AggrdeclImpl*_tmp183;struct Cyc_Absyn_PtrInfo _tmp186;
struct Cyc_Absyn_Typedefdecl*_tmp18C;_LL81: {struct Cyc_Absyn_Evar_struct*_tmp15E=(
struct Cyc_Absyn_Evar_struct*)_tmp15D;if(_tmp15E->tag != 1)goto _LL83;else{_tmp15F=
_tmp15E->f1;}}_LL82: return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp15F))->v;_LL83: {struct Cyc_Absyn_VarType_struct*_tmp160=(struct
Cyc_Absyn_VarType_struct*)_tmp15D;if(_tmp160->tag != 2)goto _LL85;else{_tmp161=
_tmp160->f1;}}_LL84: return Cyc_Tcutil_tvar_kind(_tmp161,& Cyc_Tcutil_bk);_LL85: {
struct Cyc_Absyn_VoidType_struct*_tmp162=(struct Cyc_Absyn_VoidType_struct*)
_tmp15D;if(_tmp162->tag != 0)goto _LL87;}_LL86: return& Cyc_Tcutil_mk;_LL87: {struct
Cyc_Absyn_IntType_struct*_tmp163=(struct Cyc_Absyn_IntType_struct*)_tmp15D;if(
_tmp163->tag != 6)goto _LL89;else{_tmp164=_tmp163->f2;}}_LL88: return(_tmp164 == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp164 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LL89: {struct Cyc_Absyn_FloatType_struct*_tmp165=(
struct Cyc_Absyn_FloatType_struct*)_tmp15D;if(_tmp165->tag != 7)goto _LL8B;}_LL8A:
goto _LL8C;_LL8B: {struct Cyc_Absyn_DoubleType_struct*_tmp166=(struct Cyc_Absyn_DoubleType_struct*)
_tmp15D;if(_tmp166->tag != 8)goto _LL8D;}_LL8C: goto _LL8E;_LL8D: {struct Cyc_Absyn_FnType_struct*
_tmp167=(struct Cyc_Absyn_FnType_struct*)_tmp15D;if(_tmp167->tag != 10)goto _LL8F;}
_LL8E: return& Cyc_Tcutil_mk;_LL8F: {struct Cyc_Absyn_DynRgnType_struct*_tmp168=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp15D;if(_tmp168->tag != 17)goto _LL91;}_LL90:
goto _LL92;_LL91: {struct Cyc_Absyn_EnumType_struct*_tmp169=(struct Cyc_Absyn_EnumType_struct*)
_tmp15D;if(_tmp169->tag != 14)goto _LL93;}_LL92: goto _LL94;_LL93: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp16A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp15D;if(_tmp16A->tag != 15)goto
_LL95;}_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_RgnHandleType_struct*_tmp16B=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15D;if(_tmp16B->tag != 16)goto _LL97;}
_LL96: return& Cyc_Tcutil_bk;_LL97: {struct Cyc_Absyn_UniqueRgn_struct*_tmp16C=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp15D;if(_tmp16C->tag != 22)goto _LL99;}_LL98:
return& Cyc_Tcutil_urk;_LL99: {struct Cyc_Absyn_HeapRgn_struct*_tmp16D=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp15D;if(_tmp16D->tag != 21)goto _LL9B;}_LL9A: return& Cyc_Tcutil_rk;_LL9B: {
struct Cyc_Absyn_DatatypeType_struct*_tmp16E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp15D;if(_tmp16E->tag != 3)goto _LL9D;}_LL9C: return& Cyc_Tcutil_ak;_LL9D: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp16F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15D;if(_tmp16F->tag != 4)goto _LL9F;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.field_info;
if((_tmp171.KnownDatatypefield).tag != 2)goto _LL9F;_tmp172=(struct _tuple3)(
_tmp171.KnownDatatypefield).val;_tmp173=_tmp172.f1;_tmp174=_tmp172.f2;}}_LL9E:
return& Cyc_Tcutil_mk;_LL9F: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp175=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15D;if(_tmp175->tag != 4)goto _LLA1;
else{_tmp176=_tmp175->f1;_tmp177=_tmp176.field_info;if((_tmp177.UnknownDatatypefield).tag
!= 1)goto _LLA1;_tmp178=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp177.UnknownDatatypefield).val;}}
_LLA0: {const char*_tmpF95;void*_tmpF94;(_tmpF94=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF95="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpF95,sizeof(char),39))),_tag_dyneither(_tmpF94,sizeof(void*),0)));}
_LLA1: {struct Cyc_Absyn_AggrType_struct*_tmp179=(struct Cyc_Absyn_AggrType_struct*)
_tmp15D;if(_tmp179->tag != 12)goto _LLA3;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.aggr_info;
if((_tmp17B.UnknownAggr).tag != 1)goto _LLA3;_tmp17C=(struct _tuple4)(_tmp17B.UnknownAggr).val;}}
_LLA2: return& Cyc_Tcutil_ak;_LLA3: {struct Cyc_Absyn_AggrType_struct*_tmp17D=(
struct Cyc_Absyn_AggrType_struct*)_tmp15D;if(_tmp17D->tag != 12)goto _LLA5;else{
_tmp17E=_tmp17D->f1;_tmp17F=_tmp17E.aggr_info;if((_tmp17F.KnownAggr).tag != 2)
goto _LLA5;_tmp180=(struct Cyc_Absyn_Aggrdecl**)(_tmp17F.KnownAggr).val;_tmp181=*
_tmp180;_tmp182=*_tmp181;_tmp183=_tmp182.impl;}}_LLA4: return _tmp183 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LLA5: {struct Cyc_Absyn_AnonAggrType_struct*_tmp184=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15D;if(_tmp184->tag != 13)goto _LLA7;}_LLA6: return& Cyc_Tcutil_mk;_LLA7: {
struct Cyc_Absyn_PointerType_struct*_tmp185=(struct Cyc_Absyn_PointerType_struct*)
_tmp15D;if(_tmp185->tag != 5)goto _LLA9;else{_tmp186=_tmp185->f1;}}_LLA8: {void*
_tmp192=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp186.ptr_atts).bounds);_LLBA: {struct Cyc_Absyn_DynEither_b_struct*
_tmp193=(struct Cyc_Absyn_DynEither_b_struct*)_tmp192;if(_tmp193->tag != 0)goto
_LLBC;}_LLBB: return& Cyc_Tcutil_mk;_LLBC: {struct Cyc_Absyn_Upper_b_struct*_tmp194=(
struct Cyc_Absyn_Upper_b_struct*)_tmp192;if(_tmp194->tag != 1)goto _LLB9;}_LLBD:
return& Cyc_Tcutil_bk;_LLB9:;}_LLA9: {struct Cyc_Absyn_ValueofType_struct*_tmp187=(
struct Cyc_Absyn_ValueofType_struct*)_tmp15D;if(_tmp187->tag != 19)goto _LLAB;}
_LLAA: return& Cyc_Tcutil_ik;_LLAB: {struct Cyc_Absyn_TagType_struct*_tmp188=(
struct Cyc_Absyn_TagType_struct*)_tmp15D;if(_tmp188->tag != 20)goto _LLAD;}_LLAC:
return& Cyc_Tcutil_bk;_LLAD: {struct Cyc_Absyn_ArrayType_struct*_tmp189=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15D;if(_tmp189->tag != 9)goto _LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_TupleType_struct*
_tmp18A=(struct Cyc_Absyn_TupleType_struct*)_tmp15D;if(_tmp18A->tag != 11)goto
_LLB1;}_LLB0: return& Cyc_Tcutil_mk;_LLB1: {struct Cyc_Absyn_TypedefType_struct*
_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)_tmp15D;if(_tmp18B->tag != 18)goto
_LLB3;else{_tmp18C=_tmp18B->f3;}}_LLB2: if(_tmp18C == 0  || _tmp18C->kind == 0){
const char*_tmpF99;void*_tmpF98[1];struct Cyc_String_pa_struct _tmpF97;(_tmpF97.tag=
0,((_tmpF97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpF98[0]=& _tmpF97,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpF99="typ_kind: typedef found: %s",_tag_dyneither(
_tmpF99,sizeof(char),28))),_tag_dyneither(_tmpF98,sizeof(void*),1)))))));}return(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp18C->kind))->v;_LLB3: {
struct Cyc_Absyn_AccessEff_struct*_tmp18D=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15D;if(_tmp18D->tag != 23)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_JoinEff_struct*
_tmp18E=(struct Cyc_Absyn_JoinEff_struct*)_tmp15D;if(_tmp18E->tag != 24)goto _LLB7;}
_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_RgnsEff_struct*_tmp18F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15D;if(_tmp18F->tag != 25)goto _LL80;}_LLB8: return& Cyc_Tcutil_ek;_LL80:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);int Cyc_Tcutil_kind_eq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == k2  || k1->kind == k2->kind
 && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp198;_push_handler(& _tmp198);{int _tmp19A=
0;if(setjmp(_tmp198.handler))_tmp19A=1;if(!_tmp19A){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp19B=1;_npop_handler(0);return _tmp19B;};;_pop_handler();}else{void*_tmp199=(
void*)_exn_thrown;void*_tmp19D=_tmp199;_LLBF: {struct Cyc_Tcutil_Unify_struct*
_tmp19E=(struct Cyc_Tcutil_Unify_struct*)_tmp19D;if(_tmp19E->tag != Cyc_Tcutil_Unify)
goto _LLC1;}_LLC0: return 0;_LLC1:;_LLC2:(void)_throw(_tmp19D);_LLBE:;}};}static
void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp19F=t;struct Cyc_Absyn_Tvar*_tmp1A1;struct Cyc_Core_Opt*_tmp1A3;
struct Cyc_Core_Opt*_tmp1A4;struct Cyc_Core_Opt**_tmp1A5;struct Cyc_Absyn_PtrInfo
_tmp1A7;struct Cyc_Absyn_ArrayInfo _tmp1A9;void*_tmp1AA;struct Cyc_Absyn_FnInfo
_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;void*_tmp1AF;
struct Cyc_List_List*_tmp1B0;int _tmp1B1;struct Cyc_Absyn_VarargInfo*_tmp1B2;struct
Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B6;
struct Cyc_Absyn_DatatypeInfo _tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*
_tmp1BB;struct Cyc_Absyn_DatatypeFieldInfo _tmp1BD;struct Cyc_List_List*_tmp1BE;
struct Cyc_Absyn_AggrInfo _tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*
_tmp1C3;void*_tmp1C5;void*_tmp1C7;void*_tmp1C9;void*_tmp1CB;struct Cyc_List_List*
_tmp1CD;_LLC4: {struct Cyc_Absyn_VarType_struct*_tmp1A0=(struct Cyc_Absyn_VarType_struct*)
_tmp19F;if(_tmp1A0->tag != 2)goto _LLC6;else{_tmp1A1=_tmp1A0->f1;}}_LLC5: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1A1)){{
const char*_tmpF9A;Cyc_Tcutil_failure_reason=((_tmpF9A="(type variable would escape scope)",
_tag_dyneither(_tmpF9A,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LLC3;_LLC6: {struct Cyc_Absyn_Evar_struct*_tmp1A2=(struct Cyc_Absyn_Evar_struct*)
_tmp19F;if(_tmp1A2->tag != 1)goto _LLC8;else{_tmp1A3=_tmp1A2->f2;_tmp1A4=_tmp1A2->f4;
_tmp1A5=(struct Cyc_Core_Opt**)& _tmp1A2->f4;}}_LLC7: if(t == evar){{const char*
_tmpF9B;Cyc_Tcutil_failure_reason=((_tmpF9B="(occurs check)",_tag_dyneither(
_tmpF9B,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp1A3 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1A3->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp1A5))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp1D0=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp1A5))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmpF9C;_tmp1D0=((_tmpF9C=_cycalloc(sizeof(*_tmpF9C)),((
_tmpF9C->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF9C->tl=_tmp1D0,_tmpF9C))))));}}}{
struct Cyc_Core_Opt*_tmpF9D;*_tmp1A5=((_tmpF9D=_cycalloc(sizeof(*_tmpF9D)),((
_tmpF9D->v=_tmp1D0,_tmpF9D))));};}}}goto _LLC3;_LLC8: {struct Cyc_Absyn_PointerType_struct*
_tmp1A6=(struct Cyc_Absyn_PointerType_struct*)_tmp19F;if(_tmp1A6->tag != 5)goto
_LLCA;else{_tmp1A7=_tmp1A6->f1;}}_LLC9: Cyc_Tcutil_occurs(evar,r,env,_tmp1A7.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1A7.ptr_atts).rgn);goto _LLC3;_LLCA: {struct Cyc_Absyn_ArrayType_struct*
_tmp1A8=(struct Cyc_Absyn_ArrayType_struct*)_tmp19F;if(_tmp1A8->tag != 9)goto _LLCC;
else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.elt_type;}}_LLCB: Cyc_Tcutil_occurs(evar,
r,env,_tmp1AA);goto _LLC3;_LLCC: {struct Cyc_Absyn_FnType_struct*_tmp1AB=(struct
Cyc_Absyn_FnType_struct*)_tmp19F;if(_tmp1AB->tag != 10)goto _LLCE;else{_tmp1AC=
_tmp1AB->f1;_tmp1AD=_tmp1AC.tvars;_tmp1AE=_tmp1AC.effect;_tmp1AF=_tmp1AC.ret_typ;
_tmp1B0=_tmp1AC.args;_tmp1B1=_tmp1AC.c_varargs;_tmp1B2=_tmp1AC.cyc_varargs;
_tmp1B3=_tmp1AC.rgn_po;_tmp1B4=_tmp1AC.attributes;}}_LLCD: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp1AD,env);if(_tmp1AE != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1AE->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp1AF);for(0;_tmp1B0 != 0;_tmp1B0=_tmp1B0->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp1B0->hd)).f3);}if(_tmp1B2 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp1B2->type);for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){struct _tuple0
_tmp1D4;void*_tmp1D5;void*_tmp1D6;struct _tuple0*_tmp1D3=(struct _tuple0*)_tmp1B3->hd;
_tmp1D4=*_tmp1D3;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp1D5);Cyc_Tcutil_occurs(evar,r,env,_tmp1D6);}goto _LLC3;_LLCE: {struct Cyc_Absyn_TupleType_struct*
_tmp1B5=(struct Cyc_Absyn_TupleType_struct*)_tmp19F;if(_tmp1B5->tag != 11)goto
_LLD0;else{_tmp1B6=_tmp1B5->f1;}}_LLCF: for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1B6->hd)).f2);}goto _LLC3;
_LLD0: {struct Cyc_Absyn_DatatypeType_struct*_tmp1B7=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp19F;if(_tmp1B7->tag != 3)goto _LLD2;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B8.targs;}}
_LLD1: Cyc_Tcutil_occurslist(evar,r,env,_tmp1B9);goto _LLC3;_LLD2: {struct Cyc_Absyn_TypedefType_struct*
_tmp1BA=(struct Cyc_Absyn_TypedefType_struct*)_tmp19F;if(_tmp1BA->tag != 18)goto
_LLD4;else{_tmp1BB=_tmp1BA->f2;}}_LLD3: _tmp1BE=_tmp1BB;goto _LLD5;_LLD4: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp1BC=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp19F;if(_tmp1BC->tag != 4)goto _LLD6;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BD.targs;}}
_LLD5: _tmp1C1=_tmp1BE;goto _LLD7;_LLD6: {struct Cyc_Absyn_AggrType_struct*_tmp1BF=(
struct Cyc_Absyn_AggrType_struct*)_tmp19F;if(_tmp1BF->tag != 12)goto _LLD8;else{
_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1C0.targs;}}_LLD7: Cyc_Tcutil_occurslist(evar,r,
env,_tmp1C1);goto _LLC3;_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1C2=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp19F;if(_tmp1C2->tag != 13)goto _LLDA;
else{_tmp1C3=_tmp1C2->f2;}}_LLD9: for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){Cyc_Tcutil_occurs(
evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1C3->hd)->type);}goto _LLC3;_LLDA: {
struct Cyc_Absyn_TagType_struct*_tmp1C4=(struct Cyc_Absyn_TagType_struct*)_tmp19F;
if(_tmp1C4->tag != 20)goto _LLDC;else{_tmp1C5=(void*)_tmp1C4->f1;}}_LLDB: _tmp1C7=
_tmp1C5;goto _LLDD;_LLDC: {struct Cyc_Absyn_AccessEff_struct*_tmp1C6=(struct Cyc_Absyn_AccessEff_struct*)
_tmp19F;if(_tmp1C6->tag != 23)goto _LLDE;else{_tmp1C7=(void*)_tmp1C6->f1;}}_LLDD:
_tmp1C9=_tmp1C7;goto _LLDF;_LLDE: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C8=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp19F;if(_tmp1C8->tag != 16)goto _LLE0;
else{_tmp1C9=(void*)_tmp1C8->f1;}}_LLDF: _tmp1CB=_tmp1C9;goto _LLE1;_LLE0: {struct
Cyc_Absyn_RgnsEff_struct*_tmp1CA=(struct Cyc_Absyn_RgnsEff_struct*)_tmp19F;if(
_tmp1CA->tag != 25)goto _LLE2;else{_tmp1CB=(void*)_tmp1CA->f1;}}_LLE1: Cyc_Tcutil_occurs(
evar,r,env,_tmp1CB);goto _LLC3;_LLE2: {struct Cyc_Absyn_JoinEff_struct*_tmp1CC=(
struct Cyc_Absyn_JoinEff_struct*)_tmp19F;if(_tmp1CC->tag != 24)goto _LLE4;else{
_tmp1CD=_tmp1CC->f1;}}_LLE3: Cyc_Tcutil_occurslist(evar,r,env,_tmp1CD);goto _LLC3;
_LLE4:;_LLE5: goto _LLC3;_LLC3:;};}static void Cyc_Tcutil_occurslist(void*evar,
struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2);static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2);static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,
struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const
char*_tmpFA0;void*_tmpF9F;(_tmpF9F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA0="tq1 real_const not set.",
_tag_dyneither(_tmpFA0,sizeof(char),24))),_tag_dyneither(_tmpF9F,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpFA3;void*_tmpFA2;(_tmpFA2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFA3="tq2 real_const not set.",_tag_dyneither(_tmpFA3,sizeof(char),24))),
_tag_dyneither(_tmpFA2,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpFA4;Cyc_Tcutil_failure_reason=((_tmpFA4="(qualifiers don't match)",
_tag_dyneither(_tmpFA4,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp1DC=x;union Cyc_Absyn_Constraint
_tmp1DD;int _tmp1DE;union Cyc_Absyn_Constraint _tmp1DF;void*_tmp1E0;union Cyc_Absyn_Constraint
_tmp1E1;union Cyc_Absyn_Constraint*_tmp1E2;_LLE7: _tmp1DD=*_tmp1DC;if((_tmp1DD.No_constr).tag
!= 3)goto _LLE9;_tmp1DE=(int)(_tmp1DD.No_constr).val;_LLE8:{union Cyc_Absyn_Constraint
_tmpFA5;*x=(((_tmpFA5.Forward_constr).val=y,(((_tmpFA5.Forward_constr).tag=2,
_tmpFA5))));}return;_LLE9: _tmp1DF=*_tmp1DC;if((_tmp1DF.Eq_constr).tag != 1)goto
_LLEB;_tmp1E0=(void*)(_tmp1DF.Eq_constr).val;_LLEA: {union Cyc_Absyn_Constraint*
_tmp1E4=y;union Cyc_Absyn_Constraint _tmp1E5;int _tmp1E6;union Cyc_Absyn_Constraint
_tmp1E7;void*_tmp1E8;union Cyc_Absyn_Constraint _tmp1E9;union Cyc_Absyn_Constraint*
_tmp1EA;_LLEE: _tmp1E5=*_tmp1E4;if((_tmp1E5.No_constr).tag != 3)goto _LLF0;_tmp1E6=(
int)(_tmp1E5.No_constr).val;_LLEF:*y=*x;return;_LLF0: _tmp1E7=*_tmp1E4;if((
_tmp1E7.Eq_constr).tag != 1)goto _LLF2;_tmp1E8=(void*)(_tmp1E7.Eq_constr).val;
_LLF1: if(cmp(_tmp1E0,_tmp1E8)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LLF2: _tmp1E9=*_tmp1E4;if((_tmp1E9.Forward_constr).tag
!= 2)goto _LLED;_tmp1EA=(union Cyc_Absyn_Constraint*)(_tmp1E9.Forward_constr).val;
_LLF3: {const char*_tmpFA8;void*_tmpFA7;(_tmpFA7=0,Cyc_Tcutil_impos(((_tmpFA8="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpFA8,sizeof(char),40))),_tag_dyneither(_tmpFA7,sizeof(void*),0)));}
_LLED:;}_LLEB: _tmp1E1=*_tmp1DC;if((_tmp1E1.Forward_constr).tag != 2)goto _LLE6;
_tmp1E2=(union Cyc_Absyn_Constraint*)(_tmp1E1.Forward_constr).val;_LLEC: {const
char*_tmpFAB;void*_tmpFAA;(_tmpFAA=0,Cyc_Tcutil_impos(((_tmpFAB="unify_conref: forward after compress",
_tag_dyneither(_tmpFAB,sizeof(char),37))),_tag_dyneither(_tmpFAA,sizeof(void*),0)));}
_LLE6:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp1EF;_push_handler(& _tmp1EF);{int _tmp1F1=0;if(setjmp(_tmp1EF.handler))
_tmp1F1=1;if(!_tmp1F1){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp1F2=1;_npop_handler(0);return _tmp1F2;};;
_pop_handler();}else{void*_tmp1F0=(void*)_exn_thrown;void*_tmp1F4=_tmp1F0;_LLF5: {
struct Cyc_Tcutil_Unify_struct*_tmp1F5=(struct Cyc_Tcutil_Unify_struct*)_tmp1F4;
if(_tmp1F5->tag != Cyc_Tcutil_Unify)goto _LLF7;}_LLF6: return 0;_LLF7:;_LLF8:(void)
_throw(_tmp1F4);_LLF4:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static
int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmpFAC;struct _tuple0
_tmp1F7=(_tmpFAC.f1=b1,((_tmpFAC.f2=b2,_tmpFAC)));void*_tmp1F8;void*_tmp1FA;void*
_tmp1FC;void*_tmp1FE;void*_tmp200;struct Cyc_Absyn_Exp*_tmp202;void*_tmp203;
struct Cyc_Absyn_Exp*_tmp205;_LLFA: _tmp1F8=_tmp1F7.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1F9=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1F8;if(_tmp1F9->tag != 0)goto
_LLFC;};_tmp1FA=_tmp1F7.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FB=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp1FA;if(_tmp1FB->tag != 0)goto _LLFC;};_LLFB:
return 0;_LLFC: _tmp1FC=_tmp1F7.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FD=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1FC;if(_tmp1FD->tag != 0)goto _LLFE;};
_LLFD: return - 1;_LLFE: _tmp1FE=_tmp1F7.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1FF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1FE;if(_tmp1FF->tag != 0)goto
_LL100;};_LLFF: return 1;_LL100: _tmp200=_tmp1F7.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp201=(struct Cyc_Absyn_Upper_b_struct*)_tmp200;if(_tmp201->tag != 1)goto _LLF9;
else{_tmp202=_tmp201->f1;}};_tmp203=_tmp1F7.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp204=(struct Cyc_Absyn_Upper_b_struct*)_tmp203;if(_tmp204->tag != 1)goto _LLF9;
else{_tmp205=_tmp204->f1;}};_LL101: return Cyc_Evexp_const_exp_cmp(_tmp202,_tmp205);
_LLF9:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmpFAD;struct _tuple0 _tmp207=(_tmpFAD.f1=b1,((
_tmpFAD.f2=b2,_tmpFAD)));void*_tmp208;void*_tmp20A;void*_tmp20C;void*_tmp20E;
void*_tmp210;struct Cyc_Absyn_Exp*_tmp212;void*_tmp213;struct Cyc_Absyn_Exp*
_tmp215;_LL103: _tmp208=_tmp207.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp209=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp208;if(_tmp209->tag != 0)goto _LL105;};
_tmp20A=_tmp207.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp20B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp20A;if(_tmp20B->tag != 0)goto _LL105;};_LL104: return 0;_LL105: _tmp20C=_tmp207.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp20D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp20C;if(_tmp20D->tag != 0)goto _LL107;};_LL106: return - 1;_LL107: _tmp20E=_tmp207.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp20F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp20E;if(_tmp20F->tag != 0)goto _LL109;};_LL108: return 1;_LL109: _tmp210=_tmp207.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp211=(struct Cyc_Absyn_Upper_b_struct*)_tmp210;
if(_tmp211->tag != 1)goto _LL102;else{_tmp212=_tmp211->f1;}};_tmp213=_tmp207.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp214=(struct Cyc_Absyn_Upper_b_struct*)_tmp213;
if(_tmp214->tag != 1)goto _LL102;else{_tmp215=_tmp214->f1;}};_LL10A: return Cyc_Evexp_const_exp_cmp(
_tmp212,_tmp215);_LL102:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp216=att;_LL10C: {
struct Cyc_Absyn_Regparm_att_struct*_tmp217=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp216;if(_tmp217->tag != 0)goto _LL10E;}_LL10D: return 0;_LL10E: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp218=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp216;if(_tmp218->tag != 1)goto
_LL110;}_LL10F: return 1;_LL110: {struct Cyc_Absyn_Cdecl_att_struct*_tmp219=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp216;if(_tmp219->tag != 2)goto _LL112;}_LL111:
return 2;_LL112: {struct Cyc_Absyn_Fastcall_att_struct*_tmp21A=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp216;if(_tmp21A->tag != 3)goto _LL114;}_LL113: return 3;_LL114: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp21B=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp216;if(_tmp21B->tag != 4)goto
_LL116;}_LL115: return 4;_LL116: {struct Cyc_Absyn_Const_att_struct*_tmp21C=(struct
Cyc_Absyn_Const_att_struct*)_tmp216;if(_tmp21C->tag != 5)goto _LL118;}_LL117:
return 5;_LL118: {struct Cyc_Absyn_Aligned_att_struct*_tmp21D=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp216;if(_tmp21D->tag != 6)goto _LL11A;}_LL119: return 6;_LL11A: {struct Cyc_Absyn_Packed_att_struct*
_tmp21E=(struct Cyc_Absyn_Packed_att_struct*)_tmp216;if(_tmp21E->tag != 7)goto
_LL11C;}_LL11B: return 7;_LL11C: {struct Cyc_Absyn_Section_att_struct*_tmp21F=(
struct Cyc_Absyn_Section_att_struct*)_tmp216;if(_tmp21F->tag != 8)goto _LL11E;}
_LL11D: return 8;_LL11E: {struct Cyc_Absyn_Nocommon_att_struct*_tmp220=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp216;if(_tmp220->tag != 9)goto _LL120;}_LL11F: return 9;_LL120: {struct Cyc_Absyn_Shared_att_struct*
_tmp221=(struct Cyc_Absyn_Shared_att_struct*)_tmp216;if(_tmp221->tag != 10)goto
_LL122;}_LL121: return 10;_LL122: {struct Cyc_Absyn_Unused_att_struct*_tmp222=(
struct Cyc_Absyn_Unused_att_struct*)_tmp216;if(_tmp222->tag != 11)goto _LL124;}
_LL123: return 11;_LL124: {struct Cyc_Absyn_Weak_att_struct*_tmp223=(struct Cyc_Absyn_Weak_att_struct*)
_tmp216;if(_tmp223->tag != 12)goto _LL126;}_LL125: return 12;_LL126: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp224=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp216;if(_tmp224->tag != 13)goto
_LL128;}_LL127: return 13;_LL128: {struct Cyc_Absyn_Dllexport_att_struct*_tmp225=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp216;if(_tmp225->tag != 14)goto _LL12A;}
_LL129: return 14;_LL12A: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp226=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp216;if(_tmp226->tag
!= 15)goto _LL12C;}_LL12B: return 15;_LL12C: {struct Cyc_Absyn_Constructor_att_struct*
_tmp227=(struct Cyc_Absyn_Constructor_att_struct*)_tmp216;if(_tmp227->tag != 16)
goto _LL12E;}_LL12D: return 16;_LL12E: {struct Cyc_Absyn_Destructor_att_struct*
_tmp228=(struct Cyc_Absyn_Destructor_att_struct*)_tmp216;if(_tmp228->tag != 17)
goto _LL130;}_LL12F: return 17;_LL130: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp229=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp216;if(_tmp229->tag
!= 18)goto _LL132;}_LL131: return 18;_LL132: {struct Cyc_Absyn_Format_att_struct*
_tmp22A=(struct Cyc_Absyn_Format_att_struct*)_tmp216;if(_tmp22A->tag != 19)goto
_LL134;}_LL133: return 19;_LL134: {struct Cyc_Absyn_Initializes_att_struct*_tmp22B=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp216;if(_tmp22B->tag != 20)goto _LL136;}
_LL135: return 20;_LL136:;_LL137: return 21;_LL10B:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpFAE;struct _tuple0 _tmp22D=(_tmpFAE.f1=att1,((_tmpFAE.f2=att2,
_tmpFAE)));void*_tmp22E;int _tmp230;void*_tmp231;int _tmp233;void*_tmp234;int
_tmp236;void*_tmp237;int _tmp239;void*_tmp23A;int _tmp23C;void*_tmp23D;int _tmp23F;
void*_tmp240;struct _dyneither_ptr _tmp242;void*_tmp243;struct _dyneither_ptr
_tmp245;void*_tmp246;enum Cyc_Absyn_Format_Type _tmp248;int _tmp249;int _tmp24A;
void*_tmp24B;enum Cyc_Absyn_Format_Type _tmp24D;int _tmp24E;int _tmp24F;_LL139:
_tmp22E=_tmp22D.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp22F=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp22E;if(_tmp22F->tag != 0)goto _LL13B;else{_tmp230=_tmp22F->f1;}};_tmp231=
_tmp22D.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp232=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp231;if(_tmp232->tag != 0)goto _LL13B;else{_tmp233=_tmp232->f1;}};_LL13A:
_tmp236=_tmp230;_tmp239=_tmp233;goto _LL13C;_LL13B: _tmp234=_tmp22D.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp235=(struct Cyc_Absyn_Initializes_att_struct*)_tmp234;if(_tmp235->tag != 20)
goto _LL13D;else{_tmp236=_tmp235->f1;}};_tmp237=_tmp22D.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp238=(struct Cyc_Absyn_Initializes_att_struct*)_tmp237;if(_tmp238->tag != 20)
goto _LL13D;else{_tmp239=_tmp238->f1;}};_LL13C: _tmp23C=_tmp236;_tmp23F=_tmp239;
goto _LL13E;_LL13D: _tmp23A=_tmp22D.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp23B=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp23A;if(_tmp23B->tag != 6)goto _LL13F;else{
_tmp23C=_tmp23B->f1;}};_tmp23D=_tmp22D.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp23E=(struct Cyc_Absyn_Aligned_att_struct*)_tmp23D;if(_tmp23E->tag != 6)goto
_LL13F;else{_tmp23F=_tmp23E->f1;}};_LL13E: return Cyc_Core_intcmp(_tmp23C,_tmp23F);
_LL13F: _tmp240=_tmp22D.f1;{struct Cyc_Absyn_Section_att_struct*_tmp241=(struct Cyc_Absyn_Section_att_struct*)
_tmp240;if(_tmp241->tag != 8)goto _LL141;else{_tmp242=_tmp241->f1;}};_tmp243=
_tmp22D.f2;{struct Cyc_Absyn_Section_att_struct*_tmp244=(struct Cyc_Absyn_Section_att_struct*)
_tmp243;if(_tmp244->tag != 8)goto _LL141;else{_tmp245=_tmp244->f1;}};_LL140: return
Cyc_strcmp((struct _dyneither_ptr)_tmp242,(struct _dyneither_ptr)_tmp245);_LL141:
_tmp246=_tmp22D.f1;{struct Cyc_Absyn_Format_att_struct*_tmp247=(struct Cyc_Absyn_Format_att_struct*)
_tmp246;if(_tmp247->tag != 19)goto _LL143;else{_tmp248=_tmp247->f1;_tmp249=_tmp247->f2;
_tmp24A=_tmp247->f3;}};_tmp24B=_tmp22D.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp24C=(struct Cyc_Absyn_Format_att_struct*)_tmp24B;if(_tmp24C->tag != 19)goto
_LL143;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;_tmp24F=_tmp24C->f3;}};
_LL142: {int _tmp250=Cyc_Core_intcmp((int)((unsigned int)_tmp248),(int)((
unsigned int)_tmp24D));if(_tmp250 != 0)return _tmp250;{int _tmp251=Cyc_Core_intcmp(
_tmp249,_tmp24E);if(_tmp251 != 0)return _tmp251;return Cyc_Core_intcmp(_tmp24A,
_tmp24F);};}_LL143:;_LL144: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL138:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2);static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,
a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*
pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple14*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv);static struct
_tuple14*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp252=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp253;
enum Cyc_Absyn_KindQual _tmp254;enum Cyc_Absyn_AliasQual _tmp255;struct Cyc_Absyn_Kind
_tmp256;enum Cyc_Absyn_KindQual _tmp257;enum Cyc_Absyn_AliasQual _tmp258;struct Cyc_Absyn_Kind
_tmp259;enum Cyc_Absyn_KindQual _tmp25A;struct Cyc_Absyn_Kind _tmp25B;enum Cyc_Absyn_KindQual
_tmp25C;_LL146: _tmp253=*_tmp252;_tmp254=_tmp253.kind;if(_tmp254 != Cyc_Absyn_RgnKind)
goto _LL148;_tmp255=_tmp253.aliasqual;if(_tmp255 != Cyc_Absyn_Unique)goto _LL148;
_LL147: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL145;_LL148: _tmp256=*_tmp252;
_tmp257=_tmp256.kind;if(_tmp257 != Cyc_Absyn_RgnKind)goto _LL14A;_tmp258=_tmp256.aliasqual;
if(_tmp258 != Cyc_Absyn_Aliasable)goto _LL14A;_LL149: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL145;_LL14A: _tmp259=*_tmp252;_tmp25A=_tmp259.kind;if(_tmp25A != Cyc_Absyn_EffKind)
goto _LL14C;_LL14B: t=Cyc_Absyn_empty_effect;goto _LL145;_LL14C: _tmp25B=*_tmp252;
_tmp25C=_tmp25B.kind;if(_tmp25C != Cyc_Absyn_IntKind)goto _LL14E;_LL14D:{struct Cyc_Absyn_ValueofType_struct
_tmpFB1;struct Cyc_Absyn_ValueofType_struct*_tmpFB0;t=(void*)((_tmpFB0=_cycalloc(
sizeof(*_tmpFB0)),((_tmpFB0[0]=((_tmpFB1.tag=19,((_tmpFB1.f1=Cyc_Absyn_uint_exp(
0,0),_tmpFB1)))),_tmpFB0))));}goto _LL145;_LL14E:;_LL14F: t=Cyc_Absyn_sint_typ;
goto _LL145;_LL145:;}{struct _tuple14*_tmpFB2;return(_tmpFB2=_cycalloc(sizeof(*
_tmpFB2)),((_tmpFB2->f1=tv,((_tmpFB2->f2=t,_tmpFB2)))));};}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp260=Cyc_Tcutil_compress(t);
void*_tmp26B;void*_tmp26D;void*_tmp26E;struct Cyc_Absyn_DatatypeInfo _tmp270;
struct Cyc_List_List*_tmp271;struct Cyc_Absyn_PtrInfo _tmp273;void*_tmp274;struct
Cyc_Absyn_PtrAtts _tmp275;void*_tmp276;struct Cyc_Absyn_ArrayInfo _tmp278;void*
_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_Absyn_DatatypeFieldInfo _tmp27D;
struct Cyc_List_List*_tmp27E;struct Cyc_Absyn_AggrInfo _tmp280;struct Cyc_List_List*
_tmp281;struct Cyc_List_List*_tmp283;struct Cyc_Absyn_FnInfo _tmp286;struct Cyc_List_List*
_tmp287;struct Cyc_Core_Opt*_tmp288;void*_tmp289;struct Cyc_List_List*_tmp28A;
struct Cyc_Absyn_VarargInfo*_tmp28B;struct Cyc_List_List*_tmp28C;void*_tmp292;
struct Cyc_List_List*_tmp294;_LL151: {struct Cyc_Absyn_VoidType_struct*_tmp261=(
struct Cyc_Absyn_VoidType_struct*)_tmp260;if(_tmp261->tag != 0)goto _LL153;}_LL152:
goto _LL154;_LL153: {struct Cyc_Absyn_FloatType_struct*_tmp262=(struct Cyc_Absyn_FloatType_struct*)
_tmp260;if(_tmp262->tag != 7)goto _LL155;}_LL154: goto _LL156;_LL155: {struct Cyc_Absyn_DoubleType_struct*
_tmp263=(struct Cyc_Absyn_DoubleType_struct*)_tmp260;if(_tmp263->tag != 8)goto
_LL157;}_LL156: goto _LL158;_LL157: {struct Cyc_Absyn_EnumType_struct*_tmp264=(
struct Cyc_Absyn_EnumType_struct*)_tmp260;if(_tmp264->tag != 14)goto _LL159;}_LL158:
goto _LL15A;_LL159: {struct Cyc_Absyn_AnonEnumType_struct*_tmp265=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp260;if(_tmp265->tag != 15)goto _LL15B;}_LL15A: goto _LL15C;_LL15B: {struct Cyc_Absyn_ValueofType_struct*
_tmp266=(struct Cyc_Absyn_ValueofType_struct*)_tmp260;if(_tmp266->tag != 19)goto
_LL15D;}_LL15C: goto _LL15E;_LL15D: {struct Cyc_Absyn_IntType_struct*_tmp267=(
struct Cyc_Absyn_IntType_struct*)_tmp260;if(_tmp267->tag != 6)goto _LL15F;}_LL15E:
return Cyc_Absyn_empty_effect;_LL15F: {struct Cyc_Absyn_Evar_struct*_tmp268=(
struct Cyc_Absyn_Evar_struct*)_tmp260;if(_tmp268->tag != 1)goto _LL161;}_LL160: goto
_LL162;_LL161: {struct Cyc_Absyn_VarType_struct*_tmp269=(struct Cyc_Absyn_VarType_struct*)
_tmp260;if(_tmp269->tag != 2)goto _LL163;}_LL162: {struct Cyc_Absyn_Kind*_tmp295=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp296;enum Cyc_Absyn_KindQual
_tmp297;struct Cyc_Absyn_Kind _tmp298;enum Cyc_Absyn_KindQual _tmp299;struct Cyc_Absyn_Kind
_tmp29A;enum Cyc_Absyn_KindQual _tmp29B;_LL186: _tmp296=*_tmp295;_tmp297=_tmp296.kind;
if(_tmp297 != Cyc_Absyn_RgnKind)goto _LL188;_LL187: {struct Cyc_Absyn_AccessEff_struct
_tmpFB5;struct Cyc_Absyn_AccessEff_struct*_tmpFB4;return(void*)((_tmpFB4=
_cycalloc(sizeof(*_tmpFB4)),((_tmpFB4[0]=((_tmpFB5.tag=23,((_tmpFB5.f1=(void*)t,
_tmpFB5)))),_tmpFB4))));}_LL188: _tmp298=*_tmp295;_tmp299=_tmp298.kind;if(_tmp299
!= Cyc_Absyn_EffKind)goto _LL18A;_LL189: return t;_LL18A: _tmp29A=*_tmp295;_tmp29B=
_tmp29A.kind;if(_tmp29B != Cyc_Absyn_IntKind)goto _LL18C;_LL18B: return Cyc_Absyn_empty_effect;
_LL18C:;_LL18D: {struct Cyc_Absyn_RgnsEff_struct _tmpFB8;struct Cyc_Absyn_RgnsEff_struct*
_tmpFB7;return(void*)((_tmpFB7=_cycalloc(sizeof(*_tmpFB7)),((_tmpFB7[0]=((
_tmpFB8.tag=25,((_tmpFB8.f1=(void*)t,_tmpFB8)))),_tmpFB7))));}_LL185:;}_LL163: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp26A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp260;if(_tmp26A->tag != 16)goto _LL165;else{_tmp26B=(void*)_tmp26A->f1;}}_LL164: {
struct Cyc_Absyn_AccessEff_struct _tmpFBB;struct Cyc_Absyn_AccessEff_struct*_tmpFBA;
return(void*)((_tmpFBA=_cycalloc(sizeof(*_tmpFBA)),((_tmpFBA[0]=((_tmpFBB.tag=23,((
_tmpFBB.f1=(void*)_tmp26B,_tmpFBB)))),_tmpFBA))));}_LL165: {struct Cyc_Absyn_DynRgnType_struct*
_tmp26C=(struct Cyc_Absyn_DynRgnType_struct*)_tmp260;if(_tmp26C->tag != 17)goto
_LL167;else{_tmp26D=(void*)_tmp26C->f1;_tmp26E=(void*)_tmp26C->f2;}}_LL166: {
struct Cyc_Absyn_AccessEff_struct _tmpFBE;struct Cyc_Absyn_AccessEff_struct*_tmpFBD;
return(void*)((_tmpFBD=_cycalloc(sizeof(*_tmpFBD)),((_tmpFBD[0]=((_tmpFBE.tag=23,((
_tmpFBE.f1=(void*)_tmp26E,_tmpFBE)))),_tmpFBD))));}_LL167: {struct Cyc_Absyn_DatatypeType_struct*
_tmp26F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp260;if(_tmp26F->tag != 3)goto
_LL169;else{_tmp270=_tmp26F->f1;_tmp271=_tmp270.targs;}}_LL168: {struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp271);struct Cyc_Absyn_JoinEff_struct _tmpFC1;struct Cyc_Absyn_JoinEff_struct*
_tmpFC0;return Cyc_Tcutil_normalize_effect((void*)((_tmpFC0=_cycalloc(sizeof(*
_tmpFC0)),((_tmpFC0[0]=((_tmpFC1.tag=24,((_tmpFC1.f1=ts,_tmpFC1)))),_tmpFC0)))));}
_LL169: {struct Cyc_Absyn_PointerType_struct*_tmp272=(struct Cyc_Absyn_PointerType_struct*)
_tmp260;if(_tmp272->tag != 5)goto _LL16B;else{_tmp273=_tmp272->f1;_tmp274=_tmp273.elt_typ;
_tmp275=_tmp273.ptr_atts;_tmp276=_tmp275.rgn;}}_LL16A: {struct Cyc_Absyn_JoinEff_struct
_tmpFD0;struct Cyc_Absyn_AccessEff_struct*_tmpFCF;struct Cyc_Absyn_AccessEff_struct
_tmpFCE;void*_tmpFCD[2];struct Cyc_Absyn_JoinEff_struct*_tmpFCC;return Cyc_Tcutil_normalize_effect((
void*)((_tmpFCC=_cycalloc(sizeof(*_tmpFCC)),((_tmpFCC[0]=((_tmpFD0.tag=24,((
_tmpFD0.f1=((_tmpFCD[1]=Cyc_Tcutil_rgns_of(_tmp274),((_tmpFCD[0]=(void*)((
_tmpFCF=_cycalloc(sizeof(*_tmpFCF)),((_tmpFCF[0]=((_tmpFCE.tag=23,((_tmpFCE.f1=(
void*)_tmp276,_tmpFCE)))),_tmpFCF)))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFCD,sizeof(void*),2)))))),
_tmpFD0)))),_tmpFCC)))));}_LL16B: {struct Cyc_Absyn_ArrayType_struct*_tmp277=(
struct Cyc_Absyn_ArrayType_struct*)_tmp260;if(_tmp277->tag != 9)goto _LL16D;else{
_tmp278=_tmp277->f1;_tmp279=_tmp278.elt_type;}}_LL16C: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp279));_LL16D: {struct Cyc_Absyn_TupleType_struct*_tmp27A=(
struct Cyc_Absyn_TupleType_struct*)_tmp260;if(_tmp27A->tag != 11)goto _LL16F;else{
_tmp27B=_tmp27A->f1;}}_LL16E: {struct Cyc_List_List*_tmp2AB=0;for(0;_tmp27B != 0;
_tmp27B=_tmp27B->tl){struct Cyc_List_List*_tmpFD1;_tmp2AB=((_tmpFD1=_cycalloc(
sizeof(*_tmpFD1)),((_tmpFD1->hd=(*((struct _tuple11*)_tmp27B->hd)).f2,((_tmpFD1->tl=
_tmp2AB,_tmpFD1))))));}_tmp27E=_tmp2AB;goto _LL170;}_LL16F: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp27C=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp260;if(_tmp27C->tag != 4)
goto _LL171;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27D.targs;}}_LL170: _tmp281=
_tmp27E;goto _LL172;_LL171: {struct Cyc_Absyn_AggrType_struct*_tmp27F=(struct Cyc_Absyn_AggrType_struct*)
_tmp260;if(_tmp27F->tag != 12)goto _LL173;else{_tmp280=_tmp27F->f1;_tmp281=_tmp280.targs;}}
_LL172: {struct Cyc_Absyn_JoinEff_struct _tmpFD4;struct Cyc_Absyn_JoinEff_struct*
_tmpFD3;return Cyc_Tcutil_normalize_effect((void*)((_tmpFD3=_cycalloc(sizeof(*
_tmpFD3)),((_tmpFD3[0]=((_tmpFD4.tag=24,((_tmpFD4.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp281),
_tmpFD4)))),_tmpFD3)))));}_LL173: {struct Cyc_Absyn_AnonAggrType_struct*_tmp282=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp260;if(_tmp282->tag != 13)goto _LL175;
else{_tmp283=_tmp282->f2;}}_LL174: {struct Cyc_Absyn_JoinEff_struct _tmpFD7;struct
Cyc_Absyn_JoinEff_struct*_tmpFD6;return Cyc_Tcutil_normalize_effect((void*)((
_tmpFD6=_cycalloc(sizeof(*_tmpFD6)),((_tmpFD6[0]=((_tmpFD7.tag=24,((_tmpFD7.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp283),_tmpFD7)))),_tmpFD6)))));}
_LL175: {struct Cyc_Absyn_TagType_struct*_tmp284=(struct Cyc_Absyn_TagType_struct*)
_tmp260;if(_tmp284->tag != 20)goto _LL177;}_LL176: return Cyc_Absyn_empty_effect;
_LL177: {struct Cyc_Absyn_FnType_struct*_tmp285=(struct Cyc_Absyn_FnType_struct*)
_tmp260;if(_tmp285->tag != 10)goto _LL179;else{_tmp286=_tmp285->f1;_tmp287=_tmp286.tvars;
_tmp288=_tmp286.effect;_tmp289=_tmp286.ret_typ;_tmp28A=_tmp286.args;_tmp28B=
_tmp286.cyc_varargs;_tmp28C=_tmp286.rgn_po;}}_LL178: {void*_tmp2B1=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp287),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp288))->v);return Cyc_Tcutil_normalize_effect(_tmp2B1);}_LL179: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp28D=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp260;if(_tmp28D->tag != 22)goto _LL17B;}_LL17A: goto _LL17C;_LL17B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp28E=(struct Cyc_Absyn_HeapRgn_struct*)_tmp260;if(_tmp28E->tag != 21)goto _LL17D;}
_LL17C: return Cyc_Absyn_empty_effect;_LL17D: {struct Cyc_Absyn_AccessEff_struct*
_tmp28F=(struct Cyc_Absyn_AccessEff_struct*)_tmp260;if(_tmp28F->tag != 23)goto
_LL17F;}_LL17E: goto _LL180;_LL17F: {struct Cyc_Absyn_JoinEff_struct*_tmp290=(
struct Cyc_Absyn_JoinEff_struct*)_tmp260;if(_tmp290->tag != 24)goto _LL181;}_LL180:
return t;_LL181: {struct Cyc_Absyn_RgnsEff_struct*_tmp291=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp260;if(_tmp291->tag != 25)goto _LL183;else{_tmp292=(void*)_tmp291->f1;}}_LL182:
return Cyc_Tcutil_rgns_of(_tmp292);_LL183: {struct Cyc_Absyn_TypedefType_struct*
_tmp293=(struct Cyc_Absyn_TypedefType_struct*)_tmp260;if(_tmp293->tag != 18)goto
_LL150;else{_tmp294=_tmp293->f2;}}_LL184: {struct Cyc_Absyn_JoinEff_struct _tmpFDA;
struct Cyc_Absyn_JoinEff_struct*_tmpFD9;return Cyc_Tcutil_normalize_effect((void*)((
_tmpFD9=_cycalloc(sizeof(*_tmpFD9)),((_tmpFD9[0]=((_tmpFDA.tag=24,((_tmpFDA.f1=((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp294),_tmpFDA)))),_tmpFD9)))));}_LL150:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp2B4=e;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List**_tmp2B7;void*_tmp2B9;
_LL18F: {struct Cyc_Absyn_JoinEff_struct*_tmp2B5=(struct Cyc_Absyn_JoinEff_struct*)
_tmp2B4;if(_tmp2B5->tag != 24)goto _LL191;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=(struct
Cyc_List_List**)& _tmp2B5->f1;}}_LL190: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp2B7;for(0;effs != 0;effs=effs->tl){void*_tmp2BA=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp2BA));{void*_tmp2BB=(
void*)effs->hd;void*_tmp2BE;_LL196: {struct Cyc_Absyn_JoinEff_struct*_tmp2BC=(
struct Cyc_Absyn_JoinEff_struct*)_tmp2BB;if(_tmp2BC->tag != 24)goto _LL198;}_LL197:
goto _LL199;_LL198: {struct Cyc_Absyn_AccessEff_struct*_tmp2BD=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2BB;if(_tmp2BD->tag != 23)goto _LL19A;else{_tmp2BE=(void*)_tmp2BD->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2BF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2BE;if(
_tmp2BF->tag != 21)goto _LL19A;};}}_LL199: redo_join=1;goto _LL195;_LL19A:;_LL19B:
goto _LL195;_LL195:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp2B7;for(0;effs != 0;effs=effs->tl){void*_tmp2C0=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp2C2;void*_tmp2C4;_LL19D: {struct Cyc_Absyn_JoinEff_struct*
_tmp2C1=(struct Cyc_Absyn_JoinEff_struct*)_tmp2C0;if(_tmp2C1->tag != 24)goto _LL19F;
else{_tmp2C2=_tmp2C1->f1;}}_LL19E: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp2C2,effects);goto _LL19C;_LL19F: {
struct Cyc_Absyn_AccessEff_struct*_tmp2C3=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2C0;if(_tmp2C3->tag != 23)goto _LL1A1;else{_tmp2C4=(void*)_tmp2C3->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2C5=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2C4;if(
_tmp2C5->tag != 21)goto _LL1A1;};}}_LL1A0: goto _LL19C;_LL1A1:;_LL1A2:{struct Cyc_List_List*
_tmpFDB;effects=((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB->hd=_tmp2C0,((
_tmpFDB->tl=effects,_tmpFDB))))));}goto _LL19C;_LL19C:;}}*_tmp2B7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL191: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2B8=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2B4;if(_tmp2B8->tag != 25)goto _LL193;
else{_tmp2B9=(void*)_tmp2B8->f1;}}_LL192: {void*_tmp2C7=Cyc_Tcutil_compress(
_tmp2B9);_LL1A4: {struct Cyc_Absyn_Evar_struct*_tmp2C8=(struct Cyc_Absyn_Evar_struct*)
_tmp2C7;if(_tmp2C8->tag != 1)goto _LL1A6;}_LL1A5: goto _LL1A7;_LL1A6: {struct Cyc_Absyn_VarType_struct*
_tmp2C9=(struct Cyc_Absyn_VarType_struct*)_tmp2C7;if(_tmp2C9->tag != 2)goto _LL1A8;}
_LL1A7: return e;_LL1A8:;_LL1A9: return Cyc_Tcutil_rgns_of(_tmp2B9);_LL1A3:;}_LL193:;
_LL194: return e;_LL18E:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmpFE5;
struct Cyc_Core_Opt*_tmpFE4;struct Cyc_Absyn_FnInfo _tmpFE3;struct Cyc_Absyn_FnType_struct*
_tmpFE2;struct Cyc_Absyn_FnType_struct*_tmp2CA=(_tmpFE2=_cycalloc(sizeof(*_tmpFE2)),((
_tmpFE2[0]=((_tmpFE5.tag=10,((_tmpFE5.f1=((_tmpFE3.tvars=0,((_tmpFE3.effect=((
_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((_tmpFE4->v=eff,_tmpFE4)))),((_tmpFE3.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmpFE3.args=0,((_tmpFE3.c_varargs=0,((_tmpFE3.cyc_varargs=
0,((_tmpFE3.rgn_po=0,((_tmpFE3.attributes=0,_tmpFE3)))))))))))))))),_tmpFE5)))),
_tmpFE2)));return Cyc_Absyn_atb_typ((void*)_tmp2CA,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (
void*)& Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp2CF=Cyc_Tcutil_compress(e);void*
_tmp2D1;struct Cyc_List_List*_tmp2D3;void*_tmp2D5;struct Cyc_Core_Opt*_tmp2D7;
struct Cyc_Core_Opt*_tmp2D8;struct Cyc_Core_Opt**_tmp2D9;struct Cyc_Core_Opt*
_tmp2DA;_LL1AB: {struct Cyc_Absyn_AccessEff_struct*_tmp2D0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2CF;if(_tmp2D0->tag != 23)goto _LL1AD;else{_tmp2D1=(void*)_tmp2D0->f1;}}_LL1AC:
if(constrain)return Cyc_Tcutil_unify(r,_tmp2D1);_tmp2D1=Cyc_Tcutil_compress(
_tmp2D1);if(r == _tmp2D1)return 1;{struct _tuple0 _tmpFE6;struct _tuple0 _tmp2DC=(
_tmpFE6.f1=r,((_tmpFE6.f2=_tmp2D1,_tmpFE6)));void*_tmp2DD;struct Cyc_Absyn_Tvar*
_tmp2DF;void*_tmp2E0;struct Cyc_Absyn_Tvar*_tmp2E2;_LL1B6: _tmp2DD=_tmp2DC.f1;{
struct Cyc_Absyn_VarType_struct*_tmp2DE=(struct Cyc_Absyn_VarType_struct*)_tmp2DD;
if(_tmp2DE->tag != 2)goto _LL1B8;else{_tmp2DF=_tmp2DE->f1;}};_tmp2E0=_tmp2DC.f2;{
struct Cyc_Absyn_VarType_struct*_tmp2E1=(struct Cyc_Absyn_VarType_struct*)_tmp2E0;
if(_tmp2E1->tag != 2)goto _LL1B8;else{_tmp2E2=_tmp2E1->f1;}};_LL1B7: return Cyc_Absyn_tvar_cmp(
_tmp2DF,_tmp2E2)== 0;_LL1B8:;_LL1B9: return 0;_LL1B5:;};_LL1AD: {struct Cyc_Absyn_JoinEff_struct*
_tmp2D2=(struct Cyc_Absyn_JoinEff_struct*)_tmp2CF;if(_tmp2D2->tag != 24)goto _LL1AF;
else{_tmp2D3=_tmp2D2->f1;}}_LL1AE: for(0;_tmp2D3 != 0;_tmp2D3=_tmp2D3->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp2D3->hd))return 1;}return 0;_LL1AF: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2D4=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2CF;if(_tmp2D4->tag != 25)goto _LL1B1;
else{_tmp2D5=(void*)_tmp2D4->f1;}}_LL1B0: {void*_tmp2E3=Cyc_Tcutil_rgns_of(
_tmp2D5);void*_tmp2E5;_LL1BB: {struct Cyc_Absyn_RgnsEff_struct*_tmp2E4=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp2E3;if(_tmp2E4->tag != 25)goto _LL1BD;else{_tmp2E5=(void*)_tmp2E4->f1;}}_LL1BC:
if(!constrain)return 0;{void*_tmp2E6=Cyc_Tcutil_compress(_tmp2E5);struct Cyc_Core_Opt*
_tmp2E8;struct Cyc_Core_Opt*_tmp2E9;struct Cyc_Core_Opt**_tmp2EA;struct Cyc_Core_Opt*
_tmp2EB;_LL1C0: {struct Cyc_Absyn_Evar_struct*_tmp2E7=(struct Cyc_Absyn_Evar_struct*)
_tmp2E6;if(_tmp2E7->tag != 1)goto _LL1C2;else{_tmp2E8=_tmp2E7->f1;_tmp2E9=_tmp2E7->f2;
_tmp2EA=(struct Cyc_Core_Opt**)& _tmp2E7->f2;_tmp2EB=_tmp2E7->f4;}}_LL1C1: {void*
_tmp2EC=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp2EB);Cyc_Tcutil_occurs(
_tmp2EC,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2EB))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmpFF5;struct Cyc_Absyn_AccessEff_struct*
_tmpFF4;struct Cyc_Absyn_AccessEff_struct _tmpFF3;void*_tmpFF2[2];struct Cyc_Absyn_JoinEff_struct*
_tmpFF1;void*_tmp2ED=Cyc_Tcutil_dummy_fntype((void*)((_tmpFF1=_cycalloc(sizeof(*
_tmpFF1)),((_tmpFF1[0]=((_tmpFF5.tag=24,((_tmpFF5.f1=((_tmpFF2[1]=(void*)((
_tmpFF4=_cycalloc(sizeof(*_tmpFF4)),((_tmpFF4[0]=((_tmpFF3.tag=23,((_tmpFF3.f1=(
void*)r,_tmpFF3)))),_tmpFF4)))),((_tmpFF2[0]=_tmp2EC,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFF2,sizeof(void*),2)))))),
_tmpFF5)))),_tmpFF1)))));{struct Cyc_Core_Opt*_tmpFF6;*_tmp2EA=((_tmpFF6=
_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6->v=_tmp2ED,_tmpFF6))));}return 1;};}_LL1C2:;
_LL1C3: return 0;_LL1BF:;};_LL1BD:;_LL1BE: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp2E3);_LL1BA:;}_LL1B1: {struct Cyc_Absyn_Evar_struct*_tmp2D6=(
struct Cyc_Absyn_Evar_struct*)_tmp2CF;if(_tmp2D6->tag != 1)goto _LL1B3;else{_tmp2D7=
_tmp2D6->f1;_tmp2D8=_tmp2D6->f2;_tmp2D9=(struct Cyc_Core_Opt**)& _tmp2D6->f2;
_tmp2DA=_tmp2D6->f4;}}_LL1B2: if(_tmp2D7 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D7->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmpFF9;void*_tmpFF8;(_tmpFF8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF9="effect evar has wrong kind",
_tag_dyneither(_tmpFF9,sizeof(char),27))),_tag_dyneither(_tmpFF8,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp2F6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,
_tmp2DA);Cyc_Tcutil_occurs(_tmp2F6,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2DA))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp100E;struct Cyc_List_List*_tmp100D;struct Cyc_Absyn_AccessEff_struct _tmp100C;
struct Cyc_Absyn_AccessEff_struct*_tmp100B;struct Cyc_List_List*_tmp100A;struct Cyc_Absyn_JoinEff_struct*
_tmp1009;struct Cyc_Absyn_JoinEff_struct*_tmp2F7=(_tmp1009=_cycalloc(sizeof(*
_tmp1009)),((_tmp1009[0]=((_tmp100E.tag=24,((_tmp100E.f1=((_tmp100A=_cycalloc(
sizeof(*_tmp100A)),((_tmp100A->hd=_tmp2F6,((_tmp100A->tl=((_tmp100D=_cycalloc(
sizeof(*_tmp100D)),((_tmp100D->hd=(void*)((_tmp100B=_cycalloc(sizeof(*_tmp100B)),((
_tmp100B[0]=((_tmp100C.tag=23,((_tmp100C.f1=(void*)r,_tmp100C)))),_tmp100B)))),((
_tmp100D->tl=0,_tmp100D)))))),_tmp100A)))))),_tmp100E)))),_tmp1009)));{struct Cyc_Core_Opt*
_tmp100F;*_tmp2D9=((_tmp100F=_cycalloc(sizeof(*_tmp100F)),((_tmp100F->v=(void*)
_tmp2F7,_tmp100F))));}return 1;};};_LL1B3:;_LL1B4: return 0;_LL1AA:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp2FF=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp302;void*_tmp304;struct Cyc_Core_Opt*
_tmp306;struct Cyc_Core_Opt*_tmp307;struct Cyc_Core_Opt**_tmp308;struct Cyc_Core_Opt*
_tmp309;_LL1C5: {struct Cyc_Absyn_AccessEff_struct*_tmp300=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2FF;if(_tmp300->tag != 23)goto _LL1C7;}_LL1C6: return 0;_LL1C7: {struct Cyc_Absyn_JoinEff_struct*
_tmp301=(struct Cyc_Absyn_JoinEff_struct*)_tmp2FF;if(_tmp301->tag != 24)goto _LL1C9;
else{_tmp302=_tmp301->f1;}}_LL1C8: for(0;_tmp302 != 0;_tmp302=_tmp302->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp302->hd))return 1;}return 0;_LL1C9: {struct Cyc_Absyn_RgnsEff_struct*
_tmp303=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2FF;if(_tmp303->tag != 25)goto _LL1CB;
else{_tmp304=(void*)_tmp303->f1;}}_LL1CA: _tmp304=Cyc_Tcutil_compress(_tmp304);
if(t == _tmp304)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp304);{
void*_tmp30A=Cyc_Tcutil_rgns_of(t);void*_tmp30C;_LL1D0: {struct Cyc_Absyn_RgnsEff_struct*
_tmp30B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp30A;if(_tmp30B->tag != 25)goto _LL1D2;
else{_tmp30C=(void*)_tmp30B->f1;}}_LL1D1: {struct _tuple0 _tmp1010;struct _tuple0
_tmp30E=(_tmp1010.f1=Cyc_Tcutil_compress(_tmp30C),((_tmp1010.f2=_tmp304,_tmp1010)));
void*_tmp30F;struct Cyc_Absyn_Tvar*_tmp311;void*_tmp312;struct Cyc_Absyn_Tvar*
_tmp314;_LL1D5: _tmp30F=_tmp30E.f1;{struct Cyc_Absyn_VarType_struct*_tmp310=(
struct Cyc_Absyn_VarType_struct*)_tmp30F;if(_tmp310->tag != 2)goto _LL1D7;else{
_tmp311=_tmp310->f1;}};_tmp312=_tmp30E.f2;{struct Cyc_Absyn_VarType_struct*
_tmp313=(struct Cyc_Absyn_VarType_struct*)_tmp312;if(_tmp313->tag != 2)goto _LL1D7;
else{_tmp314=_tmp313->f1;}};_LL1D6: return Cyc_Tcutil_unify(t,_tmp304);_LL1D7:;
_LL1D8: return _tmp30C == _tmp304;_LL1D4:;}_LL1D2:;_LL1D3: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp30A);_LL1CF:;};_LL1CB: {struct Cyc_Absyn_Evar_struct*
_tmp305=(struct Cyc_Absyn_Evar_struct*)_tmp2FF;if(_tmp305->tag != 1)goto _LL1CD;
else{_tmp306=_tmp305->f1;_tmp307=_tmp305->f2;_tmp308=(struct Cyc_Core_Opt**)&
_tmp305->f2;_tmp309=_tmp305->f4;}}_LL1CC: if(_tmp306 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp306->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1013;void*_tmp1012;(
_tmp1012=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1013="effect evar has wrong kind",_tag_dyneither(_tmp1013,sizeof(char),27))),
_tag_dyneither(_tmp1012,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp317=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp309);
Cyc_Tcutil_occurs(_tmp317,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp309))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1028;struct Cyc_List_List*
_tmp1027;struct Cyc_Absyn_RgnsEff_struct _tmp1026;struct Cyc_Absyn_RgnsEff_struct*
_tmp1025;struct Cyc_List_List*_tmp1024;struct Cyc_Absyn_JoinEff_struct*_tmp1023;
struct Cyc_Absyn_JoinEff_struct*_tmp318=(_tmp1023=_cycalloc(sizeof(*_tmp1023)),((
_tmp1023[0]=((_tmp1028.tag=24,((_tmp1028.f1=((_tmp1024=_cycalloc(sizeof(*
_tmp1024)),((_tmp1024->hd=_tmp317,((_tmp1024->tl=((_tmp1027=_cycalloc(sizeof(*
_tmp1027)),((_tmp1027->hd=(void*)((_tmp1025=_cycalloc(sizeof(*_tmp1025)),((
_tmp1025[0]=((_tmp1026.tag=25,((_tmp1026.f1=(void*)t,_tmp1026)))),_tmp1025)))),((
_tmp1027->tl=0,_tmp1027)))))),_tmp1024)))))),_tmp1028)))),_tmp1023)));{struct Cyc_Core_Opt*
_tmp1029;*_tmp308=((_tmp1029=_cycalloc(sizeof(*_tmp1029)),((_tmp1029->v=(void*)
_tmp318,_tmp1029))));}return 1;};};_LL1CD:;_LL1CE: return 0;_LL1C4:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp320=e;struct Cyc_Absyn_Tvar*_tmp322;struct Cyc_List_List*_tmp324;void*
_tmp326;struct Cyc_Core_Opt*_tmp328;struct Cyc_Core_Opt*_tmp329;struct Cyc_Core_Opt**
_tmp32A;struct Cyc_Core_Opt*_tmp32B;_LL1DA: {struct Cyc_Absyn_VarType_struct*
_tmp321=(struct Cyc_Absyn_VarType_struct*)_tmp320;if(_tmp321->tag != 2)goto _LL1DC;
else{_tmp322=_tmp321->f1;}}_LL1DB: return Cyc_Absyn_tvar_cmp(v,_tmp322)== 0;_LL1DC: {
struct Cyc_Absyn_JoinEff_struct*_tmp323=(struct Cyc_Absyn_JoinEff_struct*)_tmp320;
if(_tmp323->tag != 24)goto _LL1DE;else{_tmp324=_tmp323->f1;}}_LL1DD: for(0;_tmp324
!= 0;_tmp324=_tmp324->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp324->hd))return 1;}return 0;_LL1DE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp325=(struct Cyc_Absyn_RgnsEff_struct*)_tmp320;if(_tmp325->tag != 25)goto _LL1E0;
else{_tmp326=(void*)_tmp325->f1;}}_LL1DF: {void*_tmp32C=Cyc_Tcutil_rgns_of(
_tmp326);void*_tmp32E;_LL1E5: {struct Cyc_Absyn_RgnsEff_struct*_tmp32D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp32C;if(_tmp32D->tag != 25)goto _LL1E7;else{_tmp32E=(void*)_tmp32D->f1;}}_LL1E6:
if(!may_constrain_evars)return 0;{void*_tmp32F=Cyc_Tcutil_compress(_tmp32E);
struct Cyc_Core_Opt*_tmp331;struct Cyc_Core_Opt*_tmp332;struct Cyc_Core_Opt**
_tmp333;struct Cyc_Core_Opt*_tmp334;_LL1EA: {struct Cyc_Absyn_Evar_struct*_tmp330=(
struct Cyc_Absyn_Evar_struct*)_tmp32F;if(_tmp330->tag != 1)goto _LL1EC;else{_tmp331=
_tmp330->f1;_tmp332=_tmp330->f2;_tmp333=(struct Cyc_Core_Opt**)& _tmp330->f2;
_tmp334=_tmp330->f4;}}_LL1EB: {void*_tmp335=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp334);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp334))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1038;struct Cyc_Absyn_VarType_struct*_tmp1037;
struct Cyc_Absyn_VarType_struct _tmp1036;void*_tmp1035[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1034;void*_tmp336=Cyc_Tcutil_dummy_fntype((void*)((_tmp1034=_cycalloc(
sizeof(*_tmp1034)),((_tmp1034[0]=((_tmp1038.tag=24,((_tmp1038.f1=((_tmp1035[1]=(
void*)((_tmp1037=_cycalloc(sizeof(*_tmp1037)),((_tmp1037[0]=((_tmp1036.tag=2,((
_tmp1036.f1=v,_tmp1036)))),_tmp1037)))),((_tmp1035[0]=_tmp335,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1035,sizeof(void*),2)))))),
_tmp1038)))),_tmp1034)))));{struct Cyc_Core_Opt*_tmp1039;*_tmp333=((_tmp1039=
_cycalloc(sizeof(*_tmp1039)),((_tmp1039->v=_tmp336,_tmp1039))));}return 1;};}
_LL1EC:;_LL1ED: return 0;_LL1E9:;};_LL1E7:;_LL1E8: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp32C);_LL1E4:;}_LL1E0: {struct Cyc_Absyn_Evar_struct*
_tmp327=(struct Cyc_Absyn_Evar_struct*)_tmp320;if(_tmp327->tag != 1)goto _LL1E2;
else{_tmp328=_tmp327->f1;_tmp329=_tmp327->f2;_tmp32A=(struct Cyc_Core_Opt**)&
_tmp327->f2;_tmp32B=_tmp327->f4;}}_LL1E1: if(_tmp328 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp328->v)->kind != Cyc_Absyn_EffKind){const char*_tmp103C;void*_tmp103B;(
_tmp103B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp103C="effect evar has wrong kind",_tag_dyneither(_tmp103C,sizeof(char),27))),
_tag_dyneither(_tmp103B,sizeof(void*),0)));}{void*_tmp33F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp32B);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp32B))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1051;
struct Cyc_List_List*_tmp1050;struct Cyc_Absyn_VarType_struct _tmp104F;struct Cyc_Absyn_VarType_struct*
_tmp104E;struct Cyc_List_List*_tmp104D;struct Cyc_Absyn_JoinEff_struct*_tmp104C;
struct Cyc_Absyn_JoinEff_struct*_tmp340=(_tmp104C=_cycalloc(sizeof(*_tmp104C)),((
_tmp104C[0]=((_tmp1051.tag=24,((_tmp1051.f1=((_tmp104D=_cycalloc(sizeof(*
_tmp104D)),((_tmp104D->hd=_tmp33F,((_tmp104D->tl=((_tmp1050=_cycalloc(sizeof(*
_tmp1050)),((_tmp1050->hd=(void*)((_tmp104E=_cycalloc(sizeof(*_tmp104E)),((
_tmp104E[0]=((_tmp104F.tag=2,((_tmp104F.f1=v,_tmp104F)))),_tmp104E)))),((
_tmp1050->tl=0,_tmp1050)))))),_tmp104D)))))),_tmp1051)))),_tmp104C)));{struct Cyc_Core_Opt*
_tmp1052;*_tmp32A=((_tmp1052=_cycalloc(sizeof(*_tmp1052)),((_tmp1052->v=(void*)
_tmp340,_tmp1052))));}return 1;};};_LL1E2:;_LL1E3: return 0;_LL1D9:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp348=e;struct Cyc_List_List*_tmp34A;void*_tmp34C;_LL1EF: {struct Cyc_Absyn_JoinEff_struct*
_tmp349=(struct Cyc_Absyn_JoinEff_struct*)_tmp348;if(_tmp349->tag != 24)goto _LL1F1;
else{_tmp34A=_tmp349->f1;}}_LL1F0: for(0;_tmp34A != 0;_tmp34A=_tmp34A->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp34A->hd))return 1;}return 0;_LL1F1: {struct Cyc_Absyn_RgnsEff_struct*
_tmp34B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp348;if(_tmp34B->tag != 25)goto _LL1F3;
else{_tmp34C=(void*)_tmp34B->f1;}}_LL1F2: {void*_tmp34E=Cyc_Tcutil_rgns_of(
_tmp34C);void*_tmp350;_LL1F8: {struct Cyc_Absyn_RgnsEff_struct*_tmp34F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp34E;if(_tmp34F->tag != 25)goto _LL1FA;else{_tmp350=(void*)_tmp34F->f1;}}_LL1F9:
return 0;_LL1FA:;_LL1FB: return Cyc_Tcutil_evar_in_effect(evar,_tmp34E);_LL1F7:;}
_LL1F3: {struct Cyc_Absyn_Evar_struct*_tmp34D=(struct Cyc_Absyn_Evar_struct*)
_tmp348;if(_tmp34D->tag != 1)goto _LL1F5;}_LL1F4: return evar == e;_LL1F5:;_LL1F6:
return 0;_LL1EE:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp351=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp353;void*_tmp355;
struct Cyc_Absyn_Tvar*_tmp357;void*_tmp359;struct Cyc_Core_Opt*_tmp35B;struct Cyc_Core_Opt**
_tmp35C;struct Cyc_Core_Opt*_tmp35D;_LL1FD: {struct Cyc_Absyn_JoinEff_struct*
_tmp352=(struct Cyc_Absyn_JoinEff_struct*)_tmp351;if(_tmp352->tag != 24)goto _LL1FF;
else{_tmp353=_tmp352->f1;}}_LL1FE: for(0;_tmp353 != 0;_tmp353=_tmp353->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp353->hd,e2))return 0;}return 1;_LL1FF: {struct Cyc_Absyn_AccessEff_struct*
_tmp354=(struct Cyc_Absyn_AccessEff_struct*)_tmp351;if(_tmp354->tag != 23)goto
_LL201;else{_tmp355=(void*)_tmp354->f1;}}_LL200: return Cyc_Tcutil_region_in_effect(
0,_tmp355,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp355,(void*)& Cyc_Absyn_HeapRgn_val);
_LL201: {struct Cyc_Absyn_VarType_struct*_tmp356=(struct Cyc_Absyn_VarType_struct*)
_tmp351;if(_tmp356->tag != 2)goto _LL203;else{_tmp357=_tmp356->f1;}}_LL202: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp357,e2);_LL203: {struct Cyc_Absyn_RgnsEff_struct*
_tmp358=(struct Cyc_Absyn_RgnsEff_struct*)_tmp351;if(_tmp358->tag != 25)goto _LL205;
else{_tmp359=(void*)_tmp358->f1;}}_LL204: {void*_tmp35E=Cyc_Tcutil_rgns_of(
_tmp359);void*_tmp360;_LL20A: {struct Cyc_Absyn_RgnsEff_struct*_tmp35F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp35E;if(_tmp35F->tag != 25)goto _LL20C;else{_tmp360=(void*)_tmp35F->f1;}}_LL20B:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp360,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp360,Cyc_Absyn_sint_typ);_LL20C:;
_LL20D: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp35E,e2);_LL209:;}
_LL205: {struct Cyc_Absyn_Evar_struct*_tmp35A=(struct Cyc_Absyn_Evar_struct*)
_tmp351;if(_tmp35A->tag != 1)goto _LL207;else{_tmp35B=_tmp35A->f2;_tmp35C=(struct
Cyc_Core_Opt**)& _tmp35A->f2;_tmp35D=_tmp35A->f4;}}_LL206: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp1053;*_tmp35C=((_tmp1053=_cycalloc(sizeof(*
_tmp1053)),((_tmp1053->v=Cyc_Absyn_empty_effect,_tmp1053))));}return 1;_LL207:;
_LL208: {const char*_tmp1057;void*_tmp1056[1];struct Cyc_String_pa_struct _tmp1055;(
_tmp1055.tag=0,((_tmp1055.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1056[0]=& _tmp1055,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1057="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1057,sizeof(char),30))),_tag_dyneither(_tmp1056,sizeof(void*),
1)))))));}_LL1FC:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int
Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=
Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(
0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(
1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp366;void*_tmp367;void*_tmp368;struct _tuple0*_tmp365=(struct
_tuple0*)r1->hd;_tmp366=*_tmp365;_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;{int found=
_tmp367 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp36A;void*_tmp36B;void*_tmp36C;struct
_tuple0*_tmp369=(struct _tuple0*)r2->hd;_tmp36A=*_tmp369;_tmp36B=_tmp36A.f1;
_tmp36C=_tmp36A.f2;if(Cyc_Tcutil_unify(_tmp367,_tmp36B) && Cyc_Tcutil_unify(
_tmp368,_tmp36C)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple19{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple20{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp36D=t1;struct Cyc_Core_Opt*_tmp36F;struct Cyc_Core_Opt*
_tmp370;struct Cyc_Core_Opt**_tmp371;struct Cyc_Core_Opt*_tmp372;_LL20F: {struct
Cyc_Absyn_Evar_struct*_tmp36E=(struct Cyc_Absyn_Evar_struct*)_tmp36D;if(_tmp36E->tag
!= 1)goto _LL211;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36E->f2;_tmp371=(struct Cyc_Core_Opt**)&
_tmp36E->f2;_tmp372=_tmp36E->f4;}}_LL210: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp372))->v,t2);{struct
Cyc_Absyn_Kind*_tmp373=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp373,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp36F))->v)){{struct
Cyc_Core_Opt*_tmp1058;*_tmp371=((_tmp1058=_cycalloc(sizeof(*_tmp1058)),((
_tmp1058->v=t2,_tmp1058))));}return;}else{{void*_tmp375=t2;struct Cyc_Core_Opt*
_tmp377;struct Cyc_Core_Opt**_tmp378;struct Cyc_Core_Opt*_tmp379;struct Cyc_Absyn_PtrInfo
_tmp37B;_LL214: {struct Cyc_Absyn_Evar_struct*_tmp376=(struct Cyc_Absyn_Evar_struct*)
_tmp375;if(_tmp376->tag != 1)goto _LL216;else{_tmp377=_tmp376->f2;_tmp378=(struct
Cyc_Core_Opt**)& _tmp376->f2;_tmp379=_tmp376->f4;}}_LL215: {struct Cyc_List_List*
_tmp37C=(struct Cyc_List_List*)_tmp372->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp379))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp37C,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1059;Cyc_Tcutil_failure_reason=((
_tmp1059="(type variable would escape scope)",_tag_dyneither(_tmp1059,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp36F->v,_tmp373)){{struct Cyc_Core_Opt*_tmp105A;*_tmp378=((
_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A->v=t1,_tmp105A))));}return;}{
const char*_tmp105B;Cyc_Tcutil_failure_reason=((_tmp105B="(kinds are incompatible)",
_tag_dyneither(_tmp105B,sizeof(char),25)));}goto _LL213;}_LL216:{struct Cyc_Absyn_PointerType_struct*
_tmp37A=(struct Cyc_Absyn_PointerType_struct*)_tmp375;if(_tmp37A->tag != 5)goto
_LL218;else{_tmp37B=_tmp37A->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp36F->v)->kind
== Cyc_Absyn_BoxKind))goto _LL218;_LL217: {union Cyc_Absyn_Constraint*_tmp380=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp37B.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp381=_tmp380;union Cyc_Absyn_Constraint
_tmp382;int _tmp383;_LL21B: _tmp382=*_tmp381;if((_tmp382.No_constr).tag != 3)goto
_LL21D;_tmp383=(int)(_tmp382.No_constr).val;_LL21C:{struct
_union_Constraint_Eq_constr*_tmp105C;(_tmp105C=& _tmp380->Eq_constr,((_tmp105C->tag=
1,_tmp105C->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp105D;*_tmp371=((
_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D->v=t2,_tmp105D))));}return;
_LL21D:;_LL21E: goto _LL21A;_LL21A:;}goto _LL213;}_LL218:;_LL219: goto _LL213;_LL213:;}{
const char*_tmp105E;Cyc_Tcutil_failure_reason=((_tmp105E="(kinds are incompatible)",
_tag_dyneither(_tmp105E,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL211:;_LL212: goto _LL20E;_LL20E:;}{struct _tuple0 _tmp105F;struct _tuple0 _tmp388=(
_tmp105F.f1=t2,((_tmp105F.f2=t1,_tmp105F)));void*_tmp389;void*_tmp38B;void*
_tmp38D;void*_tmp38F;struct Cyc_Absyn_Tvar*_tmp391;void*_tmp392;struct Cyc_Absyn_Tvar*
_tmp394;void*_tmp395;struct Cyc_Absyn_AggrInfo _tmp397;union Cyc_Absyn_AggrInfoU
_tmp398;struct Cyc_List_List*_tmp399;void*_tmp39A;struct Cyc_Absyn_AggrInfo _tmp39C;
union Cyc_Absyn_AggrInfoU _tmp39D;struct Cyc_List_List*_tmp39E;void*_tmp39F;struct
_tuple2*_tmp3A1;void*_tmp3A2;struct _tuple2*_tmp3A4;void*_tmp3A5;struct Cyc_List_List*
_tmp3A7;void*_tmp3A8;struct Cyc_List_List*_tmp3AA;void*_tmp3AB;struct Cyc_Absyn_DatatypeInfo
_tmp3AD;union Cyc_Absyn_DatatypeInfoU _tmp3AE;struct Cyc_Absyn_Datatypedecl**
_tmp3AF;struct Cyc_Absyn_Datatypedecl*_tmp3B0;struct Cyc_List_List*_tmp3B1;void*
_tmp3B2;struct Cyc_Absyn_DatatypeInfo _tmp3B4;union Cyc_Absyn_DatatypeInfoU _tmp3B5;
struct Cyc_Absyn_Datatypedecl**_tmp3B6;struct Cyc_Absyn_Datatypedecl*_tmp3B7;
struct Cyc_List_List*_tmp3B8;void*_tmp3B9;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3BB;union Cyc_Absyn_DatatypeFieldInfoU _tmp3BC;struct _tuple3 _tmp3BD;struct Cyc_Absyn_Datatypedecl*
_tmp3BE;struct Cyc_Absyn_Datatypefield*_tmp3BF;struct Cyc_List_List*_tmp3C0;void*
_tmp3C1;struct Cyc_Absyn_DatatypeFieldInfo _tmp3C3;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3C4;struct _tuple3 _tmp3C5;struct Cyc_Absyn_Datatypedecl*_tmp3C6;struct Cyc_Absyn_Datatypefield*
_tmp3C7;struct Cyc_List_List*_tmp3C8;void*_tmp3C9;struct Cyc_Absyn_PtrInfo _tmp3CB;
void*_tmp3CC;struct Cyc_Absyn_Tqual _tmp3CD;struct Cyc_Absyn_PtrAtts _tmp3CE;void*
_tmp3CF;union Cyc_Absyn_Constraint*_tmp3D0;union Cyc_Absyn_Constraint*_tmp3D1;
union Cyc_Absyn_Constraint*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_PtrInfo _tmp3D5;
void*_tmp3D6;struct Cyc_Absyn_Tqual _tmp3D7;struct Cyc_Absyn_PtrAtts _tmp3D8;void*
_tmp3D9;union Cyc_Absyn_Constraint*_tmp3DA;union Cyc_Absyn_Constraint*_tmp3DB;
union Cyc_Absyn_Constraint*_tmp3DC;void*_tmp3DD;enum Cyc_Absyn_Sign _tmp3DF;enum 
Cyc_Absyn_Size_of _tmp3E0;void*_tmp3E1;enum Cyc_Absyn_Sign _tmp3E3;enum Cyc_Absyn_Size_of
_tmp3E4;void*_tmp3E5;void*_tmp3E7;void*_tmp3E9;int _tmp3EB;void*_tmp3EC;int
_tmp3EE;void*_tmp3EF;void*_tmp3F1;void*_tmp3F2;void*_tmp3F4;void*_tmp3F5;struct
Cyc_Absyn_Exp*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FA;void*_tmp3FB;
struct Cyc_Absyn_ArrayInfo _tmp3FD;void*_tmp3FE;struct Cyc_Absyn_Tqual _tmp3FF;
struct Cyc_Absyn_Exp*_tmp400;union Cyc_Absyn_Constraint*_tmp401;void*_tmp402;
struct Cyc_Absyn_ArrayInfo _tmp404;void*_tmp405;struct Cyc_Absyn_Tqual _tmp406;
struct Cyc_Absyn_Exp*_tmp407;union Cyc_Absyn_Constraint*_tmp408;void*_tmp409;
struct Cyc_Absyn_FnInfo _tmp40B;struct Cyc_List_List*_tmp40C;struct Cyc_Core_Opt*
_tmp40D;void*_tmp40E;struct Cyc_List_List*_tmp40F;int _tmp410;struct Cyc_Absyn_VarargInfo*
_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_List_List*_tmp413;void*_tmp414;
struct Cyc_Absyn_FnInfo _tmp416;struct Cyc_List_List*_tmp417;struct Cyc_Core_Opt*
_tmp418;void*_tmp419;struct Cyc_List_List*_tmp41A;int _tmp41B;struct Cyc_Absyn_VarargInfo*
_tmp41C;struct Cyc_List_List*_tmp41D;struct Cyc_List_List*_tmp41E;void*_tmp41F;
struct Cyc_List_List*_tmp421;void*_tmp422;struct Cyc_List_List*_tmp424;void*
_tmp425;enum Cyc_Absyn_AggrKind _tmp427;struct Cyc_List_List*_tmp428;void*_tmp429;
enum Cyc_Absyn_AggrKind _tmp42B;struct Cyc_List_List*_tmp42C;void*_tmp42D;void*
_tmp42F;void*_tmp431;void*_tmp433;void*_tmp435;void*_tmp437;void*_tmp438;void*
_tmp43A;void*_tmp43B;void*_tmp43D;void*_tmp43E;void*_tmp43F;void*_tmp441;void*
_tmp442;void*_tmp443;void*_tmp445;void*_tmp447;void*_tmp449;void*_tmp44B;void*
_tmp44D;_LL220: _tmp389=_tmp388.f1;{struct Cyc_Absyn_Evar_struct*_tmp38A=(struct
Cyc_Absyn_Evar_struct*)_tmp389;if(_tmp38A->tag != 1)goto _LL222;};_LL221: Cyc_Tcutil_unify_it(
t2,t1);return;_LL222: _tmp38B=_tmp388.f1;{struct Cyc_Absyn_VoidType_struct*_tmp38C=(
struct Cyc_Absyn_VoidType_struct*)_tmp38B;if(_tmp38C->tag != 0)goto _LL224;};
_tmp38D=_tmp388.f2;{struct Cyc_Absyn_VoidType_struct*_tmp38E=(struct Cyc_Absyn_VoidType_struct*)
_tmp38D;if(_tmp38E->tag != 0)goto _LL224;};_LL223: return;_LL224: _tmp38F=_tmp388.f1;{
struct Cyc_Absyn_VarType_struct*_tmp390=(struct Cyc_Absyn_VarType_struct*)_tmp38F;
if(_tmp390->tag != 2)goto _LL226;else{_tmp391=_tmp390->f1;}};_tmp392=_tmp388.f2;{
struct Cyc_Absyn_VarType_struct*_tmp393=(struct Cyc_Absyn_VarType_struct*)_tmp392;
if(_tmp393->tag != 2)goto _LL226;else{_tmp394=_tmp393->f1;}};_LL225: {struct
_dyneither_ptr*_tmp44F=_tmp391->name;struct _dyneither_ptr*_tmp450=_tmp394->name;
int _tmp451=_tmp391->identity;int _tmp452=_tmp394->identity;if(_tmp452 == _tmp451)
return;{const char*_tmp1060;Cyc_Tcutil_failure_reason=((_tmp1060="(variable types are not the same)",
_tag_dyneither(_tmp1060,sizeof(char),34)));}goto _LL21F;}_LL226: _tmp395=_tmp388.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp396=(struct Cyc_Absyn_AggrType_struct*)
_tmp395;if(_tmp396->tag != 12)goto _LL228;else{_tmp397=_tmp396->f1;_tmp398=_tmp397.aggr_info;
_tmp399=_tmp397.targs;}};_tmp39A=_tmp388.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp39B=(struct Cyc_Absyn_AggrType_struct*)_tmp39A;if(_tmp39B->tag != 12)goto
_LL228;else{_tmp39C=_tmp39B->f1;_tmp39D=_tmp39C.aggr_info;_tmp39E=_tmp39C.targs;}};
_LL227: {enum Cyc_Absyn_AggrKind _tmp455;struct _tuple2*_tmp456;struct _tuple12
_tmp454=Cyc_Absyn_aggr_kinded_name(_tmp39D);_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;{
enum Cyc_Absyn_AggrKind _tmp458;struct _tuple2*_tmp459;struct _tuple12 _tmp457=Cyc_Absyn_aggr_kinded_name(
_tmp398);_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;if(_tmp455 != _tmp458){{const char*
_tmp1061;Cyc_Tcutil_failure_reason=((_tmp1061="(struct and union type)",
_tag_dyneither(_tmp1061,sizeof(char),24)));}goto _LL21F;}if(Cyc_Absyn_qvar_cmp(
_tmp456,_tmp459)!= 0){{const char*_tmp1062;Cyc_Tcutil_failure_reason=((_tmp1062="(different type name)",
_tag_dyneither(_tmp1062,sizeof(char),22)));}goto _LL21F;}Cyc_Tcutil_unify_list(
_tmp39E,_tmp399);return;};}_LL228: _tmp39F=_tmp388.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp3A0=(struct Cyc_Absyn_EnumType_struct*)_tmp39F;if(_tmp3A0->tag != 14)goto
_LL22A;else{_tmp3A1=_tmp3A0->f1;}};_tmp3A2=_tmp388.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp3A3=(struct Cyc_Absyn_EnumType_struct*)_tmp3A2;if(_tmp3A3->tag != 14)goto
_LL22A;else{_tmp3A4=_tmp3A3->f1;}};_LL229: if(Cyc_Absyn_qvar_cmp(_tmp3A1,_tmp3A4)
== 0)return;{const char*_tmp1063;Cyc_Tcutil_failure_reason=((_tmp1063="(different enum types)",
_tag_dyneither(_tmp1063,sizeof(char),23)));}goto _LL21F;_LL22A: _tmp3A5=_tmp388.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp3A6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A5;if(_tmp3A6->tag != 15)goto _LL22C;else{_tmp3A7=_tmp3A6->f1;}};_tmp3A8=
_tmp388.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp3A9=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A8;if(_tmp3A9->tag != 15)goto _LL22C;else{_tmp3AA=_tmp3A9->f1;}};_LL22B: {int
bad=0;for(0;_tmp3A7 != 0  && _tmp3AA != 0;(_tmp3A7=_tmp3A7->tl,_tmp3AA=_tmp3AA->tl)){
struct Cyc_Absyn_Enumfield*_tmp45D=(struct Cyc_Absyn_Enumfield*)_tmp3A7->hd;struct
Cyc_Absyn_Enumfield*_tmp45E=(struct Cyc_Absyn_Enumfield*)_tmp3AA->hd;if(Cyc_Absyn_qvar_cmp(
_tmp45D->name,_tmp45E->name)!= 0){{const char*_tmp1064;Cyc_Tcutil_failure_reason=((
_tmp1064="(different names for enum fields)",_tag_dyneither(_tmp1064,sizeof(char),
34)));}bad=1;break;}if(_tmp45D->tag == _tmp45E->tag)continue;if(_tmp45D->tag == 0
 || _tmp45E->tag == 0){{const char*_tmp1065;Cyc_Tcutil_failure_reason=((_tmp1065="(different tag values for enum fields)",
_tag_dyneither(_tmp1065,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp45D->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp45E->tag))){{const char*_tmp1066;Cyc_Tcutil_failure_reason=((_tmp1066="(different tag values for enum fields)",
_tag_dyneither(_tmp1066,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21F;if(
_tmp3A7 == 0  && _tmp3AA == 0)return;{const char*_tmp1067;Cyc_Tcutil_failure_reason=((
_tmp1067="(different number of fields for enums)",_tag_dyneither(_tmp1067,
sizeof(char),39)));}goto _LL21F;}_LL22C: _tmp3AB=_tmp388.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp3AC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp3AB;if(_tmp3AC->tag != 3)goto
_LL22E;else{_tmp3AD=_tmp3AC->f1;_tmp3AE=_tmp3AD.datatype_info;if((_tmp3AE.KnownDatatype).tag
!= 2)goto _LL22E;_tmp3AF=(struct Cyc_Absyn_Datatypedecl**)(_tmp3AE.KnownDatatype).val;
_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3AD.targs;}};_tmp3B2=_tmp388.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp3B3=(struct Cyc_Absyn_DatatypeType_struct*)_tmp3B2;if(_tmp3B3->tag != 3)goto
_LL22E;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B4.datatype_info;if((_tmp3B5.KnownDatatype).tag
!= 2)goto _LL22E;_tmp3B6=(struct Cyc_Absyn_Datatypedecl**)(_tmp3B5.KnownDatatype).val;
_tmp3B7=*_tmp3B6;_tmp3B8=_tmp3B4.targs;}};_LL22D: if(_tmp3B0 == _tmp3B7  || Cyc_Absyn_qvar_cmp(
_tmp3B0->name,_tmp3B7->name)== 0){Cyc_Tcutil_unify_list(_tmp3B8,_tmp3B1);return;}
goto _LL21F;_LL22E: _tmp3B9=_tmp388.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3BA=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3B9;if(_tmp3BA->tag != 4)
goto _LL230;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BB.field_info;if((_tmp3BC.KnownDatatypefield).tag
!= 2)goto _LL230;_tmp3BD=(struct _tuple3)(_tmp3BC.KnownDatatypefield).val;_tmp3BE=
_tmp3BD.f1;_tmp3BF=_tmp3BD.f2;_tmp3C0=_tmp3BB.targs;}};_tmp3C1=_tmp388.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp3C2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3C1;if(_tmp3C2->tag != 4)goto _LL230;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C3.field_info;
if((_tmp3C4.KnownDatatypefield).tag != 2)goto _LL230;_tmp3C5=(struct _tuple3)(
_tmp3C4.KnownDatatypefield).val;_tmp3C6=_tmp3C5.f1;_tmp3C7=_tmp3C5.f2;_tmp3C8=
_tmp3C3.targs;}};_LL22F: if((_tmp3BE == _tmp3C6  || Cyc_Absyn_qvar_cmp(_tmp3BE->name,
_tmp3C6->name)== 0) && (_tmp3BF == _tmp3C7  || Cyc_Absyn_qvar_cmp(_tmp3BF->name,
_tmp3C7->name)== 0)){Cyc_Tcutil_unify_list(_tmp3C8,_tmp3C0);return;}{const char*
_tmp1068;Cyc_Tcutil_failure_reason=((_tmp1068="(different datatype field types)",
_tag_dyneither(_tmp1068,sizeof(char),33)));}goto _LL21F;_LL230: _tmp3C9=_tmp388.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp3CA=(struct Cyc_Absyn_PointerType_struct*)
_tmp3C9;if(_tmp3CA->tag != 5)goto _LL232;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CB.elt_typ;
_tmp3CD=_tmp3CB.elt_tq;_tmp3CE=_tmp3CB.ptr_atts;_tmp3CF=_tmp3CE.rgn;_tmp3D0=
_tmp3CE.nullable;_tmp3D1=_tmp3CE.bounds;_tmp3D2=_tmp3CE.zero_term;}};_tmp3D3=
_tmp388.f2;{struct Cyc_Absyn_PointerType_struct*_tmp3D4=(struct Cyc_Absyn_PointerType_struct*)
_tmp3D3;if(_tmp3D4->tag != 5)goto _LL232;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D5.elt_typ;
_tmp3D7=_tmp3D5.elt_tq;_tmp3D8=_tmp3D5.ptr_atts;_tmp3D9=_tmp3D8.rgn;_tmp3DA=
_tmp3D8.nullable;_tmp3DB=_tmp3D8.bounds;_tmp3DC=_tmp3D8.zero_term;}};_LL231: Cyc_Tcutil_unify_it(
_tmp3D6,_tmp3CC);Cyc_Tcutil_unify_it(_tmp3CF,_tmp3D9);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1069;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3DC,_tmp3D2,((_tmp1069="(not both zero terminated)",
_tag_dyneither(_tmp1069,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp3D7,
_tmp3D6,_tmp3CD,_tmp3CC);{const char*_tmp106A;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3DB,_tmp3D1,((
_tmp106A="(different pointer bounds)",_tag_dyneither(_tmp106A,sizeof(char),27))));}{
void*_tmp466=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp3DB);_LL25B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp467=(struct Cyc_Absyn_DynEither_b_struct*)_tmp466;if(_tmp467->tag != 0)goto
_LL25D;}_LL25C: return;_LL25D:;_LL25E: goto _LL25A;_LL25A:;}{const char*_tmp106B;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3DA,_tmp3D0,((_tmp106B="(incompatible pointer types)",_tag_dyneither(
_tmp106B,sizeof(char),29))));}return;_LL232: _tmp3DD=_tmp388.f1;{struct Cyc_Absyn_IntType_struct*
_tmp3DE=(struct Cyc_Absyn_IntType_struct*)_tmp3DD;if(_tmp3DE->tag != 6)goto _LL234;
else{_tmp3DF=_tmp3DE->f1;_tmp3E0=_tmp3DE->f2;}};_tmp3E1=_tmp388.f2;{struct Cyc_Absyn_IntType_struct*
_tmp3E2=(struct Cyc_Absyn_IntType_struct*)_tmp3E1;if(_tmp3E2->tag != 6)goto _LL234;
else{_tmp3E3=_tmp3E2->f1;_tmp3E4=_tmp3E2->f2;}};_LL233: if(_tmp3E3 == _tmp3DF  && ((
_tmp3E4 == _tmp3E0  || _tmp3E4 == Cyc_Absyn_Int_sz  && _tmp3E0 == Cyc_Absyn_Long_sz)
 || _tmp3E4 == Cyc_Absyn_Long_sz  && _tmp3E0 == Cyc_Absyn_Int_sz))return;{const char*
_tmp106C;Cyc_Tcutil_failure_reason=((_tmp106C="(different integral types)",
_tag_dyneither(_tmp106C,sizeof(char),27)));}goto _LL21F;_LL234: _tmp3E5=_tmp388.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp3E6=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E5;if(_tmp3E6->tag != 7)goto _LL236;};_tmp3E7=_tmp388.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp3E8=(struct Cyc_Absyn_FloatType_struct*)_tmp3E7;if(_tmp3E8->tag != 7)goto
_LL236;};_LL235: return;_LL236: _tmp3E9=_tmp388.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp3EA=(struct Cyc_Absyn_DoubleType_struct*)_tmp3E9;if(_tmp3EA->tag != 8)goto
_LL238;else{_tmp3EB=_tmp3EA->f1;}};_tmp3EC=_tmp388.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp3ED=(struct Cyc_Absyn_DoubleType_struct*)_tmp3EC;if(_tmp3ED->tag != 8)goto
_LL238;else{_tmp3EE=_tmp3ED->f1;}};_LL237: if(_tmp3EB == _tmp3EE)return;goto _LL21F;
_LL238: _tmp3EF=_tmp388.f1;{struct Cyc_Absyn_TagType_struct*_tmp3F0=(struct Cyc_Absyn_TagType_struct*)
_tmp3EF;if(_tmp3F0->tag != 20)goto _LL23A;else{_tmp3F1=(void*)_tmp3F0->f1;}};
_tmp3F2=_tmp388.f2;{struct Cyc_Absyn_TagType_struct*_tmp3F3=(struct Cyc_Absyn_TagType_struct*)
_tmp3F2;if(_tmp3F3->tag != 20)goto _LL23A;else{_tmp3F4=(void*)_tmp3F3->f1;}};
_LL239: Cyc_Tcutil_unify_it(_tmp3F1,_tmp3F4);return;_LL23A: _tmp3F5=_tmp388.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp3F6=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3F5;if(_tmp3F6->tag != 19)goto _LL23C;else{_tmp3F7=_tmp3F6->f1;}};_tmp3F8=
_tmp388.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp3F9=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3F8;if(_tmp3F9->tag != 19)goto _LL23C;else{_tmp3FA=_tmp3F9->f1;}};_LL23B: if(!
Cyc_Evexp_same_const_exp(_tmp3F7,_tmp3FA)){{const char*_tmp106D;Cyc_Tcutil_failure_reason=((
_tmp106D="(cannot prove expressions are the same)",_tag_dyneither(_tmp106D,
sizeof(char),40)));}goto _LL21F;}return;_LL23C: _tmp3FB=_tmp388.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp3FC=(struct Cyc_Absyn_ArrayType_struct*)_tmp3FB;if(_tmp3FC->tag != 9)goto
_LL23E;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FD.elt_type;_tmp3FF=_tmp3FD.tq;
_tmp400=_tmp3FD.num_elts;_tmp401=_tmp3FD.zero_term;}};_tmp402=_tmp388.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp403=(struct Cyc_Absyn_ArrayType_struct*)_tmp402;
if(_tmp403->tag != 9)goto _LL23E;else{_tmp404=_tmp403->f1;_tmp405=_tmp404.elt_type;
_tmp406=_tmp404.tq;_tmp407=_tmp404.num_elts;_tmp408=_tmp404.zero_term;}};_LL23D:
Cyc_Tcutil_unify_it(_tmp405,_tmp3FE);Cyc_Tcutil_unify_tqual(_tmp406,_tmp405,
_tmp3FF,_tmp3FE);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp106E;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp401,_tmp408,((_tmp106E="(not both zero terminated)",_tag_dyneither(_tmp106E,
sizeof(char),27))));}if(_tmp400 == _tmp407)return;if(_tmp400 == 0  || _tmp407 == 0)
goto _LL21F;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp400,(struct Cyc_Absyn_Exp*)
_tmp407))return;{const char*_tmp106F;Cyc_Tcutil_failure_reason=((_tmp106F="(different array sizes)",
_tag_dyneither(_tmp106F,sizeof(char),24)));}goto _LL21F;_LL23E: _tmp409=_tmp388.f1;{
struct Cyc_Absyn_FnType_struct*_tmp40A=(struct Cyc_Absyn_FnType_struct*)_tmp409;
if(_tmp40A->tag != 10)goto _LL240;else{_tmp40B=_tmp40A->f1;_tmp40C=_tmp40B.tvars;
_tmp40D=_tmp40B.effect;_tmp40E=_tmp40B.ret_typ;_tmp40F=_tmp40B.args;_tmp410=
_tmp40B.c_varargs;_tmp411=_tmp40B.cyc_varargs;_tmp412=_tmp40B.rgn_po;_tmp413=
_tmp40B.attributes;}};_tmp414=_tmp388.f2;{struct Cyc_Absyn_FnType_struct*_tmp415=(
struct Cyc_Absyn_FnType_struct*)_tmp414;if(_tmp415->tag != 10)goto _LL240;else{
_tmp416=_tmp415->f1;_tmp417=_tmp416.tvars;_tmp418=_tmp416.effect;_tmp419=_tmp416.ret_typ;
_tmp41A=_tmp416.args;_tmp41B=_tmp416.c_varargs;_tmp41C=_tmp416.cyc_varargs;
_tmp41D=_tmp416.rgn_po;_tmp41E=_tmp416.attributes;}};_LL23F: {int done=0;struct
_RegionHandle _tmp46D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp46D;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp417 != 0){if(_tmp40C == 0){{
const char*_tmp1070;Cyc_Tcutil_failure_reason=((_tmp1070="(second function type has too few type variables)",
_tag_dyneither(_tmp1070,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp46F=((struct Cyc_Absyn_Tvar*)_tmp417->hd)->kind;void*_tmp470=((struct Cyc_Absyn_Tvar*)
_tmp40C->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp46F,_tmp470)){{const char*
_tmp1076;void*_tmp1075[3];struct Cyc_String_pa_struct _tmp1074;struct Cyc_String_pa_struct
_tmp1073;struct Cyc_String_pa_struct _tmp1072;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1072.tag=0,((_tmp1072.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp40C->hd,& Cyc_Tcutil_bk))),((_tmp1073.tag=0,((_tmp1073.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp417->hd,& Cyc_Tcutil_bk))),((_tmp1074.tag=0,((_tmp1074.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp417->hd)),((_tmp1075[0]=& _tmp1074,((_tmp1075[1]=& _tmp1073,((_tmp1075[2]=&
_tmp1072,Cyc_aprintf(((_tmp1076="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1076,sizeof(char),44))),_tag_dyneither(_tmp1075,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple14*_tmp1080;struct Cyc_Absyn_VarType_struct _tmp107F;struct Cyc_Absyn_VarType_struct*
_tmp107E;struct Cyc_List_List*_tmp107D;inst=((_tmp107D=_region_malloc(rgn,sizeof(*
_tmp107D)),((_tmp107D->hd=((_tmp1080=_region_malloc(rgn,sizeof(*_tmp1080)),((
_tmp1080->f1=(struct Cyc_Absyn_Tvar*)_tmp40C->hd,((_tmp1080->f2=(void*)((_tmp107E=
_cycalloc(sizeof(*_tmp107E)),((_tmp107E[0]=((_tmp107F.tag=2,((_tmp107F.f1=(
struct Cyc_Absyn_Tvar*)_tmp417->hd,_tmp107F)))),_tmp107E)))),_tmp1080)))))),((
_tmp107D->tl=inst,_tmp107D))))));}_tmp417=_tmp417->tl;_tmp40C=_tmp40C->tl;};}if(
_tmp40C != 0){{const char*_tmp1081;Cyc_Tcutil_failure_reason=((_tmp1081="(second function type has too many type variables)",
_tag_dyneither(_tmp1081,sizeof(char),51)));}_npop_handler(0);goto _LL21F;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp108D;struct Cyc_Absyn_FnInfo _tmp108C;
struct Cyc_Absyn_FnType_struct*_tmp108B;struct Cyc_Absyn_FnType_struct _tmp1087;
struct Cyc_Absyn_FnInfo _tmp1086;struct Cyc_Absyn_FnType_struct*_tmp1085;Cyc_Tcutil_unify_it((
void*)((_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085[0]=((_tmp1087.tag=10,((
_tmp1087.f1=((_tmp1086.tvars=0,((_tmp1086.effect=_tmp418,((_tmp1086.ret_typ=
_tmp419,((_tmp1086.args=_tmp41A,((_tmp1086.c_varargs=_tmp41B,((_tmp1086.cyc_varargs=
_tmp41C,((_tmp1086.rgn_po=_tmp41D,((_tmp1086.attributes=_tmp41E,_tmp1086)))))))))))))))),
_tmp1087)))),_tmp1085)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp108B=
_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((_tmp108D.tag=10,((_tmp108D.f1=((
_tmp108C.tvars=0,((_tmp108C.effect=_tmp40D,((_tmp108C.ret_typ=_tmp40E,((_tmp108C.args=
_tmp40F,((_tmp108C.c_varargs=_tmp410,((_tmp108C.cyc_varargs=_tmp411,((_tmp108C.rgn_po=
_tmp412,((_tmp108C.attributes=_tmp413,_tmp108C)))))))))))))))),_tmp108D)))),
_tmp108B))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp419,_tmp40E);for(0;_tmp41A != 0  && _tmp40F != 0;(_tmp41A=_tmp41A->tl,_tmp40F=
_tmp40F->tl)){struct Cyc_Absyn_Tqual _tmp482;void*_tmp483;struct _tuple9 _tmp481=*((
struct _tuple9*)_tmp41A->hd);_tmp482=_tmp481.f2;_tmp483=_tmp481.f3;{struct Cyc_Absyn_Tqual
_tmp485;void*_tmp486;struct _tuple9 _tmp484=*((struct _tuple9*)_tmp40F->hd);_tmp485=
_tmp484.f2;_tmp486=_tmp484.f3;Cyc_Tcutil_unify_it(_tmp483,_tmp486);Cyc_Tcutil_unify_tqual(
_tmp482,_tmp483,_tmp485,_tmp486);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp41A != 0  || _tmp40F != 0){{const char*_tmp108E;Cyc_Tcutil_failure_reason=((
_tmp108E="(function types have different number of arguments)",_tag_dyneither(
_tmp108E,sizeof(char),52)));}_npop_handler(0);goto _LL21F;}if(_tmp41B != _tmp410){{
const char*_tmp108F;Cyc_Tcutil_failure_reason=((_tmp108F="(only one function type takes C varargs)",
_tag_dyneither(_tmp108F,sizeof(char),41)));}_npop_handler(0);goto _LL21F;}{int
bad_cyc_vararg=0;{struct _tuple19 _tmp1090;struct _tuple19 _tmp48A=(_tmp1090.f1=
_tmp41C,((_tmp1090.f2=_tmp411,_tmp1090)));struct Cyc_Absyn_VarargInfo*_tmp48B;
struct Cyc_Absyn_VarargInfo*_tmp48C;struct Cyc_Absyn_VarargInfo*_tmp48D;struct Cyc_Absyn_VarargInfo*
_tmp48E;struct Cyc_Absyn_VarargInfo*_tmp48F;struct Cyc_Absyn_VarargInfo _tmp490;
struct Cyc_Core_Opt*_tmp491;struct Cyc_Absyn_Tqual _tmp492;void*_tmp493;int _tmp494;
struct Cyc_Absyn_VarargInfo*_tmp495;struct Cyc_Absyn_VarargInfo _tmp496;struct Cyc_Core_Opt*
_tmp497;struct Cyc_Absyn_Tqual _tmp498;void*_tmp499;int _tmp49A;_LL260: _tmp48B=
_tmp48A.f1;if(_tmp48B != 0)goto _LL262;_tmp48C=_tmp48A.f2;if(_tmp48C != 0)goto
_LL262;_LL261: goto _LL25F;_LL262: _tmp48D=_tmp48A.f1;if(_tmp48D != 0)goto _LL264;
_LL263: goto _LL265;_LL264: _tmp48E=_tmp48A.f2;if(_tmp48E != 0)goto _LL266;_LL265:
bad_cyc_vararg=1;{const char*_tmp1091;Cyc_Tcutil_failure_reason=((_tmp1091="(only one function type takes varargs)",
_tag_dyneither(_tmp1091,sizeof(char),39)));}goto _LL25F;_LL266: _tmp48F=_tmp48A.f1;
if(_tmp48F == 0)goto _LL25F;_tmp490=*_tmp48F;_tmp491=_tmp490.name;_tmp492=_tmp490.tq;
_tmp493=_tmp490.type;_tmp494=_tmp490.inject;_tmp495=_tmp48A.f2;if(_tmp495 == 0)
goto _LL25F;_tmp496=*_tmp495;_tmp497=_tmp496.name;_tmp498=_tmp496.tq;_tmp499=
_tmp496.type;_tmp49A=_tmp496.inject;_LL267: Cyc_Tcutil_unify_it(_tmp493,_tmp499);
Cyc_Tcutil_unify_tqual(_tmp492,_tmp493,_tmp498,_tmp499);if(_tmp494 != _tmp49A){
bad_cyc_vararg=1;{const char*_tmp1092;Cyc_Tcutil_failure_reason=((_tmp1092="(only one function type injects varargs)",
_tag_dyneither(_tmp1092,sizeof(char),41)));};}goto _LL25F;_LL25F:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21F;}{int bad_effect=0;{struct _tuple20
_tmp1093;struct _tuple20 _tmp49E=(_tmp1093.f1=_tmp418,((_tmp1093.f2=_tmp40D,
_tmp1093)));struct Cyc_Core_Opt*_tmp49F;struct Cyc_Core_Opt*_tmp4A0;struct Cyc_Core_Opt*
_tmp4A1;struct Cyc_Core_Opt*_tmp4A2;_LL269: _tmp49F=_tmp49E.f1;if(_tmp49F != 0)goto
_LL26B;_tmp4A0=_tmp49E.f2;if(_tmp4A0 != 0)goto _LL26B;_LL26A: goto _LL268;_LL26B:
_tmp4A1=_tmp49E.f1;if(_tmp4A1 != 0)goto _LL26D;_LL26C: goto _LL26E;_LL26D: _tmp4A2=
_tmp49E.f2;if(_tmp4A2 != 0)goto _LL26F;_LL26E: bad_effect=1;goto _LL268;_LL26F:;
_LL270: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp418))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp40D))->v);
goto _LL268;_LL268:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1094;Cyc_Tcutil_failure_reason=((_tmp1094="(function type effects do not unify)",
_tag_dyneither(_tmp1094,sizeof(char),37)));}_npop_handler(0);goto _LL21F;}if(!Cyc_Tcutil_same_atts(
_tmp413,_tmp41E)){{const char*_tmp1095;Cyc_Tcutil_failure_reason=((_tmp1095="(function types have different attributes)",
_tag_dyneither(_tmp1095,sizeof(char),43)));}_npop_handler(0);goto _LL21F;}if(!Cyc_Tcutil_same_rgn_po(
_tmp412,_tmp41D)){{const char*_tmp1096;Cyc_Tcutil_failure_reason=((_tmp1096="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1096,sizeof(char),58)));}_npop_handler(0);goto _LL21F;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL240: _tmp41F=_tmp388.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp420=(struct Cyc_Absyn_TupleType_struct*)_tmp41F;
if(_tmp420->tag != 11)goto _LL242;else{_tmp421=_tmp420->f1;}};_tmp422=_tmp388.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp423=(struct Cyc_Absyn_TupleType_struct*)
_tmp422;if(_tmp423->tag != 11)goto _LL242;else{_tmp424=_tmp423->f1;}};_LL241: for(0;
_tmp424 != 0  && _tmp421 != 0;(_tmp424=_tmp424->tl,_tmp421=_tmp421->tl)){struct Cyc_Absyn_Tqual
_tmp4A7;void*_tmp4A8;struct _tuple11 _tmp4A6=*((struct _tuple11*)_tmp424->hd);
_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4AB;
struct _tuple11 _tmp4A9=*((struct _tuple11*)_tmp421->hd);_tmp4AA=_tmp4A9.f1;_tmp4AB=
_tmp4A9.f2;Cyc_Tcutil_unify_it(_tmp4A8,_tmp4AB);Cyc_Tcutil_unify_tqual(_tmp4A7,
_tmp4A8,_tmp4AA,_tmp4AB);};}if(_tmp424 == 0  && _tmp421 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1097;Cyc_Tcutil_failure_reason=((
_tmp1097="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1097,sizeof(char),51)));}goto _LL21F;_LL242: _tmp425=_tmp388.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp426=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp425;if(_tmp426->tag != 13)goto
_LL244;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}};_tmp429=_tmp388.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp42A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp429;if(_tmp42A->tag != 13)goto _LL244;else{_tmp42B=_tmp42A->f1;_tmp42C=_tmp42A->f2;}};
_LL243: if(_tmp42B != _tmp427){{const char*_tmp1098;Cyc_Tcutil_failure_reason=((
_tmp1098="(struct and union type)",_tag_dyneither(_tmp1098,sizeof(char),24)));}
goto _LL21F;}for(0;_tmp42C != 0  && _tmp428 != 0;(_tmp42C=_tmp42C->tl,_tmp428=
_tmp428->tl)){struct Cyc_Absyn_Aggrfield*_tmp4AE=(struct Cyc_Absyn_Aggrfield*)
_tmp42C->hd;struct Cyc_Absyn_Aggrfield*_tmp4AF=(struct Cyc_Absyn_Aggrfield*)
_tmp428->hd;if(Cyc_strptrcmp(_tmp4AE->name,_tmp4AF->name)!= 0){{const char*
_tmp1099;Cyc_Tcutil_failure_reason=((_tmp1099="(different member names)",
_tag_dyneither(_tmp1099,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp4AE->type,_tmp4AF->type);Cyc_Tcutil_unify_tqual(_tmp4AE->tq,
_tmp4AE->type,_tmp4AF->tq,_tmp4AF->type);if(!Cyc_Tcutil_same_atts(_tmp4AE->attributes,
_tmp4AF->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp109A;Cyc_Tcutil_failure_reason=((_tmp109A="(different attributes on member)",
_tag_dyneither(_tmp109A,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp4AE->width != 0  && _tmp4AF->width == 0  || _tmp4AF->width != 0  && _tmp4AE->width
== 0) || (_tmp4AE->width != 0  && _tmp4AF->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp4AE->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp4AF->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp109B;Cyc_Tcutil_failure_reason=((_tmp109B="(different bitfield widths on member)",
_tag_dyneither(_tmp109B,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp42C == 0  && _tmp428 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp109C;Cyc_Tcutil_failure_reason=((_tmp109C="(different number of members)",
_tag_dyneither(_tmp109C,sizeof(char),30)));}goto _LL21F;_LL244: _tmp42D=_tmp388.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp42E=(struct Cyc_Absyn_HeapRgn_struct*)_tmp42D;
if(_tmp42E->tag != 21)goto _LL246;};_tmp42F=_tmp388.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp430=(struct Cyc_Absyn_HeapRgn_struct*)_tmp42F;if(_tmp430->tag != 21)goto _LL246;};
_LL245: return;_LL246: _tmp431=_tmp388.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp432=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp431;if(_tmp432->tag != 22)goto
_LL248;};_tmp433=_tmp388.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp434=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp433;if(_tmp434->tag != 22)goto _LL248;};_LL247: return;_LL248: _tmp435=_tmp388.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp436=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp435;if(_tmp436->tag != 16)goto _LL24A;else{_tmp437=(void*)_tmp436->f1;}};
_tmp438=_tmp388.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp439=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp438;if(_tmp439->tag != 16)goto _LL24A;else{_tmp43A=(void*)_tmp439->f1;}};
_LL249: Cyc_Tcutil_unify_it(_tmp437,_tmp43A);return;_LL24A: _tmp43B=_tmp388.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp43C=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp43B;if(_tmp43C->tag != 17)goto _LL24C;else{_tmp43D=(void*)_tmp43C->f1;_tmp43E=(
void*)_tmp43C->f2;}};_tmp43F=_tmp388.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp440=(struct Cyc_Absyn_DynRgnType_struct*)_tmp43F;if(_tmp440->tag != 17)goto
_LL24C;else{_tmp441=(void*)_tmp440->f1;_tmp442=(void*)_tmp440->f2;}};_LL24B: Cyc_Tcutil_unify_it(
_tmp43D,_tmp441);Cyc_Tcutil_unify_it(_tmp43E,_tmp442);return;_LL24C: _tmp443=
_tmp388.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp444=(struct Cyc_Absyn_JoinEff_struct*)
_tmp443;if(_tmp444->tag != 24)goto _LL24E;};_LL24D: goto _LL24F;_LL24E: _tmp445=
_tmp388.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp446=(struct Cyc_Absyn_JoinEff_struct*)
_tmp445;if(_tmp446->tag != 24)goto _LL250;};_LL24F: goto _LL251;_LL250: _tmp447=
_tmp388.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp448=(struct Cyc_Absyn_AccessEff_struct*)
_tmp447;if(_tmp448->tag != 23)goto _LL252;};_LL251: goto _LL253;_LL252: _tmp449=
_tmp388.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp44A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp449;if(_tmp44A->tag != 25)goto _LL254;};_LL253: goto _LL255;_LL254: _tmp44B=
_tmp388.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp44C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp44B;if(_tmp44C->tag != 25)goto _LL256;};_LL255: goto _LL257;_LL256: _tmp44D=
_tmp388.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp44E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp44D;if(_tmp44E->tag != 23)goto _LL258;};_LL257: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp109D;Cyc_Tcutil_failure_reason=((_tmp109D="(effects don't unify)",
_tag_dyneither(_tmp109D,sizeof(char),22)));}goto _LL21F;_LL258:;_LL259: goto _LL21F;
_LL21F:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp4B5=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp4B6=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp4B5,_tmp4B6);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4B7=x;union Cyc_Absyn_Constraint _tmp4B8;int _tmp4B9;
union Cyc_Absyn_Constraint _tmp4BA;void*_tmp4BB;union Cyc_Absyn_Constraint _tmp4BC;
union Cyc_Absyn_Constraint*_tmp4BD;_LL272: _tmp4B8=*_tmp4B7;if((_tmp4B8.No_constr).tag
!= 3)goto _LL274;_tmp4B9=(int)(_tmp4B8.No_constr).val;_LL273: return - 1;_LL274:
_tmp4BA=*_tmp4B7;if((_tmp4BA.Eq_constr).tag != 1)goto _LL276;_tmp4BB=(void*)(
_tmp4BA.Eq_constr).val;_LL275: {union Cyc_Absyn_Constraint*_tmp4BE=y;union Cyc_Absyn_Constraint
_tmp4BF;int _tmp4C0;union Cyc_Absyn_Constraint _tmp4C1;void*_tmp4C2;union Cyc_Absyn_Constraint
_tmp4C3;union Cyc_Absyn_Constraint*_tmp4C4;_LL279: _tmp4BF=*_tmp4BE;if((_tmp4BF.No_constr).tag
!= 3)goto _LL27B;_tmp4C0=(int)(_tmp4BF.No_constr).val;_LL27A: return 1;_LL27B:
_tmp4C1=*_tmp4BE;if((_tmp4C1.Eq_constr).tag != 1)goto _LL27D;_tmp4C2=(void*)(
_tmp4C1.Eq_constr).val;_LL27C: return cmp(_tmp4BB,_tmp4C2);_LL27D: _tmp4C3=*_tmp4BE;
if((_tmp4C3.Forward_constr).tag != 2)goto _LL278;_tmp4C4=(union Cyc_Absyn_Constraint*)(
_tmp4C3.Forward_constr).val;_LL27E: {const char*_tmp10A0;void*_tmp109F;(_tmp109F=
0,Cyc_Tcutil_impos(((_tmp10A0="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp10A0,sizeof(char),40))),_tag_dyneither(_tmp109F,sizeof(void*),
0)));}_LL278:;}_LL276: _tmp4BC=*_tmp4B7;if((_tmp4BC.Forward_constr).tag != 2)goto
_LL271;_tmp4BD=(union Cyc_Absyn_Constraint*)(_tmp4BC.Forward_constr).val;_LL277: {
const char*_tmp10A3;void*_tmp10A2;(_tmp10A2=0,Cyc_Tcutil_impos(((_tmp10A3="unify_conref: forward after compress",
_tag_dyneither(_tmp10A3,sizeof(char),37))),_tag_dyneither(_tmp10A2,sizeof(void*),
0)));}_LL271:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp4CA;struct Cyc_Absyn_Tqual _tmp4CB;void*_tmp4CC;
struct _tuple11*_tmp4C9=tqt1;_tmp4CA=*_tmp4C9;_tmp4CB=_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;{
struct _tuple11 _tmp4CE;struct Cyc_Absyn_Tqual _tmp4CF;void*_tmp4D0;struct _tuple11*
_tmp4CD=tqt2;_tmp4CE=*_tmp4CD;_tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;{int _tmp4D1=
Cyc_Tcutil_tqual_cmp(_tmp4CB,_tmp4CF);if(_tmp4D1 != 0)return _tmp4D1;return Cyc_Tcutil_typecmp(
_tmp4CC,_tmp4D0);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp4D2=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp4D2 != 0)return _tmp4D2;{int _tmp4D3=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp4D3 != 0)return _tmp4D3;{int _tmp4D4=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp4D4 != 0)return _tmp4D4;{int _tmp4D5=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp4D5 != 0)return _tmp4D5;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp4D6=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp4D6 != 0)return _tmp4D6;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp4D7=t;_LL280: {
struct Cyc_Absyn_VoidType_struct*_tmp4D8=(struct Cyc_Absyn_VoidType_struct*)
_tmp4D7;if(_tmp4D8->tag != 0)goto _LL282;}_LL281: return 0;_LL282: {struct Cyc_Absyn_Evar_struct*
_tmp4D9=(struct Cyc_Absyn_Evar_struct*)_tmp4D7;if(_tmp4D9->tag != 1)goto _LL284;}
_LL283: return 1;_LL284: {struct Cyc_Absyn_VarType_struct*_tmp4DA=(struct Cyc_Absyn_VarType_struct*)
_tmp4D7;if(_tmp4DA->tag != 2)goto _LL286;}_LL285: return 2;_LL286: {struct Cyc_Absyn_DatatypeType_struct*
_tmp4DB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4D7;if(_tmp4DB->tag != 3)goto
_LL288;}_LL287: return 3;_LL288: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4DC=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4D7;if(_tmp4DC->tag != 4)goto _LL28A;}
_LL289: return 4;_LL28A: {struct Cyc_Absyn_PointerType_struct*_tmp4DD=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D7;if(_tmp4DD->tag != 5)goto _LL28C;}_LL28B: return 5;_LL28C: {struct Cyc_Absyn_IntType_struct*
_tmp4DE=(struct Cyc_Absyn_IntType_struct*)_tmp4D7;if(_tmp4DE->tag != 6)goto _LL28E;}
_LL28D: return 6;_LL28E: {struct Cyc_Absyn_FloatType_struct*_tmp4DF=(struct Cyc_Absyn_FloatType_struct*)
_tmp4D7;if(_tmp4DF->tag != 7)goto _LL290;}_LL28F: return 7;_LL290: {struct Cyc_Absyn_DoubleType_struct*
_tmp4E0=(struct Cyc_Absyn_DoubleType_struct*)_tmp4D7;if(_tmp4E0->tag != 8)goto
_LL292;}_LL291: return 8;_LL292: {struct Cyc_Absyn_ArrayType_struct*_tmp4E1=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4D7;if(_tmp4E1->tag != 9)goto _LL294;}_LL293:
return 9;_LL294: {struct Cyc_Absyn_FnType_struct*_tmp4E2=(struct Cyc_Absyn_FnType_struct*)
_tmp4D7;if(_tmp4E2->tag != 10)goto _LL296;}_LL295: return 10;_LL296: {struct Cyc_Absyn_TupleType_struct*
_tmp4E3=(struct Cyc_Absyn_TupleType_struct*)_tmp4D7;if(_tmp4E3->tag != 11)goto
_LL298;}_LL297: return 11;_LL298: {struct Cyc_Absyn_AggrType_struct*_tmp4E4=(struct
Cyc_Absyn_AggrType_struct*)_tmp4D7;if(_tmp4E4->tag != 12)goto _LL29A;}_LL299:
return 12;_LL29A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4E5=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4D7;if(_tmp4E5->tag != 13)goto _LL29C;}_LL29B: return 14;_LL29C: {struct Cyc_Absyn_EnumType_struct*
_tmp4E6=(struct Cyc_Absyn_EnumType_struct*)_tmp4D7;if(_tmp4E6->tag != 14)goto
_LL29E;}_LL29D: return 16;_LL29E: {struct Cyc_Absyn_AnonEnumType_struct*_tmp4E7=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp4D7;if(_tmp4E7->tag != 15)goto _LL2A0;}
_LL29F: return 17;_LL2A0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4E8=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4D7;if(_tmp4E8->tag != 16)goto _LL2A2;}_LL2A1: return 18;_LL2A2: {struct Cyc_Absyn_TypedefType_struct*
_tmp4E9=(struct Cyc_Absyn_TypedefType_struct*)_tmp4D7;if(_tmp4E9->tag != 18)goto
_LL2A4;}_LL2A3: return 19;_LL2A4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4EA=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp4D7;if(_tmp4EA->tag != 22)goto _LL2A6;}
_LL2A5: return 20;_LL2A6: {struct Cyc_Absyn_HeapRgn_struct*_tmp4EB=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp4D7;if(_tmp4EB->tag != 21)goto _LL2A8;}_LL2A7: return 21;_LL2A8: {struct Cyc_Absyn_AccessEff_struct*
_tmp4EC=(struct Cyc_Absyn_AccessEff_struct*)_tmp4D7;if(_tmp4EC->tag != 23)goto
_LL2AA;}_LL2A9: return 22;_LL2AA: {struct Cyc_Absyn_JoinEff_struct*_tmp4ED=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4D7;if(_tmp4ED->tag != 24)goto _LL2AC;}_LL2AB: return
23;_LL2AC: {struct Cyc_Absyn_RgnsEff_struct*_tmp4EE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp4D7;if(_tmp4EE->tag != 25)goto _LL2AE;}_LL2AD: return 24;_LL2AE: {struct Cyc_Absyn_TagType_struct*
_tmp4EF=(struct Cyc_Absyn_TagType_struct*)_tmp4D7;if(_tmp4EF->tag != 20)goto _LL2B0;}
_LL2AF: return 26;_LL2B0: {struct Cyc_Absyn_DynRgnType_struct*_tmp4F0=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4D7;if(_tmp4F0->tag != 17)goto _LL2B2;}_LL2B1: return 27;_LL2B2: {struct Cyc_Absyn_ValueofType_struct*
_tmp4F1=(struct Cyc_Absyn_ValueofType_struct*)_tmp4D7;if(_tmp4F1->tag != 19)goto
_LL27F;}_LL2B3: return 28;_LL27F:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp4F2=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp4F2 != 0)return _tmp4F2;{struct _tuple0 _tmp10A4;struct _tuple0 _tmp4F4=(
_tmp10A4.f1=t2,((_tmp10A4.f2=t1,_tmp10A4)));void*_tmp4F5;void*_tmp4F7;void*
_tmp4F9;struct Cyc_Absyn_Tvar*_tmp4FB;void*_tmp4FC;struct Cyc_Absyn_Tvar*_tmp4FE;
void*_tmp4FF;struct Cyc_Absyn_AggrInfo _tmp501;union Cyc_Absyn_AggrInfoU _tmp502;
struct Cyc_List_List*_tmp503;void*_tmp504;struct Cyc_Absyn_AggrInfo _tmp506;union
Cyc_Absyn_AggrInfoU _tmp507;struct Cyc_List_List*_tmp508;void*_tmp509;struct
_tuple2*_tmp50B;void*_tmp50C;struct _tuple2*_tmp50E;void*_tmp50F;struct Cyc_List_List*
_tmp511;void*_tmp512;struct Cyc_List_List*_tmp514;void*_tmp515;struct Cyc_Absyn_DatatypeInfo
_tmp517;union Cyc_Absyn_DatatypeInfoU _tmp518;struct Cyc_Absyn_Datatypedecl**
_tmp519;struct Cyc_Absyn_Datatypedecl*_tmp51A;struct Cyc_List_List*_tmp51B;void*
_tmp51C;struct Cyc_Absyn_DatatypeInfo _tmp51E;union Cyc_Absyn_DatatypeInfoU _tmp51F;
struct Cyc_Absyn_Datatypedecl**_tmp520;struct Cyc_Absyn_Datatypedecl*_tmp521;
struct Cyc_List_List*_tmp522;void*_tmp523;struct Cyc_Absyn_DatatypeFieldInfo
_tmp525;union Cyc_Absyn_DatatypeFieldInfoU _tmp526;struct _tuple3 _tmp527;struct Cyc_Absyn_Datatypedecl*
_tmp528;struct Cyc_Absyn_Datatypefield*_tmp529;struct Cyc_List_List*_tmp52A;void*
_tmp52B;struct Cyc_Absyn_DatatypeFieldInfo _tmp52D;union Cyc_Absyn_DatatypeFieldInfoU
_tmp52E;struct _tuple3 _tmp52F;struct Cyc_Absyn_Datatypedecl*_tmp530;struct Cyc_Absyn_Datatypefield*
_tmp531;struct Cyc_List_List*_tmp532;void*_tmp533;struct Cyc_Absyn_PtrInfo _tmp535;
void*_tmp536;struct Cyc_Absyn_Tqual _tmp537;struct Cyc_Absyn_PtrAtts _tmp538;void*
_tmp539;union Cyc_Absyn_Constraint*_tmp53A;union Cyc_Absyn_Constraint*_tmp53B;
union Cyc_Absyn_Constraint*_tmp53C;void*_tmp53D;struct Cyc_Absyn_PtrInfo _tmp53F;
void*_tmp540;struct Cyc_Absyn_Tqual _tmp541;struct Cyc_Absyn_PtrAtts _tmp542;void*
_tmp543;union Cyc_Absyn_Constraint*_tmp544;union Cyc_Absyn_Constraint*_tmp545;
union Cyc_Absyn_Constraint*_tmp546;void*_tmp547;enum Cyc_Absyn_Sign _tmp549;enum 
Cyc_Absyn_Size_of _tmp54A;void*_tmp54B;enum Cyc_Absyn_Sign _tmp54D;enum Cyc_Absyn_Size_of
_tmp54E;void*_tmp54F;void*_tmp551;void*_tmp553;int _tmp555;void*_tmp556;int
_tmp558;void*_tmp559;struct Cyc_Absyn_ArrayInfo _tmp55B;void*_tmp55C;struct Cyc_Absyn_Tqual
_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;union Cyc_Absyn_Constraint*_tmp55F;void*
_tmp560;struct Cyc_Absyn_ArrayInfo _tmp562;void*_tmp563;struct Cyc_Absyn_Tqual
_tmp564;struct Cyc_Absyn_Exp*_tmp565;union Cyc_Absyn_Constraint*_tmp566;void*
_tmp567;struct Cyc_Absyn_FnInfo _tmp569;struct Cyc_List_List*_tmp56A;struct Cyc_Core_Opt*
_tmp56B;void*_tmp56C;struct Cyc_List_List*_tmp56D;int _tmp56E;struct Cyc_Absyn_VarargInfo*
_tmp56F;struct Cyc_List_List*_tmp570;struct Cyc_List_List*_tmp571;void*_tmp572;
struct Cyc_Absyn_FnInfo _tmp574;struct Cyc_List_List*_tmp575;struct Cyc_Core_Opt*
_tmp576;void*_tmp577;struct Cyc_List_List*_tmp578;int _tmp579;struct Cyc_Absyn_VarargInfo*
_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57C;void*_tmp57D;
struct Cyc_List_List*_tmp57F;void*_tmp580;struct Cyc_List_List*_tmp582;void*
_tmp583;enum Cyc_Absyn_AggrKind _tmp585;struct Cyc_List_List*_tmp586;void*_tmp587;
enum Cyc_Absyn_AggrKind _tmp589;struct Cyc_List_List*_tmp58A;void*_tmp58B;void*
_tmp58D;void*_tmp58E;void*_tmp590;void*_tmp591;void*_tmp593;void*_tmp594;void*
_tmp595;void*_tmp597;void*_tmp598;void*_tmp599;void*_tmp59B;void*_tmp59C;void*
_tmp59E;void*_tmp59F;struct Cyc_Absyn_Exp*_tmp5A1;void*_tmp5A2;struct Cyc_Absyn_Exp*
_tmp5A4;void*_tmp5A5;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;void*_tmp5AD;void*
_tmp5AF;_LL2B5: _tmp4F5=_tmp4F4.f1;{struct Cyc_Absyn_Evar_struct*_tmp4F6=(struct
Cyc_Absyn_Evar_struct*)_tmp4F5;if(_tmp4F6->tag != 1)goto _LL2B7;};_tmp4F7=_tmp4F4.f2;{
struct Cyc_Absyn_Evar_struct*_tmp4F8=(struct Cyc_Absyn_Evar_struct*)_tmp4F7;if(
_tmp4F8->tag != 1)goto _LL2B7;};_LL2B6: {const char*_tmp10A7;void*_tmp10A6;(
_tmp10A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10A7="typecmp: can only compare closed types",_tag_dyneither(_tmp10A7,
sizeof(char),39))),_tag_dyneither(_tmp10A6,sizeof(void*),0)));}_LL2B7: _tmp4F9=
_tmp4F4.f1;{struct Cyc_Absyn_VarType_struct*_tmp4FA=(struct Cyc_Absyn_VarType_struct*)
_tmp4F9;if(_tmp4FA->tag != 2)goto _LL2B9;else{_tmp4FB=_tmp4FA->f1;}};_tmp4FC=
_tmp4F4.f2;{struct Cyc_Absyn_VarType_struct*_tmp4FD=(struct Cyc_Absyn_VarType_struct*)
_tmp4FC;if(_tmp4FD->tag != 2)goto _LL2B9;else{_tmp4FE=_tmp4FD->f1;}};_LL2B8: return
Cyc_Core_intcmp(_tmp4FE->identity,_tmp4FB->identity);_LL2B9: _tmp4FF=_tmp4F4.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp500=(struct Cyc_Absyn_AggrType_struct*)
_tmp4FF;if(_tmp500->tag != 12)goto _LL2BB;else{_tmp501=_tmp500->f1;_tmp502=_tmp501.aggr_info;
_tmp503=_tmp501.targs;}};_tmp504=_tmp4F4.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp505=(struct Cyc_Absyn_AggrType_struct*)_tmp504;if(_tmp505->tag != 12)goto
_LL2BB;else{_tmp506=_tmp505->f1;_tmp507=_tmp506.aggr_info;_tmp508=_tmp506.targs;}};
_LL2BA: {struct _tuple2*_tmp5B4;struct _tuple12 _tmp5B3=Cyc_Absyn_aggr_kinded_name(
_tmp502);_tmp5B4=_tmp5B3.f2;{struct _tuple2*_tmp5B6;struct _tuple12 _tmp5B5=Cyc_Absyn_aggr_kinded_name(
_tmp507);_tmp5B6=_tmp5B5.f2;{int _tmp5B7=Cyc_Absyn_qvar_cmp(_tmp5B4,_tmp5B6);if(
_tmp5B7 != 0)return _tmp5B7;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp503,_tmp508);}};};}
_LL2BB: _tmp509=_tmp4F4.f1;{struct Cyc_Absyn_EnumType_struct*_tmp50A=(struct Cyc_Absyn_EnumType_struct*)
_tmp509;if(_tmp50A->tag != 14)goto _LL2BD;else{_tmp50B=_tmp50A->f1;}};_tmp50C=
_tmp4F4.f2;{struct Cyc_Absyn_EnumType_struct*_tmp50D=(struct Cyc_Absyn_EnumType_struct*)
_tmp50C;if(_tmp50D->tag != 14)goto _LL2BD;else{_tmp50E=_tmp50D->f1;}};_LL2BC:
return Cyc_Absyn_qvar_cmp(_tmp50B,_tmp50E);_LL2BD: _tmp50F=_tmp4F4.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp510=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp50F;if(_tmp510->tag != 15)goto
_LL2BF;else{_tmp511=_tmp510->f1;}};_tmp512=_tmp4F4.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp513=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp512;if(_tmp513->tag != 15)goto
_LL2BF;else{_tmp514=_tmp513->f1;}};_LL2BE: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp511,_tmp514);_LL2BF: _tmp515=_tmp4F4.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp516=(struct Cyc_Absyn_DatatypeType_struct*)_tmp515;if(_tmp516->tag != 3)goto
_LL2C1;else{_tmp517=_tmp516->f1;_tmp518=_tmp517.datatype_info;if((_tmp518.KnownDatatype).tag
!= 2)goto _LL2C1;_tmp519=(struct Cyc_Absyn_Datatypedecl**)(_tmp518.KnownDatatype).val;
_tmp51A=*_tmp519;_tmp51B=_tmp517.targs;}};_tmp51C=_tmp4F4.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp51D=(struct Cyc_Absyn_DatatypeType_struct*)_tmp51C;if(_tmp51D->tag != 3)goto
_LL2C1;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51E.datatype_info;if((_tmp51F.KnownDatatype).tag
!= 2)goto _LL2C1;_tmp520=(struct Cyc_Absyn_Datatypedecl**)(_tmp51F.KnownDatatype).val;
_tmp521=*_tmp520;_tmp522=_tmp51E.targs;}};_LL2C0: if(_tmp521 == _tmp51A)return 0;{
int _tmp5B8=Cyc_Absyn_qvar_cmp(_tmp521->name,_tmp51A->name);if(_tmp5B8 != 0)return
_tmp5B8;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp522,_tmp51B);};_LL2C1: _tmp523=
_tmp4F4.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp524=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp523;if(_tmp524->tag != 4)goto _LL2C3;else{_tmp525=_tmp524->f1;_tmp526=_tmp525.field_info;
if((_tmp526.KnownDatatypefield).tag != 2)goto _LL2C3;_tmp527=(struct _tuple3)(
_tmp526.KnownDatatypefield).val;_tmp528=_tmp527.f1;_tmp529=_tmp527.f2;_tmp52A=
_tmp525.targs;}};_tmp52B=_tmp4F4.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp52C=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp52B;if(_tmp52C->tag != 4)
goto _LL2C3;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52D.field_info;if((_tmp52E.KnownDatatypefield).tag
!= 2)goto _LL2C3;_tmp52F=(struct _tuple3)(_tmp52E.KnownDatatypefield).val;_tmp530=
_tmp52F.f1;_tmp531=_tmp52F.f2;_tmp532=_tmp52D.targs;}};_LL2C2: if(_tmp530 == 
_tmp528)return 0;{int _tmp5B9=Cyc_Absyn_qvar_cmp(_tmp528->name,_tmp530->name);if(
_tmp5B9 != 0)return _tmp5B9;{int _tmp5BA=Cyc_Absyn_qvar_cmp(_tmp529->name,_tmp531->name);
if(_tmp5BA != 0)return _tmp5BA;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp532,_tmp52A);};};
_LL2C3: _tmp533=_tmp4F4.f1;{struct Cyc_Absyn_PointerType_struct*_tmp534=(struct Cyc_Absyn_PointerType_struct*)
_tmp533;if(_tmp534->tag != 5)goto _LL2C5;else{_tmp535=_tmp534->f1;_tmp536=_tmp535.elt_typ;
_tmp537=_tmp535.elt_tq;_tmp538=_tmp535.ptr_atts;_tmp539=_tmp538.rgn;_tmp53A=
_tmp538.nullable;_tmp53B=_tmp538.bounds;_tmp53C=_tmp538.zero_term;}};_tmp53D=
_tmp4F4.f2;{struct Cyc_Absyn_PointerType_struct*_tmp53E=(struct Cyc_Absyn_PointerType_struct*)
_tmp53D;if(_tmp53E->tag != 5)goto _LL2C5;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53F.elt_typ;
_tmp541=_tmp53F.elt_tq;_tmp542=_tmp53F.ptr_atts;_tmp543=_tmp542.rgn;_tmp544=
_tmp542.nullable;_tmp545=_tmp542.bounds;_tmp546=_tmp542.zero_term;}};_LL2C4: {
int _tmp5BB=Cyc_Tcutil_typecmp(_tmp540,_tmp536);if(_tmp5BB != 0)return _tmp5BB;{int
_tmp5BC=Cyc_Tcutil_typecmp(_tmp543,_tmp539);if(_tmp5BC != 0)return _tmp5BC;{int
_tmp5BD=Cyc_Tcutil_tqual_cmp(_tmp541,_tmp537);if(_tmp5BD != 0)return _tmp5BD;{int
_tmp5BE=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp545,_tmp53B);if(_tmp5BE != 0)
return _tmp5BE;{int _tmp5BF=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp546,
_tmp53C);if(_tmp5BF != 0)return _tmp5BF;{void*_tmp5C0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp545);_LL2EA: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C0;if(_tmp5C1->tag != 0)goto
_LL2EC;}_LL2EB: return 0;_LL2EC:;_LL2ED: goto _LL2E9;_LL2E9:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp544,_tmp53A);};};};};}_LL2C5: _tmp547=_tmp4F4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp548=(struct Cyc_Absyn_IntType_struct*)_tmp547;if(_tmp548->tag != 6)goto _LL2C7;
else{_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;}};_tmp54B=_tmp4F4.f2;{struct Cyc_Absyn_IntType_struct*
_tmp54C=(struct Cyc_Absyn_IntType_struct*)_tmp54B;if(_tmp54C->tag != 6)goto _LL2C7;
else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;}};_LL2C6: if(_tmp54D != _tmp549)
return Cyc_Core_intcmp((int)((unsigned int)_tmp54D),(int)((unsigned int)_tmp549));
if(_tmp54E != _tmp54A)return Cyc_Core_intcmp((int)((unsigned int)_tmp54E),(int)((
unsigned int)_tmp54A));return 0;_LL2C7: _tmp54F=_tmp4F4.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp550=(struct Cyc_Absyn_FloatType_struct*)_tmp54F;if(_tmp550->tag != 7)goto
_LL2C9;};_tmp551=_tmp4F4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp552=(struct Cyc_Absyn_FloatType_struct*)
_tmp551;if(_tmp552->tag != 7)goto _LL2C9;};_LL2C8: return 0;_LL2C9: _tmp553=_tmp4F4.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp554=(struct Cyc_Absyn_DoubleType_struct*)
_tmp553;if(_tmp554->tag != 8)goto _LL2CB;else{_tmp555=_tmp554->f1;}};_tmp556=
_tmp4F4.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp557=(struct Cyc_Absyn_DoubleType_struct*)
_tmp556;if(_tmp557->tag != 8)goto _LL2CB;else{_tmp558=_tmp557->f1;}};_LL2CA: if(
_tmp555 == _tmp558)return 0;else{if(_tmp555)return - 1;else{return 1;}}_LL2CB: _tmp559=
_tmp4F4.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp55A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp559;if(_tmp55A->tag != 9)goto _LL2CD;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55B.elt_type;
_tmp55D=_tmp55B.tq;_tmp55E=_tmp55B.num_elts;_tmp55F=_tmp55B.zero_term;}};_tmp560=
_tmp4F4.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp561=(struct Cyc_Absyn_ArrayType_struct*)
_tmp560;if(_tmp561->tag != 9)goto _LL2CD;else{_tmp562=_tmp561->f1;_tmp563=_tmp562.elt_type;
_tmp564=_tmp562.tq;_tmp565=_tmp562.num_elts;_tmp566=_tmp562.zero_term;}};_LL2CC: {
int _tmp5C2=Cyc_Tcutil_tqual_cmp(_tmp564,_tmp55D);if(_tmp5C2 != 0)return _tmp5C2;{
int _tmp5C3=Cyc_Tcutil_typecmp(_tmp563,_tmp55C);if(_tmp5C3 != 0)return _tmp5C3;{int
_tmp5C4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp55F,_tmp566);if(_tmp5C4 != 0)
return _tmp5C4;if(_tmp55E == _tmp565)return 0;if(_tmp55E == 0  || _tmp565 == 0){const
char*_tmp10AA;void*_tmp10A9;(_tmp10A9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10AA="missing expression in array index",
_tag_dyneither(_tmp10AA,sizeof(char),34))),_tag_dyneither(_tmp10A9,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp55E,_tmp565);};};}_LL2CD: _tmp567=_tmp4F4.f1;{struct Cyc_Absyn_FnType_struct*
_tmp568=(struct Cyc_Absyn_FnType_struct*)_tmp567;if(_tmp568->tag != 10)goto _LL2CF;
else{_tmp569=_tmp568->f1;_tmp56A=_tmp569.tvars;_tmp56B=_tmp569.effect;_tmp56C=
_tmp569.ret_typ;_tmp56D=_tmp569.args;_tmp56E=_tmp569.c_varargs;_tmp56F=_tmp569.cyc_varargs;
_tmp570=_tmp569.rgn_po;_tmp571=_tmp569.attributes;}};_tmp572=_tmp4F4.f2;{struct
Cyc_Absyn_FnType_struct*_tmp573=(struct Cyc_Absyn_FnType_struct*)_tmp572;if(
_tmp573->tag != 10)goto _LL2CF;else{_tmp574=_tmp573->f1;_tmp575=_tmp574.tvars;
_tmp576=_tmp574.effect;_tmp577=_tmp574.ret_typ;_tmp578=_tmp574.args;_tmp579=
_tmp574.c_varargs;_tmp57A=_tmp574.cyc_varargs;_tmp57B=_tmp574.rgn_po;_tmp57C=
_tmp574.attributes;}};_LL2CE: {const char*_tmp10AD;void*_tmp10AC;(_tmp10AC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10AD="typecmp: function types not handled",_tag_dyneither(_tmp10AD,sizeof(
char),36))),_tag_dyneither(_tmp10AC,sizeof(void*),0)));}_LL2CF: _tmp57D=_tmp4F4.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp57E=(struct Cyc_Absyn_TupleType_struct*)
_tmp57D;if(_tmp57E->tag != 11)goto _LL2D1;else{_tmp57F=_tmp57E->f1;}};_tmp580=
_tmp4F4.f2;{struct Cyc_Absyn_TupleType_struct*_tmp581=(struct Cyc_Absyn_TupleType_struct*)
_tmp580;if(_tmp581->tag != 11)goto _LL2D1;else{_tmp582=_tmp581->f1;}};_LL2D0:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp582,
_tmp57F);_LL2D1: _tmp583=_tmp4F4.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp584=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp583;if(_tmp584->tag != 13)goto _LL2D3;
else{_tmp585=_tmp584->f1;_tmp586=_tmp584->f2;}};_tmp587=_tmp4F4.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp588=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp587;if(_tmp588->tag != 13)goto
_LL2D3;else{_tmp589=_tmp588->f1;_tmp58A=_tmp588->f2;}};_LL2D2: if(_tmp589 != 
_tmp585){if(_tmp589 == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp58A,
_tmp586);_LL2D3: _tmp58B=_tmp4F4.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp58C=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp58B;if(_tmp58C->tag != 16)goto _LL2D5;
else{_tmp58D=(void*)_tmp58C->f1;}};_tmp58E=_tmp4F4.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp58F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp58E;if(_tmp58F->tag != 16)goto
_LL2D5;else{_tmp590=(void*)_tmp58F->f1;}};_LL2D4: return Cyc_Tcutil_typecmp(
_tmp58D,_tmp590);_LL2D5: _tmp591=_tmp4F4.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp592=(struct Cyc_Absyn_DynRgnType_struct*)_tmp591;if(_tmp592->tag != 17)goto
_LL2D7;else{_tmp593=(void*)_tmp592->f1;_tmp594=(void*)_tmp592->f2;}};_tmp595=
_tmp4F4.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp596=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp595;if(_tmp596->tag != 17)goto _LL2D7;else{_tmp597=(void*)_tmp596->f1;_tmp598=(
void*)_tmp596->f2;}};_LL2D6: {int _tmp5C9=Cyc_Tcutil_typecmp(_tmp593,_tmp597);if(
_tmp5C9 != 0)return _tmp5C9;else{return Cyc_Tcutil_typecmp(_tmp594,_tmp598);}}
_LL2D7: _tmp599=_tmp4F4.f1;{struct Cyc_Absyn_TagType_struct*_tmp59A=(struct Cyc_Absyn_TagType_struct*)
_tmp599;if(_tmp59A->tag != 20)goto _LL2D9;else{_tmp59B=(void*)_tmp59A->f1;}};
_tmp59C=_tmp4F4.f2;{struct Cyc_Absyn_TagType_struct*_tmp59D=(struct Cyc_Absyn_TagType_struct*)
_tmp59C;if(_tmp59D->tag != 20)goto _LL2D9;else{_tmp59E=(void*)_tmp59D->f1;}};
_LL2D8: return Cyc_Tcutil_typecmp(_tmp59B,_tmp59E);_LL2D9: _tmp59F=_tmp4F4.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp5A0=(struct Cyc_Absyn_ValueofType_struct*)
_tmp59F;if(_tmp5A0->tag != 19)goto _LL2DB;else{_tmp5A1=_tmp5A0->f1;}};_tmp5A2=
_tmp4F4.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp5A3=(struct Cyc_Absyn_ValueofType_struct*)
_tmp5A2;if(_tmp5A3->tag != 19)goto _LL2DB;else{_tmp5A4=_tmp5A3->f1;}};_LL2DA:
return Cyc_Evexp_const_exp_cmp(_tmp5A1,_tmp5A4);_LL2DB: _tmp5A5=_tmp4F4.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp5A6=(struct Cyc_Absyn_JoinEff_struct*)_tmp5A5;if(
_tmp5A6->tag != 24)goto _LL2DD;};_LL2DC: goto _LL2DE;_LL2DD: _tmp5A7=_tmp4F4.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp5A8=(struct Cyc_Absyn_JoinEff_struct*)_tmp5A7;
if(_tmp5A8->tag != 24)goto _LL2DF;};_LL2DE: goto _LL2E0;_LL2DF: _tmp5A9=_tmp4F4.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp5AA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5A9;if(_tmp5AA->tag != 23)goto _LL2E1;};_LL2E0: goto _LL2E2;_LL2E1: _tmp5AB=
_tmp4F4.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp5AC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5AB;if(_tmp5AC->tag != 25)goto _LL2E3;};_LL2E2: goto _LL2E4;_LL2E3: _tmp5AD=
_tmp4F4.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp5AE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5AD;if(_tmp5AE->tag != 25)goto _LL2E5;};_LL2E4: goto _LL2E6;_LL2E5: _tmp5AF=
_tmp4F4.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp5B0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5AF;if(_tmp5B0->tag != 23)goto _LL2E7;};_LL2E6: {const char*_tmp10B0;void*
_tmp10AF;(_tmp10AF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10B0="typecmp: effects not handled",_tag_dyneither(
_tmp10B0,sizeof(char),29))),_tag_dyneither(_tmp10AF,sizeof(void*),0)));}_LL2E7:;
_LL2E8: {const char*_tmp10B3;void*_tmp10B2;(_tmp10B2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10B3="Unmatched case in typecmp",
_tag_dyneither(_tmp10B3,sizeof(char),26))),_tag_dyneither(_tmp10B2,sizeof(void*),
0)));}_LL2B4:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp5CE=Cyc_Tcutil_compress(t);_LL2EF: {struct Cyc_Absyn_IntType_struct*
_tmp5CF=(struct Cyc_Absyn_IntType_struct*)_tmp5CE;if(_tmp5CF->tag != 6)goto _LL2F1;}
_LL2F0: goto _LL2F2;_LL2F1: {struct Cyc_Absyn_FloatType_struct*_tmp5D0=(struct Cyc_Absyn_FloatType_struct*)
_tmp5CE;if(_tmp5D0->tag != 7)goto _LL2F3;}_LL2F2: goto _LL2F4;_LL2F3: {struct Cyc_Absyn_DoubleType_struct*
_tmp5D1=(struct Cyc_Absyn_DoubleType_struct*)_tmp5CE;if(_tmp5D1->tag != 8)goto
_LL2F5;}_LL2F4: goto _LL2F6;_LL2F5: {struct Cyc_Absyn_EnumType_struct*_tmp5D2=(
struct Cyc_Absyn_EnumType_struct*)_tmp5CE;if(_tmp5D2->tag != 14)goto _LL2F7;}_LL2F6:
goto _LL2F8;_LL2F7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5D3=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5CE;if(_tmp5D3->tag != 15)goto _LL2F9;}_LL2F8: return 1;_LL2F9:;_LL2FA: return 0;
_LL2EE:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp10B4;struct _tuple0 _tmp5D5=(_tmp10B4.f1=t1,((_tmp10B4.f2=t2,_tmp10B4)));
void*_tmp5D6;int _tmp5D8;void*_tmp5D9;int _tmp5DB;void*_tmp5DC;void*_tmp5DE;void*
_tmp5E0;void*_tmp5E2;void*_tmp5E4;void*_tmp5E6;void*_tmp5E8;void*_tmp5EA;void*
_tmp5EC;void*_tmp5EE;void*_tmp5F0;enum Cyc_Absyn_Size_of _tmp5F2;void*_tmp5F3;
enum Cyc_Absyn_Size_of _tmp5F5;void*_tmp5F6;enum Cyc_Absyn_Size_of _tmp5F8;void*
_tmp5F9;enum Cyc_Absyn_Size_of _tmp5FB;void*_tmp5FC;enum Cyc_Absyn_Size_of _tmp5FE;
void*_tmp5FF;enum Cyc_Absyn_Size_of _tmp601;void*_tmp602;enum Cyc_Absyn_Size_of
_tmp604;void*_tmp605;enum Cyc_Absyn_Size_of _tmp607;void*_tmp608;void*_tmp60A;
enum Cyc_Absyn_Size_of _tmp60C;void*_tmp60D;void*_tmp60F;enum Cyc_Absyn_Size_of
_tmp611;void*_tmp612;enum Cyc_Absyn_Size_of _tmp614;void*_tmp615;enum Cyc_Absyn_Size_of
_tmp617;void*_tmp618;enum Cyc_Absyn_Size_of _tmp61A;void*_tmp61B;void*_tmp61D;
enum Cyc_Absyn_Size_of _tmp61F;void*_tmp620;enum Cyc_Absyn_Size_of _tmp622;void*
_tmp623;enum Cyc_Absyn_Size_of _tmp625;void*_tmp626;enum Cyc_Absyn_Size_of _tmp628;
void*_tmp629;enum Cyc_Absyn_Size_of _tmp62B;void*_tmp62C;enum Cyc_Absyn_Size_of
_tmp62E;void*_tmp62F;enum Cyc_Absyn_Size_of _tmp631;void*_tmp632;void*_tmp634;
enum Cyc_Absyn_Size_of _tmp636;_LL2FC: _tmp5D6=_tmp5D5.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5D7=(struct Cyc_Absyn_DoubleType_struct*)_tmp5D6;if(_tmp5D7->tag != 8)goto
_LL2FE;else{_tmp5D8=_tmp5D7->f1;}};_tmp5D9=_tmp5D5.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp5DA=(struct Cyc_Absyn_DoubleType_struct*)_tmp5D9;if(_tmp5DA->tag != 8)goto
_LL2FE;else{_tmp5DB=_tmp5DA->f1;}};_LL2FD: return !_tmp5DB  && _tmp5D8;_LL2FE:
_tmp5DC=_tmp5D5.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5DD=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5DC;if(_tmp5DD->tag != 8)goto _LL300;};_tmp5DE=_tmp5D5.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp5DF=(struct Cyc_Absyn_FloatType_struct*)_tmp5DE;if(_tmp5DF->tag != 7)goto
_LL300;};_LL2FF: goto _LL301;_LL300: _tmp5E0=_tmp5D5.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5E1=(struct Cyc_Absyn_DoubleType_struct*)_tmp5E0;if(_tmp5E1->tag != 8)goto
_LL302;};_tmp5E2=_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp5E3=(struct Cyc_Absyn_IntType_struct*)
_tmp5E2;if(_tmp5E3->tag != 6)goto _LL302;};_LL301: goto _LL303;_LL302: _tmp5E4=
_tmp5D5.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5E5=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5E4;if(_tmp5E5->tag != 8)goto _LL304;};_tmp5E6=_tmp5D5.f2;{struct Cyc_Absyn_TagType_struct*
_tmp5E7=(struct Cyc_Absyn_TagType_struct*)_tmp5E6;if(_tmp5E7->tag != 20)goto _LL304;};
_LL303: goto _LL305;_LL304: _tmp5E8=_tmp5D5.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp5E9=(struct Cyc_Absyn_FloatType_struct*)_tmp5E8;if(_tmp5E9->tag != 7)goto
_LL306;};_tmp5EA=_tmp5D5.f2;{struct Cyc_Absyn_TagType_struct*_tmp5EB=(struct Cyc_Absyn_TagType_struct*)
_tmp5EA;if(_tmp5EB->tag != 20)goto _LL306;};_LL305: goto _LL307;_LL306: _tmp5EC=
_tmp5D5.f1;{struct Cyc_Absyn_FloatType_struct*_tmp5ED=(struct Cyc_Absyn_FloatType_struct*)
_tmp5EC;if(_tmp5ED->tag != 7)goto _LL308;};_tmp5EE=_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5EF=(struct Cyc_Absyn_IntType_struct*)_tmp5EE;if(_tmp5EF->tag != 6)goto _LL308;};
_LL307: return 1;_LL308: _tmp5F0=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp5F1=(
struct Cyc_Absyn_IntType_struct*)_tmp5F0;if(_tmp5F1->tag != 6)goto _LL30A;else{
_tmp5F2=_tmp5F1->f2;if(_tmp5F2 != Cyc_Absyn_LongLong_sz)goto _LL30A;}};_tmp5F3=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp5F4=(struct Cyc_Absyn_IntType_struct*)
_tmp5F3;if(_tmp5F4->tag != 6)goto _LL30A;else{_tmp5F5=_tmp5F4->f2;if(_tmp5F5 != Cyc_Absyn_LongLong_sz)
goto _LL30A;}};_LL309: return 0;_LL30A: _tmp5F6=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5F7=(struct Cyc_Absyn_IntType_struct*)_tmp5F6;if(_tmp5F7->tag != 6)goto _LL30C;
else{_tmp5F8=_tmp5F7->f2;if(_tmp5F8 != Cyc_Absyn_LongLong_sz)goto _LL30C;}};_LL30B:
return 1;_LL30C: _tmp5F9=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp5FA=(
struct Cyc_Absyn_IntType_struct*)_tmp5F9;if(_tmp5FA->tag != 6)goto _LL30E;else{
_tmp5FB=_tmp5FA->f2;if(_tmp5FB != Cyc_Absyn_Long_sz)goto _LL30E;}};_tmp5FC=_tmp5D5.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5FD=(struct Cyc_Absyn_IntType_struct*)_tmp5FC;
if(_tmp5FD->tag != 6)goto _LL30E;else{_tmp5FE=_tmp5FD->f2;if(_tmp5FE != Cyc_Absyn_Int_sz)
goto _LL30E;}};_LL30D: goto _LL30F;_LL30E: _tmp5FF=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp600=(struct Cyc_Absyn_IntType_struct*)_tmp5FF;if(_tmp600->tag != 6)goto _LL310;
else{_tmp601=_tmp600->f2;if(_tmp601 != Cyc_Absyn_Int_sz)goto _LL310;}};_tmp602=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp603=(struct Cyc_Absyn_IntType_struct*)
_tmp602;if(_tmp603->tag != 6)goto _LL310;else{_tmp604=_tmp603->f2;if(_tmp604 != Cyc_Absyn_Long_sz)
goto _LL310;}};_LL30F: return 0;_LL310: _tmp605=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp606=(struct Cyc_Absyn_IntType_struct*)_tmp605;if(_tmp606->tag != 6)goto _LL312;
else{_tmp607=_tmp606->f2;if(_tmp607 != Cyc_Absyn_Long_sz)goto _LL312;}};_tmp608=
_tmp5D5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp609=(struct Cyc_Absyn_FloatType_struct*)
_tmp608;if(_tmp609->tag != 7)goto _LL312;};_LL311: goto _LL313;_LL312: _tmp60A=
_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp60B=(struct Cyc_Absyn_IntType_struct*)
_tmp60A;if(_tmp60B->tag != 6)goto _LL314;else{_tmp60C=_tmp60B->f2;if(_tmp60C != Cyc_Absyn_Int_sz)
goto _LL314;}};_tmp60D=_tmp5D5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp60E=(
struct Cyc_Absyn_FloatType_struct*)_tmp60D;if(_tmp60E->tag != 7)goto _LL314;};
_LL313: goto _LL315;_LL314: _tmp60F=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp610=(struct Cyc_Absyn_IntType_struct*)_tmp60F;if(_tmp610->tag != 6)goto _LL316;
else{_tmp611=_tmp610->f2;if(_tmp611 != Cyc_Absyn_Long_sz)goto _LL316;}};_tmp612=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp613=(struct Cyc_Absyn_IntType_struct*)
_tmp612;if(_tmp613->tag != 6)goto _LL316;else{_tmp614=_tmp613->f2;if(_tmp614 != Cyc_Absyn_Short_sz)
goto _LL316;}};_LL315: goto _LL317;_LL316: _tmp615=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp616=(struct Cyc_Absyn_IntType_struct*)_tmp615;if(_tmp616->tag != 6)goto _LL318;
else{_tmp617=_tmp616->f2;if(_tmp617 != Cyc_Absyn_Int_sz)goto _LL318;}};_tmp618=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp619=(struct Cyc_Absyn_IntType_struct*)
_tmp618;if(_tmp619->tag != 6)goto _LL318;else{_tmp61A=_tmp619->f2;if(_tmp61A != Cyc_Absyn_Short_sz)
goto _LL318;}};_LL317: goto _LL319;_LL318: _tmp61B=_tmp5D5.f1;{struct Cyc_Absyn_TagType_struct*
_tmp61C=(struct Cyc_Absyn_TagType_struct*)_tmp61B;if(_tmp61C->tag != 20)goto _LL31A;};
_tmp61D=_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp61E=(struct Cyc_Absyn_IntType_struct*)
_tmp61D;if(_tmp61E->tag != 6)goto _LL31A;else{_tmp61F=_tmp61E->f2;if(_tmp61F != Cyc_Absyn_Short_sz)
goto _LL31A;}};_LL319: goto _LL31B;_LL31A: _tmp620=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp621=(struct Cyc_Absyn_IntType_struct*)_tmp620;if(_tmp621->tag != 6)goto _LL31C;
else{_tmp622=_tmp621->f2;if(_tmp622 != Cyc_Absyn_Long_sz)goto _LL31C;}};_tmp623=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp624=(struct Cyc_Absyn_IntType_struct*)
_tmp623;if(_tmp624->tag != 6)goto _LL31C;else{_tmp625=_tmp624->f2;if(_tmp625 != Cyc_Absyn_Char_sz)
goto _LL31C;}};_LL31B: goto _LL31D;_LL31C: _tmp626=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp627=(struct Cyc_Absyn_IntType_struct*)_tmp626;if(_tmp627->tag != 6)goto _LL31E;
else{_tmp628=_tmp627->f2;if(_tmp628 != Cyc_Absyn_Int_sz)goto _LL31E;}};_tmp629=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp62A=(struct Cyc_Absyn_IntType_struct*)
_tmp629;if(_tmp62A->tag != 6)goto _LL31E;else{_tmp62B=_tmp62A->f2;if(_tmp62B != Cyc_Absyn_Char_sz)
goto _LL31E;}};_LL31D: goto _LL31F;_LL31E: _tmp62C=_tmp5D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp62D=(struct Cyc_Absyn_IntType_struct*)_tmp62C;if(_tmp62D->tag != 6)goto _LL320;
else{_tmp62E=_tmp62D->f2;if(_tmp62E != Cyc_Absyn_Short_sz)goto _LL320;}};_tmp62F=
_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp630=(struct Cyc_Absyn_IntType_struct*)
_tmp62F;if(_tmp630->tag != 6)goto _LL320;else{_tmp631=_tmp630->f2;if(_tmp631 != Cyc_Absyn_Char_sz)
goto _LL320;}};_LL31F: goto _LL321;_LL320: _tmp632=_tmp5D5.f1;{struct Cyc_Absyn_TagType_struct*
_tmp633=(struct Cyc_Absyn_TagType_struct*)_tmp632;if(_tmp633->tag != 20)goto _LL322;};
_tmp634=_tmp5D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp635=(struct Cyc_Absyn_IntType_struct*)
_tmp634;if(_tmp635->tag != 6)goto _LL322;else{_tmp636=_tmp635->f2;if(_tmp636 != Cyc_Absyn_Char_sz)
goto _LL322;}};_LL321: return 1;_LL322:;_LL323: return 0;_LL2FB:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp637=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp10B5;max_arith_type=((_tmp10B5=
_region_malloc(_tmp637,sizeof(*_tmp10B5)),((_tmp10B5->v=t1,_tmp10B5))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp10BA;void*_tmp10B9[2];struct
Cyc_String_pa_struct _tmp10B8;struct Cyc_String_pa_struct _tmp10B7;(_tmp10B7.tag=0,((
_tmp10B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp10B8.tag=0,((_tmp10B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp10B9[0]=& _tmp10B8,((_tmp10B9[1]=& _tmp10B7,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp10BA="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp10BA,sizeof(char),41))),_tag_dyneither(_tmp10B9,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp63D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL325: {struct Cyc_Absyn_PointerType_struct*
_tmp63E=(struct Cyc_Absyn_PointerType_struct*)_tmp63D;if(_tmp63E->tag != 5)goto
_LL327;}_LL326: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL324;_LL327:;_LL328: return 0;_LL324:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp63F=Cyc_Tcutil_compress(
t);_LL32A: {struct Cyc_Absyn_IntType_struct*_tmp640=(struct Cyc_Absyn_IntType_struct*)
_tmp63F;if(_tmp640->tag != 6)goto _LL32C;}_LL32B: goto _LL32D;_LL32C: {struct Cyc_Absyn_TagType_struct*
_tmp641=(struct Cyc_Absyn_TagType_struct*)_tmp63F;if(_tmp641->tag != 20)goto _LL32E;}
_LL32D: goto _LL32F;_LL32E: {struct Cyc_Absyn_EnumType_struct*_tmp642=(struct Cyc_Absyn_EnumType_struct*)
_tmp63F;if(_tmp642->tag != 14)goto _LL330;}_LL32F: goto _LL331;_LL330: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp643=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp63F;if(_tmp643->tag != 15)goto
_LL332;}_LL331: return 1;_LL332:;_LL333: return 0;_LL329:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp10BD;void*_tmp10BC;(_tmp10BC=0,Cyc_Tcutil_warn(e->loc,((_tmp10BD="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp10BD,sizeof(char),44))),_tag_dyneither(_tmp10BC,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp10C0;void*_tmp10BF;(
_tmp10BF=0,Cyc_Tcutil_warn(e->loc,((_tmp10C0="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp10C0,sizeof(char),44))),_tag_dyneither(_tmp10BF,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp10C1;struct _tuple0 _tmp649=(
_tmp10C1.f1=t1,((_tmp10C1.f2=t2,_tmp10C1)));void*_tmp64A;struct Cyc_Absyn_PtrInfo
_tmp64C;void*_tmp64D;struct Cyc_Absyn_PtrInfo _tmp64F;void*_tmp650;struct Cyc_Absyn_ArrayInfo
_tmp652;void*_tmp653;struct Cyc_Absyn_Tqual _tmp654;struct Cyc_Absyn_Exp*_tmp655;
union Cyc_Absyn_Constraint*_tmp656;void*_tmp657;struct Cyc_Absyn_ArrayInfo _tmp659;
void*_tmp65A;struct Cyc_Absyn_Tqual _tmp65B;struct Cyc_Absyn_Exp*_tmp65C;union Cyc_Absyn_Constraint*
_tmp65D;void*_tmp65E;void*_tmp660;_LL335: _tmp64A=_tmp649.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64B=(struct Cyc_Absyn_PointerType_struct*)_tmp64A;if(_tmp64B->tag != 5)goto
_LL337;else{_tmp64C=_tmp64B->f1;}};_tmp64D=_tmp649.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp64E=(struct Cyc_Absyn_PointerType_struct*)_tmp64D;if(_tmp64E->tag != 5)goto
_LL337;else{_tmp64F=_tmp64E->f1;}};_LL336: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp64C.ptr_atts).nullable,(_tmp64F.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64C.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp64C.ptr_atts).bounds,(
_tmp64F.ptr_atts).bounds)){struct _tuple0 _tmp10C2;struct _tuple0 _tmp663=(_tmp10C2.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp64C.ptr_atts).bounds),((_tmp10C2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp64F.ptr_atts).bounds),
_tmp10C2)));void*_tmp664;void*_tmp666;void*_tmp668;void*_tmp66A;void*_tmp66C;
struct Cyc_Absyn_Exp*_tmp66E;void*_tmp66F;struct Cyc_Absyn_Exp*_tmp671;void*
_tmp672;void*_tmp674;struct Cyc_Absyn_Exp*_tmp676;_LL33E: _tmp664=_tmp663.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp665=(struct Cyc_Absyn_Upper_b_struct*)_tmp664;
if(_tmp665->tag != 1)goto _LL340;};_tmp666=_tmp663.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp667=(struct Cyc_Absyn_DynEither_b_struct*)_tmp666;if(_tmp667->tag != 0)goto
_LL340;};_LL33F: goto _LL341;_LL340: _tmp668=_tmp663.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp669=(struct Cyc_Absyn_DynEither_b_struct*)_tmp668;if(_tmp669->tag != 0)goto
_LL342;};_tmp66A=_tmp663.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp66B=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp66A;if(_tmp66B->tag != 0)goto _LL342;};_LL341:
okay=1;goto _LL33D;_LL342: _tmp66C=_tmp663.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp66D=(struct Cyc_Absyn_Upper_b_struct*)_tmp66C;if(_tmp66D->tag != 1)goto _LL344;
else{_tmp66E=_tmp66D->f1;}};_tmp66F=_tmp663.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp670=(struct Cyc_Absyn_Upper_b_struct*)_tmp66F;if(_tmp670->tag != 1)goto _LL344;
else{_tmp671=_tmp670->f1;}};_LL343: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp671,
_tmp66E);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp64F.ptr_atts).zero_term)){const char*_tmp10C5;void*_tmp10C4;(_tmp10C4=0,
Cyc_Tcutil_warn(loc,((_tmp10C5="implicit cast to shorter array",_tag_dyneither(
_tmp10C5,sizeof(char),31))),_tag_dyneither(_tmp10C4,sizeof(void*),0)));}goto
_LL33D;_LL344: _tmp672=_tmp663.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp673=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp672;if(_tmp673->tag != 0)goto _LL33D;};
_tmp674=_tmp663.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp675=(struct Cyc_Absyn_Upper_b_struct*)
_tmp674;if(_tmp675->tag != 1)goto _LL33D;else{_tmp676=_tmp675->f1;}};_LL345: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64C.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp64F.ptr_atts).bounds))goto _LL33D;okay=0;goto
_LL33D;_LL33D:;}okay=okay  && (!(_tmp64C.elt_tq).real_const  || (_tmp64F.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp64C.ptr_atts).rgn,(_tmp64F.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp64C.ptr_atts).rgn,(_tmp64F.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp64C.ptr_atts).zero_term,(
_tmp64F.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp64C.ptr_atts).zero_term) && (_tmp64F.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp64C.elt_typ,_tmp64F.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp64C.elt_typ,_tmp64F.elt_typ));return okay;}_LL337: _tmp650=_tmp649.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp651=(struct Cyc_Absyn_ArrayType_struct*)_tmp650;
if(_tmp651->tag != 9)goto _LL339;else{_tmp652=_tmp651->f1;_tmp653=_tmp652.elt_type;
_tmp654=_tmp652.tq;_tmp655=_tmp652.num_elts;_tmp656=_tmp652.zero_term;}};_tmp657=
_tmp649.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp658=(struct Cyc_Absyn_ArrayType_struct*)
_tmp657;if(_tmp658->tag != 9)goto _LL339;else{_tmp659=_tmp658->f1;_tmp65A=_tmp659.elt_type;
_tmp65B=_tmp659.tq;_tmp65C=_tmp659.num_elts;_tmp65D=_tmp659.zero_term;}};_LL338: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp656,_tmp65D) && ((_tmp655 != 0
 && _tmp65C != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp655,(
struct Cyc_Absyn_Exp*)_tmp65C));return(okay  && Cyc_Tcutil_unify(_tmp653,_tmp65A))
 && (!_tmp654.real_const  || _tmp65B.real_const);}_LL339: _tmp65E=_tmp649.f1;{
struct Cyc_Absyn_TagType_struct*_tmp65F=(struct Cyc_Absyn_TagType_struct*)_tmp65E;
if(_tmp65F->tag != 20)goto _LL33B;};_tmp660=_tmp649.f2;{struct Cyc_Absyn_IntType_struct*
_tmp661=(struct Cyc_Absyn_IntType_struct*)_tmp660;if(_tmp661->tag != 6)goto _LL33B;};
_LL33A: return 0;_LL33B:;_LL33C: return Cyc_Tcutil_unify(t1,t2);_LL334:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp679=Cyc_Tcutil_compress(t);
_LL347: {struct Cyc_Absyn_PointerType_struct*_tmp67A=(struct Cyc_Absyn_PointerType_struct*)
_tmp679;if(_tmp67A->tag != 5)goto _LL349;}_LL348: return 1;_LL349:;_LL34A: return 0;
_LL346:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp67B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp67D;void*
_tmp67E;_LL34C: {struct Cyc_Absyn_PointerType_struct*_tmp67C=(struct Cyc_Absyn_PointerType_struct*)
_tmp67B;if(_tmp67C->tag != 5)goto _LL34E;else{_tmp67D=_tmp67C->f1;_tmp67E=_tmp67D.elt_typ;}}
_LL34D: return _tmp67E;_LL34E:;_LL34F: {const char*_tmp10C8;void*_tmp10C7;(_tmp10C7=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10C8="pointer_elt_type",_tag_dyneither(_tmp10C8,sizeof(char),17))),
_tag_dyneither(_tmp10C7,sizeof(void*),0)));}_LL34B:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp681=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp683;struct Cyc_Absyn_PtrAtts _tmp684;struct Cyc_Absyn_PtrAtts*
_tmp685;_LL351: {struct Cyc_Absyn_PointerType_struct*_tmp682=(struct Cyc_Absyn_PointerType_struct*)
_tmp681;if(_tmp682->tag != 5)goto _LL353;else{_tmp683=_tmp682->f1;_tmp684=_tmp683.ptr_atts;
_tmp685=(struct Cyc_Absyn_PtrAtts*)&(_tmp682->f1).ptr_atts;}}_LL352: return _tmp685->rgn;
_LL353:;_LL354: {const char*_tmp10CB;void*_tmp10CA;(_tmp10CA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CB="pointer_elt_type",
_tag_dyneither(_tmp10CB,sizeof(char),17))),_tag_dyneither(_tmp10CA,sizeof(void*),
0)));}_LL350:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp688=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp68A;struct Cyc_Absyn_PtrAtts _tmp68B;void*_tmp68C;_LL356: {struct Cyc_Absyn_PointerType_struct*
_tmp689=(struct Cyc_Absyn_PointerType_struct*)_tmp688;if(_tmp689->tag != 5)goto
_LL358;else{_tmp68A=_tmp689->f1;_tmp68B=_tmp68A.ptr_atts;_tmp68C=_tmp68B.rgn;}}
_LL357:*rgn=_tmp68C;return 1;_LL358:;_LL359: return 0;_LL355:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp68D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp68F;struct Cyc_Absyn_PtrAtts _tmp690;union Cyc_Absyn_Constraint*_tmp691;_LL35B: {
struct Cyc_Absyn_PointerType_struct*_tmp68E=(struct Cyc_Absyn_PointerType_struct*)
_tmp68D;if(_tmp68E->tag != 5)goto _LL35D;else{_tmp68F=_tmp68E->f1;_tmp690=_tmp68F.ptr_atts;
_tmp691=_tmp690.bounds;}}_LL35C:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp691)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL35D:;_LL35E: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL35A:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp692=e->r;union Cyc_Absyn_Cnst _tmp694;struct
_tuple7 _tmp695;int _tmp696;union Cyc_Absyn_Cnst _tmp698;struct _tuple5 _tmp699;char
_tmp69A;union Cyc_Absyn_Cnst _tmp69C;struct _tuple6 _tmp69D;short _tmp69E;union Cyc_Absyn_Cnst
_tmp6A0;struct _tuple8 _tmp6A1;long long _tmp6A2;void*_tmp6A4;struct Cyc_Absyn_Exp*
_tmp6A5;_LL360: {struct Cyc_Absyn_Const_e_struct*_tmp693=(struct Cyc_Absyn_Const_e_struct*)
_tmp692;if(_tmp693->tag != 0)goto _LL362;else{_tmp694=_tmp693->f1;if((_tmp694.Int_c).tag
!= 4)goto _LL362;_tmp695=(struct _tuple7)(_tmp694.Int_c).val;_tmp696=_tmp695.f2;
if(_tmp696 != 0)goto _LL362;}}_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_Const_e_struct*
_tmp697=(struct Cyc_Absyn_Const_e_struct*)_tmp692;if(_tmp697->tag != 0)goto _LL364;
else{_tmp698=_tmp697->f1;if((_tmp698.Char_c).tag != 2)goto _LL364;_tmp699=(struct
_tuple5)(_tmp698.Char_c).val;_tmp69A=_tmp699.f2;if(_tmp69A != 0)goto _LL364;}}
_LL363: goto _LL365;_LL364: {struct Cyc_Absyn_Const_e_struct*_tmp69B=(struct Cyc_Absyn_Const_e_struct*)
_tmp692;if(_tmp69B->tag != 0)goto _LL366;else{_tmp69C=_tmp69B->f1;if((_tmp69C.Short_c).tag
!= 3)goto _LL366;_tmp69D=(struct _tuple6)(_tmp69C.Short_c).val;_tmp69E=_tmp69D.f2;
if(_tmp69E != 0)goto _LL366;}}_LL365: goto _LL367;_LL366: {struct Cyc_Absyn_Const_e_struct*
_tmp69F=(struct Cyc_Absyn_Const_e_struct*)_tmp692;if(_tmp69F->tag != 0)goto _LL368;
else{_tmp6A0=_tmp69F->f1;if((_tmp6A0.LongLong_c).tag != 5)goto _LL368;_tmp6A1=(
struct _tuple8)(_tmp6A0.LongLong_c).val;_tmp6A2=_tmp6A1.f2;if(_tmp6A2 != 0)goto
_LL368;}}_LL367: return 1;_LL368: {struct Cyc_Absyn_Cast_e_struct*_tmp6A3=(struct
Cyc_Absyn_Cast_e_struct*)_tmp692;if(_tmp6A3->tag != 15)goto _LL36A;else{_tmp6A4=(
void*)_tmp6A3->f1;_tmp6A5=_tmp6A3->f2;}}_LL369: return Cyc_Tcutil_is_zero(_tmp6A5)
 && Cyc_Tcutil_admits_zero(_tmp6A4);_LL36A:;_LL36B: return 0;_LL35F:;}struct Cyc_Absyn_Kind
Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ak={
Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,
Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};struct
Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind
Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tak={
Cyc_Absyn_AnyKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,
Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};struct
Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind
Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind Cyc_Tcutil_umk={
Cyc_Absyn_MemKind,Cyc_Absyn_Unique};struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)&
Cyc_Tcutil_rk};struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};struct
Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};struct Cyc_Core_Opt Cyc_Tcutil_mko={(
void*)& Cyc_Tcutil_mk};struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};struct Cyc_Core_Opt Cyc_Tcutil_trko={(
void*)& Cyc_Tcutil_trk};struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};struct Cyc_Core_Opt Cyc_Tcutil_tmko={(
void*)& Cyc_Tcutil_tmk};struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};struct Cyc_Core_Opt Cyc_Tcutil_ubko={(
void*)& Cyc_Tcutil_ubk};struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka);struct Cyc_Core_Opt*
Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6A7;
enum Cyc_Absyn_KindQual _tmp6A8;enum Cyc_Absyn_AliasQual _tmp6A9;struct Cyc_Absyn_Kind*
_tmp6A6=ka;_tmp6A7=*_tmp6A6;_tmp6A8=_tmp6A7.kind;_tmp6A9=_tmp6A7.aliasqual;
switch(_tmp6A9){case Cyc_Absyn_Aliasable: _LL36C: switch(_tmp6A8){case Cyc_Absyn_AnyKind:
_LL36E: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL36F:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL370: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL371: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL372: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL373: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL36D: switch(_tmp6A8){case Cyc_Absyn_AnyKind: _LL376: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL377: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL378: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL379: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL37A: break;}break;case Cyc_Absyn_Top: _LL375: switch(_tmp6A8){case Cyc_Absyn_AnyKind:
_LL37D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL37E:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL37F: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL380: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL381: break;}break;}{const char*_tmp10CF;
void*_tmp10CE[1];struct Cyc_String_pa_struct _tmp10CD;(_tmp10CD.tag=0,((_tmp10CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp10CE[0]=& _tmp10CD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10CF="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp10CF,sizeof(char),26))),_tag_dyneither(_tmp10CE,sizeof(void*),1)))))));};}
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka);static void**Cyc_Tcutil_kind_to_b(
struct Cyc_Absyn_Kind*ka){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_struct mb_v={0,& Cyc_Tcutil_mk};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,& Cyc_Tcutil_bk};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_struct eb_v={0,& Cyc_Tcutil_ek};static struct Cyc_Absyn_Eq_kb_struct
ib_v={0,& Cyc_Tcutil_ik};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;static void*rb=(void*)& rb_v;static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;static struct Cyc_Absyn_Eq_kb_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_struct tmb_v={0,& Cyc_Tcutil_tmk};static struct Cyc_Absyn_Eq_kb_struct
tbb_v={0,& Cyc_Tcutil_tbk};static struct Cyc_Absyn_Eq_kb_struct trb_v={0,& Cyc_Tcutil_trk};
static void*tab=(void*)& tab_v;static void*tmb=(void*)& tmb_v;static void*tbb=(void*)&
tbb_v;static void*trb=(void*)& trb_v;static struct Cyc_Absyn_Eq_kb_struct uab_v={0,&
Cyc_Tcutil_uak};static struct Cyc_Absyn_Eq_kb_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_struct ubb_v={0,& Cyc_Tcutil_ubk};static struct Cyc_Absyn_Eq_kb_struct
urb_v={0,& Cyc_Tcutil_urk};static void*uab=(void*)& uab_v;static void*umb=(void*)&
umb_v;static void*ubb=(void*)& ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind
_tmp6AE;enum Cyc_Absyn_KindQual _tmp6AF;enum Cyc_Absyn_AliasQual _tmp6B0;struct Cyc_Absyn_Kind*
_tmp6AD=ka;_tmp6AE=*_tmp6AD;_tmp6AF=_tmp6AE.kind;_tmp6B0=_tmp6AE.aliasqual;
switch(_tmp6B0){case Cyc_Absyn_Aliasable: _LL383: switch(_tmp6AF){case Cyc_Absyn_AnyKind:
_LL385: return& ab;case Cyc_Absyn_MemKind: _LL386: return& mb;case Cyc_Absyn_BoxKind:
_LL387: return& bb;case Cyc_Absyn_RgnKind: _LL388: return& rb;case Cyc_Absyn_EffKind:
_LL389: return& eb;case Cyc_Absyn_IntKind: _LL38A: return& ib;}case Cyc_Absyn_Unique:
_LL384: switch(_tmp6AF){case Cyc_Absyn_AnyKind: _LL38D: return& uab;case Cyc_Absyn_MemKind:
_LL38E: return& umb;case Cyc_Absyn_BoxKind: _LL38F: return& ubb;case Cyc_Absyn_RgnKind:
_LL390: return& urb;default: _LL391: break;}break;case Cyc_Absyn_Top: _LL38C: switch(
_tmp6AF){case Cyc_Absyn_AnyKind: _LL394: return& tab;case Cyc_Absyn_MemKind: _LL395:
return& tmb;case Cyc_Absyn_BoxKind: _LL396: return& tbb;case Cyc_Absyn_RgnKind: _LL397:
return& trb;default: _LL398: break;}break;}{const char*_tmp10D3;void*_tmp10D2[1];
struct Cyc_String_pa_struct _tmp10D1;(_tmp10D1.tag=0,((_tmp10D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10D2[0]=&
_tmp10D1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10D3="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp10D3,sizeof(char),24))),
_tag_dyneither(_tmp10D2,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct
Cyc_Absyn_Kind*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp10D6;struct Cyc_Absyn_Const_e_struct*
_tmp10D5;e1->r=(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((
_tmp10D6.tag=0,((_tmp10D6.f1=Cyc_Absyn_Null_c,_tmp10D6)))),_tmp10D5))));}{struct
Cyc_Core_Opt*_tmp6C4=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp10E0;struct Cyc_Absyn_PtrAtts _tmp10DF;struct Cyc_Absyn_PtrInfo _tmp10DE;struct
Cyc_Absyn_PointerType_struct*_tmp10DD;struct Cyc_Absyn_PointerType_struct*_tmp6C5=(
_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((_tmp10E0.tag=5,((_tmp10E0.f1=((
_tmp10DE.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp6C4),((
_tmp10DE.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp10DE.ptr_atts=((_tmp10DF.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp6C4),((_tmp10DF.nullable=Cyc_Absyn_true_conref,((
_tmp10DF.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp10DF.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp10DF.ptrloc=0,_tmp10DF)))))))))),_tmp10DE)))))),_tmp10E0)))),_tmp10DD)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp6C5);return Cyc_Tcutil_coerce_arg(
te,e1,t2);};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion
c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL39A: {const char*_tmp10E1;return(
_tmp10E1="unknown",_tag_dyneither(_tmp10E1,sizeof(char),8));}case Cyc_Absyn_No_coercion:
_LL39B: {const char*_tmp10E2;return(_tmp10E2="no coercion",_tag_dyneither(
_tmp10E2,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL39C: {const char*
_tmp10E3;return(_tmp10E3="null check",_tag_dyneither(_tmp10E3,sizeof(char),11));}
case Cyc_Absyn_Other_coercion: _LL39D: {const char*_tmp10E4;return(_tmp10E4="other coercion",
_tag_dyneither(_tmp10E4,sizeof(char),15));}}}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);enum Cyc_Absyn_Coercion c;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp10E9;void*_tmp10E8[2];
struct Cyc_String_pa_struct _tmp10E7;struct Cyc_String_pa_struct _tmp10E6;(_tmp10E6.tag=
0,((_tmp10E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp10E7.tag=0,((_tmp10E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp10E8[0]=& _tmp10E7,((_tmp10E8[1]=& _tmp10E6,Cyc_Tcutil_warn(
e->loc,((_tmp10E9="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp10E9,sizeof(char),53))),_tag_dyneither(_tmp10E8,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,Cyc_Absyn_Other_coercion);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*
_tmp10EE;void*_tmp10ED[2];struct Cyc_String_pa_struct _tmp10EC;struct Cyc_String_pa_struct
_tmp10EB;(_tmp10EB.tag=0,((_tmp10EB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp10EC.tag=0,((_tmp10EC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp10ED[0]=& _tmp10EC,((_tmp10ED[1]=& _tmp10EB,Cyc_Tcutil_warn(e->loc,((_tmp10EE="implicit cast from %s to %s",
_tag_dyneither(_tmp10EE,sizeof(char),28))),_tag_dyneither(_tmp10ED,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp6D6=Cyc_Tcutil_compress(t);
_LL3A0: {struct Cyc_Absyn_IntType_struct*_tmp6D7=(struct Cyc_Absyn_IntType_struct*)
_tmp6D6;if(_tmp6D7->tag != 6)goto _LL3A2;}_LL3A1: goto _LL3A3;_LL3A2: {struct Cyc_Absyn_FloatType_struct*
_tmp6D8=(struct Cyc_Absyn_FloatType_struct*)_tmp6D6;if(_tmp6D8->tag != 7)goto
_LL3A4;}_LL3A3: goto _LL3A5;_LL3A4: {struct Cyc_Absyn_DoubleType_struct*_tmp6D9=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6D6;if(_tmp6D9->tag != 8)goto _LL3A6;}
_LL3A5: return 1;_LL3A6:;_LL3A7: return 0;_LL39F:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp6DB;
struct _RegionHandle*_tmp6DC;struct _tuple15 _tmp6DA=*env;_tmp6DB=_tmp6DA.f1;
_tmp6DC=_tmp6DA.f2;{struct _tuple11*_tmp10EF;return(_tmp10EF=_region_malloc(
_tmp6DC,sizeof(*_tmp10EF)),((_tmp10EF->f1=x->tq,((_tmp10EF->f2=Cyc_Tcutil_rsubstitute(
_tmp6DC,_tmp6DB,x->type),_tmp10EF)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp6DF;void*
_tmp6E0;struct _tuple11 _tmp6DE=*x;_tmp6DF=_tmp6DE.f1;_tmp6E0=_tmp6DE.f2;{struct
_tuple11*_tmp10F0;return(_tmp10F0=_region_malloc(r,sizeof(*_tmp10F0)),((_tmp10F0->f1=
_tmp6DF,((_tmp10F0->f2=_tmp6E0,_tmp10F0)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp6E2=t1;struct Cyc_List_List*_tmp6E5;struct Cyc_Absyn_AggrInfo
_tmp6E7;union Cyc_Absyn_AggrInfoU _tmp6E8;struct Cyc_Absyn_Aggrdecl**_tmp6E9;struct
Cyc_Absyn_Aggrdecl*_tmp6EA;struct Cyc_List_List*_tmp6EB;enum Cyc_Absyn_AggrKind
_tmp6ED;struct Cyc_List_List*_tmp6EE;_LL3A9: {struct Cyc_Absyn_VoidType_struct*
_tmp6E3=(struct Cyc_Absyn_VoidType_struct*)_tmp6E2;if(_tmp6E3->tag != 0)goto _LL3AB;}
_LL3AA: return 0;_LL3AB: {struct Cyc_Absyn_TupleType_struct*_tmp6E4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6E2;if(_tmp6E4->tag != 11)goto _LL3AD;else{_tmp6E5=_tmp6E4->f1;}}_LL3AC: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp6E5);_LL3AD: {struct Cyc_Absyn_AggrType_struct*
_tmp6E6=(struct Cyc_Absyn_AggrType_struct*)_tmp6E2;if(_tmp6E6->tag != 12)goto
_LL3AF;else{_tmp6E7=_tmp6E6->f1;_tmp6E8=_tmp6E7.aggr_info;if((_tmp6E8.KnownAggr).tag
!= 2)goto _LL3AF;_tmp6E9=(struct Cyc_Absyn_Aggrdecl**)(_tmp6E8.KnownAggr).val;
_tmp6EA=*_tmp6E9;_tmp6EB=_tmp6E7.targs;}}_LL3AE: if(((_tmp6EA->kind == Cyc_Absyn_UnionA
 || _tmp6EA->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6EA->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6EA->impl))->rgn_po != 0){
struct _tuple11*_tmp10F3;struct Cyc_List_List*_tmp10F2;return(_tmp10F2=
_region_malloc(r,sizeof(*_tmp10F2)),((_tmp10F2->hd=((_tmp10F3=_region_malloc(r,
sizeof(*_tmp10F3)),((_tmp10F3->f1=Cyc_Absyn_const_tqual(0),((_tmp10F3->f2=t1,
_tmp10F3)))))),((_tmp10F2->tl=0,_tmp10F2)))));}{struct Cyc_List_List*_tmp6F1=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6EA->tvs,_tmp6EB);struct _tuple15
_tmp10F4;struct _tuple15 env=(_tmp10F4.f1=_tmp6F1,((_tmp10F4.f2=r,_tmp10F4)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6EA->impl))->fields);};_LL3AF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6EC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6E2;if(_tmp6EC->tag != 13)goto
_LL3B1;else{_tmp6ED=_tmp6EC->f1;if(_tmp6ED != Cyc_Absyn_StructA)goto _LL3B1;
_tmp6EE=_tmp6EC->f2;}}_LL3B0: {struct _tuple15 _tmp10F5;struct _tuple15 env=(
_tmp10F5.f1=0,((_tmp10F5.f2=r,_tmp10F5)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp6EE);}_LL3B1:;_LL3B2: {struct _tuple11*_tmp10F8;struct Cyc_List_List*
_tmp10F7;return(_tmp10F7=_region_malloc(r,sizeof(*_tmp10F7)),((_tmp10F7->hd=((
_tmp10F8=_region_malloc(r,sizeof(*_tmp10F8)),((_tmp10F8->f1=Cyc_Absyn_const_tqual(
0),((_tmp10F8->f2=t1,_tmp10F8)))))),((_tmp10F7->tl=0,_tmp10F7)))));}_LL3A8:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp6F6=(void*)t->hd;_LL3B4: {
struct Cyc_Absyn_Pure_att_struct*_tmp6F7=(struct Cyc_Absyn_Pure_att_struct*)
_tmp6F6;if(_tmp6F7->tag != 21)goto _LL3B6;}_LL3B5: goto _LL3B7;_LL3B6: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp6F8=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp6F6;if(_tmp6F8->tag != 4)goto
_LL3B8;}_LL3B7: goto _LL3B9;_LL3B8: {struct Cyc_Absyn_Initializes_att_struct*
_tmp6F9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp6F6;if(_tmp6F9->tag != 20)
goto _LL3BA;}_LL3B9: continue;_LL3BA:;_LL3BB: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL3B3:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp10F9;struct _tuple0 _tmp6FB=(_tmp10F9.f1=t1,((_tmp10F9.f2=t2,_tmp10F9)));void*
_tmp6FC;struct Cyc_Absyn_PtrInfo _tmp6FE;void*_tmp6FF;struct Cyc_Absyn_Tqual _tmp700;
struct Cyc_Absyn_PtrAtts _tmp701;void*_tmp702;union Cyc_Absyn_Constraint*_tmp703;
union Cyc_Absyn_Constraint*_tmp704;union Cyc_Absyn_Constraint*_tmp705;void*_tmp706;
struct Cyc_Absyn_PtrInfo _tmp708;void*_tmp709;struct Cyc_Absyn_Tqual _tmp70A;struct
Cyc_Absyn_PtrAtts _tmp70B;void*_tmp70C;union Cyc_Absyn_Constraint*_tmp70D;union Cyc_Absyn_Constraint*
_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;void*_tmp710;struct Cyc_Absyn_DatatypeFieldInfo
_tmp712;union Cyc_Absyn_DatatypeFieldInfoU _tmp713;struct _tuple3 _tmp714;struct Cyc_Absyn_Datatypedecl*
_tmp715;struct Cyc_Absyn_Datatypefield*_tmp716;struct Cyc_List_List*_tmp717;void*
_tmp718;struct Cyc_Absyn_DatatypeInfo _tmp71A;union Cyc_Absyn_DatatypeInfoU _tmp71B;
struct Cyc_Absyn_Datatypedecl**_tmp71C;struct Cyc_Absyn_Datatypedecl*_tmp71D;
struct Cyc_List_List*_tmp71E;void*_tmp71F;struct Cyc_Absyn_FnInfo _tmp721;void*
_tmp722;struct Cyc_Absyn_FnInfo _tmp724;_LL3BD: _tmp6FC=_tmp6FB.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6FD=(struct Cyc_Absyn_PointerType_struct*)_tmp6FC;if(_tmp6FD->tag != 5)goto
_LL3BF;else{_tmp6FE=_tmp6FD->f1;_tmp6FF=_tmp6FE.elt_typ;_tmp700=_tmp6FE.elt_tq;
_tmp701=_tmp6FE.ptr_atts;_tmp702=_tmp701.rgn;_tmp703=_tmp701.nullable;_tmp704=
_tmp701.bounds;_tmp705=_tmp701.zero_term;}};_tmp706=_tmp6FB.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp707=(struct Cyc_Absyn_PointerType_struct*)_tmp706;if(_tmp707->tag != 5)goto
_LL3BF;else{_tmp708=_tmp707->f1;_tmp709=_tmp708.elt_typ;_tmp70A=_tmp708.elt_tq;
_tmp70B=_tmp708.ptr_atts;_tmp70C=_tmp70B.rgn;_tmp70D=_tmp70B.nullable;_tmp70E=
_tmp70B.bounds;_tmp70F=_tmp70B.zero_term;}};_LL3BE: if(_tmp700.real_const  && !
_tmp70A.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp703,
_tmp70D) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp703)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp70D))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp705,
_tmp70F) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp705)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp70F))return 0;if(!Cyc_Tcutil_unify(_tmp702,_tmp70C) && !Cyc_Tcenv_region_outlives(
te,_tmp702,_tmp70C))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp704,_tmp70E)){struct _tuple0 _tmp10FA;struct _tuple0 _tmp726=(_tmp10FA.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp704),((_tmp10FA.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70E),_tmp10FA)));void*
_tmp727;void*_tmp729;void*_tmp72B;struct Cyc_Absyn_Exp*_tmp72D;void*_tmp72E;
struct Cyc_Absyn_Exp*_tmp730;_LL3C6: _tmp727=_tmp726.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp728=(struct Cyc_Absyn_Upper_b_struct*)_tmp727;if(_tmp728->tag != 1)goto _LL3C8;};
_tmp729=_tmp726.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp72A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp729;if(_tmp72A->tag != 0)goto _LL3C8;};_LL3C7: goto _LL3C5;_LL3C8: _tmp72B=
_tmp726.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp72C=(struct Cyc_Absyn_Upper_b_struct*)
_tmp72B;if(_tmp72C->tag != 1)goto _LL3CA;else{_tmp72D=_tmp72C->f1;}};_tmp72E=
_tmp726.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp72F=(struct Cyc_Absyn_Upper_b_struct*)
_tmp72E;if(_tmp72F->tag != 1)goto _LL3CA;else{_tmp730=_tmp72F->f1;}};_LL3C9: if(!
Cyc_Evexp_lte_const_exp(_tmp730,_tmp72D))return 0;goto _LL3C5;_LL3CA:;_LL3CB:
return 0;_LL3C5:;}{struct _tuple0*_tmp10FD;struct Cyc_List_List*_tmp10FC;return Cyc_Tcutil_ptrsubtype(
te,((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC->hd=((_tmp10FD=_cycalloc(
sizeof(*_tmp10FD)),((_tmp10FD->f1=t1,((_tmp10FD->f2=t2,_tmp10FD)))))),((_tmp10FC->tl=
assume,_tmp10FC)))))),_tmp6FF,_tmp709);};_LL3BF: _tmp710=_tmp6FB.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp711=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp710;if(_tmp711->tag != 4)
goto _LL3C1;else{_tmp712=_tmp711->f1;_tmp713=_tmp712.field_info;if((_tmp713.KnownDatatypefield).tag
!= 2)goto _LL3C1;_tmp714=(struct _tuple3)(_tmp713.KnownDatatypefield).val;_tmp715=
_tmp714.f1;_tmp716=_tmp714.f2;_tmp717=_tmp712.targs;}};_tmp718=_tmp6FB.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp719=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp718;if(_tmp719->tag != 3)goto _LL3C1;else{_tmp71A=_tmp719->f1;_tmp71B=_tmp71A.datatype_info;
if((_tmp71B.KnownDatatype).tag != 2)goto _LL3C1;_tmp71C=(struct Cyc_Absyn_Datatypedecl**)(
_tmp71B.KnownDatatype).val;_tmp71D=*_tmp71C;_tmp71E=_tmp71A.targs;}};_LL3C0: if(
_tmp715 != _tmp71D  && Cyc_Absyn_qvar_cmp(_tmp715->name,_tmp71D->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp717)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp71E))return 0;for(0;_tmp717 != 0;(_tmp717=
_tmp717->tl,_tmp71E=_tmp71E->tl)){if(!Cyc_Tcutil_unify((void*)_tmp717->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp71E))->hd))return 0;}return 1;_LL3C1: _tmp71F=
_tmp6FB.f1;{struct Cyc_Absyn_FnType_struct*_tmp720=(struct Cyc_Absyn_FnType_struct*)
_tmp71F;if(_tmp720->tag != 10)goto _LL3C3;else{_tmp721=_tmp720->f1;}};_tmp722=
_tmp6FB.f2;{struct Cyc_Absyn_FnType_struct*_tmp723=(struct Cyc_Absyn_FnType_struct*)
_tmp722;if(_tmp723->tag != 10)goto _LL3C3;else{_tmp724=_tmp723->f1;}};_LL3C2: if(
_tmp721.tvars != 0  || _tmp724.tvars != 0){struct Cyc_List_List*_tmp733=_tmp721.tvars;
struct Cyc_List_List*_tmp734=_tmp724.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp733)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp734))return 0;{
struct _RegionHandle*_tmp735=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp733 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp733->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp734))->hd)->kind))return 0;{struct _tuple14*_tmp1107;struct Cyc_Absyn_VarType_struct
_tmp1106;struct Cyc_Absyn_VarType_struct*_tmp1105;struct Cyc_List_List*_tmp1104;
inst=((_tmp1104=_region_malloc(_tmp735,sizeof(*_tmp1104)),((_tmp1104->hd=((
_tmp1107=_region_malloc(_tmp735,sizeof(*_tmp1107)),((_tmp1107->f1=(struct Cyc_Absyn_Tvar*)
_tmp734->hd,((_tmp1107->f2=(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((
_tmp1105[0]=((_tmp1106.tag=2,((_tmp1106.f1=(struct Cyc_Absyn_Tvar*)_tmp733->hd,
_tmp1106)))),_tmp1105)))),_tmp1107)))))),((_tmp1104->tl=inst,_tmp1104))))));}
_tmp733=_tmp733->tl;_tmp734=_tmp734->tl;}if(inst != 0){_tmp721.tvars=0;_tmp724.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp110D;struct Cyc_Absyn_FnType_struct*_tmp110C;
struct Cyc_Absyn_FnType_struct _tmp110A;struct Cyc_Absyn_FnType_struct*_tmp1109;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1109=_cycalloc(sizeof(*_tmp1109)),((
_tmp1109[0]=((_tmp110A.tag=10,((_tmp110A.f1=_tmp721,_tmp110A)))),_tmp1109)))),(
void*)((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110D.tag=10,((
_tmp110D.f1=_tmp724,_tmp110D)))),_tmp110C)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp721.ret_typ,_tmp724.ret_typ))return 0;{struct Cyc_List_List*_tmp73E=
_tmp721.args;struct Cyc_List_List*_tmp73F=_tmp724.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp73E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp73F))return 0;for(0;_tmp73E != 0;(_tmp73E=_tmp73E->tl,_tmp73F=_tmp73F->tl)){
struct Cyc_Absyn_Tqual _tmp741;void*_tmp742;struct _tuple9 _tmp740=*((struct _tuple9*)
_tmp73E->hd);_tmp741=_tmp740.f2;_tmp742=_tmp740.f3;{struct Cyc_Absyn_Tqual _tmp744;
void*_tmp745;struct _tuple9 _tmp743=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp73F))->hd);_tmp744=_tmp743.f2;_tmp745=_tmp743.f3;if(_tmp744.real_const
 && !_tmp741.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp745,_tmp742))return
0;};}if(_tmp721.c_varargs != _tmp724.c_varargs)return 0;if(_tmp721.cyc_varargs != 0
 && _tmp724.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp746=*_tmp721.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp747=*_tmp724.cyc_varargs;if((_tmp747.tq).real_const
 && !(_tmp746.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp747.type,
_tmp746.type))return 0;}else{if(_tmp721.cyc_varargs != 0  || _tmp724.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp721.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp724.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp721.rgn_po,_tmp724.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp721.attributes,_tmp724.attributes))return 0;return 1;};_LL3C3:;_LL3C4: return 0;
_LL3BC:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp110E;struct _tuple0 _tmp749=(_tmp110E.f1=Cyc_Tcutil_compress(
t1),((_tmp110E.f2=Cyc_Tcutil_compress(t2),_tmp110E)));void*_tmp74A;enum Cyc_Absyn_Size_of
_tmp74C;void*_tmp74D;enum Cyc_Absyn_Size_of _tmp74F;_LL3CD: _tmp74A=_tmp749.f1;{
struct Cyc_Absyn_IntType_struct*_tmp74B=(struct Cyc_Absyn_IntType_struct*)_tmp74A;
if(_tmp74B->tag != 6)goto _LL3CF;else{_tmp74C=_tmp74B->f2;}};_tmp74D=_tmp749.f2;{
struct Cyc_Absyn_IntType_struct*_tmp74E=(struct Cyc_Absyn_IntType_struct*)_tmp74D;
if(_tmp74E->tag != 6)goto _LL3CF;else{_tmp74F=_tmp74E->f2;}};_LL3CE: return(_tmp74C
== _tmp74F  || _tmp74C == Cyc_Absyn_Int_sz  && _tmp74F == Cyc_Absyn_Long_sz) || 
_tmp74C == Cyc_Absyn_Long_sz  && _tmp74F == Cyc_Absyn_Int_sz;_LL3CF:;_LL3D0: return 0;
_LL3CC:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp750=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp750,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp750,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp752;struct Cyc_Absyn_Tqual
_tmp753;void*_tmp754;struct _tuple11*_tmp751=(struct _tuple11*)tqs1->hd;_tmp752=*
_tmp751;_tmp753=_tmp752.f1;_tmp754=_tmp752.f2;{struct _tuple11 _tmp756;struct Cyc_Absyn_Tqual
_tmp757;void*_tmp758;struct _tuple11*_tmp755=(struct _tuple11*)tqs2->hd;_tmp756=*
_tmp755;_tmp757=_tmp756.f1;_tmp758=_tmp756.f2;if(_tmp757.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp754,_tmp758))continue;else{if(Cyc_Tcutil_unify(_tmp754,_tmp758))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp754,_tmp758))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp759=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp75B;_LL3D2: {
struct Cyc_Absyn_IntType_struct*_tmp75A=(struct Cyc_Absyn_IntType_struct*)_tmp759;
if(_tmp75A->tag != 6)goto _LL3D4;else{_tmp75B=_tmp75A->f2;if(_tmp75B != Cyc_Absyn_Char_sz)
goto _LL3D4;}}_LL3D3: return 1;_LL3D4:;_LL3D5: return 0;_LL3D1:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp75C=
t2;enum Cyc_Absyn_Size_of _tmp75E;enum Cyc_Absyn_Size_of _tmp760;_LL3D7: {struct
Cyc_Absyn_IntType_struct*_tmp75D=(struct Cyc_Absyn_IntType_struct*)_tmp75C;if(
_tmp75D->tag != 6)goto _LL3D9;else{_tmp75E=_tmp75D->f2;if(_tmp75E != Cyc_Absyn_Int_sz)
goto _LL3D9;}}_LL3D8: goto _LL3DA;_LL3D9: {struct Cyc_Absyn_IntType_struct*_tmp75F=(
struct Cyc_Absyn_IntType_struct*)_tmp75C;if(_tmp75F->tag != 6)goto _LL3DB;else{
_tmp760=_tmp75F->f2;if(_tmp760 != Cyc_Absyn_Long_sz)goto _LL3DB;}}_LL3DA: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL3D6;_LL3DB:;
_LL3DC: goto _LL3D6;_LL3D6:;}{void*_tmp761=t1;struct Cyc_Absyn_PtrInfo _tmp763;void*
_tmp764;struct Cyc_Absyn_Tqual _tmp765;struct Cyc_Absyn_PtrAtts _tmp766;void*_tmp767;
union Cyc_Absyn_Constraint*_tmp768;union Cyc_Absyn_Constraint*_tmp769;union Cyc_Absyn_Constraint*
_tmp76A;struct Cyc_Absyn_ArrayInfo _tmp76C;void*_tmp76D;struct Cyc_Absyn_Tqual
_tmp76E;struct Cyc_Absyn_Exp*_tmp76F;union Cyc_Absyn_Constraint*_tmp770;struct Cyc_Absyn_Enumdecl*
_tmp772;void*_tmp777;_LL3DE: {struct Cyc_Absyn_PointerType_struct*_tmp762=(struct
Cyc_Absyn_PointerType_struct*)_tmp761;if(_tmp762->tag != 5)goto _LL3E0;else{
_tmp763=_tmp762->f1;_tmp764=_tmp763.elt_typ;_tmp765=_tmp763.elt_tq;_tmp766=
_tmp763.ptr_atts;_tmp767=_tmp766.rgn;_tmp768=_tmp766.nullable;_tmp769=_tmp766.bounds;
_tmp76A=_tmp766.zero_term;}}_LL3DF:{void*_tmp778=t2;struct Cyc_Absyn_PtrInfo
_tmp77A;void*_tmp77B;struct Cyc_Absyn_Tqual _tmp77C;struct Cyc_Absyn_PtrAtts _tmp77D;
void*_tmp77E;union Cyc_Absyn_Constraint*_tmp77F;union Cyc_Absyn_Constraint*_tmp780;
union Cyc_Absyn_Constraint*_tmp781;_LL3EF: {struct Cyc_Absyn_PointerType_struct*
_tmp779=(struct Cyc_Absyn_PointerType_struct*)_tmp778;if(_tmp779->tag != 5)goto
_LL3F1;else{_tmp77A=_tmp779->f1;_tmp77B=_tmp77A.elt_typ;_tmp77C=_tmp77A.elt_tq;
_tmp77D=_tmp77A.ptr_atts;_tmp77E=_tmp77D.rgn;_tmp77F=_tmp77D.nullable;_tmp780=
_tmp77D.bounds;_tmp781=_tmp77D.zero_term;}}_LL3F0: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1111;struct Cyc_List_List*
_tmp1110;struct Cyc_List_List*_tmp782=(_tmp1110=_cycalloc(sizeof(*_tmp1110)),((
_tmp1110->hd=((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111->f1=t1,((
_tmp1111->f2=t2,_tmp1111)))))),((_tmp1110->tl=0,_tmp1110)))));int _tmp783=Cyc_Tcutil_ptrsubtype(
te,_tmp782,_tmp764,_tmp77B) && (!_tmp765.real_const  || _tmp77C.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp76A,
_tmp781) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp781);
int _tmp784=_tmp783?0:((Cyc_Tcutil_bits_only(_tmp764) && Cyc_Tcutil_is_char_type(
_tmp77B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp781)) && (_tmp77C.real_const  || !_tmp765.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp769,_tmp780);if(!
bounds_ok  && !_tmp784){struct _tuple0 _tmp1112;struct _tuple0 _tmp786=(_tmp1112.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp769),((_tmp1112.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp780),_tmp1112)));
void*_tmp787;struct Cyc_Absyn_Exp*_tmp789;void*_tmp78A;struct Cyc_Absyn_Exp*
_tmp78C;_LL3F4: _tmp787=_tmp786.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp788=(
struct Cyc_Absyn_Upper_b_struct*)_tmp787;if(_tmp788->tag != 1)goto _LL3F6;else{
_tmp789=_tmp788->f1;}};_tmp78A=_tmp786.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp78B=(struct Cyc_Absyn_Upper_b_struct*)_tmp78A;if(_tmp78B->tag != 1)goto _LL3F6;
else{_tmp78C=_tmp78B->f1;}};_LL3F5: if(Cyc_Evexp_lte_const_exp(_tmp78C,_tmp789))
bounds_ok=1;goto _LL3F3;_LL3F6:;_LL3F7: bounds_ok=1;goto _LL3F3;_LL3F3:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp768) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp77F))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp783  || _tmp784)) && (Cyc_Tcutil_unify(
_tmp767,_tmp77E) || Cyc_Tcenv_region_outlives(te,_tmp767,_tmp77E)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL3F1:;_LL3F2: goto _LL3EE;
_LL3EE:;}return Cyc_Absyn_Unknown_coercion;_LL3E0: {struct Cyc_Absyn_ArrayType_struct*
_tmp76B=(struct Cyc_Absyn_ArrayType_struct*)_tmp761;if(_tmp76B->tag != 9)goto
_LL3E2;else{_tmp76C=_tmp76B->f1;_tmp76D=_tmp76C.elt_type;_tmp76E=_tmp76C.tq;
_tmp76F=_tmp76C.num_elts;_tmp770=_tmp76C.zero_term;}}_LL3E1:{void*_tmp78F=t2;
struct Cyc_Absyn_ArrayInfo _tmp791;void*_tmp792;struct Cyc_Absyn_Tqual _tmp793;
struct Cyc_Absyn_Exp*_tmp794;union Cyc_Absyn_Constraint*_tmp795;_LL3F9: {struct Cyc_Absyn_ArrayType_struct*
_tmp790=(struct Cyc_Absyn_ArrayType_struct*)_tmp78F;if(_tmp790->tag != 9)goto
_LL3FB;else{_tmp791=_tmp790->f1;_tmp792=_tmp791.elt_type;_tmp793=_tmp791.tq;
_tmp794=_tmp791.num_elts;_tmp795=_tmp791.zero_term;}}_LL3FA: {int okay;okay=((
_tmp76F != 0  && _tmp794 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp770,
_tmp795)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp794,(struct Cyc_Absyn_Exp*)
_tmp76F);return(okay  && Cyc_Tcutil_unify(_tmp76D,_tmp792)) && (!_tmp76E.real_const
 || _tmp793.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL3FB:;_LL3FC: return Cyc_Absyn_Unknown_coercion;_LL3F8:;}return Cyc_Absyn_Unknown_coercion;
_LL3E2: {struct Cyc_Absyn_EnumType_struct*_tmp771=(struct Cyc_Absyn_EnumType_struct*)
_tmp761;if(_tmp771->tag != 14)goto _LL3E4;else{_tmp772=_tmp771->f2;}}_LL3E3:{void*
_tmp796=t2;struct Cyc_Absyn_Enumdecl*_tmp798;_LL3FE: {struct Cyc_Absyn_EnumType_struct*
_tmp797=(struct Cyc_Absyn_EnumType_struct*)_tmp796;if(_tmp797->tag != 14)goto
_LL400;else{_tmp798=_tmp797->f2;}}_LL3FF: if((_tmp772->fields != 0  && _tmp798->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp772->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp798->fields))->v))return Cyc_Absyn_No_coercion;goto _LL3FD;_LL400:;_LL401: goto
_LL3FD;_LL3FD:;}goto _LL3E5;_LL3E4: {struct Cyc_Absyn_IntType_struct*_tmp773=(
struct Cyc_Absyn_IntType_struct*)_tmp761;if(_tmp773->tag != 6)goto _LL3E6;}_LL3E5:
goto _LL3E7;_LL3E6: {struct Cyc_Absyn_FloatType_struct*_tmp774=(struct Cyc_Absyn_FloatType_struct*)
_tmp761;if(_tmp774->tag != 7)goto _LL3E8;}_LL3E7: goto _LL3E9;_LL3E8: {struct Cyc_Absyn_DoubleType_struct*
_tmp775=(struct Cyc_Absyn_DoubleType_struct*)_tmp761;if(_tmp775->tag != 8)goto
_LL3EA;}_LL3E9: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL3EA: {struct Cyc_Absyn_RgnHandleType_struct*_tmp776=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp761;if(_tmp776->tag != 16)goto _LL3EC;else{_tmp777=(void*)_tmp776->f1;}}_LL3EB:{
void*_tmp799=t2;void*_tmp79B;_LL403: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp79A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp799;if(_tmp79A->tag != 16)goto
_LL405;else{_tmp79B=(void*)_tmp79A->f1;}}_LL404: if(Cyc_Tcenv_region_outlives(te,
_tmp777,_tmp79B))return Cyc_Absyn_No_coercion;goto _LL402;_LL405:;_LL406: goto
_LL402;_LL402:;}return Cyc_Absyn_Unknown_coercion;_LL3EC:;_LL3ED: return Cyc_Absyn_Unknown_coercion;
_LL3DD:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp79C=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp1115;struct Cyc_Absyn_Cast_e_struct*
_tmp1114;e->r=(void*)((_tmp1114=_cycalloc(sizeof(*_tmp1114)),((_tmp1114[0]=((
_tmp1115.tag=15,((_tmp1115.f1=(void*)t,((_tmp1115.f2=_tmp79C,((_tmp1115.f3=0,((
_tmp1115.f4=c,_tmp1115)))))))))),_tmp1114))));}{struct Cyc_Core_Opt*_tmp1116;e->topt=((
_tmp1116=_cycalloc(sizeof(*_tmp1116)),((_tmp1116->v=t,_tmp1116))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp7A0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL408: {struct Cyc_Absyn_IntType_struct*_tmp7A1=(struct Cyc_Absyn_IntType_struct*)
_tmp7A0;if(_tmp7A1->tag != 6)goto _LL40A;}_LL409: goto _LL40B;_LL40A: {struct Cyc_Absyn_EnumType_struct*
_tmp7A2=(struct Cyc_Absyn_EnumType_struct*)_tmp7A0;if(_tmp7A2->tag != 14)goto
_LL40C;}_LL40B: goto _LL40D;_LL40C: {struct Cyc_Absyn_AnonEnumType_struct*_tmp7A3=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp7A0;if(_tmp7A3->tag != 15)goto _LL40E;}
_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_TagType_struct*_tmp7A4=(struct Cyc_Absyn_TagType_struct*)
_tmp7A0;if(_tmp7A4->tag != 20)goto _LL410;}_LL40F: return 1;_LL410: {struct Cyc_Absyn_Evar_struct*
_tmp7A5=(struct Cyc_Absyn_Evar_struct*)_tmp7A0;if(_tmp7A5->tag != 1)goto _LL412;}
_LL411: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL412:;_LL413: return 0;_LL407:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp7A6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL415: {struct Cyc_Absyn_FloatType_struct*_tmp7A7=(
struct Cyc_Absyn_FloatType_struct*)_tmp7A6;if(_tmp7A7->tag != 7)goto _LL417;}_LL416:
goto _LL418;_LL417: {struct Cyc_Absyn_DoubleType_struct*_tmp7A8=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7A6;if(_tmp7A8->tag != 8)goto _LL419;}_LL418: return 1;_LL419:;_LL41A: return 0;
_LL414:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp7A9=Cyc_Tcutil_compress(t);_LL41C: {struct Cyc_Absyn_FnType_struct*
_tmp7AA=(struct Cyc_Absyn_FnType_struct*)_tmp7A9;if(_tmp7AA->tag != 10)goto _LL41E;}
_LL41D: return 1;_LL41E:;_LL41F: return 0;_LL41B:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp1117;struct _tuple0 _tmp7AC=(_tmp1117.f1=t1,((_tmp1117.f2=t2,_tmp1117)));
void*_tmp7AD;int _tmp7AF;void*_tmp7B0;int _tmp7B2;void*_tmp7B3;void*_tmp7B5;void*
_tmp7B7;void*_tmp7B9;void*_tmp7BB;enum Cyc_Absyn_Sign _tmp7BD;enum Cyc_Absyn_Size_of
_tmp7BE;void*_tmp7BF;enum Cyc_Absyn_Sign _tmp7C1;enum Cyc_Absyn_Size_of _tmp7C2;
void*_tmp7C3;enum Cyc_Absyn_Size_of _tmp7C5;void*_tmp7C6;enum Cyc_Absyn_Size_of
_tmp7C8;void*_tmp7C9;enum Cyc_Absyn_Sign _tmp7CB;enum Cyc_Absyn_Size_of _tmp7CC;
void*_tmp7CD;enum Cyc_Absyn_Sign _tmp7CF;enum Cyc_Absyn_Size_of _tmp7D0;void*
_tmp7D1;enum Cyc_Absyn_Sign _tmp7D3;enum Cyc_Absyn_Size_of _tmp7D4;void*_tmp7D5;
enum Cyc_Absyn_Sign _tmp7D7;enum Cyc_Absyn_Size_of _tmp7D8;void*_tmp7D9;enum Cyc_Absyn_Size_of
_tmp7DB;void*_tmp7DC;enum Cyc_Absyn_Size_of _tmp7DE;_LL421: _tmp7AD=_tmp7AC.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp7AE=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7AD;if(_tmp7AE->tag != 8)goto _LL423;else{_tmp7AF=_tmp7AE->f1;}};_tmp7B0=
_tmp7AC.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp7B1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7B0;if(_tmp7B1->tag != 8)goto _LL423;else{_tmp7B2=_tmp7B1->f1;}};_LL422: if(
_tmp7AF)return t1;else{return t2;}_LL423: _tmp7B3=_tmp7AC.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp7B4=(struct Cyc_Absyn_DoubleType_struct*)_tmp7B3;if(_tmp7B4->tag != 8)goto
_LL425;};_LL424: return t1;_LL425: _tmp7B5=_tmp7AC.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp7B6=(struct Cyc_Absyn_DoubleType_struct*)_tmp7B5;if(_tmp7B6->tag != 8)goto
_LL427;};_LL426: return t2;_LL427: _tmp7B7=_tmp7AC.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp7B8=(struct Cyc_Absyn_FloatType_struct*)_tmp7B7;if(_tmp7B8->tag != 7)goto
_LL429;};_LL428: goto _LL42A;_LL429: _tmp7B9=_tmp7AC.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp7BA=(struct Cyc_Absyn_FloatType_struct*)_tmp7B9;if(_tmp7BA->tag != 7)goto
_LL42B;};_LL42A: return(void*)& Cyc_Absyn_FloatType_val;_LL42B: _tmp7BB=_tmp7AC.f1;{
struct Cyc_Absyn_IntType_struct*_tmp7BC=(struct Cyc_Absyn_IntType_struct*)_tmp7BB;
if(_tmp7BC->tag != 6)goto _LL42D;else{_tmp7BD=_tmp7BC->f1;if(_tmp7BD != Cyc_Absyn_Unsigned)
goto _LL42D;_tmp7BE=_tmp7BC->f2;if(_tmp7BE != Cyc_Absyn_LongLong_sz)goto _LL42D;}};
_LL42C: goto _LL42E;_LL42D: _tmp7BF=_tmp7AC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp7C0=(struct Cyc_Absyn_IntType_struct*)_tmp7BF;if(_tmp7C0->tag != 6)goto _LL42F;
else{_tmp7C1=_tmp7C0->f1;if(_tmp7C1 != Cyc_Absyn_Unsigned)goto _LL42F;_tmp7C2=
_tmp7C0->f2;if(_tmp7C2 != Cyc_Absyn_LongLong_sz)goto _LL42F;}};_LL42E: return Cyc_Absyn_ulonglong_typ;
_LL42F: _tmp7C3=_tmp7AC.f1;{struct Cyc_Absyn_IntType_struct*_tmp7C4=(struct Cyc_Absyn_IntType_struct*)
_tmp7C3;if(_tmp7C4->tag != 6)goto _LL431;else{_tmp7C5=_tmp7C4->f2;if(_tmp7C5 != Cyc_Absyn_LongLong_sz)
goto _LL431;}};_LL430: goto _LL432;_LL431: _tmp7C6=_tmp7AC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp7C7=(struct Cyc_Absyn_IntType_struct*)_tmp7C6;if(_tmp7C7->tag != 6)goto _LL433;
else{_tmp7C8=_tmp7C7->f2;if(_tmp7C8 != Cyc_Absyn_LongLong_sz)goto _LL433;}};_LL432:
return Cyc_Absyn_slonglong_typ;_LL433: _tmp7C9=_tmp7AC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7CA=(struct Cyc_Absyn_IntType_struct*)_tmp7C9;if(_tmp7CA->tag != 6)goto _LL435;
else{_tmp7CB=_tmp7CA->f1;if(_tmp7CB != Cyc_Absyn_Unsigned)goto _LL435;_tmp7CC=
_tmp7CA->f2;if(_tmp7CC != Cyc_Absyn_Long_sz)goto _LL435;}};_LL434: goto _LL436;
_LL435: _tmp7CD=_tmp7AC.f2;{struct Cyc_Absyn_IntType_struct*_tmp7CE=(struct Cyc_Absyn_IntType_struct*)
_tmp7CD;if(_tmp7CE->tag != 6)goto _LL437;else{_tmp7CF=_tmp7CE->f1;if(_tmp7CF != Cyc_Absyn_Unsigned)
goto _LL437;_tmp7D0=_tmp7CE->f2;if(_tmp7D0 != Cyc_Absyn_Long_sz)goto _LL437;}};
_LL436: return Cyc_Absyn_ulong_typ;_LL437: _tmp7D1=_tmp7AC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7D2=(struct Cyc_Absyn_IntType_struct*)_tmp7D1;if(_tmp7D2->tag != 6)goto _LL439;
else{_tmp7D3=_tmp7D2->f1;if(_tmp7D3 != Cyc_Absyn_Unsigned)goto _LL439;_tmp7D4=
_tmp7D2->f2;if(_tmp7D4 != Cyc_Absyn_Int_sz)goto _LL439;}};_LL438: goto _LL43A;_LL439:
_tmp7D5=_tmp7AC.f2;{struct Cyc_Absyn_IntType_struct*_tmp7D6=(struct Cyc_Absyn_IntType_struct*)
_tmp7D5;if(_tmp7D6->tag != 6)goto _LL43B;else{_tmp7D7=_tmp7D6->f1;if(_tmp7D7 != Cyc_Absyn_Unsigned)
goto _LL43B;_tmp7D8=_tmp7D6->f2;if(_tmp7D8 != Cyc_Absyn_Int_sz)goto _LL43B;}};
_LL43A: return Cyc_Absyn_uint_typ;_LL43B: _tmp7D9=_tmp7AC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7DA=(struct Cyc_Absyn_IntType_struct*)_tmp7D9;if(_tmp7DA->tag != 6)goto _LL43D;
else{_tmp7DB=_tmp7DA->f2;if(_tmp7DB != Cyc_Absyn_Long_sz)goto _LL43D;}};_LL43C:
goto _LL43E;_LL43D: _tmp7DC=_tmp7AC.f2;{struct Cyc_Absyn_IntType_struct*_tmp7DD=(
struct Cyc_Absyn_IntType_struct*)_tmp7DC;if(_tmp7DD->tag != 6)goto _LL43F;else{
_tmp7DE=_tmp7DD->f2;if(_tmp7DE != Cyc_Absyn_Long_sz)goto _LL43F;}};_LL43E: return
Cyc_Absyn_slong_typ;_LL43F:;_LL440: return Cyc_Absyn_sint_typ;_LL420:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp7DF=e->r;struct Cyc_Core_Opt*_tmp7E1;_LL442: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp7E0=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp7DF;if(_tmp7E0->tag != 4)goto
_LL444;else{_tmp7E1=_tmp7E0->f2;if(_tmp7E1 != 0)goto _LL444;}}_LL443:{const char*
_tmp111A;void*_tmp1119;(_tmp1119=0,Cyc_Tcutil_warn(e->loc,((_tmp111A="assignment in test",
_tag_dyneither(_tmp111A,sizeof(char),19))),_tag_dyneither(_tmp1119,sizeof(void*),
0)));}goto _LL441;_LL444:;_LL445: goto _LL441;_LL441:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp111B;struct _tuple0 _tmp7E5=(
_tmp111B.f1=c1,((_tmp111B.f2=c2,_tmp111B)));void*_tmp7E6;struct Cyc_Absyn_Kind*
_tmp7E8;void*_tmp7E9;struct Cyc_Absyn_Kind*_tmp7EB;void*_tmp7EC;struct Cyc_Core_Opt*
_tmp7EE;struct Cyc_Core_Opt**_tmp7EF;void*_tmp7F0;struct Cyc_Core_Opt*_tmp7F2;
struct Cyc_Core_Opt**_tmp7F3;void*_tmp7F4;struct Cyc_Core_Opt*_tmp7F6;struct Cyc_Core_Opt**
_tmp7F7;struct Cyc_Absyn_Kind*_tmp7F8;void*_tmp7F9;struct Cyc_Absyn_Kind*_tmp7FB;
void*_tmp7FC;struct Cyc_Absyn_Kind*_tmp7FE;void*_tmp7FF;struct Cyc_Core_Opt*
_tmp801;struct Cyc_Core_Opt**_tmp802;struct Cyc_Absyn_Kind*_tmp803;void*_tmp804;
struct Cyc_Core_Opt*_tmp806;struct Cyc_Core_Opt**_tmp807;struct Cyc_Absyn_Kind*
_tmp808;void*_tmp809;struct Cyc_Core_Opt*_tmp80B;struct Cyc_Core_Opt**_tmp80C;
struct Cyc_Absyn_Kind*_tmp80D;_LL447: _tmp7E6=_tmp7E5.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7E7=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7E6;if(_tmp7E7->tag != 0)goto _LL449;
else{_tmp7E8=_tmp7E7->f1;}};_tmp7E9=_tmp7E5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7EA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7E9;if(_tmp7EA->tag != 0)goto _LL449;
else{_tmp7EB=_tmp7EA->f1;}};_LL448: return _tmp7E8 == _tmp7EB;_LL449: _tmp7EC=
_tmp7E5.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp7ED=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7EC;if(_tmp7ED->tag != 1)goto _LL44B;else{_tmp7EE=_tmp7ED->f1;_tmp7EF=(struct
Cyc_Core_Opt**)& _tmp7ED->f1;}};_LL44A:{struct Cyc_Core_Opt*_tmp111C;*_tmp7EF=((
_tmp111C=_cycalloc(sizeof(*_tmp111C)),((_tmp111C->v=c1,_tmp111C))));}return 1;
_LL44B: _tmp7F0=_tmp7E5.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp7F1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7F0;if(_tmp7F1->tag != 1)goto _LL44D;else{_tmp7F2=_tmp7F1->f1;_tmp7F3=(struct
Cyc_Core_Opt**)& _tmp7F1->f1;}};_LL44C:{struct Cyc_Core_Opt*_tmp111D;*_tmp7F3=((
_tmp111D=_cycalloc(sizeof(*_tmp111D)),((_tmp111D->v=c2,_tmp111D))));}return 1;
_LL44D: _tmp7F4=_tmp7E5.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp7F5=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7F4;if(_tmp7F5->tag != 2)goto _LL44F;else{_tmp7F6=_tmp7F5->f1;_tmp7F7=(struct
Cyc_Core_Opt**)& _tmp7F5->f1;_tmp7F8=_tmp7F5->f2;}};_tmp7F9=_tmp7E5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7FA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F9;if(_tmp7FA->tag != 0)goto _LL44F;
else{_tmp7FB=_tmp7FA->f1;}};_LL44E: if(Cyc_Tcutil_kind_leq(_tmp7FB,_tmp7F8)){{
struct Cyc_Core_Opt*_tmp111E;*_tmp7F7=((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((
_tmp111E->v=c2,_tmp111E))));}return 1;}else{return 0;}_LL44F: _tmp7FC=_tmp7E5.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp7FD=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7FC;if(
_tmp7FD->tag != 0)goto _LL451;else{_tmp7FE=_tmp7FD->f1;}};_tmp7FF=_tmp7E5.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp800=(struct Cyc_Absyn_Less_kb_struct*)_tmp7FF;
if(_tmp800->tag != 2)goto _LL451;else{_tmp801=_tmp800->f1;_tmp802=(struct Cyc_Core_Opt**)&
_tmp800->f1;_tmp803=_tmp800->f2;}};_LL450: if(Cyc_Tcutil_kind_leq(_tmp7FE,_tmp803)){{
struct Cyc_Core_Opt*_tmp111F;*_tmp802=((_tmp111F=_cycalloc(sizeof(*_tmp111F)),((
_tmp111F->v=c1,_tmp111F))));}return 1;}else{return 0;}_LL451: _tmp804=_tmp7E5.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp805=(struct Cyc_Absyn_Less_kb_struct*)_tmp804;
if(_tmp805->tag != 2)goto _LL446;else{_tmp806=_tmp805->f1;_tmp807=(struct Cyc_Core_Opt**)&
_tmp805->f1;_tmp808=_tmp805->f2;}};_tmp809=_tmp7E5.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp80A=(struct Cyc_Absyn_Less_kb_struct*)_tmp809;if(_tmp80A->tag != 2)goto _LL446;
else{_tmp80B=_tmp80A->f1;_tmp80C=(struct Cyc_Core_Opt**)& _tmp80A->f1;_tmp80D=
_tmp80A->f2;}};_LL452: if(Cyc_Tcutil_kind_leq(_tmp808,_tmp80D)){{struct Cyc_Core_Opt*
_tmp1120;*_tmp80C=((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120->v=c1,
_tmp1120))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp80D,_tmp808)){{struct Cyc_Core_Opt*
_tmp1121;*_tmp807=((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121->v=c2,
_tmp1121))));}return 1;}else{return 0;}}_LL446:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp1125;void*_tmp1124[1];struct Cyc_Int_pa_struct _tmp1123;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1123.tag=1,((_tmp1123.f1=(
unsigned long)i,((_tmp1124[0]=& _tmp1123,Cyc_aprintf(((_tmp1125="#%d",
_tag_dyneither(_tmp1125,sizeof(char),4))),_tag_dyneither(_tmp1124,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp1128;struct Cyc_Absyn_Tvar*_tmp1127;return(
_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127->name=((_tmp1128=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp1128[0]=s,_tmp1128)))),((_tmp1127->identity=
- 1,((_tmp1127->kind=k,_tmp1127)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp819=*t->name;return*((const char*)_check_dyneither_subscript(_tmp819,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp112C;void*
_tmp112B[1];struct Cyc_String_pa_struct _tmp112A;(_tmp112A.tag=0,((_tmp112A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp112B[0]=& _tmp112A,
Cyc_printf(((_tmp112C="%s",_tag_dyneither(_tmp112C,sizeof(char),3))),
_tag_dyneither(_tmp112B,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp112D;struct _dyneither_ptr _tmp81D=Cyc_strconcat(((_tmp112D="`",
_tag_dyneither(_tmp112D,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp1130;char _tmp112F;struct _dyneither_ptr _tmp112E;(_tmp112E=_dyneither_ptr_plus(
_tmp81D,sizeof(char),1),((_tmp112F=*((char*)_check_dyneither_subscript(_tmp112E,
sizeof(char),0)),((_tmp1130='t',((_get_dyneither_size(_tmp112E,sizeof(char))== 1
 && (_tmp112F == '\000'  && _tmp1130 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp112E.curr)=_tmp1130)))))));}{struct _dyneither_ptr*_tmp1131;t->name=((
_tmp1131=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1131[0]=(struct
_dyneither_ptr)_tmp81D,_tmp1131))));};};}struct _tuple21{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple21*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple21*x){
struct Cyc_Core_Opt*_tmp1134;struct _tuple9*_tmp1133;return(_tmp1133=_cycalloc(
sizeof(*_tmp1133)),((_tmp1133->f1=(struct Cyc_Core_Opt*)((_tmp1134=_cycalloc(
sizeof(*_tmp1134)),((_tmp1134->v=(*x).f1,_tmp1134)))),((_tmp1133->f2=(*x).f2,((
_tmp1133->f3=(*x).f3,_tmp1133)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp825=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp1135;_tmp825=((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135->hd=(void*)
atts->hd,((_tmp1135->tl=_tmp825,_tmp1135))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp113B;struct Cyc_Absyn_FnInfo _tmp113A;struct Cyc_Absyn_FnType_struct*_tmp1139;
return(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp113B.tag=
10,((_tmp113B.f1=((_tmp113A.tvars=fd->tvs,((_tmp113A.effect=fd->effect,((
_tmp113A.ret_typ=fd->ret_type,((_tmp113A.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp113A.c_varargs=fd->c_varargs,((_tmp113A.cyc_varargs=fd->cyc_varargs,((
_tmp113A.rgn_po=fd->rgn_po,((_tmp113A.attributes=_tmp825,_tmp113A)))))))))))))))),
_tmp113B)))),_tmp1139))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple22{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple22*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple22*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp113C;return(_tmp113C=_cycalloc(sizeof(*_tmp113C)),((_tmp113C->f1=(*
pr).f1,((_tmp113C->f2=t,_tmp113C)))));}struct _tuple23{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple24{struct _tuple23*f1;void*f2;};static
struct _tuple24*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple24*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple23*_tmp113F;struct _tuple24*_tmp113E;return(_tmp113E=
_region_malloc(rgn,sizeof(*_tmp113E)),((_tmp113E->f1=((_tmp113F=_region_malloc(
rgn,sizeof(*_tmp113F)),((_tmp113F->f1=(*y).f1,((_tmp113F->f2=(*y).f2,_tmp113F)))))),((
_tmp113E->f2=(*y).f3,_tmp113E)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple24*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple24*w){
struct _tuple23*_tmp82E;void*_tmp82F;struct _tuple24 _tmp82D=*w;_tmp82E=_tmp82D.f1;
_tmp82F=_tmp82D.f2;{struct Cyc_Core_Opt*_tmp831;struct Cyc_Absyn_Tqual _tmp832;
struct _tuple23 _tmp830=*_tmp82E;_tmp831=_tmp830.f1;_tmp832=_tmp830.f2;{struct
_tuple9*_tmp1140;return(_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140->f1=
_tmp831,((_tmp1140->f2=_tmp832,((_tmp1140->f3=_tmp82F,_tmp1140)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1141;return(
_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141->name=f->name,((_tmp1141->tq=f->tq,((
_tmp1141->type=t,((_tmp1141->width=f->width,((_tmp1141->attributes=f->attributes,
_tmp1141)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1142;
return(_tmp1142=_cycalloc(sizeof(*_tmp1142)),((_tmp1142->topt=old->topt,((
_tmp1142->r=r,((_tmp1142->loc=old->loc,((_tmp1142->annot=old->annot,_tmp1142)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp836=e->r;
enum Cyc_Absyn_Primop _tmp83C;struct Cyc_List_List*_tmp83D;struct Cyc_Absyn_Exp*
_tmp83F;struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Exp*
_tmp843;struct Cyc_Absyn_Exp*_tmp844;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*
_tmp847;struct Cyc_Absyn_Exp*_tmp849;struct Cyc_Absyn_Exp*_tmp84A;void*_tmp84C;
struct Cyc_Absyn_Exp*_tmp84D;int _tmp84E;enum Cyc_Absyn_Coercion _tmp84F;void*
_tmp851;struct Cyc_Absyn_Exp*_tmp853;void*_tmp855;void*_tmp856;void*_tmp858;
_LL454: {struct Cyc_Absyn_Const_e_struct*_tmp837=(struct Cyc_Absyn_Const_e_struct*)
_tmp836;if(_tmp837->tag != 0)goto _LL456;}_LL455: goto _LL457;_LL456: {struct Cyc_Absyn_Enum_e_struct*
_tmp838=(struct Cyc_Absyn_Enum_e_struct*)_tmp836;if(_tmp838->tag != 32)goto _LL458;}
_LL457: goto _LL459;_LL458: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp839=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp836;if(_tmp839->tag != 33)goto _LL45A;}_LL459: goto _LL45B;_LL45A: {struct Cyc_Absyn_Var_e_struct*
_tmp83A=(struct Cyc_Absyn_Var_e_struct*)_tmp836;if(_tmp83A->tag != 1)goto _LL45C;}
_LL45B: return e;_LL45C: {struct Cyc_Absyn_Primop_e_struct*_tmp83B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp836;if(_tmp83B->tag != 3)goto _LL45E;else{_tmp83C=_tmp83B->f1;_tmp83D=_tmp83B->f2;}}
_LL45D: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83D)== 1){struct
Cyc_Absyn_Exp*_tmp859=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp83D))->hd;struct Cyc_Absyn_Exp*_tmp85A=Cyc_Tcutil_rsubsexp(r,inst,_tmp859);
if(_tmp85A == _tmp859)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1148;struct Cyc_Absyn_Exp*
_tmp1147[1];struct Cyc_Absyn_Primop_e_struct*_tmp1146;return Cyc_Tcutil_copye(e,(
void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1148.tag=3,((
_tmp1148.f1=_tmp83C,((_tmp1148.f2=((_tmp1147[0]=_tmp85A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1147,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp1148)))))),_tmp1146)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp83D)== 2){struct Cyc_Absyn_Exp*_tmp85E=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp83D))->hd;struct Cyc_Absyn_Exp*_tmp85F=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp83D->tl))->hd;struct
Cyc_Absyn_Exp*_tmp860=Cyc_Tcutil_rsubsexp(r,inst,_tmp85E);struct Cyc_Absyn_Exp*
_tmp861=Cyc_Tcutil_rsubsexp(r,inst,_tmp85F);if(_tmp860 == _tmp85E  && _tmp861 == 
_tmp85F)return e;{struct Cyc_Absyn_Primop_e_struct _tmp114E;struct Cyc_Absyn_Exp*
_tmp114D[2];struct Cyc_Absyn_Primop_e_struct*_tmp114C;return Cyc_Tcutil_copye(e,(
void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114E.tag=3,((
_tmp114E.f1=_tmp83C,((_tmp114E.f2=((_tmp114D[1]=_tmp861,((_tmp114D[0]=_tmp860,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp114D,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp114E)))))),_tmp114C)))));};}
else{const char*_tmp1151;void*_tmp1150;return(_tmp1150=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1151="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1151,sizeof(char),34))),_tag_dyneither(_tmp1150,sizeof(void*),
0)));}}_LL45E: {struct Cyc_Absyn_Conditional_e_struct*_tmp83E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp836;if(_tmp83E->tag != 6)goto _LL460;else{_tmp83F=_tmp83E->f1;_tmp840=_tmp83E->f2;
_tmp841=_tmp83E->f3;}}_LL45F: {struct Cyc_Absyn_Exp*_tmp867=Cyc_Tcutil_rsubsexp(r,
inst,_tmp83F);struct Cyc_Absyn_Exp*_tmp868=Cyc_Tcutil_rsubsexp(r,inst,_tmp840);
struct Cyc_Absyn_Exp*_tmp869=Cyc_Tcutil_rsubsexp(r,inst,_tmp841);if((_tmp867 == 
_tmp83F  && _tmp868 == _tmp840) && _tmp869 == _tmp841)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1154;struct Cyc_Absyn_Conditional_e_struct*_tmp1153;return Cyc_Tcutil_copye(e,(
void*)((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153[0]=((_tmp1154.tag=6,((
_tmp1154.f1=_tmp867,((_tmp1154.f2=_tmp868,((_tmp1154.f3=_tmp869,_tmp1154)))))))),
_tmp1153)))));};}_LL460: {struct Cyc_Absyn_And_e_struct*_tmp842=(struct Cyc_Absyn_And_e_struct*)
_tmp836;if(_tmp842->tag != 7)goto _LL462;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;}}
_LL461: {struct Cyc_Absyn_Exp*_tmp86C=Cyc_Tcutil_rsubsexp(r,inst,_tmp843);struct
Cyc_Absyn_Exp*_tmp86D=Cyc_Tcutil_rsubsexp(r,inst,_tmp844);if(_tmp86C == _tmp843
 && _tmp86D == _tmp844)return e;{struct Cyc_Absyn_And_e_struct _tmp1157;struct Cyc_Absyn_And_e_struct*
_tmp1156;return Cyc_Tcutil_copye(e,(void*)((_tmp1156=_cycalloc(sizeof(*_tmp1156)),((
_tmp1156[0]=((_tmp1157.tag=7,((_tmp1157.f1=_tmp86C,((_tmp1157.f2=_tmp86D,
_tmp1157)))))),_tmp1156)))));};}_LL462: {struct Cyc_Absyn_Or_e_struct*_tmp845=(
struct Cyc_Absyn_Or_e_struct*)_tmp836;if(_tmp845->tag != 8)goto _LL464;else{_tmp846=
_tmp845->f1;_tmp847=_tmp845->f2;}}_LL463: {struct Cyc_Absyn_Exp*_tmp870=Cyc_Tcutil_rsubsexp(
r,inst,_tmp846);struct Cyc_Absyn_Exp*_tmp871=Cyc_Tcutil_rsubsexp(r,inst,_tmp847);
if(_tmp870 == _tmp846  && _tmp871 == _tmp847)return e;{struct Cyc_Absyn_Or_e_struct
_tmp115A;struct Cyc_Absyn_Or_e_struct*_tmp1159;return Cyc_Tcutil_copye(e,(void*)((
_tmp1159=_cycalloc(sizeof(*_tmp1159)),((_tmp1159[0]=((_tmp115A.tag=8,((_tmp115A.f1=
_tmp870,((_tmp115A.f2=_tmp871,_tmp115A)))))),_tmp1159)))));};}_LL464: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp848=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp836;if(_tmp848->tag != 9)goto _LL466;
else{_tmp849=_tmp848->f1;_tmp84A=_tmp848->f2;}}_LL465: {struct Cyc_Absyn_Exp*
_tmp874=Cyc_Tcutil_rsubsexp(r,inst,_tmp849);struct Cyc_Absyn_Exp*_tmp875=Cyc_Tcutil_rsubsexp(
r,inst,_tmp84A);if(_tmp874 == _tmp849  && _tmp875 == _tmp84A)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp115D;struct Cyc_Absyn_SeqExp_e_struct*_tmp115C;return Cyc_Tcutil_copye(e,(void*)((
_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C[0]=((_tmp115D.tag=9,((_tmp115D.f1=
_tmp874,((_tmp115D.f2=_tmp875,_tmp115D)))))),_tmp115C)))));};}_LL466: {struct Cyc_Absyn_Cast_e_struct*
_tmp84B=(struct Cyc_Absyn_Cast_e_struct*)_tmp836;if(_tmp84B->tag != 15)goto _LL468;
else{_tmp84C=(void*)_tmp84B->f1;_tmp84D=_tmp84B->f2;_tmp84E=_tmp84B->f3;_tmp84F=
_tmp84B->f4;}}_LL467: {struct Cyc_Absyn_Exp*_tmp878=Cyc_Tcutil_rsubsexp(r,inst,
_tmp84D);void*_tmp879=Cyc_Tcutil_rsubstitute(r,inst,_tmp84C);if(_tmp878 == 
_tmp84D  && _tmp879 == _tmp84C)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1160;
struct Cyc_Absyn_Cast_e_struct*_tmp115F;return Cyc_Tcutil_copye(e,(void*)((
_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F[0]=((_tmp1160.tag=15,((_tmp1160.f1=(
void*)_tmp879,((_tmp1160.f2=_tmp878,((_tmp1160.f3=_tmp84E,((_tmp1160.f4=_tmp84F,
_tmp1160)))))))))),_tmp115F)))));};}_LL468: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp850=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp836;if(_tmp850->tag != 18)goto
_LL46A;else{_tmp851=(void*)_tmp850->f1;}}_LL469: {void*_tmp87C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp851);if(_tmp87C == _tmp851)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp1163;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1162;return Cyc_Tcutil_copye(e,(
void*)((_tmp1162=_cycalloc(sizeof(*_tmp1162)),((_tmp1162[0]=((_tmp1163.tag=18,((
_tmp1163.f1=(void*)_tmp87C,_tmp1163)))),_tmp1162)))));};}_LL46A: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp852=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp836;if(_tmp852->tag != 19)goto
_LL46C;else{_tmp853=_tmp852->f1;}}_LL46B: {struct Cyc_Absyn_Exp*_tmp87F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp853);if(_tmp87F == _tmp853)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp1166;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1165;return Cyc_Tcutil_copye(e,(
void*)((_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165[0]=((_tmp1166.tag=19,((
_tmp1166.f1=_tmp87F,_tmp1166)))),_tmp1165)))));};}_LL46C: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp854=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp836;if(_tmp854->tag != 20)goto
_LL46E;else{_tmp855=(void*)_tmp854->f1;_tmp856=(void*)_tmp854->f2;}}_LL46D: {
void*_tmp882=Cyc_Tcutil_rsubstitute(r,inst,_tmp855);if(_tmp882 == _tmp855)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp1169;struct Cyc_Absyn_Offsetof_e_struct*
_tmp1168;return Cyc_Tcutil_copye(e,(void*)((_tmp1168=_cycalloc(sizeof(*_tmp1168)),((
_tmp1168[0]=((_tmp1169.tag=20,((_tmp1169.f1=(void*)_tmp882,((_tmp1169.f2=(void*)
_tmp856,_tmp1169)))))),_tmp1168)))));};}_LL46E: {struct Cyc_Absyn_Valueof_e_struct*
_tmp857=(struct Cyc_Absyn_Valueof_e_struct*)_tmp836;if(_tmp857->tag != 39)goto
_LL470;else{_tmp858=(void*)_tmp857->f1;}}_LL46F: {void*_tmp885=Cyc_Tcutil_rsubstitute(
r,inst,_tmp858);if(_tmp885 == _tmp858)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp116C;struct Cyc_Absyn_Valueof_e_struct*_tmp116B;return Cyc_Tcutil_copye(e,(
void*)((_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B[0]=((_tmp116C.tag=39,((
_tmp116C.f1=(void*)_tmp885,_tmp116C)))),_tmp116B)))));};}_LL470:;_LL471: {const
char*_tmp116F;void*_tmp116E;return(_tmp116E=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp116F="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp116F,sizeof(char),46))),_tag_dyneither(_tmp116E,sizeof(void*),
0)));}_LL453:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp88A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp88C;
struct Cyc_Absyn_AggrInfo _tmp88E;union Cyc_Absyn_AggrInfoU _tmp88F;struct Cyc_List_List*
_tmp890;struct Cyc_Absyn_DatatypeInfo _tmp892;union Cyc_Absyn_DatatypeInfoU _tmp893;
struct Cyc_List_List*_tmp894;struct Cyc_Absyn_DatatypeFieldInfo _tmp896;union Cyc_Absyn_DatatypeFieldInfoU
_tmp897;struct Cyc_List_List*_tmp898;struct _tuple2*_tmp89A;struct Cyc_List_List*
_tmp89B;struct Cyc_Absyn_Typedefdecl*_tmp89C;void**_tmp89D;struct Cyc_Absyn_ArrayInfo
_tmp89F;void*_tmp8A0;struct Cyc_Absyn_Tqual _tmp8A1;struct Cyc_Absyn_Exp*_tmp8A2;
union Cyc_Absyn_Constraint*_tmp8A3;struct Cyc_Position_Segment*_tmp8A4;struct Cyc_Absyn_PtrInfo
_tmp8A6;void*_tmp8A7;struct Cyc_Absyn_Tqual _tmp8A8;struct Cyc_Absyn_PtrAtts _tmp8A9;
void*_tmp8AA;union Cyc_Absyn_Constraint*_tmp8AB;union Cyc_Absyn_Constraint*_tmp8AC;
union Cyc_Absyn_Constraint*_tmp8AD;struct Cyc_Absyn_FnInfo _tmp8AF;struct Cyc_List_List*
_tmp8B0;struct Cyc_Core_Opt*_tmp8B1;void*_tmp8B2;struct Cyc_List_List*_tmp8B3;int
_tmp8B4;struct Cyc_Absyn_VarargInfo*_tmp8B5;struct Cyc_List_List*_tmp8B6;struct Cyc_List_List*
_tmp8B7;struct Cyc_List_List*_tmp8B9;enum Cyc_Absyn_AggrKind _tmp8BB;struct Cyc_List_List*
_tmp8BC;struct Cyc_Core_Opt*_tmp8BE;void*_tmp8C0;void*_tmp8C2;void*_tmp8C3;void*
_tmp8C5;struct Cyc_Absyn_Exp*_tmp8C7;void*_tmp8D1;void*_tmp8D3;struct Cyc_List_List*
_tmp8D5;_LL473: {struct Cyc_Absyn_VarType_struct*_tmp88B=(struct Cyc_Absyn_VarType_struct*)
_tmp88A;if(_tmp88B->tag != 2)goto _LL475;else{_tmp88C=_tmp88B->f1;}}_LL474: {
struct _handler_cons _tmp8D6;_push_handler(& _tmp8D6);{int _tmp8D8=0;if(setjmp(
_tmp8D6.handler))_tmp8D8=1;if(!_tmp8D8){{void*_tmp8D9=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp88C);_npop_handler(0);return
_tmp8D9;};_pop_handler();}else{void*_tmp8D7=(void*)_exn_thrown;void*_tmp8DB=
_tmp8D7;_LL4A8: {struct Cyc_Core_Not_found_struct*_tmp8DC=(struct Cyc_Core_Not_found_struct*)
_tmp8DB;if(_tmp8DC->tag != Cyc_Core_Not_found)goto _LL4AA;}_LL4A9: return t;_LL4AA:;
_LL4AB:(void)_throw(_tmp8DB);_LL4A7:;}};}_LL475: {struct Cyc_Absyn_AggrType_struct*
_tmp88D=(struct Cyc_Absyn_AggrType_struct*)_tmp88A;if(_tmp88D->tag != 12)goto
_LL477;else{_tmp88E=_tmp88D->f1;_tmp88F=_tmp88E.aggr_info;_tmp890=_tmp88E.targs;}}
_LL476: {struct Cyc_List_List*_tmp8DD=Cyc_Tcutil_substs(rgn,inst,_tmp890);struct
Cyc_Absyn_AggrType_struct _tmp1175;struct Cyc_Absyn_AggrInfo _tmp1174;struct Cyc_Absyn_AggrType_struct*
_tmp1173;return _tmp8DD == _tmp890?t:(void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((
_tmp1173[0]=((_tmp1175.tag=12,((_tmp1175.f1=((_tmp1174.aggr_info=_tmp88F,((
_tmp1174.targs=_tmp8DD,_tmp1174)))),_tmp1175)))),_tmp1173))));}_LL477: {struct
Cyc_Absyn_DatatypeType_struct*_tmp891=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp88A;if(_tmp891->tag != 3)goto _LL479;else{_tmp892=_tmp891->f1;_tmp893=_tmp892.datatype_info;
_tmp894=_tmp892.targs;}}_LL478: {struct Cyc_List_List*_tmp8E1=Cyc_Tcutil_substs(
rgn,inst,_tmp894);struct Cyc_Absyn_DatatypeType_struct _tmp117B;struct Cyc_Absyn_DatatypeInfo
_tmp117A;struct Cyc_Absyn_DatatypeType_struct*_tmp1179;return _tmp8E1 == _tmp894?t:(
void*)((_tmp1179=_cycalloc(sizeof(*_tmp1179)),((_tmp1179[0]=((_tmp117B.tag=3,((
_tmp117B.f1=((_tmp117A.datatype_info=_tmp893,((_tmp117A.targs=_tmp8E1,_tmp117A)))),
_tmp117B)))),_tmp1179))));}_LL479: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp895=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp88A;if(_tmp895->tag != 4)
goto _LL47B;else{_tmp896=_tmp895->f1;_tmp897=_tmp896.field_info;_tmp898=_tmp896.targs;}}
_LL47A: {struct Cyc_List_List*_tmp8E5=Cyc_Tcutil_substs(rgn,inst,_tmp898);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp1181;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1180;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp117F;return _tmp8E5 == 
_tmp898?t:(void*)((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((_tmp117F[0]=((
_tmp1181.tag=4,((_tmp1181.f1=((_tmp1180.field_info=_tmp897,((_tmp1180.targs=
_tmp8E5,_tmp1180)))),_tmp1181)))),_tmp117F))));}_LL47B: {struct Cyc_Absyn_TypedefType_struct*
_tmp899=(struct Cyc_Absyn_TypedefType_struct*)_tmp88A;if(_tmp899->tag != 18)goto
_LL47D;else{_tmp89A=_tmp899->f1;_tmp89B=_tmp899->f2;_tmp89C=_tmp899->f3;_tmp89D=
_tmp899->f4;}}_LL47C: {struct Cyc_List_List*_tmp8E9=Cyc_Tcutil_substs(rgn,inst,
_tmp89B);struct Cyc_Absyn_TypedefType_struct _tmp1184;struct Cyc_Absyn_TypedefType_struct*
_tmp1183;return _tmp8E9 == _tmp89B?t:(void*)((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((
_tmp1183[0]=((_tmp1184.tag=18,((_tmp1184.f1=_tmp89A,((_tmp1184.f2=_tmp8E9,((
_tmp1184.f3=_tmp89C,((_tmp1184.f4=_tmp89D,_tmp1184)))))))))),_tmp1183))));}
_LL47D: {struct Cyc_Absyn_ArrayType_struct*_tmp89E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp88A;if(_tmp89E->tag != 9)goto _LL47F;else{_tmp89F=_tmp89E->f1;_tmp8A0=_tmp89F.elt_type;
_tmp8A1=_tmp89F.tq;_tmp8A2=_tmp89F.num_elts;_tmp8A3=_tmp89F.zero_term;_tmp8A4=
_tmp89F.zt_loc;}}_LL47E: {void*_tmp8EC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A0);
struct Cyc_Absyn_ArrayType_struct _tmp118A;struct Cyc_Absyn_ArrayInfo _tmp1189;
struct Cyc_Absyn_ArrayType_struct*_tmp1188;return _tmp8EC == _tmp8A0?t:(void*)((
_tmp1188=_cycalloc(sizeof(*_tmp1188)),((_tmp1188[0]=((_tmp118A.tag=9,((_tmp118A.f1=((
_tmp1189.elt_type=_tmp8EC,((_tmp1189.tq=_tmp8A1,((_tmp1189.num_elts=_tmp8A2,((
_tmp1189.zero_term=_tmp8A3,((_tmp1189.zt_loc=_tmp8A4,_tmp1189)))))))))),_tmp118A)))),
_tmp1188))));}_LL47F: {struct Cyc_Absyn_PointerType_struct*_tmp8A5=(struct Cyc_Absyn_PointerType_struct*)
_tmp88A;if(_tmp8A5->tag != 5)goto _LL481;else{_tmp8A6=_tmp8A5->f1;_tmp8A7=_tmp8A6.elt_typ;
_tmp8A8=_tmp8A6.elt_tq;_tmp8A9=_tmp8A6.ptr_atts;_tmp8AA=_tmp8A9.rgn;_tmp8AB=
_tmp8A9.nullable;_tmp8AC=_tmp8A9.bounds;_tmp8AD=_tmp8A9.zero_term;}}_LL480: {
void*_tmp8F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A7);void*_tmp8F1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8AA);union Cyc_Absyn_Constraint*_tmp8F2=_tmp8AC;{void*_tmp8F3=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp8AC);struct Cyc_Absyn_Exp*_tmp8F5;_LL4AD: {struct Cyc_Absyn_Upper_b_struct*
_tmp8F4=(struct Cyc_Absyn_Upper_b_struct*)_tmp8F3;if(_tmp8F4->tag != 1)goto _LL4AF;
else{_tmp8F5=_tmp8F4->f1;}}_LL4AE: {struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp8F5);if(_tmp8F6 != _tmp8F5){struct Cyc_Absyn_Upper_b_struct _tmp118D;
struct Cyc_Absyn_Upper_b_struct*_tmp118C;_tmp8F2=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp118C=_cycalloc(sizeof(*_tmp118C)),((
_tmp118C[0]=((_tmp118D.tag=1,((_tmp118D.f1=_tmp8F6,_tmp118D)))),_tmp118C)))));}
goto _LL4AC;}_LL4AF:;_LL4B0: goto _LL4AC;_LL4AC:;}if((_tmp8F0 == _tmp8A7  && _tmp8F1
== _tmp8AA) && _tmp8F2 == _tmp8AC)return t;{struct Cyc_Absyn_PointerType_struct
_tmp1197;struct Cyc_Absyn_PtrAtts _tmp1196;struct Cyc_Absyn_PtrInfo _tmp1195;struct
Cyc_Absyn_PointerType_struct*_tmp1194;return(void*)((_tmp1194=_cycalloc(sizeof(*
_tmp1194)),((_tmp1194[0]=((_tmp1197.tag=5,((_tmp1197.f1=((_tmp1195.elt_typ=
_tmp8F0,((_tmp1195.elt_tq=_tmp8A8,((_tmp1195.ptr_atts=((_tmp1196.rgn=_tmp8F1,((
_tmp1196.nullable=_tmp8AB,((_tmp1196.bounds=_tmp8F2,((_tmp1196.zero_term=_tmp8AD,((
_tmp1196.ptrloc=0,_tmp1196)))))))))),_tmp1195)))))),_tmp1197)))),_tmp1194))));};}
_LL481: {struct Cyc_Absyn_FnType_struct*_tmp8AE=(struct Cyc_Absyn_FnType_struct*)
_tmp88A;if(_tmp8AE->tag != 10)goto _LL483;else{_tmp8AF=_tmp8AE->f1;_tmp8B0=_tmp8AF.tvars;
_tmp8B1=_tmp8AF.effect;_tmp8B2=_tmp8AF.ret_typ;_tmp8B3=_tmp8AF.args;_tmp8B4=
_tmp8AF.c_varargs;_tmp8B5=_tmp8AF.cyc_varargs;_tmp8B6=_tmp8AF.rgn_po;_tmp8B7=
_tmp8AF.attributes;}}_LL482:{struct Cyc_List_List*_tmp8FD=_tmp8B0;for(0;_tmp8FD != 
0;_tmp8FD=_tmp8FD->tl){struct _tuple14*_tmp11A1;struct Cyc_Absyn_VarType_struct
_tmp11A0;struct Cyc_Absyn_VarType_struct*_tmp119F;struct Cyc_List_List*_tmp119E;
inst=((_tmp119E=_region_malloc(rgn,sizeof(*_tmp119E)),((_tmp119E->hd=((_tmp11A1=
_region_malloc(rgn,sizeof(*_tmp11A1)),((_tmp11A1->f1=(struct Cyc_Absyn_Tvar*)
_tmp8FD->hd,((_tmp11A1->f2=(void*)((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((
_tmp119F[0]=((_tmp11A0.tag=2,((_tmp11A0.f1=(struct Cyc_Absyn_Tvar*)_tmp8FD->hd,
_tmp11A0)))),_tmp119F)))),_tmp11A1)))))),((_tmp119E->tl=inst,_tmp119E))))));}}{
struct Cyc_List_List*_tmp903;struct Cyc_List_List*_tmp904;struct _tuple1 _tmp902=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple24*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8B3));
_tmp903=_tmp902.f1;_tmp904=_tmp902.f2;{struct Cyc_List_List*_tmp905=Cyc_Tcutil_substs(
rgn,inst,_tmp904);struct Cyc_List_List*_tmp906=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple24*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp903,_tmp905));struct Cyc_Core_Opt*
eff2;if(_tmp8B1 == 0)eff2=0;else{void*_tmp907=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp8B1->v);if(_tmp907 == (void*)_tmp8B1->v)eff2=_tmp8B1;else{struct Cyc_Core_Opt*
_tmp11A2;eff2=((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((_tmp11A2->v=_tmp907,
_tmp11A2))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp8B5 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp90A;struct Cyc_Absyn_Tqual _tmp90B;void*
_tmp90C;int _tmp90D;struct Cyc_Absyn_VarargInfo _tmp909=*_tmp8B5;_tmp90A=_tmp909.name;
_tmp90B=_tmp909.tq;_tmp90C=_tmp909.type;_tmp90D=_tmp909.inject;{void*_tmp90E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp90C);if(_tmp90E == _tmp90C)cyc_varargs2=_tmp8B5;else{struct Cyc_Absyn_VarargInfo*
_tmp11A3;cyc_varargs2=((_tmp11A3=_cycalloc(sizeof(*_tmp11A3)),((_tmp11A3->name=
_tmp90A,((_tmp11A3->tq=_tmp90B,((_tmp11A3->type=_tmp90E,((_tmp11A3->inject=
_tmp90D,_tmp11A3))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp911;struct Cyc_List_List*_tmp912;struct _tuple1 _tmp910=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp8B6);_tmp911=_tmp910.f1;_tmp912=_tmp910.f2;{struct Cyc_List_List*
_tmp913=Cyc_Tcutil_substs(rgn,inst,_tmp911);struct Cyc_List_List*_tmp914=Cyc_Tcutil_substs(
rgn,inst,_tmp912);if(_tmp913 == _tmp911  && _tmp914 == _tmp912)rgn_po2=_tmp8B6;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp913,_tmp914);}{struct Cyc_Absyn_FnType_struct _tmp11A9;struct
Cyc_Absyn_FnInfo _tmp11A8;struct Cyc_Absyn_FnType_struct*_tmp11A7;return(void*)((
_tmp11A7=_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A9.tag=10,((_tmp11A9.f1=((
_tmp11A8.tvars=_tmp8B0,((_tmp11A8.effect=eff2,((_tmp11A8.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8B2),((_tmp11A8.args=_tmp906,((_tmp11A8.c_varargs=_tmp8B4,((
_tmp11A8.cyc_varargs=cyc_varargs2,((_tmp11A8.rgn_po=rgn_po2,((_tmp11A8.attributes=
_tmp8B7,_tmp11A8)))))))))))))))),_tmp11A9)))),_tmp11A7))));};};};};};};_LL483: {
struct Cyc_Absyn_TupleType_struct*_tmp8B8=(struct Cyc_Absyn_TupleType_struct*)
_tmp88A;if(_tmp8B8->tag != 11)goto _LL485;else{_tmp8B9=_tmp8B8->f1;}}_LL484: {
struct Cyc_List_List*_tmp918=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp8B9);struct Cyc_List_List*_tmp919=Cyc_Tcutil_substs(rgn,inst,_tmp918);if(
_tmp919 == _tmp918)return t;{struct Cyc_List_List*_tmp91A=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8B9,_tmp919);struct Cyc_Absyn_TupleType_struct
_tmp11AC;struct Cyc_Absyn_TupleType_struct*_tmp11AB;return(void*)((_tmp11AB=
_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=11,((_tmp11AC.f1=
_tmp91A,_tmp11AC)))),_tmp11AB))));};}_LL485: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp8BA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp88A;if(_tmp8BA->tag != 13)goto
_LL487;else{_tmp8BB=_tmp8BA->f1;_tmp8BC=_tmp8BA->f2;}}_LL486: {struct Cyc_List_List*
_tmp91D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp8BC);struct
Cyc_List_List*_tmp91E=Cyc_Tcutil_substs(rgn,inst,_tmp91D);if(_tmp91E == _tmp91D)
return t;{struct Cyc_List_List*_tmp91F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp8BC,_tmp91E);struct Cyc_Absyn_AnonAggrType_struct
_tmp11AF;struct Cyc_Absyn_AnonAggrType_struct*_tmp11AE;return(void*)((_tmp11AE=
_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((_tmp11AF.tag=13,((_tmp11AF.f1=
_tmp8BB,((_tmp11AF.f2=_tmp91F,_tmp11AF)))))),_tmp11AE))));};}_LL487: {struct Cyc_Absyn_Evar_struct*
_tmp8BD=(struct Cyc_Absyn_Evar_struct*)_tmp88A;if(_tmp8BD->tag != 1)goto _LL489;
else{_tmp8BE=_tmp8BD->f2;}}_LL488: if(_tmp8BE != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp8BE->v);else{return t;}_LL489: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8BF=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp88A;if(_tmp8BF->tag != 16)goto
_LL48B;else{_tmp8C0=(void*)_tmp8BF->f1;}}_LL48A: {void*_tmp922=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8C0);struct Cyc_Absyn_RgnHandleType_struct _tmp11B2;struct Cyc_Absyn_RgnHandleType_struct*
_tmp11B1;return _tmp922 == _tmp8C0?t:(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((
_tmp11B1[0]=((_tmp11B2.tag=16,((_tmp11B2.f1=(void*)_tmp922,_tmp11B2)))),_tmp11B1))));}
_LL48B: {struct Cyc_Absyn_DynRgnType_struct*_tmp8C1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp88A;if(_tmp8C1->tag != 17)goto _LL48D;else{_tmp8C2=(void*)_tmp8C1->f1;_tmp8C3=(
void*)_tmp8C1->f2;}}_LL48C: {void*_tmp925=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp8C2);void*_tmp926=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C3);struct Cyc_Absyn_DynRgnType_struct
_tmp11B5;struct Cyc_Absyn_DynRgnType_struct*_tmp11B4;return _tmp925 == _tmp8C2  && 
_tmp926 == _tmp8C3?t:(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((
_tmp11B5.tag=17,((_tmp11B5.f1=(void*)_tmp925,((_tmp11B5.f2=(void*)_tmp926,
_tmp11B5)))))),_tmp11B4))));}_LL48D: {struct Cyc_Absyn_TagType_struct*_tmp8C4=(
struct Cyc_Absyn_TagType_struct*)_tmp88A;if(_tmp8C4->tag != 20)goto _LL48F;else{
_tmp8C5=(void*)_tmp8C4->f1;}}_LL48E: {void*_tmp929=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp8C5);struct Cyc_Absyn_TagType_struct _tmp11B8;struct Cyc_Absyn_TagType_struct*
_tmp11B7;return _tmp929 == _tmp8C5?t:(void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((
_tmp11B7[0]=((_tmp11B8.tag=20,((_tmp11B8.f1=(void*)_tmp929,_tmp11B8)))),_tmp11B7))));}
_LL48F: {struct Cyc_Absyn_ValueofType_struct*_tmp8C6=(struct Cyc_Absyn_ValueofType_struct*)
_tmp88A;if(_tmp8C6->tag != 19)goto _LL491;else{_tmp8C7=_tmp8C6->f1;}}_LL490: {
struct Cyc_Absyn_Exp*_tmp92C=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8C7);struct Cyc_Absyn_ValueofType_struct
_tmp11BB;struct Cyc_Absyn_ValueofType_struct*_tmp11BA;return _tmp92C == _tmp8C7?t:(
void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=19,((
_tmp11BB.f1=_tmp92C,_tmp11BB)))),_tmp11BA))));}_LL491: {struct Cyc_Absyn_EnumType_struct*
_tmp8C8=(struct Cyc_Absyn_EnumType_struct*)_tmp88A;if(_tmp8C8->tag != 14)goto
_LL493;}_LL492: goto _LL494;_LL493: {struct Cyc_Absyn_AnonEnumType_struct*_tmp8C9=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp88A;if(_tmp8C9->tag != 15)goto _LL495;}
_LL494: goto _LL496;_LL495: {struct Cyc_Absyn_VoidType_struct*_tmp8CA=(struct Cyc_Absyn_VoidType_struct*)
_tmp88A;if(_tmp8CA->tag != 0)goto _LL497;}_LL496: goto _LL498;_LL497: {struct Cyc_Absyn_IntType_struct*
_tmp8CB=(struct Cyc_Absyn_IntType_struct*)_tmp88A;if(_tmp8CB->tag != 6)goto _LL499;}
_LL498: goto _LL49A;_LL499: {struct Cyc_Absyn_FloatType_struct*_tmp8CC=(struct Cyc_Absyn_FloatType_struct*)
_tmp88A;if(_tmp8CC->tag != 7)goto _LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_DoubleType_struct*
_tmp8CD=(struct Cyc_Absyn_DoubleType_struct*)_tmp88A;if(_tmp8CD->tag != 8)goto
_LL49D;}_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp8CE=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp88A;if(_tmp8CE->tag != 22)goto _LL49F;}
_LL49E: goto _LL4A0;_LL49F: {struct Cyc_Absyn_HeapRgn_struct*_tmp8CF=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp88A;if(_tmp8CF->tag != 21)goto _LL4A1;}_LL4A0: return t;_LL4A1: {struct Cyc_Absyn_RgnsEff_struct*
_tmp8D0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp88A;if(_tmp8D0->tag != 25)goto _LL4A3;
else{_tmp8D1=(void*)_tmp8D0->f1;}}_LL4A2: {void*_tmp92F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8D1);struct Cyc_Absyn_RgnsEff_struct _tmp11BE;struct Cyc_Absyn_RgnsEff_struct*
_tmp11BD;return _tmp92F == _tmp8D1?t:(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((
_tmp11BD[0]=((_tmp11BE.tag=25,((_tmp11BE.f1=(void*)_tmp92F,_tmp11BE)))),_tmp11BD))));}
_LL4A3: {struct Cyc_Absyn_AccessEff_struct*_tmp8D2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp88A;if(_tmp8D2->tag != 23)goto _LL4A5;else{_tmp8D3=(void*)_tmp8D2->f1;}}_LL4A4: {
void*_tmp932=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D3);struct Cyc_Absyn_AccessEff_struct
_tmp11C1;struct Cyc_Absyn_AccessEff_struct*_tmp11C0;return _tmp932 == _tmp8D3?t:(
void*)((_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0[0]=((_tmp11C1.tag=23,((
_tmp11C1.f1=(void*)_tmp932,_tmp11C1)))),_tmp11C0))));}_LL4A5: {struct Cyc_Absyn_JoinEff_struct*
_tmp8D4=(struct Cyc_Absyn_JoinEff_struct*)_tmp88A;if(_tmp8D4->tag != 24)goto _LL472;
else{_tmp8D5=_tmp8D4->f1;}}_LL4A6: {struct Cyc_List_List*_tmp935=Cyc_Tcutil_substs(
rgn,inst,_tmp8D5);struct Cyc_Absyn_JoinEff_struct _tmp11C4;struct Cyc_Absyn_JoinEff_struct*
_tmp11C3;return _tmp935 == _tmp8D5?t:(void*)((_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((
_tmp11C3[0]=((_tmp11C4.tag=24,((_tmp11C4.f1=_tmp935,_tmp11C4)))),_tmp11C3))));}
_LL472:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmp938=(void*)ts->hd;struct Cyc_List_List*_tmp939=ts->tl;void*
_tmp93A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp938);struct Cyc_List_List*_tmp93B=Cyc_Tcutil_substs(
rgn,inst,_tmp939);if(_tmp938 == _tmp93A  && _tmp939 == _tmp93B)return ts;{struct Cyc_List_List*
_tmp11C5;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp11C5=_cycalloc(
sizeof(*_tmp11C5)),((_tmp11C5->hd=_tmp93A,((_tmp11C5->tl=_tmp93B,_tmp11C5)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp93D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,&
Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp11C8;struct _tuple14*_tmp11C7;return(
_tmp11C7=_cycalloc(sizeof(*_tmp11C7)),((_tmp11C7->f1=tv,((_tmp11C7->f2=Cyc_Absyn_new_evar(
_tmp93D,((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8->v=s,_tmp11C8))))),
_tmp11C7)))));}struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*env,struct Cyc_Absyn_Tvar*tv){struct _tuple15 _tmp941;struct Cyc_List_List*
_tmp942;struct _RegionHandle*_tmp943;struct _tuple15*_tmp940=env;_tmp941=*_tmp940;
_tmp942=_tmp941.f1;_tmp943=_tmp941.f2;{struct Cyc_Core_Opt*_tmp944=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp11CB;struct
_tuple14*_tmp11CA;return(_tmp11CA=_region_malloc(_tmp943,sizeof(*_tmp11CA)),((
_tmp11CA->f1=tv,((_tmp11CA->f2=Cyc_Absyn_new_evar(_tmp944,((_tmp11CB=_cycalloc(
sizeof(*_tmp11CB)),((_tmp11CB->v=_tmp942,_tmp11CB))))),_tmp11CA)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp11D1;void*_tmp11D0[3];struct Cyc_String_pa_struct _tmp11CF;struct Cyc_String_pa_struct
_tmp11CE;struct Cyc_String_pa_struct _tmp11CD;(_tmp11CD.tag=0,((_tmp11CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp11CE.tag=0,((_tmp11CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp11CF.tag=0,((_tmp11CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp11D0[0]=& _tmp11CF,((_tmp11D0[1]=& _tmp11CE,((_tmp11D0[2]=&
_tmp11CD,Cyc_Tcutil_terr(loc,((_tmp11D1="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp11D1,sizeof(char),59))),_tag_dyneither(_tmp11D0,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp11D4;void*_tmp11D3;(_tmp11D3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D4="same type variable has different identity!",
_tag_dyneither(_tmp11D4,sizeof(char),43))),_tag_dyneither(_tmp11D3,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp11D5;return(_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5->hd=tv,((
_tmp11D5->tl=tvs,_tmp11D5)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp11D8;void*_tmp11D7;(_tmp11D7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D8="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp11D8,sizeof(char),39))),_tag_dyneither(_tmp11D7,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp951=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp951->identity == - 1){const char*
_tmp11DB;void*_tmp11DA;(_tmp11DA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11DB="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp11DB,sizeof(char),41))),_tag_dyneither(_tmp11DA,sizeof(void*),
0)));}if(_tmp951->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp11DC;return(_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC->hd=tv,((
_tmp11DC->tl=tvs,_tmp11DC)))));};}struct _tuple25{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp11DF;void*_tmp11DE;(_tmp11DE=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11DF="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp11DF,sizeof(char),44))),_tag_dyneither(_tmp11DE,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple25
_tmp958;struct Cyc_Absyn_Tvar*_tmp959;int _tmp95A;int*_tmp95B;struct _tuple25*
_tmp957=(struct _tuple25*)tvs2->hd;_tmp958=*_tmp957;_tmp959=_tmp958.f1;_tmp95A=
_tmp958.f2;_tmp95B=(int*)&(*_tmp957).f2;if(_tmp959->identity == - 1){const char*
_tmp11E2;void*_tmp11E1;(_tmp11E1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11E2="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp11E2,sizeof(char),46))),_tag_dyneither(_tmp11E1,sizeof(void*),
0)));}if(_tmp959->identity == tv->identity){*_tmp95B=*_tmp95B  || b;return tvs;}}}{
struct _tuple25*_tmp11E5;struct Cyc_List_List*_tmp11E4;return(_tmp11E4=
_region_malloc(r,sizeof(*_tmp11E4)),((_tmp11E4->hd=((_tmp11E5=_region_malloc(r,
sizeof(*_tmp11E5)),((_tmp11E5->f1=tv,((_tmp11E5->f2=b,_tmp11E5)))))),((_tmp11E4->tl=
tvs,_tmp11E4)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp11E9;void*_tmp11E8[1];struct Cyc_String_pa_struct _tmp11E7;(_tmp11E7.tag=0,((
_tmp11E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp11E8[0]=& _tmp11E7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11E9="bound tvar id for %s is NULL",
_tag_dyneither(_tmp11E9,sizeof(char),29))),_tag_dyneither(_tmp11E8,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp11EA;return(_tmp11EA=_cycalloc(sizeof(*
_tmp11EA)),((_tmp11EA->hd=tv,((_tmp11EA->tl=tvs,_tmp11EA)))));};}struct _tuple26{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp964=Cyc_Tcutil_compress(e);int _tmp966;_LL4B2: {struct Cyc_Absyn_Evar_struct*
_tmp965=(struct Cyc_Absyn_Evar_struct*)_tmp964;if(_tmp965->tag != 1)goto _LL4B4;
else{_tmp966=_tmp965->f3;}}_LL4B3:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple26 _tmp968;void*_tmp969;int _tmp96A;int*_tmp96B;struct
_tuple26*_tmp967=(struct _tuple26*)es2->hd;_tmp968=*_tmp967;_tmp969=_tmp968.f1;
_tmp96A=_tmp968.f2;_tmp96B=(int*)&(*_tmp967).f2;{void*_tmp96C=Cyc_Tcutil_compress(
_tmp969);int _tmp96E;_LL4B7: {struct Cyc_Absyn_Evar_struct*_tmp96D=(struct Cyc_Absyn_Evar_struct*)
_tmp96C;if(_tmp96D->tag != 1)goto _LL4B9;else{_tmp96E=_tmp96D->f3;}}_LL4B8: if(
_tmp966 == _tmp96E){if(b != *_tmp96B)*_tmp96B=1;return es;}goto _LL4B6;_LL4B9:;
_LL4BA: goto _LL4B6;_LL4B6:;};}}{struct _tuple26*_tmp11ED;struct Cyc_List_List*
_tmp11EC;return(_tmp11EC=_region_malloc(r,sizeof(*_tmp11EC)),((_tmp11EC->hd=((
_tmp11ED=_region_malloc(r,sizeof(*_tmp11ED)),((_tmp11ED->f1=e,((_tmp11ED->f2=b,
_tmp11ED)))))),((_tmp11EC->tl=es,_tmp11EC)))));};_LL4B4:;_LL4B5: return es;_LL4B1:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp11EE;r=((_tmp11EE=_region_malloc(rgn,sizeof(*_tmp11EE)),((
_tmp11EE->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp11EE->tl=r,_tmp11EE))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp973;int _tmp974;struct _tuple25 _tmp972=*((struct _tuple25*)
tvs->hd);_tmp973=_tmp972.f1;_tmp974=_tmp972.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmp973->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp11EF;res=((_tmp11EF=
_region_malloc(r,sizeof(*_tmp11EF)),((_tmp11EF->hd=(struct _tuple25*)tvs->hd,((
_tmp11EF->tl=res,_tmp11EF))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp11F3;void*_tmp11F2[1];struct Cyc_String_pa_struct
_tmp11F1;(_tmp11F1.tag=0,((_tmp11F1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp11F2[0]=& _tmp11F1,Cyc_Tcutil_terr(loc,((_tmp11F3="bitfield %s does not have constant width",
_tag_dyneither(_tmp11F3,sizeof(char),41))),_tag_dyneither(_tmp11F2,sizeof(void*),
1)))))));}else{unsigned int _tmp97A;int _tmp97B;struct _tuple13 _tmp979=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp97A=_tmp979.f1;_tmp97B=_tmp979.f2;if(!_tmp97B){
const char*_tmp11F6;void*_tmp11F5;(_tmp11F5=0,Cyc_Tcutil_terr(loc,((_tmp11F6="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp11F6,sizeof(char),45))),_tag_dyneither(_tmp11F5,sizeof(void*),
0)));}w=_tmp97A;}{void*_tmp97E=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmp980;_LL4BC: {struct Cyc_Absyn_IntType_struct*_tmp97F=(struct Cyc_Absyn_IntType_struct*)
_tmp97E;if(_tmp97F->tag != 6)goto _LL4BE;else{_tmp980=_tmp97F->f2;}}_LL4BD: switch(
_tmp980){case Cyc_Absyn_Char_sz: _LL4C0: if(w > 8){const char*_tmp11F9;void*_tmp11F8;(
_tmp11F8=0,Cyc_Tcutil_terr(loc,((_tmp11F9="bitfield larger than type",
_tag_dyneither(_tmp11F9,sizeof(char),26))),_tag_dyneither(_tmp11F8,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL4C1: if(w > 16){const char*_tmp11FC;void*
_tmp11FB;(_tmp11FB=0,Cyc_Tcutil_terr(loc,((_tmp11FC="bitfield larger than type",
_tag_dyneither(_tmp11FC,sizeof(char),26))),_tag_dyneither(_tmp11FB,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL4C2: goto _LL4C3;case Cyc_Absyn_Int_sz: _LL4C3:
if(w > 32){const char*_tmp11FF;void*_tmp11FE;(_tmp11FE=0,Cyc_Tcutil_terr(loc,((
_tmp11FF="bitfield larger than type",_tag_dyneither(_tmp11FF,sizeof(char),26))),
_tag_dyneither(_tmp11FE,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL4C4: if(w > 64){const char*_tmp1202;void*_tmp1201;(_tmp1201=0,Cyc_Tcutil_terr(
loc,((_tmp1202="bitfield larger than type",_tag_dyneither(_tmp1202,sizeof(char),
26))),_tag_dyneither(_tmp1201,sizeof(void*),0)));}break;}goto _LL4BB;_LL4BE:;
_LL4BF:{const char*_tmp1207;void*_tmp1206[2];struct Cyc_String_pa_struct _tmp1205;
struct Cyc_String_pa_struct _tmp1204;(_tmp1204.tag=0,((_tmp1204.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1205.tag=0,((_tmp1205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1206[0]=& _tmp1205,((_tmp1206[1]=& _tmp1204,Cyc_Tcutil_terr(loc,((_tmp1207="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1207,sizeof(char),52))),_tag_dyneither(_tmp1206,sizeof(void*),
2)))))))))))));}goto _LL4BB;_LL4BB:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmp98D=(void*)atts->hd;_LL4C7: {struct Cyc_Absyn_Packed_att_struct*_tmp98E=(
struct Cyc_Absyn_Packed_att_struct*)_tmp98D;if(_tmp98E->tag != 7)goto _LL4C9;}
_LL4C8: continue;_LL4C9: {struct Cyc_Absyn_Aligned_att_struct*_tmp98F=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp98D;if(_tmp98F->tag != 6)goto _LL4CB;}_LL4CA: continue;_LL4CB:;_LL4CC: {const
char*_tmp120C;void*_tmp120B[2];struct Cyc_String_pa_struct _tmp120A;struct Cyc_String_pa_struct
_tmp1209;(_tmp1209.tag=0,((_tmp1209.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp120A.tag=0,((_tmp120A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp120B[0]=&
_tmp120A,((_tmp120B[1]=& _tmp1209,Cyc_Tcutil_terr(loc,((_tmp120C="bad attribute %s on member %s",
_tag_dyneither(_tmp120C,sizeof(char),30))),_tag_dyneither(_tmp120B,sizeof(void*),
2)))))))))))));}_LL4C6:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp994=t;struct
Cyc_Absyn_Typedefdecl*_tmp996;void**_tmp997;_LL4CE: {struct Cyc_Absyn_TypedefType_struct*
_tmp995=(struct Cyc_Absyn_TypedefType_struct*)_tmp994;if(_tmp995->tag != 18)goto
_LL4D0;else{_tmp996=_tmp995->f3;_tmp997=_tmp995->f4;}}_LL4CF: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp996))->tq).real_const  || (_tmp996->tq).print_const){if(
declared_const){const char*_tmp120F;void*_tmp120E;(_tmp120E=0,Cyc_Tcutil_warn(loc,((
_tmp120F="extra const",_tag_dyneither(_tmp120F,sizeof(char),12))),_tag_dyneither(
_tmp120E,sizeof(void*),0)));}return 1;}if((unsigned int)_tmp997)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp997);else{return declared_const;}_LL4D0:;_LL4D1: return
declared_const;_LL4CD:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple27{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmp99A=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp99D;struct Cyc_Core_Opt**_tmp99E;struct Cyc_Core_Opt*
_tmp99F;struct Cyc_Core_Opt**_tmp9A0;struct Cyc_Absyn_Tvar*_tmp9A2;struct Cyc_List_List*
_tmp9A4;struct _tuple2*_tmp9A6;struct Cyc_Absyn_Enumdecl*_tmp9A7;struct Cyc_Absyn_Enumdecl**
_tmp9A8;struct Cyc_Absyn_DatatypeInfo _tmp9AA;union Cyc_Absyn_DatatypeInfoU _tmp9AB;
union Cyc_Absyn_DatatypeInfoU*_tmp9AC;struct Cyc_List_List*_tmp9AD;struct Cyc_List_List**
_tmp9AE;struct Cyc_Absyn_DatatypeFieldInfo _tmp9B0;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9B1;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9B2;struct Cyc_List_List*_tmp9B3;
struct Cyc_Absyn_PtrInfo _tmp9B5;void*_tmp9B6;struct Cyc_Absyn_Tqual _tmp9B7;struct
Cyc_Absyn_Tqual*_tmp9B8;struct Cyc_Absyn_PtrAtts _tmp9B9;void*_tmp9BA;union Cyc_Absyn_Constraint*
_tmp9BB;union Cyc_Absyn_Constraint*_tmp9BC;union Cyc_Absyn_Constraint*_tmp9BD;void*
_tmp9BF;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_ArrayInfo _tmp9C6;void*
_tmp9C7;struct Cyc_Absyn_Tqual _tmp9C8;struct Cyc_Absyn_Tqual*_tmp9C9;struct Cyc_Absyn_Exp*
_tmp9CA;struct Cyc_Absyn_Exp**_tmp9CB;union Cyc_Absyn_Constraint*_tmp9CC;struct Cyc_Position_Segment*
_tmp9CD;struct Cyc_Absyn_FnInfo _tmp9CF;struct Cyc_List_List*_tmp9D0;struct Cyc_List_List**
_tmp9D1;struct Cyc_Core_Opt*_tmp9D2;struct Cyc_Core_Opt**_tmp9D3;void*_tmp9D4;
struct Cyc_List_List*_tmp9D5;int _tmp9D6;struct Cyc_Absyn_VarargInfo*_tmp9D7;struct
Cyc_List_List*_tmp9D8;struct Cyc_List_List*_tmp9D9;struct Cyc_List_List*_tmp9DB;
enum Cyc_Absyn_AggrKind _tmp9DD;struct Cyc_List_List*_tmp9DE;struct Cyc_Absyn_AggrInfo
_tmp9E0;union Cyc_Absyn_AggrInfoU _tmp9E1;union Cyc_Absyn_AggrInfoU*_tmp9E2;struct
Cyc_List_List*_tmp9E3;struct Cyc_List_List**_tmp9E4;struct _tuple2*_tmp9E6;struct
Cyc_List_List*_tmp9E7;struct Cyc_List_List**_tmp9E8;struct Cyc_Absyn_Typedefdecl*
_tmp9E9;struct Cyc_Absyn_Typedefdecl**_tmp9EA;void**_tmp9EB;void***_tmp9EC;void*
_tmp9F0;void*_tmp9F2;void*_tmp9F3;void*_tmp9F5;void*_tmp9F7;struct Cyc_List_List*
_tmp9F9;_LL4D3: {struct Cyc_Absyn_VoidType_struct*_tmp99B=(struct Cyc_Absyn_VoidType_struct*)
_tmp99A;if(_tmp99B->tag != 0)goto _LL4D5;}_LL4D4: goto _LL4D2;_LL4D5: {struct Cyc_Absyn_Evar_struct*
_tmp99C=(struct Cyc_Absyn_Evar_struct*)_tmp99A;if(_tmp99C->tag != 1)goto _LL4D7;
else{_tmp99D=_tmp99C->f1;_tmp99E=(struct Cyc_Core_Opt**)& _tmp99C->f1;_tmp99F=
_tmp99C->f2;_tmp9A0=(struct Cyc_Core_Opt**)& _tmp99C->f2;}}_LL4D6: if(*_tmp99E == 0)*
_tmp99E=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && expected_kind->kind == Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*
_tmp9A0=(struct Cyc_Core_Opt*)& urgn;else{*_tmp9A0=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1212;struct
Cyc_Absyn_Less_kb_struct*_tmp1211;struct Cyc_Absyn_Tvar*_tmp9FA=Cyc_Tcutil_new_tvar((
void*)((_tmp1211=_cycalloc(sizeof(*_tmp1211)),((_tmp1211[0]=((_tmp1212.tag=2,((
_tmp1212.f1=0,((_tmp1212.f2=expected_kind,_tmp1212)))))),_tmp1211)))));{struct
Cyc_Absyn_VarType_struct*_tmp1218;struct Cyc_Absyn_VarType_struct _tmp1217;struct
Cyc_Core_Opt*_tmp1216;*_tmp9A0=((_tmp1216=_cycalloc(sizeof(*_tmp1216)),((
_tmp1216->v=(void*)((_tmp1218=_cycalloc(sizeof(*_tmp1218)),((_tmp1218[0]=((
_tmp1217.tag=2,((_tmp1217.f1=_tmp9FA,_tmp1217)))),_tmp1218)))),_tmp1216))));}
_tmp9A2=_tmp9FA;goto _LL4D8;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto _LL4D2;_LL4D7: {struct Cyc_Absyn_VarType_struct*
_tmp9A1=(struct Cyc_Absyn_VarType_struct*)_tmp99A;if(_tmp9A1->tag != 2)goto _LL4D9;
else{_tmp9A2=_tmp9A1->f1;}}_LL4D8:{void*_tmpA00=Cyc_Absyn_compress_kb(_tmp9A2->kind);
struct Cyc_Core_Opt*_tmpA02;struct Cyc_Core_Opt**_tmpA03;_LL508: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpA01=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpA00;if(_tmpA01->tag != 1)goto
_LL50A;else{_tmpA02=_tmpA01->f1;_tmpA03=(struct Cyc_Core_Opt**)& _tmpA01->f1;}}
_LL509:{struct Cyc_Absyn_Less_kb_struct*_tmp121E;struct Cyc_Absyn_Less_kb_struct
_tmp121D;struct Cyc_Core_Opt*_tmp121C;*_tmpA03=((_tmp121C=_cycalloc(sizeof(*
_tmp121C)),((_tmp121C->v=(void*)((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((
_tmp121E[0]=((_tmp121D.tag=2,((_tmp121D.f1=0,((_tmp121D.f2=expected_kind,
_tmp121D)))))),_tmp121E)))),_tmp121C))));}goto _LL507;_LL50A:;_LL50B: goto _LL507;
_LL507:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp9A2,put_in_effect);{void*_tmpA07=Cyc_Absyn_compress_kb(_tmp9A2->kind);struct
Cyc_Core_Opt*_tmpA09;struct Cyc_Core_Opt**_tmpA0A;struct Cyc_Absyn_Kind*_tmpA0B;
_LL50D: {struct Cyc_Absyn_Less_kb_struct*_tmpA08=(struct Cyc_Absyn_Less_kb_struct*)
_tmpA07;if(_tmpA08->tag != 2)goto _LL50F;else{_tmpA09=_tmpA08->f1;_tmpA0A=(struct
Cyc_Core_Opt**)& _tmpA08->f1;_tmpA0B=_tmpA08->f2;}}_LL50E: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpA0B)){struct Cyc_Absyn_Less_kb_struct*_tmp1224;struct Cyc_Absyn_Less_kb_struct
_tmp1223;struct Cyc_Core_Opt*_tmp1222;*_tmpA0A=((_tmp1222=_cycalloc(sizeof(*
_tmp1222)),((_tmp1222->v=(void*)((_tmp1224=_cycalloc(sizeof(*_tmp1224)),((
_tmp1224[0]=((_tmp1223.tag=2,((_tmp1223.f1=0,((_tmp1223.f2=expected_kind,
_tmp1223)))))),_tmp1224)))),_tmp1222))));}goto _LL50C;_LL50F:;_LL510: goto _LL50C;
_LL50C:;}goto _LL4D2;_LL4D9: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9A3=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmp99A;if(_tmp9A3->tag != 15)goto _LL4DB;else{
_tmp9A4=_tmp9A3->f1;}}_LL4DA: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpA0F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp9A4 != 0;_tmp9A4=_tmp9A4->tl){
struct Cyc_Absyn_Enumfield*_tmpA10=(struct Cyc_Absyn_Enumfield*)_tmp9A4->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA10->name).f2)){
const char*_tmp1228;void*_tmp1227[1];struct Cyc_String_pa_struct _tmp1226;(_tmp1226.tag=
0,((_tmp1226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA10->name).f2),((
_tmp1227[0]=& _tmp1226,Cyc_Tcutil_terr(_tmpA10->loc,((_tmp1228="duplicate enum field name %s",
_tag_dyneither(_tmp1228,sizeof(char),29))),_tag_dyneither(_tmp1227,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1229;prev_fields=((_tmp1229=
_region_malloc(_tmpA0F,sizeof(*_tmp1229)),((_tmp1229->hd=(*_tmpA10->name).f2,((
_tmp1229->tl=prev_fields,_tmp1229))))));}if(_tmpA10->tag == 0)_tmpA10->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpA10->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpA10->tag))){const char*_tmp122D;void*
_tmp122C[1];struct Cyc_String_pa_struct _tmp122B;(_tmp122B.tag=0,((_tmp122B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA10->name).f2),((_tmp122C[0]=&
_tmp122B,Cyc_Tcutil_terr(loc,((_tmp122D="enum field %s: expression is not constant",
_tag_dyneither(_tmp122D,sizeof(char),42))),_tag_dyneither(_tmp122C,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpA10->tag))).f1;tag_count=t1 + 1;(*_tmpA10->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp1233;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp1232;struct _tuple26*_tmp1231;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple26*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpA10->name).f2,(struct _tuple26*)((_tmp1231=_cycalloc(sizeof(*_tmp1231)),((
_tmp1231->f1=(void*)((_tmp1233=_cycalloc(sizeof(*_tmp1233)),((_tmp1233[0]=((
_tmp1232.tag=4,((_tmp1232.f1=(void*)t,((_tmp1232.f2=_tmpA10,_tmp1232)))))),
_tmp1233)))),((_tmp1231->f2=1,_tmp1231)))))));};};}}goto _LL4D2;}_LL4DB: {struct
Cyc_Absyn_EnumType_struct*_tmp9A5=(struct Cyc_Absyn_EnumType_struct*)_tmp99A;if(
_tmp9A5->tag != 14)goto _LL4DD;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;
_tmp9A8=(struct Cyc_Absyn_Enumdecl**)& _tmp9A5->f2;}}_LL4DC: if(*_tmp9A8 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9A8))->fields == 0){struct
_handler_cons _tmpA1B;_push_handler(& _tmpA1B);{int _tmpA1D=0;if(setjmp(_tmpA1B.handler))
_tmpA1D=1;if(!_tmpA1D){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp9A6);*_tmp9A8=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpA1C=(void*)_exn_thrown;void*_tmpA1F=_tmpA1C;_LL512: {struct Cyc_Dict_Absent_struct*
_tmpA20=(struct Cyc_Dict_Absent_struct*)_tmpA1F;if(_tmpA20->tag != Cyc_Dict_Absent)
goto _LL514;}_LL513: {struct Cyc_Tcenv_Genv*_tmpA21=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1234;struct Cyc_Absyn_Enumdecl*_tmpA22=(_tmp1234=
_cycalloc(sizeof(*_tmp1234)),((_tmp1234->sc=Cyc_Absyn_Extern,((_tmp1234->name=
_tmp9A6,((_tmp1234->fields=0,_tmp1234)))))));Cyc_Tc_tcEnumdecl(te,_tmpA21,loc,
_tmpA22);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9A6);*
_tmp9A8=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL511;};}_LL514:;_LL515:(void)_throw(
_tmpA1F);_LL511:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp9A8);*_tmp9A6=(ed->name)[0];goto _LL4D2;};_LL4DD: {struct Cyc_Absyn_DatatypeType_struct*
_tmp9A9=(struct Cyc_Absyn_DatatypeType_struct*)_tmp99A;if(_tmp9A9->tag != 3)goto
_LL4DF;else{_tmp9AA=_tmp9A9->f1;_tmp9AB=_tmp9AA.datatype_info;_tmp9AC=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmp9A9->f1).datatype_info;_tmp9AD=_tmp9AA.targs;_tmp9AE=(struct Cyc_List_List**)&(
_tmp9A9->f1).targs;}}_LL4DE: {struct Cyc_List_List*_tmpA24=*_tmp9AE;{union Cyc_Absyn_DatatypeInfoU
_tmpA25=*_tmp9AC;struct Cyc_Absyn_UnknownDatatypeInfo _tmpA26;struct _tuple2*
_tmpA27;int _tmpA28;struct Cyc_Absyn_Datatypedecl**_tmpA29;struct Cyc_Absyn_Datatypedecl*
_tmpA2A;_LL517: if((_tmpA25.UnknownDatatype).tag != 1)goto _LL519;_tmpA26=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpA25.UnknownDatatype).val;_tmpA27=_tmpA26.name;
_tmpA28=_tmpA26.is_extensible;_LL518: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpA2B;_push_handler(& _tmpA2B);{int _tmpA2D=0;if(setjmp(
_tmpA2B.handler))_tmpA2D=1;if(!_tmpA2D){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpA27);;_pop_handler();}else{void*_tmpA2C=(void*)_exn_thrown;void*_tmpA2F=
_tmpA2C;_LL51C: {struct Cyc_Dict_Absent_struct*_tmpA30=(struct Cyc_Dict_Absent_struct*)
_tmpA2F;if(_tmpA30->tag != Cyc_Dict_Absent)goto _LL51E;}_LL51D: {struct Cyc_Tcenv_Genv*
_tmpA31=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1235;struct Cyc_Absyn_Datatypedecl*
_tmpA32=(_tmp1235=_cycalloc(sizeof(*_tmp1235)),((_tmp1235->sc=Cyc_Absyn_Extern,((
_tmp1235->name=_tmpA27,((_tmp1235->tvs=0,((_tmp1235->fields=0,((_tmp1235->is_extensible=
_tmpA28,_tmp1235)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpA31,loc,_tmpA32);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA27);if(_tmpA24 != 0){{const char*
_tmp1239;void*_tmp1238[1];struct Cyc_String_pa_struct _tmp1237;(_tmp1237.tag=0,((
_tmp1237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA27)),((_tmp1238[0]=& _tmp1237,Cyc_Tcutil_terr(loc,((_tmp1239="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1239,sizeof(char),47))),_tag_dyneither(_tmp1238,sizeof(void*),
1)))))));}return cvtenv;}goto _LL51B;}_LL51E:;_LL51F:(void)_throw(_tmpA2F);_LL51B:;}};}
if(_tmpA28  && !(*tudp)->is_extensible){const char*_tmp123D;void*_tmp123C[1];
struct Cyc_String_pa_struct _tmp123B;(_tmp123B.tag=0,((_tmp123B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA27)),((
_tmp123C[0]=& _tmp123B,Cyc_Tcutil_terr(loc,((_tmp123D="datatype %s was not declared @extensible",
_tag_dyneither(_tmp123D,sizeof(char),41))),_tag_dyneither(_tmp123C,sizeof(void*),
1)))))));}*_tmp9AC=Cyc_Absyn_KnownDatatype(tudp);_tmpA2A=*tudp;goto _LL51A;}
_LL519: if((_tmpA25.KnownDatatype).tag != 2)goto _LL516;_tmpA29=(struct Cyc_Absyn_Datatypedecl**)(
_tmpA25.KnownDatatype).val;_tmpA2A=*_tmpA29;_LL51A: {struct Cyc_List_List*tvs=
_tmpA2A->tvs;for(0;_tmpA24 != 0  && tvs != 0;(_tmpA24=_tmpA24->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA24->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp123E;struct _tuple0 _tmpA3B=(_tmp123E.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp123E.f2=t,_tmp123E)));void*_tmpA3C;void*_tmpA3E;struct Cyc_Absyn_Tvar*_tmpA40;
_LL521: _tmpA3C=_tmpA3B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpA3D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA3C;if(_tmpA3D->tag != 1)goto _LL523;};_tmpA3E=_tmpA3B.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA3F=(struct Cyc_Absyn_VarType_struct*)_tmpA3E;if(_tmpA3F->tag != 2)goto _LL523;
else{_tmpA40=_tmpA3F->f1;}};_LL522: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA40);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA40,1);continue;_LL523:;_LL524: goto _LL520;_LL520:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpA24 != 0){const char*_tmp1242;void*_tmp1241[1];
struct Cyc_String_pa_struct _tmp1240;(_tmp1240.tag=0,((_tmp1240.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA2A->name)),((
_tmp1241[0]=& _tmp1240,Cyc_Tcutil_terr(loc,((_tmp1242="too many type arguments for datatype %s",
_tag_dyneither(_tmp1242,sizeof(char),40))),_tag_dyneither(_tmp1241,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1243;hidden_ts=((
_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243->hd=e,((_tmp1243->tl=hidden_ts,
_tmp1243))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*
_tmp9AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp9AE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}goto _LL516;}_LL516:;}goto _LL4D2;}_LL4DF: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp9AF=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp99A;if(_tmp9AF->tag != 4)goto _LL4E1;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9B0.field_info;
_tmp9B2=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9AF->f1).field_info;_tmp9B3=
_tmp9B0.targs;}}_LL4E0:{union Cyc_Absyn_DatatypeFieldInfoU _tmpA45=*_tmp9B2;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA46;struct _tuple2*_tmpA47;struct _tuple2*
_tmpA48;int _tmpA49;struct _tuple3 _tmpA4A;struct Cyc_Absyn_Datatypedecl*_tmpA4B;
struct Cyc_Absyn_Datatypefield*_tmpA4C;_LL526: if((_tmpA45.UnknownDatatypefield).tag
!= 1)goto _LL528;_tmpA46=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA45.UnknownDatatypefield).val;
_tmpA47=_tmpA46.datatype_name;_tmpA48=_tmpA46.field_name;_tmpA49=_tmpA46.is_extensible;
_LL527: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpA4D;_push_handler(& _tmpA4D);{int _tmpA4F=0;if(setjmp(
_tmpA4D.handler))_tmpA4F=1;if(!_tmpA4F){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpA47);;_pop_handler();}else{void*_tmpA4E=(void*)_exn_thrown;void*_tmpA51=
_tmpA4E;_LL52B: {struct Cyc_Dict_Absent_struct*_tmpA52=(struct Cyc_Dict_Absent_struct*)
_tmpA51;if(_tmpA52->tag != Cyc_Dict_Absent)goto _LL52D;}_LL52C:{const char*_tmp1247;
void*_tmp1246[1];struct Cyc_String_pa_struct _tmp1245;(_tmp1245.tag=0,((_tmp1245.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA47)),((
_tmp1246[0]=& _tmp1245,Cyc_Tcutil_terr(loc,((_tmp1247="unbound datatype %s",
_tag_dyneither(_tmp1247,sizeof(char),20))),_tag_dyneither(_tmp1246,sizeof(void*),
1)))))));}return cvtenv;_LL52D:;_LL52E:(void)_throw(_tmpA51);_LL52A:;}};}{struct
_handler_cons _tmpA56;_push_handler(& _tmpA56);{int _tmpA58=0;if(setjmp(_tmpA56.handler))
_tmpA58=1;if(!_tmpA58){{struct _RegionHandle*_tmpA59=Cyc_Tcenv_get_fnrgn(te);void*
_tmpA5A=Cyc_Tcenv_lookup_ordinary(_tmpA59,te,loc,_tmpA48);struct Cyc_Absyn_Datatypedecl*
_tmpA5C;struct Cyc_Absyn_Datatypefield*_tmpA5D;_LL530: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpA5B=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpA5A;if(_tmpA5B->tag != 2)goto
_LL532;else{_tmpA5C=_tmpA5B->f1;_tmpA5D=_tmpA5B->f2;}}_LL531: tuf=_tmpA5D;tud=
_tmpA5C;if(_tmpA49  && !tud->is_extensible){const char*_tmp124B;void*_tmp124A[1];
struct Cyc_String_pa_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA47)),((
_tmp124A[0]=& _tmp1249,Cyc_Tcutil_terr(loc,((_tmp124B="datatype %s was not declared @extensible",
_tag_dyneither(_tmp124B,sizeof(char),41))),_tag_dyneither(_tmp124A,sizeof(void*),
1)))))));}goto _LL52F;_LL532:;_LL533:{const char*_tmp1250;void*_tmp124F[2];struct
Cyc_String_pa_struct _tmp124E;struct Cyc_String_pa_struct _tmp124D;(_tmp124D.tag=0,((
_tmp124D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA47)),((_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA48)),((_tmp124F[0]=& _tmp124E,((
_tmp124F[1]=& _tmp124D,Cyc_Tcutil_terr(loc,((_tmp1250="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1250,sizeof(char),37))),_tag_dyneither(_tmp124F,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpA65=cvtenv;_npop_handler(0);return
_tmpA65;};_LL52F:;};_pop_handler();}else{void*_tmpA57=(void*)_exn_thrown;void*
_tmpA67=_tmpA57;_LL535: {struct Cyc_Dict_Absent_struct*_tmpA68=(struct Cyc_Dict_Absent_struct*)
_tmpA67;if(_tmpA68->tag != Cyc_Dict_Absent)goto _LL537;}_LL536:{const char*_tmp1255;
void*_tmp1254[2];struct Cyc_String_pa_struct _tmp1253;struct Cyc_String_pa_struct
_tmp1252;(_tmp1252.tag=0,((_tmp1252.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA47)),((_tmp1253.tag=0,((_tmp1253.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA48)),((
_tmp1254[0]=& _tmp1253,((_tmp1254[1]=& _tmp1252,Cyc_Tcutil_terr(loc,((_tmp1255="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1255,sizeof(char),37))),_tag_dyneither(_tmp1254,sizeof(void*),
2)))))))))))));}return cvtenv;_LL537:;_LL538:(void)_throw(_tmpA67);_LL534:;}};}*
_tmp9B2=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpA4B=tud;_tmpA4C=tuf;goto _LL529;}
_LL528: if((_tmpA45.KnownDatatypefield).tag != 2)goto _LL525;_tmpA4A=(struct _tuple3)(
_tmpA45.KnownDatatypefield).val;_tmpA4B=_tmpA4A.f1;_tmpA4C=_tmpA4A.f2;_LL529: {
struct Cyc_List_List*tvs=_tmpA4B->tvs;for(0;_tmp9B3 != 0  && tvs != 0;(_tmp9B3=
_tmp9B3->tl,tvs=tvs->tl)){void*t=(void*)_tmp9B3->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp1256;struct _tuple0 _tmpA6E=(
_tmp1256.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1256.f2=t,_tmp1256)));void*
_tmpA6F;void*_tmpA71;struct Cyc_Absyn_Tvar*_tmpA73;_LL53A: _tmpA6F=_tmpA6E.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpA70=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA6F;if(_tmpA70->tag != 1)goto _LL53C;};_tmpA71=_tmpA6E.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA72=(struct Cyc_Absyn_VarType_struct*)_tmpA71;if(_tmpA72->tag != 2)goto _LL53C;
else{_tmpA73=_tmpA72->f1;}};_LL53B: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA73);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA73,1);continue;_LL53C:;_LL53D: goto _LL539;_LL539:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp9B3 != 0){const char*_tmp125B;void*_tmp125A[2];
struct Cyc_String_pa_struct _tmp1259;struct Cyc_String_pa_struct _tmp1258;(_tmp1258.tag=
0,((_tmp1258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA4C->name)),((_tmp1259.tag=0,((_tmp1259.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA4B->name)),((_tmp125A[0]=& _tmp1259,((
_tmp125A[1]=& _tmp1258,Cyc_Tcutil_terr(loc,((_tmp125B="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp125B,sizeof(char),43))),_tag_dyneither(_tmp125A,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp1260;void*_tmp125F[2];struct Cyc_String_pa_struct
_tmp125E;struct Cyc_String_pa_struct _tmp125D;(_tmp125D.tag=0,((_tmp125D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA4C->name)),((
_tmp125E.tag=0,((_tmp125E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA4B->name)),((_tmp125F[0]=& _tmp125E,((_tmp125F[1]=& _tmp125D,Cyc_Tcutil_terr(
loc,((_tmp1260="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp1260,sizeof(char),42))),_tag_dyneither(_tmp125F,sizeof(void*),2)))))))))))));}
goto _LL525;}_LL525:;}goto _LL4D2;_LL4E1: {struct Cyc_Absyn_PointerType_struct*
_tmp9B4=(struct Cyc_Absyn_PointerType_struct*)_tmp99A;if(_tmp9B4->tag != 5)goto
_LL4E3;else{_tmp9B5=_tmp9B4->f1;_tmp9B6=_tmp9B5.elt_typ;_tmp9B7=_tmp9B5.elt_tq;
_tmp9B8=(struct Cyc_Absyn_Tqual*)&(_tmp9B4->f1).elt_tq;_tmp9B9=_tmp9B5.ptr_atts;
_tmp9BA=_tmp9B9.rgn;_tmp9BB=_tmp9B9.nullable;_tmp9BC=_tmp9B9.bounds;_tmp9BD=
_tmp9B9.zero_term;}}_LL4E2: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ak,_tmp9B6,1);_tmp9B8->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp9B8->print_const,_tmp9B6);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_trk,_tmp9BA,1);{union Cyc_Absyn_Constraint*_tmpA7C=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9BD);union Cyc_Absyn_Constraint
_tmpA7D;int _tmpA7E;union Cyc_Absyn_Constraint _tmpA7F;int _tmpA80;_LL53F: _tmpA7D=*
_tmpA7C;if((_tmpA7D.No_constr).tag != 3)goto _LL541;_tmpA7E=(int)(_tmpA7D.No_constr).val;
_LL540:{void*_tmpA81=Cyc_Tcutil_compress(_tmp9B6);enum Cyc_Absyn_Size_of _tmpA83;
_LL546: {struct Cyc_Absyn_IntType_struct*_tmpA82=(struct Cyc_Absyn_IntType_struct*)
_tmpA81;if(_tmpA82->tag != 6)goto _LL548;else{_tmpA83=_tmpA82->f2;if(_tmpA83 != Cyc_Absyn_Char_sz)
goto _LL548;}}_LL547:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9BD,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL545;_LL548:;_LL549:((int(*)(
int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp9BD,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL545;
_LL545:;}goto _LL53E;_LL541: _tmpA7F=*_tmpA7C;if((_tmpA7F.Eq_constr).tag != 1)goto
_LL543;_tmpA80=(int)(_tmpA7F.Eq_constr).val;if(_tmpA80 != 1)goto _LL543;_LL542: if(
!Cyc_Tcutil_admits_zero(_tmp9B6)){const char*_tmp1264;void*_tmp1263[1];struct Cyc_String_pa_struct
_tmp1262;(_tmp1262.tag=0,((_tmp1262.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9B6)),((_tmp1263[0]=& _tmp1262,Cyc_Tcutil_terr(
loc,((_tmp1264="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp1264,sizeof(char),53))),_tag_dyneither(_tmp1263,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL53E;_LL543:;_LL544: is_zero_terminated=0;
goto _LL53E;_LL53E:;}{void*_tmpA87=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp9BC);struct Cyc_Absyn_Exp*
_tmpA8A;_LL54B: {struct Cyc_Absyn_DynEither_b_struct*_tmpA88=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpA87;if(_tmpA88->tag != 0)goto _LL54D;}_LL54C: goto _LL54A;_LL54D: {struct Cyc_Absyn_Upper_b_struct*
_tmpA89=(struct Cyc_Absyn_Upper_b_struct*)_tmpA87;if(_tmpA89->tag != 1)goto _LL54A;
else{_tmpA8A=_tmpA89->f1;}}_LL54E: {struct _RegionHandle*_tmpA8B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA8C=Cyc_Tcenv_allow_valueof(_tmpA8B,te);Cyc_Tcexp_tcExp(
_tmpA8C,0,_tmpA8A);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA8A,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA8A)){const char*_tmp1267;void*
_tmp1266;(_tmp1266=0,Cyc_Tcutil_terr(loc,((_tmp1267="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp1267,sizeof(char),49))),_tag_dyneither(_tmp1266,sizeof(void*),
0)));}{unsigned int _tmpA90;int _tmpA91;struct _tuple13 _tmpA8F=Cyc_Evexp_eval_const_uint_exp(
_tmpA8A);_tmpA90=_tmpA8F.f1;_tmpA91=_tmpA8F.f2;if(is_zero_terminated  && (!
_tmpA91  || _tmpA90 < 1)){const char*_tmp126A;void*_tmp1269;(_tmp1269=0,Cyc_Tcutil_terr(
loc,((_tmp126A="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp126A,sizeof(char),55))),_tag_dyneither(_tmp1269,sizeof(void*),
0)));}goto _LL54A;};}_LL54A:;}goto _LL4D2;}_LL4E3: {struct Cyc_Absyn_TagType_struct*
_tmp9BE=(struct Cyc_Absyn_TagType_struct*)_tmp99A;if(_tmp9BE->tag != 20)goto _LL4E5;
else{_tmp9BF=(void*)_tmp9BE->f1;}}_LL4E4: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp9BF,1);goto _LL4D2;_LL4E5: {struct Cyc_Absyn_ValueofType_struct*
_tmp9C0=(struct Cyc_Absyn_ValueofType_struct*)_tmp99A;if(_tmp9C0->tag != 19)goto
_LL4E7;else{_tmp9C1=_tmp9C0->f1;}}_LL4E6: {struct _RegionHandle*_tmpA94=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA95=Cyc_Tcenv_allow_valueof(_tmpA94,te);Cyc_Tcexp_tcExp(
_tmpA95,0,_tmp9C1);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9C1)){const char*
_tmp126D;void*_tmp126C;(_tmp126C=0,Cyc_Tcutil_terr(loc,((_tmp126D="valueof_t requires an int expression",
_tag_dyneither(_tmp126D,sizeof(char),37))),_tag_dyneither(_tmp126C,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9C1,te,cvtenv);goto
_LL4D2;}_LL4E7: {struct Cyc_Absyn_IntType_struct*_tmp9C2=(struct Cyc_Absyn_IntType_struct*)
_tmp99A;if(_tmp9C2->tag != 6)goto _LL4E9;}_LL4E8: goto _LL4EA;_LL4E9: {struct Cyc_Absyn_FloatType_struct*
_tmp9C3=(struct Cyc_Absyn_FloatType_struct*)_tmp99A;if(_tmp9C3->tag != 7)goto
_LL4EB;}_LL4EA: goto _LL4EC;_LL4EB: {struct Cyc_Absyn_DoubleType_struct*_tmp9C4=(
struct Cyc_Absyn_DoubleType_struct*)_tmp99A;if(_tmp9C4->tag != 8)goto _LL4ED;}
_LL4EC: goto _LL4D2;_LL4ED: {struct Cyc_Absyn_ArrayType_struct*_tmp9C5=(struct Cyc_Absyn_ArrayType_struct*)
_tmp99A;if(_tmp9C5->tag != 9)goto _LL4EF;else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C6.elt_type;
_tmp9C8=_tmp9C6.tq;_tmp9C9=(struct Cyc_Absyn_Tqual*)&(_tmp9C5->f1).tq;_tmp9CA=
_tmp9C6.num_elts;_tmp9CB=(struct Cyc_Absyn_Exp**)&(_tmp9C5->f1).num_elts;_tmp9CC=
_tmp9C6.zero_term;_tmp9CD=_tmp9C6.zt_loc;}}_LL4EE: {struct Cyc_Absyn_Exp*_tmpA98=*
_tmp9CB;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_mk,
_tmp9C7,1);_tmp9C9->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9C9->print_const,
_tmp9C7);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpA99=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9CC);union Cyc_Absyn_Constraint
_tmpA9A;int _tmpA9B;union Cyc_Absyn_Constraint _tmpA9C;int _tmpA9D;_LL550: _tmpA9A=*
_tmpA99;if((_tmpA9A.No_constr).tag != 3)goto _LL552;_tmpA9B=(int)(_tmpA9A.No_constr).val;
_LL551:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9CC,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL54F;_LL552: _tmpA9C=*_tmpA99;if((_tmpA9C.Eq_constr).tag
!= 1)goto _LL554;_tmpA9D=(int)(_tmpA9C.Eq_constr).val;if(_tmpA9D != 1)goto _LL554;
_LL553: if(!Cyc_Tcutil_admits_zero(_tmp9C7)){const char*_tmp1271;void*_tmp1270[1];
struct Cyc_String_pa_struct _tmp126F;(_tmp126F.tag=0,((_tmp126F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9C7)),((
_tmp1270[0]=& _tmp126F,Cyc_Tcutil_terr(loc,((_tmp1271="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp1271,sizeof(char),51))),_tag_dyneither(_tmp1270,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL54F;_LL554:;_LL555: is_zero_terminated=0;
goto _LL54F;_LL54F:;}if(_tmpA98 == 0){if(is_zero_terminated)*_tmp9CB=(_tmpA98=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp1274;void*
_tmp1273;(_tmp1273=0,Cyc_Tcutil_warn(loc,((_tmp1274="array bound defaults to 1 here",
_tag_dyneither(_tmp1274,sizeof(char),31))),_tag_dyneither(_tmp1273,sizeof(void*),
0)));}*_tmp9CB=(_tmpA98=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpA98);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpA98)){const char*_tmp1277;void*_tmp1276;(_tmp1276=0,Cyc_Tcutil_terr(loc,((
_tmp1277="array bounds expression is not constant",_tag_dyneither(_tmp1277,
sizeof(char),40))),_tag_dyneither(_tmp1276,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpA98)){const char*_tmp127A;void*_tmp1279;(_tmp1279=0,
Cyc_Tcutil_terr(loc,((_tmp127A="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp127A,sizeof(char),47))),_tag_dyneither(_tmp1279,sizeof(void*),
0)));}{unsigned int _tmpAA8;int _tmpAA9;struct _tuple13 _tmpAA7=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpA98);_tmpAA8=_tmpAA7.f1;_tmpAA9=_tmpAA7.f2;if((
is_zero_terminated  && _tmpAA9) && _tmpAA8 < 1){const char*_tmp127D;void*_tmp127C;(
_tmp127C=0,Cyc_Tcutil_warn(loc,((_tmp127D="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp127D,sizeof(char),48))),_tag_dyneither(_tmp127C,sizeof(void*),
0)));}if((_tmpAA9  && _tmpAA8 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1280;void*
_tmp127F;(_tmp127F=0,Cyc_Tcutil_warn(loc,((_tmp1280="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1280,sizeof(char),75))),_tag_dyneither(_tmp127F,sizeof(void*),
0)));}*_tmp9CB=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4D2;};};}
_LL4EF: {struct Cyc_Absyn_FnType_struct*_tmp9CE=(struct Cyc_Absyn_FnType_struct*)
_tmp99A;if(_tmp9CE->tag != 10)goto _LL4F1;else{_tmp9CF=_tmp9CE->f1;_tmp9D0=_tmp9CF.tvars;
_tmp9D1=(struct Cyc_List_List**)&(_tmp9CE->f1).tvars;_tmp9D2=_tmp9CF.effect;
_tmp9D3=(struct Cyc_Core_Opt**)&(_tmp9CE->f1).effect;_tmp9D4=_tmp9CF.ret_typ;
_tmp9D5=_tmp9CF.args;_tmp9D6=_tmp9CF.c_varargs;_tmp9D7=_tmp9CF.cyc_varargs;
_tmp9D8=_tmp9CF.rgn_po;_tmp9D9=_tmp9CF.attributes;}}_LL4F0: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp9D9 != 0;
_tmp9D9=_tmp9D9->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp9D9->hd)){const char*
_tmp1284;void*_tmp1283[1];struct Cyc_String_pa_struct _tmp1282;(_tmp1282.tag=0,((
_tmp1282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp9D9->hd)),((_tmp1283[0]=& _tmp1282,Cyc_Tcutil_terr(loc,((_tmp1284="bad function type attribute %s",
_tag_dyneither(_tmp1284,sizeof(char),31))),_tag_dyneither(_tmp1283,sizeof(void*),
1)))))));}{void*_tmpAB1=(void*)_tmp9D9->hd;enum Cyc_Absyn_Format_Type _tmpAB6;int
_tmpAB7;int _tmpAB8;_LL557: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAB2=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpAB1;if(_tmpAB2->tag != 1)goto _LL559;}_LL558: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL556;_LL559: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAB3=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAB1;if(_tmpAB3->tag != 2)goto
_LL55B;}_LL55A: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL556;_LL55B: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpAB4=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpAB1;if(_tmpAB4->tag != 3)goto _LL55D;}_LL55C: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL556;_LL55D: {struct Cyc_Absyn_Format_att_struct*_tmpAB5=(
struct Cyc_Absyn_Format_att_struct*)_tmpAB1;if(_tmpAB5->tag != 19)goto _LL55F;else{
_tmpAB6=_tmpAB5->f1;_tmpAB7=_tmpAB5->f2;_tmpAB8=_tmpAB5->f3;}}_LL55E: if(!
seen_format){seen_format=1;ft=_tmpAB6;fmt_desc_arg=_tmpAB7;fmt_arg_start=_tmpAB8;}
else{const char*_tmp1287;void*_tmp1286;(_tmp1286=0,Cyc_Tcutil_terr(loc,((_tmp1287="function can't have multiple format attributes",
_tag_dyneither(_tmp1287,sizeof(char),47))),_tag_dyneither(_tmp1286,sizeof(void*),
0)));}goto _LL556;_LL55F:;_LL560: goto _LL556;_LL556:;};}if(num_convs > 1){const char*
_tmp128A;void*_tmp1289;(_tmp1289=0,Cyc_Tcutil_terr(loc,((_tmp128A="function can't have multiple calling conventions",
_tag_dyneither(_tmp128A,sizeof(char),49))),_tag_dyneither(_tmp1289,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp9D1);{struct Cyc_List_List*b=*
_tmp9D1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpABD=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpABF;_LL562: {struct Cyc_Absyn_Eq_kb_struct*_tmpABE=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpABD;if(_tmpABE->tag != 0)goto _LL564;else{
_tmpABF=_tmpABE->f1;if(_tmpABF != Cyc_Absyn_MemKind)goto _LL564;}}_LL563:{const
char*_tmp128E;void*_tmp128D[1];struct Cyc_String_pa_struct _tmp128C;(_tmp128C.tag=
0,((_tmp128C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp128D[0]=& _tmp128C,Cyc_Tcutil_terr(loc,((_tmp128E="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp128E,sizeof(char),51))),_tag_dyneither(_tmp128D,sizeof(void*),
1)))))));}goto _LL561;_LL564:;_LL565: goto _LL561;_LL561:;};}}{struct _RegionHandle*
_tmpAC3=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp128F;struct Cyc_Tcutil_CVTEnv
_tmpAC4=(_tmp128F.r=_tmpAC3,((_tmp128F.kind_env=cvtenv.kind_env,((_tmp128F.free_vars=
0,((_tmp128F.free_evars=0,((_tmp128F.generalize_evars=cvtenv.generalize_evars,((
_tmp128F.fn_result=1,_tmp128F)))))))))));_tmpAC4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpAC4,& Cyc_Tcutil_mk,_tmp9D4,1);_tmpAC4.fn_result=0;{struct Cyc_List_List*
a=_tmp9D5;for(0;a != 0;a=a->tl){struct _tuple9*_tmpAC5=(struct _tuple9*)a->hd;void*
_tmpAC6=(*_tmpAC5).f3;_tmpAC4=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAC4,& Cyc_Tcutil_mk,
_tmpAC6,1);((*_tmpAC5).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpAC5).f2).print_const,_tmpAC6);}}if(_tmp9D7 != 0){if(_tmp9D6){const char*
_tmp1292;void*_tmp1291;(_tmp1291=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1292="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1292,sizeof(char),29))),_tag_dyneither(_tmp1291,sizeof(void*),
0)));}{void*_tmpACA;int _tmpACB;struct Cyc_Absyn_VarargInfo _tmpAC9=*_tmp9D7;
_tmpACA=_tmpAC9.type;_tmpACB=_tmpAC9.inject;_tmpAC4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpAC4,& Cyc_Tcutil_mk,_tmpACA,1);(_tmp9D7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp9D7->tq).print_const,_tmpACA);if(_tmpACB){void*_tmpACC=Cyc_Tcutil_compress(
_tmpACA);struct Cyc_Absyn_PtrInfo _tmpACE;void*_tmpACF;struct Cyc_Absyn_PtrAtts
_tmpAD0;union Cyc_Absyn_Constraint*_tmpAD1;union Cyc_Absyn_Constraint*_tmpAD2;
_LL567: {struct Cyc_Absyn_PointerType_struct*_tmpACD=(struct Cyc_Absyn_PointerType_struct*)
_tmpACC;if(_tmpACD->tag != 5)goto _LL569;else{_tmpACE=_tmpACD->f1;_tmpACF=_tmpACE.elt_typ;
_tmpAD0=_tmpACE.ptr_atts;_tmpAD1=_tmpAD0.bounds;_tmpAD2=_tmpAD0.zero_term;}}
_LL568:{void*_tmpAD3=Cyc_Tcutil_compress(_tmpACF);_LL56C: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAD4=(struct Cyc_Absyn_DatatypeType_struct*)_tmpAD3;if(_tmpAD4->tag != 3)goto
_LL56E;}_LL56D: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpAD2)){const char*_tmp1295;void*_tmp1294;(_tmp1294=0,Cyc_Tcutil_terr(loc,((
_tmp1295="can't inject into a zeroterm pointer",_tag_dyneither(_tmp1295,sizeof(
char),37))),_tag_dyneither(_tmp1294,sizeof(void*),0)));}{void*_tmpAD7=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpAD1);_LL571: {struct Cyc_Absyn_DynEither_b_struct*_tmpAD8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAD7;if(_tmpAD8->tag != 0)goto _LL573;}_LL572:{const char*_tmp1298;void*_tmp1297;(
_tmp1297=0,Cyc_Tcutil_terr(loc,((_tmp1298="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp1298,sizeof(char),44))),_tag_dyneither(_tmp1297,sizeof(void*),
0)));}goto _LL570;_LL573:;_LL574: goto _LL570;_LL570:;}goto _LL56B;_LL56E:;_LL56F:{
const char*_tmp129B;void*_tmp129A;(_tmp129A=0,Cyc_Tcutil_terr(loc,((_tmp129B="can't inject a non-datatype type",
_tag_dyneither(_tmp129B,sizeof(char),33))),_tag_dyneither(_tmp129A,sizeof(void*),
0)));}goto _LL56B;_LL56B:;}goto _LL566;_LL569:;_LL56A:{const char*_tmp129E;void*
_tmp129D;(_tmp129D=0,Cyc_Tcutil_terr(loc,((_tmp129E="expecting a datatype pointer type",
_tag_dyneither(_tmp129E,sizeof(char),34))),_tag_dyneither(_tmp129D,sizeof(void*),
0)));}goto _LL566;_LL566:;}};}if(seen_format){int _tmpADF=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp9D5);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpADF) || 
fmt_arg_start < 0) || _tmp9D7 == 0  && fmt_arg_start != 0) || _tmp9D7 != 0  && 
fmt_arg_start != _tmpADF + 1){const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,Cyc_Tcutil_terr(
loc,((_tmp12A1="bad format descriptor",_tag_dyneither(_tmp12A1,sizeof(char),22))),
_tag_dyneither(_tmp12A0,sizeof(void*),0)));}else{void*_tmpAE3;struct _tuple9
_tmpAE2=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9D5,
fmt_desc_arg - 1);_tmpAE3=_tmpAE2.f3;{void*_tmpAE4=Cyc_Tcutil_compress(_tmpAE3);
struct Cyc_Absyn_PtrInfo _tmpAE6;void*_tmpAE7;struct Cyc_Absyn_PtrAtts _tmpAE8;union
Cyc_Absyn_Constraint*_tmpAE9;union Cyc_Absyn_Constraint*_tmpAEA;_LL576: {struct
Cyc_Absyn_PointerType_struct*_tmpAE5=(struct Cyc_Absyn_PointerType_struct*)
_tmpAE4;if(_tmpAE5->tag != 5)goto _LL578;else{_tmpAE6=_tmpAE5->f1;_tmpAE7=_tmpAE6.elt_typ;
_tmpAE8=_tmpAE6.ptr_atts;_tmpAE9=_tmpAE8.bounds;_tmpAEA=_tmpAE8.zero_term;}}
_LL577:{struct _tuple0 _tmp12A2;struct _tuple0 _tmpAEC=(_tmp12A2.f1=Cyc_Tcutil_compress(
_tmpAE7),((_tmp12A2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpAE9),_tmp12A2)));void*_tmpAED;enum Cyc_Absyn_Sign
_tmpAEF;enum Cyc_Absyn_Size_of _tmpAF0;void*_tmpAF1;_LL57B: _tmpAED=_tmpAEC.f1;{
struct Cyc_Absyn_IntType_struct*_tmpAEE=(struct Cyc_Absyn_IntType_struct*)_tmpAED;
if(_tmpAEE->tag != 6)goto _LL57D;else{_tmpAEF=_tmpAEE->f1;if(_tmpAEF != Cyc_Absyn_None)
goto _LL57D;_tmpAF0=_tmpAEE->f2;if(_tmpAF0 != Cyc_Absyn_Char_sz)goto _LL57D;}};
_tmpAF1=_tmpAEC.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpAF2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAF1;if(_tmpAF2->tag != 0)goto _LL57D;};_LL57C: goto _LL57A;_LL57D:;_LL57E:{const
char*_tmp12A5;void*_tmp12A4;(_tmp12A4=0,Cyc_Tcutil_terr(loc,((_tmp12A5="format descriptor is not a char ? type",
_tag_dyneither(_tmp12A5,sizeof(char),39))),_tag_dyneither(_tmp12A4,sizeof(void*),
0)));}goto _LL57A;_LL57A:;}goto _LL575;_LL578:;_LL579:{const char*_tmp12A8;void*
_tmp12A7;(_tmp12A7=0,Cyc_Tcutil_terr(loc,((_tmp12A8="format descriptor is not a char ? type",
_tag_dyneither(_tmp12A8,sizeof(char),39))),_tag_dyneither(_tmp12A7,sizeof(void*),
0)));}goto _LL575;_LL575:;}if(fmt_arg_start != 0){int problem;{struct _tuple27
_tmp12A9;struct _tuple27 _tmpAF8=(_tmp12A9.f1=ft,((_tmp12A9.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9D7))->type)),
_tmp12A9)));enum Cyc_Absyn_Format_Type _tmpAF9;void*_tmpAFA;struct Cyc_Absyn_DatatypeInfo
_tmpAFC;union Cyc_Absyn_DatatypeInfoU _tmpAFD;struct Cyc_Absyn_Datatypedecl**
_tmpAFE;struct Cyc_Absyn_Datatypedecl*_tmpAFF;enum Cyc_Absyn_Format_Type _tmpB00;
void*_tmpB01;struct Cyc_Absyn_DatatypeInfo _tmpB03;union Cyc_Absyn_DatatypeInfoU
_tmpB04;struct Cyc_Absyn_Datatypedecl**_tmpB05;struct Cyc_Absyn_Datatypedecl*
_tmpB06;_LL580: _tmpAF9=_tmpAF8.f1;if(_tmpAF9 != Cyc_Absyn_Printf_ft)goto _LL582;
_tmpAFA=_tmpAF8.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpAFB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpAFA;if(_tmpAFB->tag != 3)goto _LL582;else{_tmpAFC=_tmpAFB->f1;_tmpAFD=_tmpAFC.datatype_info;
if((_tmpAFD.KnownDatatype).tag != 2)goto _LL582;_tmpAFE=(struct Cyc_Absyn_Datatypedecl**)(
_tmpAFD.KnownDatatype).val;_tmpAFF=*_tmpAFE;}};_LL581: problem=Cyc_Absyn_qvar_cmp(
_tmpAFF->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL57F;_LL582: _tmpB00=
_tmpAF8.f1;if(_tmpB00 != Cyc_Absyn_Scanf_ft)goto _LL584;_tmpB01=_tmpAF8.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpB02=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpB01;if(_tmpB02->tag != 3)goto _LL584;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB03.datatype_info;
if((_tmpB04.KnownDatatype).tag != 2)goto _LL584;_tmpB05=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB04.KnownDatatype).val;_tmpB06=*_tmpB05;}};_LL583: problem=Cyc_Absyn_qvar_cmp(
_tmpB06->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL57F;_LL584:;_LL585:
problem=1;goto _LL57F;_LL57F:;}if(problem){const char*_tmp12AC;void*_tmp12AB;(
_tmp12AB=0,Cyc_Tcutil_terr(loc,((_tmp12AC="format attribute and vararg types don't match",
_tag_dyneither(_tmp12AC,sizeof(char),46))),_tag_dyneither(_tmp12AB,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmp9D8;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpB0A;void*_tmpB0B;void*_tmpB0C;struct _tuple0*_tmpB09=(struct _tuple0*)
rpo->hd;_tmpB0A=*_tmpB09;_tmpB0B=_tmpB0A.f1;_tmpB0C=_tmpB0A.f2;_tmpAC4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpAC4,& Cyc_Tcutil_ek,_tmpB0B,1);_tmpAC4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpAC4,& Cyc_Tcutil_trk,_tmpB0C,1);}}if(*_tmp9D3 != 0)_tmpAC4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpAC4,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp9D3))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpAC4.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB0E;int _tmpB0F;struct
_tuple25 _tmpB0D=*((struct _tuple25*)tvs->hd);_tmpB0E=_tmpB0D.f1;_tmpB0F=_tmpB0D.f2;
if(!_tmpB0F)continue;{void*_tmpB10=Cyc_Absyn_compress_kb(_tmpB0E->kind);struct
Cyc_Core_Opt*_tmpB12;struct Cyc_Core_Opt**_tmpB13;struct Cyc_Absyn_Kind*_tmpB14;
struct Cyc_Absyn_Kind*_tmpB16;struct Cyc_Absyn_Kind*_tmpB18;struct Cyc_Absyn_Kind
_tmpB19;enum Cyc_Absyn_KindQual _tmpB1A;struct Cyc_Absyn_Kind*_tmpB1C;struct Cyc_Absyn_Kind
_tmpB1D;enum Cyc_Absyn_KindQual _tmpB1E;struct Cyc_Core_Opt*_tmpB20;struct Cyc_Core_Opt**
_tmpB21;struct Cyc_Absyn_Kind*_tmpB22;struct Cyc_Absyn_Kind _tmpB23;enum Cyc_Absyn_KindQual
_tmpB24;struct Cyc_Absyn_Kind*_tmpB26;struct Cyc_Absyn_Kind _tmpB27;enum Cyc_Absyn_KindQual
_tmpB28;struct Cyc_Core_Opt*_tmpB2A;struct Cyc_Core_Opt**_tmpB2B;_LL587:{struct Cyc_Absyn_Less_kb_struct*
_tmpB11=(struct Cyc_Absyn_Less_kb_struct*)_tmpB10;if(_tmpB11->tag != 2)goto _LL589;
else{_tmpB12=_tmpB11->f1;_tmpB13=(struct Cyc_Core_Opt**)& _tmpB11->f1;_tmpB14=
_tmpB11->f2;}}if(!(_tmpB14->kind == Cyc_Absyn_RgnKind))goto _LL589;_LL588: if(
_tmpB14->aliasqual == Cyc_Absyn_Top){*_tmpB13=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpB16=_tmpB14;goto _LL58A;}*_tmpB13=Cyc_Tcutil_kind_to_bound_opt(_tmpB14);
_tmpB16=_tmpB14;goto _LL58A;_LL589:{struct Cyc_Absyn_Eq_kb_struct*_tmpB15=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpB10;if(_tmpB15->tag != 0)goto _LL58B;else{_tmpB16=
_tmpB15->f1;}}if(!(_tmpB16->kind == Cyc_Absyn_RgnKind))goto _LL58B;_LL58A:{struct
Cyc_Absyn_AccessEff_struct*_tmp12BB;struct Cyc_Absyn_VarType_struct*_tmp12BA;
struct Cyc_Absyn_VarType_struct _tmp12B9;struct Cyc_Absyn_AccessEff_struct _tmp12B8;
struct Cyc_List_List*_tmp12B7;effect=((_tmp12B7=_cycalloc(sizeof(*_tmp12B7)),((
_tmp12B7->hd=(void*)((_tmp12BB=_cycalloc(sizeof(*_tmp12BB)),((_tmp12BB[0]=((
_tmp12B8.tag=23,((_tmp12B8.f1=(void*)((void*)((_tmp12BA=_cycalloc(sizeof(*
_tmp12BA)),((_tmp12BA[0]=((_tmp12B9.tag=2,((_tmp12B9.f1=_tmpB0E,_tmp12B9)))),
_tmp12BA))))),_tmp12B8)))),_tmp12BB)))),((_tmp12B7->tl=effect,_tmp12B7))))));}
goto _LL586;_LL58B: {struct Cyc_Absyn_Less_kb_struct*_tmpB17=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB10;if(_tmpB17->tag != 2)goto _LL58D;else{_tmpB18=_tmpB17->f2;_tmpB19=*_tmpB18;
_tmpB1A=_tmpB19.kind;if(_tmpB1A != Cyc_Absyn_IntKind)goto _LL58D;}}_LL58C: goto
_LL58E;_LL58D: {struct Cyc_Absyn_Eq_kb_struct*_tmpB1B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB10;if(_tmpB1B->tag != 0)goto _LL58F;else{_tmpB1C=_tmpB1B->f1;_tmpB1D=*_tmpB1C;
_tmpB1E=_tmpB1D.kind;if(_tmpB1E != Cyc_Absyn_IntKind)goto _LL58F;}}_LL58E: goto
_LL586;_LL58F: {struct Cyc_Absyn_Less_kb_struct*_tmpB1F=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB10;if(_tmpB1F->tag != 2)goto _LL591;else{_tmpB20=_tmpB1F->f1;_tmpB21=(struct
Cyc_Core_Opt**)& _tmpB1F->f1;_tmpB22=_tmpB1F->f2;_tmpB23=*_tmpB22;_tmpB24=_tmpB23.kind;
if(_tmpB24 != Cyc_Absyn_EffKind)goto _LL591;}}_LL590:*_tmpB21=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL592;_LL591: {struct Cyc_Absyn_Eq_kb_struct*_tmpB25=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpB10;if(_tmpB25->tag != 0)goto _LL593;else{_tmpB26=
_tmpB25->f1;_tmpB27=*_tmpB26;_tmpB28=_tmpB27.kind;if(_tmpB28 != Cyc_Absyn_EffKind)
goto _LL593;}}_LL592:{struct Cyc_Absyn_VarType_struct*_tmp12C1;struct Cyc_Absyn_VarType_struct
_tmp12C0;struct Cyc_List_List*_tmp12BF;effect=((_tmp12BF=_cycalloc(sizeof(*
_tmp12BF)),((_tmp12BF->hd=(void*)((_tmp12C1=_cycalloc(sizeof(*_tmp12C1)),((
_tmp12C1[0]=((_tmp12C0.tag=2,((_tmp12C0.f1=_tmpB0E,_tmp12C0)))),_tmp12C1)))),((
_tmp12BF->tl=effect,_tmp12BF))))));}goto _LL586;_LL593: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB29=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB10;if(_tmpB29->tag != 1)goto
_LL595;else{_tmpB2A=_tmpB29->f1;_tmpB2B=(struct Cyc_Core_Opt**)& _tmpB29->f1;}}
_LL594:{struct Cyc_Absyn_Less_kb_struct*_tmp12C7;struct Cyc_Absyn_Less_kb_struct
_tmp12C6;struct Cyc_Core_Opt*_tmp12C5;*_tmpB2B=((_tmp12C5=_cycalloc(sizeof(*
_tmp12C5)),((_tmp12C5->v=(void*)((_tmp12C7=_cycalloc(sizeof(*_tmp12C7)),((
_tmp12C7[0]=((_tmp12C6.tag=2,((_tmp12C6.f1=0,((_tmp12C6.f2=& Cyc_Tcutil_ak,
_tmp12C6)))))),_tmp12C7)))),_tmp12C5))));}goto _LL596;_LL595:;_LL596:{struct Cyc_Absyn_RgnsEff_struct*
_tmp12D6;struct Cyc_Absyn_VarType_struct*_tmp12D5;struct Cyc_Absyn_VarType_struct
_tmp12D4;struct Cyc_Absyn_RgnsEff_struct _tmp12D3;struct Cyc_List_List*_tmp12D2;
effect=((_tmp12D2=_cycalloc(sizeof(*_tmp12D2)),((_tmp12D2->hd=(void*)((_tmp12D6=
_cycalloc(sizeof(*_tmp12D6)),((_tmp12D6[0]=((_tmp12D3.tag=25,((_tmp12D3.f1=(void*)((
void*)((_tmp12D5=_cycalloc(sizeof(*_tmp12D5)),((_tmp12D5[0]=((_tmp12D4.tag=2,((
_tmp12D4.f1=_tmpB0E,_tmp12D4)))),_tmp12D5))))),_tmp12D3)))),_tmp12D6)))),((
_tmp12D2->tl=effect,_tmp12D2))))));}goto _LL586;_LL586:;};}}{struct Cyc_List_List*
ts=_tmpAC4.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpB3D;int _tmpB3E;struct
_tuple26 _tmpB3C=*((struct _tuple26*)ts->hd);_tmpB3D=_tmpB3C.f1;_tmpB3E=_tmpB3C.f2;
if(!_tmpB3E)continue;{struct Cyc_Absyn_Kind*_tmpB3F=Cyc_Tcutil_typ_kind(_tmpB3D);
struct Cyc_Absyn_Kind _tmpB40;enum Cyc_Absyn_KindQual _tmpB41;struct Cyc_Absyn_Kind
_tmpB42;enum Cyc_Absyn_KindQual _tmpB43;struct Cyc_Absyn_Kind _tmpB44;enum Cyc_Absyn_KindQual
_tmpB45;_LL598: _tmpB40=*_tmpB3F;_tmpB41=_tmpB40.kind;if(_tmpB41 != Cyc_Absyn_RgnKind)
goto _LL59A;_LL599:{struct Cyc_Absyn_AccessEff_struct*_tmp12DC;struct Cyc_Absyn_AccessEff_struct
_tmp12DB;struct Cyc_List_List*_tmp12DA;effect=((_tmp12DA=_cycalloc(sizeof(*
_tmp12DA)),((_tmp12DA->hd=(void*)((_tmp12DC=_cycalloc(sizeof(*_tmp12DC)),((
_tmp12DC[0]=((_tmp12DB.tag=23,((_tmp12DB.f1=(void*)_tmpB3D,_tmp12DB)))),_tmp12DC)))),((
_tmp12DA->tl=effect,_tmp12DA))))));}goto _LL597;_LL59A: _tmpB42=*_tmpB3F;_tmpB43=
_tmpB42.kind;if(_tmpB43 != Cyc_Absyn_EffKind)goto _LL59C;_LL59B:{struct Cyc_List_List*
_tmp12DD;effect=((_tmp12DD=_cycalloc(sizeof(*_tmp12DD)),((_tmp12DD->hd=_tmpB3D,((
_tmp12DD->tl=effect,_tmp12DD))))));}goto _LL597;_LL59C: _tmpB44=*_tmpB3F;_tmpB45=
_tmpB44.kind;if(_tmpB45 != Cyc_Absyn_IntKind)goto _LL59E;_LL59D: goto _LL597;_LL59E:;
_LL59F:{struct Cyc_Absyn_RgnsEff_struct*_tmp12E3;struct Cyc_Absyn_RgnsEff_struct
_tmp12E2;struct Cyc_List_List*_tmp12E1;effect=((_tmp12E1=_cycalloc(sizeof(*
_tmp12E1)),((_tmp12E1->hd=(void*)((_tmp12E3=_cycalloc(sizeof(*_tmp12E3)),((
_tmp12E3[0]=((_tmp12E2.tag=25,((_tmp12E2.f1=(void*)_tmpB3D,_tmp12E2)))),_tmp12E3)))),((
_tmp12E1->tl=effect,_tmp12E1))))));}goto _LL597;_LL597:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp12E9;struct Cyc_Absyn_JoinEff_struct _tmp12E8;struct Cyc_Core_Opt*_tmp12E7;*
_tmp9D3=((_tmp12E7=_cycalloc(sizeof(*_tmp12E7)),((_tmp12E7->v=(void*)((_tmp12E9=
_cycalloc(sizeof(*_tmp12E9)),((_tmp12E9[0]=((_tmp12E8.tag=24,((_tmp12E8.f1=
effect,_tmp12E8)))),_tmp12E9)))),_tmp12E7))));};}if(*_tmp9D1 != 0){struct Cyc_List_List*
bs=*_tmp9D1;for(0;bs != 0;bs=bs->tl){void*_tmpB50=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpB52;struct Cyc_Core_Opt**
_tmpB53;struct Cyc_Core_Opt*_tmpB55;struct Cyc_Core_Opt**_tmpB56;struct Cyc_Absyn_Kind*
_tmpB57;struct Cyc_Absyn_Kind _tmpB58;enum Cyc_Absyn_KindQual _tmpB59;enum Cyc_Absyn_AliasQual
_tmpB5A;struct Cyc_Core_Opt*_tmpB5C;struct Cyc_Core_Opt**_tmpB5D;struct Cyc_Absyn_Kind*
_tmpB5E;struct Cyc_Absyn_Kind _tmpB5F;enum Cyc_Absyn_KindQual _tmpB60;enum Cyc_Absyn_AliasQual
_tmpB61;struct Cyc_Core_Opt*_tmpB63;struct Cyc_Core_Opt**_tmpB64;struct Cyc_Absyn_Kind*
_tmpB65;struct Cyc_Absyn_Kind _tmpB66;enum Cyc_Absyn_KindQual _tmpB67;enum Cyc_Absyn_AliasQual
_tmpB68;struct Cyc_Core_Opt*_tmpB6A;struct Cyc_Core_Opt**_tmpB6B;struct Cyc_Absyn_Kind*
_tmpB6C;struct Cyc_Absyn_Kind*_tmpB6E;struct Cyc_Absyn_Kind _tmpB6F;enum Cyc_Absyn_KindQual
_tmpB70;_LL5A1: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB51=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB50;if(_tmpB51->tag != 1)goto _LL5A3;else{_tmpB52=_tmpB51->f1;_tmpB53=(struct
Cyc_Core_Opt**)& _tmpB51->f1;}}_LL5A2:{const char*_tmp12ED;void*_tmp12EC[1];struct
Cyc_String_pa_struct _tmp12EB;(_tmp12EB.tag=0,((_tmp12EB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp12EC[0]=&
_tmp12EB,Cyc_Tcutil_warn(loc,((_tmp12ED="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp12ED,sizeof(char),47))),_tag_dyneither(_tmp12EC,sizeof(void*),
1)))))));}*_tmpB53=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL5A0;
_LL5A3: {struct Cyc_Absyn_Less_kb_struct*_tmpB54=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB50;if(_tmpB54->tag != 2)goto _LL5A5;else{_tmpB55=_tmpB54->f1;_tmpB56=(struct
Cyc_Core_Opt**)& _tmpB54->f1;_tmpB57=_tmpB54->f2;_tmpB58=*_tmpB57;_tmpB59=_tmpB58.kind;
if(_tmpB59 != Cyc_Absyn_MemKind)goto _LL5A5;_tmpB5A=_tmpB58.aliasqual;}}_LL5A4:{
struct Cyc_Absyn_Kind*_tmp12EE;*_tmpB56=Cyc_Tcutil_kind_to_bound_opt(((_tmp12EE=
_cycalloc_atomic(sizeof(*_tmp12EE)),((_tmp12EE->kind=Cyc_Absyn_BoxKind,((
_tmp12EE->aliasqual=_tmpB5A,_tmp12EE)))))));}goto _LL5A0;_LL5A5: {struct Cyc_Absyn_Less_kb_struct*
_tmpB5B=(struct Cyc_Absyn_Less_kb_struct*)_tmpB50;if(_tmpB5B->tag != 2)goto _LL5A7;
else{_tmpB5C=_tmpB5B->f1;_tmpB5D=(struct Cyc_Core_Opt**)& _tmpB5B->f1;_tmpB5E=
_tmpB5B->f2;_tmpB5F=*_tmpB5E;_tmpB60=_tmpB5F.kind;if(_tmpB60 != Cyc_Absyn_AnyKind)
goto _LL5A7;_tmpB61=_tmpB5F.aliasqual;}}_LL5A6:{struct Cyc_Absyn_Kind*_tmp12EF;*
_tmpB5D=Cyc_Tcutil_kind_to_bound_opt(((_tmp12EF=_cycalloc_atomic(sizeof(*
_tmp12EF)),((_tmp12EF->kind=Cyc_Absyn_BoxKind,((_tmp12EF->aliasqual=_tmpB61,
_tmp12EF)))))));}goto _LL5A0;_LL5A7: {struct Cyc_Absyn_Less_kb_struct*_tmpB62=(
struct Cyc_Absyn_Less_kb_struct*)_tmpB50;if(_tmpB62->tag != 2)goto _LL5A9;else{
_tmpB63=_tmpB62->f1;_tmpB64=(struct Cyc_Core_Opt**)& _tmpB62->f1;_tmpB65=_tmpB62->f2;
_tmpB66=*_tmpB65;_tmpB67=_tmpB66.kind;if(_tmpB67 != Cyc_Absyn_RgnKind)goto _LL5A9;
_tmpB68=_tmpB66.aliasqual;if(_tmpB68 != Cyc_Absyn_Top)goto _LL5A9;}}_LL5A8:*
_tmpB64=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL5A0;_LL5A9: {struct
Cyc_Absyn_Less_kb_struct*_tmpB69=(struct Cyc_Absyn_Less_kb_struct*)_tmpB50;if(
_tmpB69->tag != 2)goto _LL5AB;else{_tmpB6A=_tmpB69->f1;_tmpB6B=(struct Cyc_Core_Opt**)&
_tmpB69->f1;_tmpB6C=_tmpB69->f2;}}_LL5AA:*_tmpB6B=Cyc_Tcutil_kind_to_bound_opt(
_tmpB6C);goto _LL5A0;_LL5AB: {struct Cyc_Absyn_Eq_kb_struct*_tmpB6D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB50;if(_tmpB6D->tag != 0)goto _LL5AD;else{_tmpB6E=_tmpB6D->f1;_tmpB6F=*_tmpB6E;
_tmpB70=_tmpB6F.kind;if(_tmpB70 != Cyc_Absyn_MemKind)goto _LL5AD;}}_LL5AC:{const
char*_tmp12F2;void*_tmp12F1;(_tmp12F1=0,Cyc_Tcutil_terr(loc,((_tmp12F2="functions can't abstract M types",
_tag_dyneither(_tmp12F2,sizeof(char),33))),_tag_dyneither(_tmp12F1,sizeof(void*),
0)));}goto _LL5A0;_LL5AD:;_LL5AE: goto _LL5A0;_LL5A0:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpAC4.kind_env,*_tmp9D1);_tmpAC4.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpAC4.r,_tmpAC4.free_vars,*_tmp9D1);{struct Cyc_List_List*tvs=_tmpAC4.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB79;int _tmpB7A;struct
_tuple25 _tmpB78=*((struct _tuple25*)tvs->hd);_tmpB79=_tmpB78.f1;_tmpB7A=_tmpB78.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB79,_tmpB7A);}}{struct Cyc_List_List*evs=_tmpAC4.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpB7C;int _tmpB7D;struct _tuple26 _tmpB7B=*((struct _tuple26*)evs->hd);
_tmpB7C=_tmpB7B.f1;_tmpB7D=_tmpB7B.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpB7C,_tmpB7D);}};}goto _LL4D2;};}_LL4F1: {struct Cyc_Absyn_TupleType_struct*
_tmp9DA=(struct Cyc_Absyn_TupleType_struct*)_tmp99A;if(_tmp9DA->tag != 11)goto
_LL4F3;else{_tmp9DB=_tmp9DA->f1;}}_LL4F2: for(0;_tmp9DB != 0;_tmp9DB=_tmp9DB->tl){
struct _tuple11*_tmpB7F=(struct _tuple11*)_tmp9DB->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_mk,(*_tmpB7F).f2,1);((*_tmpB7F).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpB7F).f1).print_const,(*_tmpB7F).f2);}goto _LL4D2;_LL4F3: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9DC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp99A;if(_tmp9DC->tag != 13)goto
_LL4F5;else{_tmp9DD=_tmp9DC->f1;_tmp9DE=_tmp9DC->f2;}}_LL4F4: {struct
_RegionHandle*_tmpB80=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp9DE != 0;_tmp9DE=_tmp9DE->tl){struct Cyc_Absyn_Aggrfield _tmpB82;struct
_dyneither_ptr*_tmpB83;struct Cyc_Absyn_Tqual _tmpB84;struct Cyc_Absyn_Tqual*
_tmpB85;void*_tmpB86;struct Cyc_Absyn_Exp*_tmpB87;struct Cyc_List_List*_tmpB88;
struct Cyc_Absyn_Aggrfield*_tmpB81=(struct Cyc_Absyn_Aggrfield*)_tmp9DE->hd;
_tmpB82=*_tmpB81;_tmpB83=_tmpB82.name;_tmpB84=_tmpB82.tq;_tmpB85=(struct Cyc_Absyn_Tqual*)&(*
_tmpB81).tq;_tmpB86=_tmpB82.type;_tmpB87=_tmpB82.width;_tmpB88=_tmpB82.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB83)){
const char*_tmp12F6;void*_tmp12F5[1];struct Cyc_String_pa_struct _tmp12F4;(_tmp12F4.tag=
0,((_tmp12F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB83),((
_tmp12F5[0]=& _tmp12F4,Cyc_Tcutil_terr(loc,((_tmp12F6="duplicate field %s",
_tag_dyneither(_tmp12F6,sizeof(char),19))),_tag_dyneither(_tmp12F5,sizeof(void*),
1)))))));}{const char*_tmp12F7;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB83,((
_tmp12F7="",_tag_dyneither(_tmp12F7,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp12F8;prev_fields=((_tmp12F8=_region_malloc(_tmpB80,sizeof(*_tmp12F8)),((
_tmp12F8->hd=_tmpB83,((_tmp12F8->tl=prev_fields,_tmp12F8))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_mk,_tmpB86,1);_tmpB85->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB85->print_const,_tmpB86);if(_tmp9DD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpB86)){const char*_tmp12FC;void*_tmp12FB[1];struct Cyc_String_pa_struct _tmp12FA;(
_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpB83),((_tmp12FB[0]=& _tmp12FA,Cyc_Tcutil_warn(loc,((_tmp12FC="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp12FC,sizeof(char),74))),_tag_dyneither(_tmp12FB,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpB86,_tmpB87,_tmpB83);Cyc_Tcutil_check_field_atts(
loc,_tmpB83,_tmpB88);}}goto _LL4D2;}_LL4F5: {struct Cyc_Absyn_AggrType_struct*
_tmp9DF=(struct Cyc_Absyn_AggrType_struct*)_tmp99A;if(_tmp9DF->tag != 12)goto
_LL4F7;else{_tmp9E0=_tmp9DF->f1;_tmp9E1=_tmp9E0.aggr_info;_tmp9E2=(union Cyc_Absyn_AggrInfoU*)&(
_tmp9DF->f1).aggr_info;_tmp9E3=_tmp9E0.targs;_tmp9E4=(struct Cyc_List_List**)&(
_tmp9DF->f1).targs;}}_LL4F6:{union Cyc_Absyn_AggrInfoU _tmpB91=*_tmp9E2;struct
_tuple4 _tmpB92;enum Cyc_Absyn_AggrKind _tmpB93;struct _tuple2*_tmpB94;struct Cyc_Core_Opt*
_tmpB95;struct Cyc_Absyn_Aggrdecl**_tmpB96;struct Cyc_Absyn_Aggrdecl*_tmpB97;
_LL5B0: if((_tmpB91.UnknownAggr).tag != 1)goto _LL5B2;_tmpB92=(struct _tuple4)(
_tmpB91.UnknownAggr).val;_tmpB93=_tmpB92.f1;_tmpB94=_tmpB92.f2;_tmpB95=_tmpB92.f3;
_LL5B1: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpB98;_push_handler(&
_tmpB98);{int _tmpB9A=0;if(setjmp(_tmpB98.handler))_tmpB9A=1;if(!_tmpB9A){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB94);{struct Cyc_Absyn_Aggrdecl*_tmpB9B=*adp;if(_tmpB9B->kind != 
_tmpB93){if(_tmpB9B->kind == Cyc_Absyn_StructA){const char*_tmp1301;void*_tmp1300[
2];struct Cyc_String_pa_struct _tmp12FF;struct Cyc_String_pa_struct _tmp12FE;(
_tmp12FE.tag=0,((_tmp12FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB94)),((_tmp12FF.tag=0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB94)),((_tmp1300[0]=& _tmp12FF,((
_tmp1300[1]=& _tmp12FE,Cyc_Tcutil_terr(loc,((_tmp1301="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1301,sizeof(char),40))),_tag_dyneither(_tmp1300,sizeof(void*),
2)))))))))))));}else{const char*_tmp1306;void*_tmp1305[2];struct Cyc_String_pa_struct
_tmp1304;struct Cyc_String_pa_struct _tmp1303;(_tmp1303.tag=0,((_tmp1303.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB94)),((
_tmp1304.tag=0,((_tmp1304.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB94)),((_tmp1305[0]=& _tmp1304,((_tmp1305[1]=& _tmp1303,Cyc_Tcutil_terr(loc,((
_tmp1306="expecting union %s instead of struct %s",_tag_dyneither(_tmp1306,
sizeof(char),40))),_tag_dyneither(_tmp1305,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpB95  && (int)_tmpB95->v){if(!((unsigned int)_tmpB9B->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9B->impl))->tagged){const char*
_tmp130A;void*_tmp1309[1];struct Cyc_String_pa_struct _tmp1308;(_tmp1308.tag=0,((
_tmp1308.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB94)),((_tmp1309[0]=& _tmp1308,Cyc_Tcutil_terr(loc,((_tmp130A="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp130A,sizeof(char),42))),_tag_dyneither(_tmp1309,sizeof(void*),
1)))))));}}*_tmp9E2=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpB99=(void*)_exn_thrown;void*_tmpBA8=_tmpB99;_LL5B5: {struct Cyc_Dict_Absent_struct*
_tmpBA9=(struct Cyc_Dict_Absent_struct*)_tmpBA8;if(_tmpBA9->tag != Cyc_Dict_Absent)
goto _LL5B7;}_LL5B6: {struct Cyc_Tcenv_Genv*_tmpBAA=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp130B;struct Cyc_Absyn_Aggrdecl*_tmpBAB=(_tmp130B=
_cycalloc(sizeof(*_tmp130B)),((_tmp130B->kind=_tmpB93,((_tmp130B->sc=Cyc_Absyn_Extern,((
_tmp130B->name=_tmpB94,((_tmp130B->tvs=0,((_tmp130B->impl=0,((_tmp130B->attributes=
0,_tmp130B)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpBAA,loc,_tmpBAB);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB94);*_tmp9E2=Cyc_Absyn_KnownAggr(adp);if(*_tmp9E4 != 0){{const char*
_tmp130F;void*_tmp130E[1];struct Cyc_String_pa_struct _tmp130D;(_tmp130D.tag=0,((
_tmp130D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB94)),((_tmp130E[0]=& _tmp130D,Cyc_Tcutil_terr(loc,((_tmp130F="declare parameterized type %s before using",
_tag_dyneither(_tmp130F,sizeof(char),43))),_tag_dyneither(_tmp130E,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5B4;}_LL5B7:;_LL5B8:(void)_throw(_tmpBA8);_LL5B4:;}};}
_tmpB97=*adp;goto _LL5B3;}_LL5B2: if((_tmpB91.KnownAggr).tag != 2)goto _LL5AF;
_tmpB96=(struct Cyc_Absyn_Aggrdecl**)(_tmpB91.KnownAggr).val;_tmpB97=*_tmpB96;
_LL5B3: {struct Cyc_List_List*tvs=_tmpB97->tvs;struct Cyc_List_List*ts=*_tmp9E4;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpBB0=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpBB1=(void*)ts->hd;{struct _tuple0 _tmp1310;
struct _tuple0 _tmpBB3=(_tmp1310.f1=Cyc_Absyn_compress_kb(_tmpBB0->kind),((
_tmp1310.f2=_tmpBB1,_tmp1310)));void*_tmpBB4;void*_tmpBB6;struct Cyc_Absyn_Tvar*
_tmpBB8;_LL5BA: _tmpBB4=_tmpBB3.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBB5=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpBB4;if(_tmpBB5->tag != 1)goto _LL5BC;};
_tmpBB6=_tmpBB3.f2;{struct Cyc_Absyn_VarType_struct*_tmpBB7=(struct Cyc_Absyn_VarType_struct*)
_tmpBB6;if(_tmpBB7->tag != 2)goto _LL5BC;else{_tmpBB8=_tmpBB7->f1;}};_LL5BB: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBB8);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBB8,1);continue;_LL5BC:;_LL5BD: goto _LL5B9;_LL5B9:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1314;void*_tmp1313[1];struct Cyc_String_pa_struct _tmp1312;(
_tmp1312.tag=0,((_tmp1312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB97->name)),((_tmp1313[0]=& _tmp1312,Cyc_Tcutil_terr(loc,((_tmp1314="too many parameters for type %s",
_tag_dyneither(_tmp1314,sizeof(char),32))),_tag_dyneither(_tmp1313,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1315;hidden_ts=((
_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315->hd=e,((_tmp1315->tl=hidden_ts,
_tmp1315))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}*
_tmp9E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp9E4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL5AF:;}goto _LL4D2;_LL4F7: {struct Cyc_Absyn_TypedefType_struct*
_tmp9E5=(struct Cyc_Absyn_TypedefType_struct*)_tmp99A;if(_tmp9E5->tag != 18)goto
_LL4F9;else{_tmp9E6=_tmp9E5->f1;_tmp9E7=_tmp9E5->f2;_tmp9E8=(struct Cyc_List_List**)&
_tmp9E5->f2;_tmp9E9=_tmp9E5->f3;_tmp9EA=(struct Cyc_Absyn_Typedefdecl**)& _tmp9E5->f3;
_tmp9EB=_tmp9E5->f4;_tmp9EC=(void***)& _tmp9E5->f4;}}_LL4F8: {struct Cyc_List_List*
targs=*_tmp9E8;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpBBD;
_push_handler(& _tmpBBD);{int _tmpBBF=0;if(setjmp(_tmpBBD.handler))_tmpBBF=1;if(!
_tmpBBF){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9E6);;_pop_handler();}else{
void*_tmpBBE=(void*)_exn_thrown;void*_tmpBC1=_tmpBBE;_LL5BF: {struct Cyc_Dict_Absent_struct*
_tmpBC2=(struct Cyc_Dict_Absent_struct*)_tmpBC1;if(_tmpBC2->tag != Cyc_Dict_Absent)
goto _LL5C1;}_LL5C0:{const char*_tmp1319;void*_tmp1318[1];struct Cyc_String_pa_struct
_tmp1317;(_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E6)),((_tmp1318[0]=& _tmp1317,Cyc_Tcutil_terr(
loc,((_tmp1319="unbound typedef name %s",_tag_dyneither(_tmp1319,sizeof(char),24))),
_tag_dyneither(_tmp1318,sizeof(void*),1)))))));}return cvtenv;_LL5C1:;_LL5C2:(
void)_throw(_tmpBC1);_LL5BE:;}};}*_tmp9EA=(struct Cyc_Absyn_Typedefdecl*)td;
_tmp9E6[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpBC6=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple14*_tmp131C;struct Cyc_List_List*
_tmp131B;inst=((_tmp131B=_region_malloc(_tmpBC6,sizeof(*_tmp131B)),((_tmp131B->hd=((
_tmp131C=_region_malloc(_tmpBC6,sizeof(*_tmp131C)),((_tmp131C->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp131C->f2=(void*)ts->hd,_tmp131C)))))),((_tmp131B->tl=inst,_tmp131B))))));};}
if(ts != 0){const char*_tmp1320;void*_tmp131F[1];struct Cyc_String_pa_struct
_tmp131E;(_tmp131E.tag=0,((_tmp131E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E6)),((_tmp131F[0]=& _tmp131E,Cyc_Tcutil_terr(
loc,((_tmp1320="too many parameters for typedef %s",_tag_dyneither(_tmp1320,
sizeof(char),35))),_tag_dyneither(_tmp131F,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);void*e=Cyc_Absyn_new_evar(
0,0);{struct Cyc_List_List*_tmp1321;hidden_ts=((_tmp1321=_cycalloc(sizeof(*
_tmp1321)),((_tmp1321->hd=e,((_tmp1321->tl=hidden_ts,_tmp1321))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple14*_tmp1324;struct Cyc_List_List*_tmp1323;inst=(
struct Cyc_List_List*)((_tmp1323=_cycalloc(sizeof(*_tmp1323)),((_tmp1323->hd=(
struct _tuple14*)((_tmp1324=_cycalloc(sizeof(*_tmp1324)),((_tmp1324->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1324->f2=e,_tmp1324)))))),((_tmp1323->tl=inst,
_tmp1323))))));};}*_tmp9E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpBC6,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1325;*_tmp9EC=((_tmp1325=_cycalloc(sizeof(*
_tmp1325)),((_tmp1325[0]=new_typ,_tmp1325))));}}goto _LL4D2;};}_LL4F9: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp9ED=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp99A;if(_tmp9ED->tag != 22)goto
_LL4FB;}_LL4FA: goto _LL4FC;_LL4FB: {struct Cyc_Absyn_HeapRgn_struct*_tmp9EE=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp99A;if(_tmp9EE->tag != 21)goto _LL4FD;}_LL4FC:
goto _LL4D2;_LL4FD: {struct Cyc_Absyn_RgnHandleType_struct*_tmp9EF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp99A;if(_tmp9EF->tag != 16)goto _LL4FF;else{_tmp9F0=(void*)_tmp9EF->f1;}}_LL4FE:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp9F0,1);
goto _LL4D2;_LL4FF: {struct Cyc_Absyn_DynRgnType_struct*_tmp9F1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp99A;if(_tmp9F1->tag != 17)goto _LL501;else{_tmp9F2=(void*)_tmp9F1->f1;_tmp9F3=(
void*)_tmp9F1->f2;}}_LL500: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmp9F2,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmp9F3,1);goto _LL4D2;_LL501: {struct Cyc_Absyn_AccessEff_struct*_tmp9F4=(struct
Cyc_Absyn_AccessEff_struct*)_tmp99A;if(_tmp9F4->tag != 23)goto _LL503;else{_tmp9F5=(
void*)_tmp9F4->f1;}}_LL502: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmp9F5,1);goto _LL4D2;_LL503: {struct Cyc_Absyn_RgnsEff_struct*
_tmp9F6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp99A;if(_tmp9F6->tag != 25)goto _LL505;
else{_tmp9F7=(void*)_tmp9F6->f1;}}_LL504: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ak,_tmp9F7,1);goto _LL4D2;_LL505: {struct Cyc_Absyn_JoinEff_struct*
_tmp9F8=(struct Cyc_Absyn_JoinEff_struct*)_tmp99A;if(_tmp9F8->tag != 24)goto _LL4D2;
else{_tmp9F9=_tmp9F8->f1;}}_LL506: for(0;_tmp9F9 != 0;_tmp9F9=_tmp9F9->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp9F9->hd,1);}
goto _LL4D2;_LL4D2:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpBD0=t;struct Cyc_Core_Opt*_tmpBD2;struct Cyc_Core_Opt*_tmpBD3;_LL5C4: {
struct Cyc_Absyn_Evar_struct*_tmpBD1=(struct Cyc_Absyn_Evar_struct*)_tmpBD0;if(
_tmpBD1->tag != 1)goto _LL5C6;else{_tmpBD2=_tmpBD1->f1;_tmpBD3=_tmpBD1->f2;}}
_LL5C5: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpBD4=_tmpBD2;struct Cyc_Core_Opt
_tmpBD5;struct Cyc_Absyn_Kind*_tmpBD6;_LL5C9: if(_tmpBD4 != 0)goto _LL5CB;_LL5CA:{
const char*_tmp1326;s=((_tmp1326="kind=NULL ",_tag_dyneither(_tmp1326,sizeof(char),
11)));}goto _LL5C8;_LL5CB: if(_tmpBD4 == 0)goto _LL5C8;_tmpBD5=*_tmpBD4;_tmpBD6=(
struct Cyc_Absyn_Kind*)_tmpBD5.v;_LL5CC:{const char*_tmp132A;void*_tmp1329[1];
struct Cyc_String_pa_struct _tmp1328;s=(struct _dyneither_ptr)((_tmp1328.tag=0,((
_tmp1328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpBD6)),((_tmp1329[0]=& _tmp1328,Cyc_aprintf(((_tmp132A="kind=%s ",
_tag_dyneither(_tmp132A,sizeof(char),9))),_tag_dyneither(_tmp1329,sizeof(void*),
1))))))));}goto _LL5C8;_LL5C8:;}{struct Cyc_Core_Opt*_tmpBDB=_tmpBD3;struct Cyc_Core_Opt
_tmpBDC;void*_tmpBDD;_LL5CE: if(_tmpBDB != 0)goto _LL5D0;_LL5CF:{const char*_tmp132E;
void*_tmp132D[1];struct Cyc_String_pa_struct _tmp132C;s=(struct _dyneither_ptr)((
_tmp132C.tag=0,((_tmp132C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp132D[0]=& _tmp132C,Cyc_aprintf(((_tmp132E="%s ref=NULL",_tag_dyneither(
_tmp132E,sizeof(char),12))),_tag_dyneither(_tmp132D,sizeof(void*),1))))))));}
goto _LL5CD;_LL5D0: if(_tmpBDB == 0)goto _LL5CD;_tmpBDC=*_tmpBDB;_tmpBDD=(void*)
_tmpBDC.v;_LL5D1:{const char*_tmp1333;void*_tmp1332[2];struct Cyc_String_pa_struct
_tmp1331;struct Cyc_String_pa_struct _tmp1330;s=(struct _dyneither_ptr)((_tmp1330.tag=
0,((_tmp1330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpBDD)),((_tmp1331.tag=0,((_tmp1331.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp1332[0]=& _tmp1331,((_tmp1332[1]=& _tmp1330,Cyc_aprintf(((
_tmp1333="%s ref=%s",_tag_dyneither(_tmp1333,sizeof(char),10))),_tag_dyneither(
_tmp1332,sizeof(void*),2))))))))))))));}goto _LL5CD;_LL5CD:;}{const char*_tmp1337;
void*_tmp1336[1];struct Cyc_String_pa_struct _tmp1335;(_tmp1335.tag=0,((_tmp1335.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1336[0]=& _tmp1335,Cyc_fprintf(
Cyc_stderr,((_tmp1337="evar info: %s\n",_tag_dyneither(_tmp1337,sizeof(char),15))),
_tag_dyneither(_tmp1336,sizeof(void*),1)))))));}goto _LL5C3;}_LL5C6:;_LL5C7: goto
_LL5C3;_LL5C3:;}{const char*_tmp133D;void*_tmp133C[3];struct Cyc_String_pa_struct
_tmp133B;struct Cyc_String_pa_struct _tmp133A;struct Cyc_String_pa_struct _tmp1339;(
_tmp1339.tag=0,((_tmp1339.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp133A.tag=0,((_tmp133A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp133B.tag=0,((
_tmp133B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp133C[0]=& _tmp133B,((_tmp133C[1]=& _tmp133A,((_tmp133C[2]=& _tmp1339,Cyc_Tcutil_terr(
loc,((_tmp133D="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp133D,sizeof(char),51))),_tag_dyneither(_tmp133C,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpBED=e->r;
struct Cyc_List_List*_tmpBF3;struct Cyc_Absyn_Exp*_tmpBF5;struct Cyc_Absyn_Exp*
_tmpBF6;struct Cyc_Absyn_Exp*_tmpBF7;struct Cyc_Absyn_Exp*_tmpBF9;struct Cyc_Absyn_Exp*
_tmpBFA;struct Cyc_Absyn_Exp*_tmpBFC;struct Cyc_Absyn_Exp*_tmpBFD;struct Cyc_Absyn_Exp*
_tmpBFF;struct Cyc_Absyn_Exp*_tmpC00;void*_tmpC02;struct Cyc_Absyn_Exp*_tmpC03;
void*_tmpC05;void*_tmpC07;void*_tmpC09;struct Cyc_Absyn_Exp*_tmpC0B;_LL5D3: {
struct Cyc_Absyn_Const_e_struct*_tmpBEE=(struct Cyc_Absyn_Const_e_struct*)_tmpBED;
if(_tmpBEE->tag != 0)goto _LL5D5;}_LL5D4: goto _LL5D6;_LL5D5: {struct Cyc_Absyn_Enum_e_struct*
_tmpBEF=(struct Cyc_Absyn_Enum_e_struct*)_tmpBED;if(_tmpBEF->tag != 32)goto _LL5D7;}
_LL5D6: goto _LL5D8;_LL5D7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpBF0=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBED;if(_tmpBF0->tag != 33)goto _LL5D9;}_LL5D8: goto _LL5DA;_LL5D9: {struct Cyc_Absyn_Var_e_struct*
_tmpBF1=(struct Cyc_Absyn_Var_e_struct*)_tmpBED;if(_tmpBF1->tag != 1)goto _LL5DB;}
_LL5DA: goto _LL5D2;_LL5DB: {struct Cyc_Absyn_Primop_e_struct*_tmpBF2=(struct Cyc_Absyn_Primop_e_struct*)
_tmpBED;if(_tmpBF2->tag != 3)goto _LL5DD;else{_tmpBF3=_tmpBF2->f2;}}_LL5DC: for(0;
_tmpBF3 != 0;_tmpBF3=_tmpBF3->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpBF3->hd,te,cvtenv);}goto _LL5D2;_LL5DD: {struct Cyc_Absyn_Conditional_e_struct*
_tmpBF4=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBED;if(_tmpBF4->tag != 6)goto
_LL5DF;else{_tmpBF5=_tmpBF4->f1;_tmpBF6=_tmpBF4->f2;_tmpBF7=_tmpBF4->f3;}}_LL5DE:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBF5,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBF6,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBF7,te,
cvtenv);goto _LL5D2;_LL5DF: {struct Cyc_Absyn_And_e_struct*_tmpBF8=(struct Cyc_Absyn_And_e_struct*)
_tmpBED;if(_tmpBF8->tag != 7)goto _LL5E1;else{_tmpBF9=_tmpBF8->f1;_tmpBFA=_tmpBF8->f2;}}
_LL5E0: _tmpBFC=_tmpBF9;_tmpBFD=_tmpBFA;goto _LL5E2;_LL5E1: {struct Cyc_Absyn_Or_e_struct*
_tmpBFB=(struct Cyc_Absyn_Or_e_struct*)_tmpBED;if(_tmpBFB->tag != 8)goto _LL5E3;
else{_tmpBFC=_tmpBFB->f1;_tmpBFD=_tmpBFB->f2;}}_LL5E2: _tmpBFF=_tmpBFC;_tmpC00=
_tmpBFD;goto _LL5E4;_LL5E3: {struct Cyc_Absyn_SeqExp_e_struct*_tmpBFE=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBED;if(_tmpBFE->tag != 9)goto _LL5E5;else{_tmpBFF=_tmpBFE->f1;_tmpC00=_tmpBFE->f2;}}
_LL5E4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBFF,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC00,te,cvtenv);goto _LL5D2;_LL5E5: {
struct Cyc_Absyn_Cast_e_struct*_tmpC01=(struct Cyc_Absyn_Cast_e_struct*)_tmpBED;
if(_tmpC01->tag != 15)goto _LL5E7;else{_tmpC02=(void*)_tmpC01->f1;_tmpC03=_tmpC01->f2;}}
_LL5E6: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC03,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ak,_tmpC02,1);goto
_LL5D2;_LL5E7: {struct Cyc_Absyn_Offsetof_e_struct*_tmpC04=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBED;if(_tmpC04->tag != 20)goto _LL5E9;else{_tmpC05=(void*)_tmpC04->f1;}}_LL5E8:
_tmpC07=_tmpC05;goto _LL5EA;_LL5E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC06=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBED;if(_tmpC06->tag != 18)goto _LL5EB;
else{_tmpC07=(void*)_tmpC06->f1;}}_LL5EA: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_ak,_tmpC07,1);goto _LL5D2;_LL5EB: {struct Cyc_Absyn_Valueof_e_struct*
_tmpC08=(struct Cyc_Absyn_Valueof_e_struct*)_tmpBED;if(_tmpC08->tag != 39)goto
_LL5ED;else{_tmpC09=(void*)_tmpC08->f1;}}_LL5EC: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC09,1);goto _LL5D2;_LL5ED: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpC0A=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBED;if(_tmpC0A->tag != 19)goto
_LL5EF;else{_tmpC0B=_tmpC0A->f1;}}_LL5EE: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpC0B,te,cvtenv);goto _LL5D2;_LL5EF:;_LL5F0: {const char*_tmp1340;void*_tmp133F;(
_tmp133F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1340="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1340,sizeof(char),66))),_tag_dyneither(_tmp133F,sizeof(void*),
0)));}_LL5D2:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpC0E=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpC10;struct
_tuple25 _tmpC0F=*((struct _tuple25*)vs->hd);_tmpC10=_tmpC0F.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpC0E,_tmpC10);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpC12;struct _tuple26 _tmpC11=*((struct _tuple26*)evs->hd);_tmpC12=_tmpC11.f1;{
void*_tmpC13=Cyc_Tcutil_compress(_tmpC12);struct Cyc_Core_Opt*_tmpC15;struct Cyc_Core_Opt**
_tmpC16;_LL5F2: {struct Cyc_Absyn_Evar_struct*_tmpC14=(struct Cyc_Absyn_Evar_struct*)
_tmpC13;if(_tmpC14->tag != 1)goto _LL5F4;else{_tmpC15=_tmpC14->f4;_tmpC16=(struct
Cyc_Core_Opt**)& _tmpC14->f4;}}_LL5F3: if(*_tmpC16 == 0){struct Cyc_Core_Opt*
_tmp1341;*_tmpC16=((_tmp1341=_cycalloc(sizeof(*_tmp1341)),((_tmp1341->v=_tmpC0E,
_tmp1341))));}else{struct Cyc_List_List*_tmpC18=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpC16))->v;struct Cyc_List_List*_tmpC19=0;for(0;_tmpC18 != 0;
_tmpC18=_tmpC18->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpC0E,(struct Cyc_Absyn_Tvar*)_tmpC18->hd)){struct Cyc_List_List*_tmp1342;
_tmpC19=((_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342->hd=(struct Cyc_Absyn_Tvar*)
_tmpC18->hd,((_tmp1342->tl=_tmpC19,_tmp1342))))));}}{struct Cyc_Core_Opt*_tmp1343;*
_tmpC16=((_tmp1343=_cycalloc(sizeof(*_tmp1343)),((_tmp1343->v=_tmpC19,_tmp1343))));};}
goto _LL5F1;_LL5F4:;_LL5F5: goto _LL5F1;_LL5F1:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpC1C=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpC1D=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp1344;struct Cyc_Tcutil_CVTEnv _tmpC1E=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp1344.r=_tmpC1D,((_tmp1344.kind_env=_tmpC1C,((_tmp1344.free_vars=0,((_tmp1344.free_evars=
0,((_tmp1344.generalize_evars=generalize_evars,((_tmp1344.fn_result=0,_tmp1344)))))))))))),&
Cyc_Tcutil_mk,t);struct Cyc_List_List*_tmpC1F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC1D,(struct Cyc_Absyn_Tvar*(*)(struct _tuple25*))Cyc_Core_fst,
_tmpC1E.free_vars);struct Cyc_List_List*_tmpC20=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpC1D,(void*(*)(struct _tuple26*))Cyc_Core_fst,_tmpC1E.free_evars);if(_tmpC1C != 
0){struct Cyc_List_List*_tmpC21=0;{struct Cyc_List_List*_tmpC22=_tmpC1F;for(0;(
unsigned int)_tmpC22;_tmpC22=_tmpC22->tl){struct Cyc_Absyn_Tvar*_tmpC23=(struct
Cyc_Absyn_Tvar*)_tmpC22->hd;int found=0;{struct Cyc_List_List*_tmpC24=_tmpC1C;for(
0;(unsigned int)_tmpC24;_tmpC24=_tmpC24->tl){if(Cyc_Absyn_tvar_cmp(_tmpC23,(
struct Cyc_Absyn_Tvar*)_tmpC24->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp1345;_tmpC21=((_tmp1345=_region_malloc(_tmpC1D,sizeof(*_tmp1345)),((_tmp1345->hd=(
struct Cyc_Absyn_Tvar*)_tmpC22->hd,((_tmp1345->tl=_tmpC21,_tmp1345))))));}}}
_tmpC1F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpC21);}{struct Cyc_List_List*x=_tmpC1F;for(0;x != 0;x=x->tl){void*_tmpC26=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpC28;struct Cyc_Core_Opt**
_tmpC29;struct Cyc_Core_Opt*_tmpC2B;struct Cyc_Core_Opt**_tmpC2C;struct Cyc_Absyn_Kind*
_tmpC2D;struct Cyc_Absyn_Kind _tmpC2E;enum Cyc_Absyn_KindQual _tmpC2F;enum Cyc_Absyn_AliasQual
_tmpC30;struct Cyc_Core_Opt*_tmpC32;struct Cyc_Core_Opt**_tmpC33;struct Cyc_Absyn_Kind*
_tmpC34;struct Cyc_Absyn_Kind _tmpC35;enum Cyc_Absyn_KindQual _tmpC36;enum Cyc_Absyn_AliasQual
_tmpC37;struct Cyc_Core_Opt*_tmpC39;struct Cyc_Core_Opt**_tmpC3A;struct Cyc_Absyn_Kind*
_tmpC3B;struct Cyc_Absyn_Kind*_tmpC3D;struct Cyc_Absyn_Kind _tmpC3E;enum Cyc_Absyn_KindQual
_tmpC3F;_LL5F7: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC27=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC26;if(_tmpC27->tag != 1)goto _LL5F9;else{_tmpC28=_tmpC27->f1;_tmpC29=(struct
Cyc_Core_Opt**)& _tmpC27->f1;}}_LL5F8:*_tmpC29=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);
goto _LL5F6;_LL5F9: {struct Cyc_Absyn_Less_kb_struct*_tmpC2A=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC26;if(_tmpC2A->tag != 2)goto _LL5FB;else{_tmpC2B=_tmpC2A->f1;_tmpC2C=(struct
Cyc_Core_Opt**)& _tmpC2A->f1;_tmpC2D=_tmpC2A->f2;_tmpC2E=*_tmpC2D;_tmpC2F=_tmpC2E.kind;
if(_tmpC2F != Cyc_Absyn_MemKind)goto _LL5FB;_tmpC30=_tmpC2E.aliasqual;}}_LL5FA:{
struct Cyc_Absyn_Kind*_tmp1346;*_tmpC2C=Cyc_Tcutil_kind_to_bound_opt(((_tmp1346=
_cycalloc_atomic(sizeof(*_tmp1346)),((_tmp1346->kind=Cyc_Absyn_BoxKind,((
_tmp1346->aliasqual=_tmpC30,_tmp1346)))))));}goto _LL5F6;_LL5FB: {struct Cyc_Absyn_Less_kb_struct*
_tmpC31=(struct Cyc_Absyn_Less_kb_struct*)_tmpC26;if(_tmpC31->tag != 2)goto _LL5FD;
else{_tmpC32=_tmpC31->f1;_tmpC33=(struct Cyc_Core_Opt**)& _tmpC31->f1;_tmpC34=
_tmpC31->f2;_tmpC35=*_tmpC34;_tmpC36=_tmpC35.kind;if(_tmpC36 != Cyc_Absyn_RgnKind)
goto _LL5FD;_tmpC37=_tmpC35.aliasqual;if(_tmpC37 != Cyc_Absyn_Top)goto _LL5FD;}}
_LL5FC:*_tmpC33=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL5F6;_LL5FD: {
struct Cyc_Absyn_Less_kb_struct*_tmpC38=(struct Cyc_Absyn_Less_kb_struct*)_tmpC26;
if(_tmpC38->tag != 2)goto _LL5FF;else{_tmpC39=_tmpC38->f1;_tmpC3A=(struct Cyc_Core_Opt**)&
_tmpC38->f1;_tmpC3B=_tmpC38->f2;}}_LL5FE:*_tmpC3A=Cyc_Tcutil_kind_to_bound_opt(
_tmpC3B);goto _LL5F6;_LL5FF: {struct Cyc_Absyn_Eq_kb_struct*_tmpC3C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC26;if(_tmpC3C->tag != 0)goto _LL601;else{_tmpC3D=_tmpC3C->f1;_tmpC3E=*_tmpC3D;
_tmpC3F=_tmpC3E.kind;if(_tmpC3F != Cyc_Absyn_MemKind)goto _LL601;}}_LL600:{const
char*_tmp134A;void*_tmp1349[1];struct Cyc_String_pa_struct _tmp1348;(_tmp1348.tag=
0,((_tmp1348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1349[0]=& _tmp1348,Cyc_Tcutil_terr(loc,((
_tmp134A="type variable %s cannot have kind M",_tag_dyneither(_tmp134A,sizeof(
char),36))),_tag_dyneither(_tmp1349,sizeof(void*),1)))))));}goto _LL5F6;_LL601:;
_LL602: goto _LL5F6;_LL5F6:;}}if(_tmpC1F != 0  || _tmpC20 != 0){{void*_tmpC44=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpC46;struct Cyc_List_List*_tmpC47;struct Cyc_List_List**
_tmpC48;struct Cyc_Core_Opt*_tmpC49;void*_tmpC4A;struct Cyc_List_List*_tmpC4B;int
_tmpC4C;struct Cyc_Absyn_VarargInfo*_tmpC4D;struct Cyc_List_List*_tmpC4E;struct Cyc_List_List*
_tmpC4F;_LL604: {struct Cyc_Absyn_FnType_struct*_tmpC45=(struct Cyc_Absyn_FnType_struct*)
_tmpC44;if(_tmpC45->tag != 10)goto _LL606;else{_tmpC46=_tmpC45->f1;_tmpC47=_tmpC46.tvars;
_tmpC48=(struct Cyc_List_List**)&(_tmpC45->f1).tvars;_tmpC49=_tmpC46.effect;
_tmpC4A=_tmpC46.ret_typ;_tmpC4B=_tmpC46.args;_tmpC4C=_tmpC46.c_varargs;_tmpC4D=
_tmpC46.cyc_varargs;_tmpC4E=_tmpC46.rgn_po;_tmpC4F=_tmpC46.attributes;}}_LL605:
if(*_tmpC48 == 0){*_tmpC48=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(
_tmpC1F);_tmpC1F=0;}goto _LL603;_LL606:;_LL607: goto _LL603;_LL603:;}if(_tmpC1F != 0){
const char*_tmp134E;void*_tmp134D[1];struct Cyc_String_pa_struct _tmp134C;(_tmp134C.tag=
0,((_tmp134C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmpC1F->hd)->name),((_tmp134D[0]=& _tmp134C,Cyc_Tcutil_terr(loc,((_tmp134E="unbound type variable %s",
_tag_dyneither(_tmp134E,sizeof(char),25))),_tag_dyneither(_tmp134D,sizeof(void*),
1)))))));}if(_tmpC20 != 0)for(0;_tmpC20 != 0;_tmpC20=_tmpC20->tl){void*e=(void*)
_tmpC20->hd;struct Cyc_Absyn_Kind*_tmpC53=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpC54;enum Cyc_Absyn_KindQual _tmpC55;enum Cyc_Absyn_AliasQual _tmpC56;struct Cyc_Absyn_Kind
_tmpC57;enum Cyc_Absyn_KindQual _tmpC58;enum Cyc_Absyn_AliasQual _tmpC59;struct Cyc_Absyn_Kind
_tmpC5A;enum Cyc_Absyn_KindQual _tmpC5B;enum Cyc_Absyn_AliasQual _tmpC5C;struct Cyc_Absyn_Kind
_tmpC5D;enum Cyc_Absyn_KindQual _tmpC5E;_LL609: _tmpC54=*_tmpC53;_tmpC55=_tmpC54.kind;
if(_tmpC55 != Cyc_Absyn_RgnKind)goto _LL60B;_tmpC56=_tmpC54.aliasqual;if(_tmpC56 != 
Cyc_Absyn_Unique)goto _LL60B;_LL60A: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1351;void*_tmp1350;(_tmp1350=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1351="can't unify evar with unique region!",
_tag_dyneither(_tmp1351,sizeof(char),37))),_tag_dyneither(_tmp1350,sizeof(void*),
0)));}goto _LL608;_LL60B: _tmpC57=*_tmpC53;_tmpC58=_tmpC57.kind;if(_tmpC58 != Cyc_Absyn_RgnKind)
goto _LL60D;_tmpC59=_tmpC57.aliasqual;if(_tmpC59 != Cyc_Absyn_Aliasable)goto _LL60D;
_LL60C: goto _LL60E;_LL60D: _tmpC5A=*_tmpC53;_tmpC5B=_tmpC5A.kind;if(_tmpC5B != Cyc_Absyn_RgnKind)
goto _LL60F;_tmpC5C=_tmpC5A.aliasqual;if(_tmpC5C != Cyc_Absyn_Top)goto _LL60F;
_LL60E: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1354;
void*_tmp1353;(_tmp1353=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1354="can't unify evar with heap!",_tag_dyneither(
_tmp1354,sizeof(char),28))),_tag_dyneither(_tmp1353,sizeof(void*),0)));}goto
_LL608;_LL60F: _tmpC5D=*_tmpC53;_tmpC5E=_tmpC5D.kind;if(_tmpC5E != Cyc_Absyn_EffKind)
goto _LL611;_LL610: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1357;void*_tmp1356;(_tmp1356=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1357="can't unify evar with {}!",
_tag_dyneither(_tmp1357,sizeof(char),26))),_tag_dyneither(_tmp1356,sizeof(void*),
0)));}goto _LL608;_LL611:;_LL612:{const char*_tmp135C;void*_tmp135B[2];struct Cyc_String_pa_struct
_tmp135A;struct Cyc_String_pa_struct _tmp1359;(_tmp1359.tag=0,((_tmp1359.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp135A.tag=0,((_tmp135A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp135B[0]=& _tmp135A,((_tmp135B[1]=& _tmp1359,Cyc_Tcutil_terr(loc,((
_tmp135C="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp135C,sizeof(char),52))),_tag_dyneither(_tmp135B,sizeof(void*),2)))))))))))));}
goto _LL608;_LL608:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpC6A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpC6C;struct Cyc_List_List*
_tmpC6D;struct Cyc_Core_Opt*_tmpC6E;void*_tmpC6F;_LL614: {struct Cyc_Absyn_FnType_struct*
_tmpC6B=(struct Cyc_Absyn_FnType_struct*)_tmpC6A;if(_tmpC6B->tag != 10)goto _LL616;
else{_tmpC6C=_tmpC6B->f1;_tmpC6D=_tmpC6C.tvars;_tmpC6E=_tmpC6C.effect;_tmpC6F=
_tmpC6C.ret_typ;}}_LL615: fd->tvs=_tmpC6D;fd->effect=_tmpC6E;goto _LL613;_LL616:;
_LL617: {const char*_tmp135F;void*_tmp135E;(_tmp135E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135F="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp135F,sizeof(char),38))),_tag_dyneither(_tmp135E,sizeof(void*),
0)));}_LL613:;}{struct _RegionHandle*_tmpC72=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp1360;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple21*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC72,(struct _dyneither_ptr*(*)(struct _tuple21*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp1360="function declaration has repeated parameter",
_tag_dyneither(_tmp1360,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp1361;fd->cached_typ=((
_tmp1361=_cycalloc(sizeof(*_tmp1361)),((_tmp1361->v=t,_tmp1361))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpC75=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp1362;
struct Cyc_Tcutil_CVTEnv _tmpC76=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1362.r=
_tmpC75,((_tmp1362.kind_env=bound_tvars,((_tmp1362.free_vars=0,((_tmp1362.free_evars=
0,((_tmp1362.generalize_evars=0,((_tmp1362.fn_result=0,_tmp1362)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpC77=Cyc_Tcutil_remove_bound_tvars(
_tmpC75,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC75,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple25*))Cyc_Core_fst,_tmpC76.free_vars),bound_tvars);struct Cyc_List_List*
_tmpC78=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple26*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC75,(void*(*)(struct _tuple26*))Cyc_Core_fst,
_tmpC76.free_evars);{struct Cyc_List_List*fs=_tmpC77;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC79=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp1367;void*_tmp1366[2];struct Cyc_String_pa_struct _tmp1365;struct Cyc_String_pa_struct
_tmp1364;(_tmp1364.tag=0,((_tmp1364.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1365.tag=0,((_tmp1365.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpC79),((_tmp1366[0]=& _tmp1365,((
_tmp1366[1]=& _tmp1364,Cyc_Tcutil_terr(loc,((_tmp1367="unbound type variable %s in type %s",
_tag_dyneither(_tmp1367,sizeof(char),36))),_tag_dyneither(_tmp1366,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpC78 != 0)for(0;_tmpC78 != 0;_tmpC78=
_tmpC78->tl){void*e=(void*)_tmpC78->hd;struct Cyc_Absyn_Kind*_tmpC7E=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpC7F;enum Cyc_Absyn_KindQual _tmpC80;enum Cyc_Absyn_AliasQual
_tmpC81;struct Cyc_Absyn_Kind _tmpC82;enum Cyc_Absyn_KindQual _tmpC83;enum Cyc_Absyn_AliasQual
_tmpC84;struct Cyc_Absyn_Kind _tmpC85;enum Cyc_Absyn_KindQual _tmpC86;enum Cyc_Absyn_AliasQual
_tmpC87;struct Cyc_Absyn_Kind _tmpC88;enum Cyc_Absyn_KindQual _tmpC89;_LL619:
_tmpC7F=*_tmpC7E;_tmpC80=_tmpC7F.kind;if(_tmpC80 != Cyc_Absyn_RgnKind)goto _LL61B;
_tmpC81=_tmpC7F.aliasqual;if(_tmpC81 != Cyc_Absyn_Unique)goto _LL61B;_LL61A: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp136A;void*
_tmp1369;(_tmp1369=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp136A="can't unify evar with unique region!",
_tag_dyneither(_tmp136A,sizeof(char),37))),_tag_dyneither(_tmp1369,sizeof(void*),
0)));}goto _LL618;_LL61B: _tmpC82=*_tmpC7E;_tmpC83=_tmpC82.kind;if(_tmpC83 != Cyc_Absyn_RgnKind)
goto _LL61D;_tmpC84=_tmpC82.aliasqual;if(_tmpC84 != Cyc_Absyn_Aliasable)goto _LL61D;
_LL61C: goto _LL61E;_LL61D: _tmpC85=*_tmpC7E;_tmpC86=_tmpC85.kind;if(_tmpC86 != Cyc_Absyn_RgnKind)
goto _LL61F;_tmpC87=_tmpC85.aliasqual;if(_tmpC87 != Cyc_Absyn_Top)goto _LL61F;
_LL61E: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp136D;
void*_tmp136C;(_tmp136C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp136D="can't unify evar with heap!",_tag_dyneither(
_tmp136D,sizeof(char),28))),_tag_dyneither(_tmp136C,sizeof(void*),0)));}goto
_LL618;_LL61F: _tmpC88=*_tmpC7E;_tmpC89=_tmpC88.kind;if(_tmpC89 != Cyc_Absyn_EffKind)
goto _LL621;_LL620: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1370;void*_tmp136F;(_tmp136F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1370="can't unify evar with {}!",
_tag_dyneither(_tmp1370,sizeof(char),26))),_tag_dyneither(_tmp136F,sizeof(void*),
0)));}goto _LL618;_LL621:;_LL622:{const char*_tmp1375;void*_tmp1374[2];struct Cyc_String_pa_struct
_tmp1373;struct Cyc_String_pa_struct _tmp1372;(_tmp1372.tag=0,((_tmp1372.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1373.tag=0,((_tmp1373.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1374[0]=& _tmp1373,((_tmp1374[1]=& _tmp1372,Cyc_Tcutil_terr(loc,((
_tmp1375="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1375,sizeof(char),52))),_tag_dyneither(_tmp1374,sizeof(void*),2)))))))))))));}
goto _LL618;_LL618:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct
Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp137A;void*_tmp1379[2];struct Cyc_String_pa_struct
_tmp1378;struct Cyc_String_pa_struct _tmp1377;(_tmp1377.tag=0,((_tmp1377.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1378.tag=
0,((_tmp1378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1379[0]=&
_tmp1378,((_tmp1379[1]=& _tmp1377,Cyc_Tcutil_terr(loc,((_tmp137A="%s: %s",
_tag_dyneither(_tmp137A,sizeof(char),7))),_tag_dyneither(_tmp1379,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s);static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((
void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct
_dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,
vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(struct
Cyc_Position_Segment*loc,struct Cyc_List_List*tvs);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp137B;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp137B="duplicate type variable",
_tag_dyneither(_tmp137B,sizeof(char),24))));}struct _tuple28{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple29{struct Cyc_List_List*f1;void*f2;};struct _tuple30{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp137C;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp137C="",_tag_dyneither(_tmp137C,sizeof(char),1))))!= 0){struct _tuple28*
_tmp137F;struct Cyc_List_List*_tmp137E;fields=((_tmp137E=_cycalloc(sizeof(*
_tmp137E)),((_tmp137E->hd=((_tmp137F=_cycalloc(sizeof(*_tmp137F)),((_tmp137F->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp137F->f2=0,_tmp137F)))))),((
_tmp137E->tl=fields,_tmp137E))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp1381;const char*_tmp1380;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1381="struct",
_tag_dyneither(_tmp1381,sizeof(char),7)):((_tmp1380="union",_tag_dyneither(
_tmp1380,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple29 _tmpC9E;struct Cyc_List_List*_tmpC9F;void*_tmpCA0;struct _tuple29*
_tmpC9D=(struct _tuple29*)des->hd;_tmpC9E=*_tmpC9D;_tmpC9F=_tmpC9E.f1;_tmpCA0=
_tmpC9E.f2;if(_tmpC9F == 0){struct Cyc_List_List*_tmpCA1=fields;for(0;_tmpCA1 != 0;
_tmpCA1=_tmpCA1->tl){if(!(*((struct _tuple28*)_tmpCA1->hd)).f2){(*((struct
_tuple28*)_tmpCA1->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp1387;struct
Cyc_Absyn_FieldName_struct _tmp1386;struct Cyc_List_List*_tmp1385;(*((struct
_tuple29*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1385=_cycalloc(sizeof(*
_tmp1385)),((_tmp1385->hd=(void*)((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((
_tmp1387[0]=((_tmp1386.tag=1,((_tmp1386.f1=((*((struct _tuple28*)_tmpCA1->hd)).f1)->name,
_tmp1386)))),_tmp1387)))),((_tmp1385->tl=0,_tmp1385))))));}{struct _tuple30*
_tmp138A;struct Cyc_List_List*_tmp1389;ans=((_tmp1389=_region_malloc(rgn,sizeof(*
_tmp1389)),((_tmp1389->hd=((_tmp138A=_region_malloc(rgn,sizeof(*_tmp138A)),((
_tmp138A->f1=(*((struct _tuple28*)_tmpCA1->hd)).f1,((_tmp138A->f2=_tmpCA0,
_tmp138A)))))),((_tmp1389->tl=ans,_tmp1389))))));}break;}}if(_tmpCA1 == 0){const
char*_tmp138E;void*_tmp138D[1];struct Cyc_String_pa_struct _tmp138C;(_tmp138C.tag=
0,((_tmp138C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp138D[0]=& _tmp138C,Cyc_Tcutil_terr(loc,((_tmp138E="too many arguments to %s",
_tag_dyneither(_tmp138E,sizeof(char),25))),_tag_dyneither(_tmp138D,sizeof(void*),
1)))))));}}else{if(_tmpC9F->tl != 0){const char*_tmp1391;void*_tmp1390;(_tmp1390=0,
Cyc_Tcutil_terr(loc,((_tmp1391="multiple designators are not yet supported",
_tag_dyneither(_tmp1391,sizeof(char),43))),_tag_dyneither(_tmp1390,sizeof(void*),
0)));}else{void*_tmpCAC=(void*)_tmpC9F->hd;struct _dyneither_ptr*_tmpCAF;_LL624: {
struct Cyc_Absyn_ArrayElement_struct*_tmpCAD=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpCAC;if(_tmpCAD->tag != 0)goto _LL626;}_LL625:{const char*_tmp1395;void*_tmp1394[
1];struct Cyc_String_pa_struct _tmp1393;(_tmp1393.tag=0,((_tmp1393.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1394[0]=& _tmp1393,Cyc_Tcutil_terr(
loc,((_tmp1395="array designator used in argument to %s",_tag_dyneither(_tmp1395,
sizeof(char),40))),_tag_dyneither(_tmp1394,sizeof(void*),1)))))));}goto _LL623;
_LL626: {struct Cyc_Absyn_FieldName_struct*_tmpCAE=(struct Cyc_Absyn_FieldName_struct*)
_tmpCAC;if(_tmpCAE->tag != 1)goto _LL623;else{_tmpCAF=_tmpCAE->f1;}}_LL627: {
struct Cyc_List_List*_tmpCB3=fields;for(0;_tmpCB3 != 0;_tmpCB3=_tmpCB3->tl){if(Cyc_strptrcmp(
_tmpCAF,((*((struct _tuple28*)_tmpCB3->hd)).f1)->name)== 0){if((*((struct _tuple28*)
_tmpCB3->hd)).f2){const char*_tmp1399;void*_tmp1398[1];struct Cyc_String_pa_struct
_tmp1397;(_tmp1397.tag=0,((_tmp1397.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpCAF),((_tmp1398[0]=& _tmp1397,Cyc_Tcutil_terr(loc,((_tmp1399="member %s has already been used as an argument",
_tag_dyneither(_tmp1399,sizeof(char),47))),_tag_dyneither(_tmp1398,sizeof(void*),
1)))))));}(*((struct _tuple28*)_tmpCB3->hd)).f2=1;{struct _tuple30*_tmp139C;struct
Cyc_List_List*_tmp139B;ans=((_tmp139B=_region_malloc(rgn,sizeof(*_tmp139B)),((
_tmp139B->hd=((_tmp139C=_region_malloc(rgn,sizeof(*_tmp139C)),((_tmp139C->f1=(*((
struct _tuple28*)_tmpCB3->hd)).f1,((_tmp139C->f2=_tmpCA0,_tmp139C)))))),((
_tmp139B->tl=ans,_tmp139B))))));}break;}}if(_tmpCB3 == 0){const char*_tmp13A0;void*
_tmp139F[1];struct Cyc_String_pa_struct _tmp139E;(_tmp139E.tag=0,((_tmp139E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCAF),((_tmp139F[0]=& _tmp139E,
Cyc_Tcutil_terr(loc,((_tmp13A0="bad field designator %s",_tag_dyneither(_tmp13A0,
sizeof(char),24))),_tag_dyneither(_tmp139F,sizeof(void*),1)))))));}goto _LL623;}
_LL623:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple28*)fields->hd)).f2){{const char*_tmp13A3;void*_tmp13A2;(
_tmp13A2=0,Cyc_Tcutil_terr(loc,((_tmp13A3="too few arguments to struct",
_tag_dyneither(_tmp13A3,sizeof(char),28))),_tag_dyneither(_tmp13A2,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple28*)fields->hd)).f2){if(found){const char*_tmp13A6;void*_tmp13A5;(_tmp13A5=
0,Cyc_Tcutil_terr(loc,((_tmp13A6="only one member of a union is allowed",
_tag_dyneither(_tmp13A6,sizeof(char),38))),_tag_dyneither(_tmp13A5,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp13A9;void*_tmp13A8;(_tmp13A8=0,Cyc_Tcutil_terr(
loc,((_tmp13A9="missing member for union",_tag_dyneither(_tmp13A9,sizeof(char),
25))),_tag_dyneither(_tmp13A8,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpCC4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpCC6;
void*_tmpCC7;struct Cyc_Absyn_PtrAtts _tmpCC8;union Cyc_Absyn_Constraint*_tmpCC9;
_LL629: {struct Cyc_Absyn_PointerType_struct*_tmpCC5=(struct Cyc_Absyn_PointerType_struct*)
_tmpCC4;if(_tmpCC5->tag != 5)goto _LL62B;else{_tmpCC6=_tmpCC5->f1;_tmpCC7=_tmpCC6.elt_typ;
_tmpCC8=_tmpCC6.ptr_atts;_tmpCC9=_tmpCC8.bounds;}}_LL62A: {void*_tmpCCA=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpCC9);_LL62E: {struct Cyc_Absyn_DynEither_b_struct*_tmpCCB=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpCCA;if(_tmpCCB->tag != 0)goto _LL630;}_LL62F:*elt_typ_dest=_tmpCC7;return 1;
_LL630:;_LL631: return 0;_LL62D:;}_LL62B:;_LL62C: return 0;_LL628:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpCCC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpCCE;
void*_tmpCCF;struct Cyc_Absyn_PtrAtts _tmpCD0;union Cyc_Absyn_Constraint*_tmpCD1;
_LL633: {struct Cyc_Absyn_PointerType_struct*_tmpCCD=(struct Cyc_Absyn_PointerType_struct*)
_tmpCCC;if(_tmpCCD->tag != 5)goto _LL635;else{_tmpCCE=_tmpCCD->f1;_tmpCCF=_tmpCCE.elt_typ;
_tmpCD0=_tmpCCE.ptr_atts;_tmpCD1=_tmpCD0.zero_term;}}_LL634:*elt_typ_dest=
_tmpCCF;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpCD1);_LL635:;_LL636: return 0;_LL632:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpCD2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpCD4;void*_tmpCD5;struct Cyc_Absyn_PtrAtts _tmpCD6;
union Cyc_Absyn_Constraint*_tmpCD7;union Cyc_Absyn_Constraint*_tmpCD8;struct Cyc_Absyn_ArrayInfo
_tmpCDA;void*_tmpCDB;struct Cyc_Absyn_Tqual _tmpCDC;struct Cyc_Absyn_Exp*_tmpCDD;
union Cyc_Absyn_Constraint*_tmpCDE;_LL638: {struct Cyc_Absyn_PointerType_struct*
_tmpCD3=(struct Cyc_Absyn_PointerType_struct*)_tmpCD2;if(_tmpCD3->tag != 5)goto
_LL63A;else{_tmpCD4=_tmpCD3->f1;_tmpCD5=_tmpCD4.elt_typ;_tmpCD6=_tmpCD4.ptr_atts;
_tmpCD7=_tmpCD6.bounds;_tmpCD8=_tmpCD6.zero_term;}}_LL639: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD8)){*ptr_type=t;*elt_type=
_tmpCD5;{void*_tmpCDF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpCD7);_LL63F: {struct Cyc_Absyn_DynEither_b_struct*
_tmpCE0=(struct Cyc_Absyn_DynEither_b_struct*)_tmpCDF;if(_tmpCE0->tag != 0)goto
_LL641;}_LL640:*is_dyneither=1;goto _LL63E;_LL641:;_LL642:*is_dyneither=0;goto
_LL63E;_LL63E:;}return 1;}else{return 0;}_LL63A: {struct Cyc_Absyn_ArrayType_struct*
_tmpCD9=(struct Cyc_Absyn_ArrayType_struct*)_tmpCD2;if(_tmpCD9->tag != 9)goto
_LL63C;else{_tmpCDA=_tmpCD9->f1;_tmpCDB=_tmpCDA.elt_type;_tmpCDC=_tmpCDA.tq;
_tmpCDD=_tmpCDA.num_elts;_tmpCDE=_tmpCDA.zero_term;}}_LL63B: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCDE)){*elt_type=_tmpCDB;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp13BE;struct Cyc_Absyn_PtrAtts
_tmp13BD;struct Cyc_Absyn_Upper_b_struct _tmp13BC;struct Cyc_Absyn_Upper_b_struct*
_tmp13BB;struct Cyc_Absyn_PtrInfo _tmp13BA;struct Cyc_Absyn_PointerType_struct*
_tmp13B9;*ptr_type=(void*)((_tmp13B9=_cycalloc(sizeof(*_tmp13B9)),((_tmp13B9[0]=((
_tmp13BE.tag=5,((_tmp13BE.f1=((_tmp13BA.elt_typ=_tmpCDB,((_tmp13BA.elt_tq=
_tmpCDC,((_tmp13BA.ptr_atts=((_tmp13BD.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp13BD.nullable=Cyc_Absyn_false_conref,((_tmp13BD.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13BB=_cycalloc(sizeof(*_tmp13BB)),((
_tmp13BB[0]=((_tmp13BC.tag=1,((_tmp13BC.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpCDD),_tmp13BC)))),_tmp13BB))))),((_tmp13BD.zero_term=_tmpCDE,((_tmp13BD.ptrloc=
0,_tmp13BD)))))))))),_tmp13BA)))))),_tmp13BE)))),_tmp13B9))));}return 1;}else{
return 0;}_LL63C:;_LL63D: return 0;_LL637:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpCE7=e1->r;struct Cyc_Absyn_Exp*_tmpCEA;struct Cyc_Absyn_Exp*_tmpCEC;struct Cyc_Absyn_Exp*
_tmpCEE;struct Cyc_Absyn_Exp*_tmpCF0;struct Cyc_Absyn_Exp*_tmpCF2;struct Cyc_Absyn_Exp*
_tmpCF4;_LL644: {struct Cyc_Absyn_Cast_e_struct*_tmpCE8=(struct Cyc_Absyn_Cast_e_struct*)
_tmpCE7;if(_tmpCE8->tag != 15)goto _LL646;}_LL645: {const char*_tmp13C2;void*
_tmp13C1[1];struct Cyc_String_pa_struct _tmp13C0;(_tmp13C0.tag=0,((_tmp13C0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp13C1[0]=& _tmp13C0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp13C2="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp13C2,sizeof(char),29))),_tag_dyneither(_tmp13C1,sizeof(void*),1)))))));}
_LL646: {struct Cyc_Absyn_Deref_e_struct*_tmpCE9=(struct Cyc_Absyn_Deref_e_struct*)
_tmpCE7;if(_tmpCE9->tag != 21)goto _LL648;else{_tmpCEA=_tmpCE9->f1;}}_LL647:
_tmpCEC=_tmpCEA;goto _LL649;_LL648: {struct Cyc_Absyn_Subscript_e_struct*_tmpCEB=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCE7;if(_tmpCEB->tag != 24)goto _LL64A;
else{_tmpCEC=_tmpCEB->f1;}}_LL649: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpCEC->topt))->v,ptr_type,is_dyneither,
elt_type);_LL64A: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpCED=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpCE7;if(_tmpCED->tag != 23)goto _LL64C;else{_tmpCEE=_tmpCED->f1;}}_LL64B:
_tmpCF0=_tmpCEE;goto _LL64D;_LL64C: {struct Cyc_Absyn_AggrMember_e_struct*_tmpCEF=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpCE7;if(_tmpCEF->tag != 22)goto _LL64E;
else{_tmpCF0=_tmpCEF->f1;}}_LL64D: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpCF0->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp13C6;void*_tmp13C5[1];struct Cyc_String_pa_struct _tmp13C4;(_tmp13C4.tag=
0,((_tmp13C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp13C5[0]=& _tmp13C4,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13C6="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp13C6,sizeof(char),51))),_tag_dyneither(_tmp13C5,sizeof(void*),
1)))))));}return 0;_LL64E: {struct Cyc_Absyn_Instantiate_e_struct*_tmpCF1=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpCE7;if(_tmpCF1->tag != 14)goto _LL650;else{
_tmpCF2=_tmpCF1->f1;}}_LL64F: _tmpCF4=_tmpCF2;goto _LL651;_LL650: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpCF3=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpCE7;if(_tmpCF3->tag != 13)
goto _LL652;else{_tmpCF4=_tmpCF3->f1;}}_LL651: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpCF4->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp13CA;void*_tmp13C9[1];struct Cyc_String_pa_struct
_tmp13C8;(_tmp13C8.tag=0,((_tmp13C8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp13C9[0]=& _tmp13C8,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13CA="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp13CA,sizeof(char),49))),_tag_dyneither(_tmp13C9,sizeof(void*),
1)))))));}return 0;_LL652: {struct Cyc_Absyn_Var_e_struct*_tmpCF5=(struct Cyc_Absyn_Var_e_struct*)
_tmpCE7;if(_tmpCF5->tag != 1)goto _LL654;}_LL653: return 0;_LL654:;_LL655: {const
char*_tmp13CE;void*_tmp13CD[1];struct Cyc_String_pa_struct _tmp13CC;(_tmp13CC.tag=
0,((_tmp13CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp13CD[0]=& _tmp13CC,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13CE="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp13CE,sizeof(char),39))),_tag_dyneither(_tmp13CD,sizeof(void*),
1)))))));}_LL643:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpD02=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpD05;_LL657: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpD03=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpD02;if(_tmpD03->tag != 22)goto _LL659;}_LL658: return 1;_LL659: {struct Cyc_Absyn_VarType_struct*
_tmpD04=(struct Cyc_Absyn_VarType_struct*)_tmpD02;if(_tmpD04->tag != 2)goto _LL65B;
else{_tmpD05=_tmpD04->f1;}}_LL65A: {struct Cyc_Absyn_Kind _tmpD07;enum Cyc_Absyn_KindQual
_tmpD08;enum Cyc_Absyn_AliasQual _tmpD09;struct Cyc_Absyn_Kind*_tmpD06=Cyc_Tcutil_tvar_kind(
_tmpD05,& Cyc_Tcutil_rk);_tmpD07=*_tmpD06;_tmpD08=_tmpD07.kind;_tmpD09=_tmpD07.aliasqual;
if(_tmpD08 == Cyc_Absyn_RgnKind  && (_tmpD09 == Cyc_Absyn_Unique  || _tmpD09 == Cyc_Absyn_Top)){
void*_tmpD0A=Cyc_Absyn_compress_kb(_tmpD05->kind);struct Cyc_Core_Opt*_tmpD0C;
struct Cyc_Core_Opt**_tmpD0D;struct Cyc_Absyn_Kind*_tmpD0E;struct Cyc_Absyn_Kind
_tmpD0F;enum Cyc_Absyn_KindQual _tmpD10;enum Cyc_Absyn_AliasQual _tmpD11;_LL65E: {
struct Cyc_Absyn_Less_kb_struct*_tmpD0B=(struct Cyc_Absyn_Less_kb_struct*)_tmpD0A;
if(_tmpD0B->tag != 2)goto _LL660;else{_tmpD0C=_tmpD0B->f1;_tmpD0D=(struct Cyc_Core_Opt**)&
_tmpD0B->f1;_tmpD0E=_tmpD0B->f2;_tmpD0F=*_tmpD0E;_tmpD10=_tmpD0F.kind;if(_tmpD10
!= Cyc_Absyn_RgnKind)goto _LL660;_tmpD11=_tmpD0F.aliasqual;if(_tmpD11 != Cyc_Absyn_Top)
goto _LL660;}}_LL65F:{struct Cyc_Absyn_Less_kb_struct*_tmp13D4;struct Cyc_Absyn_Less_kb_struct
_tmp13D3;struct Cyc_Core_Opt*_tmp13D2;*_tmpD0D=((_tmp13D2=_cycalloc(sizeof(*
_tmp13D2)),((_tmp13D2->v=(void*)((_tmp13D4=_cycalloc(sizeof(*_tmp13D4)),((
_tmp13D4[0]=((_tmp13D3.tag=2,((_tmp13D3.f1=0,((_tmp13D3.f2=& Cyc_Tcutil_rk,
_tmp13D3)))))),_tmp13D4)))),_tmp13D2))));}return 0;_LL660:;_LL661: return 1;_LL65D:;}
return 0;}_LL65B:;_LL65C: return 0;_LL656:;}int Cyc_Tcutil_is_noalias_pointer(void*t);
int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpD15=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpD17;struct Cyc_Absyn_PtrAtts _tmpD18;void*_tmpD19;
_LL663: {struct Cyc_Absyn_PointerType_struct*_tmpD16=(struct Cyc_Absyn_PointerType_struct*)
_tmpD15;if(_tmpD16->tag != 5)goto _LL665;else{_tmpD17=_tmpD16->f1;_tmpD18=_tmpD17.ptr_atts;
_tmpD19=_tmpD18.rgn;}}_LL664: return Cyc_Tcutil_is_noalias_region(_tmpD19);_LL665:;
_LL666: return 0;_LL662:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpD1A=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpD1A))return 1;{void*_tmpD1B=_tmpD1A;struct Cyc_List_List*_tmpD1D;struct Cyc_Absyn_AggrInfo
_tmpD1F;union Cyc_Absyn_AggrInfoU _tmpD20;struct Cyc_Absyn_Aggrdecl**_tmpD21;struct
Cyc_List_List*_tmpD22;struct Cyc_List_List*_tmpD24;struct Cyc_Absyn_AggrInfo
_tmpD26;union Cyc_Absyn_AggrInfoU _tmpD27;struct _tuple4 _tmpD28;struct Cyc_Absyn_DatatypeInfo
_tmpD2A;union Cyc_Absyn_DatatypeInfoU _tmpD2B;struct Cyc_List_List*_tmpD2C;struct
Cyc_Absyn_DatatypeFieldInfo _tmpD2E;union Cyc_Absyn_DatatypeFieldInfoU _tmpD2F;
struct Cyc_List_List*_tmpD30;_LL668: {struct Cyc_Absyn_TupleType_struct*_tmpD1C=(
struct Cyc_Absyn_TupleType_struct*)_tmpD1B;if(_tmpD1C->tag != 11)goto _LL66A;else{
_tmpD1D=_tmpD1C->f1;}}_LL669: while(_tmpD1D != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpD1D->hd)).f2))return 1;_tmpD1D=_tmpD1D->tl;}return 0;
_LL66A: {struct Cyc_Absyn_AggrType_struct*_tmpD1E=(struct Cyc_Absyn_AggrType_struct*)
_tmpD1B;if(_tmpD1E->tag != 12)goto _LL66C;else{_tmpD1F=_tmpD1E->f1;_tmpD20=_tmpD1F.aggr_info;
if((_tmpD20.KnownAggr).tag != 2)goto _LL66C;_tmpD21=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD20.KnownAggr).val;_tmpD22=_tmpD1F.targs;}}_LL66B: if((*_tmpD21)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpD31=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpD21)->tvs,_tmpD22);struct Cyc_List_List*_tmpD32=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD21)->impl))->fields;void*t;
while(_tmpD32 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpD31,((struct Cyc_Absyn_Aggrfield*)
_tmpD32->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD32=_tmpD32->tl;}return 0;}_LL66C: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD23=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD1B;if(_tmpD23->tag != 13)goto
_LL66E;else{_tmpD24=_tmpD23->f2;}}_LL66D: while(_tmpD24 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD24->hd)->type))return 1;_tmpD24=_tmpD24->tl;}
return 0;_LL66E: {struct Cyc_Absyn_AggrType_struct*_tmpD25=(struct Cyc_Absyn_AggrType_struct*)
_tmpD1B;if(_tmpD25->tag != 12)goto _LL670;else{_tmpD26=_tmpD25->f1;_tmpD27=_tmpD26.aggr_info;
if((_tmpD27.UnknownAggr).tag != 1)goto _LL670;_tmpD28=(struct _tuple4)(_tmpD27.UnknownAggr).val;}}
_LL66F: {const char*_tmp13D7;void*_tmp13D6;(_tmp13D6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13D7="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp13D7,sizeof(char),36))),_tag_dyneither(_tmp13D6,sizeof(void*),
0)));}_LL670: {struct Cyc_Absyn_DatatypeType_struct*_tmpD29=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpD1B;if(_tmpD29->tag != 3)goto _LL672;else{_tmpD2A=_tmpD29->f1;_tmpD2B=_tmpD2A.datatype_info;
_tmpD2C=_tmpD2A.targs;}}_LL671: {union Cyc_Absyn_DatatypeInfoU _tmpD35=_tmpD2B;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpD36;struct _tuple2*_tmpD37;int _tmpD38;
struct Cyc_Absyn_Datatypedecl**_tmpD39;struct Cyc_Absyn_Datatypedecl*_tmpD3A;
struct Cyc_Absyn_Datatypedecl _tmpD3B;struct Cyc_List_List*_tmpD3C;struct Cyc_Core_Opt*
_tmpD3D;_LL677: if((_tmpD35.UnknownDatatype).tag != 1)goto _LL679;_tmpD36=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpD35.UnknownDatatype).val;_tmpD37=_tmpD36.name;
_tmpD38=_tmpD36.is_extensible;_LL678: {const char*_tmp13DA;void*_tmp13D9;(
_tmp13D9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp13DA="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp13DA,
sizeof(char),40))),_tag_dyneither(_tmp13D9,sizeof(void*),0)));}_LL679: if((
_tmpD35.KnownDatatype).tag != 2)goto _LL676;_tmpD39=(struct Cyc_Absyn_Datatypedecl**)(
_tmpD35.KnownDatatype).val;_tmpD3A=*_tmpD39;_tmpD3B=*_tmpD3A;_tmpD3C=_tmpD3B.tvs;
_tmpD3D=_tmpD3B.fields;_LL67A: return 0;_LL676:;}_LL672: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpD2D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpD1B;if(_tmpD2D->tag != 4)
goto _LL674;else{_tmpD2E=_tmpD2D->f1;_tmpD2F=_tmpD2E.field_info;_tmpD30=_tmpD2E.targs;}}
_LL673: {union Cyc_Absyn_DatatypeFieldInfoU _tmpD40=_tmpD2F;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpD41;struct _tuple3 _tmpD42;struct Cyc_Absyn_Datatypedecl*_tmpD43;struct Cyc_Absyn_Datatypefield*
_tmpD44;_LL67C: if((_tmpD40.UnknownDatatypefield).tag != 1)goto _LL67E;_tmpD41=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpD40.UnknownDatatypefield).val;
_LL67D: {const char*_tmp13DD;void*_tmp13DC;(_tmp13DC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13DD="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp13DD,sizeof(char),46))),_tag_dyneither(_tmp13DC,sizeof(void*),
0)));}_LL67E: if((_tmpD40.KnownDatatypefield).tag != 2)goto _LL67B;_tmpD42=(struct
_tuple3)(_tmpD40.KnownDatatypefield).val;_tmpD43=_tmpD42.f1;_tmpD44=_tmpD42.f2;
_LL67F: {struct Cyc_List_List*_tmpD47=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpD43->tvs,_tmpD30);struct Cyc_List_List*_tmpD48=
_tmpD44->typs;while(_tmpD48 != 0){_tmpD1A=Cyc_Tcutil_rsubstitute(rgn,_tmpD47,(*((
struct _tuple11*)_tmpD48->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpD1A))return 1;_tmpD48=_tmpD48->tl;}return 0;}_LL67B:;}_LL674:;_LL675: return 0;
_LL667:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpD49=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpD4B;union Cyc_Absyn_AggrInfoU _tmpD4C;struct Cyc_Absyn_Aggrdecl**_tmpD4D;struct
Cyc_Absyn_Aggrdecl*_tmpD4E;struct Cyc_List_List*_tmpD4F;struct Cyc_List_List*
_tmpD51;_LL681: {struct Cyc_Absyn_AggrType_struct*_tmpD4A=(struct Cyc_Absyn_AggrType_struct*)
_tmpD49;if(_tmpD4A->tag != 12)goto _LL683;else{_tmpD4B=_tmpD4A->f1;_tmpD4C=_tmpD4B.aggr_info;
if((_tmpD4C.KnownAggr).tag != 2)goto _LL683;_tmpD4D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD4C.KnownAggr).val;_tmpD4E=*_tmpD4D;_tmpD4F=_tmpD4B.targs;}}_LL682: _tmpD51=
_tmpD4E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD4E->impl))->fields;
goto _LL684;_LL683: {struct Cyc_Absyn_AnonAggrType_struct*_tmpD50=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpD49;if(_tmpD50->tag != 13)goto _LL685;else{_tmpD51=_tmpD50->f2;}}_LL684: {
struct Cyc_Absyn_Aggrfield*_tmpD52=Cyc_Absyn_lookup_field(_tmpD51,f);{struct Cyc_Absyn_Aggrfield*
_tmpD53=_tmpD52;struct Cyc_Absyn_Aggrfield _tmpD54;void*_tmpD55;_LL688: if(_tmpD53
!= 0)goto _LL68A;_LL689: {const char*_tmp13E0;void*_tmp13DF;(_tmp13DF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E0="is_noalias_field: missing field",
_tag_dyneither(_tmp13E0,sizeof(char),32))),_tag_dyneither(_tmp13DF,sizeof(void*),
0)));}_LL68A: if(_tmpD53 == 0)goto _LL687;_tmpD54=*_tmpD53;_tmpD55=_tmpD54.type;
_LL68B: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD55);_LL687:;}return 0;}
_LL685:;_LL686: {const char*_tmp13E4;void*_tmp13E3[1];struct Cyc_String_pa_struct
_tmp13E2;(_tmp13E2.tag=0,((_tmp13E2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13E3[0]=& _tmp13E2,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E4="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp13E4,sizeof(char),36))),_tag_dyneither(_tmp13E3,sizeof(void*),
1)))))));}_LL680:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpD5B=e->r;
void*_tmpD6B;struct Cyc_Absyn_Exp*_tmpD71;struct _dyneither_ptr*_tmpD72;struct Cyc_Absyn_Exp*
_tmpD74;struct Cyc_Absyn_Exp*_tmpD75;void*_tmpD7E;void*_tmpD81;void*_tmpD84;
struct Cyc_Absyn_Exp*_tmpD87;struct Cyc_Absyn_Exp*_tmpD89;struct Cyc_Absyn_Exp*
_tmpD8B;struct Cyc_Absyn_Exp*_tmpD8D;void*_tmpD8F;struct Cyc_Absyn_Exp*_tmpD90;
struct Cyc_Absyn_Stmt*_tmpD92;_LL68D: {struct Cyc_Absyn_Increment_e_struct*_tmpD5C=(
struct Cyc_Absyn_Increment_e_struct*)_tmpD5B;if(_tmpD5C->tag != 5)goto _LL68F;}
_LL68E: goto _LL690;_LL68F: {struct Cyc_Absyn_And_e_struct*_tmpD5D=(struct Cyc_Absyn_And_e_struct*)
_tmpD5B;if(_tmpD5D->tag != 7)goto _LL691;}_LL690: goto _LL692;_LL691: {struct Cyc_Absyn_Or_e_struct*
_tmpD5E=(struct Cyc_Absyn_Or_e_struct*)_tmpD5B;if(_tmpD5E->tag != 8)goto _LL693;}
_LL692: goto _LL694;_LL693: {struct Cyc_Absyn_Throw_e_struct*_tmpD5F=(struct Cyc_Absyn_Throw_e_struct*)
_tmpD5B;if(_tmpD5F->tag != 12)goto _LL695;}_LL694: goto _LL696;_LL695: {struct Cyc_Absyn_Address_e_struct*
_tmpD60=(struct Cyc_Absyn_Address_e_struct*)_tmpD5B;if(_tmpD60->tag != 16)goto
_LL697;}_LL696: goto _LL698;_LL697: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpD61=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD5B;if(_tmpD61->tag != 18)goto _LL699;}
_LL698: goto _LL69A;_LL699: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpD62=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpD5B;if(_tmpD62->tag != 19)goto _LL69B;}_LL69A: goto _LL69C;_LL69B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpD63=(struct Cyc_Absyn_Offsetof_e_struct*)_tmpD5B;if(_tmpD63->tag != 20)goto
_LL69D;}_LL69C: goto _LL69E;_LL69D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpD64=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpD5B;if(_tmpD64->tag != 26)goto _LL69F;}
_LL69E: goto _LL6A0;_LL69F: {struct Cyc_Absyn_Comprehension_e_struct*_tmpD65=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmpD5B;if(_tmpD65->tag != 28)goto _LL6A1;}
_LL6A0: goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Array_e_struct*_tmpD66=(struct Cyc_Absyn_Array_e_struct*)
_tmpD5B;if(_tmpD66->tag != 27)goto _LL6A3;}_LL6A2: goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Enum_e_struct*
_tmpD67=(struct Cyc_Absyn_Enum_e_struct*)_tmpD5B;if(_tmpD67->tag != 32)goto _LL6A5;}
_LL6A4: goto _LL6A6;_LL6A5: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD68=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD5B;if(_tmpD68->tag != 33)goto _LL6A7;}_LL6A6: goto _LL6A8;_LL6A7: {struct Cyc_Absyn_Swap_e_struct*
_tmpD69=(struct Cyc_Absyn_Swap_e_struct*)_tmpD5B;if(_tmpD69->tag != 35)goto _LL6A9;}
_LL6A8: goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Var_e_struct*_tmpD6A=(struct Cyc_Absyn_Var_e_struct*)
_tmpD5B;if(_tmpD6A->tag != 1)goto _LL6AB;else{_tmpD6B=(void*)_tmpD6A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmpD6C=(struct Cyc_Absyn_Global_b_struct*)_tmpD6B;if(
_tmpD6C->tag != 1)goto _LL6AB;};}}_LL6AA: goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Primop_e_struct*
_tmpD6D=(struct Cyc_Absyn_Primop_e_struct*)_tmpD5B;if(_tmpD6D->tag != 3)goto _LL6AD;}
_LL6AC: return 0;_LL6AD: {struct Cyc_Absyn_Deref_e_struct*_tmpD6E=(struct Cyc_Absyn_Deref_e_struct*)
_tmpD5B;if(_tmpD6E->tag != 21)goto _LL6AF;}_LL6AE: goto _LL6B0;_LL6AF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmpD6F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmpD5B;if(_tmpD6F->tag != 23)goto
_LL6B1;}_LL6B0: return 0;_LL6B1: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD70=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpD5B;if(_tmpD70->tag != 22)goto _LL6B3;
else{_tmpD71=_tmpD70->f1;_tmpD72=_tmpD70->f2;}}_LL6B2: return(ignore_leaf  || Cyc_Tcutil_is_noalias_field(
r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD71->topt))->v,_tmpD72)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD71,0);_LL6B3: {struct Cyc_Absyn_Subscript_e_struct*_tmpD73=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpD5B;if(_tmpD73->tag != 24)goto _LL6B5;else{_tmpD74=_tmpD73->f1;_tmpD75=_tmpD73->f2;}}
_LL6B4: {void*_tmpD93=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD74->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD95;void*_tmpD96;
struct Cyc_List_List*_tmpD98;_LL6D8: {struct Cyc_Absyn_PointerType_struct*_tmpD94=(
struct Cyc_Absyn_PointerType_struct*)_tmpD93;if(_tmpD94->tag != 5)goto _LL6DA;else{
_tmpD95=_tmpD94->f1;_tmpD96=_tmpD95.elt_typ;}}_LL6D9: return 0;_LL6DA: {struct Cyc_Absyn_TupleType_struct*
_tmpD97=(struct Cyc_Absyn_TupleType_struct*)_tmpD93;if(_tmpD97->tag != 11)goto
_LL6DC;else{_tmpD98=_tmpD97->f1;}}_LL6DB: {unsigned int _tmpD9A;int _tmpD9B;struct
_tuple13 _tmpD99=Cyc_Evexp_eval_const_uint_exp(_tmpD75);_tmpD9A=_tmpD99.f1;
_tmpD9B=_tmpD99.f2;if(!_tmpD9B){const char*_tmp13E7;void*_tmp13E6;(_tmp13E6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp13E7="is_noalias_path_aux: non-constant subscript",_tag_dyneither(_tmp13E7,
sizeof(char),44))),_tag_dyneither(_tmp13E6,sizeof(void*),0)));}{struct
_handler_cons _tmpD9E;_push_handler(& _tmpD9E);{int _tmpDA0=0;if(setjmp(_tmpD9E.handler))
_tmpDA0=1;if(!_tmpDA0){{void*_tmpDA1=(*((struct _tuple11*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmpD98,(int)_tmpD9A)).f2;int _tmpDA2=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpDA1)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpD74,0);_npop_handler(0);
return _tmpDA2;};_pop_handler();}else{void*_tmpD9F=(void*)_exn_thrown;void*
_tmpDA4=_tmpD9F;_LL6DF: {struct Cyc_List_Nth_struct*_tmpDA5=(struct Cyc_List_Nth_struct*)
_tmpDA4;if(_tmpDA5->tag != Cyc_List_Nth)goto _LL6E1;}_LL6E0: {const char*_tmp13EA;
void*_tmp13E9;return(_tmp13E9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13EA="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp13EA,sizeof(char),45))),_tag_dyneither(_tmp13E9,sizeof(void*),
0)));}_LL6E1:;_LL6E2:(void)_throw(_tmpDA4);_LL6DE:;}};};}_LL6DC:;_LL6DD: {const
char*_tmp13ED;void*_tmp13EC;(_tmp13EC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13ED="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp13ED,sizeof(char),52))),_tag_dyneither(_tmp13EC,sizeof(void*),
0)));}_LL6D7:;}_LL6B5: {struct Cyc_Absyn_Datatype_e_struct*_tmpD76=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpD5B;if(_tmpD76->tag != 31)goto _LL6B7;}_LL6B6: goto _LL6B8;_LL6B7: {struct Cyc_Absyn_Tuple_e_struct*
_tmpD77=(struct Cyc_Absyn_Tuple_e_struct*)_tmpD5B;if(_tmpD77->tag != 25)goto _LL6B9;}
_LL6B8: goto _LL6BA;_LL6B9: {struct Cyc_Absyn_Aggregate_e_struct*_tmpD78=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpD5B;if(_tmpD78->tag != 29)goto _LL6BB;}_LL6BA: goto _LL6BC;_LL6BB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpD79=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpD5B;if(_tmpD79->tag != 30)goto
_LL6BD;}_LL6BC: goto _LL6BE;_LL6BD: {struct Cyc_Absyn_Const_e_struct*_tmpD7A=(
struct Cyc_Absyn_Const_e_struct*)_tmpD5B;if(_tmpD7A->tag != 0)goto _LL6BF;}_LL6BE:
goto _LL6C0;_LL6BF: {struct Cyc_Absyn_Malloc_e_struct*_tmpD7B=(struct Cyc_Absyn_Malloc_e_struct*)
_tmpD5B;if(_tmpD7B->tag != 34)goto _LL6C1;}_LL6C0: goto _LL6C2;_LL6C1: {struct Cyc_Absyn_New_e_struct*
_tmpD7C=(struct Cyc_Absyn_New_e_struct*)_tmpD5B;if(_tmpD7C->tag != 17)goto _LL6C3;}
_LL6C2: goto _LL6C4;_LL6C3: {struct Cyc_Absyn_Var_e_struct*_tmpD7D=(struct Cyc_Absyn_Var_e_struct*)
_tmpD5B;if(_tmpD7D->tag != 1)goto _LL6C5;else{_tmpD7E=(void*)_tmpD7D->f2;{struct
Cyc_Absyn_Local_b_struct*_tmpD7F=(struct Cyc_Absyn_Local_b_struct*)_tmpD7E;if(
_tmpD7F->tag != 4)goto _LL6C5;};}}_LL6C4: goto _LL6C6;_LL6C5: {struct Cyc_Absyn_Var_e_struct*
_tmpD80=(struct Cyc_Absyn_Var_e_struct*)_tmpD5B;if(_tmpD80->tag != 1)goto _LL6C7;
else{_tmpD81=(void*)_tmpD80->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD82=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD81;if(_tmpD82->tag != 5)goto _LL6C7;};}}_LL6C6: goto _LL6C8;_LL6C7: {struct Cyc_Absyn_Var_e_struct*
_tmpD83=(struct Cyc_Absyn_Var_e_struct*)_tmpD5B;if(_tmpD83->tag != 1)goto _LL6C9;
else{_tmpD84=(void*)_tmpD83->f2;{struct Cyc_Absyn_Param_b_struct*_tmpD85=(struct
Cyc_Absyn_Param_b_struct*)_tmpD84;if(_tmpD85->tag != 3)goto _LL6C9;};}}_LL6C8: {
int _tmpDAA=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpDAA;}_LL6C9: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD86=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD5B;if(_tmpD86->tag != 6)goto
_LL6CB;else{_tmpD87=_tmpD86->f2;}}_LL6CA: _tmpD89=_tmpD87;goto _LL6CC;_LL6CB: {
struct Cyc_Absyn_SeqExp_e_struct*_tmpD88=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD5B;if(_tmpD88->tag != 9)goto _LL6CD;else{_tmpD89=_tmpD88->f2;}}_LL6CC: _tmpD8B=
_tmpD89;goto _LL6CE;_LL6CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmpD8A=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpD5B;if(_tmpD8A->tag != 4)goto _LL6CF;else{_tmpD8B=_tmpD8A->f1;}}_LL6CE: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD8B,ignore_leaf);_LL6CF: {struct Cyc_Absyn_FnCall_e_struct*
_tmpD8C=(struct Cyc_Absyn_FnCall_e_struct*)_tmpD5B;if(_tmpD8C->tag != 11)goto
_LL6D1;else{_tmpD8D=_tmpD8C->f1;}}_LL6D0: {void*_tmpDAB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD8D->topt))->v);struct Cyc_Absyn_FnInfo
_tmpDAD;void*_tmpDAE;struct Cyc_Absyn_PtrInfo _tmpDB0;void*_tmpDB1;_LL6E4: {struct
Cyc_Absyn_FnType_struct*_tmpDAC=(struct Cyc_Absyn_FnType_struct*)_tmpDAB;if(
_tmpDAC->tag != 10)goto _LL6E6;else{_tmpDAD=_tmpDAC->f1;_tmpDAE=_tmpDAD.ret_typ;}}
_LL6E5: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpDAE);
_LL6E6: {struct Cyc_Absyn_PointerType_struct*_tmpDAF=(struct Cyc_Absyn_PointerType_struct*)
_tmpDAB;if(_tmpDAF->tag != 5)goto _LL6E8;else{_tmpDB0=_tmpDAF->f1;_tmpDB1=_tmpDB0.elt_typ;}}
_LL6E7:{void*_tmpDB2=Cyc_Tcutil_compress(_tmpDB1);struct Cyc_Absyn_FnInfo _tmpDB4;
void*_tmpDB5;_LL6EB: {struct Cyc_Absyn_FnType_struct*_tmpDB3=(struct Cyc_Absyn_FnType_struct*)
_tmpDB2;if(_tmpDB3->tag != 10)goto _LL6ED;else{_tmpDB4=_tmpDB3->f1;_tmpDB5=_tmpDB4.ret_typ;}}
_LL6EC: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpDB5);
_LL6ED:;_LL6EE: goto _LL6EA;_LL6EA:;}goto _LL6E9;_LL6E8:;_LL6E9: {const char*
_tmp13F1;void*_tmp13F0[1];struct Cyc_String_pa_struct _tmp13EF;(_tmp13EF.tag=0,((
_tmp13EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD8D->topt))->v)),((_tmp13F0[0]=&
_tmp13EF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp13F1="Fncall has non function type %s",_tag_dyneither(_tmp13F1,sizeof(char),
32))),_tag_dyneither(_tmp13F0,sizeof(void*),1)))))));}_LL6E3:;}_LL6D1: {struct
Cyc_Absyn_Cast_e_struct*_tmpD8E=(struct Cyc_Absyn_Cast_e_struct*)_tmpD5B;if(
_tmpD8E->tag != 15)goto _LL6D3;else{_tmpD8F=(void*)_tmpD8E->f1;_tmpD90=_tmpD8E->f2;}}
_LL6D2: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD8F) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD90,1);_LL6D3: {struct Cyc_Absyn_StmtExp_e_struct*_tmpD91=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpD5B;if(_tmpD91->tag != 37)goto _LL6D5;else{_tmpD92=_tmpD91->f1;}}_LL6D4: while(
1){void*_tmpDB9=_tmpD92->r;struct Cyc_Absyn_Stmt*_tmpDBB;struct Cyc_Absyn_Stmt*
_tmpDBC;struct Cyc_Absyn_Decl*_tmpDBE;struct Cyc_Absyn_Stmt*_tmpDBF;struct Cyc_Absyn_Exp*
_tmpDC1;_LL6F0: {struct Cyc_Absyn_Seq_s_struct*_tmpDBA=(struct Cyc_Absyn_Seq_s_struct*)
_tmpDB9;if(_tmpDBA->tag != 2)goto _LL6F2;else{_tmpDBB=_tmpDBA->f1;_tmpDBC=_tmpDBA->f2;}}
_LL6F1: _tmpD92=_tmpDBC;goto _LL6EF;_LL6F2: {struct Cyc_Absyn_Decl_s_struct*_tmpDBD=(
struct Cyc_Absyn_Decl_s_struct*)_tmpDB9;if(_tmpDBD->tag != 12)goto _LL6F4;else{
_tmpDBE=_tmpDBD->f1;_tmpDBF=_tmpDBD->f2;}}_LL6F3: _tmpD92=_tmpDBF;goto _LL6EF;
_LL6F4: {struct Cyc_Absyn_Exp_s_struct*_tmpDC0=(struct Cyc_Absyn_Exp_s_struct*)
_tmpDB9;if(_tmpDC0->tag != 1)goto _LL6F6;else{_tmpDC1=_tmpDC0->f1;}}_LL6F5: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpDC1,ignore_leaf);_LL6F6:;_LL6F7: {const char*
_tmp13F4;void*_tmp13F3;(_tmp13F3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F4="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp13F4,sizeof(char),40))),_tag_dyneither(_tmp13F3,sizeof(void*),
0)));}_LL6EF:;}_LL6D5:;_LL6D6: return 0;_LL68C:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple16 _tmp13F5;struct _tuple16 bogus_ans=(
_tmp13F5.f1=0,((_tmp13F5.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13F5)));void*
_tmpDC4=e->r;struct _tuple2*_tmpDC6;void*_tmpDC7;struct Cyc_Absyn_Exp*_tmpDC9;
struct _dyneither_ptr*_tmpDCA;int _tmpDCB;struct Cyc_Absyn_Exp*_tmpDCD;struct
_dyneither_ptr*_tmpDCE;int _tmpDCF;struct Cyc_Absyn_Exp*_tmpDD1;struct Cyc_Absyn_Exp*
_tmpDD3;struct Cyc_Absyn_Exp*_tmpDD4;_LL6F9: {struct Cyc_Absyn_Var_e_struct*
_tmpDC5=(struct Cyc_Absyn_Var_e_struct*)_tmpDC4;if(_tmpDC5->tag != 1)goto _LL6FB;
else{_tmpDC6=_tmpDC5->f1;_tmpDC7=(void*)_tmpDC5->f2;}}_LL6FA: {void*_tmpDD5=
_tmpDC7;struct Cyc_Absyn_Vardecl*_tmpDD9;struct Cyc_Absyn_Vardecl*_tmpDDB;struct
Cyc_Absyn_Vardecl*_tmpDDD;struct Cyc_Absyn_Vardecl*_tmpDDF;_LL706: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpDD6=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpDD5;if(_tmpDD6->tag != 0)goto
_LL708;}_LL707: goto _LL709;_LL708: {struct Cyc_Absyn_Funname_b_struct*_tmpDD7=(
struct Cyc_Absyn_Funname_b_struct*)_tmpDD5;if(_tmpDD7->tag != 2)goto _LL70A;}_LL709:
return bogus_ans;_LL70A: {struct Cyc_Absyn_Global_b_struct*_tmpDD8=(struct Cyc_Absyn_Global_b_struct*)
_tmpDD5;if(_tmpDD8->tag != 1)goto _LL70C;else{_tmpDD9=_tmpDD8->f1;}}_LL70B: {void*
_tmpDE0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL713: {struct Cyc_Absyn_ArrayType_struct*_tmpDE1=(struct Cyc_Absyn_ArrayType_struct*)
_tmpDE0;if(_tmpDE1->tag != 9)goto _LL715;}_LL714: {struct _tuple16 _tmp13F6;return(
_tmp13F6.f1=1,((_tmp13F6.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13F6)));}_LL715:;
_LL716: {struct _tuple16 _tmp13F7;return(_tmp13F7.f1=(_tmpDD9->tq).real_const,((
_tmp13F7.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13F7)));}_LL712:;}_LL70C: {struct
Cyc_Absyn_Local_b_struct*_tmpDDA=(struct Cyc_Absyn_Local_b_struct*)_tmpDD5;if(
_tmpDDA->tag != 4)goto _LL70E;else{_tmpDDB=_tmpDDA->f1;}}_LL70D: {void*_tmpDE4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL718: {struct Cyc_Absyn_ArrayType_struct*
_tmpDE5=(struct Cyc_Absyn_ArrayType_struct*)_tmpDE4;if(_tmpDE5->tag != 9)goto
_LL71A;}_LL719: {struct _tuple16 _tmp13F8;return(_tmp13F8.f1=1,((_tmp13F8.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpDDB->rgn))->v,_tmp13F8)));}_LL71A:;_LL71B:
_tmpDDB->escapes=1;{struct _tuple16 _tmp13F9;return(_tmp13F9.f1=(_tmpDDB->tq).real_const,((
_tmp13F9.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpDDB->rgn))->v,_tmp13F9)));};
_LL717:;}_LL70E: {struct Cyc_Absyn_Pat_b_struct*_tmpDDC=(struct Cyc_Absyn_Pat_b_struct*)
_tmpDD5;if(_tmpDDC->tag != 5)goto _LL710;else{_tmpDDD=_tmpDDC->f1;}}_LL70F: _tmpDDF=
_tmpDDD;goto _LL711;_LL710: {struct Cyc_Absyn_Param_b_struct*_tmpDDE=(struct Cyc_Absyn_Param_b_struct*)
_tmpDD5;if(_tmpDDE->tag != 3)goto _LL705;else{_tmpDDF=_tmpDDE->f1;}}_LL711: _tmpDDF->escapes=
1;{struct _tuple16 _tmp13FA;return(_tmp13FA.f1=(_tmpDDF->tq).real_const,((_tmp13FA.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDDF->rgn))->v,_tmp13FA)));};_LL705:;}
_LL6FB: {struct Cyc_Absyn_AggrMember_e_struct*_tmpDC8=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDC4;if(_tmpDC8->tag != 22)goto _LL6FD;else{_tmpDC9=_tmpDC8->f1;_tmpDCA=_tmpDC8->f2;
_tmpDCB=_tmpDC8->f3;}}_LL6FC: if(_tmpDCB)return bogus_ans;{void*_tmpDE9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDC9->topt))->v);struct Cyc_List_List*
_tmpDEB;struct Cyc_Absyn_AggrInfo _tmpDED;union Cyc_Absyn_AggrInfoU _tmpDEE;struct
Cyc_Absyn_Aggrdecl**_tmpDEF;struct Cyc_Absyn_Aggrdecl*_tmpDF0;_LL71D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpDEA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpDE9;if(_tmpDEA->tag != 13)goto
_LL71F;else{_tmpDEB=_tmpDEA->f2;}}_LL71E: {struct Cyc_Absyn_Aggrfield*_tmpDF1=Cyc_Absyn_lookup_field(
_tmpDEB,_tmpDCA);if(_tmpDF1 != 0  && _tmpDF1->width != 0){struct _tuple16 _tmp13FB;
return(_tmp13FB.f1=(_tmpDF1->tq).real_const,((_tmp13FB.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDC9)).f2,_tmp13FB)));}return bogus_ans;}_LL71F: {struct Cyc_Absyn_AggrType_struct*
_tmpDEC=(struct Cyc_Absyn_AggrType_struct*)_tmpDE9;if(_tmpDEC->tag != 12)goto
_LL721;else{_tmpDED=_tmpDEC->f1;_tmpDEE=_tmpDED.aggr_info;if((_tmpDEE.KnownAggr).tag
!= 2)goto _LL721;_tmpDEF=(struct Cyc_Absyn_Aggrdecl**)(_tmpDEE.KnownAggr).val;
_tmpDF0=*_tmpDEF;}}_LL720: {struct Cyc_Absyn_Aggrfield*_tmpDF3=Cyc_Absyn_lookup_decl_field(
_tmpDF0,_tmpDCA);if(_tmpDF3 != 0  && _tmpDF3->width != 0){struct _tuple16 _tmp13FC;
return(_tmp13FC.f1=(_tmpDF3->tq).real_const,((_tmp13FC.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDC9)).f2,_tmp13FC)));}return bogus_ans;}_LL721:;_LL722: return bogus_ans;
_LL71C:;};_LL6FD: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpDCC=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDC4;if(_tmpDCC->tag != 23)goto _LL6FF;else{_tmpDCD=_tmpDCC->f1;_tmpDCE=_tmpDCC->f2;
_tmpDCF=_tmpDCC->f3;}}_LL6FE: if(_tmpDCF)return bogus_ans;{void*_tmpDF5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDCD->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpDF7;void*_tmpDF8;struct Cyc_Absyn_PtrAtts _tmpDF9;void*_tmpDFA;_LL724: {struct
Cyc_Absyn_PointerType_struct*_tmpDF6=(struct Cyc_Absyn_PointerType_struct*)
_tmpDF5;if(_tmpDF6->tag != 5)goto _LL726;else{_tmpDF7=_tmpDF6->f1;_tmpDF8=_tmpDF7.elt_typ;
_tmpDF9=_tmpDF7.ptr_atts;_tmpDFA=_tmpDF9.rgn;}}_LL725: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpDFB=Cyc_Tcutil_compress(_tmpDF8);struct Cyc_List_List*_tmpDFD;
struct Cyc_Absyn_AggrInfo _tmpDFF;union Cyc_Absyn_AggrInfoU _tmpE00;struct Cyc_Absyn_Aggrdecl**
_tmpE01;struct Cyc_Absyn_Aggrdecl*_tmpE02;_LL729: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpDFC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpDFB;if(_tmpDFC->tag != 13)goto
_LL72B;else{_tmpDFD=_tmpDFC->f2;}}_LL72A: finfo=Cyc_Absyn_lookup_field(_tmpDFD,
_tmpDCE);goto _LL728;_LL72B: {struct Cyc_Absyn_AggrType_struct*_tmpDFE=(struct Cyc_Absyn_AggrType_struct*)
_tmpDFB;if(_tmpDFE->tag != 12)goto _LL72D;else{_tmpDFF=_tmpDFE->f1;_tmpE00=_tmpDFF.aggr_info;
if((_tmpE00.KnownAggr).tag != 2)goto _LL72D;_tmpE01=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE00.KnownAggr).val;_tmpE02=*_tmpE01;}}_LL72C: finfo=Cyc_Absyn_lookup_decl_field(
_tmpE02,_tmpDCE);goto _LL728;_LL72D:;_LL72E: return bogus_ans;_LL728:;}if(finfo != 0
 && finfo->width != 0){struct _tuple16 _tmp13FD;return(_tmp13FD.f1=(finfo->tq).real_const,((
_tmp13FD.f2=_tmpDFA,_tmp13FD)));}return bogus_ans;}_LL726:;_LL727: return bogus_ans;
_LL723:;};_LL6FF: {struct Cyc_Absyn_Deref_e_struct*_tmpDD0=(struct Cyc_Absyn_Deref_e_struct*)
_tmpDC4;if(_tmpDD0->tag != 21)goto _LL701;else{_tmpDD1=_tmpDD0->f1;}}_LL700: {void*
_tmpE04=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpDD1->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpE06;struct Cyc_Absyn_Tqual _tmpE07;struct Cyc_Absyn_PtrAtts
_tmpE08;void*_tmpE09;_LL730: {struct Cyc_Absyn_PointerType_struct*_tmpE05=(struct
Cyc_Absyn_PointerType_struct*)_tmpE04;if(_tmpE05->tag != 5)goto _LL732;else{
_tmpE06=_tmpE05->f1;_tmpE07=_tmpE06.elt_tq;_tmpE08=_tmpE06.ptr_atts;_tmpE09=
_tmpE08.rgn;}}_LL731: {struct _tuple16 _tmp13FE;return(_tmp13FE.f1=_tmpE07.real_const,((
_tmp13FE.f2=_tmpE09,_tmp13FE)));}_LL732:;_LL733: return bogus_ans;_LL72F:;}_LL701: {
struct Cyc_Absyn_Subscript_e_struct*_tmpDD2=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpDC4;if(_tmpDD2->tag != 24)goto _LL703;else{_tmpDD3=_tmpDD2->f1;_tmpDD4=_tmpDD2->f2;}}
_LL702: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpDD3->topt))->v);void*_tmpE0B=t;struct Cyc_List_List*_tmpE0D;struct Cyc_Absyn_PtrInfo
_tmpE0F;struct Cyc_Absyn_Tqual _tmpE10;struct Cyc_Absyn_PtrAtts _tmpE11;void*_tmpE12;
struct Cyc_Absyn_ArrayInfo _tmpE14;struct Cyc_Absyn_Tqual _tmpE15;_LL735: {struct Cyc_Absyn_TupleType_struct*
_tmpE0C=(struct Cyc_Absyn_TupleType_struct*)_tmpE0B;if(_tmpE0C->tag != 11)goto
_LL737;else{_tmpE0D=_tmpE0C->f1;}}_LL736: {unsigned int _tmpE17;int _tmpE18;struct
_tuple13 _tmpE16=Cyc_Evexp_eval_const_uint_exp(_tmpDD4);_tmpE17=_tmpE16.f1;
_tmpE18=_tmpE16.f2;if(!_tmpE18)return bogus_ans;{struct _tuple11*_tmpE19=Cyc_Absyn_lookup_tuple_field(
_tmpE0D,(int)_tmpE17);if(_tmpE19 != 0){struct _tuple16 _tmp13FF;return(_tmp13FF.f1=((*
_tmpE19).f1).real_const,((_tmp13FF.f2=(Cyc_Tcutil_addressof_props(te,_tmpDD3)).f2,
_tmp13FF)));}return bogus_ans;};}_LL737: {struct Cyc_Absyn_PointerType_struct*
_tmpE0E=(struct Cyc_Absyn_PointerType_struct*)_tmpE0B;if(_tmpE0E->tag != 5)goto
_LL739;else{_tmpE0F=_tmpE0E->f1;_tmpE10=_tmpE0F.elt_tq;_tmpE11=_tmpE0F.ptr_atts;
_tmpE12=_tmpE11.rgn;}}_LL738: {struct _tuple16 _tmp1400;return(_tmp1400.f1=_tmpE10.real_const,((
_tmp1400.f2=_tmpE12,_tmp1400)));}_LL739: {struct Cyc_Absyn_ArrayType_struct*
_tmpE13=(struct Cyc_Absyn_ArrayType_struct*)_tmpE0B;if(_tmpE13->tag != 9)goto
_LL73B;else{_tmpE14=_tmpE13->f1;_tmpE15=_tmpE14.tq;}}_LL73A: {struct _tuple16
_tmp1401;return(_tmp1401.f1=_tmpE15.real_const,((_tmp1401.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDD3)).f2,_tmp1401)));}_LL73B:;_LL73C: return bogus_ans;_LL734:;}_LL703:;
_LL704:{const char*_tmp1404;void*_tmp1403;(_tmp1403=0,Cyc_Tcutil_terr(e->loc,((
_tmp1404="unary & applied to non-lvalue",_tag_dyneither(_tmp1404,sizeof(char),30))),
_tag_dyneither(_tmp1403,sizeof(void*),0)));}return bogus_ans;_LL6F8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpE20=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpE22;void*_tmpE23;struct Cyc_Absyn_Tqual
_tmpE24;union Cyc_Absyn_Constraint*_tmpE25;_LL73E: {struct Cyc_Absyn_ArrayType_struct*
_tmpE21=(struct Cyc_Absyn_ArrayType_struct*)_tmpE20;if(_tmpE21->tag != 9)goto
_LL740;else{_tmpE22=_tmpE21->f1;_tmpE23=_tmpE22.elt_type;_tmpE24=_tmpE22.tq;
_tmpE25=_tmpE22.zero_term;}}_LL73F: {void*_tmpE27;struct _tuple16 _tmpE26=Cyc_Tcutil_addressof_props(
te,e);_tmpE27=_tmpE26.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1407;struct Cyc_Absyn_Upper_b_struct*
_tmp1406;return Cyc_Absyn_atb_typ(_tmpE23,_tmpE27,_tmpE24,(void*)((_tmp1406=
_cycalloc(sizeof(*_tmp1406)),((_tmp1406[0]=((_tmp1407.tag=1,((_tmp1407.f1=e,
_tmp1407)))),_tmp1406)))),_tmpE25);};}_LL740:;_LL741: return e_typ;_LL73D:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpE2A=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpE2D;
_LL743: {struct Cyc_Absyn_DynEither_b_struct*_tmpE2B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE2A;if(_tmpE2B->tag != 0)goto _LL745;}_LL744: return;_LL745: {struct Cyc_Absyn_Upper_b_struct*
_tmpE2C=(struct Cyc_Absyn_Upper_b_struct*)_tmpE2A;if(_tmpE2C->tag != 1)goto _LL742;
else{_tmpE2D=_tmpE2C->f1;}}_LL746: {unsigned int _tmpE2F;int _tmpE30;struct
_tuple13 _tmpE2E=Cyc_Evexp_eval_const_uint_exp(_tmpE2D);_tmpE2F=_tmpE2E.f1;
_tmpE30=_tmpE2E.f2;if(_tmpE30  && _tmpE2F <= i){const char*_tmp140C;void*_tmp140B[2];
struct Cyc_Int_pa_struct _tmp140A;struct Cyc_Int_pa_struct _tmp1409;(_tmp1409.tag=1,((
_tmp1409.f1=(unsigned long)((int)i),((_tmp140A.tag=1,((_tmp140A.f1=(
unsigned long)((int)_tmpE2F),((_tmp140B[0]=& _tmp140A,((_tmp140B[1]=& _tmp1409,Cyc_Tcutil_terr(
loc,((_tmp140C="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp140C,
sizeof(char),39))),_tag_dyneither(_tmp140B,sizeof(void*),2)))))))))))));}return;}
_LL742:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpE35=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpE37;_LL748: {struct
Cyc_Absyn_Upper_b_struct*_tmpE36=(struct Cyc_Absyn_Upper_b_struct*)_tmpE35;if(
_tmpE36->tag != 1)goto _LL74A;else{_tmpE37=_tmpE36->f1;}}_LL749: {unsigned int
_tmpE39;int _tmpE3A;struct _tuple13 _tmpE38=Cyc_Evexp_eval_const_uint_exp(_tmpE37);
_tmpE39=_tmpE38.f1;_tmpE3A=_tmpE38.f2;return _tmpE3A  && _tmpE39 == 1;}_LL74A:;
_LL74B: return 0;_LL747:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpE3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpE43;
void*_tmpE44;union Cyc_Absyn_Constraint*_tmpE45;struct Cyc_List_List*_tmpE47;
struct Cyc_Absyn_AggrInfo _tmpE49;union Cyc_Absyn_AggrInfoU _tmpE4A;struct _tuple4
_tmpE4B;struct Cyc_Absyn_AggrInfo _tmpE4D;union Cyc_Absyn_AggrInfoU _tmpE4E;struct
Cyc_Absyn_Aggrdecl**_tmpE4F;struct Cyc_Absyn_Aggrdecl*_tmpE50;struct Cyc_List_List*
_tmpE51;struct Cyc_List_List*_tmpE53;_LL74D: {struct Cyc_Absyn_VoidType_struct*
_tmpE3C=(struct Cyc_Absyn_VoidType_struct*)_tmpE3B;if(_tmpE3C->tag != 0)goto _LL74F;}
_LL74E: goto _LL750;_LL74F: {struct Cyc_Absyn_IntType_struct*_tmpE3D=(struct Cyc_Absyn_IntType_struct*)
_tmpE3B;if(_tmpE3D->tag != 6)goto _LL751;}_LL750: goto _LL752;_LL751: {struct Cyc_Absyn_FloatType_struct*
_tmpE3E=(struct Cyc_Absyn_FloatType_struct*)_tmpE3B;if(_tmpE3E->tag != 7)goto
_LL753;}_LL752: goto _LL754;_LL753: {struct Cyc_Absyn_DoubleType_struct*_tmpE3F=(
struct Cyc_Absyn_DoubleType_struct*)_tmpE3B;if(_tmpE3F->tag != 8)goto _LL755;}
_LL754: return 1;_LL755: {struct Cyc_Absyn_EnumType_struct*_tmpE40=(struct Cyc_Absyn_EnumType_struct*)
_tmpE3B;if(_tmpE40->tag != 14)goto _LL757;}_LL756: goto _LL758;_LL757: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpE41=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE3B;if(_tmpE41->tag != 15)goto
_LL759;}_LL758: return 0;_LL759: {struct Cyc_Absyn_ArrayType_struct*_tmpE42=(struct
Cyc_Absyn_ArrayType_struct*)_tmpE3B;if(_tmpE42->tag != 9)goto _LL75B;else{_tmpE43=
_tmpE42->f1;_tmpE44=_tmpE43.elt_type;_tmpE45=_tmpE43.zero_term;}}_LL75A: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE45) && Cyc_Tcutil_bits_only(
_tmpE44);_LL75B: {struct Cyc_Absyn_TupleType_struct*_tmpE46=(struct Cyc_Absyn_TupleType_struct*)
_tmpE3B;if(_tmpE46->tag != 11)goto _LL75D;else{_tmpE47=_tmpE46->f1;}}_LL75C: for(0;
_tmpE47 != 0;_tmpE47=_tmpE47->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpE47->hd)).f2))return 0;}return 1;_LL75D: {struct Cyc_Absyn_AggrType_struct*
_tmpE48=(struct Cyc_Absyn_AggrType_struct*)_tmpE3B;if(_tmpE48->tag != 12)goto
_LL75F;else{_tmpE49=_tmpE48->f1;_tmpE4A=_tmpE49.aggr_info;if((_tmpE4A.UnknownAggr).tag
!= 1)goto _LL75F;_tmpE4B=(struct _tuple4)(_tmpE4A.UnknownAggr).val;}}_LL75E: return
0;_LL75F: {struct Cyc_Absyn_AggrType_struct*_tmpE4C=(struct Cyc_Absyn_AggrType_struct*)
_tmpE3B;if(_tmpE4C->tag != 12)goto _LL761;else{_tmpE4D=_tmpE4C->f1;_tmpE4E=_tmpE4D.aggr_info;
if((_tmpE4E.KnownAggr).tag != 2)goto _LL761;_tmpE4F=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE4E.KnownAggr).val;_tmpE50=*_tmpE4F;_tmpE51=_tmpE4D.targs;}}_LL760: if(
_tmpE50->impl == 0)return 0;{struct _RegionHandle _tmpE54=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpE54;_push_region(rgn);{struct Cyc_List_List*_tmpE55=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpE50->tvs,_tmpE51);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE50->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpE55,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpE56=0;_npop_handler(0);return
_tmpE56;}}}{int _tmpE57=1;_npop_handler(0);return _tmpE57;};};_pop_region(rgn);};
_LL761: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE52=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE3B;if(_tmpE52->tag != 13)goto _LL763;else{_tmpE53=_tmpE52->f2;}}_LL762: for(0;
_tmpE53 != 0;_tmpE53=_tmpE53->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpE53->hd)->type))return 0;}return 1;_LL763:;_LL764: return 0;_LL74C:;}struct
_tuple31{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpE58=e->r;
struct _tuple2*_tmpE60;void*_tmpE61;struct Cyc_Absyn_Exp*_tmpE63;struct Cyc_Absyn_Exp*
_tmpE64;struct Cyc_Absyn_Exp*_tmpE65;struct Cyc_Absyn_Exp*_tmpE67;struct Cyc_Absyn_Exp*
_tmpE68;struct Cyc_Absyn_Exp*_tmpE6A;struct Cyc_Absyn_Exp*_tmpE6C;void*_tmpE6E;
struct Cyc_Absyn_Exp*_tmpE6F;enum Cyc_Absyn_Coercion _tmpE70;void*_tmpE72;struct
Cyc_Absyn_Exp*_tmpE73;struct Cyc_Absyn_Exp*_tmpE75;struct Cyc_Absyn_Exp*_tmpE77;
struct Cyc_Absyn_Exp*_tmpE78;struct Cyc_List_List*_tmpE7A;struct Cyc_List_List*
_tmpE7C;struct Cyc_List_List*_tmpE7E;enum Cyc_Absyn_Primop _tmpE80;struct Cyc_List_List*
_tmpE81;struct Cyc_List_List*_tmpE83;struct Cyc_List_List*_tmpE85;_LL766: {struct
Cyc_Absyn_Const_e_struct*_tmpE59=(struct Cyc_Absyn_Const_e_struct*)_tmpE58;if(
_tmpE59->tag != 0)goto _LL768;}_LL767: goto _LL769;_LL768: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpE5A=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE58;if(_tmpE5A->tag != 18)goto
_LL76A;}_LL769: goto _LL76B;_LL76A: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpE5B=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE58;if(_tmpE5B->tag != 19)goto _LL76C;}
_LL76B: goto _LL76D;_LL76C: {struct Cyc_Absyn_Offsetof_e_struct*_tmpE5C=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpE58;if(_tmpE5C->tag != 20)goto _LL76E;}_LL76D: goto _LL76F;_LL76E: {struct Cyc_Absyn_Enum_e_struct*
_tmpE5D=(struct Cyc_Absyn_Enum_e_struct*)_tmpE58;if(_tmpE5D->tag != 32)goto _LL770;}
_LL76F: goto _LL771;_LL770: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpE5E=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpE58;if(_tmpE5E->tag != 33)goto _LL772;}_LL771: return 1;_LL772: {struct Cyc_Absyn_Var_e_struct*
_tmpE5F=(struct Cyc_Absyn_Var_e_struct*)_tmpE58;if(_tmpE5F->tag != 1)goto _LL774;
else{_tmpE60=_tmpE5F->f1;_tmpE61=(void*)_tmpE5F->f2;}}_LL773: {void*_tmpE86=
_tmpE61;struct Cyc_Absyn_Vardecl*_tmpE89;_LL793: {struct Cyc_Absyn_Funname_b_struct*
_tmpE87=(struct Cyc_Absyn_Funname_b_struct*)_tmpE86;if(_tmpE87->tag != 2)goto
_LL795;}_LL794: return 1;_LL795: {struct Cyc_Absyn_Global_b_struct*_tmpE88=(struct
Cyc_Absyn_Global_b_struct*)_tmpE86;if(_tmpE88->tag != 1)goto _LL797;else{_tmpE89=
_tmpE88->f1;}}_LL796: {void*_tmpE8B=Cyc_Tcutil_compress(_tmpE89->type);_LL79C: {
struct Cyc_Absyn_ArrayType_struct*_tmpE8C=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE8B;if(_tmpE8C->tag != 9)goto _LL79E;}_LL79D: goto _LL79F;_LL79E: {struct Cyc_Absyn_FnType_struct*
_tmpE8D=(struct Cyc_Absyn_FnType_struct*)_tmpE8B;if(_tmpE8D->tag != 10)goto _LL7A0;}
_LL79F: return 1;_LL7A0:;_LL7A1: return var_okay;_LL79B:;}_LL797: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpE8A=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpE86;if(_tmpE8A->tag != 0)goto
_LL799;}_LL798: return 0;_LL799:;_LL79A: return var_okay;_LL792:;}_LL774: {struct Cyc_Absyn_Conditional_e_struct*
_tmpE62=(struct Cyc_Absyn_Conditional_e_struct*)_tmpE58;if(_tmpE62->tag != 6)goto
_LL776;else{_tmpE63=_tmpE62->f1;_tmpE64=_tmpE62->f2;_tmpE65=_tmpE62->f3;}}_LL775:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpE63) && Cyc_Tcutil_cnst_exp(te,0,_tmpE64))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpE65);_LL776: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpE66=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpE58;if(_tmpE66->tag != 9)goto _LL778;
else{_tmpE67=_tmpE66->f1;_tmpE68=_tmpE66->f2;}}_LL777: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE67) && Cyc_Tcutil_cnst_exp(te,0,_tmpE68);_LL778: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE69=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE58;if(_tmpE69->tag != 13)
goto _LL77A;else{_tmpE6A=_tmpE69->f1;}}_LL779: _tmpE6C=_tmpE6A;goto _LL77B;_LL77A: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpE6B=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpE58;if(_tmpE6B->tag != 14)goto _LL77C;else{_tmpE6C=_tmpE6B->f1;}}_LL77B: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE6C);_LL77C: {struct Cyc_Absyn_Cast_e_struct*
_tmpE6D=(struct Cyc_Absyn_Cast_e_struct*)_tmpE58;if(_tmpE6D->tag != 15)goto _LL77E;
else{_tmpE6E=(void*)_tmpE6D->f1;_tmpE6F=_tmpE6D->f2;_tmpE70=_tmpE6D->f4;if(
_tmpE70 != Cyc_Absyn_No_coercion)goto _LL77E;}}_LL77D: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpE6F);_LL77E: {struct Cyc_Absyn_Cast_e_struct*_tmpE71=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE58;if(_tmpE71->tag != 15)goto _LL780;else{_tmpE72=(void*)_tmpE71->f1;_tmpE73=
_tmpE71->f2;}}_LL77F: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE73);_LL780: {
struct Cyc_Absyn_Address_e_struct*_tmpE74=(struct Cyc_Absyn_Address_e_struct*)
_tmpE58;if(_tmpE74->tag != 16)goto _LL782;else{_tmpE75=_tmpE74->f1;}}_LL781: return
Cyc_Tcutil_cnst_exp(te,1,_tmpE75);_LL782: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpE76=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpE58;if(_tmpE76->tag != 28)
goto _LL784;else{_tmpE77=_tmpE76->f2;_tmpE78=_tmpE76->f3;}}_LL783: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE77) && Cyc_Tcutil_cnst_exp(te,0,_tmpE78);_LL784: {struct Cyc_Absyn_Array_e_struct*
_tmpE79=(struct Cyc_Absyn_Array_e_struct*)_tmpE58;if(_tmpE79->tag != 27)goto _LL786;
else{_tmpE7A=_tmpE79->f1;}}_LL785: _tmpE7C=_tmpE7A;goto _LL787;_LL786: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpE7B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpE58;if(_tmpE7B->tag != 30)goto
_LL788;else{_tmpE7C=_tmpE7B->f2;}}_LL787: _tmpE7E=_tmpE7C;goto _LL789;_LL788: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpE7D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpE58;if(_tmpE7D->tag != 29)goto _LL78A;else{_tmpE7E=_tmpE7D->f3;}}_LL789: for(0;
_tmpE7E != 0;_tmpE7E=_tmpE7E->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple31*)
_tmpE7E->hd)).f2))return 0;}return 1;_LL78A: {struct Cyc_Absyn_Primop_e_struct*
_tmpE7F=(struct Cyc_Absyn_Primop_e_struct*)_tmpE58;if(_tmpE7F->tag != 3)goto _LL78C;
else{_tmpE80=_tmpE7F->f1;_tmpE81=_tmpE7F->f2;}}_LL78B: _tmpE83=_tmpE81;goto _LL78D;
_LL78C: {struct Cyc_Absyn_Tuple_e_struct*_tmpE82=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpE58;if(_tmpE82->tag != 25)goto _LL78E;else{_tmpE83=_tmpE82->f1;}}_LL78D:
_tmpE85=_tmpE83;goto _LL78F;_LL78E: {struct Cyc_Absyn_Datatype_e_struct*_tmpE84=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpE58;if(_tmpE84->tag != 31)goto _LL790;else{
_tmpE85=_tmpE84->f1;}}_LL78F: for(0;_tmpE85 != 0;_tmpE85=_tmpE85->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpE85->hd))return 0;}return 1;_LL790:;_LL791: return 0;
_LL765:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpE8E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE94;struct Cyc_Absyn_PtrAtts _tmpE95;union Cyc_Absyn_Constraint*
_tmpE96;union Cyc_Absyn_Constraint*_tmpE97;struct Cyc_Absyn_ArrayInfo _tmpE99;void*
_tmpE9A;struct Cyc_List_List*_tmpE9C;struct Cyc_Absyn_AggrInfo _tmpE9E;union Cyc_Absyn_AggrInfoU
_tmpE9F;struct Cyc_List_List*_tmpEA0;struct Cyc_List_List*_tmpEA2;_LL7A3: {struct
Cyc_Absyn_VoidType_struct*_tmpE8F=(struct Cyc_Absyn_VoidType_struct*)_tmpE8E;if(
_tmpE8F->tag != 0)goto _LL7A5;}_LL7A4: goto _LL7A6;_LL7A5: {struct Cyc_Absyn_IntType_struct*
_tmpE90=(struct Cyc_Absyn_IntType_struct*)_tmpE8E;if(_tmpE90->tag != 6)goto _LL7A7;}
_LL7A6: goto _LL7A8;_LL7A7: {struct Cyc_Absyn_FloatType_struct*_tmpE91=(struct Cyc_Absyn_FloatType_struct*)
_tmpE8E;if(_tmpE91->tag != 7)goto _LL7A9;}_LL7A8: goto _LL7AA;_LL7A9: {struct Cyc_Absyn_DoubleType_struct*
_tmpE92=(struct Cyc_Absyn_DoubleType_struct*)_tmpE8E;if(_tmpE92->tag != 8)goto
_LL7AB;}_LL7AA: return 1;_LL7AB: {struct Cyc_Absyn_PointerType_struct*_tmpE93=(
struct Cyc_Absyn_PointerType_struct*)_tmpE8E;if(_tmpE93->tag != 5)goto _LL7AD;else{
_tmpE94=_tmpE93->f1;_tmpE95=_tmpE94.ptr_atts;_tmpE96=_tmpE95.nullable;_tmpE97=
_tmpE95.bounds;}}_LL7AC: {void*_tmpEA5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE97);_LL7BC: {
struct Cyc_Absyn_DynEither_b_struct*_tmpEA6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEA5;if(_tmpEA6->tag != 0)goto _LL7BE;}_LL7BD: return 1;_LL7BE:;_LL7BF: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpE96);_LL7BB:;}
_LL7AD: {struct Cyc_Absyn_ArrayType_struct*_tmpE98=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE8E;if(_tmpE98->tag != 9)goto _LL7AF;else{_tmpE99=_tmpE98->f1;_tmpE9A=_tmpE99.elt_type;}}
_LL7AE: return Cyc_Tcutil_supports_default(_tmpE9A);_LL7AF: {struct Cyc_Absyn_TupleType_struct*
_tmpE9B=(struct Cyc_Absyn_TupleType_struct*)_tmpE8E;if(_tmpE9B->tag != 11)goto
_LL7B1;else{_tmpE9C=_tmpE9B->f1;}}_LL7B0: for(0;_tmpE9C != 0;_tmpE9C=_tmpE9C->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpE9C->hd)).f2))return 0;}
return 1;_LL7B1: {struct Cyc_Absyn_AggrType_struct*_tmpE9D=(struct Cyc_Absyn_AggrType_struct*)
_tmpE8E;if(_tmpE9D->tag != 12)goto _LL7B3;else{_tmpE9E=_tmpE9D->f1;_tmpE9F=_tmpE9E.aggr_info;
_tmpEA0=_tmpE9E.targs;}}_LL7B2: {struct Cyc_Absyn_Aggrdecl*_tmpEA7=Cyc_Absyn_get_known_aggrdecl(
_tmpE9F);if(_tmpEA7->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpEA7->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpEA7->tvs,_tmpEA0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEA7->impl))->fields);}
_LL7B3: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEA1=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE8E;if(_tmpEA1->tag != 13)goto _LL7B5;else{_tmpEA2=_tmpEA1->f2;}}_LL7B4: return
Cyc_Tcutil_fields_support_default(0,0,_tmpEA2);_LL7B5: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpEA3=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE8E;if(_tmpEA3->tag != 15)goto
_LL7B7;}_LL7B6: goto _LL7B8;_LL7B7: {struct Cyc_Absyn_EnumType_struct*_tmpEA4=(
struct Cyc_Absyn_EnumType_struct*)_tmpE8E;if(_tmpEA4->tag != 14)goto _LL7B9;}_LL7B8:
return 1;_LL7B9:;_LL7BA: return 0;_LL7A2:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpEA8=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpEA8;_push_region(rgn);{struct Cyc_List_List*_tmpEA9=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpEA9,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpEAA=0;_npop_handler(0);return _tmpEAA;}}}{
int _tmpEAB=1;_npop_handler(0);return _tmpEAB;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpEAC=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpEB1;struct Cyc_Absyn_PtrAtts _tmpEB2;union Cyc_Absyn_Constraint*
_tmpEB3;union Cyc_Absyn_Constraint*_tmpEB4;_LL7C1: {struct Cyc_Absyn_IntType_struct*
_tmpEAD=(struct Cyc_Absyn_IntType_struct*)_tmpEAC;if(_tmpEAD->tag != 6)goto _LL7C3;}
_LL7C2: goto _LL7C4;_LL7C3: {struct Cyc_Absyn_FloatType_struct*_tmpEAE=(struct Cyc_Absyn_FloatType_struct*)
_tmpEAC;if(_tmpEAE->tag != 7)goto _LL7C5;}_LL7C4: goto _LL7C6;_LL7C5: {struct Cyc_Absyn_DoubleType_struct*
_tmpEAF=(struct Cyc_Absyn_DoubleType_struct*)_tmpEAC;if(_tmpEAF->tag != 8)goto
_LL7C7;}_LL7C6: return 1;_LL7C7: {struct Cyc_Absyn_PointerType_struct*_tmpEB0=(
struct Cyc_Absyn_PointerType_struct*)_tmpEAC;if(_tmpEB0->tag != 5)goto _LL7C9;else{
_tmpEB1=_tmpEB0->f1;_tmpEB2=_tmpEB1.ptr_atts;_tmpEB3=_tmpEB2.nullable;_tmpEB4=
_tmpEB2.bounds;}}_LL7C8: {void*_tmpEB5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpEB4);_LL7CC: {
struct Cyc_Absyn_DynEither_b_struct*_tmpEB6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEB5;if(_tmpEB6->tag != 0)goto _LL7CE;}_LL7CD: return 0;_LL7CE:;_LL7CF: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEB3);_LL7CB:;}
_LL7C9:;_LL7CA: return 0;_LL7C0:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpEB7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEB9;void*
_tmpEBA;struct Cyc_Absyn_FnInfo _tmpEBC;struct Cyc_List_List*_tmpEBD;_LL7D1: {
struct Cyc_Absyn_PointerType_struct*_tmpEB8=(struct Cyc_Absyn_PointerType_struct*)
_tmpEB7;if(_tmpEB8->tag != 5)goto _LL7D3;else{_tmpEB9=_tmpEB8->f1;_tmpEBA=_tmpEB9.elt_typ;}}
_LL7D2: return Cyc_Tcutil_is_noreturn(_tmpEBA);_LL7D3: {struct Cyc_Absyn_FnType_struct*
_tmpEBB=(struct Cyc_Absyn_FnType_struct*)_tmpEB7;if(_tmpEBB->tag != 10)goto _LL7D5;
else{_tmpEBC=_tmpEBB->f1;_tmpEBD=_tmpEBC.attributes;}}_LL7D4: for(0;_tmpEBD != 0;
_tmpEBD=_tmpEBD->tl){void*_tmpEBE=(void*)_tmpEBD->hd;_LL7D8: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpEBF=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpEBE;if(_tmpEBF->tag != 4)goto
_LL7DA;}_LL7D9: return 1;_LL7DA:;_LL7DB: continue;_LL7D7:;}goto _LL7D0;_LL7D5:;
_LL7D6: goto _LL7D0;_LL7D0:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpEC0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpEC2;struct Cyc_List_List*_tmpEC3;struct Cyc_List_List**_tmpEC4;_LL7DD: {struct
Cyc_Absyn_FnType_struct*_tmpEC1=(struct Cyc_Absyn_FnType_struct*)_tmpEC0;if(
_tmpEC1->tag != 10)goto _LL7DF;else{_tmpEC2=_tmpEC1->f1;_tmpEC3=_tmpEC2.attributes;
_tmpEC4=(struct Cyc_List_List**)&(_tmpEC1->f1).attributes;}}_LL7DE: {struct Cyc_List_List*
_tmpEC5=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp140D;*_tmpEC4=((_tmp140D=_cycalloc(sizeof(*_tmp140D)),((
_tmp140D->hd=(void*)atts->hd,((_tmp140D->tl=*_tmpEC4,_tmp140D))))));}else{struct
Cyc_List_List*_tmp140E;_tmpEC5=((_tmp140E=_cycalloc(sizeof(*_tmp140E)),((
_tmp140E->hd=(void*)atts->hd,((_tmp140E->tl=_tmpEC5,_tmp140E))))));}}return
_tmpEC5;}_LL7DF:;_LL7E0: {const char*_tmp1411;void*_tmp1410;(_tmp1410=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1411="transfer_fn_type_atts",
_tag_dyneither(_tmp1411,sizeof(char),22))),_tag_dyneither(_tmp1410,sizeof(void*),
0)));}_LL7DC:;}
