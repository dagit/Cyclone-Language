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
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
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
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
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
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple2*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*
es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct
_tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct
Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
t);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple14
Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);extern int Cyc_Tcutil_warn_alias_coerce;
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct
_tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple15*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct
_tuple16*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int
allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple17{int f1;void*f2;
};struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
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
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpFDD;void*
_tmpFDC[2];const char*_tmpFDB;const char*_tmpFDA;struct Cyc_String_pa_struct _tmpFD9;
struct Cyc_String_pa_struct _tmpFD8;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpFD8.tag=0,((_tmpFD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpFD9.tag=0,((_tmpFD9.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpFDA="const ",_tag_dyneither(_tmpFDA,sizeof(char),7)):((_tmpFDB="",
_tag_dyneither(_tmpFDB,sizeof(char),1)))),((_tmpFDC[0]=& _tmpFD9,((_tmpFDC[1]=&
_tmpFD8,Cyc_aprintf(((_tmpFDD="%s%s",_tag_dyneither(_tmpFDD,sizeof(char),5))),
_tag_dyneither(_tmpFDC,sizeof(void*),2))))))))))))));const char*_tmpFE6;void*
_tmpFE5[2];const char*_tmpFE4;const char*_tmpFE3;struct Cyc_String_pa_struct _tmpFE2;
struct Cyc_String_pa_struct _tmpFE1;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpFE1.tag=0,((_tmpFE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpFE2.tag=0,((_tmpFE2.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpFE3="const ",_tag_dyneither(_tmpFE3,sizeof(char),7)):((_tmpFE4="",
_tag_dyneither(_tmpFE4,sizeof(char),1)))),((_tmpFE5[0]=& _tmpFE2,((_tmpFE5[1]=&
_tmpFE1,Cyc_aprintf(((_tmpFE6="%s%s",_tag_dyneither(_tmpFE6,sizeof(char),5))),
_tag_dyneither(_tmpFE5,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpFEA;
void*_tmpFE9[1];struct Cyc_String_pa_struct _tmpFE8;(_tmpFE8.tag=0,((_tmpFE8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpFE9[0]=& _tmpFE8,Cyc_fprintf(
Cyc_stderr,((_tmpFEA="  %s",_tag_dyneither(_tmpFEA,sizeof(char),5))),
_tag_dyneither(_tmpFE9,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpFED;void*_tmpFEC;(_tmpFEC=0,Cyc_fprintf(
Cyc_stderr,((_tmpFED="\n\t",_tag_dyneither(_tmpFED,sizeof(char),3))),
_tag_dyneither(_tmpFEC,sizeof(void*),0)));}pos=8;}else{{const char*_tmpFF0;void*
_tmpFEF;(_tmpFEF=0,Cyc_fprintf(Cyc_stderr,((_tmpFF0=" ",_tag_dyneither(_tmpFF0,
sizeof(char),2))),_tag_dyneither(_tmpFEF,sizeof(void*),0)));}++ pos;}{const char*
_tmpFF3;void*_tmpFF2;(_tmpFF2=0,Cyc_fprintf(Cyc_stderr,((_tmpFF3="and ",
_tag_dyneither(_tmpFF3,sizeof(char),5))),_tag_dyneither(_tmpFF2,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpFF6;
void*_tmpFF5;(_tmpFF5=0,Cyc_fprintf(Cyc_stderr,((_tmpFF6="\n\t",_tag_dyneither(
_tmpFF6,sizeof(char),3))),_tag_dyneither(_tmpFF5,sizeof(void*),0)));}pos=8;}{
const char*_tmpFFA;void*_tmpFF9[1];struct Cyc_String_pa_struct _tmpFF8;(_tmpFF8.tag=
0,((_tmpFF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpFF9[0]=&
_tmpFF8,Cyc_fprintf(Cyc_stderr,((_tmpFFA="%s ",_tag_dyneither(_tmpFFA,sizeof(
char),4))),_tag_dyneither(_tmpFF9,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpFFD;void*
_tmpFFC;(_tmpFFC=0,Cyc_fprintf(Cyc_stderr,((_tmpFFD="\n\t",_tag_dyneither(
_tmpFFD,sizeof(char),3))),_tag_dyneither(_tmpFFC,sizeof(void*),0)));}pos=8;}{
const char*_tmp1000;void*_tmpFFF;(_tmpFFF=0,Cyc_fprintf(Cyc_stderr,((_tmp1000="are not compatible. ",
_tag_dyneither(_tmp1000,sizeof(char),21))),_tag_dyneither(_tmpFFF,sizeof(void*),
0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(
0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)
>= 80){const char*_tmp1003;void*_tmp1002;(_tmp1002=0,Cyc_fprintf(Cyc_stderr,((
_tmp1003="\n\t",_tag_dyneither(_tmp1003,sizeof(char),3))),_tag_dyneither(
_tmp1002,sizeof(void*),0)));}{const char*_tmp1007;void*_tmp1006[1];struct Cyc_String_pa_struct
_tmp1005;(_tmp1005.tag=0,((_tmp1005.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp1006[0]=& _tmp1005,Cyc_fprintf(
Cyc_stderr,((_tmp1007="%s",_tag_dyneither(_tmp1007,sizeof(char),3))),
_tag_dyneither(_tmp1006,sizeof(void*),1)))))));};}{const char*_tmp100A;void*
_tmp1009;(_tmp1009=0,Cyc_fprintf(Cyc_stderr,((_tmp100A="\n",_tag_dyneither(
_tmp100A,sizeof(char),2))),_tag_dyneither(_tmp1009,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmp100E;void*_tmp100D[1];struct Cyc_String_pa_struct _tmp100C;(_tmp100C.tag=
0,((_tmp100C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp100D[0]=&
_tmp100C,Cyc_fprintf(Cyc_stderr,((_tmp100E="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmp100E,sizeof(char),36))),_tag_dyneither(_tmp100D,sizeof(void*),
1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmp1011;struct Cyc_Core_Impossible_struct*_tmp1010;(int)_throw((void*)((_tmp1010=
_cycalloc(sizeof(*_tmp1010)),((_tmp1010[0]=((_tmp1011.tag=Cyc_Core_Impossible,((
_tmp1011.f1=msg,_tmp1011)))),_tmp1010)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmp1016;void*_tmp1015[2];struct Cyc_String_pa_struct _tmp1014;struct Cyc_String_pa_struct
_tmp1013;(_tmp1013.tag=0,((_tmp1013.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((
_tmp1014.tag=0,((_tmp1014.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmp1015[0]=& _tmp1014,((_tmp1015[1]=& _tmp1013,
Cyc_fprintf(Cyc_stderr,((_tmp1016="%s::%s ",_tag_dyneither(_tmp1016,sizeof(char),
8))),_tag_dyneither(_tmp1015,sizeof(void*),2)))))))))))));}{const char*_tmp1019;
void*_tmp1018;(_tmp1018=0,Cyc_fprintf(Cyc_stderr,((_tmp1019="\n",_tag_dyneither(
_tmp1019,sizeof(char),2))),_tag_dyneither(_tmp1018,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmp101A;Cyc_Tcutil_warning_segs=((_tmp101A=
_cycalloc(sizeof(*_tmp101A)),((_tmp101A->hd=sg,((_tmp101A->tl=Cyc_Tcutil_warning_segs,
_tmp101A))))));}{struct _dyneither_ptr*_tmp101D;struct Cyc_List_List*_tmp101C;Cyc_Tcutil_warning_msgs=((
_tmp101C=_cycalloc(sizeof(*_tmp101C)),((_tmp101C->hd=((_tmp101D=_cycalloc(
sizeof(*_tmp101D)),((_tmp101D[0]=msg,_tmp101D)))),((_tmp101C->tl=Cyc_Tcutil_warning_msgs,
_tmp101C))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmp1020;void*_tmp101F;(
_tmp101F=0,Cyc_fprintf(Cyc_stderr,((_tmp1020="***Warnings***\n",_tag_dyneither(
_tmp1020,sizeof(char),16))),_tag_dyneither(_tmp101F,sizeof(void*),0)));}{struct
Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){{const char*_tmp1025;void*_tmp1024[2];struct Cyc_String_pa_struct _tmp1023;
struct Cyc_String_pa_struct _tmp1022;(_tmp1022.tag=0,((_tmp1022.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp1023.tag=0,((_tmp1023.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(_tmp36))->hd)),((_tmp1024[0]=& _tmp1023,((_tmp1024[1]=& _tmp1022,Cyc_fprintf(
Cyc_stderr,((_tmp1025="%s: %s\n",_tag_dyneither(_tmp1025,sizeof(char),8))),
_tag_dyneither(_tmp1024,sizeof(void*),2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*
_tmp1028;void*_tmp1027;(_tmp1027=0,Cyc_fprintf(Cyc_stderr,((_tmp1028="**************\n",
_tag_dyneither(_tmp1028,sizeof(char),16))),_tag_dyneither(_tmp1027,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp1029;*_tmp44=((_tmp1029=
_cycalloc(sizeof(*_tmp1029)),((_tmp1029[0]=t2,_tmp1029))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp102A;*_tmp47=((_tmp102A=_cycalloc(sizeof(*
_tmp102A)),((_tmp102A->v=t2,_tmp102A))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
_tmp102D;struct Cyc_Absyn_Unknown_kb_struct*_tmp102C;return(void*)((_tmp102C=
_cycalloc(sizeof(*_tmp102C)),((_tmp102C[0]=((_tmp102D.tag=1,((_tmp102D.f1=0,
_tmp102D)))),_tmp102C))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct
Cyc_Absyn_Less_kb_struct*)_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}
_LL1D: {struct Cyc_Absyn_Less_kb_struct _tmp1030;struct Cyc_Absyn_Less_kb_struct*
_tmp102F;return(void*)((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F[0]=((
_tmp1030.tag=2,((_tmp1030.f1=0,((_tmp1030.f2=_tmp59,_tmp1030)))))),_tmp102F))));}
_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp1031;return(_tmp1031=_cycalloc(
sizeof(*_tmp1031)),((_tmp1031->name=tv->name,((_tmp1031->identity=- 1,((_tmp1031->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmp1031)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmp1032;return(_tmp1032=_cycalloc(sizeof(*
_tmp1032)),((_tmp1032->f1=_tmp61,((_tmp1032->f2=_tmp62,((_tmp1032->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmp1032)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmp1033;return(_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033->f1=_tmp67,((
_tmp1033->f2=Cyc_Tcutil_copy_type(_tmp68),_tmp1033)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp1034;return(_tmp1034=_cycalloc(sizeof(*_tmp1034)),((_tmp1034->name=f->name,((
_tmp1034->tq=f->tq,((_tmp1034->type=Cyc_Tcutil_copy_type(f->type),((_tmp1034->width=
f->width,((_tmp1034->attributes=f->attributes,_tmp1034)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*
_tmp1035;return(_tmp1035=_cycalloc(sizeof(*_tmp1035)),((_tmp1035->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmp1035->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmp1035)))));};}static
struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmp1036;return(_tmp1036=_cycalloc(sizeof(*_tmp1036)),((
_tmp1036->name=f->name,((_tmp1036->tag=f->tag,((_tmp1036->loc=f->loc,_tmp1036)))))));}
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
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmp1039;struct
Cyc_Absyn_VarType_struct*_tmp1038;return(void*)((_tmp1038=_cycalloc(sizeof(*
_tmp1038)),((_tmp1038[0]=((_tmp1039.tag=2,((_tmp1039.f1=Cyc_Tcutil_copy_tvar(
_tmp75),_tmp1039)))),_tmp1038))));}_LL27: {struct Cyc_Absyn_DatatypeType_struct*
_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)_tmp71;if(_tmp76->tag != 3)goto _LL29;
else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;_tmp79=_tmp77.targs;}}_LL28: {
struct Cyc_Absyn_DatatypeType_struct _tmp103F;struct Cyc_Absyn_DatatypeInfo _tmp103E;
struct Cyc_Absyn_DatatypeType_struct*_tmp103D;return(void*)((_tmp103D=_cycalloc(
sizeof(*_tmp103D)),((_tmp103D[0]=((_tmp103F.tag=3,((_tmp103F.f1=((_tmp103E.datatype_info=
_tmp78,((_tmp103E.targs=Cyc_Tcutil_copy_types(_tmp79),_tmp103E)))),_tmp103F)))),
_tmp103D))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp71;if(_tmp7A->tag != 4)goto _LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;
_tmp7D=_tmp7B.targs;}}_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp1045;
struct Cyc_Absyn_DatatypeFieldInfo _tmp1044;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp1043;return(void*)((_tmp1043=_cycalloc(sizeof(*_tmp1043)),((_tmp1043[0]=((
_tmp1045.tag=4,((_tmp1045.f1=((_tmp1044.field_info=_tmp7C,((_tmp1044.targs=Cyc_Tcutil_copy_types(
_tmp7D),_tmp1044)))),_tmp1045)))),_tmp1043))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*
_tmp7E=(struct Cyc_Absyn_PointerType_struct*)_tmp71;if(_tmp7E->tag != 5)goto _LL2D;
else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;
_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;
_tmp87=_tmp82.ptrloc;}}_LL2C: {void*_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*
_tmpD1=Cyc_Tcutil_copy_type(_tmp83);union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual
_tmpD3=_tmp81;union Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*
_tmpD5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmp104F;struct Cyc_Absyn_PtrAtts
_tmp104E;struct Cyc_Absyn_PtrInfo _tmp104D;struct Cyc_Absyn_PointerType_struct*
_tmp104C;return(void*)((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C[0]=((
_tmp104F.tag=5,((_tmp104F.f1=((_tmp104D.elt_typ=_tmpD0,((_tmp104D.elt_tq=_tmpD3,((
_tmp104D.ptr_atts=((_tmp104E.rgn=_tmpD1,((_tmp104E.nullable=_tmpD2,((_tmp104E.bounds=
_tmpD4,((_tmp104E.zero_term=_tmpD5,((_tmp104E.ptrloc=_tmp87,_tmp104E)))))))))),
_tmp104D)))))),_tmp104F)))),_tmp104C))));}_LL2D: {struct Cyc_Absyn_IntType_struct*
_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp71;if(_tmp88->tag != 6)goto _LL2F;}
_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*_tmp89=(struct Cyc_Absyn_FloatType_struct*)
_tmp71;if(_tmp89->tag != 7)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*
_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)_tmp71;if(_tmp8A->tag != 8)goto _LL33;}
_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp71;if(_tmp8B->tag != 9)goto _LL35;else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;
_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}
_LL34: {struct Cyc_Absyn_ArrayType_struct _tmp1055;struct Cyc_Absyn_ArrayInfo
_tmp1054;struct Cyc_Absyn_ArrayType_struct*_tmp1053;return(void*)((_tmp1053=
_cycalloc(sizeof(*_tmp1053)),((_tmp1053[0]=((_tmp1055.tag=9,((_tmp1055.f1=((
_tmp1054.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((_tmp1054.tq=_tmp8E,((_tmp1054.num_elts=
_tmp8F,((_tmp1054.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmp1054.zt_loc=_tmp91,_tmp1054)))))))))),
_tmp1055)))),_tmp1053))));}_LL35: {struct Cyc_Absyn_FnType_struct*_tmp92=(struct
Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;else{_tmp93=_tmp92->f1;
_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;_tmp97=_tmp93.args;
_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=_tmp93.rgn_po;_tmp9B=
_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*_tmp1056;struct Cyc_Core_Opt*
_tmpDE=_tmp95 == 0?0:((_tmp1056=_cycalloc(sizeof(*_tmp1056)),((_tmp1056->v=Cyc_Tcutil_copy_type((
void*)_tmp95->v),_tmp1056))));void*_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*
_tmpE0=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp99 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp99;struct Cyc_Absyn_VarargInfo*_tmp1057;cyc_varargs2=((_tmp1057=_cycalloc(
sizeof(*_tmp1057)),((_tmp1057->name=cv->name,((_tmp1057->tq=cv->tq,((_tmp1057->type=
Cyc_Tcutil_copy_type(cv->type),((_tmp1057->inject=cv->inject,_tmp1057))))))))));}{
struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9A);
struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmp105D;struct
Cyc_Absyn_FnInfo _tmp105C;struct Cyc_Absyn_FnType_struct*_tmp105B;return(void*)((
_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B[0]=((_tmp105D.tag=10,((_tmp105D.f1=((
_tmp105C.tvars=_tmpDD,((_tmp105C.effect=_tmpDE,((_tmp105C.ret_typ=_tmpDF,((
_tmp105C.args=_tmpE0,((_tmp105C.c_varargs=_tmpE1,((_tmp105C.cyc_varargs=
cyc_varargs2,((_tmp105C.rgn_po=_tmpE3,((_tmp105C.attributes=_tmpE4,_tmp105C)))))))))))))))),
_tmp105D)))),_tmp105B))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(
struct Cyc_Absyn_TupleType_struct*)_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{
_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct _tmp1060;struct Cyc_Absyn_TupleType_struct*
_tmp105F;return(void*)((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F[0]=((
_tmp1060.tag=11,((_tmp1060.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),
_tmp1060)))),_tmp105F))));}_LL39: {struct Cyc_Absyn_AggrType_struct*_tmp9E=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmp9E->tag != 12)goto _LL3B;else{
_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((_tmpA0.UnknownAggr).tag != 1)goto
_LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;_tmpA2=_tmpA1.f1;_tmpA3=
_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {struct Cyc_Absyn_AggrType_struct
_tmp1066;struct Cyc_Absyn_AggrInfo _tmp1065;struct Cyc_Absyn_AggrType_struct*
_tmp1064;return(void*)((_tmp1064=_cycalloc(sizeof(*_tmp1064)),((_tmp1064[0]=((
_tmp1066.tag=12,((_tmp1066.f1=((_tmp1065.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmp1065.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmp1065)))),
_tmp1066)))),_tmp1064))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{
_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;if((_tmpA8.KnownAggr).tag != 2)goto
_LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}
_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp106C;struct Cyc_Absyn_AggrInfo _tmp106B;
struct Cyc_Absyn_AggrType_struct*_tmp106A;return(void*)((_tmp106A=_cycalloc(
sizeof(*_tmp106A)),((_tmp106A[0]=((_tmp106C.tag=12,((_tmp106C.f1=((_tmp106B.aggr_info=
Cyc_Absyn_KnownAggr(_tmpA9),((_tmp106B.targs=Cyc_Tcutil_copy_types(_tmpAA),
_tmp106B)))),_tmp106C)))),_tmp106A))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp106F;struct Cyc_Absyn_AnonAggrType_struct*_tmp106E;return(void*)((_tmp106E=
_cycalloc(sizeof(*_tmp106E)),((_tmp106E[0]=((_tmp106F.tag=13,((_tmp106F.f1=
_tmpAC,((_tmp106F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpAD),_tmp106F)))))),_tmp106E))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpAE=(struct Cyc_Absyn_EnumType_struct*)_tmp71;if(_tmpAE->tag != 14)goto _LL41;
else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp1072;struct Cyc_Absyn_EnumType_struct*_tmp1071;return(void*)((_tmp1071=
_cycalloc(sizeof(*_tmp1071)),((_tmp1071[0]=((_tmp1072.tag=14,((_tmp1072.f1=
_tmpAF,((_tmp1072.f2=_tmpB0,_tmp1072)))))),_tmp1071))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp71;if(_tmpB1->tag != 15)goto
_LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1075;struct Cyc_Absyn_AnonEnumType_struct*_tmp1074;return(void*)((_tmp1074=
_cycalloc(sizeof(*_tmp1074)),((_tmp1074[0]=((_tmp1075.tag=15,((_tmp1075.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmp1075)))),
_tmp1074))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp1078;struct Cyc_Absyn_TagType_struct*_tmp1077;
return(void*)((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077[0]=((_tmp1078.tag=
20,((_tmp1078.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmp1078)))),_tmp1077))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp107B;struct Cyc_Absyn_ValueofType_struct*_tmp107A;return(void*)((_tmp107A=
_cycalloc(sizeof(*_tmp107A)),((_tmp107A[0]=((_tmp107B.tag=19,((_tmp107B.f1=
_tmpB6,_tmp107B)))),_tmp107A))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB7=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto
_LL49;else{_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp107E;struct Cyc_Absyn_RgnHandleType_struct*_tmp107D;return(void*)((_tmp107D=
_cycalloc(sizeof(*_tmp107D)),((_tmp107D[0]=((_tmp107E.tag=16,((_tmp107E.f1=(void*)
Cyc_Tcutil_copy_type(_tmpB8),_tmp107E)))),_tmp107D))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp1081;struct Cyc_Absyn_DynRgnType_struct*_tmp1080;return(void*)((_tmp1080=
_cycalloc(sizeof(*_tmp1080)),((_tmp1080[0]=((_tmp1081.tag=17,((_tmp1081.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmp1081.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmp1081)))))),_tmp1080))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp1084;struct Cyc_Absyn_TypedefType_struct*_tmp1083;return(void*)((_tmp1083=
_cycalloc(sizeof(*_tmp1083)),((_tmp1083[0]=((_tmp1084.tag=18,((_tmp1084.f1=
_tmpBD,((_tmp1084.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmp1084.f3=_tmpBF,((
_tmp1084.f4=0,_tmp1084)))))))))),_tmp1083))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC0=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmp1087;
struct Cyc_Absyn_AccessEff_struct*_tmp1086;return(void*)((_tmp1086=_cycalloc(
sizeof(*_tmp1086)),((_tmp1086[0]=((_tmp1087.tag=23,((_tmp1087.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmp1087)))),_tmp1086))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmp108A;struct Cyc_Absyn_JoinEff_struct*
_tmp1089;return(void*)((_tmp1089=_cycalloc(sizeof(*_tmp1089)),((_tmp1089[0]=((
_tmp108A.tag=24,((_tmp108A.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmp108A)))),
_tmp1089))));}_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmp108D;struct Cyc_Absyn_RgnsEff_struct*_tmp108C;
return(void*)((_tmp108C=_cycalloc(sizeof(*_tmp108C)),((_tmp108C[0]=((_tmp108D.tag=
25,((_tmp108D.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp108D)))),_tmp108C))));}
_LL20:;}static void*Cyc_Tcutil_copy_designator(void*d);static void*Cyc_Tcutil_copy_designator(
void*d){void*_tmp107=d;struct Cyc_Absyn_Exp*_tmp109;struct _dyneither_ptr*_tmp10B;
_LL58: {struct Cyc_Absyn_ArrayElement_struct*_tmp108=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp107;if(_tmp108->tag != 0)goto _LL5A;else{_tmp109=_tmp108->f1;}}_LL59: {struct
Cyc_Absyn_ArrayElement_struct _tmp1090;struct Cyc_Absyn_ArrayElement_struct*
_tmp108F;return(void*)((_tmp108F=_cycalloc(sizeof(*_tmp108F)),((_tmp108F[0]=((
_tmp1090.tag=0,((_tmp1090.f1=Cyc_Tcutil_deep_copy_exp(_tmp109),_tmp1090)))),
_tmp108F))));}_LL5A: {struct Cyc_Absyn_FieldName_struct*_tmp10A=(struct Cyc_Absyn_FieldName_struct*)
_tmp107;if(_tmp10A->tag != 1)goto _LL57;else{_tmp10B=_tmp10A->f1;}}_LL5B: return d;
_LL57:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e);static struct _tuple18*Cyc_Tcutil_copy_eds(
struct _tuple18*e){struct _tuple18*_tmp1091;return(_tmp1091=_cycalloc(sizeof(*
_tmp1091)),((_tmp1091->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp1091->f2=Cyc_Tcutil_deep_copy_exp((
e[0]).f2),_tmp1091)))));}struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){void*
_tmp10F=e->r;union Cyc_Absyn_Cnst _tmp111;struct _tuple2*_tmp113;void*_tmp114;
struct _tuple2*_tmp116;enum Cyc_Absyn_Primop _tmp118;struct Cyc_List_List*_tmp119;
struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;enum Cyc_Absyn_Incrementor _tmp120;struct Cyc_Absyn_Exp*
_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*
_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*
_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_VarargCallInfo*_tmp134;struct Cyc_Absyn_Exp*_tmp136;
struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*
_tmp13B;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;int _tmp13F;enum Cyc_Absyn_Coercion
_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*
_tmp145;void*_tmp147;struct Cyc_Absyn_Exp*_tmp149;void*_tmp14B;void*_tmp14C;
struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*
_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct _dyneither_ptr*
_tmp156;int _tmp157;int _tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*
_tmp15B;struct Cyc_List_List*_tmp15D;struct _tuple9*_tmp15F;struct _tuple9 _tmp160;
struct Cyc_Core_Opt*_tmp161;struct Cyc_Absyn_Tqual _tmp162;void*_tmp163;struct Cyc_List_List*
_tmp164;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Vardecl*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;int _tmp16B;struct _tuple2*_tmp16D;struct Cyc_List_List*
_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Aggrdecl*_tmp170;void*
_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypedecl*
_tmp176;struct Cyc_Absyn_Datatypefield*_tmp177;struct _tuple2*_tmp179;struct Cyc_Absyn_Enumdecl*
_tmp17A;struct Cyc_Absyn_Enumfield*_tmp17B;struct _tuple2*_tmp17D;void*_tmp17E;
struct Cyc_Absyn_Enumfield*_tmp17F;struct Cyc_Absyn_MallocInfo _tmp181;int _tmp182;
struct Cyc_Absyn_Exp*_tmp183;void**_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;
struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Core_Opt*
_tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Exp*_tmp18F;struct
_dyneither_ptr*_tmp190;void*_tmp192;_LL5D: {struct Cyc_Absyn_Const_e_struct*
_tmp110=(struct Cyc_Absyn_Const_e_struct*)_tmp10F;if(_tmp110->tag != 0)goto _LL5F;
else{_tmp111=_tmp110->f1;}}_LL5E: return Cyc_Absyn_const_exp(_tmp111,e->loc);_LL5F: {
struct Cyc_Absyn_Var_e_struct*_tmp112=(struct Cyc_Absyn_Var_e_struct*)_tmp10F;if(
_tmp112->tag != 1)goto _LL61;else{_tmp113=_tmp112->f1;_tmp114=(void*)_tmp112->f2;}}
_LL60: return Cyc_Absyn_varb_exp(_tmp113,_tmp114,e->loc);_LL61: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp115=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp10F;if(_tmp115->tag != 2)goto
_LL63;else{_tmp116=_tmp115->f1;}}_LL62: return Cyc_Absyn_unknownid_exp(_tmp116,e->loc);
_LL63: {struct Cyc_Absyn_Primop_e_struct*_tmp117=(struct Cyc_Absyn_Primop_e_struct*)
_tmp10F;if(_tmp117->tag != 3)goto _LL65;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}
_LL64: return Cyc_Absyn_primop_exp(_tmp118,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp119),e->loc);_LL65: {struct Cyc_Absyn_AssignOp_e_struct*_tmp11A=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp10F;if(_tmp11A->tag != 4)goto _LL67;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;
_tmp11D=_tmp11A->f3;}}_LL66: {struct Cyc_Core_Opt*_tmp1092;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp11B),(unsigned int)_tmp11C?(_tmp1092=
_cycalloc_atomic(sizeof(*_tmp1092)),((_tmp1092->v=(void*)((enum Cyc_Absyn_Primop)
_tmp11C->v),_tmp1092))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11D),e->loc);}_LL67: {
struct Cyc_Absyn_Increment_e_struct*_tmp11E=(struct Cyc_Absyn_Increment_e_struct*)
_tmp10F;if(_tmp11E->tag != 5)goto _LL69;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;}}
_LL68: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11F),_tmp120,e->loc);
_LL69: {struct Cyc_Absyn_Conditional_e_struct*_tmp121=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp10F;if(_tmp121->tag != 6)goto _LL6B;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;
_tmp124=_tmp121->f3;}}_LL6A: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp124),e->loc);
_LL6B: {struct Cyc_Absyn_And_e_struct*_tmp125=(struct Cyc_Absyn_And_e_struct*)
_tmp10F;if(_tmp125->tag != 7)goto _LL6D;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}
_LL6C: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp126),Cyc_Tcutil_deep_copy_exp(
_tmp127),e->loc);_LL6D: {struct Cyc_Absyn_Or_e_struct*_tmp128=(struct Cyc_Absyn_Or_e_struct*)
_tmp10F;if(_tmp128->tag != 8)goto _LL6F;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}
_LL6E: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp129),Cyc_Tcutil_deep_copy_exp(
_tmp12A),e->loc);_LL6F: {struct Cyc_Absyn_SeqExp_e_struct*_tmp12B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp10F;if(_tmp12B->tag != 9)goto _LL71;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}
_LL70: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp12C),Cyc_Tcutil_deep_copy_exp(
_tmp12D),e->loc);_LL71: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp12E=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp10F;if(_tmp12E->tag != 10)goto _LL73;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;}}
_LL72: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp12F),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp130),e->loc);_LL73: {struct Cyc_Absyn_FnCall_e_struct*
_tmp131=(struct Cyc_Absyn_FnCall_e_struct*)_tmp10F;if(_tmp131->tag != 11)goto _LL75;
else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=_tmp131->f3;}}_LL74: {
struct Cyc_Absyn_VarargCallInfo*_tmp194=_tmp134;struct Cyc_Absyn_VarargCallInfo
_tmp195;int _tmp196;struct Cyc_List_List*_tmp197;struct Cyc_Absyn_VarargInfo*
_tmp198;_LLAE: if(_tmp194 == 0)goto _LLB0;_tmp195=*_tmp194;_tmp196=_tmp195.num_varargs;
_tmp197=_tmp195.injectors;_tmp198=_tmp195.vai;_LLAF: {struct Cyc_Absyn_VarargInfo
_tmp19A;struct Cyc_Core_Opt*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;void*_tmp19D;int
_tmp19E;struct Cyc_Absyn_VarargInfo*_tmp199=_tmp198;_tmp19A=*_tmp199;_tmp19B=
_tmp19A.name;_tmp19C=_tmp19A.tq;_tmp19D=_tmp19A.type;_tmp19E=_tmp19A.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp109C;struct Cyc_Absyn_VarargInfo*_tmp109B;
struct Cyc_Absyn_VarargCallInfo*_tmp109A;struct Cyc_Absyn_FnCall_e_struct*_tmp1099;
return Cyc_Absyn_new_exp((void*)((_tmp1099=_cycalloc(sizeof(*_tmp1099)),((
_tmp1099[0]=((_tmp109C.tag=11,((_tmp109C.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp109C.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp109C.f3=((_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A->num_varargs=
_tmp196,((_tmp109A->injectors=_tmp197,((_tmp109A->vai=((_tmp109B=_cycalloc(
sizeof(*_tmp109B)),((_tmp109B->name=_tmp19B,((_tmp109B->tq=_tmp19C,((_tmp109B->type=
Cyc_Tcutil_copy_type(_tmp19D),((_tmp109B->inject=_tmp19E,_tmp109B)))))))))),
_tmp109A)))))))),_tmp109C)))))))),_tmp1099)))),e->loc);};}_LLB0:;_LLB1: {struct
Cyc_Absyn_FnCall_e_struct _tmp109F;struct Cyc_Absyn_FnCall_e_struct*_tmp109E;
return Cyc_Absyn_new_exp((void*)((_tmp109E=_cycalloc(sizeof(*_tmp109E)),((
_tmp109E[0]=((_tmp109F.tag=11,((_tmp109F.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp109F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp109F.f3=_tmp134,_tmp109F)))))))),_tmp109E)))),e->loc);}_LLAD:;}_LL75: {
struct Cyc_Absyn_Throw_e_struct*_tmp135=(struct Cyc_Absyn_Throw_e_struct*)_tmp10F;
if(_tmp135->tag != 12)goto _LL77;else{_tmp136=_tmp135->f1;}}_LL76: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp136),e->loc);_LL77: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp137=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp10F;if(_tmp137->tag != 13)
goto _LL79;else{_tmp138=_tmp137->f1;}}_LL78: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp138),e->loc);_LL79: {struct Cyc_Absyn_Instantiate_e_struct*_tmp139=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp10F;if(_tmp139->tag != 14)goto _LL7B;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;}}
_LL7A: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp13A),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp13B),e->loc);_LL7B: {struct Cyc_Absyn_Cast_e_struct*_tmp13C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp10F;if(_tmp13C->tag != 15)goto _LL7D;else{_tmp13D=(void*)_tmp13C->f1;_tmp13E=
_tmp13C->f2;_tmp13F=_tmp13C->f3;_tmp140=_tmp13C->f4;}}_LL7C: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp13D),Cyc_Tcutil_deep_copy_exp(_tmp13E),_tmp13F,_tmp140,
e->loc);_LL7D: {struct Cyc_Absyn_Address_e_struct*_tmp141=(struct Cyc_Absyn_Address_e_struct*)
_tmp10F;if(_tmp141->tag != 16)goto _LL7F;else{_tmp142=_tmp141->f1;}}_LL7E: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp142),e->loc);_LL7F: {struct
Cyc_Absyn_New_e_struct*_tmp143=(struct Cyc_Absyn_New_e_struct*)_tmp10F;if(_tmp143->tag
!= 17)goto _LL81;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;}}_LL80: {struct Cyc_Absyn_Exp*
eo1=_tmp144;if(_tmp144 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp144);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp145),e->loc);}_LL81: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp146=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp10F;if(_tmp146->tag != 18)goto _LL83;else{_tmp147=(void*)_tmp146->f1;}}_LL82:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp147),e->loc);_LL83: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp148=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp10F;if(_tmp148->tag != 19)goto _LL85;else{_tmp149=_tmp148->f1;}}_LL84: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL85: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp14A=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp10F;
if(_tmp14A->tag != 20)goto _LL87;else{_tmp14B=(void*)_tmp14A->f1;_tmp14C=(void*)
_tmp14A->f2;}}_LL86: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp14B),
_tmp14C,e->loc);_LL87: {struct Cyc_Absyn_Deref_e_struct*_tmp14D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp10F;if(_tmp14D->tag != 21)goto _LL89;else{_tmp14E=_tmp14D->f1;}}_LL88: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp14E),e->loc);_LL89: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp14F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp10F;if(_tmp14F->tag != 22)goto
_LL8B;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=
_tmp14F->f4;}}_LL8A: {struct Cyc_Absyn_AggrMember_e_struct _tmp10A2;struct Cyc_Absyn_AggrMember_e_struct*
_tmp10A1;return Cyc_Absyn_new_exp((void*)((_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((
_tmp10A1[0]=((_tmp10A2.tag=22,((_tmp10A2.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((
_tmp10A2.f2=_tmp151,((_tmp10A2.f3=_tmp152,((_tmp10A2.f4=_tmp153,_tmp10A2)))))))))),
_tmp10A1)))),e->loc);}_LL8B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp154=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp10F;if(_tmp154->tag != 23)goto _LL8D;else{
_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;}}
_LL8C: {struct Cyc_Absyn_AggrArrow_e_struct _tmp10A5;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp10A4;return Cyc_Absyn_new_exp((void*)((_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((
_tmp10A4[0]=((_tmp10A5.tag=23,((_tmp10A5.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp10A5.f2=_tmp156,((_tmp10A5.f3=_tmp157,((_tmp10A5.f4=_tmp158,_tmp10A5)))))))))),
_tmp10A4)))),e->loc);}_LL8D: {struct Cyc_Absyn_Subscript_e_struct*_tmp159=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp10F;if(_tmp159->tag != 24)goto _LL8F;else{
_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;}}_LL8E: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp15A),Cyc_Tcutil_deep_copy_exp(_tmp15B),e->loc);
_LL8F: {struct Cyc_Absyn_Tuple_e_struct*_tmp15C=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp10F;if(_tmp15C->tag != 25)goto _LL91;else{_tmp15D=_tmp15C->f1;}}_LL90: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp15D),e->loc);
_LL91: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp15E=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp10F;if(_tmp15E->tag != 26)goto _LL93;else{_tmp15F=_tmp15E->f1;_tmp160=*_tmp15F;
_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;_tmp163=_tmp160.f3;_tmp164=_tmp15E->f2;}}
_LL92: {struct Cyc_Core_Opt*vopt1=_tmp161;if(_tmp161 != 0){struct Cyc_Core_Opt*
_tmp10A6;vopt1=((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6->v=(struct
_dyneither_ptr*)_tmp161->v,_tmp10A6))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp10AC;struct _tuple9*_tmp10AB;struct Cyc_Absyn_CompoundLit_e_struct*_tmp10AA;
return Cyc_Absyn_new_exp((void*)((_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((
_tmp10AA[0]=((_tmp10AC.tag=26,((_tmp10AC.f1=((_tmp10AB=_cycalloc(sizeof(*
_tmp10AB)),((_tmp10AB->f1=vopt1,((_tmp10AB->f2=_tmp162,((_tmp10AB->f3=Cyc_Tcutil_copy_type(
_tmp163),_tmp10AB)))))))),((_tmp10AC.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp164),_tmp10AC)))))),_tmp10AA)))),e->loc);};}_LL93: {struct Cyc_Absyn_Array_e_struct*
_tmp165=(struct Cyc_Absyn_Array_e_struct*)_tmp10F;if(_tmp165->tag != 27)goto _LL95;
else{_tmp166=_tmp165->f1;}}_LL94: {struct Cyc_Absyn_Array_e_struct _tmp10AF;struct
Cyc_Absyn_Array_e_struct*_tmp10AE;return Cyc_Absyn_new_exp((void*)((_tmp10AE=
_cycalloc(sizeof(*_tmp10AE)),((_tmp10AE[0]=((_tmp10AF.tag=27,((_tmp10AF.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp166),_tmp10AF)))),_tmp10AE)))),e->loc);}
_LL95: {struct Cyc_Absyn_Comprehension_e_struct*_tmp167=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp10F;if(_tmp167->tag != 28)goto _LL97;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;
_tmp16A=_tmp167->f3;_tmp16B=_tmp167->f4;}}_LL96: {struct Cyc_Absyn_Comprehension_e_struct
_tmp10B2;struct Cyc_Absyn_Comprehension_e_struct*_tmp10B1;return Cyc_Absyn_new_exp((
void*)((_tmp10B1=_cycalloc(sizeof(*_tmp10B1)),((_tmp10B1[0]=((_tmp10B2.tag=28,((
_tmp10B2.f1=_tmp168,((_tmp10B2.f2=Cyc_Tcutil_deep_copy_exp(_tmp169),((_tmp10B2.f3=
Cyc_Tcutil_deep_copy_exp(_tmp16A),((_tmp10B2.f4=_tmp16B,_tmp10B2)))))))))),
_tmp10B1)))),e->loc);}_LL97: {struct Cyc_Absyn_Aggregate_e_struct*_tmp16C=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp10F;if(_tmp16C->tag != 29)goto _LL99;else{
_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;_tmp16F=_tmp16C->f3;_tmp170=_tmp16C->f4;}}
_LL98: {struct Cyc_Absyn_Aggregate_e_struct _tmp10B5;struct Cyc_Absyn_Aggregate_e_struct*
_tmp10B4;return Cyc_Absyn_new_exp((void*)((_tmp10B4=_cycalloc(sizeof(*_tmp10B4)),((
_tmp10B4[0]=((_tmp10B5.tag=29,((_tmp10B5.f1=_tmp16D,((_tmp10B5.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp16E),((_tmp10B5.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16F),((
_tmp10B5.f4=_tmp170,_tmp10B5)))))))))),_tmp10B4)))),e->loc);}_LL99: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp171=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp10F;if(_tmp171->tag != 30)goto
_LL9B;else{_tmp172=(void*)_tmp171->f1;_tmp173=_tmp171->f2;}}_LL9A: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp10B8;struct Cyc_Absyn_AnonStruct_e_struct*_tmp10B7;return Cyc_Absyn_new_exp((
void*)((_tmp10B7=_cycalloc(sizeof(*_tmp10B7)),((_tmp10B7[0]=((_tmp10B8.tag=30,((
_tmp10B8.f1=(void*)Cyc_Tcutil_copy_type(_tmp172),((_tmp10B8.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp173),_tmp10B8)))))),_tmp10B7)))),e->loc);}_LL9B: {struct Cyc_Absyn_Datatype_e_struct*
_tmp174=(struct Cyc_Absyn_Datatype_e_struct*)_tmp10F;if(_tmp174->tag != 31)goto
_LL9D;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;_tmp177=_tmp174->f3;}}_LL9C: {
struct Cyc_Absyn_Datatype_e_struct _tmp10BB;struct Cyc_Absyn_Datatype_e_struct*
_tmp10BA;return Cyc_Absyn_new_exp((void*)((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((
_tmp10BA[0]=((_tmp10BB.tag=31,((_tmp10BB.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp175),((_tmp10BB.f2=_tmp176,((_tmp10BB.f3=_tmp177,_tmp10BB)))))))),_tmp10BA)))),
e->loc);}_LL9D: {struct Cyc_Absyn_Enum_e_struct*_tmp178=(struct Cyc_Absyn_Enum_e_struct*)
_tmp10F;if(_tmp178->tag != 32)goto _LL9F;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;
_tmp17B=_tmp178->f3;}}_LL9E: return e;_LL9F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp17C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp10F;if(_tmp17C->tag != 33)goto
_LLA1;else{_tmp17D=_tmp17C->f1;_tmp17E=(void*)_tmp17C->f2;_tmp17F=_tmp17C->f3;}}
_LLA0: {struct Cyc_Absyn_AnonEnum_e_struct _tmp10BE;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp10BD;return Cyc_Absyn_new_exp((void*)((_tmp10BD=_cycalloc(sizeof(*_tmp10BD)),((
_tmp10BD[0]=((_tmp10BE.tag=33,((_tmp10BE.f1=_tmp17D,((_tmp10BE.f2=(void*)Cyc_Tcutil_copy_type(
_tmp17E),((_tmp10BE.f3=_tmp17F,_tmp10BE)))))))),_tmp10BD)))),e->loc);}_LLA1: {
struct Cyc_Absyn_Malloc_e_struct*_tmp180=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp10F;if(_tmp180->tag != 34)goto _LLA3;else{_tmp181=_tmp180->f1;_tmp182=_tmp181.is_calloc;
_tmp183=_tmp181.rgn;_tmp184=_tmp181.elt_type;_tmp185=_tmp181.num_elts;_tmp186=
_tmp181.fat_result;}}_LLA2: {struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp183;if(_tmp183 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp183);{void**t1=_tmp184;if(_tmp184 != 0){void**_tmp10BF;
t1=((_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=Cyc_Tcutil_copy_type(*
_tmp184),_tmp10BF))));}{struct Cyc_Absyn_Malloc_e_struct _tmp10C5;struct Cyc_Absyn_MallocInfo
_tmp10C4;struct Cyc_Absyn_Malloc_e_struct*_tmp10C3;_tmp1B9->r=(void*)((_tmp10C3=
_cycalloc(sizeof(*_tmp10C3)),((_tmp10C3[0]=((_tmp10C5.tag=34,((_tmp10C5.f1=((
_tmp10C4.is_calloc=_tmp182,((_tmp10C4.rgn=r1,((_tmp10C4.elt_type=t1,((_tmp10C4.num_elts=
_tmp185,((_tmp10C4.fat_result=_tmp186,_tmp10C4)))))))))),_tmp10C5)))),_tmp10C3))));}
return _tmp1B9;};}_LLA3: {struct Cyc_Absyn_Swap_e_struct*_tmp187=(struct Cyc_Absyn_Swap_e_struct*)
_tmp10F;if(_tmp187->tag != 35)goto _LLA5;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}
_LLA4: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp188),Cyc_Tcutil_deep_copy_exp(
_tmp189),e->loc);_LLA5: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp18A=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp10F;if(_tmp18A->tag != 36)goto _LLA7;else{
_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLA6: {struct Cyc_Core_Opt*nopt1=
_tmp18B;if(_tmp18B != 0){struct Cyc_Core_Opt*_tmp10C6;nopt1=((_tmp10C6=_cycalloc(
sizeof(*_tmp10C6)),((_tmp10C6->v=(struct _tuple2*)_tmp18B->v,_tmp10C6))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp10C9;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp10C8;return Cyc_Absyn_new_exp((void*)((_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((
_tmp10C8[0]=((_tmp10C9.tag=36,((_tmp10C9.f1=nopt1,((_tmp10C9.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp18C),_tmp10C9)))))),_tmp10C8)))),e->loc);};}_LLA7: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp18D=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp10F;if(_tmp18D->tag != 37)goto
_LLA9;}_LLA8: {struct Cyc_Core_Failure_struct _tmp10CF;const char*_tmp10CE;struct
Cyc_Core_Failure_struct*_tmp10CD;(int)_throw((void*)((_tmp10CD=_cycalloc(sizeof(*
_tmp10CD)),((_tmp10CD[0]=((_tmp10CF.tag=Cyc_Core_Failure,((_tmp10CF.f1=((
_tmp10CE="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp10CE,
sizeof(char),45))),_tmp10CF)))),_tmp10CD)))));}_LLA9: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp18E=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp10F;if(_tmp18E->tag != 38)goto
_LLAB;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LLAA: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp10D2;struct Cyc_Absyn_Tagcheck_e_struct*_tmp10D1;return Cyc_Absyn_new_exp((
void*)((_tmp10D1=_cycalloc(sizeof(*_tmp10D1)),((_tmp10D1[0]=((_tmp10D2.tag=38,((
_tmp10D2.f1=Cyc_Tcutil_deep_copy_exp(_tmp18F),((_tmp10D2.f2=_tmp190,_tmp10D2)))))),
_tmp10D1)))),e->loc);}_LLAB: {struct Cyc_Absyn_Valueof_e_struct*_tmp191=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp10F;if(_tmp191->tag != 39)goto _LL5C;else{_tmp192=(
void*)_tmp191->f1;}}_LLAC: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp192),e->loc);_LL5C:;}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual
f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};int
Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1C7;
enum Cyc_Absyn_KindQual _tmp1C8;enum Cyc_Absyn_AliasQual _tmp1C9;struct Cyc_Absyn_Kind*
_tmp1C6=ka1;_tmp1C7=*_tmp1C6;_tmp1C8=_tmp1C7.kind;_tmp1C9=_tmp1C7.aliasqual;{
struct Cyc_Absyn_Kind _tmp1CB;enum Cyc_Absyn_KindQual _tmp1CC;enum Cyc_Absyn_AliasQual
_tmp1CD;struct Cyc_Absyn_Kind*_tmp1CA=ka2;_tmp1CB=*_tmp1CA;_tmp1CC=_tmp1CB.kind;
_tmp1CD=_tmp1CB.aliasqual;if(_tmp1C8 != _tmp1CC){struct _tuple19 _tmp10D3;struct
_tuple19 _tmp1CF=(_tmp10D3.f1=_tmp1C8,((_tmp10D3.f2=_tmp1CC,_tmp10D3)));enum Cyc_Absyn_KindQual
_tmp1D0;enum Cyc_Absyn_KindQual _tmp1D1;enum Cyc_Absyn_KindQual _tmp1D2;enum Cyc_Absyn_KindQual
_tmp1D3;enum Cyc_Absyn_KindQual _tmp1D4;enum Cyc_Absyn_KindQual _tmp1D5;_LLB3:
_tmp1D0=_tmp1CF.f1;if(_tmp1D0 != Cyc_Absyn_BoxKind)goto _LLB5;_tmp1D1=_tmp1CF.f2;
if(_tmp1D1 != Cyc_Absyn_MemKind)goto _LLB5;_LLB4: goto _LLB6;_LLB5: _tmp1D2=_tmp1CF.f1;
if(_tmp1D2 != Cyc_Absyn_BoxKind)goto _LLB7;_tmp1D3=_tmp1CF.f2;if(_tmp1D3 != Cyc_Absyn_AnyKind)
goto _LLB7;_LLB6: goto _LLB8;_LLB7: _tmp1D4=_tmp1CF.f1;if(_tmp1D4 != Cyc_Absyn_MemKind)
goto _LLB9;_tmp1D5=_tmp1CF.f2;if(_tmp1D5 != Cyc_Absyn_AnyKind)goto _LLB9;_LLB8: goto
_LLB2;_LLB9:;_LLBA: return 0;_LLB2:;}if(_tmp1C9 != _tmp1CD){struct _tuple20 _tmp10D4;
struct _tuple20 _tmp1D7=(_tmp10D4.f1=_tmp1C9,((_tmp10D4.f2=_tmp1CD,_tmp10D4)));
enum Cyc_Absyn_AliasQual _tmp1D8;enum Cyc_Absyn_AliasQual _tmp1D9;enum Cyc_Absyn_AliasQual
_tmp1DA;enum Cyc_Absyn_AliasQual _tmp1DB;_LLBC: _tmp1D8=_tmp1D7.f1;if(_tmp1D8 != 
Cyc_Absyn_Aliasable)goto _LLBE;_tmp1D9=_tmp1D7.f2;if(_tmp1D9 != Cyc_Absyn_Top)goto
_LLBE;_LLBD: goto _LLBF;_LLBE: _tmp1DA=_tmp1D7.f1;if(_tmp1DA != Cyc_Absyn_Unique)
goto _LLC0;_tmp1DB=_tmp1D7.f2;if(_tmp1DB != Cyc_Absyn_Top)goto _LLC0;_LLBF: return 1;
_LLC0:;_LLC1: return 0;_LLBB:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp1DC=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;_LLC3: {
struct Cyc_Absyn_Eq_kb_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1DC;if(
_tmp1DD->tag != 0)goto _LLC5;else{_tmp1DE=_tmp1DD->f1;}}_LLC4: return _tmp1DE;_LLC5: {
struct Cyc_Absyn_Less_kb_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_struct*)_tmp1DC;
if(_tmp1DF->tag != 2)goto _LLC7;else{_tmp1E0=_tmp1DF->f2;}}_LLC6: return _tmp1E0;
_LLC7:;_LLC8:{struct Cyc_Absyn_Less_kb_struct _tmp10D7;struct Cyc_Absyn_Less_kb_struct*
_tmp10D6;tv->kind=(void*)((_tmp10D6=_cycalloc(sizeof(*_tmp10D6)),((_tmp10D6[0]=((
_tmp10D7.tag=2,((_tmp10D7.f1=0,((_tmp10D7.f2=def,_tmp10D7)))))),_tmp10D6))));}
return def;_LLC2:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2){struct _tuple0 _tmp10D8;struct _tuple0 _tmp1E4=(_tmp10D8.f1=Cyc_Absyn_compress_kb(
kb1),((_tmp10D8.f2=Cyc_Absyn_compress_kb(kb2),_tmp10D8)));void*_tmp1E5;struct Cyc_Absyn_Kind*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Kind*_tmp1EA;void*_tmp1EB;struct Cyc_Core_Opt*
_tmp1ED;struct Cyc_Core_Opt**_tmp1EE;struct Cyc_Absyn_Kind*_tmp1EF;void*_tmp1F0;
struct Cyc_Absyn_Kind*_tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Kind*_tmp1F5;void*
_tmp1F6;struct Cyc_Core_Opt*_tmp1F8;struct Cyc_Core_Opt**_tmp1F9;struct Cyc_Absyn_Kind*
_tmp1FA;void*_tmp1FB;struct Cyc_Core_Opt*_tmp1FD;struct Cyc_Core_Opt**_tmp1FE;
struct Cyc_Absyn_Kind*_tmp1FF;void*_tmp200;struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt**
_tmp203;struct Cyc_Absyn_Kind*_tmp204;void*_tmp205;struct Cyc_Core_Opt*_tmp207;
struct Cyc_Core_Opt**_tmp208;void*_tmp209;void*_tmp20A;void*_tmp20B;struct Cyc_Core_Opt*
_tmp20D;struct Cyc_Core_Opt**_tmp20E;_LLCA: _tmp1E5=_tmp1E4.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1E6=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1E5;if(_tmp1E6->tag != 0)goto _LLCC;
else{_tmp1E7=_tmp1E6->f1;}};_tmp1E8=_tmp1E4.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1E9=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1E8;if(_tmp1E9->tag != 0)goto _LLCC;
else{_tmp1EA=_tmp1E9->f1;}};_LLCB: return _tmp1E7 == _tmp1EA;_LLCC: _tmp1EB=_tmp1E4.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp1EC=(struct Cyc_Absyn_Less_kb_struct*)_tmp1EB;
if(_tmp1EC->tag != 2)goto _LLCE;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=(struct Cyc_Core_Opt**)&
_tmp1EC->f1;_tmp1EF=_tmp1EC->f2;}};_tmp1F0=_tmp1E4.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1F1=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LLCE;
else{_tmp1F2=_tmp1F1->f1;}};_LLCD: if(Cyc_Tcutil_kind_leq(_tmp1F2,_tmp1EF)){{void**
_tmp10D9;*_tmp1EE=(struct Cyc_Core_Opt*)((_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((
_tmp10D9[0]=kb2,_tmp10D9))));}return 1;}else{return 0;}_LLCE: _tmp1F3=_tmp1E4.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp1F4=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F3;if(
_tmp1F4->tag != 0)goto _LLD0;else{_tmp1F5=_tmp1F4->f1;}};_tmp1F6=_tmp1E4.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp1F7=(struct Cyc_Absyn_Less_kb_struct*)_tmp1F6;
if(_tmp1F7->tag != 2)goto _LLD0;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=(struct Cyc_Core_Opt**)&
_tmp1F7->f1;_tmp1FA=_tmp1F7->f2;}};_LLCF: if(Cyc_Tcutil_kind_leq(_tmp1F5,_tmp1FA)){{
void**_tmp10DA;*_tmp1F9=(struct Cyc_Core_Opt*)((_tmp10DA=_cycalloc(sizeof(*
_tmp10DA)),((_tmp10DA[0]=kb1,_tmp10DA))));}return 1;}else{return 0;}_LLD0: _tmp1FB=
_tmp1E4.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp1FC=(struct Cyc_Absyn_Less_kb_struct*)
_tmp1FB;if(_tmp1FC->tag != 2)goto _LLD2;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=(struct
Cyc_Core_Opt**)& _tmp1FC->f1;_tmp1FF=_tmp1FC->f2;}};_tmp200=_tmp1E4.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp201=(struct Cyc_Absyn_Less_kb_struct*)_tmp200;if(_tmp201->tag != 2)goto _LLD2;
else{_tmp202=_tmp201->f1;_tmp203=(struct Cyc_Core_Opt**)& _tmp201->f1;_tmp204=
_tmp201->f2;}};_LLD1: if(Cyc_Tcutil_kind_leq(_tmp1FF,_tmp204)){{void**_tmp10DB;*
_tmp203=(struct Cyc_Core_Opt*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[
0]=kb1,_tmp10DB))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp204,_tmp1FF)){{
void**_tmp10DC;*_tmp1FE=(struct Cyc_Core_Opt*)((_tmp10DC=_cycalloc(sizeof(*
_tmp10DC)),((_tmp10DC[0]=kb2,_tmp10DC))));}return 1;}else{return 0;}}_LLD2: _tmp205=
_tmp1E4.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp206=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp205;if(_tmp206->tag != 1)goto _LLD4;else{_tmp207=_tmp206->f1;_tmp208=(struct
Cyc_Core_Opt**)& _tmp206->f1;}};_tmp209=_tmp1E4.f2;_LLD3: _tmp20A=_tmp209;_tmp20E=
_tmp208;goto _LLD5;_LLD4: _tmp20A=_tmp1E4.f1;_tmp20B=_tmp1E4.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp20C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp20B;if(_tmp20C->tag != 1)goto
_LLC9;else{_tmp20D=_tmp20C->f1;_tmp20E=(struct Cyc_Core_Opt**)& _tmp20C->f1;}};
_LLD5:{void**_tmp10DD;*_tmp20E=(struct Cyc_Core_Opt*)((_tmp10DD=_cycalloc(sizeof(*
_tmp10DD)),((_tmp10DD[0]=_tmp20A,_tmp10DD))));}return 1;_LLC9:;}struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp214=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp216;_LLD7: {struct Cyc_Absyn_VarType_struct*
_tmp215=(struct Cyc_Absyn_VarType_struct*)_tmp214;if(_tmp215->tag != 2)goto _LLD9;
else{_tmp216=_tmp215->f1;}}_LLD8: {void*_tmp217=_tmp216->kind;_tmp216->kind=kb;{
struct _tuple15 _tmp10DE;return(_tmp10DE.f1=_tmp216,((_tmp10DE.f2=_tmp217,_tmp10DE)));};}
_LLD9:;_LLDA: {const char*_tmp10E2;void*_tmp10E1[1];struct Cyc_String_pa_struct
_tmp10E0;(_tmp10E0.tag=0,((_tmp10E0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10E1[0]=& _tmp10E0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E2="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp10E2,sizeof(char),40))),_tag_dyneither(_tmp10E1,sizeof(void*),
1)))))));}_LLD6:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);struct Cyc_Absyn_Kind*
Cyc_Tcutil_typ_kind(void*t){void*_tmp21C=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp21E;struct Cyc_Absyn_Tvar*_tmp220;enum Cyc_Absyn_Size_of _tmp223;struct Cyc_Absyn_DatatypeFieldInfo
_tmp22F;union Cyc_Absyn_DatatypeFieldInfoU _tmp230;struct _tuple3 _tmp231;struct Cyc_Absyn_Datatypedecl*
_tmp232;struct Cyc_Absyn_Datatypefield*_tmp233;struct Cyc_Absyn_DatatypeFieldInfo
_tmp235;union Cyc_Absyn_DatatypeFieldInfoU _tmp236;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp237;struct Cyc_Absyn_AggrInfo _tmp239;union Cyc_Absyn_AggrInfoU _tmp23A;struct
_tuple4 _tmp23B;struct Cyc_Absyn_AggrInfo _tmp23D;union Cyc_Absyn_AggrInfoU _tmp23E;
struct Cyc_Absyn_Aggrdecl**_tmp23F;struct Cyc_Absyn_Aggrdecl*_tmp240;struct Cyc_Absyn_Aggrdecl
_tmp241;struct Cyc_Absyn_AggrdeclImpl*_tmp242;struct Cyc_Absyn_PtrInfo _tmp245;
struct Cyc_Absyn_Typedefdecl*_tmp24B;_LLDC: {struct Cyc_Absyn_Evar_struct*_tmp21D=(
struct Cyc_Absyn_Evar_struct*)_tmp21C;if(_tmp21D->tag != 1)goto _LLDE;else{_tmp21E=
_tmp21D->f1;}}_LLDD: return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp21E))->v;_LLDE: {struct Cyc_Absyn_VarType_struct*_tmp21F=(struct
Cyc_Absyn_VarType_struct*)_tmp21C;if(_tmp21F->tag != 2)goto _LLE0;else{_tmp220=
_tmp21F->f1;}}_LLDF: return Cyc_Tcutil_tvar_kind(_tmp220,& Cyc_Tcutil_bk);_LLE0: {
struct Cyc_Absyn_VoidType_struct*_tmp221=(struct Cyc_Absyn_VoidType_struct*)
_tmp21C;if(_tmp221->tag != 0)goto _LLE2;}_LLE1: return& Cyc_Tcutil_mk;_LLE2: {struct
Cyc_Absyn_IntType_struct*_tmp222=(struct Cyc_Absyn_IntType_struct*)_tmp21C;if(
_tmp222->tag != 6)goto _LLE4;else{_tmp223=_tmp222->f2;}}_LLE3: return(_tmp223 == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp223 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLE4: {struct Cyc_Absyn_FloatType_struct*_tmp224=(
struct Cyc_Absyn_FloatType_struct*)_tmp21C;if(_tmp224->tag != 7)goto _LLE6;}_LLE5:
goto _LLE7;_LLE6: {struct Cyc_Absyn_DoubleType_struct*_tmp225=(struct Cyc_Absyn_DoubleType_struct*)
_tmp21C;if(_tmp225->tag != 8)goto _LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_FnType_struct*
_tmp226=(struct Cyc_Absyn_FnType_struct*)_tmp21C;if(_tmp226->tag != 10)goto _LLEA;}
_LLE9: return& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_DynRgnType_struct*_tmp227=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp21C;if(_tmp227->tag != 17)goto _LLEC;}_LLEB:
goto _LLED;_LLEC: {struct Cyc_Absyn_EnumType_struct*_tmp228=(struct Cyc_Absyn_EnumType_struct*)
_tmp21C;if(_tmp228->tag != 14)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp229=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp21C;if(_tmp229->tag != 15)goto
_LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp22A=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp21C;if(_tmp22A->tag != 16)goto _LLF2;}
_LLF1: return& Cyc_Tcutil_bk;_LLF2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp22B=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp21C;if(_tmp22B->tag != 22)goto _LLF4;}_LLF3:
return& Cyc_Tcutil_urk;_LLF4: {struct Cyc_Absyn_HeapRgn_struct*_tmp22C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp21C;if(_tmp22C->tag != 21)goto _LLF6;}_LLF5: return& Cyc_Tcutil_rk;_LLF6: {
struct Cyc_Absyn_DatatypeType_struct*_tmp22D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp21C;if(_tmp22D->tag != 3)goto _LLF8;}_LLF7: return& Cyc_Tcutil_ak;_LLF8: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp22E=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp21C;if(_tmp22E->tag != 4)goto _LLFA;else{_tmp22F=_tmp22E->f1;_tmp230=_tmp22F.field_info;
if((_tmp230.KnownDatatypefield).tag != 2)goto _LLFA;_tmp231=(struct _tuple3)(
_tmp230.KnownDatatypefield).val;_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;}}_LLF9:
return& Cyc_Tcutil_mk;_LLFA: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp234=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp21C;if(_tmp234->tag != 4)goto _LLFC;
else{_tmp235=_tmp234->f1;_tmp236=_tmp235.field_info;if((_tmp236.UnknownDatatypefield).tag
!= 1)goto _LLFC;_tmp237=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp236.UnknownDatatypefield).val;}}
_LLFB: {const char*_tmp10E5;void*_tmp10E4;(_tmp10E4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E5="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp10E5,sizeof(char),39))),_tag_dyneither(_tmp10E4,sizeof(void*),
0)));}_LLFC: {struct Cyc_Absyn_AggrType_struct*_tmp238=(struct Cyc_Absyn_AggrType_struct*)
_tmp21C;if(_tmp238->tag != 12)goto _LLFE;else{_tmp239=_tmp238->f1;_tmp23A=_tmp239.aggr_info;
if((_tmp23A.UnknownAggr).tag != 1)goto _LLFE;_tmp23B=(struct _tuple4)(_tmp23A.UnknownAggr).val;}}
_LLFD: return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_AggrType_struct*_tmp23C=(
struct Cyc_Absyn_AggrType_struct*)_tmp21C;if(_tmp23C->tag != 12)goto _LL100;else{
_tmp23D=_tmp23C->f1;_tmp23E=_tmp23D.aggr_info;if((_tmp23E.KnownAggr).tag != 2)
goto _LL100;_tmp23F=(struct Cyc_Absyn_Aggrdecl**)(_tmp23E.KnownAggr).val;_tmp240=*
_tmp23F;_tmp241=*_tmp240;_tmp242=_tmp241.impl;}}_LLFF: return _tmp242 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_AnonAggrType_struct*_tmp243=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp21C;if(_tmp243->tag != 13)goto _LL102;}_LL101: return& Cyc_Tcutil_mk;_LL102: {
struct Cyc_Absyn_PointerType_struct*_tmp244=(struct Cyc_Absyn_PointerType_struct*)
_tmp21C;if(_tmp244->tag != 5)goto _LL104;else{_tmp245=_tmp244->f1;}}_LL103: {void*
_tmp251=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp245.ptr_atts).bounds);_LL115: {struct Cyc_Absyn_DynEither_b_struct*
_tmp252=(struct Cyc_Absyn_DynEither_b_struct*)_tmp251;if(_tmp252->tag != 0)goto
_LL117;}_LL116: {struct Cyc_Absyn_Kind*_tmp10E6;return(_tmp10E6=_cycalloc_atomic(
sizeof(*_tmp10E6)),((_tmp10E6->kind=Cyc_Absyn_MemKind,((_tmp10E6->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp245.ptr_atts).rgn))->aliasqual,_tmp10E6)))));}_LL117: {struct Cyc_Absyn_Upper_b_struct*
_tmp253=(struct Cyc_Absyn_Upper_b_struct*)_tmp251;if(_tmp253->tag != 1)goto _LL114;}
_LL118: {struct Cyc_Absyn_Kind*_tmp10E7;return(_tmp10E7=_cycalloc_atomic(sizeof(*
_tmp10E7)),((_tmp10E7->kind=Cyc_Absyn_BoxKind,((_tmp10E7->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp245.ptr_atts).rgn))->aliasqual,_tmp10E7)))));}_LL114:;}_LL104: {struct Cyc_Absyn_ValueofType_struct*
_tmp246=(struct Cyc_Absyn_ValueofType_struct*)_tmp21C;if(_tmp246->tag != 19)goto
_LL106;}_LL105: return& Cyc_Tcutil_ik;_LL106: {struct Cyc_Absyn_TagType_struct*
_tmp247=(struct Cyc_Absyn_TagType_struct*)_tmp21C;if(_tmp247->tag != 20)goto _LL108;}
_LL107: return& Cyc_Tcutil_bk;_LL108: {struct Cyc_Absyn_ArrayType_struct*_tmp248=(
struct Cyc_Absyn_ArrayType_struct*)_tmp21C;if(_tmp248->tag != 9)goto _LL10A;}_LL109:
goto _LL10B;_LL10A: {struct Cyc_Absyn_TupleType_struct*_tmp249=(struct Cyc_Absyn_TupleType_struct*)
_tmp21C;if(_tmp249->tag != 11)goto _LL10C;}_LL10B: return& Cyc_Tcutil_mk;_LL10C: {
struct Cyc_Absyn_TypedefType_struct*_tmp24A=(struct Cyc_Absyn_TypedefType_struct*)
_tmp21C;if(_tmp24A->tag != 18)goto _LL10E;else{_tmp24B=_tmp24A->f3;}}_LL10D: if(
_tmp24B == 0  || _tmp24B->kind == 0){const char*_tmp10EB;void*_tmp10EA[1];struct Cyc_String_pa_struct
_tmp10E9;(_tmp10E9.tag=0,((_tmp10E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10EA[0]=& _tmp10E9,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EB="typ_kind: typedef found: %s",
_tag_dyneither(_tmp10EB,sizeof(char),28))),_tag_dyneither(_tmp10EA,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp24B->kind))->v;
_LL10E: {struct Cyc_Absyn_AccessEff_struct*_tmp24C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp21C;if(_tmp24C->tag != 23)goto _LL110;}_LL10F: goto _LL111;_LL110: {struct Cyc_Absyn_JoinEff_struct*
_tmp24D=(struct Cyc_Absyn_JoinEff_struct*)_tmp21C;if(_tmp24D->tag != 24)goto _LL112;}
_LL111: goto _LL113;_LL112: {struct Cyc_Absyn_RgnsEff_struct*_tmp24E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp21C;if(_tmp24E->tag != 25)goto _LLDB;}_LL113: return& Cyc_Tcutil_ek;_LLDB:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);int Cyc_Tcutil_kind_eq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == k2  || k1->kind == k2->kind
 && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp259;_push_handler(& _tmp259);{int _tmp25B=
0;if(setjmp(_tmp259.handler))_tmp25B=1;if(!_tmp25B){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp25C=1;_npop_handler(0);return _tmp25C;};;_pop_handler();}else{void*_tmp25A=(
void*)_exn_thrown;void*_tmp25E=_tmp25A;_LL11A: {struct Cyc_Tcutil_Unify_struct*
_tmp25F=(struct Cyc_Tcutil_Unify_struct*)_tmp25E;if(_tmp25F->tag != Cyc_Tcutil_Unify)
goto _LL11C;}_LL11B: return 0;_LL11C:;_LL11D:(void)_throw(_tmp25E);_LL119:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp260=t;struct Cyc_Absyn_Tvar*_tmp262;struct Cyc_Core_Opt*_tmp264;
struct Cyc_Core_Opt*_tmp265;struct Cyc_Core_Opt**_tmp266;struct Cyc_Absyn_PtrInfo
_tmp268;struct Cyc_Absyn_ArrayInfo _tmp26A;void*_tmp26B;struct Cyc_Absyn_FnInfo
_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_Core_Opt*_tmp26F;void*_tmp270;
struct Cyc_List_List*_tmp271;int _tmp272;struct Cyc_Absyn_VarargInfo*_tmp273;struct
Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp277;
struct Cyc_Absyn_DatatypeInfo _tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*
_tmp27C;struct Cyc_Absyn_DatatypeFieldInfo _tmp27E;struct Cyc_List_List*_tmp27F;
struct Cyc_Absyn_AggrInfo _tmp281;struct Cyc_List_List*_tmp282;struct Cyc_List_List*
_tmp284;void*_tmp286;void*_tmp288;void*_tmp28A;void*_tmp28C;struct Cyc_List_List*
_tmp28E;_LL11F: {struct Cyc_Absyn_VarType_struct*_tmp261=(struct Cyc_Absyn_VarType_struct*)
_tmp260;if(_tmp261->tag != 2)goto _LL121;else{_tmp262=_tmp261->f1;}}_LL120: if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp262)){{
const char*_tmp10EC;Cyc_Tcutil_failure_reason=((_tmp10EC="(type variable would escape scope)",
_tag_dyneither(_tmp10EC,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LL11E;_LL121: {struct Cyc_Absyn_Evar_struct*_tmp263=(struct Cyc_Absyn_Evar_struct*)
_tmp260;if(_tmp263->tag != 1)goto _LL123;else{_tmp264=_tmp263->f2;_tmp265=_tmp263->f4;
_tmp266=(struct Cyc_Core_Opt**)& _tmp263->f4;}}_LL122: if(t == evar){{const char*
_tmp10ED;Cyc_Tcutil_failure_reason=((_tmp10ED="(occurs check)",_tag_dyneither(
_tmp10ED,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp264 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp264->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp266))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp291=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp266))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmp10EE;_tmp291=((_tmp10EE=_cycalloc(sizeof(*
_tmp10EE)),((_tmp10EE->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp10EE->tl=_tmp291,
_tmp10EE))))));}}}{struct Cyc_Core_Opt*_tmp10EF;*_tmp266=((_tmp10EF=_cycalloc(
sizeof(*_tmp10EF)),((_tmp10EF->v=_tmp291,_tmp10EF))));};}}}goto _LL11E;_LL123: {
struct Cyc_Absyn_PointerType_struct*_tmp267=(struct Cyc_Absyn_PointerType_struct*)
_tmp260;if(_tmp267->tag != 5)goto _LL125;else{_tmp268=_tmp267->f1;}}_LL124: Cyc_Tcutil_occurs(
evar,r,env,_tmp268.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp268.ptr_atts).rgn);
goto _LL11E;_LL125: {struct Cyc_Absyn_ArrayType_struct*_tmp269=(struct Cyc_Absyn_ArrayType_struct*)
_tmp260;if(_tmp269->tag != 9)goto _LL127;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp26A.elt_type;}}
_LL126: Cyc_Tcutil_occurs(evar,r,env,_tmp26B);goto _LL11E;_LL127: {struct Cyc_Absyn_FnType_struct*
_tmp26C=(struct Cyc_Absyn_FnType_struct*)_tmp260;if(_tmp26C->tag != 10)goto _LL129;
else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26D.tvars;_tmp26F=_tmp26D.effect;_tmp270=
_tmp26D.ret_typ;_tmp271=_tmp26D.args;_tmp272=_tmp26D.c_varargs;_tmp273=_tmp26D.cyc_varargs;
_tmp274=_tmp26D.rgn_po;_tmp275=_tmp26D.attributes;}}_LL128: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp26E,env);if(_tmp26F != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp26F->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp270);for(0;_tmp271 != 0;_tmp271=_tmp271->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp271->hd)).f3);}if(_tmp273 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp273->type);for(0;_tmp274 != 0;_tmp274=_tmp274->tl){struct _tuple0
_tmp295;void*_tmp296;void*_tmp297;struct _tuple0*_tmp294=(struct _tuple0*)_tmp274->hd;
_tmp295=*_tmp294;_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp296);Cyc_Tcutil_occurs(evar,r,env,_tmp297);}goto _LL11E;_LL129: {struct
Cyc_Absyn_TupleType_struct*_tmp276=(struct Cyc_Absyn_TupleType_struct*)_tmp260;
if(_tmp276->tag != 11)goto _LL12B;else{_tmp277=_tmp276->f1;}}_LL12A: for(0;_tmp277
!= 0;_tmp277=_tmp277->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)
_tmp277->hd)).f2);}goto _LL11E;_LL12B: {struct Cyc_Absyn_DatatypeType_struct*
_tmp278=(struct Cyc_Absyn_DatatypeType_struct*)_tmp260;if(_tmp278->tag != 3)goto
_LL12D;else{_tmp279=_tmp278->f1;_tmp27A=_tmp279.targs;}}_LL12C: Cyc_Tcutil_occurslist(
evar,r,env,_tmp27A);goto _LL11E;_LL12D: {struct Cyc_Absyn_TypedefType_struct*
_tmp27B=(struct Cyc_Absyn_TypedefType_struct*)_tmp260;if(_tmp27B->tag != 18)goto
_LL12F;else{_tmp27C=_tmp27B->f2;}}_LL12E: _tmp27F=_tmp27C;goto _LL130;_LL12F: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp27D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp260;if(_tmp27D->tag != 4)goto _LL131;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27E.targs;}}
_LL130: _tmp282=_tmp27F;goto _LL132;_LL131: {struct Cyc_Absyn_AggrType_struct*
_tmp280=(struct Cyc_Absyn_AggrType_struct*)_tmp260;if(_tmp280->tag != 12)goto
_LL133;else{_tmp281=_tmp280->f1;_tmp282=_tmp281.targs;}}_LL132: Cyc_Tcutil_occurslist(
evar,r,env,_tmp282);goto _LL11E;_LL133: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp283=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp260;if(_tmp283->tag != 13)goto
_LL135;else{_tmp284=_tmp283->f2;}}_LL134: for(0;_tmp284 != 0;_tmp284=_tmp284->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp284->hd)->type);}
goto _LL11E;_LL135: {struct Cyc_Absyn_TagType_struct*_tmp285=(struct Cyc_Absyn_TagType_struct*)
_tmp260;if(_tmp285->tag != 20)goto _LL137;else{_tmp286=(void*)_tmp285->f1;}}_LL136:
_tmp288=_tmp286;goto _LL138;_LL137: {struct Cyc_Absyn_AccessEff_struct*_tmp287=(
struct Cyc_Absyn_AccessEff_struct*)_tmp260;if(_tmp287->tag != 23)goto _LL139;else{
_tmp288=(void*)_tmp287->f1;}}_LL138: _tmp28A=_tmp288;goto _LL13A;_LL139: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp289=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp260;if(_tmp289->tag != 16)goto _LL13B;else{_tmp28A=(void*)_tmp289->f1;}}_LL13A:
_tmp28C=_tmp28A;goto _LL13C;_LL13B: {struct Cyc_Absyn_RgnsEff_struct*_tmp28B=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp260;if(_tmp28B->tag != 25)goto _LL13D;else{
_tmp28C=(void*)_tmp28B->f1;}}_LL13C: Cyc_Tcutil_occurs(evar,r,env,_tmp28C);goto
_LL11E;_LL13D: {struct Cyc_Absyn_JoinEff_struct*_tmp28D=(struct Cyc_Absyn_JoinEff_struct*)
_tmp260;if(_tmp28D->tag != 24)goto _LL13F;else{_tmp28E=_tmp28D->f1;}}_LL13E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp28E);goto _LL11E;_LL13F:;_LL140: goto _LL11E;_LL11E:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmp10F2;void*_tmp10F1;(_tmp10F1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F2="tq1 real_const not set.",
_tag_dyneither(_tmp10F2,sizeof(char),24))),_tag_dyneither(_tmp10F1,sizeof(void*),
0)));}if(tq2.print_const  && !tq2.real_const){const char*_tmp10F5;void*_tmp10F4;(
_tmp10F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10F5="tq2 real_const not set.",_tag_dyneither(_tmp10F5,sizeof(char),24))),
_tag_dyneither(_tmp10F4,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmp10F6;Cyc_Tcutil_failure_reason=((_tmp10F6="(qualifiers don't match)",
_tag_dyneither(_tmp10F6,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp29D=x;union Cyc_Absyn_Constraint
_tmp29E;int _tmp29F;union Cyc_Absyn_Constraint _tmp2A0;void*_tmp2A1;union Cyc_Absyn_Constraint
_tmp2A2;union Cyc_Absyn_Constraint*_tmp2A3;_LL142: _tmp29E=*_tmp29D;if((_tmp29E.No_constr).tag
!= 3)goto _LL144;_tmp29F=(int)(_tmp29E.No_constr).val;_LL143:{union Cyc_Absyn_Constraint
_tmp10F7;*x=(((_tmp10F7.Forward_constr).val=y,(((_tmp10F7.Forward_constr).tag=2,
_tmp10F7))));}return;_LL144: _tmp2A0=*_tmp29D;if((_tmp2A0.Eq_constr).tag != 1)goto
_LL146;_tmp2A1=(void*)(_tmp2A0.Eq_constr).val;_LL145: {union Cyc_Absyn_Constraint*
_tmp2A5=y;union Cyc_Absyn_Constraint _tmp2A6;int _tmp2A7;union Cyc_Absyn_Constraint
_tmp2A8;void*_tmp2A9;union Cyc_Absyn_Constraint _tmp2AA;union Cyc_Absyn_Constraint*
_tmp2AB;_LL149: _tmp2A6=*_tmp2A5;if((_tmp2A6.No_constr).tag != 3)goto _LL14B;
_tmp2A7=(int)(_tmp2A6.No_constr).val;_LL14A:*y=*x;return;_LL14B: _tmp2A8=*_tmp2A5;
if((_tmp2A8.Eq_constr).tag != 1)goto _LL14D;_tmp2A9=(void*)(_tmp2A8.Eq_constr).val;
_LL14C: if(cmp(_tmp2A1,_tmp2A9)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LL14D: _tmp2AA=*_tmp2A5;if((_tmp2AA.Forward_constr).tag
!= 2)goto _LL148;_tmp2AB=(union Cyc_Absyn_Constraint*)(_tmp2AA.Forward_constr).val;
_LL14E: {const char*_tmp10FA;void*_tmp10F9;(_tmp10F9=0,Cyc_Tcutil_impos(((
_tmp10FA="unify_conref: forward after compress(2)",_tag_dyneither(_tmp10FA,
sizeof(char),40))),_tag_dyneither(_tmp10F9,sizeof(void*),0)));}_LL148:;}_LL146:
_tmp2A2=*_tmp29D;if((_tmp2A2.Forward_constr).tag != 2)goto _LL141;_tmp2A3=(union
Cyc_Absyn_Constraint*)(_tmp2A2.Forward_constr).val;_LL147: {const char*_tmp10FD;
void*_tmp10FC;(_tmp10FC=0,Cyc_Tcutil_impos(((_tmp10FD="unify_conref: forward after compress",
_tag_dyneither(_tmp10FD,sizeof(char),37))),_tag_dyneither(_tmp10FC,sizeof(void*),
0)));}_LL141:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp2B0;_push_handler(& _tmp2B0);{int _tmp2B2=0;if(setjmp(
_tmp2B0.handler))_tmp2B2=1;if(!_tmp2B2){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(
struct _dyneither_ptr)_tag_dyneither(0,0,0));{int _tmp2B3=1;_npop_handler(0);
return _tmp2B3;};;_pop_handler();}else{void*_tmp2B1=(void*)_exn_thrown;void*
_tmp2B5=_tmp2B1;_LL150: {struct Cyc_Tcutil_Unify_struct*_tmp2B6=(struct Cyc_Tcutil_Unify_struct*)
_tmp2B5;if(_tmp2B6->tag != Cyc_Tcutil_Unify)goto _LL152;}_LL151: return 0;_LL152:;
_LL153:(void)_throw(_tmp2B5);_LL14F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,
void*b2);static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmp10FE;
struct _tuple0 _tmp2B8=(_tmp10FE.f1=b1,((_tmp10FE.f2=b2,_tmp10FE)));void*_tmp2B9;
void*_tmp2BB;void*_tmp2BD;void*_tmp2BF;void*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;
void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C6;_LL155: _tmp2B9=_tmp2B8.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2BA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2B9;if(_tmp2BA->tag != 0)goto
_LL157;};_tmp2BB=_tmp2B8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp2BC=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp2BB;if(_tmp2BC->tag != 0)goto _LL157;};_LL156:
return 0;_LL157: _tmp2BD=_tmp2B8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2BE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp2BD;if(_tmp2BE->tag != 0)goto _LL159;};
_LL158: return - 1;_LL159: _tmp2BF=_tmp2B8.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2C0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2BF;if(_tmp2C0->tag != 0)goto
_LL15B;};_LL15A: return 1;_LL15B: _tmp2C1=_tmp2B8.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C2=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C1;if(_tmp2C2->tag != 1)goto _LL154;
else{_tmp2C3=_tmp2C2->f1;}};_tmp2C4=_tmp2B8.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL154;
else{_tmp2C6=_tmp2C5->f1;}};_LL15C: return Cyc_Evexp_const_exp_cmp(_tmp2C3,_tmp2C6);
_LL154:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmp10FF;struct _tuple0 _tmp2C8=(_tmp10FF.f1=b1,((
_tmp10FF.f2=b2,_tmp10FF)));void*_tmp2C9;void*_tmp2CB;void*_tmp2CD;void*_tmp2CF;
void*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D4;struct Cyc_Absyn_Exp*
_tmp2D6;_LL15E: _tmp2C9=_tmp2C8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2CA=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp2C9;if(_tmp2CA->tag != 0)goto _LL160;};
_tmp2CB=_tmp2C8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp2CC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CB;if(_tmp2CC->tag != 0)goto _LL160;};_LL15F: return 0;_LL160: _tmp2CD=_tmp2C8.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2CE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CD;if(_tmp2CE->tag != 0)goto _LL162;};_LL161: return - 1;_LL162: _tmp2CF=_tmp2C8.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2D0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CF;if(_tmp2D0->tag != 0)goto _LL164;};_LL163: return 1;_LL164: _tmp2D1=_tmp2C8.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp2D2=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D1;
if(_tmp2D2->tag != 1)goto _LL15D;else{_tmp2D3=_tmp2D2->f1;}};_tmp2D4=_tmp2C8.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp2D5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D4;
if(_tmp2D5->tag != 1)goto _LL15D;else{_tmp2D6=_tmp2D5->f1;}};_LL165: return Cyc_Evexp_const_exp_cmp(
_tmp2D3,_tmp2D6);_LL15D:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp2D7=att;_LL167: {
struct Cyc_Absyn_Regparm_att_struct*_tmp2D8=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2D7;if(_tmp2D8->tag != 0)goto _LL169;}_LL168: return 0;_LL169: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2D9=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2D7;if(_tmp2D9->tag != 1)goto
_LL16B;}_LL16A: return 1;_LL16B: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2DA=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp2D7;if(_tmp2DA->tag != 2)goto _LL16D;}_LL16C:
return 2;_LL16D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2DB=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2D7;if(_tmp2DB->tag != 3)goto _LL16F;}_LL16E: return 3;_LL16F: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp2DC=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp2D7;if(_tmp2DC->tag != 4)goto
_LL171;}_LL170: return 4;_LL171: {struct Cyc_Absyn_Const_att_struct*_tmp2DD=(struct
Cyc_Absyn_Const_att_struct*)_tmp2D7;if(_tmp2DD->tag != 5)goto _LL173;}_LL172:
return 5;_LL173: {struct Cyc_Absyn_Aligned_att_struct*_tmp2DE=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp2D7;if(_tmp2DE->tag != 6)goto _LL175;}_LL174: return 6;_LL175: {struct Cyc_Absyn_Packed_att_struct*
_tmp2DF=(struct Cyc_Absyn_Packed_att_struct*)_tmp2D7;if(_tmp2DF->tag != 7)goto
_LL177;}_LL176: return 7;_LL177: {struct Cyc_Absyn_Section_att_struct*_tmp2E0=(
struct Cyc_Absyn_Section_att_struct*)_tmp2D7;if(_tmp2E0->tag != 8)goto _LL179;}
_LL178: return 8;_LL179: {struct Cyc_Absyn_Nocommon_att_struct*_tmp2E1=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp2D7;if(_tmp2E1->tag != 9)goto _LL17B;}_LL17A: return 9;_LL17B: {struct Cyc_Absyn_Shared_att_struct*
_tmp2E2=(struct Cyc_Absyn_Shared_att_struct*)_tmp2D7;if(_tmp2E2->tag != 10)goto
_LL17D;}_LL17C: return 10;_LL17D: {struct Cyc_Absyn_Unused_att_struct*_tmp2E3=(
struct Cyc_Absyn_Unused_att_struct*)_tmp2D7;if(_tmp2E3->tag != 11)goto _LL17F;}
_LL17E: return 11;_LL17F: {struct Cyc_Absyn_Weak_att_struct*_tmp2E4=(struct Cyc_Absyn_Weak_att_struct*)
_tmp2D7;if(_tmp2E4->tag != 12)goto _LL181;}_LL180: return 12;_LL181: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp2E5=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp2D7;if(_tmp2E5->tag != 13)goto
_LL183;}_LL182: return 13;_LL183: {struct Cyc_Absyn_Dllexport_att_struct*_tmp2E6=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp2D7;if(_tmp2E6->tag != 14)goto _LL185;}
_LL184: return 14;_LL185: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp2E7=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp2D7;if(_tmp2E7->tag
!= 15)goto _LL187;}_LL186: return 15;_LL187: {struct Cyc_Absyn_Constructor_att_struct*
_tmp2E8=(struct Cyc_Absyn_Constructor_att_struct*)_tmp2D7;if(_tmp2E8->tag != 16)
goto _LL189;}_LL188: return 16;_LL189: {struct Cyc_Absyn_Destructor_att_struct*
_tmp2E9=(struct Cyc_Absyn_Destructor_att_struct*)_tmp2D7;if(_tmp2E9->tag != 17)
goto _LL18B;}_LL18A: return 17;_LL18B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp2EA=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp2D7;if(_tmp2EA->tag
!= 18)goto _LL18D;}_LL18C: return 18;_LL18D: {struct Cyc_Absyn_Format_att_struct*
_tmp2EB=(struct Cyc_Absyn_Format_att_struct*)_tmp2D7;if(_tmp2EB->tag != 19)goto
_LL18F;}_LL18E: return 19;_LL18F: {struct Cyc_Absyn_Initializes_att_struct*_tmp2EC=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp2D7;if(_tmp2EC->tag != 20)goto _LL191;}
_LL190: return 20;_LL191:;_LL192: return 21;_LL166:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1100;struct _tuple0 _tmp2EE=(_tmp1100.f1=att1,((_tmp1100.f2=att2,
_tmp1100)));void*_tmp2EF;int _tmp2F1;void*_tmp2F2;int _tmp2F4;void*_tmp2F5;int
_tmp2F7;void*_tmp2F8;int _tmp2FA;void*_tmp2FB;int _tmp2FD;void*_tmp2FE;int _tmp300;
void*_tmp301;struct _dyneither_ptr _tmp303;void*_tmp304;struct _dyneither_ptr
_tmp306;void*_tmp307;enum Cyc_Absyn_Format_Type _tmp309;int _tmp30A;int _tmp30B;
void*_tmp30C;enum Cyc_Absyn_Format_Type _tmp30E;int _tmp30F;int _tmp310;_LL194:
_tmp2EF=_tmp2EE.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp2F0=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EF;if(_tmp2F0->tag != 0)goto _LL196;else{_tmp2F1=_tmp2F0->f1;}};_tmp2F2=
_tmp2EE.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp2F3=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2F2;if(_tmp2F3->tag != 0)goto _LL196;else{_tmp2F4=_tmp2F3->f1;}};_LL195:
_tmp2F7=_tmp2F1;_tmp2FA=_tmp2F4;goto _LL197;_LL196: _tmp2F5=_tmp2EE.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp2F6=(struct Cyc_Absyn_Initializes_att_struct*)_tmp2F5;if(_tmp2F6->tag != 20)
goto _LL198;else{_tmp2F7=_tmp2F6->f1;}};_tmp2F8=_tmp2EE.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp2F9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp2F8;if(_tmp2F9->tag != 20)
goto _LL198;else{_tmp2FA=_tmp2F9->f1;}};_LL197: _tmp2FD=_tmp2F7;_tmp300=_tmp2FA;
goto _LL199;_LL198: _tmp2FB=_tmp2EE.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp2FC=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp2FB;if(_tmp2FC->tag != 6)goto _LL19A;else{
_tmp2FD=_tmp2FC->f1;}};_tmp2FE=_tmp2EE.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp2FF=(struct Cyc_Absyn_Aligned_att_struct*)_tmp2FE;if(_tmp2FF->tag != 6)goto
_LL19A;else{_tmp300=_tmp2FF->f1;}};_LL199: return Cyc_Core_intcmp(_tmp2FD,_tmp300);
_LL19A: _tmp301=_tmp2EE.f1;{struct Cyc_Absyn_Section_att_struct*_tmp302=(struct Cyc_Absyn_Section_att_struct*)
_tmp301;if(_tmp302->tag != 8)goto _LL19C;else{_tmp303=_tmp302->f1;}};_tmp304=
_tmp2EE.f2;{struct Cyc_Absyn_Section_att_struct*_tmp305=(struct Cyc_Absyn_Section_att_struct*)
_tmp304;if(_tmp305->tag != 8)goto _LL19C;else{_tmp306=_tmp305->f1;}};_LL19B: return
Cyc_strcmp((struct _dyneither_ptr)_tmp303,(struct _dyneither_ptr)_tmp306);_LL19C:
_tmp307=_tmp2EE.f1;{struct Cyc_Absyn_Format_att_struct*_tmp308=(struct Cyc_Absyn_Format_att_struct*)
_tmp307;if(_tmp308->tag != 19)goto _LL19E;else{_tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;
_tmp30B=_tmp308->f3;}};_tmp30C=_tmp2EE.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp30D=(struct Cyc_Absyn_Format_att_struct*)_tmp30C;if(_tmp30D->tag != 19)goto
_LL19E;else{_tmp30E=_tmp30D->f1;_tmp30F=_tmp30D->f2;_tmp310=_tmp30D->f3;}};
_LL19D: {int _tmp311=Cyc_Core_intcmp((int)((unsigned int)_tmp309),(int)((
unsigned int)_tmp30E));if(_tmp311 != 0)return _tmp311;{int _tmp312=Cyc_Core_intcmp(
_tmp30A,_tmp30F);if(_tmp312 != 0)return _tmp312;return Cyc_Core_intcmp(_tmp30B,
_tmp310);};}_LL19E:;_LL19F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL193:;}static int Cyc_Tcutil_equal_att(
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
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv);static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp313=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp314;
enum Cyc_Absyn_KindQual _tmp315;enum Cyc_Absyn_AliasQual _tmp316;struct Cyc_Absyn_Kind
_tmp317;enum Cyc_Absyn_KindQual _tmp318;enum Cyc_Absyn_AliasQual _tmp319;struct Cyc_Absyn_Kind
_tmp31A;enum Cyc_Absyn_KindQual _tmp31B;struct Cyc_Absyn_Kind _tmp31C;enum Cyc_Absyn_KindQual
_tmp31D;_LL1A1: _tmp314=*_tmp313;_tmp315=_tmp314.kind;if(_tmp315 != Cyc_Absyn_RgnKind)
goto _LL1A3;_tmp316=_tmp314.aliasqual;if(_tmp316 != Cyc_Absyn_Unique)goto _LL1A3;
_LL1A2: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1A0;_LL1A3: _tmp317=*_tmp313;
_tmp318=_tmp317.kind;if(_tmp318 != Cyc_Absyn_RgnKind)goto _LL1A5;_tmp319=_tmp317.aliasqual;
if(_tmp319 != Cyc_Absyn_Aliasable)goto _LL1A5;_LL1A4: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1A0;_LL1A5: _tmp31A=*_tmp313;_tmp31B=_tmp31A.kind;if(_tmp31B != Cyc_Absyn_EffKind)
goto _LL1A7;_LL1A6: t=Cyc_Absyn_empty_effect;goto _LL1A0;_LL1A7: _tmp31C=*_tmp313;
_tmp31D=_tmp31C.kind;if(_tmp31D != Cyc_Absyn_IntKind)goto _LL1A9;_LL1A8:{struct Cyc_Absyn_ValueofType_struct
_tmp1103;struct Cyc_Absyn_ValueofType_struct*_tmp1102;t=(void*)((_tmp1102=
_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1103.tag=19,((_tmp1103.f1=Cyc_Absyn_uint_exp(
0,0),_tmp1103)))),_tmp1102))));}goto _LL1A0;_LL1A9:;_LL1AA: t=Cyc_Absyn_sint_typ;
goto _LL1A0;_LL1A0:;}{struct _tuple15*_tmp1104;return(_tmp1104=_cycalloc(sizeof(*
_tmp1104)),((_tmp1104->f1=tv,((_tmp1104->f2=t,_tmp1104)))));};}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp321=Cyc_Tcutil_compress(t);
void*_tmp32C;void*_tmp32E;void*_tmp32F;struct Cyc_Absyn_DatatypeInfo _tmp331;
struct Cyc_List_List*_tmp332;struct Cyc_Absyn_PtrInfo _tmp334;void*_tmp335;struct
Cyc_Absyn_PtrAtts _tmp336;void*_tmp337;struct Cyc_Absyn_ArrayInfo _tmp339;void*
_tmp33A;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_DatatypeFieldInfo _tmp33E;
struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_AggrInfo _tmp341;struct Cyc_List_List*
_tmp342;struct Cyc_List_List*_tmp344;struct Cyc_Absyn_FnInfo _tmp347;struct Cyc_List_List*
_tmp348;struct Cyc_Core_Opt*_tmp349;void*_tmp34A;struct Cyc_List_List*_tmp34B;
struct Cyc_Absyn_VarargInfo*_tmp34C;struct Cyc_List_List*_tmp34D;void*_tmp353;
struct Cyc_List_List*_tmp355;_LL1AC: {struct Cyc_Absyn_VoidType_struct*_tmp322=(
struct Cyc_Absyn_VoidType_struct*)_tmp321;if(_tmp322->tag != 0)goto _LL1AE;}_LL1AD:
goto _LL1AF;_LL1AE: {struct Cyc_Absyn_FloatType_struct*_tmp323=(struct Cyc_Absyn_FloatType_struct*)
_tmp321;if(_tmp323->tag != 7)goto _LL1B0;}_LL1AF: goto _LL1B1;_LL1B0: {struct Cyc_Absyn_DoubleType_struct*
_tmp324=(struct Cyc_Absyn_DoubleType_struct*)_tmp321;if(_tmp324->tag != 8)goto
_LL1B2;}_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_EnumType_struct*_tmp325=(
struct Cyc_Absyn_EnumType_struct*)_tmp321;if(_tmp325->tag != 14)goto _LL1B4;}_LL1B3:
goto _LL1B5;_LL1B4: {struct Cyc_Absyn_AnonEnumType_struct*_tmp326=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp321;if(_tmp326->tag != 15)goto _LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_ValueofType_struct*
_tmp327=(struct Cyc_Absyn_ValueofType_struct*)_tmp321;if(_tmp327->tag != 19)goto
_LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_IntType_struct*_tmp328=(
struct Cyc_Absyn_IntType_struct*)_tmp321;if(_tmp328->tag != 6)goto _LL1BA;}_LL1B9:
return Cyc_Absyn_empty_effect;_LL1BA: {struct Cyc_Absyn_Evar_struct*_tmp329=(
struct Cyc_Absyn_Evar_struct*)_tmp321;if(_tmp329->tag != 1)goto _LL1BC;}_LL1BB: goto
_LL1BD;_LL1BC: {struct Cyc_Absyn_VarType_struct*_tmp32A=(struct Cyc_Absyn_VarType_struct*)
_tmp321;if(_tmp32A->tag != 2)goto _LL1BE;}_LL1BD: {struct Cyc_Absyn_Kind*_tmp356=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp357;enum Cyc_Absyn_KindQual
_tmp358;struct Cyc_Absyn_Kind _tmp359;enum Cyc_Absyn_KindQual _tmp35A;struct Cyc_Absyn_Kind
_tmp35B;enum Cyc_Absyn_KindQual _tmp35C;_LL1E1: _tmp357=*_tmp356;_tmp358=_tmp357.kind;
if(_tmp358 != Cyc_Absyn_RgnKind)goto _LL1E3;_LL1E2: {struct Cyc_Absyn_AccessEff_struct
_tmp1107;struct Cyc_Absyn_AccessEff_struct*_tmp1106;return(void*)((_tmp1106=
_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1107.tag=23,((_tmp1107.f1=(void*)
t,_tmp1107)))),_tmp1106))));}_LL1E3: _tmp359=*_tmp356;_tmp35A=_tmp359.kind;if(
_tmp35A != Cyc_Absyn_EffKind)goto _LL1E5;_LL1E4: return t;_LL1E5: _tmp35B=*_tmp356;
_tmp35C=_tmp35B.kind;if(_tmp35C != Cyc_Absyn_IntKind)goto _LL1E7;_LL1E6: return Cyc_Absyn_empty_effect;
_LL1E7:;_LL1E8: {struct Cyc_Absyn_RgnsEff_struct _tmp110A;struct Cyc_Absyn_RgnsEff_struct*
_tmp1109;return(void*)((_tmp1109=_cycalloc(sizeof(*_tmp1109)),((_tmp1109[0]=((
_tmp110A.tag=25,((_tmp110A.f1=(void*)t,_tmp110A)))),_tmp1109))));}_LL1E0:;}
_LL1BE: {struct Cyc_Absyn_RgnHandleType_struct*_tmp32B=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp321;if(_tmp32B->tag != 16)goto _LL1C0;else{_tmp32C=(void*)_tmp32B->f1;}}_LL1BF: {
struct Cyc_Absyn_AccessEff_struct _tmp110D;struct Cyc_Absyn_AccessEff_struct*
_tmp110C;return(void*)((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((
_tmp110D.tag=23,((_tmp110D.f1=(void*)_tmp32C,_tmp110D)))),_tmp110C))));}_LL1C0: {
struct Cyc_Absyn_DynRgnType_struct*_tmp32D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp321;if(_tmp32D->tag != 17)goto _LL1C2;else{_tmp32E=(void*)_tmp32D->f1;_tmp32F=(
void*)_tmp32D->f2;}}_LL1C1: {struct Cyc_Absyn_AccessEff_struct _tmp1110;struct Cyc_Absyn_AccessEff_struct*
_tmp110F;return(void*)((_tmp110F=_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((
_tmp1110.tag=23,((_tmp1110.f1=(void*)_tmp32F,_tmp1110)))),_tmp110F))));}_LL1C2: {
struct Cyc_Absyn_DatatypeType_struct*_tmp330=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp321;if(_tmp330->tag != 3)goto _LL1C4;else{_tmp331=_tmp330->f1;_tmp332=_tmp331.targs;}}
_LL1C3: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp332);struct Cyc_Absyn_JoinEff_struct
_tmp1113;struct Cyc_Absyn_JoinEff_struct*_tmp1112;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=24,((
_tmp1113.f1=ts,_tmp1113)))),_tmp1112)))));}_LL1C4: {struct Cyc_Absyn_PointerType_struct*
_tmp333=(struct Cyc_Absyn_PointerType_struct*)_tmp321;if(_tmp333->tag != 5)goto
_LL1C6;else{_tmp334=_tmp333->f1;_tmp335=_tmp334.elt_typ;_tmp336=_tmp334.ptr_atts;
_tmp337=_tmp336.rgn;}}_LL1C5: {struct Cyc_Absyn_JoinEff_struct _tmp1122;struct Cyc_Absyn_AccessEff_struct*
_tmp1121;struct Cyc_Absyn_AccessEff_struct _tmp1120;void*_tmp111F[2];struct Cyc_Absyn_JoinEff_struct*
_tmp111E;return Cyc_Tcutil_normalize_effect((void*)((_tmp111E=_cycalloc(sizeof(*
_tmp111E)),((_tmp111E[0]=((_tmp1122.tag=24,((_tmp1122.f1=((_tmp111F[1]=Cyc_Tcutil_rgns_of(
_tmp335),((_tmp111F[0]=(void*)((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[
0]=((_tmp1120.tag=23,((_tmp1120.f1=(void*)_tmp337,_tmp1120)))),_tmp1121)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp111F,sizeof(void*),2)))))),_tmp1122)))),_tmp111E)))));}_LL1C6: {struct Cyc_Absyn_ArrayType_struct*
_tmp338=(struct Cyc_Absyn_ArrayType_struct*)_tmp321;if(_tmp338->tag != 9)goto
_LL1C8;else{_tmp339=_tmp338->f1;_tmp33A=_tmp339.elt_type;}}_LL1C7: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp33A));_LL1C8: {struct Cyc_Absyn_TupleType_struct*_tmp33B=(
struct Cyc_Absyn_TupleType_struct*)_tmp321;if(_tmp33B->tag != 11)goto _LL1CA;else{
_tmp33C=_tmp33B->f1;}}_LL1C9: {struct Cyc_List_List*_tmp36C=0;for(0;_tmp33C != 0;
_tmp33C=_tmp33C->tl){struct Cyc_List_List*_tmp1123;_tmp36C=((_tmp1123=_cycalloc(
sizeof(*_tmp1123)),((_tmp1123->hd=(*((struct _tuple11*)_tmp33C->hd)).f2,((
_tmp1123->tl=_tmp36C,_tmp1123))))));}_tmp33F=_tmp36C;goto _LL1CB;}_LL1CA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp33D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp321;if(_tmp33D->tag != 4)goto _LL1CC;else{_tmp33E=_tmp33D->f1;_tmp33F=_tmp33E.targs;}}
_LL1CB: _tmp342=_tmp33F;goto _LL1CD;_LL1CC: {struct Cyc_Absyn_AggrType_struct*
_tmp340=(struct Cyc_Absyn_AggrType_struct*)_tmp321;if(_tmp340->tag != 12)goto
_LL1CE;else{_tmp341=_tmp340->f1;_tmp342=_tmp341.targs;}}_LL1CD: {struct Cyc_Absyn_JoinEff_struct
_tmp1126;struct Cyc_Absyn_JoinEff_struct*_tmp1125;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1125=_cycalloc(sizeof(*_tmp1125)),((_tmp1125[0]=((_tmp1126.tag=24,((
_tmp1126.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp342),_tmp1126)))),_tmp1125)))));}_LL1CE: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp343=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp321;if(_tmp343->tag != 13)goto _LL1D0;else{_tmp344=_tmp343->f2;}}_LL1CF: {
struct Cyc_Absyn_JoinEff_struct _tmp1129;struct Cyc_Absyn_JoinEff_struct*_tmp1128;
return Cyc_Tcutil_normalize_effect((void*)((_tmp1128=_cycalloc(sizeof(*_tmp1128)),((
_tmp1128[0]=((_tmp1129.tag=24,((_tmp1129.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp344),_tmp1129)))),_tmp1128)))));}_LL1D0: {struct Cyc_Absyn_TagType_struct*
_tmp345=(struct Cyc_Absyn_TagType_struct*)_tmp321;if(_tmp345->tag != 20)goto _LL1D2;}
_LL1D1: return Cyc_Absyn_empty_effect;_LL1D2: {struct Cyc_Absyn_FnType_struct*
_tmp346=(struct Cyc_Absyn_FnType_struct*)_tmp321;if(_tmp346->tag != 10)goto _LL1D4;
else{_tmp347=_tmp346->f1;_tmp348=_tmp347.tvars;_tmp349=_tmp347.effect;_tmp34A=
_tmp347.ret_typ;_tmp34B=_tmp347.args;_tmp34C=_tmp347.cyc_varargs;_tmp34D=_tmp347.rgn_po;}}
_LL1D3: {void*_tmp372=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct
_tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp348),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp349))->v);return Cyc_Tcutil_normalize_effect(
_tmp372);}_LL1D4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp34E=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp321;if(_tmp34E->tag != 22)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp34F=(struct Cyc_Absyn_HeapRgn_struct*)_tmp321;if(_tmp34F->tag != 21)goto _LL1D8;}
_LL1D7: return Cyc_Absyn_empty_effect;_LL1D8: {struct Cyc_Absyn_AccessEff_struct*
_tmp350=(struct Cyc_Absyn_AccessEff_struct*)_tmp321;if(_tmp350->tag != 23)goto
_LL1DA;}_LL1D9: goto _LL1DB;_LL1DA: {struct Cyc_Absyn_JoinEff_struct*_tmp351=(
struct Cyc_Absyn_JoinEff_struct*)_tmp321;if(_tmp351->tag != 24)goto _LL1DC;}_LL1DB:
return t;_LL1DC: {struct Cyc_Absyn_RgnsEff_struct*_tmp352=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp321;if(_tmp352->tag != 25)goto _LL1DE;else{_tmp353=(void*)_tmp352->f1;}}_LL1DD:
return Cyc_Tcutil_rgns_of(_tmp353);_LL1DE: {struct Cyc_Absyn_TypedefType_struct*
_tmp354=(struct Cyc_Absyn_TypedefType_struct*)_tmp321;if(_tmp354->tag != 18)goto
_LL1AB;else{_tmp355=_tmp354->f2;}}_LL1DF: {struct Cyc_Absyn_JoinEff_struct
_tmp112C;struct Cyc_Absyn_JoinEff_struct*_tmp112B;return Cyc_Tcutil_normalize_effect((
void*)((_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B[0]=((_tmp112C.tag=24,((
_tmp112C.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp355),_tmp112C)))),_tmp112B)))));}_LL1AB:;}
void*Cyc_Tcutil_normalize_effect(void*e);void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{void*_tmp375=e;struct Cyc_List_List*_tmp377;struct Cyc_List_List**
_tmp378;void*_tmp37A;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*_tmp376=(struct Cyc_Absyn_JoinEff_struct*)
_tmp375;if(_tmp376->tag != 24)goto _LL1EC;else{_tmp377=_tmp376->f1;_tmp378=(struct
Cyc_List_List**)& _tmp376->f1;}}_LL1EB: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp378;for(0;effs != 0;effs=effs->tl){void*_tmp37B=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp37B));{void*_tmp37C=(
void*)effs->hd;void*_tmp37F;_LL1F1: {struct Cyc_Absyn_JoinEff_struct*_tmp37D=(
struct Cyc_Absyn_JoinEff_struct*)_tmp37C;if(_tmp37D->tag != 24)goto _LL1F3;}_LL1F2:
goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AccessEff_struct*_tmp37E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp37C;if(_tmp37E->tag != 23)goto _LL1F5;else{_tmp37F=(void*)_tmp37E->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp380=(struct Cyc_Absyn_HeapRgn_struct*)_tmp37F;if(
_tmp380->tag != 21)goto _LL1F5;};}}_LL1F4: redo_join=1;goto _LL1F0;_LL1F5:;_LL1F6:
goto _LL1F0;_LL1F0:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp378;for(0;effs != 0;effs=effs->tl){void*_tmp381=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp383;void*_tmp385;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*
_tmp382=(struct Cyc_Absyn_JoinEff_struct*)_tmp381;if(_tmp382->tag != 24)goto _LL1FA;
else{_tmp383=_tmp382->f1;}}_LL1F9: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp383,effects);goto _LL1F7;_LL1FA: {
struct Cyc_Absyn_AccessEff_struct*_tmp384=(struct Cyc_Absyn_AccessEff_struct*)
_tmp381;if(_tmp384->tag != 23)goto _LL1FC;else{_tmp385=(void*)_tmp384->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp386=(struct Cyc_Absyn_HeapRgn_struct*)_tmp385;if(
_tmp386->tag != 21)goto _LL1FC;};}}_LL1FB: goto _LL1F7;_LL1FC:;_LL1FD:{struct Cyc_List_List*
_tmp112D;effects=((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((_tmp112D->hd=_tmp381,((
_tmp112D->tl=effects,_tmp112D))))));}goto _LL1F7;_LL1F7:;}}*_tmp378=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp379=(struct Cyc_Absyn_RgnsEff_struct*)_tmp375;if(_tmp379->tag != 25)goto _LL1EE;
else{_tmp37A=(void*)_tmp379->f1;}}_LL1ED: {void*_tmp388=Cyc_Tcutil_compress(
_tmp37A);_LL1FF: {struct Cyc_Absyn_Evar_struct*_tmp389=(struct Cyc_Absyn_Evar_struct*)
_tmp388;if(_tmp389->tag != 1)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_VarType_struct*
_tmp38A=(struct Cyc_Absyn_VarType_struct*)_tmp388;if(_tmp38A->tag != 2)goto _LL203;}
_LL202: return e;_LL203:;_LL204: return Cyc_Tcutil_rgns_of(_tmp37A);_LL1FE:;}_LL1EE:;
_LL1EF: return e;_LL1E9:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmp1137;
struct Cyc_Core_Opt*_tmp1136;struct Cyc_Absyn_FnInfo _tmp1135;struct Cyc_Absyn_FnType_struct*
_tmp1134;struct Cyc_Absyn_FnType_struct*_tmp38B=(_tmp1134=_cycalloc(sizeof(*
_tmp1134)),((_tmp1134[0]=((_tmp1137.tag=10,((_tmp1137.f1=((_tmp1135.tvars=0,((
_tmp1135.effect=((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136->v=eff,
_tmp1136)))),((_tmp1135.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1135.args=0,((
_tmp1135.c_varargs=0,((_tmp1135.cyc_varargs=0,((_tmp1135.rgn_po=0,((_tmp1135.attributes=
0,_tmp1135)))))))))))))))),_tmp1137)))),_tmp1134)));return Cyc_Absyn_atb_typ((
void*)_tmp38B,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val)
return 1;{void*_tmp390=Cyc_Tcutil_compress(e);void*_tmp392;struct Cyc_List_List*
_tmp394;void*_tmp396;struct Cyc_Core_Opt*_tmp398;struct Cyc_Core_Opt*_tmp399;
struct Cyc_Core_Opt**_tmp39A;struct Cyc_Core_Opt*_tmp39B;_LL206: {struct Cyc_Absyn_AccessEff_struct*
_tmp391=(struct Cyc_Absyn_AccessEff_struct*)_tmp390;if(_tmp391->tag != 23)goto
_LL208;else{_tmp392=(void*)_tmp391->f1;}}_LL207: if(constrain)return Cyc_Tcutil_unify(
r,_tmp392);_tmp392=Cyc_Tcutil_compress(_tmp392);if(r == _tmp392)return 1;{struct
_tuple0 _tmp1138;struct _tuple0 _tmp39D=(_tmp1138.f1=r,((_tmp1138.f2=_tmp392,
_tmp1138)));void*_tmp39E;struct Cyc_Absyn_Tvar*_tmp3A0;void*_tmp3A1;struct Cyc_Absyn_Tvar*
_tmp3A3;_LL211: _tmp39E=_tmp39D.f1;{struct Cyc_Absyn_VarType_struct*_tmp39F=(
struct Cyc_Absyn_VarType_struct*)_tmp39E;if(_tmp39F->tag != 2)goto _LL213;else{
_tmp3A0=_tmp39F->f1;}};_tmp3A1=_tmp39D.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3A2=(struct Cyc_Absyn_VarType_struct*)_tmp3A1;if(_tmp3A2->tag != 2)goto _LL213;
else{_tmp3A3=_tmp3A2->f1;}};_LL212: return Cyc_Absyn_tvar_cmp(_tmp3A0,_tmp3A3)== 0;
_LL213:;_LL214: return 0;_LL210:;};_LL208: {struct Cyc_Absyn_JoinEff_struct*_tmp393=(
struct Cyc_Absyn_JoinEff_struct*)_tmp390;if(_tmp393->tag != 24)goto _LL20A;else{
_tmp394=_tmp393->f1;}}_LL209: for(0;_tmp394 != 0;_tmp394=_tmp394->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp394->hd))return 1;}return 0;_LL20A: {struct Cyc_Absyn_RgnsEff_struct*
_tmp395=(struct Cyc_Absyn_RgnsEff_struct*)_tmp390;if(_tmp395->tag != 25)goto _LL20C;
else{_tmp396=(void*)_tmp395->f1;}}_LL20B: {void*_tmp3A4=Cyc_Tcutil_rgns_of(
_tmp396);void*_tmp3A6;_LL216: {struct Cyc_Absyn_RgnsEff_struct*_tmp3A5=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3A4;if(_tmp3A5->tag != 25)goto _LL218;else{_tmp3A6=(void*)_tmp3A5->f1;}}_LL217:
if(!constrain)return 0;{void*_tmp3A7=Cyc_Tcutil_compress(_tmp3A6);struct Cyc_Core_Opt*
_tmp3A9;struct Cyc_Core_Opt*_tmp3AA;struct Cyc_Core_Opt**_tmp3AB;struct Cyc_Core_Opt*
_tmp3AC;_LL21B: {struct Cyc_Absyn_Evar_struct*_tmp3A8=(struct Cyc_Absyn_Evar_struct*)
_tmp3A7;if(_tmp3A8->tag != 1)goto _LL21D;else{_tmp3A9=_tmp3A8->f1;_tmp3AA=_tmp3A8->f2;
_tmp3AB=(struct Cyc_Core_Opt**)& _tmp3A8->f2;_tmp3AC=_tmp3A8->f4;}}_LL21C: {void*
_tmp3AD=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3AC);Cyc_Tcutil_occurs(
_tmp3AD,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AC))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1147;struct Cyc_Absyn_AccessEff_struct*
_tmp1146;struct Cyc_Absyn_AccessEff_struct _tmp1145;void*_tmp1144[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1143;void*_tmp3AE=Cyc_Tcutil_dummy_fntype((void*)((_tmp1143=_cycalloc(
sizeof(*_tmp1143)),((_tmp1143[0]=((_tmp1147.tag=24,((_tmp1147.f1=((_tmp1144[1]=(
void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1145.tag=23,((
_tmp1145.f1=(void*)r,_tmp1145)))),_tmp1146)))),((_tmp1144[0]=_tmp3AD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1144,sizeof(void*),2)))))),
_tmp1147)))),_tmp1143)))));{struct Cyc_Core_Opt*_tmp1148;*_tmp3AB=((_tmp1148=
_cycalloc(sizeof(*_tmp1148)),((_tmp1148->v=_tmp3AE,_tmp1148))));}return 1;};}
_LL21D:;_LL21E: return 0;_LL21A:;};_LL218:;_LL219: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3A4);_LL215:;}_LL20C: {struct Cyc_Absyn_Evar_struct*_tmp397=(
struct Cyc_Absyn_Evar_struct*)_tmp390;if(_tmp397->tag != 1)goto _LL20E;else{_tmp398=
_tmp397->f1;_tmp399=_tmp397->f2;_tmp39A=(struct Cyc_Core_Opt**)& _tmp397->f2;
_tmp39B=_tmp397->f4;}}_LL20D: if(_tmp398 == 0  || ((struct Cyc_Absyn_Kind*)_tmp398->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp114B;void*_tmp114A;(_tmp114A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp114B="effect evar has wrong kind",
_tag_dyneither(_tmp114B,sizeof(char),27))),_tag_dyneither(_tmp114A,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3B7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp39B);Cyc_Tcutil_occurs(_tmp3B7,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39B))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1160;struct Cyc_List_List*_tmp115F;struct Cyc_Absyn_AccessEff_struct _tmp115E;
struct Cyc_Absyn_AccessEff_struct*_tmp115D;struct Cyc_List_List*_tmp115C;struct Cyc_Absyn_JoinEff_struct*
_tmp115B;struct Cyc_Absyn_JoinEff_struct*_tmp3B8=(_tmp115B=_cycalloc(sizeof(*
_tmp115B)),((_tmp115B[0]=((_tmp1160.tag=24,((_tmp1160.f1=((_tmp115C=_cycalloc(
sizeof(*_tmp115C)),((_tmp115C->hd=_tmp3B7,((_tmp115C->tl=((_tmp115F=_cycalloc(
sizeof(*_tmp115F)),((_tmp115F->hd=(void*)((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((
_tmp115D[0]=((_tmp115E.tag=23,((_tmp115E.f1=(void*)r,_tmp115E)))),_tmp115D)))),((
_tmp115F->tl=0,_tmp115F)))))),_tmp115C)))))),_tmp1160)))),_tmp115B)));{struct Cyc_Core_Opt*
_tmp1161;*_tmp39A=((_tmp1161=_cycalloc(sizeof(*_tmp1161)),((_tmp1161->v=(void*)
_tmp3B8,_tmp1161))));}return 1;};};_LL20E:;_LL20F: return 0;_LL205:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3C0=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp3C3;void*_tmp3C5;struct Cyc_Core_Opt*
_tmp3C7;struct Cyc_Core_Opt*_tmp3C8;struct Cyc_Core_Opt**_tmp3C9;struct Cyc_Core_Opt*
_tmp3CA;_LL220: {struct Cyc_Absyn_AccessEff_struct*_tmp3C1=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C0;if(_tmp3C1->tag != 23)goto _LL222;}_LL221: return 0;_LL222: {struct Cyc_Absyn_JoinEff_struct*
_tmp3C2=(struct Cyc_Absyn_JoinEff_struct*)_tmp3C0;if(_tmp3C2->tag != 24)goto _LL224;
else{_tmp3C3=_tmp3C2->f1;}}_LL223: for(0;_tmp3C3 != 0;_tmp3C3=_tmp3C3->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp3C3->hd))return 1;}return 0;_LL224: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3C4=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3C0;if(_tmp3C4->tag != 25)goto _LL226;
else{_tmp3C5=(void*)_tmp3C4->f1;}}_LL225: _tmp3C5=Cyc_Tcutil_compress(_tmp3C5);
if(t == _tmp3C5)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3C5);{
void*_tmp3CB=Cyc_Tcutil_rgns_of(t);void*_tmp3CD;_LL22B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3CC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3CB;if(_tmp3CC->tag != 25)goto _LL22D;
else{_tmp3CD=(void*)_tmp3CC->f1;}}_LL22C: {struct _tuple0 _tmp1162;struct _tuple0
_tmp3CF=(_tmp1162.f1=Cyc_Tcutil_compress(_tmp3CD),((_tmp1162.f2=_tmp3C5,_tmp1162)));
void*_tmp3D0;struct Cyc_Absyn_Tvar*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Tvar*
_tmp3D5;_LL230: _tmp3D0=_tmp3CF.f1;{struct Cyc_Absyn_VarType_struct*_tmp3D1=(
struct Cyc_Absyn_VarType_struct*)_tmp3D0;if(_tmp3D1->tag != 2)goto _LL232;else{
_tmp3D2=_tmp3D1->f1;}};_tmp3D3=_tmp3CF.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3D4=(struct Cyc_Absyn_VarType_struct*)_tmp3D3;if(_tmp3D4->tag != 2)goto _LL232;
else{_tmp3D5=_tmp3D4->f1;}};_LL231: return Cyc_Tcutil_unify(t,_tmp3C5);_LL232:;
_LL233: return _tmp3CD == _tmp3C5;_LL22F:;}_LL22D:;_LL22E: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp3CB);_LL22A:;};_LL226: {struct Cyc_Absyn_Evar_struct*
_tmp3C6=(struct Cyc_Absyn_Evar_struct*)_tmp3C0;if(_tmp3C6->tag != 1)goto _LL228;
else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_tmp3C9=(struct Cyc_Core_Opt**)&
_tmp3C6->f2;_tmp3CA=_tmp3C6->f4;}}_LL227: if(_tmp3C7 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3C7->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1165;void*_tmp1164;(
_tmp1164=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1165="effect evar has wrong kind",_tag_dyneither(_tmp1165,sizeof(char),27))),
_tag_dyneither(_tmp1164,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp3D8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3CA);
Cyc_Tcutil_occurs(_tmp3D8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3CA))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp117A;struct Cyc_List_List*
_tmp1179;struct Cyc_Absyn_RgnsEff_struct _tmp1178;struct Cyc_Absyn_RgnsEff_struct*
_tmp1177;struct Cyc_List_List*_tmp1176;struct Cyc_Absyn_JoinEff_struct*_tmp1175;
struct Cyc_Absyn_JoinEff_struct*_tmp3D9=(_tmp1175=_cycalloc(sizeof(*_tmp1175)),((
_tmp1175[0]=((_tmp117A.tag=24,((_tmp117A.f1=((_tmp1176=_cycalloc(sizeof(*
_tmp1176)),((_tmp1176->hd=_tmp3D8,((_tmp1176->tl=((_tmp1179=_cycalloc(sizeof(*
_tmp1179)),((_tmp1179->hd=(void*)((_tmp1177=_cycalloc(sizeof(*_tmp1177)),((
_tmp1177[0]=((_tmp1178.tag=25,((_tmp1178.f1=(void*)t,_tmp1178)))),_tmp1177)))),((
_tmp1179->tl=0,_tmp1179)))))),_tmp1176)))))),_tmp117A)))),_tmp1175)));{struct Cyc_Core_Opt*
_tmp117B;*_tmp3C9=((_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B->v=(void*)
_tmp3D9,_tmp117B))));}return 1;};};_LL228:;_LL229: return 0;_LL21F:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp3E1=e;struct Cyc_Absyn_Tvar*_tmp3E3;struct Cyc_List_List*_tmp3E5;void*
_tmp3E7;struct Cyc_Core_Opt*_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;struct Cyc_Core_Opt**
_tmp3EB;struct Cyc_Core_Opt*_tmp3EC;_LL235: {struct Cyc_Absyn_VarType_struct*
_tmp3E2=(struct Cyc_Absyn_VarType_struct*)_tmp3E1;if(_tmp3E2->tag != 2)goto _LL237;
else{_tmp3E3=_tmp3E2->f1;}}_LL236: return Cyc_Absyn_tvar_cmp(v,_tmp3E3)== 0;_LL237: {
struct Cyc_Absyn_JoinEff_struct*_tmp3E4=(struct Cyc_Absyn_JoinEff_struct*)_tmp3E1;
if(_tmp3E4->tag != 24)goto _LL239;else{_tmp3E5=_tmp3E4->f1;}}_LL238: for(0;_tmp3E5
!= 0;_tmp3E5=_tmp3E5->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp3E5->hd))return 1;}return 0;_LL239: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3E6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3E1;if(_tmp3E6->tag != 25)goto _LL23B;
else{_tmp3E7=(void*)_tmp3E6->f1;}}_LL23A: {void*_tmp3ED=Cyc_Tcutil_rgns_of(
_tmp3E7);void*_tmp3EF;_LL240: {struct Cyc_Absyn_RgnsEff_struct*_tmp3EE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3ED;if(_tmp3EE->tag != 25)goto _LL242;else{_tmp3EF=(void*)_tmp3EE->f1;}}_LL241:
if(!may_constrain_evars)return 0;{void*_tmp3F0=Cyc_Tcutil_compress(_tmp3EF);
struct Cyc_Core_Opt*_tmp3F2;struct Cyc_Core_Opt*_tmp3F3;struct Cyc_Core_Opt**
_tmp3F4;struct Cyc_Core_Opt*_tmp3F5;_LL245: {struct Cyc_Absyn_Evar_struct*_tmp3F1=(
struct Cyc_Absyn_Evar_struct*)_tmp3F0;if(_tmp3F1->tag != 1)goto _LL247;else{_tmp3F2=
_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;_tmp3F4=(struct Cyc_Core_Opt**)& _tmp3F1->f2;
_tmp3F5=_tmp3F1->f4;}}_LL246: {void*_tmp3F6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3F5);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp118A;struct Cyc_Absyn_VarType_struct*_tmp1189;
struct Cyc_Absyn_VarType_struct _tmp1188;void*_tmp1187[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1186;void*_tmp3F7=Cyc_Tcutil_dummy_fntype((void*)((_tmp1186=_cycalloc(
sizeof(*_tmp1186)),((_tmp1186[0]=((_tmp118A.tag=24,((_tmp118A.f1=((_tmp1187[1]=(
void*)((_tmp1189=_cycalloc(sizeof(*_tmp1189)),((_tmp1189[0]=((_tmp1188.tag=2,((
_tmp1188.f1=v,_tmp1188)))),_tmp1189)))),((_tmp1187[0]=_tmp3F6,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1187,sizeof(void*),2)))))),
_tmp118A)))),_tmp1186)))));{struct Cyc_Core_Opt*_tmp118B;*_tmp3F4=((_tmp118B=
_cycalloc(sizeof(*_tmp118B)),((_tmp118B->v=_tmp3F7,_tmp118B))));}return 1;};}
_LL247:;_LL248: return 0;_LL244:;};_LL242:;_LL243: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp3ED);_LL23F:;}_LL23B: {struct Cyc_Absyn_Evar_struct*
_tmp3E8=(struct Cyc_Absyn_Evar_struct*)_tmp3E1;if(_tmp3E8->tag != 1)goto _LL23D;
else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;_tmp3EB=(struct Cyc_Core_Opt**)&
_tmp3E8->f2;_tmp3EC=_tmp3E8->f4;}}_LL23C: if(_tmp3E9 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3E9->v)->kind != Cyc_Absyn_EffKind){const char*_tmp118E;void*_tmp118D;(
_tmp118D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp118E="effect evar has wrong kind",_tag_dyneither(_tmp118E,sizeof(char),27))),
_tag_dyneither(_tmp118D,sizeof(void*),0)));}{void*_tmp400=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3EC);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EC))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp11A3;
struct Cyc_List_List*_tmp11A2;struct Cyc_Absyn_VarType_struct _tmp11A1;struct Cyc_Absyn_VarType_struct*
_tmp11A0;struct Cyc_List_List*_tmp119F;struct Cyc_Absyn_JoinEff_struct*_tmp119E;
struct Cyc_Absyn_JoinEff_struct*_tmp401=(_tmp119E=_cycalloc(sizeof(*_tmp119E)),((
_tmp119E[0]=((_tmp11A3.tag=24,((_tmp11A3.f1=((_tmp119F=_cycalloc(sizeof(*
_tmp119F)),((_tmp119F->hd=_tmp400,((_tmp119F->tl=((_tmp11A2=_cycalloc(sizeof(*
_tmp11A2)),((_tmp11A2->hd=(void*)((_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((
_tmp11A0[0]=((_tmp11A1.tag=2,((_tmp11A1.f1=v,_tmp11A1)))),_tmp11A0)))),((
_tmp11A2->tl=0,_tmp11A2)))))),_tmp119F)))))),_tmp11A3)))),_tmp119E)));{struct Cyc_Core_Opt*
_tmp11A4;*_tmp3EB=((_tmp11A4=_cycalloc(sizeof(*_tmp11A4)),((_tmp11A4->v=(void*)
_tmp401,_tmp11A4))));}return 1;};};_LL23D:;_LL23E: return 0;_LL234:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp409=e;struct Cyc_List_List*_tmp40B;void*_tmp40D;_LL24A: {struct Cyc_Absyn_JoinEff_struct*
_tmp40A=(struct Cyc_Absyn_JoinEff_struct*)_tmp409;if(_tmp40A->tag != 24)goto _LL24C;
else{_tmp40B=_tmp40A->f1;}}_LL24B: for(0;_tmp40B != 0;_tmp40B=_tmp40B->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp40B->hd))return 1;}return 0;_LL24C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp40C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp409;if(_tmp40C->tag != 25)goto _LL24E;
else{_tmp40D=(void*)_tmp40C->f1;}}_LL24D: {void*_tmp40F=Cyc_Tcutil_rgns_of(
_tmp40D);void*_tmp411;_LL253: {struct Cyc_Absyn_RgnsEff_struct*_tmp410=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp40F;if(_tmp410->tag != 25)goto _LL255;else{_tmp411=(void*)_tmp410->f1;}}_LL254:
return 0;_LL255:;_LL256: return Cyc_Tcutil_evar_in_effect(evar,_tmp40F);_LL252:;}
_LL24E: {struct Cyc_Absyn_Evar_struct*_tmp40E=(struct Cyc_Absyn_Evar_struct*)
_tmp409;if(_tmp40E->tag != 1)goto _LL250;}_LL24F: return evar == e;_LL250:;_LL251:
return 0;_LL249:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp412=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp414;void*_tmp416;
struct Cyc_Absyn_Tvar*_tmp418;void*_tmp41A;struct Cyc_Core_Opt*_tmp41C;struct Cyc_Core_Opt**
_tmp41D;struct Cyc_Core_Opt*_tmp41E;_LL258: {struct Cyc_Absyn_JoinEff_struct*
_tmp413=(struct Cyc_Absyn_JoinEff_struct*)_tmp412;if(_tmp413->tag != 24)goto _LL25A;
else{_tmp414=_tmp413->f1;}}_LL259: for(0;_tmp414 != 0;_tmp414=_tmp414->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp414->hd,e2))return 0;}return 1;_LL25A: {struct Cyc_Absyn_AccessEff_struct*
_tmp415=(struct Cyc_Absyn_AccessEff_struct*)_tmp412;if(_tmp415->tag != 23)goto
_LL25C;else{_tmp416=(void*)_tmp415->f1;}}_LL25B: return Cyc_Tcutil_region_in_effect(
0,_tmp416,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp416,(void*)& Cyc_Absyn_HeapRgn_val);
_LL25C: {struct Cyc_Absyn_VarType_struct*_tmp417=(struct Cyc_Absyn_VarType_struct*)
_tmp412;if(_tmp417->tag != 2)goto _LL25E;else{_tmp418=_tmp417->f1;}}_LL25D: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp418,e2);_LL25E: {struct Cyc_Absyn_RgnsEff_struct*
_tmp419=(struct Cyc_Absyn_RgnsEff_struct*)_tmp412;if(_tmp419->tag != 25)goto _LL260;
else{_tmp41A=(void*)_tmp419->f1;}}_LL25F: {void*_tmp41F=Cyc_Tcutil_rgns_of(
_tmp41A);void*_tmp421;_LL265: {struct Cyc_Absyn_RgnsEff_struct*_tmp420=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp41F;if(_tmp420->tag != 25)goto _LL267;else{_tmp421=(void*)_tmp420->f1;}}_LL266:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp421,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp421,Cyc_Absyn_sint_typ);_LL267:;
_LL268: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp41F,e2);_LL264:;}
_LL260: {struct Cyc_Absyn_Evar_struct*_tmp41B=(struct Cyc_Absyn_Evar_struct*)
_tmp412;if(_tmp41B->tag != 1)goto _LL262;else{_tmp41C=_tmp41B->f2;_tmp41D=(struct
Cyc_Core_Opt**)& _tmp41B->f2;_tmp41E=_tmp41B->f4;}}_LL261: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp11A5;*_tmp41D=((_tmp11A5=_cycalloc(sizeof(*
_tmp11A5)),((_tmp11A5->v=Cyc_Absyn_empty_effect,_tmp11A5))));}return 1;_LL262:;
_LL263: {const char*_tmp11A9;void*_tmp11A8[1];struct Cyc_String_pa_struct _tmp11A7;(
_tmp11A7.tag=0,((_tmp11A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp11A8[0]=& _tmp11A7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A9="subset_effect: bad effect: %s",
_tag_dyneither(_tmp11A9,sizeof(char),30))),_tag_dyneither(_tmp11A8,sizeof(void*),
1)))))));}_LL257:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int
Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=
Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(
0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(
1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp427;void*_tmp428;void*_tmp429;struct _tuple0*_tmp426=(struct
_tuple0*)r1->hd;_tmp427=*_tmp426;_tmp428=_tmp427.f1;_tmp429=_tmp427.f2;{int found=
_tmp428 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp42B;void*_tmp42C;void*_tmp42D;struct
_tuple0*_tmp42A=(struct _tuple0*)r2->hd;_tmp42B=*_tmp42A;_tmp42C=_tmp42B.f1;
_tmp42D=_tmp42B.f2;if(Cyc_Tcutil_unify(_tmp428,_tmp42C) && Cyc_Tcutil_unify(
_tmp429,_tmp42D)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp42E=t1;struct Cyc_Core_Opt*_tmp430;struct Cyc_Core_Opt*
_tmp431;struct Cyc_Core_Opt**_tmp432;struct Cyc_Core_Opt*_tmp433;_LL26A: {struct
Cyc_Absyn_Evar_struct*_tmp42F=(struct Cyc_Absyn_Evar_struct*)_tmp42E;if(_tmp42F->tag
!= 1)goto _LL26C;else{_tmp430=_tmp42F->f1;_tmp431=_tmp42F->f2;_tmp432=(struct Cyc_Core_Opt**)&
_tmp42F->f2;_tmp433=_tmp42F->f4;}}_LL26B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp433))->v,t2);{struct
Cyc_Absyn_Kind*_tmp434=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp434,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp430))->v)){{struct
Cyc_Core_Opt*_tmp11AA;*_tmp432=((_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((
_tmp11AA->v=t2,_tmp11AA))));}return;}else{{void*_tmp436=t2;struct Cyc_Core_Opt*
_tmp438;struct Cyc_Core_Opt**_tmp439;struct Cyc_Core_Opt*_tmp43A;struct Cyc_Absyn_PtrInfo
_tmp43C;_LL26F: {struct Cyc_Absyn_Evar_struct*_tmp437=(struct Cyc_Absyn_Evar_struct*)
_tmp436;if(_tmp437->tag != 1)goto _LL271;else{_tmp438=_tmp437->f2;_tmp439=(struct
Cyc_Core_Opt**)& _tmp437->f2;_tmp43A=_tmp437->f4;}}_LL270: {struct Cyc_List_List*
_tmp43D=(struct Cyc_List_List*)_tmp433->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp43A))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp43D,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp11AB;Cyc_Tcutil_failure_reason=((
_tmp11AB="(type variable would escape scope)",_tag_dyneither(_tmp11AB,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp430->v,_tmp434)){{struct Cyc_Core_Opt*_tmp11AC;*_tmp439=((
_tmp11AC=_cycalloc(sizeof(*_tmp11AC)),((_tmp11AC->v=t1,_tmp11AC))));}return;}{
const char*_tmp11AD;Cyc_Tcutil_failure_reason=((_tmp11AD="(kinds are incompatible)",
_tag_dyneither(_tmp11AD,sizeof(char),25)));}goto _LL26E;}_LL271:{struct Cyc_Absyn_PointerType_struct*
_tmp43B=(struct Cyc_Absyn_PointerType_struct*)_tmp436;if(_tmp43B->tag != 5)goto
_LL273;else{_tmp43C=_tmp43B->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp430->v)->kind
== Cyc_Absyn_BoxKind))goto _LL273;_LL272: {union Cyc_Absyn_Constraint*_tmp441=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp43C.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp442=_tmp441;union Cyc_Absyn_Constraint
_tmp443;int _tmp444;_LL276: _tmp443=*_tmp442;if((_tmp443.No_constr).tag != 3)goto
_LL278;_tmp444=(int)(_tmp443.No_constr).val;_LL277:{struct
_union_Constraint_Eq_constr*_tmp11AE;(_tmp11AE=& _tmp441->Eq_constr,((_tmp11AE->tag=
1,_tmp11AE->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp11AF;*_tmp432=((
_tmp11AF=_cycalloc(sizeof(*_tmp11AF)),((_tmp11AF->v=t2,_tmp11AF))));}return;
_LL278:;_LL279: goto _LL275;_LL275:;}goto _LL26E;}_LL273:;_LL274: goto _LL26E;_LL26E:;}{
const char*_tmp11B0;Cyc_Tcutil_failure_reason=((_tmp11B0="(kinds are incompatible)",
_tag_dyneither(_tmp11B0,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL26C:;_LL26D: goto _LL269;_LL269:;}{struct _tuple0 _tmp11B1;struct _tuple0 _tmp449=(
_tmp11B1.f1=t2,((_tmp11B1.f2=t1,_tmp11B1)));void*_tmp44A;void*_tmp44C;void*
_tmp44E;void*_tmp450;struct Cyc_Absyn_Tvar*_tmp452;void*_tmp453;struct Cyc_Absyn_Tvar*
_tmp455;void*_tmp456;struct Cyc_Absyn_AggrInfo _tmp458;union Cyc_Absyn_AggrInfoU
_tmp459;struct Cyc_List_List*_tmp45A;void*_tmp45B;struct Cyc_Absyn_AggrInfo _tmp45D;
union Cyc_Absyn_AggrInfoU _tmp45E;struct Cyc_List_List*_tmp45F;void*_tmp460;struct
_tuple2*_tmp462;void*_tmp463;struct _tuple2*_tmp465;void*_tmp466;struct Cyc_List_List*
_tmp468;void*_tmp469;struct Cyc_List_List*_tmp46B;void*_tmp46C;struct Cyc_Absyn_DatatypeInfo
_tmp46E;union Cyc_Absyn_DatatypeInfoU _tmp46F;struct Cyc_Absyn_Datatypedecl**
_tmp470;struct Cyc_Absyn_Datatypedecl*_tmp471;struct Cyc_List_List*_tmp472;void*
_tmp473;struct Cyc_Absyn_DatatypeInfo _tmp475;union Cyc_Absyn_DatatypeInfoU _tmp476;
struct Cyc_Absyn_Datatypedecl**_tmp477;struct Cyc_Absyn_Datatypedecl*_tmp478;
struct Cyc_List_List*_tmp479;void*_tmp47A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp47C;union Cyc_Absyn_DatatypeFieldInfoU _tmp47D;struct _tuple3 _tmp47E;struct Cyc_Absyn_Datatypedecl*
_tmp47F;struct Cyc_Absyn_Datatypefield*_tmp480;struct Cyc_List_List*_tmp481;void*
_tmp482;struct Cyc_Absyn_DatatypeFieldInfo _tmp484;union Cyc_Absyn_DatatypeFieldInfoU
_tmp485;struct _tuple3 _tmp486;struct Cyc_Absyn_Datatypedecl*_tmp487;struct Cyc_Absyn_Datatypefield*
_tmp488;struct Cyc_List_List*_tmp489;void*_tmp48A;struct Cyc_Absyn_PtrInfo _tmp48C;
void*_tmp48D;struct Cyc_Absyn_Tqual _tmp48E;struct Cyc_Absyn_PtrAtts _tmp48F;void*
_tmp490;union Cyc_Absyn_Constraint*_tmp491;union Cyc_Absyn_Constraint*_tmp492;
union Cyc_Absyn_Constraint*_tmp493;void*_tmp494;struct Cyc_Absyn_PtrInfo _tmp496;
void*_tmp497;struct Cyc_Absyn_Tqual _tmp498;struct Cyc_Absyn_PtrAtts _tmp499;void*
_tmp49A;union Cyc_Absyn_Constraint*_tmp49B;union Cyc_Absyn_Constraint*_tmp49C;
union Cyc_Absyn_Constraint*_tmp49D;void*_tmp49E;enum Cyc_Absyn_Sign _tmp4A0;enum 
Cyc_Absyn_Size_of _tmp4A1;void*_tmp4A2;enum Cyc_Absyn_Sign _tmp4A4;enum Cyc_Absyn_Size_of
_tmp4A5;void*_tmp4A6;void*_tmp4A8;void*_tmp4AA;int _tmp4AC;void*_tmp4AD;int
_tmp4AF;void*_tmp4B0;void*_tmp4B2;void*_tmp4B3;void*_tmp4B5;void*_tmp4B6;struct
Cyc_Absyn_Exp*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;void*_tmp4BC;
struct Cyc_Absyn_ArrayInfo _tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Tqual _tmp4C0;
struct Cyc_Absyn_Exp*_tmp4C1;union Cyc_Absyn_Constraint*_tmp4C2;void*_tmp4C3;
struct Cyc_Absyn_ArrayInfo _tmp4C5;void*_tmp4C6;struct Cyc_Absyn_Tqual _tmp4C7;
struct Cyc_Absyn_Exp*_tmp4C8;union Cyc_Absyn_Constraint*_tmp4C9;void*_tmp4CA;
struct Cyc_Absyn_FnInfo _tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Core_Opt*
_tmp4CE;void*_tmp4CF;struct Cyc_List_List*_tmp4D0;int _tmp4D1;struct Cyc_Absyn_VarargInfo*
_tmp4D2;struct Cyc_List_List*_tmp4D3;struct Cyc_List_List*_tmp4D4;void*_tmp4D5;
struct Cyc_Absyn_FnInfo _tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_Core_Opt*
_tmp4D9;void*_tmp4DA;struct Cyc_List_List*_tmp4DB;int _tmp4DC;struct Cyc_Absyn_VarargInfo*
_tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4DF;void*_tmp4E0;
struct Cyc_List_List*_tmp4E2;void*_tmp4E3;struct Cyc_List_List*_tmp4E5;void*
_tmp4E6;enum Cyc_Absyn_AggrKind _tmp4E8;struct Cyc_List_List*_tmp4E9;void*_tmp4EA;
enum Cyc_Absyn_AggrKind _tmp4EC;struct Cyc_List_List*_tmp4ED;void*_tmp4EE;void*
_tmp4F0;void*_tmp4F2;void*_tmp4F4;void*_tmp4F6;void*_tmp4F8;void*_tmp4F9;void*
_tmp4FB;void*_tmp4FC;void*_tmp4FE;void*_tmp4FF;void*_tmp500;void*_tmp502;void*
_tmp503;void*_tmp504;void*_tmp506;void*_tmp508;void*_tmp50A;void*_tmp50C;void*
_tmp50E;_LL27B: _tmp44A=_tmp449.f1;{struct Cyc_Absyn_Evar_struct*_tmp44B=(struct
Cyc_Absyn_Evar_struct*)_tmp44A;if(_tmp44B->tag != 1)goto _LL27D;};_LL27C: Cyc_Tcutil_unify_it(
t2,t1);return;_LL27D: _tmp44C=_tmp449.f1;{struct Cyc_Absyn_VoidType_struct*_tmp44D=(
struct Cyc_Absyn_VoidType_struct*)_tmp44C;if(_tmp44D->tag != 0)goto _LL27F;};
_tmp44E=_tmp449.f2;{struct Cyc_Absyn_VoidType_struct*_tmp44F=(struct Cyc_Absyn_VoidType_struct*)
_tmp44E;if(_tmp44F->tag != 0)goto _LL27F;};_LL27E: return;_LL27F: _tmp450=_tmp449.f1;{
struct Cyc_Absyn_VarType_struct*_tmp451=(struct Cyc_Absyn_VarType_struct*)_tmp450;
if(_tmp451->tag != 2)goto _LL281;else{_tmp452=_tmp451->f1;}};_tmp453=_tmp449.f2;{
struct Cyc_Absyn_VarType_struct*_tmp454=(struct Cyc_Absyn_VarType_struct*)_tmp453;
if(_tmp454->tag != 2)goto _LL281;else{_tmp455=_tmp454->f1;}};_LL280: {struct
_dyneither_ptr*_tmp510=_tmp452->name;struct _dyneither_ptr*_tmp511=_tmp455->name;
int _tmp512=_tmp452->identity;int _tmp513=_tmp455->identity;if(_tmp513 == _tmp512)
return;{const char*_tmp11B2;Cyc_Tcutil_failure_reason=((_tmp11B2="(variable types are not the same)",
_tag_dyneither(_tmp11B2,sizeof(char),34)));}goto _LL27A;}_LL281: _tmp456=_tmp449.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp457=(struct Cyc_Absyn_AggrType_struct*)
_tmp456;if(_tmp457->tag != 12)goto _LL283;else{_tmp458=_tmp457->f1;_tmp459=_tmp458.aggr_info;
_tmp45A=_tmp458.targs;}};_tmp45B=_tmp449.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp45C=(struct Cyc_Absyn_AggrType_struct*)_tmp45B;if(_tmp45C->tag != 12)goto
_LL283;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45D.aggr_info;_tmp45F=_tmp45D.targs;}};
_LL282: {enum Cyc_Absyn_AggrKind _tmp516;struct _tuple2*_tmp517;struct _tuple12
_tmp515=Cyc_Absyn_aggr_kinded_name(_tmp45E);_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;{
enum Cyc_Absyn_AggrKind _tmp519;struct _tuple2*_tmp51A;struct _tuple12 _tmp518=Cyc_Absyn_aggr_kinded_name(
_tmp459);_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;if(_tmp516 != _tmp519){{const char*
_tmp11B3;Cyc_Tcutil_failure_reason=((_tmp11B3="(struct and union type)",
_tag_dyneither(_tmp11B3,sizeof(char),24)));}goto _LL27A;}if(Cyc_Absyn_qvar_cmp(
_tmp517,_tmp51A)!= 0){{const char*_tmp11B4;Cyc_Tcutil_failure_reason=((_tmp11B4="(different type name)",
_tag_dyneither(_tmp11B4,sizeof(char),22)));}goto _LL27A;}Cyc_Tcutil_unify_list(
_tmp45F,_tmp45A);return;};}_LL283: _tmp460=_tmp449.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp461=(struct Cyc_Absyn_EnumType_struct*)_tmp460;if(_tmp461->tag != 14)goto
_LL285;else{_tmp462=_tmp461->f1;}};_tmp463=_tmp449.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp464=(struct Cyc_Absyn_EnumType_struct*)_tmp463;if(_tmp464->tag != 14)goto
_LL285;else{_tmp465=_tmp464->f1;}};_LL284: if(Cyc_Absyn_qvar_cmp(_tmp462,_tmp465)
== 0)return;{const char*_tmp11B5;Cyc_Tcutil_failure_reason=((_tmp11B5="(different enum types)",
_tag_dyneither(_tmp11B5,sizeof(char),23)));}goto _LL27A;_LL285: _tmp466=_tmp449.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp467=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp466;if(_tmp467->tag != 15)goto _LL287;else{_tmp468=_tmp467->f1;}};_tmp469=
_tmp449.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp46A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp469;if(_tmp46A->tag != 15)goto _LL287;else{_tmp46B=_tmp46A->f1;}};_LL286: {int
bad=0;for(0;_tmp468 != 0  && _tmp46B != 0;(_tmp468=_tmp468->tl,_tmp46B=_tmp46B->tl)){
struct Cyc_Absyn_Enumfield*_tmp51E=(struct Cyc_Absyn_Enumfield*)_tmp468->hd;struct
Cyc_Absyn_Enumfield*_tmp51F=(struct Cyc_Absyn_Enumfield*)_tmp46B->hd;if(Cyc_Absyn_qvar_cmp(
_tmp51E->name,_tmp51F->name)!= 0){{const char*_tmp11B6;Cyc_Tcutil_failure_reason=((
_tmp11B6="(different names for enum fields)",_tag_dyneither(_tmp11B6,sizeof(char),
34)));}bad=1;break;}if(_tmp51E->tag == _tmp51F->tag)continue;if(_tmp51E->tag == 0
 || _tmp51F->tag == 0){{const char*_tmp11B7;Cyc_Tcutil_failure_reason=((_tmp11B7="(different tag values for enum fields)",
_tag_dyneither(_tmp11B7,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp51E->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp51F->tag))){{const char*_tmp11B8;Cyc_Tcutil_failure_reason=((_tmp11B8="(different tag values for enum fields)",
_tag_dyneither(_tmp11B8,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL27A;if(
_tmp468 == 0  && _tmp46B == 0)return;{const char*_tmp11B9;Cyc_Tcutil_failure_reason=((
_tmp11B9="(different number of fields for enums)",_tag_dyneither(_tmp11B9,
sizeof(char),39)));}goto _LL27A;}_LL287: _tmp46C=_tmp449.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp46D=(struct Cyc_Absyn_DatatypeType_struct*)_tmp46C;if(_tmp46D->tag != 3)goto
_LL289;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.datatype_info;if((_tmp46F.KnownDatatype).tag
!= 2)goto _LL289;_tmp470=(struct Cyc_Absyn_Datatypedecl**)(_tmp46F.KnownDatatype).val;
_tmp471=*_tmp470;_tmp472=_tmp46E.targs;}};_tmp473=_tmp449.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp474=(struct Cyc_Absyn_DatatypeType_struct*)_tmp473;if(_tmp474->tag != 3)goto
_LL289;else{_tmp475=_tmp474->f1;_tmp476=_tmp475.datatype_info;if((_tmp476.KnownDatatype).tag
!= 2)goto _LL289;_tmp477=(struct Cyc_Absyn_Datatypedecl**)(_tmp476.KnownDatatype).val;
_tmp478=*_tmp477;_tmp479=_tmp475.targs;}};_LL288: if(_tmp471 == _tmp478  || Cyc_Absyn_qvar_cmp(
_tmp471->name,_tmp478->name)== 0){Cyc_Tcutil_unify_list(_tmp479,_tmp472);return;}
goto _LL27A;_LL289: _tmp47A=_tmp449.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp47B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp47A;if(_tmp47B->tag != 4)
goto _LL28B;else{_tmp47C=_tmp47B->f1;_tmp47D=_tmp47C.field_info;if((_tmp47D.KnownDatatypefield).tag
!= 2)goto _LL28B;_tmp47E=(struct _tuple3)(_tmp47D.KnownDatatypefield).val;_tmp47F=
_tmp47E.f1;_tmp480=_tmp47E.f2;_tmp481=_tmp47C.targs;}};_tmp482=_tmp449.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp483=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp482;if(_tmp483->tag != 4)goto _LL28B;else{_tmp484=_tmp483->f1;_tmp485=_tmp484.field_info;
if((_tmp485.KnownDatatypefield).tag != 2)goto _LL28B;_tmp486=(struct _tuple3)(
_tmp485.KnownDatatypefield).val;_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;_tmp489=
_tmp484.targs;}};_LL28A: if((_tmp47F == _tmp487  || Cyc_Absyn_qvar_cmp(_tmp47F->name,
_tmp487->name)== 0) && (_tmp480 == _tmp488  || Cyc_Absyn_qvar_cmp(_tmp480->name,
_tmp488->name)== 0)){Cyc_Tcutil_unify_list(_tmp489,_tmp481);return;}{const char*
_tmp11BA;Cyc_Tcutil_failure_reason=((_tmp11BA="(different datatype field types)",
_tag_dyneither(_tmp11BA,sizeof(char),33)));}goto _LL27A;_LL28B: _tmp48A=_tmp449.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp48B=(struct Cyc_Absyn_PointerType_struct*)
_tmp48A;if(_tmp48B->tag != 5)goto _LL28D;else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48C.elt_typ;
_tmp48E=_tmp48C.elt_tq;_tmp48F=_tmp48C.ptr_atts;_tmp490=_tmp48F.rgn;_tmp491=
_tmp48F.nullable;_tmp492=_tmp48F.bounds;_tmp493=_tmp48F.zero_term;}};_tmp494=
_tmp449.f2;{struct Cyc_Absyn_PointerType_struct*_tmp495=(struct Cyc_Absyn_PointerType_struct*)
_tmp494;if(_tmp495->tag != 5)goto _LL28D;else{_tmp496=_tmp495->f1;_tmp497=_tmp496.elt_typ;
_tmp498=_tmp496.elt_tq;_tmp499=_tmp496.ptr_atts;_tmp49A=_tmp499.rgn;_tmp49B=
_tmp499.nullable;_tmp49C=_tmp499.bounds;_tmp49D=_tmp499.zero_term;}};_LL28C: Cyc_Tcutil_unify_it(
_tmp497,_tmp48D);Cyc_Tcutil_unify_it(_tmp490,_tmp49A);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp11BB;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp49D,_tmp493,((_tmp11BB="(not both zero terminated)",
_tag_dyneither(_tmp11BB,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp498,
_tmp497,_tmp48E,_tmp48D);{const char*_tmp11BC;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp49C,_tmp492,((
_tmp11BC="(different pointer bounds)",_tag_dyneither(_tmp11BC,sizeof(char),27))));}{
void*_tmp527=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp49C);_LL2B6: {struct Cyc_Absyn_DynEither_b_struct*
_tmp528=(struct Cyc_Absyn_DynEither_b_struct*)_tmp527;if(_tmp528->tag != 0)goto
_LL2B8;}_LL2B7: return;_LL2B8:;_LL2B9: goto _LL2B5;_LL2B5:;}{const char*_tmp11BD;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp49B,_tmp491,((_tmp11BD="(incompatible pointer types)",_tag_dyneither(
_tmp11BD,sizeof(char),29))));}return;_LL28D: _tmp49E=_tmp449.f1;{struct Cyc_Absyn_IntType_struct*
_tmp49F=(struct Cyc_Absyn_IntType_struct*)_tmp49E;if(_tmp49F->tag != 6)goto _LL28F;
else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}};_tmp4A2=_tmp449.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4A3=(struct Cyc_Absyn_IntType_struct*)_tmp4A2;if(_tmp4A3->tag != 6)goto _LL28F;
else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}};_LL28E: if(_tmp4A4 == _tmp4A0  && ((
_tmp4A5 == _tmp4A1  || _tmp4A5 == Cyc_Absyn_Int_sz  && _tmp4A1 == Cyc_Absyn_Long_sz)
 || _tmp4A5 == Cyc_Absyn_Long_sz  && _tmp4A1 == Cyc_Absyn_Int_sz))return;{const char*
_tmp11BE;Cyc_Tcutil_failure_reason=((_tmp11BE="(different integral types)",
_tag_dyneither(_tmp11BE,sizeof(char),27)));}goto _LL27A;_LL28F: _tmp4A6=_tmp449.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4A7=(struct Cyc_Absyn_FloatType_struct*)
_tmp4A6;if(_tmp4A7->tag != 7)goto _LL291;};_tmp4A8=_tmp449.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4A9=(struct Cyc_Absyn_FloatType_struct*)_tmp4A8;if(_tmp4A9->tag != 7)goto
_LL291;};_LL290: return;_LL291: _tmp4AA=_tmp449.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4AB=(struct Cyc_Absyn_DoubleType_struct*)_tmp4AA;if(_tmp4AB->tag != 8)goto
_LL293;else{_tmp4AC=_tmp4AB->f1;}};_tmp4AD=_tmp449.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4AE=(struct Cyc_Absyn_DoubleType_struct*)_tmp4AD;if(_tmp4AE->tag != 8)goto
_LL293;else{_tmp4AF=_tmp4AE->f1;}};_LL292: if(_tmp4AC == _tmp4AF)return;goto _LL27A;
_LL293: _tmp4B0=_tmp449.f1;{struct Cyc_Absyn_TagType_struct*_tmp4B1=(struct Cyc_Absyn_TagType_struct*)
_tmp4B0;if(_tmp4B1->tag != 20)goto _LL295;else{_tmp4B2=(void*)_tmp4B1->f1;}};
_tmp4B3=_tmp449.f2;{struct Cyc_Absyn_TagType_struct*_tmp4B4=(struct Cyc_Absyn_TagType_struct*)
_tmp4B3;if(_tmp4B4->tag != 20)goto _LL295;else{_tmp4B5=(void*)_tmp4B4->f1;}};
_LL294: Cyc_Tcutil_unify_it(_tmp4B2,_tmp4B5);return;_LL295: _tmp4B6=_tmp449.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp4B7=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4B6;if(_tmp4B7->tag != 19)goto _LL297;else{_tmp4B8=_tmp4B7->f1;}};_tmp4B9=
_tmp449.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp4BA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4B9;if(_tmp4BA->tag != 19)goto _LL297;else{_tmp4BB=_tmp4BA->f1;}};_LL296: if(!
Cyc_Evexp_same_const_exp(_tmp4B8,_tmp4BB)){{const char*_tmp11BF;Cyc_Tcutil_failure_reason=((
_tmp11BF="(cannot prove expressions are the same)",_tag_dyneither(_tmp11BF,
sizeof(char),40)));}goto _LL27A;}return;_LL297: _tmp4BC=_tmp449.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4BD=(struct Cyc_Absyn_ArrayType_struct*)_tmp4BC;if(_tmp4BD->tag != 9)goto
_LL299;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.elt_type;_tmp4C0=_tmp4BE.tq;
_tmp4C1=_tmp4BE.num_elts;_tmp4C2=_tmp4BE.zero_term;}};_tmp4C3=_tmp449.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4C4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C3;
if(_tmp4C4->tag != 9)goto _LL299;else{_tmp4C5=_tmp4C4->f1;_tmp4C6=_tmp4C5.elt_type;
_tmp4C7=_tmp4C5.tq;_tmp4C8=_tmp4C5.num_elts;_tmp4C9=_tmp4C5.zero_term;}};_LL298:
Cyc_Tcutil_unify_it(_tmp4C6,_tmp4BF);Cyc_Tcutil_unify_tqual(_tmp4C7,_tmp4C6,
_tmp4C0,_tmp4BF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp11C0;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4C2,_tmp4C9,((_tmp11C0="(not both zero terminated)",_tag_dyneither(_tmp11C0,
sizeof(char),27))));}if(_tmp4C1 == _tmp4C8)return;if(_tmp4C1 == 0  || _tmp4C8 == 0)
goto _LL27A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4C1,(struct Cyc_Absyn_Exp*)
_tmp4C8))return;{const char*_tmp11C1;Cyc_Tcutil_failure_reason=((_tmp11C1="(different array sizes)",
_tag_dyneither(_tmp11C1,sizeof(char),24)));}goto _LL27A;_LL299: _tmp4CA=_tmp449.f1;{
struct Cyc_Absyn_FnType_struct*_tmp4CB=(struct Cyc_Absyn_FnType_struct*)_tmp4CA;
if(_tmp4CB->tag != 10)goto _LL29B;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CC.tvars;
_tmp4CE=_tmp4CC.effect;_tmp4CF=_tmp4CC.ret_typ;_tmp4D0=_tmp4CC.args;_tmp4D1=
_tmp4CC.c_varargs;_tmp4D2=_tmp4CC.cyc_varargs;_tmp4D3=_tmp4CC.rgn_po;_tmp4D4=
_tmp4CC.attributes;}};_tmp4D5=_tmp449.f2;{struct Cyc_Absyn_FnType_struct*_tmp4D6=(
struct Cyc_Absyn_FnType_struct*)_tmp4D5;if(_tmp4D6->tag != 10)goto _LL29B;else{
_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D7.tvars;_tmp4D9=_tmp4D7.effect;_tmp4DA=_tmp4D7.ret_typ;
_tmp4DB=_tmp4D7.args;_tmp4DC=_tmp4D7.c_varargs;_tmp4DD=_tmp4D7.cyc_varargs;
_tmp4DE=_tmp4D7.rgn_po;_tmp4DF=_tmp4D7.attributes;}};_LL29A: {int done=0;struct
_RegionHandle _tmp52E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp52E;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp4D8 != 0){if(_tmp4CD == 0){{
const char*_tmp11C2;Cyc_Tcutil_failure_reason=((_tmp11C2="(second function type has too few type variables)",
_tag_dyneither(_tmp11C2,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp530=((struct Cyc_Absyn_Tvar*)_tmp4D8->hd)->kind;void*_tmp531=((struct Cyc_Absyn_Tvar*)
_tmp4CD->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp530,_tmp531)){{const char*
_tmp11C8;void*_tmp11C7[3];struct Cyc_String_pa_struct _tmp11C6;struct Cyc_String_pa_struct
_tmp11C5;struct Cyc_String_pa_struct _tmp11C4;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp11C4.tag=0,((_tmp11C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp4CD->hd,& Cyc_Tcutil_bk))),((_tmp11C5.tag=0,((_tmp11C5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp4D8->hd,& Cyc_Tcutil_bk))),((_tmp11C6.tag=0,((_tmp11C6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp4D8->hd)),((_tmp11C7[0]=& _tmp11C6,((_tmp11C7[1]=& _tmp11C5,((_tmp11C7[2]=&
_tmp11C4,Cyc_aprintf(((_tmp11C8="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp11C8,sizeof(char),44))),_tag_dyneither(_tmp11C7,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp11D2;struct Cyc_Absyn_VarType_struct _tmp11D1;struct Cyc_Absyn_VarType_struct*
_tmp11D0;struct Cyc_List_List*_tmp11CF;inst=((_tmp11CF=_region_malloc(rgn,sizeof(*
_tmp11CF)),((_tmp11CF->hd=((_tmp11D2=_region_malloc(rgn,sizeof(*_tmp11D2)),((
_tmp11D2->f1=(struct Cyc_Absyn_Tvar*)_tmp4CD->hd,((_tmp11D2->f2=(void*)((_tmp11D0=
_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0[0]=((_tmp11D1.tag=2,((_tmp11D1.f1=(
struct Cyc_Absyn_Tvar*)_tmp4D8->hd,_tmp11D1)))),_tmp11D0)))),_tmp11D2)))))),((
_tmp11CF->tl=inst,_tmp11CF))))));}_tmp4D8=_tmp4D8->tl;_tmp4CD=_tmp4CD->tl;};}if(
_tmp4CD != 0){{const char*_tmp11D3;Cyc_Tcutil_failure_reason=((_tmp11D3="(second function type has too many type variables)",
_tag_dyneither(_tmp11D3,sizeof(char),51)));}_npop_handler(0);goto _LL27A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp11DF;struct Cyc_Absyn_FnInfo _tmp11DE;
struct Cyc_Absyn_FnType_struct*_tmp11DD;struct Cyc_Absyn_FnType_struct _tmp11D9;
struct Cyc_Absyn_FnInfo _tmp11D8;struct Cyc_Absyn_FnType_struct*_tmp11D7;Cyc_Tcutil_unify_it((
void*)((_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7[0]=((_tmp11D9.tag=10,((
_tmp11D9.f1=((_tmp11D8.tvars=0,((_tmp11D8.effect=_tmp4D9,((_tmp11D8.ret_typ=
_tmp4DA,((_tmp11D8.args=_tmp4DB,((_tmp11D8.c_varargs=_tmp4DC,((_tmp11D8.cyc_varargs=
_tmp4DD,((_tmp11D8.rgn_po=_tmp4DE,((_tmp11D8.attributes=_tmp4DF,_tmp11D8)))))))))))))))),
_tmp11D9)))),_tmp11D7)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp11DD=
_cycalloc(sizeof(*_tmp11DD)),((_tmp11DD[0]=((_tmp11DF.tag=10,((_tmp11DF.f1=((
_tmp11DE.tvars=0,((_tmp11DE.effect=_tmp4CE,((_tmp11DE.ret_typ=_tmp4CF,((_tmp11DE.args=
_tmp4D0,((_tmp11DE.c_varargs=_tmp4D1,((_tmp11DE.cyc_varargs=_tmp4D2,((_tmp11DE.rgn_po=
_tmp4D3,((_tmp11DE.attributes=_tmp4D4,_tmp11DE)))))))))))))))),_tmp11DF)))),
_tmp11DD))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp4DA,_tmp4CF);for(0;_tmp4DB != 0  && _tmp4D0 != 0;(_tmp4DB=_tmp4DB->tl,_tmp4D0=
_tmp4D0->tl)){struct Cyc_Absyn_Tqual _tmp543;void*_tmp544;struct _tuple9 _tmp542=*((
struct _tuple9*)_tmp4DB->hd);_tmp543=_tmp542.f2;_tmp544=_tmp542.f3;{struct Cyc_Absyn_Tqual
_tmp546;void*_tmp547;struct _tuple9 _tmp545=*((struct _tuple9*)_tmp4D0->hd);_tmp546=
_tmp545.f2;_tmp547=_tmp545.f3;Cyc_Tcutil_unify_it(_tmp544,_tmp547);Cyc_Tcutil_unify_tqual(
_tmp543,_tmp544,_tmp546,_tmp547);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp4DB != 0  || _tmp4D0 != 0){{const char*_tmp11E0;Cyc_Tcutil_failure_reason=((
_tmp11E0="(function types have different number of arguments)",_tag_dyneither(
_tmp11E0,sizeof(char),52)));}_npop_handler(0);goto _LL27A;}if(_tmp4DC != _tmp4D1){{
const char*_tmp11E1;Cyc_Tcutil_failure_reason=((_tmp11E1="(only one function type takes C varargs)",
_tag_dyneither(_tmp11E1,sizeof(char),41)));}_npop_handler(0);goto _LL27A;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp11E2;struct _tuple21 _tmp54B=(_tmp11E2.f1=
_tmp4DD,((_tmp11E2.f2=_tmp4D2,_tmp11E2)));struct Cyc_Absyn_VarargInfo*_tmp54C;
struct Cyc_Absyn_VarargInfo*_tmp54D;struct Cyc_Absyn_VarargInfo*_tmp54E;struct Cyc_Absyn_VarargInfo*
_tmp54F;struct Cyc_Absyn_VarargInfo*_tmp550;struct Cyc_Absyn_VarargInfo _tmp551;
struct Cyc_Core_Opt*_tmp552;struct Cyc_Absyn_Tqual _tmp553;void*_tmp554;int _tmp555;
struct Cyc_Absyn_VarargInfo*_tmp556;struct Cyc_Absyn_VarargInfo _tmp557;struct Cyc_Core_Opt*
_tmp558;struct Cyc_Absyn_Tqual _tmp559;void*_tmp55A;int _tmp55B;_LL2BB: _tmp54C=
_tmp54B.f1;if(_tmp54C != 0)goto _LL2BD;_tmp54D=_tmp54B.f2;if(_tmp54D != 0)goto
_LL2BD;_LL2BC: goto _LL2BA;_LL2BD: _tmp54E=_tmp54B.f1;if(_tmp54E != 0)goto _LL2BF;
_LL2BE: goto _LL2C0;_LL2BF: _tmp54F=_tmp54B.f2;if(_tmp54F != 0)goto _LL2C1;_LL2C0:
bad_cyc_vararg=1;{const char*_tmp11E3;Cyc_Tcutil_failure_reason=((_tmp11E3="(only one function type takes varargs)",
_tag_dyneither(_tmp11E3,sizeof(char),39)));}goto _LL2BA;_LL2C1: _tmp550=_tmp54B.f1;
if(_tmp550 == 0)goto _LL2BA;_tmp551=*_tmp550;_tmp552=_tmp551.name;_tmp553=_tmp551.tq;
_tmp554=_tmp551.type;_tmp555=_tmp551.inject;_tmp556=_tmp54B.f2;if(_tmp556 == 0)
goto _LL2BA;_tmp557=*_tmp556;_tmp558=_tmp557.name;_tmp559=_tmp557.tq;_tmp55A=
_tmp557.type;_tmp55B=_tmp557.inject;_LL2C2: Cyc_Tcutil_unify_it(_tmp554,_tmp55A);
Cyc_Tcutil_unify_tqual(_tmp553,_tmp554,_tmp559,_tmp55A);if(_tmp555 != _tmp55B){
bad_cyc_vararg=1;{const char*_tmp11E4;Cyc_Tcutil_failure_reason=((_tmp11E4="(only one function type injects varargs)",
_tag_dyneither(_tmp11E4,sizeof(char),41)));};}goto _LL2BA;_LL2BA:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL27A;}{int bad_effect=0;{struct _tuple22
_tmp11E5;struct _tuple22 _tmp55F=(_tmp11E5.f1=_tmp4D9,((_tmp11E5.f2=_tmp4CE,
_tmp11E5)));struct Cyc_Core_Opt*_tmp560;struct Cyc_Core_Opt*_tmp561;struct Cyc_Core_Opt*
_tmp562;struct Cyc_Core_Opt*_tmp563;_LL2C4: _tmp560=_tmp55F.f1;if(_tmp560 != 0)goto
_LL2C6;_tmp561=_tmp55F.f2;if(_tmp561 != 0)goto _LL2C6;_LL2C5: goto _LL2C3;_LL2C6:
_tmp562=_tmp55F.f1;if(_tmp562 != 0)goto _LL2C8;_LL2C7: goto _LL2C9;_LL2C8: _tmp563=
_tmp55F.f2;if(_tmp563 != 0)goto _LL2CA;_LL2C9: bad_effect=1;goto _LL2C3;_LL2CA:;
_LL2CB: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4D9))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CE))->v);
goto _LL2C3;_LL2C3:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp11E6;Cyc_Tcutil_failure_reason=((_tmp11E6="(function type effects do not unify)",
_tag_dyneither(_tmp11E6,sizeof(char),37)));}_npop_handler(0);goto _LL27A;}if(!Cyc_Tcutil_same_atts(
_tmp4D4,_tmp4DF)){{const char*_tmp11E7;Cyc_Tcutil_failure_reason=((_tmp11E7="(function types have different attributes)",
_tag_dyneither(_tmp11E7,sizeof(char),43)));}_npop_handler(0);goto _LL27A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp4D3,_tmp4DE)){{const char*_tmp11E8;Cyc_Tcutil_failure_reason=((_tmp11E8="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp11E8,sizeof(char),58)));}_npop_handler(0);goto _LL27A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL29B: _tmp4E0=_tmp449.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp4E1=(struct Cyc_Absyn_TupleType_struct*)_tmp4E0;
if(_tmp4E1->tag != 11)goto _LL29D;else{_tmp4E2=_tmp4E1->f1;}};_tmp4E3=_tmp449.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp4E4=(struct Cyc_Absyn_TupleType_struct*)
_tmp4E3;if(_tmp4E4->tag != 11)goto _LL29D;else{_tmp4E5=_tmp4E4->f1;}};_LL29C: for(0;
_tmp4E5 != 0  && _tmp4E2 != 0;(_tmp4E5=_tmp4E5->tl,_tmp4E2=_tmp4E2->tl)){struct Cyc_Absyn_Tqual
_tmp568;void*_tmp569;struct _tuple11 _tmp567=*((struct _tuple11*)_tmp4E5->hd);
_tmp568=_tmp567.f1;_tmp569=_tmp567.f2;{struct Cyc_Absyn_Tqual _tmp56B;void*_tmp56C;
struct _tuple11 _tmp56A=*((struct _tuple11*)_tmp4E2->hd);_tmp56B=_tmp56A.f1;_tmp56C=
_tmp56A.f2;Cyc_Tcutil_unify_it(_tmp569,_tmp56C);Cyc_Tcutil_unify_tqual(_tmp568,
_tmp569,_tmp56B,_tmp56C);};}if(_tmp4E5 == 0  && _tmp4E2 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp11E9;Cyc_Tcutil_failure_reason=((
_tmp11E9="(tuple types have different numbers of components)",_tag_dyneither(
_tmp11E9,sizeof(char),51)));}goto _LL27A;_LL29D: _tmp4E6=_tmp449.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp4E7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4E6;if(_tmp4E7->tag != 13)goto
_LL29F;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;}};_tmp4EA=_tmp449.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp4EB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4EA;if(_tmp4EB->tag != 13)goto _LL29F;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}};
_LL29E: if(_tmp4EC != _tmp4E8){{const char*_tmp11EA;Cyc_Tcutil_failure_reason=((
_tmp11EA="(struct and union type)",_tag_dyneither(_tmp11EA,sizeof(char),24)));}
goto _LL27A;}for(0;_tmp4ED != 0  && _tmp4E9 != 0;(_tmp4ED=_tmp4ED->tl,_tmp4E9=
_tmp4E9->tl)){struct Cyc_Absyn_Aggrfield*_tmp56F=(struct Cyc_Absyn_Aggrfield*)
_tmp4ED->hd;struct Cyc_Absyn_Aggrfield*_tmp570=(struct Cyc_Absyn_Aggrfield*)
_tmp4E9->hd;if(Cyc_strptrcmp(_tmp56F->name,_tmp570->name)!= 0){{const char*
_tmp11EB;Cyc_Tcutil_failure_reason=((_tmp11EB="(different member names)",
_tag_dyneither(_tmp11EB,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp56F->type,_tmp570->type);Cyc_Tcutil_unify_tqual(_tmp56F->tq,
_tmp56F->type,_tmp570->tq,_tmp570->type);if(!Cyc_Tcutil_same_atts(_tmp56F->attributes,
_tmp570->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp11EC;Cyc_Tcutil_failure_reason=((_tmp11EC="(different attributes on member)",
_tag_dyneither(_tmp11EC,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp56F->width != 0  && _tmp570->width == 0  || _tmp570->width != 0  && _tmp56F->width
== 0) || (_tmp56F->width != 0  && _tmp570->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp56F->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp570->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp11ED;Cyc_Tcutil_failure_reason=((_tmp11ED="(different bitfield widths on member)",
_tag_dyneither(_tmp11ED,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp4ED == 0  && _tmp4E9 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp11EE;Cyc_Tcutil_failure_reason=((_tmp11EE="(different number of members)",
_tag_dyneither(_tmp11EE,sizeof(char),30)));}goto _LL27A;_LL29F: _tmp4EE=_tmp449.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp4EF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4EE;
if(_tmp4EF->tag != 21)goto _LL2A1;};_tmp4F0=_tmp449.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp4F1=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4F0;if(_tmp4F1->tag != 21)goto _LL2A1;};
_LL2A0: return;_LL2A1: _tmp4F2=_tmp449.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp4F3=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp4F2;if(_tmp4F3->tag != 22)goto
_LL2A3;};_tmp4F4=_tmp449.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp4F5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp4F4;if(_tmp4F5->tag != 22)goto _LL2A3;};_LL2A2: return;_LL2A3: _tmp4F6=_tmp449.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp4F7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F6;if(_tmp4F7->tag != 16)goto _LL2A5;else{_tmp4F8=(void*)_tmp4F7->f1;}};
_tmp4F9=_tmp449.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp4FA=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F9;if(_tmp4FA->tag != 16)goto _LL2A5;else{_tmp4FB=(void*)_tmp4FA->f1;}};
_LL2A4: Cyc_Tcutil_unify_it(_tmp4F8,_tmp4FB);return;_LL2A5: _tmp4FC=_tmp449.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp4FD=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4FC;if(_tmp4FD->tag != 17)goto _LL2A7;else{_tmp4FE=(void*)_tmp4FD->f1;_tmp4FF=(
void*)_tmp4FD->f2;}};_tmp500=_tmp449.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp501=(struct Cyc_Absyn_DynRgnType_struct*)_tmp500;if(_tmp501->tag != 17)goto
_LL2A7;else{_tmp502=(void*)_tmp501->f1;_tmp503=(void*)_tmp501->f2;}};_LL2A6: Cyc_Tcutil_unify_it(
_tmp4FE,_tmp502);Cyc_Tcutil_unify_it(_tmp4FF,_tmp503);return;_LL2A7: _tmp504=
_tmp449.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp505=(struct Cyc_Absyn_JoinEff_struct*)
_tmp504;if(_tmp505->tag != 24)goto _LL2A9;};_LL2A8: goto _LL2AA;_LL2A9: _tmp506=
_tmp449.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp507=(struct Cyc_Absyn_JoinEff_struct*)
_tmp506;if(_tmp507->tag != 24)goto _LL2AB;};_LL2AA: goto _LL2AC;_LL2AB: _tmp508=
_tmp449.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp509=(struct Cyc_Absyn_AccessEff_struct*)
_tmp508;if(_tmp509->tag != 23)goto _LL2AD;};_LL2AC: goto _LL2AE;_LL2AD: _tmp50A=
_tmp449.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp50B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50A;if(_tmp50B->tag != 25)goto _LL2AF;};_LL2AE: goto _LL2B0;_LL2AF: _tmp50C=
_tmp449.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp50D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50C;if(_tmp50D->tag != 25)goto _LL2B1;};_LL2B0: goto _LL2B2;_LL2B1: _tmp50E=
_tmp449.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp50F=(struct Cyc_Absyn_AccessEff_struct*)
_tmp50E;if(_tmp50F->tag != 23)goto _LL2B3;};_LL2B2: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp11EF;Cyc_Tcutil_failure_reason=((_tmp11EF="(effects don't unify)",
_tag_dyneither(_tmp11EF,sizeof(char),22)));}goto _LL27A;_LL2B3:;_LL2B4: goto _LL27A;
_LL27A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp576=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp577=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp576,_tmp577);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp578=x;union Cyc_Absyn_Constraint _tmp579;int _tmp57A;
union Cyc_Absyn_Constraint _tmp57B;void*_tmp57C;union Cyc_Absyn_Constraint _tmp57D;
union Cyc_Absyn_Constraint*_tmp57E;_LL2CD: _tmp579=*_tmp578;if((_tmp579.No_constr).tag
!= 3)goto _LL2CF;_tmp57A=(int)(_tmp579.No_constr).val;_LL2CE: return - 1;_LL2CF:
_tmp57B=*_tmp578;if((_tmp57B.Eq_constr).tag != 1)goto _LL2D1;_tmp57C=(void*)(
_tmp57B.Eq_constr).val;_LL2D0: {union Cyc_Absyn_Constraint*_tmp57F=y;union Cyc_Absyn_Constraint
_tmp580;int _tmp581;union Cyc_Absyn_Constraint _tmp582;void*_tmp583;union Cyc_Absyn_Constraint
_tmp584;union Cyc_Absyn_Constraint*_tmp585;_LL2D4: _tmp580=*_tmp57F;if((_tmp580.No_constr).tag
!= 3)goto _LL2D6;_tmp581=(int)(_tmp580.No_constr).val;_LL2D5: return 1;_LL2D6:
_tmp582=*_tmp57F;if((_tmp582.Eq_constr).tag != 1)goto _LL2D8;_tmp583=(void*)(
_tmp582.Eq_constr).val;_LL2D7: return cmp(_tmp57C,_tmp583);_LL2D8: _tmp584=*_tmp57F;
if((_tmp584.Forward_constr).tag != 2)goto _LL2D3;_tmp585=(union Cyc_Absyn_Constraint*)(
_tmp584.Forward_constr).val;_LL2D9: {const char*_tmp11F2;void*_tmp11F1;(_tmp11F1=
0,Cyc_Tcutil_impos(((_tmp11F2="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11F2,sizeof(char),40))),_tag_dyneither(_tmp11F1,sizeof(void*),
0)));}_LL2D3:;}_LL2D1: _tmp57D=*_tmp578;if((_tmp57D.Forward_constr).tag != 2)goto
_LL2CC;_tmp57E=(union Cyc_Absyn_Constraint*)(_tmp57D.Forward_constr).val;_LL2D2: {
const char*_tmp11F5;void*_tmp11F4;(_tmp11F4=0,Cyc_Tcutil_impos(((_tmp11F5="unify_conref: forward after compress",
_tag_dyneither(_tmp11F5,sizeof(char),37))),_tag_dyneither(_tmp11F4,sizeof(void*),
0)));}_LL2CC:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp58B;struct Cyc_Absyn_Tqual _tmp58C;void*_tmp58D;
struct _tuple11*_tmp58A=tqt1;_tmp58B=*_tmp58A;_tmp58C=_tmp58B.f1;_tmp58D=_tmp58B.f2;{
struct _tuple11 _tmp58F;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;struct _tuple11*
_tmp58E=tqt2;_tmp58F=*_tmp58E;_tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;{int _tmp592=
Cyc_Tcutil_tqual_cmp(_tmp58C,_tmp590);if(_tmp592 != 0)return _tmp592;return Cyc_Tcutil_typecmp(
_tmp58D,_tmp591);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp593=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp593 != 0)return _tmp593;{int _tmp594=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp594 != 0)return _tmp594;{int _tmp595=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp595 != 0)return _tmp595;{int _tmp596=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp596 != 0)return _tmp596;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp597=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp597 != 0)return _tmp597;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp598=t;_LL2DB: {
struct Cyc_Absyn_VoidType_struct*_tmp599=(struct Cyc_Absyn_VoidType_struct*)
_tmp598;if(_tmp599->tag != 0)goto _LL2DD;}_LL2DC: return 0;_LL2DD: {struct Cyc_Absyn_Evar_struct*
_tmp59A=(struct Cyc_Absyn_Evar_struct*)_tmp598;if(_tmp59A->tag != 1)goto _LL2DF;}
_LL2DE: return 1;_LL2DF: {struct Cyc_Absyn_VarType_struct*_tmp59B=(struct Cyc_Absyn_VarType_struct*)
_tmp598;if(_tmp59B->tag != 2)goto _LL2E1;}_LL2E0: return 2;_LL2E1: {struct Cyc_Absyn_DatatypeType_struct*
_tmp59C=(struct Cyc_Absyn_DatatypeType_struct*)_tmp598;if(_tmp59C->tag != 3)goto
_LL2E3;}_LL2E2: return 3;_LL2E3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp59D=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp598;if(_tmp59D->tag != 4)goto _LL2E5;}
_LL2E4: return 4;_LL2E5: {struct Cyc_Absyn_PointerType_struct*_tmp59E=(struct Cyc_Absyn_PointerType_struct*)
_tmp598;if(_tmp59E->tag != 5)goto _LL2E7;}_LL2E6: return 5;_LL2E7: {struct Cyc_Absyn_IntType_struct*
_tmp59F=(struct Cyc_Absyn_IntType_struct*)_tmp598;if(_tmp59F->tag != 6)goto _LL2E9;}
_LL2E8: return 6;_LL2E9: {struct Cyc_Absyn_FloatType_struct*_tmp5A0=(struct Cyc_Absyn_FloatType_struct*)
_tmp598;if(_tmp5A0->tag != 7)goto _LL2EB;}_LL2EA: return 7;_LL2EB: {struct Cyc_Absyn_DoubleType_struct*
_tmp5A1=(struct Cyc_Absyn_DoubleType_struct*)_tmp598;if(_tmp5A1->tag != 8)goto
_LL2ED;}_LL2EC: return 8;_LL2ED: {struct Cyc_Absyn_ArrayType_struct*_tmp5A2=(struct
Cyc_Absyn_ArrayType_struct*)_tmp598;if(_tmp5A2->tag != 9)goto _LL2EF;}_LL2EE:
return 9;_LL2EF: {struct Cyc_Absyn_FnType_struct*_tmp5A3=(struct Cyc_Absyn_FnType_struct*)
_tmp598;if(_tmp5A3->tag != 10)goto _LL2F1;}_LL2F0: return 10;_LL2F1: {struct Cyc_Absyn_TupleType_struct*
_tmp5A4=(struct Cyc_Absyn_TupleType_struct*)_tmp598;if(_tmp5A4->tag != 11)goto
_LL2F3;}_LL2F2: return 11;_LL2F3: {struct Cyc_Absyn_AggrType_struct*_tmp5A5=(struct
Cyc_Absyn_AggrType_struct*)_tmp598;if(_tmp5A5->tag != 12)goto _LL2F5;}_LL2F4:
return 12;_LL2F5: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5A6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp598;if(_tmp5A6->tag != 13)goto _LL2F7;}_LL2F6: return 14;_LL2F7: {struct Cyc_Absyn_EnumType_struct*
_tmp5A7=(struct Cyc_Absyn_EnumType_struct*)_tmp598;if(_tmp5A7->tag != 14)goto
_LL2F9;}_LL2F8: return 16;_LL2F9: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5A8=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp598;if(_tmp5A8->tag != 15)goto _LL2FB;}
_LL2FA: return 17;_LL2FB: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5A9=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp598;if(_tmp5A9->tag != 16)goto _LL2FD;}_LL2FC: return 18;_LL2FD: {struct Cyc_Absyn_TypedefType_struct*
_tmp5AA=(struct Cyc_Absyn_TypedefType_struct*)_tmp598;if(_tmp5AA->tag != 18)goto
_LL2FF;}_LL2FE: return 19;_LL2FF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp5AB=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp598;if(_tmp5AB->tag != 22)goto _LL301;}
_LL300: return 20;_LL301: {struct Cyc_Absyn_HeapRgn_struct*_tmp5AC=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp598;if(_tmp5AC->tag != 21)goto _LL303;}_LL302: return 21;_LL303: {struct Cyc_Absyn_AccessEff_struct*
_tmp5AD=(struct Cyc_Absyn_AccessEff_struct*)_tmp598;if(_tmp5AD->tag != 23)goto
_LL305;}_LL304: return 22;_LL305: {struct Cyc_Absyn_JoinEff_struct*_tmp5AE=(struct
Cyc_Absyn_JoinEff_struct*)_tmp598;if(_tmp5AE->tag != 24)goto _LL307;}_LL306: return
23;_LL307: {struct Cyc_Absyn_RgnsEff_struct*_tmp5AF=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp598;if(_tmp5AF->tag != 25)goto _LL309;}_LL308: return 24;_LL309: {struct Cyc_Absyn_TagType_struct*
_tmp5B0=(struct Cyc_Absyn_TagType_struct*)_tmp598;if(_tmp5B0->tag != 20)goto _LL30B;}
_LL30A: return 26;_LL30B: {struct Cyc_Absyn_DynRgnType_struct*_tmp5B1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp598;if(_tmp5B1->tag != 17)goto _LL30D;}_LL30C: return 27;_LL30D: {struct Cyc_Absyn_ValueofType_struct*
_tmp5B2=(struct Cyc_Absyn_ValueofType_struct*)_tmp598;if(_tmp5B2->tag != 19)goto
_LL2DA;}_LL30E: return 28;_LL2DA:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp5B3=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp5B3 != 0)return _tmp5B3;{struct _tuple0 _tmp11F6;struct _tuple0 _tmp5B5=(
_tmp11F6.f1=t2,((_tmp11F6.f2=t1,_tmp11F6)));void*_tmp5B6;void*_tmp5B8;void*
_tmp5BA;struct Cyc_Absyn_Tvar*_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_Tvar*_tmp5BF;
void*_tmp5C0;struct Cyc_Absyn_AggrInfo _tmp5C2;union Cyc_Absyn_AggrInfoU _tmp5C3;
struct Cyc_List_List*_tmp5C4;void*_tmp5C5;struct Cyc_Absyn_AggrInfo _tmp5C7;union
Cyc_Absyn_AggrInfoU _tmp5C8;struct Cyc_List_List*_tmp5C9;void*_tmp5CA;struct
_tuple2*_tmp5CC;void*_tmp5CD;struct _tuple2*_tmp5CF;void*_tmp5D0;struct Cyc_List_List*
_tmp5D2;void*_tmp5D3;struct Cyc_List_List*_tmp5D5;void*_tmp5D6;struct Cyc_Absyn_DatatypeInfo
_tmp5D8;union Cyc_Absyn_DatatypeInfoU _tmp5D9;struct Cyc_Absyn_Datatypedecl**
_tmp5DA;struct Cyc_Absyn_Datatypedecl*_tmp5DB;struct Cyc_List_List*_tmp5DC;void*
_tmp5DD;struct Cyc_Absyn_DatatypeInfo _tmp5DF;union Cyc_Absyn_DatatypeInfoU _tmp5E0;
struct Cyc_Absyn_Datatypedecl**_tmp5E1;struct Cyc_Absyn_Datatypedecl*_tmp5E2;
struct Cyc_List_List*_tmp5E3;void*_tmp5E4;struct Cyc_Absyn_DatatypeFieldInfo
_tmp5E6;union Cyc_Absyn_DatatypeFieldInfoU _tmp5E7;struct _tuple3 _tmp5E8;struct Cyc_Absyn_Datatypedecl*
_tmp5E9;struct Cyc_Absyn_Datatypefield*_tmp5EA;struct Cyc_List_List*_tmp5EB;void*
_tmp5EC;struct Cyc_Absyn_DatatypeFieldInfo _tmp5EE;union Cyc_Absyn_DatatypeFieldInfoU
_tmp5EF;struct _tuple3 _tmp5F0;struct Cyc_Absyn_Datatypedecl*_tmp5F1;struct Cyc_Absyn_Datatypefield*
_tmp5F2;struct Cyc_List_List*_tmp5F3;void*_tmp5F4;struct Cyc_Absyn_PtrInfo _tmp5F6;
void*_tmp5F7;struct Cyc_Absyn_Tqual _tmp5F8;struct Cyc_Absyn_PtrAtts _tmp5F9;void*
_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;
union Cyc_Absyn_Constraint*_tmp5FD;void*_tmp5FE;struct Cyc_Absyn_PtrInfo _tmp600;
void*_tmp601;struct Cyc_Absyn_Tqual _tmp602;struct Cyc_Absyn_PtrAtts _tmp603;void*
_tmp604;union Cyc_Absyn_Constraint*_tmp605;union Cyc_Absyn_Constraint*_tmp606;
union Cyc_Absyn_Constraint*_tmp607;void*_tmp608;enum Cyc_Absyn_Sign _tmp60A;enum 
Cyc_Absyn_Size_of _tmp60B;void*_tmp60C;enum Cyc_Absyn_Sign _tmp60E;enum Cyc_Absyn_Size_of
_tmp60F;void*_tmp610;void*_tmp612;void*_tmp614;int _tmp616;void*_tmp617;int
_tmp619;void*_tmp61A;struct Cyc_Absyn_ArrayInfo _tmp61C;void*_tmp61D;struct Cyc_Absyn_Tqual
_tmp61E;struct Cyc_Absyn_Exp*_tmp61F;union Cyc_Absyn_Constraint*_tmp620;void*
_tmp621;struct Cyc_Absyn_ArrayInfo _tmp623;void*_tmp624;struct Cyc_Absyn_Tqual
_tmp625;struct Cyc_Absyn_Exp*_tmp626;union Cyc_Absyn_Constraint*_tmp627;void*
_tmp628;struct Cyc_Absyn_FnInfo _tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_Core_Opt*
_tmp62C;void*_tmp62D;struct Cyc_List_List*_tmp62E;int _tmp62F;struct Cyc_Absyn_VarargInfo*
_tmp630;struct Cyc_List_List*_tmp631;struct Cyc_List_List*_tmp632;void*_tmp633;
struct Cyc_Absyn_FnInfo _tmp635;struct Cyc_List_List*_tmp636;struct Cyc_Core_Opt*
_tmp637;void*_tmp638;struct Cyc_List_List*_tmp639;int _tmp63A;struct Cyc_Absyn_VarargInfo*
_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_List_List*_tmp63D;void*_tmp63E;
struct Cyc_List_List*_tmp640;void*_tmp641;struct Cyc_List_List*_tmp643;void*
_tmp644;enum Cyc_Absyn_AggrKind _tmp646;struct Cyc_List_List*_tmp647;void*_tmp648;
enum Cyc_Absyn_AggrKind _tmp64A;struct Cyc_List_List*_tmp64B;void*_tmp64C;void*
_tmp64E;void*_tmp64F;void*_tmp651;void*_tmp652;void*_tmp654;void*_tmp655;void*
_tmp656;void*_tmp658;void*_tmp659;void*_tmp65A;void*_tmp65C;void*_tmp65D;void*
_tmp65F;void*_tmp660;struct Cyc_Absyn_Exp*_tmp662;void*_tmp663;struct Cyc_Absyn_Exp*
_tmp665;void*_tmp666;void*_tmp668;void*_tmp66A;void*_tmp66C;void*_tmp66E;void*
_tmp670;_LL310: _tmp5B6=_tmp5B5.f1;{struct Cyc_Absyn_Evar_struct*_tmp5B7=(struct
Cyc_Absyn_Evar_struct*)_tmp5B6;if(_tmp5B7->tag != 1)goto _LL312;};_tmp5B8=_tmp5B5.f2;{
struct Cyc_Absyn_Evar_struct*_tmp5B9=(struct Cyc_Absyn_Evar_struct*)_tmp5B8;if(
_tmp5B9->tag != 1)goto _LL312;};_LL311: {const char*_tmp11F9;void*_tmp11F8;(
_tmp11F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11F9="typecmp: can only compare closed types",_tag_dyneither(_tmp11F9,
sizeof(char),39))),_tag_dyneither(_tmp11F8,sizeof(void*),0)));}_LL312: _tmp5BA=
_tmp5B5.f1;{struct Cyc_Absyn_VarType_struct*_tmp5BB=(struct Cyc_Absyn_VarType_struct*)
_tmp5BA;if(_tmp5BB->tag != 2)goto _LL314;else{_tmp5BC=_tmp5BB->f1;}};_tmp5BD=
_tmp5B5.f2;{struct Cyc_Absyn_VarType_struct*_tmp5BE=(struct Cyc_Absyn_VarType_struct*)
_tmp5BD;if(_tmp5BE->tag != 2)goto _LL314;else{_tmp5BF=_tmp5BE->f1;}};_LL313: return
Cyc_Core_intcmp(_tmp5BF->identity,_tmp5BC->identity);_LL314: _tmp5C0=_tmp5B5.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp5C1=(struct Cyc_Absyn_AggrType_struct*)
_tmp5C0;if(_tmp5C1->tag != 12)goto _LL316;else{_tmp5C2=_tmp5C1->f1;_tmp5C3=_tmp5C2.aggr_info;
_tmp5C4=_tmp5C2.targs;}};_tmp5C5=_tmp5B5.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp5C6=(struct Cyc_Absyn_AggrType_struct*)_tmp5C5;if(_tmp5C6->tag != 12)goto
_LL316;else{_tmp5C7=_tmp5C6->f1;_tmp5C8=_tmp5C7.aggr_info;_tmp5C9=_tmp5C7.targs;}};
_LL315: {struct _tuple2*_tmp675;struct _tuple12 _tmp674=Cyc_Absyn_aggr_kinded_name(
_tmp5C3);_tmp675=_tmp674.f2;{struct _tuple2*_tmp677;struct _tuple12 _tmp676=Cyc_Absyn_aggr_kinded_name(
_tmp5C8);_tmp677=_tmp676.f2;{int _tmp678=Cyc_Absyn_qvar_cmp(_tmp675,_tmp677);if(
_tmp678 != 0)return _tmp678;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5C4,_tmp5C9);}};};}
_LL316: _tmp5CA=_tmp5B5.f1;{struct Cyc_Absyn_EnumType_struct*_tmp5CB=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CA;if(_tmp5CB->tag != 14)goto _LL318;else{_tmp5CC=_tmp5CB->f1;}};_tmp5CD=
_tmp5B5.f2;{struct Cyc_Absyn_EnumType_struct*_tmp5CE=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CD;if(_tmp5CE->tag != 14)goto _LL318;else{_tmp5CF=_tmp5CE->f1;}};_LL317:
return Cyc_Absyn_qvar_cmp(_tmp5CC,_tmp5CF);_LL318: _tmp5D0=_tmp5B5.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D0;if(_tmp5D1->tag != 15)goto
_LL31A;else{_tmp5D2=_tmp5D1->f1;}};_tmp5D3=_tmp5B5.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D3;if(_tmp5D4->tag != 15)goto
_LL31A;else{_tmp5D5=_tmp5D4->f1;}};_LL319: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp5D2,_tmp5D5);_LL31A: _tmp5D6=_tmp5B5.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5D7=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5D6;if(_tmp5D7->tag != 3)goto
_LL31C;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D8.datatype_info;if((_tmp5D9.KnownDatatype).tag
!= 2)goto _LL31C;_tmp5DA=(struct Cyc_Absyn_Datatypedecl**)(_tmp5D9.KnownDatatype).val;
_tmp5DB=*_tmp5DA;_tmp5DC=_tmp5D8.targs;}};_tmp5DD=_tmp5B5.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5DE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5DD;if(_tmp5DE->tag != 3)goto
_LL31C;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DF.datatype_info;if((_tmp5E0.KnownDatatype).tag
!= 2)goto _LL31C;_tmp5E1=(struct Cyc_Absyn_Datatypedecl**)(_tmp5E0.KnownDatatype).val;
_tmp5E2=*_tmp5E1;_tmp5E3=_tmp5DF.targs;}};_LL31B: if(_tmp5E2 == _tmp5DB)return 0;{
int _tmp679=Cyc_Absyn_qvar_cmp(_tmp5E2->name,_tmp5DB->name);if(_tmp679 != 0)return
_tmp679;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5E3,_tmp5DC);};_LL31C: _tmp5E4=
_tmp5B5.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp5E5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp5E4;if(_tmp5E5->tag != 4)goto _LL31E;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E6.field_info;
if((_tmp5E7.KnownDatatypefield).tag != 2)goto _LL31E;_tmp5E8=(struct _tuple3)(
_tmp5E7.KnownDatatypefield).val;_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;_tmp5EB=
_tmp5E6.targs;}};_tmp5EC=_tmp5B5.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5ED=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5EC;if(_tmp5ED->tag != 4)
goto _LL31E;else{_tmp5EE=_tmp5ED->f1;_tmp5EF=_tmp5EE.field_info;if((_tmp5EF.KnownDatatypefield).tag
!= 2)goto _LL31E;_tmp5F0=(struct _tuple3)(_tmp5EF.KnownDatatypefield).val;_tmp5F1=
_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;_tmp5F3=_tmp5EE.targs;}};_LL31D: if(_tmp5F1 == 
_tmp5E9)return 0;{int _tmp67A=Cyc_Absyn_qvar_cmp(_tmp5E9->name,_tmp5F1->name);if(
_tmp67A != 0)return _tmp67A;{int _tmp67B=Cyc_Absyn_qvar_cmp(_tmp5EA->name,_tmp5F2->name);
if(_tmp67B != 0)return _tmp67B;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5F3,_tmp5EB);};};
_LL31E: _tmp5F4=_tmp5B5.f1;{struct Cyc_Absyn_PointerType_struct*_tmp5F5=(struct Cyc_Absyn_PointerType_struct*)
_tmp5F4;if(_tmp5F5->tag != 5)goto _LL320;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F6.elt_typ;
_tmp5F8=_tmp5F6.elt_tq;_tmp5F9=_tmp5F6.ptr_atts;_tmp5FA=_tmp5F9.rgn;_tmp5FB=
_tmp5F9.nullable;_tmp5FC=_tmp5F9.bounds;_tmp5FD=_tmp5F9.zero_term;}};_tmp5FE=
_tmp5B5.f2;{struct Cyc_Absyn_PointerType_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_struct*)
_tmp5FE;if(_tmp5FF->tag != 5)goto _LL320;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp600.elt_typ;
_tmp602=_tmp600.elt_tq;_tmp603=_tmp600.ptr_atts;_tmp604=_tmp603.rgn;_tmp605=
_tmp603.nullable;_tmp606=_tmp603.bounds;_tmp607=_tmp603.zero_term;}};_LL31F: {
int _tmp67C=Cyc_Tcutil_typecmp(_tmp601,_tmp5F7);if(_tmp67C != 0)return _tmp67C;{int
_tmp67D=Cyc_Tcutil_typecmp(_tmp604,_tmp5FA);if(_tmp67D != 0)return _tmp67D;{int
_tmp67E=Cyc_Tcutil_tqual_cmp(_tmp602,_tmp5F8);if(_tmp67E != 0)return _tmp67E;{int
_tmp67F=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp606,_tmp5FC);if(_tmp67F != 0)
return _tmp67F;{int _tmp680=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp607,
_tmp5FD);if(_tmp680 != 0)return _tmp680;{void*_tmp681=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp606);_LL345: {struct Cyc_Absyn_DynEither_b_struct*
_tmp682=(struct Cyc_Absyn_DynEither_b_struct*)_tmp681;if(_tmp682->tag != 0)goto
_LL347;}_LL346: return 0;_LL347:;_LL348: goto _LL344;_LL344:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp605,_tmp5FB);};};};};}_LL320: _tmp608=_tmp5B5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp609=(struct Cyc_Absyn_IntType_struct*)_tmp608;if(_tmp609->tag != 6)goto _LL322;
else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;}};_tmp60C=_tmp5B5.f2;{struct Cyc_Absyn_IntType_struct*
_tmp60D=(struct Cyc_Absyn_IntType_struct*)_tmp60C;if(_tmp60D->tag != 6)goto _LL322;
else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60D->f2;}};_LL321: if(_tmp60E != _tmp60A)
return Cyc_Core_intcmp((int)((unsigned int)_tmp60E),(int)((unsigned int)_tmp60A));
if(_tmp60F != _tmp60B)return Cyc_Core_intcmp((int)((unsigned int)_tmp60F),(int)((
unsigned int)_tmp60B));return 0;_LL322: _tmp610=_tmp5B5.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp611=(struct Cyc_Absyn_FloatType_struct*)_tmp610;if(_tmp611->tag != 7)goto
_LL324;};_tmp612=_tmp5B5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp613=(struct Cyc_Absyn_FloatType_struct*)
_tmp612;if(_tmp613->tag != 7)goto _LL324;};_LL323: return 0;_LL324: _tmp614=_tmp5B5.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp615=(struct Cyc_Absyn_DoubleType_struct*)
_tmp614;if(_tmp615->tag != 8)goto _LL326;else{_tmp616=_tmp615->f1;}};_tmp617=
_tmp5B5.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp618=(struct Cyc_Absyn_DoubleType_struct*)
_tmp617;if(_tmp618->tag != 8)goto _LL326;else{_tmp619=_tmp618->f1;}};_LL325: if(
_tmp616 == _tmp619)return 0;else{if(_tmp616)return - 1;else{return 1;}}_LL326: _tmp61A=
_tmp5B5.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp61B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp61A;if(_tmp61B->tag != 9)goto _LL328;else{_tmp61C=_tmp61B->f1;_tmp61D=_tmp61C.elt_type;
_tmp61E=_tmp61C.tq;_tmp61F=_tmp61C.num_elts;_tmp620=_tmp61C.zero_term;}};_tmp621=
_tmp5B5.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp622=(struct Cyc_Absyn_ArrayType_struct*)
_tmp621;if(_tmp622->tag != 9)goto _LL328;else{_tmp623=_tmp622->f1;_tmp624=_tmp623.elt_type;
_tmp625=_tmp623.tq;_tmp626=_tmp623.num_elts;_tmp627=_tmp623.zero_term;}};_LL327: {
int _tmp683=Cyc_Tcutil_tqual_cmp(_tmp625,_tmp61E);if(_tmp683 != 0)return _tmp683;{
int _tmp684=Cyc_Tcutil_typecmp(_tmp624,_tmp61D);if(_tmp684 != 0)return _tmp684;{int
_tmp685=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp620,_tmp627);if(_tmp685 != 0)
return _tmp685;if(_tmp61F == _tmp626)return 0;if(_tmp61F == 0  || _tmp626 == 0){const
char*_tmp11FC;void*_tmp11FB;(_tmp11FB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11FC="missing expression in array index",
_tag_dyneither(_tmp11FC,sizeof(char),34))),_tag_dyneither(_tmp11FB,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp61F,_tmp626);};};}_LL328: _tmp628=_tmp5B5.f1;{struct Cyc_Absyn_FnType_struct*
_tmp629=(struct Cyc_Absyn_FnType_struct*)_tmp628;if(_tmp629->tag != 10)goto _LL32A;
else{_tmp62A=_tmp629->f1;_tmp62B=_tmp62A.tvars;_tmp62C=_tmp62A.effect;_tmp62D=
_tmp62A.ret_typ;_tmp62E=_tmp62A.args;_tmp62F=_tmp62A.c_varargs;_tmp630=_tmp62A.cyc_varargs;
_tmp631=_tmp62A.rgn_po;_tmp632=_tmp62A.attributes;}};_tmp633=_tmp5B5.f2;{struct
Cyc_Absyn_FnType_struct*_tmp634=(struct Cyc_Absyn_FnType_struct*)_tmp633;if(
_tmp634->tag != 10)goto _LL32A;else{_tmp635=_tmp634->f1;_tmp636=_tmp635.tvars;
_tmp637=_tmp635.effect;_tmp638=_tmp635.ret_typ;_tmp639=_tmp635.args;_tmp63A=
_tmp635.c_varargs;_tmp63B=_tmp635.cyc_varargs;_tmp63C=_tmp635.rgn_po;_tmp63D=
_tmp635.attributes;}};_LL329: {const char*_tmp11FF;void*_tmp11FE;(_tmp11FE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11FF="typecmp: function types not handled",_tag_dyneither(_tmp11FF,sizeof(
char),36))),_tag_dyneither(_tmp11FE,sizeof(void*),0)));}_LL32A: _tmp63E=_tmp5B5.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp63F=(struct Cyc_Absyn_TupleType_struct*)
_tmp63E;if(_tmp63F->tag != 11)goto _LL32C;else{_tmp640=_tmp63F->f1;}};_tmp641=
_tmp5B5.f2;{struct Cyc_Absyn_TupleType_struct*_tmp642=(struct Cyc_Absyn_TupleType_struct*)
_tmp641;if(_tmp642->tag != 11)goto _LL32C;else{_tmp643=_tmp642->f1;}};_LL32B:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp643,
_tmp640);_LL32C: _tmp644=_tmp5B5.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp645=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp644;if(_tmp645->tag != 13)goto _LL32E;
else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;}};_tmp648=_tmp5B5.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp649=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp648;if(_tmp649->tag != 13)goto
_LL32E;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;}};_LL32D: if(_tmp64A != 
_tmp646){if(_tmp64A == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp64B,
_tmp647);_LL32E: _tmp64C=_tmp5B5.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp64D=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp64C;if(_tmp64D->tag != 16)goto _LL330;
else{_tmp64E=(void*)_tmp64D->f1;}};_tmp64F=_tmp5B5.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp650=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp64F;if(_tmp650->tag != 16)goto
_LL330;else{_tmp651=(void*)_tmp650->f1;}};_LL32F: return Cyc_Tcutil_typecmp(
_tmp64E,_tmp651);_LL330: _tmp652=_tmp5B5.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp653=(struct Cyc_Absyn_DynRgnType_struct*)_tmp652;if(_tmp653->tag != 17)goto
_LL332;else{_tmp654=(void*)_tmp653->f1;_tmp655=(void*)_tmp653->f2;}};_tmp656=
_tmp5B5.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp657=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp656;if(_tmp657->tag != 17)goto _LL332;else{_tmp658=(void*)_tmp657->f1;_tmp659=(
void*)_tmp657->f2;}};_LL331: {int _tmp68A=Cyc_Tcutil_typecmp(_tmp654,_tmp658);if(
_tmp68A != 0)return _tmp68A;else{return Cyc_Tcutil_typecmp(_tmp655,_tmp659);}}
_LL332: _tmp65A=_tmp5B5.f1;{struct Cyc_Absyn_TagType_struct*_tmp65B=(struct Cyc_Absyn_TagType_struct*)
_tmp65A;if(_tmp65B->tag != 20)goto _LL334;else{_tmp65C=(void*)_tmp65B->f1;}};
_tmp65D=_tmp5B5.f2;{struct Cyc_Absyn_TagType_struct*_tmp65E=(struct Cyc_Absyn_TagType_struct*)
_tmp65D;if(_tmp65E->tag != 20)goto _LL334;else{_tmp65F=(void*)_tmp65E->f1;}};
_LL333: return Cyc_Tcutil_typecmp(_tmp65C,_tmp65F);_LL334: _tmp660=_tmp5B5.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp661=(struct Cyc_Absyn_ValueofType_struct*)
_tmp660;if(_tmp661->tag != 19)goto _LL336;else{_tmp662=_tmp661->f1;}};_tmp663=
_tmp5B5.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp664=(struct Cyc_Absyn_ValueofType_struct*)
_tmp663;if(_tmp664->tag != 19)goto _LL336;else{_tmp665=_tmp664->f1;}};_LL335:
return Cyc_Evexp_const_exp_cmp(_tmp662,_tmp665);_LL336: _tmp666=_tmp5B5.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp667=(struct Cyc_Absyn_JoinEff_struct*)_tmp666;if(
_tmp667->tag != 24)goto _LL338;};_LL337: goto _LL339;_LL338: _tmp668=_tmp5B5.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp669=(struct Cyc_Absyn_JoinEff_struct*)_tmp668;
if(_tmp669->tag != 24)goto _LL33A;};_LL339: goto _LL33B;_LL33A: _tmp66A=_tmp5B5.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp66B=(struct Cyc_Absyn_AccessEff_struct*)
_tmp66A;if(_tmp66B->tag != 23)goto _LL33C;};_LL33B: goto _LL33D;_LL33C: _tmp66C=
_tmp5B5.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp66D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp66C;if(_tmp66D->tag != 25)goto _LL33E;};_LL33D: goto _LL33F;_LL33E: _tmp66E=
_tmp5B5.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp66F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp66E;if(_tmp66F->tag != 25)goto _LL340;};_LL33F: goto _LL341;_LL340: _tmp670=
_tmp5B5.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp671=(struct Cyc_Absyn_AccessEff_struct*)
_tmp670;if(_tmp671->tag != 23)goto _LL342;};_LL341: {const char*_tmp1202;void*
_tmp1201;(_tmp1201=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1202="typecmp: effects not handled",_tag_dyneither(
_tmp1202,sizeof(char),29))),_tag_dyneither(_tmp1201,sizeof(void*),0)));}_LL342:;
_LL343: {const char*_tmp1205;void*_tmp1204;(_tmp1204=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1205="Unmatched case in typecmp",
_tag_dyneither(_tmp1205,sizeof(char),26))),_tag_dyneither(_tmp1204,sizeof(void*),
0)));}_LL30F:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp68F=Cyc_Tcutil_compress(t);_LL34A: {struct Cyc_Absyn_IntType_struct*
_tmp690=(struct Cyc_Absyn_IntType_struct*)_tmp68F;if(_tmp690->tag != 6)goto _LL34C;}
_LL34B: goto _LL34D;_LL34C: {struct Cyc_Absyn_FloatType_struct*_tmp691=(struct Cyc_Absyn_FloatType_struct*)
_tmp68F;if(_tmp691->tag != 7)goto _LL34E;}_LL34D: goto _LL34F;_LL34E: {struct Cyc_Absyn_DoubleType_struct*
_tmp692=(struct Cyc_Absyn_DoubleType_struct*)_tmp68F;if(_tmp692->tag != 8)goto
_LL350;}_LL34F: goto _LL351;_LL350: {struct Cyc_Absyn_EnumType_struct*_tmp693=(
struct Cyc_Absyn_EnumType_struct*)_tmp68F;if(_tmp693->tag != 14)goto _LL352;}_LL351:
goto _LL353;_LL352: {struct Cyc_Absyn_AnonEnumType_struct*_tmp694=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp68F;if(_tmp694->tag != 15)goto _LL354;}_LL353: return 1;_LL354:;_LL355: return 0;
_LL349:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1206;struct _tuple0 _tmp696=(_tmp1206.f1=t1,((_tmp1206.f2=t2,_tmp1206)));
void*_tmp697;int _tmp699;void*_tmp69A;int _tmp69C;void*_tmp69D;void*_tmp69F;void*
_tmp6A1;void*_tmp6A3;void*_tmp6A5;void*_tmp6A7;void*_tmp6A9;void*_tmp6AB;void*
_tmp6AD;void*_tmp6AF;void*_tmp6B1;enum Cyc_Absyn_Size_of _tmp6B3;void*_tmp6B4;
enum Cyc_Absyn_Size_of _tmp6B6;void*_tmp6B7;enum Cyc_Absyn_Size_of _tmp6B9;void*
_tmp6BA;enum Cyc_Absyn_Size_of _tmp6BC;void*_tmp6BD;enum Cyc_Absyn_Size_of _tmp6BF;
void*_tmp6C0;enum Cyc_Absyn_Size_of _tmp6C2;void*_tmp6C3;enum Cyc_Absyn_Size_of
_tmp6C5;void*_tmp6C6;enum Cyc_Absyn_Size_of _tmp6C8;void*_tmp6C9;void*_tmp6CB;
enum Cyc_Absyn_Size_of _tmp6CD;void*_tmp6CE;void*_tmp6D0;enum Cyc_Absyn_Size_of
_tmp6D2;void*_tmp6D3;enum Cyc_Absyn_Size_of _tmp6D5;void*_tmp6D6;enum Cyc_Absyn_Size_of
_tmp6D8;void*_tmp6D9;enum Cyc_Absyn_Size_of _tmp6DB;void*_tmp6DC;void*_tmp6DE;
enum Cyc_Absyn_Size_of _tmp6E0;void*_tmp6E1;enum Cyc_Absyn_Size_of _tmp6E3;void*
_tmp6E4;enum Cyc_Absyn_Size_of _tmp6E6;void*_tmp6E7;enum Cyc_Absyn_Size_of _tmp6E9;
void*_tmp6EA;enum Cyc_Absyn_Size_of _tmp6EC;void*_tmp6ED;enum Cyc_Absyn_Size_of
_tmp6EF;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;void*_tmp6F5;
enum Cyc_Absyn_Size_of _tmp6F7;_LL357: _tmp697=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp698=(struct Cyc_Absyn_DoubleType_struct*)_tmp697;if(_tmp698->tag != 8)goto
_LL359;else{_tmp699=_tmp698->f1;}};_tmp69A=_tmp696.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp69B=(struct Cyc_Absyn_DoubleType_struct*)_tmp69A;if(_tmp69B->tag != 8)goto
_LL359;else{_tmp69C=_tmp69B->f1;}};_LL358: return !_tmp69C  && _tmp699;_LL359:
_tmp69D=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp69E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp69D;if(_tmp69E->tag != 8)goto _LL35B;};_tmp69F=_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6A0=(struct Cyc_Absyn_FloatType_struct*)_tmp69F;if(_tmp6A0->tag != 7)goto
_LL35B;};_LL35A: goto _LL35C;_LL35B: _tmp6A1=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6A2=(struct Cyc_Absyn_DoubleType_struct*)_tmp6A1;if(_tmp6A2->tag != 8)goto
_LL35D;};_tmp6A3=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6A4=(struct Cyc_Absyn_IntType_struct*)
_tmp6A3;if(_tmp6A4->tag != 6)goto _LL35D;};_LL35C: goto _LL35E;_LL35D: _tmp6A5=
_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6A6=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6A5;if(_tmp6A6->tag != 8)goto _LL35F;};_tmp6A7=_tmp696.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6A8=(struct Cyc_Absyn_TagType_struct*)_tmp6A7;if(_tmp6A8->tag != 20)goto _LL35F;};
_LL35E: goto _LL360;_LL35F: _tmp6A9=_tmp696.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6AA=(struct Cyc_Absyn_FloatType_struct*)_tmp6A9;if(_tmp6AA->tag != 7)goto
_LL361;};_tmp6AB=_tmp696.f2;{struct Cyc_Absyn_TagType_struct*_tmp6AC=(struct Cyc_Absyn_TagType_struct*)
_tmp6AB;if(_tmp6AC->tag != 20)goto _LL361;};_LL360: goto _LL362;_LL361: _tmp6AD=
_tmp696.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6AE=(struct Cyc_Absyn_FloatType_struct*)
_tmp6AD;if(_tmp6AE->tag != 7)goto _LL363;};_tmp6AF=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6B0=(struct Cyc_Absyn_IntType_struct*)_tmp6AF;if(_tmp6B0->tag != 6)goto _LL363;};
_LL362: return 1;_LL363: _tmp6B1=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6B2=(
struct Cyc_Absyn_IntType_struct*)_tmp6B1;if(_tmp6B2->tag != 6)goto _LL365;else{
_tmp6B3=_tmp6B2->f2;if(_tmp6B3 != Cyc_Absyn_LongLong_sz)goto _LL365;}};_tmp6B4=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6B5=(struct Cyc_Absyn_IntType_struct*)
_tmp6B4;if(_tmp6B5->tag != 6)goto _LL365;else{_tmp6B6=_tmp6B5->f2;if(_tmp6B6 != Cyc_Absyn_LongLong_sz)
goto _LL365;}};_LL364: return 0;_LL365: _tmp6B7=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6B8=(struct Cyc_Absyn_IntType_struct*)_tmp6B7;if(_tmp6B8->tag != 6)goto _LL367;
else{_tmp6B9=_tmp6B8->f2;if(_tmp6B9 != Cyc_Absyn_LongLong_sz)goto _LL367;}};_LL366:
return 1;_LL367: _tmp6BA=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6BB=(
struct Cyc_Absyn_IntType_struct*)_tmp6BA;if(_tmp6BB->tag != 6)goto _LL369;else{
_tmp6BC=_tmp6BB->f2;if(_tmp6BC != Cyc_Absyn_Long_sz)goto _LL369;}};_tmp6BD=_tmp696.f2;{
struct Cyc_Absyn_IntType_struct*_tmp6BE=(struct Cyc_Absyn_IntType_struct*)_tmp6BD;
if(_tmp6BE->tag != 6)goto _LL369;else{_tmp6BF=_tmp6BE->f2;if(_tmp6BF != Cyc_Absyn_Int_sz)
goto _LL369;}};_LL368: goto _LL36A;_LL369: _tmp6C0=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6C1=(struct Cyc_Absyn_IntType_struct*)_tmp6C0;if(_tmp6C1->tag != 6)goto _LL36B;
else{_tmp6C2=_tmp6C1->f2;if(_tmp6C2 != Cyc_Absyn_Int_sz)goto _LL36B;}};_tmp6C3=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6C4=(struct Cyc_Absyn_IntType_struct*)
_tmp6C3;if(_tmp6C4->tag != 6)goto _LL36B;else{_tmp6C5=_tmp6C4->f2;if(_tmp6C5 != Cyc_Absyn_Long_sz)
goto _LL36B;}};_LL36A: return 0;_LL36B: _tmp6C6=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6C7=(struct Cyc_Absyn_IntType_struct*)_tmp6C6;if(_tmp6C7->tag != 6)goto _LL36D;
else{_tmp6C8=_tmp6C7->f2;if(_tmp6C8 != Cyc_Absyn_Long_sz)goto _LL36D;}};_tmp6C9=
_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6CA=(struct Cyc_Absyn_FloatType_struct*)
_tmp6C9;if(_tmp6CA->tag != 7)goto _LL36D;};_LL36C: goto _LL36E;_LL36D: _tmp6CB=
_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6CC=(struct Cyc_Absyn_IntType_struct*)
_tmp6CB;if(_tmp6CC->tag != 6)goto _LL36F;else{_tmp6CD=_tmp6CC->f2;if(_tmp6CD != Cyc_Absyn_Int_sz)
goto _LL36F;}};_tmp6CE=_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6CF=(
struct Cyc_Absyn_FloatType_struct*)_tmp6CE;if(_tmp6CF->tag != 7)goto _LL36F;};
_LL36E: goto _LL370;_LL36F: _tmp6D0=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6D1=(struct Cyc_Absyn_IntType_struct*)_tmp6D0;if(_tmp6D1->tag != 6)goto _LL371;
else{_tmp6D2=_tmp6D1->f2;if(_tmp6D2 != Cyc_Absyn_Long_sz)goto _LL371;}};_tmp6D3=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6D4=(struct Cyc_Absyn_IntType_struct*)
_tmp6D3;if(_tmp6D4->tag != 6)goto _LL371;else{_tmp6D5=_tmp6D4->f2;if(_tmp6D5 != Cyc_Absyn_Short_sz)
goto _LL371;}};_LL370: goto _LL372;_LL371: _tmp6D6=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6D7=(struct Cyc_Absyn_IntType_struct*)_tmp6D6;if(_tmp6D7->tag != 6)goto _LL373;
else{_tmp6D8=_tmp6D7->f2;if(_tmp6D8 != Cyc_Absyn_Int_sz)goto _LL373;}};_tmp6D9=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DA=(struct Cyc_Absyn_IntType_struct*)
_tmp6D9;if(_tmp6DA->tag != 6)goto _LL373;else{_tmp6DB=_tmp6DA->f2;if(_tmp6DB != Cyc_Absyn_Short_sz)
goto _LL373;}};_LL372: goto _LL374;_LL373: _tmp6DC=_tmp696.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6DD=(struct Cyc_Absyn_TagType_struct*)_tmp6DC;if(_tmp6DD->tag != 20)goto _LL375;};
_tmp6DE=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DF=(struct Cyc_Absyn_IntType_struct*)
_tmp6DE;if(_tmp6DF->tag != 6)goto _LL375;else{_tmp6E0=_tmp6DF->f2;if(_tmp6E0 != Cyc_Absyn_Short_sz)
goto _LL375;}};_LL374: goto _LL376;_LL375: _tmp6E1=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6E2=(struct Cyc_Absyn_IntType_struct*)_tmp6E1;if(_tmp6E2->tag != 6)goto _LL377;
else{_tmp6E3=_tmp6E2->f2;if(_tmp6E3 != Cyc_Absyn_Long_sz)goto _LL377;}};_tmp6E4=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6E5=(struct Cyc_Absyn_IntType_struct*)
_tmp6E4;if(_tmp6E5->tag != 6)goto _LL377;else{_tmp6E6=_tmp6E5->f2;if(_tmp6E6 != Cyc_Absyn_Char_sz)
goto _LL377;}};_LL376: goto _LL378;_LL377: _tmp6E7=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6E8=(struct Cyc_Absyn_IntType_struct*)_tmp6E7;if(_tmp6E8->tag != 6)goto _LL379;
else{_tmp6E9=_tmp6E8->f2;if(_tmp6E9 != Cyc_Absyn_Int_sz)goto _LL379;}};_tmp6EA=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6EB=(struct Cyc_Absyn_IntType_struct*)
_tmp6EA;if(_tmp6EB->tag != 6)goto _LL379;else{_tmp6EC=_tmp6EB->f2;if(_tmp6EC != Cyc_Absyn_Char_sz)
goto _LL379;}};_LL378: goto _LL37A;_LL379: _tmp6ED=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6EE=(struct Cyc_Absyn_IntType_struct*)_tmp6ED;if(_tmp6EE->tag != 6)goto _LL37B;
else{_tmp6EF=_tmp6EE->f2;if(_tmp6EF != Cyc_Absyn_Short_sz)goto _LL37B;}};_tmp6F0=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F1=(struct Cyc_Absyn_IntType_struct*)
_tmp6F0;if(_tmp6F1->tag != 6)goto _LL37B;else{_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_Char_sz)
goto _LL37B;}};_LL37A: goto _LL37C;_LL37B: _tmp6F3=_tmp696.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6F4=(struct Cyc_Absyn_TagType_struct*)_tmp6F3;if(_tmp6F4->tag != 20)goto _LL37D;};
_tmp6F5=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F6=(struct Cyc_Absyn_IntType_struct*)
_tmp6F5;if(_tmp6F6->tag != 6)goto _LL37D;else{_tmp6F7=_tmp6F6->f2;if(_tmp6F7 != Cyc_Absyn_Char_sz)
goto _LL37D;}};_LL37C: return 1;_LL37D:;_LL37E: return 0;_LL356:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp6F8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1207;max_arith_type=((_tmp1207=
_region_malloc(_tmp6F8,sizeof(*_tmp1207)),((_tmp1207->v=t1,_tmp1207))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp120C;void*_tmp120B[2];struct
Cyc_String_pa_struct _tmp120A;struct Cyc_String_pa_struct _tmp1209;(_tmp1209.tag=0,((
_tmp1209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp120A.tag=0,((_tmp120A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp120B[0]=& _tmp120A,((_tmp120B[1]=& _tmp1209,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp120C="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp120C,sizeof(char),41))),_tag_dyneither(_tmp120B,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp6FE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL380: {struct Cyc_Absyn_PointerType_struct*
_tmp6FF=(struct Cyc_Absyn_PointerType_struct*)_tmp6FE;if(_tmp6FF->tag != 5)goto
_LL382;}_LL381: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL37F;_LL382:;_LL383: return 0;_LL37F:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp700=Cyc_Tcutil_compress(
t);_LL385: {struct Cyc_Absyn_IntType_struct*_tmp701=(struct Cyc_Absyn_IntType_struct*)
_tmp700;if(_tmp701->tag != 6)goto _LL387;}_LL386: goto _LL388;_LL387: {struct Cyc_Absyn_TagType_struct*
_tmp702=(struct Cyc_Absyn_TagType_struct*)_tmp700;if(_tmp702->tag != 20)goto _LL389;}
_LL388: goto _LL38A;_LL389: {struct Cyc_Absyn_EnumType_struct*_tmp703=(struct Cyc_Absyn_EnumType_struct*)
_tmp700;if(_tmp703->tag != 14)goto _LL38B;}_LL38A: goto _LL38C;_LL38B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp704=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp700;if(_tmp704->tag != 15)goto
_LL38D;}_LL38C: return 1;_LL38D:;_LL38E: return 0;_LL384:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp120F;void*_tmp120E;(_tmp120E=0,Cyc_Tcutil_warn(e->loc,((_tmp120F="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp120F,sizeof(char),44))),_tag_dyneither(_tmp120E,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp1212;void*_tmp1211;(
_tmp1211=0,Cyc_Tcutil_warn(e->loc,((_tmp1212="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp1212,sizeof(char),44))),_tag_dyneither(_tmp1211,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1213;struct _tuple0 _tmp70A=(
_tmp1213.f1=t1,((_tmp1213.f2=t2,_tmp1213)));void*_tmp70B;struct Cyc_Absyn_PtrInfo
_tmp70D;void*_tmp70E;struct Cyc_Absyn_PtrInfo _tmp710;void*_tmp711;struct Cyc_Absyn_ArrayInfo
_tmp713;void*_tmp714;struct Cyc_Absyn_Tqual _tmp715;struct Cyc_Absyn_Exp*_tmp716;
union Cyc_Absyn_Constraint*_tmp717;void*_tmp718;struct Cyc_Absyn_ArrayInfo _tmp71A;
void*_tmp71B;struct Cyc_Absyn_Tqual _tmp71C;struct Cyc_Absyn_Exp*_tmp71D;union Cyc_Absyn_Constraint*
_tmp71E;void*_tmp71F;void*_tmp721;_LL390: _tmp70B=_tmp70A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp70C=(struct Cyc_Absyn_PointerType_struct*)_tmp70B;if(_tmp70C->tag != 5)goto
_LL392;else{_tmp70D=_tmp70C->f1;}};_tmp70E=_tmp70A.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp70F=(struct Cyc_Absyn_PointerType_struct*)_tmp70E;if(_tmp70F->tag != 5)goto
_LL392;else{_tmp710=_tmp70F->f1;}};_LL391: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp70D.ptr_atts).nullable,(_tmp710.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp70D.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp70D.ptr_atts).bounds,(
_tmp710.ptr_atts).bounds)){struct _tuple0 _tmp1214;struct _tuple0 _tmp724=(_tmp1214.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp70D.ptr_atts).bounds),((_tmp1214.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp710.ptr_atts).bounds),
_tmp1214)));void*_tmp725;void*_tmp727;void*_tmp729;void*_tmp72B;void*_tmp72D;
struct Cyc_Absyn_Exp*_tmp72F;void*_tmp730;struct Cyc_Absyn_Exp*_tmp732;void*
_tmp733;void*_tmp735;struct Cyc_Absyn_Exp*_tmp737;_LL399: _tmp725=_tmp724.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp726=(struct Cyc_Absyn_Upper_b_struct*)_tmp725;
if(_tmp726->tag != 1)goto _LL39B;};_tmp727=_tmp724.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp728=(struct Cyc_Absyn_DynEither_b_struct*)_tmp727;if(_tmp728->tag != 0)goto
_LL39B;};_LL39A: goto _LL39C;_LL39B: _tmp729=_tmp724.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp72A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp729;if(_tmp72A->tag != 0)goto
_LL39D;};_tmp72B=_tmp724.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp72C=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp72B;if(_tmp72C->tag != 0)goto _LL39D;};_LL39C:
okay=1;goto _LL398;_LL39D: _tmp72D=_tmp724.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp72E=(struct Cyc_Absyn_Upper_b_struct*)_tmp72D;if(_tmp72E->tag != 1)goto _LL39F;
else{_tmp72F=_tmp72E->f1;}};_tmp730=_tmp724.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp731=(struct Cyc_Absyn_Upper_b_struct*)_tmp730;if(_tmp731->tag != 1)goto _LL39F;
else{_tmp732=_tmp731->f1;}};_LL39E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp732,
_tmp72F);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp710.ptr_atts).zero_term)){const char*_tmp1217;void*_tmp1216;(_tmp1216=0,
Cyc_Tcutil_warn(loc,((_tmp1217="implicit cast to shorter array",_tag_dyneither(
_tmp1217,sizeof(char),31))),_tag_dyneither(_tmp1216,sizeof(void*),0)));}goto
_LL398;_LL39F: _tmp733=_tmp724.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp734=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp733;if(_tmp734->tag != 0)goto _LL398;};
_tmp735=_tmp724.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp736=(struct Cyc_Absyn_Upper_b_struct*)
_tmp735;if(_tmp736->tag != 1)goto _LL398;else{_tmp737=_tmp736->f1;}};_LL3A0: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp70D.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp710.ptr_atts).bounds))goto _LL398;okay=0;goto
_LL398;_LL398:;}okay=okay  && (!(_tmp70D.elt_tq).real_const  || (_tmp710.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp70D.ptr_atts).rgn,(_tmp710.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp70D.ptr_atts).rgn,(_tmp710.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp70D.ptr_atts).zero_term,(
_tmp710.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp70D.ptr_atts).zero_term) && (_tmp710.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp70D.elt_typ,_tmp710.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp70D.elt_typ,_tmp710.elt_typ));return okay;}_LL392: _tmp711=_tmp70A.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp712=(struct Cyc_Absyn_ArrayType_struct*)_tmp711;
if(_tmp712->tag != 9)goto _LL394;else{_tmp713=_tmp712->f1;_tmp714=_tmp713.elt_type;
_tmp715=_tmp713.tq;_tmp716=_tmp713.num_elts;_tmp717=_tmp713.zero_term;}};_tmp718=
_tmp70A.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp719=(struct Cyc_Absyn_ArrayType_struct*)
_tmp718;if(_tmp719->tag != 9)goto _LL394;else{_tmp71A=_tmp719->f1;_tmp71B=_tmp71A.elt_type;
_tmp71C=_tmp71A.tq;_tmp71D=_tmp71A.num_elts;_tmp71E=_tmp71A.zero_term;}};_LL393: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp717,_tmp71E) && ((_tmp716 != 0
 && _tmp71D != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp716,(
struct Cyc_Absyn_Exp*)_tmp71D));return(okay  && Cyc_Tcutil_unify(_tmp714,_tmp71B))
 && (!_tmp715.real_const  || _tmp71C.real_const);}_LL394: _tmp71F=_tmp70A.f1;{
struct Cyc_Absyn_TagType_struct*_tmp720=(struct Cyc_Absyn_TagType_struct*)_tmp71F;
if(_tmp720->tag != 20)goto _LL396;};_tmp721=_tmp70A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp722=(struct Cyc_Absyn_IntType_struct*)_tmp721;if(_tmp722->tag != 6)goto _LL396;};
_LL395: return 0;_LL396:;_LL397: return Cyc_Tcutil_unify(t1,t2);_LL38F:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp73A=Cyc_Tcutil_compress(t);
_LL3A2: {struct Cyc_Absyn_PointerType_struct*_tmp73B=(struct Cyc_Absyn_PointerType_struct*)
_tmp73A;if(_tmp73B->tag != 5)goto _LL3A4;}_LL3A3: return 1;_LL3A4:;_LL3A5: return 0;
_LL3A1:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp73C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp73E;void*
_tmp73F;_LL3A7: {struct Cyc_Absyn_PointerType_struct*_tmp73D=(struct Cyc_Absyn_PointerType_struct*)
_tmp73C;if(_tmp73D->tag != 5)goto _LL3A9;else{_tmp73E=_tmp73D->f1;_tmp73F=_tmp73E.elt_typ;}}
_LL3A8: return _tmp73F;_LL3A9:;_LL3AA: {const char*_tmp121A;void*_tmp1219;(_tmp1219=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp121A="pointer_elt_type",_tag_dyneither(_tmp121A,sizeof(char),17))),
_tag_dyneither(_tmp1219,sizeof(void*),0)));}_LL3A6:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp742=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp744;struct Cyc_Absyn_PtrAtts _tmp745;struct Cyc_Absyn_PtrAtts*
_tmp746;_LL3AC: {struct Cyc_Absyn_PointerType_struct*_tmp743=(struct Cyc_Absyn_PointerType_struct*)
_tmp742;if(_tmp743->tag != 5)goto _LL3AE;else{_tmp744=_tmp743->f1;_tmp745=_tmp744.ptr_atts;
_tmp746=(struct Cyc_Absyn_PtrAtts*)&(_tmp743->f1).ptr_atts;}}_LL3AD: return _tmp746->rgn;
_LL3AE:;_LL3AF: {const char*_tmp121D;void*_tmp121C;(_tmp121C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp121D="pointer_elt_type",
_tag_dyneither(_tmp121D,sizeof(char),17))),_tag_dyneither(_tmp121C,sizeof(void*),
0)));}_LL3AB:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp749=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp74B;struct Cyc_Absyn_PtrAtts _tmp74C;void*_tmp74D;_LL3B1: {struct Cyc_Absyn_PointerType_struct*
_tmp74A=(struct Cyc_Absyn_PointerType_struct*)_tmp749;if(_tmp74A->tag != 5)goto
_LL3B3;else{_tmp74B=_tmp74A->f1;_tmp74C=_tmp74B.ptr_atts;_tmp74D=_tmp74C.rgn;}}
_LL3B2:*rgn=_tmp74D;return 1;_LL3B3:;_LL3B4: return 0;_LL3B0:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp74E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp750;struct Cyc_Absyn_PtrAtts _tmp751;union Cyc_Absyn_Constraint*_tmp752;_LL3B6: {
struct Cyc_Absyn_PointerType_struct*_tmp74F=(struct Cyc_Absyn_PointerType_struct*)
_tmp74E;if(_tmp74F->tag != 5)goto _LL3B8;else{_tmp750=_tmp74F->f1;_tmp751=_tmp750.ptr_atts;
_tmp752=_tmp751.bounds;}}_LL3B7:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp752)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3B8:;_LL3B9: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3B5:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp753=e->r;union Cyc_Absyn_Cnst _tmp755;struct
_tuple7 _tmp756;int _tmp757;union Cyc_Absyn_Cnst _tmp759;struct _tuple5 _tmp75A;char
_tmp75B;union Cyc_Absyn_Cnst _tmp75D;struct _tuple6 _tmp75E;short _tmp75F;union Cyc_Absyn_Cnst
_tmp761;struct _tuple8 _tmp762;long long _tmp763;void*_tmp765;struct Cyc_Absyn_Exp*
_tmp766;_LL3BB: {struct Cyc_Absyn_Const_e_struct*_tmp754=(struct Cyc_Absyn_Const_e_struct*)
_tmp753;if(_tmp754->tag != 0)goto _LL3BD;else{_tmp755=_tmp754->f1;if((_tmp755.Int_c).tag
!= 4)goto _LL3BD;_tmp756=(struct _tuple7)(_tmp755.Int_c).val;_tmp757=_tmp756.f2;
if(_tmp757 != 0)goto _LL3BD;}}_LL3BC: goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Const_e_struct*
_tmp758=(struct Cyc_Absyn_Const_e_struct*)_tmp753;if(_tmp758->tag != 0)goto _LL3BF;
else{_tmp759=_tmp758->f1;if((_tmp759.Char_c).tag != 2)goto _LL3BF;_tmp75A=(struct
_tuple5)(_tmp759.Char_c).val;_tmp75B=_tmp75A.f2;if(_tmp75B != 0)goto _LL3BF;}}
_LL3BE: goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Const_e_struct*_tmp75C=(struct Cyc_Absyn_Const_e_struct*)
_tmp753;if(_tmp75C->tag != 0)goto _LL3C1;else{_tmp75D=_tmp75C->f1;if((_tmp75D.Short_c).tag
!= 3)goto _LL3C1;_tmp75E=(struct _tuple6)(_tmp75D.Short_c).val;_tmp75F=_tmp75E.f2;
if(_tmp75F != 0)goto _LL3C1;}}_LL3C0: goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Const_e_struct*
_tmp760=(struct Cyc_Absyn_Const_e_struct*)_tmp753;if(_tmp760->tag != 0)goto _LL3C3;
else{_tmp761=_tmp760->f1;if((_tmp761.LongLong_c).tag != 5)goto _LL3C3;_tmp762=(
struct _tuple8)(_tmp761.LongLong_c).val;_tmp763=_tmp762.f2;if(_tmp763 != 0)goto
_LL3C3;}}_LL3C2: return 1;_LL3C3: {struct Cyc_Absyn_Cast_e_struct*_tmp764=(struct
Cyc_Absyn_Cast_e_struct*)_tmp753;if(_tmp764->tag != 15)goto _LL3C5;else{_tmp765=(
void*)_tmp764->f1;_tmp766=_tmp764->f2;}}_LL3C4: return Cyc_Tcutil_is_zero(_tmp766)
 && Cyc_Tcutil_admits_zero(_tmp765);_LL3C5:;_LL3C6: return 0;_LL3BA:;}struct Cyc_Absyn_Kind
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
Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp768;
enum Cyc_Absyn_KindQual _tmp769;enum Cyc_Absyn_AliasQual _tmp76A;struct Cyc_Absyn_Kind*
_tmp767=ka;_tmp768=*_tmp767;_tmp769=_tmp768.kind;_tmp76A=_tmp768.aliasqual;
switch(_tmp76A){case Cyc_Absyn_Aliasable: _LL3C7: switch(_tmp769){case Cyc_Absyn_AnyKind:
_LL3C9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3CA:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3CB: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3CC: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3CD: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3CE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3C8: switch(_tmp769){case Cyc_Absyn_AnyKind: _LL3D1: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3D2: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3D3: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3D4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3D5: break;}break;case Cyc_Absyn_Top: _LL3D0: switch(_tmp769){case Cyc_Absyn_AnyKind:
_LL3D8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3D9:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3DA: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3DB: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3DC: break;}break;}{const char*_tmp1221;
void*_tmp1220[1];struct Cyc_String_pa_struct _tmp121F;(_tmp121F.tag=0,((_tmp121F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp1220[0]=& _tmp121F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1221="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp1221,sizeof(char),26))),_tag_dyneither(_tmp1220,sizeof(void*),1)))))));};}
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
_tmp76F;enum Cyc_Absyn_KindQual _tmp770;enum Cyc_Absyn_AliasQual _tmp771;struct Cyc_Absyn_Kind*
_tmp76E=ka;_tmp76F=*_tmp76E;_tmp770=_tmp76F.kind;_tmp771=_tmp76F.aliasqual;
switch(_tmp771){case Cyc_Absyn_Aliasable: _LL3DE: switch(_tmp770){case Cyc_Absyn_AnyKind:
_LL3E0: return& ab;case Cyc_Absyn_MemKind: _LL3E1: return& mb;case Cyc_Absyn_BoxKind:
_LL3E2: return& bb;case Cyc_Absyn_RgnKind: _LL3E3: return& rb;case Cyc_Absyn_EffKind:
_LL3E4: return& eb;case Cyc_Absyn_IntKind: _LL3E5: return& ib;}case Cyc_Absyn_Unique:
_LL3DF: switch(_tmp770){case Cyc_Absyn_AnyKind: _LL3E8: return& uab;case Cyc_Absyn_MemKind:
_LL3E9: return& umb;case Cyc_Absyn_BoxKind: _LL3EA: return& ubb;case Cyc_Absyn_RgnKind:
_LL3EB: return& urb;default: _LL3EC: break;}break;case Cyc_Absyn_Top: _LL3E7: switch(
_tmp770){case Cyc_Absyn_AnyKind: _LL3EF: return& tab;case Cyc_Absyn_MemKind: _LL3F0:
return& tmb;case Cyc_Absyn_BoxKind: _LL3F1: return& tbb;case Cyc_Absyn_RgnKind: _LL3F2:
return& trb;default: _LL3F3: break;}break;}{const char*_tmp1225;void*_tmp1224[1];
struct Cyc_String_pa_struct _tmp1223;(_tmp1223.tag=0,((_tmp1223.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1224[0]=&
_tmp1223,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1225="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1225,sizeof(char),24))),
_tag_dyneither(_tmp1224,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct
Cyc_Absyn_Kind*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp1228;struct Cyc_Absyn_Const_e_struct*
_tmp1227;e1->r=(void*)((_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227[0]=((
_tmp1228.tag=0,((_tmp1228.f1=Cyc_Absyn_Null_c,_tmp1228)))),_tmp1227))));}{struct
Cyc_Core_Opt*_tmp785=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp1232;struct Cyc_Absyn_PtrAtts _tmp1231;struct Cyc_Absyn_PtrInfo _tmp1230;struct
Cyc_Absyn_PointerType_struct*_tmp122F;struct Cyc_Absyn_PointerType_struct*_tmp786=(
_tmp122F=_cycalloc(sizeof(*_tmp122F)),((_tmp122F[0]=((_tmp1232.tag=5,((_tmp1232.f1=((
_tmp1230.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp785),((
_tmp1230.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp1230.ptr_atts=((_tmp1231.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp785),((_tmp1231.nullable=Cyc_Absyn_true_conref,((
_tmp1231.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1231.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1231.ptrloc=0,_tmp1231)))))))))),_tmp1230)))))),_tmp1232)))),_tmp122F)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp786);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp1237;void*_tmp1236[2];struct Cyc_String_pa_struct _tmp1235;struct Cyc_String_pa_struct
_tmp1234;(_tmp1234.tag=0,((_tmp1234.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp1235.tag=0,((
_tmp1235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1236[0]=& _tmp1235,((_tmp1236[1]=& _tmp1234,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1237="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp1237,sizeof(char),56))),_tag_dyneither(_tmp1236,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum 
Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL3F5: {const char*
_tmp1238;return(_tmp1238="unknown",_tag_dyneither(_tmp1238,sizeof(char),8));}
case Cyc_Absyn_No_coercion: _LL3F6: {const char*_tmp1239;return(_tmp1239="no coercion",
_tag_dyneither(_tmp1239,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL3F7: {
const char*_tmp123A;return(_tmp123A="null check",_tag_dyneither(_tmp123A,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL3F8: {const char*_tmp123B;return(
_tmp123B="other coercion",_tag_dyneither(_tmp123B,sizeof(char),15));}}}static int
Cyc_Tcutil_is_nonunique_region(void*r);int Cyc_Tcutil_warn_alias_coerce=0;struct
_tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);struct _tuple14
Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static struct Cyc_Absyn_Eq_kb_struct
rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct _dyneither_ptr*_tmp1248;const
char*_tmp1247;void*_tmp1246[1];struct Cyc_Int_pa_struct _tmp1245;struct _tuple2*
_tmp1244;struct _tuple2*v=(_tmp1244=_cycalloc(sizeof(*_tmp1244)),((_tmp1244->f1=
Cyc_Absyn_Loc_n,((_tmp1244->f2=(struct _dyneither_ptr*)((_tmp1248=_cycalloc(
sizeof(*_tmp1248)),((_tmp1248[0]=((_tmp1245.tag=1,((_tmp1245.f1=(unsigned long)
counter ++,((_tmp1246[0]=& _tmp1245,Cyc_aprintf(((_tmp1247="__aliasvar%d",
_tag_dyneither(_tmp1247,sizeof(char),13))),_tag_dyneither(_tmp1246,sizeof(void*),
1)))))))),_tmp1248)))),_tmp1244)))));struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
v,(void*)& Cyc_Absyn_VoidType_val,0);struct Cyc_Absyn_Local_b_struct _tmp124B;
struct Cyc_Absyn_Local_b_struct*_tmp124A;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(
v,(void*)((_tmp124A=_cycalloc(sizeof(*_tmp124A)),((_tmp124A[0]=((_tmp124B.tag=4,((
_tmp124B.f1=vd,_tmp124B)))),_tmp124A)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((
void*)& rgn_kb);{void*_tmp793=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo
_tmp795;struct Cyc_Absyn_PtrAtts _tmp796;void*_tmp797;void**_tmp798;_LL3FB: {
struct Cyc_Absyn_PointerType_struct*_tmp794=(struct Cyc_Absyn_PointerType_struct*)
_tmp793;if(_tmp794->tag != 5)goto _LL3FD;else{_tmp795=_tmp794->f1;_tmp796=_tmp795.ptr_atts;
_tmp797=_tmp796.rgn;_tmp798=(void**)&((_tmp794->f1).ptr_atts).rgn;}}_LL3FC:{
struct Cyc_Absyn_VarType_struct _tmp124E;struct Cyc_Absyn_VarType_struct*_tmp124D;*
_tmp798=(void*)((_tmp124D=_cycalloc(sizeof(*_tmp124D)),((_tmp124D[0]=((_tmp124E.tag=
2,((_tmp124E.f1=tv,_tmp124E)))),_tmp124D))));}goto _LL3FA;_LL3FD:;_LL3FE: goto
_LL3FA;_LL3FA:;}e->topt=0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,
e->loc);{struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct
_tuple14 _tmp124F;return(_tmp124F.f1=d,((_tmp124F.f2=ve,_tmp124F)));};}static int
Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,
struct Cyc_Position_Segment*loc);static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7A4=_new_region("r");struct _RegionHandle*r=& _tmp7A4;_push_region(r);if(Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)){void*_tmp7A5=Cyc_Tcutil_compress(
wants_typ);struct Cyc_Absyn_PtrInfo _tmp7A7;struct Cyc_Absyn_PtrAtts _tmp7A8;void*
_tmp7A9;_LL400: {struct Cyc_Absyn_PointerType_struct*_tmp7A6=(struct Cyc_Absyn_PointerType_struct*)
_tmp7A5;if(_tmp7A6->tag != 5)goto _LL402;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A7.ptr_atts;
_tmp7A9=_tmp7A8.rgn;}}_LL401: {void*_tmp7AB=Cyc_Tcutil_compress(_tmp7A9);_LL407: {
struct Cyc_Absyn_HeapRgn_struct*_tmp7AC=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7AB;
if(_tmp7AC->tag != 21)goto _LL409;}_LL408: {int _tmp7AD=0;_npop_handler(0);return
_tmp7AD;}_LL409:;_LL40A: {int _tmp7AE=Cyc_Tcutil_is_nonunique_region(_tmp7A9);
_npop_handler(0);return _tmp7AE;}_LL406:;}_LL402: {struct Cyc_Absyn_TypedefType_struct*
_tmp7AA=(struct Cyc_Absyn_TypedefType_struct*)_tmp7A5;if(_tmp7AA->tag != 18)goto
_LL404;}_LL403: {const char*_tmp1254;void*_tmp1253[2];struct Cyc_String_pa_struct
_tmp1252;struct Cyc_String_pa_struct _tmp1251;(_tmp1251.tag=0,((_tmp1251.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(loc)),((
_tmp1252.tag=0,((_tmp1252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_compress(wants_typ))),((_tmp1253[0]=& _tmp1252,((_tmp1253[1]=& _tmp1251,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1254="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1254,
sizeof(char),42))),_tag_dyneither(_tmp1253,sizeof(void*),2)))))))))))));}_LL404:;
_LL405: {int _tmp7B3=0;_npop_handler(0);return _tmp7B3;}_LL3FF:;}{int _tmp7B4=0;
_npop_handler(0);return _tmp7B4;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1259;void*_tmp1258[2];
struct Cyc_String_pa_struct _tmp1257;struct Cyc_String_pa_struct _tmp1256;(_tmp1256.tag=
0,((_tmp1256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1258[0]=& _tmp1257,((_tmp1258[1]=& _tmp1256,Cyc_Tcutil_warn(
e->loc,((_tmp1259="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1259,sizeof(char),53))),_tag_dyneither(_tmp1258,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp125F;void*_tmp125E[3];struct Cyc_String_pa_struct _tmp125D;struct Cyc_String_pa_struct
_tmp125C;struct Cyc_String_pa_struct _tmp125B;(_tmp125B.tag=0,((_tmp125B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp125E[0]=& _tmp125D,((_tmp125E[1]=& _tmp125C,((
_tmp125E[2]=& _tmp125B,Cyc_Tcutil_warn(e->loc,((_tmp125F="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp125F,sizeof(char),40))),_tag_dyneither(_tmp125E,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp1264;void*_tmp1263[2];
struct Cyc_String_pa_struct _tmp1262;struct Cyc_String_pa_struct _tmp1261;(_tmp1261.tag=
0,((_tmp1261.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1262.tag=0,((_tmp1262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1263[0]=& _tmp1262,((_tmp1263[1]=& _tmp1261,Cyc_Tcutil_warn(
e->loc,((_tmp1264="implicit cast from %s to %s",_tag_dyneither(_tmp1264,sizeof(
char),28))),_tag_dyneither(_tmp1263,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp7C2=Cyc_Tcutil_compress(t);
_LL40C: {struct Cyc_Absyn_IntType_struct*_tmp7C3=(struct Cyc_Absyn_IntType_struct*)
_tmp7C2;if(_tmp7C3->tag != 6)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_FloatType_struct*
_tmp7C4=(struct Cyc_Absyn_FloatType_struct*)_tmp7C2;if(_tmp7C4->tag != 7)goto
_LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_DoubleType_struct*_tmp7C5=(
struct Cyc_Absyn_DoubleType_struct*)_tmp7C2;if(_tmp7C5->tag != 8)goto _LL412;}
_LL411: return 1;_LL412:;_LL413: return 0;_LL40B:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp7C7;
struct _RegionHandle*_tmp7C8;struct _tuple16 _tmp7C6=*env;_tmp7C7=_tmp7C6.f1;
_tmp7C8=_tmp7C6.f2;{struct _tuple11*_tmp1265;return(_tmp1265=_region_malloc(
_tmp7C8,sizeof(*_tmp1265)),((_tmp1265->f1=x->tq,((_tmp1265->f2=Cyc_Tcutil_rsubstitute(
_tmp7C8,_tmp7C7,x->type),_tmp1265)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp7CB;void*
_tmp7CC;struct _tuple11 _tmp7CA=*x;_tmp7CB=_tmp7CA.f1;_tmp7CC=_tmp7CA.f2;{struct
_tuple11*_tmp1266;return(_tmp1266=_region_malloc(r,sizeof(*_tmp1266)),((_tmp1266->f1=
_tmp7CB,((_tmp1266->f2=_tmp7CC,_tmp1266)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp7CE=t1;struct Cyc_List_List*_tmp7D1;struct Cyc_Absyn_AggrInfo
_tmp7D3;union Cyc_Absyn_AggrInfoU _tmp7D4;struct Cyc_Absyn_Aggrdecl**_tmp7D5;struct
Cyc_Absyn_Aggrdecl*_tmp7D6;struct Cyc_List_List*_tmp7D7;enum Cyc_Absyn_AggrKind
_tmp7D9;struct Cyc_List_List*_tmp7DA;_LL415: {struct Cyc_Absyn_VoidType_struct*
_tmp7CF=(struct Cyc_Absyn_VoidType_struct*)_tmp7CE;if(_tmp7CF->tag != 0)goto _LL417;}
_LL416: return 0;_LL417: {struct Cyc_Absyn_TupleType_struct*_tmp7D0=(struct Cyc_Absyn_TupleType_struct*)
_tmp7CE;if(_tmp7D0->tag != 11)goto _LL419;else{_tmp7D1=_tmp7D0->f1;}}_LL418: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp7D1);_LL419: {struct Cyc_Absyn_AggrType_struct*
_tmp7D2=(struct Cyc_Absyn_AggrType_struct*)_tmp7CE;if(_tmp7D2->tag != 12)goto
_LL41B;else{_tmp7D3=_tmp7D2->f1;_tmp7D4=_tmp7D3.aggr_info;if((_tmp7D4.KnownAggr).tag
!= 2)goto _LL41B;_tmp7D5=(struct Cyc_Absyn_Aggrdecl**)(_tmp7D4.KnownAggr).val;
_tmp7D6=*_tmp7D5;_tmp7D7=_tmp7D3.targs;}}_LL41A: if(((_tmp7D6->kind == Cyc_Absyn_UnionA
 || _tmp7D6->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D6->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D6->impl))->rgn_po != 0){
struct _tuple11*_tmp1269;struct Cyc_List_List*_tmp1268;return(_tmp1268=
_region_malloc(r,sizeof(*_tmp1268)),((_tmp1268->hd=((_tmp1269=_region_malloc(r,
sizeof(*_tmp1269)),((_tmp1269->f1=Cyc_Absyn_const_tqual(0),((_tmp1269->f2=t1,
_tmp1269)))))),((_tmp1268->tl=0,_tmp1268)))));}{struct Cyc_List_List*_tmp7DD=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp7D6->tvs,_tmp7D7);struct _tuple16
_tmp126A;struct _tuple16 env=(_tmp126A.f1=_tmp7DD,((_tmp126A.f2=r,_tmp126A)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple16*,struct Cyc_Absyn_Aggrfield*),struct _tuple16*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7D6->impl))->fields);};_LL41B: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp7D8=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp7CE;if(_tmp7D8->tag != 13)goto
_LL41D;else{_tmp7D9=_tmp7D8->f1;if(_tmp7D9 != Cyc_Absyn_StructA)goto _LL41D;
_tmp7DA=_tmp7D8->f2;}}_LL41C: {struct _tuple16 _tmp126B;struct _tuple16 env=(
_tmp126B.f1=0,((_tmp126B.f2=r,_tmp126B)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple16*,struct Cyc_Absyn_Aggrfield*),
struct _tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp7DA);}_LL41D:;_LL41E: {struct _tuple11*_tmp126E;struct Cyc_List_List*
_tmp126D;return(_tmp126D=_region_malloc(r,sizeof(*_tmp126D)),((_tmp126D->hd=((
_tmp126E=_region_malloc(r,sizeof(*_tmp126E)),((_tmp126E->f1=Cyc_Absyn_const_tqual(
0),((_tmp126E->f2=t1,_tmp126E)))))),((_tmp126D->tl=0,_tmp126D)))));}_LL414:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp7E2=(void*)t->hd;_LL420: {
struct Cyc_Absyn_Pure_att_struct*_tmp7E3=(struct Cyc_Absyn_Pure_att_struct*)
_tmp7E2;if(_tmp7E3->tag != 22)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp7E4=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp7E2;if(_tmp7E4->tag != 4)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Initializes_att_struct*
_tmp7E5=(struct Cyc_Absyn_Initializes_att_struct*)_tmp7E2;if(_tmp7E5->tag != 20)
goto _LL426;}_LL425: continue;_LL426:;_LL427: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL41F:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp126F;struct _tuple0 _tmp7E7=(_tmp126F.f1=t1,((_tmp126F.f2=t2,_tmp126F)));void*
_tmp7E8;struct Cyc_Absyn_PtrInfo _tmp7EA;void*_tmp7EB;struct Cyc_Absyn_Tqual _tmp7EC;
struct Cyc_Absyn_PtrAtts _tmp7ED;void*_tmp7EE;union Cyc_Absyn_Constraint*_tmp7EF;
union Cyc_Absyn_Constraint*_tmp7F0;union Cyc_Absyn_Constraint*_tmp7F1;void*_tmp7F2;
struct Cyc_Absyn_PtrInfo _tmp7F4;void*_tmp7F5;struct Cyc_Absyn_Tqual _tmp7F6;struct
Cyc_Absyn_PtrAtts _tmp7F7;void*_tmp7F8;union Cyc_Absyn_Constraint*_tmp7F9;union Cyc_Absyn_Constraint*
_tmp7FA;union Cyc_Absyn_Constraint*_tmp7FB;void*_tmp7FC;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7FE;union Cyc_Absyn_DatatypeFieldInfoU _tmp7FF;struct _tuple3 _tmp800;struct Cyc_Absyn_Datatypedecl*
_tmp801;struct Cyc_Absyn_Datatypefield*_tmp802;struct Cyc_List_List*_tmp803;void*
_tmp804;struct Cyc_Absyn_DatatypeInfo _tmp806;union Cyc_Absyn_DatatypeInfoU _tmp807;
struct Cyc_Absyn_Datatypedecl**_tmp808;struct Cyc_Absyn_Datatypedecl*_tmp809;
struct Cyc_List_List*_tmp80A;void*_tmp80B;struct Cyc_Absyn_FnInfo _tmp80D;void*
_tmp80E;struct Cyc_Absyn_FnInfo _tmp810;_LL429: _tmp7E8=_tmp7E7.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp7E9=(struct Cyc_Absyn_PointerType_struct*)_tmp7E8;if(_tmp7E9->tag != 5)goto
_LL42B;else{_tmp7EA=_tmp7E9->f1;_tmp7EB=_tmp7EA.elt_typ;_tmp7EC=_tmp7EA.elt_tq;
_tmp7ED=_tmp7EA.ptr_atts;_tmp7EE=_tmp7ED.rgn;_tmp7EF=_tmp7ED.nullable;_tmp7F0=
_tmp7ED.bounds;_tmp7F1=_tmp7ED.zero_term;}};_tmp7F2=_tmp7E7.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp7F3=(struct Cyc_Absyn_PointerType_struct*)_tmp7F2;if(_tmp7F3->tag != 5)goto
_LL42B;else{_tmp7F4=_tmp7F3->f1;_tmp7F5=_tmp7F4.elt_typ;_tmp7F6=_tmp7F4.elt_tq;
_tmp7F7=_tmp7F4.ptr_atts;_tmp7F8=_tmp7F7.rgn;_tmp7F9=_tmp7F7.nullable;_tmp7FA=
_tmp7F7.bounds;_tmp7FB=_tmp7F7.zero_term;}};_LL42A: if(_tmp7EC.real_const  && !
_tmp7F6.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7EF,
_tmp7F9) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7EF)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F9))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7F1,
_tmp7FB) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F1)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7FB))return 0;if(!Cyc_Tcutil_unify(_tmp7EE,_tmp7F8) && !Cyc_Tcenv_region_outlives(
te,_tmp7EE,_tmp7F8))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp7F0,_tmp7FA)){struct _tuple0 _tmp1270;struct _tuple0 _tmp812=(_tmp1270.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7F0),((_tmp1270.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7FA),_tmp1270)));void*
_tmp813;void*_tmp815;void*_tmp817;struct Cyc_Absyn_Exp*_tmp819;void*_tmp81A;
struct Cyc_Absyn_Exp*_tmp81C;_LL432: _tmp813=_tmp812.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp814=(struct Cyc_Absyn_Upper_b_struct*)_tmp813;if(_tmp814->tag != 1)goto _LL434;};
_tmp815=_tmp812.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp816=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp815;if(_tmp816->tag != 0)goto _LL434;};_LL433: goto _LL431;_LL434: _tmp817=
_tmp812.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp818=(struct Cyc_Absyn_Upper_b_struct*)
_tmp817;if(_tmp818->tag != 1)goto _LL436;else{_tmp819=_tmp818->f1;}};_tmp81A=
_tmp812.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp81B=(struct Cyc_Absyn_Upper_b_struct*)
_tmp81A;if(_tmp81B->tag != 1)goto _LL436;else{_tmp81C=_tmp81B->f1;}};_LL435: if(!
Cyc_Evexp_lte_const_exp(_tmp81C,_tmp819))return 0;goto _LL431;_LL436:;_LL437:
return 0;_LL431:;}{struct _tuple0*_tmp1273;struct Cyc_List_List*_tmp1272;return Cyc_Tcutil_ptrsubtype(
te,((_tmp1272=_cycalloc(sizeof(*_tmp1272)),((_tmp1272->hd=((_tmp1273=_cycalloc(
sizeof(*_tmp1273)),((_tmp1273->f1=t1,((_tmp1273->f2=t2,_tmp1273)))))),((_tmp1272->tl=
assume,_tmp1272)))))),_tmp7EB,_tmp7F5);};_LL42B: _tmp7FC=_tmp7E7.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7FD=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp7FC;if(_tmp7FD->tag != 4)
goto _LL42D;else{_tmp7FE=_tmp7FD->f1;_tmp7FF=_tmp7FE.field_info;if((_tmp7FF.KnownDatatypefield).tag
!= 2)goto _LL42D;_tmp800=(struct _tuple3)(_tmp7FF.KnownDatatypefield).val;_tmp801=
_tmp800.f1;_tmp802=_tmp800.f2;_tmp803=_tmp7FE.targs;}};_tmp804=_tmp7E7.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp805=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp804;if(_tmp805->tag != 3)goto _LL42D;else{_tmp806=_tmp805->f1;_tmp807=_tmp806.datatype_info;
if((_tmp807.KnownDatatype).tag != 2)goto _LL42D;_tmp808=(struct Cyc_Absyn_Datatypedecl**)(
_tmp807.KnownDatatype).val;_tmp809=*_tmp808;_tmp80A=_tmp806.targs;}};_LL42C: if(
_tmp801 != _tmp809  && Cyc_Absyn_qvar_cmp(_tmp801->name,_tmp809->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp803)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp80A))return 0;for(0;_tmp803 != 0;(_tmp803=
_tmp803->tl,_tmp80A=_tmp80A->tl)){if(!Cyc_Tcutil_unify((void*)_tmp803->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp80A))->hd))return 0;}return 1;_LL42D: _tmp80B=
_tmp7E7.f1;{struct Cyc_Absyn_FnType_struct*_tmp80C=(struct Cyc_Absyn_FnType_struct*)
_tmp80B;if(_tmp80C->tag != 10)goto _LL42F;else{_tmp80D=_tmp80C->f1;}};_tmp80E=
_tmp7E7.f2;{struct Cyc_Absyn_FnType_struct*_tmp80F=(struct Cyc_Absyn_FnType_struct*)
_tmp80E;if(_tmp80F->tag != 10)goto _LL42F;else{_tmp810=_tmp80F->f1;}};_LL42E: if(
_tmp80D.tvars != 0  || _tmp810.tvars != 0){struct Cyc_List_List*_tmp81F=_tmp80D.tvars;
struct Cyc_List_List*_tmp820=_tmp810.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp81F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp820))return 0;{
struct _RegionHandle*_tmp821=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp81F != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp81F->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp820))->hd)->kind))return 0;{struct _tuple15*_tmp127D;struct Cyc_Absyn_VarType_struct
_tmp127C;struct Cyc_Absyn_VarType_struct*_tmp127B;struct Cyc_List_List*_tmp127A;
inst=((_tmp127A=_region_malloc(_tmp821,sizeof(*_tmp127A)),((_tmp127A->hd=((
_tmp127D=_region_malloc(_tmp821,sizeof(*_tmp127D)),((_tmp127D->f1=(struct Cyc_Absyn_Tvar*)
_tmp820->hd,((_tmp127D->f2=(void*)((_tmp127B=_cycalloc(sizeof(*_tmp127B)),((
_tmp127B[0]=((_tmp127C.tag=2,((_tmp127C.f1=(struct Cyc_Absyn_Tvar*)_tmp81F->hd,
_tmp127C)))),_tmp127B)))),_tmp127D)))))),((_tmp127A->tl=inst,_tmp127A))))));}
_tmp81F=_tmp81F->tl;_tmp820=_tmp820->tl;}if(inst != 0){_tmp80D.tvars=0;_tmp810.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1283;struct Cyc_Absyn_FnType_struct*_tmp1282;
struct Cyc_Absyn_FnType_struct _tmp1280;struct Cyc_Absyn_FnType_struct*_tmp127F;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((
_tmp127F[0]=((_tmp1280.tag=10,((_tmp1280.f1=_tmp80D,_tmp1280)))),_tmp127F)))),(
void*)((_tmp1282=_cycalloc(sizeof(*_tmp1282)),((_tmp1282[0]=((_tmp1283.tag=10,((
_tmp1283.f1=_tmp810,_tmp1283)))),_tmp1282)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp80D.ret_typ,_tmp810.ret_typ))return 0;{struct Cyc_List_List*_tmp82A=
_tmp80D.args;struct Cyc_List_List*_tmp82B=_tmp810.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp82A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp82B))return 0;for(0;_tmp82A != 0;(_tmp82A=_tmp82A->tl,_tmp82B=_tmp82B->tl)){
struct Cyc_Absyn_Tqual _tmp82D;void*_tmp82E;struct _tuple9 _tmp82C=*((struct _tuple9*)
_tmp82A->hd);_tmp82D=_tmp82C.f2;_tmp82E=_tmp82C.f3;{struct Cyc_Absyn_Tqual _tmp830;
void*_tmp831;struct _tuple9 _tmp82F=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp82B))->hd);_tmp830=_tmp82F.f2;_tmp831=_tmp82F.f3;if(_tmp830.real_const
 && !_tmp82D.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp831,_tmp82E))return
0;};}if(_tmp80D.c_varargs != _tmp810.c_varargs)return 0;if(_tmp80D.cyc_varargs != 0
 && _tmp810.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp832=*_tmp80D.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp833=*_tmp810.cyc_varargs;if((_tmp833.tq).real_const
 && !(_tmp832.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp833.type,
_tmp832.type))return 0;}else{if(_tmp80D.cyc_varargs != 0  || _tmp810.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp80D.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp810.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp80D.rgn_po,_tmp810.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp80D.attributes,_tmp810.attributes))return 0;return 1;};_LL42F:;_LL430: return 0;
_LL428:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp1284;struct _tuple0 _tmp835=(_tmp1284.f1=Cyc_Tcutil_compress(
t1),((_tmp1284.f2=Cyc_Tcutil_compress(t2),_tmp1284)));void*_tmp836;enum Cyc_Absyn_Size_of
_tmp838;void*_tmp839;enum Cyc_Absyn_Size_of _tmp83B;_LL439: _tmp836=_tmp835.f1;{
struct Cyc_Absyn_IntType_struct*_tmp837=(struct Cyc_Absyn_IntType_struct*)_tmp836;
if(_tmp837->tag != 6)goto _LL43B;else{_tmp838=_tmp837->f2;}};_tmp839=_tmp835.f2;{
struct Cyc_Absyn_IntType_struct*_tmp83A=(struct Cyc_Absyn_IntType_struct*)_tmp839;
if(_tmp83A->tag != 6)goto _LL43B;else{_tmp83B=_tmp83A->f2;}};_LL43A: return(_tmp838
== _tmp83B  || _tmp838 == Cyc_Absyn_Int_sz  && _tmp83B == Cyc_Absyn_Long_sz) || 
_tmp838 == Cyc_Absyn_Long_sz  && _tmp83B == Cyc_Absyn_Int_sz;_LL43B:;_LL43C: return 0;
_LL438:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp83C=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp83C,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp83C,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp83E;struct Cyc_Absyn_Tqual
_tmp83F;void*_tmp840;struct _tuple11*_tmp83D=(struct _tuple11*)tqs1->hd;_tmp83E=*
_tmp83D;_tmp83F=_tmp83E.f1;_tmp840=_tmp83E.f2;{struct _tuple11 _tmp842;struct Cyc_Absyn_Tqual
_tmp843;void*_tmp844;struct _tuple11*_tmp841=(struct _tuple11*)tqs2->hd;_tmp842=*
_tmp841;_tmp843=_tmp842.f1;_tmp844=_tmp842.f2;if(_tmp843.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp840,_tmp844))continue;else{if(Cyc_Tcutil_unify(_tmp840,_tmp844))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp840,_tmp844))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp845=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp847;_LL43E: {
struct Cyc_Absyn_IntType_struct*_tmp846=(struct Cyc_Absyn_IntType_struct*)_tmp845;
if(_tmp846->tag != 6)goto _LL440;else{_tmp847=_tmp846->f2;if(_tmp847 != Cyc_Absyn_Char_sz)
goto _LL440;}}_LL43F: return 1;_LL440:;_LL441: return 0;_LL43D:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp848=
t2;enum Cyc_Absyn_Size_of _tmp84A;enum Cyc_Absyn_Size_of _tmp84C;_LL443: {struct
Cyc_Absyn_IntType_struct*_tmp849=(struct Cyc_Absyn_IntType_struct*)_tmp848;if(
_tmp849->tag != 6)goto _LL445;else{_tmp84A=_tmp849->f2;if(_tmp84A != Cyc_Absyn_Int_sz)
goto _LL445;}}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_IntType_struct*_tmp84B=(
struct Cyc_Absyn_IntType_struct*)_tmp848;if(_tmp84B->tag != 6)goto _LL447;else{
_tmp84C=_tmp84B->f2;if(_tmp84C != Cyc_Absyn_Long_sz)goto _LL447;}}_LL446: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL442;_LL447:;
_LL448: goto _LL442;_LL442:;}{void*_tmp84D=t1;struct Cyc_Absyn_PtrInfo _tmp84F;void*
_tmp850;struct Cyc_Absyn_Tqual _tmp851;struct Cyc_Absyn_PtrAtts _tmp852;void*_tmp853;
union Cyc_Absyn_Constraint*_tmp854;union Cyc_Absyn_Constraint*_tmp855;union Cyc_Absyn_Constraint*
_tmp856;struct Cyc_Absyn_ArrayInfo _tmp858;void*_tmp859;struct Cyc_Absyn_Tqual
_tmp85A;struct Cyc_Absyn_Exp*_tmp85B;union Cyc_Absyn_Constraint*_tmp85C;struct Cyc_Absyn_Enumdecl*
_tmp85E;void*_tmp863;_LL44A: {struct Cyc_Absyn_PointerType_struct*_tmp84E=(struct
Cyc_Absyn_PointerType_struct*)_tmp84D;if(_tmp84E->tag != 5)goto _LL44C;else{
_tmp84F=_tmp84E->f1;_tmp850=_tmp84F.elt_typ;_tmp851=_tmp84F.elt_tq;_tmp852=
_tmp84F.ptr_atts;_tmp853=_tmp852.rgn;_tmp854=_tmp852.nullable;_tmp855=_tmp852.bounds;
_tmp856=_tmp852.zero_term;}}_LL44B:{void*_tmp864=t2;struct Cyc_Absyn_PtrInfo
_tmp866;void*_tmp867;struct Cyc_Absyn_Tqual _tmp868;struct Cyc_Absyn_PtrAtts _tmp869;
void*_tmp86A;union Cyc_Absyn_Constraint*_tmp86B;union Cyc_Absyn_Constraint*_tmp86C;
union Cyc_Absyn_Constraint*_tmp86D;_LL45B: {struct Cyc_Absyn_PointerType_struct*
_tmp865=(struct Cyc_Absyn_PointerType_struct*)_tmp864;if(_tmp865->tag != 5)goto
_LL45D;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.elt_typ;_tmp868=_tmp866.elt_tq;
_tmp869=_tmp866.ptr_atts;_tmp86A=_tmp869.rgn;_tmp86B=_tmp869.nullable;_tmp86C=
_tmp869.bounds;_tmp86D=_tmp869.zero_term;}}_LL45C: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1287;struct Cyc_List_List*
_tmp1286;struct Cyc_List_List*_tmp86E=(_tmp1286=_cycalloc(sizeof(*_tmp1286)),((
_tmp1286->hd=((_tmp1287=_cycalloc(sizeof(*_tmp1287)),((_tmp1287->f1=t1,((
_tmp1287->f2=t2,_tmp1287)))))),((_tmp1286->tl=0,_tmp1286)))));int _tmp86F=Cyc_Tcutil_ptrsubtype(
te,_tmp86E,_tmp850,_tmp867) && (!_tmp851.real_const  || _tmp868.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp856,
_tmp86D) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86D);
int _tmp870=_tmp86F?0:((Cyc_Tcutil_bits_only(_tmp850) && Cyc_Tcutil_is_char_type(
_tmp867)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp86D)) && (_tmp868.real_const  || !_tmp851.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp855,_tmp86C);if(!
bounds_ok  && !_tmp870){struct _tuple0 _tmp1288;struct _tuple0 _tmp872=(_tmp1288.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp855),((_tmp1288.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86C),_tmp1288)));
void*_tmp873;struct Cyc_Absyn_Exp*_tmp875;void*_tmp876;struct Cyc_Absyn_Exp*
_tmp878;_LL460: _tmp873=_tmp872.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp874=(
struct Cyc_Absyn_Upper_b_struct*)_tmp873;if(_tmp874->tag != 1)goto _LL462;else{
_tmp875=_tmp874->f1;}};_tmp876=_tmp872.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp877=(struct Cyc_Absyn_Upper_b_struct*)_tmp876;if(_tmp877->tag != 1)goto _LL462;
else{_tmp878=_tmp877->f1;}};_LL461: if(Cyc_Evexp_lte_const_exp(_tmp878,_tmp875))
bounds_ok=1;goto _LL45F;_LL462:;_LL463: bounds_ok=1;goto _LL45F;_LL45F:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp854) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86B))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp86F  || _tmp870)) && (Cyc_Tcutil_unify(
_tmp853,_tmp86A) || Cyc_Tcenv_region_outlives(te,_tmp853,_tmp86A)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL45D:;_LL45E: goto _LL45A;
_LL45A:;}return Cyc_Absyn_Unknown_coercion;_LL44C: {struct Cyc_Absyn_ArrayType_struct*
_tmp857=(struct Cyc_Absyn_ArrayType_struct*)_tmp84D;if(_tmp857->tag != 9)goto
_LL44E;else{_tmp858=_tmp857->f1;_tmp859=_tmp858.elt_type;_tmp85A=_tmp858.tq;
_tmp85B=_tmp858.num_elts;_tmp85C=_tmp858.zero_term;}}_LL44D:{void*_tmp87B=t2;
struct Cyc_Absyn_ArrayInfo _tmp87D;void*_tmp87E;struct Cyc_Absyn_Tqual _tmp87F;
struct Cyc_Absyn_Exp*_tmp880;union Cyc_Absyn_Constraint*_tmp881;_LL465: {struct Cyc_Absyn_ArrayType_struct*
_tmp87C=(struct Cyc_Absyn_ArrayType_struct*)_tmp87B;if(_tmp87C->tag != 9)goto
_LL467;else{_tmp87D=_tmp87C->f1;_tmp87E=_tmp87D.elt_type;_tmp87F=_tmp87D.tq;
_tmp880=_tmp87D.num_elts;_tmp881=_tmp87D.zero_term;}}_LL466: {int okay;okay=((
_tmp85B != 0  && _tmp880 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp85C,
_tmp881)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp880,(struct Cyc_Absyn_Exp*)
_tmp85B);return(okay  && Cyc_Tcutil_unify(_tmp859,_tmp87E)) && (!_tmp85A.real_const
 || _tmp87F.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL467:;_LL468: return Cyc_Absyn_Unknown_coercion;_LL464:;}return Cyc_Absyn_Unknown_coercion;
_LL44E: {struct Cyc_Absyn_EnumType_struct*_tmp85D=(struct Cyc_Absyn_EnumType_struct*)
_tmp84D;if(_tmp85D->tag != 14)goto _LL450;else{_tmp85E=_tmp85D->f2;}}_LL44F:{void*
_tmp882=t2;struct Cyc_Absyn_Enumdecl*_tmp884;_LL46A: {struct Cyc_Absyn_EnumType_struct*
_tmp883=(struct Cyc_Absyn_EnumType_struct*)_tmp882;if(_tmp883->tag != 14)goto
_LL46C;else{_tmp884=_tmp883->f2;}}_LL46B: if((_tmp85E->fields != 0  && _tmp884->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp85E->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp884->fields))->v))return Cyc_Absyn_No_coercion;goto _LL469;_LL46C:;_LL46D: goto
_LL469;_LL469:;}goto _LL451;_LL450: {struct Cyc_Absyn_IntType_struct*_tmp85F=(
struct Cyc_Absyn_IntType_struct*)_tmp84D;if(_tmp85F->tag != 6)goto _LL452;}_LL451:
goto _LL453;_LL452: {struct Cyc_Absyn_FloatType_struct*_tmp860=(struct Cyc_Absyn_FloatType_struct*)
_tmp84D;if(_tmp860->tag != 7)goto _LL454;}_LL453: goto _LL455;_LL454: {struct Cyc_Absyn_DoubleType_struct*
_tmp861=(struct Cyc_Absyn_DoubleType_struct*)_tmp84D;if(_tmp861->tag != 8)goto
_LL456;}_LL455: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL456: {struct Cyc_Absyn_RgnHandleType_struct*_tmp862=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp84D;if(_tmp862->tag != 16)goto _LL458;else{_tmp863=(void*)_tmp862->f1;}}_LL457:{
void*_tmp885=t2;void*_tmp887;_LL46F: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp886=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp885;if(_tmp886->tag != 16)goto
_LL471;else{_tmp887=(void*)_tmp886->f1;}}_LL470: if(Cyc_Tcenv_region_outlives(te,
_tmp863,_tmp887))return Cyc_Absyn_No_coercion;goto _LL46E;_LL471:;_LL472: goto
_LL46E;_LL46E:;}return Cyc_Absyn_Unknown_coercion;_LL458:;_LL459: return Cyc_Absyn_Unknown_coercion;
_LL449:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp888=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp128B;struct Cyc_Absyn_Cast_e_struct*
_tmp128A;e->r=(void*)((_tmp128A=_cycalloc(sizeof(*_tmp128A)),((_tmp128A[0]=((
_tmp128B.tag=15,((_tmp128B.f1=(void*)t,((_tmp128B.f2=_tmp888,((_tmp128B.f3=0,((
_tmp128B.f4=c,_tmp128B)))))))))),_tmp128A))));}{struct Cyc_Core_Opt*_tmp128C;e->topt=((
_tmp128C=_cycalloc(sizeof(*_tmp128C)),((_tmp128C->v=t,_tmp128C))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp88C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL474: {struct Cyc_Absyn_IntType_struct*_tmp88D=(struct Cyc_Absyn_IntType_struct*)
_tmp88C;if(_tmp88D->tag != 6)goto _LL476;}_LL475: goto _LL477;_LL476: {struct Cyc_Absyn_EnumType_struct*
_tmp88E=(struct Cyc_Absyn_EnumType_struct*)_tmp88C;if(_tmp88E->tag != 14)goto
_LL478;}_LL477: goto _LL479;_LL478: {struct Cyc_Absyn_AnonEnumType_struct*_tmp88F=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp88C;if(_tmp88F->tag != 15)goto _LL47A;}
_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_TagType_struct*_tmp890=(struct Cyc_Absyn_TagType_struct*)
_tmp88C;if(_tmp890->tag != 20)goto _LL47C;}_LL47B: return 1;_LL47C: {struct Cyc_Absyn_Evar_struct*
_tmp891=(struct Cyc_Absyn_Evar_struct*)_tmp88C;if(_tmp891->tag != 1)goto _LL47E;}
_LL47D: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL47E:;_LL47F: return 0;_LL473:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp892=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL481: {struct Cyc_Absyn_FloatType_struct*_tmp893=(
struct Cyc_Absyn_FloatType_struct*)_tmp892;if(_tmp893->tag != 7)goto _LL483;}_LL482:
goto _LL484;_LL483: {struct Cyc_Absyn_DoubleType_struct*_tmp894=(struct Cyc_Absyn_DoubleType_struct*)
_tmp892;if(_tmp894->tag != 8)goto _LL485;}_LL484: return 1;_LL485:;_LL486: return 0;
_LL480:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp895=Cyc_Tcutil_compress(t);_LL488: {struct Cyc_Absyn_FnType_struct*
_tmp896=(struct Cyc_Absyn_FnType_struct*)_tmp895;if(_tmp896->tag != 10)goto _LL48A;}
_LL489: return 1;_LL48A:;_LL48B: return 0;_LL487:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp128D;struct _tuple0 _tmp898=(_tmp128D.f1=t1,((_tmp128D.f2=t2,_tmp128D)));
void*_tmp899;int _tmp89B;void*_tmp89C;int _tmp89E;void*_tmp89F;void*_tmp8A1;void*
_tmp8A3;void*_tmp8A5;void*_tmp8A7;enum Cyc_Absyn_Sign _tmp8A9;enum Cyc_Absyn_Size_of
_tmp8AA;void*_tmp8AB;enum Cyc_Absyn_Sign _tmp8AD;enum Cyc_Absyn_Size_of _tmp8AE;
void*_tmp8AF;enum Cyc_Absyn_Size_of _tmp8B1;void*_tmp8B2;enum Cyc_Absyn_Size_of
_tmp8B4;void*_tmp8B5;enum Cyc_Absyn_Sign _tmp8B7;enum Cyc_Absyn_Size_of _tmp8B8;
void*_tmp8B9;enum Cyc_Absyn_Sign _tmp8BB;enum Cyc_Absyn_Size_of _tmp8BC;void*
_tmp8BD;enum Cyc_Absyn_Sign _tmp8BF;enum Cyc_Absyn_Size_of _tmp8C0;void*_tmp8C1;
enum Cyc_Absyn_Sign _tmp8C3;enum Cyc_Absyn_Size_of _tmp8C4;void*_tmp8C5;enum Cyc_Absyn_Size_of
_tmp8C7;void*_tmp8C8;enum Cyc_Absyn_Size_of _tmp8CA;_LL48D: _tmp899=_tmp898.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp89A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp899;if(_tmp89A->tag != 8)goto _LL48F;else{_tmp89B=_tmp89A->f1;}};_tmp89C=
_tmp898.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp89D=(struct Cyc_Absyn_DoubleType_struct*)
_tmp89C;if(_tmp89D->tag != 8)goto _LL48F;else{_tmp89E=_tmp89D->f1;}};_LL48E: if(
_tmp89B)return t1;else{return t2;}_LL48F: _tmp89F=_tmp898.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A0=(struct Cyc_Absyn_DoubleType_struct*)_tmp89F;if(_tmp8A0->tag != 8)goto
_LL491;};_LL490: return t1;_LL491: _tmp8A1=_tmp898.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A2=(struct Cyc_Absyn_DoubleType_struct*)_tmp8A1;if(_tmp8A2->tag != 8)goto
_LL493;};_LL492: return t2;_LL493: _tmp8A3=_tmp898.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp8A4=(struct Cyc_Absyn_FloatType_struct*)_tmp8A3;if(_tmp8A4->tag != 7)goto
_LL495;};_LL494: goto _LL496;_LL495: _tmp8A5=_tmp898.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp8A6=(struct Cyc_Absyn_FloatType_struct*)_tmp8A5;if(_tmp8A6->tag != 7)goto
_LL497;};_LL496: return(void*)& Cyc_Absyn_FloatType_val;_LL497: _tmp8A7=_tmp898.f1;{
struct Cyc_Absyn_IntType_struct*_tmp8A8=(struct Cyc_Absyn_IntType_struct*)_tmp8A7;
if(_tmp8A8->tag != 6)goto _LL499;else{_tmp8A9=_tmp8A8->f1;if(_tmp8A9 != Cyc_Absyn_Unsigned)
goto _LL499;_tmp8AA=_tmp8A8->f2;if(_tmp8AA != Cyc_Absyn_LongLong_sz)goto _LL499;}};
_LL498: goto _LL49A;_LL499: _tmp8AB=_tmp898.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8AC=(struct Cyc_Absyn_IntType_struct*)_tmp8AB;if(_tmp8AC->tag != 6)goto _LL49B;
else{_tmp8AD=_tmp8AC->f1;if(_tmp8AD != Cyc_Absyn_Unsigned)goto _LL49B;_tmp8AE=
_tmp8AC->f2;if(_tmp8AE != Cyc_Absyn_LongLong_sz)goto _LL49B;}};_LL49A: return Cyc_Absyn_ulonglong_typ;
_LL49B: _tmp8AF=_tmp898.f1;{struct Cyc_Absyn_IntType_struct*_tmp8B0=(struct Cyc_Absyn_IntType_struct*)
_tmp8AF;if(_tmp8B0->tag != 6)goto _LL49D;else{_tmp8B1=_tmp8B0->f2;if(_tmp8B1 != Cyc_Absyn_LongLong_sz)
goto _LL49D;}};_LL49C: goto _LL49E;_LL49D: _tmp8B2=_tmp898.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8B3=(struct Cyc_Absyn_IntType_struct*)_tmp8B2;if(_tmp8B3->tag != 6)goto _LL49F;
else{_tmp8B4=_tmp8B3->f2;if(_tmp8B4 != Cyc_Absyn_LongLong_sz)goto _LL49F;}};_LL49E:
return Cyc_Absyn_slonglong_typ;_LL49F: _tmp8B5=_tmp898.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8B6=(struct Cyc_Absyn_IntType_struct*)_tmp8B5;if(_tmp8B6->tag != 6)goto _LL4A1;
else{_tmp8B7=_tmp8B6->f1;if(_tmp8B7 != Cyc_Absyn_Unsigned)goto _LL4A1;_tmp8B8=
_tmp8B6->f2;if(_tmp8B8 != Cyc_Absyn_Long_sz)goto _LL4A1;}};_LL4A0: goto _LL4A2;
_LL4A1: _tmp8B9=_tmp898.f2;{struct Cyc_Absyn_IntType_struct*_tmp8BA=(struct Cyc_Absyn_IntType_struct*)
_tmp8B9;if(_tmp8BA->tag != 6)goto _LL4A3;else{_tmp8BB=_tmp8BA->f1;if(_tmp8BB != Cyc_Absyn_Unsigned)
goto _LL4A3;_tmp8BC=_tmp8BA->f2;if(_tmp8BC != Cyc_Absyn_Long_sz)goto _LL4A3;}};
_LL4A2: return Cyc_Absyn_ulong_typ;_LL4A3: _tmp8BD=_tmp898.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8BE=(struct Cyc_Absyn_IntType_struct*)_tmp8BD;if(_tmp8BE->tag != 6)goto _LL4A5;
else{_tmp8BF=_tmp8BE->f1;if(_tmp8BF != Cyc_Absyn_Unsigned)goto _LL4A5;_tmp8C0=
_tmp8BE->f2;if(_tmp8C0 != Cyc_Absyn_Int_sz)goto _LL4A5;}};_LL4A4: goto _LL4A6;_LL4A5:
_tmp8C1=_tmp898.f2;{struct Cyc_Absyn_IntType_struct*_tmp8C2=(struct Cyc_Absyn_IntType_struct*)
_tmp8C1;if(_tmp8C2->tag != 6)goto _LL4A7;else{_tmp8C3=_tmp8C2->f1;if(_tmp8C3 != Cyc_Absyn_Unsigned)
goto _LL4A7;_tmp8C4=_tmp8C2->f2;if(_tmp8C4 != Cyc_Absyn_Int_sz)goto _LL4A7;}};
_LL4A6: return Cyc_Absyn_uint_typ;_LL4A7: _tmp8C5=_tmp898.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8C6=(struct Cyc_Absyn_IntType_struct*)_tmp8C5;if(_tmp8C6->tag != 6)goto _LL4A9;
else{_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Long_sz)goto _LL4A9;}};_LL4A8:
goto _LL4AA;_LL4A9: _tmp8C8=_tmp898.f2;{struct Cyc_Absyn_IntType_struct*_tmp8C9=(
struct Cyc_Absyn_IntType_struct*)_tmp8C8;if(_tmp8C9->tag != 6)goto _LL4AB;else{
_tmp8CA=_tmp8C9->f2;if(_tmp8CA != Cyc_Absyn_Long_sz)goto _LL4AB;}};_LL4AA: return
Cyc_Absyn_slong_typ;_LL4AB:;_LL4AC: return Cyc_Absyn_sint_typ;_LL48C:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp8CB=e->r;struct Cyc_Core_Opt*_tmp8CD;_LL4AE: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp8CC=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp8CB;if(_tmp8CC->tag != 4)goto
_LL4B0;else{_tmp8CD=_tmp8CC->f2;if(_tmp8CD != 0)goto _LL4B0;}}_LL4AF:{const char*
_tmp1290;void*_tmp128F;(_tmp128F=0,Cyc_Tcutil_warn(e->loc,((_tmp1290="assignment in test",
_tag_dyneither(_tmp1290,sizeof(char),19))),_tag_dyneither(_tmp128F,sizeof(void*),
0)));}goto _LL4AD;_LL4B0:;_LL4B1: goto _LL4AD;_LL4AD:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1291;struct _tuple0 _tmp8D1=(
_tmp1291.f1=c1,((_tmp1291.f2=c2,_tmp1291)));void*_tmp8D2;struct Cyc_Absyn_Kind*
_tmp8D4;void*_tmp8D5;struct Cyc_Absyn_Kind*_tmp8D7;void*_tmp8D8;struct Cyc_Core_Opt*
_tmp8DA;struct Cyc_Core_Opt**_tmp8DB;void*_tmp8DC;struct Cyc_Core_Opt*_tmp8DE;
struct Cyc_Core_Opt**_tmp8DF;void*_tmp8E0;struct Cyc_Core_Opt*_tmp8E2;struct Cyc_Core_Opt**
_tmp8E3;struct Cyc_Absyn_Kind*_tmp8E4;void*_tmp8E5;struct Cyc_Absyn_Kind*_tmp8E7;
void*_tmp8E8;struct Cyc_Absyn_Kind*_tmp8EA;void*_tmp8EB;struct Cyc_Core_Opt*
_tmp8ED;struct Cyc_Core_Opt**_tmp8EE;struct Cyc_Absyn_Kind*_tmp8EF;void*_tmp8F0;
struct Cyc_Core_Opt*_tmp8F2;struct Cyc_Core_Opt**_tmp8F3;struct Cyc_Absyn_Kind*
_tmp8F4;void*_tmp8F5;struct Cyc_Core_Opt*_tmp8F7;struct Cyc_Core_Opt**_tmp8F8;
struct Cyc_Absyn_Kind*_tmp8F9;_LL4B3: _tmp8D2=_tmp8D1.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8D3=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D2;if(_tmp8D3->tag != 0)goto _LL4B5;
else{_tmp8D4=_tmp8D3->f1;}};_tmp8D5=_tmp8D1.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8D6=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D5;if(_tmp8D6->tag != 0)goto _LL4B5;
else{_tmp8D7=_tmp8D6->f1;}};_LL4B4: return _tmp8D4 == _tmp8D7;_LL4B5: _tmp8D8=
_tmp8D1.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8D9=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8D8;if(_tmp8D9->tag != 1)goto _LL4B7;else{_tmp8DA=_tmp8D9->f1;_tmp8DB=(struct
Cyc_Core_Opt**)& _tmp8D9->f1;}};_LL4B6:{struct Cyc_Core_Opt*_tmp1292;*_tmp8DB=((
_tmp1292=_cycalloc(sizeof(*_tmp1292)),((_tmp1292->v=c1,_tmp1292))));}return 1;
_LL4B7: _tmp8DC=_tmp8D1.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8DD=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8DC;if(_tmp8DD->tag != 1)goto _LL4B9;else{_tmp8DE=_tmp8DD->f1;_tmp8DF=(struct
Cyc_Core_Opt**)& _tmp8DD->f1;}};_LL4B8:{struct Cyc_Core_Opt*_tmp1293;*_tmp8DF=((
_tmp1293=_cycalloc(sizeof(*_tmp1293)),((_tmp1293->v=c2,_tmp1293))));}return 1;
_LL4B9: _tmp8E0=_tmp8D1.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp8E1=(struct Cyc_Absyn_Less_kb_struct*)
_tmp8E0;if(_tmp8E1->tag != 2)goto _LL4BB;else{_tmp8E2=_tmp8E1->f1;_tmp8E3=(struct
Cyc_Core_Opt**)& _tmp8E1->f1;_tmp8E4=_tmp8E1->f2;}};_tmp8E5=_tmp8D1.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8E6=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8E5;if(_tmp8E6->tag != 0)goto _LL4BB;
else{_tmp8E7=_tmp8E6->f1;}};_LL4BA: if(Cyc_Tcutil_kind_leq(_tmp8E7,_tmp8E4)){{
struct Cyc_Core_Opt*_tmp1294;*_tmp8E3=((_tmp1294=_cycalloc(sizeof(*_tmp1294)),((
_tmp1294->v=c2,_tmp1294))));}return 1;}else{return 0;}_LL4BB: _tmp8E8=_tmp8D1.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp8E9=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8E8;if(
_tmp8E9->tag != 0)goto _LL4BD;else{_tmp8EA=_tmp8E9->f1;}};_tmp8EB=_tmp8D1.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp8EC=(struct Cyc_Absyn_Less_kb_struct*)_tmp8EB;
if(_tmp8EC->tag != 2)goto _LL4BD;else{_tmp8ED=_tmp8EC->f1;_tmp8EE=(struct Cyc_Core_Opt**)&
_tmp8EC->f1;_tmp8EF=_tmp8EC->f2;}};_LL4BC: if(Cyc_Tcutil_kind_leq(_tmp8EA,_tmp8EF)){{
struct Cyc_Core_Opt*_tmp1295;*_tmp8EE=((_tmp1295=_cycalloc(sizeof(*_tmp1295)),((
_tmp1295->v=c1,_tmp1295))));}return 1;}else{return 0;}_LL4BD: _tmp8F0=_tmp8D1.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp8F1=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F0;
if(_tmp8F1->tag != 2)goto _LL4B2;else{_tmp8F2=_tmp8F1->f1;_tmp8F3=(struct Cyc_Core_Opt**)&
_tmp8F1->f1;_tmp8F4=_tmp8F1->f2;}};_tmp8F5=_tmp8D1.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp8F6=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F5;if(_tmp8F6->tag != 2)goto _LL4B2;
else{_tmp8F7=_tmp8F6->f1;_tmp8F8=(struct Cyc_Core_Opt**)& _tmp8F6->f1;_tmp8F9=
_tmp8F6->f2;}};_LL4BE: if(Cyc_Tcutil_kind_leq(_tmp8F4,_tmp8F9)){{struct Cyc_Core_Opt*
_tmp1296;*_tmp8F8=((_tmp1296=_cycalloc(sizeof(*_tmp1296)),((_tmp1296->v=c1,
_tmp1296))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp8F9,_tmp8F4)){{struct Cyc_Core_Opt*
_tmp1297;*_tmp8F3=((_tmp1297=_cycalloc(sizeof(*_tmp1297)),((_tmp1297->v=c2,
_tmp1297))));}return 1;}else{return 0;}}_LL4B2:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp129B;void*_tmp129A[1];struct Cyc_Int_pa_struct _tmp1299;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1299.tag=1,((_tmp1299.f1=(
unsigned long)i,((_tmp129A[0]=& _tmp1299,Cyc_aprintf(((_tmp129B="#%d",
_tag_dyneither(_tmp129B,sizeof(char),4))),_tag_dyneither(_tmp129A,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp129E;struct Cyc_Absyn_Tvar*_tmp129D;return(
_tmp129D=_cycalloc(sizeof(*_tmp129D)),((_tmp129D->name=((_tmp129E=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp129E[0]=s,_tmp129E)))),((_tmp129D->identity=
- 1,((_tmp129D->kind=k,_tmp129D)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp905=*t->name;return*((const char*)_check_dyneither_subscript(_tmp905,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp12A2;void*
_tmp12A1[1];struct Cyc_String_pa_struct _tmp12A0;(_tmp12A0.tag=0,((_tmp12A0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp12A1[0]=& _tmp12A0,
Cyc_printf(((_tmp12A2="%s",_tag_dyneither(_tmp12A2,sizeof(char),3))),
_tag_dyneither(_tmp12A1,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp12A3;struct _dyneither_ptr _tmp909=Cyc_strconcat(((_tmp12A3="`",
_tag_dyneither(_tmp12A3,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp12A6;char _tmp12A5;struct _dyneither_ptr _tmp12A4;(_tmp12A4=_dyneither_ptr_plus(
_tmp909,sizeof(char),1),((_tmp12A5=*((char*)_check_dyneither_subscript(_tmp12A4,
sizeof(char),0)),((_tmp12A6='t',((_get_dyneither_size(_tmp12A4,sizeof(char))== 1
 && (_tmp12A5 == '\000'  && _tmp12A6 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp12A4.curr)=_tmp12A6)))))));}{struct _dyneither_ptr*_tmp12A7;t->name=((
_tmp12A7=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp12A7[0]=(struct
_dyneither_ptr)_tmp909,_tmp12A7))));};};}struct _tuple23{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple23*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple23*x){
struct Cyc_Core_Opt*_tmp12AA;struct _tuple9*_tmp12A9;return(_tmp12A9=_cycalloc(
sizeof(*_tmp12A9)),((_tmp12A9->f1=(struct Cyc_Core_Opt*)((_tmp12AA=_cycalloc(
sizeof(*_tmp12AA)),((_tmp12AA->v=(*x).f1,_tmp12AA)))),((_tmp12A9->f2=(*x).f2,((
_tmp12A9->f3=(*x).f3,_tmp12A9)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp911=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp12AB;_tmp911=((_tmp12AB=_cycalloc(sizeof(*_tmp12AB)),((_tmp12AB->hd=(void*)
atts->hd,((_tmp12AB->tl=_tmp911,_tmp12AB))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp12B1;struct Cyc_Absyn_FnInfo _tmp12B0;struct Cyc_Absyn_FnType_struct*_tmp12AF;
return(void*)((_tmp12AF=_cycalloc(sizeof(*_tmp12AF)),((_tmp12AF[0]=((_tmp12B1.tag=
10,((_tmp12B1.f1=((_tmp12B0.tvars=fd->tvs,((_tmp12B0.effect=fd->effect,((
_tmp12B0.ret_typ=fd->ret_type,((_tmp12B0.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp12B0.c_varargs=fd->c_varargs,((_tmp12B0.cyc_varargs=fd->cyc_varargs,((
_tmp12B0.rgn_po=fd->rgn_po,((_tmp12B0.attributes=_tmp911,_tmp12B0)))))))))))))))),
_tmp12B1)))),_tmp12AF))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple24{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple24*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple24*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp12B2;return(_tmp12B2=_cycalloc(sizeof(*_tmp12B2)),((_tmp12B2->f1=(*
pr).f1,((_tmp12B2->f2=t,_tmp12B2)))));}struct _tuple25{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};static
struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple25*_tmp12B5;struct _tuple26*_tmp12B4;return(_tmp12B4=
_region_malloc(rgn,sizeof(*_tmp12B4)),((_tmp12B4->f1=((_tmp12B5=_region_malloc(
rgn,sizeof(*_tmp12B5)),((_tmp12B5->f1=(*y).f1,((_tmp12B5->f2=(*y).f2,_tmp12B5)))))),((
_tmp12B4->f2=(*y).f3,_tmp12B4)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple26*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple26*w){
struct _tuple25*_tmp91A;void*_tmp91B;struct _tuple26 _tmp919=*w;_tmp91A=_tmp919.f1;
_tmp91B=_tmp919.f2;{struct Cyc_Core_Opt*_tmp91D;struct Cyc_Absyn_Tqual _tmp91E;
struct _tuple25 _tmp91C=*_tmp91A;_tmp91D=_tmp91C.f1;_tmp91E=_tmp91C.f2;{struct
_tuple9*_tmp12B6;return(_tmp12B6=_cycalloc(sizeof(*_tmp12B6)),((_tmp12B6->f1=
_tmp91D,((_tmp12B6->f2=_tmp91E,((_tmp12B6->f3=_tmp91B,_tmp12B6)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp12B7;return(
_tmp12B7=_cycalloc(sizeof(*_tmp12B7)),((_tmp12B7->name=f->name,((_tmp12B7->tq=f->tq,((
_tmp12B7->type=t,((_tmp12B7->width=f->width,((_tmp12B7->attributes=f->attributes,
_tmp12B7)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp12B8;
return(_tmp12B8=_cycalloc(sizeof(*_tmp12B8)),((_tmp12B8->topt=old->topt,((
_tmp12B8->r=r,((_tmp12B8->loc=old->loc,((_tmp12B8->annot=old->annot,_tmp12B8)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp922=e->r;
enum Cyc_Absyn_Primop _tmp928;struct Cyc_List_List*_tmp929;struct Cyc_Absyn_Exp*
_tmp92B;struct Cyc_Absyn_Exp*_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;struct Cyc_Absyn_Exp*
_tmp92F;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*
_tmp933;struct Cyc_Absyn_Exp*_tmp935;struct Cyc_Absyn_Exp*_tmp936;void*_tmp938;
struct Cyc_Absyn_Exp*_tmp939;int _tmp93A;enum Cyc_Absyn_Coercion _tmp93B;void*
_tmp93D;struct Cyc_Absyn_Exp*_tmp93F;void*_tmp941;void*_tmp942;void*_tmp944;
_LL4C0: {struct Cyc_Absyn_Const_e_struct*_tmp923=(struct Cyc_Absyn_Const_e_struct*)
_tmp922;if(_tmp923->tag != 0)goto _LL4C2;}_LL4C1: goto _LL4C3;_LL4C2: {struct Cyc_Absyn_Enum_e_struct*
_tmp924=(struct Cyc_Absyn_Enum_e_struct*)_tmp922;if(_tmp924->tag != 32)goto _LL4C4;}
_LL4C3: goto _LL4C5;_LL4C4: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp925=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp922;if(_tmp925->tag != 33)goto _LL4C6;}_LL4C5: goto _LL4C7;_LL4C6: {struct Cyc_Absyn_Var_e_struct*
_tmp926=(struct Cyc_Absyn_Var_e_struct*)_tmp922;if(_tmp926->tag != 1)goto _LL4C8;}
_LL4C7: return e;_LL4C8: {struct Cyc_Absyn_Primop_e_struct*_tmp927=(struct Cyc_Absyn_Primop_e_struct*)
_tmp922;if(_tmp927->tag != 3)goto _LL4CA;else{_tmp928=_tmp927->f1;_tmp929=_tmp927->f2;}}
_LL4C9: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp929)== 1){struct
Cyc_Absyn_Exp*_tmp945=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp929))->hd;struct Cyc_Absyn_Exp*_tmp946=Cyc_Tcutil_rsubsexp(r,inst,_tmp945);
if(_tmp946 == _tmp945)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12BE;struct Cyc_Absyn_Exp*
_tmp12BD[1];struct Cyc_Absyn_Primop_e_struct*_tmp12BC;return Cyc_Tcutil_copye(e,(
void*)((_tmp12BC=_cycalloc(sizeof(*_tmp12BC)),((_tmp12BC[0]=((_tmp12BE.tag=3,((
_tmp12BE.f1=_tmp928,((_tmp12BE.f2=((_tmp12BD[0]=_tmp946,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp12BD,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp12BE)))))),_tmp12BC)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp929)== 2){struct Cyc_Absyn_Exp*_tmp94A=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp929))->hd;struct Cyc_Absyn_Exp*_tmp94B=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp929->tl))->hd;struct
Cyc_Absyn_Exp*_tmp94C=Cyc_Tcutil_rsubsexp(r,inst,_tmp94A);struct Cyc_Absyn_Exp*
_tmp94D=Cyc_Tcutil_rsubsexp(r,inst,_tmp94B);if(_tmp94C == _tmp94A  && _tmp94D == 
_tmp94B)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12C4;struct Cyc_Absyn_Exp*
_tmp12C3[2];struct Cyc_Absyn_Primop_e_struct*_tmp12C2;return Cyc_Tcutil_copye(e,(
void*)((_tmp12C2=_cycalloc(sizeof(*_tmp12C2)),((_tmp12C2[0]=((_tmp12C4.tag=3,((
_tmp12C4.f1=_tmp928,((_tmp12C4.f2=((_tmp12C3[1]=_tmp94D,((_tmp12C3[0]=_tmp94C,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp12C3,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp12C4)))))),_tmp12C2)))));};}
else{const char*_tmp12C7;void*_tmp12C6;return(_tmp12C6=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C7="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp12C7,sizeof(char),34))),_tag_dyneither(_tmp12C6,sizeof(void*),
0)));}}_LL4CA: {struct Cyc_Absyn_Conditional_e_struct*_tmp92A=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp922;if(_tmp92A->tag != 6)goto _LL4CC;else{_tmp92B=_tmp92A->f1;_tmp92C=_tmp92A->f2;
_tmp92D=_tmp92A->f3;}}_LL4CB: {struct Cyc_Absyn_Exp*_tmp953=Cyc_Tcutil_rsubsexp(r,
inst,_tmp92B);struct Cyc_Absyn_Exp*_tmp954=Cyc_Tcutil_rsubsexp(r,inst,_tmp92C);
struct Cyc_Absyn_Exp*_tmp955=Cyc_Tcutil_rsubsexp(r,inst,_tmp92D);if((_tmp953 == 
_tmp92B  && _tmp954 == _tmp92C) && _tmp955 == _tmp92D)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp12CA;struct Cyc_Absyn_Conditional_e_struct*_tmp12C9;return Cyc_Tcutil_copye(e,(
void*)((_tmp12C9=_cycalloc(sizeof(*_tmp12C9)),((_tmp12C9[0]=((_tmp12CA.tag=6,((
_tmp12CA.f1=_tmp953,((_tmp12CA.f2=_tmp954,((_tmp12CA.f3=_tmp955,_tmp12CA)))))))),
_tmp12C9)))));};}_LL4CC: {struct Cyc_Absyn_And_e_struct*_tmp92E=(struct Cyc_Absyn_And_e_struct*)
_tmp922;if(_tmp92E->tag != 7)goto _LL4CE;else{_tmp92F=_tmp92E->f1;_tmp930=_tmp92E->f2;}}
_LL4CD: {struct Cyc_Absyn_Exp*_tmp958=Cyc_Tcutil_rsubsexp(r,inst,_tmp92F);struct
Cyc_Absyn_Exp*_tmp959=Cyc_Tcutil_rsubsexp(r,inst,_tmp930);if(_tmp958 == _tmp92F
 && _tmp959 == _tmp930)return e;{struct Cyc_Absyn_And_e_struct _tmp12CD;struct Cyc_Absyn_And_e_struct*
_tmp12CC;return Cyc_Tcutil_copye(e,(void*)((_tmp12CC=_cycalloc(sizeof(*_tmp12CC)),((
_tmp12CC[0]=((_tmp12CD.tag=7,((_tmp12CD.f1=_tmp958,((_tmp12CD.f2=_tmp959,
_tmp12CD)))))),_tmp12CC)))));};}_LL4CE: {struct Cyc_Absyn_Or_e_struct*_tmp931=(
struct Cyc_Absyn_Or_e_struct*)_tmp922;if(_tmp931->tag != 8)goto _LL4D0;else{_tmp932=
_tmp931->f1;_tmp933=_tmp931->f2;}}_LL4CF: {struct Cyc_Absyn_Exp*_tmp95C=Cyc_Tcutil_rsubsexp(
r,inst,_tmp932);struct Cyc_Absyn_Exp*_tmp95D=Cyc_Tcutil_rsubsexp(r,inst,_tmp933);
if(_tmp95C == _tmp932  && _tmp95D == _tmp933)return e;{struct Cyc_Absyn_Or_e_struct
_tmp12D0;struct Cyc_Absyn_Or_e_struct*_tmp12CF;return Cyc_Tcutil_copye(e,(void*)((
_tmp12CF=_cycalloc(sizeof(*_tmp12CF)),((_tmp12CF[0]=((_tmp12D0.tag=8,((_tmp12D0.f1=
_tmp95C,((_tmp12D0.f2=_tmp95D,_tmp12D0)))))),_tmp12CF)))));};}_LL4D0: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp934=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp922;if(_tmp934->tag != 9)goto _LL4D2;
else{_tmp935=_tmp934->f1;_tmp936=_tmp934->f2;}}_LL4D1: {struct Cyc_Absyn_Exp*
_tmp960=Cyc_Tcutil_rsubsexp(r,inst,_tmp935);struct Cyc_Absyn_Exp*_tmp961=Cyc_Tcutil_rsubsexp(
r,inst,_tmp936);if(_tmp960 == _tmp935  && _tmp961 == _tmp936)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp12D3;struct Cyc_Absyn_SeqExp_e_struct*_tmp12D2;return Cyc_Tcutil_copye(e,(void*)((
_tmp12D2=_cycalloc(sizeof(*_tmp12D2)),((_tmp12D2[0]=((_tmp12D3.tag=9,((_tmp12D3.f1=
_tmp960,((_tmp12D3.f2=_tmp961,_tmp12D3)))))),_tmp12D2)))));};}_LL4D2: {struct Cyc_Absyn_Cast_e_struct*
_tmp937=(struct Cyc_Absyn_Cast_e_struct*)_tmp922;if(_tmp937->tag != 15)goto _LL4D4;
else{_tmp938=(void*)_tmp937->f1;_tmp939=_tmp937->f2;_tmp93A=_tmp937->f3;_tmp93B=
_tmp937->f4;}}_LL4D3: {struct Cyc_Absyn_Exp*_tmp964=Cyc_Tcutil_rsubsexp(r,inst,
_tmp939);void*_tmp965=Cyc_Tcutil_rsubstitute(r,inst,_tmp938);if(_tmp964 == 
_tmp939  && _tmp965 == _tmp938)return e;{struct Cyc_Absyn_Cast_e_struct _tmp12D6;
struct Cyc_Absyn_Cast_e_struct*_tmp12D5;return Cyc_Tcutil_copye(e,(void*)((
_tmp12D5=_cycalloc(sizeof(*_tmp12D5)),((_tmp12D5[0]=((_tmp12D6.tag=15,((_tmp12D6.f1=(
void*)_tmp965,((_tmp12D6.f2=_tmp964,((_tmp12D6.f3=_tmp93A,((_tmp12D6.f4=_tmp93B,
_tmp12D6)))))))))),_tmp12D5)))));};}_LL4D4: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp93C=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp922;if(_tmp93C->tag != 18)goto
_LL4D6;else{_tmp93D=(void*)_tmp93C->f1;}}_LL4D5: {void*_tmp968=Cyc_Tcutil_rsubstitute(
r,inst,_tmp93D);if(_tmp968 == _tmp93D)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp12D9;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp12D8;return Cyc_Tcutil_copye(e,(
void*)((_tmp12D8=_cycalloc(sizeof(*_tmp12D8)),((_tmp12D8[0]=((_tmp12D9.tag=18,((
_tmp12D9.f1=(void*)_tmp968,_tmp12D9)))),_tmp12D8)))));};}_LL4D6: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp93E=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp922;if(_tmp93E->tag != 19)goto
_LL4D8;else{_tmp93F=_tmp93E->f1;}}_LL4D7: {struct Cyc_Absyn_Exp*_tmp96B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp93F);if(_tmp96B == _tmp93F)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp12DC;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp12DB;return Cyc_Tcutil_copye(e,(
void*)((_tmp12DB=_cycalloc(sizeof(*_tmp12DB)),((_tmp12DB[0]=((_tmp12DC.tag=19,((
_tmp12DC.f1=_tmp96B,_tmp12DC)))),_tmp12DB)))));};}_LL4D8: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp940=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp922;if(_tmp940->tag != 20)goto
_LL4DA;else{_tmp941=(void*)_tmp940->f1;_tmp942=(void*)_tmp940->f2;}}_LL4D9: {
void*_tmp96E=Cyc_Tcutil_rsubstitute(r,inst,_tmp941);if(_tmp96E == _tmp941)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp12DF;struct Cyc_Absyn_Offsetof_e_struct*
_tmp12DE;return Cyc_Tcutil_copye(e,(void*)((_tmp12DE=_cycalloc(sizeof(*_tmp12DE)),((
_tmp12DE[0]=((_tmp12DF.tag=20,((_tmp12DF.f1=(void*)_tmp96E,((_tmp12DF.f2=(void*)
_tmp942,_tmp12DF)))))),_tmp12DE)))));};}_LL4DA: {struct Cyc_Absyn_Valueof_e_struct*
_tmp943=(struct Cyc_Absyn_Valueof_e_struct*)_tmp922;if(_tmp943->tag != 39)goto
_LL4DC;else{_tmp944=(void*)_tmp943->f1;}}_LL4DB: {void*_tmp971=Cyc_Tcutil_rsubstitute(
r,inst,_tmp944);if(_tmp971 == _tmp944)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp12E2;struct Cyc_Absyn_Valueof_e_struct*_tmp12E1;return Cyc_Tcutil_copye(e,(
void*)((_tmp12E1=_cycalloc(sizeof(*_tmp12E1)),((_tmp12E1[0]=((_tmp12E2.tag=39,((
_tmp12E2.f1=(void*)_tmp971,_tmp12E2)))),_tmp12E1)))));};}_LL4DC:;_LL4DD: {const
char*_tmp12E5;void*_tmp12E4;return(_tmp12E4=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E5="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp12E5,sizeof(char),46))),_tag_dyneither(_tmp12E4,sizeof(void*),
0)));}_LL4BF:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp976=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp978;
struct Cyc_Absyn_AggrInfo _tmp97A;union Cyc_Absyn_AggrInfoU _tmp97B;struct Cyc_List_List*
_tmp97C;struct Cyc_Absyn_DatatypeInfo _tmp97E;union Cyc_Absyn_DatatypeInfoU _tmp97F;
struct Cyc_List_List*_tmp980;struct Cyc_Absyn_DatatypeFieldInfo _tmp982;union Cyc_Absyn_DatatypeFieldInfoU
_tmp983;struct Cyc_List_List*_tmp984;struct _tuple2*_tmp986;struct Cyc_List_List*
_tmp987;struct Cyc_Absyn_Typedefdecl*_tmp988;void**_tmp989;struct Cyc_Absyn_ArrayInfo
_tmp98B;void*_tmp98C;struct Cyc_Absyn_Tqual _tmp98D;struct Cyc_Absyn_Exp*_tmp98E;
union Cyc_Absyn_Constraint*_tmp98F;struct Cyc_Position_Segment*_tmp990;struct Cyc_Absyn_PtrInfo
_tmp992;void*_tmp993;struct Cyc_Absyn_Tqual _tmp994;struct Cyc_Absyn_PtrAtts _tmp995;
void*_tmp996;union Cyc_Absyn_Constraint*_tmp997;union Cyc_Absyn_Constraint*_tmp998;
union Cyc_Absyn_Constraint*_tmp999;struct Cyc_Absyn_FnInfo _tmp99B;struct Cyc_List_List*
_tmp99C;struct Cyc_Core_Opt*_tmp99D;void*_tmp99E;struct Cyc_List_List*_tmp99F;int
_tmp9A0;struct Cyc_Absyn_VarargInfo*_tmp9A1;struct Cyc_List_List*_tmp9A2;struct Cyc_List_List*
_tmp9A3;struct Cyc_List_List*_tmp9A5;enum Cyc_Absyn_AggrKind _tmp9A7;struct Cyc_List_List*
_tmp9A8;struct Cyc_Core_Opt*_tmp9AA;void*_tmp9AC;void*_tmp9AE;void*_tmp9AF;void*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;void*_tmp9BD;void*_tmp9BF;struct Cyc_List_List*
_tmp9C1;_LL4DF: {struct Cyc_Absyn_VarType_struct*_tmp977=(struct Cyc_Absyn_VarType_struct*)
_tmp976;if(_tmp977->tag != 2)goto _LL4E1;else{_tmp978=_tmp977->f1;}}_LL4E0: {
struct _handler_cons _tmp9C2;_push_handler(& _tmp9C2);{int _tmp9C4=0;if(setjmp(
_tmp9C2.handler))_tmp9C4=1;if(!_tmp9C4){{void*_tmp9C5=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp978);_npop_handler(0);return
_tmp9C5;};_pop_handler();}else{void*_tmp9C3=(void*)_exn_thrown;void*_tmp9C7=
_tmp9C3;_LL514: {struct Cyc_Core_Not_found_struct*_tmp9C8=(struct Cyc_Core_Not_found_struct*)
_tmp9C7;if(_tmp9C8->tag != Cyc_Core_Not_found)goto _LL516;}_LL515: return t;_LL516:;
_LL517:(void)_throw(_tmp9C7);_LL513:;}};}_LL4E1: {struct Cyc_Absyn_AggrType_struct*
_tmp979=(struct Cyc_Absyn_AggrType_struct*)_tmp976;if(_tmp979->tag != 12)goto
_LL4E3;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp97A.aggr_info;_tmp97C=_tmp97A.targs;}}
_LL4E2: {struct Cyc_List_List*_tmp9C9=Cyc_Tcutil_substs(rgn,inst,_tmp97C);struct
Cyc_Absyn_AggrType_struct _tmp12EB;struct Cyc_Absyn_AggrInfo _tmp12EA;struct Cyc_Absyn_AggrType_struct*
_tmp12E9;return _tmp9C9 == _tmp97C?t:(void*)((_tmp12E9=_cycalloc(sizeof(*_tmp12E9)),((
_tmp12E9[0]=((_tmp12EB.tag=12,((_tmp12EB.f1=((_tmp12EA.aggr_info=_tmp97B,((
_tmp12EA.targs=_tmp9C9,_tmp12EA)))),_tmp12EB)))),_tmp12E9))));}_LL4E3: {struct
Cyc_Absyn_DatatypeType_struct*_tmp97D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp976;if(_tmp97D->tag != 3)goto _LL4E5;else{_tmp97E=_tmp97D->f1;_tmp97F=_tmp97E.datatype_info;
_tmp980=_tmp97E.targs;}}_LL4E4: {struct Cyc_List_List*_tmp9CD=Cyc_Tcutil_substs(
rgn,inst,_tmp980);struct Cyc_Absyn_DatatypeType_struct _tmp12F1;struct Cyc_Absyn_DatatypeInfo
_tmp12F0;struct Cyc_Absyn_DatatypeType_struct*_tmp12EF;return _tmp9CD == _tmp980?t:(
void*)((_tmp12EF=_cycalloc(sizeof(*_tmp12EF)),((_tmp12EF[0]=((_tmp12F1.tag=3,((
_tmp12F1.f1=((_tmp12F0.datatype_info=_tmp97F,((_tmp12F0.targs=_tmp9CD,_tmp12F0)))),
_tmp12F1)))),_tmp12EF))));}_LL4E5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp981=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp976;if(_tmp981->tag != 4)
goto _LL4E7;else{_tmp982=_tmp981->f1;_tmp983=_tmp982.field_info;_tmp984=_tmp982.targs;}}
_LL4E6: {struct Cyc_List_List*_tmp9D1=Cyc_Tcutil_substs(rgn,inst,_tmp984);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp12F7;struct Cyc_Absyn_DatatypeFieldInfo
_tmp12F6;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp12F5;return _tmp9D1 == 
_tmp984?t:(void*)((_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((_tmp12F5[0]=((
_tmp12F7.tag=4,((_tmp12F7.f1=((_tmp12F6.field_info=_tmp983,((_tmp12F6.targs=
_tmp9D1,_tmp12F6)))),_tmp12F7)))),_tmp12F5))));}_LL4E7: {struct Cyc_Absyn_TypedefType_struct*
_tmp985=(struct Cyc_Absyn_TypedefType_struct*)_tmp976;if(_tmp985->tag != 18)goto
_LL4E9;else{_tmp986=_tmp985->f1;_tmp987=_tmp985->f2;_tmp988=_tmp985->f3;_tmp989=
_tmp985->f4;}}_LL4E8: {struct Cyc_List_List*_tmp9D5=Cyc_Tcutil_substs(rgn,inst,
_tmp987);struct Cyc_Absyn_TypedefType_struct _tmp12FA;struct Cyc_Absyn_TypedefType_struct*
_tmp12F9;return _tmp9D5 == _tmp987?t:(void*)((_tmp12F9=_cycalloc(sizeof(*_tmp12F9)),((
_tmp12F9[0]=((_tmp12FA.tag=18,((_tmp12FA.f1=_tmp986,((_tmp12FA.f2=_tmp9D5,((
_tmp12FA.f3=_tmp988,((_tmp12FA.f4=_tmp989,_tmp12FA)))))))))),_tmp12F9))));}
_LL4E9: {struct Cyc_Absyn_ArrayType_struct*_tmp98A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp976;if(_tmp98A->tag != 9)goto _LL4EB;else{_tmp98B=_tmp98A->f1;_tmp98C=_tmp98B.elt_type;
_tmp98D=_tmp98B.tq;_tmp98E=_tmp98B.num_elts;_tmp98F=_tmp98B.zero_term;_tmp990=
_tmp98B.zt_loc;}}_LL4EA: {void*_tmp9D8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp98C);
struct Cyc_Absyn_ArrayType_struct _tmp1300;struct Cyc_Absyn_ArrayInfo _tmp12FF;
struct Cyc_Absyn_ArrayType_struct*_tmp12FE;return _tmp9D8 == _tmp98C?t:(void*)((
_tmp12FE=_cycalloc(sizeof(*_tmp12FE)),((_tmp12FE[0]=((_tmp1300.tag=9,((_tmp1300.f1=((
_tmp12FF.elt_type=_tmp9D8,((_tmp12FF.tq=_tmp98D,((_tmp12FF.num_elts=_tmp98E,((
_tmp12FF.zero_term=_tmp98F,((_tmp12FF.zt_loc=_tmp990,_tmp12FF)))))))))),_tmp1300)))),
_tmp12FE))));}_LL4EB: {struct Cyc_Absyn_PointerType_struct*_tmp991=(struct Cyc_Absyn_PointerType_struct*)
_tmp976;if(_tmp991->tag != 5)goto _LL4ED;else{_tmp992=_tmp991->f1;_tmp993=_tmp992.elt_typ;
_tmp994=_tmp992.elt_tq;_tmp995=_tmp992.ptr_atts;_tmp996=_tmp995.rgn;_tmp997=
_tmp995.nullable;_tmp998=_tmp995.bounds;_tmp999=_tmp995.zero_term;}}_LL4EC: {
void*_tmp9DC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp993);void*_tmp9DD=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp996);union Cyc_Absyn_Constraint*_tmp9DE=_tmp998;{void*_tmp9DF=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp998);struct Cyc_Absyn_Exp*_tmp9E1;_LL519: {struct Cyc_Absyn_Upper_b_struct*
_tmp9E0=(struct Cyc_Absyn_Upper_b_struct*)_tmp9DF;if(_tmp9E0->tag != 1)goto _LL51B;
else{_tmp9E1=_tmp9E0->f1;}}_LL51A: {struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp9E1);if(_tmp9E2 != _tmp9E1){struct Cyc_Absyn_Upper_b_struct _tmp1303;
struct Cyc_Absyn_Upper_b_struct*_tmp1302;_tmp9DE=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1302=_cycalloc(sizeof(*_tmp1302)),((
_tmp1302[0]=((_tmp1303.tag=1,((_tmp1303.f1=_tmp9E2,_tmp1303)))),_tmp1302)))));}
goto _LL518;}_LL51B:;_LL51C: goto _LL518;_LL518:;}if((_tmp9DC == _tmp993  && _tmp9DD
== _tmp996) && _tmp9DE == _tmp998)return t;{struct Cyc_Absyn_PointerType_struct
_tmp130D;struct Cyc_Absyn_PtrAtts _tmp130C;struct Cyc_Absyn_PtrInfo _tmp130B;struct
Cyc_Absyn_PointerType_struct*_tmp130A;return(void*)((_tmp130A=_cycalloc(sizeof(*
_tmp130A)),((_tmp130A[0]=((_tmp130D.tag=5,((_tmp130D.f1=((_tmp130B.elt_typ=
_tmp9DC,((_tmp130B.elt_tq=_tmp994,((_tmp130B.ptr_atts=((_tmp130C.rgn=_tmp9DD,((
_tmp130C.nullable=_tmp997,((_tmp130C.bounds=_tmp9DE,((_tmp130C.zero_term=_tmp999,((
_tmp130C.ptrloc=0,_tmp130C)))))))))),_tmp130B)))))),_tmp130D)))),_tmp130A))));};}
_LL4ED: {struct Cyc_Absyn_FnType_struct*_tmp99A=(struct Cyc_Absyn_FnType_struct*)
_tmp976;if(_tmp99A->tag != 10)goto _LL4EF;else{_tmp99B=_tmp99A->f1;_tmp99C=_tmp99B.tvars;
_tmp99D=_tmp99B.effect;_tmp99E=_tmp99B.ret_typ;_tmp99F=_tmp99B.args;_tmp9A0=
_tmp99B.c_varargs;_tmp9A1=_tmp99B.cyc_varargs;_tmp9A2=_tmp99B.rgn_po;_tmp9A3=
_tmp99B.attributes;}}_LL4EE:{struct Cyc_List_List*_tmp9E9=_tmp99C;for(0;_tmp9E9 != 
0;_tmp9E9=_tmp9E9->tl){struct _tuple15*_tmp1317;struct Cyc_Absyn_VarType_struct
_tmp1316;struct Cyc_Absyn_VarType_struct*_tmp1315;struct Cyc_List_List*_tmp1314;
inst=((_tmp1314=_region_malloc(rgn,sizeof(*_tmp1314)),((_tmp1314->hd=((_tmp1317=
_region_malloc(rgn,sizeof(*_tmp1317)),((_tmp1317->f1=(struct Cyc_Absyn_Tvar*)
_tmp9E9->hd,((_tmp1317->f2=(void*)((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((
_tmp1315[0]=((_tmp1316.tag=2,((_tmp1316.f1=(struct Cyc_Absyn_Tvar*)_tmp9E9->hd,
_tmp1316)))),_tmp1315)))),_tmp1317)))))),((_tmp1314->tl=inst,_tmp1314))))));}}{
struct Cyc_List_List*_tmp9EF;struct Cyc_List_List*_tmp9F0;struct _tuple1 _tmp9EE=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp99F));
_tmp9EF=_tmp9EE.f1;_tmp9F0=_tmp9EE.f2;{struct Cyc_List_List*_tmp9F1=Cyc_Tcutil_substs(
rgn,inst,_tmp9F0);struct Cyc_List_List*_tmp9F2=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9EF,_tmp9F1));struct Cyc_Core_Opt*
eff2;if(_tmp99D == 0)eff2=0;else{void*_tmp9F3=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp99D->v);if(_tmp9F3 == (void*)_tmp99D->v)eff2=_tmp99D;else{struct Cyc_Core_Opt*
_tmp1318;eff2=((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((_tmp1318->v=_tmp9F3,
_tmp1318))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp9A1 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp9F6;struct Cyc_Absyn_Tqual _tmp9F7;void*
_tmp9F8;int _tmp9F9;struct Cyc_Absyn_VarargInfo _tmp9F5=*_tmp9A1;_tmp9F6=_tmp9F5.name;
_tmp9F7=_tmp9F5.tq;_tmp9F8=_tmp9F5.type;_tmp9F9=_tmp9F5.inject;{void*_tmp9FA=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9F8);if(_tmp9FA == _tmp9F8)cyc_varargs2=_tmp9A1;else{struct Cyc_Absyn_VarargInfo*
_tmp1319;cyc_varargs2=((_tmp1319=_cycalloc(sizeof(*_tmp1319)),((_tmp1319->name=
_tmp9F6,((_tmp1319->tq=_tmp9F7,((_tmp1319->type=_tmp9FA,((_tmp1319->inject=
_tmp9F9,_tmp1319))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp9FD;struct Cyc_List_List*_tmp9FE;struct _tuple1 _tmp9FC=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp9A2);_tmp9FD=_tmp9FC.f1;_tmp9FE=_tmp9FC.f2;{struct Cyc_List_List*
_tmp9FF=Cyc_Tcutil_substs(rgn,inst,_tmp9FD);struct Cyc_List_List*_tmpA00=Cyc_Tcutil_substs(
rgn,inst,_tmp9FE);if(_tmp9FF == _tmp9FD  && _tmpA00 == _tmp9FE)rgn_po2=_tmp9A2;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp9FF,_tmpA00);}{struct Cyc_Absyn_FnType_struct _tmp131F;struct
Cyc_Absyn_FnInfo _tmp131E;struct Cyc_Absyn_FnType_struct*_tmp131D;return(void*)((
_tmp131D=_cycalloc(sizeof(*_tmp131D)),((_tmp131D[0]=((_tmp131F.tag=10,((_tmp131F.f1=((
_tmp131E.tvars=_tmp99C,((_tmp131E.effect=eff2,((_tmp131E.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp99E),((_tmp131E.args=_tmp9F2,((_tmp131E.c_varargs=_tmp9A0,((
_tmp131E.cyc_varargs=cyc_varargs2,((_tmp131E.rgn_po=rgn_po2,((_tmp131E.attributes=
_tmp9A3,_tmp131E)))))))))))))))),_tmp131F)))),_tmp131D))));};};};};};};_LL4EF: {
struct Cyc_Absyn_TupleType_struct*_tmp9A4=(struct Cyc_Absyn_TupleType_struct*)
_tmp976;if(_tmp9A4->tag != 11)goto _LL4F1;else{_tmp9A5=_tmp9A4->f1;}}_LL4F0: {
struct Cyc_List_List*_tmpA04=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp9A5);struct Cyc_List_List*_tmpA05=Cyc_Tcutil_substs(rgn,inst,_tmpA04);if(
_tmpA05 == _tmpA04)return t;{struct Cyc_List_List*_tmpA06=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp9A5,_tmpA05);struct Cyc_Absyn_TupleType_struct
_tmp1322;struct Cyc_Absyn_TupleType_struct*_tmp1321;return(void*)((_tmp1321=
_cycalloc(sizeof(*_tmp1321)),((_tmp1321[0]=((_tmp1322.tag=11,((_tmp1322.f1=
_tmpA06,_tmp1322)))),_tmp1321))));};}_LL4F1: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9A6=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp976;if(_tmp9A6->tag != 13)goto
_LL4F3;else{_tmp9A7=_tmp9A6->f1;_tmp9A8=_tmp9A6->f2;}}_LL4F2: {struct Cyc_List_List*
_tmpA09=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp9A8);struct
Cyc_List_List*_tmpA0A=Cyc_Tcutil_substs(rgn,inst,_tmpA09);if(_tmpA0A == _tmpA09)
return t;{struct Cyc_List_List*_tmpA0B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp9A8,_tmpA0A);struct Cyc_Absyn_AnonAggrType_struct
_tmp1325;struct Cyc_Absyn_AnonAggrType_struct*_tmp1324;return(void*)((_tmp1324=
_cycalloc(sizeof(*_tmp1324)),((_tmp1324[0]=((_tmp1325.tag=13,((_tmp1325.f1=
_tmp9A7,((_tmp1325.f2=_tmpA0B,_tmp1325)))))),_tmp1324))));};}_LL4F3: {struct Cyc_Absyn_Evar_struct*
_tmp9A9=(struct Cyc_Absyn_Evar_struct*)_tmp976;if(_tmp9A9->tag != 1)goto _LL4F5;
else{_tmp9AA=_tmp9A9->f2;}}_LL4F4: if(_tmp9AA != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp9AA->v);else{return t;}_LL4F5: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp9AB=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp976;if(_tmp9AB->tag != 16)goto
_LL4F7;else{_tmp9AC=(void*)_tmp9AB->f1;}}_LL4F6: {void*_tmpA0E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9AC);struct Cyc_Absyn_RgnHandleType_struct _tmp1328;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1327;return _tmpA0E == _tmp9AC?t:(void*)((_tmp1327=_cycalloc(sizeof(*_tmp1327)),((
_tmp1327[0]=((_tmp1328.tag=16,((_tmp1328.f1=(void*)_tmpA0E,_tmp1328)))),_tmp1327))));}
_LL4F7: {struct Cyc_Absyn_DynRgnType_struct*_tmp9AD=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp976;if(_tmp9AD->tag != 17)goto _LL4F9;else{_tmp9AE=(void*)_tmp9AD->f1;_tmp9AF=(
void*)_tmp9AD->f2;}}_LL4F8: {void*_tmpA11=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp9AE);void*_tmpA12=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9AF);struct Cyc_Absyn_DynRgnType_struct
_tmp132B;struct Cyc_Absyn_DynRgnType_struct*_tmp132A;return _tmpA11 == _tmp9AE  && 
_tmpA12 == _tmp9AF?t:(void*)((_tmp132A=_cycalloc(sizeof(*_tmp132A)),((_tmp132A[0]=((
_tmp132B.tag=17,((_tmp132B.f1=(void*)_tmpA11,((_tmp132B.f2=(void*)_tmpA12,
_tmp132B)))))),_tmp132A))));}_LL4F9: {struct Cyc_Absyn_TagType_struct*_tmp9B0=(
struct Cyc_Absyn_TagType_struct*)_tmp976;if(_tmp9B0->tag != 20)goto _LL4FB;else{
_tmp9B1=(void*)_tmp9B0->f1;}}_LL4FA: {void*_tmpA15=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp9B1);struct Cyc_Absyn_TagType_struct _tmp132E;struct Cyc_Absyn_TagType_struct*
_tmp132D;return _tmpA15 == _tmp9B1?t:(void*)((_tmp132D=_cycalloc(sizeof(*_tmp132D)),((
_tmp132D[0]=((_tmp132E.tag=20,((_tmp132E.f1=(void*)_tmpA15,_tmp132E)))),_tmp132D))));}
_LL4FB: {struct Cyc_Absyn_ValueofType_struct*_tmp9B2=(struct Cyc_Absyn_ValueofType_struct*)
_tmp976;if(_tmp9B2->tag != 19)goto _LL4FD;else{_tmp9B3=_tmp9B2->f1;}}_LL4FC: {
struct Cyc_Absyn_Exp*_tmpA18=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp9B3);struct Cyc_Absyn_ValueofType_struct
_tmp1331;struct Cyc_Absyn_ValueofType_struct*_tmp1330;return _tmpA18 == _tmp9B3?t:(
void*)((_tmp1330=_cycalloc(sizeof(*_tmp1330)),((_tmp1330[0]=((_tmp1331.tag=19,((
_tmp1331.f1=_tmpA18,_tmp1331)))),_tmp1330))));}_LL4FD: {struct Cyc_Absyn_EnumType_struct*
_tmp9B4=(struct Cyc_Absyn_EnumType_struct*)_tmp976;if(_tmp9B4->tag != 14)goto
_LL4FF;}_LL4FE: goto _LL500;_LL4FF: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9B5=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp976;if(_tmp9B5->tag != 15)goto _LL501;}
_LL500: goto _LL502;_LL501: {struct Cyc_Absyn_VoidType_struct*_tmp9B6=(struct Cyc_Absyn_VoidType_struct*)
_tmp976;if(_tmp9B6->tag != 0)goto _LL503;}_LL502: goto _LL504;_LL503: {struct Cyc_Absyn_IntType_struct*
_tmp9B7=(struct Cyc_Absyn_IntType_struct*)_tmp976;if(_tmp9B7->tag != 6)goto _LL505;}
_LL504: goto _LL506;_LL505: {struct Cyc_Absyn_FloatType_struct*_tmp9B8=(struct Cyc_Absyn_FloatType_struct*)
_tmp976;if(_tmp9B8->tag != 7)goto _LL507;}_LL506: goto _LL508;_LL507: {struct Cyc_Absyn_DoubleType_struct*
_tmp9B9=(struct Cyc_Absyn_DoubleType_struct*)_tmp976;if(_tmp9B9->tag != 8)goto
_LL509;}_LL508: goto _LL50A;_LL509: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9BA=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp976;if(_tmp9BA->tag != 22)goto _LL50B;}
_LL50A: goto _LL50C;_LL50B: {struct Cyc_Absyn_HeapRgn_struct*_tmp9BB=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp976;if(_tmp9BB->tag != 21)goto _LL50D;}_LL50C: return t;_LL50D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp9BC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp976;if(_tmp9BC->tag != 25)goto _LL50F;
else{_tmp9BD=(void*)_tmp9BC->f1;}}_LL50E: {void*_tmpA1B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9BD);struct Cyc_Absyn_RgnsEff_struct _tmp1334;struct Cyc_Absyn_RgnsEff_struct*
_tmp1333;return _tmpA1B == _tmp9BD?t:(void*)((_tmp1333=_cycalloc(sizeof(*_tmp1333)),((
_tmp1333[0]=((_tmp1334.tag=25,((_tmp1334.f1=(void*)_tmpA1B,_tmp1334)))),_tmp1333))));}
_LL50F: {struct Cyc_Absyn_AccessEff_struct*_tmp9BE=(struct Cyc_Absyn_AccessEff_struct*)
_tmp976;if(_tmp9BE->tag != 23)goto _LL511;else{_tmp9BF=(void*)_tmp9BE->f1;}}_LL510: {
void*_tmpA1E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9BF);struct Cyc_Absyn_AccessEff_struct
_tmp1337;struct Cyc_Absyn_AccessEff_struct*_tmp1336;return _tmpA1E == _tmp9BF?t:(
void*)((_tmp1336=_cycalloc(sizeof(*_tmp1336)),((_tmp1336[0]=((_tmp1337.tag=23,((
_tmp1337.f1=(void*)_tmpA1E,_tmp1337)))),_tmp1336))));}_LL511: {struct Cyc_Absyn_JoinEff_struct*
_tmp9C0=(struct Cyc_Absyn_JoinEff_struct*)_tmp976;if(_tmp9C0->tag != 24)goto _LL4DE;
else{_tmp9C1=_tmp9C0->f1;}}_LL512: {struct Cyc_List_List*_tmpA21=Cyc_Tcutil_substs(
rgn,inst,_tmp9C1);struct Cyc_Absyn_JoinEff_struct _tmp133A;struct Cyc_Absyn_JoinEff_struct*
_tmp1339;return _tmpA21 == _tmp9C1?t:(void*)((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((
_tmp1339[0]=((_tmp133A.tag=24,((_tmp133A.f1=_tmpA21,_tmp133A)))),_tmp1339))));}
_LL4DE:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmpA24=(void*)ts->hd;struct Cyc_List_List*_tmpA25=ts->tl;void*
_tmpA26=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA24);struct Cyc_List_List*_tmpA27=Cyc_Tcutil_substs(
rgn,inst,_tmpA25);if(_tmpA24 == _tmpA26  && _tmpA25 == _tmpA27)return ts;{struct Cyc_List_List*
_tmp133B;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp133B=_cycalloc(
sizeof(*_tmp133B)),((_tmp133B->hd=_tmpA26,((_tmp133B->tl=_tmpA27,_tmp133B)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmpA29=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,&
Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp133E;struct _tuple15*_tmp133D;return(
_tmp133D=_cycalloc(sizeof(*_tmp133D)),((_tmp133D->f1=tv,((_tmp133D->f2=Cyc_Absyn_new_evar(
_tmpA29,((_tmp133E=_cycalloc(sizeof(*_tmp133E)),((_tmp133E->v=s,_tmp133E))))),
_tmp133D)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct
_tuple16*env,struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA2D;struct Cyc_List_List*
_tmpA2E;struct _RegionHandle*_tmpA2F;struct _tuple16*_tmpA2C=env;_tmpA2D=*_tmpA2C;
_tmpA2E=_tmpA2D.f1;_tmpA2F=_tmpA2D.f2;{struct Cyc_Core_Opt*_tmpA30=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1341;struct
_tuple15*_tmp1340;return(_tmp1340=_region_malloc(_tmpA2F,sizeof(*_tmp1340)),((
_tmp1340->f1=tv,((_tmp1340->f2=Cyc_Absyn_new_evar(_tmpA30,((_tmp1341=_cycalloc(
sizeof(*_tmp1341)),((_tmp1341->v=_tmpA2E,_tmp1341))))),_tmp1340)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp1347;void*_tmp1346[3];struct Cyc_String_pa_struct _tmp1345;struct Cyc_String_pa_struct
_tmp1344;struct Cyc_String_pa_struct _tmp1343;(_tmp1343.tag=0,((_tmp1343.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1344.tag=0,((_tmp1344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1345.tag=0,((_tmp1345.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1346[0]=& _tmp1345,((_tmp1346[1]=& _tmp1344,((_tmp1346[2]=&
_tmp1343,Cyc_Tcutil_terr(loc,((_tmp1347="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1347,sizeof(char),59))),_tag_dyneither(_tmp1346,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp134A;void*_tmp1349;(_tmp1349=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134A="same type variable has different identity!",
_tag_dyneither(_tmp134A,sizeof(char),43))),_tag_dyneither(_tmp1349,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp134B;return(_tmp134B=_cycalloc(sizeof(*_tmp134B)),((_tmp134B->hd=tv,((
_tmp134B->tl=tvs,_tmp134B)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp134E;void*_tmp134D;(_tmp134D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134E="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp134E,sizeof(char),39))),_tag_dyneither(_tmp134D,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpA3D=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpA3D->identity == - 1){const char*
_tmp1351;void*_tmp1350;(_tmp1350=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1351="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1351,sizeof(char),41))),_tag_dyneither(_tmp1350,sizeof(void*),
0)));}if(_tmpA3D->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1352;return(_tmp1352=_cycalloc(sizeof(*_tmp1352)),((_tmp1352->hd=tv,((
_tmp1352->tl=tvs,_tmp1352)))));};}struct _tuple27{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1355;void*_tmp1354;(_tmp1354=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1355="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1355,sizeof(char),44))),_tag_dyneither(_tmp1354,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple27
_tmpA44;struct Cyc_Absyn_Tvar*_tmpA45;int _tmpA46;int*_tmpA47;struct _tuple27*
_tmpA43=(struct _tuple27*)tvs2->hd;_tmpA44=*_tmpA43;_tmpA45=_tmpA44.f1;_tmpA46=
_tmpA44.f2;_tmpA47=(int*)&(*_tmpA43).f2;if(_tmpA45->identity == - 1){const char*
_tmp1358;void*_tmp1357;(_tmp1357=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1358="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1358,sizeof(char),46))),_tag_dyneither(_tmp1357,sizeof(void*),
0)));}if(_tmpA45->identity == tv->identity){*_tmpA47=*_tmpA47  || b;return tvs;}}}{
struct _tuple27*_tmp135B;struct Cyc_List_List*_tmp135A;return(_tmp135A=
_region_malloc(r,sizeof(*_tmp135A)),((_tmp135A->hd=((_tmp135B=_region_malloc(r,
sizeof(*_tmp135B)),((_tmp135B->f1=tv,((_tmp135B->f2=b,_tmp135B)))))),((_tmp135A->tl=
tvs,_tmp135A)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp135F;void*_tmp135E[1];struct Cyc_String_pa_struct _tmp135D;(_tmp135D.tag=0,((
_tmp135D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp135E[0]=& _tmp135D,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135F="bound tvar id for %s is NULL",
_tag_dyneither(_tmp135F,sizeof(char),29))),_tag_dyneither(_tmp135E,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1360;return(_tmp1360=_cycalloc(sizeof(*
_tmp1360)),((_tmp1360->hd=tv,((_tmp1360->tl=tvs,_tmp1360)))));};}struct _tuple28{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmpA50=Cyc_Tcutil_compress(e);int _tmpA52;_LL51E: {struct Cyc_Absyn_Evar_struct*
_tmpA51=(struct Cyc_Absyn_Evar_struct*)_tmpA50;if(_tmpA51->tag != 1)goto _LL520;
else{_tmpA52=_tmpA51->f3;}}_LL51F:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple28 _tmpA54;void*_tmpA55;int _tmpA56;int*_tmpA57;struct
_tuple28*_tmpA53=(struct _tuple28*)es2->hd;_tmpA54=*_tmpA53;_tmpA55=_tmpA54.f1;
_tmpA56=_tmpA54.f2;_tmpA57=(int*)&(*_tmpA53).f2;{void*_tmpA58=Cyc_Tcutil_compress(
_tmpA55);int _tmpA5A;_LL523: {struct Cyc_Absyn_Evar_struct*_tmpA59=(struct Cyc_Absyn_Evar_struct*)
_tmpA58;if(_tmpA59->tag != 1)goto _LL525;else{_tmpA5A=_tmpA59->f3;}}_LL524: if(
_tmpA52 == _tmpA5A){if(b != *_tmpA57)*_tmpA57=1;return es;}goto _LL522;_LL525:;
_LL526: goto _LL522;_LL522:;};}}{struct _tuple28*_tmp1363;struct Cyc_List_List*
_tmp1362;return(_tmp1362=_region_malloc(r,sizeof(*_tmp1362)),((_tmp1362->hd=((
_tmp1363=_region_malloc(r,sizeof(*_tmp1363)),((_tmp1363->f1=e,((_tmp1363->f2=b,
_tmp1363)))))),((_tmp1362->tl=es,_tmp1362)))));};_LL520:;_LL521: return es;_LL51D:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1364;r=((_tmp1364=_region_malloc(rgn,sizeof(*_tmp1364)),((
_tmp1364->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1364->tl=r,_tmp1364))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA5F;int _tmpA60;struct _tuple27 _tmpA5E=*((struct _tuple27*)
tvs->hd);_tmpA5F=_tmpA5E.f1;_tmpA60=_tmpA5E.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmpA5F->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp1365;res=((_tmp1365=
_region_malloc(r,sizeof(*_tmp1365)),((_tmp1365->hd=(struct _tuple27*)tvs->hd,((
_tmp1365->tl=res,_tmp1365))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp1369;void*_tmp1368[1];struct Cyc_String_pa_struct
_tmp1367;(_tmp1367.tag=0,((_tmp1367.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1368[0]=& _tmp1367,Cyc_Tcutil_terr(loc,((_tmp1369="bitfield %s does not have constant width",
_tag_dyneither(_tmp1369,sizeof(char),41))),_tag_dyneither(_tmp1368,sizeof(void*),
1)))))));}else{unsigned int _tmpA66;int _tmpA67;struct _tuple13 _tmpA65=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpA66=_tmpA65.f1;_tmpA67=_tmpA65.f2;if(!_tmpA67){
const char*_tmp136C;void*_tmp136B;(_tmp136B=0,Cyc_Tcutil_terr(loc,((_tmp136C="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp136C,sizeof(char),45))),_tag_dyneither(_tmp136B,sizeof(void*),
0)));}w=_tmpA66;}{void*_tmpA6A=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpA6C;_LL528: {struct Cyc_Absyn_IntType_struct*_tmpA6B=(struct Cyc_Absyn_IntType_struct*)
_tmpA6A;if(_tmpA6B->tag != 6)goto _LL52A;else{_tmpA6C=_tmpA6B->f2;}}_LL529: switch(
_tmpA6C){case Cyc_Absyn_Char_sz: _LL52C: if(w > 8){const char*_tmp136F;void*_tmp136E;(
_tmp136E=0,Cyc_Tcutil_terr(loc,((_tmp136F="bitfield larger than type",
_tag_dyneither(_tmp136F,sizeof(char),26))),_tag_dyneither(_tmp136E,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL52D: if(w > 16){const char*_tmp1372;void*
_tmp1371;(_tmp1371=0,Cyc_Tcutil_terr(loc,((_tmp1372="bitfield larger than type",
_tag_dyneither(_tmp1372,sizeof(char),26))),_tag_dyneither(_tmp1371,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL52E: goto _LL52F;case Cyc_Absyn_Int_sz: _LL52F:
if(w > 32){const char*_tmp1375;void*_tmp1374;(_tmp1374=0,Cyc_Tcutil_terr(loc,((
_tmp1375="bitfield larger than type",_tag_dyneither(_tmp1375,sizeof(char),26))),
_tag_dyneither(_tmp1374,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL530: if(w > 64){const char*_tmp1378;void*_tmp1377;(_tmp1377=0,Cyc_Tcutil_terr(
loc,((_tmp1378="bitfield larger than type",_tag_dyneither(_tmp1378,sizeof(char),
26))),_tag_dyneither(_tmp1377,sizeof(void*),0)));}break;}goto _LL527;_LL52A:;
_LL52B:{const char*_tmp137D;void*_tmp137C[2];struct Cyc_String_pa_struct _tmp137B;
struct Cyc_String_pa_struct _tmp137A;(_tmp137A.tag=0,((_tmp137A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp137B.tag=0,((_tmp137B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp137C[0]=& _tmp137B,((_tmp137C[1]=& _tmp137A,Cyc_Tcutil_terr(loc,((_tmp137D="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp137D,sizeof(char),52))),_tag_dyneither(_tmp137C,sizeof(void*),
2)))))))))))));}goto _LL527;_LL527:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmpA79=(void*)atts->hd;_LL533: {struct Cyc_Absyn_Packed_att_struct*_tmpA7A=(
struct Cyc_Absyn_Packed_att_struct*)_tmpA79;if(_tmpA7A->tag != 7)goto _LL535;}
_LL534: continue;_LL535: {struct Cyc_Absyn_Aligned_att_struct*_tmpA7B=(struct Cyc_Absyn_Aligned_att_struct*)
_tmpA79;if(_tmpA7B->tag != 6)goto _LL537;}_LL536: continue;_LL537:;_LL538: {const
char*_tmp1382;void*_tmp1381[2];struct Cyc_String_pa_struct _tmp1380;struct Cyc_String_pa_struct
_tmp137F;(_tmp137F.tag=0,((_tmp137F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1380.tag=0,((_tmp1380.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1381[0]=&
_tmp1380,((_tmp1381[1]=& _tmp137F,Cyc_Tcutil_terr(loc,((_tmp1382="bad attribute %s on member %s",
_tag_dyneither(_tmp1382,sizeof(char),30))),_tag_dyneither(_tmp1381,sizeof(void*),
2)))))))))))));}_LL532:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpA80=t;struct
Cyc_Absyn_Typedefdecl*_tmpA82;void**_tmpA83;_LL53A: {struct Cyc_Absyn_TypedefType_struct*
_tmpA81=(struct Cyc_Absyn_TypedefType_struct*)_tmpA80;if(_tmpA81->tag != 18)goto
_LL53C;else{_tmpA82=_tmpA81->f3;_tmpA83=_tmpA81->f4;}}_LL53B: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpA82))->tq).real_const  || (_tmpA82->tq).print_const){if(
declared_const){const char*_tmp1385;void*_tmp1384;(_tmp1384=0,Cyc_Tcutil_warn(loc,((
_tmp1385="extra const",_tag_dyneither(_tmp1385,sizeof(char),12))),_tag_dyneither(
_tmp1384,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpA83)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpA83);else{return declared_const;}_LL53C:;_LL53D: return
declared_const;_LL539:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td);static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(
struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn
!= 0){void*_tmpA86=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
td->defn))->v);struct Cyc_Absyn_PtrInfo _tmpA88;struct Cyc_Absyn_PtrAtts _tmpA89;
void*_tmpA8A;_LL53F: {struct Cyc_Absyn_PointerType_struct*_tmpA87=(struct Cyc_Absyn_PointerType_struct*)
_tmpA86;if(_tmpA87->tag != 5)goto _LL541;else{_tmpA88=_tmpA87->f1;_tmpA89=_tmpA88.ptr_atts;
_tmpA8A=_tmpA89.rgn;}}_LL540:{void*_tmpA8B=Cyc_Tcutil_compress(_tmpA8A);struct
Cyc_Absyn_Tvar*_tmpA8D;_LL544: {struct Cyc_Absyn_VarType_struct*_tmpA8C=(struct
Cyc_Absyn_VarType_struct*)_tmpA8B;if(_tmpA8C->tag != 2)goto _LL546;else{_tmpA8D=
_tmpA8C->f1;}}_LL545: return Cyc_Absyn_tvar_cmp(tvar,_tmpA8D)== 0;_LL546:;_LL547:
goto _LL543;_LL543:;}goto _LL53E;_LL541:;_LL542: goto _LL53E;_LL53E:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td);static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpA8E=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpA90;
struct Cyc_Absyn_Kind _tmpA91;enum Cyc_Absyn_KindQual _tmpA92;enum Cyc_Absyn_AliasQual
_tmpA93;struct Cyc_Absyn_Kind*_tmpA95;struct Cyc_Absyn_Kind _tmpA96;enum Cyc_Absyn_KindQual
_tmpA97;enum Cyc_Absyn_AliasQual _tmpA98;_LL549: {struct Cyc_Absyn_Less_kb_struct*
_tmpA8F=(struct Cyc_Absyn_Less_kb_struct*)_tmpA8E;if(_tmpA8F->tag != 2)goto _LL54B;
else{_tmpA90=_tmpA8F->f2;_tmpA91=*_tmpA90;_tmpA92=_tmpA91.kind;if(_tmpA92 != Cyc_Absyn_RgnKind)
goto _LL54B;_tmpA93=_tmpA91.aliasqual;if(_tmpA93 != Cyc_Absyn_Top)goto _LL54B;}}
_LL54A: goto _LL54C;_LL54B: {struct Cyc_Absyn_Eq_kb_struct*_tmpA94=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpA8E;if(_tmpA94->tag != 0)goto _LL54D;else{_tmpA95=_tmpA94->f1;_tmpA96=*_tmpA95;
_tmpA97=_tmpA96.kind;if(_tmpA97 != Cyc_Absyn_RgnKind)goto _LL54D;_tmpA98=_tmpA96.aliasqual;
if(_tmpA98 != Cyc_Absyn_Top)goto _LL54D;}}_LL54C: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL54D:;_LL54E: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL548:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple29{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmpA99=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmpA9C;struct Cyc_Core_Opt**_tmpA9D;struct Cyc_Core_Opt*
_tmpA9E;struct Cyc_Core_Opt**_tmpA9F;struct Cyc_Absyn_Tvar*_tmpAA1;struct Cyc_List_List*
_tmpAA3;struct _tuple2*_tmpAA5;struct Cyc_Absyn_Enumdecl*_tmpAA6;struct Cyc_Absyn_Enumdecl**
_tmpAA7;struct Cyc_Absyn_DatatypeInfo _tmpAA9;union Cyc_Absyn_DatatypeInfoU _tmpAAA;
union Cyc_Absyn_DatatypeInfoU*_tmpAAB;struct Cyc_List_List*_tmpAAC;struct Cyc_List_List**
_tmpAAD;struct Cyc_Absyn_DatatypeFieldInfo _tmpAAF;union Cyc_Absyn_DatatypeFieldInfoU
_tmpAB0;union Cyc_Absyn_DatatypeFieldInfoU*_tmpAB1;struct Cyc_List_List*_tmpAB2;
struct Cyc_Absyn_PtrInfo _tmpAB4;void*_tmpAB5;struct Cyc_Absyn_Tqual _tmpAB6;struct
Cyc_Absyn_Tqual*_tmpAB7;struct Cyc_Absyn_PtrAtts _tmpAB8;void*_tmpAB9;union Cyc_Absyn_Constraint*
_tmpABA;union Cyc_Absyn_Constraint*_tmpABB;union Cyc_Absyn_Constraint*_tmpABC;void*
_tmpABE;struct Cyc_Absyn_Exp*_tmpAC0;struct Cyc_Absyn_ArrayInfo _tmpAC5;void*
_tmpAC6;struct Cyc_Absyn_Tqual _tmpAC7;struct Cyc_Absyn_Tqual*_tmpAC8;struct Cyc_Absyn_Exp*
_tmpAC9;struct Cyc_Absyn_Exp**_tmpACA;union Cyc_Absyn_Constraint*_tmpACB;struct Cyc_Position_Segment*
_tmpACC;struct Cyc_Absyn_FnInfo _tmpACE;struct Cyc_List_List*_tmpACF;struct Cyc_List_List**
_tmpAD0;struct Cyc_Core_Opt*_tmpAD1;struct Cyc_Core_Opt**_tmpAD2;void*_tmpAD3;
struct Cyc_List_List*_tmpAD4;int _tmpAD5;struct Cyc_Absyn_VarargInfo*_tmpAD6;struct
Cyc_List_List*_tmpAD7;struct Cyc_List_List*_tmpAD8;struct Cyc_List_List*_tmpADA;
enum Cyc_Absyn_AggrKind _tmpADC;struct Cyc_List_List*_tmpADD;struct Cyc_Absyn_AggrInfo
_tmpADF;union Cyc_Absyn_AggrInfoU _tmpAE0;union Cyc_Absyn_AggrInfoU*_tmpAE1;struct
Cyc_List_List*_tmpAE2;struct Cyc_List_List**_tmpAE3;struct _tuple2*_tmpAE5;struct
Cyc_List_List*_tmpAE6;struct Cyc_List_List**_tmpAE7;struct Cyc_Absyn_Typedefdecl*
_tmpAE8;struct Cyc_Absyn_Typedefdecl**_tmpAE9;void**_tmpAEA;void***_tmpAEB;void*
_tmpAEF;void*_tmpAF1;void*_tmpAF2;void*_tmpAF4;void*_tmpAF6;struct Cyc_List_List*
_tmpAF8;_LL550: {struct Cyc_Absyn_VoidType_struct*_tmpA9A=(struct Cyc_Absyn_VoidType_struct*)
_tmpA99;if(_tmpA9A->tag != 0)goto _LL552;}_LL551: goto _LL54F;_LL552: {struct Cyc_Absyn_Evar_struct*
_tmpA9B=(struct Cyc_Absyn_Evar_struct*)_tmpA99;if(_tmpA9B->tag != 1)goto _LL554;
else{_tmpA9C=_tmpA9B->f1;_tmpA9D=(struct Cyc_Core_Opt**)& _tmpA9B->f1;_tmpA9E=
_tmpA9B->f2;_tmpA9F=(struct Cyc_Core_Opt**)& _tmpA9B->f2;}}_LL553: if(*_tmpA9D == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpA9D))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpA9D))->v,expected_kind))*_tmpA9D=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpA9F=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpA9F=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1388;struct Cyc_Absyn_Less_kb_struct*
_tmp1387;struct Cyc_Absyn_Tvar*_tmpAF9=Cyc_Tcutil_new_tvar((void*)((_tmp1387=
_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=2,((_tmp1388.f1=0,((
_tmp1388.f2=expected_kind,_tmp1388)))))),_tmp1387)))));{struct Cyc_Absyn_VarType_struct*
_tmp138E;struct Cyc_Absyn_VarType_struct _tmp138D;struct Cyc_Core_Opt*_tmp138C;*
_tmpA9F=((_tmp138C=_cycalloc(sizeof(*_tmp138C)),((_tmp138C->v=(void*)((_tmp138E=
_cycalloc(sizeof(*_tmp138E)),((_tmp138E[0]=((_tmp138D.tag=2,((_tmp138D.f1=
_tmpAF9,_tmp138D)))),_tmp138E)))),_tmp138C))));}_tmpAA1=_tmpAF9;goto _LL555;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL54F;_LL554: {struct Cyc_Absyn_VarType_struct*_tmpAA0=(
struct Cyc_Absyn_VarType_struct*)_tmpA99;if(_tmpAA0->tag != 2)goto _LL556;else{
_tmpAA1=_tmpAA0->f1;}}_LL555:{void*_tmpAFF=Cyc_Absyn_compress_kb(_tmpAA1->kind);
struct Cyc_Core_Opt*_tmpB01;struct Cyc_Core_Opt**_tmpB02;_LL585: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB00=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpAFF;if(_tmpB00->tag != 1)goto
_LL587;else{_tmpB01=_tmpB00->f1;_tmpB02=(struct Cyc_Core_Opt**)& _tmpB00->f1;}}
_LL586:{struct Cyc_Absyn_Less_kb_struct*_tmp1394;struct Cyc_Absyn_Less_kb_struct
_tmp1393;struct Cyc_Core_Opt*_tmp1392;*_tmpB02=((_tmp1392=_cycalloc(sizeof(*
_tmp1392)),((_tmp1392->v=(void*)((_tmp1394=_cycalloc(sizeof(*_tmp1394)),((
_tmp1394[0]=((_tmp1393.tag=2,((_tmp1393.f1=0,((_tmp1393.f2=expected_kind,
_tmp1393)))))),_tmp1394)))),_tmp1392))));}goto _LL584;_LL587:;_LL588: goto _LL584;
_LL584:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAA1);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpAA1,put_in_effect);{void*_tmpB06=Cyc_Absyn_compress_kb(_tmpAA1->kind);struct
Cyc_Core_Opt*_tmpB08;struct Cyc_Core_Opt**_tmpB09;struct Cyc_Absyn_Kind*_tmpB0A;
_LL58A: {struct Cyc_Absyn_Less_kb_struct*_tmpB07=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB06;if(_tmpB07->tag != 2)goto _LL58C;else{_tmpB08=_tmpB07->f1;_tmpB09=(struct
Cyc_Core_Opt**)& _tmpB07->f1;_tmpB0A=_tmpB07->f2;}}_LL58B: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB0A)){struct Cyc_Absyn_Less_kb_struct*_tmp139A;struct Cyc_Absyn_Less_kb_struct
_tmp1399;struct Cyc_Core_Opt*_tmp1398;*_tmpB09=((_tmp1398=_cycalloc(sizeof(*
_tmp1398)),((_tmp1398->v=(void*)((_tmp139A=_cycalloc(sizeof(*_tmp139A)),((
_tmp139A[0]=((_tmp1399.tag=2,((_tmp1399.f1=0,((_tmp1399.f2=expected_kind,
_tmp1399)))))),_tmp139A)))),_tmp1398))));}goto _LL589;_LL58C:;_LL58D: goto _LL589;
_LL589:;}goto _LL54F;_LL556: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAA2=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmpA99;if(_tmpAA2->tag != 15)goto _LL558;else{
_tmpAA3=_tmpAA2->f1;}}_LL557: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB0E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpAA3 != 0;_tmpAA3=_tmpAA3->tl){
struct Cyc_Absyn_Enumfield*_tmpB0F=(struct Cyc_Absyn_Enumfield*)_tmpAA3->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB0F->name).f2)){
const char*_tmp139E;void*_tmp139D[1];struct Cyc_String_pa_struct _tmp139C;(_tmp139C.tag=
0,((_tmp139C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB0F->name).f2),((
_tmp139D[0]=& _tmp139C,Cyc_Tcutil_terr(_tmpB0F->loc,((_tmp139E="duplicate enum field name %s",
_tag_dyneither(_tmp139E,sizeof(char),29))),_tag_dyneither(_tmp139D,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp139F;prev_fields=((_tmp139F=
_region_malloc(_tmpB0E,sizeof(*_tmp139F)),((_tmp139F->hd=(*_tmpB0F->name).f2,((
_tmp139F->tl=prev_fields,_tmp139F))))));}if(_tmpB0F->tag == 0)_tmpB0F->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB0F->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB0F->tag))){const char*_tmp13A3;void*
_tmp13A2[1];struct Cyc_String_pa_struct _tmp13A1;(_tmp13A1.tag=0,((_tmp13A1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB0F->name).f2),((_tmp13A2[0]=&
_tmp13A1,Cyc_Tcutil_terr(loc,((_tmp13A3="enum field %s: expression is not constant",
_tag_dyneither(_tmp13A3,sizeof(char),42))),_tag_dyneither(_tmp13A2,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB0F->tag))).f1;tag_count=t1 + 1;(*_tmpB0F->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp13A9;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp13A8;struct _tuple28*_tmp13A7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple28*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB0F->name).f2,(struct _tuple28*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((
_tmp13A7->f1=(void*)((_tmp13A9=_cycalloc(sizeof(*_tmp13A9)),((_tmp13A9[0]=((
_tmp13A8.tag=4,((_tmp13A8.f1=(void*)t,((_tmp13A8.f2=_tmpB0F,_tmp13A8)))))),
_tmp13A9)))),((_tmp13A7->f2=1,_tmp13A7)))))));};};}}goto _LL54F;}_LL558: {struct
Cyc_Absyn_EnumType_struct*_tmpAA4=(struct Cyc_Absyn_EnumType_struct*)_tmpA99;if(
_tmpAA4->tag != 14)goto _LL55A;else{_tmpAA5=_tmpAA4->f1;_tmpAA6=_tmpAA4->f2;
_tmpAA7=(struct Cyc_Absyn_Enumdecl**)& _tmpAA4->f2;}}_LL559: if(*_tmpAA7 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpAA7))->fields == 0){struct
_handler_cons _tmpB1A;_push_handler(& _tmpB1A);{int _tmpB1C=0;if(setjmp(_tmpB1A.handler))
_tmpB1C=1;if(!_tmpB1C){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpAA5);*_tmpAA7=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB1B=(void*)_exn_thrown;void*_tmpB1E=_tmpB1B;_LL58F: {struct Cyc_Dict_Absent_struct*
_tmpB1F=(struct Cyc_Dict_Absent_struct*)_tmpB1E;if(_tmpB1F->tag != Cyc_Dict_Absent)
goto _LL591;}_LL590: {struct Cyc_Tcenv_Genv*_tmpB20=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp13AA;struct Cyc_Absyn_Enumdecl*_tmpB21=(_tmp13AA=
_cycalloc(sizeof(*_tmp13AA)),((_tmp13AA->sc=Cyc_Absyn_Extern,((_tmp13AA->name=
_tmpAA5,((_tmp13AA->fields=0,_tmp13AA)))))));Cyc_Tc_tcEnumdecl(te,_tmpB20,loc,
_tmpB21);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpAA5);*
_tmpAA7=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL58E;};}_LL591:;_LL592:(void)_throw(
_tmpB1E);_LL58E:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpAA7);*_tmpAA5=(ed->name)[0];goto _LL54F;};_LL55A: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAA8=(struct Cyc_Absyn_DatatypeType_struct*)_tmpA99;if(_tmpAA8->tag != 3)goto
_LL55C;else{_tmpAA9=_tmpAA8->f1;_tmpAAA=_tmpAA9.datatype_info;_tmpAAB=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpAA8->f1).datatype_info;_tmpAAC=_tmpAA9.targs;_tmpAAD=(struct Cyc_List_List**)&(
_tmpAA8->f1).targs;}}_LL55B: {struct Cyc_List_List*_tmpB23=*_tmpAAD;{union Cyc_Absyn_DatatypeInfoU
_tmpB24=*_tmpAAB;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB25;struct _tuple2*
_tmpB26;int _tmpB27;struct Cyc_Absyn_Datatypedecl**_tmpB28;struct Cyc_Absyn_Datatypedecl*
_tmpB29;_LL594: if((_tmpB24.UnknownDatatype).tag != 1)goto _LL596;_tmpB25=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpB24.UnknownDatatype).val;_tmpB26=_tmpB25.name;
_tmpB27=_tmpB25.is_extensible;_LL595: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpB2A;_push_handler(& _tmpB2A);{int _tmpB2C=0;if(setjmp(
_tmpB2A.handler))_tmpB2C=1;if(!_tmpB2C){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpB26);;_pop_handler();}else{void*_tmpB2B=(void*)_exn_thrown;void*_tmpB2E=
_tmpB2B;_LL599: {struct Cyc_Dict_Absent_struct*_tmpB2F=(struct Cyc_Dict_Absent_struct*)
_tmpB2E;if(_tmpB2F->tag != Cyc_Dict_Absent)goto _LL59B;}_LL59A: {struct Cyc_Tcenv_Genv*
_tmpB30=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp13AB;struct Cyc_Absyn_Datatypedecl*
_tmpB31=(_tmp13AB=_cycalloc(sizeof(*_tmp13AB)),((_tmp13AB->sc=Cyc_Absyn_Extern,((
_tmp13AB->name=_tmpB26,((_tmp13AB->tvs=0,((_tmp13AB->fields=0,((_tmp13AB->is_extensible=
_tmpB27,_tmp13AB)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpB30,loc,_tmpB31);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpB26);if(_tmpB23 != 0){{const char*
_tmp13AF;void*_tmp13AE[1];struct Cyc_String_pa_struct _tmp13AD;(_tmp13AD.tag=0,((
_tmp13AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB26)),((_tmp13AE[0]=& _tmp13AD,Cyc_Tcutil_terr(loc,((_tmp13AF="declare parameterized datatype %s before using",
_tag_dyneither(_tmp13AF,sizeof(char),47))),_tag_dyneither(_tmp13AE,sizeof(void*),
1)))))));}return cvtenv;}goto _LL598;}_LL59B:;_LL59C:(void)_throw(_tmpB2E);_LL598:;}};}
if(_tmpB27  && !(*tudp)->is_extensible){const char*_tmp13B3;void*_tmp13B2[1];
struct Cyc_String_pa_struct _tmp13B1;(_tmp13B1.tag=0,((_tmp13B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB26)),((
_tmp13B2[0]=& _tmp13B1,Cyc_Tcutil_terr(loc,((_tmp13B3="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13B3,sizeof(char),41))),_tag_dyneither(_tmp13B2,sizeof(void*),
1)))))));}*_tmpAAB=Cyc_Absyn_KnownDatatype(tudp);_tmpB29=*tudp;goto _LL597;}
_LL596: if((_tmpB24.KnownDatatype).tag != 2)goto _LL593;_tmpB28=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB24.KnownDatatype).val;_tmpB29=*_tmpB28;_LL597: {struct Cyc_List_List*tvs=
_tmpB29->tvs;for(0;_tmpB23 != 0  && tvs != 0;(_tmpB23=_tmpB23->tl,tvs=tvs->tl)){
void*t=(void*)_tmpB23->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp13B4;struct _tuple0 _tmpB3A=(_tmp13B4.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp13B4.f2=t,_tmp13B4)));void*_tmpB3B;void*_tmpB3D;struct Cyc_Absyn_Tvar*_tmpB3F;
_LL59E: _tmpB3B=_tmpB3A.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB3C=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB3B;if(_tmpB3C->tag != 1)goto _LL5A0;};_tmpB3D=_tmpB3A.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB3E=(struct Cyc_Absyn_VarType_struct*)_tmpB3D;if(_tmpB3E->tag != 2)goto _LL5A0;
else{_tmpB3F=_tmpB3E->f1;}};_LL59F: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB3F);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB3F,1);continue;_LL5A0:;_LL5A1: goto _LL59D;_LL59D:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB23 != 0){const char*_tmp13B8;void*_tmp13B7[1];
struct Cyc_String_pa_struct _tmp13B6;(_tmp13B6.tag=0,((_tmp13B6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB29->name)),((
_tmp13B7[0]=& _tmp13B6,Cyc_Tcutil_terr(loc,((_tmp13B8="too many type arguments for datatype %s",
_tag_dyneither(_tmp13B8,sizeof(char),40))),_tag_dyneither(_tmp13B7,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp13B9;hidden_ts=((_tmp13B9=_cycalloc(sizeof(*_tmp13B9)),((_tmp13B9->hd=e,((
_tmp13B9->tl=hidden_ts,_tmp13B9))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpAAD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAAD,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL593;}_LL593:;}goto
_LL54F;}_LL55C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpAAE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpA99;if(_tmpAAE->tag != 4)goto _LL55E;else{_tmpAAF=_tmpAAE->f1;_tmpAB0=_tmpAAF.field_info;
_tmpAB1=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpAAE->f1).field_info;_tmpAB2=
_tmpAAF.targs;}}_LL55D:{union Cyc_Absyn_DatatypeFieldInfoU _tmpB44=*_tmpAB1;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB45;struct _tuple2*_tmpB46;struct _tuple2*
_tmpB47;int _tmpB48;struct _tuple3 _tmpB49;struct Cyc_Absyn_Datatypedecl*_tmpB4A;
struct Cyc_Absyn_Datatypefield*_tmpB4B;_LL5A3: if((_tmpB44.UnknownDatatypefield).tag
!= 1)goto _LL5A5;_tmpB45=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpB44.UnknownDatatypefield).val;
_tmpB46=_tmpB45.datatype_name;_tmpB47=_tmpB45.field_name;_tmpB48=_tmpB45.is_extensible;
_LL5A4: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpB4C;_push_handler(& _tmpB4C);{int _tmpB4E=0;if(setjmp(
_tmpB4C.handler))_tmpB4E=1;if(!_tmpB4E){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpB46);;_pop_handler();}else{void*_tmpB4D=(void*)_exn_thrown;void*_tmpB50=
_tmpB4D;_LL5A8: {struct Cyc_Dict_Absent_struct*_tmpB51=(struct Cyc_Dict_Absent_struct*)
_tmpB50;if(_tmpB51->tag != Cyc_Dict_Absent)goto _LL5AA;}_LL5A9:{const char*_tmp13BD;
void*_tmp13BC[1];struct Cyc_String_pa_struct _tmp13BB;(_tmp13BB.tag=0,((_tmp13BB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB46)),((
_tmp13BC[0]=& _tmp13BB,Cyc_Tcutil_terr(loc,((_tmp13BD="unbound datatype %s",
_tag_dyneither(_tmp13BD,sizeof(char),20))),_tag_dyneither(_tmp13BC,sizeof(void*),
1)))))));}return cvtenv;_LL5AA:;_LL5AB:(void)_throw(_tmpB50);_LL5A7:;}};}{struct
_handler_cons _tmpB55;_push_handler(& _tmpB55);{int _tmpB57=0;if(setjmp(_tmpB55.handler))
_tmpB57=1;if(!_tmpB57){{struct _RegionHandle*_tmpB58=Cyc_Tcenv_get_fnrgn(te);void*
_tmpB59=Cyc_Tcenv_lookup_ordinary(_tmpB58,te,loc,_tmpB47);struct Cyc_Absyn_Datatypedecl*
_tmpB5B;struct Cyc_Absyn_Datatypefield*_tmpB5C;_LL5AD: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpB5A=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpB59;if(_tmpB5A->tag != 2)goto
_LL5AF;else{_tmpB5B=_tmpB5A->f1;_tmpB5C=_tmpB5A->f2;}}_LL5AE: tuf=_tmpB5C;tud=
_tmpB5B;if(_tmpB48  && !tud->is_extensible){const char*_tmp13C1;void*_tmp13C0[1];
struct Cyc_String_pa_struct _tmp13BF;(_tmp13BF.tag=0,((_tmp13BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB46)),((
_tmp13C0[0]=& _tmp13BF,Cyc_Tcutil_terr(loc,((_tmp13C1="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13C1,sizeof(char),41))),_tag_dyneither(_tmp13C0,sizeof(void*),
1)))))));}goto _LL5AC;_LL5AF:;_LL5B0:{const char*_tmp13C6;void*_tmp13C5[2];struct
Cyc_String_pa_struct _tmp13C4;struct Cyc_String_pa_struct _tmp13C3;(_tmp13C3.tag=0,((
_tmp13C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB46)),((_tmp13C4.tag=0,((_tmp13C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((_tmp13C5[0]=& _tmp13C4,((
_tmp13C5[1]=& _tmp13C3,Cyc_Tcutil_terr(loc,((_tmp13C6="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13C6,sizeof(char),37))),_tag_dyneither(_tmp13C5,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpB64=cvtenv;_npop_handler(0);return
_tmpB64;};_LL5AC:;};_pop_handler();}else{void*_tmpB56=(void*)_exn_thrown;void*
_tmpB66=_tmpB56;_LL5B2: {struct Cyc_Dict_Absent_struct*_tmpB67=(struct Cyc_Dict_Absent_struct*)
_tmpB66;if(_tmpB67->tag != Cyc_Dict_Absent)goto _LL5B4;}_LL5B3:{const char*_tmp13CB;
void*_tmp13CA[2];struct Cyc_String_pa_struct _tmp13C9;struct Cyc_String_pa_struct
_tmp13C8;(_tmp13C8.tag=0,((_tmp13C8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB46)),((_tmp13C9.tag=0,((_tmp13C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((
_tmp13CA[0]=& _tmp13C9,((_tmp13CA[1]=& _tmp13C8,Cyc_Tcutil_terr(loc,((_tmp13CB="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13CB,sizeof(char),37))),_tag_dyneither(_tmp13CA,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5B4:;_LL5B5:(void)_throw(_tmpB66);_LL5B1:;}};}*
_tmpAB1=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpB4A=tud;_tmpB4B=tuf;goto _LL5A6;}
_LL5A5: if((_tmpB44.KnownDatatypefield).tag != 2)goto _LL5A2;_tmpB49=(struct _tuple3)(
_tmpB44.KnownDatatypefield).val;_tmpB4A=_tmpB49.f1;_tmpB4B=_tmpB49.f2;_LL5A6: {
struct Cyc_List_List*tvs=_tmpB4A->tvs;for(0;_tmpAB2 != 0  && tvs != 0;(_tmpAB2=
_tmpAB2->tl,tvs=tvs->tl)){void*t=(void*)_tmpAB2->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp13CC;struct _tuple0 _tmpB6D=(
_tmp13CC.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp13CC.f2=t,_tmp13CC)));void*
_tmpB6E;void*_tmpB70;struct Cyc_Absyn_Tvar*_tmpB72;_LL5B7: _tmpB6E=_tmpB6D.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpB6F=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB6E;if(_tmpB6F->tag != 1)goto _LL5B9;};_tmpB70=_tmpB6D.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB71=(struct Cyc_Absyn_VarType_struct*)_tmpB70;if(_tmpB71->tag != 2)goto _LL5B9;
else{_tmpB72=_tmpB71->f1;}};_LL5B8: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB72);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB72,1);continue;_LL5B9:;_LL5BA: goto _LL5B6;_LL5B6:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpAB2 != 0){const char*_tmp13D1;void*_tmp13D0[2];
struct Cyc_String_pa_struct _tmp13CF;struct Cyc_String_pa_struct _tmp13CE;(_tmp13CE.tag=
0,((_tmp13CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4B->name)),((_tmp13CF.tag=0,((_tmp13CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4A->name)),((_tmp13D0[0]=& _tmp13CF,((
_tmp13D0[1]=& _tmp13CE,Cyc_Tcutil_terr(loc,((_tmp13D1="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp13D1,sizeof(char),43))),_tag_dyneither(_tmp13D0,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp13D6;void*_tmp13D5[2];struct Cyc_String_pa_struct
_tmp13D4;struct Cyc_String_pa_struct _tmp13D3;(_tmp13D3.tag=0,((_tmp13D3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4B->name)),((
_tmp13D4.tag=0,((_tmp13D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4A->name)),((_tmp13D5[0]=& _tmp13D4,((_tmp13D5[1]=& _tmp13D3,Cyc_Tcutil_terr(
loc,((_tmp13D6="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp13D6,sizeof(char),42))),_tag_dyneither(_tmp13D5,sizeof(void*),2)))))))))))));}
goto _LL5A2;}_LL5A2:;}goto _LL54F;_LL55E: {struct Cyc_Absyn_PointerType_struct*
_tmpAB3=(struct Cyc_Absyn_PointerType_struct*)_tmpA99;if(_tmpAB3->tag != 5)goto
_LL560;else{_tmpAB4=_tmpAB3->f1;_tmpAB5=_tmpAB4.elt_typ;_tmpAB6=_tmpAB4.elt_tq;
_tmpAB7=(struct Cyc_Absyn_Tqual*)&(_tmpAB3->f1).elt_tq;_tmpAB8=_tmpAB4.ptr_atts;
_tmpAB9=_tmpAB8.rgn;_tmpABA=_tmpAB8.nullable;_tmpABB=_tmpAB8.bounds;_tmpABC=
_tmpAB8.zero_term;}}_LL55F: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAB5,1);_tmpAB7->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpAB7->print_const,_tmpAB5);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5BB: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5BC: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5BD: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpAB9,1);{union Cyc_Absyn_Constraint*
_tmpB7B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpABC);union Cyc_Absyn_Constraint _tmpB7C;int _tmpB7D;union Cyc_Absyn_Constraint
_tmpB7E;int _tmpB7F;_LL5C0: _tmpB7C=*_tmpB7B;if((_tmpB7C.No_constr).tag != 3)goto
_LL5C2;_tmpB7D=(int)(_tmpB7C.No_constr).val;_LL5C1:{void*_tmpB80=Cyc_Tcutil_compress(
_tmpAB5);enum Cyc_Absyn_Size_of _tmpB82;_LL5C7: {struct Cyc_Absyn_IntType_struct*
_tmpB81=(struct Cyc_Absyn_IntType_struct*)_tmpB80;if(_tmpB81->tag != 6)goto _LL5C9;
else{_tmpB82=_tmpB81->f2;if(_tmpB82 != Cyc_Absyn_Char_sz)goto _LL5C9;}}_LL5C8:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpABC,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5C6;_LL5C9:;_LL5CA:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpABC,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5C6;
_LL5C6:;}goto _LL5BF;_LL5C2: _tmpB7E=*_tmpB7B;if((_tmpB7E.Eq_constr).tag != 1)goto
_LL5C4;_tmpB7F=(int)(_tmpB7E.Eq_constr).val;if(_tmpB7F != 1)goto _LL5C4;_LL5C3: if(
!Cyc_Tcutil_admits_zero(_tmpAB5)){const char*_tmp13DA;void*_tmp13D9[1];struct Cyc_String_pa_struct
_tmp13D8;(_tmp13D8.tag=0,((_tmp13D8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAB5)),((_tmp13D9[0]=& _tmp13D8,Cyc_Tcutil_terr(
loc,((_tmp13DA="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp13DA,sizeof(char),53))),_tag_dyneither(_tmp13D9,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5BF;_LL5C4:;_LL5C5: is_zero_terminated=0;
goto _LL5BF;_LL5BF:;}{void*_tmpB86=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpABB);struct Cyc_Absyn_Exp*
_tmpB89;_LL5CC: {struct Cyc_Absyn_DynEither_b_struct*_tmpB87=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpB86;if(_tmpB87->tag != 0)goto _LL5CE;}_LL5CD: goto _LL5CB;_LL5CE: {struct Cyc_Absyn_Upper_b_struct*
_tmpB88=(struct Cyc_Absyn_Upper_b_struct*)_tmpB86;if(_tmpB88->tag != 1)goto _LL5CB;
else{_tmpB89=_tmpB88->f1;}}_LL5CF: {struct _RegionHandle*_tmpB8A=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB8B=Cyc_Tcenv_allow_valueof(_tmpB8A,te);Cyc_Tcexp_tcExp(
_tmpB8B,0,_tmpB89);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB89,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB89)){const char*_tmp13DD;void*
_tmp13DC;(_tmp13DC=0,Cyc_Tcutil_terr(loc,((_tmp13DD="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp13DD,sizeof(char),49))),_tag_dyneither(_tmp13DC,sizeof(void*),
0)));}{unsigned int _tmpB8F;int _tmpB90;struct _tuple13 _tmpB8E=Cyc_Evexp_eval_const_uint_exp(
_tmpB89);_tmpB8F=_tmpB8E.f1;_tmpB90=_tmpB8E.f2;if(is_zero_terminated  && (!
_tmpB90  || _tmpB8F < 1)){const char*_tmp13E0;void*_tmp13DF;(_tmp13DF=0,Cyc_Tcutil_terr(
loc,((_tmp13E0="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp13E0,sizeof(char),55))),_tag_dyneither(_tmp13DF,sizeof(void*),
0)));}goto _LL5CB;};}_LL5CB:;}goto _LL54F;};}_LL560: {struct Cyc_Absyn_TagType_struct*
_tmpABD=(struct Cyc_Absyn_TagType_struct*)_tmpA99;if(_tmpABD->tag != 20)goto _LL562;
else{_tmpABE=(void*)_tmpABD->f1;}}_LL561: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpABE,1);goto _LL54F;_LL562: {struct Cyc_Absyn_ValueofType_struct*
_tmpABF=(struct Cyc_Absyn_ValueofType_struct*)_tmpA99;if(_tmpABF->tag != 19)goto
_LL564;else{_tmpAC0=_tmpABF->f1;}}_LL563: {struct _RegionHandle*_tmpB93=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB94=Cyc_Tcenv_allow_valueof(_tmpB93,te);Cyc_Tcexp_tcExp(
_tmpB94,0,_tmpAC0);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC0)){const char*
_tmp13E3;void*_tmp13E2;(_tmp13E2=0,Cyc_Tcutil_terr(loc,((_tmp13E3="valueof_t requires an int expression",
_tag_dyneither(_tmp13E3,sizeof(char),37))),_tag_dyneither(_tmp13E2,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC0,te,cvtenv);goto
_LL54F;}_LL564: {struct Cyc_Absyn_IntType_struct*_tmpAC1=(struct Cyc_Absyn_IntType_struct*)
_tmpA99;if(_tmpAC1->tag != 6)goto _LL566;}_LL565: goto _LL567;_LL566: {struct Cyc_Absyn_FloatType_struct*
_tmpAC2=(struct Cyc_Absyn_FloatType_struct*)_tmpA99;if(_tmpAC2->tag != 7)goto
_LL568;}_LL567: goto _LL569;_LL568: {struct Cyc_Absyn_DoubleType_struct*_tmpAC3=(
struct Cyc_Absyn_DoubleType_struct*)_tmpA99;if(_tmpAC3->tag != 8)goto _LL56A;}
_LL569: goto _LL54F;_LL56A: {struct Cyc_Absyn_ArrayType_struct*_tmpAC4=(struct Cyc_Absyn_ArrayType_struct*)
_tmpA99;if(_tmpAC4->tag != 9)goto _LL56C;else{_tmpAC5=_tmpAC4->f1;_tmpAC6=_tmpAC5.elt_type;
_tmpAC7=_tmpAC5.tq;_tmpAC8=(struct Cyc_Absyn_Tqual*)&(_tmpAC4->f1).tq;_tmpAC9=
_tmpAC5.num_elts;_tmpACA=(struct Cyc_Absyn_Exp**)&(_tmpAC4->f1).num_elts;_tmpACB=
_tmpAC5.zero_term;_tmpACC=_tmpAC5.zt_loc;}}_LL56B: {struct Cyc_Absyn_Exp*_tmpB97=*
_tmpACA;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpAC6,1);_tmpAC8->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAC8->print_const,
_tmpAC6);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpB98=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpACB);union Cyc_Absyn_Constraint
_tmpB99;int _tmpB9A;union Cyc_Absyn_Constraint _tmpB9B;int _tmpB9C;_LL5D1: _tmpB99=*
_tmpB98;if((_tmpB99.No_constr).tag != 3)goto _LL5D3;_tmpB9A=(int)(_tmpB99.No_constr).val;
_LL5D2:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpACB,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5D0;_LL5D3: _tmpB9B=*_tmpB98;if((_tmpB9B.Eq_constr).tag
!= 1)goto _LL5D5;_tmpB9C=(int)(_tmpB9B.Eq_constr).val;if(_tmpB9C != 1)goto _LL5D5;
_LL5D4: if(!Cyc_Tcutil_admits_zero(_tmpAC6)){const char*_tmp13E7;void*_tmp13E6[1];
struct Cyc_String_pa_struct _tmp13E5;(_tmp13E5.tag=0,((_tmp13E5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAC6)),((
_tmp13E6[0]=& _tmp13E5,Cyc_Tcutil_terr(loc,((_tmp13E7="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp13E7,sizeof(char),51))),_tag_dyneither(_tmp13E6,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5D0;_LL5D5:;_LL5D6: is_zero_terminated=0;
goto _LL5D0;_LL5D0:;}if(_tmpB97 == 0){if(is_zero_terminated)*_tmpACA=(_tmpB97=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp13EA;void*
_tmp13E9;(_tmp13E9=0,Cyc_Tcutil_warn(loc,((_tmp13EA="array bound defaults to 1 here",
_tag_dyneither(_tmp13EA,sizeof(char),31))),_tag_dyneither(_tmp13E9,sizeof(void*),
0)));}*_tmpACA=(_tmpB97=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpB97);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpB97)){const char*_tmp13ED;void*_tmp13EC;(_tmp13EC=0,Cyc_Tcutil_terr(loc,((
_tmp13ED="array bounds expression is not constant",_tag_dyneither(_tmp13ED,
sizeof(char),40))),_tag_dyneither(_tmp13EC,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpB97)){const char*_tmp13F0;void*_tmp13EF;(_tmp13EF=0,
Cyc_Tcutil_terr(loc,((_tmp13F0="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp13F0,sizeof(char),47))),_tag_dyneither(_tmp13EF,sizeof(void*),
0)));}{unsigned int _tmpBA7;int _tmpBA8;struct _tuple13 _tmpBA6=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpB97);_tmpBA7=_tmpBA6.f1;_tmpBA8=_tmpBA6.f2;if((
is_zero_terminated  && _tmpBA8) && _tmpBA7 < 1){const char*_tmp13F3;void*_tmp13F2;(
_tmp13F2=0,Cyc_Tcutil_warn(loc,((_tmp13F3="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp13F3,sizeof(char),48))),_tag_dyneither(_tmp13F2,sizeof(void*),
0)));}if((_tmpBA8  && _tmpBA7 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp13F6;void*
_tmp13F5;(_tmp13F5=0,Cyc_Tcutil_warn(loc,((_tmp13F6="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp13F6,sizeof(char),75))),_tag_dyneither(_tmp13F5,sizeof(void*),
0)));}*_tmpACA=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL54F;};};}
_LL56C: {struct Cyc_Absyn_FnType_struct*_tmpACD=(struct Cyc_Absyn_FnType_struct*)
_tmpA99;if(_tmpACD->tag != 10)goto _LL56E;else{_tmpACE=_tmpACD->f1;_tmpACF=_tmpACE.tvars;
_tmpAD0=(struct Cyc_List_List**)&(_tmpACD->f1).tvars;_tmpAD1=_tmpACE.effect;
_tmpAD2=(struct Cyc_Core_Opt**)&(_tmpACD->f1).effect;_tmpAD3=_tmpACE.ret_typ;
_tmpAD4=_tmpACE.args;_tmpAD5=_tmpACE.c_varargs;_tmpAD6=_tmpACE.cyc_varargs;
_tmpAD7=_tmpACE.rgn_po;_tmpAD8=_tmpACE.attributes;}}_LL56D: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpAD8 != 0;
_tmpAD8=_tmpAD8->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpAD8->hd)){const char*
_tmp13FA;void*_tmp13F9[1];struct Cyc_String_pa_struct _tmp13F8;(_tmp13F8.tag=0,((
_tmp13F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpAD8->hd)),((_tmp13F9[0]=& _tmp13F8,Cyc_Tcutil_terr(loc,((_tmp13FA="bad function type attribute %s",
_tag_dyneither(_tmp13FA,sizeof(char),31))),_tag_dyneither(_tmp13F9,sizeof(void*),
1)))))));}{void*_tmpBB0=(void*)_tmpAD8->hd;enum Cyc_Absyn_Format_Type _tmpBB5;int
_tmpBB6;int _tmpBB7;_LL5D8: {struct Cyc_Absyn_Stdcall_att_struct*_tmpBB1=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpBB0;if(_tmpBB1->tag != 1)goto _LL5DA;}_LL5D9: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5D7;_LL5DA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpBB2=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpBB0;if(_tmpBB2->tag != 2)goto
_LL5DC;}_LL5DB: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5D7;_LL5DC: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpBB3=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpBB0;if(_tmpBB3->tag != 3)goto _LL5DE;}_LL5DD: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5D7;_LL5DE: {struct Cyc_Absyn_Format_att_struct*_tmpBB4=(
struct Cyc_Absyn_Format_att_struct*)_tmpBB0;if(_tmpBB4->tag != 19)goto _LL5E0;else{
_tmpBB5=_tmpBB4->f1;_tmpBB6=_tmpBB4->f2;_tmpBB7=_tmpBB4->f3;}}_LL5DF: if(!
seen_format){seen_format=1;ft=_tmpBB5;fmt_desc_arg=_tmpBB6;fmt_arg_start=_tmpBB7;}
else{const char*_tmp13FD;void*_tmp13FC;(_tmp13FC=0,Cyc_Tcutil_terr(loc,((_tmp13FD="function can't have multiple format attributes",
_tag_dyneither(_tmp13FD,sizeof(char),47))),_tag_dyneither(_tmp13FC,sizeof(void*),
0)));}goto _LL5D7;_LL5E0:;_LL5E1: goto _LL5D7;_LL5D7:;};}if(num_convs > 1){const char*
_tmp1400;void*_tmp13FF;(_tmp13FF=0,Cyc_Tcutil_terr(loc,((_tmp1400="function can't have multiple calling conventions",
_tag_dyneither(_tmp1400,sizeof(char),49))),_tag_dyneither(_tmp13FF,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpAD0);{struct Cyc_List_List*b=*
_tmpAD0;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpBBC=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpBBE;struct Cyc_Absyn_Kind _tmpBBF;enum Cyc_Absyn_KindQual
_tmpBC0;_LL5E3: {struct Cyc_Absyn_Eq_kb_struct*_tmpBBD=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpBBC;if(_tmpBBD->tag != 0)goto _LL5E5;else{_tmpBBE=_tmpBBD->f1;_tmpBBF=*_tmpBBE;
_tmpBC0=_tmpBBF.kind;if(_tmpBC0 != Cyc_Absyn_MemKind)goto _LL5E5;}}_LL5E4:{const
char*_tmp1404;void*_tmp1403[1];struct Cyc_String_pa_struct _tmp1402;(_tmp1402.tag=
0,((_tmp1402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1403[0]=& _tmp1402,Cyc_Tcutil_terr(loc,((_tmp1404="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1404,sizeof(char),51))),_tag_dyneither(_tmp1403,sizeof(void*),
1)))))));}goto _LL5E2;_LL5E5:;_LL5E6: goto _LL5E2;_LL5E2:;};}}{struct _RegionHandle*
_tmpBC4=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1405;struct Cyc_Tcutil_CVTEnv
_tmpBC5=(_tmp1405.r=_tmpBC4,((_tmp1405.kind_env=cvtenv.kind_env,((_tmp1405.free_vars=
0,((_tmp1405.free_evars=0,((_tmp1405.generalize_evars=cvtenv.generalize_evars,((
_tmp1405.fn_result=1,_tmp1405)))))))))));_tmpBC5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC5,& Cyc_Tcutil_tmk,_tmpAD3,1);_tmpBC5.fn_result=0;{struct Cyc_List_List*
a=_tmpAD4;for(0;a != 0;a=a->tl){struct _tuple9*_tmpBC6=(struct _tuple9*)a->hd;void*
_tmpBC7=(*_tmpBC6).f3;_tmpBC5=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpBC5,& Cyc_Tcutil_tmk,
_tmpBC7,1);((*_tmpBC6).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpBC6).f2).print_const,_tmpBC7);}}if(_tmpAD6 != 0){if(_tmpAD5){const char*
_tmp1408;void*_tmp1407;(_tmp1407=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1408="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1408,sizeof(char),29))),_tag_dyneither(_tmp1407,sizeof(void*),
0)));}{void*_tmpBCB;int _tmpBCC;struct Cyc_Absyn_VarargInfo _tmpBCA=*_tmpAD6;
_tmpBCB=_tmpBCA.type;_tmpBCC=_tmpBCA.inject;_tmpBC5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC5,& Cyc_Tcutil_tmk,_tmpBCB,1);(_tmpAD6->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpAD6->tq).print_const,_tmpBCB);if(_tmpBCC){void*_tmpBCD=Cyc_Tcutil_compress(
_tmpBCB);struct Cyc_Absyn_PtrInfo _tmpBCF;void*_tmpBD0;struct Cyc_Absyn_PtrAtts
_tmpBD1;union Cyc_Absyn_Constraint*_tmpBD2;union Cyc_Absyn_Constraint*_tmpBD3;
_LL5E8: {struct Cyc_Absyn_PointerType_struct*_tmpBCE=(struct Cyc_Absyn_PointerType_struct*)
_tmpBCD;if(_tmpBCE->tag != 5)goto _LL5EA;else{_tmpBCF=_tmpBCE->f1;_tmpBD0=_tmpBCF.elt_typ;
_tmpBD1=_tmpBCF.ptr_atts;_tmpBD2=_tmpBD1.bounds;_tmpBD3=_tmpBD1.zero_term;}}
_LL5E9:{void*_tmpBD4=Cyc_Tcutil_compress(_tmpBD0);_LL5ED: {struct Cyc_Absyn_DatatypeType_struct*
_tmpBD5=(struct Cyc_Absyn_DatatypeType_struct*)_tmpBD4;if(_tmpBD5->tag != 3)goto
_LL5EF;}_LL5EE: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpBD3)){const char*_tmp140B;void*_tmp140A;(_tmp140A=0,Cyc_Tcutil_terr(loc,((
_tmp140B="can't inject into a zeroterm pointer",_tag_dyneither(_tmp140B,sizeof(
char),37))),_tag_dyneither(_tmp140A,sizeof(void*),0)));}{void*_tmpBD8=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpBD2);_LL5F2: {struct Cyc_Absyn_DynEither_b_struct*_tmpBD9=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBD8;if(_tmpBD9->tag != 0)goto _LL5F4;}_LL5F3:{const char*_tmp140E;void*_tmp140D;(
_tmp140D=0,Cyc_Tcutil_terr(loc,((_tmp140E="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp140E,sizeof(char),44))),_tag_dyneither(_tmp140D,sizeof(void*),
0)));}goto _LL5F1;_LL5F4:;_LL5F5: goto _LL5F1;_LL5F1:;}goto _LL5EC;_LL5EF:;_LL5F0:{
const char*_tmp1411;void*_tmp1410;(_tmp1410=0,Cyc_Tcutil_terr(loc,((_tmp1411="can't inject a non-datatype type",
_tag_dyneither(_tmp1411,sizeof(char),33))),_tag_dyneither(_tmp1410,sizeof(void*),
0)));}goto _LL5EC;_LL5EC:;}goto _LL5E7;_LL5EA:;_LL5EB:{const char*_tmp1414;void*
_tmp1413;(_tmp1413=0,Cyc_Tcutil_terr(loc,((_tmp1414="expecting a datatype pointer type",
_tag_dyneither(_tmp1414,sizeof(char),34))),_tag_dyneither(_tmp1413,sizeof(void*),
0)));}goto _LL5E7;_LL5E7:;}};}if(seen_format){int _tmpBE0=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpAD4);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpBE0) || 
fmt_arg_start < 0) || _tmpAD6 == 0  && fmt_arg_start != 0) || _tmpAD6 != 0  && 
fmt_arg_start != _tmpBE0 + 1){const char*_tmp1417;void*_tmp1416;(_tmp1416=0,Cyc_Tcutil_terr(
loc,((_tmp1417="bad format descriptor",_tag_dyneither(_tmp1417,sizeof(char),22))),
_tag_dyneither(_tmp1416,sizeof(void*),0)));}else{void*_tmpBE4;struct _tuple9
_tmpBE3=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpAD4,
fmt_desc_arg - 1);_tmpBE4=_tmpBE3.f3;{void*_tmpBE5=Cyc_Tcutil_compress(_tmpBE4);
struct Cyc_Absyn_PtrInfo _tmpBE7;void*_tmpBE8;struct Cyc_Absyn_PtrAtts _tmpBE9;union
Cyc_Absyn_Constraint*_tmpBEA;union Cyc_Absyn_Constraint*_tmpBEB;_LL5F7: {struct
Cyc_Absyn_PointerType_struct*_tmpBE6=(struct Cyc_Absyn_PointerType_struct*)
_tmpBE5;if(_tmpBE6->tag != 5)goto _LL5F9;else{_tmpBE7=_tmpBE6->f1;_tmpBE8=_tmpBE7.elt_typ;
_tmpBE9=_tmpBE7.ptr_atts;_tmpBEA=_tmpBE9.bounds;_tmpBEB=_tmpBE9.zero_term;}}
_LL5F8:{struct _tuple0 _tmp1418;struct _tuple0 _tmpBED=(_tmp1418.f1=Cyc_Tcutil_compress(
_tmpBE8),((_tmp1418.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpBEA),_tmp1418)));void*_tmpBEE;enum Cyc_Absyn_Sign
_tmpBF0;enum Cyc_Absyn_Size_of _tmpBF1;void*_tmpBF2;_LL5FC: _tmpBEE=_tmpBED.f1;{
struct Cyc_Absyn_IntType_struct*_tmpBEF=(struct Cyc_Absyn_IntType_struct*)_tmpBEE;
if(_tmpBEF->tag != 6)goto _LL5FE;else{_tmpBF0=_tmpBEF->f1;if(_tmpBF0 != Cyc_Absyn_None)
goto _LL5FE;_tmpBF1=_tmpBEF->f2;if(_tmpBF1 != Cyc_Absyn_Char_sz)goto _LL5FE;}};
_tmpBF2=_tmpBED.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpBF3=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBF2;if(_tmpBF3->tag != 0)goto _LL5FE;};_LL5FD: goto _LL5FB;_LL5FE:;_LL5FF:{const
char*_tmp141B;void*_tmp141A;(_tmp141A=0,Cyc_Tcutil_terr(loc,((_tmp141B="format descriptor is not a char ? type",
_tag_dyneither(_tmp141B,sizeof(char),39))),_tag_dyneither(_tmp141A,sizeof(void*),
0)));}goto _LL5FB;_LL5FB:;}goto _LL5F6;_LL5F9:;_LL5FA:{const char*_tmp141E;void*
_tmp141D;(_tmp141D=0,Cyc_Tcutil_terr(loc,((_tmp141E="format descriptor is not a char ? type",
_tag_dyneither(_tmp141E,sizeof(char),39))),_tag_dyneither(_tmp141D,sizeof(void*),
0)));}goto _LL5F6;_LL5F6:;}if(fmt_arg_start != 0){int problem;{struct _tuple29
_tmp141F;struct _tuple29 _tmpBF9=(_tmp141F.f1=ft,((_tmp141F.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpAD6))->type)),
_tmp141F)));enum Cyc_Absyn_Format_Type _tmpBFA;void*_tmpBFB;struct Cyc_Absyn_DatatypeInfo
_tmpBFD;union Cyc_Absyn_DatatypeInfoU _tmpBFE;struct Cyc_Absyn_Datatypedecl**
_tmpBFF;struct Cyc_Absyn_Datatypedecl*_tmpC00;enum Cyc_Absyn_Format_Type _tmpC01;
void*_tmpC02;struct Cyc_Absyn_DatatypeInfo _tmpC04;union Cyc_Absyn_DatatypeInfoU
_tmpC05;struct Cyc_Absyn_Datatypedecl**_tmpC06;struct Cyc_Absyn_Datatypedecl*
_tmpC07;_LL601: _tmpBFA=_tmpBF9.f1;if(_tmpBFA != Cyc_Absyn_Printf_ft)goto _LL603;
_tmpBFB=_tmpBF9.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpBFC=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpBFB;if(_tmpBFC->tag != 3)goto _LL603;else{_tmpBFD=_tmpBFC->f1;_tmpBFE=_tmpBFD.datatype_info;
if((_tmpBFE.KnownDatatype).tag != 2)goto _LL603;_tmpBFF=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBFE.KnownDatatype).val;_tmpC00=*_tmpBFF;}};_LL602: problem=Cyc_Absyn_qvar_cmp(
_tmpC00->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL600;_LL603: _tmpC01=
_tmpBF9.f1;if(_tmpC01 != Cyc_Absyn_Scanf_ft)goto _LL605;_tmpC02=_tmpBF9.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC03=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC02;if(_tmpC03->tag != 3)goto _LL605;else{_tmpC04=_tmpC03->f1;_tmpC05=_tmpC04.datatype_info;
if((_tmpC05.KnownDatatype).tag != 2)goto _LL605;_tmpC06=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC05.KnownDatatype).val;_tmpC07=*_tmpC06;}};_LL604: problem=Cyc_Absyn_qvar_cmp(
_tmpC07->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL600;_LL605:;_LL606:
problem=1;goto _LL600;_LL600:;}if(problem){const char*_tmp1422;void*_tmp1421;(
_tmp1421=0,Cyc_Tcutil_terr(loc,((_tmp1422="format attribute and vararg types don't match",
_tag_dyneither(_tmp1422,sizeof(char),46))),_tag_dyneither(_tmp1421,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpAD7;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC0B;void*_tmpC0C;void*_tmpC0D;struct _tuple0*_tmpC0A=(struct _tuple0*)
rpo->hd;_tmpC0B=*_tmpC0A;_tmpC0C=_tmpC0B.f1;_tmpC0D=_tmpC0B.f2;_tmpBC5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC5,& Cyc_Tcutil_ek,_tmpC0C,1);_tmpBC5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC5,& Cyc_Tcutil_trk,_tmpC0D,1);}}if(*_tmpAD2 != 0)_tmpBC5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC5,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpAD2))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpBC5.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC0F;int _tmpC10;struct
_tuple27 _tmpC0E=*((struct _tuple27*)tvs->hd);_tmpC0F=_tmpC0E.f1;_tmpC10=_tmpC0E.f2;
if(!_tmpC10)continue;{void*_tmpC11=Cyc_Absyn_compress_kb(_tmpC0F->kind);struct
Cyc_Core_Opt*_tmpC13;struct Cyc_Core_Opt**_tmpC14;struct Cyc_Absyn_Kind*_tmpC15;
struct Cyc_Absyn_Kind*_tmpC17;struct Cyc_Absyn_Kind*_tmpC19;struct Cyc_Absyn_Kind
_tmpC1A;enum Cyc_Absyn_KindQual _tmpC1B;struct Cyc_Absyn_Kind*_tmpC1D;struct Cyc_Absyn_Kind
_tmpC1E;enum Cyc_Absyn_KindQual _tmpC1F;struct Cyc_Core_Opt*_tmpC21;struct Cyc_Core_Opt**
_tmpC22;struct Cyc_Absyn_Kind*_tmpC23;struct Cyc_Absyn_Kind _tmpC24;enum Cyc_Absyn_KindQual
_tmpC25;struct Cyc_Absyn_Kind*_tmpC27;struct Cyc_Absyn_Kind _tmpC28;enum Cyc_Absyn_KindQual
_tmpC29;struct Cyc_Core_Opt*_tmpC2B;struct Cyc_Core_Opt**_tmpC2C;_LL608:{struct Cyc_Absyn_Less_kb_struct*
_tmpC12=(struct Cyc_Absyn_Less_kb_struct*)_tmpC11;if(_tmpC12->tag != 2)goto _LL60A;
else{_tmpC13=_tmpC12->f1;_tmpC14=(struct Cyc_Core_Opt**)& _tmpC12->f1;_tmpC15=
_tmpC12->f2;}}if(!(_tmpC15->kind == Cyc_Absyn_RgnKind))goto _LL60A;_LL609: if(
_tmpC15->aliasqual == Cyc_Absyn_Top){*_tmpC14=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpC17=_tmpC15;goto _LL60B;}*_tmpC14=Cyc_Tcutil_kind_to_bound_opt(_tmpC15);
_tmpC17=_tmpC15;goto _LL60B;_LL60A:{struct Cyc_Absyn_Eq_kb_struct*_tmpC16=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC11;if(_tmpC16->tag != 0)goto _LL60C;else{_tmpC17=
_tmpC16->f1;}}if(!(_tmpC17->kind == Cyc_Absyn_RgnKind))goto _LL60C;_LL60B:{struct
Cyc_Absyn_AccessEff_struct*_tmp1431;struct Cyc_Absyn_VarType_struct*_tmp1430;
struct Cyc_Absyn_VarType_struct _tmp142F;struct Cyc_Absyn_AccessEff_struct _tmp142E;
struct Cyc_List_List*_tmp142D;effect=((_tmp142D=_cycalloc(sizeof(*_tmp142D)),((
_tmp142D->hd=(void*)((_tmp1431=_cycalloc(sizeof(*_tmp1431)),((_tmp1431[0]=((
_tmp142E.tag=23,((_tmp142E.f1=(void*)((void*)((_tmp1430=_cycalloc(sizeof(*
_tmp1430)),((_tmp1430[0]=((_tmp142F.tag=2,((_tmp142F.f1=_tmpC0F,_tmp142F)))),
_tmp1430))))),_tmp142E)))),_tmp1431)))),((_tmp142D->tl=effect,_tmp142D))))));}
goto _LL607;_LL60C: {struct Cyc_Absyn_Less_kb_struct*_tmpC18=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC11;if(_tmpC18->tag != 2)goto _LL60E;else{_tmpC19=_tmpC18->f2;_tmpC1A=*_tmpC19;
_tmpC1B=_tmpC1A.kind;if(_tmpC1B != Cyc_Absyn_IntKind)goto _LL60E;}}_LL60D: goto
_LL60F;_LL60E: {struct Cyc_Absyn_Eq_kb_struct*_tmpC1C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC11;if(_tmpC1C->tag != 0)goto _LL610;else{_tmpC1D=_tmpC1C->f1;_tmpC1E=*_tmpC1D;
_tmpC1F=_tmpC1E.kind;if(_tmpC1F != Cyc_Absyn_IntKind)goto _LL610;}}_LL60F: goto
_LL607;_LL610: {struct Cyc_Absyn_Less_kb_struct*_tmpC20=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC11;if(_tmpC20->tag != 2)goto _LL612;else{_tmpC21=_tmpC20->f1;_tmpC22=(struct
Cyc_Core_Opt**)& _tmpC20->f1;_tmpC23=_tmpC20->f2;_tmpC24=*_tmpC23;_tmpC25=_tmpC24.kind;
if(_tmpC25 != Cyc_Absyn_EffKind)goto _LL612;}}_LL611:*_tmpC22=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL613;_LL612: {struct Cyc_Absyn_Eq_kb_struct*_tmpC26=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC11;if(_tmpC26->tag != 0)goto _LL614;else{_tmpC27=
_tmpC26->f1;_tmpC28=*_tmpC27;_tmpC29=_tmpC28.kind;if(_tmpC29 != Cyc_Absyn_EffKind)
goto _LL614;}}_LL613:{struct Cyc_Absyn_VarType_struct*_tmp1437;struct Cyc_Absyn_VarType_struct
_tmp1436;struct Cyc_List_List*_tmp1435;effect=((_tmp1435=_cycalloc(sizeof(*
_tmp1435)),((_tmp1435->hd=(void*)((_tmp1437=_cycalloc(sizeof(*_tmp1437)),((
_tmp1437[0]=((_tmp1436.tag=2,((_tmp1436.f1=_tmpC0F,_tmp1436)))),_tmp1437)))),((
_tmp1435->tl=effect,_tmp1435))))));}goto _LL607;_LL614: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC2A=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC11;if(_tmpC2A->tag != 1)goto
_LL616;else{_tmpC2B=_tmpC2A->f1;_tmpC2C=(struct Cyc_Core_Opt**)& _tmpC2A->f1;}}
_LL615:{struct Cyc_Absyn_Less_kb_struct*_tmp143D;struct Cyc_Absyn_Less_kb_struct
_tmp143C;struct Cyc_Core_Opt*_tmp143B;*_tmpC2C=((_tmp143B=_cycalloc(sizeof(*
_tmp143B)),((_tmp143B->v=(void*)((_tmp143D=_cycalloc(sizeof(*_tmp143D)),((
_tmp143D[0]=((_tmp143C.tag=2,((_tmp143C.f1=0,((_tmp143C.f2=& Cyc_Tcutil_ak,
_tmp143C)))))),_tmp143D)))),_tmp143B))));}goto _LL617;_LL616:;_LL617:{struct Cyc_Absyn_RgnsEff_struct*
_tmp144C;struct Cyc_Absyn_VarType_struct*_tmp144B;struct Cyc_Absyn_VarType_struct
_tmp144A;struct Cyc_Absyn_RgnsEff_struct _tmp1449;struct Cyc_List_List*_tmp1448;
effect=((_tmp1448=_cycalloc(sizeof(*_tmp1448)),((_tmp1448->hd=(void*)((_tmp144C=
_cycalloc(sizeof(*_tmp144C)),((_tmp144C[0]=((_tmp1449.tag=25,((_tmp1449.f1=(void*)((
void*)((_tmp144B=_cycalloc(sizeof(*_tmp144B)),((_tmp144B[0]=((_tmp144A.tag=2,((
_tmp144A.f1=_tmpC0F,_tmp144A)))),_tmp144B))))),_tmp1449)))),_tmp144C)))),((
_tmp1448->tl=effect,_tmp1448))))));}goto _LL607;_LL607:;};}}{struct Cyc_List_List*
ts=_tmpBC5.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpC3E;int _tmpC3F;struct
_tuple28 _tmpC3D=*((struct _tuple28*)ts->hd);_tmpC3E=_tmpC3D.f1;_tmpC3F=_tmpC3D.f2;
if(!_tmpC3F)continue;{struct Cyc_Absyn_Kind*_tmpC40=Cyc_Tcutil_typ_kind(_tmpC3E);
struct Cyc_Absyn_Kind _tmpC41;enum Cyc_Absyn_KindQual _tmpC42;struct Cyc_Absyn_Kind
_tmpC43;enum Cyc_Absyn_KindQual _tmpC44;struct Cyc_Absyn_Kind _tmpC45;enum Cyc_Absyn_KindQual
_tmpC46;_LL619: _tmpC41=*_tmpC40;_tmpC42=_tmpC41.kind;if(_tmpC42 != Cyc_Absyn_RgnKind)
goto _LL61B;_LL61A:{struct Cyc_Absyn_AccessEff_struct*_tmp1452;struct Cyc_Absyn_AccessEff_struct
_tmp1451;struct Cyc_List_List*_tmp1450;effect=((_tmp1450=_cycalloc(sizeof(*
_tmp1450)),((_tmp1450->hd=(void*)((_tmp1452=_cycalloc(sizeof(*_tmp1452)),((
_tmp1452[0]=((_tmp1451.tag=23,((_tmp1451.f1=(void*)_tmpC3E,_tmp1451)))),_tmp1452)))),((
_tmp1450->tl=effect,_tmp1450))))));}goto _LL618;_LL61B: _tmpC43=*_tmpC40;_tmpC44=
_tmpC43.kind;if(_tmpC44 != Cyc_Absyn_EffKind)goto _LL61D;_LL61C:{struct Cyc_List_List*
_tmp1453;effect=((_tmp1453=_cycalloc(sizeof(*_tmp1453)),((_tmp1453->hd=_tmpC3E,((
_tmp1453->tl=effect,_tmp1453))))));}goto _LL618;_LL61D: _tmpC45=*_tmpC40;_tmpC46=
_tmpC45.kind;if(_tmpC46 != Cyc_Absyn_IntKind)goto _LL61F;_LL61E: goto _LL618;_LL61F:;
_LL620:{struct Cyc_Absyn_RgnsEff_struct*_tmp1459;struct Cyc_Absyn_RgnsEff_struct
_tmp1458;struct Cyc_List_List*_tmp1457;effect=((_tmp1457=_cycalloc(sizeof(*
_tmp1457)),((_tmp1457->hd=(void*)((_tmp1459=_cycalloc(sizeof(*_tmp1459)),((
_tmp1459[0]=((_tmp1458.tag=25,((_tmp1458.f1=(void*)_tmpC3E,_tmp1458)))),_tmp1459)))),((
_tmp1457->tl=effect,_tmp1457))))));}goto _LL618;_LL618:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp145F;struct Cyc_Absyn_JoinEff_struct _tmp145E;struct Cyc_Core_Opt*_tmp145D;*
_tmpAD2=((_tmp145D=_cycalloc(sizeof(*_tmp145D)),((_tmp145D->v=(void*)((_tmp145F=
_cycalloc(sizeof(*_tmp145F)),((_tmp145F[0]=((_tmp145E.tag=24,((_tmp145E.f1=
effect,_tmp145E)))),_tmp145F)))),_tmp145D))));};}if(*_tmpAD0 != 0){struct Cyc_List_List*
bs=*_tmpAD0;for(0;bs != 0;bs=bs->tl){void*_tmpC51=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpC53;struct Cyc_Core_Opt**
_tmpC54;struct Cyc_Core_Opt*_tmpC56;struct Cyc_Core_Opt**_tmpC57;struct Cyc_Absyn_Kind*
_tmpC58;struct Cyc_Absyn_Kind _tmpC59;enum Cyc_Absyn_KindQual _tmpC5A;enum Cyc_Absyn_AliasQual
_tmpC5B;struct Cyc_Core_Opt*_tmpC5D;struct Cyc_Core_Opt**_tmpC5E;struct Cyc_Absyn_Kind*
_tmpC5F;struct Cyc_Absyn_Kind _tmpC60;enum Cyc_Absyn_KindQual _tmpC61;enum Cyc_Absyn_AliasQual
_tmpC62;struct Cyc_Core_Opt*_tmpC64;struct Cyc_Core_Opt**_tmpC65;struct Cyc_Absyn_Kind*
_tmpC66;struct Cyc_Absyn_Kind _tmpC67;enum Cyc_Absyn_KindQual _tmpC68;enum Cyc_Absyn_AliasQual
_tmpC69;struct Cyc_Core_Opt*_tmpC6B;struct Cyc_Core_Opt**_tmpC6C;struct Cyc_Absyn_Kind*
_tmpC6D;struct Cyc_Absyn_Kind _tmpC6E;enum Cyc_Absyn_KindQual _tmpC6F;enum Cyc_Absyn_AliasQual
_tmpC70;struct Cyc_Core_Opt*_tmpC72;struct Cyc_Core_Opt**_tmpC73;struct Cyc_Absyn_Kind*
_tmpC74;struct Cyc_Absyn_Kind _tmpC75;enum Cyc_Absyn_KindQual _tmpC76;enum Cyc_Absyn_AliasQual
_tmpC77;struct Cyc_Core_Opt*_tmpC79;struct Cyc_Core_Opt**_tmpC7A;struct Cyc_Absyn_Kind*
_tmpC7B;struct Cyc_Absyn_Kind _tmpC7C;enum Cyc_Absyn_KindQual _tmpC7D;enum Cyc_Absyn_AliasQual
_tmpC7E;struct Cyc_Core_Opt*_tmpC80;struct Cyc_Core_Opt**_tmpC81;struct Cyc_Absyn_Kind*
_tmpC82;struct Cyc_Absyn_Kind _tmpC83;enum Cyc_Absyn_KindQual _tmpC84;enum Cyc_Absyn_AliasQual
_tmpC85;struct Cyc_Core_Opt*_tmpC87;struct Cyc_Core_Opt**_tmpC88;struct Cyc_Absyn_Kind*
_tmpC89;struct Cyc_Absyn_Kind _tmpC8A;enum Cyc_Absyn_KindQual _tmpC8B;enum Cyc_Absyn_AliasQual
_tmpC8C;struct Cyc_Core_Opt*_tmpC8E;struct Cyc_Core_Opt**_tmpC8F;struct Cyc_Absyn_Kind*
_tmpC90;struct Cyc_Absyn_Kind*_tmpC92;struct Cyc_Absyn_Kind _tmpC93;enum Cyc_Absyn_KindQual
_tmpC94;_LL622: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC52=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC51;if(_tmpC52->tag != 1)goto _LL624;else{_tmpC53=_tmpC52->f1;_tmpC54=(struct
Cyc_Core_Opt**)& _tmpC52->f1;}}_LL623:{const char*_tmp1463;void*_tmp1462[1];struct
Cyc_String_pa_struct _tmp1461;(_tmp1461.tag=0,((_tmp1461.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1462[0]=&
_tmp1461,Cyc_Tcutil_warn(loc,((_tmp1463="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1463,sizeof(char),47))),_tag_dyneither(_tmp1462,sizeof(void*),
1)))))));}_tmpC57=_tmpC54;goto _LL625;_LL624: {struct Cyc_Absyn_Less_kb_struct*
_tmpC55=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC55->tag != 2)goto _LL626;
else{_tmpC56=_tmpC55->f1;_tmpC57=(struct Cyc_Core_Opt**)& _tmpC55->f1;_tmpC58=
_tmpC55->f2;_tmpC59=*_tmpC58;_tmpC5A=_tmpC59.kind;if(_tmpC5A != Cyc_Absyn_BoxKind)
goto _LL626;_tmpC5B=_tmpC59.aliasqual;if(_tmpC5B != Cyc_Absyn_Top)goto _LL626;}}
_LL625: _tmpC5E=_tmpC57;goto _LL627;_LL626: {struct Cyc_Absyn_Less_kb_struct*
_tmpC5C=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC5C->tag != 2)goto _LL628;
else{_tmpC5D=_tmpC5C->f1;_tmpC5E=(struct Cyc_Core_Opt**)& _tmpC5C->f1;_tmpC5F=
_tmpC5C->f2;_tmpC60=*_tmpC5F;_tmpC61=_tmpC60.kind;if(_tmpC61 != Cyc_Absyn_MemKind)
goto _LL628;_tmpC62=_tmpC60.aliasqual;if(_tmpC62 != Cyc_Absyn_Top)goto _LL628;}}
_LL627: _tmpC65=_tmpC5E;goto _LL629;_LL628: {struct Cyc_Absyn_Less_kb_struct*
_tmpC63=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC63->tag != 2)goto _LL62A;
else{_tmpC64=_tmpC63->f1;_tmpC65=(struct Cyc_Core_Opt**)& _tmpC63->f1;_tmpC66=
_tmpC63->f2;_tmpC67=*_tmpC66;_tmpC68=_tmpC67.kind;if(_tmpC68 != Cyc_Absyn_MemKind)
goto _LL62A;_tmpC69=_tmpC67.aliasqual;if(_tmpC69 != Cyc_Absyn_Aliasable)goto _LL62A;}}
_LL629: _tmpC6C=_tmpC65;goto _LL62B;_LL62A: {struct Cyc_Absyn_Less_kb_struct*
_tmpC6A=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC6A->tag != 2)goto _LL62C;
else{_tmpC6B=_tmpC6A->f1;_tmpC6C=(struct Cyc_Core_Opt**)& _tmpC6A->f1;_tmpC6D=
_tmpC6A->f2;_tmpC6E=*_tmpC6D;_tmpC6F=_tmpC6E.kind;if(_tmpC6F != Cyc_Absyn_AnyKind)
goto _LL62C;_tmpC70=_tmpC6E.aliasqual;if(_tmpC70 != Cyc_Absyn_Top)goto _LL62C;}}
_LL62B: _tmpC73=_tmpC6C;goto _LL62D;_LL62C: {struct Cyc_Absyn_Less_kb_struct*
_tmpC71=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC71->tag != 2)goto _LL62E;
else{_tmpC72=_tmpC71->f1;_tmpC73=(struct Cyc_Core_Opt**)& _tmpC71->f1;_tmpC74=
_tmpC71->f2;_tmpC75=*_tmpC74;_tmpC76=_tmpC75.kind;if(_tmpC76 != Cyc_Absyn_AnyKind)
goto _LL62E;_tmpC77=_tmpC75.aliasqual;if(_tmpC77 != Cyc_Absyn_Aliasable)goto _LL62E;}}
_LL62D:*_tmpC73=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL621;_LL62E: {
struct Cyc_Absyn_Less_kb_struct*_tmpC78=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;
if(_tmpC78->tag != 2)goto _LL630;else{_tmpC79=_tmpC78->f1;_tmpC7A=(struct Cyc_Core_Opt**)&
_tmpC78->f1;_tmpC7B=_tmpC78->f2;_tmpC7C=*_tmpC7B;_tmpC7D=_tmpC7C.kind;if(_tmpC7D
!= Cyc_Absyn_MemKind)goto _LL630;_tmpC7E=_tmpC7C.aliasqual;if(_tmpC7E != Cyc_Absyn_Unique)
goto _LL630;}}_LL62F: _tmpC81=_tmpC7A;goto _LL631;_LL630: {struct Cyc_Absyn_Less_kb_struct*
_tmpC7F=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;if(_tmpC7F->tag != 2)goto _LL632;
else{_tmpC80=_tmpC7F->f1;_tmpC81=(struct Cyc_Core_Opt**)& _tmpC7F->f1;_tmpC82=
_tmpC7F->f2;_tmpC83=*_tmpC82;_tmpC84=_tmpC83.kind;if(_tmpC84 != Cyc_Absyn_AnyKind)
goto _LL632;_tmpC85=_tmpC83.aliasqual;if(_tmpC85 != Cyc_Absyn_Unique)goto _LL632;}}
_LL631:*_tmpC81=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL621;_LL632: {
struct Cyc_Absyn_Less_kb_struct*_tmpC86=(struct Cyc_Absyn_Less_kb_struct*)_tmpC51;
if(_tmpC86->tag != 2)goto _LL634;else{_tmpC87=_tmpC86->f1;_tmpC88=(struct Cyc_Core_Opt**)&
_tmpC86->f1;_tmpC89=_tmpC86->f2;_tmpC8A=*_tmpC89;_tmpC8B=_tmpC8A.kind;if(_tmpC8B
!= Cyc_Absyn_RgnKind)goto _LL634;_tmpC8C=_tmpC8A.aliasqual;if(_tmpC8C != Cyc_Absyn_Top)
goto _LL634;}}_LL633:*_tmpC88=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL621;_LL634: {struct Cyc_Absyn_Less_kb_struct*_tmpC8D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC51;if(_tmpC8D->tag != 2)goto _LL636;else{_tmpC8E=_tmpC8D->f1;_tmpC8F=(struct
Cyc_Core_Opt**)& _tmpC8D->f1;_tmpC90=_tmpC8D->f2;}}_LL635:*_tmpC8F=Cyc_Tcutil_kind_to_bound_opt(
_tmpC90);goto _LL621;_LL636: {struct Cyc_Absyn_Eq_kb_struct*_tmpC91=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC51;if(_tmpC91->tag != 0)goto _LL638;else{_tmpC92=_tmpC91->f1;_tmpC93=*_tmpC92;
_tmpC94=_tmpC93.kind;if(_tmpC94 != Cyc_Absyn_MemKind)goto _LL638;}}_LL637:{const
char*_tmp1466;void*_tmp1465;(_tmp1465=0,Cyc_Tcutil_terr(loc,((_tmp1466="functions can't abstract M types",
_tag_dyneither(_tmp1466,sizeof(char),33))),_tag_dyneither(_tmp1465,sizeof(void*),
0)));}goto _LL621;_LL638:;_LL639: goto _LL621;_LL621:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpBC5.kind_env,*_tmpAD0);_tmpBC5.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpBC5.r,_tmpBC5.free_vars,*_tmpAD0);{struct Cyc_List_List*tvs=_tmpBC5.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC9B;int _tmpC9C;struct
_tuple27 _tmpC9A=*((struct _tuple27*)tvs->hd);_tmpC9B=_tmpC9A.f1;_tmpC9C=_tmpC9A.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpC9B,_tmpC9C);}}{struct Cyc_List_List*evs=_tmpBC5.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpC9E;int _tmpC9F;struct _tuple28 _tmpC9D=*((struct _tuple28*)evs->hd);
_tmpC9E=_tmpC9D.f1;_tmpC9F=_tmpC9D.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpC9E,_tmpC9F);}};}goto _LL54F;};}_LL56E: {struct Cyc_Absyn_TupleType_struct*
_tmpAD9=(struct Cyc_Absyn_TupleType_struct*)_tmpA99;if(_tmpAD9->tag != 11)goto
_LL570;else{_tmpADA=_tmpAD9->f1;}}_LL56F: for(0;_tmpADA != 0;_tmpADA=_tmpADA->tl){
struct _tuple11*_tmpCA1=(struct _tuple11*)_tmpADA->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpCA1).f2,1);((*_tmpCA1).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpCA1).f1).print_const,(*_tmpCA1).f2);}goto _LL54F;_LL570: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpADB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpA99;if(_tmpADB->tag != 13)goto
_LL572;else{_tmpADC=_tmpADB->f1;_tmpADD=_tmpADB->f2;}}_LL571: {struct
_RegionHandle*_tmpCA2=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpADD != 0;_tmpADD=_tmpADD->tl){struct Cyc_Absyn_Aggrfield _tmpCA4;struct
_dyneither_ptr*_tmpCA5;struct Cyc_Absyn_Tqual _tmpCA6;struct Cyc_Absyn_Tqual*
_tmpCA7;void*_tmpCA8;struct Cyc_Absyn_Exp*_tmpCA9;struct Cyc_List_List*_tmpCAA;
struct Cyc_Absyn_Aggrfield*_tmpCA3=(struct Cyc_Absyn_Aggrfield*)_tmpADD->hd;
_tmpCA4=*_tmpCA3;_tmpCA5=_tmpCA4.name;_tmpCA6=_tmpCA4.tq;_tmpCA7=(struct Cyc_Absyn_Tqual*)&(*
_tmpCA3).tq;_tmpCA8=_tmpCA4.type;_tmpCA9=_tmpCA4.width;_tmpCAA=_tmpCA4.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpCA5)){
const char*_tmp146A;void*_tmp1469[1];struct Cyc_String_pa_struct _tmp1468;(_tmp1468.tag=
0,((_tmp1468.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA5),((
_tmp1469[0]=& _tmp1468,Cyc_Tcutil_terr(loc,((_tmp146A="duplicate field %s",
_tag_dyneither(_tmp146A,sizeof(char),19))),_tag_dyneither(_tmp1469,sizeof(void*),
1)))))));}{const char*_tmp146B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpCA5,((
_tmp146B="",_tag_dyneither(_tmp146B,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp146C;prev_fields=((_tmp146C=_region_malloc(_tmpCA2,sizeof(*_tmp146C)),((
_tmp146C->hd=_tmpCA5,((_tmp146C->tl=prev_fields,_tmp146C))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpCA8,1);_tmpCA7->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpCA7->print_const,_tmpCA8);if(_tmpADC == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpCA8)){const char*_tmp1470;void*_tmp146F[1];struct Cyc_String_pa_struct _tmp146E;(
_tmp146E.tag=0,((_tmp146E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpCA5),((_tmp146F[0]=& _tmp146E,Cyc_Tcutil_warn(loc,((_tmp1470="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1470,sizeof(char),74))),_tag_dyneither(_tmp146F,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpCA8,_tmpCA9,_tmpCA5);Cyc_Tcutil_check_field_atts(
loc,_tmpCA5,_tmpCAA);}}goto _LL54F;}_LL572: {struct Cyc_Absyn_AggrType_struct*
_tmpADE=(struct Cyc_Absyn_AggrType_struct*)_tmpA99;if(_tmpADE->tag != 12)goto
_LL574;else{_tmpADF=_tmpADE->f1;_tmpAE0=_tmpADF.aggr_info;_tmpAE1=(union Cyc_Absyn_AggrInfoU*)&(
_tmpADE->f1).aggr_info;_tmpAE2=_tmpADF.targs;_tmpAE3=(struct Cyc_List_List**)&(
_tmpADE->f1).targs;}}_LL573:{union Cyc_Absyn_AggrInfoU _tmpCB3=*_tmpAE1;struct
_tuple4 _tmpCB4;enum Cyc_Absyn_AggrKind _tmpCB5;struct _tuple2*_tmpCB6;struct Cyc_Core_Opt*
_tmpCB7;struct Cyc_Absyn_Aggrdecl**_tmpCB8;struct Cyc_Absyn_Aggrdecl*_tmpCB9;
_LL63B: if((_tmpCB3.UnknownAggr).tag != 1)goto _LL63D;_tmpCB4=(struct _tuple4)(
_tmpCB3.UnknownAggr).val;_tmpCB5=_tmpCB4.f1;_tmpCB6=_tmpCB4.f2;_tmpCB7=_tmpCB4.f3;
_LL63C: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpCBA;_push_handler(&
_tmpCBA);{int _tmpCBC=0;if(setjmp(_tmpCBA.handler))_tmpCBC=1;if(!_tmpCBC){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCB6);{struct Cyc_Absyn_Aggrdecl*_tmpCBD=*adp;if(_tmpCBD->kind != 
_tmpCB5){if(_tmpCBD->kind == Cyc_Absyn_StructA){const char*_tmp1475;void*_tmp1474[
2];struct Cyc_String_pa_struct _tmp1473;struct Cyc_String_pa_struct _tmp1472;(
_tmp1472.tag=0,((_tmp1472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB6)),((_tmp1473.tag=0,((_tmp1473.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCB6)),((_tmp1474[0]=& _tmp1473,((
_tmp1474[1]=& _tmp1472,Cyc_Tcutil_terr(loc,((_tmp1475="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1475,sizeof(char),40))),_tag_dyneither(_tmp1474,sizeof(void*),
2)))))))))))));}else{const char*_tmp147A;void*_tmp1479[2];struct Cyc_String_pa_struct
_tmp1478;struct Cyc_String_pa_struct _tmp1477;(_tmp1477.tag=0,((_tmp1477.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCB6)),((
_tmp1478.tag=0,((_tmp1478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB6)),((_tmp1479[0]=& _tmp1478,((_tmp1479[1]=& _tmp1477,Cyc_Tcutil_terr(loc,((
_tmp147A="expecting union %s instead of struct %s",_tag_dyneither(_tmp147A,
sizeof(char),40))),_tag_dyneither(_tmp1479,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpCB7  && (int)_tmpCB7->v){if(!((unsigned int)_tmpCBD->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCBD->impl))->tagged){const char*
_tmp147E;void*_tmp147D[1];struct Cyc_String_pa_struct _tmp147C;(_tmp147C.tag=0,((
_tmp147C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB6)),((_tmp147D[0]=& _tmp147C,Cyc_Tcutil_terr(loc,((_tmp147E="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp147E,sizeof(char),42))),_tag_dyneither(_tmp147D,sizeof(void*),
1)))))));}}*_tmpAE1=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpCBB=(void*)_exn_thrown;void*_tmpCCA=_tmpCBB;_LL640: {struct Cyc_Dict_Absent_struct*
_tmpCCB=(struct Cyc_Dict_Absent_struct*)_tmpCCA;if(_tmpCCB->tag != Cyc_Dict_Absent)
goto _LL642;}_LL641: {struct Cyc_Tcenv_Genv*_tmpCCC=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp147F;struct Cyc_Absyn_Aggrdecl*_tmpCCD=(_tmp147F=
_cycalloc(sizeof(*_tmp147F)),((_tmp147F->kind=_tmpCB5,((_tmp147F->sc=Cyc_Absyn_Extern,((
_tmp147F->name=_tmpCB6,((_tmp147F->tvs=0,((_tmp147F->impl=0,((_tmp147F->attributes=
0,_tmp147F)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpCCC,loc,_tmpCCD);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCB6);*_tmpAE1=Cyc_Absyn_KnownAggr(adp);if(*_tmpAE3 != 0){{const char*
_tmp1483;void*_tmp1482[1];struct Cyc_String_pa_struct _tmp1481;(_tmp1481.tag=0,((
_tmp1481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB6)),((_tmp1482[0]=& _tmp1481,Cyc_Tcutil_terr(loc,((_tmp1483="declare parameterized type %s before using",
_tag_dyneither(_tmp1483,sizeof(char),43))),_tag_dyneither(_tmp1482,sizeof(void*),
1)))))));}return cvtenv;}goto _LL63F;}_LL642:;_LL643:(void)_throw(_tmpCCA);_LL63F:;}};}
_tmpCB9=*adp;goto _LL63E;}_LL63D: if((_tmpCB3.KnownAggr).tag != 2)goto _LL63A;
_tmpCB8=(struct Cyc_Absyn_Aggrdecl**)(_tmpCB3.KnownAggr).val;_tmpCB9=*_tmpCB8;
_LL63E: {struct Cyc_List_List*tvs=_tmpCB9->tvs;struct Cyc_List_List*ts=*_tmpAE3;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpCD2=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpCD3=(void*)ts->hd;{struct _tuple0 _tmp1484;
struct _tuple0 _tmpCD5=(_tmp1484.f1=Cyc_Absyn_compress_kb(_tmpCD2->kind),((
_tmp1484.f2=_tmpCD3,_tmp1484)));void*_tmpCD6;void*_tmpCD8;struct Cyc_Absyn_Tvar*
_tmpCDA;_LL645: _tmpCD6=_tmpCD5.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpCD7=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpCD6;if(_tmpCD7->tag != 1)goto _LL647;};
_tmpCD8=_tmpCD5.f2;{struct Cyc_Absyn_VarType_struct*_tmpCD9=(struct Cyc_Absyn_VarType_struct*)
_tmpCD8;if(_tmpCD9->tag != 2)goto _LL647;else{_tmpCDA=_tmpCD9->f1;}};_LL646: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpCDA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpCDA,1);continue;_LL647:;_LL648: goto _LL644;_LL644:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1488;void*_tmp1487[1];struct Cyc_String_pa_struct _tmp1486;(
_tmp1486.tag=0,((_tmp1486.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB9->name)),((_tmp1487[0]=& _tmp1486,Cyc_Tcutil_terr(loc,((_tmp1488="too many parameters for type %s",
_tag_dyneither(_tmp1488,sizeof(char),32))),_tag_dyneither(_tmp1487,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1489;hidden_ts=((_tmp1489=_cycalloc(sizeof(*_tmp1489)),((_tmp1489->hd=e,((
_tmp1489->tl=hidden_ts,_tmp1489))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmpAE3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAE3,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL63A:;}goto _LL54F;
_LL574: {struct Cyc_Absyn_TypedefType_struct*_tmpAE4=(struct Cyc_Absyn_TypedefType_struct*)
_tmpA99;if(_tmpAE4->tag != 18)goto _LL576;else{_tmpAE5=_tmpAE4->f1;_tmpAE6=_tmpAE4->f2;
_tmpAE7=(struct Cyc_List_List**)& _tmpAE4->f2;_tmpAE8=_tmpAE4->f3;_tmpAE9=(struct
Cyc_Absyn_Typedefdecl**)& _tmpAE4->f3;_tmpAEA=_tmpAE4->f4;_tmpAEB=(void***)&
_tmpAE4->f4;}}_LL575: {struct Cyc_List_List*targs=*_tmpAE7;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpCDF;_push_handler(& _tmpCDF);{int _tmpCE1=0;if(setjmp(
_tmpCDF.handler))_tmpCE1=1;if(!_tmpCE1){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpAE5);;_pop_handler();}else{void*_tmpCE0=(void*)_exn_thrown;void*_tmpCE3=
_tmpCE0;_LL64A: {struct Cyc_Dict_Absent_struct*_tmpCE4=(struct Cyc_Dict_Absent_struct*)
_tmpCE3;if(_tmpCE4->tag != Cyc_Dict_Absent)goto _LL64C;}_LL64B:{const char*_tmp148D;
void*_tmp148C[1];struct Cyc_String_pa_struct _tmp148B;(_tmp148B.tag=0,((_tmp148B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE5)),((
_tmp148C[0]=& _tmp148B,Cyc_Tcutil_terr(loc,((_tmp148D="unbound typedef name %s",
_tag_dyneither(_tmp148D,sizeof(char),24))),_tag_dyneither(_tmp148C,sizeof(void*),
1)))))));}return cvtenv;_LL64C:;_LL64D:(void)_throw(_tmpCE3);_LL649:;}};}*_tmpAE9=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpAE5[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpCE8=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple15*_tmp1490;struct Cyc_List_List*
_tmp148F;inst=((_tmp148F=_region_malloc(_tmpCE8,sizeof(*_tmp148F)),((_tmp148F->hd=((
_tmp1490=_region_malloc(_tmpCE8,sizeof(*_tmp1490)),((_tmp1490->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1490->f2=(void*)ts->hd,_tmp1490)))))),((_tmp148F->tl=inst,_tmp148F))))));};}
if(ts != 0){const char*_tmp1494;void*_tmp1493[1];struct Cyc_String_pa_struct
_tmp1492;(_tmp1492.tag=0,((_tmp1492.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE5)),((_tmp1493[0]=& _tmp1492,Cyc_Tcutil_terr(
loc,((_tmp1494="too many parameters for typedef %s",_tag_dyneither(_tmp1494,
sizeof(char),35))),_tag_dyneither(_tmp1493,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1495;hidden_ts=((_tmp1495=_cycalloc(sizeof(*_tmp1495)),((
_tmp1495->hd=e,((_tmp1495->tl=hidden_ts,_tmp1495))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp1498;struct Cyc_List_List*_tmp1497;inst=(
struct Cyc_List_List*)((_tmp1497=_cycalloc(sizeof(*_tmp1497)),((_tmp1497->hd=(
struct _tuple15*)((_tmp1498=_cycalloc(sizeof(*_tmp1498)),((_tmp1498->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1498->f2=e,_tmp1498)))))),((_tmp1497->tl=inst,
_tmp1497))))));};}*_tmpAE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpCE8,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1499;*_tmpAEB=((_tmp1499=_cycalloc(sizeof(*
_tmp1499)),((_tmp1499[0]=new_typ,_tmp1499))));}}goto _LL54F;};}_LL576: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpAEC=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpA99;if(_tmpAEC->tag != 22)goto
_LL578;}_LL577: goto _LL579;_LL578: {struct Cyc_Absyn_HeapRgn_struct*_tmpAED=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpA99;if(_tmpAED->tag != 21)goto _LL57A;}_LL579:
goto _LL54F;_LL57A: {struct Cyc_Absyn_RgnHandleType_struct*_tmpAEE=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpA99;if(_tmpAEE->tag != 16)goto _LL57C;else{_tmpAEF=(void*)_tmpAEE->f1;}}_LL57B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpAEF,1);
goto _LL54F;_LL57C: {struct Cyc_Absyn_DynRgnType_struct*_tmpAF0=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpA99;if(_tmpAF0->tag != 17)goto _LL57E;else{_tmpAF1=(void*)_tmpAF0->f1;_tmpAF2=(
void*)_tmpAF0->f2;}}_LL57D: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpAF1,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpAF2,1);goto _LL54F;_LL57E: {struct Cyc_Absyn_AccessEff_struct*_tmpAF3=(struct
Cyc_Absyn_AccessEff_struct*)_tmpA99;if(_tmpAF3->tag != 23)goto _LL580;else{_tmpAF4=(
void*)_tmpAF3->f1;}}_LL57F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpAF4,1);goto _LL54F;_LL580: {struct Cyc_Absyn_RgnsEff_struct*
_tmpAF5=(struct Cyc_Absyn_RgnsEff_struct*)_tmpA99;if(_tmpAF5->tag != 25)goto _LL582;
else{_tmpAF6=(void*)_tmpAF5->f1;}}_LL581: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAF6,1);goto _LL54F;_LL582: {struct Cyc_Absyn_JoinEff_struct*
_tmpAF7=(struct Cyc_Absyn_JoinEff_struct*)_tmpA99;if(_tmpAF7->tag != 24)goto _LL54F;
else{_tmpAF8=_tmpAF7->f1;}}_LL583: for(0;_tmpAF8 != 0;_tmpAF8=_tmpAF8->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpAF8->hd,1);}
goto _LL54F;_LL54F:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpCF2=t;struct Cyc_Core_Opt*_tmpCF4;struct Cyc_Core_Opt*_tmpCF5;_LL64F: {
struct Cyc_Absyn_Evar_struct*_tmpCF3=(struct Cyc_Absyn_Evar_struct*)_tmpCF2;if(
_tmpCF3->tag != 1)goto _LL651;else{_tmpCF4=_tmpCF3->f1;_tmpCF5=_tmpCF3->f2;}}
_LL650: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpCF6=_tmpCF4;struct Cyc_Core_Opt
_tmpCF7;struct Cyc_Absyn_Kind*_tmpCF8;_LL654: if(_tmpCF6 != 0)goto _LL656;_LL655:{
const char*_tmp149A;s=((_tmp149A="kind=NULL ",_tag_dyneither(_tmp149A,sizeof(char),
11)));}goto _LL653;_LL656: if(_tmpCF6 == 0)goto _LL653;_tmpCF7=*_tmpCF6;_tmpCF8=(
struct Cyc_Absyn_Kind*)_tmpCF7.v;_LL657:{const char*_tmp149E;void*_tmp149D[1];
struct Cyc_String_pa_struct _tmp149C;s=(struct _dyneither_ptr)((_tmp149C.tag=0,((
_tmp149C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpCF8)),((_tmp149D[0]=& _tmp149C,Cyc_aprintf(((_tmp149E="kind=%s ",
_tag_dyneither(_tmp149E,sizeof(char),9))),_tag_dyneither(_tmp149D,sizeof(void*),
1))))))));}goto _LL653;_LL653:;}{struct Cyc_Core_Opt*_tmpCFD=_tmpCF5;struct Cyc_Core_Opt
_tmpCFE;void*_tmpCFF;_LL659: if(_tmpCFD != 0)goto _LL65B;_LL65A:{const char*_tmp14A2;
void*_tmp14A1[1];struct Cyc_String_pa_struct _tmp14A0;s=(struct _dyneither_ptr)((
_tmp14A0.tag=0,((_tmp14A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp14A1[0]=& _tmp14A0,Cyc_aprintf(((_tmp14A2="%s ref=NULL",_tag_dyneither(
_tmp14A2,sizeof(char),12))),_tag_dyneither(_tmp14A1,sizeof(void*),1))))))));}
goto _LL658;_LL65B: if(_tmpCFD == 0)goto _LL658;_tmpCFE=*_tmpCFD;_tmpCFF=(void*)
_tmpCFE.v;_LL65C:{const char*_tmp14A7;void*_tmp14A6[2];struct Cyc_String_pa_struct
_tmp14A5;struct Cyc_String_pa_struct _tmp14A4;s=(struct _dyneither_ptr)((_tmp14A4.tag=
0,((_tmp14A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpCFF)),((_tmp14A5.tag=0,((_tmp14A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp14A6[0]=& _tmp14A5,((_tmp14A6[1]=& _tmp14A4,Cyc_aprintf(((
_tmp14A7="%s ref=%s",_tag_dyneither(_tmp14A7,sizeof(char),10))),_tag_dyneither(
_tmp14A6,sizeof(void*),2))))))))))))));}goto _LL658;_LL658:;}{const char*_tmp14AB;
void*_tmp14AA[1];struct Cyc_String_pa_struct _tmp14A9;(_tmp14A9.tag=0,((_tmp14A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp14AA[0]=& _tmp14A9,Cyc_fprintf(
Cyc_stderr,((_tmp14AB="evar info: %s\n",_tag_dyneither(_tmp14AB,sizeof(char),15))),
_tag_dyneither(_tmp14AA,sizeof(void*),1)))))));}goto _LL64E;}_LL651:;_LL652: goto
_LL64E;_LL64E:;}{const char*_tmp14B1;void*_tmp14B0[3];struct Cyc_String_pa_struct
_tmp14AF;struct Cyc_String_pa_struct _tmp14AE;struct Cyc_String_pa_struct _tmp14AD;(
_tmp14AD.tag=0,((_tmp14AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp14AE.tag=0,((_tmp14AE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp14AF.tag=0,((
_tmp14AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp14B0[0]=& _tmp14AF,((_tmp14B0[1]=& _tmp14AE,((_tmp14B0[2]=& _tmp14AD,Cyc_Tcutil_terr(
loc,((_tmp14B1="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp14B1,sizeof(char),51))),_tag_dyneither(_tmp14B0,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpD0F=e->r;
struct Cyc_List_List*_tmpD15;struct Cyc_Absyn_Exp*_tmpD17;struct Cyc_Absyn_Exp*
_tmpD18;struct Cyc_Absyn_Exp*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1B;struct Cyc_Absyn_Exp*
_tmpD1C;struct Cyc_Absyn_Exp*_tmpD1E;struct Cyc_Absyn_Exp*_tmpD1F;struct Cyc_Absyn_Exp*
_tmpD21;struct Cyc_Absyn_Exp*_tmpD22;void*_tmpD24;struct Cyc_Absyn_Exp*_tmpD25;
void*_tmpD27;void*_tmpD29;void*_tmpD2B;struct Cyc_Absyn_Exp*_tmpD2D;_LL65E: {
struct Cyc_Absyn_Const_e_struct*_tmpD10=(struct Cyc_Absyn_Const_e_struct*)_tmpD0F;
if(_tmpD10->tag != 0)goto _LL660;}_LL65F: goto _LL661;_LL660: {struct Cyc_Absyn_Enum_e_struct*
_tmpD11=(struct Cyc_Absyn_Enum_e_struct*)_tmpD0F;if(_tmpD11->tag != 32)goto _LL662;}
_LL661: goto _LL663;_LL662: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD12=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD0F;if(_tmpD12->tag != 33)goto _LL664;}_LL663: goto _LL665;_LL664: {struct Cyc_Absyn_Var_e_struct*
_tmpD13=(struct Cyc_Absyn_Var_e_struct*)_tmpD0F;if(_tmpD13->tag != 1)goto _LL666;}
_LL665: goto _LL65D;_LL666: {struct Cyc_Absyn_Primop_e_struct*_tmpD14=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD0F;if(_tmpD14->tag != 3)goto _LL668;else{_tmpD15=_tmpD14->f2;}}_LL667: for(0;
_tmpD15 != 0;_tmpD15=_tmpD15->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD15->hd,te,cvtenv);}goto _LL65D;_LL668: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD16=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD0F;if(_tmpD16->tag != 6)goto
_LL66A;else{_tmpD17=_tmpD16->f1;_tmpD18=_tmpD16->f2;_tmpD19=_tmpD16->f3;}}_LL669:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD17,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD18,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD19,te,
cvtenv);goto _LL65D;_LL66A: {struct Cyc_Absyn_And_e_struct*_tmpD1A=(struct Cyc_Absyn_And_e_struct*)
_tmpD0F;if(_tmpD1A->tag != 7)goto _LL66C;else{_tmpD1B=_tmpD1A->f1;_tmpD1C=_tmpD1A->f2;}}
_LL66B: _tmpD1E=_tmpD1B;_tmpD1F=_tmpD1C;goto _LL66D;_LL66C: {struct Cyc_Absyn_Or_e_struct*
_tmpD1D=(struct Cyc_Absyn_Or_e_struct*)_tmpD0F;if(_tmpD1D->tag != 8)goto _LL66E;
else{_tmpD1E=_tmpD1D->f1;_tmpD1F=_tmpD1D->f2;}}_LL66D: _tmpD21=_tmpD1E;_tmpD22=
_tmpD1F;goto _LL66F;_LL66E: {struct Cyc_Absyn_SeqExp_e_struct*_tmpD20=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD0F;if(_tmpD20->tag != 9)goto _LL670;else{_tmpD21=_tmpD20->f1;_tmpD22=_tmpD20->f2;}}
_LL66F: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD21,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD22,te,cvtenv);goto _LL65D;_LL670: {
struct Cyc_Absyn_Cast_e_struct*_tmpD23=(struct Cyc_Absyn_Cast_e_struct*)_tmpD0F;
if(_tmpD23->tag != 15)goto _LL672;else{_tmpD24=(void*)_tmpD23->f1;_tmpD25=_tmpD23->f2;}}
_LL671: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD25,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpD24,1);goto
_LL65D;_LL672: {struct Cyc_Absyn_Offsetof_e_struct*_tmpD26=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD0F;if(_tmpD26->tag != 20)goto _LL674;else{_tmpD27=(void*)_tmpD26->f1;}}_LL673:
_tmpD29=_tmpD27;goto _LL675;_LL674: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpD28=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD0F;if(_tmpD28->tag != 18)goto _LL676;
else{_tmpD29=(void*)_tmpD28->f1;}}_LL675: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpD29,1);goto _LL65D;_LL676: {struct Cyc_Absyn_Valueof_e_struct*
_tmpD2A=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD0F;if(_tmpD2A->tag != 39)goto
_LL678;else{_tmpD2B=(void*)_tmpD2A->f1;}}_LL677: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpD2B,1);goto _LL65D;_LL678: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD2C=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD0F;if(_tmpD2C->tag != 19)goto
_LL67A;else{_tmpD2D=_tmpD2C->f1;}}_LL679: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD2D,te,cvtenv);goto _LL65D;_LL67A:;_LL67B: {const char*_tmp14B4;void*_tmp14B3;(
_tmp14B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp14B4="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp14B4,sizeof(char),66))),_tag_dyneither(_tmp14B3,sizeof(void*),
0)));}_LL65D:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpD30=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpD32;struct
_tuple27 _tmpD31=*((struct _tuple27*)vs->hd);_tmpD32=_tmpD31.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpD30,_tmpD32);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpD34;struct _tuple28 _tmpD33=*((struct _tuple28*)evs->hd);_tmpD34=_tmpD33.f1;{
void*_tmpD35=Cyc_Tcutil_compress(_tmpD34);struct Cyc_Core_Opt*_tmpD37;struct Cyc_Core_Opt**
_tmpD38;_LL67D: {struct Cyc_Absyn_Evar_struct*_tmpD36=(struct Cyc_Absyn_Evar_struct*)
_tmpD35;if(_tmpD36->tag != 1)goto _LL67F;else{_tmpD37=_tmpD36->f4;_tmpD38=(struct
Cyc_Core_Opt**)& _tmpD36->f4;}}_LL67E: if(*_tmpD38 == 0){struct Cyc_Core_Opt*
_tmp14B5;*_tmpD38=((_tmp14B5=_cycalloc(sizeof(*_tmp14B5)),((_tmp14B5->v=_tmpD30,
_tmp14B5))));}else{struct Cyc_List_List*_tmpD3A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpD38))->v;struct Cyc_List_List*_tmpD3B=0;for(0;_tmpD3A != 0;
_tmpD3A=_tmpD3A->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpD30,(struct Cyc_Absyn_Tvar*)_tmpD3A->hd)){struct Cyc_List_List*_tmp14B6;
_tmpD3B=((_tmp14B6=_cycalloc(sizeof(*_tmp14B6)),((_tmp14B6->hd=(struct Cyc_Absyn_Tvar*)
_tmpD3A->hd,((_tmp14B6->tl=_tmpD3B,_tmp14B6))))));}}{struct Cyc_Core_Opt*_tmp14B7;*
_tmpD38=((_tmp14B7=_cycalloc(sizeof(*_tmp14B7)),((_tmp14B7->v=_tmpD3B,_tmp14B7))));};}
goto _LL67C;_LL67F:;_LL680: goto _LL67C;_LL67C:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpD3E=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpD3F=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp14B8;struct Cyc_Tcutil_CVTEnv _tmpD40=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp14B8.r=_tmpD3F,((_tmp14B8.kind_env=_tmpD3E,((_tmp14B8.free_vars=0,((_tmp14B8.free_evars=
0,((_tmp14B8.generalize_evars=generalize_evars,((_tmp14B8.fn_result=0,_tmp14B8)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpD41=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple27*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpD3F,(struct Cyc_Absyn_Tvar*(*)(struct _tuple27*))Cyc_Core_fst,
_tmpD40.free_vars);struct Cyc_List_List*_tmpD42=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpD3F,(void*(*)(struct _tuple28*))Cyc_Core_fst,_tmpD40.free_evars);if(_tmpD3E != 
0){struct Cyc_List_List*_tmpD43=0;{struct Cyc_List_List*_tmpD44=_tmpD41;for(0;(
unsigned int)_tmpD44;_tmpD44=_tmpD44->tl){struct Cyc_Absyn_Tvar*_tmpD45=(struct
Cyc_Absyn_Tvar*)_tmpD44->hd;int found=0;{struct Cyc_List_List*_tmpD46=_tmpD3E;for(
0;(unsigned int)_tmpD46;_tmpD46=_tmpD46->tl){if(Cyc_Absyn_tvar_cmp(_tmpD45,(
struct Cyc_Absyn_Tvar*)_tmpD46->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp14B9;_tmpD43=((_tmp14B9=_region_malloc(_tmpD3F,sizeof(*_tmp14B9)),((_tmp14B9->hd=(
struct Cyc_Absyn_Tvar*)_tmpD44->hd,((_tmp14B9->tl=_tmpD43,_tmp14B9))))));}}}
_tmpD41=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD43);}{struct Cyc_List_List*x=_tmpD41;for(0;x != 0;x=x->tl){void*_tmpD48=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpD4A;struct Cyc_Core_Opt**
_tmpD4B;struct Cyc_Core_Opt*_tmpD4D;struct Cyc_Core_Opt**_tmpD4E;struct Cyc_Absyn_Kind*
_tmpD4F;struct Cyc_Absyn_Kind _tmpD50;enum Cyc_Absyn_KindQual _tmpD51;enum Cyc_Absyn_AliasQual
_tmpD52;struct Cyc_Core_Opt*_tmpD54;struct Cyc_Core_Opt**_tmpD55;struct Cyc_Absyn_Kind*
_tmpD56;struct Cyc_Absyn_Kind _tmpD57;enum Cyc_Absyn_KindQual _tmpD58;enum Cyc_Absyn_AliasQual
_tmpD59;struct Cyc_Core_Opt*_tmpD5B;struct Cyc_Core_Opt**_tmpD5C;struct Cyc_Absyn_Kind*
_tmpD5D;struct Cyc_Absyn_Kind _tmpD5E;enum Cyc_Absyn_KindQual _tmpD5F;enum Cyc_Absyn_AliasQual
_tmpD60;struct Cyc_Core_Opt*_tmpD62;struct Cyc_Core_Opt**_tmpD63;struct Cyc_Absyn_Kind*
_tmpD64;struct Cyc_Absyn_Kind _tmpD65;enum Cyc_Absyn_KindQual _tmpD66;enum Cyc_Absyn_AliasQual
_tmpD67;struct Cyc_Core_Opt*_tmpD69;struct Cyc_Core_Opt**_tmpD6A;struct Cyc_Absyn_Kind*
_tmpD6B;struct Cyc_Absyn_Kind _tmpD6C;enum Cyc_Absyn_KindQual _tmpD6D;enum Cyc_Absyn_AliasQual
_tmpD6E;struct Cyc_Core_Opt*_tmpD70;struct Cyc_Core_Opt**_tmpD71;struct Cyc_Absyn_Kind*
_tmpD72;struct Cyc_Absyn_Kind*_tmpD74;struct Cyc_Absyn_Kind _tmpD75;enum Cyc_Absyn_KindQual
_tmpD76;enum Cyc_Absyn_AliasQual _tmpD77;_LL682: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpD49=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpD48;if(_tmpD49->tag != 1)goto
_LL684;else{_tmpD4A=_tmpD49->f1;_tmpD4B=(struct Cyc_Core_Opt**)& _tmpD49->f1;}}
_LL683: _tmpD4E=_tmpD4B;goto _LL685;_LL684: {struct Cyc_Absyn_Less_kb_struct*
_tmpD4C=(struct Cyc_Absyn_Less_kb_struct*)_tmpD48;if(_tmpD4C->tag != 2)goto _LL686;
else{_tmpD4D=_tmpD4C->f1;_tmpD4E=(struct Cyc_Core_Opt**)& _tmpD4C->f1;_tmpD4F=
_tmpD4C->f2;_tmpD50=*_tmpD4F;_tmpD51=_tmpD50.kind;if(_tmpD51 != Cyc_Absyn_BoxKind)
goto _LL686;_tmpD52=_tmpD50.aliasqual;if(_tmpD52 != Cyc_Absyn_Top)goto _LL686;}}
_LL685: _tmpD55=_tmpD4E;goto _LL687;_LL686: {struct Cyc_Absyn_Less_kb_struct*
_tmpD53=(struct Cyc_Absyn_Less_kb_struct*)_tmpD48;if(_tmpD53->tag != 2)goto _LL688;
else{_tmpD54=_tmpD53->f1;_tmpD55=(struct Cyc_Core_Opt**)& _tmpD53->f1;_tmpD56=
_tmpD53->f2;_tmpD57=*_tmpD56;_tmpD58=_tmpD57.kind;if(_tmpD58 != Cyc_Absyn_MemKind)
goto _LL688;_tmpD59=_tmpD57.aliasqual;if(_tmpD59 != Cyc_Absyn_Top)goto _LL688;}}
_LL687: _tmpD5C=_tmpD55;goto _LL689;_LL688: {struct Cyc_Absyn_Less_kb_struct*
_tmpD5A=(struct Cyc_Absyn_Less_kb_struct*)_tmpD48;if(_tmpD5A->tag != 2)goto _LL68A;
else{_tmpD5B=_tmpD5A->f1;_tmpD5C=(struct Cyc_Core_Opt**)& _tmpD5A->f1;_tmpD5D=
_tmpD5A->f2;_tmpD5E=*_tmpD5D;_tmpD5F=_tmpD5E.kind;if(_tmpD5F != Cyc_Absyn_MemKind)
goto _LL68A;_tmpD60=_tmpD5E.aliasqual;if(_tmpD60 != Cyc_Absyn_Aliasable)goto _LL68A;}}
_LL689:*_tmpD5C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL681;_LL68A: {
struct Cyc_Absyn_Less_kb_struct*_tmpD61=(struct Cyc_Absyn_Less_kb_struct*)_tmpD48;
if(_tmpD61->tag != 2)goto _LL68C;else{_tmpD62=_tmpD61->f1;_tmpD63=(struct Cyc_Core_Opt**)&
_tmpD61->f1;_tmpD64=_tmpD61->f2;_tmpD65=*_tmpD64;_tmpD66=_tmpD65.kind;if(_tmpD66
!= Cyc_Absyn_MemKind)goto _LL68C;_tmpD67=_tmpD65.aliasqual;if(_tmpD67 != Cyc_Absyn_Unique)
goto _LL68C;}}_LL68B:*_tmpD63=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL681;_LL68C: {struct Cyc_Absyn_Less_kb_struct*_tmpD68=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD48;if(_tmpD68->tag != 2)goto _LL68E;else{_tmpD69=_tmpD68->f1;_tmpD6A=(struct
Cyc_Core_Opt**)& _tmpD68->f1;_tmpD6B=_tmpD68->f2;_tmpD6C=*_tmpD6B;_tmpD6D=_tmpD6C.kind;
if(_tmpD6D != Cyc_Absyn_RgnKind)goto _LL68E;_tmpD6E=_tmpD6C.aliasqual;if(_tmpD6E != 
Cyc_Absyn_Top)goto _LL68E;}}_LL68D:*_tmpD6A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL681;_LL68E: {struct Cyc_Absyn_Less_kb_struct*_tmpD6F=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD48;if(_tmpD6F->tag != 2)goto _LL690;else{_tmpD70=_tmpD6F->f1;_tmpD71=(struct
Cyc_Core_Opt**)& _tmpD6F->f1;_tmpD72=_tmpD6F->f2;}}_LL68F:*_tmpD71=Cyc_Tcutil_kind_to_bound_opt(
_tmpD72);goto _LL681;_LL690: {struct Cyc_Absyn_Eq_kb_struct*_tmpD73=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpD48;if(_tmpD73->tag != 0)goto _LL692;else{_tmpD74=_tmpD73->f1;_tmpD75=*_tmpD74;
_tmpD76=_tmpD75.kind;if(_tmpD76 != Cyc_Absyn_MemKind)goto _LL692;_tmpD77=_tmpD75.aliasqual;}}
_LL691:{const char*_tmp14C0;void*_tmp14BF[2];struct Cyc_String_pa_struct _tmp14BE;
struct Cyc_Absyn_Kind*_tmp14BD;struct Cyc_String_pa_struct _tmp14BC;(_tmp14BC.tag=0,((
_tmp14BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp14BD=_cycalloc_atomic(sizeof(*_tmp14BD)),((_tmp14BD->kind=Cyc_Absyn_MemKind,((
_tmp14BD->aliasqual=_tmpD77,_tmp14BD)))))))),((_tmp14BE.tag=0,((_tmp14BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp14BF[0]=& _tmp14BE,((_tmp14BF[1]=& _tmp14BC,Cyc_Tcutil_terr(loc,((
_tmp14C0="type variable %s cannot have kind %s",_tag_dyneither(_tmp14C0,sizeof(
char),37))),_tag_dyneither(_tmp14BF,sizeof(void*),2)))))))))))));}goto _LL681;
_LL692:;_LL693: goto _LL681;_LL681:;}}if(_tmpD41 != 0  || _tmpD42 != 0){{void*_tmpD7D=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpD7F;struct Cyc_List_List*_tmpD80;
struct Cyc_List_List**_tmpD81;struct Cyc_Core_Opt*_tmpD82;void*_tmpD83;struct Cyc_List_List*
_tmpD84;int _tmpD85;struct Cyc_Absyn_VarargInfo*_tmpD86;struct Cyc_List_List*
_tmpD87;struct Cyc_List_List*_tmpD88;_LL695: {struct Cyc_Absyn_FnType_struct*
_tmpD7E=(struct Cyc_Absyn_FnType_struct*)_tmpD7D;if(_tmpD7E->tag != 10)goto _LL697;
else{_tmpD7F=_tmpD7E->f1;_tmpD80=_tmpD7F.tvars;_tmpD81=(struct Cyc_List_List**)&(
_tmpD7E->f1).tvars;_tmpD82=_tmpD7F.effect;_tmpD83=_tmpD7F.ret_typ;_tmpD84=
_tmpD7F.args;_tmpD85=_tmpD7F.c_varargs;_tmpD86=_tmpD7F.cyc_varargs;_tmpD87=
_tmpD7F.rgn_po;_tmpD88=_tmpD7F.attributes;}}_LL696: if(*_tmpD81 == 0){*_tmpD81=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpD41);_tmpD41=0;}
goto _LL694;_LL697:;_LL698: goto _LL694;_LL694:;}if(_tmpD41 != 0){const char*_tmp14C4;
void*_tmp14C3[1];struct Cyc_String_pa_struct _tmp14C2;(_tmp14C2.tag=0,((_tmp14C2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpD41->hd)->name),((
_tmp14C3[0]=& _tmp14C2,Cyc_Tcutil_terr(loc,((_tmp14C4="unbound type variable %s",
_tag_dyneither(_tmp14C4,sizeof(char),25))),_tag_dyneither(_tmp14C3,sizeof(void*),
1)))))));}if(_tmpD42 != 0)for(0;_tmpD42 != 0;_tmpD42=_tmpD42->tl){void*e=(void*)
_tmpD42->hd;struct Cyc_Absyn_Kind*_tmpD8C=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpD8D;enum Cyc_Absyn_KindQual _tmpD8E;enum Cyc_Absyn_AliasQual _tmpD8F;struct Cyc_Absyn_Kind
_tmpD90;enum Cyc_Absyn_KindQual _tmpD91;enum Cyc_Absyn_AliasQual _tmpD92;struct Cyc_Absyn_Kind
_tmpD93;enum Cyc_Absyn_KindQual _tmpD94;enum Cyc_Absyn_AliasQual _tmpD95;struct Cyc_Absyn_Kind
_tmpD96;enum Cyc_Absyn_KindQual _tmpD97;_LL69A: _tmpD8D=*_tmpD8C;_tmpD8E=_tmpD8D.kind;
if(_tmpD8E != Cyc_Absyn_RgnKind)goto _LL69C;_tmpD8F=_tmpD8D.aliasqual;if(_tmpD8F != 
Cyc_Absyn_Unique)goto _LL69C;_LL69B: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp14C7;void*_tmp14C6;(_tmp14C6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C7="can't unify evar with unique region!",
_tag_dyneither(_tmp14C7,sizeof(char),37))),_tag_dyneither(_tmp14C6,sizeof(void*),
0)));}goto _LL699;_LL69C: _tmpD90=*_tmpD8C;_tmpD91=_tmpD90.kind;if(_tmpD91 != Cyc_Absyn_RgnKind)
goto _LL69E;_tmpD92=_tmpD90.aliasqual;if(_tmpD92 != Cyc_Absyn_Aliasable)goto _LL69E;
_LL69D: goto _LL69F;_LL69E: _tmpD93=*_tmpD8C;_tmpD94=_tmpD93.kind;if(_tmpD94 != Cyc_Absyn_RgnKind)
goto _LL6A0;_tmpD95=_tmpD93.aliasqual;if(_tmpD95 != Cyc_Absyn_Top)goto _LL6A0;
_LL69F: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14CA;
void*_tmp14C9;(_tmp14C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14CA="can't unify evar with heap!",_tag_dyneither(
_tmp14CA,sizeof(char),28))),_tag_dyneither(_tmp14C9,sizeof(void*),0)));}goto
_LL699;_LL6A0: _tmpD96=*_tmpD8C;_tmpD97=_tmpD96.kind;if(_tmpD97 != Cyc_Absyn_EffKind)
goto _LL6A2;_LL6A1: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14CD;void*_tmp14CC;(_tmp14CC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14CD="can't unify evar with {}!",
_tag_dyneither(_tmp14CD,sizeof(char),26))),_tag_dyneither(_tmp14CC,sizeof(void*),
0)));}goto _LL699;_LL6A2:;_LL6A3:{const char*_tmp14D2;void*_tmp14D1[2];struct Cyc_String_pa_struct
_tmp14D0;struct Cyc_String_pa_struct _tmp14CF;(_tmp14CF.tag=0,((_tmp14CF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14D0.tag=0,((_tmp14D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14D1[0]=& _tmp14D0,((_tmp14D1[1]=& _tmp14CF,Cyc_Tcutil_terr(loc,((
_tmp14D2="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14D2,sizeof(char),52))),_tag_dyneither(_tmp14D1,sizeof(void*),2)))))))))))));}
goto _LL699;_LL699:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpDA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpDA5;struct Cyc_List_List*
_tmpDA6;struct Cyc_Core_Opt*_tmpDA7;void*_tmpDA8;_LL6A5: {struct Cyc_Absyn_FnType_struct*
_tmpDA4=(struct Cyc_Absyn_FnType_struct*)_tmpDA3;if(_tmpDA4->tag != 10)goto _LL6A7;
else{_tmpDA5=_tmpDA4->f1;_tmpDA6=_tmpDA5.tvars;_tmpDA7=_tmpDA5.effect;_tmpDA8=
_tmpDA5.ret_typ;}}_LL6A6: fd->tvs=_tmpDA6;fd->effect=_tmpDA7;goto _LL6A4;_LL6A7:;
_LL6A8: {const char*_tmp14D5;void*_tmp14D4;(_tmp14D4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14D5="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp14D5,sizeof(char),38))),_tag_dyneither(_tmp14D4,sizeof(void*),
0)));}_LL6A4:;}{struct _RegionHandle*_tmpDAB=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp14D6;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple23*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDAB,(struct _dyneither_ptr*(*)(struct _tuple23*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp14D6="function declaration has repeated parameter",
_tag_dyneither(_tmp14D6,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp14D7;fd->cached_typ=((
_tmp14D7=_cycalloc(sizeof(*_tmp14D7)),((_tmp14D7->v=t,_tmp14D7))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpDAE=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp14D8;
struct Cyc_Tcutil_CVTEnv _tmpDAF=Cyc_Tcutil_check_valid_type(loc,te,((_tmp14D8.r=
_tmpDAE,((_tmp14D8.kind_env=bound_tvars,((_tmp14D8.free_vars=0,((_tmp14D8.free_evars=
0,((_tmp14D8.generalize_evars=0,((_tmp14D8.fn_result=0,_tmp14D8)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpDB0=Cyc_Tcutil_remove_bound_tvars(
_tmpDAE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple27*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDAE,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple27*))Cyc_Core_fst,_tmpDAF.free_vars),bound_tvars);struct Cyc_List_List*
_tmpDB1=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple28*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDAE,(void*(*)(struct _tuple28*))Cyc_Core_fst,
_tmpDAF.free_evars);{struct Cyc_List_List*fs=_tmpDB0;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpDB2=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp14DD;void*_tmp14DC[2];struct Cyc_String_pa_struct _tmp14DB;struct Cyc_String_pa_struct
_tmp14DA;(_tmp14DA.tag=0,((_tmp14DA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp14DB.tag=0,((_tmp14DB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpDB2),((_tmp14DC[0]=& _tmp14DB,((
_tmp14DC[1]=& _tmp14DA,Cyc_Tcutil_terr(loc,((_tmp14DD="unbound type variable %s in type %s",
_tag_dyneither(_tmp14DD,sizeof(char),36))),_tag_dyneither(_tmp14DC,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpDB1 != 0)for(0;_tmpDB1 != 0;_tmpDB1=
_tmpDB1->tl){void*e=(void*)_tmpDB1->hd;struct Cyc_Absyn_Kind*_tmpDB7=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpDB8;enum Cyc_Absyn_KindQual _tmpDB9;enum Cyc_Absyn_AliasQual
_tmpDBA;struct Cyc_Absyn_Kind _tmpDBB;enum Cyc_Absyn_KindQual _tmpDBC;enum Cyc_Absyn_AliasQual
_tmpDBD;struct Cyc_Absyn_Kind _tmpDBE;enum Cyc_Absyn_KindQual _tmpDBF;enum Cyc_Absyn_AliasQual
_tmpDC0;struct Cyc_Absyn_Kind _tmpDC1;enum Cyc_Absyn_KindQual _tmpDC2;_LL6AA:
_tmpDB8=*_tmpDB7;_tmpDB9=_tmpDB8.kind;if(_tmpDB9 != Cyc_Absyn_RgnKind)goto _LL6AC;
_tmpDBA=_tmpDB8.aliasqual;if(_tmpDBA != Cyc_Absyn_Unique)goto _LL6AC;_LL6AB: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp14E0;void*
_tmp14DF;(_tmp14DF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14E0="can't unify evar with unique region!",
_tag_dyneither(_tmp14E0,sizeof(char),37))),_tag_dyneither(_tmp14DF,sizeof(void*),
0)));}goto _LL6A9;_LL6AC: _tmpDBB=*_tmpDB7;_tmpDBC=_tmpDBB.kind;if(_tmpDBC != Cyc_Absyn_RgnKind)
goto _LL6AE;_tmpDBD=_tmpDBB.aliasqual;if(_tmpDBD != Cyc_Absyn_Aliasable)goto _LL6AE;
_LL6AD: goto _LL6AF;_LL6AE: _tmpDBE=*_tmpDB7;_tmpDBF=_tmpDBE.kind;if(_tmpDBF != Cyc_Absyn_RgnKind)
goto _LL6B0;_tmpDC0=_tmpDBE.aliasqual;if(_tmpDC0 != Cyc_Absyn_Top)goto _LL6B0;
_LL6AF: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14E3;
void*_tmp14E2;(_tmp14E2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14E3="can't unify evar with heap!",_tag_dyneither(
_tmp14E3,sizeof(char),28))),_tag_dyneither(_tmp14E2,sizeof(void*),0)));}goto
_LL6A9;_LL6B0: _tmpDC1=*_tmpDB7;_tmpDC2=_tmpDC1.kind;if(_tmpDC2 != Cyc_Absyn_EffKind)
goto _LL6B2;_LL6B1: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14E6;void*_tmp14E5;(_tmp14E5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14E6="can't unify evar with {}!",
_tag_dyneither(_tmp14E6,sizeof(char),26))),_tag_dyneither(_tmp14E5,sizeof(void*),
0)));}goto _LL6A9;_LL6B2:;_LL6B3:{const char*_tmp14EB;void*_tmp14EA[2];struct Cyc_String_pa_struct
_tmp14E9;struct Cyc_String_pa_struct _tmp14E8;(_tmp14E8.tag=0,((_tmp14E8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14E9.tag=0,((_tmp14E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14EA[0]=& _tmp14E9,((_tmp14EA[1]=& _tmp14E8,Cyc_Tcutil_terr(loc,((
_tmp14EB="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14EB,sizeof(char),52))),_tag_dyneither(_tmp14EA,sizeof(void*),2)))))))))))));}
goto _LL6A9;_LL6A9:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
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
vs->hd,(void*)vs2->hd)== 0){const char*_tmp14F0;void*_tmp14EF[2];struct Cyc_String_pa_struct
_tmp14EE;struct Cyc_String_pa_struct _tmp14ED;(_tmp14ED.tag=0,((_tmp14ED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp14EE.tag=
0,((_tmp14EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp14EF[0]=&
_tmp14EE,((_tmp14EF[1]=& _tmp14ED,Cyc_Tcutil_terr(loc,((_tmp14F0="%s: %s",
_tag_dyneither(_tmp14F0,sizeof(char),7))),_tag_dyneither(_tmp14EF,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp14F1;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp14F1="duplicate type variable",
_tag_dyneither(_tmp14F1,sizeof(char),24))));}struct _tuple30{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple31{struct Cyc_List_List*f1;void*f2;};struct _tuple32{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp14F2;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp14F2="",_tag_dyneither(_tmp14F2,sizeof(char),1))))!= 0){struct _tuple30*
_tmp14F5;struct Cyc_List_List*_tmp14F4;fields=((_tmp14F4=_cycalloc(sizeof(*
_tmp14F4)),((_tmp14F4->hd=((_tmp14F5=_cycalloc(sizeof(*_tmp14F5)),((_tmp14F5->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp14F5->f2=0,_tmp14F5)))))),((
_tmp14F4->tl=fields,_tmp14F4))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp14F7;const char*_tmp14F6;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp14F7="struct",
_tag_dyneither(_tmp14F7,sizeof(char),7)):((_tmp14F6="union",_tag_dyneither(
_tmp14F6,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple31 _tmpDD7;struct Cyc_List_List*_tmpDD8;void*_tmpDD9;struct _tuple31*
_tmpDD6=(struct _tuple31*)des->hd;_tmpDD7=*_tmpDD6;_tmpDD8=_tmpDD7.f1;_tmpDD9=
_tmpDD7.f2;if(_tmpDD8 == 0){struct Cyc_List_List*_tmpDDA=fields;for(0;_tmpDDA != 0;
_tmpDDA=_tmpDDA->tl){if(!(*((struct _tuple30*)_tmpDDA->hd)).f2){(*((struct
_tuple30*)_tmpDDA->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp14FD;struct
Cyc_Absyn_FieldName_struct _tmp14FC;struct Cyc_List_List*_tmp14FB;(*((struct
_tuple31*)des->hd)).f1=(struct Cyc_List_List*)((_tmp14FB=_cycalloc(sizeof(*
_tmp14FB)),((_tmp14FB->hd=(void*)((_tmp14FD=_cycalloc(sizeof(*_tmp14FD)),((
_tmp14FD[0]=((_tmp14FC.tag=1,((_tmp14FC.f1=((*((struct _tuple30*)_tmpDDA->hd)).f1)->name,
_tmp14FC)))),_tmp14FD)))),((_tmp14FB->tl=0,_tmp14FB))))));}{struct _tuple32*
_tmp1500;struct Cyc_List_List*_tmp14FF;ans=((_tmp14FF=_region_malloc(rgn,sizeof(*
_tmp14FF)),((_tmp14FF->hd=((_tmp1500=_region_malloc(rgn,sizeof(*_tmp1500)),((
_tmp1500->f1=(*((struct _tuple30*)_tmpDDA->hd)).f1,((_tmp1500->f2=_tmpDD9,
_tmp1500)))))),((_tmp14FF->tl=ans,_tmp14FF))))));}break;}}if(_tmpDDA == 0){const
char*_tmp1504;void*_tmp1503[1];struct Cyc_String_pa_struct _tmp1502;(_tmp1502.tag=
0,((_tmp1502.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp1503[0]=& _tmp1502,Cyc_Tcutil_terr(loc,((_tmp1504="too many arguments to %s",
_tag_dyneither(_tmp1504,sizeof(char),25))),_tag_dyneither(_tmp1503,sizeof(void*),
1)))))));}}else{if(_tmpDD8->tl != 0){const char*_tmp1507;void*_tmp1506;(_tmp1506=0,
Cyc_Tcutil_terr(loc,((_tmp1507="multiple designators are not yet supported",
_tag_dyneither(_tmp1507,sizeof(char),43))),_tag_dyneither(_tmp1506,sizeof(void*),
0)));}else{void*_tmpDE5=(void*)_tmpDD8->hd;struct _dyneither_ptr*_tmpDE8;_LL6B5: {
struct Cyc_Absyn_ArrayElement_struct*_tmpDE6=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpDE5;if(_tmpDE6->tag != 0)goto _LL6B7;}_LL6B6:{const char*_tmp150B;void*_tmp150A[
1];struct Cyc_String_pa_struct _tmp1509;(_tmp1509.tag=0,((_tmp1509.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp150A[0]=& _tmp1509,Cyc_Tcutil_terr(
loc,((_tmp150B="array designator used in argument to %s",_tag_dyneither(_tmp150B,
sizeof(char),40))),_tag_dyneither(_tmp150A,sizeof(void*),1)))))));}goto _LL6B4;
_LL6B7: {struct Cyc_Absyn_FieldName_struct*_tmpDE7=(struct Cyc_Absyn_FieldName_struct*)
_tmpDE5;if(_tmpDE7->tag != 1)goto _LL6B4;else{_tmpDE8=_tmpDE7->f1;}}_LL6B8: {
struct Cyc_List_List*_tmpDEC=fields;for(0;_tmpDEC != 0;_tmpDEC=_tmpDEC->tl){if(Cyc_strptrcmp(
_tmpDE8,((*((struct _tuple30*)_tmpDEC->hd)).f1)->name)== 0){if((*((struct _tuple30*)
_tmpDEC->hd)).f2){const char*_tmp150F;void*_tmp150E[1];struct Cyc_String_pa_struct
_tmp150D;(_tmp150D.tag=0,((_tmp150D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpDE8),((_tmp150E[0]=& _tmp150D,Cyc_Tcutil_terr(loc,((_tmp150F="member %s has already been used as an argument",
_tag_dyneither(_tmp150F,sizeof(char),47))),_tag_dyneither(_tmp150E,sizeof(void*),
1)))))));}(*((struct _tuple30*)_tmpDEC->hd)).f2=1;{struct _tuple32*_tmp1512;struct
Cyc_List_List*_tmp1511;ans=((_tmp1511=_region_malloc(rgn,sizeof(*_tmp1511)),((
_tmp1511->hd=((_tmp1512=_region_malloc(rgn,sizeof(*_tmp1512)),((_tmp1512->f1=(*((
struct _tuple30*)_tmpDEC->hd)).f1,((_tmp1512->f2=_tmpDD9,_tmp1512)))))),((
_tmp1511->tl=ans,_tmp1511))))));}break;}}if(_tmpDEC == 0){const char*_tmp1516;void*
_tmp1515[1];struct Cyc_String_pa_struct _tmp1514;(_tmp1514.tag=0,((_tmp1514.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpDE8),((_tmp1515[0]=& _tmp1514,
Cyc_Tcutil_terr(loc,((_tmp1516="bad field designator %s",_tag_dyneither(_tmp1516,
sizeof(char),24))),_tag_dyneither(_tmp1515,sizeof(void*),1)))))));}goto _LL6B4;}
_LL6B4:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple30*)fields->hd)).f2){{const char*_tmp1519;void*_tmp1518;(
_tmp1518=0,Cyc_Tcutil_terr(loc,((_tmp1519="too few arguments to struct",
_tag_dyneither(_tmp1519,sizeof(char),28))),_tag_dyneither(_tmp1518,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple30*)fields->hd)).f2){if(found){const char*_tmp151C;void*_tmp151B;(_tmp151B=
0,Cyc_Tcutil_terr(loc,((_tmp151C="only one member of a union is allowed",
_tag_dyneither(_tmp151C,sizeof(char),38))),_tag_dyneither(_tmp151B,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp151F;void*_tmp151E;(_tmp151E=0,Cyc_Tcutil_terr(
loc,((_tmp151F="missing member for union",_tag_dyneither(_tmp151F,sizeof(char),
25))),_tag_dyneither(_tmp151E,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpDFD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpDFF;
void*_tmpE00;struct Cyc_Absyn_PtrAtts _tmpE01;union Cyc_Absyn_Constraint*_tmpE02;
_LL6BA: {struct Cyc_Absyn_PointerType_struct*_tmpDFE=(struct Cyc_Absyn_PointerType_struct*)
_tmpDFD;if(_tmpDFE->tag != 5)goto _LL6BC;else{_tmpDFF=_tmpDFE->f1;_tmpE00=_tmpDFF.elt_typ;
_tmpE01=_tmpDFF.ptr_atts;_tmpE02=_tmpE01.bounds;}}_LL6BB: {void*_tmpE03=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE02);_LL6BF: {struct Cyc_Absyn_DynEither_b_struct*_tmpE04=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE03;if(_tmpE04->tag != 0)goto _LL6C1;}_LL6C0:*elt_typ_dest=_tmpE00;return 1;
_LL6C1:;_LL6C2: return 0;_LL6BE:;}_LL6BC:;_LL6BD: return 0;_LL6B9:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpE05=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE07;
void*_tmpE08;struct Cyc_Absyn_PtrAtts _tmpE09;union Cyc_Absyn_Constraint*_tmpE0A;
_LL6C4: {struct Cyc_Absyn_PointerType_struct*_tmpE06=(struct Cyc_Absyn_PointerType_struct*)
_tmpE05;if(_tmpE06->tag != 5)goto _LL6C6;else{_tmpE07=_tmpE06->f1;_tmpE08=_tmpE07.elt_typ;
_tmpE09=_tmpE07.ptr_atts;_tmpE0A=_tmpE09.zero_term;}}_LL6C5:*elt_typ_dest=
_tmpE08;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE0A);_LL6C6:;_LL6C7: return 0;_LL6C3:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE0B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE0D;void*_tmpE0E;struct Cyc_Absyn_PtrAtts _tmpE0F;
union Cyc_Absyn_Constraint*_tmpE10;union Cyc_Absyn_Constraint*_tmpE11;struct Cyc_Absyn_ArrayInfo
_tmpE13;void*_tmpE14;struct Cyc_Absyn_Tqual _tmpE15;struct Cyc_Absyn_Exp*_tmpE16;
union Cyc_Absyn_Constraint*_tmpE17;_LL6C9: {struct Cyc_Absyn_PointerType_struct*
_tmpE0C=(struct Cyc_Absyn_PointerType_struct*)_tmpE0B;if(_tmpE0C->tag != 5)goto
_LL6CB;else{_tmpE0D=_tmpE0C->f1;_tmpE0E=_tmpE0D.elt_typ;_tmpE0F=_tmpE0D.ptr_atts;
_tmpE10=_tmpE0F.bounds;_tmpE11=_tmpE0F.zero_term;}}_LL6CA: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE11)){*ptr_type=t;*elt_type=
_tmpE0E;{void*_tmpE18=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE10);_LL6D0: {struct Cyc_Absyn_DynEither_b_struct*
_tmpE19=(struct Cyc_Absyn_DynEither_b_struct*)_tmpE18;if(_tmpE19->tag != 0)goto
_LL6D2;}_LL6D1:*is_dyneither=1;goto _LL6CF;_LL6D2:;_LL6D3:*is_dyneither=0;goto
_LL6CF;_LL6CF:;}return 1;}else{return 0;}_LL6CB: {struct Cyc_Absyn_ArrayType_struct*
_tmpE12=(struct Cyc_Absyn_ArrayType_struct*)_tmpE0B;if(_tmpE12->tag != 9)goto
_LL6CD;else{_tmpE13=_tmpE12->f1;_tmpE14=_tmpE13.elt_type;_tmpE15=_tmpE13.tq;
_tmpE16=_tmpE13.num_elts;_tmpE17=_tmpE13.zero_term;}}_LL6CC: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE17)){*elt_type=_tmpE14;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1534;struct Cyc_Absyn_PtrAtts
_tmp1533;struct Cyc_Absyn_Upper_b_struct _tmp1532;struct Cyc_Absyn_Upper_b_struct*
_tmp1531;struct Cyc_Absyn_PtrInfo _tmp1530;struct Cyc_Absyn_PointerType_struct*
_tmp152F;*ptr_type=(void*)((_tmp152F=_cycalloc(sizeof(*_tmp152F)),((_tmp152F[0]=((
_tmp1534.tag=5,((_tmp1534.f1=((_tmp1530.elt_typ=_tmpE14,((_tmp1530.elt_tq=
_tmpE15,((_tmp1530.ptr_atts=((_tmp1533.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1533.nullable=Cyc_Absyn_false_conref,((_tmp1533.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1531=_cycalloc(sizeof(*_tmp1531)),((
_tmp1531[0]=((_tmp1532.tag=1,((_tmp1532.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpE16),_tmp1532)))),_tmp1531))))),((_tmp1533.zero_term=_tmpE17,((_tmp1533.ptrloc=
0,_tmp1533)))))))))),_tmp1530)))))),_tmp1534)))),_tmp152F))));}return 1;}else{
return 0;}_LL6CD:;_LL6CE: return 0;_LL6C8:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpE20=e1->r;struct Cyc_Absyn_Exp*_tmpE23;struct Cyc_Absyn_Exp*_tmpE25;struct Cyc_Absyn_Exp*
_tmpE27;struct Cyc_Absyn_Exp*_tmpE29;struct Cyc_Absyn_Exp*_tmpE2B;struct Cyc_Absyn_Exp*
_tmpE2D;_LL6D5: {struct Cyc_Absyn_Cast_e_struct*_tmpE21=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE20;if(_tmpE21->tag != 15)goto _LL6D7;}_LL6D6: {const char*_tmp1538;void*
_tmp1537[1];struct Cyc_String_pa_struct _tmp1536;(_tmp1536.tag=0,((_tmp1536.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1537[0]=& _tmp1536,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1538="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1538,sizeof(char),29))),_tag_dyneither(_tmp1537,sizeof(void*),1)))))));}
_LL6D7: {struct Cyc_Absyn_Deref_e_struct*_tmpE22=(struct Cyc_Absyn_Deref_e_struct*)
_tmpE20;if(_tmpE22->tag != 21)goto _LL6D9;else{_tmpE23=_tmpE22->f1;}}_LL6D8:
_tmpE25=_tmpE23;goto _LL6DA;_LL6D9: {struct Cyc_Absyn_Subscript_e_struct*_tmpE24=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpE20;if(_tmpE24->tag != 24)goto _LL6DB;
else{_tmpE25=_tmpE24->f1;}}_LL6DA: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE25->topt))->v,ptr_type,is_dyneither,
elt_type);_LL6DB: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpE26=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE20;if(_tmpE26->tag != 23)goto _LL6DD;else{_tmpE27=_tmpE26->f1;}}_LL6DC:
_tmpE29=_tmpE27;goto _LL6DE;_LL6DD: {struct Cyc_Absyn_AggrMember_e_struct*_tmpE28=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE20;if(_tmpE28->tag != 22)goto _LL6DF;
else{_tmpE29=_tmpE28->f1;}}_LL6DE: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpE29->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_struct _tmp153A;(_tmp153A.tag=
0,((_tmp153A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp153B[0]=& _tmp153A,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp153C="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp153C,sizeof(char),51))),_tag_dyneither(_tmp153B,sizeof(void*),
1)))))));}return 0;_LL6DF: {struct Cyc_Absyn_Instantiate_e_struct*_tmpE2A=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpE20;if(_tmpE2A->tag != 14)goto _LL6E1;else{
_tmpE2B=_tmpE2A->f1;}}_LL6E0: _tmpE2D=_tmpE2B;goto _LL6E2;_LL6E1: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE2C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE20;if(_tmpE2C->tag != 13)
goto _LL6E3;else{_tmpE2D=_tmpE2C->f1;}}_LL6E2: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE2D->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1540;void*_tmp153F[1];struct Cyc_String_pa_struct
_tmp153E;(_tmp153E.tag=0,((_tmp153E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp153F[0]=& _tmp153E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1540="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1540,sizeof(char),49))),_tag_dyneither(_tmp153F,sizeof(void*),
1)))))));}return 0;_LL6E3: {struct Cyc_Absyn_Var_e_struct*_tmpE2E=(struct Cyc_Absyn_Var_e_struct*)
_tmpE20;if(_tmpE2E->tag != 1)goto _LL6E5;}_LL6E4: return 0;_LL6E5:;_LL6E6: {const
char*_tmp1544;void*_tmp1543[1];struct Cyc_String_pa_struct _tmp1542;(_tmp1542.tag=
0,((_tmp1542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1543[0]=& _tmp1542,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1544="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1544,sizeof(char),39))),_tag_dyneither(_tmp1543,sizeof(void*),
1)))))));}_LL6D4:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_nonunique_region(void*r);static int Cyc_Tcutil_is_nonunique_region(
void*r){{void*_tmpE3B=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpE3E;_LL6E8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpE3C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpE3B;if(_tmpE3C->tag != 22)goto _LL6EA;}_LL6E9: return 0;_LL6EA: {struct Cyc_Absyn_VarType_struct*
_tmpE3D=(struct Cyc_Absyn_VarType_struct*)_tmpE3B;if(_tmpE3D->tag != 2)goto _LL6EC;
else{_tmpE3E=_tmpE3D->f1;}}_LL6EB: {struct Cyc_Absyn_Kind*_tmpE3F=Cyc_Tcutil_tvar_kind(
_tmpE3E,& Cyc_Tcutil_rk);if(_tmpE3F->kind == Cyc_Absyn_RgnKind  && _tmpE3F->aliasqual
== Cyc_Absyn_Unique)return 0;goto _LL6E7;}_LL6EC:;_LL6ED: goto _LL6E7;_LL6E7:;}
return 1;}static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);static
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpE40=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpE43;_LL6EF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpE41=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpE40;if(_tmpE41->tag != 22)goto
_LL6F1;}_LL6F0: return 1;_LL6F1: {struct Cyc_Absyn_VarType_struct*_tmpE42=(struct
Cyc_Absyn_VarType_struct*)_tmpE40;if(_tmpE42->tag != 2)goto _LL6F3;else{_tmpE43=
_tmpE42->f1;}}_LL6F2: {struct Cyc_Absyn_Kind _tmpE45;enum Cyc_Absyn_KindQual
_tmpE46;enum Cyc_Absyn_AliasQual _tmpE47;struct Cyc_Absyn_Kind*_tmpE44=Cyc_Tcutil_tvar_kind(
_tmpE43,& Cyc_Tcutil_rk);_tmpE45=*_tmpE44;_tmpE46=_tmpE45.kind;_tmpE47=_tmpE45.aliasqual;
if(_tmpE46 == Cyc_Absyn_RgnKind  && (_tmpE47 == Cyc_Absyn_Unique  || _tmpE47 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpE48=Cyc_Absyn_compress_kb(_tmpE43->kind);struct
Cyc_Core_Opt*_tmpE4A;struct Cyc_Core_Opt**_tmpE4B;struct Cyc_Absyn_Kind*_tmpE4C;
struct Cyc_Absyn_Kind _tmpE4D;enum Cyc_Absyn_KindQual _tmpE4E;enum Cyc_Absyn_AliasQual
_tmpE4F;_LL6F6: {struct Cyc_Absyn_Less_kb_struct*_tmpE49=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE48;if(_tmpE49->tag != 2)goto _LL6F8;else{_tmpE4A=_tmpE49->f1;_tmpE4B=(struct
Cyc_Core_Opt**)& _tmpE49->f1;_tmpE4C=_tmpE49->f2;_tmpE4D=*_tmpE4C;_tmpE4E=_tmpE4D.kind;
if(_tmpE4E != Cyc_Absyn_RgnKind)goto _LL6F8;_tmpE4F=_tmpE4D.aliasqual;if(_tmpE4F != 
Cyc_Absyn_Top)goto _LL6F8;}}_LL6F7:{struct Cyc_Absyn_Less_kb_struct*_tmp154A;
struct Cyc_Absyn_Less_kb_struct _tmp1549;struct Cyc_Core_Opt*_tmp1548;*_tmpE4B=((
_tmp1548=_cycalloc(sizeof(*_tmp1548)),((_tmp1548->v=(void*)((_tmp154A=_cycalloc(
sizeof(*_tmp154A)),((_tmp154A[0]=((_tmp1549.tag=2,((_tmp1549.f1=0,((_tmp1549.f2=&
Cyc_Tcutil_rk,_tmp1549)))))),_tmp154A)))),_tmp1548))));}return 0;_LL6F8:;_LL6F9:
return 1;_LL6F5:;}return 0;}_LL6F3:;_LL6F4: return 0;_LL6EE:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique);static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int
must_be_unique){void*_tmpE53=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE55;struct Cyc_Absyn_PtrAtts _tmpE56;void*_tmpE57;struct Cyc_Absyn_Tvar*_tmpE59;
_LL6FB: {struct Cyc_Absyn_PointerType_struct*_tmpE54=(struct Cyc_Absyn_PointerType_struct*)
_tmpE53;if(_tmpE54->tag != 5)goto _LL6FD;else{_tmpE55=_tmpE54->f1;_tmpE56=_tmpE55.ptr_atts;
_tmpE57=_tmpE56.rgn;}}_LL6FC: return Cyc_Tcutil_is_noalias_region(_tmpE57,
must_be_unique);_LL6FD: {struct Cyc_Absyn_VarType_struct*_tmpE58=(struct Cyc_Absyn_VarType_struct*)
_tmpE53;if(_tmpE58->tag != 2)goto _LL6FF;else{_tmpE59=_tmpE58->f1;}}_LL6FE: {
struct Cyc_Absyn_Kind _tmpE5B;enum Cyc_Absyn_KindQual _tmpE5C;enum Cyc_Absyn_AliasQual
_tmpE5D;struct Cyc_Absyn_Kind*_tmpE5A=Cyc_Tcutil_tvar_kind(_tmpE59,& Cyc_Tcutil_bk);
_tmpE5B=*_tmpE5A;_tmpE5C=_tmpE5B.kind;_tmpE5D=_tmpE5B.aliasqual;switch(_tmpE5C){
case Cyc_Absyn_BoxKind: _LL701: goto _LL702;case Cyc_Absyn_AnyKind: _LL702: goto _LL703;
case Cyc_Absyn_MemKind: _LL703: if(_tmpE5D == Cyc_Absyn_Unique  || _tmpE5D == Cyc_Absyn_Top){
void*_tmpE5E=Cyc_Absyn_compress_kb(_tmpE59->kind);struct Cyc_Core_Opt*_tmpE60;
struct Cyc_Core_Opt**_tmpE61;struct Cyc_Absyn_Kind*_tmpE62;struct Cyc_Absyn_Kind
_tmpE63;enum Cyc_Absyn_KindQual _tmpE64;enum Cyc_Absyn_AliasQual _tmpE65;_LL706: {
struct Cyc_Absyn_Less_kb_struct*_tmpE5F=(struct Cyc_Absyn_Less_kb_struct*)_tmpE5E;
if(_tmpE5F->tag != 2)goto _LL708;else{_tmpE60=_tmpE5F->f1;_tmpE61=(struct Cyc_Core_Opt**)&
_tmpE5F->f1;_tmpE62=_tmpE5F->f2;_tmpE63=*_tmpE62;_tmpE64=_tmpE63.kind;_tmpE65=
_tmpE63.aliasqual;if(_tmpE65 != Cyc_Absyn_Top)goto _LL708;}}_LL707:{struct Cyc_Absyn_Less_kb_struct*
_tmp1554;struct Cyc_Absyn_Kind*_tmp1553;struct Cyc_Absyn_Less_kb_struct _tmp1552;
struct Cyc_Core_Opt*_tmp1551;*_tmpE61=((_tmp1551=_cycalloc(sizeof(*_tmp1551)),((
_tmp1551->v=(void*)((_tmp1554=_cycalloc(sizeof(*_tmp1554)),((_tmp1554[0]=((
_tmp1552.tag=2,((_tmp1552.f1=0,((_tmp1552.f2=((_tmp1553=_cycalloc_atomic(sizeof(*
_tmp1553)),((_tmp1553->kind=_tmpE64,((_tmp1553->aliasqual=Cyc_Absyn_Aliasable,
_tmp1553)))))),_tmp1552)))))),_tmp1554)))),_tmp1551))));}return 0;_LL708:;_LL709:
return 1;_LL705:;}return 0;default: _LL704: return 0;}}_LL6FF:;_LL700: return 0;_LL6FA:;}
int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){
return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpE6A=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpE6A))return 1;{void*_tmpE6B=_tmpE6A;struct Cyc_List_List*_tmpE6D;struct Cyc_Absyn_AggrInfo
_tmpE6F;union Cyc_Absyn_AggrInfoU _tmpE70;struct Cyc_Absyn_Aggrdecl**_tmpE71;struct
Cyc_List_List*_tmpE72;struct Cyc_List_List*_tmpE74;struct Cyc_Absyn_AggrInfo
_tmpE76;union Cyc_Absyn_AggrInfoU _tmpE77;struct _tuple4 _tmpE78;struct Cyc_Absyn_DatatypeInfo
_tmpE7A;union Cyc_Absyn_DatatypeInfoU _tmpE7B;struct Cyc_List_List*_tmpE7C;struct
Cyc_Absyn_DatatypeFieldInfo _tmpE7E;union Cyc_Absyn_DatatypeFieldInfoU _tmpE7F;
struct Cyc_List_List*_tmpE80;_LL70C: {struct Cyc_Absyn_TupleType_struct*_tmpE6C=(
struct Cyc_Absyn_TupleType_struct*)_tmpE6B;if(_tmpE6C->tag != 11)goto _LL70E;else{
_tmpE6D=_tmpE6C->f1;}}_LL70D: while(_tmpE6D != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpE6D->hd)).f2))return 1;_tmpE6D=_tmpE6D->tl;}return 0;
_LL70E: {struct Cyc_Absyn_AggrType_struct*_tmpE6E=(struct Cyc_Absyn_AggrType_struct*)
_tmpE6B;if(_tmpE6E->tag != 12)goto _LL710;else{_tmpE6F=_tmpE6E->f1;_tmpE70=_tmpE6F.aggr_info;
if((_tmpE70.KnownAggr).tag != 2)goto _LL710;_tmpE71=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE70.KnownAggr).val;_tmpE72=_tmpE6F.targs;}}_LL70F: if((*_tmpE71)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpE81=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpE71)->tvs,_tmpE72);struct Cyc_List_List*_tmpE82=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpE71)->impl))->fields;void*t;
while(_tmpE82 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpE81,((struct Cyc_Absyn_Aggrfield*)
_tmpE82->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpE82=_tmpE82->tl;}return 0;}_LL710: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpE73=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpE6B;if(_tmpE73->tag != 13)goto
_LL712;else{_tmpE74=_tmpE73->f2;}}_LL711: while(_tmpE74 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpE74->hd)->type))return 1;_tmpE74=_tmpE74->tl;}
return 0;_LL712: {struct Cyc_Absyn_AggrType_struct*_tmpE75=(struct Cyc_Absyn_AggrType_struct*)
_tmpE6B;if(_tmpE75->tag != 12)goto _LL714;else{_tmpE76=_tmpE75->f1;_tmpE77=_tmpE76.aggr_info;
if((_tmpE77.UnknownAggr).tag != 1)goto _LL714;_tmpE78=(struct _tuple4)(_tmpE77.UnknownAggr).val;}}
_LL713: {const char*_tmp1557;void*_tmp1556;(_tmp1556=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1557="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1557,sizeof(char),36))),_tag_dyneither(_tmp1556,sizeof(void*),
0)));}_LL714: {struct Cyc_Absyn_DatatypeType_struct*_tmpE79=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpE6B;if(_tmpE79->tag != 3)goto _LL716;else{_tmpE7A=_tmpE79->f1;_tmpE7B=_tmpE7A.datatype_info;
_tmpE7C=_tmpE7A.targs;}}_LL715: {union Cyc_Absyn_DatatypeInfoU _tmpE85=_tmpE7B;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpE86;struct _tuple2*_tmpE87;int _tmpE88;
struct Cyc_Absyn_Datatypedecl**_tmpE89;struct Cyc_Absyn_Datatypedecl*_tmpE8A;
struct Cyc_Absyn_Datatypedecl _tmpE8B;struct Cyc_List_List*_tmpE8C;struct Cyc_Core_Opt*
_tmpE8D;_LL71B: if((_tmpE85.UnknownDatatype).tag != 1)goto _LL71D;_tmpE86=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpE85.UnknownDatatype).val;_tmpE87=_tmpE86.name;
_tmpE88=_tmpE86.is_extensible;_LL71C: {const char*_tmp155A;void*_tmp1559;(
_tmp1559=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp155A="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp155A,
sizeof(char),40))),_tag_dyneither(_tmp1559,sizeof(void*),0)));}_LL71D: if((
_tmpE85.KnownDatatype).tag != 2)goto _LL71A;_tmpE89=(struct Cyc_Absyn_Datatypedecl**)(
_tmpE85.KnownDatatype).val;_tmpE8A=*_tmpE89;_tmpE8B=*_tmpE8A;_tmpE8C=_tmpE8B.tvs;
_tmpE8D=_tmpE8B.fields;_LL71E: return 0;_LL71A:;}_LL716: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpE7D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpE6B;if(_tmpE7D->tag != 4)
goto _LL718;else{_tmpE7E=_tmpE7D->f1;_tmpE7F=_tmpE7E.field_info;_tmpE80=_tmpE7E.targs;}}
_LL717: {union Cyc_Absyn_DatatypeFieldInfoU _tmpE90=_tmpE7F;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpE91;struct _tuple3 _tmpE92;struct Cyc_Absyn_Datatypedecl*_tmpE93;struct Cyc_Absyn_Datatypefield*
_tmpE94;_LL720: if((_tmpE90.UnknownDatatypefield).tag != 1)goto _LL722;_tmpE91=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpE90.UnknownDatatypefield).val;
_LL721: {const char*_tmp155D;void*_tmp155C;(_tmp155C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp155D="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp155D,sizeof(char),46))),_tag_dyneither(_tmp155C,sizeof(void*),
0)));}_LL722: if((_tmpE90.KnownDatatypefield).tag != 2)goto _LL71F;_tmpE92=(struct
_tuple3)(_tmpE90.KnownDatatypefield).val;_tmpE93=_tmpE92.f1;_tmpE94=_tmpE92.f2;
_LL723: {struct Cyc_List_List*_tmpE97=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpE93->tvs,_tmpE80);struct Cyc_List_List*_tmpE98=
_tmpE94->typs;while(_tmpE98 != 0){_tmpE6A=Cyc_Tcutil_rsubstitute(rgn,_tmpE97,(*((
struct _tuple11*)_tmpE98->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpE6A))return 1;_tmpE98=_tmpE98->tl;}return 0;}_LL71F:;}_LL718:;_LL719: return 0;
_LL70B:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpE99=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpE9B;union Cyc_Absyn_AggrInfoU _tmpE9C;struct Cyc_Absyn_Aggrdecl**_tmpE9D;struct
Cyc_Absyn_Aggrdecl*_tmpE9E;struct Cyc_List_List*_tmpE9F;struct Cyc_List_List*
_tmpEA1;_LL725: {struct Cyc_Absyn_AggrType_struct*_tmpE9A=(struct Cyc_Absyn_AggrType_struct*)
_tmpE99;if(_tmpE9A->tag != 12)goto _LL727;else{_tmpE9B=_tmpE9A->f1;_tmpE9C=_tmpE9B.aggr_info;
if((_tmpE9C.KnownAggr).tag != 2)goto _LL727;_tmpE9D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE9C.KnownAggr).val;_tmpE9E=*_tmpE9D;_tmpE9F=_tmpE9B.targs;}}_LL726: _tmpEA1=
_tmpE9E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE9E->impl))->fields;
goto _LL728;_LL727: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEA0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE99;if(_tmpEA0->tag != 13)goto _LL729;else{_tmpEA1=_tmpEA0->f2;}}_LL728: {
struct Cyc_Absyn_Aggrfield*_tmpEA2=Cyc_Absyn_lookup_field(_tmpEA1,f);{struct Cyc_Absyn_Aggrfield*
_tmpEA3=_tmpEA2;struct Cyc_Absyn_Aggrfield _tmpEA4;void*_tmpEA5;_LL72C: if(_tmpEA3
!= 0)goto _LL72E;_LL72D: {const char*_tmp1560;void*_tmp155F;(_tmp155F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1560="is_noalias_field: missing field",
_tag_dyneither(_tmp1560,sizeof(char),32))),_tag_dyneither(_tmp155F,sizeof(void*),
0)));}_LL72E: if(_tmpEA3 == 0)goto _LL72B;_tmpEA4=*_tmpEA3;_tmpEA5=_tmpEA4.type;
_LL72F: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpEA5);_LL72B:;}return 0;}
_LL729:;_LL72A: {const char*_tmp1564;void*_tmp1563[1];struct Cyc_String_pa_struct
_tmp1562;(_tmp1562.tag=0,((_tmp1562.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1563[0]=& _tmp1562,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1564="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1564,sizeof(char),36))),_tag_dyneither(_tmp1563,sizeof(void*),
1)))))));}_LL724:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_is_noalias_path_aux(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){void*_tmpEAB=e->r;void*_tmpEAD;struct Cyc_Absyn_Exp*
_tmpEB0;struct Cyc_Absyn_Exp*_tmpEB2;struct Cyc_Absyn_Exp*_tmpEB4;struct
_dyneither_ptr*_tmpEB5;struct Cyc_Absyn_Exp*_tmpEB7;struct Cyc_Absyn_Exp*_tmpEB8;
struct Cyc_Absyn_Exp*_tmpEBA;struct Cyc_Absyn_Exp*_tmpEBB;struct Cyc_Absyn_Exp*
_tmpEBD;struct Cyc_Absyn_Exp*_tmpEBF;struct Cyc_Absyn_Stmt*_tmpEC1;_LL731: {struct
Cyc_Absyn_Var_e_struct*_tmpEAC=(struct Cyc_Absyn_Var_e_struct*)_tmpEAB;if(_tmpEAC->tag
!= 1)goto _LL733;else{_tmpEAD=(void*)_tmpEAC->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpEAE=(struct Cyc_Absyn_Global_b_struct*)_tmpEAD;if(_tmpEAE->tag != 1)goto _LL733;};}}
_LL732: return 0;_LL733: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEAF=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEAB;if(_tmpEAF->tag != 23)goto _LL735;else{_tmpEB0=_tmpEAF->f1;}}_LL734:
_tmpEB2=_tmpEB0;goto _LL736;_LL735: {struct Cyc_Absyn_Deref_e_struct*_tmpEB1=(
struct Cyc_Absyn_Deref_e_struct*)_tmpEAB;if(_tmpEB1->tag != 21)goto _LL737;else{
_tmpEB2=_tmpEB1->f1;}}_LL736: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEB2->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEB2);_LL737: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEB3=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEAB;if(_tmpEB3->tag != 22)goto _LL739;else{_tmpEB4=_tmpEB3->f1;_tmpEB5=_tmpEB3->f2;}}
_LL738: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB4);_LL739: {struct Cyc_Absyn_Subscript_e_struct*
_tmpEB6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpEAB;if(_tmpEB6->tag != 24)goto
_LL73B;else{_tmpEB7=_tmpEB6->f1;_tmpEB8=_tmpEB6->f2;}}_LL73A: {void*_tmpEC2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEB7->topt))->v);_LL746: {struct Cyc_Absyn_TupleType_struct*
_tmpEC3=(struct Cyc_Absyn_TupleType_struct*)_tmpEC2;if(_tmpEC3->tag != 11)goto
_LL748;}_LL747: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB7);_LL748:;_LL749:
return 0;_LL745:;}_LL73B: {struct Cyc_Absyn_Conditional_e_struct*_tmpEB9=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpEAB;if(_tmpEB9->tag != 6)goto _LL73D;else{
_tmpEBA=_tmpEB9->f2;_tmpEBB=_tmpEB9->f3;}}_LL73C: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEBA) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpEBB);_LL73D: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpEBC=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpEAB;if(_tmpEBC->tag != 9)goto _LL73F;
else{_tmpEBD=_tmpEBC->f2;}}_LL73E: _tmpEBF=_tmpEBD;goto _LL740;_LL73F: {struct Cyc_Absyn_Cast_e_struct*
_tmpEBE=(struct Cyc_Absyn_Cast_e_struct*)_tmpEAB;if(_tmpEBE->tag != 15)goto _LL741;
else{_tmpEBF=_tmpEBE->f2;}}_LL740: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEBF);
_LL741: {struct Cyc_Absyn_StmtExp_e_struct*_tmpEC0=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpEAB;if(_tmpEC0->tag != 37)goto _LL743;else{_tmpEC1=_tmpEC0->f1;}}_LL742: while(
1){void*_tmpEC4=_tmpEC1->r;struct Cyc_Absyn_Stmt*_tmpEC6;struct Cyc_Absyn_Stmt*
_tmpEC7;struct Cyc_Absyn_Decl*_tmpEC9;struct Cyc_Absyn_Stmt*_tmpECA;struct Cyc_Absyn_Exp*
_tmpECC;_LL74B: {struct Cyc_Absyn_Seq_s_struct*_tmpEC5=(struct Cyc_Absyn_Seq_s_struct*)
_tmpEC4;if(_tmpEC5->tag != 2)goto _LL74D;else{_tmpEC6=_tmpEC5->f1;_tmpEC7=_tmpEC5->f2;}}
_LL74C: _tmpEC1=_tmpEC7;goto _LL74A;_LL74D: {struct Cyc_Absyn_Decl_s_struct*_tmpEC8=(
struct Cyc_Absyn_Decl_s_struct*)_tmpEC4;if(_tmpEC8->tag != 12)goto _LL74F;else{
_tmpEC9=_tmpEC8->f1;_tmpECA=_tmpEC8->f2;}}_LL74E: _tmpEC1=_tmpECA;goto _LL74A;
_LL74F: {struct Cyc_Absyn_Exp_s_struct*_tmpECB=(struct Cyc_Absyn_Exp_s_struct*)
_tmpEC4;if(_tmpECB->tag != 1)goto _LL751;else{_tmpECC=_tmpECB->f1;}}_LL750: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpECC);_LL751:;_LL752: {const char*_tmp1567;
void*_tmp1566;(_tmp1566=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1567="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1567,sizeof(char),40))),_tag_dyneither(_tmp1566,sizeof(void*),
0)));}_LL74A:;}_LL743:;_LL744: return 1;_LL730:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp1568;struct _tuple17 bogus_ans=(_tmp1568.f1=0,((_tmp1568.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1568)));void*_tmpECF=e->r;struct _tuple2*_tmpED1;
void*_tmpED2;struct Cyc_Absyn_Exp*_tmpED4;struct _dyneither_ptr*_tmpED5;int _tmpED6;
struct Cyc_Absyn_Exp*_tmpED8;struct _dyneither_ptr*_tmpED9;int _tmpEDA;struct Cyc_Absyn_Exp*
_tmpEDC;struct Cyc_Absyn_Exp*_tmpEDE;struct Cyc_Absyn_Exp*_tmpEDF;_LL754: {struct
Cyc_Absyn_Var_e_struct*_tmpED0=(struct Cyc_Absyn_Var_e_struct*)_tmpECF;if(_tmpED0->tag
!= 1)goto _LL756;else{_tmpED1=_tmpED0->f1;_tmpED2=(void*)_tmpED0->f2;}}_LL755: {
void*_tmpEE0=_tmpED2;struct Cyc_Absyn_Vardecl*_tmpEE4;struct Cyc_Absyn_Vardecl*
_tmpEE6;struct Cyc_Absyn_Vardecl*_tmpEE8;struct Cyc_Absyn_Vardecl*_tmpEEA;_LL761: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpEE1=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpEE0;if(_tmpEE1->tag != 0)goto _LL763;}_LL762: goto _LL764;_LL763: {struct Cyc_Absyn_Funname_b_struct*
_tmpEE2=(struct Cyc_Absyn_Funname_b_struct*)_tmpEE0;if(_tmpEE2->tag != 2)goto
_LL765;}_LL764: return bogus_ans;_LL765: {struct Cyc_Absyn_Global_b_struct*_tmpEE3=(
struct Cyc_Absyn_Global_b_struct*)_tmpEE0;if(_tmpEE3->tag != 1)goto _LL767;else{
_tmpEE4=_tmpEE3->f1;}}_LL766: {void*_tmpEEB=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL76E: {struct Cyc_Absyn_ArrayType_struct*
_tmpEEC=(struct Cyc_Absyn_ArrayType_struct*)_tmpEEB;if(_tmpEEC->tag != 9)goto
_LL770;}_LL76F: {struct _tuple17 _tmp1569;return(_tmp1569.f1=1,((_tmp1569.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1569)));}_LL770:;_LL771: {struct _tuple17 _tmp156A;
return(_tmp156A.f1=(_tmpEE4->tq).real_const,((_tmp156A.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp156A)));}_LL76D:;}_LL767: {struct Cyc_Absyn_Local_b_struct*_tmpEE5=(struct Cyc_Absyn_Local_b_struct*)
_tmpEE0;if(_tmpEE5->tag != 4)goto _LL769;else{_tmpEE6=_tmpEE5->f1;}}_LL768: {void*
_tmpEEF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL773: {struct Cyc_Absyn_ArrayType_struct*_tmpEF0=(struct Cyc_Absyn_ArrayType_struct*)
_tmpEEF;if(_tmpEF0->tag != 9)goto _LL775;}_LL774: {struct _tuple17 _tmp156B;return(
_tmp156B.f1=1,((_tmp156B.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE6->rgn))->v,
_tmp156B)));}_LL775:;_LL776: _tmpEE6->escapes=1;{struct _tuple17 _tmp156C;return(
_tmp156C.f1=(_tmpEE6->tq).real_const,((_tmp156C.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE6->rgn))->v,_tmp156C)));};_LL772:;}_LL769: {struct Cyc_Absyn_Pat_b_struct*
_tmpEE7=(struct Cyc_Absyn_Pat_b_struct*)_tmpEE0;if(_tmpEE7->tag != 5)goto _LL76B;
else{_tmpEE8=_tmpEE7->f1;}}_LL76A: _tmpEEA=_tmpEE8;goto _LL76C;_LL76B: {struct Cyc_Absyn_Param_b_struct*
_tmpEE9=(struct Cyc_Absyn_Param_b_struct*)_tmpEE0;if(_tmpEE9->tag != 3)goto _LL760;
else{_tmpEEA=_tmpEE9->f1;}}_LL76C: _tmpEEA->escapes=1;{struct _tuple17 _tmp156D;
return(_tmp156D.f1=(_tmpEEA->tq).real_const,((_tmp156D.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEEA->rgn))->v,_tmp156D)));};_LL760:;}_LL756: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpED3=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpECF;if(_tmpED3->tag != 22)goto
_LL758;else{_tmpED4=_tmpED3->f1;_tmpED5=_tmpED3->f2;_tmpED6=_tmpED3->f3;}}_LL757:
if(_tmpED6)return bogus_ans;{void*_tmpEF4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpED4->topt))->v);struct Cyc_List_List*_tmpEF6;struct Cyc_Absyn_AggrInfo
_tmpEF8;union Cyc_Absyn_AggrInfoU _tmpEF9;struct Cyc_Absyn_Aggrdecl**_tmpEFA;struct
Cyc_Absyn_Aggrdecl*_tmpEFB;_LL778: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEF5=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF4;if(_tmpEF5->tag != 13)goto _LL77A;
else{_tmpEF6=_tmpEF5->f2;}}_LL779: {struct Cyc_Absyn_Aggrfield*_tmpEFC=Cyc_Absyn_lookup_field(
_tmpEF6,_tmpED5);if(_tmpEFC != 0  && _tmpEFC->width != 0){struct _tuple17 _tmp156E;
return(_tmp156E.f1=(_tmpEFC->tq).real_const,((_tmp156E.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED4)).f2,_tmp156E)));}return bogus_ans;}_LL77A: {struct Cyc_Absyn_AggrType_struct*
_tmpEF7=(struct Cyc_Absyn_AggrType_struct*)_tmpEF4;if(_tmpEF7->tag != 12)goto
_LL77C;else{_tmpEF8=_tmpEF7->f1;_tmpEF9=_tmpEF8.aggr_info;if((_tmpEF9.KnownAggr).tag
!= 2)goto _LL77C;_tmpEFA=(struct Cyc_Absyn_Aggrdecl**)(_tmpEF9.KnownAggr).val;
_tmpEFB=*_tmpEFA;}}_LL77B: {struct Cyc_Absyn_Aggrfield*_tmpEFE=Cyc_Absyn_lookup_decl_field(
_tmpEFB,_tmpED5);if(_tmpEFE != 0  && _tmpEFE->width != 0){struct _tuple17 _tmp156F;
return(_tmp156F.f1=(_tmpEFE->tq).real_const,((_tmp156F.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED4)).f2,_tmp156F)));}return bogus_ans;}_LL77C:;_LL77D: return bogus_ans;
_LL777:;};_LL758: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpED7=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpECF;if(_tmpED7->tag != 23)goto _LL75A;else{_tmpED8=_tmpED7->f1;_tmpED9=_tmpED7->f2;
_tmpEDA=_tmpED7->f3;}}_LL759: if(_tmpEDA)return bogus_ans;{void*_tmpF00=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED8->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF02;void*_tmpF03;struct Cyc_Absyn_PtrAtts _tmpF04;void*_tmpF05;_LL77F: {struct
Cyc_Absyn_PointerType_struct*_tmpF01=(struct Cyc_Absyn_PointerType_struct*)
_tmpF00;if(_tmpF01->tag != 5)goto _LL781;else{_tmpF02=_tmpF01->f1;_tmpF03=_tmpF02.elt_typ;
_tmpF04=_tmpF02.ptr_atts;_tmpF05=_tmpF04.rgn;}}_LL780: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF06=Cyc_Tcutil_compress(_tmpF03);struct Cyc_List_List*_tmpF08;
struct Cyc_Absyn_AggrInfo _tmpF0A;union Cyc_Absyn_AggrInfoU _tmpF0B;struct Cyc_Absyn_Aggrdecl**
_tmpF0C;struct Cyc_Absyn_Aggrdecl*_tmpF0D;_LL784: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF07=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF06;if(_tmpF07->tag != 13)goto
_LL786;else{_tmpF08=_tmpF07->f2;}}_LL785: finfo=Cyc_Absyn_lookup_field(_tmpF08,
_tmpED9);goto _LL783;_LL786: {struct Cyc_Absyn_AggrType_struct*_tmpF09=(struct Cyc_Absyn_AggrType_struct*)
_tmpF06;if(_tmpF09->tag != 12)goto _LL788;else{_tmpF0A=_tmpF09->f1;_tmpF0B=_tmpF0A.aggr_info;
if((_tmpF0B.KnownAggr).tag != 2)goto _LL788;_tmpF0C=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF0B.KnownAggr).val;_tmpF0D=*_tmpF0C;}}_LL787: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF0D,_tmpED9);goto _LL783;_LL788:;_LL789: return bogus_ans;_LL783:;}if(finfo != 0
 && finfo->width != 0){struct _tuple17 _tmp1570;return(_tmp1570.f1=(finfo->tq).real_const,((
_tmp1570.f2=_tmpF05,_tmp1570)));}return bogus_ans;}_LL781:;_LL782: return bogus_ans;
_LL77E:;};_LL75A: {struct Cyc_Absyn_Deref_e_struct*_tmpEDB=(struct Cyc_Absyn_Deref_e_struct*)
_tmpECF;if(_tmpEDB->tag != 21)goto _LL75C;else{_tmpEDC=_tmpEDB->f1;}}_LL75B: {void*
_tmpF0F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEDC->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF11;struct Cyc_Absyn_Tqual _tmpF12;struct Cyc_Absyn_PtrAtts
_tmpF13;void*_tmpF14;_LL78B: {struct Cyc_Absyn_PointerType_struct*_tmpF10=(struct
Cyc_Absyn_PointerType_struct*)_tmpF0F;if(_tmpF10->tag != 5)goto _LL78D;else{
_tmpF11=_tmpF10->f1;_tmpF12=_tmpF11.elt_tq;_tmpF13=_tmpF11.ptr_atts;_tmpF14=
_tmpF13.rgn;}}_LL78C: {struct _tuple17 _tmp1571;return(_tmp1571.f1=_tmpF12.real_const,((
_tmp1571.f2=_tmpF14,_tmp1571)));}_LL78D:;_LL78E: return bogus_ans;_LL78A:;}_LL75C: {
struct Cyc_Absyn_Subscript_e_struct*_tmpEDD=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpECF;if(_tmpEDD->tag != 24)goto _LL75E;else{_tmpEDE=_tmpEDD->f1;_tmpEDF=_tmpEDD->f2;}}
_LL75D: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpEDE->topt))->v);void*_tmpF16=t;struct Cyc_List_List*_tmpF18;struct Cyc_Absyn_PtrInfo
_tmpF1A;struct Cyc_Absyn_Tqual _tmpF1B;struct Cyc_Absyn_PtrAtts _tmpF1C;void*_tmpF1D;
struct Cyc_Absyn_ArrayInfo _tmpF1F;struct Cyc_Absyn_Tqual _tmpF20;_LL790: {struct Cyc_Absyn_TupleType_struct*
_tmpF17=(struct Cyc_Absyn_TupleType_struct*)_tmpF16;if(_tmpF17->tag != 11)goto
_LL792;else{_tmpF18=_tmpF17->f1;}}_LL791: {unsigned int _tmpF22;int _tmpF23;struct
_tuple13 _tmpF21=Cyc_Evexp_eval_const_uint_exp(_tmpEDF);_tmpF22=_tmpF21.f1;
_tmpF23=_tmpF21.f2;if(!_tmpF23)return bogus_ans;{struct _tuple11*_tmpF24=Cyc_Absyn_lookup_tuple_field(
_tmpF18,(int)_tmpF22);if(_tmpF24 != 0){struct _tuple17 _tmp1572;return(_tmp1572.f1=((*
_tmpF24).f1).real_const,((_tmp1572.f2=(Cyc_Tcutil_addressof_props(te,_tmpEDE)).f2,
_tmp1572)));}return bogus_ans;};}_LL792: {struct Cyc_Absyn_PointerType_struct*
_tmpF19=(struct Cyc_Absyn_PointerType_struct*)_tmpF16;if(_tmpF19->tag != 5)goto
_LL794;else{_tmpF1A=_tmpF19->f1;_tmpF1B=_tmpF1A.elt_tq;_tmpF1C=_tmpF1A.ptr_atts;
_tmpF1D=_tmpF1C.rgn;}}_LL793: {struct _tuple17 _tmp1573;return(_tmp1573.f1=_tmpF1B.real_const,((
_tmp1573.f2=_tmpF1D,_tmp1573)));}_LL794: {struct Cyc_Absyn_ArrayType_struct*
_tmpF1E=(struct Cyc_Absyn_ArrayType_struct*)_tmpF16;if(_tmpF1E->tag != 9)goto
_LL796;else{_tmpF1F=_tmpF1E->f1;_tmpF20=_tmpF1F.tq;}}_LL795: {struct _tuple17
_tmp1574;return(_tmp1574.f1=_tmpF20.real_const,((_tmp1574.f2=(Cyc_Tcutil_addressof_props(
te,_tmpEDE)).f2,_tmp1574)));}_LL796:;_LL797: return bogus_ans;_LL78F:;}_LL75E:;
_LL75F:{const char*_tmp1577;void*_tmp1576;(_tmp1576=0,Cyc_Tcutil_terr(e->loc,((
_tmp1577="unary & applied to non-lvalue",_tag_dyneither(_tmp1577,sizeof(char),30))),
_tag_dyneither(_tmp1576,sizeof(void*),0)));}return bogus_ans;_LL753:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpF2B=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpF2D;void*_tmpF2E;struct Cyc_Absyn_Tqual
_tmpF2F;union Cyc_Absyn_Constraint*_tmpF30;_LL799: {struct Cyc_Absyn_ArrayType_struct*
_tmpF2C=(struct Cyc_Absyn_ArrayType_struct*)_tmpF2B;if(_tmpF2C->tag != 9)goto
_LL79B;else{_tmpF2D=_tmpF2C->f1;_tmpF2E=_tmpF2D.elt_type;_tmpF2F=_tmpF2D.tq;
_tmpF30=_tmpF2D.zero_term;}}_LL79A: {void*_tmpF32;struct _tuple17 _tmpF31=Cyc_Tcutil_addressof_props(
te,e);_tmpF32=_tmpF31.f2;{struct Cyc_Absyn_Upper_b_struct _tmp157A;struct Cyc_Absyn_Upper_b_struct*
_tmp1579;return Cyc_Absyn_atb_typ(_tmpF2E,_tmpF32,_tmpF2F,(void*)((_tmp1579=
_cycalloc(sizeof(*_tmp1579)),((_tmp1579[0]=((_tmp157A.tag=1,((_tmp157A.f1=e,
_tmp157A)))),_tmp1579)))),_tmpF30);};}_LL79B:;_LL79C: return e_typ;_LL798:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpF35=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpF38;
_LL79E: {struct Cyc_Absyn_DynEither_b_struct*_tmpF36=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpF35;if(_tmpF36->tag != 0)goto _LL7A0;}_LL79F: return;_LL7A0: {struct Cyc_Absyn_Upper_b_struct*
_tmpF37=(struct Cyc_Absyn_Upper_b_struct*)_tmpF35;if(_tmpF37->tag != 1)goto _LL79D;
else{_tmpF38=_tmpF37->f1;}}_LL7A1: {unsigned int _tmpF3A;int _tmpF3B;struct
_tuple13 _tmpF39=Cyc_Evexp_eval_const_uint_exp(_tmpF38);_tmpF3A=_tmpF39.f1;
_tmpF3B=_tmpF39.f2;if(_tmpF3B  && _tmpF3A <= i){const char*_tmp157F;void*_tmp157E[2];
struct Cyc_Int_pa_struct _tmp157D;struct Cyc_Int_pa_struct _tmp157C;(_tmp157C.tag=1,((
_tmp157C.f1=(unsigned long)((int)i),((_tmp157D.tag=1,((_tmp157D.f1=(
unsigned long)((int)_tmpF3A),((_tmp157E[0]=& _tmp157D,((_tmp157E[1]=& _tmp157C,Cyc_Tcutil_terr(
loc,((_tmp157F="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp157F,
sizeof(char),39))),_tag_dyneither(_tmp157E,sizeof(void*),2)))))))))))));}return;}
_LL79D:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpF40=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpF42;_LL7A3: {struct
Cyc_Absyn_Upper_b_struct*_tmpF41=(struct Cyc_Absyn_Upper_b_struct*)_tmpF40;if(
_tmpF41->tag != 1)goto _LL7A5;else{_tmpF42=_tmpF41->f1;}}_LL7A4: {unsigned int
_tmpF44;int _tmpF45;struct _tuple13 _tmpF43=Cyc_Evexp_eval_const_uint_exp(_tmpF42);
_tmpF44=_tmpF43.f1;_tmpF45=_tmpF43.f2;return _tmpF45  && _tmpF44 == 1;}_LL7A5:;
_LL7A6: return 0;_LL7A2:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpF46=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpF4E;
void*_tmpF4F;union Cyc_Absyn_Constraint*_tmpF50;struct Cyc_List_List*_tmpF52;
struct Cyc_Absyn_AggrInfo _tmpF54;union Cyc_Absyn_AggrInfoU _tmpF55;struct _tuple4
_tmpF56;struct Cyc_Absyn_AggrInfo _tmpF58;union Cyc_Absyn_AggrInfoU _tmpF59;struct
Cyc_Absyn_Aggrdecl**_tmpF5A;struct Cyc_Absyn_Aggrdecl*_tmpF5B;struct Cyc_List_List*
_tmpF5C;struct Cyc_List_List*_tmpF5E;_LL7A8: {struct Cyc_Absyn_VoidType_struct*
_tmpF47=(struct Cyc_Absyn_VoidType_struct*)_tmpF46;if(_tmpF47->tag != 0)goto _LL7AA;}
_LL7A9: goto _LL7AB;_LL7AA: {struct Cyc_Absyn_IntType_struct*_tmpF48=(struct Cyc_Absyn_IntType_struct*)
_tmpF46;if(_tmpF48->tag != 6)goto _LL7AC;}_LL7AB: goto _LL7AD;_LL7AC: {struct Cyc_Absyn_FloatType_struct*
_tmpF49=(struct Cyc_Absyn_FloatType_struct*)_tmpF46;if(_tmpF49->tag != 7)goto
_LL7AE;}_LL7AD: goto _LL7AF;_LL7AE: {struct Cyc_Absyn_DoubleType_struct*_tmpF4A=(
struct Cyc_Absyn_DoubleType_struct*)_tmpF46;if(_tmpF4A->tag != 8)goto _LL7B0;}
_LL7AF: return 1;_LL7B0: {struct Cyc_Absyn_EnumType_struct*_tmpF4B=(struct Cyc_Absyn_EnumType_struct*)
_tmpF46;if(_tmpF4B->tag != 14)goto _LL7B2;}_LL7B1: goto _LL7B3;_LL7B2: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpF4C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF46;if(_tmpF4C->tag != 15)goto
_LL7B4;}_LL7B3: return 0;_LL7B4: {struct Cyc_Absyn_ArrayType_struct*_tmpF4D=(struct
Cyc_Absyn_ArrayType_struct*)_tmpF46;if(_tmpF4D->tag != 9)goto _LL7B6;else{_tmpF4E=
_tmpF4D->f1;_tmpF4F=_tmpF4E.elt_type;_tmpF50=_tmpF4E.zero_term;}}_LL7B5: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF50) && Cyc_Tcutil_bits_only(
_tmpF4F);_LL7B6: {struct Cyc_Absyn_TupleType_struct*_tmpF51=(struct Cyc_Absyn_TupleType_struct*)
_tmpF46;if(_tmpF51->tag != 11)goto _LL7B8;else{_tmpF52=_tmpF51->f1;}}_LL7B7: for(0;
_tmpF52 != 0;_tmpF52=_tmpF52->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpF52->hd)).f2))return 0;}return 1;_LL7B8: {struct Cyc_Absyn_AggrType_struct*
_tmpF53=(struct Cyc_Absyn_AggrType_struct*)_tmpF46;if(_tmpF53->tag != 12)goto
_LL7BA;else{_tmpF54=_tmpF53->f1;_tmpF55=_tmpF54.aggr_info;if((_tmpF55.UnknownAggr).tag
!= 1)goto _LL7BA;_tmpF56=(struct _tuple4)(_tmpF55.UnknownAggr).val;}}_LL7B9: return
0;_LL7BA: {struct Cyc_Absyn_AggrType_struct*_tmpF57=(struct Cyc_Absyn_AggrType_struct*)
_tmpF46;if(_tmpF57->tag != 12)goto _LL7BC;else{_tmpF58=_tmpF57->f1;_tmpF59=_tmpF58.aggr_info;
if((_tmpF59.KnownAggr).tag != 2)goto _LL7BC;_tmpF5A=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF59.KnownAggr).val;_tmpF5B=*_tmpF5A;_tmpF5C=_tmpF58.targs;}}_LL7BB: if(
_tmpF5B->impl == 0)return 0;{struct _RegionHandle _tmpF5F=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpF5F;_push_region(rgn);{struct Cyc_List_List*_tmpF60=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF5B->tvs,_tmpF5C);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF5B->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpF60,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpF61=0;_npop_handler(0);return
_tmpF61;}}}{int _tmpF62=1;_npop_handler(0);return _tmpF62;};};_pop_region(rgn);};
_LL7BC: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF5D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF46;if(_tmpF5D->tag != 13)goto _LL7BE;else{_tmpF5E=_tmpF5D->f2;}}_LL7BD: for(0;
_tmpF5E != 0;_tmpF5E=_tmpF5E->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpF5E->hd)->type))return 0;}return 1;_LL7BE:;_LL7BF: return 0;_LL7A7:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);
static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpF63=e->r;struct _tuple2*_tmpF6B;void*_tmpF6C;struct Cyc_Absyn_Exp*
_tmpF6E;struct Cyc_Absyn_Exp*_tmpF6F;struct Cyc_Absyn_Exp*_tmpF70;struct Cyc_Absyn_Exp*
_tmpF72;struct Cyc_Absyn_Exp*_tmpF73;struct Cyc_Absyn_Exp*_tmpF75;struct Cyc_Absyn_Exp*
_tmpF77;void*_tmpF79;struct Cyc_Absyn_Exp*_tmpF7A;enum Cyc_Absyn_Coercion _tmpF7B;
void*_tmpF7D;struct Cyc_Absyn_Exp*_tmpF7E;struct Cyc_Absyn_Exp*_tmpF80;struct Cyc_Absyn_Exp*
_tmpF82;struct Cyc_Absyn_Exp*_tmpF83;struct Cyc_List_List*_tmpF85;struct Cyc_List_List*
_tmpF87;struct Cyc_List_List*_tmpF89;enum Cyc_Absyn_Primop _tmpF8B;struct Cyc_List_List*
_tmpF8C;struct Cyc_List_List*_tmpF8E;struct Cyc_List_List*_tmpF90;_LL7C1: {struct
Cyc_Absyn_Const_e_struct*_tmpF64=(struct Cyc_Absyn_Const_e_struct*)_tmpF63;if(
_tmpF64->tag != 0)goto _LL7C3;}_LL7C2: goto _LL7C4;_LL7C3: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpF65=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpF63;if(_tmpF65->tag != 18)goto
_LL7C5;}_LL7C4: goto _LL7C6;_LL7C5: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpF66=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpF63;if(_tmpF66->tag != 19)goto _LL7C7;}
_LL7C6: goto _LL7C8;_LL7C7: {struct Cyc_Absyn_Offsetof_e_struct*_tmpF67=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpF63;if(_tmpF67->tag != 20)goto _LL7C9;}_LL7C8: goto _LL7CA;_LL7C9: {struct Cyc_Absyn_Enum_e_struct*
_tmpF68=(struct Cyc_Absyn_Enum_e_struct*)_tmpF63;if(_tmpF68->tag != 32)goto _LL7CB;}
_LL7CA: goto _LL7CC;_LL7CB: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpF69=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpF63;if(_tmpF69->tag != 33)goto _LL7CD;}_LL7CC: return 1;_LL7CD: {struct Cyc_Absyn_Var_e_struct*
_tmpF6A=(struct Cyc_Absyn_Var_e_struct*)_tmpF63;if(_tmpF6A->tag != 1)goto _LL7CF;
else{_tmpF6B=_tmpF6A->f1;_tmpF6C=(void*)_tmpF6A->f2;}}_LL7CE: {void*_tmpF91=
_tmpF6C;struct Cyc_Absyn_Vardecl*_tmpF94;_LL7EE: {struct Cyc_Absyn_Funname_b_struct*
_tmpF92=(struct Cyc_Absyn_Funname_b_struct*)_tmpF91;if(_tmpF92->tag != 2)goto
_LL7F0;}_LL7EF: return 1;_LL7F0: {struct Cyc_Absyn_Global_b_struct*_tmpF93=(struct
Cyc_Absyn_Global_b_struct*)_tmpF91;if(_tmpF93->tag != 1)goto _LL7F2;else{_tmpF94=
_tmpF93->f1;}}_LL7F1: {void*_tmpF96=Cyc_Tcutil_compress(_tmpF94->type);_LL7F7: {
struct Cyc_Absyn_ArrayType_struct*_tmpF97=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF96;if(_tmpF97->tag != 9)goto _LL7F9;}_LL7F8: goto _LL7FA;_LL7F9: {struct Cyc_Absyn_FnType_struct*
_tmpF98=(struct Cyc_Absyn_FnType_struct*)_tmpF96;if(_tmpF98->tag != 10)goto _LL7FB;}
_LL7FA: return 1;_LL7FB:;_LL7FC: return var_okay;_LL7F6:;}_LL7F2: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpF95=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpF91;if(_tmpF95->tag != 0)goto
_LL7F4;}_LL7F3: return 0;_LL7F4:;_LL7F5: return var_okay;_LL7ED:;}_LL7CF: {struct Cyc_Absyn_Conditional_e_struct*
_tmpF6D=(struct Cyc_Absyn_Conditional_e_struct*)_tmpF63;if(_tmpF6D->tag != 6)goto
_LL7D1;else{_tmpF6E=_tmpF6D->f1;_tmpF6F=_tmpF6D->f2;_tmpF70=_tmpF6D->f3;}}_LL7D0:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpF6E) && Cyc_Tcutil_cnst_exp(te,0,_tmpF6F))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpF70);_LL7D1: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF71=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF63;if(_tmpF71->tag != 9)goto _LL7D3;
else{_tmpF72=_tmpF71->f1;_tmpF73=_tmpF71->f2;}}_LL7D2: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF72) && Cyc_Tcutil_cnst_exp(te,0,_tmpF73);_LL7D3: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpF74=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpF63;if(_tmpF74->tag != 13)
goto _LL7D5;else{_tmpF75=_tmpF74->f1;}}_LL7D4: _tmpF77=_tmpF75;goto _LL7D6;_LL7D5: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpF76=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpF63;if(_tmpF76->tag != 14)goto _LL7D7;else{_tmpF77=_tmpF76->f1;}}_LL7D6: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF77);_LL7D7: {struct Cyc_Absyn_Cast_e_struct*
_tmpF78=(struct Cyc_Absyn_Cast_e_struct*)_tmpF63;if(_tmpF78->tag != 15)goto _LL7D9;
else{_tmpF79=(void*)_tmpF78->f1;_tmpF7A=_tmpF78->f2;_tmpF7B=_tmpF78->f4;if(
_tmpF7B != Cyc_Absyn_No_coercion)goto _LL7D9;}}_LL7D8: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpF7A);_LL7D9: {struct Cyc_Absyn_Cast_e_struct*_tmpF7C=(struct Cyc_Absyn_Cast_e_struct*)
_tmpF63;if(_tmpF7C->tag != 15)goto _LL7DB;else{_tmpF7D=(void*)_tmpF7C->f1;_tmpF7E=
_tmpF7C->f2;}}_LL7DA: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF7E);_LL7DB: {
struct Cyc_Absyn_Address_e_struct*_tmpF7F=(struct Cyc_Absyn_Address_e_struct*)
_tmpF63;if(_tmpF7F->tag != 16)goto _LL7DD;else{_tmpF80=_tmpF7F->f1;}}_LL7DC: return
Cyc_Tcutil_cnst_exp(te,1,_tmpF80);_LL7DD: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpF81=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpF63;if(_tmpF81->tag != 28)
goto _LL7DF;else{_tmpF82=_tmpF81->f2;_tmpF83=_tmpF81->f3;}}_LL7DE: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF82) && Cyc_Tcutil_cnst_exp(te,0,_tmpF83);_LL7DF: {struct Cyc_Absyn_Array_e_struct*
_tmpF84=(struct Cyc_Absyn_Array_e_struct*)_tmpF63;if(_tmpF84->tag != 27)goto _LL7E1;
else{_tmpF85=_tmpF84->f1;}}_LL7E0: _tmpF87=_tmpF85;goto _LL7E2;_LL7E1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpF86=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpF63;if(_tmpF86->tag != 30)goto
_LL7E3;else{_tmpF87=_tmpF86->f2;}}_LL7E2: _tmpF89=_tmpF87;goto _LL7E4;_LL7E3: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpF88=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpF63;if(_tmpF88->tag != 29)goto _LL7E5;else{_tmpF89=_tmpF88->f3;}}_LL7E4: for(0;
_tmpF89 != 0;_tmpF89=_tmpF89->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)
_tmpF89->hd)).f2))return 0;}return 1;_LL7E5: {struct Cyc_Absyn_Primop_e_struct*
_tmpF8A=(struct Cyc_Absyn_Primop_e_struct*)_tmpF63;if(_tmpF8A->tag != 3)goto _LL7E7;
else{_tmpF8B=_tmpF8A->f1;_tmpF8C=_tmpF8A->f2;}}_LL7E6: _tmpF8E=_tmpF8C;goto _LL7E8;
_LL7E7: {struct Cyc_Absyn_Tuple_e_struct*_tmpF8D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpF63;if(_tmpF8D->tag != 25)goto _LL7E9;else{_tmpF8E=_tmpF8D->f1;}}_LL7E8:
_tmpF90=_tmpF8E;goto _LL7EA;_LL7E9: {struct Cyc_Absyn_Datatype_e_struct*_tmpF8F=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpF63;if(_tmpF8F->tag != 31)goto _LL7EB;else{
_tmpF90=_tmpF8F->f1;}}_LL7EA: for(0;_tmpF90 != 0;_tmpF90=_tmpF90->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpF90->hd))return 0;}return 1;_LL7EB:;_LL7EC: return 0;
_LL7C0:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpF99=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpF9F;struct Cyc_Absyn_PtrAtts _tmpFA0;union Cyc_Absyn_Constraint*
_tmpFA1;union Cyc_Absyn_Constraint*_tmpFA2;struct Cyc_Absyn_ArrayInfo _tmpFA4;void*
_tmpFA5;struct Cyc_List_List*_tmpFA7;struct Cyc_Absyn_AggrInfo _tmpFA9;union Cyc_Absyn_AggrInfoU
_tmpFAA;struct Cyc_List_List*_tmpFAB;struct Cyc_List_List*_tmpFAD;_LL7FE: {struct
Cyc_Absyn_VoidType_struct*_tmpF9A=(struct Cyc_Absyn_VoidType_struct*)_tmpF99;if(
_tmpF9A->tag != 0)goto _LL800;}_LL7FF: goto _LL801;_LL800: {struct Cyc_Absyn_IntType_struct*
_tmpF9B=(struct Cyc_Absyn_IntType_struct*)_tmpF99;if(_tmpF9B->tag != 6)goto _LL802;}
_LL801: goto _LL803;_LL802: {struct Cyc_Absyn_FloatType_struct*_tmpF9C=(struct Cyc_Absyn_FloatType_struct*)
_tmpF99;if(_tmpF9C->tag != 7)goto _LL804;}_LL803: goto _LL805;_LL804: {struct Cyc_Absyn_DoubleType_struct*
_tmpF9D=(struct Cyc_Absyn_DoubleType_struct*)_tmpF99;if(_tmpF9D->tag != 8)goto
_LL806;}_LL805: return 1;_LL806: {struct Cyc_Absyn_PointerType_struct*_tmpF9E=(
struct Cyc_Absyn_PointerType_struct*)_tmpF99;if(_tmpF9E->tag != 5)goto _LL808;else{
_tmpF9F=_tmpF9E->f1;_tmpFA0=_tmpF9F.ptr_atts;_tmpFA1=_tmpFA0.nullable;_tmpFA2=
_tmpFA0.bounds;}}_LL807: {void*_tmpFB0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFA2);_LL817: {
struct Cyc_Absyn_DynEither_b_struct*_tmpFB1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFB0;if(_tmpFB1->tag != 0)goto _LL819;}_LL818: return 1;_LL819:;_LL81A: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpFA1);_LL816:;}
_LL808: {struct Cyc_Absyn_ArrayType_struct*_tmpFA3=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF99;if(_tmpFA3->tag != 9)goto _LL80A;else{_tmpFA4=_tmpFA3->f1;_tmpFA5=_tmpFA4.elt_type;}}
_LL809: return Cyc_Tcutil_supports_default(_tmpFA5);_LL80A: {struct Cyc_Absyn_TupleType_struct*
_tmpFA6=(struct Cyc_Absyn_TupleType_struct*)_tmpF99;if(_tmpFA6->tag != 11)goto
_LL80C;else{_tmpFA7=_tmpFA6->f1;}}_LL80B: for(0;_tmpFA7 != 0;_tmpFA7=_tmpFA7->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpFA7->hd)).f2))return 0;}
return 1;_LL80C: {struct Cyc_Absyn_AggrType_struct*_tmpFA8=(struct Cyc_Absyn_AggrType_struct*)
_tmpF99;if(_tmpFA8->tag != 12)goto _LL80E;else{_tmpFA9=_tmpFA8->f1;_tmpFAA=_tmpFA9.aggr_info;
_tmpFAB=_tmpFA9.targs;}}_LL80D: {struct Cyc_Absyn_Aggrdecl*_tmpFB2=Cyc_Absyn_get_known_aggrdecl(
_tmpFAA);if(_tmpFB2->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFB2->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpFB2->tvs,_tmpFAB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFB2->impl))->fields);}
_LL80E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFAC=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF99;if(_tmpFAC->tag != 13)goto _LL810;else{_tmpFAD=_tmpFAC->f2;}}_LL80F: return
Cyc_Tcutil_fields_support_default(0,0,_tmpFAD);_LL810: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFAE=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF99;if(_tmpFAE->tag != 15)goto
_LL812;}_LL811: goto _LL813;_LL812: {struct Cyc_Absyn_EnumType_struct*_tmpFAF=(
struct Cyc_Absyn_EnumType_struct*)_tmpF99;if(_tmpFAF->tag != 14)goto _LL814;}_LL813:
return 1;_LL814:;_LL815: return 0;_LL7FD:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpFB3=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFB3;_push_region(rgn);{struct Cyc_List_List*_tmpFB4=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpFB4,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpFB5=0;_npop_handler(0);return _tmpFB5;}}}{
int _tmpFB6=1;_npop_handler(0);return _tmpFB6;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpFB7=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpFBC;struct Cyc_Absyn_PtrAtts _tmpFBD;union Cyc_Absyn_Constraint*
_tmpFBE;union Cyc_Absyn_Constraint*_tmpFBF;_LL81C: {struct Cyc_Absyn_IntType_struct*
_tmpFB8=(struct Cyc_Absyn_IntType_struct*)_tmpFB7;if(_tmpFB8->tag != 6)goto _LL81E;}
_LL81D: goto _LL81F;_LL81E: {struct Cyc_Absyn_FloatType_struct*_tmpFB9=(struct Cyc_Absyn_FloatType_struct*)
_tmpFB7;if(_tmpFB9->tag != 7)goto _LL820;}_LL81F: goto _LL821;_LL820: {struct Cyc_Absyn_DoubleType_struct*
_tmpFBA=(struct Cyc_Absyn_DoubleType_struct*)_tmpFB7;if(_tmpFBA->tag != 8)goto
_LL822;}_LL821: return 1;_LL822: {struct Cyc_Absyn_PointerType_struct*_tmpFBB=(
struct Cyc_Absyn_PointerType_struct*)_tmpFB7;if(_tmpFBB->tag != 5)goto _LL824;else{
_tmpFBC=_tmpFBB->f1;_tmpFBD=_tmpFBC.ptr_atts;_tmpFBE=_tmpFBD.nullable;_tmpFBF=
_tmpFBD.bounds;}}_LL823: {void*_tmpFC0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFBF);_LL827: {
struct Cyc_Absyn_DynEither_b_struct*_tmpFC1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFC0;if(_tmpFC1->tag != 0)goto _LL829;}_LL828: return 0;_LL829:;_LL82A: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFBE);_LL826:;}
_LL824:;_LL825: return 0;_LL81B:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpFC2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFC4;void*
_tmpFC5;struct Cyc_Absyn_FnInfo _tmpFC7;struct Cyc_List_List*_tmpFC8;_LL82C: {
struct Cyc_Absyn_PointerType_struct*_tmpFC3=(struct Cyc_Absyn_PointerType_struct*)
_tmpFC2;if(_tmpFC3->tag != 5)goto _LL82E;else{_tmpFC4=_tmpFC3->f1;_tmpFC5=_tmpFC4.elt_typ;}}
_LL82D: return Cyc_Tcutil_is_noreturn(_tmpFC5);_LL82E: {struct Cyc_Absyn_FnType_struct*
_tmpFC6=(struct Cyc_Absyn_FnType_struct*)_tmpFC2;if(_tmpFC6->tag != 10)goto _LL830;
else{_tmpFC7=_tmpFC6->f1;_tmpFC8=_tmpFC7.attributes;}}_LL82F: for(0;_tmpFC8 != 0;
_tmpFC8=_tmpFC8->tl){void*_tmpFC9=(void*)_tmpFC8->hd;_LL833: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpFCA=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpFC9;if(_tmpFCA->tag != 4)goto
_LL835;}_LL834: return 1;_LL835:;_LL836: continue;_LL832:;}goto _LL82B;_LL830:;
_LL831: goto _LL82B;_LL82B:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpFCB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpFCD;struct Cyc_List_List*_tmpFCE;struct Cyc_List_List**_tmpFCF;_LL838: {struct
Cyc_Absyn_FnType_struct*_tmpFCC=(struct Cyc_Absyn_FnType_struct*)_tmpFCB;if(
_tmpFCC->tag != 10)goto _LL83A;else{_tmpFCD=_tmpFCC->f1;_tmpFCE=_tmpFCD.attributes;
_tmpFCF=(struct Cyc_List_List**)&(_tmpFCC->f1).attributes;}}_LL839: {struct Cyc_List_List*
_tmpFD0=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1580;*_tmpFCF=((_tmp1580=_cycalloc(sizeof(*_tmp1580)),((
_tmp1580->hd=(void*)atts->hd,((_tmp1580->tl=*_tmpFCF,_tmp1580))))));}else{struct
Cyc_List_List*_tmp1581;_tmpFD0=((_tmp1581=_cycalloc(sizeof(*_tmp1581)),((
_tmp1581->hd=(void*)atts->hd,((_tmp1581->tl=_tmpFD0,_tmp1581))))));}}return
_tmpFD0;}_LL83A:;_LL83B: {const char*_tmp1584;void*_tmp1583;(_tmp1583=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1584="transfer_fn_type_atts",
_tag_dyneither(_tmp1584,sizeof(char),22))),_tag_dyneither(_tmp1583,sizeof(void*),
0)));}_LL837:;}
