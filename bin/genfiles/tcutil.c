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
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct _dyneither_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
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
t);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);enum Cyc_Absyn_Kind
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind
Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ek;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple14{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*
kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,enum Cyc_Absyn_Kind k,int
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
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpE62;void*
_tmpE61[2];const char*_tmpE60;const char*_tmpE5F;struct Cyc_String_pa_struct _tmpE5E;
struct Cyc_String_pa_struct _tmpE5D;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpE5D.tag=0,((_tmpE5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpE5E.tag=0,((_tmpE5E.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpE5F="const ",_tag_dyneither(_tmpE5F,sizeof(char),7)):((_tmpE60="",
_tag_dyneither(_tmpE60,sizeof(char),1)))),((_tmpE61[0]=& _tmpE5E,((_tmpE61[1]=&
_tmpE5D,Cyc_aprintf(((_tmpE62="%s%s",_tag_dyneither(_tmpE62,sizeof(char),5))),
_tag_dyneither(_tmpE61,sizeof(void*),2))))))))))))));const char*_tmpE6B;void*
_tmpE6A[2];const char*_tmpE69;const char*_tmpE68;struct Cyc_String_pa_struct _tmpE67;
struct Cyc_String_pa_struct _tmpE66;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpE66.tag=0,((_tmpE66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpE67.tag=0,((_tmpE67.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpE68="const ",_tag_dyneither(_tmpE68,sizeof(char),7)):((_tmpE69="",
_tag_dyneither(_tmpE69,sizeof(char),1)))),((_tmpE6A[0]=& _tmpE67,((_tmpE6A[1]=&
_tmpE66,Cyc_aprintf(((_tmpE6B="%s%s",_tag_dyneither(_tmpE6B,sizeof(char),5))),
_tag_dyneither(_tmpE6A,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpE6F;
void*_tmpE6E[1];struct Cyc_String_pa_struct _tmpE6D;(_tmpE6D.tag=0,((_tmpE6D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpE6E[0]=& _tmpE6D,Cyc_fprintf(
Cyc_stderr,((_tmpE6F="  %s",_tag_dyneither(_tmpE6F,sizeof(char),5))),
_tag_dyneither(_tmpE6E,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpE72;void*_tmpE71;(_tmpE71=0,Cyc_fprintf(
Cyc_stderr,((_tmpE72="\n\t",_tag_dyneither(_tmpE72,sizeof(char),3))),
_tag_dyneither(_tmpE71,sizeof(void*),0)));}pos=8;}else{{const char*_tmpE75;void*
_tmpE74;(_tmpE74=0,Cyc_fprintf(Cyc_stderr,((_tmpE75=" ",_tag_dyneither(_tmpE75,
sizeof(char),2))),_tag_dyneither(_tmpE74,sizeof(void*),0)));}++ pos;}{const char*
_tmpE78;void*_tmpE77;(_tmpE77=0,Cyc_fprintf(Cyc_stderr,((_tmpE78="and ",
_tag_dyneither(_tmpE78,sizeof(char),5))),_tag_dyneither(_tmpE77,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpE7B;
void*_tmpE7A;(_tmpE7A=0,Cyc_fprintf(Cyc_stderr,((_tmpE7B="\n\t",_tag_dyneither(
_tmpE7B,sizeof(char),3))),_tag_dyneither(_tmpE7A,sizeof(void*),0)));}pos=8;}{
const char*_tmpE7F;void*_tmpE7E[1];struct Cyc_String_pa_struct _tmpE7D;(_tmpE7D.tag=
0,((_tmpE7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpE7E[0]=&
_tmpE7D,Cyc_fprintf(Cyc_stderr,((_tmpE7F="%s ",_tag_dyneither(_tmpE7F,sizeof(
char),4))),_tag_dyneither(_tmpE7E,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpE82;void*
_tmpE81;(_tmpE81=0,Cyc_fprintf(Cyc_stderr,((_tmpE82="\n\t",_tag_dyneither(
_tmpE82,sizeof(char),3))),_tag_dyneither(_tmpE81,sizeof(void*),0)));}pos=8;}{
const char*_tmpE85;void*_tmpE84;(_tmpE84=0,Cyc_fprintf(Cyc_stderr,((_tmpE85="are not compatible. ",
_tag_dyneither(_tmpE85,sizeof(char),21))),_tag_dyneither(_tmpE84,sizeof(void*),0)));}
pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const
char*_tmpE88;void*_tmpE87;(_tmpE87=0,Cyc_fprintf(Cyc_stderr,((_tmpE88="\n\t",
_tag_dyneither(_tmpE88,sizeof(char),3))),_tag_dyneither(_tmpE87,sizeof(void*),0)));}{
const char*_tmpE8C;void*_tmpE8B[1];struct Cyc_String_pa_struct _tmpE8A;(_tmpE8A.tag=
0,((_tmpE8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpE8B[0]=& _tmpE8A,Cyc_fprintf(Cyc_stderr,((_tmpE8C="%s",_tag_dyneither(_tmpE8C,
sizeof(char),3))),_tag_dyneither(_tmpE8B,sizeof(void*),1)))))));};}{const char*
_tmpE8F;void*_tmpE8E;(_tmpE8E=0,Cyc_fprintf(Cyc_stderr,((_tmpE8F="\n",
_tag_dyneither(_tmpE8F,sizeof(char),2))),_tag_dyneither(_tmpE8E,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpE93;void*_tmpE92[1];struct Cyc_String_pa_struct _tmpE91;(_tmpE91.tag=
0,((_tmpE91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpE92[0]=&
_tmpE91,Cyc_fprintf(Cyc_stderr,((_tmpE93="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpE93,sizeof(char),36))),_tag_dyneither(_tmpE92,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpE96;struct Cyc_Core_Impossible_struct*_tmpE95;(int)_throw((void*)((_tmpE95=
_cycalloc(sizeof(*_tmpE95)),((_tmpE95[0]=((_tmpE96.tag=Cyc_Core_Impossible,((
_tmpE96.f1=msg,_tmpE96)))),_tmpE95)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpE9B;void*_tmpE9A[2];struct Cyc_String_pa_struct _tmpE99;struct Cyc_String_pa_struct
_tmpE98;(_tmpE98.tag=0,((_tmpE98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpE99.tag=
0,((_tmpE99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpE9A[0]=& _tmpE99,((_tmpE9A[1]=& _tmpE98,Cyc_fprintf(
Cyc_stderr,((_tmpE9B="%s::%s ",_tag_dyneither(_tmpE9B,sizeof(char),8))),
_tag_dyneither(_tmpE9A,sizeof(void*),2)))))))))))));}{const char*_tmpE9E;void*
_tmpE9D;(_tmpE9D=0,Cyc_fprintf(Cyc_stderr,((_tmpE9E="\n",_tag_dyneither(_tmpE9E,
sizeof(char),2))),_tag_dyneither(_tmpE9D,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpE9F;Cyc_Tcutil_warning_segs=((_tmpE9F=_cycalloc(
sizeof(*_tmpE9F)),((_tmpE9F->hd=sg,((_tmpE9F->tl=Cyc_Tcutil_warning_segs,_tmpE9F))))));}{
struct _dyneither_ptr*_tmpEA2;struct Cyc_List_List*_tmpEA1;Cyc_Tcutil_warning_msgs=((
_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1->hd=((_tmpEA2=_cycalloc(sizeof(*
_tmpEA2)),((_tmpEA2[0]=msg,_tmpEA2)))),((_tmpEA1->tl=Cyc_Tcutil_warning_msgs,
_tmpEA1))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpEA5;void*_tmpEA4;(_tmpEA4=0,
Cyc_fprintf(Cyc_stderr,((_tmpEA5="***Warnings***\n",_tag_dyneither(_tmpEA5,
sizeof(char),16))),_tag_dyneither(_tmpEA4,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpEAA;
void*_tmpEA9[2];struct Cyc_String_pa_struct _tmpEA8;struct Cyc_String_pa_struct
_tmpEA7;(_tmpEA7.tag=0,((_tmpEA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpEA8.tag=0,((_tmpEA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmpEA9[0]=& _tmpEA8,((_tmpEA9[1]=& _tmpEA7,Cyc_fprintf(Cyc_stderr,((_tmpEAA="%s: %s\n",
_tag_dyneither(_tmpEAA,sizeof(char),8))),_tag_dyneither(_tmpEA9,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpEAD;void*_tmpEAC;(_tmpEAC=0,Cyc_fprintf(Cyc_stderr,((_tmpEAD="**************\n",
_tag_dyneither(_tmpEAD,sizeof(char),16))),_tag_dyneither(_tmpEAC,sizeof(void*),0)));}
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmpEAE;*_tmp44=((_tmpEAE=
_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=t2,_tmpEAE))));}return t2;}_LL7: {struct
Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(_tmp45->tag
!= 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)& _tmp45->f2;}}
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp47))->v){
struct Cyc_Core_Opt*_tmpEAF;*_tmp47=((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((
_tmpEAF->v=t2,_tmpEAF))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
void*kb){void*_tmp56=Cyc_Absyn_compress_kb(kb);enum Cyc_Absyn_Kind _tmp59;_LL1A: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1C;}_LL1B: {struct Cyc_Absyn_Unknown_kb_struct
_tmpEB2;struct Cyc_Absyn_Unknown_kb_struct*_tmpEB1;return(void*)((_tmpEB1=
_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=((_tmpEB2.tag=1,((_tmpEB2.f1=0,_tmpEB2)))),
_tmpEB1))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct Cyc_Absyn_Less_kb_struct*)
_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}_LL1D: {struct Cyc_Absyn_Less_kb_struct
_tmpEB5;struct Cyc_Absyn_Less_kb_struct*_tmpEB4;return(void*)((_tmpEB4=_cycalloc(
sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB5.tag=2,((_tmpEB5.f1=0,((_tmpEB5.f2=
_tmp59,_tmpEB5)))))),_tmpEB4))));}_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpEB6;return(_tmpEB6=_cycalloc(
sizeof(*_tmpEB6)),((_tmpEB6->name=tv->name,((_tmpEB6->identity=- 1,((_tmpEB6->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEB6)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmpEB7;return(_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((
_tmpEB7->f1=_tmp61,((_tmpEB7->f2=_tmp62,((_tmpEB7->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmpEB7)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmpEB8;return(_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8->f1=_tmp67,((
_tmpEB8->f2=Cyc_Tcutil_copy_type(_tmp68),_tmpEB8)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpEB9;return(_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9->name=f->name,((
_tmpEB9->tq=f->tq,((_tmpEB9->type=Cyc_Tcutil_copy_type(f->type),((_tmpEB9->width=
f->width,((_tmpEB9->attributes=f->attributes,_tmpEB9)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*_tmpEBA;
return(_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmpEBA->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmpEBA)))));};}static struct
Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmpEBB;return(_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((
_tmpEBB->name=f->name,((_tmpEBB->tag=f->tag,((_tmpEBB->loc=f->loc,_tmpEBB)))))));}
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
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmpEBE;struct Cyc_Absyn_VarType_struct*
_tmpEBD;return(void*)((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((
_tmpEBE.tag=2,((_tmpEBE.f1=Cyc_Tcutil_copy_tvar(_tmp75),_tmpEBE)))),_tmpEBD))));}
_LL27: {struct Cyc_Absyn_DatatypeType_struct*_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp71;if(_tmp76->tag != 3)goto _LL29;else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;
_tmp79=_tmp77.targs;}}_LL28: {struct Cyc_Absyn_DatatypeType_struct _tmpEC4;struct
Cyc_Absyn_DatatypeInfo _tmpEC3;struct Cyc_Absyn_DatatypeType_struct*_tmpEC2;return(
void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC4.tag=3,((
_tmpEC4.f1=((_tmpEC3.datatype_info=_tmp78,((_tmpEC3.targs=Cyc_Tcutil_copy_types(
_tmp79),_tmpEC3)))),_tmpEC4)))),_tmpEC2))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp71;if(_tmp7A->tag != 4)goto
_LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}
_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpECA;struct Cyc_Absyn_DatatypeFieldInfo
_tmpEC9;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpEC8;return(void*)((_tmpEC8=
_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8[0]=((_tmpECA.tag=4,((_tmpECA.f1=((_tmpEC9.field_info=
_tmp7C,((_tmpEC9.targs=Cyc_Tcutil_copy_types(_tmp7D),_tmpEC9)))),_tmpECA)))),
_tmpEC8))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*_tmp7E=(struct Cyc_Absyn_PointerType_struct*)
_tmp71;if(_tmp7E->tag != 5)goto _LL2D;else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;
_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;
_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;_tmp87=_tmp82.ptrloc;}}_LL2C: {void*
_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*_tmpD1=Cyc_Tcutil_copy_type(_tmp83);
union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual _tmpD3=_tmp81;union
Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*_tmpD5=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmpED4;struct Cyc_Absyn_PtrAtts _tmpED3;
struct Cyc_Absyn_PtrInfo _tmpED2;struct Cyc_Absyn_PointerType_struct*_tmpED1;return(
void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED4.tag=5,((
_tmpED4.f1=((_tmpED2.elt_typ=_tmpD0,((_tmpED2.elt_tq=_tmpD3,((_tmpED2.ptr_atts=((
_tmpED3.rgn=_tmpD1,((_tmpED3.nullable=_tmpD2,((_tmpED3.bounds=_tmpD4,((_tmpED3.zero_term=
_tmpD5,((_tmpED3.ptrloc=_tmp87,_tmpED3)))))))))),_tmpED2)))))),_tmpED4)))),
_tmpED1))));}_LL2D: {struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)
_tmp71;if(_tmp88->tag != 6)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*
_tmp89=(struct Cyc_Absyn_FloatType_struct*)_tmp71;if(_tmp89->tag != 7)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp71;if(_tmp8A->tag != 8)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)_tmp71;if(_tmp8B->tag != 9)goto _LL35;
else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;
_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmpEDA;struct Cyc_Absyn_ArrayInfo _tmpED9;struct Cyc_Absyn_ArrayType_struct*
_tmpED8;return(void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((
_tmpEDA.tag=9,((_tmpEDA.f1=((_tmpED9.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((
_tmpED9.tq=_tmp8E,((_tmpED9.num_elts=_tmp8F,((_tmpED9.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmpED9.zt_loc=
_tmp91,_tmpED9)))))))))),_tmpEDA)))),_tmpED8))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp92=(struct Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;
else{_tmp93=_tmp92->f1;_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;
_tmp97=_tmp93.args;_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=
_tmp93.rgn_po;_tmp9B=_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*
_tmpEDB;struct Cyc_Core_Opt*_tmpDE=_tmp95 == 0?0:((_tmpEDB=_cycalloc(sizeof(*
_tmpEDB)),((_tmpEDB->v=Cyc_Tcutil_copy_type((void*)_tmp95->v),_tmpEDB))));void*
_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp99 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp99;struct Cyc_Absyn_VarargInfo*
_tmpEDC;cyc_varargs2=((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC->name=cv->name,((
_tmpEDC->tq=cv->tq,((_tmpEDC->type=Cyc_Tcutil_copy_type(cv->type),((_tmpEDC->inject=
cv->inject,_tmpEDC))))))))));}{struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp9A);struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmpEE2;
struct Cyc_Absyn_FnInfo _tmpEE1;struct Cyc_Absyn_FnType_struct*_tmpEE0;return(void*)((
_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=((_tmpEE2.tag=10,((_tmpEE2.f1=((
_tmpEE1.tvars=_tmpDD,((_tmpEE1.effect=_tmpDE,((_tmpEE1.ret_typ=_tmpDF,((_tmpEE1.args=
_tmpE0,((_tmpEE1.c_varargs=_tmpE1,((_tmpEE1.cyc_varargs=cyc_varargs2,((_tmpEE1.rgn_po=
_tmpE3,((_tmpEE1.attributes=_tmpE4,_tmpEE1)))))))))))))))),_tmpEE2)))),_tmpEE0))));};}
_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(struct Cyc_Absyn_TupleType_struct*)
_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmpEE5;struct Cyc_Absyn_TupleType_struct*_tmpEE4;return(void*)((_tmpEE4=
_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=((_tmpEE5.tag=11,((_tmpEE5.f1=((struct
Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),_tmpEE5)))),_tmpEE4))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmp9E=(struct Cyc_Absyn_AggrType_struct*)_tmp71;
if(_tmp9E->tag != 12)goto _LL3B;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((
_tmpA0.UnknownAggr).tag != 1)goto _LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;
_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmpEEB;struct Cyc_Absyn_AggrInfo _tmpEEA;struct Cyc_Absyn_AggrType_struct*
_tmpEE9;return(void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((
_tmpEEB.tag=12,((_tmpEEB.f1=((_tmpEEA.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmpEEA.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmpEEA)))),_tmpEEB)))),
_tmpEE9))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(struct Cyc_Absyn_AggrType_struct*)
_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;
if((_tmpA8.KnownAggr).tag != 2)goto _LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmpEF1;struct Cyc_Absyn_AggrInfo _tmpEF0;struct Cyc_Absyn_AggrType_struct*_tmpEEF;
return(void*)((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=((_tmpEF1.tag=12,((
_tmpEF1.f1=((_tmpEF0.aggr_info=Cyc_Absyn_KnownAggr(_tmpA9),((_tmpEF0.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmpEF0)))),_tmpEF1)))),_tmpEEF))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmpEF4;struct Cyc_Absyn_AnonAggrType_struct*_tmpEF3;return(void*)((_tmpEF3=
_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3[0]=((_tmpEF4.tag=13,((_tmpEF4.f1=_tmpAC,((
_tmpEF4.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpAD),_tmpEF4)))))),
_tmpEF3))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmpAE=(struct Cyc_Absyn_EnumType_struct*)
_tmp71;if(_tmpAE->tag != 14)goto _LL41;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}
_LL40: {struct Cyc_Absyn_EnumType_struct _tmpEF7;struct Cyc_Absyn_EnumType_struct*
_tmpEF6;return(void*)((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=((
_tmpEF7.tag=14,((_tmpEF7.f1=_tmpAF,((_tmpEF7.f2=_tmpB0,_tmpEF7)))))),_tmpEF6))));}
_LL41: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp71;if(_tmpB1->tag != 15)goto _LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmpEFA;struct Cyc_Absyn_AnonEnumType_struct*_tmpEF9;return(void*)((_tmpEF9=
_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEFA.tag=15,((_tmpEFA.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmpEFA)))),
_tmpEF9))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmpEFD;struct Cyc_Absyn_TagType_struct*_tmpEFC;
return(void*)((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFD.tag=20,((
_tmpEFD.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmpEFD)))),_tmpEFC))));}_LL45: {
struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmpF00;struct Cyc_Absyn_ValueofType_struct*_tmpEFF;return(void*)((_tmpEFF=
_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpF00.tag=19,((_tmpF00.f1=_tmpB6,
_tmpF00)))),_tmpEFF))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto _LL49;else{
_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct _tmpF03;
struct Cyc_Absyn_RgnHandleType_struct*_tmpF02;return(void*)((_tmpF02=_cycalloc(
sizeof(*_tmpF02)),((_tmpF02[0]=((_tmpF03.tag=16,((_tmpF03.f1=(void*)Cyc_Tcutil_copy_type(
_tmpB8),_tmpF03)))),_tmpF02))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmpF06;struct Cyc_Absyn_DynRgnType_struct*_tmpF05;return(void*)((_tmpF05=
_cycalloc(sizeof(*_tmpF05)),((_tmpF05[0]=((_tmpF06.tag=17,((_tmpF06.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmpF06.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmpF06)))))),_tmpF05))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmpF09;struct Cyc_Absyn_TypedefType_struct*_tmpF08;return(void*)((_tmpF08=
_cycalloc(sizeof(*_tmpF08)),((_tmpF08[0]=((_tmpF09.tag=18,((_tmpF09.f1=_tmpBD,((
_tmpF09.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmpF09.f3=_tmpBF,((_tmpF09.f4=0,
_tmpF09)))))))))),_tmpF08))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*_tmpC0=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}_LL4E:
goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmpF0C;
struct Cyc_Absyn_AccessEff_struct*_tmpF0B;return(void*)((_tmpF0B=_cycalloc(
sizeof(*_tmpF0B)),((_tmpF0B[0]=((_tmpF0C.tag=23,((_tmpF0C.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmpF0C)))),_tmpF0B))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmpF0F;struct Cyc_Absyn_JoinEff_struct*
_tmpF0E;return(void*)((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E[0]=((
_tmpF0F.tag=24,((_tmpF0F.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmpF0F)))),_tmpF0E))));}
_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmpF12;struct Cyc_Absyn_RgnsEff_struct*_tmpF11;
return(void*)((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11[0]=((_tmpF12.tag=25,((
_tmpF12.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmpF12)))),_tmpF11))));}_LL20:;}
struct _tuple17{enum Cyc_Absyn_Kind f1;enum Cyc_Absyn_Kind f2;};int Cyc_Tcutil_kind_leq(
enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind
k1,enum Cyc_Absyn_Kind k2){if(k1 == k2)return 1;{struct _tuple17 _tmpF13;struct
_tuple17 _tmp108=(_tmpF13.f1=k1,((_tmpF13.f2=k2,_tmpF13)));enum Cyc_Absyn_Kind
_tmp109;enum Cyc_Absyn_Kind _tmp10A;enum Cyc_Absyn_Kind _tmp10B;enum Cyc_Absyn_Kind
_tmp10C;enum Cyc_Absyn_Kind _tmp10D;enum Cyc_Absyn_Kind _tmp10E;enum Cyc_Absyn_Kind
_tmp10F;enum Cyc_Absyn_Kind _tmp110;enum Cyc_Absyn_Kind _tmp111;enum Cyc_Absyn_Kind
_tmp112;_LL58: _tmp109=_tmp108.f1;if(_tmp109 != Cyc_Absyn_BoxKind)goto _LL5A;
_tmp10A=_tmp108.f2;if(_tmp10A != Cyc_Absyn_MemKind)goto _LL5A;_LL59: goto _LL5B;
_LL5A: _tmp10B=_tmp108.f1;if(_tmp10B != Cyc_Absyn_BoxKind)goto _LL5C;_tmp10C=
_tmp108.f2;if(_tmp10C != Cyc_Absyn_AnyKind)goto _LL5C;_LL5B: goto _LL5D;_LL5C:
_tmp10D=_tmp108.f1;if(_tmp10D != Cyc_Absyn_MemKind)goto _LL5E;_tmp10E=_tmp108.f2;
if(_tmp10E != Cyc_Absyn_AnyKind)goto _LL5E;_LL5D: goto _LL5F;_LL5E: _tmp10F=_tmp108.f1;
if(_tmp10F != Cyc_Absyn_RgnKind)goto _LL60;_tmp110=_tmp108.f2;if(_tmp110 != Cyc_Absyn_TopRgnKind)
goto _LL60;_LL5F: goto _LL61;_LL60: _tmp111=_tmp108.f1;if(_tmp111 != Cyc_Absyn_UniqueRgnKind)
goto _LL62;_tmp112=_tmp108.f2;if(_tmp112 != Cyc_Absyn_TopRgnKind)goto _LL62;_LL61:
return 1;_LL62:;_LL63: return 0;_LL57:;};}static int Cyc_Tcutil_is_region_kind(enum 
Cyc_Absyn_Kind k);static int Cyc_Tcutil_is_region_kind(enum Cyc_Absyn_Kind k){
switch(k){case Cyc_Absyn_RgnKind: _LL64: goto _LL65;case Cyc_Absyn_TopRgnKind: _LL65:
goto _LL66;case Cyc_Absyn_UniqueRgnKind: _LL66: return 1;default: _LL67: return 0;}}
enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def){void*_tmp113=Cyc_Absyn_compress_kb(tv->kind);enum Cyc_Absyn_Kind _tmp115;
enum Cyc_Absyn_Kind _tmp117;_LL6A: {struct Cyc_Absyn_Eq_kb_struct*_tmp114=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp113;if(_tmp114->tag != 0)goto _LL6C;else{_tmp115=
_tmp114->f1;}}_LL6B: return _tmp115;_LL6C: {struct Cyc_Absyn_Less_kb_struct*_tmp116=(
struct Cyc_Absyn_Less_kb_struct*)_tmp113;if(_tmp116->tag != 2)goto _LL6E;else{
_tmp117=_tmp116->f2;}}_LL6D: return _tmp117;_LL6E:;_LL6F:{struct Cyc_Absyn_Less_kb_struct
_tmpF16;struct Cyc_Absyn_Less_kb_struct*_tmpF15;tv->kind=(void*)((_tmpF15=
_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=2,((_tmpF16.f1=0,((
_tmpF16.f2=def,_tmpF16)))))),_tmpF15))));}return def;_LL69:;}int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct
_tuple0 _tmpF17;struct _tuple0 _tmp11B=(_tmpF17.f1=Cyc_Absyn_compress_kb(kb1),((
_tmpF17.f2=Cyc_Absyn_compress_kb(kb2),_tmpF17)));void*_tmp11C;enum Cyc_Absyn_Kind
_tmp11E;void*_tmp11F;enum Cyc_Absyn_Kind _tmp121;void*_tmp122;struct Cyc_Core_Opt*
_tmp124;struct Cyc_Core_Opt**_tmp125;enum Cyc_Absyn_Kind _tmp126;void*_tmp127;
enum Cyc_Absyn_Kind _tmp129;void*_tmp12A;enum Cyc_Absyn_Kind _tmp12C;void*_tmp12D;
struct Cyc_Core_Opt*_tmp12F;struct Cyc_Core_Opt**_tmp130;enum Cyc_Absyn_Kind
_tmp131;void*_tmp132;struct Cyc_Core_Opt*_tmp134;struct Cyc_Core_Opt**_tmp135;
enum Cyc_Absyn_Kind _tmp136;void*_tmp137;struct Cyc_Core_Opt*_tmp139;struct Cyc_Core_Opt**
_tmp13A;enum Cyc_Absyn_Kind _tmp13B;void*_tmp13C;struct Cyc_Core_Opt*_tmp13E;
struct Cyc_Core_Opt**_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;struct Cyc_Core_Opt*
_tmp144;struct Cyc_Core_Opt**_tmp145;_LL71: _tmp11C=_tmp11B.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp11D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL73;
else{_tmp11E=_tmp11D->f1;}};_tmp11F=_tmp11B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp120=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LL73;
else{_tmp121=_tmp120->f1;}};_LL72: return _tmp11E == _tmp121;_LL73: _tmp122=_tmp11B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp123=(struct Cyc_Absyn_Less_kb_struct*)_tmp122;
if(_tmp123->tag != 2)goto _LL75;else{_tmp124=_tmp123->f1;_tmp125=(struct Cyc_Core_Opt**)&
_tmp123->f1;_tmp126=_tmp123->f2;}};_tmp127=_tmp11B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp128=(struct Cyc_Absyn_Eq_kb_struct*)_tmp127;if(_tmp128->tag != 0)goto _LL75;
else{_tmp129=_tmp128->f1;}};_LL74: if(Cyc_Tcutil_kind_leq(_tmp129,_tmp126)){{void**
_tmpF18;*_tmp125=(struct Cyc_Core_Opt*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((
_tmpF18[0]=kb2,_tmpF18))));}return 1;}else{return 0;}_LL75: _tmp12A=_tmp11B.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp12B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp12A;if(
_tmp12B->tag != 0)goto _LL77;else{_tmp12C=_tmp12B->f1;}};_tmp12D=_tmp11B.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp12E=(struct Cyc_Absyn_Less_kb_struct*)_tmp12D;
if(_tmp12E->tag != 2)goto _LL77;else{_tmp12F=_tmp12E->f1;_tmp130=(struct Cyc_Core_Opt**)&
_tmp12E->f1;_tmp131=_tmp12E->f2;}};_LL76: if(Cyc_Tcutil_kind_leq(_tmp12C,_tmp131)){{
void**_tmpF19;*_tmp130=(struct Cyc_Core_Opt*)((_tmpF19=_cycalloc(sizeof(*_tmpF19)),((
_tmpF19[0]=kb1,_tmpF19))));}return 1;}else{return 0;}_LL77: _tmp132=_tmp11B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp133=(struct Cyc_Absyn_Less_kb_struct*)_tmp132;
if(_tmp133->tag != 2)goto _LL79;else{_tmp134=_tmp133->f1;_tmp135=(struct Cyc_Core_Opt**)&
_tmp133->f1;_tmp136=_tmp133->f2;}};_tmp137=_tmp11B.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp138=(struct Cyc_Absyn_Less_kb_struct*)_tmp137;if(_tmp138->tag != 2)goto _LL79;
else{_tmp139=_tmp138->f1;_tmp13A=(struct Cyc_Core_Opt**)& _tmp138->f1;_tmp13B=
_tmp138->f2;}};_LL78: if(Cyc_Tcutil_kind_leq(_tmp136,_tmp13B)){{void**_tmpF1A;*
_tmp13A=(struct Cyc_Core_Opt*)((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A[0]=
kb1,_tmpF1A))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp13B,_tmp136)){{void**
_tmpF1B;*_tmp135=(struct Cyc_Core_Opt*)((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((
_tmpF1B[0]=kb2,_tmpF1B))));}return 1;}else{return 0;}}_LL79: _tmp13C=_tmp11B.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp13D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp13C;if(_tmp13D->tag != 1)goto _LL7B;else{_tmp13E=_tmp13D->f1;_tmp13F=(struct
Cyc_Core_Opt**)& _tmp13D->f1;}};_tmp140=_tmp11B.f2;_LL7A: _tmp141=_tmp140;_tmp145=
_tmp13F;goto _LL7C;_LL7B: _tmp141=_tmp11B.f1;_tmp142=_tmp11B.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp143=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp142;if(_tmp143->tag != 1)goto
_LL70;else{_tmp144=_tmp143->f1;_tmp145=(struct Cyc_Core_Opt**)& _tmp143->f1;}};
_LL7C:{void**_tmpF1C;*_tmp145=(struct Cyc_Core_Opt*)((_tmpF1C=_cycalloc(sizeof(*
_tmpF1C)),((_tmpF1C[0]=_tmp141,_tmpF1C))));}return 1;_LL70:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp14B=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp14D;_LL7E: {struct Cyc_Absyn_VarType_struct*
_tmp14C=(struct Cyc_Absyn_VarType_struct*)_tmp14B;if(_tmp14C->tag != 2)goto _LL80;
else{_tmp14D=_tmp14C->f1;}}_LL7F: {void*_tmp14E=_tmp14D->kind;_tmp14D->kind=kb;{
struct _tuple14 _tmpF1D;return(_tmpF1D.f1=_tmp14D,((_tmpF1D.f2=_tmp14E,_tmpF1D)));};}
_LL80:;_LL81: {const char*_tmpF21;void*_tmpF20[1];struct Cyc_String_pa_struct
_tmpF1F;(_tmpF1F.tag=0,((_tmpF1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF20[0]=& _tmpF1F,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF21="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpF21,sizeof(char),40))),_tag_dyneither(_tmpF20,sizeof(void*),1)))))));}
_LL7D:;}enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(void*t);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t){void*_tmp153=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp155;struct Cyc_Absyn_Tvar*
_tmp157;enum Cyc_Absyn_Size_of _tmp15A;struct Cyc_Absyn_DatatypeFieldInfo _tmp166;
union Cyc_Absyn_DatatypeFieldInfoU _tmp167;struct _tuple3 _tmp168;struct Cyc_Absyn_Datatypedecl*
_tmp169;struct Cyc_Absyn_Datatypefield*_tmp16A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp16C;union Cyc_Absyn_DatatypeFieldInfoU _tmp16D;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp16E;struct Cyc_Absyn_AggrInfo _tmp170;union Cyc_Absyn_AggrInfoU _tmp171;struct
_tuple4 _tmp172;struct Cyc_Absyn_AggrInfo _tmp174;union Cyc_Absyn_AggrInfoU _tmp175;
struct Cyc_Absyn_Aggrdecl**_tmp176;struct Cyc_Absyn_Aggrdecl*_tmp177;struct Cyc_Absyn_Aggrdecl
_tmp178;struct Cyc_Absyn_AggrdeclImpl*_tmp179;struct Cyc_Absyn_PtrInfo _tmp17C;
struct Cyc_Absyn_Typedefdecl*_tmp182;_LL83: {struct Cyc_Absyn_Evar_struct*_tmp154=(
struct Cyc_Absyn_Evar_struct*)_tmp153;if(_tmp154->tag != 1)goto _LL85;else{_tmp155=
_tmp154->f1;}}_LL84: return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(
_tmp155))->v;_LL85: {struct Cyc_Absyn_VarType_struct*_tmp156=(struct Cyc_Absyn_VarType_struct*)
_tmp153;if(_tmp156->tag != 2)goto _LL87;else{_tmp157=_tmp156->f1;}}_LL86: return Cyc_Tcutil_tvar_kind(
_tmp157,Cyc_Absyn_BoxKind);_LL87: {struct Cyc_Absyn_VoidType_struct*_tmp158=(
struct Cyc_Absyn_VoidType_struct*)_tmp153;if(_tmp158->tag != 0)goto _LL89;}_LL88:
return Cyc_Absyn_MemKind;_LL89: {struct Cyc_Absyn_IntType_struct*_tmp159=(struct
Cyc_Absyn_IntType_struct*)_tmp153;if(_tmp159->tag != 6)goto _LL8B;else{_tmp15A=
_tmp159->f2;}}_LL8A: return(_tmp15A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz
 || _tmp15A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?Cyc_Absyn_BoxKind: Cyc_Absyn_MemKind;
_LL8B: {struct Cyc_Absyn_FloatType_struct*_tmp15B=(struct Cyc_Absyn_FloatType_struct*)
_tmp153;if(_tmp15B->tag != 7)goto _LL8D;}_LL8C: goto _LL8E;_LL8D: {struct Cyc_Absyn_DoubleType_struct*
_tmp15C=(struct Cyc_Absyn_DoubleType_struct*)_tmp153;if(_tmp15C->tag != 8)goto
_LL8F;}_LL8E: goto _LL90;_LL8F: {struct Cyc_Absyn_FnType_struct*_tmp15D=(struct Cyc_Absyn_FnType_struct*)
_tmp153;if(_tmp15D->tag != 10)goto _LL91;}_LL90: return Cyc_Absyn_MemKind;_LL91: {
struct Cyc_Absyn_DynRgnType_struct*_tmp15E=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp153;if(_tmp15E->tag != 17)goto _LL93;}_LL92: goto _LL94;_LL93: {struct Cyc_Absyn_EnumType_struct*
_tmp15F=(struct Cyc_Absyn_EnumType_struct*)_tmp153;if(_tmp15F->tag != 14)goto _LL95;}
_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_AnonEnumType_struct*_tmp160=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp153;if(_tmp160->tag != 15)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp161=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp153;if(_tmp161->tag != 16)goto
_LL99;}_LL98: return Cyc_Absyn_BoxKind;_LL99: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp162=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp153;if(_tmp162->tag != 22)goto
_LL9B;}_LL9A: return Cyc_Absyn_UniqueRgnKind;_LL9B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp163=(struct Cyc_Absyn_HeapRgn_struct*)_tmp153;if(_tmp163->tag != 21)goto _LL9D;}
_LL9C: return Cyc_Absyn_RgnKind;_LL9D: {struct Cyc_Absyn_DatatypeType_struct*
_tmp164=(struct Cyc_Absyn_DatatypeType_struct*)_tmp153;if(_tmp164->tag != 3)goto
_LL9F;}_LL9E: return Cyc_Absyn_AnyKind;_LL9F: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp165=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp153;if(_tmp165->tag != 4)
goto _LLA1;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.field_info;if((_tmp167.KnownDatatypefield).tag
!= 2)goto _LLA1;_tmp168=(struct _tuple3)(_tmp167.KnownDatatypefield).val;_tmp169=
_tmp168.f1;_tmp16A=_tmp168.f2;}}_LLA0: return Cyc_Absyn_MemKind;_LLA1: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp153;if(_tmp16B->tag != 4)
goto _LLA3;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16C.field_info;if((_tmp16D.UnknownDatatypefield).tag
!= 1)goto _LLA3;_tmp16E=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp16D.UnknownDatatypefield).val;}}
_LLA2: {const char*_tmpF24;void*_tmpF23;(_tmpF23=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF24="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpF24,sizeof(char),39))),_tag_dyneither(_tmpF23,sizeof(void*),0)));}
_LLA3: {struct Cyc_Absyn_AggrType_struct*_tmp16F=(struct Cyc_Absyn_AggrType_struct*)
_tmp153;if(_tmp16F->tag != 12)goto _LLA5;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.aggr_info;
if((_tmp171.UnknownAggr).tag != 1)goto _LLA5;_tmp172=(struct _tuple4)(_tmp171.UnknownAggr).val;}}
_LLA4: return Cyc_Absyn_AnyKind;_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp173=(
struct Cyc_Absyn_AggrType_struct*)_tmp153;if(_tmp173->tag != 12)goto _LLA7;else{
_tmp174=_tmp173->f1;_tmp175=_tmp174.aggr_info;if((_tmp175.KnownAggr).tag != 2)
goto _LLA7;_tmp176=(struct Cyc_Absyn_Aggrdecl**)(_tmp175.KnownAggr).val;_tmp177=*
_tmp176;_tmp178=*_tmp177;_tmp179=_tmp178.impl;}}_LLA6: return _tmp179 == 0?Cyc_Absyn_AnyKind:
Cyc_Absyn_MemKind;_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp17A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp153;if(_tmp17A->tag != 13)goto _LLA9;}_LLA8: return Cyc_Absyn_MemKind;_LLA9: {
struct Cyc_Absyn_PointerType_struct*_tmp17B=(struct Cyc_Absyn_PointerType_struct*)
_tmp153;if(_tmp17B->tag != 5)goto _LLAB;else{_tmp17C=_tmp17B->f1;}}_LLAA: {void*
_tmp188=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp17C.ptr_atts).bounds);_LLBC: {struct Cyc_Absyn_DynEither_b_struct*
_tmp189=(struct Cyc_Absyn_DynEither_b_struct*)_tmp188;if(_tmp189->tag != 0)goto
_LLBE;}_LLBD: return Cyc_Absyn_MemKind;_LLBE: {struct Cyc_Absyn_Upper_b_struct*
_tmp18A=(struct Cyc_Absyn_Upper_b_struct*)_tmp188;if(_tmp18A->tag != 1)goto _LLBB;}
_LLBF: return Cyc_Absyn_BoxKind;_LLBB:;}_LLAB: {struct Cyc_Absyn_ValueofType_struct*
_tmp17D=(struct Cyc_Absyn_ValueofType_struct*)_tmp153;if(_tmp17D->tag != 19)goto
_LLAD;}_LLAC: return Cyc_Absyn_IntKind;_LLAD: {struct Cyc_Absyn_TagType_struct*
_tmp17E=(struct Cyc_Absyn_TagType_struct*)_tmp153;if(_tmp17E->tag != 20)goto _LLAF;}
_LLAE: return Cyc_Absyn_BoxKind;_LLAF: {struct Cyc_Absyn_ArrayType_struct*_tmp17F=(
struct Cyc_Absyn_ArrayType_struct*)_tmp153;if(_tmp17F->tag != 9)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp153;if(_tmp180->tag != 11)goto _LLB3;}_LLB2: return Cyc_Absyn_MemKind;_LLB3: {
struct Cyc_Absyn_TypedefType_struct*_tmp181=(struct Cyc_Absyn_TypedefType_struct*)
_tmp153;if(_tmp181->tag != 18)goto _LLB5;else{_tmp182=_tmp181->f3;}}_LLB4: if(
_tmp182 == 0  || _tmp182->kind == 0){const char*_tmpF28;void*_tmpF27[1];struct Cyc_String_pa_struct
_tmpF26;(_tmpF26.tag=0,((_tmpF26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF27[0]=& _tmpF26,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF28="typ_kind: typedef found: %s",
_tag_dyneither(_tmpF28,sizeof(char),28))),_tag_dyneither(_tmpF27,sizeof(void*),1)))))));}
return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp182->kind))->v;
_LLB5: {struct Cyc_Absyn_AccessEff_struct*_tmp183=(struct Cyc_Absyn_AccessEff_struct*)
_tmp153;if(_tmp183->tag != 23)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_JoinEff_struct*
_tmp184=(struct Cyc_Absyn_JoinEff_struct*)_tmp153;if(_tmp184->tag != 24)goto _LLB9;}
_LLB8: goto _LLBA;_LLB9: {struct Cyc_Absyn_RgnsEff_struct*_tmp185=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp153;if(_tmp185->tag != 25)goto _LL82;}_LLBA: return Cyc_Absyn_EffKind;_LL82:;}
int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(void*t1,void*t2){struct
_handler_cons _tmp18E;_push_handler(& _tmp18E);{int _tmp190=0;if(setjmp(_tmp18E.handler))
_tmp190=1;if(!_tmp190){Cyc_Tcutil_unify_it(t1,t2);{int _tmp191=1;_npop_handler(0);
return _tmp191;};;_pop_handler();}else{void*_tmp18F=(void*)_exn_thrown;void*
_tmp193=_tmp18F;_LLC1: {struct Cyc_Tcutil_Unify_struct*_tmp194=(struct Cyc_Tcutil_Unify_struct*)
_tmp193;if(_tmp194->tag != Cyc_Tcutil_Unify)goto _LLC3;}_LLC2: return 0;_LLC3:;_LLC4:(
void)_throw(_tmp193);_LLC0:;}};}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp195=t;struct Cyc_Absyn_Tvar*_tmp197;struct Cyc_Core_Opt*_tmp199;
struct Cyc_Core_Opt*_tmp19A;struct Cyc_Core_Opt**_tmp19B;struct Cyc_Absyn_PtrInfo
_tmp19D;struct Cyc_Absyn_ArrayInfo _tmp19F;void*_tmp1A0;struct Cyc_Absyn_FnInfo
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Core_Opt*_tmp1A4;void*_tmp1A5;
struct Cyc_List_List*_tmp1A6;int _tmp1A7;struct Cyc_Absyn_VarargInfo*_tmp1A8;struct
Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1AC;
struct Cyc_Absyn_DatatypeInfo _tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*
_tmp1B1;struct Cyc_Absyn_DatatypeFieldInfo _tmp1B3;struct Cyc_List_List*_tmp1B4;
struct Cyc_Absyn_AggrInfo _tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_List_List*
_tmp1B9;void*_tmp1BB;void*_tmp1BD;void*_tmp1BF;void*_tmp1C1;struct Cyc_List_List*
_tmp1C3;_LLC6: {struct Cyc_Absyn_VarType_struct*_tmp196=(struct Cyc_Absyn_VarType_struct*)
_tmp195;if(_tmp196->tag != 2)goto _LLC8;else{_tmp197=_tmp196->f1;}}_LLC7: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp197)){{
const char*_tmpF29;Cyc_Tcutil_failure_reason=((_tmpF29="(type variable would escape scope)",
_tag_dyneither(_tmpF29,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LLC5;_LLC8: {struct Cyc_Absyn_Evar_struct*_tmp198=(struct Cyc_Absyn_Evar_struct*)
_tmp195;if(_tmp198->tag != 1)goto _LLCA;else{_tmp199=_tmp198->f2;_tmp19A=_tmp198->f4;
_tmp19B=(struct Cyc_Core_Opt**)& _tmp198->f4;}}_LLC9: if(t == evar){{const char*
_tmpF2A;Cyc_Tcutil_failure_reason=((_tmpF2A="(occurs check)",_tag_dyneither(
_tmpF2A,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp199 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp199->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp19B))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp1C6=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp19B))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmpF2B;_tmp1C6=((_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((
_tmpF2B->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF2B->tl=_tmp1C6,_tmpF2B))))));}}}{
struct Cyc_Core_Opt*_tmpF2C;*_tmp19B=((_tmpF2C=_cycalloc(sizeof(*_tmpF2C)),((
_tmpF2C->v=_tmp1C6,_tmpF2C))));};}}}goto _LLC5;_LLCA: {struct Cyc_Absyn_PointerType_struct*
_tmp19C=(struct Cyc_Absyn_PointerType_struct*)_tmp195;if(_tmp19C->tag != 5)goto
_LLCC;else{_tmp19D=_tmp19C->f1;}}_LLCB: Cyc_Tcutil_occurs(evar,r,env,_tmp19D.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp19D.ptr_atts).rgn);goto _LLC5;_LLCC: {struct Cyc_Absyn_ArrayType_struct*
_tmp19E=(struct Cyc_Absyn_ArrayType_struct*)_tmp195;if(_tmp19E->tag != 9)goto _LLCE;
else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19F.elt_type;}}_LLCD: Cyc_Tcutil_occurs(evar,
r,env,_tmp1A0);goto _LLC5;_LLCE: {struct Cyc_Absyn_FnType_struct*_tmp1A1=(struct
Cyc_Absyn_FnType_struct*)_tmp195;if(_tmp1A1->tag != 10)goto _LLD0;else{_tmp1A2=
_tmp1A1->f1;_tmp1A3=_tmp1A2.tvars;_tmp1A4=_tmp1A2.effect;_tmp1A5=_tmp1A2.ret_typ;
_tmp1A6=_tmp1A2.args;_tmp1A7=_tmp1A2.c_varargs;_tmp1A8=_tmp1A2.cyc_varargs;
_tmp1A9=_tmp1A2.rgn_po;_tmp1AA=_tmp1A2.attributes;}}_LLCF: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp1A3,env);if(_tmp1A4 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1A4->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp1A5);for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp1A6->hd)).f3);}if(_tmp1A8 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp1A8->type);for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){struct _tuple0
_tmp1CA;void*_tmp1CB;void*_tmp1CC;struct _tuple0*_tmp1C9=(struct _tuple0*)_tmp1A9->hd;
_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp1CB);Cyc_Tcutil_occurs(evar,r,env,_tmp1CC);}goto _LLC5;_LLD0: {struct Cyc_Absyn_TupleType_struct*
_tmp1AB=(struct Cyc_Absyn_TupleType_struct*)_tmp195;if(_tmp1AB->tag != 11)goto
_LLD2;else{_tmp1AC=_tmp1AB->f1;}}_LLD1: for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1AC->hd)).f2);}goto _LLC5;
_LLD2: {struct Cyc_Absyn_DatatypeType_struct*_tmp1AD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp195;if(_tmp1AD->tag != 3)goto _LLD4;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AE.targs;}}
_LLD3: Cyc_Tcutil_occurslist(evar,r,env,_tmp1AF);goto _LLC5;_LLD4: {struct Cyc_Absyn_TypedefType_struct*
_tmp1B0=(struct Cyc_Absyn_TypedefType_struct*)_tmp195;if(_tmp1B0->tag != 18)goto
_LLD6;else{_tmp1B1=_tmp1B0->f2;}}_LLD5: _tmp1B4=_tmp1B1;goto _LLD7;_LLD6: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp1B2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp195;if(_tmp1B2->tag != 4)goto _LLD8;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B3.targs;}}
_LLD7: _tmp1B7=_tmp1B4;goto _LLD9;_LLD8: {struct Cyc_Absyn_AggrType_struct*_tmp1B5=(
struct Cyc_Absyn_AggrType_struct*)_tmp195;if(_tmp1B5->tag != 12)goto _LLDA;else{
_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B6.targs;}}_LLD9: Cyc_Tcutil_occurslist(evar,r,
env,_tmp1B7);goto _LLC5;_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1B8=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp195;if(_tmp1B8->tag != 13)goto _LLDC;
else{_tmp1B9=_tmp1B8->f2;}}_LLDB: for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){Cyc_Tcutil_occurs(
evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1B9->hd)->type);}goto _LLC5;_LLDC: {
struct Cyc_Absyn_TagType_struct*_tmp1BA=(struct Cyc_Absyn_TagType_struct*)_tmp195;
if(_tmp1BA->tag != 20)goto _LLDE;else{_tmp1BB=(void*)_tmp1BA->f1;}}_LLDD: _tmp1BD=
_tmp1BB;goto _LLDF;_LLDE: {struct Cyc_Absyn_AccessEff_struct*_tmp1BC=(struct Cyc_Absyn_AccessEff_struct*)
_tmp195;if(_tmp1BC->tag != 23)goto _LLE0;else{_tmp1BD=(void*)_tmp1BC->f1;}}_LLDF:
_tmp1BF=_tmp1BD;goto _LLE1;_LLE0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1BE=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp195;if(_tmp1BE->tag != 16)goto _LLE2;
else{_tmp1BF=(void*)_tmp1BE->f1;}}_LLE1: _tmp1C1=_tmp1BF;goto _LLE3;_LLE2: {struct
Cyc_Absyn_RgnsEff_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp195;if(
_tmp1C0->tag != 25)goto _LLE4;else{_tmp1C1=(void*)_tmp1C0->f1;}}_LLE3: Cyc_Tcutil_occurs(
evar,r,env,_tmp1C1);goto _LLC5;_LLE4: {struct Cyc_Absyn_JoinEff_struct*_tmp1C2=(
struct Cyc_Absyn_JoinEff_struct*)_tmp195;if(_tmp1C2->tag != 24)goto _LLE6;else{
_tmp1C3=_tmp1C2->f1;}}_LLE5: Cyc_Tcutil_occurslist(evar,r,env,_tmp1C3);goto _LLC5;
_LLE6:;_LLE7: goto _LLC5;_LLC5:;};}static void Cyc_Tcutil_occurslist(void*evar,
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
char*_tmpF2F;void*_tmpF2E;(_tmpF2E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2F="tq1 real_const not set.",
_tag_dyneither(_tmpF2F,sizeof(char),24))),_tag_dyneither(_tmpF2E,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpF32;void*_tmpF31;(_tmpF31=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpF32="tq2 real_const not set.",_tag_dyneither(_tmpF32,sizeof(char),24))),
_tag_dyneither(_tmpF31,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpF33;Cyc_Tcutil_failure_reason=((_tmpF33="(qualifiers don't match)",
_tag_dyneither(_tmpF33,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp1D2=x;union Cyc_Absyn_Constraint
_tmp1D3;int _tmp1D4;union Cyc_Absyn_Constraint _tmp1D5;void*_tmp1D6;union Cyc_Absyn_Constraint
_tmp1D7;union Cyc_Absyn_Constraint*_tmp1D8;_LLE9: _tmp1D3=*_tmp1D2;if((_tmp1D3.No_constr).tag
!= 3)goto _LLEB;_tmp1D4=(int)(_tmp1D3.No_constr).val;_LLEA:{union Cyc_Absyn_Constraint
_tmpF34;*x=(((_tmpF34.Forward_constr).val=y,(((_tmpF34.Forward_constr).tag=2,
_tmpF34))));}return;_LLEB: _tmp1D5=*_tmp1D2;if((_tmp1D5.Eq_constr).tag != 1)goto
_LLED;_tmp1D6=(void*)(_tmp1D5.Eq_constr).val;_LLEC: {union Cyc_Absyn_Constraint*
_tmp1DA=y;union Cyc_Absyn_Constraint _tmp1DB;int _tmp1DC;union Cyc_Absyn_Constraint
_tmp1DD;void*_tmp1DE;union Cyc_Absyn_Constraint _tmp1DF;union Cyc_Absyn_Constraint*
_tmp1E0;_LLF0: _tmp1DB=*_tmp1DA;if((_tmp1DB.No_constr).tag != 3)goto _LLF2;_tmp1DC=(
int)(_tmp1DB.No_constr).val;_LLF1:*y=*x;return;_LLF2: _tmp1DD=*_tmp1DA;if((
_tmp1DD.Eq_constr).tag != 1)goto _LLF4;_tmp1DE=(void*)(_tmp1DD.Eq_constr).val;
_LLF3: if(cmp(_tmp1D6,_tmp1DE)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LLF4: _tmp1DF=*_tmp1DA;if((_tmp1DF.Forward_constr).tag
!= 2)goto _LLEF;_tmp1E0=(union Cyc_Absyn_Constraint*)(_tmp1DF.Forward_constr).val;
_LLF5: {const char*_tmpF37;void*_tmpF36;(_tmpF36=0,Cyc_Tcutil_impos(((_tmpF37="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpF37,sizeof(char),40))),_tag_dyneither(_tmpF36,sizeof(void*),0)));}
_LLEF:;}_LLED: _tmp1D7=*_tmp1D2;if((_tmp1D7.Forward_constr).tag != 2)goto _LLE8;
_tmp1D8=(union Cyc_Absyn_Constraint*)(_tmp1D7.Forward_constr).val;_LLEE: {const
char*_tmpF3A;void*_tmpF39;(_tmpF39=0,Cyc_Tcutil_impos(((_tmpF3A="unify_conref: forward after compress",
_tag_dyneither(_tmpF3A,sizeof(char),37))),_tag_dyneither(_tmpF39,sizeof(void*),0)));}
_LLE8:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp1E5;_push_handler(& _tmp1E5);{int _tmp1E7=0;if(setjmp(_tmp1E5.handler))
_tmp1E7=1;if(!_tmp1E7){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp1E8=1;_npop_handler(0);return _tmp1E8;};;
_pop_handler();}else{void*_tmp1E6=(void*)_exn_thrown;void*_tmp1EA=_tmp1E6;_LLF7: {
struct Cyc_Tcutil_Unify_struct*_tmp1EB=(struct Cyc_Tcutil_Unify_struct*)_tmp1EA;
if(_tmp1EB->tag != Cyc_Tcutil_Unify)goto _LLF9;}_LLF8: return 0;_LLF9:;_LLFA:(void)
_throw(_tmp1EA);_LLF6:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static
int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmpF3B;struct _tuple0
_tmp1ED=(_tmpF3B.f1=b1,((_tmpF3B.f2=b2,_tmpF3B)));void*_tmp1EE;void*_tmp1F0;void*
_tmp1F2;void*_tmp1F4;void*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;
struct Cyc_Absyn_Exp*_tmp1FB;_LLFC: _tmp1EE=_tmp1ED.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1EF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1EE;if(_tmp1EF->tag != 0)goto
_LLFE;};_tmp1F0=_tmp1ED.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1F1=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LLFE;};_LLFD:
return 0;_LLFE: _tmp1F2=_tmp1ED.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1F3=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1F2;if(_tmp1F3->tag != 0)goto _LL100;};
_LLFF: return - 1;_LL100: _tmp1F4=_tmp1ED.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1F5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1F4;if(_tmp1F5->tag != 0)goto
_LL102;};_LL101: return 1;_LL102: _tmp1F6=_tmp1ED.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp1F7=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F6;if(_tmp1F7->tag != 1)goto _LLFB;
else{_tmp1F8=_tmp1F7->f1;}};_tmp1F9=_tmp1ED.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp1FA=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F9;if(_tmp1FA->tag != 1)goto _LLFB;
else{_tmp1FB=_tmp1FA->f1;}};_LL103: return Cyc_Evexp_const_exp_cmp(_tmp1F8,_tmp1FB);
_LLFB:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmpF3C;struct _tuple0 _tmp1FD=(_tmpF3C.f1=b1,((
_tmpF3C.f2=b2,_tmpF3C)));void*_tmp1FE;void*_tmp200;void*_tmp202;void*_tmp204;
void*_tmp206;struct Cyc_Absyn_Exp*_tmp208;void*_tmp209;struct Cyc_Absyn_Exp*
_tmp20B;_LL105: _tmp1FE=_tmp1FD.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1FE;if(_tmp1FF->tag != 0)goto _LL107;};
_tmp200=_tmp1FD.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp201=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp200;if(_tmp201->tag != 0)goto _LL107;};_LL106: return 0;_LL107: _tmp202=_tmp1FD.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp203=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp202;if(_tmp203->tag != 0)goto _LL109;};_LL108: return - 1;_LL109: _tmp204=_tmp1FD.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp205=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp204;if(_tmp205->tag != 0)goto _LL10B;};_LL10A: return 1;_LL10B: _tmp206=_tmp1FD.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp207=(struct Cyc_Absyn_Upper_b_struct*)_tmp206;
if(_tmp207->tag != 1)goto _LL104;else{_tmp208=_tmp207->f1;}};_tmp209=_tmp1FD.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp20A=(struct Cyc_Absyn_Upper_b_struct*)_tmp209;
if(_tmp20A->tag != 1)goto _LL104;else{_tmp20B=_tmp20A->f1;}};_LL10C: return Cyc_Evexp_const_exp_cmp(
_tmp208,_tmp20B);_LL104:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp20C=att;_LL10E: {
struct Cyc_Absyn_Regparm_att_struct*_tmp20D=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp20C;if(_tmp20D->tag != 0)goto _LL110;}_LL10F: return 0;_LL110: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp20E=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp20C;if(_tmp20E->tag != 1)goto
_LL112;}_LL111: return 1;_LL112: {struct Cyc_Absyn_Cdecl_att_struct*_tmp20F=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp20C;if(_tmp20F->tag != 2)goto _LL114;}_LL113:
return 2;_LL114: {struct Cyc_Absyn_Fastcall_att_struct*_tmp210=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp20C;if(_tmp210->tag != 3)goto _LL116;}_LL115: return 3;_LL116: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp211=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp20C;if(_tmp211->tag != 4)goto
_LL118;}_LL117: return 4;_LL118: {struct Cyc_Absyn_Const_att_struct*_tmp212=(struct
Cyc_Absyn_Const_att_struct*)_tmp20C;if(_tmp212->tag != 5)goto _LL11A;}_LL119:
return 5;_LL11A: {struct Cyc_Absyn_Aligned_att_struct*_tmp213=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp20C;if(_tmp213->tag != 6)goto _LL11C;}_LL11B: return 6;_LL11C: {struct Cyc_Absyn_Packed_att_struct*
_tmp214=(struct Cyc_Absyn_Packed_att_struct*)_tmp20C;if(_tmp214->tag != 7)goto
_LL11E;}_LL11D: return 7;_LL11E: {struct Cyc_Absyn_Section_att_struct*_tmp215=(
struct Cyc_Absyn_Section_att_struct*)_tmp20C;if(_tmp215->tag != 8)goto _LL120;}
_LL11F: return 8;_LL120: {struct Cyc_Absyn_Nocommon_att_struct*_tmp216=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp20C;if(_tmp216->tag != 9)goto _LL122;}_LL121: return 9;_LL122: {struct Cyc_Absyn_Shared_att_struct*
_tmp217=(struct Cyc_Absyn_Shared_att_struct*)_tmp20C;if(_tmp217->tag != 10)goto
_LL124;}_LL123: return 10;_LL124: {struct Cyc_Absyn_Unused_att_struct*_tmp218=(
struct Cyc_Absyn_Unused_att_struct*)_tmp20C;if(_tmp218->tag != 11)goto _LL126;}
_LL125: return 11;_LL126: {struct Cyc_Absyn_Weak_att_struct*_tmp219=(struct Cyc_Absyn_Weak_att_struct*)
_tmp20C;if(_tmp219->tag != 12)goto _LL128;}_LL127: return 12;_LL128: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp21A=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp20C;if(_tmp21A->tag != 13)goto
_LL12A;}_LL129: return 13;_LL12A: {struct Cyc_Absyn_Dllexport_att_struct*_tmp21B=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp20C;if(_tmp21B->tag != 14)goto _LL12C;}
_LL12B: return 14;_LL12C: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp21C=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp20C;if(_tmp21C->tag
!= 15)goto _LL12E;}_LL12D: return 15;_LL12E: {struct Cyc_Absyn_Constructor_att_struct*
_tmp21D=(struct Cyc_Absyn_Constructor_att_struct*)_tmp20C;if(_tmp21D->tag != 16)
goto _LL130;}_LL12F: return 16;_LL130: {struct Cyc_Absyn_Destructor_att_struct*
_tmp21E=(struct Cyc_Absyn_Destructor_att_struct*)_tmp20C;if(_tmp21E->tag != 17)
goto _LL132;}_LL131: return 17;_LL132: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp21F=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp20C;if(_tmp21F->tag
!= 18)goto _LL134;}_LL133: return 18;_LL134: {struct Cyc_Absyn_Format_att_struct*
_tmp220=(struct Cyc_Absyn_Format_att_struct*)_tmp20C;if(_tmp220->tag != 19)goto
_LL136;}_LL135: return 19;_LL136: {struct Cyc_Absyn_Initializes_att_struct*_tmp221=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp20C;if(_tmp221->tag != 20)goto _LL138;}
_LL137: return 20;_LL138:;_LL139: return 21;_LL10D:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpF3D;struct _tuple0 _tmp223=(_tmpF3D.f1=att1,((_tmpF3D.f2=att2,
_tmpF3D)));void*_tmp224;int _tmp226;void*_tmp227;int _tmp229;void*_tmp22A;int
_tmp22C;void*_tmp22D;int _tmp22F;void*_tmp230;int _tmp232;void*_tmp233;int _tmp235;
void*_tmp236;struct _dyneither_ptr _tmp238;void*_tmp239;struct _dyneither_ptr
_tmp23B;void*_tmp23C;enum Cyc_Absyn_Format_Type _tmp23E;int _tmp23F;int _tmp240;
void*_tmp241;enum Cyc_Absyn_Format_Type _tmp243;int _tmp244;int _tmp245;_LL13B:
_tmp224=_tmp223.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp225=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp224;if(_tmp225->tag != 0)goto _LL13D;else{_tmp226=_tmp225->f1;}};_tmp227=
_tmp223.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp228=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp227;if(_tmp228->tag != 0)goto _LL13D;else{_tmp229=_tmp228->f1;}};_LL13C:
_tmp22C=_tmp226;_tmp22F=_tmp229;goto _LL13E;_LL13D: _tmp22A=_tmp223.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp22B=(struct Cyc_Absyn_Initializes_att_struct*)_tmp22A;if(_tmp22B->tag != 20)
goto _LL13F;else{_tmp22C=_tmp22B->f1;}};_tmp22D=_tmp223.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp22E=(struct Cyc_Absyn_Initializes_att_struct*)_tmp22D;if(_tmp22E->tag != 20)
goto _LL13F;else{_tmp22F=_tmp22E->f1;}};_LL13E: _tmp232=_tmp22C;_tmp235=_tmp22F;
goto _LL140;_LL13F: _tmp230=_tmp223.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp231=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp230;if(_tmp231->tag != 6)goto _LL141;else{
_tmp232=_tmp231->f1;}};_tmp233=_tmp223.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp234=(struct Cyc_Absyn_Aligned_att_struct*)_tmp233;if(_tmp234->tag != 6)goto
_LL141;else{_tmp235=_tmp234->f1;}};_LL140: return Cyc_Core_intcmp(_tmp232,_tmp235);
_LL141: _tmp236=_tmp223.f1;{struct Cyc_Absyn_Section_att_struct*_tmp237=(struct Cyc_Absyn_Section_att_struct*)
_tmp236;if(_tmp237->tag != 8)goto _LL143;else{_tmp238=_tmp237->f1;}};_tmp239=
_tmp223.f2;{struct Cyc_Absyn_Section_att_struct*_tmp23A=(struct Cyc_Absyn_Section_att_struct*)
_tmp239;if(_tmp23A->tag != 8)goto _LL143;else{_tmp23B=_tmp23A->f1;}};_LL142: return
Cyc_strcmp((struct _dyneither_ptr)_tmp238,(struct _dyneither_ptr)_tmp23B);_LL143:
_tmp23C=_tmp223.f1;{struct Cyc_Absyn_Format_att_struct*_tmp23D=(struct Cyc_Absyn_Format_att_struct*)
_tmp23C;if(_tmp23D->tag != 19)goto _LL145;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;
_tmp240=_tmp23D->f3;}};_tmp241=_tmp223.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp242=(struct Cyc_Absyn_Format_att_struct*)_tmp241;if(_tmp242->tag != 19)goto
_LL145;else{_tmp243=_tmp242->f1;_tmp244=_tmp242->f2;_tmp245=_tmp242->f3;}};
_LL144: {int _tmp246=Cyc_Core_intcmp((int)((unsigned int)_tmp23E),(int)((
unsigned int)_tmp243));if(_tmp246 != 0)return _tmp246;{int _tmp247=Cyc_Core_intcmp(
_tmp23F,_tmp244);if(_tmp247 != 0)return _tmp247;return Cyc_Core_intcmp(_tmp240,
_tmp245);};}_LL145:;_LL146: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL13A:;}static int Cyc_Tcutil_equal_att(
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
_tuple14*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;switch(Cyc_Tcutil_tvar_kind(
tv,Cyc_Absyn_BoxKind)){case Cyc_Absyn_UniqueRgnKind: _LL147: t=(void*)& Cyc_Absyn_UniqueRgn_val;
break;case Cyc_Absyn_RgnKind: _LL148: t=(void*)& Cyc_Absyn_HeapRgn_val;break;case Cyc_Absyn_EffKind:
_LL149: t=Cyc_Absyn_empty_effect;break;case Cyc_Absyn_IntKind: _LL14A:{struct Cyc_Absyn_ValueofType_struct
_tmpF40;struct Cyc_Absyn_ValueofType_struct*_tmpF3F;t=(void*)((_tmpF3F=_cycalloc(
sizeof(*_tmpF3F)),((_tmpF3F[0]=((_tmpF40.tag=19,((_tmpF40.f1=Cyc_Absyn_uint_exp(
0,0),_tmpF40)))),_tmpF3F))));}break;default: _LL14B: t=Cyc_Absyn_sint_typ;break;}{
struct _tuple14*_tmpF41;return(_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41->f1=
tv,((_tmpF41->f2=t,_tmpF41)))));};}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of(void*t){void*_tmp24B=Cyc_Tcutil_compress(t);void*_tmp256;
void*_tmp258;void*_tmp259;struct Cyc_Absyn_DatatypeInfo _tmp25B;struct Cyc_List_List*
_tmp25C;struct Cyc_Absyn_PtrInfo _tmp25E;void*_tmp25F;struct Cyc_Absyn_PtrAtts
_tmp260;void*_tmp261;struct Cyc_Absyn_ArrayInfo _tmp263;void*_tmp264;struct Cyc_List_List*
_tmp266;struct Cyc_Absyn_DatatypeFieldInfo _tmp268;struct Cyc_List_List*_tmp269;
struct Cyc_Absyn_AggrInfo _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*
_tmp26E;struct Cyc_Absyn_FnInfo _tmp271;struct Cyc_List_List*_tmp272;struct Cyc_Core_Opt*
_tmp273;void*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_Absyn_VarargInfo*
_tmp276;struct Cyc_List_List*_tmp277;void*_tmp27D;struct Cyc_List_List*_tmp27F;
_LL14E: {struct Cyc_Absyn_VoidType_struct*_tmp24C=(struct Cyc_Absyn_VoidType_struct*)
_tmp24B;if(_tmp24C->tag != 0)goto _LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_FloatType_struct*
_tmp24D=(struct Cyc_Absyn_FloatType_struct*)_tmp24B;if(_tmp24D->tag != 7)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_DoubleType_struct*_tmp24E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp24B;if(_tmp24E->tag != 8)goto _LL154;}
_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_EnumType_struct*_tmp24F=(struct Cyc_Absyn_EnumType_struct*)
_tmp24B;if(_tmp24F->tag != 14)goto _LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp250=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp24B;if(_tmp250->tag != 15)goto
_LL158;}_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_ValueofType_struct*_tmp251=(
struct Cyc_Absyn_ValueofType_struct*)_tmp24B;if(_tmp251->tag != 19)goto _LL15A;}
_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_IntType_struct*_tmp252=(struct Cyc_Absyn_IntType_struct*)
_tmp24B;if(_tmp252->tag != 6)goto _LL15C;}_LL15B: return Cyc_Absyn_empty_effect;
_LL15C: {struct Cyc_Absyn_Evar_struct*_tmp253=(struct Cyc_Absyn_Evar_struct*)
_tmp24B;if(_tmp253->tag != 1)goto _LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_VarType_struct*
_tmp254=(struct Cyc_Absyn_VarType_struct*)_tmp24B;if(_tmp254->tag != 2)goto _LL160;}
_LL15F: switch(Cyc_Tcutil_typ_kind(t)){case Cyc_Absyn_RgnKind: _LL182: goto _LL183;
case Cyc_Absyn_UniqueRgnKind: _LL183: goto _LL184;case Cyc_Absyn_TopRgnKind: _LL184: {
struct Cyc_Absyn_AccessEff_struct _tmpF44;struct Cyc_Absyn_AccessEff_struct*_tmpF43;
return(void*)((_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43[0]=((_tmpF44.tag=23,((
_tmpF44.f1=(void*)t,_tmpF44)))),_tmpF43))));}case Cyc_Absyn_EffKind: _LL185: return
t;case Cyc_Absyn_IntKind: _LL186: return Cyc_Absyn_empty_effect;default: _LL187: {
struct Cyc_Absyn_RgnsEff_struct _tmpF47;struct Cyc_Absyn_RgnsEff_struct*_tmpF46;
return(void*)((_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46[0]=((_tmpF47.tag=25,((
_tmpF47.f1=(void*)t,_tmpF47)))),_tmpF46))));}}_LL160: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp255=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp24B;if(_tmp255->tag != 16)goto
_LL162;else{_tmp256=(void*)_tmp255->f1;}}_LL161: {struct Cyc_Absyn_AccessEff_struct
_tmpF4A;struct Cyc_Absyn_AccessEff_struct*_tmpF49;return(void*)((_tmpF49=
_cycalloc(sizeof(*_tmpF49)),((_tmpF49[0]=((_tmpF4A.tag=23,((_tmpF4A.f1=(void*)
_tmp256,_tmpF4A)))),_tmpF49))));}_LL162: {struct Cyc_Absyn_DynRgnType_struct*
_tmp257=(struct Cyc_Absyn_DynRgnType_struct*)_tmp24B;if(_tmp257->tag != 17)goto
_LL164;else{_tmp258=(void*)_tmp257->f1;_tmp259=(void*)_tmp257->f2;}}_LL163: {
struct Cyc_Absyn_AccessEff_struct _tmpF4D;struct Cyc_Absyn_AccessEff_struct*_tmpF4C;
return(void*)((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C[0]=((_tmpF4D.tag=23,((
_tmpF4D.f1=(void*)_tmp259,_tmpF4D)))),_tmpF4C))));}_LL164: {struct Cyc_Absyn_DatatypeType_struct*
_tmp25A=(struct Cyc_Absyn_DatatypeType_struct*)_tmp24B;if(_tmp25A->tag != 3)goto
_LL166;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.targs;}}_LL165: {struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp25C);struct Cyc_Absyn_JoinEff_struct _tmpF50;struct Cyc_Absyn_JoinEff_struct*
_tmpF4F;return Cyc_Tcutil_normalize_effect((void*)((_tmpF4F=_cycalloc(sizeof(*
_tmpF4F)),((_tmpF4F[0]=((_tmpF50.tag=24,((_tmpF50.f1=ts,_tmpF50)))),_tmpF4F)))));}
_LL166: {struct Cyc_Absyn_PointerType_struct*_tmp25D=(struct Cyc_Absyn_PointerType_struct*)
_tmp24B;if(_tmp25D->tag != 5)goto _LL168;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25E.elt_typ;
_tmp260=_tmp25E.ptr_atts;_tmp261=_tmp260.rgn;}}_LL167: {struct Cyc_Absyn_JoinEff_struct
_tmpF5F;struct Cyc_Absyn_AccessEff_struct*_tmpF5E;struct Cyc_Absyn_AccessEff_struct
_tmpF5D;void*_tmpF5C[2];struct Cyc_Absyn_JoinEff_struct*_tmpF5B;return Cyc_Tcutil_normalize_effect((
void*)((_tmpF5B=_cycalloc(sizeof(*_tmpF5B)),((_tmpF5B[0]=((_tmpF5F.tag=24,((
_tmpF5F.f1=((_tmpF5C[1]=Cyc_Tcutil_rgns_of(_tmp25F),((_tmpF5C[0]=(void*)((
_tmpF5E=_cycalloc(sizeof(*_tmpF5E)),((_tmpF5E[0]=((_tmpF5D.tag=23,((_tmpF5D.f1=(
void*)_tmp261,_tmpF5D)))),_tmpF5E)))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF5C,sizeof(void*),2)))))),
_tmpF5F)))),_tmpF5B)))));}_LL168: {struct Cyc_Absyn_ArrayType_struct*_tmp262=(
struct Cyc_Absyn_ArrayType_struct*)_tmp24B;if(_tmp262->tag != 9)goto _LL16A;else{
_tmp263=_tmp262->f1;_tmp264=_tmp263.elt_type;}}_LL169: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp264));_LL16A: {struct Cyc_Absyn_TupleType_struct*_tmp265=(
struct Cyc_Absyn_TupleType_struct*)_tmp24B;if(_tmp265->tag != 11)goto _LL16C;else{
_tmp266=_tmp265->f1;}}_LL16B: {struct Cyc_List_List*_tmp28F=0;for(0;_tmp266 != 0;
_tmp266=_tmp266->tl){struct Cyc_List_List*_tmpF60;_tmp28F=((_tmpF60=_cycalloc(
sizeof(*_tmpF60)),((_tmpF60->hd=(*((struct _tuple11*)_tmp266->hd)).f2,((_tmpF60->tl=
_tmp28F,_tmpF60))))));}_tmp269=_tmp28F;goto _LL16D;}_LL16C: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp267=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp24B;if(_tmp267->tag != 4)
goto _LL16E;else{_tmp268=_tmp267->f1;_tmp269=_tmp268.targs;}}_LL16D: _tmp26C=
_tmp269;goto _LL16F;_LL16E: {struct Cyc_Absyn_AggrType_struct*_tmp26A=(struct Cyc_Absyn_AggrType_struct*)
_tmp24B;if(_tmp26A->tag != 12)goto _LL170;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26B.targs;}}
_LL16F: {struct Cyc_Absyn_JoinEff_struct _tmpF63;struct Cyc_Absyn_JoinEff_struct*
_tmpF62;return Cyc_Tcutil_normalize_effect((void*)((_tmpF62=_cycalloc(sizeof(*
_tmpF62)),((_tmpF62[0]=((_tmpF63.tag=24,((_tmpF63.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26C),
_tmpF63)))),_tmpF62)))));}_LL170: {struct Cyc_Absyn_AnonAggrType_struct*_tmp26D=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp24B;if(_tmp26D->tag != 13)goto _LL172;
else{_tmp26E=_tmp26D->f2;}}_LL171: {struct Cyc_Absyn_JoinEff_struct _tmpF66;struct
Cyc_Absyn_JoinEff_struct*_tmpF65;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF65=_cycalloc(sizeof(*_tmpF65)),((_tmpF65[0]=((_tmpF66.tag=24,((_tmpF66.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp26E),_tmpF66)))),_tmpF65)))));}
_LL172: {struct Cyc_Absyn_TagType_struct*_tmp26F=(struct Cyc_Absyn_TagType_struct*)
_tmp24B;if(_tmp26F->tag != 20)goto _LL174;}_LL173: return Cyc_Absyn_empty_effect;
_LL174: {struct Cyc_Absyn_FnType_struct*_tmp270=(struct Cyc_Absyn_FnType_struct*)
_tmp24B;if(_tmp270->tag != 10)goto _LL176;else{_tmp271=_tmp270->f1;_tmp272=_tmp271.tvars;
_tmp273=_tmp271.effect;_tmp274=_tmp271.ret_typ;_tmp275=_tmp271.args;_tmp276=
_tmp271.cyc_varargs;_tmp277=_tmp271.rgn_po;}}_LL175: {void*_tmp295=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp272),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp273))->v);return Cyc_Tcutil_normalize_effect(_tmp295);}_LL176: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp278=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp24B;if(_tmp278->tag != 22)goto _LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_HeapRgn_struct*
_tmp279=(struct Cyc_Absyn_HeapRgn_struct*)_tmp24B;if(_tmp279->tag != 21)goto _LL17A;}
_LL179: return Cyc_Absyn_empty_effect;_LL17A: {struct Cyc_Absyn_AccessEff_struct*
_tmp27A=(struct Cyc_Absyn_AccessEff_struct*)_tmp24B;if(_tmp27A->tag != 23)goto
_LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_JoinEff_struct*_tmp27B=(
struct Cyc_Absyn_JoinEff_struct*)_tmp24B;if(_tmp27B->tag != 24)goto _LL17E;}_LL17D:
return t;_LL17E: {struct Cyc_Absyn_RgnsEff_struct*_tmp27C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp24B;if(_tmp27C->tag != 25)goto _LL180;else{_tmp27D=(void*)_tmp27C->f1;}}_LL17F:
return Cyc_Tcutil_rgns_of(_tmp27D);_LL180: {struct Cyc_Absyn_TypedefType_struct*
_tmp27E=(struct Cyc_Absyn_TypedefType_struct*)_tmp24B;if(_tmp27E->tag != 18)goto
_LL14D;else{_tmp27F=_tmp27E->f2;}}_LL181: {struct Cyc_Absyn_JoinEff_struct _tmpF69;
struct Cyc_Absyn_JoinEff_struct*_tmpF68;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68[0]=((_tmpF69.tag=24,((_tmpF69.f1=((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp27F),_tmpF69)))),_tmpF68)))));}_LL14D:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp298=e;struct Cyc_List_List*_tmp29A;struct Cyc_List_List**_tmp29B;void*_tmp29D;
_LL18A: {struct Cyc_Absyn_JoinEff_struct*_tmp299=(struct Cyc_Absyn_JoinEff_struct*)
_tmp298;if(_tmp299->tag != 24)goto _LL18C;else{_tmp29A=_tmp299->f1;_tmp29B=(struct
Cyc_List_List**)& _tmp299->f1;}}_LL18B: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp29B;for(0;effs != 0;effs=effs->tl){void*_tmp29E=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp29E));{void*_tmp29F=(
void*)effs->hd;void*_tmp2A2;_LL191: {struct Cyc_Absyn_JoinEff_struct*_tmp2A0=(
struct Cyc_Absyn_JoinEff_struct*)_tmp29F;if(_tmp2A0->tag != 24)goto _LL193;}_LL192:
goto _LL194;_LL193: {struct Cyc_Absyn_AccessEff_struct*_tmp2A1=(struct Cyc_Absyn_AccessEff_struct*)
_tmp29F;if(_tmp2A1->tag != 23)goto _LL195;else{_tmp2A2=(void*)_tmp2A1->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2A3=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2A2;if(
_tmp2A3->tag != 21)goto _LL195;};}}_LL194: redo_join=1;goto _LL190;_LL195:;_LL196:
goto _LL190;_LL190:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp29B;for(0;effs != 0;effs=effs->tl){void*_tmp2A4=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp2A6;void*_tmp2A8;_LL198: {struct Cyc_Absyn_JoinEff_struct*
_tmp2A5=(struct Cyc_Absyn_JoinEff_struct*)_tmp2A4;if(_tmp2A5->tag != 24)goto _LL19A;
else{_tmp2A6=_tmp2A5->f1;}}_LL199: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp2A6,effects);goto _LL197;_LL19A: {
struct Cyc_Absyn_AccessEff_struct*_tmp2A7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2A4;if(_tmp2A7->tag != 23)goto _LL19C;else{_tmp2A8=(void*)_tmp2A7->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2A9=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2A8;if(
_tmp2A9->tag != 21)goto _LL19C;};}}_LL19B: goto _LL197;_LL19C:;_LL19D:{struct Cyc_List_List*
_tmpF6A;effects=((_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A->hd=_tmp2A4,((
_tmpF6A->tl=effects,_tmpF6A))))));}goto _LL197;_LL197:;}}*_tmp29B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL18C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp29C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp298;if(_tmp29C->tag != 25)goto _LL18E;
else{_tmp29D=(void*)_tmp29C->f1;}}_LL18D: {void*_tmp2AB=Cyc_Tcutil_compress(
_tmp29D);_LL19F: {struct Cyc_Absyn_Evar_struct*_tmp2AC=(struct Cyc_Absyn_Evar_struct*)
_tmp2AB;if(_tmp2AC->tag != 1)goto _LL1A1;}_LL1A0: goto _LL1A2;_LL1A1: {struct Cyc_Absyn_VarType_struct*
_tmp2AD=(struct Cyc_Absyn_VarType_struct*)_tmp2AB;if(_tmp2AD->tag != 2)goto _LL1A3;}
_LL1A2: return e;_LL1A3:;_LL1A4: return Cyc_Tcutil_rgns_of(_tmp29D);_LL19E:;}_LL18E:;
_LL18F: return e;_LL189:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmpF74;
struct Cyc_Core_Opt*_tmpF73;struct Cyc_Absyn_FnInfo _tmpF72;struct Cyc_Absyn_FnType_struct*
_tmpF71;struct Cyc_Absyn_FnType_struct*_tmp2AE=(_tmpF71=_cycalloc(sizeof(*_tmpF71)),((
_tmpF71[0]=((_tmpF74.tag=10,((_tmpF74.f1=((_tmpF72.tvars=0,((_tmpF72.effect=((
_tmpF73=_cycalloc(sizeof(*_tmpF73)),((_tmpF73->v=eff,_tmpF73)))),((_tmpF72.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmpF72.args=0,((_tmpF72.c_varargs=0,((_tmpF72.cyc_varargs=
0,((_tmpF72.rgn_po=0,((_tmpF72.attributes=0,_tmpF72)))))))))))))))),_tmpF74)))),
_tmpF71)));return Cyc_Absyn_atb_typ((void*)_tmp2AE,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (
void*)& Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp2B3=Cyc_Tcutil_compress(e);void*
_tmp2B5;struct Cyc_List_List*_tmp2B7;void*_tmp2B9;struct Cyc_Core_Opt*_tmp2BB;
struct Cyc_Core_Opt*_tmp2BC;struct Cyc_Core_Opt**_tmp2BD;struct Cyc_Core_Opt*
_tmp2BE;_LL1A6: {struct Cyc_Absyn_AccessEff_struct*_tmp2B4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2B3;if(_tmp2B4->tag != 23)goto _LL1A8;else{_tmp2B5=(void*)_tmp2B4->f1;}}_LL1A7:
if(constrain)return Cyc_Tcutil_unify(r,_tmp2B5);_tmp2B5=Cyc_Tcutil_compress(
_tmp2B5);if(r == _tmp2B5)return 1;{struct _tuple0 _tmpF75;struct _tuple0 _tmp2C0=(
_tmpF75.f1=r,((_tmpF75.f2=_tmp2B5,_tmpF75)));void*_tmp2C1;struct Cyc_Absyn_Tvar*
_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Tvar*_tmp2C6;_LL1B1: _tmp2C1=_tmp2C0.f1;{
struct Cyc_Absyn_VarType_struct*_tmp2C2=(struct Cyc_Absyn_VarType_struct*)_tmp2C1;
if(_tmp2C2->tag != 2)goto _LL1B3;else{_tmp2C3=_tmp2C2->f1;}};_tmp2C4=_tmp2C0.f2;{
struct Cyc_Absyn_VarType_struct*_tmp2C5=(struct Cyc_Absyn_VarType_struct*)_tmp2C4;
if(_tmp2C5->tag != 2)goto _LL1B3;else{_tmp2C6=_tmp2C5->f1;}};_LL1B2: return Cyc_Absyn_tvar_cmp(
_tmp2C3,_tmp2C6)== 0;_LL1B3:;_LL1B4: return 0;_LL1B0:;};_LL1A8: {struct Cyc_Absyn_JoinEff_struct*
_tmp2B6=(struct Cyc_Absyn_JoinEff_struct*)_tmp2B3;if(_tmp2B6->tag != 24)goto _LL1AA;
else{_tmp2B7=_tmp2B6->f1;}}_LL1A9: for(0;_tmp2B7 != 0;_tmp2B7=_tmp2B7->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp2B7->hd))return 1;}return 0;_LL1AA: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2B8=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2B3;if(_tmp2B8->tag != 25)goto _LL1AC;
else{_tmp2B9=(void*)_tmp2B8->f1;}}_LL1AB: {void*_tmp2C7=Cyc_Tcutil_rgns_of(
_tmp2B9);void*_tmp2C9;_LL1B6: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp2C7;if(_tmp2C8->tag != 25)goto _LL1B8;else{_tmp2C9=(void*)_tmp2C8->f1;}}_LL1B7:
if(!constrain)return 0;{void*_tmp2CA=Cyc_Tcutil_compress(_tmp2C9);struct Cyc_Core_Opt*
_tmp2CC;struct Cyc_Core_Opt*_tmp2CD;struct Cyc_Core_Opt**_tmp2CE;struct Cyc_Core_Opt*
_tmp2CF;_LL1BB: {struct Cyc_Absyn_Evar_struct*_tmp2CB=(struct Cyc_Absyn_Evar_struct*)
_tmp2CA;if(_tmp2CB->tag != 1)goto _LL1BD;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;
_tmp2CE=(struct Cyc_Core_Opt**)& _tmp2CB->f2;_tmp2CF=_tmp2CB->f4;}}_LL1BC: {void*
_tmp2D0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp2CF);Cyc_Tcutil_occurs(
_tmp2D0,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CF))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmpF84;struct Cyc_Absyn_AccessEff_struct*
_tmpF83;struct Cyc_Absyn_AccessEff_struct _tmpF82;void*_tmpF81[2];struct Cyc_Absyn_JoinEff_struct*
_tmpF80;void*_tmp2D1=Cyc_Tcutil_dummy_fntype((void*)((_tmpF80=_cycalloc(sizeof(*
_tmpF80)),((_tmpF80[0]=((_tmpF84.tag=24,((_tmpF84.f1=((_tmpF81[1]=(void*)((
_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF82.tag=23,((_tmpF82.f1=(
void*)r,_tmpF82)))),_tmpF83)))),((_tmpF81[0]=_tmp2D0,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF81,sizeof(void*),2)))))),
_tmpF84)))),_tmpF80)))));{struct Cyc_Core_Opt*_tmpF85;*_tmp2CE=((_tmpF85=
_cycalloc(sizeof(*_tmpF85)),((_tmpF85->v=_tmp2D1,_tmpF85))));}return 1;};}_LL1BD:;
_LL1BE: return 0;_LL1BA:;};_LL1B8:;_LL1B9: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp2C7);_LL1B5:;}_LL1AC: {struct Cyc_Absyn_Evar_struct*_tmp2BA=(
struct Cyc_Absyn_Evar_struct*)_tmp2B3;if(_tmp2BA->tag != 1)goto _LL1AE;else{_tmp2BB=
_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;_tmp2BD=(struct Cyc_Core_Opt**)& _tmp2BA->f2;
_tmp2BE=_tmp2BA->f4;}}_LL1AD: if(_tmp2BB == 0  || (enum Cyc_Absyn_Kind)_tmp2BB->v != 
Cyc_Absyn_EffKind){const char*_tmpF88;void*_tmpF87;(_tmpF87=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF88="effect evar has wrong kind",
_tag_dyneither(_tmpF88,sizeof(char),27))),_tag_dyneither(_tmpF87,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp2DA=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp2BE);Cyc_Tcutil_occurs(_tmp2DA,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2BE))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpF9D;struct Cyc_List_List*_tmpF9C;struct Cyc_Absyn_AccessEff_struct _tmpF9B;
struct Cyc_Absyn_AccessEff_struct*_tmpF9A;struct Cyc_List_List*_tmpF99;struct Cyc_Absyn_JoinEff_struct*
_tmpF98;struct Cyc_Absyn_JoinEff_struct*_tmp2DB=(_tmpF98=_cycalloc(sizeof(*
_tmpF98)),((_tmpF98[0]=((_tmpF9D.tag=24,((_tmpF9D.f1=((_tmpF99=_cycalloc(sizeof(*
_tmpF99)),((_tmpF99->hd=_tmp2DA,((_tmpF99->tl=((_tmpF9C=_cycalloc(sizeof(*
_tmpF9C)),((_tmpF9C->hd=(void*)((_tmpF9A=_cycalloc(sizeof(*_tmpF9A)),((_tmpF9A[0]=((
_tmpF9B.tag=23,((_tmpF9B.f1=(void*)r,_tmpF9B)))),_tmpF9A)))),((_tmpF9C->tl=0,
_tmpF9C)))))),_tmpF99)))))),_tmpF9D)))),_tmpF98)));{struct Cyc_Core_Opt*_tmpF9E;*
_tmp2BD=((_tmpF9E=_cycalloc(sizeof(*_tmpF9E)),((_tmpF9E->v=(void*)_tmp2DB,
_tmpF9E))));}return 1;};};_LL1AE:;_LL1AF: return 0;_LL1A5:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp2E3=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp2E6;void*_tmp2E8;struct Cyc_Core_Opt*
_tmp2EA;struct Cyc_Core_Opt*_tmp2EB;struct Cyc_Core_Opt**_tmp2EC;struct Cyc_Core_Opt*
_tmp2ED;_LL1C0: {struct Cyc_Absyn_AccessEff_struct*_tmp2E4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2E3;if(_tmp2E4->tag != 23)goto _LL1C2;}_LL1C1: return 0;_LL1C2: {struct Cyc_Absyn_JoinEff_struct*
_tmp2E5=(struct Cyc_Absyn_JoinEff_struct*)_tmp2E3;if(_tmp2E5->tag != 24)goto _LL1C4;
else{_tmp2E6=_tmp2E5->f1;}}_LL1C3: for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp2E6->hd))return 1;}return 0;_LL1C4: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2E7=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2E3;if(_tmp2E7->tag != 25)goto _LL1C6;
else{_tmp2E8=(void*)_tmp2E7->f1;}}_LL1C5: _tmp2E8=Cyc_Tcutil_compress(_tmp2E8);
if(t == _tmp2E8)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2E8);{
void*_tmp2EE=Cyc_Tcutil_rgns_of(t);void*_tmp2F0;_LL1CB: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2EF=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2EE;if(_tmp2EF->tag != 25)goto _LL1CD;
else{_tmp2F0=(void*)_tmp2EF->f1;}}_LL1CC: {struct _tuple0 _tmpF9F;struct _tuple0
_tmp2F2=(_tmpF9F.f1=Cyc_Tcutil_compress(_tmp2F0),((_tmpF9F.f2=_tmp2E8,_tmpF9F)));
void*_tmp2F3;struct Cyc_Absyn_Tvar*_tmp2F5;void*_tmp2F6;struct Cyc_Absyn_Tvar*
_tmp2F8;_LL1D0: _tmp2F3=_tmp2F2.f1;{struct Cyc_Absyn_VarType_struct*_tmp2F4=(
struct Cyc_Absyn_VarType_struct*)_tmp2F3;if(_tmp2F4->tag != 2)goto _LL1D2;else{
_tmp2F5=_tmp2F4->f1;}};_tmp2F6=_tmp2F2.f2;{struct Cyc_Absyn_VarType_struct*
_tmp2F7=(struct Cyc_Absyn_VarType_struct*)_tmp2F6;if(_tmp2F7->tag != 2)goto _LL1D2;
else{_tmp2F8=_tmp2F7->f1;}};_LL1D1: return Cyc_Tcutil_unify(t,_tmp2E8);_LL1D2:;
_LL1D3: return _tmp2F0 == _tmp2E8;_LL1CF:;}_LL1CD:;_LL1CE: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp2EE);_LL1CA:;};_LL1C6: {struct Cyc_Absyn_Evar_struct*
_tmp2E9=(struct Cyc_Absyn_Evar_struct*)_tmp2E3;if(_tmp2E9->tag != 1)goto _LL1C8;
else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2E9->f2;_tmp2EC=(struct Cyc_Core_Opt**)&
_tmp2E9->f2;_tmp2ED=_tmp2E9->f4;}}_LL1C7: if(_tmp2EA == 0  || (enum Cyc_Absyn_Kind)
_tmp2EA->v != Cyc_Absyn_EffKind){const char*_tmpFA2;void*_tmpFA1;(_tmpFA1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA2="effect evar has wrong kind",
_tag_dyneither(_tmpFA2,sizeof(char),27))),_tag_dyneither(_tmpFA1,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp2FB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp2ED);Cyc_Tcutil_occurs(_tmp2FB,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2ED))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpFB7;struct Cyc_List_List*_tmpFB6;struct Cyc_Absyn_RgnsEff_struct _tmpFB5;struct
Cyc_Absyn_RgnsEff_struct*_tmpFB4;struct Cyc_List_List*_tmpFB3;struct Cyc_Absyn_JoinEff_struct*
_tmpFB2;struct Cyc_Absyn_JoinEff_struct*_tmp2FC=(_tmpFB2=_cycalloc(sizeof(*
_tmpFB2)),((_tmpFB2[0]=((_tmpFB7.tag=24,((_tmpFB7.f1=((_tmpFB3=_cycalloc(sizeof(*
_tmpFB3)),((_tmpFB3->hd=_tmp2FB,((_tmpFB3->tl=((_tmpFB6=_cycalloc(sizeof(*
_tmpFB6)),((_tmpFB6->hd=(void*)((_tmpFB4=_cycalloc(sizeof(*_tmpFB4)),((_tmpFB4[0]=((
_tmpFB5.tag=25,((_tmpFB5.f1=(void*)t,_tmpFB5)))),_tmpFB4)))),((_tmpFB6->tl=0,
_tmpFB6)))))),_tmpFB3)))))),_tmpFB7)))),_tmpFB2)));{struct Cyc_Core_Opt*_tmpFB8;*
_tmp2EC=((_tmpFB8=_cycalloc(sizeof(*_tmpFB8)),((_tmpFB8->v=(void*)_tmp2FC,
_tmpFB8))));}return 1;};};_LL1C8:;_LL1C9: return 0;_LL1BF:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp304=e;struct Cyc_Absyn_Tvar*_tmp306;struct Cyc_List_List*_tmp308;void*
_tmp30A;struct Cyc_Core_Opt*_tmp30C;struct Cyc_Core_Opt*_tmp30D;struct Cyc_Core_Opt**
_tmp30E;struct Cyc_Core_Opt*_tmp30F;_LL1D5: {struct Cyc_Absyn_VarType_struct*
_tmp305=(struct Cyc_Absyn_VarType_struct*)_tmp304;if(_tmp305->tag != 2)goto _LL1D7;
else{_tmp306=_tmp305->f1;}}_LL1D6: return Cyc_Absyn_tvar_cmp(v,_tmp306)== 0;_LL1D7: {
struct Cyc_Absyn_JoinEff_struct*_tmp307=(struct Cyc_Absyn_JoinEff_struct*)_tmp304;
if(_tmp307->tag != 24)goto _LL1D9;else{_tmp308=_tmp307->f1;}}_LL1D8: for(0;_tmp308
!= 0;_tmp308=_tmp308->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp308->hd))return 1;}return 0;_LL1D9: {struct Cyc_Absyn_RgnsEff_struct*
_tmp309=(struct Cyc_Absyn_RgnsEff_struct*)_tmp304;if(_tmp309->tag != 25)goto _LL1DB;
else{_tmp30A=(void*)_tmp309->f1;}}_LL1DA: {void*_tmp310=Cyc_Tcutil_rgns_of(
_tmp30A);void*_tmp312;_LL1E0: {struct Cyc_Absyn_RgnsEff_struct*_tmp311=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp310;if(_tmp311->tag != 25)goto _LL1E2;else{_tmp312=(void*)_tmp311->f1;}}_LL1E1:
if(!may_constrain_evars)return 0;{void*_tmp313=Cyc_Tcutil_compress(_tmp312);
struct Cyc_Core_Opt*_tmp315;struct Cyc_Core_Opt*_tmp316;struct Cyc_Core_Opt**
_tmp317;struct Cyc_Core_Opt*_tmp318;_LL1E5: {struct Cyc_Absyn_Evar_struct*_tmp314=(
struct Cyc_Absyn_Evar_struct*)_tmp313;if(_tmp314->tag != 1)goto _LL1E7;else{_tmp315=
_tmp314->f1;_tmp316=_tmp314->f2;_tmp317=(struct Cyc_Core_Opt**)& _tmp314->f2;
_tmp318=_tmp314->f4;}}_LL1E6: {void*_tmp319=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp318);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp318))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmpFC7;struct Cyc_Absyn_VarType_struct*_tmpFC6;
struct Cyc_Absyn_VarType_struct _tmpFC5;void*_tmpFC4[2];struct Cyc_Absyn_JoinEff_struct*
_tmpFC3;void*_tmp31A=Cyc_Tcutil_dummy_fntype((void*)((_tmpFC3=_cycalloc(sizeof(*
_tmpFC3)),((_tmpFC3[0]=((_tmpFC7.tag=24,((_tmpFC7.f1=((_tmpFC4[1]=(void*)((
_tmpFC6=_cycalloc(sizeof(*_tmpFC6)),((_tmpFC6[0]=((_tmpFC5.tag=2,((_tmpFC5.f1=v,
_tmpFC5)))),_tmpFC6)))),((_tmpFC4[0]=_tmp319,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFC4,sizeof(void*),2)))))),
_tmpFC7)))),_tmpFC3)))));{struct Cyc_Core_Opt*_tmpFC8;*_tmp317=((_tmpFC8=
_cycalloc(sizeof(*_tmpFC8)),((_tmpFC8->v=_tmp31A,_tmpFC8))));}return 1;};}_LL1E7:;
_LL1E8: return 0;_LL1E4:;};_LL1E2:;_LL1E3: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp310);_LL1DF:;}_LL1DB: {struct Cyc_Absyn_Evar_struct*
_tmp30B=(struct Cyc_Absyn_Evar_struct*)_tmp304;if(_tmp30B->tag != 1)goto _LL1DD;
else{_tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;_tmp30E=(struct Cyc_Core_Opt**)&
_tmp30B->f2;_tmp30F=_tmp30B->f4;}}_LL1DC: if(_tmp30C == 0  || (enum Cyc_Absyn_Kind)
_tmp30C->v != Cyc_Absyn_EffKind){const char*_tmpFCB;void*_tmpFCA;(_tmpFCA=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCB="effect evar has wrong kind",
_tag_dyneither(_tmpFCB,sizeof(char),27))),_tag_dyneither(_tmpFCA,sizeof(void*),0)));}{
void*_tmp323=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp30F);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp30F))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpFE0;struct Cyc_List_List*_tmpFDF;struct Cyc_Absyn_VarType_struct _tmpFDE;struct
Cyc_Absyn_VarType_struct*_tmpFDD;struct Cyc_List_List*_tmpFDC;struct Cyc_Absyn_JoinEff_struct*
_tmpFDB;struct Cyc_Absyn_JoinEff_struct*_tmp324=(_tmpFDB=_cycalloc(sizeof(*
_tmpFDB)),((_tmpFDB[0]=((_tmpFE0.tag=24,((_tmpFE0.f1=((_tmpFDC=_cycalloc(sizeof(*
_tmpFDC)),((_tmpFDC->hd=_tmp323,((_tmpFDC->tl=((_tmpFDF=_cycalloc(sizeof(*
_tmpFDF)),((_tmpFDF->hd=(void*)((_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD[0]=((
_tmpFDE.tag=2,((_tmpFDE.f1=v,_tmpFDE)))),_tmpFDD)))),((_tmpFDF->tl=0,_tmpFDF)))))),
_tmpFDC)))))),_tmpFE0)))),_tmpFDB)));{struct Cyc_Core_Opt*_tmpFE1;*_tmp30E=((
_tmpFE1=_cycalloc(sizeof(*_tmpFE1)),((_tmpFE1->v=(void*)_tmp324,_tmpFE1))));}
return 1;};};_LL1DD:;_LL1DE: return 0;_LL1D4:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp32C=e;struct Cyc_List_List*_tmp32E;void*_tmp330;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*
_tmp32D=(struct Cyc_Absyn_JoinEff_struct*)_tmp32C;if(_tmp32D->tag != 24)goto _LL1EC;
else{_tmp32E=_tmp32D->f1;}}_LL1EB: for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp32E->hd))return 1;}return 0;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp32F=(struct Cyc_Absyn_RgnsEff_struct*)_tmp32C;if(_tmp32F->tag != 25)goto _LL1EE;
else{_tmp330=(void*)_tmp32F->f1;}}_LL1ED: {void*_tmp332=Cyc_Tcutil_rgns_of(
_tmp330);void*_tmp334;_LL1F3: {struct Cyc_Absyn_RgnsEff_struct*_tmp333=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp332;if(_tmp333->tag != 25)goto _LL1F5;else{_tmp334=(void*)_tmp333->f1;}}_LL1F4:
return 0;_LL1F5:;_LL1F6: return Cyc_Tcutil_evar_in_effect(evar,_tmp332);_LL1F2:;}
_LL1EE: {struct Cyc_Absyn_Evar_struct*_tmp331=(struct Cyc_Absyn_Evar_struct*)
_tmp32C;if(_tmp331->tag != 1)goto _LL1F0;}_LL1EF: return evar == e;_LL1F0:;_LL1F1:
return 0;_LL1E9:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp335=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp337;void*_tmp339;
struct Cyc_Absyn_Tvar*_tmp33B;void*_tmp33D;struct Cyc_Core_Opt*_tmp33F;struct Cyc_Core_Opt**
_tmp340;struct Cyc_Core_Opt*_tmp341;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*
_tmp336=(struct Cyc_Absyn_JoinEff_struct*)_tmp335;if(_tmp336->tag != 24)goto _LL1FA;
else{_tmp337=_tmp336->f1;}}_LL1F9: for(0;_tmp337 != 0;_tmp337=_tmp337->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp337->hd,e2))return 0;}return 1;_LL1FA: {struct Cyc_Absyn_AccessEff_struct*
_tmp338=(struct Cyc_Absyn_AccessEff_struct*)_tmp335;if(_tmp338->tag != 23)goto
_LL1FC;else{_tmp339=(void*)_tmp338->f1;}}_LL1FB: return Cyc_Tcutil_region_in_effect(
0,_tmp339,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp339,(void*)& Cyc_Absyn_HeapRgn_val);
_LL1FC: {struct Cyc_Absyn_VarType_struct*_tmp33A=(struct Cyc_Absyn_VarType_struct*)
_tmp335;if(_tmp33A->tag != 2)goto _LL1FE;else{_tmp33B=_tmp33A->f1;}}_LL1FD: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp33B,e2);_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp33C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp335;if(_tmp33C->tag != 25)goto _LL200;
else{_tmp33D=(void*)_tmp33C->f1;}}_LL1FF: {void*_tmp342=Cyc_Tcutil_rgns_of(
_tmp33D);void*_tmp344;_LL205: {struct Cyc_Absyn_RgnsEff_struct*_tmp343=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp342;if(_tmp343->tag != 25)goto _LL207;else{_tmp344=(void*)_tmp343->f1;}}_LL206:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp344,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp344,Cyc_Absyn_sint_typ);_LL207:;
_LL208: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp342,e2);_LL204:;}
_LL200: {struct Cyc_Absyn_Evar_struct*_tmp33E=(struct Cyc_Absyn_Evar_struct*)
_tmp335;if(_tmp33E->tag != 1)goto _LL202;else{_tmp33F=_tmp33E->f2;_tmp340=(struct
Cyc_Core_Opt**)& _tmp33E->f2;_tmp341=_tmp33E->f4;}}_LL201: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmpFE2;*_tmp340=((_tmpFE2=_cycalloc(sizeof(*_tmpFE2)),((
_tmpFE2->v=Cyc_Absyn_empty_effect,_tmpFE2))));}return 1;_LL202:;_LL203: {const
char*_tmpFE6;void*_tmpFE5[1];struct Cyc_String_pa_struct _tmpFE4;(_tmpFE4.tag=0,((
_tmpFE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmpFE5[0]=& _tmpFE4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpFE6="subset_effect: bad effect: %s",_tag_dyneither(
_tmpFE6,sizeof(char),30))),_tag_dyneither(_tmpFE5,sizeof(void*),1)))))));}_LL1F7:;}
static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp34A;void*_tmp34B;void*_tmp34C;struct _tuple0*_tmp349=(struct
_tuple0*)r1->hd;_tmp34A=*_tmp349;_tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;{int found=
_tmp34B == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp34E;void*_tmp34F;void*_tmp350;struct
_tuple0*_tmp34D=(struct _tuple0*)r2->hd;_tmp34E=*_tmp34D;_tmp34F=_tmp34E.f1;
_tmp350=_tmp34E.f2;if(Cyc_Tcutil_unify(_tmp34B,_tmp34F) && Cyc_Tcutil_unify(
_tmp34C,_tmp350)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple18{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple19{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp351=t1;struct Cyc_Core_Opt*_tmp353;struct Cyc_Core_Opt*
_tmp354;struct Cyc_Core_Opt**_tmp355;struct Cyc_Core_Opt*_tmp356;_LL20A: {struct
Cyc_Absyn_Evar_struct*_tmp352=(struct Cyc_Absyn_Evar_struct*)_tmp351;if(_tmp352->tag
!= 1)goto _LL20C;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;_tmp355=(struct Cyc_Core_Opt**)&
_tmp352->f2;_tmp356=_tmp352->f4;}}_LL20B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp356))->v,t2);{enum 
Cyc_Absyn_Kind _tmp357=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp357,(
enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp353))->v)){{struct Cyc_Core_Opt*
_tmpFE7;*_tmp355=((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7->v=t2,_tmpFE7))));}
return;}else{{void*_tmp359=t2;struct Cyc_Core_Opt*_tmp35B;struct Cyc_Core_Opt**
_tmp35C;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Absyn_PtrInfo _tmp35F;_LL20F: {
struct Cyc_Absyn_Evar_struct*_tmp35A=(struct Cyc_Absyn_Evar_struct*)_tmp359;if(
_tmp35A->tag != 1)goto _LL211;else{_tmp35B=_tmp35A->f2;_tmp35C=(struct Cyc_Core_Opt**)&
_tmp35A->f2;_tmp35D=_tmp35A->f4;}}_LL210: {struct Cyc_List_List*_tmp360=(struct
Cyc_List_List*)_tmp356->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp35D))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp360,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpFE8;Cyc_Tcutil_failure_reason=((_tmpFE8="(type variable would escape scope)",
_tag_dyneither(_tmpFE8,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
if(Cyc_Tcutil_kind_leq((enum Cyc_Absyn_Kind)_tmp353->v,_tmp357)){{struct Cyc_Core_Opt*
_tmpFE9;*_tmp35C=((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9->v=t1,_tmpFE9))));}
return;}{const char*_tmpFEA;Cyc_Tcutil_failure_reason=((_tmpFEA="(kinds are incompatible)",
_tag_dyneither(_tmpFEA,sizeof(char),25)));}goto _LL20E;}_LL211:{struct Cyc_Absyn_PointerType_struct*
_tmp35E=(struct Cyc_Absyn_PointerType_struct*)_tmp359;if(_tmp35E->tag != 5)goto
_LL213;else{_tmp35F=_tmp35E->f1;}}if(!((enum Cyc_Absyn_Kind)_tmp353->v == Cyc_Absyn_BoxKind))
goto _LL213;_LL212: {union Cyc_Absyn_Constraint*_tmp364=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp35F.ptr_atts).bounds);{
union Cyc_Absyn_Constraint*_tmp365=_tmp364;union Cyc_Absyn_Constraint _tmp366;int
_tmp367;_LL216: _tmp366=*_tmp365;if((_tmp366.No_constr).tag != 3)goto _LL218;
_tmp367=(int)(_tmp366.No_constr).val;_LL217:{struct _union_Constraint_Eq_constr*
_tmpFEB;(_tmpFEB=& _tmp364->Eq_constr,((_tmpFEB->tag=1,_tmpFEB->val=Cyc_Absyn_bounds_one)));}{
struct Cyc_Core_Opt*_tmpFEC;*_tmp355=((_tmpFEC=_cycalloc(sizeof(*_tmpFEC)),((
_tmpFEC->v=t2,_tmpFEC))));}return;_LL218:;_LL219: goto _LL215;_LL215:;}goto _LL20E;}
_LL213:;_LL214: goto _LL20E;_LL20E:;}{const char*_tmpFED;Cyc_Tcutil_failure_reason=((
_tmpFED="(kinds are incompatible)",_tag_dyneither(_tmpFED,sizeof(char),25)));}(
int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL20C:;_LL20D: goto _LL209;_LL209:;}{
struct _tuple0 _tmpFEE;struct _tuple0 _tmp36C=(_tmpFEE.f1=t2,((_tmpFEE.f2=t1,_tmpFEE)));
void*_tmp36D;void*_tmp36F;void*_tmp371;void*_tmp373;struct Cyc_Absyn_Tvar*_tmp375;
void*_tmp376;struct Cyc_Absyn_Tvar*_tmp378;void*_tmp379;struct Cyc_Absyn_AggrInfo
_tmp37B;union Cyc_Absyn_AggrInfoU _tmp37C;struct Cyc_List_List*_tmp37D;void*_tmp37E;
struct Cyc_Absyn_AggrInfo _tmp380;union Cyc_Absyn_AggrInfoU _tmp381;struct Cyc_List_List*
_tmp382;void*_tmp383;struct _tuple2*_tmp385;void*_tmp386;struct _tuple2*_tmp388;
void*_tmp389;struct Cyc_List_List*_tmp38B;void*_tmp38C;struct Cyc_List_List*
_tmp38E;void*_tmp38F;struct Cyc_Absyn_DatatypeInfo _tmp391;union Cyc_Absyn_DatatypeInfoU
_tmp392;struct Cyc_Absyn_Datatypedecl**_tmp393;struct Cyc_Absyn_Datatypedecl*
_tmp394;struct Cyc_List_List*_tmp395;void*_tmp396;struct Cyc_Absyn_DatatypeInfo
_tmp398;union Cyc_Absyn_DatatypeInfoU _tmp399;struct Cyc_Absyn_Datatypedecl**
_tmp39A;struct Cyc_Absyn_Datatypedecl*_tmp39B;struct Cyc_List_List*_tmp39C;void*
_tmp39D;struct Cyc_Absyn_DatatypeFieldInfo _tmp39F;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3A0;struct _tuple3 _tmp3A1;struct Cyc_Absyn_Datatypedecl*_tmp3A2;struct Cyc_Absyn_Datatypefield*
_tmp3A3;struct Cyc_List_List*_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3A7;union Cyc_Absyn_DatatypeFieldInfoU _tmp3A8;struct _tuple3 _tmp3A9;struct Cyc_Absyn_Datatypedecl*
_tmp3AA;struct Cyc_Absyn_Datatypefield*_tmp3AB;struct Cyc_List_List*_tmp3AC;void*
_tmp3AD;struct Cyc_Absyn_PtrInfo _tmp3AF;void*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3B1;
struct Cyc_Absyn_PtrAtts _tmp3B2;void*_tmp3B3;union Cyc_Absyn_Constraint*_tmp3B4;
union Cyc_Absyn_Constraint*_tmp3B5;union Cyc_Absyn_Constraint*_tmp3B6;void*_tmp3B7;
struct Cyc_Absyn_PtrInfo _tmp3B9;void*_tmp3BA;struct Cyc_Absyn_Tqual _tmp3BB;struct
Cyc_Absyn_PtrAtts _tmp3BC;void*_tmp3BD;union Cyc_Absyn_Constraint*_tmp3BE;union Cyc_Absyn_Constraint*
_tmp3BF;union Cyc_Absyn_Constraint*_tmp3C0;void*_tmp3C1;enum Cyc_Absyn_Sign
_tmp3C3;enum Cyc_Absyn_Size_of _tmp3C4;void*_tmp3C5;enum Cyc_Absyn_Sign _tmp3C7;
enum Cyc_Absyn_Size_of _tmp3C8;void*_tmp3C9;void*_tmp3CB;void*_tmp3CD;int _tmp3CF;
void*_tmp3D0;int _tmp3D2;void*_tmp3D3;void*_tmp3D5;void*_tmp3D6;void*_tmp3D8;void*
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DB;void*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;
void*_tmp3DF;struct Cyc_Absyn_ArrayInfo _tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Tqual
_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;union Cyc_Absyn_Constraint*_tmp3E5;void*
_tmp3E6;struct Cyc_Absyn_ArrayInfo _tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tqual
_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;union Cyc_Absyn_Constraint*_tmp3EC;void*
_tmp3ED;struct Cyc_Absyn_FnInfo _tmp3EF;struct Cyc_List_List*_tmp3F0;struct Cyc_Core_Opt*
_tmp3F1;void*_tmp3F2;struct Cyc_List_List*_tmp3F3;int _tmp3F4;struct Cyc_Absyn_VarargInfo*
_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F7;void*_tmp3F8;
struct Cyc_Absyn_FnInfo _tmp3FA;struct Cyc_List_List*_tmp3FB;struct Cyc_Core_Opt*
_tmp3FC;void*_tmp3FD;struct Cyc_List_List*_tmp3FE;int _tmp3FF;struct Cyc_Absyn_VarargInfo*
_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;void*_tmp403;
struct Cyc_List_List*_tmp405;void*_tmp406;struct Cyc_List_List*_tmp408;void*
_tmp409;enum Cyc_Absyn_AggrKind _tmp40B;struct Cyc_List_List*_tmp40C;void*_tmp40D;
enum Cyc_Absyn_AggrKind _tmp40F;struct Cyc_List_List*_tmp410;void*_tmp411;void*
_tmp413;void*_tmp415;void*_tmp417;void*_tmp419;void*_tmp41B;void*_tmp41C;void*
_tmp41E;void*_tmp41F;void*_tmp421;void*_tmp422;void*_tmp423;void*_tmp425;void*
_tmp426;void*_tmp427;void*_tmp429;void*_tmp42B;void*_tmp42D;void*_tmp42F;void*
_tmp431;_LL21B: _tmp36D=_tmp36C.f1;{struct Cyc_Absyn_Evar_struct*_tmp36E=(struct
Cyc_Absyn_Evar_struct*)_tmp36D;if(_tmp36E->tag != 1)goto _LL21D;};_LL21C: Cyc_Tcutil_unify_it(
t2,t1);return;_LL21D: _tmp36F=_tmp36C.f1;{struct Cyc_Absyn_VoidType_struct*_tmp370=(
struct Cyc_Absyn_VoidType_struct*)_tmp36F;if(_tmp370->tag != 0)goto _LL21F;};
_tmp371=_tmp36C.f2;{struct Cyc_Absyn_VoidType_struct*_tmp372=(struct Cyc_Absyn_VoidType_struct*)
_tmp371;if(_tmp372->tag != 0)goto _LL21F;};_LL21E: return;_LL21F: _tmp373=_tmp36C.f1;{
struct Cyc_Absyn_VarType_struct*_tmp374=(struct Cyc_Absyn_VarType_struct*)_tmp373;
if(_tmp374->tag != 2)goto _LL221;else{_tmp375=_tmp374->f1;}};_tmp376=_tmp36C.f2;{
struct Cyc_Absyn_VarType_struct*_tmp377=(struct Cyc_Absyn_VarType_struct*)_tmp376;
if(_tmp377->tag != 2)goto _LL221;else{_tmp378=_tmp377->f1;}};_LL220: {struct
_dyneither_ptr*_tmp433=_tmp375->name;struct _dyneither_ptr*_tmp434=_tmp378->name;
int _tmp435=_tmp375->identity;int _tmp436=_tmp378->identity;if(_tmp436 == _tmp435)
return;{const char*_tmpFEF;Cyc_Tcutil_failure_reason=((_tmpFEF="(variable types are not the same)",
_tag_dyneither(_tmpFEF,sizeof(char),34)));}goto _LL21A;}_LL221: _tmp379=_tmp36C.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp37A=(struct Cyc_Absyn_AggrType_struct*)
_tmp379;if(_tmp37A->tag != 12)goto _LL223;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37B.aggr_info;
_tmp37D=_tmp37B.targs;}};_tmp37E=_tmp36C.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp37F=(struct Cyc_Absyn_AggrType_struct*)_tmp37E;if(_tmp37F->tag != 12)goto
_LL223;else{_tmp380=_tmp37F->f1;_tmp381=_tmp380.aggr_info;_tmp382=_tmp380.targs;}};
_LL222: {enum Cyc_Absyn_AggrKind _tmp439;struct _tuple2*_tmp43A;struct _tuple12
_tmp438=Cyc_Absyn_aggr_kinded_name(_tmp381);_tmp439=_tmp438.f1;_tmp43A=_tmp438.f2;{
enum Cyc_Absyn_AggrKind _tmp43C;struct _tuple2*_tmp43D;struct _tuple12 _tmp43B=Cyc_Absyn_aggr_kinded_name(
_tmp37C);_tmp43C=_tmp43B.f1;_tmp43D=_tmp43B.f2;if(_tmp439 != _tmp43C){{const char*
_tmpFF0;Cyc_Tcutil_failure_reason=((_tmpFF0="(struct and union type)",
_tag_dyneither(_tmpFF0,sizeof(char),24)));}goto _LL21A;}if(Cyc_Absyn_qvar_cmp(
_tmp43A,_tmp43D)!= 0){{const char*_tmpFF1;Cyc_Tcutil_failure_reason=((_tmpFF1="(different type name)",
_tag_dyneither(_tmpFF1,sizeof(char),22)));}goto _LL21A;}Cyc_Tcutil_unify_list(
_tmp382,_tmp37D);return;};}_LL223: _tmp383=_tmp36C.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp384=(struct Cyc_Absyn_EnumType_struct*)_tmp383;if(_tmp384->tag != 14)goto
_LL225;else{_tmp385=_tmp384->f1;}};_tmp386=_tmp36C.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp387=(struct Cyc_Absyn_EnumType_struct*)_tmp386;if(_tmp387->tag != 14)goto
_LL225;else{_tmp388=_tmp387->f1;}};_LL224: if(Cyc_Absyn_qvar_cmp(_tmp385,_tmp388)
== 0)return;{const char*_tmpFF2;Cyc_Tcutil_failure_reason=((_tmpFF2="(different enum types)",
_tag_dyneither(_tmpFF2,sizeof(char),23)));}goto _LL21A;_LL225: _tmp389=_tmp36C.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp38A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp389;if(_tmp38A->tag != 15)goto _LL227;else{_tmp38B=_tmp38A->f1;}};_tmp38C=
_tmp36C.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp38D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp38C;if(_tmp38D->tag != 15)goto _LL227;else{_tmp38E=_tmp38D->f1;}};_LL226: {int
bad=0;for(0;_tmp38B != 0  && _tmp38E != 0;(_tmp38B=_tmp38B->tl,_tmp38E=_tmp38E->tl)){
struct Cyc_Absyn_Enumfield*_tmp441=(struct Cyc_Absyn_Enumfield*)_tmp38B->hd;struct
Cyc_Absyn_Enumfield*_tmp442=(struct Cyc_Absyn_Enumfield*)_tmp38E->hd;if(Cyc_Absyn_qvar_cmp(
_tmp441->name,_tmp442->name)!= 0){{const char*_tmpFF3;Cyc_Tcutil_failure_reason=((
_tmpFF3="(different names for enum fields)",_tag_dyneither(_tmpFF3,sizeof(char),
34)));}bad=1;break;}if(_tmp441->tag == _tmp442->tag)continue;if(_tmp441->tag == 0
 || _tmp442->tag == 0){{const char*_tmpFF4;Cyc_Tcutil_failure_reason=((_tmpFF4="(different tag values for enum fields)",
_tag_dyneither(_tmpFF4,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp441->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp442->tag))){{const char*_tmpFF5;Cyc_Tcutil_failure_reason=((_tmpFF5="(different tag values for enum fields)",
_tag_dyneither(_tmpFF5,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21A;if(
_tmp38B == 0  && _tmp38E == 0)return;{const char*_tmpFF6;Cyc_Tcutil_failure_reason=((
_tmpFF6="(different number of fields for enums)",_tag_dyneither(_tmpFF6,sizeof(
char),39)));}goto _LL21A;}_LL227: _tmp38F=_tmp36C.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp390=(struct Cyc_Absyn_DatatypeType_struct*)_tmp38F;if(_tmp390->tag != 3)goto
_LL229;else{_tmp391=_tmp390->f1;_tmp392=_tmp391.datatype_info;if((_tmp392.KnownDatatype).tag
!= 2)goto _LL229;_tmp393=(struct Cyc_Absyn_Datatypedecl**)(_tmp392.KnownDatatype).val;
_tmp394=*_tmp393;_tmp395=_tmp391.targs;}};_tmp396=_tmp36C.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp397=(struct Cyc_Absyn_DatatypeType_struct*)_tmp396;if(_tmp397->tag != 3)goto
_LL229;else{_tmp398=_tmp397->f1;_tmp399=_tmp398.datatype_info;if((_tmp399.KnownDatatype).tag
!= 2)goto _LL229;_tmp39A=(struct Cyc_Absyn_Datatypedecl**)(_tmp399.KnownDatatype).val;
_tmp39B=*_tmp39A;_tmp39C=_tmp398.targs;}};_LL228: if(_tmp394 == _tmp39B  || Cyc_Absyn_qvar_cmp(
_tmp394->name,_tmp39B->name)== 0){Cyc_Tcutil_unify_list(_tmp39C,_tmp395);return;}
goto _LL21A;_LL229: _tmp39D=_tmp36C.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp39E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp39D;if(_tmp39E->tag != 4)
goto _LL22B;else{_tmp39F=_tmp39E->f1;_tmp3A0=_tmp39F.field_info;if((_tmp3A0.KnownDatatypefield).tag
!= 2)goto _LL22B;_tmp3A1=(struct _tuple3)(_tmp3A0.KnownDatatypefield).val;_tmp3A2=
_tmp3A1.f1;_tmp3A3=_tmp3A1.f2;_tmp3A4=_tmp39F.targs;}};_tmp3A5=_tmp36C.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp3A6=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3A5;if(_tmp3A6->tag != 4)goto _LL22B;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A7.field_info;
if((_tmp3A8.KnownDatatypefield).tag != 2)goto _LL22B;_tmp3A9=(struct _tuple3)(
_tmp3A8.KnownDatatypefield).val;_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;_tmp3AC=
_tmp3A7.targs;}};_LL22A: if((_tmp3A2 == _tmp3AA  || Cyc_Absyn_qvar_cmp(_tmp3A2->name,
_tmp3AA->name)== 0) && (_tmp3A3 == _tmp3AB  || Cyc_Absyn_qvar_cmp(_tmp3A3->name,
_tmp3AB->name)== 0)){Cyc_Tcutil_unify_list(_tmp3AC,_tmp3A4);return;}{const char*
_tmpFF7;Cyc_Tcutil_failure_reason=((_tmpFF7="(different datatype field types)",
_tag_dyneither(_tmpFF7,sizeof(char),33)));}goto _LL21A;_LL22B: _tmp3AD=_tmp36C.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp3AE=(struct Cyc_Absyn_PointerType_struct*)
_tmp3AD;if(_tmp3AE->tag != 5)goto _LL22D;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AF.elt_typ;
_tmp3B1=_tmp3AF.elt_tq;_tmp3B2=_tmp3AF.ptr_atts;_tmp3B3=_tmp3B2.rgn;_tmp3B4=
_tmp3B2.nullable;_tmp3B5=_tmp3B2.bounds;_tmp3B6=_tmp3B2.zero_term;}};_tmp3B7=
_tmp36C.f2;{struct Cyc_Absyn_PointerType_struct*_tmp3B8=(struct Cyc_Absyn_PointerType_struct*)
_tmp3B7;if(_tmp3B8->tag != 5)goto _LL22D;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B9.elt_typ;
_tmp3BB=_tmp3B9.elt_tq;_tmp3BC=_tmp3B9.ptr_atts;_tmp3BD=_tmp3BC.rgn;_tmp3BE=
_tmp3BC.nullable;_tmp3BF=_tmp3BC.bounds;_tmp3C0=_tmp3BC.zero_term;}};_LL22C: Cyc_Tcutil_unify_it(
_tmp3BA,_tmp3B0);Cyc_Tcutil_unify_it(_tmp3B3,_tmp3BD);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmpFF8;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3C0,_tmp3B6,((_tmpFF8="(not both zero terminated)",
_tag_dyneither(_tmpFF8,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp3BB,
_tmp3BA,_tmp3B1,_tmp3B0);{const char*_tmpFF9;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3BF,_tmp3B5,((
_tmpFF9="(different pointer bounds)",_tag_dyneither(_tmpFF9,sizeof(char),27))));}{
void*_tmp44A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp3BF);_LL256: {struct Cyc_Absyn_DynEither_b_struct*
_tmp44B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp44A;if(_tmp44B->tag != 0)goto
_LL258;}_LL257: return;_LL258:;_LL259: goto _LL255;_LL255:;}{const char*_tmpFFA;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3BE,_tmp3B4,((_tmpFFA="(incompatible pointer types)",_tag_dyneither(_tmpFFA,
sizeof(char),29))));}return;_LL22D: _tmp3C1=_tmp36C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp3C2=(struct Cyc_Absyn_IntType_struct*)_tmp3C1;if(_tmp3C2->tag != 6)goto _LL22F;
else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}};_tmp3C5=_tmp36C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp3C6=(struct Cyc_Absyn_IntType_struct*)_tmp3C5;if(_tmp3C6->tag != 6)goto _LL22F;
else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}};_LL22E: if(_tmp3C7 == _tmp3C3  && ((
_tmp3C8 == _tmp3C4  || _tmp3C8 == Cyc_Absyn_Int_sz  && _tmp3C4 == Cyc_Absyn_Long_sz)
 || _tmp3C8 == Cyc_Absyn_Long_sz  && _tmp3C4 == Cyc_Absyn_Int_sz))return;{const char*
_tmpFFB;Cyc_Tcutil_failure_reason=((_tmpFFB="(different integral types)",
_tag_dyneither(_tmpFFB,sizeof(char),27)));}goto _LL21A;_LL22F: _tmp3C9=_tmp36C.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp3CA=(struct Cyc_Absyn_FloatType_struct*)
_tmp3C9;if(_tmp3CA->tag != 7)goto _LL231;};_tmp3CB=_tmp36C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp3CC=(struct Cyc_Absyn_FloatType_struct*)_tmp3CB;if(_tmp3CC->tag != 7)goto
_LL231;};_LL230: return;_LL231: _tmp3CD=_tmp36C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp3CE=(struct Cyc_Absyn_DoubleType_struct*)_tmp3CD;if(_tmp3CE->tag != 8)goto
_LL233;else{_tmp3CF=_tmp3CE->f1;}};_tmp3D0=_tmp36C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp3D1=(struct Cyc_Absyn_DoubleType_struct*)_tmp3D0;if(_tmp3D1->tag != 8)goto
_LL233;else{_tmp3D2=_tmp3D1->f1;}};_LL232: if(_tmp3CF == _tmp3D2)return;goto _LL21A;
_LL233: _tmp3D3=_tmp36C.f1;{struct Cyc_Absyn_TagType_struct*_tmp3D4=(struct Cyc_Absyn_TagType_struct*)
_tmp3D3;if(_tmp3D4->tag != 20)goto _LL235;else{_tmp3D5=(void*)_tmp3D4->f1;}};
_tmp3D6=_tmp36C.f2;{struct Cyc_Absyn_TagType_struct*_tmp3D7=(struct Cyc_Absyn_TagType_struct*)
_tmp3D6;if(_tmp3D7->tag != 20)goto _LL235;else{_tmp3D8=(void*)_tmp3D7->f1;}};
_LL234: Cyc_Tcutil_unify_it(_tmp3D5,_tmp3D8);return;_LL235: _tmp3D9=_tmp36C.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp3DA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3D9;if(_tmp3DA->tag != 19)goto _LL237;else{_tmp3DB=_tmp3DA->f1;}};_tmp3DC=
_tmp36C.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp3DD=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3DC;if(_tmp3DD->tag != 19)goto _LL237;else{_tmp3DE=_tmp3DD->f1;}};_LL236: if(!
Cyc_Evexp_same_const_exp(_tmp3DB,_tmp3DE)){{const char*_tmpFFC;Cyc_Tcutil_failure_reason=((
_tmpFFC="(cannot prove expressions are the same)",_tag_dyneither(_tmpFFC,sizeof(
char),40)));}goto _LL21A;}return;_LL237: _tmp3DF=_tmp36C.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp3E0=(struct Cyc_Absyn_ArrayType_struct*)_tmp3DF;if(_tmp3E0->tag != 9)goto
_LL239;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E1.elt_type;_tmp3E3=_tmp3E1.tq;
_tmp3E4=_tmp3E1.num_elts;_tmp3E5=_tmp3E1.zero_term;}};_tmp3E6=_tmp36C.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp3E7=(struct Cyc_Absyn_ArrayType_struct*)_tmp3E6;
if(_tmp3E7->tag != 9)goto _LL239;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E8.elt_type;
_tmp3EA=_tmp3E8.tq;_tmp3EB=_tmp3E8.num_elts;_tmp3EC=_tmp3E8.zero_term;}};_LL238:
Cyc_Tcutil_unify_it(_tmp3E9,_tmp3E2);Cyc_Tcutil_unify_tqual(_tmp3EA,_tmp3E9,
_tmp3E3,_tmp3E2);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmpFFD;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3E5,_tmp3EC,((_tmpFFD="(not both zero terminated)",_tag_dyneither(_tmpFFD,
sizeof(char),27))));}if(_tmp3E4 == _tmp3EB)return;if(_tmp3E4 == 0  || _tmp3EB == 0)
goto _LL21A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp3E4,(struct Cyc_Absyn_Exp*)
_tmp3EB))return;{const char*_tmpFFE;Cyc_Tcutil_failure_reason=((_tmpFFE="(different array sizes)",
_tag_dyneither(_tmpFFE,sizeof(char),24)));}goto _LL21A;_LL239: _tmp3ED=_tmp36C.f1;{
struct Cyc_Absyn_FnType_struct*_tmp3EE=(struct Cyc_Absyn_FnType_struct*)_tmp3ED;
if(_tmp3EE->tag != 10)goto _LL23B;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EF.tvars;
_tmp3F1=_tmp3EF.effect;_tmp3F2=_tmp3EF.ret_typ;_tmp3F3=_tmp3EF.args;_tmp3F4=
_tmp3EF.c_varargs;_tmp3F5=_tmp3EF.cyc_varargs;_tmp3F6=_tmp3EF.rgn_po;_tmp3F7=
_tmp3EF.attributes;}};_tmp3F8=_tmp36C.f2;{struct Cyc_Absyn_FnType_struct*_tmp3F9=(
struct Cyc_Absyn_FnType_struct*)_tmp3F8;if(_tmp3F9->tag != 10)goto _LL23B;else{
_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3FA.tvars;_tmp3FC=_tmp3FA.effect;_tmp3FD=_tmp3FA.ret_typ;
_tmp3FE=_tmp3FA.args;_tmp3FF=_tmp3FA.c_varargs;_tmp400=_tmp3FA.cyc_varargs;
_tmp401=_tmp3FA.rgn_po;_tmp402=_tmp3FA.attributes;}};_LL23A: {int done=0;struct
_RegionHandle _tmp451=_new_region("rgn");struct _RegionHandle*rgn=& _tmp451;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp3FB != 0){if(_tmp3F0 == 0){{
const char*_tmpFFF;Cyc_Tcutil_failure_reason=((_tmpFFF="(second function type has too few type variables)",
_tag_dyneither(_tmpFFF,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp453=((struct Cyc_Absyn_Tvar*)_tmp3FB->hd)->kind;void*_tmp454=((struct Cyc_Absyn_Tvar*)
_tmp3F0->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp453,_tmp454)){{const char*
_tmp1005;void*_tmp1004[3];struct Cyc_String_pa_struct _tmp1003;struct Cyc_String_pa_struct
_tmp1002;struct Cyc_String_pa_struct _tmp1001;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1001.tag=0,((_tmp1001.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp3F0->hd,Cyc_Absyn_BoxKind))),((_tmp1002.tag=0,((_tmp1002.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp3FB->hd,Cyc_Absyn_BoxKind))),((_tmp1003.tag=0,((
_tmp1003.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp3FB->hd)),((_tmp1004[0]=& _tmp1003,((_tmp1004[1]=&
_tmp1002,((_tmp1004[2]=& _tmp1001,Cyc_aprintf(((_tmp1005="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1005,sizeof(char),44))),_tag_dyneither(_tmp1004,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple14*_tmp100F;struct Cyc_Absyn_VarType_struct _tmp100E;struct Cyc_Absyn_VarType_struct*
_tmp100D;struct Cyc_List_List*_tmp100C;inst=((_tmp100C=_region_malloc(rgn,sizeof(*
_tmp100C)),((_tmp100C->hd=((_tmp100F=_region_malloc(rgn,sizeof(*_tmp100F)),((
_tmp100F->f1=(struct Cyc_Absyn_Tvar*)_tmp3F0->hd,((_tmp100F->f2=(void*)((_tmp100D=
_cycalloc(sizeof(*_tmp100D)),((_tmp100D[0]=((_tmp100E.tag=2,((_tmp100E.f1=(
struct Cyc_Absyn_Tvar*)_tmp3FB->hd,_tmp100E)))),_tmp100D)))),_tmp100F)))))),((
_tmp100C->tl=inst,_tmp100C))))));}_tmp3FB=_tmp3FB->tl;_tmp3F0=_tmp3F0->tl;};}if(
_tmp3F0 != 0){{const char*_tmp1010;Cyc_Tcutil_failure_reason=((_tmp1010="(second function type has too many type variables)",
_tag_dyneither(_tmp1010,sizeof(char),51)));}_npop_handler(0);goto _LL21A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp101C;struct Cyc_Absyn_FnInfo _tmp101B;
struct Cyc_Absyn_FnType_struct*_tmp101A;struct Cyc_Absyn_FnType_struct _tmp1016;
struct Cyc_Absyn_FnInfo _tmp1015;struct Cyc_Absyn_FnType_struct*_tmp1014;Cyc_Tcutil_unify_it((
void*)((_tmp1014=_cycalloc(sizeof(*_tmp1014)),((_tmp1014[0]=((_tmp1016.tag=10,((
_tmp1016.f1=((_tmp1015.tvars=0,((_tmp1015.effect=_tmp3FC,((_tmp1015.ret_typ=
_tmp3FD,((_tmp1015.args=_tmp3FE,((_tmp1015.c_varargs=_tmp3FF,((_tmp1015.cyc_varargs=
_tmp400,((_tmp1015.rgn_po=_tmp401,((_tmp1015.attributes=_tmp402,_tmp1015)))))))))))))))),
_tmp1016)))),_tmp1014)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp101A=
_cycalloc(sizeof(*_tmp101A)),((_tmp101A[0]=((_tmp101C.tag=10,((_tmp101C.f1=((
_tmp101B.tvars=0,((_tmp101B.effect=_tmp3F1,((_tmp101B.ret_typ=_tmp3F2,((_tmp101B.args=
_tmp3F3,((_tmp101B.c_varargs=_tmp3F4,((_tmp101B.cyc_varargs=_tmp3F5,((_tmp101B.rgn_po=
_tmp3F6,((_tmp101B.attributes=_tmp3F7,_tmp101B)))))))))))))))),_tmp101C)))),
_tmp101A))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp3FD,_tmp3F2);for(0;_tmp3FE != 0  && _tmp3F3 != 0;(_tmp3FE=_tmp3FE->tl,_tmp3F3=
_tmp3F3->tl)){struct Cyc_Absyn_Tqual _tmp466;void*_tmp467;struct _tuple9 _tmp465=*((
struct _tuple9*)_tmp3FE->hd);_tmp466=_tmp465.f2;_tmp467=_tmp465.f3;{struct Cyc_Absyn_Tqual
_tmp469;void*_tmp46A;struct _tuple9 _tmp468=*((struct _tuple9*)_tmp3F3->hd);_tmp469=
_tmp468.f2;_tmp46A=_tmp468.f3;Cyc_Tcutil_unify_it(_tmp467,_tmp46A);Cyc_Tcutil_unify_tqual(
_tmp466,_tmp467,_tmp469,_tmp46A);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp3FE != 0  || _tmp3F3 != 0){{const char*_tmp101D;Cyc_Tcutil_failure_reason=((
_tmp101D="(function types have different number of arguments)",_tag_dyneither(
_tmp101D,sizeof(char),52)));}_npop_handler(0);goto _LL21A;}if(_tmp3FF != _tmp3F4){{
const char*_tmp101E;Cyc_Tcutil_failure_reason=((_tmp101E="(only one function type takes C varargs)",
_tag_dyneither(_tmp101E,sizeof(char),41)));}_npop_handler(0);goto _LL21A;}{int
bad_cyc_vararg=0;{struct _tuple18 _tmp101F;struct _tuple18 _tmp46E=(_tmp101F.f1=
_tmp400,((_tmp101F.f2=_tmp3F5,_tmp101F)));struct Cyc_Absyn_VarargInfo*_tmp46F;
struct Cyc_Absyn_VarargInfo*_tmp470;struct Cyc_Absyn_VarargInfo*_tmp471;struct Cyc_Absyn_VarargInfo*
_tmp472;struct Cyc_Absyn_VarargInfo*_tmp473;struct Cyc_Absyn_VarargInfo _tmp474;
struct Cyc_Core_Opt*_tmp475;struct Cyc_Absyn_Tqual _tmp476;void*_tmp477;int _tmp478;
struct Cyc_Absyn_VarargInfo*_tmp479;struct Cyc_Absyn_VarargInfo _tmp47A;struct Cyc_Core_Opt*
_tmp47B;struct Cyc_Absyn_Tqual _tmp47C;void*_tmp47D;int _tmp47E;_LL25B: _tmp46F=
_tmp46E.f1;if(_tmp46F != 0)goto _LL25D;_tmp470=_tmp46E.f2;if(_tmp470 != 0)goto
_LL25D;_LL25C: goto _LL25A;_LL25D: _tmp471=_tmp46E.f1;if(_tmp471 != 0)goto _LL25F;
_LL25E: goto _LL260;_LL25F: _tmp472=_tmp46E.f2;if(_tmp472 != 0)goto _LL261;_LL260:
bad_cyc_vararg=1;{const char*_tmp1020;Cyc_Tcutil_failure_reason=((_tmp1020="(only one function type takes varargs)",
_tag_dyneither(_tmp1020,sizeof(char),39)));}goto _LL25A;_LL261: _tmp473=_tmp46E.f1;
if(_tmp473 == 0)goto _LL25A;_tmp474=*_tmp473;_tmp475=_tmp474.name;_tmp476=_tmp474.tq;
_tmp477=_tmp474.type;_tmp478=_tmp474.inject;_tmp479=_tmp46E.f2;if(_tmp479 == 0)
goto _LL25A;_tmp47A=*_tmp479;_tmp47B=_tmp47A.name;_tmp47C=_tmp47A.tq;_tmp47D=
_tmp47A.type;_tmp47E=_tmp47A.inject;_LL262: Cyc_Tcutil_unify_it(_tmp477,_tmp47D);
Cyc_Tcutil_unify_tqual(_tmp476,_tmp477,_tmp47C,_tmp47D);if(_tmp478 != _tmp47E){
bad_cyc_vararg=1;{const char*_tmp1021;Cyc_Tcutil_failure_reason=((_tmp1021="(only one function type injects varargs)",
_tag_dyneither(_tmp1021,sizeof(char),41)));};}goto _LL25A;_LL25A:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21A;}{int bad_effect=0;{struct _tuple19
_tmp1022;struct _tuple19 _tmp482=(_tmp1022.f1=_tmp3FC,((_tmp1022.f2=_tmp3F1,
_tmp1022)));struct Cyc_Core_Opt*_tmp483;struct Cyc_Core_Opt*_tmp484;struct Cyc_Core_Opt*
_tmp485;struct Cyc_Core_Opt*_tmp486;_LL264: _tmp483=_tmp482.f1;if(_tmp483 != 0)goto
_LL266;_tmp484=_tmp482.f2;if(_tmp484 != 0)goto _LL266;_LL265: goto _LL263;_LL266:
_tmp485=_tmp482.f1;if(_tmp485 != 0)goto _LL268;_LL267: goto _LL269;_LL268: _tmp486=
_tmp482.f2;if(_tmp486 != 0)goto _LL26A;_LL269: bad_effect=1;goto _LL263;_LL26A:;
_LL26B: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FC))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F1))->v);
goto _LL263;_LL263:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1023;Cyc_Tcutil_failure_reason=((_tmp1023="(function type effects do not unify)",
_tag_dyneither(_tmp1023,sizeof(char),37)));}_npop_handler(0);goto _LL21A;}if(!Cyc_Tcutil_same_atts(
_tmp3F7,_tmp402)){{const char*_tmp1024;Cyc_Tcutil_failure_reason=((_tmp1024="(function types have different attributes)",
_tag_dyneither(_tmp1024,sizeof(char),43)));}_npop_handler(0);goto _LL21A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp3F6,_tmp401)){{const char*_tmp1025;Cyc_Tcutil_failure_reason=((_tmp1025="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1025,sizeof(char),58)));}_npop_handler(0);goto _LL21A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL23B: _tmp403=_tmp36C.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp404=(struct Cyc_Absyn_TupleType_struct*)_tmp403;
if(_tmp404->tag != 11)goto _LL23D;else{_tmp405=_tmp404->f1;}};_tmp406=_tmp36C.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp407=(struct Cyc_Absyn_TupleType_struct*)
_tmp406;if(_tmp407->tag != 11)goto _LL23D;else{_tmp408=_tmp407->f1;}};_LL23C: for(0;
_tmp408 != 0  && _tmp405 != 0;(_tmp408=_tmp408->tl,_tmp405=_tmp405->tl)){struct Cyc_Absyn_Tqual
_tmp48B;void*_tmp48C;struct _tuple11 _tmp48A=*((struct _tuple11*)_tmp408->hd);
_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;{struct Cyc_Absyn_Tqual _tmp48E;void*_tmp48F;
struct _tuple11 _tmp48D=*((struct _tuple11*)_tmp405->hd);_tmp48E=_tmp48D.f1;_tmp48F=
_tmp48D.f2;Cyc_Tcutil_unify_it(_tmp48C,_tmp48F);Cyc_Tcutil_unify_tqual(_tmp48B,
_tmp48C,_tmp48E,_tmp48F);};}if(_tmp408 == 0  && _tmp405 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1026;Cyc_Tcutil_failure_reason=((
_tmp1026="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1026,sizeof(char),51)));}goto _LL21A;_LL23D: _tmp409=_tmp36C.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp40A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp409;if(_tmp40A->tag != 13)goto
_LL23F;else{_tmp40B=_tmp40A->f1;_tmp40C=_tmp40A->f2;}};_tmp40D=_tmp36C.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp40E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp40D;if(_tmp40E->tag != 13)goto _LL23F;else{_tmp40F=_tmp40E->f1;_tmp410=_tmp40E->f2;}};
_LL23E: if(_tmp40F != _tmp40B){{const char*_tmp1027;Cyc_Tcutil_failure_reason=((
_tmp1027="(struct and union type)",_tag_dyneither(_tmp1027,sizeof(char),24)));}
goto _LL21A;}for(0;_tmp410 != 0  && _tmp40C != 0;(_tmp410=_tmp410->tl,_tmp40C=
_tmp40C->tl)){struct Cyc_Absyn_Aggrfield*_tmp492=(struct Cyc_Absyn_Aggrfield*)
_tmp410->hd;struct Cyc_Absyn_Aggrfield*_tmp493=(struct Cyc_Absyn_Aggrfield*)
_tmp40C->hd;if(Cyc_strptrcmp(_tmp492->name,_tmp493->name)!= 0){{const char*
_tmp1028;Cyc_Tcutil_failure_reason=((_tmp1028="(different member names)",
_tag_dyneither(_tmp1028,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp492->type,_tmp493->type);Cyc_Tcutil_unify_tqual(_tmp492->tq,
_tmp492->type,_tmp493->tq,_tmp493->type);if(!Cyc_Tcutil_same_atts(_tmp492->attributes,
_tmp493->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp1029;Cyc_Tcutil_failure_reason=((_tmp1029="(different attributes on member)",
_tag_dyneither(_tmp1029,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp492->width != 0  && _tmp493->width == 0  || _tmp493->width != 0  && _tmp492->width
== 0) || (_tmp492->width != 0  && _tmp493->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp492->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp493->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp102A;Cyc_Tcutil_failure_reason=((_tmp102A="(different bitfield widths on member)",
_tag_dyneither(_tmp102A,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp410 == 0  && _tmp40C == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp102B;Cyc_Tcutil_failure_reason=((_tmp102B="(different number of members)",
_tag_dyneither(_tmp102B,sizeof(char),30)));}goto _LL21A;_LL23F: _tmp411=_tmp36C.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp412=(struct Cyc_Absyn_HeapRgn_struct*)_tmp411;
if(_tmp412->tag != 21)goto _LL241;};_tmp413=_tmp36C.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp414=(struct Cyc_Absyn_HeapRgn_struct*)_tmp413;if(_tmp414->tag != 21)goto _LL241;};
_LL240: return;_LL241: _tmp415=_tmp36C.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp416=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp415;if(_tmp416->tag != 22)goto
_LL243;};_tmp417=_tmp36C.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp418=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp417;if(_tmp418->tag != 22)goto _LL243;};_LL242: return;_LL243: _tmp419=_tmp36C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp41A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp419;if(_tmp41A->tag != 16)goto _LL245;else{_tmp41B=(void*)_tmp41A->f1;}};
_tmp41C=_tmp36C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp41D=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41C;if(_tmp41D->tag != 16)goto _LL245;else{_tmp41E=(void*)_tmp41D->f1;}};
_LL244: Cyc_Tcutil_unify_it(_tmp41B,_tmp41E);return;_LL245: _tmp41F=_tmp36C.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp420=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp41F;if(_tmp420->tag != 17)goto _LL247;else{_tmp421=(void*)_tmp420->f1;_tmp422=(
void*)_tmp420->f2;}};_tmp423=_tmp36C.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp424=(struct Cyc_Absyn_DynRgnType_struct*)_tmp423;if(_tmp424->tag != 17)goto
_LL247;else{_tmp425=(void*)_tmp424->f1;_tmp426=(void*)_tmp424->f2;}};_LL246: Cyc_Tcutil_unify_it(
_tmp421,_tmp425);Cyc_Tcutil_unify_it(_tmp422,_tmp426);return;_LL247: _tmp427=
_tmp36C.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp428=(struct Cyc_Absyn_JoinEff_struct*)
_tmp427;if(_tmp428->tag != 24)goto _LL249;};_LL248: goto _LL24A;_LL249: _tmp429=
_tmp36C.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp42A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp429;if(_tmp42A->tag != 24)goto _LL24B;};_LL24A: goto _LL24C;_LL24B: _tmp42B=
_tmp36C.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp42C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp42B;if(_tmp42C->tag != 23)goto _LL24D;};_LL24C: goto _LL24E;_LL24D: _tmp42D=
_tmp36C.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp42E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp42D;if(_tmp42E->tag != 25)goto _LL24F;};_LL24E: goto _LL250;_LL24F: _tmp42F=
_tmp36C.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp430=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp42F;if(_tmp430->tag != 25)goto _LL251;};_LL250: goto _LL252;_LL251: _tmp431=
_tmp36C.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp432=(struct Cyc_Absyn_AccessEff_struct*)
_tmp431;if(_tmp432->tag != 23)goto _LL253;};_LL252: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp102C;Cyc_Tcutil_failure_reason=((_tmp102C="(effects don't unify)",
_tag_dyneither(_tmp102C,sizeof(char),22)));}goto _LL21A;_LL253:;_LL254: goto _LL21A;
_LL21A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp499=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp49A=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp499,_tmp49A);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp49B=x;union Cyc_Absyn_Constraint _tmp49C;int _tmp49D;
union Cyc_Absyn_Constraint _tmp49E;void*_tmp49F;union Cyc_Absyn_Constraint _tmp4A0;
union Cyc_Absyn_Constraint*_tmp4A1;_LL26D: _tmp49C=*_tmp49B;if((_tmp49C.No_constr).tag
!= 3)goto _LL26F;_tmp49D=(int)(_tmp49C.No_constr).val;_LL26E: return - 1;_LL26F:
_tmp49E=*_tmp49B;if((_tmp49E.Eq_constr).tag != 1)goto _LL271;_tmp49F=(void*)(
_tmp49E.Eq_constr).val;_LL270: {union Cyc_Absyn_Constraint*_tmp4A2=y;union Cyc_Absyn_Constraint
_tmp4A3;int _tmp4A4;union Cyc_Absyn_Constraint _tmp4A5;void*_tmp4A6;union Cyc_Absyn_Constraint
_tmp4A7;union Cyc_Absyn_Constraint*_tmp4A8;_LL274: _tmp4A3=*_tmp4A2;if((_tmp4A3.No_constr).tag
!= 3)goto _LL276;_tmp4A4=(int)(_tmp4A3.No_constr).val;_LL275: return 1;_LL276:
_tmp4A5=*_tmp4A2;if((_tmp4A5.Eq_constr).tag != 1)goto _LL278;_tmp4A6=(void*)(
_tmp4A5.Eq_constr).val;_LL277: return cmp(_tmp49F,_tmp4A6);_LL278: _tmp4A7=*_tmp4A2;
if((_tmp4A7.Forward_constr).tag != 2)goto _LL273;_tmp4A8=(union Cyc_Absyn_Constraint*)(
_tmp4A7.Forward_constr).val;_LL279: {const char*_tmp102F;void*_tmp102E;(_tmp102E=
0,Cyc_Tcutil_impos(((_tmp102F="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp102F,sizeof(char),40))),_tag_dyneither(_tmp102E,sizeof(void*),
0)));}_LL273:;}_LL271: _tmp4A0=*_tmp49B;if((_tmp4A0.Forward_constr).tag != 2)goto
_LL26C;_tmp4A1=(union Cyc_Absyn_Constraint*)(_tmp4A0.Forward_constr).val;_LL272: {
const char*_tmp1032;void*_tmp1031;(_tmp1031=0,Cyc_Tcutil_impos(((_tmp1032="unify_conref: forward after compress",
_tag_dyneither(_tmp1032,sizeof(char),37))),_tag_dyneither(_tmp1031,sizeof(void*),
0)));}_LL26C:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4B0;
struct _tuple11*_tmp4AD=tqt1;_tmp4AE=*_tmp4AD;_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;{
struct _tuple11 _tmp4B2;struct Cyc_Absyn_Tqual _tmp4B3;void*_tmp4B4;struct _tuple11*
_tmp4B1=tqt2;_tmp4B2=*_tmp4B1;_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{int _tmp4B5=
Cyc_Tcutil_tqual_cmp(_tmp4AF,_tmp4B3);if(_tmp4B5 != 0)return _tmp4B5;return Cyc_Tcutil_typecmp(
_tmp4B0,_tmp4B4);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp4B6=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp4B6 != 0)return _tmp4B6;{int _tmp4B7=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp4B7 != 0)return _tmp4B7;{int _tmp4B8=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp4B8 != 0)return _tmp4B8;{int _tmp4B9=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp4B9 != 0)return _tmp4B9;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp4BA=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp4BA != 0)return _tmp4BA;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp4BB=t;_LL27B: {
struct Cyc_Absyn_VoidType_struct*_tmp4BC=(struct Cyc_Absyn_VoidType_struct*)
_tmp4BB;if(_tmp4BC->tag != 0)goto _LL27D;}_LL27C: return 0;_LL27D: {struct Cyc_Absyn_Evar_struct*
_tmp4BD=(struct Cyc_Absyn_Evar_struct*)_tmp4BB;if(_tmp4BD->tag != 1)goto _LL27F;}
_LL27E: return 1;_LL27F: {struct Cyc_Absyn_VarType_struct*_tmp4BE=(struct Cyc_Absyn_VarType_struct*)
_tmp4BB;if(_tmp4BE->tag != 2)goto _LL281;}_LL280: return 2;_LL281: {struct Cyc_Absyn_DatatypeType_struct*
_tmp4BF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4BB;if(_tmp4BF->tag != 3)goto
_LL283;}_LL282: return 3;_LL283: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4C0=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4BB;if(_tmp4C0->tag != 4)goto _LL285;}
_LL284: return 4;_LL285: {struct Cyc_Absyn_PointerType_struct*_tmp4C1=(struct Cyc_Absyn_PointerType_struct*)
_tmp4BB;if(_tmp4C1->tag != 5)goto _LL287;}_LL286: return 5;_LL287: {struct Cyc_Absyn_IntType_struct*
_tmp4C2=(struct Cyc_Absyn_IntType_struct*)_tmp4BB;if(_tmp4C2->tag != 6)goto _LL289;}
_LL288: return 6;_LL289: {struct Cyc_Absyn_FloatType_struct*_tmp4C3=(struct Cyc_Absyn_FloatType_struct*)
_tmp4BB;if(_tmp4C3->tag != 7)goto _LL28B;}_LL28A: return 7;_LL28B: {struct Cyc_Absyn_DoubleType_struct*
_tmp4C4=(struct Cyc_Absyn_DoubleType_struct*)_tmp4BB;if(_tmp4C4->tag != 8)goto
_LL28D;}_LL28C: return 8;_LL28D: {struct Cyc_Absyn_ArrayType_struct*_tmp4C5=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4BB;if(_tmp4C5->tag != 9)goto _LL28F;}_LL28E:
return 9;_LL28F: {struct Cyc_Absyn_FnType_struct*_tmp4C6=(struct Cyc_Absyn_FnType_struct*)
_tmp4BB;if(_tmp4C6->tag != 10)goto _LL291;}_LL290: return 10;_LL291: {struct Cyc_Absyn_TupleType_struct*
_tmp4C7=(struct Cyc_Absyn_TupleType_struct*)_tmp4BB;if(_tmp4C7->tag != 11)goto
_LL293;}_LL292: return 11;_LL293: {struct Cyc_Absyn_AggrType_struct*_tmp4C8=(struct
Cyc_Absyn_AggrType_struct*)_tmp4BB;if(_tmp4C8->tag != 12)goto _LL295;}_LL294:
return 12;_LL295: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4C9=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4BB;if(_tmp4C9->tag != 13)goto _LL297;}_LL296: return 14;_LL297: {struct Cyc_Absyn_EnumType_struct*
_tmp4CA=(struct Cyc_Absyn_EnumType_struct*)_tmp4BB;if(_tmp4CA->tag != 14)goto
_LL299;}_LL298: return 16;_LL299: {struct Cyc_Absyn_AnonEnumType_struct*_tmp4CB=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp4BB;if(_tmp4CB->tag != 15)goto _LL29B;}
_LL29A: return 17;_LL29B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4CC=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4BB;if(_tmp4CC->tag != 16)goto _LL29D;}_LL29C: return 18;_LL29D: {struct Cyc_Absyn_TypedefType_struct*
_tmp4CD=(struct Cyc_Absyn_TypedefType_struct*)_tmp4BB;if(_tmp4CD->tag != 18)goto
_LL29F;}_LL29E: return 19;_LL29F: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4CE=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp4BB;if(_tmp4CE->tag != 22)goto _LL2A1;}
_LL2A0: return 20;_LL2A1: {struct Cyc_Absyn_HeapRgn_struct*_tmp4CF=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp4BB;if(_tmp4CF->tag != 21)goto _LL2A3;}_LL2A2: return 21;_LL2A3: {struct Cyc_Absyn_AccessEff_struct*
_tmp4D0=(struct Cyc_Absyn_AccessEff_struct*)_tmp4BB;if(_tmp4D0->tag != 23)goto
_LL2A5;}_LL2A4: return 22;_LL2A5: {struct Cyc_Absyn_JoinEff_struct*_tmp4D1=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4BB;if(_tmp4D1->tag != 24)goto _LL2A7;}_LL2A6: return
23;_LL2A7: {struct Cyc_Absyn_RgnsEff_struct*_tmp4D2=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp4BB;if(_tmp4D2->tag != 25)goto _LL2A9;}_LL2A8: return 24;_LL2A9: {struct Cyc_Absyn_TagType_struct*
_tmp4D3=(struct Cyc_Absyn_TagType_struct*)_tmp4BB;if(_tmp4D3->tag != 20)goto _LL2AB;}
_LL2AA: return 26;_LL2AB: {struct Cyc_Absyn_DynRgnType_struct*_tmp4D4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4BB;if(_tmp4D4->tag != 17)goto _LL2AD;}_LL2AC: return 27;_LL2AD: {struct Cyc_Absyn_ValueofType_struct*
_tmp4D5=(struct Cyc_Absyn_ValueofType_struct*)_tmp4BB;if(_tmp4D5->tag != 19)goto
_LL27A;}_LL2AE: return 28;_LL27A:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp4D6=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp4D6 != 0)return _tmp4D6;{struct _tuple0 _tmp1033;struct _tuple0 _tmp4D8=(
_tmp1033.f1=t2,((_tmp1033.f2=t1,_tmp1033)));void*_tmp4D9;void*_tmp4DB;void*
_tmp4DD;struct Cyc_Absyn_Tvar*_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tvar*_tmp4E2;
void*_tmp4E3;struct Cyc_Absyn_AggrInfo _tmp4E5;union Cyc_Absyn_AggrInfoU _tmp4E6;
struct Cyc_List_List*_tmp4E7;void*_tmp4E8;struct Cyc_Absyn_AggrInfo _tmp4EA;union
Cyc_Absyn_AggrInfoU _tmp4EB;struct Cyc_List_List*_tmp4EC;void*_tmp4ED;struct
_tuple2*_tmp4EF;void*_tmp4F0;struct _tuple2*_tmp4F2;void*_tmp4F3;struct Cyc_List_List*
_tmp4F5;void*_tmp4F6;struct Cyc_List_List*_tmp4F8;void*_tmp4F9;struct Cyc_Absyn_DatatypeInfo
_tmp4FB;union Cyc_Absyn_DatatypeInfoU _tmp4FC;struct Cyc_Absyn_Datatypedecl**
_tmp4FD;struct Cyc_Absyn_Datatypedecl*_tmp4FE;struct Cyc_List_List*_tmp4FF;void*
_tmp500;struct Cyc_Absyn_DatatypeInfo _tmp502;union Cyc_Absyn_DatatypeInfoU _tmp503;
struct Cyc_Absyn_Datatypedecl**_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp505;
struct Cyc_List_List*_tmp506;void*_tmp507;struct Cyc_Absyn_DatatypeFieldInfo
_tmp509;union Cyc_Absyn_DatatypeFieldInfoU _tmp50A;struct _tuple3 _tmp50B;struct Cyc_Absyn_Datatypedecl*
_tmp50C;struct Cyc_Absyn_Datatypefield*_tmp50D;struct Cyc_List_List*_tmp50E;void*
_tmp50F;struct Cyc_Absyn_DatatypeFieldInfo _tmp511;union Cyc_Absyn_DatatypeFieldInfoU
_tmp512;struct _tuple3 _tmp513;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_Absyn_Datatypefield*
_tmp515;struct Cyc_List_List*_tmp516;void*_tmp517;struct Cyc_Absyn_PtrInfo _tmp519;
void*_tmp51A;struct Cyc_Absyn_Tqual _tmp51B;struct Cyc_Absyn_PtrAtts _tmp51C;void*
_tmp51D;union Cyc_Absyn_Constraint*_tmp51E;union Cyc_Absyn_Constraint*_tmp51F;
union Cyc_Absyn_Constraint*_tmp520;void*_tmp521;struct Cyc_Absyn_PtrInfo _tmp523;
void*_tmp524;struct Cyc_Absyn_Tqual _tmp525;struct Cyc_Absyn_PtrAtts _tmp526;void*
_tmp527;union Cyc_Absyn_Constraint*_tmp528;union Cyc_Absyn_Constraint*_tmp529;
union Cyc_Absyn_Constraint*_tmp52A;void*_tmp52B;enum Cyc_Absyn_Sign _tmp52D;enum 
Cyc_Absyn_Size_of _tmp52E;void*_tmp52F;enum Cyc_Absyn_Sign _tmp531;enum Cyc_Absyn_Size_of
_tmp532;void*_tmp533;void*_tmp535;void*_tmp537;int _tmp539;void*_tmp53A;int
_tmp53C;void*_tmp53D;struct Cyc_Absyn_ArrayInfo _tmp53F;void*_tmp540;struct Cyc_Absyn_Tqual
_tmp541;struct Cyc_Absyn_Exp*_tmp542;union Cyc_Absyn_Constraint*_tmp543;void*
_tmp544;struct Cyc_Absyn_ArrayInfo _tmp546;void*_tmp547;struct Cyc_Absyn_Tqual
_tmp548;struct Cyc_Absyn_Exp*_tmp549;union Cyc_Absyn_Constraint*_tmp54A;void*
_tmp54B;struct Cyc_Absyn_FnInfo _tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_Core_Opt*
_tmp54F;void*_tmp550;struct Cyc_List_List*_tmp551;int _tmp552;struct Cyc_Absyn_VarargInfo*
_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;void*_tmp556;
struct Cyc_Absyn_FnInfo _tmp558;struct Cyc_List_List*_tmp559;struct Cyc_Core_Opt*
_tmp55A;void*_tmp55B;struct Cyc_List_List*_tmp55C;int _tmp55D;struct Cyc_Absyn_VarargInfo*
_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp560;void*_tmp561;
struct Cyc_List_List*_tmp563;void*_tmp564;struct Cyc_List_List*_tmp566;void*
_tmp567;enum Cyc_Absyn_AggrKind _tmp569;struct Cyc_List_List*_tmp56A;void*_tmp56B;
enum Cyc_Absyn_AggrKind _tmp56D;struct Cyc_List_List*_tmp56E;void*_tmp56F;void*
_tmp571;void*_tmp572;void*_tmp574;void*_tmp575;void*_tmp577;void*_tmp578;void*
_tmp579;void*_tmp57B;void*_tmp57C;void*_tmp57D;void*_tmp57F;void*_tmp580;void*
_tmp582;void*_tmp583;struct Cyc_Absyn_Exp*_tmp585;void*_tmp586;struct Cyc_Absyn_Exp*
_tmp588;void*_tmp589;void*_tmp58B;void*_tmp58D;void*_tmp58F;void*_tmp591;void*
_tmp593;_LL2B0: _tmp4D9=_tmp4D8.f1;{struct Cyc_Absyn_Evar_struct*_tmp4DA=(struct
Cyc_Absyn_Evar_struct*)_tmp4D9;if(_tmp4DA->tag != 1)goto _LL2B2;};_tmp4DB=_tmp4D8.f2;{
struct Cyc_Absyn_Evar_struct*_tmp4DC=(struct Cyc_Absyn_Evar_struct*)_tmp4DB;if(
_tmp4DC->tag != 1)goto _LL2B2;};_LL2B1: {const char*_tmp1036;void*_tmp1035;(
_tmp1035=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1036="typecmp: can only compare closed types",_tag_dyneither(_tmp1036,
sizeof(char),39))),_tag_dyneither(_tmp1035,sizeof(void*),0)));}_LL2B2: _tmp4DD=
_tmp4D8.f1;{struct Cyc_Absyn_VarType_struct*_tmp4DE=(struct Cyc_Absyn_VarType_struct*)
_tmp4DD;if(_tmp4DE->tag != 2)goto _LL2B4;else{_tmp4DF=_tmp4DE->f1;}};_tmp4E0=
_tmp4D8.f2;{struct Cyc_Absyn_VarType_struct*_tmp4E1=(struct Cyc_Absyn_VarType_struct*)
_tmp4E0;if(_tmp4E1->tag != 2)goto _LL2B4;else{_tmp4E2=_tmp4E1->f1;}};_LL2B3: return
Cyc_Core_intcmp(_tmp4E2->identity,_tmp4DF->identity);_LL2B4: _tmp4E3=_tmp4D8.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4E4=(struct Cyc_Absyn_AggrType_struct*)
_tmp4E3;if(_tmp4E4->tag != 12)goto _LL2B6;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.aggr_info;
_tmp4E7=_tmp4E5.targs;}};_tmp4E8=_tmp4D8.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4E9=(struct Cyc_Absyn_AggrType_struct*)_tmp4E8;if(_tmp4E9->tag != 12)goto
_LL2B6;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4EA.aggr_info;_tmp4EC=_tmp4EA.targs;}};
_LL2B5: {struct _tuple2*_tmp598;struct _tuple12 _tmp597=Cyc_Absyn_aggr_kinded_name(
_tmp4E6);_tmp598=_tmp597.f2;{struct _tuple2*_tmp59A;struct _tuple12 _tmp599=Cyc_Absyn_aggr_kinded_name(
_tmp4EB);_tmp59A=_tmp599.f2;{int _tmp59B=Cyc_Absyn_qvar_cmp(_tmp598,_tmp59A);if(
_tmp59B != 0)return _tmp59B;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp4E7,_tmp4EC);}};};}
_LL2B6: _tmp4ED=_tmp4D8.f1;{struct Cyc_Absyn_EnumType_struct*_tmp4EE=(struct Cyc_Absyn_EnumType_struct*)
_tmp4ED;if(_tmp4EE->tag != 14)goto _LL2B8;else{_tmp4EF=_tmp4EE->f1;}};_tmp4F0=
_tmp4D8.f2;{struct Cyc_Absyn_EnumType_struct*_tmp4F1=(struct Cyc_Absyn_EnumType_struct*)
_tmp4F0;if(_tmp4F1->tag != 14)goto _LL2B8;else{_tmp4F2=_tmp4F1->f1;}};_LL2B7:
return Cyc_Absyn_qvar_cmp(_tmp4EF,_tmp4F2);_LL2B8: _tmp4F3=_tmp4D8.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4F3;if(_tmp4F4->tag != 15)goto
_LL2BA;else{_tmp4F5=_tmp4F4->f1;}};_tmp4F6=_tmp4D8.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4F6;if(_tmp4F7->tag != 15)goto
_LL2BA;else{_tmp4F8=_tmp4F7->f1;}};_LL2B9: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp4F5,_tmp4F8);_LL2BA: _tmp4F9=_tmp4D8.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4FA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4F9;if(_tmp4FA->tag != 3)goto
_LL2BC;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FB.datatype_info;if((_tmp4FC.KnownDatatype).tag
!= 2)goto _LL2BC;_tmp4FD=(struct Cyc_Absyn_Datatypedecl**)(_tmp4FC.KnownDatatype).val;
_tmp4FE=*_tmp4FD;_tmp4FF=_tmp4FB.targs;}};_tmp500=_tmp4D8.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp501=(struct Cyc_Absyn_DatatypeType_struct*)_tmp500;if(_tmp501->tag != 3)goto
_LL2BC;else{_tmp502=_tmp501->f1;_tmp503=_tmp502.datatype_info;if((_tmp503.KnownDatatype).tag
!= 2)goto _LL2BC;_tmp504=(struct Cyc_Absyn_Datatypedecl**)(_tmp503.KnownDatatype).val;
_tmp505=*_tmp504;_tmp506=_tmp502.targs;}};_LL2BB: if(_tmp505 == _tmp4FE)return 0;{
int _tmp59C=Cyc_Absyn_qvar_cmp(_tmp505->name,_tmp4FE->name);if(_tmp59C != 0)return
_tmp59C;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp506,_tmp4FF);};_LL2BC: _tmp507=
_tmp4D8.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp508=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp507;if(_tmp508->tag != 4)goto _LL2BE;else{_tmp509=_tmp508->f1;_tmp50A=_tmp509.field_info;
if((_tmp50A.KnownDatatypefield).tag != 2)goto _LL2BE;_tmp50B=(struct _tuple3)(
_tmp50A.KnownDatatypefield).val;_tmp50C=_tmp50B.f1;_tmp50D=_tmp50B.f2;_tmp50E=
_tmp509.targs;}};_tmp50F=_tmp4D8.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp510=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp50F;if(_tmp510->tag != 4)
goto _LL2BE;else{_tmp511=_tmp510->f1;_tmp512=_tmp511.field_info;if((_tmp512.KnownDatatypefield).tag
!= 2)goto _LL2BE;_tmp513=(struct _tuple3)(_tmp512.KnownDatatypefield).val;_tmp514=
_tmp513.f1;_tmp515=_tmp513.f2;_tmp516=_tmp511.targs;}};_LL2BD: if(_tmp514 == 
_tmp50C)return 0;{int _tmp59D=Cyc_Absyn_qvar_cmp(_tmp50C->name,_tmp514->name);if(
_tmp59D != 0)return _tmp59D;{int _tmp59E=Cyc_Absyn_qvar_cmp(_tmp50D->name,_tmp515->name);
if(_tmp59E != 0)return _tmp59E;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp516,_tmp50E);};};
_LL2BE: _tmp517=_tmp4D8.f1;{struct Cyc_Absyn_PointerType_struct*_tmp518=(struct Cyc_Absyn_PointerType_struct*)
_tmp517;if(_tmp518->tag != 5)goto _LL2C0;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.elt_typ;
_tmp51B=_tmp519.elt_tq;_tmp51C=_tmp519.ptr_atts;_tmp51D=_tmp51C.rgn;_tmp51E=
_tmp51C.nullable;_tmp51F=_tmp51C.bounds;_tmp520=_tmp51C.zero_term;}};_tmp521=
_tmp4D8.f2;{struct Cyc_Absyn_PointerType_struct*_tmp522=(struct Cyc_Absyn_PointerType_struct*)
_tmp521;if(_tmp522->tag != 5)goto _LL2C0;else{_tmp523=_tmp522->f1;_tmp524=_tmp523.elt_typ;
_tmp525=_tmp523.elt_tq;_tmp526=_tmp523.ptr_atts;_tmp527=_tmp526.rgn;_tmp528=
_tmp526.nullable;_tmp529=_tmp526.bounds;_tmp52A=_tmp526.zero_term;}};_LL2BF: {
int _tmp59F=Cyc_Tcutil_typecmp(_tmp524,_tmp51A);if(_tmp59F != 0)return _tmp59F;{int
_tmp5A0=Cyc_Tcutil_typecmp(_tmp527,_tmp51D);if(_tmp5A0 != 0)return _tmp5A0;{int
_tmp5A1=Cyc_Tcutil_tqual_cmp(_tmp525,_tmp51B);if(_tmp5A1 != 0)return _tmp5A1;{int
_tmp5A2=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp529,_tmp51F);if(_tmp5A2 != 0)
return _tmp5A2;{int _tmp5A3=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52A,
_tmp520);if(_tmp5A3 != 0)return _tmp5A3;{void*_tmp5A4=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp529);_LL2E5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A4;if(_tmp5A5->tag != 0)goto
_LL2E7;}_LL2E6: return 0;_LL2E7:;_LL2E8: goto _LL2E4;_LL2E4:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp528,_tmp51E);};};};};}_LL2C0: _tmp52B=_tmp4D8.f1;{struct Cyc_Absyn_IntType_struct*
_tmp52C=(struct Cyc_Absyn_IntType_struct*)_tmp52B;if(_tmp52C->tag != 6)goto _LL2C2;
else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}};_tmp52F=_tmp4D8.f2;{struct Cyc_Absyn_IntType_struct*
_tmp530=(struct Cyc_Absyn_IntType_struct*)_tmp52F;if(_tmp530->tag != 6)goto _LL2C2;
else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}};_LL2C1: if(_tmp531 != _tmp52D)
return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52D));
if(_tmp532 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp532),(int)((
unsigned int)_tmp52E));return 0;_LL2C2: _tmp533=_tmp4D8.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp534=(struct Cyc_Absyn_FloatType_struct*)_tmp533;if(_tmp534->tag != 7)goto
_LL2C4;};_tmp535=_tmp4D8.f2;{struct Cyc_Absyn_FloatType_struct*_tmp536=(struct Cyc_Absyn_FloatType_struct*)
_tmp535;if(_tmp536->tag != 7)goto _LL2C4;};_LL2C3: return 0;_LL2C4: _tmp537=_tmp4D8.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp538=(struct Cyc_Absyn_DoubleType_struct*)
_tmp537;if(_tmp538->tag != 8)goto _LL2C6;else{_tmp539=_tmp538->f1;}};_tmp53A=
_tmp4D8.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp53B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp53A;if(_tmp53B->tag != 8)goto _LL2C6;else{_tmp53C=_tmp53B->f1;}};_LL2C5: if(
_tmp539 == _tmp53C)return 0;else{if(_tmp539)return - 1;else{return 1;}}_LL2C6: _tmp53D=
_tmp4D8.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp53E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp53D;if(_tmp53E->tag != 9)goto _LL2C8;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53F.elt_type;
_tmp541=_tmp53F.tq;_tmp542=_tmp53F.num_elts;_tmp543=_tmp53F.zero_term;}};_tmp544=
_tmp4D8.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp545=(struct Cyc_Absyn_ArrayType_struct*)
_tmp544;if(_tmp545->tag != 9)goto _LL2C8;else{_tmp546=_tmp545->f1;_tmp547=_tmp546.elt_type;
_tmp548=_tmp546.tq;_tmp549=_tmp546.num_elts;_tmp54A=_tmp546.zero_term;}};_LL2C7: {
int _tmp5A6=Cyc_Tcutil_tqual_cmp(_tmp548,_tmp541);if(_tmp5A6 != 0)return _tmp5A6;{
int _tmp5A7=Cyc_Tcutil_typecmp(_tmp547,_tmp540);if(_tmp5A7 != 0)return _tmp5A7;{int
_tmp5A8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp543,_tmp54A);if(_tmp5A8 != 0)
return _tmp5A8;if(_tmp542 == _tmp549)return 0;if(_tmp542 == 0  || _tmp549 == 0){const
char*_tmp1039;void*_tmp1038;(_tmp1038=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1039="missing expression in array index",
_tag_dyneither(_tmp1039,sizeof(char),34))),_tag_dyneither(_tmp1038,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp542,_tmp549);};};}_LL2C8: _tmp54B=_tmp4D8.f1;{struct Cyc_Absyn_FnType_struct*
_tmp54C=(struct Cyc_Absyn_FnType_struct*)_tmp54B;if(_tmp54C->tag != 10)goto _LL2CA;
else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54D.tvars;_tmp54F=_tmp54D.effect;_tmp550=
_tmp54D.ret_typ;_tmp551=_tmp54D.args;_tmp552=_tmp54D.c_varargs;_tmp553=_tmp54D.cyc_varargs;
_tmp554=_tmp54D.rgn_po;_tmp555=_tmp54D.attributes;}};_tmp556=_tmp4D8.f2;{struct
Cyc_Absyn_FnType_struct*_tmp557=(struct Cyc_Absyn_FnType_struct*)_tmp556;if(
_tmp557->tag != 10)goto _LL2CA;else{_tmp558=_tmp557->f1;_tmp559=_tmp558.tvars;
_tmp55A=_tmp558.effect;_tmp55B=_tmp558.ret_typ;_tmp55C=_tmp558.args;_tmp55D=
_tmp558.c_varargs;_tmp55E=_tmp558.cyc_varargs;_tmp55F=_tmp558.rgn_po;_tmp560=
_tmp558.attributes;}};_LL2C9: {const char*_tmp103C;void*_tmp103B;(_tmp103B=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp103C="typecmp: function types not handled",_tag_dyneither(_tmp103C,sizeof(
char),36))),_tag_dyneither(_tmp103B,sizeof(void*),0)));}_LL2CA: _tmp561=_tmp4D8.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp562=(struct Cyc_Absyn_TupleType_struct*)
_tmp561;if(_tmp562->tag != 11)goto _LL2CC;else{_tmp563=_tmp562->f1;}};_tmp564=
_tmp4D8.f2;{struct Cyc_Absyn_TupleType_struct*_tmp565=(struct Cyc_Absyn_TupleType_struct*)
_tmp564;if(_tmp565->tag != 11)goto _LL2CC;else{_tmp566=_tmp565->f1;}};_LL2CB:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp566,
_tmp563);_LL2CC: _tmp567=_tmp4D8.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp568=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp567;if(_tmp568->tag != 13)goto _LL2CE;
else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;}};_tmp56B=_tmp4D8.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp56C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp56B;if(_tmp56C->tag != 13)goto
_LL2CE;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;}};_LL2CD: if(_tmp56D != 
_tmp569){if(_tmp56D == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp56E,
_tmp56A);_LL2CE: _tmp56F=_tmp4D8.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp570=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp56F;if(_tmp570->tag != 16)goto _LL2D0;
else{_tmp571=(void*)_tmp570->f1;}};_tmp572=_tmp4D8.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp573=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp572;if(_tmp573->tag != 16)goto
_LL2D0;else{_tmp574=(void*)_tmp573->f1;}};_LL2CF: return Cyc_Tcutil_typecmp(
_tmp571,_tmp574);_LL2D0: _tmp575=_tmp4D8.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp576=(struct Cyc_Absyn_DynRgnType_struct*)_tmp575;if(_tmp576->tag != 17)goto
_LL2D2;else{_tmp577=(void*)_tmp576->f1;_tmp578=(void*)_tmp576->f2;}};_tmp579=
_tmp4D8.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp57A=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp579;if(_tmp57A->tag != 17)goto _LL2D2;else{_tmp57B=(void*)_tmp57A->f1;_tmp57C=(
void*)_tmp57A->f2;}};_LL2D1: {int _tmp5AD=Cyc_Tcutil_typecmp(_tmp577,_tmp57B);if(
_tmp5AD != 0)return _tmp5AD;else{return Cyc_Tcutil_typecmp(_tmp578,_tmp57C);}}
_LL2D2: _tmp57D=_tmp4D8.f1;{struct Cyc_Absyn_TagType_struct*_tmp57E=(struct Cyc_Absyn_TagType_struct*)
_tmp57D;if(_tmp57E->tag != 20)goto _LL2D4;else{_tmp57F=(void*)_tmp57E->f1;}};
_tmp580=_tmp4D8.f2;{struct Cyc_Absyn_TagType_struct*_tmp581=(struct Cyc_Absyn_TagType_struct*)
_tmp580;if(_tmp581->tag != 20)goto _LL2D4;else{_tmp582=(void*)_tmp581->f1;}};
_LL2D3: return Cyc_Tcutil_typecmp(_tmp57F,_tmp582);_LL2D4: _tmp583=_tmp4D8.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp584=(struct Cyc_Absyn_ValueofType_struct*)
_tmp583;if(_tmp584->tag != 19)goto _LL2D6;else{_tmp585=_tmp584->f1;}};_tmp586=
_tmp4D8.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp587=(struct Cyc_Absyn_ValueofType_struct*)
_tmp586;if(_tmp587->tag != 19)goto _LL2D6;else{_tmp588=_tmp587->f1;}};_LL2D5:
return Cyc_Evexp_const_exp_cmp(_tmp585,_tmp588);_LL2D6: _tmp589=_tmp4D8.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp58A=(struct Cyc_Absyn_JoinEff_struct*)_tmp589;if(
_tmp58A->tag != 24)goto _LL2D8;};_LL2D7: goto _LL2D9;_LL2D8: _tmp58B=_tmp4D8.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp58C=(struct Cyc_Absyn_JoinEff_struct*)_tmp58B;
if(_tmp58C->tag != 24)goto _LL2DA;};_LL2D9: goto _LL2DB;_LL2DA: _tmp58D=_tmp4D8.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp58E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp58D;if(_tmp58E->tag != 23)goto _LL2DC;};_LL2DB: goto _LL2DD;_LL2DC: _tmp58F=
_tmp4D8.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp590=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp58F;if(_tmp590->tag != 25)goto _LL2DE;};_LL2DD: goto _LL2DF;_LL2DE: _tmp591=
_tmp4D8.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp592=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp591;if(_tmp592->tag != 25)goto _LL2E0;};_LL2DF: goto _LL2E1;_LL2E0: _tmp593=
_tmp4D8.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp594=(struct Cyc_Absyn_AccessEff_struct*)
_tmp593;if(_tmp594->tag != 23)goto _LL2E2;};_LL2E1: {const char*_tmp103F;void*
_tmp103E;(_tmp103E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp103F="typecmp: effects not handled",_tag_dyneither(
_tmp103F,sizeof(char),29))),_tag_dyneither(_tmp103E,sizeof(void*),0)));}_LL2E2:;
_LL2E3: {const char*_tmp1042;void*_tmp1041;(_tmp1041=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1042="Unmatched case in typecmp",
_tag_dyneither(_tmp1042,sizeof(char),26))),_tag_dyneither(_tmp1041,sizeof(void*),
0)));}_LL2AF:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp5B2=Cyc_Tcutil_compress(t);_LL2EA: {struct Cyc_Absyn_IntType_struct*
_tmp5B3=(struct Cyc_Absyn_IntType_struct*)_tmp5B2;if(_tmp5B3->tag != 6)goto _LL2EC;}
_LL2EB: goto _LL2ED;_LL2EC: {struct Cyc_Absyn_FloatType_struct*_tmp5B4=(struct Cyc_Absyn_FloatType_struct*)
_tmp5B2;if(_tmp5B4->tag != 7)goto _LL2EE;}_LL2ED: goto _LL2EF;_LL2EE: {struct Cyc_Absyn_DoubleType_struct*
_tmp5B5=(struct Cyc_Absyn_DoubleType_struct*)_tmp5B2;if(_tmp5B5->tag != 8)goto
_LL2F0;}_LL2EF: goto _LL2F1;_LL2F0: {struct Cyc_Absyn_EnumType_struct*_tmp5B6=(
struct Cyc_Absyn_EnumType_struct*)_tmp5B2;if(_tmp5B6->tag != 14)goto _LL2F2;}_LL2F1:
goto _LL2F3;_LL2F2: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5B7=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5B2;if(_tmp5B7->tag != 15)goto _LL2F4;}_LL2F3: return 1;_LL2F4:;_LL2F5: return 0;
_LL2E9:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1043;struct _tuple0 _tmp5B9=(_tmp1043.f1=t1,((_tmp1043.f2=t2,_tmp1043)));
void*_tmp5BA;int _tmp5BC;void*_tmp5BD;int _tmp5BF;void*_tmp5C0;void*_tmp5C2;void*
_tmp5C4;void*_tmp5C6;void*_tmp5C8;void*_tmp5CA;void*_tmp5CC;void*_tmp5CE;void*
_tmp5D0;void*_tmp5D2;void*_tmp5D4;enum Cyc_Absyn_Size_of _tmp5D6;void*_tmp5D7;
enum Cyc_Absyn_Size_of _tmp5D9;void*_tmp5DA;enum Cyc_Absyn_Size_of _tmp5DC;void*
_tmp5DD;enum Cyc_Absyn_Size_of _tmp5DF;void*_tmp5E0;enum Cyc_Absyn_Size_of _tmp5E2;
void*_tmp5E3;enum Cyc_Absyn_Size_of _tmp5E5;void*_tmp5E6;enum Cyc_Absyn_Size_of
_tmp5E8;void*_tmp5E9;enum Cyc_Absyn_Size_of _tmp5EB;void*_tmp5EC;void*_tmp5EE;
enum Cyc_Absyn_Size_of _tmp5F0;void*_tmp5F1;void*_tmp5F3;enum Cyc_Absyn_Size_of
_tmp5F5;void*_tmp5F6;enum Cyc_Absyn_Size_of _tmp5F8;void*_tmp5F9;enum Cyc_Absyn_Size_of
_tmp5FB;void*_tmp5FC;enum Cyc_Absyn_Size_of _tmp5FE;void*_tmp5FF;void*_tmp601;
enum Cyc_Absyn_Size_of _tmp603;void*_tmp604;enum Cyc_Absyn_Size_of _tmp606;void*
_tmp607;enum Cyc_Absyn_Size_of _tmp609;void*_tmp60A;enum Cyc_Absyn_Size_of _tmp60C;
void*_tmp60D;enum Cyc_Absyn_Size_of _tmp60F;void*_tmp610;enum Cyc_Absyn_Size_of
_tmp612;void*_tmp613;enum Cyc_Absyn_Size_of _tmp615;void*_tmp616;void*_tmp618;
enum Cyc_Absyn_Size_of _tmp61A;_LL2F7: _tmp5BA=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5BB=(struct Cyc_Absyn_DoubleType_struct*)_tmp5BA;if(_tmp5BB->tag != 8)goto
_LL2F9;else{_tmp5BC=_tmp5BB->f1;}};_tmp5BD=_tmp5B9.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp5BE=(struct Cyc_Absyn_DoubleType_struct*)_tmp5BD;if(_tmp5BE->tag != 8)goto
_LL2F9;else{_tmp5BF=_tmp5BE->f1;}};_LL2F8: return !_tmp5BF  && _tmp5BC;_LL2F9:
_tmp5C0=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5C1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5C0;if(_tmp5C1->tag != 8)goto _LL2FB;};_tmp5C2=_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp5C3=(struct Cyc_Absyn_FloatType_struct*)_tmp5C2;if(_tmp5C3->tag != 7)goto
_LL2FB;};_LL2FA: goto _LL2FC;_LL2FB: _tmp5C4=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5C5=(struct Cyc_Absyn_DoubleType_struct*)_tmp5C4;if(_tmp5C5->tag != 8)goto
_LL2FD;};_tmp5C6=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5C7=(struct Cyc_Absyn_IntType_struct*)
_tmp5C6;if(_tmp5C7->tag != 6)goto _LL2FD;};_LL2FC: goto _LL2FE;_LL2FD: _tmp5C8=
_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5C9=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5C8;if(_tmp5C9->tag != 8)goto _LL2FF;};_tmp5CA=_tmp5B9.f2;{struct Cyc_Absyn_TagType_struct*
_tmp5CB=(struct Cyc_Absyn_TagType_struct*)_tmp5CA;if(_tmp5CB->tag != 20)goto _LL2FF;};
_LL2FE: goto _LL300;_LL2FF: _tmp5CC=_tmp5B9.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp5CD=(struct Cyc_Absyn_FloatType_struct*)_tmp5CC;if(_tmp5CD->tag != 7)goto
_LL301;};_tmp5CE=_tmp5B9.f2;{struct Cyc_Absyn_TagType_struct*_tmp5CF=(struct Cyc_Absyn_TagType_struct*)
_tmp5CE;if(_tmp5CF->tag != 20)goto _LL301;};_LL300: goto _LL302;_LL301: _tmp5D0=
_tmp5B9.f1;{struct Cyc_Absyn_FloatType_struct*_tmp5D1=(struct Cyc_Absyn_FloatType_struct*)
_tmp5D0;if(_tmp5D1->tag != 7)goto _LL303;};_tmp5D2=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5D3=(struct Cyc_Absyn_IntType_struct*)_tmp5D2;if(_tmp5D3->tag != 6)goto _LL303;};
_LL302: return 1;_LL303: _tmp5D4=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5D5=(
struct Cyc_Absyn_IntType_struct*)_tmp5D4;if(_tmp5D5->tag != 6)goto _LL305;else{
_tmp5D6=_tmp5D5->f2;if(_tmp5D6 != Cyc_Absyn_LongLong_sz)goto _LL305;}};_tmp5D7=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5D8=(struct Cyc_Absyn_IntType_struct*)
_tmp5D7;if(_tmp5D8->tag != 6)goto _LL305;else{_tmp5D9=_tmp5D8->f2;if(_tmp5D9 != Cyc_Absyn_LongLong_sz)
goto _LL305;}};_LL304: return 0;_LL305: _tmp5DA=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5DB=(struct Cyc_Absyn_IntType_struct*)_tmp5DA;if(_tmp5DB->tag != 6)goto _LL307;
else{_tmp5DC=_tmp5DB->f2;if(_tmp5DC != Cyc_Absyn_LongLong_sz)goto _LL307;}};_LL306:
return 1;_LL307: _tmp5DD=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5DE=(
struct Cyc_Absyn_IntType_struct*)_tmp5DD;if(_tmp5DE->tag != 6)goto _LL309;else{
_tmp5DF=_tmp5DE->f2;if(_tmp5DF != Cyc_Absyn_Long_sz)goto _LL309;}};_tmp5E0=_tmp5B9.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5E1=(struct Cyc_Absyn_IntType_struct*)_tmp5E0;
if(_tmp5E1->tag != 6)goto _LL309;else{_tmp5E2=_tmp5E1->f2;if(_tmp5E2 != Cyc_Absyn_Int_sz)
goto _LL309;}};_LL308: goto _LL30A;_LL309: _tmp5E3=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5E4=(struct Cyc_Absyn_IntType_struct*)_tmp5E3;if(_tmp5E4->tag != 6)goto _LL30B;
else{_tmp5E5=_tmp5E4->f2;if(_tmp5E5 != Cyc_Absyn_Int_sz)goto _LL30B;}};_tmp5E6=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5E7=(struct Cyc_Absyn_IntType_struct*)
_tmp5E6;if(_tmp5E7->tag != 6)goto _LL30B;else{_tmp5E8=_tmp5E7->f2;if(_tmp5E8 != Cyc_Absyn_Long_sz)
goto _LL30B;}};_LL30A: return 0;_LL30B: _tmp5E9=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5EA=(struct Cyc_Absyn_IntType_struct*)_tmp5E9;if(_tmp5EA->tag != 6)goto _LL30D;
else{_tmp5EB=_tmp5EA->f2;if(_tmp5EB != Cyc_Absyn_Long_sz)goto _LL30D;}};_tmp5EC=
_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5ED=(struct Cyc_Absyn_FloatType_struct*)
_tmp5EC;if(_tmp5ED->tag != 7)goto _LL30D;};_LL30C: goto _LL30E;_LL30D: _tmp5EE=
_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5EF=(struct Cyc_Absyn_IntType_struct*)
_tmp5EE;if(_tmp5EF->tag != 6)goto _LL30F;else{_tmp5F0=_tmp5EF->f2;if(_tmp5F0 != Cyc_Absyn_Int_sz)
goto _LL30F;}};_tmp5F1=_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5F2=(
struct Cyc_Absyn_FloatType_struct*)_tmp5F1;if(_tmp5F2->tag != 7)goto _LL30F;};
_LL30E: goto _LL310;_LL30F: _tmp5F3=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5F4=(struct Cyc_Absyn_IntType_struct*)_tmp5F3;if(_tmp5F4->tag != 6)goto _LL311;
else{_tmp5F5=_tmp5F4->f2;if(_tmp5F5 != Cyc_Absyn_Long_sz)goto _LL311;}};_tmp5F6=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5F7=(struct Cyc_Absyn_IntType_struct*)
_tmp5F6;if(_tmp5F7->tag != 6)goto _LL311;else{_tmp5F8=_tmp5F7->f2;if(_tmp5F8 != Cyc_Absyn_Short_sz)
goto _LL311;}};_LL310: goto _LL312;_LL311: _tmp5F9=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5FA=(struct Cyc_Absyn_IntType_struct*)_tmp5F9;if(_tmp5FA->tag != 6)goto _LL313;
else{_tmp5FB=_tmp5FA->f2;if(_tmp5FB != Cyc_Absyn_Int_sz)goto _LL313;}};_tmp5FC=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5FD=(struct Cyc_Absyn_IntType_struct*)
_tmp5FC;if(_tmp5FD->tag != 6)goto _LL313;else{_tmp5FE=_tmp5FD->f2;if(_tmp5FE != Cyc_Absyn_Short_sz)
goto _LL313;}};_LL312: goto _LL314;_LL313: _tmp5FF=_tmp5B9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp600=(struct Cyc_Absyn_TagType_struct*)_tmp5FF;if(_tmp600->tag != 20)goto _LL315;};
_tmp601=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp602=(struct Cyc_Absyn_IntType_struct*)
_tmp601;if(_tmp602->tag != 6)goto _LL315;else{_tmp603=_tmp602->f2;if(_tmp603 != Cyc_Absyn_Short_sz)
goto _LL315;}};_LL314: goto _LL316;_LL315: _tmp604=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp605=(struct Cyc_Absyn_IntType_struct*)_tmp604;if(_tmp605->tag != 6)goto _LL317;
else{_tmp606=_tmp605->f2;if(_tmp606 != Cyc_Absyn_Long_sz)goto _LL317;}};_tmp607=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp608=(struct Cyc_Absyn_IntType_struct*)
_tmp607;if(_tmp608->tag != 6)goto _LL317;else{_tmp609=_tmp608->f2;if(_tmp609 != Cyc_Absyn_Char_sz)
goto _LL317;}};_LL316: goto _LL318;_LL317: _tmp60A=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp60B=(struct Cyc_Absyn_IntType_struct*)_tmp60A;if(_tmp60B->tag != 6)goto _LL319;
else{_tmp60C=_tmp60B->f2;if(_tmp60C != Cyc_Absyn_Int_sz)goto _LL319;}};_tmp60D=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp60E=(struct Cyc_Absyn_IntType_struct*)
_tmp60D;if(_tmp60E->tag != 6)goto _LL319;else{_tmp60F=_tmp60E->f2;if(_tmp60F != Cyc_Absyn_Char_sz)
goto _LL319;}};_LL318: goto _LL31A;_LL319: _tmp610=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp611=(struct Cyc_Absyn_IntType_struct*)_tmp610;if(_tmp611->tag != 6)goto _LL31B;
else{_tmp612=_tmp611->f2;if(_tmp612 != Cyc_Absyn_Short_sz)goto _LL31B;}};_tmp613=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp614=(struct Cyc_Absyn_IntType_struct*)
_tmp613;if(_tmp614->tag != 6)goto _LL31B;else{_tmp615=_tmp614->f2;if(_tmp615 != Cyc_Absyn_Char_sz)
goto _LL31B;}};_LL31A: goto _LL31C;_LL31B: _tmp616=_tmp5B9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp617=(struct Cyc_Absyn_TagType_struct*)_tmp616;if(_tmp617->tag != 20)goto _LL31D;};
_tmp618=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp619=(struct Cyc_Absyn_IntType_struct*)
_tmp618;if(_tmp619->tag != 6)goto _LL31D;else{_tmp61A=_tmp619->f2;if(_tmp61A != Cyc_Absyn_Char_sz)
goto _LL31D;}};_LL31C: return 1;_LL31D:;_LL31E: return 0;_LL2F6:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp61B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1044;max_arith_type=((_tmp1044=
_region_malloc(_tmp61B,sizeof(*_tmp1044)),((_tmp1044->v=t1,_tmp1044))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp1049;void*_tmp1048[2];struct
Cyc_String_pa_struct _tmp1047;struct Cyc_String_pa_struct _tmp1046;(_tmp1046.tag=0,((
_tmp1046.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1047.tag=0,((_tmp1047.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1048[0]=& _tmp1047,((_tmp1048[1]=& _tmp1046,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp1049="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp1049,sizeof(char),41))),_tag_dyneither(_tmp1048,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp621=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL320: {struct Cyc_Absyn_PointerType_struct*
_tmp622=(struct Cyc_Absyn_PointerType_struct*)_tmp621;if(_tmp622->tag != 5)goto
_LL322;}_LL321: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL31F;_LL322:;_LL323: return 0;_LL31F:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp623=Cyc_Tcutil_compress(
t);_LL325: {struct Cyc_Absyn_IntType_struct*_tmp624=(struct Cyc_Absyn_IntType_struct*)
_tmp623;if(_tmp624->tag != 6)goto _LL327;}_LL326: goto _LL328;_LL327: {struct Cyc_Absyn_TagType_struct*
_tmp625=(struct Cyc_Absyn_TagType_struct*)_tmp623;if(_tmp625->tag != 20)goto _LL329;}
_LL328: goto _LL32A;_LL329: {struct Cyc_Absyn_EnumType_struct*_tmp626=(struct Cyc_Absyn_EnumType_struct*)
_tmp623;if(_tmp626->tag != 14)goto _LL32B;}_LL32A: goto _LL32C;_LL32B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp627=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp623;if(_tmp627->tag != 15)goto
_LL32D;}_LL32C: return 1;_LL32D:;_LL32E: return 0;_LL324:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp104C;void*_tmp104B;(_tmp104B=0,Cyc_Tcutil_warn(e->loc,((_tmp104C="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp104C,sizeof(char),44))),_tag_dyneither(_tmp104B,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp104F;void*_tmp104E;(
_tmp104E=0,Cyc_Tcutil_warn(e->loc,((_tmp104F="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp104F,sizeof(char),44))),_tag_dyneither(_tmp104E,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1050;struct _tuple0 _tmp62D=(
_tmp1050.f1=t1,((_tmp1050.f2=t2,_tmp1050)));void*_tmp62E;struct Cyc_Absyn_PtrInfo
_tmp630;void*_tmp631;struct Cyc_Absyn_PtrInfo _tmp633;void*_tmp634;struct Cyc_Absyn_ArrayInfo
_tmp636;void*_tmp637;struct Cyc_Absyn_Tqual _tmp638;struct Cyc_Absyn_Exp*_tmp639;
union Cyc_Absyn_Constraint*_tmp63A;void*_tmp63B;struct Cyc_Absyn_ArrayInfo _tmp63D;
void*_tmp63E;struct Cyc_Absyn_Tqual _tmp63F;struct Cyc_Absyn_Exp*_tmp640;union Cyc_Absyn_Constraint*
_tmp641;void*_tmp642;void*_tmp644;_LL330: _tmp62E=_tmp62D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp62F=(struct Cyc_Absyn_PointerType_struct*)_tmp62E;if(_tmp62F->tag != 5)goto
_LL332;else{_tmp630=_tmp62F->f1;}};_tmp631=_tmp62D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp632=(struct Cyc_Absyn_PointerType_struct*)_tmp631;if(_tmp632->tag != 5)goto
_LL332;else{_tmp633=_tmp632->f1;}};_LL331: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp630.ptr_atts).nullable,(_tmp633.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp630.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp630.ptr_atts).bounds,(
_tmp633.ptr_atts).bounds)){struct _tuple0 _tmp1051;struct _tuple0 _tmp647=(_tmp1051.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp630.ptr_atts).bounds),((_tmp1051.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp633.ptr_atts).bounds),
_tmp1051)));void*_tmp648;void*_tmp64A;void*_tmp64C;void*_tmp64E;void*_tmp650;
struct Cyc_Absyn_Exp*_tmp652;void*_tmp653;struct Cyc_Absyn_Exp*_tmp655;void*
_tmp656;void*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;_LL339: _tmp648=_tmp647.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp649=(struct Cyc_Absyn_Upper_b_struct*)_tmp648;
if(_tmp649->tag != 1)goto _LL33B;};_tmp64A=_tmp647.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp64B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp64A;if(_tmp64B->tag != 0)goto
_LL33B;};_LL33A: goto _LL33C;_LL33B: _tmp64C=_tmp647.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp64D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp64C;if(_tmp64D->tag != 0)goto
_LL33D;};_tmp64E=_tmp647.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp64F=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp64E;if(_tmp64F->tag != 0)goto _LL33D;};_LL33C:
okay=1;goto _LL338;_LL33D: _tmp650=_tmp647.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp651=(struct Cyc_Absyn_Upper_b_struct*)_tmp650;if(_tmp651->tag != 1)goto _LL33F;
else{_tmp652=_tmp651->f1;}};_tmp653=_tmp647.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp654=(struct Cyc_Absyn_Upper_b_struct*)_tmp653;if(_tmp654->tag != 1)goto _LL33F;
else{_tmp655=_tmp654->f1;}};_LL33E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp655,
_tmp652);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp633.ptr_atts).zero_term)){const char*_tmp1054;void*_tmp1053;(_tmp1053=0,
Cyc_Tcutil_warn(loc,((_tmp1054="implicit cast to shorter array",_tag_dyneither(
_tmp1054,sizeof(char),31))),_tag_dyneither(_tmp1053,sizeof(void*),0)));}goto
_LL338;_LL33F: _tmp656=_tmp647.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp657=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp656;if(_tmp657->tag != 0)goto _LL338;};
_tmp658=_tmp647.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp659=(struct Cyc_Absyn_Upper_b_struct*)
_tmp658;if(_tmp659->tag != 1)goto _LL338;else{_tmp65A=_tmp659->f1;}};_LL340: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp630.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp633.ptr_atts).bounds))goto _LL338;okay=0;goto
_LL338;_LL338:;}okay=okay  && (!(_tmp630.elt_tq).real_const  || (_tmp633.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp630.ptr_atts).rgn,(_tmp633.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp630.ptr_atts).rgn,(_tmp633.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp630.ptr_atts).zero_term,(
_tmp633.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp630.ptr_atts).zero_term) && (_tmp633.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp630.elt_typ,_tmp633.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp630.elt_typ,_tmp633.elt_typ));return okay;}_LL332: _tmp634=_tmp62D.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp635=(struct Cyc_Absyn_ArrayType_struct*)_tmp634;
if(_tmp635->tag != 9)goto _LL334;else{_tmp636=_tmp635->f1;_tmp637=_tmp636.elt_type;
_tmp638=_tmp636.tq;_tmp639=_tmp636.num_elts;_tmp63A=_tmp636.zero_term;}};_tmp63B=
_tmp62D.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp63C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp63B;if(_tmp63C->tag != 9)goto _LL334;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63D.elt_type;
_tmp63F=_tmp63D.tq;_tmp640=_tmp63D.num_elts;_tmp641=_tmp63D.zero_term;}};_LL333: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp63A,_tmp641) && ((_tmp639 != 0
 && _tmp640 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp639,(
struct Cyc_Absyn_Exp*)_tmp640));return(okay  && Cyc_Tcutil_unify(_tmp637,_tmp63E))
 && (!_tmp638.real_const  || _tmp63F.real_const);}_LL334: _tmp642=_tmp62D.f1;{
struct Cyc_Absyn_TagType_struct*_tmp643=(struct Cyc_Absyn_TagType_struct*)_tmp642;
if(_tmp643->tag != 20)goto _LL336;};_tmp644=_tmp62D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp645=(struct Cyc_Absyn_IntType_struct*)_tmp644;if(_tmp645->tag != 6)goto _LL336;};
_LL335: return 0;_LL336:;_LL337: return Cyc_Tcutil_unify(t1,t2);_LL32F:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp65D=Cyc_Tcutil_compress(t);
_LL342: {struct Cyc_Absyn_PointerType_struct*_tmp65E=(struct Cyc_Absyn_PointerType_struct*)
_tmp65D;if(_tmp65E->tag != 5)goto _LL344;}_LL343: return 1;_LL344:;_LL345: return 0;
_LL341:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp65F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp661;void*
_tmp662;_LL347: {struct Cyc_Absyn_PointerType_struct*_tmp660=(struct Cyc_Absyn_PointerType_struct*)
_tmp65F;if(_tmp660->tag != 5)goto _LL349;else{_tmp661=_tmp660->f1;_tmp662=_tmp661.elt_typ;}}
_LL348: return _tmp662;_LL349:;_LL34A: {const char*_tmp1057;void*_tmp1056;(_tmp1056=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1057="pointer_elt_type",_tag_dyneither(_tmp1057,sizeof(char),17))),
_tag_dyneither(_tmp1056,sizeof(void*),0)));}_LL346:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp665=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp667;struct Cyc_Absyn_PtrAtts _tmp668;struct Cyc_Absyn_PtrAtts*
_tmp669;_LL34C: {struct Cyc_Absyn_PointerType_struct*_tmp666=(struct Cyc_Absyn_PointerType_struct*)
_tmp665;if(_tmp666->tag != 5)goto _LL34E;else{_tmp667=_tmp666->f1;_tmp668=_tmp667.ptr_atts;
_tmp669=(struct Cyc_Absyn_PtrAtts*)&(_tmp666->f1).ptr_atts;}}_LL34D: return _tmp669->rgn;
_LL34E:;_LL34F: {const char*_tmp105A;void*_tmp1059;(_tmp1059=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105A="pointer_elt_type",
_tag_dyneither(_tmp105A,sizeof(char),17))),_tag_dyneither(_tmp1059,sizeof(void*),
0)));}_LL34B:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp66C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp66E;struct Cyc_Absyn_PtrAtts _tmp66F;void*_tmp670;_LL351: {struct Cyc_Absyn_PointerType_struct*
_tmp66D=(struct Cyc_Absyn_PointerType_struct*)_tmp66C;if(_tmp66D->tag != 5)goto
_LL353;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66E.ptr_atts;_tmp670=_tmp66F.rgn;}}
_LL352:*rgn=_tmp670;return 1;_LL353:;_LL354: return 0;_LL350:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp671=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp673;struct Cyc_Absyn_PtrAtts _tmp674;union Cyc_Absyn_Constraint*_tmp675;_LL356: {
struct Cyc_Absyn_PointerType_struct*_tmp672=(struct Cyc_Absyn_PointerType_struct*)
_tmp671;if(_tmp672->tag != 5)goto _LL358;else{_tmp673=_tmp672->f1;_tmp674=_tmp673.ptr_atts;
_tmp675=_tmp674.bounds;}}_LL357:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp675)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL358:;_LL359: return Cyc_Tcutil_typ_kind(t)== Cyc_Absyn_BoxKind;_LL355:;}
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp676=e->r;union Cyc_Absyn_Cnst _tmp678;struct _tuple7 _tmp679;int _tmp67A;
union Cyc_Absyn_Cnst _tmp67C;struct _tuple5 _tmp67D;char _tmp67E;union Cyc_Absyn_Cnst
_tmp680;struct _tuple6 _tmp681;short _tmp682;union Cyc_Absyn_Cnst _tmp684;struct
_tuple8 _tmp685;long long _tmp686;void*_tmp688;struct Cyc_Absyn_Exp*_tmp689;_LL35B: {
struct Cyc_Absyn_Const_e_struct*_tmp677=(struct Cyc_Absyn_Const_e_struct*)_tmp676;
if(_tmp677->tag != 0)goto _LL35D;else{_tmp678=_tmp677->f1;if((_tmp678.Int_c).tag != 
4)goto _LL35D;_tmp679=(struct _tuple7)(_tmp678.Int_c).val;_tmp67A=_tmp679.f2;if(
_tmp67A != 0)goto _LL35D;}}_LL35C: goto _LL35E;_LL35D: {struct Cyc_Absyn_Const_e_struct*
_tmp67B=(struct Cyc_Absyn_Const_e_struct*)_tmp676;if(_tmp67B->tag != 0)goto _LL35F;
else{_tmp67C=_tmp67B->f1;if((_tmp67C.Char_c).tag != 2)goto _LL35F;_tmp67D=(struct
_tuple5)(_tmp67C.Char_c).val;_tmp67E=_tmp67D.f2;if(_tmp67E != 0)goto _LL35F;}}
_LL35E: goto _LL360;_LL35F: {struct Cyc_Absyn_Const_e_struct*_tmp67F=(struct Cyc_Absyn_Const_e_struct*)
_tmp676;if(_tmp67F->tag != 0)goto _LL361;else{_tmp680=_tmp67F->f1;if((_tmp680.Short_c).tag
!= 3)goto _LL361;_tmp681=(struct _tuple6)(_tmp680.Short_c).val;_tmp682=_tmp681.f2;
if(_tmp682 != 0)goto _LL361;}}_LL360: goto _LL362;_LL361: {struct Cyc_Absyn_Const_e_struct*
_tmp683=(struct Cyc_Absyn_Const_e_struct*)_tmp676;if(_tmp683->tag != 0)goto _LL363;
else{_tmp684=_tmp683->f1;if((_tmp684.LongLong_c).tag != 5)goto _LL363;_tmp685=(
struct _tuple8)(_tmp684.LongLong_c).val;_tmp686=_tmp685.f2;if(_tmp686 != 0)goto
_LL363;}}_LL362: return 1;_LL363: {struct Cyc_Absyn_Cast_e_struct*_tmp687=(struct
Cyc_Absyn_Cast_e_struct*)_tmp676;if(_tmp687->tag != 15)goto _LL365;else{_tmp688=(
void*)_tmp687->f1;_tmp689=_tmp687->f2;}}_LL364: return Cyc_Tcutil_is_zero(_tmp689)
 && Cyc_Tcutil_admits_zero(_tmp688);_LL365:;_LL366: return 0;_LL35A:;}struct Cyc_Core_Opt
Cyc_Tcutil_trk={(void*)Cyc_Absyn_TopRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_urk={(
void*)Cyc_Absyn_UniqueRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)Cyc_Absyn_RgnKind};
struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)Cyc_Absyn_AnyKind};struct Cyc_Core_Opt Cyc_Tcutil_bk={(
void*)Cyc_Absyn_BoxKind};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)Cyc_Absyn_MemKind};
struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)Cyc_Absyn_IntKind};struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)Cyc_Absyn_EffKind};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k){switch(k){
case Cyc_Absyn_AnyKind: _LL367: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;case Cyc_Absyn_MemKind:
_LL368: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;case Cyc_Absyn_BoxKind: _LL369:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;case Cyc_Absyn_RgnKind: _LL36A: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;case Cyc_Absyn_UniqueRgnKind: _LL36B: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;case Cyc_Absyn_TopRgnKind: _LL36C: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;case Cyc_Absyn_EffKind: _LL36D: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek;case Cyc_Absyn_IntKind: _LL36E: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik;}}static void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k);static
void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,Cyc_Absyn_AnyKind};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,Cyc_Absyn_MemKind};
static struct Cyc_Absyn_Eq_kb_struct bb_v={0,Cyc_Absyn_BoxKind};static struct Cyc_Absyn_Eq_kb_struct
rb_v={0,Cyc_Absyn_RgnKind};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,Cyc_Absyn_UniqueRgnKind};
static struct Cyc_Absyn_Eq_kb_struct tb_v={0,Cyc_Absyn_TopRgnKind};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,Cyc_Absyn_EffKind};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,Cyc_Absyn_IntKind};
static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;
static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;switch(k){case Cyc_Absyn_AnyKind:
_LL370: return& ab;case Cyc_Absyn_MemKind: _LL371: return& mb;case Cyc_Absyn_BoxKind:
_LL372: return& bb;case Cyc_Absyn_RgnKind: _LL373: return& rb;case Cyc_Absyn_UniqueRgnKind:
_LL374: return& ub;case Cyc_Absyn_TopRgnKind: _LL375: return& tb;case Cyc_Absyn_EffKind:
_LL376: return& eb;case Cyc_Absyn_IntKind: _LL377: return& ib;}}void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k){return*
Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum 
Cyc_Absyn_Kind k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum Cyc_Absyn_Kind
k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp105D;struct Cyc_Absyn_Const_e_struct*
_tmp105C;e1->r=(void*)((_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C[0]=((
_tmp105D.tag=0,((_tmp105D.f1=Cyc_Absyn_Null_c,_tmp105D)))),_tmp105C))));}{struct
Cyc_Core_Opt*_tmp694=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp1067;struct Cyc_Absyn_PtrAtts _tmp1066;struct Cyc_Absyn_PtrInfo _tmp1065;struct
Cyc_Absyn_PointerType_struct*_tmp1064;struct Cyc_Absyn_PointerType_struct*_tmp695=(
_tmp1064=_cycalloc(sizeof(*_tmp1064)),((_tmp1064[0]=((_tmp1067.tag=5,((_tmp1067.f1=((
_tmp1065.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp694),((
_tmp1065.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp1065.ptr_atts=((_tmp1066.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,_tmp694),((_tmp1066.nullable=Cyc_Absyn_true_conref,((
_tmp1066.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1066.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1066.ptrloc=0,_tmp1066)))))))))),_tmp1065)))))),_tmp1067)))),_tmp1064)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp695);return Cyc_Tcutil_coerce_arg(
te,e1,t2);};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion
c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL379: {const char*_tmp1068;return(
_tmp1068="unknown",_tag_dyneither(_tmp1068,sizeof(char),8));}case Cyc_Absyn_No_coercion:
_LL37A: {const char*_tmp1069;return(_tmp1069="no coercion",_tag_dyneither(
_tmp1069,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL37B: {const char*
_tmp106A;return(_tmp106A="null check",_tag_dyneither(_tmp106A,sizeof(char),11));}
case Cyc_Absyn_Other_coercion: _LL37C: {const char*_tmp106B;return(_tmp106B="other coercion",
_tag_dyneither(_tmp106B,sizeof(char),15));}}}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);enum Cyc_Absyn_Coercion c;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1070;void*_tmp106F[2];
struct Cyc_String_pa_struct _tmp106E;struct Cyc_String_pa_struct _tmp106D;(_tmp106D.tag=
0,((_tmp106D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp106E.tag=0,((_tmp106E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp106F[0]=& _tmp106E,((_tmp106F[1]=& _tmp106D,Cyc_Tcutil_warn(
e->loc,((_tmp1070="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1070,sizeof(char),53))),_tag_dyneither(_tmp106F,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,Cyc_Absyn_Other_coercion);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*
_tmp1075;void*_tmp1074[2];struct Cyc_String_pa_struct _tmp1073;struct Cyc_String_pa_struct
_tmp1072;(_tmp1072.tag=0,((_tmp1072.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp1073.tag=0,((_tmp1073.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp1074[0]=& _tmp1073,((_tmp1074[1]=& _tmp1072,Cyc_Tcutil_warn(e->loc,((_tmp1075="implicit cast from %s to %s",
_tag_dyneither(_tmp1075,sizeof(char),28))),_tag_dyneither(_tmp1074,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp6A6=Cyc_Tcutil_compress(t);
_LL37F: {struct Cyc_Absyn_IntType_struct*_tmp6A7=(struct Cyc_Absyn_IntType_struct*)
_tmp6A6;if(_tmp6A7->tag != 6)goto _LL381;}_LL380: goto _LL382;_LL381: {struct Cyc_Absyn_FloatType_struct*
_tmp6A8=(struct Cyc_Absyn_FloatType_struct*)_tmp6A6;if(_tmp6A8->tag != 7)goto
_LL383;}_LL382: goto _LL384;_LL383: {struct Cyc_Absyn_DoubleType_struct*_tmp6A9=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6A6;if(_tmp6A9->tag != 8)goto _LL385;}
_LL384: return 1;_LL385:;_LL386: return 0;_LL37E:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp6AB;
struct _RegionHandle*_tmp6AC;struct _tuple15 _tmp6AA=*env;_tmp6AB=_tmp6AA.f1;
_tmp6AC=_tmp6AA.f2;{struct _tuple11*_tmp1076;return(_tmp1076=_region_malloc(
_tmp6AC,sizeof(*_tmp1076)),((_tmp1076->f1=x->tq,((_tmp1076->f2=Cyc_Tcutil_rsubstitute(
_tmp6AC,_tmp6AB,x->type),_tmp1076)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp6AF;void*
_tmp6B0;struct _tuple11 _tmp6AE=*x;_tmp6AF=_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;{struct
_tuple11*_tmp1077;return(_tmp1077=_region_malloc(r,sizeof(*_tmp1077)),((_tmp1077->f1=
_tmp6AF,((_tmp1077->f2=_tmp6B0,_tmp1077)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp6B2=t1;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_AggrInfo
_tmp6B7;union Cyc_Absyn_AggrInfoU _tmp6B8;struct Cyc_Absyn_Aggrdecl**_tmp6B9;struct
Cyc_Absyn_Aggrdecl*_tmp6BA;struct Cyc_List_List*_tmp6BB;enum Cyc_Absyn_AggrKind
_tmp6BD;struct Cyc_List_List*_tmp6BE;_LL388: {struct Cyc_Absyn_VoidType_struct*
_tmp6B3=(struct Cyc_Absyn_VoidType_struct*)_tmp6B2;if(_tmp6B3->tag != 0)goto _LL38A;}
_LL389: return 0;_LL38A: {struct Cyc_Absyn_TupleType_struct*_tmp6B4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B2;if(_tmp6B4->tag != 11)goto _LL38C;else{_tmp6B5=_tmp6B4->f1;}}_LL38B: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp6B5);_LL38C: {struct Cyc_Absyn_AggrType_struct*
_tmp6B6=(struct Cyc_Absyn_AggrType_struct*)_tmp6B2;if(_tmp6B6->tag != 12)goto
_LL38E;else{_tmp6B7=_tmp6B6->f1;_tmp6B8=_tmp6B7.aggr_info;if((_tmp6B8.KnownAggr).tag
!= 2)goto _LL38E;_tmp6B9=(struct Cyc_Absyn_Aggrdecl**)(_tmp6B8.KnownAggr).val;
_tmp6BA=*_tmp6B9;_tmp6BB=_tmp6B7.targs;}}_LL38D: if(((_tmp6BA->kind == Cyc_Absyn_UnionA
 || _tmp6BA->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->rgn_po != 0){
struct _tuple11*_tmp107A;struct Cyc_List_List*_tmp1079;return(_tmp1079=
_region_malloc(r,sizeof(*_tmp1079)),((_tmp1079->hd=((_tmp107A=_region_malloc(r,
sizeof(*_tmp107A)),((_tmp107A->f1=Cyc_Absyn_const_tqual(0),((_tmp107A->f2=t1,
_tmp107A)))))),((_tmp1079->tl=0,_tmp1079)))));}{struct Cyc_List_List*_tmp6C1=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6BA->tvs,_tmp6BB);struct _tuple15
_tmp107B;struct _tuple15 env=(_tmp107B.f1=_tmp6C1,((_tmp107B.f2=r,_tmp107B)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6BA->impl))->fields);};_LL38E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6BC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6B2;if(_tmp6BC->tag != 13)goto
_LL390;else{_tmp6BD=_tmp6BC->f1;if(_tmp6BD != Cyc_Absyn_StructA)goto _LL390;
_tmp6BE=_tmp6BC->f2;}}_LL38F: {struct _tuple15 _tmp107C;struct _tuple15 env=(
_tmp107C.f1=0,((_tmp107C.f2=r,_tmp107C)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp6BE);}_LL390:;_LL391: {struct _tuple11*_tmp107F;struct Cyc_List_List*
_tmp107E;return(_tmp107E=_region_malloc(r,sizeof(*_tmp107E)),((_tmp107E->hd=((
_tmp107F=_region_malloc(r,sizeof(*_tmp107F)),((_tmp107F->f1=Cyc_Absyn_const_tqual(
0),((_tmp107F->f2=t1,_tmp107F)))))),((_tmp107E->tl=0,_tmp107E)))));}_LL387:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp6C6=(void*)t->hd;_LL393: {
struct Cyc_Absyn_Pure_att_struct*_tmp6C7=(struct Cyc_Absyn_Pure_att_struct*)
_tmp6C6;if(_tmp6C7->tag != 21)goto _LL395;}_LL394: goto _LL396;_LL395: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp6C8=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp6C6;if(_tmp6C8->tag != 4)goto
_LL397;}_LL396: goto _LL398;_LL397: {struct Cyc_Absyn_Initializes_att_struct*
_tmp6C9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp6C6;if(_tmp6C9->tag != 20)
goto _LL399;}_LL398: continue;_LL399:;_LL39A: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL392:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp1080;struct _tuple0 _tmp6CB=(_tmp1080.f1=t1,((_tmp1080.f2=t2,_tmp1080)));void*
_tmp6CC;struct Cyc_Absyn_PtrInfo _tmp6CE;void*_tmp6CF;struct Cyc_Absyn_Tqual _tmp6D0;
struct Cyc_Absyn_PtrAtts _tmp6D1;void*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D3;
union Cyc_Absyn_Constraint*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D5;void*_tmp6D6;
struct Cyc_Absyn_PtrInfo _tmp6D8;void*_tmp6D9;struct Cyc_Absyn_Tqual _tmp6DA;struct
Cyc_Absyn_PtrAtts _tmp6DB;void*_tmp6DC;union Cyc_Absyn_Constraint*_tmp6DD;union Cyc_Absyn_Constraint*
_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DF;void*_tmp6E0;struct Cyc_Absyn_DatatypeFieldInfo
_tmp6E2;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E3;struct _tuple3 _tmp6E4;struct Cyc_Absyn_Datatypedecl*
_tmp6E5;struct Cyc_Absyn_Datatypefield*_tmp6E6;struct Cyc_List_List*_tmp6E7;void*
_tmp6E8;struct Cyc_Absyn_DatatypeInfo _tmp6EA;union Cyc_Absyn_DatatypeInfoU _tmp6EB;
struct Cyc_Absyn_Datatypedecl**_tmp6EC;struct Cyc_Absyn_Datatypedecl*_tmp6ED;
struct Cyc_List_List*_tmp6EE;void*_tmp6EF;struct Cyc_Absyn_FnInfo _tmp6F1;void*
_tmp6F2;struct Cyc_Absyn_FnInfo _tmp6F4;_LL39C: _tmp6CC=_tmp6CB.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6CD=(struct Cyc_Absyn_PointerType_struct*)_tmp6CC;if(_tmp6CD->tag != 5)goto
_LL39E;else{_tmp6CE=_tmp6CD->f1;_tmp6CF=_tmp6CE.elt_typ;_tmp6D0=_tmp6CE.elt_tq;
_tmp6D1=_tmp6CE.ptr_atts;_tmp6D2=_tmp6D1.rgn;_tmp6D3=_tmp6D1.nullable;_tmp6D4=
_tmp6D1.bounds;_tmp6D5=_tmp6D1.zero_term;}};_tmp6D6=_tmp6CB.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6D7=(struct Cyc_Absyn_PointerType_struct*)_tmp6D6;if(_tmp6D7->tag != 5)goto
_LL39E;else{_tmp6D8=_tmp6D7->f1;_tmp6D9=_tmp6D8.elt_typ;_tmp6DA=_tmp6D8.elt_tq;
_tmp6DB=_tmp6D8.ptr_atts;_tmp6DC=_tmp6DB.rgn;_tmp6DD=_tmp6DB.nullable;_tmp6DE=
_tmp6DB.bounds;_tmp6DF=_tmp6DB.zero_term;}};_LL39D: if(_tmp6D0.real_const  && !
_tmp6DA.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6D3,
_tmp6DD) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D3)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DD))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6D5,
_tmp6DF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D5)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DF))return 0;if(!Cyc_Tcutil_unify(_tmp6D2,_tmp6DC) && !Cyc_Tcenv_region_outlives(
te,_tmp6D2,_tmp6DC))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp6D4,_tmp6DE)){struct _tuple0 _tmp1081;struct _tuple0 _tmp6F6=(_tmp1081.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D4),((_tmp1081.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6DE),_tmp1081)));void*
_tmp6F7;void*_tmp6F9;void*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;void*_tmp6FE;
struct Cyc_Absyn_Exp*_tmp700;_LL3A5: _tmp6F7=_tmp6F6.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp6F8=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F7;if(_tmp6F8->tag != 1)goto _LL3A7;};
_tmp6F9=_tmp6F6.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp6FA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F9;if(_tmp6FA->tag != 0)goto _LL3A7;};_LL3A6: goto _LL3A4;_LL3A7: _tmp6FB=
_tmp6F6.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp6FC=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6FB;if(_tmp6FC->tag != 1)goto _LL3A9;else{_tmp6FD=_tmp6FC->f1;}};_tmp6FE=
_tmp6F6.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6FF=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6FE;if(_tmp6FF->tag != 1)goto _LL3A9;else{_tmp700=_tmp6FF->f1;}};_LL3A8: if(!
Cyc_Evexp_lte_const_exp(_tmp700,_tmp6FD))return 0;goto _LL3A4;_LL3A9:;_LL3AA:
return 0;_LL3A4:;}{struct _tuple0*_tmp1084;struct Cyc_List_List*_tmp1083;return Cyc_Tcutil_ptrsubtype(
te,((_tmp1083=_cycalloc(sizeof(*_tmp1083)),((_tmp1083->hd=((_tmp1084=_cycalloc(
sizeof(*_tmp1084)),((_tmp1084->f1=t1,((_tmp1084->f2=t2,_tmp1084)))))),((_tmp1083->tl=
assume,_tmp1083)))))),_tmp6CF,_tmp6D9);};_LL39E: _tmp6E0=_tmp6CB.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp6E1=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp6E0;if(_tmp6E1->tag != 4)
goto _LL3A0;else{_tmp6E2=_tmp6E1->f1;_tmp6E3=_tmp6E2.field_info;if((_tmp6E3.KnownDatatypefield).tag
!= 2)goto _LL3A0;_tmp6E4=(struct _tuple3)(_tmp6E3.KnownDatatypefield).val;_tmp6E5=
_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;_tmp6E7=_tmp6E2.targs;}};_tmp6E8=_tmp6CB.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp6E9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp6E8;if(_tmp6E9->tag != 3)goto _LL3A0;else{_tmp6EA=_tmp6E9->f1;_tmp6EB=_tmp6EA.datatype_info;
if((_tmp6EB.KnownDatatype).tag != 2)goto _LL3A0;_tmp6EC=(struct Cyc_Absyn_Datatypedecl**)(
_tmp6EB.KnownDatatype).val;_tmp6ED=*_tmp6EC;_tmp6EE=_tmp6EA.targs;}};_LL39F: if(
_tmp6E5 != _tmp6ED  && Cyc_Absyn_qvar_cmp(_tmp6E5->name,_tmp6ED->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E7)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp6EE))return 0;for(0;_tmp6E7 != 0;(_tmp6E7=
_tmp6E7->tl,_tmp6EE=_tmp6EE->tl)){if(!Cyc_Tcutil_unify((void*)_tmp6E7->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp6EE))->hd))return 0;}return 1;_LL3A0: _tmp6EF=
_tmp6CB.f1;{struct Cyc_Absyn_FnType_struct*_tmp6F0=(struct Cyc_Absyn_FnType_struct*)
_tmp6EF;if(_tmp6F0->tag != 10)goto _LL3A2;else{_tmp6F1=_tmp6F0->f1;}};_tmp6F2=
_tmp6CB.f2;{struct Cyc_Absyn_FnType_struct*_tmp6F3=(struct Cyc_Absyn_FnType_struct*)
_tmp6F2;if(_tmp6F3->tag != 10)goto _LL3A2;else{_tmp6F4=_tmp6F3->f1;}};_LL3A1: if(
_tmp6F1.tvars != 0  || _tmp6F4.tvars != 0){struct Cyc_List_List*_tmp703=_tmp6F1.tvars;
struct Cyc_List_List*_tmp704=_tmp6F4.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp703)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp704))return 0;{
struct _RegionHandle*_tmp705=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp703 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp703->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp704))->hd)->kind))return 0;{struct _tuple14*_tmp108E;struct Cyc_Absyn_VarType_struct
_tmp108D;struct Cyc_Absyn_VarType_struct*_tmp108C;struct Cyc_List_List*_tmp108B;
inst=((_tmp108B=_region_malloc(_tmp705,sizeof(*_tmp108B)),((_tmp108B->hd=((
_tmp108E=_region_malloc(_tmp705,sizeof(*_tmp108E)),((_tmp108E->f1=(struct Cyc_Absyn_Tvar*)
_tmp704->hd,((_tmp108E->f2=(void*)((_tmp108C=_cycalloc(sizeof(*_tmp108C)),((
_tmp108C[0]=((_tmp108D.tag=2,((_tmp108D.f1=(struct Cyc_Absyn_Tvar*)_tmp703->hd,
_tmp108D)))),_tmp108C)))),_tmp108E)))))),((_tmp108B->tl=inst,_tmp108B))))));}
_tmp703=_tmp703->tl;_tmp704=_tmp704->tl;}if(inst != 0){_tmp6F1.tvars=0;_tmp6F4.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1094;struct Cyc_Absyn_FnType_struct*_tmp1093;
struct Cyc_Absyn_FnType_struct _tmp1091;struct Cyc_Absyn_FnType_struct*_tmp1090;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1090=_cycalloc(sizeof(*_tmp1090)),((
_tmp1090[0]=((_tmp1091.tag=10,((_tmp1091.f1=_tmp6F1,_tmp1091)))),_tmp1090)))),(
void*)((_tmp1093=_cycalloc(sizeof(*_tmp1093)),((_tmp1093[0]=((_tmp1094.tag=10,((
_tmp1094.f1=_tmp6F4,_tmp1094)))),_tmp1093)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp6F1.ret_typ,_tmp6F4.ret_typ))return 0;{struct Cyc_List_List*_tmp70E=
_tmp6F1.args;struct Cyc_List_List*_tmp70F=_tmp6F4.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp70E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp70F))return 0;for(0;_tmp70E != 0;(_tmp70E=_tmp70E->tl,_tmp70F=_tmp70F->tl)){
struct Cyc_Absyn_Tqual _tmp711;void*_tmp712;struct _tuple9 _tmp710=*((struct _tuple9*)
_tmp70E->hd);_tmp711=_tmp710.f2;_tmp712=_tmp710.f3;{struct Cyc_Absyn_Tqual _tmp714;
void*_tmp715;struct _tuple9 _tmp713=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd);_tmp714=_tmp713.f2;_tmp715=_tmp713.f3;if(_tmp714.real_const
 && !_tmp711.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp715,_tmp712))return
0;};}if(_tmp6F1.c_varargs != _tmp6F4.c_varargs)return 0;if(_tmp6F1.cyc_varargs != 0
 && _tmp6F4.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp716=*_tmp6F1.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp717=*_tmp6F4.cyc_varargs;if((_tmp717.tq).real_const
 && !(_tmp716.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp717.type,
_tmp716.type))return 0;}else{if(_tmp6F1.cyc_varargs != 0  || _tmp6F4.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6F1.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6F4.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp6F1.rgn_po,_tmp6F4.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp6F1.attributes,_tmp6F4.attributes))return 0;return 1;};_LL3A2:;_LL3A3: return 0;
_LL39B:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp1095;struct _tuple0 _tmp719=(_tmp1095.f1=Cyc_Tcutil_compress(
t1),((_tmp1095.f2=Cyc_Tcutil_compress(t2),_tmp1095)));void*_tmp71A;enum Cyc_Absyn_Size_of
_tmp71C;void*_tmp71D;enum Cyc_Absyn_Size_of _tmp71F;_LL3AC: _tmp71A=_tmp719.f1;{
struct Cyc_Absyn_IntType_struct*_tmp71B=(struct Cyc_Absyn_IntType_struct*)_tmp71A;
if(_tmp71B->tag != 6)goto _LL3AE;else{_tmp71C=_tmp71B->f2;}};_tmp71D=_tmp719.f2;{
struct Cyc_Absyn_IntType_struct*_tmp71E=(struct Cyc_Absyn_IntType_struct*)_tmp71D;
if(_tmp71E->tag != 6)goto _LL3AE;else{_tmp71F=_tmp71E->f2;}};_LL3AD: return(_tmp71C
== _tmp71F  || _tmp71C == Cyc_Absyn_Int_sz  && _tmp71F == Cyc_Absyn_Long_sz) || 
_tmp71C == Cyc_Absyn_Long_sz  && _tmp71F == Cyc_Absyn_Int_sz;_LL3AE:;_LL3AF: return 0;
_LL3AB:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp720=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp720,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp720,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp722;struct Cyc_Absyn_Tqual
_tmp723;void*_tmp724;struct _tuple11*_tmp721=(struct _tuple11*)tqs1->hd;_tmp722=*
_tmp721;_tmp723=_tmp722.f1;_tmp724=_tmp722.f2;{struct _tuple11 _tmp726;struct Cyc_Absyn_Tqual
_tmp727;void*_tmp728;struct _tuple11*_tmp725=(struct _tuple11*)tqs2->hd;_tmp726=*
_tmp725;_tmp727=_tmp726.f1;_tmp728=_tmp726.f2;if(_tmp727.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp724,_tmp728))continue;else{if(Cyc_Tcutil_unify(_tmp724,_tmp728))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp724,_tmp728))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp729=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp72B;_LL3B1: {
struct Cyc_Absyn_IntType_struct*_tmp72A=(struct Cyc_Absyn_IntType_struct*)_tmp729;
if(_tmp72A->tag != 6)goto _LL3B3;else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Char_sz)
goto _LL3B3;}}_LL3B2: return 1;_LL3B3:;_LL3B4: return 0;_LL3B0:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp72C=
t2;enum Cyc_Absyn_Size_of _tmp72E;enum Cyc_Absyn_Size_of _tmp730;_LL3B6: {struct
Cyc_Absyn_IntType_struct*_tmp72D=(struct Cyc_Absyn_IntType_struct*)_tmp72C;if(
_tmp72D->tag != 6)goto _LL3B8;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Int_sz)
goto _LL3B8;}}_LL3B7: goto _LL3B9;_LL3B8: {struct Cyc_Absyn_IntType_struct*_tmp72F=(
struct Cyc_Absyn_IntType_struct*)_tmp72C;if(_tmp72F->tag != 6)goto _LL3BA;else{
_tmp730=_tmp72F->f2;if(_tmp730 != Cyc_Absyn_Long_sz)goto _LL3BA;}}_LL3B9: if(Cyc_Tcutil_typ_kind(
t1)== Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL3B5;_LL3BA:;_LL3BB:
goto _LL3B5;_LL3B5:;}{void*_tmp731=t1;struct Cyc_Absyn_PtrInfo _tmp733;void*_tmp734;
struct Cyc_Absyn_Tqual _tmp735;struct Cyc_Absyn_PtrAtts _tmp736;void*_tmp737;union
Cyc_Absyn_Constraint*_tmp738;union Cyc_Absyn_Constraint*_tmp739;union Cyc_Absyn_Constraint*
_tmp73A;struct Cyc_Absyn_ArrayInfo _tmp73C;void*_tmp73D;struct Cyc_Absyn_Tqual
_tmp73E;struct Cyc_Absyn_Exp*_tmp73F;union Cyc_Absyn_Constraint*_tmp740;struct Cyc_Absyn_Enumdecl*
_tmp742;void*_tmp747;_LL3BD: {struct Cyc_Absyn_PointerType_struct*_tmp732=(struct
Cyc_Absyn_PointerType_struct*)_tmp731;if(_tmp732->tag != 5)goto _LL3BF;else{
_tmp733=_tmp732->f1;_tmp734=_tmp733.elt_typ;_tmp735=_tmp733.elt_tq;_tmp736=
_tmp733.ptr_atts;_tmp737=_tmp736.rgn;_tmp738=_tmp736.nullable;_tmp739=_tmp736.bounds;
_tmp73A=_tmp736.zero_term;}}_LL3BE:{void*_tmp748=t2;struct Cyc_Absyn_PtrInfo
_tmp74A;void*_tmp74B;struct Cyc_Absyn_Tqual _tmp74C;struct Cyc_Absyn_PtrAtts _tmp74D;
void*_tmp74E;union Cyc_Absyn_Constraint*_tmp74F;union Cyc_Absyn_Constraint*_tmp750;
union Cyc_Absyn_Constraint*_tmp751;_LL3CE: {struct Cyc_Absyn_PointerType_struct*
_tmp749=(struct Cyc_Absyn_PointerType_struct*)_tmp748;if(_tmp749->tag != 5)goto
_LL3D0;else{_tmp74A=_tmp749->f1;_tmp74B=_tmp74A.elt_typ;_tmp74C=_tmp74A.elt_tq;
_tmp74D=_tmp74A.ptr_atts;_tmp74E=_tmp74D.rgn;_tmp74F=_tmp74D.nullable;_tmp750=
_tmp74D.bounds;_tmp751=_tmp74D.zero_term;}}_LL3CF: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1098;struct Cyc_List_List*
_tmp1097;struct Cyc_List_List*_tmp752=(_tmp1097=_cycalloc(sizeof(*_tmp1097)),((
_tmp1097->hd=((_tmp1098=_cycalloc(sizeof(*_tmp1098)),((_tmp1098->f1=t1,((
_tmp1098->f2=t2,_tmp1098)))))),((_tmp1097->tl=0,_tmp1097)))));int _tmp753=Cyc_Tcutil_ptrsubtype(
te,_tmp752,_tmp734,_tmp74B) && (!_tmp735.real_const  || _tmp74C.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp73A,
_tmp751) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp751);
int _tmp754=_tmp753?0:((Cyc_Tcutil_bits_only(_tmp734) && Cyc_Tcutil_is_char_type(
_tmp74B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp751)) && (_tmp74C.real_const  || !_tmp735.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp739,_tmp750);if(!
bounds_ok  && !_tmp754){struct _tuple0 _tmp1099;struct _tuple0 _tmp756=(_tmp1099.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp739),((_tmp1099.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp750),_tmp1099)));
void*_tmp757;struct Cyc_Absyn_Exp*_tmp759;void*_tmp75A;struct Cyc_Absyn_Exp*
_tmp75C;_LL3D3: _tmp757=_tmp756.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp758=(
struct Cyc_Absyn_Upper_b_struct*)_tmp757;if(_tmp758->tag != 1)goto _LL3D5;else{
_tmp759=_tmp758->f1;}};_tmp75A=_tmp756.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp75B=(struct Cyc_Absyn_Upper_b_struct*)_tmp75A;if(_tmp75B->tag != 1)goto _LL3D5;
else{_tmp75C=_tmp75B->f1;}};_LL3D4: if(Cyc_Evexp_lte_const_exp(_tmp75C,_tmp759))
bounds_ok=1;goto _LL3D2;_LL3D5:;_LL3D6: bounds_ok=1;goto _LL3D2;_LL3D2:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp738) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74F))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp753  || _tmp754)) && (Cyc_Tcutil_unify(
_tmp737,_tmp74E) || Cyc_Tcenv_region_outlives(te,_tmp737,_tmp74E)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL3D0:;_LL3D1: goto _LL3CD;
_LL3CD:;}return Cyc_Absyn_Unknown_coercion;_LL3BF: {struct Cyc_Absyn_ArrayType_struct*
_tmp73B=(struct Cyc_Absyn_ArrayType_struct*)_tmp731;if(_tmp73B->tag != 9)goto
_LL3C1;else{_tmp73C=_tmp73B->f1;_tmp73D=_tmp73C.elt_type;_tmp73E=_tmp73C.tq;
_tmp73F=_tmp73C.num_elts;_tmp740=_tmp73C.zero_term;}}_LL3C0:{void*_tmp75F=t2;
struct Cyc_Absyn_ArrayInfo _tmp761;void*_tmp762;struct Cyc_Absyn_Tqual _tmp763;
struct Cyc_Absyn_Exp*_tmp764;union Cyc_Absyn_Constraint*_tmp765;_LL3D8: {struct Cyc_Absyn_ArrayType_struct*
_tmp760=(struct Cyc_Absyn_ArrayType_struct*)_tmp75F;if(_tmp760->tag != 9)goto
_LL3DA;else{_tmp761=_tmp760->f1;_tmp762=_tmp761.elt_type;_tmp763=_tmp761.tq;
_tmp764=_tmp761.num_elts;_tmp765=_tmp761.zero_term;}}_LL3D9: {int okay;okay=((
_tmp73F != 0  && _tmp764 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp740,
_tmp765)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp764,(struct Cyc_Absyn_Exp*)
_tmp73F);return(okay  && Cyc_Tcutil_unify(_tmp73D,_tmp762)) && (!_tmp73E.real_const
 || _tmp763.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL3DA:;_LL3DB: return Cyc_Absyn_Unknown_coercion;_LL3D7:;}return Cyc_Absyn_Unknown_coercion;
_LL3C1: {struct Cyc_Absyn_EnumType_struct*_tmp741=(struct Cyc_Absyn_EnumType_struct*)
_tmp731;if(_tmp741->tag != 14)goto _LL3C3;else{_tmp742=_tmp741->f2;}}_LL3C2:{void*
_tmp766=t2;struct Cyc_Absyn_Enumdecl*_tmp768;_LL3DD: {struct Cyc_Absyn_EnumType_struct*
_tmp767=(struct Cyc_Absyn_EnumType_struct*)_tmp766;if(_tmp767->tag != 14)goto
_LL3DF;else{_tmp768=_tmp767->f2;}}_LL3DE: if((_tmp742->fields != 0  && _tmp768->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp742->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp768->fields))->v))return Cyc_Absyn_No_coercion;goto _LL3DC;_LL3DF:;_LL3E0: goto
_LL3DC;_LL3DC:;}goto _LL3C4;_LL3C3: {struct Cyc_Absyn_IntType_struct*_tmp743=(
struct Cyc_Absyn_IntType_struct*)_tmp731;if(_tmp743->tag != 6)goto _LL3C5;}_LL3C4:
goto _LL3C6;_LL3C5: {struct Cyc_Absyn_FloatType_struct*_tmp744=(struct Cyc_Absyn_FloatType_struct*)
_tmp731;if(_tmp744->tag != 7)goto _LL3C7;}_LL3C6: goto _LL3C8;_LL3C7: {struct Cyc_Absyn_DoubleType_struct*
_tmp745=(struct Cyc_Absyn_DoubleType_struct*)_tmp731;if(_tmp745->tag != 8)goto
_LL3C9;}_LL3C8: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL3C9: {struct Cyc_Absyn_RgnHandleType_struct*_tmp746=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp731;if(_tmp746->tag != 16)goto _LL3CB;else{_tmp747=(void*)_tmp746->f1;}}_LL3CA:{
void*_tmp769=t2;void*_tmp76B;_LL3E2: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp76A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp769;if(_tmp76A->tag != 16)goto
_LL3E4;else{_tmp76B=(void*)_tmp76A->f1;}}_LL3E3: if(Cyc_Tcenv_region_outlives(te,
_tmp747,_tmp76B))return Cyc_Absyn_No_coercion;goto _LL3E1;_LL3E4:;_LL3E5: goto
_LL3E1;_LL3E1:;}return Cyc_Absyn_Unknown_coercion;_LL3CB:;_LL3CC: return Cyc_Absyn_Unknown_coercion;
_LL3BC:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp76C=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp109C;struct Cyc_Absyn_Cast_e_struct*
_tmp109B;e->r=(void*)((_tmp109B=_cycalloc(sizeof(*_tmp109B)),((_tmp109B[0]=((
_tmp109C.tag=15,((_tmp109C.f1=(void*)t,((_tmp109C.f2=_tmp76C,((_tmp109C.f3=0,((
_tmp109C.f4=c,_tmp109C)))))))))),_tmp109B))));}{struct Cyc_Core_Opt*_tmp109D;e->topt=((
_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D->v=t,_tmp109D))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp770=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3E7: {struct Cyc_Absyn_IntType_struct*_tmp771=(struct Cyc_Absyn_IntType_struct*)
_tmp770;if(_tmp771->tag != 6)goto _LL3E9;}_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_EnumType_struct*
_tmp772=(struct Cyc_Absyn_EnumType_struct*)_tmp770;if(_tmp772->tag != 14)goto
_LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_AnonEnumType_struct*_tmp773=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp770;if(_tmp773->tag != 15)goto _LL3ED;}
_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_TagType_struct*_tmp774=(struct Cyc_Absyn_TagType_struct*)
_tmp770;if(_tmp774->tag != 20)goto _LL3EF;}_LL3EE: return 1;_LL3EF: {struct Cyc_Absyn_Evar_struct*
_tmp775=(struct Cyc_Absyn_Evar_struct*)_tmp770;if(_tmp775->tag != 1)goto _LL3F1;}
_LL3F0: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL3F1:;_LL3F2: return 0;_LL3E6:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp776=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3F4: {struct Cyc_Absyn_FloatType_struct*_tmp777=(
struct Cyc_Absyn_FloatType_struct*)_tmp776;if(_tmp777->tag != 7)goto _LL3F6;}_LL3F5:
goto _LL3F7;_LL3F6: {struct Cyc_Absyn_DoubleType_struct*_tmp778=(struct Cyc_Absyn_DoubleType_struct*)
_tmp776;if(_tmp778->tag != 8)goto _LL3F8;}_LL3F7: return 1;_LL3F8:;_LL3F9: return 0;
_LL3F3:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp779=Cyc_Tcutil_compress(t);_LL3FB: {struct Cyc_Absyn_FnType_struct*
_tmp77A=(struct Cyc_Absyn_FnType_struct*)_tmp779;if(_tmp77A->tag != 10)goto _LL3FD;}
_LL3FC: return 1;_LL3FD:;_LL3FE: return 0;_LL3FA:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp109E;struct _tuple0 _tmp77C=(_tmp109E.f1=t1,((_tmp109E.f2=t2,_tmp109E)));
void*_tmp77D;int _tmp77F;void*_tmp780;int _tmp782;void*_tmp783;void*_tmp785;void*
_tmp787;void*_tmp789;void*_tmp78B;enum Cyc_Absyn_Sign _tmp78D;enum Cyc_Absyn_Size_of
_tmp78E;void*_tmp78F;enum Cyc_Absyn_Sign _tmp791;enum Cyc_Absyn_Size_of _tmp792;
void*_tmp793;enum Cyc_Absyn_Size_of _tmp795;void*_tmp796;enum Cyc_Absyn_Size_of
_tmp798;void*_tmp799;enum Cyc_Absyn_Sign _tmp79B;enum Cyc_Absyn_Size_of _tmp79C;
void*_tmp79D;enum Cyc_Absyn_Sign _tmp79F;enum Cyc_Absyn_Size_of _tmp7A0;void*
_tmp7A1;enum Cyc_Absyn_Sign _tmp7A3;enum Cyc_Absyn_Size_of _tmp7A4;void*_tmp7A5;
enum Cyc_Absyn_Sign _tmp7A7;enum Cyc_Absyn_Size_of _tmp7A8;void*_tmp7A9;enum Cyc_Absyn_Size_of
_tmp7AB;void*_tmp7AC;enum Cyc_Absyn_Size_of _tmp7AE;_LL400: _tmp77D=_tmp77C.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp77E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp77D;if(_tmp77E->tag != 8)goto _LL402;else{_tmp77F=_tmp77E->f1;}};_tmp780=
_tmp77C.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp781=(struct Cyc_Absyn_DoubleType_struct*)
_tmp780;if(_tmp781->tag != 8)goto _LL402;else{_tmp782=_tmp781->f1;}};_LL401: if(
_tmp77F)return t1;else{return t2;}_LL402: _tmp783=_tmp77C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp784=(struct Cyc_Absyn_DoubleType_struct*)_tmp783;if(_tmp784->tag != 8)goto
_LL404;};_LL403: return t1;_LL404: _tmp785=_tmp77C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp786=(struct Cyc_Absyn_DoubleType_struct*)_tmp785;if(_tmp786->tag != 8)goto
_LL406;};_LL405: return t2;_LL406: _tmp787=_tmp77C.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp788=(struct Cyc_Absyn_FloatType_struct*)_tmp787;if(_tmp788->tag != 7)goto
_LL408;};_LL407: goto _LL409;_LL408: _tmp789=_tmp77C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp78A=(struct Cyc_Absyn_FloatType_struct*)_tmp789;if(_tmp78A->tag != 7)goto
_LL40A;};_LL409: return(void*)& Cyc_Absyn_FloatType_val;_LL40A: _tmp78B=_tmp77C.f1;{
struct Cyc_Absyn_IntType_struct*_tmp78C=(struct Cyc_Absyn_IntType_struct*)_tmp78B;
if(_tmp78C->tag != 6)goto _LL40C;else{_tmp78D=_tmp78C->f1;if(_tmp78D != Cyc_Absyn_Unsigned)
goto _LL40C;_tmp78E=_tmp78C->f2;if(_tmp78E != Cyc_Absyn_LongLong_sz)goto _LL40C;}};
_LL40B: goto _LL40D;_LL40C: _tmp78F=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp790=(struct Cyc_Absyn_IntType_struct*)_tmp78F;if(_tmp790->tag != 6)goto _LL40E;
else{_tmp791=_tmp790->f1;if(_tmp791 != Cyc_Absyn_Unsigned)goto _LL40E;_tmp792=
_tmp790->f2;if(_tmp792 != Cyc_Absyn_LongLong_sz)goto _LL40E;}};_LL40D: return Cyc_Absyn_ulonglong_typ;
_LL40E: _tmp793=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*_tmp794=(struct Cyc_Absyn_IntType_struct*)
_tmp793;if(_tmp794->tag != 6)goto _LL410;else{_tmp795=_tmp794->f2;if(_tmp795 != Cyc_Absyn_LongLong_sz)
goto _LL410;}};_LL40F: goto _LL411;_LL410: _tmp796=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp797=(struct Cyc_Absyn_IntType_struct*)_tmp796;if(_tmp797->tag != 6)goto _LL412;
else{_tmp798=_tmp797->f2;if(_tmp798 != Cyc_Absyn_LongLong_sz)goto _LL412;}};_LL411:
return Cyc_Absyn_slonglong_typ;_LL412: _tmp799=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp79A=(struct Cyc_Absyn_IntType_struct*)_tmp799;if(_tmp79A->tag != 6)goto _LL414;
else{_tmp79B=_tmp79A->f1;if(_tmp79B != Cyc_Absyn_Unsigned)goto _LL414;_tmp79C=
_tmp79A->f2;if(_tmp79C != Cyc_Absyn_Long_sz)goto _LL414;}};_LL413: goto _LL415;
_LL414: _tmp79D=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp79E=(struct Cyc_Absyn_IntType_struct*)
_tmp79D;if(_tmp79E->tag != 6)goto _LL416;else{_tmp79F=_tmp79E->f1;if(_tmp79F != Cyc_Absyn_Unsigned)
goto _LL416;_tmp7A0=_tmp79E->f2;if(_tmp7A0 != Cyc_Absyn_Long_sz)goto _LL416;}};
_LL415: return Cyc_Absyn_ulong_typ;_LL416: _tmp7A1=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7A2=(struct Cyc_Absyn_IntType_struct*)_tmp7A1;if(_tmp7A2->tag != 6)goto _LL418;
else{_tmp7A3=_tmp7A2->f1;if(_tmp7A3 != Cyc_Absyn_Unsigned)goto _LL418;_tmp7A4=
_tmp7A2->f2;if(_tmp7A4 != Cyc_Absyn_Int_sz)goto _LL418;}};_LL417: goto _LL419;_LL418:
_tmp7A5=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp7A6=(struct Cyc_Absyn_IntType_struct*)
_tmp7A5;if(_tmp7A6->tag != 6)goto _LL41A;else{_tmp7A7=_tmp7A6->f1;if(_tmp7A7 != Cyc_Absyn_Unsigned)
goto _LL41A;_tmp7A8=_tmp7A6->f2;if(_tmp7A8 != Cyc_Absyn_Int_sz)goto _LL41A;}};
_LL419: return Cyc_Absyn_uint_typ;_LL41A: _tmp7A9=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7AA=(struct Cyc_Absyn_IntType_struct*)_tmp7A9;if(_tmp7AA->tag != 6)goto _LL41C;
else{_tmp7AB=_tmp7AA->f2;if(_tmp7AB != Cyc_Absyn_Long_sz)goto _LL41C;}};_LL41B:
goto _LL41D;_LL41C: _tmp7AC=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp7AD=(
struct Cyc_Absyn_IntType_struct*)_tmp7AC;if(_tmp7AD->tag != 6)goto _LL41E;else{
_tmp7AE=_tmp7AD->f2;if(_tmp7AE != Cyc_Absyn_Long_sz)goto _LL41E;}};_LL41D: return
Cyc_Absyn_slong_typ;_LL41E:;_LL41F: return Cyc_Absyn_sint_typ;_LL3FF:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp7AF=e->r;struct Cyc_Core_Opt*_tmp7B1;_LL421: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp7B0=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp7AF;if(_tmp7B0->tag != 4)goto
_LL423;else{_tmp7B1=_tmp7B0->f2;if(_tmp7B1 != 0)goto _LL423;}}_LL422:{const char*
_tmp10A1;void*_tmp10A0;(_tmp10A0=0,Cyc_Tcutil_warn(e->loc,((_tmp10A1="assignment in test",
_tag_dyneither(_tmp10A1,sizeof(char),19))),_tag_dyneither(_tmp10A0,sizeof(void*),
0)));}goto _LL420;_LL423:;_LL424: goto _LL420;_LL420:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp10A2;struct _tuple0 _tmp7B5=(
_tmp10A2.f1=c1,((_tmp10A2.f2=c2,_tmp10A2)));void*_tmp7B6;enum Cyc_Absyn_Kind
_tmp7B8;void*_tmp7B9;enum Cyc_Absyn_Kind _tmp7BB;void*_tmp7BC;struct Cyc_Core_Opt*
_tmp7BE;struct Cyc_Core_Opt**_tmp7BF;void*_tmp7C0;struct Cyc_Core_Opt*_tmp7C2;
struct Cyc_Core_Opt**_tmp7C3;void*_tmp7C4;struct Cyc_Core_Opt*_tmp7C6;struct Cyc_Core_Opt**
_tmp7C7;enum Cyc_Absyn_Kind _tmp7C8;void*_tmp7C9;enum Cyc_Absyn_Kind _tmp7CB;void*
_tmp7CC;enum Cyc_Absyn_Kind _tmp7CE;void*_tmp7CF;struct Cyc_Core_Opt*_tmp7D1;
struct Cyc_Core_Opt**_tmp7D2;enum Cyc_Absyn_Kind _tmp7D3;void*_tmp7D4;struct Cyc_Core_Opt*
_tmp7D6;struct Cyc_Core_Opt**_tmp7D7;enum Cyc_Absyn_Kind _tmp7D8;void*_tmp7D9;
struct Cyc_Core_Opt*_tmp7DB;struct Cyc_Core_Opt**_tmp7DC;enum Cyc_Absyn_Kind
_tmp7DD;_LL426: _tmp7B6=_tmp7B5.f1;{struct Cyc_Absyn_Eq_kb_struct*_tmp7B7=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B6;if(_tmp7B7->tag != 0)goto _LL428;else{_tmp7B8=
_tmp7B7->f1;}};_tmp7B9=_tmp7B5.f2;{struct Cyc_Absyn_Eq_kb_struct*_tmp7BA=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B9;if(_tmp7BA->tag != 0)goto _LL428;else{_tmp7BB=
_tmp7BA->f1;}};_LL427: return _tmp7B8 == _tmp7BB;_LL428: _tmp7BC=_tmp7B5.f2;{struct
Cyc_Absyn_Unknown_kb_struct*_tmp7BD=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7BC;
if(_tmp7BD->tag != 1)goto _LL42A;else{_tmp7BE=_tmp7BD->f1;_tmp7BF=(struct Cyc_Core_Opt**)&
_tmp7BD->f1;}};_LL429:{struct Cyc_Core_Opt*_tmp10A3;*_tmp7BF=((_tmp10A3=_cycalloc(
sizeof(*_tmp10A3)),((_tmp10A3->v=c1,_tmp10A3))));}return 1;_LL42A: _tmp7C0=_tmp7B5.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp7C1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7C0;if(_tmp7C1->tag != 1)goto _LL42C;else{_tmp7C2=_tmp7C1->f1;_tmp7C3=(struct
Cyc_Core_Opt**)& _tmp7C1->f1;}};_LL42B:{struct Cyc_Core_Opt*_tmp10A4;*_tmp7C3=((
_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4->v=c2,_tmp10A4))));}return 1;
_LL42C: _tmp7C4=_tmp7B5.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp7C5=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7C4;if(_tmp7C5->tag != 2)goto _LL42E;else{_tmp7C6=_tmp7C5->f1;_tmp7C7=(struct
Cyc_Core_Opt**)& _tmp7C5->f1;_tmp7C8=_tmp7C5->f2;}};_tmp7C9=_tmp7B5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7CA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7C9;if(_tmp7CA->tag != 0)goto _LL42E;
else{_tmp7CB=_tmp7CA->f1;}};_LL42D: if(Cyc_Tcutil_kind_leq(_tmp7CB,_tmp7C8)){{
struct Cyc_Core_Opt*_tmp10A5;*_tmp7C7=((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((
_tmp10A5->v=c2,_tmp10A5))));}return 1;}else{return 0;}_LL42E: _tmp7CC=_tmp7B5.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp7CD=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7CC;if(
_tmp7CD->tag != 0)goto _LL430;else{_tmp7CE=_tmp7CD->f1;}};_tmp7CF=_tmp7B5.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp7D0=(struct Cyc_Absyn_Less_kb_struct*)_tmp7CF;
if(_tmp7D0->tag != 2)goto _LL430;else{_tmp7D1=_tmp7D0->f1;_tmp7D2=(struct Cyc_Core_Opt**)&
_tmp7D0->f1;_tmp7D3=_tmp7D0->f2;}};_LL42F: if(Cyc_Tcutil_kind_leq(_tmp7CE,_tmp7D3)){{
struct Cyc_Core_Opt*_tmp10A6;*_tmp7D2=((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((
_tmp10A6->v=c1,_tmp10A6))));}return 1;}else{return 0;}_LL430: _tmp7D4=_tmp7B5.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp7D5=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D4;
if(_tmp7D5->tag != 2)goto _LL425;else{_tmp7D6=_tmp7D5->f1;_tmp7D7=(struct Cyc_Core_Opt**)&
_tmp7D5->f1;_tmp7D8=_tmp7D5->f2;}};_tmp7D9=_tmp7B5.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp7DA=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D9;if(_tmp7DA->tag != 2)goto _LL425;
else{_tmp7DB=_tmp7DA->f1;_tmp7DC=(struct Cyc_Core_Opt**)& _tmp7DA->f1;_tmp7DD=
_tmp7DA->f2;}};_LL431: if(Cyc_Tcutil_kind_leq(_tmp7D8,_tmp7DD)){{struct Cyc_Core_Opt*
_tmp10A7;*_tmp7DC=((_tmp10A7=_cycalloc(sizeof(*_tmp10A7)),((_tmp10A7->v=c1,
_tmp10A7))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp7DD,_tmp7D8)){{struct Cyc_Core_Opt*
_tmp10A8;*_tmp7D7=((_tmp10A8=_cycalloc(sizeof(*_tmp10A8)),((_tmp10A8->v=c2,
_tmp10A8))));}return 1;}else{return 0;}}_LL425:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp10AC;void*_tmp10AB[1];struct Cyc_Int_pa_struct _tmp10AA;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp10AA.tag=1,((_tmp10AA.f1=(
unsigned long)i,((_tmp10AB[0]=& _tmp10AA,Cyc_aprintf(((_tmp10AC="#%d",
_tag_dyneither(_tmp10AC,sizeof(char),4))),_tag_dyneither(_tmp10AB,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp10AF;struct Cyc_Absyn_Tvar*_tmp10AE;return(
_tmp10AE=_cycalloc(sizeof(*_tmp10AE)),((_tmp10AE->name=((_tmp10AF=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp10AF[0]=s,_tmp10AF)))),((_tmp10AE->identity=
- 1,((_tmp10AE->kind=k,_tmp10AE)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp7E9=*t->name;return*((const char*)_check_dyneither_subscript(_tmp7E9,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp10B3;void*
_tmp10B2[1];struct Cyc_String_pa_struct _tmp10B1;(_tmp10B1.tag=0,((_tmp10B1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp10B2[0]=& _tmp10B1,
Cyc_printf(((_tmp10B3="%s",_tag_dyneither(_tmp10B3,sizeof(char),3))),
_tag_dyneither(_tmp10B2,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp10B4;struct _dyneither_ptr _tmp7ED=Cyc_strconcat(((_tmp10B4="`",
_tag_dyneither(_tmp10B4,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp10B7;char _tmp10B6;struct _dyneither_ptr _tmp10B5;(_tmp10B5=_dyneither_ptr_plus(
_tmp7ED,sizeof(char),1),((_tmp10B6=*((char*)_check_dyneither_subscript(_tmp10B5,
sizeof(char),0)),((_tmp10B7='t',((_get_dyneither_size(_tmp10B5,sizeof(char))== 1
 && (_tmp10B6 == '\000'  && _tmp10B7 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp10B5.curr)=_tmp10B7)))))));}{struct _dyneither_ptr*_tmp10B8;t->name=((
_tmp10B8=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp10B8[0]=(struct
_dyneither_ptr)_tmp7ED,_tmp10B8))));};};}struct _tuple20{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple20*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple20*x){
struct Cyc_Core_Opt*_tmp10BB;struct _tuple9*_tmp10BA;return(_tmp10BA=_cycalloc(
sizeof(*_tmp10BA)),((_tmp10BA->f1=(struct Cyc_Core_Opt*)((_tmp10BB=_cycalloc(
sizeof(*_tmp10BB)),((_tmp10BB->v=(*x).f1,_tmp10BB)))),((_tmp10BA->f2=(*x).f2,((
_tmp10BA->f3=(*x).f3,_tmp10BA)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp7F5=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp10BC;_tmp7F5=((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC->hd=(void*)
atts->hd,((_tmp10BC->tl=_tmp7F5,_tmp10BC))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp10C2;struct Cyc_Absyn_FnInfo _tmp10C1;struct Cyc_Absyn_FnType_struct*_tmp10C0;
return(void*)((_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0[0]=((_tmp10C2.tag=
10,((_tmp10C2.f1=((_tmp10C1.tvars=fd->tvs,((_tmp10C1.effect=fd->effect,((
_tmp10C1.ret_typ=fd->ret_type,((_tmp10C1.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp10C1.c_varargs=fd->c_varargs,((_tmp10C1.cyc_varargs=fd->cyc_varargs,((
_tmp10C1.rgn_po=fd->rgn_po,((_tmp10C1.attributes=_tmp7F5,_tmp10C1)))))))))))))))),
_tmp10C2)))),_tmp10C0))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple21*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple21*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp10C3;return(_tmp10C3=_cycalloc(sizeof(*_tmp10C3)),((_tmp10C3->f1=(*
pr).f1,((_tmp10C3->f2=t,_tmp10C3)))));}struct _tuple22{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple23{struct _tuple22*f1;void*f2;};static
struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple22*_tmp10C6;struct _tuple23*_tmp10C5;return(_tmp10C5=
_region_malloc(rgn,sizeof(*_tmp10C5)),((_tmp10C5->f1=((_tmp10C6=_region_malloc(
rgn,sizeof(*_tmp10C6)),((_tmp10C6->f1=(*y).f1,((_tmp10C6->f2=(*y).f2,_tmp10C6)))))),((
_tmp10C5->f2=(*y).f3,_tmp10C5)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple23*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple23*w){
struct _tuple22*_tmp7FE;void*_tmp7FF;struct _tuple23 _tmp7FD=*w;_tmp7FE=_tmp7FD.f1;
_tmp7FF=_tmp7FD.f2;{struct Cyc_Core_Opt*_tmp801;struct Cyc_Absyn_Tqual _tmp802;
struct _tuple22 _tmp800=*_tmp7FE;_tmp801=_tmp800.f1;_tmp802=_tmp800.f2;{struct
_tuple9*_tmp10C7;return(_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7->f1=
_tmp801,((_tmp10C7->f2=_tmp802,((_tmp10C7->f3=_tmp7FF,_tmp10C7)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp10C8;return(
_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8->name=f->name,((_tmp10C8->tq=f->tq,((
_tmp10C8->type=t,((_tmp10C8->width=f->width,((_tmp10C8->attributes=f->attributes,
_tmp10C8)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp10C9;
return(_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9->topt=old->topt,((
_tmp10C9->r=r,((_tmp10C9->loc=old->loc,((_tmp10C9->annot=old->annot,_tmp10C9)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp806=e->r;
enum Cyc_Absyn_Primop _tmp80C;struct Cyc_List_List*_tmp80D;struct Cyc_Absyn_Exp*
_tmp80F;struct Cyc_Absyn_Exp*_tmp810;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*
_tmp813;struct Cyc_Absyn_Exp*_tmp814;struct Cyc_Absyn_Exp*_tmp816;struct Cyc_Absyn_Exp*
_tmp817;struct Cyc_Absyn_Exp*_tmp819;struct Cyc_Absyn_Exp*_tmp81A;void*_tmp81C;
struct Cyc_Absyn_Exp*_tmp81D;int _tmp81E;enum Cyc_Absyn_Coercion _tmp81F;void*
_tmp821;struct Cyc_Absyn_Exp*_tmp823;void*_tmp825;void*_tmp826;void*_tmp828;
_LL433: {struct Cyc_Absyn_Const_e_struct*_tmp807=(struct Cyc_Absyn_Const_e_struct*)
_tmp806;if(_tmp807->tag != 0)goto _LL435;}_LL434: goto _LL436;_LL435: {struct Cyc_Absyn_Enum_e_struct*
_tmp808=(struct Cyc_Absyn_Enum_e_struct*)_tmp806;if(_tmp808->tag != 32)goto _LL437;}
_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp809=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp806;if(_tmp809->tag != 33)goto _LL439;}_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Var_e_struct*
_tmp80A=(struct Cyc_Absyn_Var_e_struct*)_tmp806;if(_tmp80A->tag != 1)goto _LL43B;}
_LL43A: return e;_LL43B: {struct Cyc_Absyn_Primop_e_struct*_tmp80B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp806;if(_tmp80B->tag != 3)goto _LL43D;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}
_LL43C: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp80D)== 1){struct
Cyc_Absyn_Exp*_tmp829=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp80D))->hd;struct Cyc_Absyn_Exp*_tmp82A=Cyc_Tcutil_rsubsexp(r,inst,_tmp829);
if(_tmp82A == _tmp829)return e;{struct Cyc_Absyn_Primop_e_struct _tmp10CF;struct Cyc_Absyn_Exp*
_tmp10CE[1];struct Cyc_Absyn_Primop_e_struct*_tmp10CD;return Cyc_Tcutil_copye(e,(
void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CF.tag=3,((
_tmp10CF.f1=_tmp80C,((_tmp10CF.f2=((_tmp10CE[0]=_tmp82A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp10CE,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp10CF)))))),_tmp10CD)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp80D)== 2){struct Cyc_Absyn_Exp*_tmp82E=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp80D))->hd;struct Cyc_Absyn_Exp*_tmp82F=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp80D->tl))->hd;struct
Cyc_Absyn_Exp*_tmp830=Cyc_Tcutil_rsubsexp(r,inst,_tmp82E);struct Cyc_Absyn_Exp*
_tmp831=Cyc_Tcutil_rsubsexp(r,inst,_tmp82F);if(_tmp830 == _tmp82E  && _tmp831 == 
_tmp82F)return e;{struct Cyc_Absyn_Primop_e_struct _tmp10D5;struct Cyc_Absyn_Exp*
_tmp10D4[2];struct Cyc_Absyn_Primop_e_struct*_tmp10D3;return Cyc_Tcutil_copye(e,(
void*)((_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3[0]=((_tmp10D5.tag=3,((
_tmp10D5.f1=_tmp80C,((_tmp10D5.f2=((_tmp10D4[1]=_tmp831,((_tmp10D4[0]=_tmp830,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp10D4,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp10D5)))))),_tmp10D3)))));};}
else{const char*_tmp10D8;void*_tmp10D7;return(_tmp10D7=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D8="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp10D8,sizeof(char),34))),_tag_dyneither(_tmp10D7,sizeof(void*),
0)));}}_LL43D: {struct Cyc_Absyn_Conditional_e_struct*_tmp80E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp806;if(_tmp80E->tag != 6)goto _LL43F;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;
_tmp811=_tmp80E->f3;}}_LL43E: {struct Cyc_Absyn_Exp*_tmp837=Cyc_Tcutil_rsubsexp(r,
inst,_tmp80F);struct Cyc_Absyn_Exp*_tmp838=Cyc_Tcutil_rsubsexp(r,inst,_tmp810);
struct Cyc_Absyn_Exp*_tmp839=Cyc_Tcutil_rsubsexp(r,inst,_tmp811);if((_tmp837 == 
_tmp80F  && _tmp838 == _tmp810) && _tmp839 == _tmp811)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp10DB;struct Cyc_Absyn_Conditional_e_struct*_tmp10DA;return Cyc_Tcutil_copye(e,(
void*)((_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA[0]=((_tmp10DB.tag=6,((
_tmp10DB.f1=_tmp837,((_tmp10DB.f2=_tmp838,((_tmp10DB.f3=_tmp839,_tmp10DB)))))))),
_tmp10DA)))));};}_LL43F: {struct Cyc_Absyn_And_e_struct*_tmp812=(struct Cyc_Absyn_And_e_struct*)
_tmp806;if(_tmp812->tag != 7)goto _LL441;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}
_LL440: {struct Cyc_Absyn_Exp*_tmp83C=Cyc_Tcutil_rsubsexp(r,inst,_tmp813);struct
Cyc_Absyn_Exp*_tmp83D=Cyc_Tcutil_rsubsexp(r,inst,_tmp814);if(_tmp83C == _tmp813
 && _tmp83D == _tmp814)return e;{struct Cyc_Absyn_And_e_struct _tmp10DE;struct Cyc_Absyn_And_e_struct*
_tmp10DD;return Cyc_Tcutil_copye(e,(void*)((_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((
_tmp10DD[0]=((_tmp10DE.tag=7,((_tmp10DE.f1=_tmp83C,((_tmp10DE.f2=_tmp83D,
_tmp10DE)))))),_tmp10DD)))));};}_LL441: {struct Cyc_Absyn_Or_e_struct*_tmp815=(
struct Cyc_Absyn_Or_e_struct*)_tmp806;if(_tmp815->tag != 8)goto _LL443;else{_tmp816=
_tmp815->f1;_tmp817=_tmp815->f2;}}_LL442: {struct Cyc_Absyn_Exp*_tmp840=Cyc_Tcutil_rsubsexp(
r,inst,_tmp816);struct Cyc_Absyn_Exp*_tmp841=Cyc_Tcutil_rsubsexp(r,inst,_tmp817);
if(_tmp840 == _tmp816  && _tmp841 == _tmp817)return e;{struct Cyc_Absyn_Or_e_struct
_tmp10E1;struct Cyc_Absyn_Or_e_struct*_tmp10E0;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0[0]=((_tmp10E1.tag=8,((_tmp10E1.f1=
_tmp840,((_tmp10E1.f2=_tmp841,_tmp10E1)))))),_tmp10E0)))));};}_LL443: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp818=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp806;if(_tmp818->tag != 9)goto _LL445;
else{_tmp819=_tmp818->f1;_tmp81A=_tmp818->f2;}}_LL444: {struct Cyc_Absyn_Exp*
_tmp844=Cyc_Tcutil_rsubsexp(r,inst,_tmp819);struct Cyc_Absyn_Exp*_tmp845=Cyc_Tcutil_rsubsexp(
r,inst,_tmp81A);if(_tmp844 == _tmp819  && _tmp845 == _tmp81A)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp10E4;struct Cyc_Absyn_SeqExp_e_struct*_tmp10E3;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3[0]=((_tmp10E4.tag=9,((_tmp10E4.f1=
_tmp844,((_tmp10E4.f2=_tmp845,_tmp10E4)))))),_tmp10E3)))));};}_LL445: {struct Cyc_Absyn_Cast_e_struct*
_tmp81B=(struct Cyc_Absyn_Cast_e_struct*)_tmp806;if(_tmp81B->tag != 15)goto _LL447;
else{_tmp81C=(void*)_tmp81B->f1;_tmp81D=_tmp81B->f2;_tmp81E=_tmp81B->f3;_tmp81F=
_tmp81B->f4;}}_LL446: {struct Cyc_Absyn_Exp*_tmp848=Cyc_Tcutil_rsubsexp(r,inst,
_tmp81D);void*_tmp849=Cyc_Tcutil_rsubstitute(r,inst,_tmp81C);if(_tmp848 == 
_tmp81D  && _tmp849 == _tmp81C)return e;{struct Cyc_Absyn_Cast_e_struct _tmp10E7;
struct Cyc_Absyn_Cast_e_struct*_tmp10E6;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E6=_cycalloc(sizeof(*_tmp10E6)),((_tmp10E6[0]=((_tmp10E7.tag=15,((_tmp10E7.f1=(
void*)_tmp849,((_tmp10E7.f2=_tmp848,((_tmp10E7.f3=_tmp81E,((_tmp10E7.f4=_tmp81F,
_tmp10E7)))))))))),_tmp10E6)))));};}_LL447: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp820=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp806;if(_tmp820->tag != 18)goto
_LL449;else{_tmp821=(void*)_tmp820->f1;}}_LL448: {void*_tmp84C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp821);if(_tmp84C == _tmp821)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp10EA;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp10E9;return Cyc_Tcutil_copye(e,(
void*)((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9[0]=((_tmp10EA.tag=18,((
_tmp10EA.f1=(void*)_tmp84C,_tmp10EA)))),_tmp10E9)))));};}_LL449: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp822=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp806;if(_tmp822->tag != 19)goto
_LL44B;else{_tmp823=_tmp822->f1;}}_LL44A: {struct Cyc_Absyn_Exp*_tmp84F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp823);if(_tmp84F == _tmp823)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp10ED;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp10EC;return Cyc_Tcutil_copye(e,(
void*)((_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((_tmp10EC[0]=((_tmp10ED.tag=19,((
_tmp10ED.f1=_tmp84F,_tmp10ED)))),_tmp10EC)))));};}_LL44B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp824=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp806;if(_tmp824->tag != 20)goto
_LL44D;else{_tmp825=(void*)_tmp824->f1;_tmp826=(void*)_tmp824->f2;}}_LL44C: {
void*_tmp852=Cyc_Tcutil_rsubstitute(r,inst,_tmp825);if(_tmp852 == _tmp825)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp10F0;struct Cyc_Absyn_Offsetof_e_struct*
_tmp10EF;return Cyc_Tcutil_copye(e,(void*)((_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((
_tmp10EF[0]=((_tmp10F0.tag=20,((_tmp10F0.f1=(void*)_tmp852,((_tmp10F0.f2=(void*)
_tmp826,_tmp10F0)))))),_tmp10EF)))));};}_LL44D: {struct Cyc_Absyn_Valueof_e_struct*
_tmp827=(struct Cyc_Absyn_Valueof_e_struct*)_tmp806;if(_tmp827->tag != 39)goto
_LL44F;else{_tmp828=(void*)_tmp827->f1;}}_LL44E: {void*_tmp855=Cyc_Tcutil_rsubstitute(
r,inst,_tmp828);if(_tmp855 == _tmp828)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp10F3;struct Cyc_Absyn_Valueof_e_struct*_tmp10F2;return Cyc_Tcutil_copye(e,(
void*)((_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2[0]=((_tmp10F3.tag=39,((
_tmp10F3.f1=(void*)_tmp855,_tmp10F3)))),_tmp10F2)))));};}_LL44F:;_LL450: {const
char*_tmp10F6;void*_tmp10F5;return(_tmp10F5=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F6="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp10F6,sizeof(char),46))),_tag_dyneither(_tmp10F5,sizeof(void*),
0)));}_LL432:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp85A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp85C;
struct Cyc_Absyn_AggrInfo _tmp85E;union Cyc_Absyn_AggrInfoU _tmp85F;struct Cyc_List_List*
_tmp860;struct Cyc_Absyn_DatatypeInfo _tmp862;union Cyc_Absyn_DatatypeInfoU _tmp863;
struct Cyc_List_List*_tmp864;struct Cyc_Absyn_DatatypeFieldInfo _tmp866;union Cyc_Absyn_DatatypeFieldInfoU
_tmp867;struct Cyc_List_List*_tmp868;struct _tuple2*_tmp86A;struct Cyc_List_List*
_tmp86B;struct Cyc_Absyn_Typedefdecl*_tmp86C;void**_tmp86D;struct Cyc_Absyn_ArrayInfo
_tmp86F;void*_tmp870;struct Cyc_Absyn_Tqual _tmp871;struct Cyc_Absyn_Exp*_tmp872;
union Cyc_Absyn_Constraint*_tmp873;struct Cyc_Position_Segment*_tmp874;struct Cyc_Absyn_PtrInfo
_tmp876;void*_tmp877;struct Cyc_Absyn_Tqual _tmp878;struct Cyc_Absyn_PtrAtts _tmp879;
void*_tmp87A;union Cyc_Absyn_Constraint*_tmp87B;union Cyc_Absyn_Constraint*_tmp87C;
union Cyc_Absyn_Constraint*_tmp87D;struct Cyc_Absyn_FnInfo _tmp87F;struct Cyc_List_List*
_tmp880;struct Cyc_Core_Opt*_tmp881;void*_tmp882;struct Cyc_List_List*_tmp883;int
_tmp884;struct Cyc_Absyn_VarargInfo*_tmp885;struct Cyc_List_List*_tmp886;struct Cyc_List_List*
_tmp887;struct Cyc_List_List*_tmp889;enum Cyc_Absyn_AggrKind _tmp88B;struct Cyc_List_List*
_tmp88C;struct Cyc_Core_Opt*_tmp88E;void*_tmp890;void*_tmp892;void*_tmp893;void*
_tmp895;struct Cyc_Absyn_Exp*_tmp897;void*_tmp8A1;void*_tmp8A3;struct Cyc_List_List*
_tmp8A5;_LL452: {struct Cyc_Absyn_VarType_struct*_tmp85B=(struct Cyc_Absyn_VarType_struct*)
_tmp85A;if(_tmp85B->tag != 2)goto _LL454;else{_tmp85C=_tmp85B->f1;}}_LL453: {
struct _handler_cons _tmp8A6;_push_handler(& _tmp8A6);{int _tmp8A8=0;if(setjmp(
_tmp8A6.handler))_tmp8A8=1;if(!_tmp8A8){{void*_tmp8A9=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp85C);_npop_handler(0);return
_tmp8A9;};_pop_handler();}else{void*_tmp8A7=(void*)_exn_thrown;void*_tmp8AB=
_tmp8A7;_LL487: {struct Cyc_Core_Not_found_struct*_tmp8AC=(struct Cyc_Core_Not_found_struct*)
_tmp8AB;if(_tmp8AC->tag != Cyc_Core_Not_found)goto _LL489;}_LL488: return t;_LL489:;
_LL48A:(void)_throw(_tmp8AB);_LL486:;}};}_LL454: {struct Cyc_Absyn_AggrType_struct*
_tmp85D=(struct Cyc_Absyn_AggrType_struct*)_tmp85A;if(_tmp85D->tag != 12)goto
_LL456;else{_tmp85E=_tmp85D->f1;_tmp85F=_tmp85E.aggr_info;_tmp860=_tmp85E.targs;}}
_LL455: {struct Cyc_List_List*_tmp8AD=Cyc_Tcutil_substs(rgn,inst,_tmp860);struct
Cyc_Absyn_AggrType_struct _tmp10FC;struct Cyc_Absyn_AggrInfo _tmp10FB;struct Cyc_Absyn_AggrType_struct*
_tmp10FA;return _tmp8AD == _tmp860?t:(void*)((_tmp10FA=_cycalloc(sizeof(*_tmp10FA)),((
_tmp10FA[0]=((_tmp10FC.tag=12,((_tmp10FC.f1=((_tmp10FB.aggr_info=_tmp85F,((
_tmp10FB.targs=_tmp8AD,_tmp10FB)))),_tmp10FC)))),_tmp10FA))));}_LL456: {struct
Cyc_Absyn_DatatypeType_struct*_tmp861=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp85A;if(_tmp861->tag != 3)goto _LL458;else{_tmp862=_tmp861->f1;_tmp863=_tmp862.datatype_info;
_tmp864=_tmp862.targs;}}_LL457: {struct Cyc_List_List*_tmp8B1=Cyc_Tcutil_substs(
rgn,inst,_tmp864);struct Cyc_Absyn_DatatypeType_struct _tmp1102;struct Cyc_Absyn_DatatypeInfo
_tmp1101;struct Cyc_Absyn_DatatypeType_struct*_tmp1100;return _tmp8B1 == _tmp864?t:(
void*)((_tmp1100=_cycalloc(sizeof(*_tmp1100)),((_tmp1100[0]=((_tmp1102.tag=3,((
_tmp1102.f1=((_tmp1101.datatype_info=_tmp863,((_tmp1101.targs=_tmp8B1,_tmp1101)))),
_tmp1102)))),_tmp1100))));}_LL458: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp865=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp85A;if(_tmp865->tag != 4)
goto _LL45A;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.field_info;_tmp868=_tmp866.targs;}}
_LL459: {struct Cyc_List_List*_tmp8B5=Cyc_Tcutil_substs(rgn,inst,_tmp868);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp1108;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1107;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1106;return _tmp8B5 == 
_tmp868?t:(void*)((_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((
_tmp1108.tag=4,((_tmp1108.f1=((_tmp1107.field_info=_tmp867,((_tmp1107.targs=
_tmp8B5,_tmp1107)))),_tmp1108)))),_tmp1106))));}_LL45A: {struct Cyc_Absyn_TypedefType_struct*
_tmp869=(struct Cyc_Absyn_TypedefType_struct*)_tmp85A;if(_tmp869->tag != 18)goto
_LL45C;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp869->f2;_tmp86C=_tmp869->f3;_tmp86D=
_tmp869->f4;}}_LL45B: {struct Cyc_List_List*_tmp8B9=Cyc_Tcutil_substs(rgn,inst,
_tmp86B);struct Cyc_Absyn_TypedefType_struct _tmp110B;struct Cyc_Absyn_TypedefType_struct*
_tmp110A;return _tmp8B9 == _tmp86B?t:(void*)((_tmp110A=_cycalloc(sizeof(*_tmp110A)),((
_tmp110A[0]=((_tmp110B.tag=18,((_tmp110B.f1=_tmp86A,((_tmp110B.f2=_tmp8B9,((
_tmp110B.f3=_tmp86C,((_tmp110B.f4=_tmp86D,_tmp110B)))))))))),_tmp110A))));}
_LL45C: {struct Cyc_Absyn_ArrayType_struct*_tmp86E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp85A;if(_tmp86E->tag != 9)goto _LL45E;else{_tmp86F=_tmp86E->f1;_tmp870=_tmp86F.elt_type;
_tmp871=_tmp86F.tq;_tmp872=_tmp86F.num_elts;_tmp873=_tmp86F.zero_term;_tmp874=
_tmp86F.zt_loc;}}_LL45D: {void*_tmp8BC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp870);
struct Cyc_Absyn_ArrayType_struct _tmp1111;struct Cyc_Absyn_ArrayInfo _tmp1110;
struct Cyc_Absyn_ArrayType_struct*_tmp110F;return _tmp8BC == _tmp870?t:(void*)((
_tmp110F=_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((_tmp1111.tag=9,((_tmp1111.f1=((
_tmp1110.elt_type=_tmp8BC,((_tmp1110.tq=_tmp871,((_tmp1110.num_elts=_tmp872,((
_tmp1110.zero_term=_tmp873,((_tmp1110.zt_loc=_tmp874,_tmp1110)))))))))),_tmp1111)))),
_tmp110F))));}_LL45E: {struct Cyc_Absyn_PointerType_struct*_tmp875=(struct Cyc_Absyn_PointerType_struct*)
_tmp85A;if(_tmp875->tag != 5)goto _LL460;else{_tmp876=_tmp875->f1;_tmp877=_tmp876.elt_typ;
_tmp878=_tmp876.elt_tq;_tmp879=_tmp876.ptr_atts;_tmp87A=_tmp879.rgn;_tmp87B=
_tmp879.nullable;_tmp87C=_tmp879.bounds;_tmp87D=_tmp879.zero_term;}}_LL45F: {
void*_tmp8C0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp877);void*_tmp8C1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp87A);union Cyc_Absyn_Constraint*_tmp8C2=_tmp87C;{void*_tmp8C3=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp87C);struct Cyc_Absyn_Exp*_tmp8C5;_LL48C: {struct Cyc_Absyn_Upper_b_struct*
_tmp8C4=(struct Cyc_Absyn_Upper_b_struct*)_tmp8C3;if(_tmp8C4->tag != 1)goto _LL48E;
else{_tmp8C5=_tmp8C4->f1;}}_LL48D: {struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp8C5);if(_tmp8C6 != _tmp8C5){struct Cyc_Absyn_Upper_b_struct _tmp1114;
struct Cyc_Absyn_Upper_b_struct*_tmp1113;_tmp8C2=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1113=_cycalloc(sizeof(*_tmp1113)),((
_tmp1113[0]=((_tmp1114.tag=1,((_tmp1114.f1=_tmp8C6,_tmp1114)))),_tmp1113)))));}
goto _LL48B;}_LL48E:;_LL48F: goto _LL48B;_LL48B:;}if((_tmp8C0 == _tmp877  && _tmp8C1
== _tmp87A) && _tmp8C2 == _tmp87C)return t;{struct Cyc_Absyn_PointerType_struct
_tmp111E;struct Cyc_Absyn_PtrAtts _tmp111D;struct Cyc_Absyn_PtrInfo _tmp111C;struct
Cyc_Absyn_PointerType_struct*_tmp111B;return(void*)((_tmp111B=_cycalloc(sizeof(*
_tmp111B)),((_tmp111B[0]=((_tmp111E.tag=5,((_tmp111E.f1=((_tmp111C.elt_typ=
_tmp8C0,((_tmp111C.elt_tq=_tmp878,((_tmp111C.ptr_atts=((_tmp111D.rgn=_tmp8C1,((
_tmp111D.nullable=_tmp87B,((_tmp111D.bounds=_tmp8C2,((_tmp111D.zero_term=_tmp87D,((
_tmp111D.ptrloc=0,_tmp111D)))))))))),_tmp111C)))))),_tmp111E)))),_tmp111B))));};}
_LL460: {struct Cyc_Absyn_FnType_struct*_tmp87E=(struct Cyc_Absyn_FnType_struct*)
_tmp85A;if(_tmp87E->tag != 10)goto _LL462;else{_tmp87F=_tmp87E->f1;_tmp880=_tmp87F.tvars;
_tmp881=_tmp87F.effect;_tmp882=_tmp87F.ret_typ;_tmp883=_tmp87F.args;_tmp884=
_tmp87F.c_varargs;_tmp885=_tmp87F.cyc_varargs;_tmp886=_tmp87F.rgn_po;_tmp887=
_tmp87F.attributes;}}_LL461:{struct Cyc_List_List*_tmp8CD=_tmp880;for(0;_tmp8CD != 
0;_tmp8CD=_tmp8CD->tl){struct _tuple14*_tmp1128;struct Cyc_Absyn_VarType_struct
_tmp1127;struct Cyc_Absyn_VarType_struct*_tmp1126;struct Cyc_List_List*_tmp1125;
inst=((_tmp1125=_region_malloc(rgn,sizeof(*_tmp1125)),((_tmp1125->hd=((_tmp1128=
_region_malloc(rgn,sizeof(*_tmp1128)),((_tmp1128->f1=(struct Cyc_Absyn_Tvar*)
_tmp8CD->hd,((_tmp1128->f2=(void*)((_tmp1126=_cycalloc(sizeof(*_tmp1126)),((
_tmp1126[0]=((_tmp1127.tag=2,((_tmp1127.f1=(struct Cyc_Absyn_Tvar*)_tmp8CD->hd,
_tmp1127)))),_tmp1126)))),_tmp1128)))))),((_tmp1125->tl=inst,_tmp1125))))));}}{
struct Cyc_List_List*_tmp8D3;struct Cyc_List_List*_tmp8D4;struct _tuple1 _tmp8D2=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple23*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp883));
_tmp8D3=_tmp8D2.f1;_tmp8D4=_tmp8D2.f2;{struct Cyc_List_List*_tmp8D5=Cyc_Tcutil_substs(
rgn,inst,_tmp8D4);struct Cyc_List_List*_tmp8D6=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp8D3,_tmp8D5));struct Cyc_Core_Opt*
eff2;if(_tmp881 == 0)eff2=0;else{void*_tmp8D7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp881->v);if(_tmp8D7 == (void*)_tmp881->v)eff2=_tmp881;else{struct Cyc_Core_Opt*
_tmp1129;eff2=((_tmp1129=_cycalloc(sizeof(*_tmp1129)),((_tmp1129->v=_tmp8D7,
_tmp1129))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp885 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp8DA;struct Cyc_Absyn_Tqual _tmp8DB;void*
_tmp8DC;int _tmp8DD;struct Cyc_Absyn_VarargInfo _tmp8D9=*_tmp885;_tmp8DA=_tmp8D9.name;
_tmp8DB=_tmp8D9.tq;_tmp8DC=_tmp8D9.type;_tmp8DD=_tmp8D9.inject;{void*_tmp8DE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8DC);if(_tmp8DE == _tmp8DC)cyc_varargs2=_tmp885;else{struct Cyc_Absyn_VarargInfo*
_tmp112A;cyc_varargs2=((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A->name=
_tmp8DA,((_tmp112A->tq=_tmp8DB,((_tmp112A->type=_tmp8DE,((_tmp112A->inject=
_tmp8DD,_tmp112A))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp8E1;struct Cyc_List_List*_tmp8E2;struct _tuple1 _tmp8E0=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp886);_tmp8E1=_tmp8E0.f1;_tmp8E2=_tmp8E0.f2;{struct Cyc_List_List*
_tmp8E3=Cyc_Tcutil_substs(rgn,inst,_tmp8E1);struct Cyc_List_List*_tmp8E4=Cyc_Tcutil_substs(
rgn,inst,_tmp8E2);if(_tmp8E3 == _tmp8E1  && _tmp8E4 == _tmp8E2)rgn_po2=_tmp886;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp8E3,_tmp8E4);}{struct Cyc_Absyn_FnType_struct _tmp1130;struct
Cyc_Absyn_FnInfo _tmp112F;struct Cyc_Absyn_FnType_struct*_tmp112E;return(void*)((
_tmp112E=_cycalloc(sizeof(*_tmp112E)),((_tmp112E[0]=((_tmp1130.tag=10,((_tmp1130.f1=((
_tmp112F.tvars=_tmp880,((_tmp112F.effect=eff2,((_tmp112F.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp882),((_tmp112F.args=_tmp8D6,((_tmp112F.c_varargs=_tmp884,((
_tmp112F.cyc_varargs=cyc_varargs2,((_tmp112F.rgn_po=rgn_po2,((_tmp112F.attributes=
_tmp887,_tmp112F)))))))))))))))),_tmp1130)))),_tmp112E))));};};};};};};_LL462: {
struct Cyc_Absyn_TupleType_struct*_tmp888=(struct Cyc_Absyn_TupleType_struct*)
_tmp85A;if(_tmp888->tag != 11)goto _LL464;else{_tmp889=_tmp888->f1;}}_LL463: {
struct Cyc_List_List*_tmp8E8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp889);struct Cyc_List_List*_tmp8E9=Cyc_Tcutil_substs(rgn,inst,_tmp8E8);if(
_tmp8E9 == _tmp8E8)return t;{struct Cyc_List_List*_tmp8EA=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp889,_tmp8E9);struct Cyc_Absyn_TupleType_struct
_tmp1133;struct Cyc_Absyn_TupleType_struct*_tmp1132;return(void*)((_tmp1132=
_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((_tmp1133.tag=11,((_tmp1133.f1=
_tmp8EA,_tmp1133)))),_tmp1132))));};}_LL464: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp88A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp85A;if(_tmp88A->tag != 13)goto
_LL466;else{_tmp88B=_tmp88A->f1;_tmp88C=_tmp88A->f2;}}_LL465: {struct Cyc_List_List*
_tmp8ED=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp88C);struct
Cyc_List_List*_tmp8EE=Cyc_Tcutil_substs(rgn,inst,_tmp8ED);if(_tmp8EE == _tmp8ED)
return t;{struct Cyc_List_List*_tmp8EF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp88C,_tmp8EE);struct Cyc_Absyn_AnonAggrType_struct
_tmp1136;struct Cyc_Absyn_AnonAggrType_struct*_tmp1135;return(void*)((_tmp1135=
_cycalloc(sizeof(*_tmp1135)),((_tmp1135[0]=((_tmp1136.tag=13,((_tmp1136.f1=
_tmp88B,((_tmp1136.f2=_tmp8EF,_tmp1136)))))),_tmp1135))));};}_LL466: {struct Cyc_Absyn_Evar_struct*
_tmp88D=(struct Cyc_Absyn_Evar_struct*)_tmp85A;if(_tmp88D->tag != 1)goto _LL468;
else{_tmp88E=_tmp88D->f2;}}_LL467: if(_tmp88E != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp88E->v);else{return t;}_LL468: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp88F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp85A;if(_tmp88F->tag != 16)goto
_LL46A;else{_tmp890=(void*)_tmp88F->f1;}}_LL469: {void*_tmp8F2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp890);struct Cyc_Absyn_RgnHandleType_struct _tmp1139;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1138;return _tmp8F2 == _tmp890?t:(void*)((_tmp1138=_cycalloc(sizeof(*_tmp1138)),((
_tmp1138[0]=((_tmp1139.tag=16,((_tmp1139.f1=(void*)_tmp8F2,_tmp1139)))),_tmp1138))));}
_LL46A: {struct Cyc_Absyn_DynRgnType_struct*_tmp891=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp85A;if(_tmp891->tag != 17)goto _LL46C;else{_tmp892=(void*)_tmp891->f1;_tmp893=(
void*)_tmp891->f2;}}_LL46B: {void*_tmp8F5=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp892);void*_tmp8F6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp893);struct Cyc_Absyn_DynRgnType_struct
_tmp113C;struct Cyc_Absyn_DynRgnType_struct*_tmp113B;return _tmp8F5 == _tmp892  && 
_tmp8F6 == _tmp893?t:(void*)((_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=((
_tmp113C.tag=17,((_tmp113C.f1=(void*)_tmp8F5,((_tmp113C.f2=(void*)_tmp8F6,
_tmp113C)))))),_tmp113B))));}_LL46C: {struct Cyc_Absyn_TagType_struct*_tmp894=(
struct Cyc_Absyn_TagType_struct*)_tmp85A;if(_tmp894->tag != 20)goto _LL46E;else{
_tmp895=(void*)_tmp894->f1;}}_LL46D: {void*_tmp8F9=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp895);struct Cyc_Absyn_TagType_struct _tmp113F;struct Cyc_Absyn_TagType_struct*
_tmp113E;return _tmp8F9 == _tmp895?t:(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((
_tmp113E[0]=((_tmp113F.tag=20,((_tmp113F.f1=(void*)_tmp8F9,_tmp113F)))),_tmp113E))));}
_LL46E: {struct Cyc_Absyn_ValueofType_struct*_tmp896=(struct Cyc_Absyn_ValueofType_struct*)
_tmp85A;if(_tmp896->tag != 19)goto _LL470;else{_tmp897=_tmp896->f1;}}_LL46F: {
struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp897);struct Cyc_Absyn_ValueofType_struct
_tmp1142;struct Cyc_Absyn_ValueofType_struct*_tmp1141;return _tmp8FC == _tmp897?t:(
void*)((_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141[0]=((_tmp1142.tag=19,((
_tmp1142.f1=_tmp8FC,_tmp1142)))),_tmp1141))));}_LL470: {struct Cyc_Absyn_EnumType_struct*
_tmp898=(struct Cyc_Absyn_EnumType_struct*)_tmp85A;if(_tmp898->tag != 14)goto
_LL472;}_LL471: goto _LL473;_LL472: {struct Cyc_Absyn_AnonEnumType_struct*_tmp899=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp85A;if(_tmp899->tag != 15)goto _LL474;}
_LL473: goto _LL475;_LL474: {struct Cyc_Absyn_VoidType_struct*_tmp89A=(struct Cyc_Absyn_VoidType_struct*)
_tmp85A;if(_tmp89A->tag != 0)goto _LL476;}_LL475: goto _LL477;_LL476: {struct Cyc_Absyn_IntType_struct*
_tmp89B=(struct Cyc_Absyn_IntType_struct*)_tmp85A;if(_tmp89B->tag != 6)goto _LL478;}
_LL477: goto _LL479;_LL478: {struct Cyc_Absyn_FloatType_struct*_tmp89C=(struct Cyc_Absyn_FloatType_struct*)
_tmp85A;if(_tmp89C->tag != 7)goto _LL47A;}_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_DoubleType_struct*
_tmp89D=(struct Cyc_Absyn_DoubleType_struct*)_tmp85A;if(_tmp89D->tag != 8)goto
_LL47C;}_LL47B: goto _LL47D;_LL47C: {struct Cyc_Absyn_UniqueRgn_struct*_tmp89E=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp85A;if(_tmp89E->tag != 22)goto _LL47E;}
_LL47D: goto _LL47F;_LL47E: {struct Cyc_Absyn_HeapRgn_struct*_tmp89F=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp85A;if(_tmp89F->tag != 21)goto _LL480;}_LL47F: return t;_LL480: {struct Cyc_Absyn_RgnsEff_struct*
_tmp8A0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp85A;if(_tmp8A0->tag != 25)goto _LL482;
else{_tmp8A1=(void*)_tmp8A0->f1;}}_LL481: {void*_tmp8FF=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8A1);struct Cyc_Absyn_RgnsEff_struct _tmp1145;struct Cyc_Absyn_RgnsEff_struct*
_tmp1144;return _tmp8FF == _tmp8A1?t:(void*)((_tmp1144=_cycalloc(sizeof(*_tmp1144)),((
_tmp1144[0]=((_tmp1145.tag=25,((_tmp1145.f1=(void*)_tmp8FF,_tmp1145)))),_tmp1144))));}
_LL482: {struct Cyc_Absyn_AccessEff_struct*_tmp8A2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp85A;if(_tmp8A2->tag != 23)goto _LL484;else{_tmp8A3=(void*)_tmp8A2->f1;}}_LL483: {
void*_tmp902=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A3);struct Cyc_Absyn_AccessEff_struct
_tmp1148;struct Cyc_Absyn_AccessEff_struct*_tmp1147;return _tmp902 == _tmp8A3?t:(
void*)((_tmp1147=_cycalloc(sizeof(*_tmp1147)),((_tmp1147[0]=((_tmp1148.tag=23,((
_tmp1148.f1=(void*)_tmp902,_tmp1148)))),_tmp1147))));}_LL484: {struct Cyc_Absyn_JoinEff_struct*
_tmp8A4=(struct Cyc_Absyn_JoinEff_struct*)_tmp85A;if(_tmp8A4->tag != 24)goto _LL451;
else{_tmp8A5=_tmp8A4->f1;}}_LL485: {struct Cyc_List_List*_tmp905=Cyc_Tcutil_substs(
rgn,inst,_tmp8A5);struct Cyc_Absyn_JoinEff_struct _tmp114B;struct Cyc_Absyn_JoinEff_struct*
_tmp114A;return _tmp905 == _tmp8A5?t:(void*)((_tmp114A=_cycalloc(sizeof(*_tmp114A)),((
_tmp114A[0]=((_tmp114B.tag=24,((_tmp114B.f1=_tmp905,_tmp114B)))),_tmp114A))));}
_LL451:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmp908=(void*)ts->hd;struct Cyc_List_List*_tmp909=ts->tl;void*
_tmp90A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp908);struct Cyc_List_List*_tmp90B=Cyc_Tcutil_substs(
rgn,inst,_tmp909);if(_tmp908 == _tmp90A  && _tmp909 == _tmp90B)return ts;{struct Cyc_List_List*
_tmp114C;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp114C=_cycalloc(
sizeof(*_tmp114C)),((_tmp114C->hd=_tmp90A,((_tmp114C->tl=_tmp90B,_tmp114C)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp90D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,
Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp114F;struct _tuple14*_tmp114E;return(
_tmp114E=_cycalloc(sizeof(*_tmp114E)),((_tmp114E->f1=tv,((_tmp114E->f2=Cyc_Absyn_new_evar(
_tmp90D,((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F->v=s,_tmp114F))))),
_tmp114E)))));}struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*env,struct Cyc_Absyn_Tvar*tv){struct _tuple15 _tmp911;struct Cyc_List_List*
_tmp912;struct _RegionHandle*_tmp913;struct _tuple15*_tmp910=env;_tmp911=*_tmp910;
_tmp912=_tmp911.f1;_tmp913=_tmp911.f2;{struct Cyc_Core_Opt*_tmp914=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp1152;struct
_tuple14*_tmp1151;return(_tmp1151=_region_malloc(_tmp913,sizeof(*_tmp1151)),((
_tmp1151->f1=tv,((_tmp1151->f2=Cyc_Absyn_new_evar(_tmp914,((_tmp1152=_cycalloc(
sizeof(*_tmp1152)),((_tmp1152->v=_tmp912,_tmp1152))))),_tmp1151)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp1158;void*_tmp1157[3];struct Cyc_String_pa_struct _tmp1156;struct Cyc_String_pa_struct
_tmp1155;struct Cyc_String_pa_struct _tmp1154;(_tmp1154.tag=0,((_tmp1154.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1155.tag=0,((_tmp1155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1156.tag=0,((_tmp1156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1157[0]=& _tmp1156,((_tmp1157[1]=& _tmp1155,((_tmp1157[2]=&
_tmp1154,Cyc_Tcutil_terr(loc,((_tmp1158="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1158,sizeof(char),59))),_tag_dyneither(_tmp1157,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp115B;void*_tmp115A;(_tmp115A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115B="same type variable has different identity!",
_tag_dyneither(_tmp115B,sizeof(char),43))),_tag_dyneither(_tmp115A,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp115C;return(_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C->hd=tv,((
_tmp115C->tl=tvs,_tmp115C)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp115F;void*_tmp115E;(_tmp115E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115F="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp115F,sizeof(char),39))),_tag_dyneither(_tmp115E,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp921=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp921->identity == - 1){const char*
_tmp1162;void*_tmp1161;(_tmp1161=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1162="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1162,sizeof(char),41))),_tag_dyneither(_tmp1161,sizeof(void*),
0)));}if(_tmp921->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1163;return(_tmp1163=_cycalloc(sizeof(*_tmp1163)),((_tmp1163->hd=tv,((
_tmp1163->tl=tvs,_tmp1163)))));};}struct _tuple24{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1166;void*_tmp1165;(_tmp1165=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1166="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1166,sizeof(char),44))),_tag_dyneither(_tmp1165,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple24
_tmp928;struct Cyc_Absyn_Tvar*_tmp929;int _tmp92A;int*_tmp92B;struct _tuple24*
_tmp927=(struct _tuple24*)tvs2->hd;_tmp928=*_tmp927;_tmp929=_tmp928.f1;_tmp92A=
_tmp928.f2;_tmp92B=(int*)&(*_tmp927).f2;if(_tmp929->identity == - 1){const char*
_tmp1169;void*_tmp1168;(_tmp1168=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1169="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1169,sizeof(char),46))),_tag_dyneither(_tmp1168,sizeof(void*),
0)));}if(_tmp929->identity == tv->identity){*_tmp92B=*_tmp92B  || b;return tvs;}}}{
struct _tuple24*_tmp116C;struct Cyc_List_List*_tmp116B;return(_tmp116B=
_region_malloc(r,sizeof(*_tmp116B)),((_tmp116B->hd=((_tmp116C=_region_malloc(r,
sizeof(*_tmp116C)),((_tmp116C->f1=tv,((_tmp116C->f2=b,_tmp116C)))))),((_tmp116B->tl=
tvs,_tmp116B)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1170;void*_tmp116F[1];struct Cyc_String_pa_struct _tmp116E;(_tmp116E.tag=0,((
_tmp116E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp116F[0]=& _tmp116E,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1170="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1170,sizeof(char),29))),_tag_dyneither(_tmp116F,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1171;return(_tmp1171=_cycalloc(sizeof(*
_tmp1171)),((_tmp1171->hd=tv,((_tmp1171->tl=tvs,_tmp1171)))));};}struct _tuple25{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp934=Cyc_Tcutil_compress(e);int _tmp936;_LL491: {struct Cyc_Absyn_Evar_struct*
_tmp935=(struct Cyc_Absyn_Evar_struct*)_tmp934;if(_tmp935->tag != 1)goto _LL493;
else{_tmp936=_tmp935->f3;}}_LL492:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple25 _tmp938;void*_tmp939;int _tmp93A;int*_tmp93B;struct
_tuple25*_tmp937=(struct _tuple25*)es2->hd;_tmp938=*_tmp937;_tmp939=_tmp938.f1;
_tmp93A=_tmp938.f2;_tmp93B=(int*)&(*_tmp937).f2;{void*_tmp93C=Cyc_Tcutil_compress(
_tmp939);int _tmp93E;_LL496: {struct Cyc_Absyn_Evar_struct*_tmp93D=(struct Cyc_Absyn_Evar_struct*)
_tmp93C;if(_tmp93D->tag != 1)goto _LL498;else{_tmp93E=_tmp93D->f3;}}_LL497: if(
_tmp936 == _tmp93E){if(b != *_tmp93B)*_tmp93B=1;return es;}goto _LL495;_LL498:;
_LL499: goto _LL495;_LL495:;};}}{struct _tuple25*_tmp1174;struct Cyc_List_List*
_tmp1173;return(_tmp1173=_region_malloc(r,sizeof(*_tmp1173)),((_tmp1173->hd=((
_tmp1174=_region_malloc(r,sizeof(*_tmp1174)),((_tmp1174->f1=e,((_tmp1174->f2=b,
_tmp1174)))))),((_tmp1173->tl=es,_tmp1173)))));};_LL493:;_LL494: return es;_LL490:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1175;r=((_tmp1175=_region_malloc(rgn,sizeof(*_tmp1175)),((
_tmp1175->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1175->tl=r,_tmp1175))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp943;int _tmp944;struct _tuple24 _tmp942=*((struct _tuple24*)
tvs->hd);_tmp943=_tmp942.f1;_tmp944=_tmp942.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmp943->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp1176;res=((_tmp1176=
_region_malloc(r,sizeof(*_tmp1176)),((_tmp1176->hd=(struct _tuple24*)tvs->hd,((
_tmp1176->tl=res,_tmp1176))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp117A;void*_tmp1179[1];struct Cyc_String_pa_struct
_tmp1178;(_tmp1178.tag=0,((_tmp1178.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1179[0]=& _tmp1178,Cyc_Tcutil_terr(loc,((_tmp117A="bitfield %s does not have constant width",
_tag_dyneither(_tmp117A,sizeof(char),41))),_tag_dyneither(_tmp1179,sizeof(void*),
1)))))));}else{unsigned int _tmp94A;int _tmp94B;struct _tuple13 _tmp949=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp94A=_tmp949.f1;_tmp94B=_tmp949.f2;if(!_tmp94B){
const char*_tmp117D;void*_tmp117C;(_tmp117C=0,Cyc_Tcutil_terr(loc,((_tmp117D="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp117D,sizeof(char),45))),_tag_dyneither(_tmp117C,sizeof(void*),
0)));}w=_tmp94A;}{void*_tmp94E=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmp950;_LL49B: {struct Cyc_Absyn_IntType_struct*_tmp94F=(struct Cyc_Absyn_IntType_struct*)
_tmp94E;if(_tmp94F->tag != 6)goto _LL49D;else{_tmp950=_tmp94F->f2;}}_LL49C: switch(
_tmp950){case Cyc_Absyn_Char_sz: _LL49F: if(w > 8){const char*_tmp1180;void*_tmp117F;(
_tmp117F=0,Cyc_Tcutil_terr(loc,((_tmp1180="bitfield larger than type",
_tag_dyneither(_tmp1180,sizeof(char),26))),_tag_dyneither(_tmp117F,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL4A0: if(w > 16){const char*_tmp1183;void*
_tmp1182;(_tmp1182=0,Cyc_Tcutil_terr(loc,((_tmp1183="bitfield larger than type",
_tag_dyneither(_tmp1183,sizeof(char),26))),_tag_dyneither(_tmp1182,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL4A1: goto _LL4A2;case Cyc_Absyn_Int_sz: _LL4A2:
if(w > 32){const char*_tmp1186;void*_tmp1185;(_tmp1185=0,Cyc_Tcutil_terr(loc,((
_tmp1186="bitfield larger than type",_tag_dyneither(_tmp1186,sizeof(char),26))),
_tag_dyneither(_tmp1185,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL4A3: if(w > 64){const char*_tmp1189;void*_tmp1188;(_tmp1188=0,Cyc_Tcutil_terr(
loc,((_tmp1189="bitfield larger than type",_tag_dyneither(_tmp1189,sizeof(char),
26))),_tag_dyneither(_tmp1188,sizeof(void*),0)));}break;}goto _LL49A;_LL49D:;
_LL49E:{const char*_tmp118E;void*_tmp118D[2];struct Cyc_String_pa_struct _tmp118C;
struct Cyc_String_pa_struct _tmp118B;(_tmp118B.tag=0,((_tmp118B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp118C.tag=0,((_tmp118C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp118D[0]=& _tmp118C,((_tmp118D[1]=& _tmp118B,Cyc_Tcutil_terr(loc,((_tmp118E="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp118E,sizeof(char),52))),_tag_dyneither(_tmp118D,sizeof(void*),
2)))))))))))));}goto _LL49A;_LL49A:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmp95D=(void*)atts->hd;_LL4A6: {struct Cyc_Absyn_Packed_att_struct*_tmp95E=(
struct Cyc_Absyn_Packed_att_struct*)_tmp95D;if(_tmp95E->tag != 7)goto _LL4A8;}
_LL4A7: continue;_LL4A8: {struct Cyc_Absyn_Aligned_att_struct*_tmp95F=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp95D;if(_tmp95F->tag != 6)goto _LL4AA;}_LL4A9: continue;_LL4AA:;_LL4AB: {const
char*_tmp1193;void*_tmp1192[2];struct Cyc_String_pa_struct _tmp1191;struct Cyc_String_pa_struct
_tmp1190;(_tmp1190.tag=0,((_tmp1190.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1191.tag=0,((_tmp1191.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1192[0]=&
_tmp1191,((_tmp1192[1]=& _tmp1190,Cyc_Tcutil_terr(loc,((_tmp1193="bad attribute %s on member %s",
_tag_dyneither(_tmp1193,sizeof(char),30))),_tag_dyneither(_tmp1192,sizeof(void*),
2)))))))))))));}_LL4A5:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp964=t;struct
Cyc_Absyn_Typedefdecl*_tmp966;void**_tmp967;_LL4AD: {struct Cyc_Absyn_TypedefType_struct*
_tmp965=(struct Cyc_Absyn_TypedefType_struct*)_tmp964;if(_tmp965->tag != 18)goto
_LL4AF;else{_tmp966=_tmp965->f3;_tmp967=_tmp965->f4;}}_LL4AE: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp966))->tq).real_const  || (_tmp966->tq).print_const){if(
declared_const){const char*_tmp1196;void*_tmp1195;(_tmp1195=0,Cyc_Tcutil_warn(loc,((
_tmp1196="extra const",_tag_dyneither(_tmp1196,sizeof(char),12))),_tag_dyneither(
_tmp1195,sizeof(void*),0)));}return 1;}if((unsigned int)_tmp967)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp967);else{return declared_const;}_LL4AF:;_LL4B0: return
declared_const;_LL4AC:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple26{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,enum Cyc_Absyn_Kind expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
enum Cyc_Absyn_Kind expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmp96A=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp96D;struct Cyc_Core_Opt**_tmp96E;struct Cyc_Core_Opt*
_tmp96F;struct Cyc_Core_Opt**_tmp970;struct Cyc_Absyn_Tvar*_tmp972;struct Cyc_List_List*
_tmp974;struct _tuple2*_tmp976;struct Cyc_Absyn_Enumdecl*_tmp977;struct Cyc_Absyn_Enumdecl**
_tmp978;struct Cyc_Absyn_DatatypeInfo _tmp97A;union Cyc_Absyn_DatatypeInfoU _tmp97B;
union Cyc_Absyn_DatatypeInfoU*_tmp97C;struct Cyc_List_List*_tmp97D;struct Cyc_List_List**
_tmp97E;struct Cyc_Absyn_DatatypeFieldInfo _tmp980;union Cyc_Absyn_DatatypeFieldInfoU
_tmp981;union Cyc_Absyn_DatatypeFieldInfoU*_tmp982;struct Cyc_List_List*_tmp983;
struct Cyc_Absyn_PtrInfo _tmp985;void*_tmp986;struct Cyc_Absyn_Tqual _tmp987;struct
Cyc_Absyn_Tqual*_tmp988;struct Cyc_Absyn_PtrAtts _tmp989;void*_tmp98A;union Cyc_Absyn_Constraint*
_tmp98B;union Cyc_Absyn_Constraint*_tmp98C;union Cyc_Absyn_Constraint*_tmp98D;void*
_tmp98F;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_ArrayInfo _tmp996;void*
_tmp997;struct Cyc_Absyn_Tqual _tmp998;struct Cyc_Absyn_Tqual*_tmp999;struct Cyc_Absyn_Exp*
_tmp99A;struct Cyc_Absyn_Exp**_tmp99B;union Cyc_Absyn_Constraint*_tmp99C;struct Cyc_Position_Segment*
_tmp99D;struct Cyc_Absyn_FnInfo _tmp99F;struct Cyc_List_List*_tmp9A0;struct Cyc_List_List**
_tmp9A1;struct Cyc_Core_Opt*_tmp9A2;struct Cyc_Core_Opt**_tmp9A3;void*_tmp9A4;
struct Cyc_List_List*_tmp9A5;int _tmp9A6;struct Cyc_Absyn_VarargInfo*_tmp9A7;struct
Cyc_List_List*_tmp9A8;struct Cyc_List_List*_tmp9A9;struct Cyc_List_List*_tmp9AB;
enum Cyc_Absyn_AggrKind _tmp9AD;struct Cyc_List_List*_tmp9AE;struct Cyc_Absyn_AggrInfo
_tmp9B0;union Cyc_Absyn_AggrInfoU _tmp9B1;union Cyc_Absyn_AggrInfoU*_tmp9B2;struct
Cyc_List_List*_tmp9B3;struct Cyc_List_List**_tmp9B4;struct _tuple2*_tmp9B6;struct
Cyc_List_List*_tmp9B7;struct Cyc_List_List**_tmp9B8;struct Cyc_Absyn_Typedefdecl*
_tmp9B9;struct Cyc_Absyn_Typedefdecl**_tmp9BA;void**_tmp9BB;void***_tmp9BC;void*
_tmp9C0;void*_tmp9C2;void*_tmp9C3;void*_tmp9C5;void*_tmp9C7;struct Cyc_List_List*
_tmp9C9;_LL4B2: {struct Cyc_Absyn_VoidType_struct*_tmp96B=(struct Cyc_Absyn_VoidType_struct*)
_tmp96A;if(_tmp96B->tag != 0)goto _LL4B4;}_LL4B3: goto _LL4B1;_LL4B4: {struct Cyc_Absyn_Evar_struct*
_tmp96C=(struct Cyc_Absyn_Evar_struct*)_tmp96A;if(_tmp96C->tag != 1)goto _LL4B6;
else{_tmp96D=_tmp96C->f1;_tmp96E=(struct Cyc_Core_Opt**)& _tmp96C->f1;_tmp96F=
_tmp96C->f2;_tmp970=(struct Cyc_Core_Opt**)& _tmp96C->f2;}}_LL4B5: if(*_tmp96E == 0)*
_tmp96E=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && Cyc_Tcutil_is_region_kind(expected_kind)){if(expected_kind == Cyc_Absyn_UniqueRgnKind)*
_tmp970=(struct Cyc_Core_Opt*)& urgn;else{*_tmp970=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1199;struct
Cyc_Absyn_Less_kb_struct*_tmp1198;struct Cyc_Absyn_Tvar*_tmp9CA=Cyc_Tcutil_new_tvar((
void*)((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198[0]=((_tmp1199.tag=2,((
_tmp1199.f1=0,((_tmp1199.f2=expected_kind,_tmp1199)))))),_tmp1198)))));{struct
Cyc_Absyn_VarType_struct*_tmp119F;struct Cyc_Absyn_VarType_struct _tmp119E;struct
Cyc_Core_Opt*_tmp119D;*_tmp970=((_tmp119D=_cycalloc(sizeof(*_tmp119D)),((
_tmp119D->v=(void*)((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F[0]=((
_tmp119E.tag=2,((_tmp119E.f1=_tmp9CA,_tmp119E)))),_tmp119F)))),_tmp119D))));}
_tmp972=_tmp9CA;goto _LL4B7;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto _LL4B1;_LL4B6: {struct Cyc_Absyn_VarType_struct*
_tmp971=(struct Cyc_Absyn_VarType_struct*)_tmp96A;if(_tmp971->tag != 2)goto _LL4B8;
else{_tmp972=_tmp971->f1;}}_LL4B7:{void*_tmp9D0=Cyc_Absyn_compress_kb(_tmp972->kind);
struct Cyc_Core_Opt*_tmp9D2;struct Cyc_Core_Opt**_tmp9D3;_LL4E7: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp9D1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp9D0;if(_tmp9D1->tag != 1)goto
_LL4E9;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=(struct Cyc_Core_Opt**)& _tmp9D1->f1;}}
_LL4E8:{struct Cyc_Absyn_Less_kb_struct*_tmp11A5;struct Cyc_Absyn_Less_kb_struct
_tmp11A4;struct Cyc_Core_Opt*_tmp11A3;*_tmp9D3=((_tmp11A3=_cycalloc(sizeof(*
_tmp11A3)),((_tmp11A3->v=(void*)((_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((
_tmp11A5[0]=((_tmp11A4.tag=2,((_tmp11A4.f1=0,((_tmp11A4.f2=expected_kind,
_tmp11A4)))))),_tmp11A5)))),_tmp11A3))));}goto _LL4E6;_LL4E9:;_LL4EA: goto _LL4E6;
_LL4E6:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp972);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp972,put_in_effect);{void*_tmp9D7=Cyc_Absyn_compress_kb(_tmp972->kind);struct
Cyc_Core_Opt*_tmp9D9;struct Cyc_Core_Opt**_tmp9DA;enum Cyc_Absyn_Kind _tmp9DB;
_LL4EC: {struct Cyc_Absyn_Less_kb_struct*_tmp9D8=(struct Cyc_Absyn_Less_kb_struct*)
_tmp9D7;if(_tmp9D8->tag != 2)goto _LL4EE;else{_tmp9D9=_tmp9D8->f1;_tmp9DA=(struct
Cyc_Core_Opt**)& _tmp9D8->f1;_tmp9DB=_tmp9D8->f2;}}_LL4ED: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmp9DB)){struct Cyc_Absyn_Less_kb_struct*_tmp11AB;struct Cyc_Absyn_Less_kb_struct
_tmp11AA;struct Cyc_Core_Opt*_tmp11A9;*_tmp9DA=((_tmp11A9=_cycalloc(sizeof(*
_tmp11A9)),((_tmp11A9->v=(void*)((_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((
_tmp11AB[0]=((_tmp11AA.tag=2,((_tmp11AA.f1=0,((_tmp11AA.f2=expected_kind,
_tmp11AA)))))),_tmp11AB)))),_tmp11A9))));}goto _LL4EB;_LL4EE:;_LL4EF: goto _LL4EB;
_LL4EB:;}goto _LL4B1;_LL4B8: {struct Cyc_Absyn_AnonEnumType_struct*_tmp973=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmp96A;if(_tmp973->tag != 15)goto _LL4BA;else{
_tmp974=_tmp973->f1;}}_LL4B9: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmp9DF=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp974 != 0;_tmp974=_tmp974->tl){
struct Cyc_Absyn_Enumfield*_tmp9E0=(struct Cyc_Absyn_Enumfield*)_tmp974->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp9E0->name).f2)){
const char*_tmp11AF;void*_tmp11AE[1];struct Cyc_String_pa_struct _tmp11AD;(_tmp11AD.tag=
0,((_tmp11AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E0->name).f2),((
_tmp11AE[0]=& _tmp11AD,Cyc_Tcutil_terr(_tmp9E0->loc,((_tmp11AF="duplicate enum field name %s",
_tag_dyneither(_tmp11AF,sizeof(char),29))),_tag_dyneither(_tmp11AE,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp11B0;prev_fields=((_tmp11B0=
_region_malloc(_tmp9DF,sizeof(*_tmp11B0)),((_tmp11B0->hd=(*_tmp9E0->name).f2,((
_tmp11B0->tl=prev_fields,_tmp11B0))))));}if(_tmp9E0->tag == 0)_tmp9E0->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp9E0->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E0->tag))){const char*_tmp11B4;void*
_tmp11B3[1];struct Cyc_String_pa_struct _tmp11B2;(_tmp11B2.tag=0,((_tmp11B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E0->name).f2),((_tmp11B3[0]=&
_tmp11B2,Cyc_Tcutil_terr(loc,((_tmp11B4="enum field %s: expression is not constant",
_tag_dyneither(_tmp11B4,sizeof(char),42))),_tag_dyneither(_tmp11B3,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp9E0->tag))).f1;tag_count=t1 + 1;(*_tmp9E0->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp11BA;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp11B9;struct _tuple25*_tmp11B8;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple25*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp9E0->name).f2,(struct _tuple25*)((_tmp11B8=_cycalloc(sizeof(*_tmp11B8)),((
_tmp11B8->f1=(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((
_tmp11B9.tag=4,((_tmp11B9.f1=(void*)t,((_tmp11B9.f2=_tmp9E0,_tmp11B9)))))),
_tmp11BA)))),((_tmp11B8->f2=1,_tmp11B8)))))));};};}}goto _LL4B1;}_LL4BA: {struct
Cyc_Absyn_EnumType_struct*_tmp975=(struct Cyc_Absyn_EnumType_struct*)_tmp96A;if(
_tmp975->tag != 14)goto _LL4BC;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;
_tmp978=(struct Cyc_Absyn_Enumdecl**)& _tmp975->f2;}}_LL4BB: if(*_tmp978 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp978))->fields == 0){struct
_handler_cons _tmp9EB;_push_handler(& _tmp9EB);{int _tmp9ED=0;if(setjmp(_tmp9EB.handler))
_tmp9ED=1;if(!_tmp9ED){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp976);*_tmp978=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp9EC=(void*)_exn_thrown;void*_tmp9EF=_tmp9EC;_LL4F1: {struct Cyc_Dict_Absent_struct*
_tmp9F0=(struct Cyc_Dict_Absent_struct*)_tmp9EF;if(_tmp9F0->tag != Cyc_Dict_Absent)
goto _LL4F3;}_LL4F2: {struct Cyc_Tcenv_Genv*_tmp9F1=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp11BB;struct Cyc_Absyn_Enumdecl*_tmp9F2=(_tmp11BB=
_cycalloc(sizeof(*_tmp11BB)),((_tmp11BB->sc=Cyc_Absyn_Extern,((_tmp11BB->name=
_tmp976,((_tmp11BB->fields=0,_tmp11BB)))))));Cyc_Tc_tcEnumdecl(te,_tmp9F1,loc,
_tmp9F2);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp976);*
_tmp978=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL4F0;};}_LL4F3:;_LL4F4:(void)_throw(
_tmp9EF);_LL4F0:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp978);*_tmp976=(ed->name)[0];goto _LL4B1;};_LL4BC: {struct Cyc_Absyn_DatatypeType_struct*
_tmp979=(struct Cyc_Absyn_DatatypeType_struct*)_tmp96A;if(_tmp979->tag != 3)goto
_LL4BE;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp97A.datatype_info;_tmp97C=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmp979->f1).datatype_info;_tmp97D=_tmp97A.targs;_tmp97E=(struct Cyc_List_List**)&(
_tmp979->f1).targs;}}_LL4BD: {struct Cyc_List_List*_tmp9F4=*_tmp97E;{union Cyc_Absyn_DatatypeInfoU
_tmp9F5=*_tmp97C;struct Cyc_Absyn_UnknownDatatypeInfo _tmp9F6;struct _tuple2*
_tmp9F7;int _tmp9F8;struct Cyc_Absyn_Datatypedecl**_tmp9F9;struct Cyc_Absyn_Datatypedecl*
_tmp9FA;_LL4F6: if((_tmp9F5.UnknownDatatype).tag != 1)goto _LL4F8;_tmp9F6=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmp9F5.UnknownDatatype).val;_tmp9F7=_tmp9F6.name;
_tmp9F8=_tmp9F6.is_extensible;_LL4F7: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmp9FB;_push_handler(& _tmp9FB);{int _tmp9FD=0;if(setjmp(
_tmp9FB.handler))_tmp9FD=1;if(!_tmp9FD){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmp9F7);;_pop_handler();}else{void*_tmp9FC=(void*)_exn_thrown;void*_tmp9FF=
_tmp9FC;_LL4FB: {struct Cyc_Dict_Absent_struct*_tmpA00=(struct Cyc_Dict_Absent_struct*)
_tmp9FF;if(_tmpA00->tag != Cyc_Dict_Absent)goto _LL4FD;}_LL4FC: {struct Cyc_Tcenv_Genv*
_tmpA01=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp11BC;struct Cyc_Absyn_Datatypedecl*
_tmpA02=(_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC->sc=Cyc_Absyn_Extern,((
_tmp11BC->name=_tmp9F7,((_tmp11BC->tvs=0,((_tmp11BC->fields=0,((_tmp11BC->is_extensible=
_tmp9F8,_tmp11BC)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpA01,loc,_tmpA02);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9F7);if(_tmp9F4 != 0){{const char*
_tmp11C0;void*_tmp11BF[1];struct Cyc_String_pa_struct _tmp11BE;(_tmp11BE.tag=0,((
_tmp11BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9F7)),((_tmp11BF[0]=& _tmp11BE,Cyc_Tcutil_terr(loc,((_tmp11C0="declare parameterized datatype %s before using",
_tag_dyneither(_tmp11C0,sizeof(char),47))),_tag_dyneither(_tmp11BF,sizeof(void*),
1)))))));}return cvtenv;}goto _LL4FA;}_LL4FD:;_LL4FE:(void)_throw(_tmp9FF);_LL4FA:;}};}
if(_tmp9F8  && !(*tudp)->is_extensible){const char*_tmp11C4;void*_tmp11C3[1];
struct Cyc_String_pa_struct _tmp11C2;(_tmp11C2.tag=0,((_tmp11C2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F7)),((
_tmp11C3[0]=& _tmp11C2,Cyc_Tcutil_terr(loc,((_tmp11C4="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11C4,sizeof(char),41))),_tag_dyneither(_tmp11C3,sizeof(void*),
1)))))));}*_tmp97C=Cyc_Absyn_KnownDatatype(tudp);_tmp9FA=*tudp;goto _LL4F9;}
_LL4F8: if((_tmp9F5.KnownDatatype).tag != 2)goto _LL4F5;_tmp9F9=(struct Cyc_Absyn_Datatypedecl**)(
_tmp9F5.KnownDatatype).val;_tmp9FA=*_tmp9F9;_LL4F9: {struct Cyc_List_List*tvs=
_tmp9FA->tvs;for(0;_tmp9F4 != 0  && tvs != 0;(_tmp9F4=_tmp9F4->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9F4->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp11C5;struct _tuple0 _tmpA0B=(_tmp11C5.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp11C5.f2=t,_tmp11C5)));void*_tmpA0C;void*_tmpA0E;struct Cyc_Absyn_Tvar*_tmpA10;
_LL500: _tmpA0C=_tmpA0B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpA0D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA0C;if(_tmpA0D->tag != 1)goto _LL502;};_tmpA0E=_tmpA0B.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA0F=(struct Cyc_Absyn_VarType_struct*)_tmpA0E;if(_tmpA0F->tag != 2)goto _LL502;
else{_tmpA10=_tmpA0F->f1;}};_LL501: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA10);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA10,1);continue;_LL502:;_LL503: goto _LL4FF;_LL4FF:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp9F4 != 0){const char*_tmp11C9;void*_tmp11C8[1];
struct Cyc_String_pa_struct _tmp11C7;(_tmp11C7.tag=0,((_tmp11C7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9FA->name)),((
_tmp11C8[0]=& _tmp11C7,Cyc_Tcutil_terr(loc,((_tmp11C9="too many type arguments for datatype %s",
_tag_dyneither(_tmp11C9,sizeof(char),40))),_tag_dyneither(_tmp11C8,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp11CA;hidden_ts=((
_tmp11CA=_cycalloc(sizeof(*_tmp11CA)),((_tmp11CA->hd=e,((_tmp11CA->tl=hidden_ts,
_tmp11CA))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*
_tmp97E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp97E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}goto _LL4F5;}_LL4F5:;}goto _LL4B1;}_LL4BE: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp97F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp96A;if(_tmp97F->tag != 4)goto _LL4C0;else{_tmp980=_tmp97F->f1;_tmp981=_tmp980.field_info;
_tmp982=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp97F->f1).field_info;_tmp983=
_tmp980.targs;}}_LL4BF:{union Cyc_Absyn_DatatypeFieldInfoU _tmpA15=*_tmp982;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA16;struct _tuple2*_tmpA17;struct _tuple2*
_tmpA18;int _tmpA19;struct _tuple3 _tmpA1A;struct Cyc_Absyn_Datatypedecl*_tmpA1B;
struct Cyc_Absyn_Datatypefield*_tmpA1C;_LL505: if((_tmpA15.UnknownDatatypefield).tag
!= 1)goto _LL507;_tmpA16=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA15.UnknownDatatypefield).val;
_tmpA17=_tmpA16.datatype_name;_tmpA18=_tmpA16.field_name;_tmpA19=_tmpA16.is_extensible;
_LL506: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpA1D;_push_handler(& _tmpA1D);{int _tmpA1F=0;if(setjmp(
_tmpA1D.handler))_tmpA1F=1;if(!_tmpA1F){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpA17);;_pop_handler();}else{void*_tmpA1E=(void*)_exn_thrown;void*_tmpA21=
_tmpA1E;_LL50A: {struct Cyc_Dict_Absent_struct*_tmpA22=(struct Cyc_Dict_Absent_struct*)
_tmpA21;if(_tmpA22->tag != Cyc_Dict_Absent)goto _LL50C;}_LL50B:{const char*_tmp11CE;
void*_tmp11CD[1];struct Cyc_String_pa_struct _tmp11CC;(_tmp11CC.tag=0,((_tmp11CC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((
_tmp11CD[0]=& _tmp11CC,Cyc_Tcutil_terr(loc,((_tmp11CE="unbound datatype %s",
_tag_dyneither(_tmp11CE,sizeof(char),20))),_tag_dyneither(_tmp11CD,sizeof(void*),
1)))))));}return cvtenv;_LL50C:;_LL50D:(void)_throw(_tmpA21);_LL509:;}};}{struct
_handler_cons _tmpA26;_push_handler(& _tmpA26);{int _tmpA28=0;if(setjmp(_tmpA26.handler))
_tmpA28=1;if(!_tmpA28){{struct _RegionHandle*_tmpA29=Cyc_Tcenv_get_fnrgn(te);void*
_tmpA2A=Cyc_Tcenv_lookup_ordinary(_tmpA29,te,loc,_tmpA18);struct Cyc_Absyn_Datatypedecl*
_tmpA2C;struct Cyc_Absyn_Datatypefield*_tmpA2D;_LL50F: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpA2B=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpA2A;if(_tmpA2B->tag != 2)goto
_LL511;else{_tmpA2C=_tmpA2B->f1;_tmpA2D=_tmpA2B->f2;}}_LL510: tuf=_tmpA2D;tud=
_tmpA2C;if(_tmpA19  && !tud->is_extensible){const char*_tmp11D2;void*_tmp11D1[1];
struct Cyc_String_pa_struct _tmp11D0;(_tmp11D0.tag=0,((_tmp11D0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((
_tmp11D1[0]=& _tmp11D0,Cyc_Tcutil_terr(loc,((_tmp11D2="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11D2,sizeof(char),41))),_tag_dyneither(_tmp11D1,sizeof(void*),
1)))))));}goto _LL50E;_LL511:;_LL512:{const char*_tmp11D7;void*_tmp11D6[2];struct
Cyc_String_pa_struct _tmp11D5;struct Cyc_String_pa_struct _tmp11D4;(_tmp11D4.tag=0,((
_tmp11D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA17)),((_tmp11D5.tag=0,((_tmp11D5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA18)),((_tmp11D6[0]=& _tmp11D5,((
_tmp11D6[1]=& _tmp11D4,Cyc_Tcutil_terr(loc,((_tmp11D7="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11D7,sizeof(char),37))),_tag_dyneither(_tmp11D6,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpA35=cvtenv;_npop_handler(0);return
_tmpA35;};_LL50E:;};_pop_handler();}else{void*_tmpA27=(void*)_exn_thrown;void*
_tmpA37=_tmpA27;_LL514: {struct Cyc_Dict_Absent_struct*_tmpA38=(struct Cyc_Dict_Absent_struct*)
_tmpA37;if(_tmpA38->tag != Cyc_Dict_Absent)goto _LL516;}_LL515:{const char*_tmp11DC;
void*_tmp11DB[2];struct Cyc_String_pa_struct _tmp11DA;struct Cyc_String_pa_struct
_tmp11D9;(_tmp11D9.tag=0,((_tmp11D9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((_tmp11DA.tag=0,((_tmp11DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA18)),((
_tmp11DB[0]=& _tmp11DA,((_tmp11DB[1]=& _tmp11D9,Cyc_Tcutil_terr(loc,((_tmp11DC="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11DC,sizeof(char),37))),_tag_dyneither(_tmp11DB,sizeof(void*),
2)))))))))))));}return cvtenv;_LL516:;_LL517:(void)_throw(_tmpA37);_LL513:;}};}*
_tmp982=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpA1B=tud;_tmpA1C=tuf;goto _LL508;}
_LL507: if((_tmpA15.KnownDatatypefield).tag != 2)goto _LL504;_tmpA1A=(struct _tuple3)(
_tmpA15.KnownDatatypefield).val;_tmpA1B=_tmpA1A.f1;_tmpA1C=_tmpA1A.f2;_LL508: {
struct Cyc_List_List*tvs=_tmpA1B->tvs;for(0;_tmp983 != 0  && tvs != 0;(_tmp983=
_tmp983->tl,tvs=tvs->tl)){void*t=(void*)_tmp983->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp11DD;struct _tuple0 _tmpA3E=(
_tmp11DD.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp11DD.f2=t,_tmp11DD)));void*
_tmpA3F;void*_tmpA41;struct Cyc_Absyn_Tvar*_tmpA43;_LL519: _tmpA3F=_tmpA3E.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpA40=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA3F;if(_tmpA40->tag != 1)goto _LL51B;};_tmpA41=_tmpA3E.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA42=(struct Cyc_Absyn_VarType_struct*)_tmpA41;if(_tmpA42->tag != 2)goto _LL51B;
else{_tmpA43=_tmpA42->f1;}};_LL51A: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA43);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA43,1);continue;_LL51B:;_LL51C: goto _LL518;_LL518:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp983 != 0){const char*_tmp11E2;void*_tmp11E1[2];
struct Cyc_String_pa_struct _tmp11E0;struct Cyc_String_pa_struct _tmp11DF;(_tmp11DF.tag=
0,((_tmp11DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA1C->name)),((_tmp11E0.tag=0,((_tmp11E0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1B->name)),((_tmp11E1[0]=& _tmp11E0,((
_tmp11E1[1]=& _tmp11DF,Cyc_Tcutil_terr(loc,((_tmp11E2="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp11E2,sizeof(char),43))),_tag_dyneither(_tmp11E1,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp11E7;void*_tmp11E6[2];struct Cyc_String_pa_struct
_tmp11E5;struct Cyc_String_pa_struct _tmp11E4;(_tmp11E4.tag=0,((_tmp11E4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1C->name)),((
_tmp11E5.tag=0,((_tmp11E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA1B->name)),((_tmp11E6[0]=& _tmp11E5,((_tmp11E6[1]=& _tmp11E4,Cyc_Tcutil_terr(
loc,((_tmp11E7="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp11E7,sizeof(char),42))),_tag_dyneither(_tmp11E6,sizeof(void*),2)))))))))))));}
goto _LL504;}_LL504:;}goto _LL4B1;_LL4C0: {struct Cyc_Absyn_PointerType_struct*
_tmp984=(struct Cyc_Absyn_PointerType_struct*)_tmp96A;if(_tmp984->tag != 5)goto
_LL4C2;else{_tmp985=_tmp984->f1;_tmp986=_tmp985.elt_typ;_tmp987=_tmp985.elt_tq;
_tmp988=(struct Cyc_Absyn_Tqual*)&(_tmp984->f1).elt_tq;_tmp989=_tmp985.ptr_atts;
_tmp98A=_tmp989.rgn;_tmp98B=_tmp989.nullable;_tmp98C=_tmp989.bounds;_tmp98D=
_tmp989.zero_term;}}_LL4C1: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp986,1);_tmp988->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp988->print_const,_tmp986);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp98A,1);{union Cyc_Absyn_Constraint*_tmpA4C=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmp98D);union Cyc_Absyn_Constraint _tmpA4D;int _tmpA4E;union Cyc_Absyn_Constraint
_tmpA4F;int _tmpA50;_LL51E: _tmpA4D=*_tmpA4C;if((_tmpA4D.No_constr).tag != 3)goto
_LL520;_tmpA4E=(int)(_tmpA4D.No_constr).val;_LL51F:{void*_tmpA51=Cyc_Tcutil_compress(
_tmp986);enum Cyc_Absyn_Size_of _tmpA53;_LL525: {struct Cyc_Absyn_IntType_struct*
_tmpA52=(struct Cyc_Absyn_IntType_struct*)_tmpA51;if(_tmpA52->tag != 6)goto _LL527;
else{_tmpA53=_tmpA52->f2;if(_tmpA53 != Cyc_Absyn_Char_sz)goto _LL527;}}_LL526:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp98D,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL524;_LL527:;_LL528:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp98D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL524;
_LL524:;}goto _LL51D;_LL520: _tmpA4F=*_tmpA4C;if((_tmpA4F.Eq_constr).tag != 1)goto
_LL522;_tmpA50=(int)(_tmpA4F.Eq_constr).val;if(_tmpA50 != 1)goto _LL522;_LL521: if(
!Cyc_Tcutil_admits_zero(_tmp986)){const char*_tmp11EB;void*_tmp11EA[1];struct Cyc_String_pa_struct
_tmp11E9;(_tmp11E9.tag=0,((_tmp11E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp986)),((_tmp11EA[0]=& _tmp11E9,Cyc_Tcutil_terr(
loc,((_tmp11EB="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp11EB,sizeof(char),53))),_tag_dyneither(_tmp11EA,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL51D;_LL522:;_LL523: is_zero_terminated=0;
goto _LL51D;_LL51D:;}{void*_tmpA57=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp98C);struct Cyc_Absyn_Exp*
_tmpA5A;_LL52A: {struct Cyc_Absyn_DynEither_b_struct*_tmpA58=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpA57;if(_tmpA58->tag != 0)goto _LL52C;}_LL52B: goto _LL529;_LL52C: {struct Cyc_Absyn_Upper_b_struct*
_tmpA59=(struct Cyc_Absyn_Upper_b_struct*)_tmpA57;if(_tmpA59->tag != 1)goto _LL529;
else{_tmpA5A=_tmpA59->f1;}}_LL52D: {struct _RegionHandle*_tmpA5B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA5C=Cyc_Tcenv_allow_valueof(_tmpA5B,te);Cyc_Tcexp_tcExp(
_tmpA5C,0,_tmpA5A);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA5A,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA5A)){const char*_tmp11EE;void*
_tmp11ED;(_tmp11ED=0,Cyc_Tcutil_terr(loc,((_tmp11EE="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp11EE,sizeof(char),49))),_tag_dyneither(_tmp11ED,sizeof(void*),
0)));}{unsigned int _tmpA60;int _tmpA61;struct _tuple13 _tmpA5F=Cyc_Evexp_eval_const_uint_exp(
_tmpA5A);_tmpA60=_tmpA5F.f1;_tmpA61=_tmpA5F.f2;if(is_zero_terminated  && (!
_tmpA61  || _tmpA60 < 1)){const char*_tmp11F1;void*_tmp11F0;(_tmp11F0=0,Cyc_Tcutil_terr(
loc,((_tmp11F1="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp11F1,sizeof(char),55))),_tag_dyneither(_tmp11F0,sizeof(void*),
0)));}goto _LL529;};}_LL529:;}goto _LL4B1;}_LL4C2: {struct Cyc_Absyn_TagType_struct*
_tmp98E=(struct Cyc_Absyn_TagType_struct*)_tmp96A;if(_tmp98E->tag != 20)goto _LL4C4;
else{_tmp98F=(void*)_tmp98E->f1;}}_LL4C3: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_IntKind,_tmp98F,1);goto _LL4B1;_LL4C4: {struct Cyc_Absyn_ValueofType_struct*
_tmp990=(struct Cyc_Absyn_ValueofType_struct*)_tmp96A;if(_tmp990->tag != 19)goto
_LL4C6;else{_tmp991=_tmp990->f1;}}_LL4C5: {struct _RegionHandle*_tmpA64=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA65=Cyc_Tcenv_allow_valueof(_tmpA64,te);Cyc_Tcexp_tcExp(
_tmpA65,0,_tmp991);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp991)){const char*
_tmp11F4;void*_tmp11F3;(_tmp11F3=0,Cyc_Tcutil_terr(loc,((_tmp11F4="valueof_t requires an int expression",
_tag_dyneither(_tmp11F4,sizeof(char),37))),_tag_dyneither(_tmp11F3,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp991,te,cvtenv);goto
_LL4B1;}_LL4C6: {struct Cyc_Absyn_IntType_struct*_tmp992=(struct Cyc_Absyn_IntType_struct*)
_tmp96A;if(_tmp992->tag != 6)goto _LL4C8;}_LL4C7: goto _LL4C9;_LL4C8: {struct Cyc_Absyn_FloatType_struct*
_tmp993=(struct Cyc_Absyn_FloatType_struct*)_tmp96A;if(_tmp993->tag != 7)goto
_LL4CA;}_LL4C9: goto _LL4CB;_LL4CA: {struct Cyc_Absyn_DoubleType_struct*_tmp994=(
struct Cyc_Absyn_DoubleType_struct*)_tmp96A;if(_tmp994->tag != 8)goto _LL4CC;}
_LL4CB: goto _LL4B1;_LL4CC: {struct Cyc_Absyn_ArrayType_struct*_tmp995=(struct Cyc_Absyn_ArrayType_struct*)
_tmp96A;if(_tmp995->tag != 9)goto _LL4CE;else{_tmp996=_tmp995->f1;_tmp997=_tmp996.elt_type;
_tmp998=_tmp996.tq;_tmp999=(struct Cyc_Absyn_Tqual*)&(_tmp995->f1).tq;_tmp99A=
_tmp996.num_elts;_tmp99B=(struct Cyc_Absyn_Exp**)&(_tmp995->f1).num_elts;_tmp99C=
_tmp996.zero_term;_tmp99D=_tmp996.zt_loc;}}_LL4CD: {struct Cyc_Absyn_Exp*_tmpA68=*
_tmp99B;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_MemKind,
_tmp997,1);_tmp999->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp999->print_const,
_tmp997);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpA69=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp99C);union Cyc_Absyn_Constraint
_tmpA6A;int _tmpA6B;union Cyc_Absyn_Constraint _tmpA6C;int _tmpA6D;_LL52F: _tmpA6A=*
_tmpA69;if((_tmpA6A.No_constr).tag != 3)goto _LL531;_tmpA6B=(int)(_tmpA6A.No_constr).val;
_LL530:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99C,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL52E;_LL531: _tmpA6C=*_tmpA69;if((_tmpA6C.Eq_constr).tag
!= 1)goto _LL533;_tmpA6D=(int)(_tmpA6C.Eq_constr).val;if(_tmpA6D != 1)goto _LL533;
_LL532: if(!Cyc_Tcutil_admits_zero(_tmp997)){const char*_tmp11F8;void*_tmp11F7[1];
struct Cyc_String_pa_struct _tmp11F6;(_tmp11F6.tag=0,((_tmp11F6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp997)),((
_tmp11F7[0]=& _tmp11F6,Cyc_Tcutil_terr(loc,((_tmp11F8="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp11F8,sizeof(char),51))),_tag_dyneither(_tmp11F7,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL52E;_LL533:;_LL534: is_zero_terminated=0;
goto _LL52E;_LL52E:;}if(_tmpA68 == 0){if(is_zero_terminated)*_tmp99B=(_tmpA68=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp11FB;void*
_tmp11FA;(_tmp11FA=0,Cyc_Tcutil_warn(loc,((_tmp11FB="array bound defaults to 1 here",
_tag_dyneither(_tmp11FB,sizeof(char),31))),_tag_dyneither(_tmp11FA,sizeof(void*),
0)));}*_tmp99B=(_tmpA68=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpA68);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpA68)){const char*_tmp11FE;void*_tmp11FD;(_tmp11FD=0,Cyc_Tcutil_terr(loc,((
_tmp11FE="array bounds expression is not constant",_tag_dyneither(_tmp11FE,
sizeof(char),40))),_tag_dyneither(_tmp11FD,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpA68)){const char*_tmp1201;void*_tmp1200;(_tmp1200=0,
Cyc_Tcutil_terr(loc,((_tmp1201="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp1201,sizeof(char),47))),_tag_dyneither(_tmp1200,sizeof(void*),
0)));}{unsigned int _tmpA78;int _tmpA79;struct _tuple13 _tmpA77=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpA68);_tmpA78=_tmpA77.f1;_tmpA79=_tmpA77.f2;if((
is_zero_terminated  && _tmpA79) && _tmpA78 < 1){const char*_tmp1204;void*_tmp1203;(
_tmp1203=0,Cyc_Tcutil_warn(loc,((_tmp1204="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp1204,sizeof(char),48))),_tag_dyneither(_tmp1203,sizeof(void*),
0)));}if((_tmpA79  && _tmpA78 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1207;void*
_tmp1206;(_tmp1206=0,Cyc_Tcutil_warn(loc,((_tmp1207="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1207,sizeof(char),75))),_tag_dyneither(_tmp1206,sizeof(void*),
0)));}*_tmp99B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4B1;};};}
_LL4CE: {struct Cyc_Absyn_FnType_struct*_tmp99E=(struct Cyc_Absyn_FnType_struct*)
_tmp96A;if(_tmp99E->tag != 10)goto _LL4D0;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99F.tvars;
_tmp9A1=(struct Cyc_List_List**)&(_tmp99E->f1).tvars;_tmp9A2=_tmp99F.effect;
_tmp9A3=(struct Cyc_Core_Opt**)&(_tmp99E->f1).effect;_tmp9A4=_tmp99F.ret_typ;
_tmp9A5=_tmp99F.args;_tmp9A6=_tmp99F.c_varargs;_tmp9A7=_tmp99F.cyc_varargs;
_tmp9A8=_tmp99F.rgn_po;_tmp9A9=_tmp99F.attributes;}}_LL4CF: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp9A9 != 0;
_tmp9A9=_tmp9A9->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp9A9->hd)){const char*
_tmp120B;void*_tmp120A[1];struct Cyc_String_pa_struct _tmp1209;(_tmp1209.tag=0,((
_tmp1209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp9A9->hd)),((_tmp120A[0]=& _tmp1209,Cyc_Tcutil_terr(loc,((_tmp120B="bad function type attribute %s",
_tag_dyneither(_tmp120B,sizeof(char),31))),_tag_dyneither(_tmp120A,sizeof(void*),
1)))))));}{void*_tmpA81=(void*)_tmp9A9->hd;enum Cyc_Absyn_Format_Type _tmpA86;int
_tmpA87;int _tmpA88;_LL536: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA82=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpA81;if(_tmpA82->tag != 1)goto _LL538;}_LL537: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL535;_LL538: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA83=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA81;if(_tmpA83->tag != 2)goto
_LL53A;}_LL539: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL535;_LL53A: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpA84=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpA81;if(_tmpA84->tag != 3)goto _LL53C;}_LL53B: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL535;_LL53C: {struct Cyc_Absyn_Format_att_struct*_tmpA85=(
struct Cyc_Absyn_Format_att_struct*)_tmpA81;if(_tmpA85->tag != 19)goto _LL53E;else{
_tmpA86=_tmpA85->f1;_tmpA87=_tmpA85->f2;_tmpA88=_tmpA85->f3;}}_LL53D: if(!
seen_format){seen_format=1;ft=_tmpA86;fmt_desc_arg=_tmpA87;fmt_arg_start=_tmpA88;}
else{const char*_tmp120E;void*_tmp120D;(_tmp120D=0,Cyc_Tcutil_terr(loc,((_tmp120E="function can't have multiple format attributes",
_tag_dyneither(_tmp120E,sizeof(char),47))),_tag_dyneither(_tmp120D,sizeof(void*),
0)));}goto _LL535;_LL53E:;_LL53F: goto _LL535;_LL535:;};}if(num_convs > 1){const char*
_tmp1211;void*_tmp1210;(_tmp1210=0,Cyc_Tcutil_terr(loc,((_tmp1211="function can't have multiple calling conventions",
_tag_dyneither(_tmp1211,sizeof(char),49))),_tag_dyneither(_tmp1210,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp9A1);{struct Cyc_List_List*b=*
_tmp9A1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpA8D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
enum Cyc_Absyn_Kind _tmpA8F;_LL541: {struct Cyc_Absyn_Eq_kb_struct*_tmpA8E=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpA8D;if(_tmpA8E->tag != 0)goto _LL543;else{_tmpA8F=
_tmpA8E->f1;if(_tmpA8F != Cyc_Absyn_MemKind)goto _LL543;}}_LL542:{const char*
_tmp1215;void*_tmp1214[1];struct Cyc_String_pa_struct _tmp1213;(_tmp1213.tag=0,((
_tmp1213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1214[0]=& _tmp1213,Cyc_Tcutil_terr(loc,((_tmp1215="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1215,sizeof(char),51))),_tag_dyneither(_tmp1214,sizeof(void*),
1)))))));}goto _LL540;_LL543:;_LL544: goto _LL540;_LL540:;};}}{struct _RegionHandle*
_tmpA93=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1216;struct Cyc_Tcutil_CVTEnv
_tmpA94=(_tmp1216.r=_tmpA93,((_tmp1216.kind_env=cvtenv.kind_env,((_tmp1216.free_vars=
0,((_tmp1216.free_evars=0,((_tmp1216.generalize_evars=cvtenv.generalize_evars,((
_tmp1216.fn_result=1,_tmp1216)))))))))));_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_MemKind,_tmp9A4,1);_tmpA94.fn_result=0;{struct Cyc_List_List*
a=_tmp9A5;for(0;a != 0;a=a->tl){struct _tuple9*_tmpA95=(struct _tuple9*)a->hd;void*
_tmpA96=(*_tmpA95).f3;_tmpA94=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA94,Cyc_Absyn_MemKind,
_tmpA96,1);((*_tmpA95).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpA95).f2).print_const,_tmpA96);}}if(_tmp9A7 != 0){if(_tmp9A6){const char*
_tmp1219;void*_tmp1218;(_tmp1218=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1219="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1219,sizeof(char),29))),_tag_dyneither(_tmp1218,sizeof(void*),
0)));}{void*_tmpA9A;int _tmpA9B;struct Cyc_Absyn_VarargInfo _tmpA99=*_tmp9A7;
_tmpA9A=_tmpA99.type;_tmpA9B=_tmpA99.inject;_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_MemKind,_tmpA9A,1);(_tmp9A7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp9A7->tq).print_const,_tmpA9A);if(_tmpA9B){void*_tmpA9C=Cyc_Tcutil_compress(
_tmpA9A);struct Cyc_Absyn_PtrInfo _tmpA9E;void*_tmpA9F;struct Cyc_Absyn_PtrAtts
_tmpAA0;union Cyc_Absyn_Constraint*_tmpAA1;union Cyc_Absyn_Constraint*_tmpAA2;
_LL546: {struct Cyc_Absyn_PointerType_struct*_tmpA9D=(struct Cyc_Absyn_PointerType_struct*)
_tmpA9C;if(_tmpA9D->tag != 5)goto _LL548;else{_tmpA9E=_tmpA9D->f1;_tmpA9F=_tmpA9E.elt_typ;
_tmpAA0=_tmpA9E.ptr_atts;_tmpAA1=_tmpAA0.bounds;_tmpAA2=_tmpAA0.zero_term;}}
_LL547:{void*_tmpAA3=Cyc_Tcutil_compress(_tmpA9F);_LL54B: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAA4=(struct Cyc_Absyn_DatatypeType_struct*)_tmpAA3;if(_tmpAA4->tag != 3)goto
_LL54D;}_LL54C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpAA2)){const char*_tmp121C;void*_tmp121B;(_tmp121B=0,Cyc_Tcutil_terr(loc,((
_tmp121C="can't inject into a zeroterm pointer",_tag_dyneither(_tmp121C,sizeof(
char),37))),_tag_dyneither(_tmp121B,sizeof(void*),0)));}{void*_tmpAA7=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpAA1);_LL550: {struct Cyc_Absyn_DynEither_b_struct*_tmpAA8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAA7;if(_tmpAA8->tag != 0)goto _LL552;}_LL551:{const char*_tmp121F;void*_tmp121E;(
_tmp121E=0,Cyc_Tcutil_terr(loc,((_tmp121F="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp121F,sizeof(char),44))),_tag_dyneither(_tmp121E,sizeof(void*),
0)));}goto _LL54F;_LL552:;_LL553: goto _LL54F;_LL54F:;}goto _LL54A;_LL54D:;_LL54E:{
const char*_tmp1222;void*_tmp1221;(_tmp1221=0,Cyc_Tcutil_terr(loc,((_tmp1222="can't inject a non-datatype type",
_tag_dyneither(_tmp1222,sizeof(char),33))),_tag_dyneither(_tmp1221,sizeof(void*),
0)));}goto _LL54A;_LL54A:;}goto _LL545;_LL548:;_LL549:{const char*_tmp1225;void*
_tmp1224;(_tmp1224=0,Cyc_Tcutil_terr(loc,((_tmp1225="expecting a datatype pointer type",
_tag_dyneither(_tmp1225,sizeof(char),34))),_tag_dyneither(_tmp1224,sizeof(void*),
0)));}goto _LL545;_LL545:;}};}if(seen_format){int _tmpAAF=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp9A5);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpAAF) || 
fmt_arg_start < 0) || _tmp9A7 == 0  && fmt_arg_start != 0) || _tmp9A7 != 0  && 
fmt_arg_start != _tmpAAF + 1){const char*_tmp1228;void*_tmp1227;(_tmp1227=0,Cyc_Tcutil_terr(
loc,((_tmp1228="bad format descriptor",_tag_dyneither(_tmp1228,sizeof(char),22))),
_tag_dyneither(_tmp1227,sizeof(void*),0)));}else{void*_tmpAB3;struct _tuple9
_tmpAB2=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9A5,
fmt_desc_arg - 1);_tmpAB3=_tmpAB2.f3;{void*_tmpAB4=Cyc_Tcutil_compress(_tmpAB3);
struct Cyc_Absyn_PtrInfo _tmpAB6;void*_tmpAB7;struct Cyc_Absyn_PtrAtts _tmpAB8;union
Cyc_Absyn_Constraint*_tmpAB9;union Cyc_Absyn_Constraint*_tmpABA;_LL555: {struct
Cyc_Absyn_PointerType_struct*_tmpAB5=(struct Cyc_Absyn_PointerType_struct*)
_tmpAB4;if(_tmpAB5->tag != 5)goto _LL557;else{_tmpAB6=_tmpAB5->f1;_tmpAB7=_tmpAB6.elt_typ;
_tmpAB8=_tmpAB6.ptr_atts;_tmpAB9=_tmpAB8.bounds;_tmpABA=_tmpAB8.zero_term;}}
_LL556:{struct _tuple0 _tmp1229;struct _tuple0 _tmpABC=(_tmp1229.f1=Cyc_Tcutil_compress(
_tmpAB7),((_tmp1229.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpAB9),_tmp1229)));void*_tmpABD;enum Cyc_Absyn_Sign
_tmpABF;enum Cyc_Absyn_Size_of _tmpAC0;void*_tmpAC1;_LL55A: _tmpABD=_tmpABC.f1;{
struct Cyc_Absyn_IntType_struct*_tmpABE=(struct Cyc_Absyn_IntType_struct*)_tmpABD;
if(_tmpABE->tag != 6)goto _LL55C;else{_tmpABF=_tmpABE->f1;if(_tmpABF != Cyc_Absyn_None)
goto _LL55C;_tmpAC0=_tmpABE->f2;if(_tmpAC0 != Cyc_Absyn_Char_sz)goto _LL55C;}};
_tmpAC1=_tmpABC.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpAC2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAC1;if(_tmpAC2->tag != 0)goto _LL55C;};_LL55B: goto _LL559;_LL55C:;_LL55D:{const
char*_tmp122C;void*_tmp122B;(_tmp122B=0,Cyc_Tcutil_terr(loc,((_tmp122C="format descriptor is not a char ? type",
_tag_dyneither(_tmp122C,sizeof(char),39))),_tag_dyneither(_tmp122B,sizeof(void*),
0)));}goto _LL559;_LL559:;}goto _LL554;_LL557:;_LL558:{const char*_tmp122F;void*
_tmp122E;(_tmp122E=0,Cyc_Tcutil_terr(loc,((_tmp122F="format descriptor is not a char ? type",
_tag_dyneither(_tmp122F,sizeof(char),39))),_tag_dyneither(_tmp122E,sizeof(void*),
0)));}goto _LL554;_LL554:;}if(fmt_arg_start != 0){int problem;{struct _tuple26
_tmp1230;struct _tuple26 _tmpAC8=(_tmp1230.f1=ft,((_tmp1230.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9A7))->type)),
_tmp1230)));enum Cyc_Absyn_Format_Type _tmpAC9;void*_tmpACA;struct Cyc_Absyn_DatatypeInfo
_tmpACC;union Cyc_Absyn_DatatypeInfoU _tmpACD;struct Cyc_Absyn_Datatypedecl**
_tmpACE;struct Cyc_Absyn_Datatypedecl*_tmpACF;enum Cyc_Absyn_Format_Type _tmpAD0;
void*_tmpAD1;struct Cyc_Absyn_DatatypeInfo _tmpAD3;union Cyc_Absyn_DatatypeInfoU
_tmpAD4;struct Cyc_Absyn_Datatypedecl**_tmpAD5;struct Cyc_Absyn_Datatypedecl*
_tmpAD6;_LL55F: _tmpAC9=_tmpAC8.f1;if(_tmpAC9 != Cyc_Absyn_Printf_ft)goto _LL561;
_tmpACA=_tmpAC8.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpACB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpACA;if(_tmpACB->tag != 3)goto _LL561;else{_tmpACC=_tmpACB->f1;_tmpACD=_tmpACC.datatype_info;
if((_tmpACD.KnownDatatype).tag != 2)goto _LL561;_tmpACE=(struct Cyc_Absyn_Datatypedecl**)(
_tmpACD.KnownDatatype).val;_tmpACF=*_tmpACE;}};_LL560: problem=Cyc_Absyn_qvar_cmp(
_tmpACF->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL55E;_LL561: _tmpAD0=
_tmpAC8.f1;if(_tmpAD0 != Cyc_Absyn_Scanf_ft)goto _LL563;_tmpAD1=_tmpAC8.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpAD2=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpAD1;if(_tmpAD2->tag != 3)goto _LL563;else{_tmpAD3=_tmpAD2->f1;_tmpAD4=_tmpAD3.datatype_info;
if((_tmpAD4.KnownDatatype).tag != 2)goto _LL563;_tmpAD5=(struct Cyc_Absyn_Datatypedecl**)(
_tmpAD4.KnownDatatype).val;_tmpAD6=*_tmpAD5;}};_LL562: problem=Cyc_Absyn_qvar_cmp(
_tmpAD6->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL55E;_LL563:;_LL564:
problem=1;goto _LL55E;_LL55E:;}if(problem){const char*_tmp1233;void*_tmp1232;(
_tmp1232=0,Cyc_Tcutil_terr(loc,((_tmp1233="format attribute and vararg types don't match",
_tag_dyneither(_tmp1233,sizeof(char),46))),_tag_dyneither(_tmp1232,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmp9A8;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpADA;void*_tmpADB;void*_tmpADC;struct _tuple0*_tmpAD9=(struct _tuple0*)
rpo->hd;_tmpADA=*_tmpAD9;_tmpADB=_tmpADA.f1;_tmpADC=_tmpADA.f2;_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_EffKind,_tmpADB,1);_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_TopRgnKind,_tmpADC,1);}}if(*_tmp9A3 != 0)_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_EffKind,(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp9A3))->v,1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=
_tmpA94.free_vars;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpADE;int
_tmpADF;struct _tuple24 _tmpADD=*((struct _tuple24*)tvs->hd);_tmpADE=_tmpADD.f1;
_tmpADF=_tmpADD.f2;if(!_tmpADF)continue;{void*_tmpAE0=Cyc_Absyn_compress_kb(
_tmpADE->kind);struct Cyc_Core_Opt*_tmpAE2;struct Cyc_Core_Opt**_tmpAE3;enum Cyc_Absyn_Kind
_tmpAE4;struct Cyc_Core_Opt*_tmpAE6;struct Cyc_Core_Opt**_tmpAE7;enum Cyc_Absyn_Kind
_tmpAE8;enum Cyc_Absyn_Kind _tmpAEA;enum Cyc_Absyn_Kind _tmpAEC;enum Cyc_Absyn_Kind
_tmpAEE;struct Cyc_Core_Opt*_tmpAF0;struct Cyc_Core_Opt**_tmpAF1;enum Cyc_Absyn_Kind
_tmpAF2;enum Cyc_Absyn_Kind _tmpAF4;struct Cyc_Core_Opt*_tmpAF6;struct Cyc_Core_Opt**
_tmpAF7;_LL566: {struct Cyc_Absyn_Less_kb_struct*_tmpAE1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpAE0;if(_tmpAE1->tag != 2)goto _LL568;else{_tmpAE2=_tmpAE1->f1;_tmpAE3=(struct
Cyc_Core_Opt**)& _tmpAE1->f1;_tmpAE4=_tmpAE1->f2;if(_tmpAE4 != Cyc_Absyn_TopRgnKind)
goto _LL568;}}_LL567: _tmpAE7=_tmpAE3;_tmpAE8=Cyc_Absyn_RgnKind;goto _LL569;_LL568:{
struct Cyc_Absyn_Less_kb_struct*_tmpAE5=(struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;
if(_tmpAE5->tag != 2)goto _LL56A;else{_tmpAE6=_tmpAE5->f1;_tmpAE7=(struct Cyc_Core_Opt**)&
_tmpAE5->f1;_tmpAE8=_tmpAE5->f2;}}if(!(_tmpAE8 == Cyc_Absyn_RgnKind  || _tmpAE8 == 
Cyc_Absyn_UniqueRgnKind))goto _LL56A;_LL569:*_tmpAE7=Cyc_Tcutil_kind_to_bound_opt(
_tmpAE8);_tmpAEA=_tmpAE8;goto _LL56B;_LL56A:{struct Cyc_Absyn_Eq_kb_struct*_tmpAE9=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpAE0;if(_tmpAE9->tag != 0)goto _LL56C;else{
_tmpAEA=_tmpAE9->f1;}}if(!((_tmpAEA == Cyc_Absyn_RgnKind  || _tmpAEA == Cyc_Absyn_UniqueRgnKind)
 || _tmpAEA == Cyc_Absyn_TopRgnKind))goto _LL56C;_LL56B:{struct Cyc_Absyn_AccessEff_struct*
_tmp1242;struct Cyc_Absyn_VarType_struct*_tmp1241;struct Cyc_Absyn_VarType_struct
_tmp1240;struct Cyc_Absyn_AccessEff_struct _tmp123F;struct Cyc_List_List*_tmp123E;
effect=((_tmp123E=_cycalloc(sizeof(*_tmp123E)),((_tmp123E->hd=(void*)((_tmp1242=
_cycalloc(sizeof(*_tmp1242)),((_tmp1242[0]=((_tmp123F.tag=23,((_tmp123F.f1=(void*)((
void*)((_tmp1241=_cycalloc(sizeof(*_tmp1241)),((_tmp1241[0]=((_tmp1240.tag=2,((
_tmp1240.f1=_tmpADE,_tmp1240)))),_tmp1241))))),_tmp123F)))),_tmp1242)))),((
_tmp123E->tl=effect,_tmp123E))))));}goto _LL565;_LL56C: {struct Cyc_Absyn_Less_kb_struct*
_tmpAEB=(struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;if(_tmpAEB->tag != 2)goto _LL56E;
else{_tmpAEC=_tmpAEB->f2;if(_tmpAEC != Cyc_Absyn_IntKind)goto _LL56E;}}_LL56D: goto
_LL56F;_LL56E: {struct Cyc_Absyn_Eq_kb_struct*_tmpAED=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAE0;if(_tmpAED->tag != 0)goto _LL570;else{_tmpAEE=_tmpAED->f1;if(_tmpAEE != Cyc_Absyn_IntKind)
goto _LL570;}}_LL56F: goto _LL565;_LL570: {struct Cyc_Absyn_Less_kb_struct*_tmpAEF=(
struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;if(_tmpAEF->tag != 2)goto _LL572;else{
_tmpAF0=_tmpAEF->f1;_tmpAF1=(struct Cyc_Core_Opt**)& _tmpAEF->f1;_tmpAF2=_tmpAEF->f2;
if(_tmpAF2 != Cyc_Absyn_EffKind)goto _LL572;}}_LL571:*_tmpAF1=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_EffKind);goto _LL573;_LL572: {struct Cyc_Absyn_Eq_kb_struct*_tmpAF3=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpAE0;if(_tmpAF3->tag != 0)goto _LL574;else{
_tmpAF4=_tmpAF3->f1;if(_tmpAF4 != Cyc_Absyn_EffKind)goto _LL574;}}_LL573:{struct
Cyc_Absyn_VarType_struct*_tmp1248;struct Cyc_Absyn_VarType_struct _tmp1247;struct
Cyc_List_List*_tmp1246;effect=((_tmp1246=_cycalloc(sizeof(*_tmp1246)),((_tmp1246->hd=(
void*)((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((_tmp1248[0]=((_tmp1247.tag=2,((
_tmp1247.f1=_tmpADE,_tmp1247)))),_tmp1248)))),((_tmp1246->tl=effect,_tmp1246))))));}
goto _LL565;_LL574: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAF5=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpAE0;if(_tmpAF5->tag != 1)goto _LL576;else{_tmpAF6=_tmpAF5->f1;_tmpAF7=(struct
Cyc_Core_Opt**)& _tmpAF5->f1;}}_LL575:{struct Cyc_Absyn_Less_kb_struct*_tmp124E;
struct Cyc_Absyn_Less_kb_struct _tmp124D;struct Cyc_Core_Opt*_tmp124C;*_tmpAF7=((
_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C->v=(void*)((_tmp124E=_cycalloc(
sizeof(*_tmp124E)),((_tmp124E[0]=((_tmp124D.tag=2,((_tmp124D.f1=0,((_tmp124D.f2=
Cyc_Absyn_AnyKind,_tmp124D)))))),_tmp124E)))),_tmp124C))));}goto _LL577;_LL576:;
_LL577:{struct Cyc_Absyn_RgnsEff_struct*_tmp125D;struct Cyc_Absyn_VarType_struct*
_tmp125C;struct Cyc_Absyn_VarType_struct _tmp125B;struct Cyc_Absyn_RgnsEff_struct
_tmp125A;struct Cyc_List_List*_tmp1259;effect=((_tmp1259=_cycalloc(sizeof(*
_tmp1259)),((_tmp1259->hd=(void*)((_tmp125D=_cycalloc(sizeof(*_tmp125D)),((
_tmp125D[0]=((_tmp125A.tag=25,((_tmp125A.f1=(void*)((void*)((_tmp125C=_cycalloc(
sizeof(*_tmp125C)),((_tmp125C[0]=((_tmp125B.tag=2,((_tmp125B.f1=_tmpADE,_tmp125B)))),
_tmp125C))))),_tmp125A)))),_tmp125D)))),((_tmp1259->tl=effect,_tmp1259))))));}
goto _LL565;_LL565:;};}}{struct Cyc_List_List*ts=_tmpA94.free_evars;for(0;ts != 0;
ts=ts->tl){void*_tmpB09;int _tmpB0A;struct _tuple25 _tmpB08=*((struct _tuple25*)ts->hd);
_tmpB09=_tmpB08.f1;_tmpB0A=_tmpB08.f2;if(!_tmpB0A)continue;switch(Cyc_Tcutil_typ_kind(
_tmpB09)){case Cyc_Absyn_TopRgnKind: _LL578: goto _LL579;case Cyc_Absyn_UniqueRgnKind:
_LL579: goto _LL57A;case Cyc_Absyn_RgnKind: _LL57A:{struct Cyc_Absyn_AccessEff_struct*
_tmp1263;struct Cyc_Absyn_AccessEff_struct _tmp1262;struct Cyc_List_List*_tmp1261;
effect=((_tmp1261=_cycalloc(sizeof(*_tmp1261)),((_tmp1261->hd=(void*)((_tmp1263=
_cycalloc(sizeof(*_tmp1263)),((_tmp1263[0]=((_tmp1262.tag=23,((_tmp1262.f1=(void*)
_tmpB09,_tmp1262)))),_tmp1263)))),((_tmp1261->tl=effect,_tmp1261))))));}break;
case Cyc_Absyn_EffKind: _LL57B:{struct Cyc_List_List*_tmp1264;effect=((_tmp1264=
_cycalloc(sizeof(*_tmp1264)),((_tmp1264->hd=_tmpB09,((_tmp1264->tl=effect,
_tmp1264))))));}break;case Cyc_Absyn_IntKind: _LL57C: break;default: _LL57D:{struct
Cyc_Absyn_RgnsEff_struct*_tmp126A;struct Cyc_Absyn_RgnsEff_struct _tmp1269;struct
Cyc_List_List*_tmp1268;effect=((_tmp1268=_cycalloc(sizeof(*_tmp1268)),((_tmp1268->hd=(
void*)((_tmp126A=_cycalloc(sizeof(*_tmp126A)),((_tmp126A[0]=((_tmp1269.tag=25,((
_tmp1269.f1=(void*)_tmpB09,_tmp1269)))),_tmp126A)))),((_tmp1268->tl=effect,
_tmp1268))))));}break;}}}{struct Cyc_Absyn_JoinEff_struct*_tmp1270;struct Cyc_Absyn_JoinEff_struct
_tmp126F;struct Cyc_Core_Opt*_tmp126E;*_tmp9A3=((_tmp126E=_cycalloc(sizeof(*
_tmp126E)),((_tmp126E->v=(void*)((_tmp1270=_cycalloc(sizeof(*_tmp1270)),((
_tmp1270[0]=((_tmp126F.tag=24,((_tmp126F.f1=effect,_tmp126F)))),_tmp1270)))),
_tmp126E))));};}if(*_tmp9A1 != 0){struct Cyc_List_List*bs=*_tmp9A1;for(0;bs != 0;bs=
bs->tl){void*_tmpB15=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**_tmpB18;struct Cyc_Core_Opt*
_tmpB1A;struct Cyc_Core_Opt**_tmpB1B;enum Cyc_Absyn_Kind _tmpB1C;struct Cyc_Core_Opt*
_tmpB1E;struct Cyc_Core_Opt**_tmpB1F;enum Cyc_Absyn_Kind _tmpB20;struct Cyc_Core_Opt*
_tmpB22;struct Cyc_Core_Opt**_tmpB23;enum Cyc_Absyn_Kind _tmpB24;struct Cyc_Core_Opt*
_tmpB26;struct Cyc_Core_Opt**_tmpB27;enum Cyc_Absyn_Kind _tmpB28;enum Cyc_Absyn_Kind
_tmpB2A;_LL580: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB16=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB15;if(_tmpB16->tag != 1)goto _LL582;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct
Cyc_Core_Opt**)& _tmpB16->f1;}}_LL581:{const char*_tmp1274;void*_tmp1273[1];struct
Cyc_String_pa_struct _tmp1272;(_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1273[0]=&
_tmp1272,Cyc_Tcutil_warn(loc,((_tmp1274="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1274,sizeof(char),47))),_tag_dyneither(_tmp1273,sizeof(void*),
1)))))));}_tmpB1B=_tmpB18;goto _LL583;_LL582: {struct Cyc_Absyn_Less_kb_struct*
_tmpB19=(struct Cyc_Absyn_Less_kb_struct*)_tmpB15;if(_tmpB19->tag != 2)goto _LL584;
else{_tmpB1A=_tmpB19->f1;_tmpB1B=(struct Cyc_Core_Opt**)& _tmpB19->f1;_tmpB1C=
_tmpB19->f2;if(_tmpB1C != Cyc_Absyn_MemKind)goto _LL584;}}_LL583: _tmpB1F=_tmpB1B;
goto _LL585;_LL584: {struct Cyc_Absyn_Less_kb_struct*_tmpB1D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB1D->tag != 2)goto _LL586;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=(struct
Cyc_Core_Opt**)& _tmpB1D->f1;_tmpB20=_tmpB1D->f2;if(_tmpB20 != Cyc_Absyn_AnyKind)
goto _LL586;}}_LL585:*_tmpB1F=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_BoxKind);
goto _LL57F;_LL586: {struct Cyc_Absyn_Less_kb_struct*_tmpB21=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB21->tag != 2)goto _LL588;else{_tmpB22=_tmpB21->f1;_tmpB23=(struct
Cyc_Core_Opt**)& _tmpB21->f1;_tmpB24=_tmpB21->f2;if(_tmpB24 != Cyc_Absyn_TopRgnKind)
goto _LL588;}}_LL587:*_tmpB23=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_RgnKind);
goto _LL57F;_LL588: {struct Cyc_Absyn_Less_kb_struct*_tmpB25=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB25->tag != 2)goto _LL58A;else{_tmpB26=_tmpB25->f1;_tmpB27=(struct
Cyc_Core_Opt**)& _tmpB25->f1;_tmpB28=_tmpB25->f2;}}_LL589:*_tmpB27=Cyc_Tcutil_kind_to_bound_opt(
_tmpB28);goto _LL57F;_LL58A: {struct Cyc_Absyn_Eq_kb_struct*_tmpB29=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB15;if(_tmpB29->tag != 0)goto _LL58C;else{_tmpB2A=_tmpB29->f1;if(_tmpB2A != Cyc_Absyn_MemKind)
goto _LL58C;}}_LL58B:{const char*_tmp1277;void*_tmp1276;(_tmp1276=0,Cyc_Tcutil_terr(
loc,((_tmp1277="functions can't abstract M types",_tag_dyneither(_tmp1277,
sizeof(char),33))),_tag_dyneither(_tmp1276,sizeof(void*),0)));}goto _LL57F;_LL58C:;
_LL58D: goto _LL57F;_LL57F:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmpA94.kind_env,*_tmp9A1);_tmpA94.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpA94.r,_tmpA94.free_vars,*_tmp9A1);{struct Cyc_List_List*tvs=_tmpA94.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB31;int _tmpB32;struct
_tuple24 _tmpB30=*((struct _tuple24*)tvs->hd);_tmpB31=_tmpB30.f1;_tmpB32=_tmpB30.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB31,_tmpB32);}}{struct Cyc_List_List*evs=_tmpA94.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpB34;int _tmpB35;struct _tuple25 _tmpB33=*((struct _tuple25*)evs->hd);
_tmpB34=_tmpB33.f1;_tmpB35=_tmpB33.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpB34,_tmpB35);}};}goto _LL4B1;};}_LL4D0: {struct Cyc_Absyn_TupleType_struct*
_tmp9AA=(struct Cyc_Absyn_TupleType_struct*)_tmp96A;if(_tmp9AA->tag != 11)goto
_LL4D2;else{_tmp9AB=_tmp9AA->f1;}}_LL4D1: for(0;_tmp9AB != 0;_tmp9AB=_tmp9AB->tl){
struct _tuple11*_tmpB37=(struct _tuple11*)_tmp9AB->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,(*_tmpB37).f2,1);((*_tmpB37).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpB37).f1).print_const,(*_tmpB37).f2);}goto _LL4B1;_LL4D2: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9AC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp96A;if(_tmp9AC->tag != 13)goto
_LL4D4;else{_tmp9AD=_tmp9AC->f1;_tmp9AE=_tmp9AC->f2;}}_LL4D3: {struct
_RegionHandle*_tmpB38=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp9AE != 0;_tmp9AE=_tmp9AE->tl){struct Cyc_Absyn_Aggrfield _tmpB3A;struct
_dyneither_ptr*_tmpB3B;struct Cyc_Absyn_Tqual _tmpB3C;struct Cyc_Absyn_Tqual*
_tmpB3D;void*_tmpB3E;struct Cyc_Absyn_Exp*_tmpB3F;struct Cyc_List_List*_tmpB40;
struct Cyc_Absyn_Aggrfield*_tmpB39=(struct Cyc_Absyn_Aggrfield*)_tmp9AE->hd;
_tmpB3A=*_tmpB39;_tmpB3B=_tmpB3A.name;_tmpB3C=_tmpB3A.tq;_tmpB3D=(struct Cyc_Absyn_Tqual*)&(*
_tmpB39).tq;_tmpB3E=_tmpB3A.type;_tmpB3F=_tmpB3A.width;_tmpB40=_tmpB3A.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB3B)){
const char*_tmp127B;void*_tmp127A[1];struct Cyc_String_pa_struct _tmp1279;(_tmp1279.tag=
0,((_tmp1279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3B),((
_tmp127A[0]=& _tmp1279,Cyc_Tcutil_terr(loc,((_tmp127B="duplicate field %s",
_tag_dyneither(_tmp127B,sizeof(char),19))),_tag_dyneither(_tmp127A,sizeof(void*),
1)))))));}{const char*_tmp127C;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB3B,((
_tmp127C="",_tag_dyneither(_tmp127C,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp127D;prev_fields=((_tmp127D=_region_malloc(_tmpB38,sizeof(*_tmp127D)),((
_tmp127D->hd=_tmpB3B,((_tmp127D->tl=prev_fields,_tmp127D))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,_tmpB3E,1);_tmpB3D->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB3D->print_const,_tmpB3E);if(_tmp9AD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpB3E)){const char*_tmp1281;void*_tmp1280[1];struct Cyc_String_pa_struct _tmp127F;(
_tmp127F.tag=0,((_tmp127F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpB3B),((_tmp1280[0]=& _tmp127F,Cyc_Tcutil_warn(loc,((_tmp1281="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1281,sizeof(char),74))),_tag_dyneither(_tmp1280,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpB3E,_tmpB3F,_tmpB3B);Cyc_Tcutil_check_field_atts(
loc,_tmpB3B,_tmpB40);}}goto _LL4B1;}_LL4D4: {struct Cyc_Absyn_AggrType_struct*
_tmp9AF=(struct Cyc_Absyn_AggrType_struct*)_tmp96A;if(_tmp9AF->tag != 12)goto
_LL4D6;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9B0.aggr_info;_tmp9B2=(union Cyc_Absyn_AggrInfoU*)&(
_tmp9AF->f1).aggr_info;_tmp9B3=_tmp9B0.targs;_tmp9B4=(struct Cyc_List_List**)&(
_tmp9AF->f1).targs;}}_LL4D5:{union Cyc_Absyn_AggrInfoU _tmpB49=*_tmp9B2;struct
_tuple4 _tmpB4A;enum Cyc_Absyn_AggrKind _tmpB4B;struct _tuple2*_tmpB4C;struct Cyc_Core_Opt*
_tmpB4D;struct Cyc_Absyn_Aggrdecl**_tmpB4E;struct Cyc_Absyn_Aggrdecl*_tmpB4F;
_LL58F: if((_tmpB49.UnknownAggr).tag != 1)goto _LL591;_tmpB4A=(struct _tuple4)(
_tmpB49.UnknownAggr).val;_tmpB4B=_tmpB4A.f1;_tmpB4C=_tmpB4A.f2;_tmpB4D=_tmpB4A.f3;
_LL590: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpB50;_push_handler(&
_tmpB50);{int _tmpB52=0;if(setjmp(_tmpB50.handler))_tmpB52=1;if(!_tmpB52){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB4C);{struct Cyc_Absyn_Aggrdecl*_tmpB53=*adp;if(_tmpB53->kind != 
_tmpB4B){if(_tmpB53->kind == Cyc_Absyn_StructA){const char*_tmp1286;void*_tmp1285[
2];struct Cyc_String_pa_struct _tmp1284;struct Cyc_String_pa_struct _tmp1283;(
_tmp1283.tag=0,((_tmp1283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp1284.tag=0,((_tmp1284.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4C)),((_tmp1285[0]=& _tmp1284,((
_tmp1285[1]=& _tmp1283,Cyc_Tcutil_terr(loc,((_tmp1286="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1286,sizeof(char),40))),_tag_dyneither(_tmp1285,sizeof(void*),
2)))))))))))));}else{const char*_tmp128B;void*_tmp128A[2];struct Cyc_String_pa_struct
_tmp1289;struct Cyc_String_pa_struct _tmp1288;(_tmp1288.tag=0,((_tmp1288.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4C)),((
_tmp1289.tag=0,((_tmp1289.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp128A[0]=& _tmp1289,((_tmp128A[1]=& _tmp1288,Cyc_Tcutil_terr(loc,((
_tmp128B="expecting union %s instead of struct %s",_tag_dyneither(_tmp128B,
sizeof(char),40))),_tag_dyneither(_tmp128A,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpB4D  && (int)_tmpB4D->v){if(!((unsigned int)_tmpB53->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB53->impl))->tagged){const char*
_tmp128F;void*_tmp128E[1];struct Cyc_String_pa_struct _tmp128D;(_tmp128D.tag=0,((
_tmp128D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp128E[0]=& _tmp128D,Cyc_Tcutil_terr(loc,((_tmp128F="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp128F,sizeof(char),42))),_tag_dyneither(_tmp128E,sizeof(void*),
1)))))));}}*_tmp9B2=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpB51=(void*)_exn_thrown;void*_tmpB60=_tmpB51;_LL594: {struct Cyc_Dict_Absent_struct*
_tmpB61=(struct Cyc_Dict_Absent_struct*)_tmpB60;if(_tmpB61->tag != Cyc_Dict_Absent)
goto _LL596;}_LL595: {struct Cyc_Tcenv_Genv*_tmpB62=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1290;struct Cyc_Absyn_Aggrdecl*_tmpB63=(_tmp1290=
_cycalloc(sizeof(*_tmp1290)),((_tmp1290->kind=_tmpB4B,((_tmp1290->sc=Cyc_Absyn_Extern,((
_tmp1290->name=_tmpB4C,((_tmp1290->tvs=0,((_tmp1290->impl=0,((_tmp1290->attributes=
0,_tmp1290)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpB62,loc,_tmpB63);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB4C);*_tmp9B2=Cyc_Absyn_KnownAggr(adp);if(*_tmp9B4 != 0){{const char*
_tmp1294;void*_tmp1293[1];struct Cyc_String_pa_struct _tmp1292;(_tmp1292.tag=0,((
_tmp1292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp1293[0]=& _tmp1292,Cyc_Tcutil_terr(loc,((_tmp1294="declare parameterized type %s before using",
_tag_dyneither(_tmp1294,sizeof(char),43))),_tag_dyneither(_tmp1293,sizeof(void*),
1)))))));}return cvtenv;}goto _LL593;}_LL596:;_LL597:(void)_throw(_tmpB60);_LL593:;}};}
_tmpB4F=*adp;goto _LL592;}_LL591: if((_tmpB49.KnownAggr).tag != 2)goto _LL58E;
_tmpB4E=(struct Cyc_Absyn_Aggrdecl**)(_tmpB49.KnownAggr).val;_tmpB4F=*_tmpB4E;
_LL592: {struct Cyc_List_List*tvs=_tmpB4F->tvs;struct Cyc_List_List*ts=*_tmp9B4;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpB68=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpB69=(void*)ts->hd;{struct _tuple0 _tmp1295;
struct _tuple0 _tmpB6B=(_tmp1295.f1=Cyc_Absyn_compress_kb(_tmpB68->kind),((
_tmp1295.f2=_tmpB69,_tmp1295)));void*_tmpB6C;void*_tmpB6E;struct Cyc_Absyn_Tvar*
_tmpB70;_LL599: _tmpB6C=_tmpB6B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB6D=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB6C;if(_tmpB6D->tag != 1)goto _LL59B;};
_tmpB6E=_tmpB6B.f2;{struct Cyc_Absyn_VarType_struct*_tmpB6F=(struct Cyc_Absyn_VarType_struct*)
_tmpB6E;if(_tmpB6F->tag != 2)goto _LL59B;else{_tmpB70=_tmpB6F->f1;}};_LL59A: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB70);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB70,1);continue;_LL59B:;_LL59C: goto _LL598;_LL598:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1299;void*_tmp1298[1];struct Cyc_String_pa_struct _tmp1297;(
_tmp1297.tag=0,((_tmp1297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4F->name)),((_tmp1298[0]=& _tmp1297,Cyc_Tcutil_terr(loc,((_tmp1299="too many parameters for type %s",
_tag_dyneither(_tmp1299,sizeof(char),32))),_tag_dyneither(_tmp1298,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp129A;hidden_ts=((
_tmp129A=_cycalloc(sizeof(*_tmp129A)),((_tmp129A->hd=e,((_tmp129A->tl=hidden_ts,
_tmp129A))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}*
_tmp9B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp9B4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL58E:;}goto _LL4B1;_LL4D6: {struct Cyc_Absyn_TypedefType_struct*
_tmp9B5=(struct Cyc_Absyn_TypedefType_struct*)_tmp96A;if(_tmp9B5->tag != 18)goto
_LL4D8;else{_tmp9B6=_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;_tmp9B8=(struct Cyc_List_List**)&
_tmp9B5->f2;_tmp9B9=_tmp9B5->f3;_tmp9BA=(struct Cyc_Absyn_Typedefdecl**)& _tmp9B5->f3;
_tmp9BB=_tmp9B5->f4;_tmp9BC=(void***)& _tmp9B5->f4;}}_LL4D7: {struct Cyc_List_List*
targs=*_tmp9B8;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpB75;
_push_handler(& _tmpB75);{int _tmpB77=0;if(setjmp(_tmpB75.handler))_tmpB77=1;if(!
_tmpB77){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9B6);;_pop_handler();}else{
void*_tmpB76=(void*)_exn_thrown;void*_tmpB79=_tmpB76;_LL59E: {struct Cyc_Dict_Absent_struct*
_tmpB7A=(struct Cyc_Dict_Absent_struct*)_tmpB79;if(_tmpB7A->tag != Cyc_Dict_Absent)
goto _LL5A0;}_LL59F:{const char*_tmp129E;void*_tmp129D[1];struct Cyc_String_pa_struct
_tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp129D[0]=& _tmp129C,Cyc_Tcutil_terr(
loc,((_tmp129E="unbound typedef name %s",_tag_dyneither(_tmp129E,sizeof(char),24))),
_tag_dyneither(_tmp129D,sizeof(void*),1)))))));}return cvtenv;_LL5A0:;_LL5A1:(
void)_throw(_tmpB79);_LL59D:;}};}*_tmp9BA=(struct Cyc_Absyn_Typedefdecl*)td;
_tmp9B6[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpB7E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple14*_tmp12A1;struct Cyc_List_List*
_tmp12A0;inst=((_tmp12A0=_region_malloc(_tmpB7E,sizeof(*_tmp12A0)),((_tmp12A0->hd=((
_tmp12A1=_region_malloc(_tmpB7E,sizeof(*_tmp12A1)),((_tmp12A1->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp12A1->f2=(void*)ts->hd,_tmp12A1)))))),((_tmp12A0->tl=inst,_tmp12A0))))));};}
if(ts != 0){const char*_tmp12A5;void*_tmp12A4[1];struct Cyc_String_pa_struct
_tmp12A3;(_tmp12A3.tag=0,((_tmp12A3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp12A4[0]=& _tmp12A3,Cyc_Tcutil_terr(
loc,((_tmp12A5="too many parameters for typedef %s",_tag_dyneither(_tmp12A5,
sizeof(char),35))),_tag_dyneither(_tmp12A4,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){enum Cyc_Absyn_Kind k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);void*e=
Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp12A6;hidden_ts=((_tmp12A6=
_cycalloc(sizeof(*_tmp12A6)),((_tmp12A6->hd=e,((_tmp12A6->tl=hidden_ts,_tmp12A6))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{struct _tuple14*
_tmp12A9;struct Cyc_List_List*_tmp12A8;inst=(struct Cyc_List_List*)((_tmp12A8=
_cycalloc(sizeof(*_tmp12A8)),((_tmp12A8->hd=(struct _tuple14*)((_tmp12A9=
_cycalloc(sizeof(*_tmp12A9)),((_tmp12A9->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmp12A9->f2=e,_tmp12A9)))))),((_tmp12A8->tl=inst,_tmp12A8))))));};}*_tmp9B8=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(_tmpB7E,inst,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);void**_tmp12AA;*_tmp9BC=((
_tmp12AA=_cycalloc(sizeof(*_tmp12AA)),((_tmp12AA[0]=new_typ,_tmp12AA))));}}goto
_LL4B1;};}_LL4D8: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9BD=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp96A;if(_tmp9BD->tag != 22)goto _LL4DA;}_LL4D9: goto _LL4DB;_LL4DA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp9BE=(struct Cyc_Absyn_HeapRgn_struct*)_tmp96A;if(_tmp9BE->tag != 21)goto _LL4DC;}
_LL4DB: goto _LL4B1;_LL4DC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp9BF=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp96A;if(_tmp9BF->tag != 16)goto _LL4DE;else{
_tmp9C0=(void*)_tmp9BF->f1;}}_LL4DD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp9C0,1);goto _LL4B1;_LL4DE: {struct Cyc_Absyn_DynRgnType_struct*
_tmp9C1=(struct Cyc_Absyn_DynRgnType_struct*)_tmp96A;if(_tmp9C1->tag != 17)goto
_LL4E0;else{_tmp9C2=(void*)_tmp9C1->f1;_tmp9C3=(void*)_tmp9C1->f2;}}_LL4DF:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9C2,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9C3,1);
goto _LL4B1;_LL4E0: {struct Cyc_Absyn_AccessEff_struct*_tmp9C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp96A;if(_tmp9C4->tag != 23)goto _LL4E2;else{_tmp9C5=(void*)_tmp9C4->f1;}}_LL4E1:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_TopRgnKind,_tmp9C5,
1);goto _LL4B1;_LL4E2: {struct Cyc_Absyn_RgnsEff_struct*_tmp9C6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp96A;if(_tmp9C6->tag != 25)goto _LL4E4;else{_tmp9C7=(void*)_tmp9C6->f1;}}_LL4E3:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp9C7,1);
goto _LL4B1;_LL4E4: {struct Cyc_Absyn_JoinEff_struct*_tmp9C8=(struct Cyc_Absyn_JoinEff_struct*)
_tmp96A;if(_tmp9C8->tag != 24)goto _LL4B1;else{_tmp9C9=_tmp9C8->f1;}}_LL4E5: for(0;
_tmp9C9 != 0;_tmp9C9=_tmp9C9->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_EffKind,(void*)_tmp9C9->hd,1);}goto _LL4B1;_LL4B1:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpB88=t;struct Cyc_Core_Opt*
_tmpB8A;struct Cyc_Core_Opt*_tmpB8B;_LL5A3: {struct Cyc_Absyn_Evar_struct*_tmpB89=(
struct Cyc_Absyn_Evar_struct*)_tmpB88;if(_tmpB89->tag != 1)goto _LL5A5;else{_tmpB8A=
_tmpB89->f1;_tmpB8B=_tmpB89->f2;}}_LL5A4: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpB8C=_tmpB8A;struct Cyc_Core_Opt _tmpB8D;enum Cyc_Absyn_Kind _tmpB8E;_LL5A8: if(
_tmpB8C != 0)goto _LL5AA;_LL5A9:{const char*_tmp12AB;s=((_tmp12AB="kind=NULL ",
_tag_dyneither(_tmp12AB,sizeof(char),11)));}goto _LL5A7;_LL5AA: if(_tmpB8C == 0)
goto _LL5A7;_tmpB8D=*_tmpB8C;_tmpB8E=(enum Cyc_Absyn_Kind)_tmpB8D.v;_LL5AB:{const
char*_tmp12AF;void*_tmp12AE[1];struct Cyc_String_pa_struct _tmp12AD;s=(struct
_dyneither_ptr)((_tmp12AD.tag=0,((_tmp12AD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB8E)),((_tmp12AE[0]=& _tmp12AD,Cyc_aprintf(((
_tmp12AF="kind=%s ",_tag_dyneither(_tmp12AF,sizeof(char),9))),_tag_dyneither(
_tmp12AE,sizeof(void*),1))))))));}goto _LL5A7;_LL5A7:;}{struct Cyc_Core_Opt*
_tmpB93=_tmpB8B;struct Cyc_Core_Opt _tmpB94;void*_tmpB95;_LL5AD: if(_tmpB93 != 0)
goto _LL5AF;_LL5AE:{const char*_tmp12B3;void*_tmp12B2[1];struct Cyc_String_pa_struct
_tmp12B1;s=(struct _dyneither_ptr)((_tmp12B1.tag=0,((_tmp12B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp12B2[0]=& _tmp12B1,Cyc_aprintf(((
_tmp12B3="%s ref=NULL",_tag_dyneither(_tmp12B3,sizeof(char),12))),_tag_dyneither(
_tmp12B2,sizeof(void*),1))))))));}goto _LL5AC;_LL5AF: if(_tmpB93 == 0)goto _LL5AC;
_tmpB94=*_tmpB93;_tmpB95=(void*)_tmpB94.v;_LL5B0:{const char*_tmp12B8;void*
_tmp12B7[2];struct Cyc_String_pa_struct _tmp12B6;struct Cyc_String_pa_struct
_tmp12B5;s=(struct _dyneither_ptr)((_tmp12B5.tag=0,((_tmp12B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB95)),((
_tmp12B6.tag=0,((_tmp12B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp12B7[0]=& _tmp12B6,((_tmp12B7[1]=& _tmp12B5,Cyc_aprintf(((_tmp12B8="%s ref=%s",
_tag_dyneither(_tmp12B8,sizeof(char),10))),_tag_dyneither(_tmp12B7,sizeof(void*),
2))))))))))))));}goto _LL5AC;_LL5AC:;}{const char*_tmp12BC;void*_tmp12BB[1];struct
Cyc_String_pa_struct _tmp12BA;(_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp12BB[0]=& _tmp12BA,Cyc_fprintf(Cyc_stderr,((
_tmp12BC="evar info: %s\n",_tag_dyneither(_tmp12BC,sizeof(char),15))),
_tag_dyneither(_tmp12BB,sizeof(void*),1)))))));}goto _LL5A2;}_LL5A5:;_LL5A6: goto
_LL5A2;_LL5A2:;}{const char*_tmp12C2;void*_tmp12C1[3];struct Cyc_String_pa_struct
_tmp12C0;struct Cyc_String_pa_struct _tmp12BF;struct Cyc_String_pa_struct _tmp12BE;(
_tmp12BE.tag=0,((_tmp12BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp12BF.tag=0,((_tmp12BF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp12C0.tag=0,((
_tmp12C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12C1[0]=& _tmp12C0,((_tmp12C1[1]=& _tmp12BF,((_tmp12C1[2]=& _tmp12BE,Cyc_Tcutil_terr(
loc,((_tmp12C2="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp12C2,sizeof(char),51))),_tag_dyneither(_tmp12C1,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpBA5=e->r;
struct Cyc_List_List*_tmpBAB;struct Cyc_Absyn_Exp*_tmpBAD;struct Cyc_Absyn_Exp*
_tmpBAE;struct Cyc_Absyn_Exp*_tmpBAF;struct Cyc_Absyn_Exp*_tmpBB1;struct Cyc_Absyn_Exp*
_tmpBB2;struct Cyc_Absyn_Exp*_tmpBB4;struct Cyc_Absyn_Exp*_tmpBB5;struct Cyc_Absyn_Exp*
_tmpBB7;struct Cyc_Absyn_Exp*_tmpBB8;void*_tmpBBA;struct Cyc_Absyn_Exp*_tmpBBB;
void*_tmpBBD;void*_tmpBBF;void*_tmpBC1;struct Cyc_Absyn_Exp*_tmpBC3;_LL5B2: {
struct Cyc_Absyn_Const_e_struct*_tmpBA6=(struct Cyc_Absyn_Const_e_struct*)_tmpBA5;
if(_tmpBA6->tag != 0)goto _LL5B4;}_LL5B3: goto _LL5B5;_LL5B4: {struct Cyc_Absyn_Enum_e_struct*
_tmpBA7=(struct Cyc_Absyn_Enum_e_struct*)_tmpBA5;if(_tmpBA7->tag != 32)goto _LL5B6;}
_LL5B5: goto _LL5B7;_LL5B6: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpBA8=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBA5;if(_tmpBA8->tag != 33)goto _LL5B8;}_LL5B7: goto _LL5B9;_LL5B8: {struct Cyc_Absyn_Var_e_struct*
_tmpBA9=(struct Cyc_Absyn_Var_e_struct*)_tmpBA5;if(_tmpBA9->tag != 1)goto _LL5BA;}
_LL5B9: goto _LL5B1;_LL5BA: {struct Cyc_Absyn_Primop_e_struct*_tmpBAA=(struct Cyc_Absyn_Primop_e_struct*)
_tmpBA5;if(_tmpBAA->tag != 3)goto _LL5BC;else{_tmpBAB=_tmpBAA->f2;}}_LL5BB: for(0;
_tmpBAB != 0;_tmpBAB=_tmpBAB->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpBAB->hd,te,cvtenv);}goto _LL5B1;_LL5BC: {struct Cyc_Absyn_Conditional_e_struct*
_tmpBAC=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBA5;if(_tmpBAC->tag != 6)goto
_LL5BE;else{_tmpBAD=_tmpBAC->f1;_tmpBAE=_tmpBAC->f2;_tmpBAF=_tmpBAC->f3;}}_LL5BD:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAD,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBAE,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAF,te,
cvtenv);goto _LL5B1;_LL5BE: {struct Cyc_Absyn_And_e_struct*_tmpBB0=(struct Cyc_Absyn_And_e_struct*)
_tmpBA5;if(_tmpBB0->tag != 7)goto _LL5C0;else{_tmpBB1=_tmpBB0->f1;_tmpBB2=_tmpBB0->f2;}}
_LL5BF: _tmpBB4=_tmpBB1;_tmpBB5=_tmpBB2;goto _LL5C1;_LL5C0: {struct Cyc_Absyn_Or_e_struct*
_tmpBB3=(struct Cyc_Absyn_Or_e_struct*)_tmpBA5;if(_tmpBB3->tag != 8)goto _LL5C2;
else{_tmpBB4=_tmpBB3->f1;_tmpBB5=_tmpBB3->f2;}}_LL5C1: _tmpBB7=_tmpBB4;_tmpBB8=
_tmpBB5;goto _LL5C3;_LL5C2: {struct Cyc_Absyn_SeqExp_e_struct*_tmpBB6=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA5;if(_tmpBB6->tag != 9)goto _LL5C4;else{_tmpBB7=_tmpBB6->f1;_tmpBB8=_tmpBB6->f2;}}
_LL5C3: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB7,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB8,te,cvtenv);goto _LL5B1;_LL5C4: {
struct Cyc_Absyn_Cast_e_struct*_tmpBB9=(struct Cyc_Absyn_Cast_e_struct*)_tmpBA5;
if(_tmpBB9->tag != 15)goto _LL5C6;else{_tmpBBA=(void*)_tmpBB9->f1;_tmpBBB=_tmpBB9->f2;}}
_LL5C5: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBB,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmpBBA,1);goto
_LL5B1;_LL5C6: {struct Cyc_Absyn_Offsetof_e_struct*_tmpBBC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA5;if(_tmpBBC->tag != 20)goto _LL5C8;else{_tmpBBD=(void*)_tmpBBC->f1;}}_LL5C7:
_tmpBBF=_tmpBBD;goto _LL5C9;_LL5C8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpBBE=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBA5;if(_tmpBBE->tag != 18)goto _LL5CA;
else{_tmpBBF=(void*)_tmpBBE->f1;}}_LL5C9: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,Cyc_Absyn_AnyKind,_tmpBBF,1);goto _LL5B1;_LL5CA: {struct Cyc_Absyn_Valueof_e_struct*
_tmpBC0=(struct Cyc_Absyn_Valueof_e_struct*)_tmpBA5;if(_tmpBC0->tag != 39)goto
_LL5CC;else{_tmpBC1=(void*)_tmpBC0->f1;}}_LL5CB: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,Cyc_Absyn_IntKind,_tmpBC1,1);goto _LL5B1;_LL5CC: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpBC2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBA5;if(_tmpBC2->tag != 19)goto
_LL5CE;else{_tmpBC3=_tmpBC2->f1;}}_LL5CD: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBC3,te,cvtenv);goto _LL5B1;_LL5CE:;_LL5CF: {const char*_tmp12C5;void*_tmp12C4;(
_tmp12C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12C5="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp12C5,sizeof(char),66))),_tag_dyneither(_tmp12C4,sizeof(void*),
0)));}_LL5B1:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t){struct Cyc_List_List*_tmpBC6=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpBC8;struct
_tuple24 _tmpBC7=*((struct _tuple24*)vs->hd);_tmpBC8=_tmpBC7.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpBC6,_tmpBC8);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpBCA;struct _tuple25 _tmpBC9=*((struct _tuple25*)evs->hd);_tmpBCA=_tmpBC9.f1;{
void*_tmpBCB=Cyc_Tcutil_compress(_tmpBCA);struct Cyc_Core_Opt*_tmpBCD;struct Cyc_Core_Opt**
_tmpBCE;_LL5D1: {struct Cyc_Absyn_Evar_struct*_tmpBCC=(struct Cyc_Absyn_Evar_struct*)
_tmpBCB;if(_tmpBCC->tag != 1)goto _LL5D3;else{_tmpBCD=_tmpBCC->f4;_tmpBCE=(struct
Cyc_Core_Opt**)& _tmpBCC->f4;}}_LL5D2: if(*_tmpBCE == 0){struct Cyc_Core_Opt*
_tmp12C6;*_tmpBCE=((_tmp12C6=_cycalloc(sizeof(*_tmp12C6)),((_tmp12C6->v=_tmpBC6,
_tmp12C6))));}else{struct Cyc_List_List*_tmpBD0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpBCE))->v;struct Cyc_List_List*_tmpBD1=0;for(0;_tmpBD0 != 0;
_tmpBD0=_tmpBD0->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpBC6,(struct Cyc_Absyn_Tvar*)_tmpBD0->hd)){struct Cyc_List_List*_tmp12C7;
_tmpBD1=((_tmp12C7=_cycalloc(sizeof(*_tmp12C7)),((_tmp12C7->hd=(struct Cyc_Absyn_Tvar*)
_tmpBD0->hd,((_tmp12C7->tl=_tmpBD1,_tmp12C7))))));}}{struct Cyc_Core_Opt*_tmp12C8;*
_tmpBCE=((_tmp12C8=_cycalloc(sizeof(*_tmp12C8)),((_tmp12C8->v=_tmpBD1,_tmp12C8))));};}
goto _LL5D0;_LL5D3:;_LL5D4: goto _LL5D0;_LL5D0:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpBD4=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpBD5=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp12C9;struct Cyc_Tcutil_CVTEnv _tmpBD6=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp12C9.r=_tmpBD5,((_tmp12C9.kind_env=_tmpBD4,((_tmp12C9.free_vars=0,((_tmp12C9.free_evars=
0,((_tmp12C9.generalize_evars=generalize_evars,((_tmp12C9.fn_result=0,_tmp12C9)))))))))))),
Cyc_Absyn_MemKind,t);struct Cyc_List_List*_tmpBD7=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple24*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmpBD5,(struct Cyc_Absyn_Tvar*(*)(struct _tuple24*))Cyc_Core_fst,
_tmpBD6.free_vars);struct Cyc_List_List*_tmpBD8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpBD5,(void*(*)(struct _tuple25*))Cyc_Core_fst,_tmpBD6.free_evars);if(_tmpBD4 != 
0){struct Cyc_List_List*_tmpBD9=0;{struct Cyc_List_List*_tmpBDA=_tmpBD7;for(0;(
unsigned int)_tmpBDA;_tmpBDA=_tmpBDA->tl){struct Cyc_Absyn_Tvar*_tmpBDB=(struct
Cyc_Absyn_Tvar*)_tmpBDA->hd;int found=0;{struct Cyc_List_List*_tmpBDC=_tmpBD4;for(
0;(unsigned int)_tmpBDC;_tmpBDC=_tmpBDC->tl){if(Cyc_Absyn_tvar_cmp(_tmpBDB,(
struct Cyc_Absyn_Tvar*)_tmpBDC->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp12CA;_tmpBD9=((_tmp12CA=_region_malloc(_tmpBD5,sizeof(*_tmp12CA)),((_tmp12CA->hd=(
struct Cyc_Absyn_Tvar*)_tmpBDA->hd,((_tmp12CA->tl=_tmpBD9,_tmp12CA))))));}}}
_tmpBD7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpBD9);}{struct Cyc_List_List*x=_tmpBD7;for(0;x != 0;x=x->tl){void*_tmpBDE=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpBE0;struct Cyc_Core_Opt**
_tmpBE1;struct Cyc_Core_Opt*_tmpBE3;struct Cyc_Core_Opt**_tmpBE4;enum Cyc_Absyn_Kind
_tmpBE5;struct Cyc_Core_Opt*_tmpBE7;struct Cyc_Core_Opt**_tmpBE8;enum Cyc_Absyn_Kind
_tmpBE9;struct Cyc_Core_Opt*_tmpBEB;struct Cyc_Core_Opt**_tmpBEC;enum Cyc_Absyn_Kind
_tmpBED;enum Cyc_Absyn_Kind _tmpBEF;_LL5D6: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpBDF=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpBDE;if(_tmpBDF->tag != 1)goto
_LL5D8;else{_tmpBE0=_tmpBDF->f1;_tmpBE1=(struct Cyc_Core_Opt**)& _tmpBDF->f1;}}
_LL5D7: _tmpBE4=_tmpBE1;goto _LL5D9;_LL5D8: {struct Cyc_Absyn_Less_kb_struct*
_tmpBE2=(struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBE2->tag != 2)goto _LL5DA;
else{_tmpBE3=_tmpBE2->f1;_tmpBE4=(struct Cyc_Core_Opt**)& _tmpBE2->f1;_tmpBE5=
_tmpBE2->f2;if(_tmpBE5 != Cyc_Absyn_MemKind)goto _LL5DA;}}_LL5D9:*_tmpBE4=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_BoxKind);goto _LL5D5;_LL5DA: {struct Cyc_Absyn_Less_kb_struct*_tmpBE6=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBE6->tag != 2)goto _LL5DC;else{
_tmpBE7=_tmpBE6->f1;_tmpBE8=(struct Cyc_Core_Opt**)& _tmpBE6->f1;_tmpBE9=_tmpBE6->f2;
if(_tmpBE9 != Cyc_Absyn_TopRgnKind)goto _LL5DC;}}_LL5DB:*_tmpBE8=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_RgnKind);goto _LL5D5;_LL5DC: {struct Cyc_Absyn_Less_kb_struct*_tmpBEA=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBEA->tag != 2)goto _LL5DE;else{
_tmpBEB=_tmpBEA->f1;_tmpBEC=(struct Cyc_Core_Opt**)& _tmpBEA->f1;_tmpBED=_tmpBEA->f2;}}
_LL5DD:*_tmpBEC=Cyc_Tcutil_kind_to_bound_opt(_tmpBED);goto _LL5D5;_LL5DE: {struct
Cyc_Absyn_Eq_kb_struct*_tmpBEE=(struct Cyc_Absyn_Eq_kb_struct*)_tmpBDE;if(_tmpBEE->tag
!= 0)goto _LL5E0;else{_tmpBEF=_tmpBEE->f1;if(_tmpBEF != Cyc_Absyn_MemKind)goto
_LL5E0;}}_LL5DF:{const char*_tmp12CE;void*_tmp12CD[1];struct Cyc_String_pa_struct
_tmp12CC;(_tmp12CC.tag=0,((_tmp12CC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp12CD[
0]=& _tmp12CC,Cyc_Tcutil_terr(loc,((_tmp12CE="type variable %s cannot have kind M",
_tag_dyneither(_tmp12CE,sizeof(char),36))),_tag_dyneither(_tmp12CD,sizeof(void*),
1)))))));}goto _LL5D5;_LL5E0:;_LL5E1: goto _LL5D5;_LL5D5:;}}if(_tmpBD7 != 0  || 
_tmpBD8 != 0){{void*_tmpBF3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpBF5;
struct Cyc_List_List*_tmpBF6;struct Cyc_List_List**_tmpBF7;struct Cyc_Core_Opt*
_tmpBF8;void*_tmpBF9;struct Cyc_List_List*_tmpBFA;int _tmpBFB;struct Cyc_Absyn_VarargInfo*
_tmpBFC;struct Cyc_List_List*_tmpBFD;struct Cyc_List_List*_tmpBFE;_LL5E3: {struct
Cyc_Absyn_FnType_struct*_tmpBF4=(struct Cyc_Absyn_FnType_struct*)_tmpBF3;if(
_tmpBF4->tag != 10)goto _LL5E5;else{_tmpBF5=_tmpBF4->f1;_tmpBF6=_tmpBF5.tvars;
_tmpBF7=(struct Cyc_List_List**)&(_tmpBF4->f1).tvars;_tmpBF8=_tmpBF5.effect;
_tmpBF9=_tmpBF5.ret_typ;_tmpBFA=_tmpBF5.args;_tmpBFB=_tmpBF5.c_varargs;_tmpBFC=
_tmpBF5.cyc_varargs;_tmpBFD=_tmpBF5.rgn_po;_tmpBFE=_tmpBF5.attributes;}}_LL5E4:
if(*_tmpBF7 == 0){*_tmpBF7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(
_tmpBD7);_tmpBD7=0;}goto _LL5E2;_LL5E5:;_LL5E6: goto _LL5E2;_LL5E2:;}if(_tmpBD7 != 0){
const char*_tmp12D2;void*_tmp12D1[1];struct Cyc_String_pa_struct _tmp12D0;(_tmp12D0.tag=
0,((_tmp12D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmpBD7->hd)->name),((_tmp12D1[0]=& _tmp12D0,Cyc_Tcutil_terr(loc,((_tmp12D2="unbound type variable %s",
_tag_dyneither(_tmp12D2,sizeof(char),25))),_tag_dyneither(_tmp12D1,sizeof(void*),
1)))))));}if(_tmpBD8 != 0)for(0;_tmpBD8 != 0;_tmpBD8=_tmpBD8->tl){void*e=(void*)
_tmpBD8->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind: _LL5E7:
if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp12D5;void*
_tmp12D4;(_tmp12D4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12D5="can't unify evar with unique region!",
_tag_dyneither(_tmp12D5,sizeof(char),37))),_tag_dyneither(_tmp12D4,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5E8: goto _LL5E9;case Cyc_Absyn_RgnKind:
_LL5E9: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12D8;
void*_tmp12D7;(_tmp12D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12D8="can't unify evar with heap!",_tag_dyneither(
_tmp12D8,sizeof(char),28))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5EA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12DB;void*_tmp12DA;(_tmp12DA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DB="can't unify evar with {}!",
_tag_dyneither(_tmp12DB,sizeof(char),26))),_tag_dyneither(_tmp12DA,sizeof(void*),
0)));}break;default: _LL5EB:{const char*_tmp12E0;void*_tmp12DF[2];struct Cyc_String_pa_struct
_tmp12DE;struct Cyc_String_pa_struct _tmp12DD;(_tmp12DD.tag=0,((_tmp12DD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12DE.tag=0,((_tmp12DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12DF[0]=& _tmp12DE,((_tmp12DF[1]=& _tmp12DD,Cyc_Tcutil_terr(loc,((
_tmp12E0="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12E0,sizeof(char),52))),_tag_dyneither(_tmp12DF,sizeof(void*),2)))))))))))));}
break;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpC0D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpC0F;struct Cyc_List_List*
_tmpC10;struct Cyc_Core_Opt*_tmpC11;void*_tmpC12;_LL5EE: {struct Cyc_Absyn_FnType_struct*
_tmpC0E=(struct Cyc_Absyn_FnType_struct*)_tmpC0D;if(_tmpC0E->tag != 10)goto _LL5F0;
else{_tmpC0F=_tmpC0E->f1;_tmpC10=_tmpC0F.tvars;_tmpC11=_tmpC0F.effect;_tmpC12=
_tmpC0F.ret_typ;}}_LL5EF: fd->tvs=_tmpC10;fd->effect=_tmpC11;goto _LL5ED;_LL5F0:;
_LL5F1: {const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E3="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp12E3,sizeof(char),38))),_tag_dyneither(_tmp12E2,sizeof(void*),
0)));}_LL5ED:;}{struct _RegionHandle*_tmpC15=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp12E4;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple20*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC15,(struct _dyneither_ptr*(*)(struct _tuple20*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp12E4="function declaration has repeated parameter",
_tag_dyneither(_tmp12E4,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp12E5;fd->cached_typ=((
_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((_tmp12E5->v=t,_tmp12E5))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpC18=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp12E6;
struct Cyc_Tcutil_CVTEnv _tmpC19=Cyc_Tcutil_check_valid_type(loc,te,((_tmp12E6.r=
_tmpC18,((_tmp12E6.kind_env=bound_tvars,((_tmp12E6.free_vars=0,((_tmp12E6.free_evars=
0,((_tmp12E6.generalize_evars=0,((_tmp12E6.fn_result=0,_tmp12E6)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpC1A=Cyc_Tcutil_remove_bound_tvars(
_tmpC18,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple24*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC18,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple24*))Cyc_Core_fst,_tmpC19.free_vars),bound_tvars);struct Cyc_List_List*
_tmpC1B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple25*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC18,(void*(*)(struct _tuple25*))Cyc_Core_fst,
_tmpC19.free_evars);{struct Cyc_List_List*fs=_tmpC1A;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC1C=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp12EB;void*_tmp12EA[2];struct Cyc_String_pa_struct _tmp12E9;struct Cyc_String_pa_struct
_tmp12E8;(_tmp12E8.tag=0,((_tmp12E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12E9.tag=0,((_tmp12E9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpC1C),((_tmp12EA[0]=& _tmp12E9,((
_tmp12EA[1]=& _tmp12E8,Cyc_Tcutil_terr(loc,((_tmp12EB="unbound type variable %s in type %s",
_tag_dyneither(_tmp12EB,sizeof(char),36))),_tag_dyneither(_tmp12EA,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpC1B != 0)for(0;_tmpC1B != 0;_tmpC1B=
_tmpC1B->tl){void*e=(void*)_tmpC1B->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind:
_LL5F2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*
_tmp12EE;void*_tmp12ED;(_tmp12ED=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EE="can't unify evar with unique region!",
_tag_dyneither(_tmp12EE,sizeof(char),37))),_tag_dyneither(_tmp12ED,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5F3: goto _LL5F4;case Cyc_Absyn_RgnKind:
_LL5F4: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12F1;
void*_tmp12F0;(_tmp12F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12F1="can't unify evar with heap!",_tag_dyneither(
_tmp12F1,sizeof(char),28))),_tag_dyneither(_tmp12F0,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5F5: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12F4;void*_tmp12F3;(_tmp12F3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F4="can't unify evar with {}!",
_tag_dyneither(_tmp12F4,sizeof(char),26))),_tag_dyneither(_tmp12F3,sizeof(void*),
0)));}break;default: _LL5F6:{const char*_tmp12F9;void*_tmp12F8[2];struct Cyc_String_pa_struct
_tmp12F7;struct Cyc_String_pa_struct _tmp12F6;(_tmp12F6.tag=0,((_tmp12F6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12F7.tag=0,((_tmp12F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12F8[0]=& _tmp12F7,((_tmp12F8[1]=& _tmp12F6,Cyc_Tcutil_terr(loc,((
_tmp12F9="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12F9,sizeof(char),52))),_tag_dyneither(_tmp12F8,sizeof(void*),2)))))))))))));}
break;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp12FE;void*_tmp12FD[2];struct Cyc_String_pa_struct
_tmp12FC;struct Cyc_String_pa_struct _tmp12FB;(_tmp12FB.tag=0,((_tmp12FB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp12FC.tag=
0,((_tmp12FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp12FD[0]=&
_tmp12FC,((_tmp12FD[1]=& _tmp12FB,Cyc_Tcutil_terr(loc,((_tmp12FE="%s: %s",
_tag_dyneither(_tmp12FE,sizeof(char),7))),_tag_dyneither(_tmp12FD,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp12FF;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp12FF="duplicate type variable",
_tag_dyneither(_tmp12FF,sizeof(char),24))));}struct _tuple27{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp1300;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp1300="",_tag_dyneither(_tmp1300,sizeof(char),1))))!= 0){struct _tuple27*
_tmp1303;struct Cyc_List_List*_tmp1302;fields=((_tmp1302=_cycalloc(sizeof(*
_tmp1302)),((_tmp1302->hd=((_tmp1303=_cycalloc(sizeof(*_tmp1303)),((_tmp1303->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1303->f2=0,_tmp1303)))))),((
_tmp1302->tl=fields,_tmp1302))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp1305;const char*_tmp1304;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1305="struct",
_tag_dyneither(_tmp1305,sizeof(char),7)):((_tmp1304="union",_tag_dyneither(
_tmp1304,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple28 _tmpC35;struct Cyc_List_List*_tmpC36;void*_tmpC37;struct _tuple28*
_tmpC34=(struct _tuple28*)des->hd;_tmpC35=*_tmpC34;_tmpC36=_tmpC35.f1;_tmpC37=
_tmpC35.f2;if(_tmpC36 == 0){struct Cyc_List_List*_tmpC38=fields;for(0;_tmpC38 != 0;
_tmpC38=_tmpC38->tl){if(!(*((struct _tuple27*)_tmpC38->hd)).f2){(*((struct
_tuple27*)_tmpC38->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp130B;struct
Cyc_Absyn_FieldName_struct _tmp130A;struct Cyc_List_List*_tmp1309;(*((struct
_tuple28*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1309=_cycalloc(sizeof(*
_tmp1309)),((_tmp1309->hd=(void*)((_tmp130B=_cycalloc(sizeof(*_tmp130B)),((
_tmp130B[0]=((_tmp130A.tag=1,((_tmp130A.f1=((*((struct _tuple27*)_tmpC38->hd)).f1)->name,
_tmp130A)))),_tmp130B)))),((_tmp1309->tl=0,_tmp1309))))));}{struct _tuple29*
_tmp130E;struct Cyc_List_List*_tmp130D;ans=((_tmp130D=_region_malloc(rgn,sizeof(*
_tmp130D)),((_tmp130D->hd=((_tmp130E=_region_malloc(rgn,sizeof(*_tmp130E)),((
_tmp130E->f1=(*((struct _tuple27*)_tmpC38->hd)).f1,((_tmp130E->f2=_tmpC37,
_tmp130E)))))),((_tmp130D->tl=ans,_tmp130D))))));}break;}}if(_tmpC38 == 0){const
char*_tmp1312;void*_tmp1311[1];struct Cyc_String_pa_struct _tmp1310;(_tmp1310.tag=
0,((_tmp1310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp1311[0]=& _tmp1310,Cyc_Tcutil_terr(loc,((_tmp1312="too many arguments to %s",
_tag_dyneither(_tmp1312,sizeof(char),25))),_tag_dyneither(_tmp1311,sizeof(void*),
1)))))));}}else{if(_tmpC36->tl != 0){const char*_tmp1315;void*_tmp1314;(_tmp1314=0,
Cyc_Tcutil_terr(loc,((_tmp1315="multiple designators are not yet supported",
_tag_dyneither(_tmp1315,sizeof(char),43))),_tag_dyneither(_tmp1314,sizeof(void*),
0)));}else{void*_tmpC43=(void*)_tmpC36->hd;struct _dyneither_ptr*_tmpC46;_LL5F9: {
struct Cyc_Absyn_ArrayElement_struct*_tmpC44=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpC43;if(_tmpC44->tag != 0)goto _LL5FB;}_LL5FA:{const char*_tmp1319;void*_tmp1318[
1];struct Cyc_String_pa_struct _tmp1317;(_tmp1317.tag=0,((_tmp1317.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1318[0]=& _tmp1317,Cyc_Tcutil_terr(
loc,((_tmp1319="array designator used in argument to %s",_tag_dyneither(_tmp1319,
sizeof(char),40))),_tag_dyneither(_tmp1318,sizeof(void*),1)))))));}goto _LL5F8;
_LL5FB: {struct Cyc_Absyn_FieldName_struct*_tmpC45=(struct Cyc_Absyn_FieldName_struct*)
_tmpC43;if(_tmpC45->tag != 1)goto _LL5F8;else{_tmpC46=_tmpC45->f1;}}_LL5FC: {
struct Cyc_List_List*_tmpC4A=fields;for(0;_tmpC4A != 0;_tmpC4A=_tmpC4A->tl){if(Cyc_strptrcmp(
_tmpC46,((*((struct _tuple27*)_tmpC4A->hd)).f1)->name)== 0){if((*((struct _tuple27*)
_tmpC4A->hd)).f2){const char*_tmp131D;void*_tmp131C[1];struct Cyc_String_pa_struct
_tmp131B;(_tmp131B.tag=0,((_tmp131B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpC46),((_tmp131C[0]=& _tmp131B,Cyc_Tcutil_terr(loc,((_tmp131D="member %s has already been used as an argument",
_tag_dyneither(_tmp131D,sizeof(char),47))),_tag_dyneither(_tmp131C,sizeof(void*),
1)))))));}(*((struct _tuple27*)_tmpC4A->hd)).f2=1;{struct _tuple29*_tmp1320;struct
Cyc_List_List*_tmp131F;ans=((_tmp131F=_region_malloc(rgn,sizeof(*_tmp131F)),((
_tmp131F->hd=((_tmp1320=_region_malloc(rgn,sizeof(*_tmp1320)),((_tmp1320->f1=(*((
struct _tuple27*)_tmpC4A->hd)).f1,((_tmp1320->f2=_tmpC37,_tmp1320)))))),((
_tmp131F->tl=ans,_tmp131F))))));}break;}}if(_tmpC4A == 0){const char*_tmp1324;void*
_tmp1323[1];struct Cyc_String_pa_struct _tmp1322;(_tmp1322.tag=0,((_tmp1322.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC46),((_tmp1323[0]=& _tmp1322,
Cyc_Tcutil_terr(loc,((_tmp1324="bad field designator %s",_tag_dyneither(_tmp1324,
sizeof(char),24))),_tag_dyneither(_tmp1323,sizeof(void*),1)))))));}goto _LL5F8;}
_LL5F8:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple27*)fields->hd)).f2){{const char*_tmp1327;void*_tmp1326;(
_tmp1326=0,Cyc_Tcutil_terr(loc,((_tmp1327="too few arguments to struct",
_tag_dyneither(_tmp1327,sizeof(char),28))),_tag_dyneither(_tmp1326,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple27*)fields->hd)).f2){if(found){const char*_tmp132A;void*_tmp1329;(_tmp1329=
0,Cyc_Tcutil_terr(loc,((_tmp132A="only one member of a union is allowed",
_tag_dyneither(_tmp132A,sizeof(char),38))),_tag_dyneither(_tmp1329,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp132D;void*_tmp132C;(_tmp132C=0,Cyc_Tcutil_terr(
loc,((_tmp132D="missing member for union",_tag_dyneither(_tmp132D,sizeof(char),
25))),_tag_dyneither(_tmp132C,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC5B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC5D;
void*_tmpC5E;struct Cyc_Absyn_PtrAtts _tmpC5F;union Cyc_Absyn_Constraint*_tmpC60;
_LL5FE: {struct Cyc_Absyn_PointerType_struct*_tmpC5C=(struct Cyc_Absyn_PointerType_struct*)
_tmpC5B;if(_tmpC5C->tag != 5)goto _LL600;else{_tmpC5D=_tmpC5C->f1;_tmpC5E=_tmpC5D.elt_typ;
_tmpC5F=_tmpC5D.ptr_atts;_tmpC60=_tmpC5F.bounds;}}_LL5FF: {void*_tmpC61=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpC60);_LL603: {struct Cyc_Absyn_DynEither_b_struct*_tmpC62=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC61;if(_tmpC62->tag != 0)goto _LL605;}_LL604:*elt_typ_dest=_tmpC5E;return 1;
_LL605:;_LL606: return 0;_LL602:;}_LL600:;_LL601: return 0;_LL5FD:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC63=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC65;
void*_tmpC66;struct Cyc_Absyn_PtrAtts _tmpC67;union Cyc_Absyn_Constraint*_tmpC68;
_LL608: {struct Cyc_Absyn_PointerType_struct*_tmpC64=(struct Cyc_Absyn_PointerType_struct*)
_tmpC63;if(_tmpC64->tag != 5)goto _LL60A;else{_tmpC65=_tmpC64->f1;_tmpC66=_tmpC65.elt_typ;
_tmpC67=_tmpC65.ptr_atts;_tmpC68=_tmpC67.zero_term;}}_LL609:*elt_typ_dest=
_tmpC66;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpC68);_LL60A:;_LL60B: return 0;_LL607:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpC69=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpC6B;void*_tmpC6C;struct Cyc_Absyn_PtrAtts _tmpC6D;
union Cyc_Absyn_Constraint*_tmpC6E;union Cyc_Absyn_Constraint*_tmpC6F;struct Cyc_Absyn_ArrayInfo
_tmpC71;void*_tmpC72;struct Cyc_Absyn_Tqual _tmpC73;struct Cyc_Absyn_Exp*_tmpC74;
union Cyc_Absyn_Constraint*_tmpC75;_LL60D: {struct Cyc_Absyn_PointerType_struct*
_tmpC6A=(struct Cyc_Absyn_PointerType_struct*)_tmpC69;if(_tmpC6A->tag != 5)goto
_LL60F;else{_tmpC6B=_tmpC6A->f1;_tmpC6C=_tmpC6B.elt_typ;_tmpC6D=_tmpC6B.ptr_atts;
_tmpC6E=_tmpC6D.bounds;_tmpC6F=_tmpC6D.zero_term;}}_LL60E: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6F)){*ptr_type=t;*elt_type=
_tmpC6C;{void*_tmpC76=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpC6E);_LL614: {struct Cyc_Absyn_DynEither_b_struct*
_tmpC77=(struct Cyc_Absyn_DynEither_b_struct*)_tmpC76;if(_tmpC77->tag != 0)goto
_LL616;}_LL615:*is_dyneither=1;goto _LL613;_LL616:;_LL617:*is_dyneither=0;goto
_LL613;_LL613:;}return 1;}else{return 0;}_LL60F: {struct Cyc_Absyn_ArrayType_struct*
_tmpC70=(struct Cyc_Absyn_ArrayType_struct*)_tmpC69;if(_tmpC70->tag != 9)goto
_LL611;else{_tmpC71=_tmpC70->f1;_tmpC72=_tmpC71.elt_type;_tmpC73=_tmpC71.tq;
_tmpC74=_tmpC71.num_elts;_tmpC75=_tmpC71.zero_term;}}_LL610: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC75)){*elt_type=_tmpC72;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1342;struct Cyc_Absyn_PtrAtts
_tmp1341;struct Cyc_Absyn_Upper_b_struct _tmp1340;struct Cyc_Absyn_Upper_b_struct*
_tmp133F;struct Cyc_Absyn_PtrInfo _tmp133E;struct Cyc_Absyn_PointerType_struct*
_tmp133D;*ptr_type=(void*)((_tmp133D=_cycalloc(sizeof(*_tmp133D)),((_tmp133D[0]=((
_tmp1342.tag=5,((_tmp1342.f1=((_tmp133E.elt_typ=_tmpC72,((_tmp133E.elt_tq=
_tmpC73,((_tmp133E.ptr_atts=((_tmp1341.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1341.nullable=Cyc_Absyn_false_conref,((_tmp1341.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((
_tmp133F[0]=((_tmp1340.tag=1,((_tmp1340.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpC74),_tmp1340)))),_tmp133F))))),((_tmp1341.zero_term=_tmpC75,((_tmp1341.ptrloc=
0,_tmp1341)))))))))),_tmp133E)))))),_tmp1342)))),_tmp133D))));}return 1;}else{
return 0;}_LL611:;_LL612: return 0;_LL60C:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpC7E=e1->r;struct Cyc_Absyn_Exp*_tmpC81;struct Cyc_Absyn_Exp*_tmpC83;struct Cyc_Absyn_Exp*
_tmpC85;struct Cyc_Absyn_Exp*_tmpC87;struct Cyc_Absyn_Exp*_tmpC89;struct Cyc_Absyn_Exp*
_tmpC8B;_LL619: {struct Cyc_Absyn_Cast_e_struct*_tmpC7F=(struct Cyc_Absyn_Cast_e_struct*)
_tmpC7E;if(_tmpC7F->tag != 15)goto _LL61B;}_LL61A: {const char*_tmp1346;void*
_tmp1345[1];struct Cyc_String_pa_struct _tmp1344;(_tmp1344.tag=0,((_tmp1344.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1345[0]=& _tmp1344,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1346="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1346,sizeof(char),29))),_tag_dyneither(_tmp1345,sizeof(void*),1)))))));}
_LL61B: {struct Cyc_Absyn_Deref_e_struct*_tmpC80=(struct Cyc_Absyn_Deref_e_struct*)
_tmpC7E;if(_tmpC80->tag != 21)goto _LL61D;else{_tmpC81=_tmpC80->f1;}}_LL61C:
_tmpC83=_tmpC81;goto _LL61E;_LL61D: {struct Cyc_Absyn_Subscript_e_struct*_tmpC82=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpC7E;if(_tmpC82->tag != 24)goto _LL61F;
else{_tmpC83=_tmpC82->f1;}}_LL61E: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC83->topt))->v,ptr_type,is_dyneither,
elt_type);_LL61F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpC84=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpC7E;if(_tmpC84->tag != 23)goto _LL621;else{_tmpC85=_tmpC84->f1;}}_LL620:
_tmpC87=_tmpC85;goto _LL622;_LL621: {struct Cyc_Absyn_AggrMember_e_struct*_tmpC86=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpC7E;if(_tmpC86->tag != 22)goto _LL623;
else{_tmpC87=_tmpC86->f1;}}_LL622: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpC87->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp134A;void*_tmp1349[1];struct Cyc_String_pa_struct _tmp1348;(_tmp1348.tag=
0,((_tmp1348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1349[0]=& _tmp1348,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134A="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp134A,sizeof(char),51))),_tag_dyneither(_tmp1349,sizeof(void*),
1)))))));}return 0;_LL623: {struct Cyc_Absyn_Instantiate_e_struct*_tmpC88=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpC7E;if(_tmpC88->tag != 14)goto _LL625;else{
_tmpC89=_tmpC88->f1;}}_LL624: _tmpC8B=_tmpC89;goto _LL626;_LL625: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC8A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpC7E;if(_tmpC8A->tag != 13)
goto _LL627;else{_tmpC8B=_tmpC8A->f1;}}_LL626: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC8B->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp134E;void*_tmp134D[1];struct Cyc_String_pa_struct
_tmp134C;(_tmp134C.tag=0,((_tmp134C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp134D[0]=& _tmp134C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134E="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp134E,sizeof(char),49))),_tag_dyneither(_tmp134D,sizeof(void*),
1)))))));}return 0;_LL627: {struct Cyc_Absyn_Var_e_struct*_tmpC8C=(struct Cyc_Absyn_Var_e_struct*)
_tmpC7E;if(_tmpC8C->tag != 1)goto _LL629;}_LL628: return 0;_LL629:;_LL62A: {const
char*_tmp1352;void*_tmp1351[1];struct Cyc_String_pa_struct _tmp1350;(_tmp1350.tag=
0,((_tmp1350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1351[0]=& _tmp1350,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1352="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1352,sizeof(char),39))),_tag_dyneither(_tmp1351,sizeof(void*),
1)))))));}_LL618:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpC99=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC9C;_LL62C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpC9A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpC99;if(_tmpC9A->tag != 22)goto _LL62E;}_LL62D: return 1;_LL62E: {struct Cyc_Absyn_VarType_struct*
_tmpC9B=(struct Cyc_Absyn_VarType_struct*)_tmpC99;if(_tmpC9B->tag != 2)goto _LL630;
else{_tmpC9C=_tmpC9B->f1;}}_LL62F: if(Cyc_Tcutil_tvar_kind(_tmpC9C,Cyc_Absyn_RgnKind)
== Cyc_Absyn_UniqueRgnKind  || Cyc_Tcutil_tvar_kind(_tmpC9C,Cyc_Absyn_RgnKind)== 
Cyc_Absyn_TopRgnKind){void*_tmpC9D=Cyc_Absyn_compress_kb(_tmpC9C->kind);struct
Cyc_Core_Opt*_tmpC9F;struct Cyc_Core_Opt**_tmpCA0;enum Cyc_Absyn_Kind _tmpCA1;
_LL633: {struct Cyc_Absyn_Less_kb_struct*_tmpC9E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9D;if(_tmpC9E->tag != 2)goto _LL635;else{_tmpC9F=_tmpC9E->f1;_tmpCA0=(struct
Cyc_Core_Opt**)& _tmpC9E->f1;_tmpCA1=_tmpC9E->f2;if(_tmpCA1 != Cyc_Absyn_TopRgnKind)
goto _LL635;}}_LL634:{struct Cyc_Absyn_Less_kb_struct*_tmp1358;struct Cyc_Absyn_Less_kb_struct
_tmp1357;struct Cyc_Core_Opt*_tmp1356;*_tmpCA0=((_tmp1356=_cycalloc(sizeof(*
_tmp1356)),((_tmp1356->v=(void*)((_tmp1358=_cycalloc(sizeof(*_tmp1358)),((
_tmp1358[0]=((_tmp1357.tag=2,((_tmp1357.f1=0,((_tmp1357.f2=Cyc_Absyn_RgnKind,
_tmp1357)))))),_tmp1358)))),_tmp1356))));}return 0;_LL635:;_LL636: return 1;_LL632:;}
return 0;_LL630:;_LL631: return 0;_LL62B:;}int Cyc_Tcutil_is_noalias_pointer(void*t);
int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpCA5=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpCA7;struct Cyc_Absyn_PtrAtts _tmpCA8;void*_tmpCA9;
_LL638: {struct Cyc_Absyn_PointerType_struct*_tmpCA6=(struct Cyc_Absyn_PointerType_struct*)
_tmpCA5;if(_tmpCA6->tag != 5)goto _LL63A;else{_tmpCA7=_tmpCA6->f1;_tmpCA8=_tmpCA7.ptr_atts;
_tmpCA9=_tmpCA8.rgn;}}_LL639: return Cyc_Tcutil_is_noalias_region(_tmpCA9);_LL63A:;
_LL63B: return 0;_LL637:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpCAA=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpCAA))return 1;{void*_tmpCAB=_tmpCAA;struct Cyc_List_List*_tmpCAD;struct Cyc_Absyn_AggrInfo
_tmpCAF;union Cyc_Absyn_AggrInfoU _tmpCB0;struct Cyc_Absyn_Aggrdecl**_tmpCB1;struct
Cyc_List_List*_tmpCB2;struct Cyc_List_List*_tmpCB4;struct Cyc_Absyn_AggrInfo
_tmpCB6;union Cyc_Absyn_AggrInfoU _tmpCB7;struct _tuple4 _tmpCB8;struct Cyc_Absyn_DatatypeInfo
_tmpCBA;union Cyc_Absyn_DatatypeInfoU _tmpCBB;struct Cyc_List_List*_tmpCBC;struct
Cyc_Absyn_DatatypeFieldInfo _tmpCBE;union Cyc_Absyn_DatatypeFieldInfoU _tmpCBF;
struct Cyc_List_List*_tmpCC0;_LL63D: {struct Cyc_Absyn_TupleType_struct*_tmpCAC=(
struct Cyc_Absyn_TupleType_struct*)_tmpCAB;if(_tmpCAC->tag != 11)goto _LL63F;else{
_tmpCAD=_tmpCAC->f1;}}_LL63E: while(_tmpCAD != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpCAD->hd)).f2))return 1;_tmpCAD=_tmpCAD->tl;}return 0;
_LL63F: {struct Cyc_Absyn_AggrType_struct*_tmpCAE=(struct Cyc_Absyn_AggrType_struct*)
_tmpCAB;if(_tmpCAE->tag != 12)goto _LL641;else{_tmpCAF=_tmpCAE->f1;_tmpCB0=_tmpCAF.aggr_info;
if((_tmpCB0.KnownAggr).tag != 2)goto _LL641;_tmpCB1=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCB0.KnownAggr).val;_tmpCB2=_tmpCAF.targs;}}_LL640: if((*_tmpCB1)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpCC1=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpCB1)->tvs,_tmpCB2);struct Cyc_List_List*_tmpCC2=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpCB1)->impl))->fields;void*t;
while(_tmpCC2 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpCC1,((struct Cyc_Absyn_Aggrfield*)
_tmpCC2->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpCC2=_tmpCC2->tl;}return 0;}_LL641: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpCB3=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpCAB;if(_tmpCB3->tag != 13)goto
_LL643;else{_tmpCB4=_tmpCB3->f2;}}_LL642: while(_tmpCB4 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpCB4->hd)->type))return 1;_tmpCB4=_tmpCB4->tl;}
return 0;_LL643: {struct Cyc_Absyn_AggrType_struct*_tmpCB5=(struct Cyc_Absyn_AggrType_struct*)
_tmpCAB;if(_tmpCB5->tag != 12)goto _LL645;else{_tmpCB6=_tmpCB5->f1;_tmpCB7=_tmpCB6.aggr_info;
if((_tmpCB7.UnknownAggr).tag != 1)goto _LL645;_tmpCB8=(struct _tuple4)(_tmpCB7.UnknownAggr).val;}}
_LL644: {const char*_tmp135B;void*_tmp135A;(_tmp135A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135B="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp135B,sizeof(char),36))),_tag_dyneither(_tmp135A,sizeof(void*),
0)));}_LL645: {struct Cyc_Absyn_DatatypeType_struct*_tmpCB9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCAB;if(_tmpCB9->tag != 3)goto _LL647;else{_tmpCBA=_tmpCB9->f1;_tmpCBB=_tmpCBA.datatype_info;
_tmpCBC=_tmpCBA.targs;}}_LL646: {union Cyc_Absyn_DatatypeInfoU _tmpCC5=_tmpCBB;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpCC6;struct _tuple2*_tmpCC7;int _tmpCC8;
struct Cyc_Absyn_Datatypedecl**_tmpCC9;struct Cyc_Absyn_Datatypedecl*_tmpCCA;
struct Cyc_Absyn_Datatypedecl _tmpCCB;struct Cyc_List_List*_tmpCCC;struct Cyc_Core_Opt*
_tmpCCD;_LL64C: if((_tmpCC5.UnknownDatatype).tag != 1)goto _LL64E;_tmpCC6=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpCC5.UnknownDatatype).val;_tmpCC7=_tmpCC6.name;
_tmpCC8=_tmpCC6.is_extensible;_LL64D: {const char*_tmp135E;void*_tmp135D;(
_tmp135D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp135E="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp135E,
sizeof(char),40))),_tag_dyneither(_tmp135D,sizeof(void*),0)));}_LL64E: if((
_tmpCC5.KnownDatatype).tag != 2)goto _LL64B;_tmpCC9=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCC5.KnownDatatype).val;_tmpCCA=*_tmpCC9;_tmpCCB=*_tmpCCA;_tmpCCC=_tmpCCB.tvs;
_tmpCCD=_tmpCCB.fields;_LL64F: return 0;_LL64B:;}_LL647: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpCBD=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpCAB;if(_tmpCBD->tag != 4)
goto _LL649;else{_tmpCBE=_tmpCBD->f1;_tmpCBF=_tmpCBE.field_info;_tmpCC0=_tmpCBE.targs;}}
_LL648: {union Cyc_Absyn_DatatypeFieldInfoU _tmpCD0=_tmpCBF;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpCD1;struct _tuple3 _tmpCD2;struct Cyc_Absyn_Datatypedecl*_tmpCD3;struct Cyc_Absyn_Datatypefield*
_tmpCD4;_LL651: if((_tmpCD0.UnknownDatatypefield).tag != 1)goto _LL653;_tmpCD1=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCD0.UnknownDatatypefield).val;
_LL652: {const char*_tmp1361;void*_tmp1360;(_tmp1360=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1361="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1361,sizeof(char),46))),_tag_dyneither(_tmp1360,sizeof(void*),
0)));}_LL653: if((_tmpCD0.KnownDatatypefield).tag != 2)goto _LL650;_tmpCD2=(struct
_tuple3)(_tmpCD0.KnownDatatypefield).val;_tmpCD3=_tmpCD2.f1;_tmpCD4=_tmpCD2.f2;
_LL654: {struct Cyc_List_List*_tmpCD7=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpCD3->tvs,_tmpCC0);struct Cyc_List_List*_tmpCD8=
_tmpCD4->typs;while(_tmpCD8 != 0){_tmpCAA=Cyc_Tcutil_rsubstitute(rgn,_tmpCD7,(*((
struct _tuple11*)_tmpCD8->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpCAA))return 1;_tmpCD8=_tmpCD8->tl;}return 0;}_LL650:;}_LL649:;_LL64A: return 0;
_LL63C:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpCD9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpCDB;union Cyc_Absyn_AggrInfoU _tmpCDC;struct Cyc_Absyn_Aggrdecl**_tmpCDD;struct
Cyc_Absyn_Aggrdecl*_tmpCDE;struct Cyc_List_List*_tmpCDF;struct Cyc_List_List*
_tmpCE1;_LL656: {struct Cyc_Absyn_AggrType_struct*_tmpCDA=(struct Cyc_Absyn_AggrType_struct*)
_tmpCD9;if(_tmpCDA->tag != 12)goto _LL658;else{_tmpCDB=_tmpCDA->f1;_tmpCDC=_tmpCDB.aggr_info;
if((_tmpCDC.KnownAggr).tag != 2)goto _LL658;_tmpCDD=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCDC.KnownAggr).val;_tmpCDE=*_tmpCDD;_tmpCDF=_tmpCDB.targs;}}_LL657: _tmpCE1=
_tmpCDE->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDE->impl))->fields;
goto _LL659;_LL658: {struct Cyc_Absyn_AnonAggrType_struct*_tmpCE0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpCD9;if(_tmpCE0->tag != 13)goto _LL65A;else{_tmpCE1=_tmpCE0->f2;}}_LL659: {
struct Cyc_Absyn_Aggrfield*_tmpCE2=Cyc_Absyn_lookup_field(_tmpCE1,f);{struct Cyc_Absyn_Aggrfield*
_tmpCE3=_tmpCE2;struct Cyc_Absyn_Aggrfield _tmpCE4;void*_tmpCE5;_LL65D: if(_tmpCE3
!= 0)goto _LL65F;_LL65E: {const char*_tmp1364;void*_tmp1363;(_tmp1363=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1364="is_noalias_field: missing field",
_tag_dyneither(_tmp1364,sizeof(char),32))),_tag_dyneither(_tmp1363,sizeof(void*),
0)));}_LL65F: if(_tmpCE3 == 0)goto _LL65C;_tmpCE4=*_tmpCE3;_tmpCE5=_tmpCE4.type;
_LL660: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpCE5);_LL65C:;}return 0;}
_LL65A:;_LL65B: {const char*_tmp1368;void*_tmp1367[1];struct Cyc_String_pa_struct
_tmp1366;(_tmp1366.tag=0,((_tmp1366.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1367[0]=& _tmp1366,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1368="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1368,sizeof(char),36))),_tag_dyneither(_tmp1367,sizeof(void*),
1)))))));}_LL655:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpCEB=e->r;
void*_tmpCFB;struct Cyc_Absyn_Exp*_tmpD01;struct _dyneither_ptr*_tmpD02;struct Cyc_Absyn_Exp*
_tmpD04;struct Cyc_Absyn_Exp*_tmpD05;void*_tmpD0E;void*_tmpD11;void*_tmpD14;
struct Cyc_Absyn_Exp*_tmpD17;struct Cyc_Absyn_Exp*_tmpD19;struct Cyc_Absyn_Exp*
_tmpD1B;struct Cyc_Absyn_Exp*_tmpD1D;void*_tmpD1F;struct Cyc_Absyn_Exp*_tmpD20;
struct Cyc_Absyn_Stmt*_tmpD22;_LL662: {struct Cyc_Absyn_Increment_e_struct*_tmpCEC=(
struct Cyc_Absyn_Increment_e_struct*)_tmpCEB;if(_tmpCEC->tag != 5)goto _LL664;}
_LL663: goto _LL665;_LL664: {struct Cyc_Absyn_And_e_struct*_tmpCED=(struct Cyc_Absyn_And_e_struct*)
_tmpCEB;if(_tmpCED->tag != 7)goto _LL666;}_LL665: goto _LL667;_LL666: {struct Cyc_Absyn_Or_e_struct*
_tmpCEE=(struct Cyc_Absyn_Or_e_struct*)_tmpCEB;if(_tmpCEE->tag != 8)goto _LL668;}
_LL667: goto _LL669;_LL668: {struct Cyc_Absyn_Throw_e_struct*_tmpCEF=(struct Cyc_Absyn_Throw_e_struct*)
_tmpCEB;if(_tmpCEF->tag != 12)goto _LL66A;}_LL669: goto _LL66B;_LL66A: {struct Cyc_Absyn_Address_e_struct*
_tmpCF0=(struct Cyc_Absyn_Address_e_struct*)_tmpCEB;if(_tmpCF0->tag != 16)goto
_LL66C;}_LL66B: goto _LL66D;_LL66C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCF1=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpCEB;if(_tmpCF1->tag != 18)goto _LL66E;}
_LL66D: goto _LL66F;_LL66E: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpCF2=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpCEB;if(_tmpCF2->tag != 19)goto _LL670;}_LL66F: goto _LL671;_LL670: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpCF3=(struct Cyc_Absyn_Offsetof_e_struct*)_tmpCEB;if(_tmpCF3->tag != 20)goto
_LL672;}_LL671: goto _LL673;_LL672: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpCF4=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpCEB;if(_tmpCF4->tag != 26)goto _LL674;}
_LL673: goto _LL675;_LL674: {struct Cyc_Absyn_Comprehension_e_struct*_tmpCF5=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmpCEB;if(_tmpCF5->tag != 28)goto _LL676;}
_LL675: goto _LL677;_LL676: {struct Cyc_Absyn_Array_e_struct*_tmpCF6=(struct Cyc_Absyn_Array_e_struct*)
_tmpCEB;if(_tmpCF6->tag != 27)goto _LL678;}_LL677: goto _LL679;_LL678: {struct Cyc_Absyn_Enum_e_struct*
_tmpCF7=(struct Cyc_Absyn_Enum_e_struct*)_tmpCEB;if(_tmpCF7->tag != 32)goto _LL67A;}
_LL679: goto _LL67B;_LL67A: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpCF8=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpCEB;if(_tmpCF8->tag != 33)goto _LL67C;}_LL67B: goto _LL67D;_LL67C: {struct Cyc_Absyn_Swap_e_struct*
_tmpCF9=(struct Cyc_Absyn_Swap_e_struct*)_tmpCEB;if(_tmpCF9->tag != 35)goto _LL67E;}
_LL67D: goto _LL67F;_LL67E: {struct Cyc_Absyn_Var_e_struct*_tmpCFA=(struct Cyc_Absyn_Var_e_struct*)
_tmpCEB;if(_tmpCFA->tag != 1)goto _LL680;else{_tmpCFB=(void*)_tmpCFA->f2;{struct
Cyc_Absyn_Global_b_struct*_tmpCFC=(struct Cyc_Absyn_Global_b_struct*)_tmpCFB;if(
_tmpCFC->tag != 1)goto _LL680;};}}_LL67F: goto _LL681;_LL680: {struct Cyc_Absyn_Primop_e_struct*
_tmpCFD=(struct Cyc_Absyn_Primop_e_struct*)_tmpCEB;if(_tmpCFD->tag != 3)goto _LL682;}
_LL681: return 0;_LL682: {struct Cyc_Absyn_Deref_e_struct*_tmpCFE=(struct Cyc_Absyn_Deref_e_struct*)
_tmpCEB;if(_tmpCFE->tag != 21)goto _LL684;}_LL683: goto _LL685;_LL684: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmpCFF=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmpCEB;if(_tmpCFF->tag != 23)goto
_LL686;}_LL685: return 0;_LL686: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD00=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpCEB;if(_tmpD00->tag != 22)goto _LL688;
else{_tmpD01=_tmpD00->f1;_tmpD02=_tmpD00->f2;}}_LL687: return(ignore_leaf  || Cyc_Tcutil_is_noalias_field(
r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD01->topt))->v,_tmpD02)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD01,0);_LL688: {struct Cyc_Absyn_Subscript_e_struct*_tmpD03=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpCEB;if(_tmpD03->tag != 24)goto _LL68A;else{_tmpD04=_tmpD03->f1;_tmpD05=_tmpD03->f2;}}
_LL689: {void*_tmpD23=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD04->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD25;void*_tmpD26;
struct Cyc_List_List*_tmpD28;_LL6AD: {struct Cyc_Absyn_PointerType_struct*_tmpD24=(
struct Cyc_Absyn_PointerType_struct*)_tmpD23;if(_tmpD24->tag != 5)goto _LL6AF;else{
_tmpD25=_tmpD24->f1;_tmpD26=_tmpD25.elt_typ;}}_LL6AE: return 0;_LL6AF: {struct Cyc_Absyn_TupleType_struct*
_tmpD27=(struct Cyc_Absyn_TupleType_struct*)_tmpD23;if(_tmpD27->tag != 11)goto
_LL6B1;else{_tmpD28=_tmpD27->f1;}}_LL6B0: {unsigned int _tmpD2A;int _tmpD2B;struct
_tuple13 _tmpD29=Cyc_Evexp_eval_const_uint_exp(_tmpD05);_tmpD2A=_tmpD29.f1;
_tmpD2B=_tmpD29.f2;if(!_tmpD2B){const char*_tmp136B;void*_tmp136A;(_tmp136A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp136B="is_noalias_path_aux: non-constant subscript",_tag_dyneither(_tmp136B,
sizeof(char),44))),_tag_dyneither(_tmp136A,sizeof(void*),0)));}{struct
_handler_cons _tmpD2E;_push_handler(& _tmpD2E);{int _tmpD30=0;if(setjmp(_tmpD2E.handler))
_tmpD30=1;if(!_tmpD30){{void*_tmpD31=(*((struct _tuple11*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmpD28,(int)_tmpD2A)).f2;int _tmpD32=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpD31)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpD04,0);_npop_handler(0);
return _tmpD32;};_pop_handler();}else{void*_tmpD2F=(void*)_exn_thrown;void*
_tmpD34=_tmpD2F;_LL6B4: {struct Cyc_List_Nth_struct*_tmpD35=(struct Cyc_List_Nth_struct*)
_tmpD34;if(_tmpD35->tag != Cyc_List_Nth)goto _LL6B6;}_LL6B5: {const char*_tmp136E;
void*_tmp136D;return(_tmp136D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136E="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp136E,sizeof(char),45))),_tag_dyneither(_tmp136D,sizeof(void*),
0)));}_LL6B6:;_LL6B7:(void)_throw(_tmpD34);_LL6B3:;}};};}_LL6B1:;_LL6B2: {const
char*_tmp1371;void*_tmp1370;(_tmp1370=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1371="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp1371,sizeof(char),52))),_tag_dyneither(_tmp1370,sizeof(void*),
0)));}_LL6AC:;}_LL68A: {struct Cyc_Absyn_Datatype_e_struct*_tmpD06=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpCEB;if(_tmpD06->tag != 31)goto _LL68C;}_LL68B: goto _LL68D;_LL68C: {struct Cyc_Absyn_Tuple_e_struct*
_tmpD07=(struct Cyc_Absyn_Tuple_e_struct*)_tmpCEB;if(_tmpD07->tag != 25)goto _LL68E;}
_LL68D: goto _LL68F;_LL68E: {struct Cyc_Absyn_Aggregate_e_struct*_tmpD08=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpCEB;if(_tmpD08->tag != 29)goto _LL690;}_LL68F: goto _LL691;_LL690: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpD09=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpCEB;if(_tmpD09->tag != 30)goto
_LL692;}_LL691: goto _LL693;_LL692: {struct Cyc_Absyn_Const_e_struct*_tmpD0A=(
struct Cyc_Absyn_Const_e_struct*)_tmpCEB;if(_tmpD0A->tag != 0)goto _LL694;}_LL693:
goto _LL695;_LL694: {struct Cyc_Absyn_Malloc_e_struct*_tmpD0B=(struct Cyc_Absyn_Malloc_e_struct*)
_tmpCEB;if(_tmpD0B->tag != 34)goto _LL696;}_LL695: goto _LL697;_LL696: {struct Cyc_Absyn_New_e_struct*
_tmpD0C=(struct Cyc_Absyn_New_e_struct*)_tmpCEB;if(_tmpD0C->tag != 17)goto _LL698;}
_LL697: goto _LL699;_LL698: {struct Cyc_Absyn_Var_e_struct*_tmpD0D=(struct Cyc_Absyn_Var_e_struct*)
_tmpCEB;if(_tmpD0D->tag != 1)goto _LL69A;else{_tmpD0E=(void*)_tmpD0D->f2;{struct
Cyc_Absyn_Local_b_struct*_tmpD0F=(struct Cyc_Absyn_Local_b_struct*)_tmpD0E;if(
_tmpD0F->tag != 4)goto _LL69A;};}}_LL699: goto _LL69B;_LL69A: {struct Cyc_Absyn_Var_e_struct*
_tmpD10=(struct Cyc_Absyn_Var_e_struct*)_tmpCEB;if(_tmpD10->tag != 1)goto _LL69C;
else{_tmpD11=(void*)_tmpD10->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD12=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD11;if(_tmpD12->tag != 5)goto _LL69C;};}}_LL69B: goto _LL69D;_LL69C: {struct Cyc_Absyn_Var_e_struct*
_tmpD13=(struct Cyc_Absyn_Var_e_struct*)_tmpCEB;if(_tmpD13->tag != 1)goto _LL69E;
else{_tmpD14=(void*)_tmpD13->f2;{struct Cyc_Absyn_Param_b_struct*_tmpD15=(struct
Cyc_Absyn_Param_b_struct*)_tmpD14;if(_tmpD15->tag != 3)goto _LL69E;};}}_LL69D: {
int _tmpD3A=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpD3A;}_LL69E: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD16=(struct Cyc_Absyn_Conditional_e_struct*)_tmpCEB;if(_tmpD16->tag != 6)goto
_LL6A0;else{_tmpD17=_tmpD16->f2;}}_LL69F: _tmpD19=_tmpD17;goto _LL6A1;_LL6A0: {
struct Cyc_Absyn_SeqExp_e_struct*_tmpD18=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpCEB;if(_tmpD18->tag != 9)goto _LL6A2;else{_tmpD19=_tmpD18->f2;}}_LL6A1: _tmpD1B=
_tmpD19;goto _LL6A3;_LL6A2: {struct Cyc_Absyn_AssignOp_e_struct*_tmpD1A=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpCEB;if(_tmpD1A->tag != 4)goto _LL6A4;else{_tmpD1B=_tmpD1A->f1;}}_LL6A3: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD1B,ignore_leaf);_LL6A4: {struct Cyc_Absyn_FnCall_e_struct*
_tmpD1C=(struct Cyc_Absyn_FnCall_e_struct*)_tmpCEB;if(_tmpD1C->tag != 11)goto
_LL6A6;else{_tmpD1D=_tmpD1C->f1;}}_LL6A5: {void*_tmpD3B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD1D->topt))->v);struct Cyc_Absyn_FnInfo
_tmpD3D;void*_tmpD3E;struct Cyc_Absyn_PtrInfo _tmpD40;void*_tmpD41;_LL6B9: {struct
Cyc_Absyn_FnType_struct*_tmpD3C=(struct Cyc_Absyn_FnType_struct*)_tmpD3B;if(
_tmpD3C->tag != 10)goto _LL6BB;else{_tmpD3D=_tmpD3C->f1;_tmpD3E=_tmpD3D.ret_typ;}}
_LL6BA: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD3E);
_LL6BB: {struct Cyc_Absyn_PointerType_struct*_tmpD3F=(struct Cyc_Absyn_PointerType_struct*)
_tmpD3B;if(_tmpD3F->tag != 5)goto _LL6BD;else{_tmpD40=_tmpD3F->f1;_tmpD41=_tmpD40.elt_typ;}}
_LL6BC:{void*_tmpD42=Cyc_Tcutil_compress(_tmpD41);struct Cyc_Absyn_FnInfo _tmpD44;
void*_tmpD45;_LL6C0: {struct Cyc_Absyn_FnType_struct*_tmpD43=(struct Cyc_Absyn_FnType_struct*)
_tmpD42;if(_tmpD43->tag != 10)goto _LL6C2;else{_tmpD44=_tmpD43->f1;_tmpD45=_tmpD44.ret_typ;}}
_LL6C1: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD45);
_LL6C2:;_LL6C3: goto _LL6BF;_LL6BF:;}goto _LL6BE;_LL6BD:;_LL6BE: {const char*
_tmp1375;void*_tmp1374[1];struct Cyc_String_pa_struct _tmp1373;(_tmp1373.tag=0,((
_tmp1373.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD1D->topt))->v)),((_tmp1374[0]=&
_tmp1373,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1375="Fncall has non function type %s",_tag_dyneither(_tmp1375,sizeof(char),
32))),_tag_dyneither(_tmp1374,sizeof(void*),1)))))));}_LL6B8:;}_LL6A6: {struct
Cyc_Absyn_Cast_e_struct*_tmpD1E=(struct Cyc_Absyn_Cast_e_struct*)_tmpCEB;if(
_tmpD1E->tag != 15)goto _LL6A8;else{_tmpD1F=(void*)_tmpD1E->f1;_tmpD20=_tmpD1E->f2;}}
_LL6A7: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD1F) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD20,1);_LL6A8: {struct Cyc_Absyn_StmtExp_e_struct*_tmpD21=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpCEB;if(_tmpD21->tag != 37)goto _LL6AA;else{_tmpD22=_tmpD21->f1;}}_LL6A9: while(
1){void*_tmpD49=_tmpD22->r;struct Cyc_Absyn_Stmt*_tmpD4B;struct Cyc_Absyn_Stmt*
_tmpD4C;struct Cyc_Absyn_Decl*_tmpD4E;struct Cyc_Absyn_Stmt*_tmpD4F;struct Cyc_Absyn_Exp*
_tmpD51;_LL6C5: {struct Cyc_Absyn_Seq_s_struct*_tmpD4A=(struct Cyc_Absyn_Seq_s_struct*)
_tmpD49;if(_tmpD4A->tag != 2)goto _LL6C7;else{_tmpD4B=_tmpD4A->f1;_tmpD4C=_tmpD4A->f2;}}
_LL6C6: _tmpD22=_tmpD4C;goto _LL6C4;_LL6C7: {struct Cyc_Absyn_Decl_s_struct*_tmpD4D=(
struct Cyc_Absyn_Decl_s_struct*)_tmpD49;if(_tmpD4D->tag != 12)goto _LL6C9;else{
_tmpD4E=_tmpD4D->f1;_tmpD4F=_tmpD4D->f2;}}_LL6C8: _tmpD22=_tmpD4F;goto _LL6C4;
_LL6C9: {struct Cyc_Absyn_Exp_s_struct*_tmpD50=(struct Cyc_Absyn_Exp_s_struct*)
_tmpD49;if(_tmpD50->tag != 1)goto _LL6CB;else{_tmpD51=_tmpD50->f1;}}_LL6CA: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD51,ignore_leaf);_LL6CB:;_LL6CC: {const char*
_tmp1378;void*_tmp1377;(_tmp1377=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1378="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1378,sizeof(char),40))),_tag_dyneither(_tmp1377,sizeof(void*),
0)));}_LL6C4:;}_LL6AA:;_LL6AB: return 0;_LL661:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple16 _tmp1379;struct _tuple16 bogus_ans=(
_tmp1379.f1=0,((_tmp1379.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1379)));void*
_tmpD54=e->r;struct _tuple2*_tmpD56;void*_tmpD57;struct Cyc_Absyn_Exp*_tmpD59;
struct _dyneither_ptr*_tmpD5A;int _tmpD5B;struct Cyc_Absyn_Exp*_tmpD5D;struct
_dyneither_ptr*_tmpD5E;int _tmpD5F;struct Cyc_Absyn_Exp*_tmpD61;struct Cyc_Absyn_Exp*
_tmpD63;struct Cyc_Absyn_Exp*_tmpD64;_LL6CE: {struct Cyc_Absyn_Var_e_struct*
_tmpD55=(struct Cyc_Absyn_Var_e_struct*)_tmpD54;if(_tmpD55->tag != 1)goto _LL6D0;
else{_tmpD56=_tmpD55->f1;_tmpD57=(void*)_tmpD55->f2;}}_LL6CF: {void*_tmpD65=
_tmpD57;struct Cyc_Absyn_Vardecl*_tmpD69;struct Cyc_Absyn_Vardecl*_tmpD6B;struct
Cyc_Absyn_Vardecl*_tmpD6D;struct Cyc_Absyn_Vardecl*_tmpD6F;_LL6DB: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpD66=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpD65;if(_tmpD66->tag != 0)goto
_LL6DD;}_LL6DC: goto _LL6DE;_LL6DD: {struct Cyc_Absyn_Funname_b_struct*_tmpD67=(
struct Cyc_Absyn_Funname_b_struct*)_tmpD65;if(_tmpD67->tag != 2)goto _LL6DF;}_LL6DE:
return bogus_ans;_LL6DF: {struct Cyc_Absyn_Global_b_struct*_tmpD68=(struct Cyc_Absyn_Global_b_struct*)
_tmpD65;if(_tmpD68->tag != 1)goto _LL6E1;else{_tmpD69=_tmpD68->f1;}}_LL6E0: {void*
_tmpD70=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL6E8: {struct Cyc_Absyn_ArrayType_struct*_tmpD71=(struct Cyc_Absyn_ArrayType_struct*)
_tmpD70;if(_tmpD71->tag != 9)goto _LL6EA;}_LL6E9: {struct _tuple16 _tmp137A;return(
_tmp137A.f1=1,((_tmp137A.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp137A)));}_LL6EA:;
_LL6EB: {struct _tuple16 _tmp137B;return(_tmp137B.f1=(_tmpD69->tq).real_const,((
_tmp137B.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp137B)));}_LL6E7:;}_LL6E1: {struct
Cyc_Absyn_Local_b_struct*_tmpD6A=(struct Cyc_Absyn_Local_b_struct*)_tmpD65;if(
_tmpD6A->tag != 4)goto _LL6E3;else{_tmpD6B=_tmpD6A->f1;}}_LL6E2: {void*_tmpD74=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6ED: {struct Cyc_Absyn_ArrayType_struct*
_tmpD75=(struct Cyc_Absyn_ArrayType_struct*)_tmpD74;if(_tmpD75->tag != 9)goto
_LL6EF;}_LL6EE: {struct _tuple16 _tmp137C;return(_tmp137C.f1=1,((_tmp137C.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpD6B->rgn))->v,_tmp137C)));}_LL6EF:;_LL6F0:
_tmpD6B->escapes=1;{struct _tuple16 _tmp137D;return(_tmp137D.f1=(_tmpD6B->tq).real_const,((
_tmp137D.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD6B->rgn))->v,_tmp137D)));};
_LL6EC:;}_LL6E3: {struct Cyc_Absyn_Pat_b_struct*_tmpD6C=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD65;if(_tmpD6C->tag != 5)goto _LL6E5;else{_tmpD6D=_tmpD6C->f1;}}_LL6E4: _tmpD6F=
_tmpD6D;goto _LL6E6;_LL6E5: {struct Cyc_Absyn_Param_b_struct*_tmpD6E=(struct Cyc_Absyn_Param_b_struct*)
_tmpD65;if(_tmpD6E->tag != 3)goto _LL6DA;else{_tmpD6F=_tmpD6E->f1;}}_LL6E6: _tmpD6F->escapes=
1;{struct _tuple16 _tmp137E;return(_tmp137E.f1=(_tmpD6F->tq).real_const,((_tmp137E.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD6F->rgn))->v,_tmp137E)));};_LL6DA:;}
_LL6D0: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD58=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpD54;if(_tmpD58->tag != 22)goto _LL6D2;else{_tmpD59=_tmpD58->f1;_tmpD5A=_tmpD58->f2;
_tmpD5B=_tmpD58->f3;}}_LL6D1: if(_tmpD5B)return bogus_ans;{void*_tmpD79=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD59->topt))->v);struct Cyc_List_List*
_tmpD7B;struct Cyc_Absyn_AggrInfo _tmpD7D;union Cyc_Absyn_AggrInfoU _tmpD7E;struct
Cyc_Absyn_Aggrdecl**_tmpD7F;struct Cyc_Absyn_Aggrdecl*_tmpD80;_LL6F2: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD7A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD79;if(_tmpD7A->tag != 13)goto
_LL6F4;else{_tmpD7B=_tmpD7A->f2;}}_LL6F3: {struct Cyc_Absyn_Aggrfield*_tmpD81=Cyc_Absyn_lookup_field(
_tmpD7B,_tmpD5A);if(_tmpD81 != 0  && _tmpD81->width != 0){struct _tuple16 _tmp137F;
return(_tmp137F.f1=(_tmpD81->tq).real_const,((_tmp137F.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD59)).f2,_tmp137F)));}return bogus_ans;}_LL6F4: {struct Cyc_Absyn_AggrType_struct*
_tmpD7C=(struct Cyc_Absyn_AggrType_struct*)_tmpD79;if(_tmpD7C->tag != 12)goto
_LL6F6;else{_tmpD7D=_tmpD7C->f1;_tmpD7E=_tmpD7D.aggr_info;if((_tmpD7E.KnownAggr).tag
!= 2)goto _LL6F6;_tmpD7F=(struct Cyc_Absyn_Aggrdecl**)(_tmpD7E.KnownAggr).val;
_tmpD80=*_tmpD7F;}}_LL6F5: {struct Cyc_Absyn_Aggrfield*_tmpD83=Cyc_Absyn_lookup_decl_field(
_tmpD80,_tmpD5A);if(_tmpD83 != 0  && _tmpD83->width != 0){struct _tuple16 _tmp1380;
return(_tmp1380.f1=(_tmpD83->tq).real_const,((_tmp1380.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD59)).f2,_tmp1380)));}return bogus_ans;}_LL6F6:;_LL6F7: return bogus_ans;
_LL6F1:;};_LL6D2: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpD5C=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD54;if(_tmpD5C->tag != 23)goto _LL6D4;else{_tmpD5D=_tmpD5C->f1;_tmpD5E=_tmpD5C->f2;
_tmpD5F=_tmpD5C->f3;}}_LL6D3: if(_tmpD5F)return bogus_ans;{void*_tmpD85=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD5D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpD87;void*_tmpD88;struct Cyc_Absyn_PtrAtts _tmpD89;void*_tmpD8A;_LL6F9: {struct
Cyc_Absyn_PointerType_struct*_tmpD86=(struct Cyc_Absyn_PointerType_struct*)
_tmpD85;if(_tmpD86->tag != 5)goto _LL6FB;else{_tmpD87=_tmpD86->f1;_tmpD88=_tmpD87.elt_typ;
_tmpD89=_tmpD87.ptr_atts;_tmpD8A=_tmpD89.rgn;}}_LL6FA: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpD8B=Cyc_Tcutil_compress(_tmpD88);struct Cyc_List_List*_tmpD8D;
struct Cyc_Absyn_AggrInfo _tmpD8F;union Cyc_Absyn_AggrInfoU _tmpD90;struct Cyc_Absyn_Aggrdecl**
_tmpD91;struct Cyc_Absyn_Aggrdecl*_tmpD92;_LL6FE: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD8C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD8B;if(_tmpD8C->tag != 13)goto
_LL700;else{_tmpD8D=_tmpD8C->f2;}}_LL6FF: finfo=Cyc_Absyn_lookup_field(_tmpD8D,
_tmpD5E);goto _LL6FD;_LL700: {struct Cyc_Absyn_AggrType_struct*_tmpD8E=(struct Cyc_Absyn_AggrType_struct*)
_tmpD8B;if(_tmpD8E->tag != 12)goto _LL702;else{_tmpD8F=_tmpD8E->f1;_tmpD90=_tmpD8F.aggr_info;
if((_tmpD90.KnownAggr).tag != 2)goto _LL702;_tmpD91=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD90.KnownAggr).val;_tmpD92=*_tmpD91;}}_LL701: finfo=Cyc_Absyn_lookup_decl_field(
_tmpD92,_tmpD5E);goto _LL6FD;_LL702:;_LL703: return bogus_ans;_LL6FD:;}if(finfo != 0
 && finfo->width != 0){struct _tuple16 _tmp1381;return(_tmp1381.f1=(finfo->tq).real_const,((
_tmp1381.f2=_tmpD8A,_tmp1381)));}return bogus_ans;}_LL6FB:;_LL6FC: return bogus_ans;
_LL6F8:;};_LL6D4: {struct Cyc_Absyn_Deref_e_struct*_tmpD60=(struct Cyc_Absyn_Deref_e_struct*)
_tmpD54;if(_tmpD60->tag != 21)goto _LL6D6;else{_tmpD61=_tmpD60->f1;}}_LL6D5: {void*
_tmpD94=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpD61->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpD96;struct Cyc_Absyn_Tqual _tmpD97;struct Cyc_Absyn_PtrAtts
_tmpD98;void*_tmpD99;_LL705: {struct Cyc_Absyn_PointerType_struct*_tmpD95=(struct
Cyc_Absyn_PointerType_struct*)_tmpD94;if(_tmpD95->tag != 5)goto _LL707;else{
_tmpD96=_tmpD95->f1;_tmpD97=_tmpD96.elt_tq;_tmpD98=_tmpD96.ptr_atts;_tmpD99=
_tmpD98.rgn;}}_LL706: {struct _tuple16 _tmp1382;return(_tmp1382.f1=_tmpD97.real_const,((
_tmp1382.f2=_tmpD99,_tmp1382)));}_LL707:;_LL708: return bogus_ans;_LL704:;}_LL6D6: {
struct Cyc_Absyn_Subscript_e_struct*_tmpD62=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpD54;if(_tmpD62->tag != 24)goto _LL6D8;else{_tmpD63=_tmpD62->f1;_tmpD64=_tmpD62->f2;}}
_LL6D7: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpD63->topt))->v);void*_tmpD9B=t;struct Cyc_List_List*_tmpD9D;struct Cyc_Absyn_PtrInfo
_tmpD9F;struct Cyc_Absyn_Tqual _tmpDA0;struct Cyc_Absyn_PtrAtts _tmpDA1;void*_tmpDA2;
struct Cyc_Absyn_ArrayInfo _tmpDA4;struct Cyc_Absyn_Tqual _tmpDA5;_LL70A: {struct Cyc_Absyn_TupleType_struct*
_tmpD9C=(struct Cyc_Absyn_TupleType_struct*)_tmpD9B;if(_tmpD9C->tag != 11)goto
_LL70C;else{_tmpD9D=_tmpD9C->f1;}}_LL70B: {unsigned int _tmpDA7;int _tmpDA8;struct
_tuple13 _tmpDA6=Cyc_Evexp_eval_const_uint_exp(_tmpD64);_tmpDA7=_tmpDA6.f1;
_tmpDA8=_tmpDA6.f2;if(!_tmpDA8)return bogus_ans;{struct _tuple11*_tmpDA9=Cyc_Absyn_lookup_tuple_field(
_tmpD9D,(int)_tmpDA7);if(_tmpDA9 != 0){struct _tuple16 _tmp1383;return(_tmp1383.f1=((*
_tmpDA9).f1).real_const,((_tmp1383.f2=(Cyc_Tcutil_addressof_props(te,_tmpD63)).f2,
_tmp1383)));}return bogus_ans;};}_LL70C: {struct Cyc_Absyn_PointerType_struct*
_tmpD9E=(struct Cyc_Absyn_PointerType_struct*)_tmpD9B;if(_tmpD9E->tag != 5)goto
_LL70E;else{_tmpD9F=_tmpD9E->f1;_tmpDA0=_tmpD9F.elt_tq;_tmpDA1=_tmpD9F.ptr_atts;
_tmpDA2=_tmpDA1.rgn;}}_LL70D: {struct _tuple16 _tmp1384;return(_tmp1384.f1=_tmpDA0.real_const,((
_tmp1384.f2=_tmpDA2,_tmp1384)));}_LL70E: {struct Cyc_Absyn_ArrayType_struct*
_tmpDA3=(struct Cyc_Absyn_ArrayType_struct*)_tmpD9B;if(_tmpDA3->tag != 9)goto
_LL710;else{_tmpDA4=_tmpDA3->f1;_tmpDA5=_tmpDA4.tq;}}_LL70F: {struct _tuple16
_tmp1385;return(_tmp1385.f1=_tmpDA5.real_const,((_tmp1385.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD63)).f2,_tmp1385)));}_LL710:;_LL711: return bogus_ans;_LL709:;}_LL6D8:;
_LL6D9:{const char*_tmp1388;void*_tmp1387;(_tmp1387=0,Cyc_Tcutil_terr(e->loc,((
_tmp1388="unary & applied to non-lvalue",_tag_dyneither(_tmp1388,sizeof(char),30))),
_tag_dyneither(_tmp1387,sizeof(void*),0)));}return bogus_ans;_LL6CD:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpDB0=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpDB2;void*_tmpDB3;struct Cyc_Absyn_Tqual
_tmpDB4;union Cyc_Absyn_Constraint*_tmpDB5;_LL713: {struct Cyc_Absyn_ArrayType_struct*
_tmpDB1=(struct Cyc_Absyn_ArrayType_struct*)_tmpDB0;if(_tmpDB1->tag != 9)goto
_LL715;else{_tmpDB2=_tmpDB1->f1;_tmpDB3=_tmpDB2.elt_type;_tmpDB4=_tmpDB2.tq;
_tmpDB5=_tmpDB2.zero_term;}}_LL714: {void*_tmpDB7;struct _tuple16 _tmpDB6=Cyc_Tcutil_addressof_props(
te,e);_tmpDB7=_tmpDB6.f2;{struct Cyc_Absyn_Upper_b_struct _tmp138B;struct Cyc_Absyn_Upper_b_struct*
_tmp138A;return Cyc_Absyn_atb_typ(_tmpDB3,_tmpDB7,_tmpDB4,(void*)((_tmp138A=
_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=1,((_tmp138B.f1=e,
_tmp138B)))),_tmp138A)))),_tmpDB5);};}_LL715:;_LL716: return e_typ;_LL712:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpDBA=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpDBD;
_LL718: {struct Cyc_Absyn_DynEither_b_struct*_tmpDBB=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpDBA;if(_tmpDBB->tag != 0)goto _LL71A;}_LL719: return;_LL71A: {struct Cyc_Absyn_Upper_b_struct*
_tmpDBC=(struct Cyc_Absyn_Upper_b_struct*)_tmpDBA;if(_tmpDBC->tag != 1)goto _LL717;
else{_tmpDBD=_tmpDBC->f1;}}_LL71B: {unsigned int _tmpDBF;int _tmpDC0;struct
_tuple13 _tmpDBE=Cyc_Evexp_eval_const_uint_exp(_tmpDBD);_tmpDBF=_tmpDBE.f1;
_tmpDC0=_tmpDBE.f2;if(_tmpDC0  && _tmpDBF <= i){const char*_tmp1390;void*_tmp138F[2];
struct Cyc_Int_pa_struct _tmp138E;struct Cyc_Int_pa_struct _tmp138D;(_tmp138D.tag=1,((
_tmp138D.f1=(unsigned long)((int)i),((_tmp138E.tag=1,((_tmp138E.f1=(
unsigned long)((int)_tmpDBF),((_tmp138F[0]=& _tmp138E,((_tmp138F[1]=& _tmp138D,Cyc_Tcutil_terr(
loc,((_tmp1390="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1390,
sizeof(char),39))),_tag_dyneither(_tmp138F,sizeof(void*),2)))))))))))));}return;}
_LL717:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpDC5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpDC7;_LL71D: {struct
Cyc_Absyn_Upper_b_struct*_tmpDC6=(struct Cyc_Absyn_Upper_b_struct*)_tmpDC5;if(
_tmpDC6->tag != 1)goto _LL71F;else{_tmpDC7=_tmpDC6->f1;}}_LL71E: {unsigned int
_tmpDC9;int _tmpDCA;struct _tuple13 _tmpDC8=Cyc_Evexp_eval_const_uint_exp(_tmpDC7);
_tmpDC9=_tmpDC8.f1;_tmpDCA=_tmpDC8.f2;return _tmpDCA  && _tmpDC9 == 1;}_LL71F:;
_LL720: return 0;_LL71C:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpDCB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpDD3;
void*_tmpDD4;union Cyc_Absyn_Constraint*_tmpDD5;struct Cyc_List_List*_tmpDD7;
struct Cyc_Absyn_AggrInfo _tmpDD9;union Cyc_Absyn_AggrInfoU _tmpDDA;struct _tuple4
_tmpDDB;struct Cyc_Absyn_AggrInfo _tmpDDD;union Cyc_Absyn_AggrInfoU _tmpDDE;struct
Cyc_Absyn_Aggrdecl**_tmpDDF;struct Cyc_Absyn_Aggrdecl*_tmpDE0;struct Cyc_List_List*
_tmpDE1;struct Cyc_List_List*_tmpDE3;_LL722: {struct Cyc_Absyn_VoidType_struct*
_tmpDCC=(struct Cyc_Absyn_VoidType_struct*)_tmpDCB;if(_tmpDCC->tag != 0)goto _LL724;}
_LL723: goto _LL725;_LL724: {struct Cyc_Absyn_IntType_struct*_tmpDCD=(struct Cyc_Absyn_IntType_struct*)
_tmpDCB;if(_tmpDCD->tag != 6)goto _LL726;}_LL725: goto _LL727;_LL726: {struct Cyc_Absyn_FloatType_struct*
_tmpDCE=(struct Cyc_Absyn_FloatType_struct*)_tmpDCB;if(_tmpDCE->tag != 7)goto
_LL728;}_LL727: goto _LL729;_LL728: {struct Cyc_Absyn_DoubleType_struct*_tmpDCF=(
struct Cyc_Absyn_DoubleType_struct*)_tmpDCB;if(_tmpDCF->tag != 8)goto _LL72A;}
_LL729: return 1;_LL72A: {struct Cyc_Absyn_EnumType_struct*_tmpDD0=(struct Cyc_Absyn_EnumType_struct*)
_tmpDCB;if(_tmpDD0->tag != 14)goto _LL72C;}_LL72B: goto _LL72D;_LL72C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpDD1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpDCB;if(_tmpDD1->tag != 15)goto
_LL72E;}_LL72D: return 0;_LL72E: {struct Cyc_Absyn_ArrayType_struct*_tmpDD2=(struct
Cyc_Absyn_ArrayType_struct*)_tmpDCB;if(_tmpDD2->tag != 9)goto _LL730;else{_tmpDD3=
_tmpDD2->f1;_tmpDD4=_tmpDD3.elt_type;_tmpDD5=_tmpDD3.zero_term;}}_LL72F: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDD5) && Cyc_Tcutil_bits_only(
_tmpDD4);_LL730: {struct Cyc_Absyn_TupleType_struct*_tmpDD6=(struct Cyc_Absyn_TupleType_struct*)
_tmpDCB;if(_tmpDD6->tag != 11)goto _LL732;else{_tmpDD7=_tmpDD6->f1;}}_LL731: for(0;
_tmpDD7 != 0;_tmpDD7=_tmpDD7->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpDD7->hd)).f2))return 0;}return 1;_LL732: {struct Cyc_Absyn_AggrType_struct*
_tmpDD8=(struct Cyc_Absyn_AggrType_struct*)_tmpDCB;if(_tmpDD8->tag != 12)goto
_LL734;else{_tmpDD9=_tmpDD8->f1;_tmpDDA=_tmpDD9.aggr_info;if((_tmpDDA.UnknownAggr).tag
!= 1)goto _LL734;_tmpDDB=(struct _tuple4)(_tmpDDA.UnknownAggr).val;}}_LL733: return
0;_LL734: {struct Cyc_Absyn_AggrType_struct*_tmpDDC=(struct Cyc_Absyn_AggrType_struct*)
_tmpDCB;if(_tmpDDC->tag != 12)goto _LL736;else{_tmpDDD=_tmpDDC->f1;_tmpDDE=_tmpDDD.aggr_info;
if((_tmpDDE.KnownAggr).tag != 2)goto _LL736;_tmpDDF=(struct Cyc_Absyn_Aggrdecl**)(
_tmpDDE.KnownAggr).val;_tmpDE0=*_tmpDDF;_tmpDE1=_tmpDDD.targs;}}_LL735: if(
_tmpDE0->impl == 0)return 0;{struct _RegionHandle _tmpDE4=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpDE4;_push_region(rgn);{struct Cyc_List_List*_tmpDE5=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDE0->tvs,_tmpDE1);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE0->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDE5,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDE6=0;_npop_handler(0);return
_tmpDE6;}}}{int _tmpDE7=1;_npop_handler(0);return _tmpDE7;};};_pop_region(rgn);};
_LL736: {struct Cyc_Absyn_AnonAggrType_struct*_tmpDE2=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDCB;if(_tmpDE2->tag != 13)goto _LL738;else{_tmpDE3=_tmpDE2->f2;}}_LL737: for(0;
_tmpDE3 != 0;_tmpDE3=_tmpDE3->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpDE3->hd)->type))return 0;}return 1;_LL738:;_LL739: return 0;_LL721:;}struct
_tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpDE8=e->r;
struct _tuple2*_tmpDF0;void*_tmpDF1;struct Cyc_Absyn_Exp*_tmpDF3;struct Cyc_Absyn_Exp*
_tmpDF4;struct Cyc_Absyn_Exp*_tmpDF5;struct Cyc_Absyn_Exp*_tmpDF7;struct Cyc_Absyn_Exp*
_tmpDF8;struct Cyc_Absyn_Exp*_tmpDFA;struct Cyc_Absyn_Exp*_tmpDFC;void*_tmpDFE;
struct Cyc_Absyn_Exp*_tmpDFF;enum Cyc_Absyn_Coercion _tmpE00;void*_tmpE02;struct
Cyc_Absyn_Exp*_tmpE03;struct Cyc_Absyn_Exp*_tmpE05;struct Cyc_Absyn_Exp*_tmpE07;
struct Cyc_Absyn_Exp*_tmpE08;struct Cyc_List_List*_tmpE0A;struct Cyc_List_List*
_tmpE0C;struct Cyc_List_List*_tmpE0E;enum Cyc_Absyn_Primop _tmpE10;struct Cyc_List_List*
_tmpE11;struct Cyc_List_List*_tmpE13;struct Cyc_List_List*_tmpE15;_LL73B: {struct
Cyc_Absyn_Const_e_struct*_tmpDE9=(struct Cyc_Absyn_Const_e_struct*)_tmpDE8;if(
_tmpDE9->tag != 0)goto _LL73D;}_LL73C: goto _LL73E;_LL73D: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDEA=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDE8;if(_tmpDEA->tag != 18)goto
_LL73F;}_LL73E: goto _LL740;_LL73F: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpDEB=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDE8;if(_tmpDEB->tag != 19)goto _LL741;}
_LL740: goto _LL742;_LL741: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDEC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDE8;if(_tmpDEC->tag != 20)goto _LL743;}_LL742: goto _LL744;_LL743: {struct Cyc_Absyn_Enum_e_struct*
_tmpDED=(struct Cyc_Absyn_Enum_e_struct*)_tmpDE8;if(_tmpDED->tag != 32)goto _LL745;}
_LL744: goto _LL746;_LL745: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDEE=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDE8;if(_tmpDEE->tag != 33)goto _LL747;}_LL746: return 1;_LL747: {struct Cyc_Absyn_Var_e_struct*
_tmpDEF=(struct Cyc_Absyn_Var_e_struct*)_tmpDE8;if(_tmpDEF->tag != 1)goto _LL749;
else{_tmpDF0=_tmpDEF->f1;_tmpDF1=(void*)_tmpDEF->f2;}}_LL748: {void*_tmpE16=
_tmpDF1;struct Cyc_Absyn_Vardecl*_tmpE19;_LL768: {struct Cyc_Absyn_Funname_b_struct*
_tmpE17=(struct Cyc_Absyn_Funname_b_struct*)_tmpE16;if(_tmpE17->tag != 2)goto
_LL76A;}_LL769: return 1;_LL76A: {struct Cyc_Absyn_Global_b_struct*_tmpE18=(struct
Cyc_Absyn_Global_b_struct*)_tmpE16;if(_tmpE18->tag != 1)goto _LL76C;else{_tmpE19=
_tmpE18->f1;}}_LL76B: {void*_tmpE1B=Cyc_Tcutil_compress(_tmpE19->type);_LL771: {
struct Cyc_Absyn_ArrayType_struct*_tmpE1C=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE1B;if(_tmpE1C->tag != 9)goto _LL773;}_LL772: goto _LL774;_LL773: {struct Cyc_Absyn_FnType_struct*
_tmpE1D=(struct Cyc_Absyn_FnType_struct*)_tmpE1B;if(_tmpE1D->tag != 10)goto _LL775;}
_LL774: return 1;_LL775:;_LL776: return var_okay;_LL770:;}_LL76C: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpE1A=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpE16;if(_tmpE1A->tag != 0)goto
_LL76E;}_LL76D: return 0;_LL76E:;_LL76F: return var_okay;_LL767:;}_LL749: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDF2=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDE8;if(_tmpDF2->tag != 6)goto
_LL74B;else{_tmpDF3=_tmpDF2->f1;_tmpDF4=_tmpDF2->f2;_tmpDF5=_tmpDF2->f3;}}_LL74A:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpDF3) && Cyc_Tcutil_cnst_exp(te,0,_tmpDF4))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpDF5);_LL74B: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpDF6=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpDE8;if(_tmpDF6->tag != 9)goto _LL74D;
else{_tmpDF7=_tmpDF6->f1;_tmpDF8=_tmpDF6->f2;}}_LL74C: return Cyc_Tcutil_cnst_exp(
te,0,_tmpDF7) && Cyc_Tcutil_cnst_exp(te,0,_tmpDF8);_LL74D: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpDF9=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpDE8;if(_tmpDF9->tag != 13)
goto _LL74F;else{_tmpDFA=_tmpDF9->f1;}}_LL74E: _tmpDFC=_tmpDFA;goto _LL750;_LL74F: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpDFB=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpDE8;if(_tmpDFB->tag != 14)goto _LL751;else{_tmpDFC=_tmpDFB->f1;}}_LL750: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpDFC);_LL751: {struct Cyc_Absyn_Cast_e_struct*
_tmpDFD=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE8;if(_tmpDFD->tag != 15)goto _LL753;
else{_tmpDFE=(void*)_tmpDFD->f1;_tmpDFF=_tmpDFD->f2;_tmpE00=_tmpDFD->f4;if(
_tmpE00 != Cyc_Absyn_No_coercion)goto _LL753;}}_LL752: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpDFF);_LL753: {struct Cyc_Absyn_Cast_e_struct*_tmpE01=(struct Cyc_Absyn_Cast_e_struct*)
_tmpDE8;if(_tmpE01->tag != 15)goto _LL755;else{_tmpE02=(void*)_tmpE01->f1;_tmpE03=
_tmpE01->f2;}}_LL754: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE03);_LL755: {
struct Cyc_Absyn_Address_e_struct*_tmpE04=(struct Cyc_Absyn_Address_e_struct*)
_tmpDE8;if(_tmpE04->tag != 16)goto _LL757;else{_tmpE05=_tmpE04->f1;}}_LL756: return
Cyc_Tcutil_cnst_exp(te,1,_tmpE05);_LL757: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpE06=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpDE8;if(_tmpE06->tag != 28)
goto _LL759;else{_tmpE07=_tmpE06->f2;_tmpE08=_tmpE06->f3;}}_LL758: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE07) && Cyc_Tcutil_cnst_exp(te,0,_tmpE08);_LL759: {struct Cyc_Absyn_Array_e_struct*
_tmpE09=(struct Cyc_Absyn_Array_e_struct*)_tmpDE8;if(_tmpE09->tag != 27)goto _LL75B;
else{_tmpE0A=_tmpE09->f1;}}_LL75A: _tmpE0C=_tmpE0A;goto _LL75C;_LL75B: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpE0B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpDE8;if(_tmpE0B->tag != 30)goto
_LL75D;else{_tmpE0C=_tmpE0B->f2;}}_LL75C: _tmpE0E=_tmpE0C;goto _LL75E;_LL75D: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpE0D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDE8;if(_tmpE0D->tag != 29)goto _LL75F;else{_tmpE0E=_tmpE0D->f3;}}_LL75E: for(0;
_tmpE0E != 0;_tmpE0E=_tmpE0E->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple30*)
_tmpE0E->hd)).f2))return 0;}return 1;_LL75F: {struct Cyc_Absyn_Primop_e_struct*
_tmpE0F=(struct Cyc_Absyn_Primop_e_struct*)_tmpDE8;if(_tmpE0F->tag != 3)goto _LL761;
else{_tmpE10=_tmpE0F->f1;_tmpE11=_tmpE0F->f2;}}_LL760: _tmpE13=_tmpE11;goto _LL762;
_LL761: {struct Cyc_Absyn_Tuple_e_struct*_tmpE12=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpDE8;if(_tmpE12->tag != 25)goto _LL763;else{_tmpE13=_tmpE12->f1;}}_LL762:
_tmpE15=_tmpE13;goto _LL764;_LL763: {struct Cyc_Absyn_Datatype_e_struct*_tmpE14=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpDE8;if(_tmpE14->tag != 31)goto _LL765;else{
_tmpE15=_tmpE14->f1;}}_LL764: for(0;_tmpE15 != 0;_tmpE15=_tmpE15->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpE15->hd))return 0;}return 1;_LL765:;_LL766: return 0;
_LL73A:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpE1E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE24;struct Cyc_Absyn_PtrAtts _tmpE25;union Cyc_Absyn_Constraint*
_tmpE26;union Cyc_Absyn_Constraint*_tmpE27;struct Cyc_Absyn_ArrayInfo _tmpE29;void*
_tmpE2A;struct Cyc_List_List*_tmpE2C;struct Cyc_Absyn_AggrInfo _tmpE2E;union Cyc_Absyn_AggrInfoU
_tmpE2F;struct Cyc_List_List*_tmpE30;struct Cyc_List_List*_tmpE32;_LL778: {struct
Cyc_Absyn_VoidType_struct*_tmpE1F=(struct Cyc_Absyn_VoidType_struct*)_tmpE1E;if(
_tmpE1F->tag != 0)goto _LL77A;}_LL779: goto _LL77B;_LL77A: {struct Cyc_Absyn_IntType_struct*
_tmpE20=(struct Cyc_Absyn_IntType_struct*)_tmpE1E;if(_tmpE20->tag != 6)goto _LL77C;}
_LL77B: goto _LL77D;_LL77C: {struct Cyc_Absyn_FloatType_struct*_tmpE21=(struct Cyc_Absyn_FloatType_struct*)
_tmpE1E;if(_tmpE21->tag != 7)goto _LL77E;}_LL77D: goto _LL77F;_LL77E: {struct Cyc_Absyn_DoubleType_struct*
_tmpE22=(struct Cyc_Absyn_DoubleType_struct*)_tmpE1E;if(_tmpE22->tag != 8)goto
_LL780;}_LL77F: return 1;_LL780: {struct Cyc_Absyn_PointerType_struct*_tmpE23=(
struct Cyc_Absyn_PointerType_struct*)_tmpE1E;if(_tmpE23->tag != 5)goto _LL782;else{
_tmpE24=_tmpE23->f1;_tmpE25=_tmpE24.ptr_atts;_tmpE26=_tmpE25.nullable;_tmpE27=
_tmpE25.bounds;}}_LL781: {void*_tmpE35=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE27);_LL791: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE36=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE35;if(_tmpE36->tag != 0)goto _LL793;}_LL792: return 1;_LL793:;_LL794: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpE26);_LL790:;}
_LL782: {struct Cyc_Absyn_ArrayType_struct*_tmpE28=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE1E;if(_tmpE28->tag != 9)goto _LL784;else{_tmpE29=_tmpE28->f1;_tmpE2A=_tmpE29.elt_type;}}
_LL783: return Cyc_Tcutil_supports_default(_tmpE2A);_LL784: {struct Cyc_Absyn_TupleType_struct*
_tmpE2B=(struct Cyc_Absyn_TupleType_struct*)_tmpE1E;if(_tmpE2B->tag != 11)goto
_LL786;else{_tmpE2C=_tmpE2B->f1;}}_LL785: for(0;_tmpE2C != 0;_tmpE2C=_tmpE2C->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpE2C->hd)).f2))return 0;}
return 1;_LL786: {struct Cyc_Absyn_AggrType_struct*_tmpE2D=(struct Cyc_Absyn_AggrType_struct*)
_tmpE1E;if(_tmpE2D->tag != 12)goto _LL788;else{_tmpE2E=_tmpE2D->f1;_tmpE2F=_tmpE2E.aggr_info;
_tmpE30=_tmpE2E.targs;}}_LL787: {struct Cyc_Absyn_Aggrdecl*_tmpE37=Cyc_Absyn_get_known_aggrdecl(
_tmpE2F);if(_tmpE37->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpE37->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpE37->tvs,_tmpE30,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE37->impl))->fields);}
_LL788: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE31=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE1E;if(_tmpE31->tag != 13)goto _LL78A;else{_tmpE32=_tmpE31->f2;}}_LL789: return
Cyc_Tcutil_fields_support_default(0,0,_tmpE32);_LL78A: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpE33=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE1E;if(_tmpE33->tag != 15)goto
_LL78C;}_LL78B: goto _LL78D;_LL78C: {struct Cyc_Absyn_EnumType_struct*_tmpE34=(
struct Cyc_Absyn_EnumType_struct*)_tmpE1E;if(_tmpE34->tag != 14)goto _LL78E;}_LL78D:
return 1;_LL78E:;_LL78F: return 0;_LL777:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpE38=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpE38;_push_region(rgn);{struct Cyc_List_List*_tmpE39=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpE39,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE3A=0;_npop_handler(0);return _tmpE3A;}}}{
int _tmpE3B=1;_npop_handler(0);return _tmpE3B;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpE3C=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpE41;struct Cyc_Absyn_PtrAtts _tmpE42;union Cyc_Absyn_Constraint*
_tmpE43;union Cyc_Absyn_Constraint*_tmpE44;_LL796: {struct Cyc_Absyn_IntType_struct*
_tmpE3D=(struct Cyc_Absyn_IntType_struct*)_tmpE3C;if(_tmpE3D->tag != 6)goto _LL798;}
_LL797: goto _LL799;_LL798: {struct Cyc_Absyn_FloatType_struct*_tmpE3E=(struct Cyc_Absyn_FloatType_struct*)
_tmpE3C;if(_tmpE3E->tag != 7)goto _LL79A;}_LL799: goto _LL79B;_LL79A: {struct Cyc_Absyn_DoubleType_struct*
_tmpE3F=(struct Cyc_Absyn_DoubleType_struct*)_tmpE3C;if(_tmpE3F->tag != 8)goto
_LL79C;}_LL79B: return 1;_LL79C: {struct Cyc_Absyn_PointerType_struct*_tmpE40=(
struct Cyc_Absyn_PointerType_struct*)_tmpE3C;if(_tmpE40->tag != 5)goto _LL79E;else{
_tmpE41=_tmpE40->f1;_tmpE42=_tmpE41.ptr_atts;_tmpE43=_tmpE42.nullable;_tmpE44=
_tmpE42.bounds;}}_LL79D: {void*_tmpE45=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE44);_LL7A1: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE46=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE45;if(_tmpE46->tag != 0)goto _LL7A3;}_LL7A2: return 0;_LL7A3:;_LL7A4: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE43);_LL7A0:;}
_LL79E:;_LL79F: return 0;_LL795:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpE47=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE49;void*
_tmpE4A;struct Cyc_Absyn_FnInfo _tmpE4C;struct Cyc_List_List*_tmpE4D;_LL7A6: {
struct Cyc_Absyn_PointerType_struct*_tmpE48=(struct Cyc_Absyn_PointerType_struct*)
_tmpE47;if(_tmpE48->tag != 5)goto _LL7A8;else{_tmpE49=_tmpE48->f1;_tmpE4A=_tmpE49.elt_typ;}}
_LL7A7: return Cyc_Tcutil_is_noreturn(_tmpE4A);_LL7A8: {struct Cyc_Absyn_FnType_struct*
_tmpE4B=(struct Cyc_Absyn_FnType_struct*)_tmpE47;if(_tmpE4B->tag != 10)goto _LL7AA;
else{_tmpE4C=_tmpE4B->f1;_tmpE4D=_tmpE4C.attributes;}}_LL7A9: for(0;_tmpE4D != 0;
_tmpE4D=_tmpE4D->tl){void*_tmpE4E=(void*)_tmpE4D->hd;_LL7AD: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpE4F=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpE4E;if(_tmpE4F->tag != 4)goto
_LL7AF;}_LL7AE: return 1;_LL7AF:;_LL7B0: continue;_LL7AC:;}goto _LL7A5;_LL7AA:;
_LL7AB: goto _LL7A5;_LL7A5:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpE50=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpE52;struct Cyc_List_List*_tmpE53;struct Cyc_List_List**_tmpE54;_LL7B2: {struct
Cyc_Absyn_FnType_struct*_tmpE51=(struct Cyc_Absyn_FnType_struct*)_tmpE50;if(
_tmpE51->tag != 10)goto _LL7B4;else{_tmpE52=_tmpE51->f1;_tmpE53=_tmpE52.attributes;
_tmpE54=(struct Cyc_List_List**)&(_tmpE51->f1).attributes;}}_LL7B3: {struct Cyc_List_List*
_tmpE55=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1391;*_tmpE54=((_tmp1391=_cycalloc(sizeof(*_tmp1391)),((
_tmp1391->hd=(void*)atts->hd,((_tmp1391->tl=*_tmpE54,_tmp1391))))));}else{struct
Cyc_List_List*_tmp1392;_tmpE55=((_tmp1392=_cycalloc(sizeof(*_tmp1392)),((
_tmp1392->hd=(void*)atts->hd,((_tmp1392->tl=_tmpE55,_tmp1392))))));}}return
_tmpE55;}_LL7B4:;_LL7B5: {const char*_tmp1395;void*_tmp1394;(_tmp1394=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1395="transfer_fn_type_atts",
_tag_dyneither(_tmp1395,sizeof(char),22))),_tag_dyneither(_tmp1394,sizeof(void*),
0)));}_LL7B1:;}
