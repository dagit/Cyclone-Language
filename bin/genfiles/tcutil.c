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
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
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
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpEEE;void*
_tmpEED[2];const char*_tmpEEC;const char*_tmpEEB;struct Cyc_String_pa_struct _tmpEEA;
struct Cyc_String_pa_struct _tmpEE9;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpEE9.tag=0,((_tmpEE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpEEA.tag=0,((_tmpEEA.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpEEB="const ",_tag_dyneither(_tmpEEB,sizeof(char),7)):((_tmpEEC="",
_tag_dyneither(_tmpEEC,sizeof(char),1)))),((_tmpEED[0]=& _tmpEEA,((_tmpEED[1]=&
_tmpEE9,Cyc_aprintf(((_tmpEEE="%s%s",_tag_dyneither(_tmpEEE,sizeof(char),5))),
_tag_dyneither(_tmpEED,sizeof(void*),2))))))))))))));const char*_tmpEF7;void*
_tmpEF6[2];const char*_tmpEF5;const char*_tmpEF4;struct Cyc_String_pa_struct _tmpEF3;
struct Cyc_String_pa_struct _tmpEF2;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpEF2.tag=0,((_tmpEF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpEF3.tag=0,((_tmpEF3.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpEF4="const ",_tag_dyneither(_tmpEF4,sizeof(char),7)):((_tmpEF5="",
_tag_dyneither(_tmpEF5,sizeof(char),1)))),((_tmpEF6[0]=& _tmpEF3,((_tmpEF6[1]=&
_tmpEF2,Cyc_aprintf(((_tmpEF7="%s%s",_tag_dyneither(_tmpEF7,sizeof(char),5))),
_tag_dyneither(_tmpEF6,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpEFB;
void*_tmpEFA[1];struct Cyc_String_pa_struct _tmpEF9;(_tmpEF9.tag=0,((_tmpEF9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEFA[0]=& _tmpEF9,Cyc_fprintf(
Cyc_stderr,((_tmpEFB="  %s",_tag_dyneither(_tmpEFB,sizeof(char),5))),
_tag_dyneither(_tmpEFA,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpEFE;void*_tmpEFD;(_tmpEFD=0,Cyc_fprintf(
Cyc_stderr,((_tmpEFE="\n\t",_tag_dyneither(_tmpEFE,sizeof(char),3))),
_tag_dyneither(_tmpEFD,sizeof(void*),0)));}pos=8;}else{{const char*_tmpF01;void*
_tmpF00;(_tmpF00=0,Cyc_fprintf(Cyc_stderr,((_tmpF01=" ",_tag_dyneither(_tmpF01,
sizeof(char),2))),_tag_dyneither(_tmpF00,sizeof(void*),0)));}++ pos;}{const char*
_tmpF04;void*_tmpF03;(_tmpF03=0,Cyc_fprintf(Cyc_stderr,((_tmpF04="and ",
_tag_dyneither(_tmpF04,sizeof(char),5))),_tag_dyneither(_tmpF03,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpF07;
void*_tmpF06;(_tmpF06=0,Cyc_fprintf(Cyc_stderr,((_tmpF07="\n\t",_tag_dyneither(
_tmpF07,sizeof(char),3))),_tag_dyneither(_tmpF06,sizeof(void*),0)));}pos=8;}{
const char*_tmpF0B;void*_tmpF0A[1];struct Cyc_String_pa_struct _tmpF09;(_tmpF09.tag=
0,((_tmpF09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpF0A[0]=&
_tmpF09,Cyc_fprintf(Cyc_stderr,((_tmpF0B="%s ",_tag_dyneither(_tmpF0B,sizeof(
char),4))),_tag_dyneither(_tmpF0A,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpF0E;void*
_tmpF0D;(_tmpF0D=0,Cyc_fprintf(Cyc_stderr,((_tmpF0E="\n\t",_tag_dyneither(
_tmpF0E,sizeof(char),3))),_tag_dyneither(_tmpF0D,sizeof(void*),0)));}pos=8;}{
const char*_tmpF11;void*_tmpF10;(_tmpF10=0,Cyc_fprintf(Cyc_stderr,((_tmpF11="are not compatible. ",
_tag_dyneither(_tmpF11,sizeof(char),21))),_tag_dyneither(_tmpF10,sizeof(void*),0)));}
pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const
char*_tmpF14;void*_tmpF13;(_tmpF13=0,Cyc_fprintf(Cyc_stderr,((_tmpF14="\n\t",
_tag_dyneither(_tmpF14,sizeof(char),3))),_tag_dyneither(_tmpF13,sizeof(void*),0)));}{
const char*_tmpF18;void*_tmpF17[1];struct Cyc_String_pa_struct _tmpF16;(_tmpF16.tag=
0,((_tmpF16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpF17[0]=& _tmpF16,Cyc_fprintf(Cyc_stderr,((_tmpF18="%s",_tag_dyneither(_tmpF18,
sizeof(char),3))),_tag_dyneither(_tmpF17,sizeof(void*),1)))))));};}{const char*
_tmpF1B;void*_tmpF1A;(_tmpF1A=0,Cyc_fprintf(Cyc_stderr,((_tmpF1B="\n",
_tag_dyneither(_tmpF1B,sizeof(char),2))),_tag_dyneither(_tmpF1A,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpF1F;void*_tmpF1E[1];struct Cyc_String_pa_struct _tmpF1D;(_tmpF1D.tag=
0,((_tmpF1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpF1E[0]=&
_tmpF1D,Cyc_fprintf(Cyc_stderr,((_tmpF1F="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpF1F,sizeof(char),36))),_tag_dyneither(_tmpF1E,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpF22;struct Cyc_Core_Impossible_struct*_tmpF21;(int)_throw((void*)((_tmpF21=
_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF22.tag=Cyc_Core_Impossible,((
_tmpF22.f1=msg,_tmpF22)))),_tmpF21)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpF27;void*_tmpF26[2];struct Cyc_String_pa_struct _tmpF25;struct Cyc_String_pa_struct
_tmpF24;(_tmpF24.tag=0,((_tmpF24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpF25.tag=
0,((_tmpF25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpF26[0]=& _tmpF25,((_tmpF26[1]=& _tmpF24,Cyc_fprintf(
Cyc_stderr,((_tmpF27="%s::%s ",_tag_dyneither(_tmpF27,sizeof(char),8))),
_tag_dyneither(_tmpF26,sizeof(void*),2)))))))))))));}{const char*_tmpF2A;void*
_tmpF29;(_tmpF29=0,Cyc_fprintf(Cyc_stderr,((_tmpF2A="\n",_tag_dyneither(_tmpF2A,
sizeof(char),2))),_tag_dyneither(_tmpF29,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpF2B;Cyc_Tcutil_warning_segs=((_tmpF2B=_cycalloc(
sizeof(*_tmpF2B)),((_tmpF2B->hd=sg,((_tmpF2B->tl=Cyc_Tcutil_warning_segs,_tmpF2B))))));}{
struct _dyneither_ptr*_tmpF2E;struct Cyc_List_List*_tmpF2D;Cyc_Tcutil_warning_msgs=((
_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D->hd=((_tmpF2E=_cycalloc(sizeof(*
_tmpF2E)),((_tmpF2E[0]=msg,_tmpF2E)))),((_tmpF2D->tl=Cyc_Tcutil_warning_msgs,
_tmpF2D))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpF31;void*_tmpF30;(_tmpF30=0,
Cyc_fprintf(Cyc_stderr,((_tmpF31="***Warnings***\n",_tag_dyneither(_tmpF31,
sizeof(char),16))),_tag_dyneither(_tmpF30,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpF36;
void*_tmpF35[2];struct Cyc_String_pa_struct _tmpF34;struct Cyc_String_pa_struct
_tmpF33;(_tmpF33.tag=0,((_tmpF33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpF34.tag=0,((_tmpF34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmpF35[0]=& _tmpF34,((_tmpF35[1]=& _tmpF33,Cyc_fprintf(Cyc_stderr,((_tmpF36="%s: %s\n",
_tag_dyneither(_tmpF36,sizeof(char),8))),_tag_dyneither(_tmpF35,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpF39;void*_tmpF38;(_tmpF38=0,Cyc_fprintf(Cyc_stderr,((_tmpF39="**************\n",
_tag_dyneither(_tmpF39,sizeof(char),16))),_tag_dyneither(_tmpF38,sizeof(void*),0)));}
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmpF3A;*_tmp44=((_tmpF3A=
_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A[0]=t2,_tmpF3A))));}return t2;}_LL7: {struct
Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(_tmp45->tag
!= 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)& _tmp45->f2;}}
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp47))->v){
struct Cyc_Core_Opt*_tmpF3B;*_tmp47=((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((
_tmpF3B->v=t2,_tmpF3B))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
_tmpF3E;struct Cyc_Absyn_Unknown_kb_struct*_tmpF3D;return(void*)((_tmpF3D=
_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D[0]=((_tmpF3E.tag=1,((_tmpF3E.f1=0,_tmpF3E)))),
_tmpF3D))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct Cyc_Absyn_Less_kb_struct*)
_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}_LL1D: {struct Cyc_Absyn_Less_kb_struct
_tmpF41;struct Cyc_Absyn_Less_kb_struct*_tmpF40;return(void*)((_tmpF40=_cycalloc(
sizeof(*_tmpF40)),((_tmpF40[0]=((_tmpF41.tag=2,((_tmpF41.f1=0,((_tmpF41.f2=
_tmp59,_tmpF41)))))),_tmpF40))));}_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpF42;return(_tmpF42=_cycalloc(
sizeof(*_tmpF42)),((_tmpF42->name=tv->name,((_tmpF42->identity=- 1,((_tmpF42->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmpF42)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmpF43;return(_tmpF43=_cycalloc(sizeof(*_tmpF43)),((
_tmpF43->f1=_tmp61,((_tmpF43->f2=_tmp62,((_tmpF43->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmpF43)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmpF44;return(_tmpF44=_cycalloc(sizeof(*_tmpF44)),((_tmpF44->f1=_tmp67,((
_tmpF44->f2=Cyc_Tcutil_copy_type(_tmp68),_tmpF44)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpF45;return(_tmpF45=_cycalloc(sizeof(*_tmpF45)),((_tmpF45->name=f->name,((
_tmpF45->tq=f->tq,((_tmpF45->type=Cyc_Tcutil_copy_type(f->type),((_tmpF45->width=
f->width,((_tmpF45->attributes=f->attributes,_tmpF45)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*_tmpF46;
return(_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmpF46->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmpF46)))));};}static struct
Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmpF47;return(_tmpF47=_cycalloc(sizeof(*_tmpF47)),((
_tmpF47->name=f->name,((_tmpF47->tag=f->tag,((_tmpF47->loc=f->loc,_tmpF47)))))));}
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
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmpF4A;struct Cyc_Absyn_VarType_struct*
_tmpF49;return(void*)((_tmpF49=_cycalloc(sizeof(*_tmpF49)),((_tmpF49[0]=((
_tmpF4A.tag=2,((_tmpF4A.f1=Cyc_Tcutil_copy_tvar(_tmp75),_tmpF4A)))),_tmpF49))));}
_LL27: {struct Cyc_Absyn_DatatypeType_struct*_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp71;if(_tmp76->tag != 3)goto _LL29;else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;
_tmp79=_tmp77.targs;}}_LL28: {struct Cyc_Absyn_DatatypeType_struct _tmpF50;struct
Cyc_Absyn_DatatypeInfo _tmpF4F;struct Cyc_Absyn_DatatypeType_struct*_tmpF4E;return(
void*)((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E[0]=((_tmpF50.tag=3,((
_tmpF50.f1=((_tmpF4F.datatype_info=_tmp78,((_tmpF4F.targs=Cyc_Tcutil_copy_types(
_tmp79),_tmpF4F)))),_tmpF50)))),_tmpF4E))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp71;if(_tmp7A->tag != 4)goto
_LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}
_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpF56;struct Cyc_Absyn_DatatypeFieldInfo
_tmpF55;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF54;return(void*)((_tmpF54=
_cycalloc(sizeof(*_tmpF54)),((_tmpF54[0]=((_tmpF56.tag=4,((_tmpF56.f1=((_tmpF55.field_info=
_tmp7C,((_tmpF55.targs=Cyc_Tcutil_copy_types(_tmp7D),_tmpF55)))),_tmpF56)))),
_tmpF54))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*_tmp7E=(struct Cyc_Absyn_PointerType_struct*)
_tmp71;if(_tmp7E->tag != 5)goto _LL2D;else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;
_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;
_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;_tmp87=_tmp82.ptrloc;}}_LL2C: {void*
_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*_tmpD1=Cyc_Tcutil_copy_type(_tmp83);
union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual _tmpD3=_tmp81;union
Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*_tmpD5=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmpF60;struct Cyc_Absyn_PtrAtts _tmpF5F;
struct Cyc_Absyn_PtrInfo _tmpF5E;struct Cyc_Absyn_PointerType_struct*_tmpF5D;return(
void*)((_tmpF5D=_cycalloc(sizeof(*_tmpF5D)),((_tmpF5D[0]=((_tmpF60.tag=5,((
_tmpF60.f1=((_tmpF5E.elt_typ=_tmpD0,((_tmpF5E.elt_tq=_tmpD3,((_tmpF5E.ptr_atts=((
_tmpF5F.rgn=_tmpD1,((_tmpF5F.nullable=_tmpD2,((_tmpF5F.bounds=_tmpD4,((_tmpF5F.zero_term=
_tmpD5,((_tmpF5F.ptrloc=_tmp87,_tmpF5F)))))))))),_tmpF5E)))))),_tmpF60)))),
_tmpF5D))));}_LL2D: {struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)
_tmp71;if(_tmp88->tag != 6)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*
_tmp89=(struct Cyc_Absyn_FloatType_struct*)_tmp71;if(_tmp89->tag != 7)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp71;if(_tmp8A->tag != 8)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)_tmp71;if(_tmp8B->tag != 9)goto _LL35;
else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;
_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmpF66;struct Cyc_Absyn_ArrayInfo _tmpF65;struct Cyc_Absyn_ArrayType_struct*
_tmpF64;return(void*)((_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64[0]=((
_tmpF66.tag=9,((_tmpF66.f1=((_tmpF65.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((
_tmpF65.tq=_tmp8E,((_tmpF65.num_elts=_tmp8F,((_tmpF65.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmpF65.zt_loc=
_tmp91,_tmpF65)))))))))),_tmpF66)))),_tmpF64))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp92=(struct Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;
else{_tmp93=_tmp92->f1;_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;
_tmp97=_tmp93.args;_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=
_tmp93.rgn_po;_tmp9B=_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*
_tmpF67;struct Cyc_Core_Opt*_tmpDE=_tmp95 == 0?0:((_tmpF67=_cycalloc(sizeof(*
_tmpF67)),((_tmpF67->v=Cyc_Tcutil_copy_type((void*)_tmp95->v),_tmpF67))));void*
_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp99 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp99;struct Cyc_Absyn_VarargInfo*
_tmpF68;cyc_varargs2=((_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68->name=cv->name,((
_tmpF68->tq=cv->tq,((_tmpF68->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF68->inject=
cv->inject,_tmpF68))))))))));}{struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp9A);struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmpF6E;
struct Cyc_Absyn_FnInfo _tmpF6D;struct Cyc_Absyn_FnType_struct*_tmpF6C;return(void*)((
_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6E.tag=10,((_tmpF6E.f1=((
_tmpF6D.tvars=_tmpDD,((_tmpF6D.effect=_tmpDE,((_tmpF6D.ret_typ=_tmpDF,((_tmpF6D.args=
_tmpE0,((_tmpF6D.c_varargs=_tmpE1,((_tmpF6D.cyc_varargs=cyc_varargs2,((_tmpF6D.rgn_po=
_tmpE3,((_tmpF6D.attributes=_tmpE4,_tmpF6D)))))))))))))))),_tmpF6E)))),_tmpF6C))));};}
_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(struct Cyc_Absyn_TupleType_struct*)
_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmpF71;struct Cyc_Absyn_TupleType_struct*_tmpF70;return(void*)((_tmpF70=
_cycalloc(sizeof(*_tmpF70)),((_tmpF70[0]=((_tmpF71.tag=11,((_tmpF71.f1=((struct
Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),_tmpF71)))),_tmpF70))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmp9E=(struct Cyc_Absyn_AggrType_struct*)_tmp71;
if(_tmp9E->tag != 12)goto _LL3B;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((
_tmpA0.UnknownAggr).tag != 1)goto _LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;
_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmpF77;struct Cyc_Absyn_AggrInfo _tmpF76;struct Cyc_Absyn_AggrType_struct*
_tmpF75;return(void*)((_tmpF75=_cycalloc(sizeof(*_tmpF75)),((_tmpF75[0]=((
_tmpF77.tag=12,((_tmpF77.f1=((_tmpF76.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmpF76.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmpF76)))),_tmpF77)))),
_tmpF75))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(struct Cyc_Absyn_AggrType_struct*)
_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;
if((_tmpA8.KnownAggr).tag != 2)goto _LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmpF7D;struct Cyc_Absyn_AggrInfo _tmpF7C;struct Cyc_Absyn_AggrType_struct*_tmpF7B;
return(void*)((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((_tmpF7B[0]=((_tmpF7D.tag=12,((
_tmpF7D.f1=((_tmpF7C.aggr_info=Cyc_Absyn_KnownAggr(_tmpA9),((_tmpF7C.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmpF7C)))),_tmpF7D)))),_tmpF7B))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmpF80;struct Cyc_Absyn_AnonAggrType_struct*_tmpF7F;return(void*)((_tmpF7F=
_cycalloc(sizeof(*_tmpF7F)),((_tmpF7F[0]=((_tmpF80.tag=13,((_tmpF80.f1=_tmpAC,((
_tmpF80.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpAD),_tmpF80)))))),
_tmpF7F))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmpAE=(struct Cyc_Absyn_EnumType_struct*)
_tmp71;if(_tmpAE->tag != 14)goto _LL41;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}
_LL40: {struct Cyc_Absyn_EnumType_struct _tmpF83;struct Cyc_Absyn_EnumType_struct*
_tmpF82;return(void*)((_tmpF82=_cycalloc(sizeof(*_tmpF82)),((_tmpF82[0]=((
_tmpF83.tag=14,((_tmpF83.f1=_tmpAF,((_tmpF83.f2=_tmpB0,_tmpF83)))))),_tmpF82))));}
_LL41: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp71;if(_tmpB1->tag != 15)goto _LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmpF86;struct Cyc_Absyn_AnonEnumType_struct*_tmpF85;return(void*)((_tmpF85=
_cycalloc(sizeof(*_tmpF85)),((_tmpF85[0]=((_tmpF86.tag=15,((_tmpF86.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmpF86)))),
_tmpF85))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmpF89;struct Cyc_Absyn_TagType_struct*_tmpF88;
return(void*)((_tmpF88=_cycalloc(sizeof(*_tmpF88)),((_tmpF88[0]=((_tmpF89.tag=20,((
_tmpF89.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmpF89)))),_tmpF88))));}_LL45: {
struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmpF8C;struct Cyc_Absyn_ValueofType_struct*_tmpF8B;return(void*)((_tmpF8B=
_cycalloc(sizeof(*_tmpF8B)),((_tmpF8B[0]=((_tmpF8C.tag=19,((_tmpF8C.f1=_tmpB6,
_tmpF8C)))),_tmpF8B))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto _LL49;else{
_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct _tmpF8F;
struct Cyc_Absyn_RgnHandleType_struct*_tmpF8E;return(void*)((_tmpF8E=_cycalloc(
sizeof(*_tmpF8E)),((_tmpF8E[0]=((_tmpF8F.tag=16,((_tmpF8F.f1=(void*)Cyc_Tcutil_copy_type(
_tmpB8),_tmpF8F)))),_tmpF8E))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmpF92;struct Cyc_Absyn_DynRgnType_struct*_tmpF91;return(void*)((_tmpF91=
_cycalloc(sizeof(*_tmpF91)),((_tmpF91[0]=((_tmpF92.tag=17,((_tmpF92.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmpF92.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmpF92)))))),_tmpF91))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmpF95;struct Cyc_Absyn_TypedefType_struct*_tmpF94;return(void*)((_tmpF94=
_cycalloc(sizeof(*_tmpF94)),((_tmpF94[0]=((_tmpF95.tag=18,((_tmpF95.f1=_tmpBD,((
_tmpF95.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmpF95.f3=_tmpBF,((_tmpF95.f4=0,
_tmpF95)))))))))),_tmpF94))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*_tmpC0=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}_LL4E:
goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmpF98;
struct Cyc_Absyn_AccessEff_struct*_tmpF97;return(void*)((_tmpF97=_cycalloc(
sizeof(*_tmpF97)),((_tmpF97[0]=((_tmpF98.tag=23,((_tmpF98.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmpF98)))),_tmpF97))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmpF9B;struct Cyc_Absyn_JoinEff_struct*
_tmpF9A;return(void*)((_tmpF9A=_cycalloc(sizeof(*_tmpF9A)),((_tmpF9A[0]=((
_tmpF9B.tag=24,((_tmpF9B.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmpF9B)))),_tmpF9A))));}
_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmpF9E;struct Cyc_Absyn_RgnsEff_struct*_tmpF9D;
return(void*)((_tmpF9D=_cycalloc(sizeof(*_tmpF9D)),((_tmpF9D[0]=((_tmpF9E.tag=25,((
_tmpF9E.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmpF9E)))),_tmpF9D))));}_LL20:;}
struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct
_tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2);int Cyc_Tcutil_kind_leq(struct
Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp108;enum 
Cyc_Absyn_KindQual _tmp109;enum Cyc_Absyn_AliasQual _tmp10A;struct Cyc_Absyn_Kind*
_tmp107=ka1;_tmp108=*_tmp107;_tmp109=_tmp108.kind;_tmp10A=_tmp108.aliasqual;{
struct Cyc_Absyn_Kind _tmp10C;enum Cyc_Absyn_KindQual _tmp10D;enum Cyc_Absyn_AliasQual
_tmp10E;struct Cyc_Absyn_Kind*_tmp10B=ka2;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.kind;
_tmp10E=_tmp10C.aliasqual;if(_tmp109 != _tmp10D){struct _tuple17 _tmpF9F;struct
_tuple17 _tmp110=(_tmpF9F.f1=_tmp109,((_tmpF9F.f2=_tmp10D,_tmpF9F)));enum Cyc_Absyn_KindQual
_tmp111;enum Cyc_Absyn_KindQual _tmp112;enum Cyc_Absyn_KindQual _tmp113;enum Cyc_Absyn_KindQual
_tmp114;enum Cyc_Absyn_KindQual _tmp115;enum Cyc_Absyn_KindQual _tmp116;_LL58:
_tmp111=_tmp110.f1;if(_tmp111 != Cyc_Absyn_BoxKind)goto _LL5A;_tmp112=_tmp110.f2;
if(_tmp112 != Cyc_Absyn_MemKind)goto _LL5A;_LL59: goto _LL5B;_LL5A: _tmp113=_tmp110.f1;
if(_tmp113 != Cyc_Absyn_BoxKind)goto _LL5C;_tmp114=_tmp110.f2;if(_tmp114 != Cyc_Absyn_AnyKind)
goto _LL5C;_LL5B: goto _LL5D;_LL5C: _tmp115=_tmp110.f1;if(_tmp115 != Cyc_Absyn_MemKind)
goto _LL5E;_tmp116=_tmp110.f2;if(_tmp116 != Cyc_Absyn_AnyKind)goto _LL5E;_LL5D: goto
_LL57;_LL5E:;_LL5F: return 0;_LL57:;}if(_tmp10A != _tmp10E){struct _tuple18 _tmpFA0;
struct _tuple18 _tmp118=(_tmpFA0.f1=_tmp10A,((_tmpFA0.f2=_tmp10E,_tmpFA0)));enum 
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
_LL6C:;_LL6D:{struct Cyc_Absyn_Less_kb_struct _tmpFA3;struct Cyc_Absyn_Less_kb_struct*
_tmpFA2;tv->kind=(void*)((_tmpFA2=_cycalloc(sizeof(*_tmpFA2)),((_tmpFA2[0]=((
_tmpFA3.tag=2,((_tmpFA3.f1=0,((_tmpFA3.f2=def,_tmpFA3)))))),_tmpFA2))));}return
def;_LL67:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2){struct _tuple0 _tmpFA4;struct _tuple0 _tmp125=(_tmpFA4.f1=Cyc_Absyn_compress_kb(
kb1),((_tmpFA4.f2=Cyc_Absyn_compress_kb(kb2),_tmpFA4)));void*_tmp126;struct Cyc_Absyn_Kind*
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
_tmpFA5;*_tmp12F=(struct Cyc_Core_Opt*)((_tmpFA5=_cycalloc(sizeof(*_tmpFA5)),((
_tmpFA5[0]=kb2,_tmpFA5))));}return 1;}else{return 0;}_LL73: _tmp134=_tmp125.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp135=(struct Cyc_Absyn_Eq_kb_struct*)_tmp134;if(
_tmp135->tag != 0)goto _LL75;else{_tmp136=_tmp135->f1;}};_tmp137=_tmp125.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp138=(struct Cyc_Absyn_Less_kb_struct*)_tmp137;
if(_tmp138->tag != 2)goto _LL75;else{_tmp139=_tmp138->f1;_tmp13A=(struct Cyc_Core_Opt**)&
_tmp138->f1;_tmp13B=_tmp138->f2;}};_LL74: if(Cyc_Tcutil_kind_leq(_tmp136,_tmp13B)){{
void**_tmpFA6;*_tmp13A=(struct Cyc_Core_Opt*)((_tmpFA6=_cycalloc(sizeof(*_tmpFA6)),((
_tmpFA6[0]=kb1,_tmpFA6))));}return 1;}else{return 0;}_LL75: _tmp13C=_tmp125.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp13D=(struct Cyc_Absyn_Less_kb_struct*)_tmp13C;
if(_tmp13D->tag != 2)goto _LL77;else{_tmp13E=_tmp13D->f1;_tmp13F=(struct Cyc_Core_Opt**)&
_tmp13D->f1;_tmp140=_tmp13D->f2;}};_tmp141=_tmp125.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp142=(struct Cyc_Absyn_Less_kb_struct*)_tmp141;if(_tmp142->tag != 2)goto _LL77;
else{_tmp143=_tmp142->f1;_tmp144=(struct Cyc_Core_Opt**)& _tmp142->f1;_tmp145=
_tmp142->f2;}};_LL76: if(Cyc_Tcutil_kind_leq(_tmp140,_tmp145)){{void**_tmpFA7;*
_tmp144=(struct Cyc_Core_Opt*)((_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((_tmpFA7[0]=
kb1,_tmpFA7))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp145,_tmp140)){{void**
_tmpFA8;*_tmp13F=(struct Cyc_Core_Opt*)((_tmpFA8=_cycalloc(sizeof(*_tmpFA8)),((
_tmpFA8[0]=kb2,_tmpFA8))));}return 1;}else{return 0;}}_LL77: _tmp146=_tmp125.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp147=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp146;if(_tmp147->tag != 1)goto _LL79;else{_tmp148=_tmp147->f1;_tmp149=(struct
Cyc_Core_Opt**)& _tmp147->f1;}};_tmp14A=_tmp125.f2;_LL78: _tmp14B=_tmp14A;_tmp14F=
_tmp149;goto _LL7A;_LL79: _tmp14B=_tmp125.f1;_tmp14C=_tmp125.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp14D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp14C;if(_tmp14D->tag != 1)goto
_LL6E;else{_tmp14E=_tmp14D->f1;_tmp14F=(struct Cyc_Core_Opt**)& _tmp14D->f1;}};
_LL7A:{void**_tmpFA9;*_tmp14F=(struct Cyc_Core_Opt*)((_tmpFA9=_cycalloc(sizeof(*
_tmpFA9)),((_tmpFA9[0]=_tmp14B,_tmpFA9))));}return 1;_LL6E:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp155=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp157;_LL7C: {struct Cyc_Absyn_VarType_struct*
_tmp156=(struct Cyc_Absyn_VarType_struct*)_tmp155;if(_tmp156->tag != 2)goto _LL7E;
else{_tmp157=_tmp156->f1;}}_LL7D: {void*_tmp158=_tmp157->kind;_tmp157->kind=kb;{
struct _tuple14 _tmpFAA;return(_tmpFAA.f1=_tmp157,((_tmpFAA.f2=_tmp158,_tmpFAA)));};}
_LL7E:;_LL7F: {const char*_tmpFAE;void*_tmpFAD[1];struct Cyc_String_pa_struct
_tmpFAC;(_tmpFAC.tag=0,((_tmpFAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpFAD[0]=& _tmpFAC,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFAE="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpFAE,sizeof(char),40))),_tag_dyneither(_tmpFAD,sizeof(void*),1)))))));}
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
_LLA0: {const char*_tmpFB1;void*_tmpFB0;(_tmpFB0=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB1="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpFB1,sizeof(char),39))),_tag_dyneither(_tmpFB0,sizeof(void*),0)));}
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
_LLBC;}_LLBB: {struct Cyc_Absyn_Kind*_tmpFB2;return(_tmpFB2=_cycalloc_atomic(
sizeof(*_tmpFB2)),((_tmpFB2->kind=Cyc_Absyn_MemKind,((_tmpFB2->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp186.ptr_atts).rgn))->aliasqual,_tmpFB2)))));}_LLBC: {struct Cyc_Absyn_Upper_b_struct*
_tmp194=(struct Cyc_Absyn_Upper_b_struct*)_tmp192;if(_tmp194->tag != 1)goto _LLB9;}
_LLBD: {struct Cyc_Absyn_Kind*_tmpFB3;return(_tmpFB3=_cycalloc_atomic(sizeof(*
_tmpFB3)),((_tmpFB3->kind=Cyc_Absyn_BoxKind,((_tmpFB3->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp186.ptr_atts).rgn))->aliasqual,_tmpFB3)))));}_LLB9:;}_LLA9: {struct Cyc_Absyn_ValueofType_struct*
_tmp187=(struct Cyc_Absyn_ValueofType_struct*)_tmp15D;if(_tmp187->tag != 19)goto
_LLAB;}_LLAA: return& Cyc_Tcutil_ik;_LLAB: {struct Cyc_Absyn_TagType_struct*_tmp188=(
struct Cyc_Absyn_TagType_struct*)_tmp15D;if(_tmp188->tag != 20)goto _LLAD;}_LLAC:
return& Cyc_Tcutil_bk;_LLAD: {struct Cyc_Absyn_ArrayType_struct*_tmp189=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15D;if(_tmp189->tag != 9)goto _LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_TupleType_struct*
_tmp18A=(struct Cyc_Absyn_TupleType_struct*)_tmp15D;if(_tmp18A->tag != 11)goto
_LLB1;}_LLB0: return& Cyc_Tcutil_mk;_LLB1: {struct Cyc_Absyn_TypedefType_struct*
_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)_tmp15D;if(_tmp18B->tag != 18)goto
_LLB3;else{_tmp18C=_tmp18B->f3;}}_LLB2: if(_tmp18C == 0  || _tmp18C->kind == 0){
const char*_tmpFB7;void*_tmpFB6[1];struct Cyc_String_pa_struct _tmpFB5;(_tmpFB5.tag=
0,((_tmpFB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpFB6[0]=& _tmpFB5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpFB7="typ_kind: typedef found: %s",_tag_dyneither(
_tmpFB7,sizeof(char),28))),_tag_dyneither(_tmpFB6,sizeof(void*),1)))))));}return(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp18C->kind))->v;_LLB3: {
struct Cyc_Absyn_AccessEff_struct*_tmp18D=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15D;if(_tmp18D->tag != 23)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_JoinEff_struct*
_tmp18E=(struct Cyc_Absyn_JoinEff_struct*)_tmp15D;if(_tmp18E->tag != 24)goto _LLB7;}
_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_RgnsEff_struct*_tmp18F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15D;if(_tmp18F->tag != 25)goto _LL80;}_LLB8: return& Cyc_Tcutil_ek;_LL80:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);int Cyc_Tcutil_kind_eq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == k2  || k1->kind == k2->kind
 && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=
0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp19D=1;_npop_handler(0);return _tmp19D;};;_pop_handler();}else{void*_tmp19B=(
void*)_exn_thrown;void*_tmp19F=_tmp19B;_LLBF: {struct Cyc_Tcutil_Unify_struct*
_tmp1A0=(struct Cyc_Tcutil_Unify_struct*)_tmp19F;if(_tmp1A0->tag != Cyc_Tcutil_Unify)
goto _LLC1;}_LLC0: return 0;_LLC1:;_LLC2:(void)_throw(_tmp19F);_LLBE:;}};}static
void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp1A1=t;struct Cyc_Absyn_Tvar*_tmp1A3;struct Cyc_Core_Opt*_tmp1A5;
struct Cyc_Core_Opt*_tmp1A6;struct Cyc_Core_Opt**_tmp1A7;struct Cyc_Absyn_PtrInfo
_tmp1A9;struct Cyc_Absyn_ArrayInfo _tmp1AB;void*_tmp1AC;struct Cyc_Absyn_FnInfo
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Core_Opt*_tmp1B0;void*_tmp1B1;
struct Cyc_List_List*_tmp1B2;int _tmp1B3;struct Cyc_Absyn_VarargInfo*_tmp1B4;struct
Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B8;
struct Cyc_Absyn_DatatypeInfo _tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*
_tmp1BD;struct Cyc_Absyn_DatatypeFieldInfo _tmp1BF;struct Cyc_List_List*_tmp1C0;
struct Cyc_Absyn_AggrInfo _tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*
_tmp1C5;void*_tmp1C7;void*_tmp1C9;void*_tmp1CB;void*_tmp1CD;struct Cyc_List_List*
_tmp1CF;_LLC4: {struct Cyc_Absyn_VarType_struct*_tmp1A2=(struct Cyc_Absyn_VarType_struct*)
_tmp1A1;if(_tmp1A2->tag != 2)goto _LLC6;else{_tmp1A3=_tmp1A2->f1;}}_LLC5: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1A3)){{
const char*_tmpFB8;Cyc_Tcutil_failure_reason=((_tmpFB8="(type variable would escape scope)",
_tag_dyneither(_tmpFB8,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LLC3;_LLC6: {struct Cyc_Absyn_Evar_struct*_tmp1A4=(struct Cyc_Absyn_Evar_struct*)
_tmp1A1;if(_tmp1A4->tag != 1)goto _LLC8;else{_tmp1A5=_tmp1A4->f2;_tmp1A6=_tmp1A4->f4;
_tmp1A7=(struct Cyc_Core_Opt**)& _tmp1A4->f4;}}_LLC7: if(t == evar){{const char*
_tmpFB9;Cyc_Tcutil_failure_reason=((_tmpFB9="(occurs check)",_tag_dyneither(
_tmpFB9,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp1A5 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1A5->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp1A7))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp1D2=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp1A7))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmpFBA;_tmp1D2=((_tmpFBA=_cycalloc(sizeof(*_tmpFBA)),((
_tmpFBA->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpFBA->tl=_tmp1D2,_tmpFBA))))));}}}{
struct Cyc_Core_Opt*_tmpFBB;*_tmp1A7=((_tmpFBB=_cycalloc(sizeof(*_tmpFBB)),((
_tmpFBB->v=_tmp1D2,_tmpFBB))));};}}}goto _LLC3;_LLC8: {struct Cyc_Absyn_PointerType_struct*
_tmp1A8=(struct Cyc_Absyn_PointerType_struct*)_tmp1A1;if(_tmp1A8->tag != 5)goto
_LLCA;else{_tmp1A9=_tmp1A8->f1;}}_LLC9: Cyc_Tcutil_occurs(evar,r,env,_tmp1A9.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1A9.ptr_atts).rgn);goto _LLC3;_LLCA: {struct Cyc_Absyn_ArrayType_struct*
_tmp1AA=(struct Cyc_Absyn_ArrayType_struct*)_tmp1A1;if(_tmp1AA->tag != 9)goto _LLCC;
else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AB.elt_type;}}_LLCB: Cyc_Tcutil_occurs(evar,
r,env,_tmp1AC);goto _LLC3;_LLCC: {struct Cyc_Absyn_FnType_struct*_tmp1AD=(struct
Cyc_Absyn_FnType_struct*)_tmp1A1;if(_tmp1AD->tag != 10)goto _LLCE;else{_tmp1AE=
_tmp1AD->f1;_tmp1AF=_tmp1AE.tvars;_tmp1B0=_tmp1AE.effect;_tmp1B1=_tmp1AE.ret_typ;
_tmp1B2=_tmp1AE.args;_tmp1B3=_tmp1AE.c_varargs;_tmp1B4=_tmp1AE.cyc_varargs;
_tmp1B5=_tmp1AE.rgn_po;_tmp1B6=_tmp1AE.attributes;}}_LLCD: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp1AF,env);if(_tmp1B0 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1B0->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp1B1);for(0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp1B2->hd)).f3);}if(_tmp1B4 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp1B4->type);for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){struct _tuple0
_tmp1D6;void*_tmp1D7;void*_tmp1D8;struct _tuple0*_tmp1D5=(struct _tuple0*)_tmp1B5->hd;
_tmp1D6=*_tmp1D5;_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp1D7);Cyc_Tcutil_occurs(evar,r,env,_tmp1D8);}goto _LLC3;_LLCE: {struct Cyc_Absyn_TupleType_struct*
_tmp1B7=(struct Cyc_Absyn_TupleType_struct*)_tmp1A1;if(_tmp1B7->tag != 11)goto
_LLD0;else{_tmp1B8=_tmp1B7->f1;}}_LLCF: for(0;_tmp1B8 != 0;_tmp1B8=_tmp1B8->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1B8->hd)).f2);}goto _LLC3;
_LLD0: {struct Cyc_Absyn_DatatypeType_struct*_tmp1B9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp1A1;if(_tmp1B9->tag != 3)goto _LLD2;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1BA.targs;}}
_LLD1: Cyc_Tcutil_occurslist(evar,r,env,_tmp1BB);goto _LLC3;_LLD2: {struct Cyc_Absyn_TypedefType_struct*
_tmp1BC=(struct Cyc_Absyn_TypedefType_struct*)_tmp1A1;if(_tmp1BC->tag != 18)goto
_LLD4;else{_tmp1BD=_tmp1BC->f2;}}_LLD3: _tmp1C0=_tmp1BD;goto _LLD5;_LLD4: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp1BE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1A1;if(_tmp1BE->tag != 4)goto _LLD6;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BF.targs;}}
_LLD5: _tmp1C3=_tmp1C0;goto _LLD7;_LLD6: {struct Cyc_Absyn_AggrType_struct*_tmp1C1=(
struct Cyc_Absyn_AggrType_struct*)_tmp1A1;if(_tmp1C1->tag != 12)goto _LLD8;else{
_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C2.targs;}}_LLD7: Cyc_Tcutil_occurslist(evar,r,
env,_tmp1C3);goto _LLC3;_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1C4=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1A1;if(_tmp1C4->tag != 13)goto _LLDA;
else{_tmp1C5=_tmp1C4->f2;}}_LLD9: for(0;_tmp1C5 != 0;_tmp1C5=_tmp1C5->tl){Cyc_Tcutil_occurs(
evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1C5->hd)->type);}goto _LLC3;_LLDA: {
struct Cyc_Absyn_TagType_struct*_tmp1C6=(struct Cyc_Absyn_TagType_struct*)_tmp1A1;
if(_tmp1C6->tag != 20)goto _LLDC;else{_tmp1C7=(void*)_tmp1C6->f1;}}_LLDB: _tmp1C9=
_tmp1C7;goto _LLDD;_LLDC: {struct Cyc_Absyn_AccessEff_struct*_tmp1C8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp1A1;if(_tmp1C8->tag != 23)goto _LLDE;else{_tmp1C9=(void*)_tmp1C8->f1;}}_LLDD:
_tmp1CB=_tmp1C9;goto _LLDF;_LLDE: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1CA=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1A1;if(_tmp1CA->tag != 16)goto _LLE0;
else{_tmp1CB=(void*)_tmp1CA->f1;}}_LLDF: _tmp1CD=_tmp1CB;goto _LLE1;_LLE0: {struct
Cyc_Absyn_RgnsEff_struct*_tmp1CC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp1A1;if(
_tmp1CC->tag != 25)goto _LLE2;else{_tmp1CD=(void*)_tmp1CC->f1;}}_LLE1: Cyc_Tcutil_occurs(
evar,r,env,_tmp1CD);goto _LLC3;_LLE2: {struct Cyc_Absyn_JoinEff_struct*_tmp1CE=(
struct Cyc_Absyn_JoinEff_struct*)_tmp1A1;if(_tmp1CE->tag != 24)goto _LLE4;else{
_tmp1CF=_tmp1CE->f1;}}_LLE3: Cyc_Tcutil_occurslist(evar,r,env,_tmp1CF);goto _LLC3;
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
char*_tmpFBE;void*_tmpFBD;(_tmpFBD=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBE="tq1 real_const not set.",
_tag_dyneither(_tmpFBE,sizeof(char),24))),_tag_dyneither(_tmpFBD,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpFC1;void*_tmpFC0;(_tmpFC0=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFC1="tq2 real_const not set.",_tag_dyneither(_tmpFC1,sizeof(char),24))),
_tag_dyneither(_tmpFC0,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpFC2;Cyc_Tcutil_failure_reason=((_tmpFC2="(qualifiers don't match)",
_tag_dyneither(_tmpFC2,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp1DE=x;union Cyc_Absyn_Constraint
_tmp1DF;int _tmp1E0;union Cyc_Absyn_Constraint _tmp1E1;void*_tmp1E2;union Cyc_Absyn_Constraint
_tmp1E3;union Cyc_Absyn_Constraint*_tmp1E4;_LLE7: _tmp1DF=*_tmp1DE;if((_tmp1DF.No_constr).tag
!= 3)goto _LLE9;_tmp1E0=(int)(_tmp1DF.No_constr).val;_LLE8:{union Cyc_Absyn_Constraint
_tmpFC3;*x=(((_tmpFC3.Forward_constr).val=y,(((_tmpFC3.Forward_constr).tag=2,
_tmpFC3))));}return;_LLE9: _tmp1E1=*_tmp1DE;if((_tmp1E1.Eq_constr).tag != 1)goto
_LLEB;_tmp1E2=(void*)(_tmp1E1.Eq_constr).val;_LLEA: {union Cyc_Absyn_Constraint*
_tmp1E6=y;union Cyc_Absyn_Constraint _tmp1E7;int _tmp1E8;union Cyc_Absyn_Constraint
_tmp1E9;void*_tmp1EA;union Cyc_Absyn_Constraint _tmp1EB;union Cyc_Absyn_Constraint*
_tmp1EC;_LLEE: _tmp1E7=*_tmp1E6;if((_tmp1E7.No_constr).tag != 3)goto _LLF0;_tmp1E8=(
int)(_tmp1E7.No_constr).val;_LLEF:*y=*x;return;_LLF0: _tmp1E9=*_tmp1E6;if((
_tmp1E9.Eq_constr).tag != 1)goto _LLF2;_tmp1EA=(void*)(_tmp1E9.Eq_constr).val;
_LLF1: if(cmp(_tmp1E2,_tmp1EA)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LLF2: _tmp1EB=*_tmp1E6;if((_tmp1EB.Forward_constr).tag
!= 2)goto _LLED;_tmp1EC=(union Cyc_Absyn_Constraint*)(_tmp1EB.Forward_constr).val;
_LLF3: {const char*_tmpFC6;void*_tmpFC5;(_tmpFC5=0,Cyc_Tcutil_impos(((_tmpFC6="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpFC6,sizeof(char),40))),_tag_dyneither(_tmpFC5,sizeof(void*),0)));}
_LLED:;}_LLEB: _tmp1E3=*_tmp1DE;if((_tmp1E3.Forward_constr).tag != 2)goto _LLE6;
_tmp1E4=(union Cyc_Absyn_Constraint*)(_tmp1E3.Forward_constr).val;_LLEC: {const
char*_tmpFC9;void*_tmpFC8;(_tmpFC8=0,Cyc_Tcutil_impos(((_tmpFC9="unify_conref: forward after compress",
_tag_dyneither(_tmpFC9,sizeof(char),37))),_tag_dyneither(_tmpFC8,sizeof(void*),0)));}
_LLE6:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp1F1;_push_handler(& _tmp1F1);{int _tmp1F3=0;if(setjmp(_tmp1F1.handler))
_tmp1F3=1;if(!_tmp1F3){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp1F4=1;_npop_handler(0);return _tmp1F4;};;
_pop_handler();}else{void*_tmp1F2=(void*)_exn_thrown;void*_tmp1F6=_tmp1F2;_LLF5: {
struct Cyc_Tcutil_Unify_struct*_tmp1F7=(struct Cyc_Tcutil_Unify_struct*)_tmp1F6;
if(_tmp1F7->tag != Cyc_Tcutil_Unify)goto _LLF7;}_LLF6: return 0;_LLF7:;_LLF8:(void)
_throw(_tmp1F6);_LLF4:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static
int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmpFCA;struct _tuple0
_tmp1F9=(_tmpFCA.f1=b1,((_tmpFCA.f2=b2,_tmpFCA)));void*_tmp1FA;void*_tmp1FC;void*
_tmp1FE;void*_tmp200;void*_tmp202;struct Cyc_Absyn_Exp*_tmp204;void*_tmp205;
struct Cyc_Absyn_Exp*_tmp207;_LLFA: _tmp1FA=_tmp1F9.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1FB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1FA;if(_tmp1FB->tag != 0)goto
_LLFC;};_tmp1FC=_tmp1F9.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FD=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp1FC;if(_tmp1FD->tag != 0)goto _LLFC;};_LLFB:
return 0;_LLFC: _tmp1FE=_tmp1F9.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1FE;if(_tmp1FF->tag != 0)goto _LLFE;};
_LLFD: return - 1;_LLFE: _tmp200=_tmp1F9.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp201=(struct Cyc_Absyn_DynEither_b_struct*)_tmp200;if(_tmp201->tag != 0)goto
_LL100;};_LLFF: return 1;_LL100: _tmp202=_tmp1F9.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp203=(struct Cyc_Absyn_Upper_b_struct*)_tmp202;if(_tmp203->tag != 1)goto _LLF9;
else{_tmp204=_tmp203->f1;}};_tmp205=_tmp1F9.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp206=(struct Cyc_Absyn_Upper_b_struct*)_tmp205;if(_tmp206->tag != 1)goto _LLF9;
else{_tmp207=_tmp206->f1;}};_LL101: return Cyc_Evexp_const_exp_cmp(_tmp204,_tmp207);
_LLF9:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmpFCB;struct _tuple0 _tmp209=(_tmpFCB.f1=b1,((
_tmpFCB.f2=b2,_tmpFCB)));void*_tmp20A;void*_tmp20C;void*_tmp20E;void*_tmp210;
void*_tmp212;struct Cyc_Absyn_Exp*_tmp214;void*_tmp215;struct Cyc_Absyn_Exp*
_tmp217;_LL103: _tmp20A=_tmp209.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp20B=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp20A;if(_tmp20B->tag != 0)goto _LL105;};
_tmp20C=_tmp209.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp20D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp20C;if(_tmp20D->tag != 0)goto _LL105;};_LL104: return 0;_LL105: _tmp20E=_tmp209.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp20F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp20E;if(_tmp20F->tag != 0)goto _LL107;};_LL106: return - 1;_LL107: _tmp210=_tmp209.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp211=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp210;if(_tmp211->tag != 0)goto _LL109;};_LL108: return 1;_LL109: _tmp212=_tmp209.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp213=(struct Cyc_Absyn_Upper_b_struct*)_tmp212;
if(_tmp213->tag != 1)goto _LL102;else{_tmp214=_tmp213->f1;}};_tmp215=_tmp209.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp216=(struct Cyc_Absyn_Upper_b_struct*)_tmp215;
if(_tmp216->tag != 1)goto _LL102;else{_tmp217=_tmp216->f1;}};_LL10A: return Cyc_Evexp_const_exp_cmp(
_tmp214,_tmp217);_LL102:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp218=att;_LL10C: {
struct Cyc_Absyn_Regparm_att_struct*_tmp219=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp218;if(_tmp219->tag != 0)goto _LL10E;}_LL10D: return 0;_LL10E: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp21A=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp218;if(_tmp21A->tag != 1)goto
_LL110;}_LL10F: return 1;_LL110: {struct Cyc_Absyn_Cdecl_att_struct*_tmp21B=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp218;if(_tmp21B->tag != 2)goto _LL112;}_LL111:
return 2;_LL112: {struct Cyc_Absyn_Fastcall_att_struct*_tmp21C=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp218;if(_tmp21C->tag != 3)goto _LL114;}_LL113: return 3;_LL114: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp21D=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp218;if(_tmp21D->tag != 4)goto
_LL116;}_LL115: return 4;_LL116: {struct Cyc_Absyn_Const_att_struct*_tmp21E=(struct
Cyc_Absyn_Const_att_struct*)_tmp218;if(_tmp21E->tag != 5)goto _LL118;}_LL117:
return 5;_LL118: {struct Cyc_Absyn_Aligned_att_struct*_tmp21F=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp218;if(_tmp21F->tag != 6)goto _LL11A;}_LL119: return 6;_LL11A: {struct Cyc_Absyn_Packed_att_struct*
_tmp220=(struct Cyc_Absyn_Packed_att_struct*)_tmp218;if(_tmp220->tag != 7)goto
_LL11C;}_LL11B: return 7;_LL11C: {struct Cyc_Absyn_Section_att_struct*_tmp221=(
struct Cyc_Absyn_Section_att_struct*)_tmp218;if(_tmp221->tag != 8)goto _LL11E;}
_LL11D: return 8;_LL11E: {struct Cyc_Absyn_Nocommon_att_struct*_tmp222=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp218;if(_tmp222->tag != 9)goto _LL120;}_LL11F: return 9;_LL120: {struct Cyc_Absyn_Shared_att_struct*
_tmp223=(struct Cyc_Absyn_Shared_att_struct*)_tmp218;if(_tmp223->tag != 10)goto
_LL122;}_LL121: return 10;_LL122: {struct Cyc_Absyn_Unused_att_struct*_tmp224=(
struct Cyc_Absyn_Unused_att_struct*)_tmp218;if(_tmp224->tag != 11)goto _LL124;}
_LL123: return 11;_LL124: {struct Cyc_Absyn_Weak_att_struct*_tmp225=(struct Cyc_Absyn_Weak_att_struct*)
_tmp218;if(_tmp225->tag != 12)goto _LL126;}_LL125: return 12;_LL126: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp226=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp218;if(_tmp226->tag != 13)goto
_LL128;}_LL127: return 13;_LL128: {struct Cyc_Absyn_Dllexport_att_struct*_tmp227=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp218;if(_tmp227->tag != 14)goto _LL12A;}
_LL129: return 14;_LL12A: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp228=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp218;if(_tmp228->tag
!= 15)goto _LL12C;}_LL12B: return 15;_LL12C: {struct Cyc_Absyn_Constructor_att_struct*
_tmp229=(struct Cyc_Absyn_Constructor_att_struct*)_tmp218;if(_tmp229->tag != 16)
goto _LL12E;}_LL12D: return 16;_LL12E: {struct Cyc_Absyn_Destructor_att_struct*
_tmp22A=(struct Cyc_Absyn_Destructor_att_struct*)_tmp218;if(_tmp22A->tag != 17)
goto _LL130;}_LL12F: return 17;_LL130: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp22B=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp218;if(_tmp22B->tag
!= 18)goto _LL132;}_LL131: return 18;_LL132: {struct Cyc_Absyn_Format_att_struct*
_tmp22C=(struct Cyc_Absyn_Format_att_struct*)_tmp218;if(_tmp22C->tag != 19)goto
_LL134;}_LL133: return 19;_LL134: {struct Cyc_Absyn_Initializes_att_struct*_tmp22D=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp218;if(_tmp22D->tag != 20)goto _LL136;}
_LL135: return 20;_LL136:;_LL137: return 21;_LL10B:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpFCC;struct _tuple0 _tmp22F=(_tmpFCC.f1=att1,((_tmpFCC.f2=att2,
_tmpFCC)));void*_tmp230;int _tmp232;void*_tmp233;int _tmp235;void*_tmp236;int
_tmp238;void*_tmp239;int _tmp23B;void*_tmp23C;int _tmp23E;void*_tmp23F;int _tmp241;
void*_tmp242;struct _dyneither_ptr _tmp244;void*_tmp245;struct _dyneither_ptr
_tmp247;void*_tmp248;enum Cyc_Absyn_Format_Type _tmp24A;int _tmp24B;int _tmp24C;
void*_tmp24D;enum Cyc_Absyn_Format_Type _tmp24F;int _tmp250;int _tmp251;_LL139:
_tmp230=_tmp22F.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp231=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp230;if(_tmp231->tag != 0)goto _LL13B;else{_tmp232=_tmp231->f1;}};_tmp233=
_tmp22F.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp234=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp233;if(_tmp234->tag != 0)goto _LL13B;else{_tmp235=_tmp234->f1;}};_LL13A:
_tmp238=_tmp232;_tmp23B=_tmp235;goto _LL13C;_LL13B: _tmp236=_tmp22F.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp237=(struct Cyc_Absyn_Initializes_att_struct*)_tmp236;if(_tmp237->tag != 20)
goto _LL13D;else{_tmp238=_tmp237->f1;}};_tmp239=_tmp22F.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp23A=(struct Cyc_Absyn_Initializes_att_struct*)_tmp239;if(_tmp23A->tag != 20)
goto _LL13D;else{_tmp23B=_tmp23A->f1;}};_LL13C: _tmp23E=_tmp238;_tmp241=_tmp23B;
goto _LL13E;_LL13D: _tmp23C=_tmp22F.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp23D=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp23C;if(_tmp23D->tag != 6)goto _LL13F;else{
_tmp23E=_tmp23D->f1;}};_tmp23F=_tmp22F.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp240=(struct Cyc_Absyn_Aligned_att_struct*)_tmp23F;if(_tmp240->tag != 6)goto
_LL13F;else{_tmp241=_tmp240->f1;}};_LL13E: return Cyc_Core_intcmp(_tmp23E,_tmp241);
_LL13F: _tmp242=_tmp22F.f1;{struct Cyc_Absyn_Section_att_struct*_tmp243=(struct Cyc_Absyn_Section_att_struct*)
_tmp242;if(_tmp243->tag != 8)goto _LL141;else{_tmp244=_tmp243->f1;}};_tmp245=
_tmp22F.f2;{struct Cyc_Absyn_Section_att_struct*_tmp246=(struct Cyc_Absyn_Section_att_struct*)
_tmp245;if(_tmp246->tag != 8)goto _LL141;else{_tmp247=_tmp246->f1;}};_LL140: return
Cyc_strcmp((struct _dyneither_ptr)_tmp244,(struct _dyneither_ptr)_tmp247);_LL141:
_tmp248=_tmp22F.f1;{struct Cyc_Absyn_Format_att_struct*_tmp249=(struct Cyc_Absyn_Format_att_struct*)
_tmp248;if(_tmp249->tag != 19)goto _LL143;else{_tmp24A=_tmp249->f1;_tmp24B=_tmp249->f2;
_tmp24C=_tmp249->f3;}};_tmp24D=_tmp22F.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp24E=(struct Cyc_Absyn_Format_att_struct*)_tmp24D;if(_tmp24E->tag != 19)goto
_LL143;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;_tmp251=_tmp24E->f3;}};
_LL142: {int _tmp252=Cyc_Core_intcmp((int)((unsigned int)_tmp24A),(int)((
unsigned int)_tmp24F));if(_tmp252 != 0)return _tmp252;{int _tmp253=Cyc_Core_intcmp(
_tmp24B,_tmp250);if(_tmp253 != 0)return _tmp253;return Cyc_Core_intcmp(_tmp24C,
_tmp251);};}_LL143:;_LL144: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
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
_tmp254=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp255;
enum Cyc_Absyn_KindQual _tmp256;enum Cyc_Absyn_AliasQual _tmp257;struct Cyc_Absyn_Kind
_tmp258;enum Cyc_Absyn_KindQual _tmp259;enum Cyc_Absyn_AliasQual _tmp25A;struct Cyc_Absyn_Kind
_tmp25B;enum Cyc_Absyn_KindQual _tmp25C;struct Cyc_Absyn_Kind _tmp25D;enum Cyc_Absyn_KindQual
_tmp25E;_LL146: _tmp255=*_tmp254;_tmp256=_tmp255.kind;if(_tmp256 != Cyc_Absyn_RgnKind)
goto _LL148;_tmp257=_tmp255.aliasqual;if(_tmp257 != Cyc_Absyn_Unique)goto _LL148;
_LL147: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL145;_LL148: _tmp258=*_tmp254;
_tmp259=_tmp258.kind;if(_tmp259 != Cyc_Absyn_RgnKind)goto _LL14A;_tmp25A=_tmp258.aliasqual;
if(_tmp25A != Cyc_Absyn_Aliasable)goto _LL14A;_LL149: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL145;_LL14A: _tmp25B=*_tmp254;_tmp25C=_tmp25B.kind;if(_tmp25C != Cyc_Absyn_EffKind)
goto _LL14C;_LL14B: t=Cyc_Absyn_empty_effect;goto _LL145;_LL14C: _tmp25D=*_tmp254;
_tmp25E=_tmp25D.kind;if(_tmp25E != Cyc_Absyn_IntKind)goto _LL14E;_LL14D:{struct Cyc_Absyn_ValueofType_struct
_tmpFCF;struct Cyc_Absyn_ValueofType_struct*_tmpFCE;t=(void*)((_tmpFCE=_cycalloc(
sizeof(*_tmpFCE)),((_tmpFCE[0]=((_tmpFCF.tag=19,((_tmpFCF.f1=Cyc_Absyn_uint_exp(
0,0),_tmpFCF)))),_tmpFCE))));}goto _LL145;_LL14E:;_LL14F: t=Cyc_Absyn_sint_typ;
goto _LL145;_LL145:;}{struct _tuple14*_tmpFD0;return(_tmpFD0=_cycalloc(sizeof(*
_tmpFD0)),((_tmpFD0->f1=tv,((_tmpFD0->f2=t,_tmpFD0)))));};}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp262=Cyc_Tcutil_compress(t);
void*_tmp26D;void*_tmp26F;void*_tmp270;struct Cyc_Absyn_DatatypeInfo _tmp272;
struct Cyc_List_List*_tmp273;struct Cyc_Absyn_PtrInfo _tmp275;void*_tmp276;struct
Cyc_Absyn_PtrAtts _tmp277;void*_tmp278;struct Cyc_Absyn_ArrayInfo _tmp27A;void*
_tmp27B;struct Cyc_List_List*_tmp27D;struct Cyc_Absyn_DatatypeFieldInfo _tmp27F;
struct Cyc_List_List*_tmp280;struct Cyc_Absyn_AggrInfo _tmp282;struct Cyc_List_List*
_tmp283;struct Cyc_List_List*_tmp285;struct Cyc_Absyn_FnInfo _tmp288;struct Cyc_List_List*
_tmp289;struct Cyc_Core_Opt*_tmp28A;void*_tmp28B;struct Cyc_List_List*_tmp28C;
struct Cyc_Absyn_VarargInfo*_tmp28D;struct Cyc_List_List*_tmp28E;void*_tmp294;
struct Cyc_List_List*_tmp296;_LL151: {struct Cyc_Absyn_VoidType_struct*_tmp263=(
struct Cyc_Absyn_VoidType_struct*)_tmp262;if(_tmp263->tag != 0)goto _LL153;}_LL152:
goto _LL154;_LL153: {struct Cyc_Absyn_FloatType_struct*_tmp264=(struct Cyc_Absyn_FloatType_struct*)
_tmp262;if(_tmp264->tag != 7)goto _LL155;}_LL154: goto _LL156;_LL155: {struct Cyc_Absyn_DoubleType_struct*
_tmp265=(struct Cyc_Absyn_DoubleType_struct*)_tmp262;if(_tmp265->tag != 8)goto
_LL157;}_LL156: goto _LL158;_LL157: {struct Cyc_Absyn_EnumType_struct*_tmp266=(
struct Cyc_Absyn_EnumType_struct*)_tmp262;if(_tmp266->tag != 14)goto _LL159;}_LL158:
goto _LL15A;_LL159: {struct Cyc_Absyn_AnonEnumType_struct*_tmp267=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp262;if(_tmp267->tag != 15)goto _LL15B;}_LL15A: goto _LL15C;_LL15B: {struct Cyc_Absyn_ValueofType_struct*
_tmp268=(struct Cyc_Absyn_ValueofType_struct*)_tmp262;if(_tmp268->tag != 19)goto
_LL15D;}_LL15C: goto _LL15E;_LL15D: {struct Cyc_Absyn_IntType_struct*_tmp269=(
struct Cyc_Absyn_IntType_struct*)_tmp262;if(_tmp269->tag != 6)goto _LL15F;}_LL15E:
return Cyc_Absyn_empty_effect;_LL15F: {struct Cyc_Absyn_Evar_struct*_tmp26A=(
struct Cyc_Absyn_Evar_struct*)_tmp262;if(_tmp26A->tag != 1)goto _LL161;}_LL160: goto
_LL162;_LL161: {struct Cyc_Absyn_VarType_struct*_tmp26B=(struct Cyc_Absyn_VarType_struct*)
_tmp262;if(_tmp26B->tag != 2)goto _LL163;}_LL162: {struct Cyc_Absyn_Kind*_tmp297=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp298;enum Cyc_Absyn_KindQual
_tmp299;struct Cyc_Absyn_Kind _tmp29A;enum Cyc_Absyn_KindQual _tmp29B;struct Cyc_Absyn_Kind
_tmp29C;enum Cyc_Absyn_KindQual _tmp29D;_LL186: _tmp298=*_tmp297;_tmp299=_tmp298.kind;
if(_tmp299 != Cyc_Absyn_RgnKind)goto _LL188;_LL187: {struct Cyc_Absyn_AccessEff_struct
_tmpFD3;struct Cyc_Absyn_AccessEff_struct*_tmpFD2;return(void*)((_tmpFD2=
_cycalloc(sizeof(*_tmpFD2)),((_tmpFD2[0]=((_tmpFD3.tag=23,((_tmpFD3.f1=(void*)t,
_tmpFD3)))),_tmpFD2))));}_LL188: _tmp29A=*_tmp297;_tmp29B=_tmp29A.kind;if(_tmp29B
!= Cyc_Absyn_EffKind)goto _LL18A;_LL189: return t;_LL18A: _tmp29C=*_tmp297;_tmp29D=
_tmp29C.kind;if(_tmp29D != Cyc_Absyn_IntKind)goto _LL18C;_LL18B: return Cyc_Absyn_empty_effect;
_LL18C:;_LL18D: {struct Cyc_Absyn_RgnsEff_struct _tmpFD6;struct Cyc_Absyn_RgnsEff_struct*
_tmpFD5;return(void*)((_tmpFD5=_cycalloc(sizeof(*_tmpFD5)),((_tmpFD5[0]=((
_tmpFD6.tag=25,((_tmpFD6.f1=(void*)t,_tmpFD6)))),_tmpFD5))));}_LL185:;}_LL163: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp26C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp262;if(_tmp26C->tag != 16)goto _LL165;else{_tmp26D=(void*)_tmp26C->f1;}}_LL164: {
struct Cyc_Absyn_AccessEff_struct _tmpFD9;struct Cyc_Absyn_AccessEff_struct*_tmpFD8;
return(void*)((_tmpFD8=_cycalloc(sizeof(*_tmpFD8)),((_tmpFD8[0]=((_tmpFD9.tag=23,((
_tmpFD9.f1=(void*)_tmp26D,_tmpFD9)))),_tmpFD8))));}_LL165: {struct Cyc_Absyn_DynRgnType_struct*
_tmp26E=(struct Cyc_Absyn_DynRgnType_struct*)_tmp262;if(_tmp26E->tag != 17)goto
_LL167;else{_tmp26F=(void*)_tmp26E->f1;_tmp270=(void*)_tmp26E->f2;}}_LL166: {
struct Cyc_Absyn_AccessEff_struct _tmpFDC;struct Cyc_Absyn_AccessEff_struct*_tmpFDB;
return(void*)((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB[0]=((_tmpFDC.tag=23,((
_tmpFDC.f1=(void*)_tmp270,_tmpFDC)))),_tmpFDB))));}_LL167: {struct Cyc_Absyn_DatatypeType_struct*
_tmp271=(struct Cyc_Absyn_DatatypeType_struct*)_tmp262;if(_tmp271->tag != 3)goto
_LL169;else{_tmp272=_tmp271->f1;_tmp273=_tmp272.targs;}}_LL168: {struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp273);struct Cyc_Absyn_JoinEff_struct _tmpFDF;struct Cyc_Absyn_JoinEff_struct*
_tmpFDE;return Cyc_Tcutil_normalize_effect((void*)((_tmpFDE=_cycalloc(sizeof(*
_tmpFDE)),((_tmpFDE[0]=((_tmpFDF.tag=24,((_tmpFDF.f1=ts,_tmpFDF)))),_tmpFDE)))));}
_LL169: {struct Cyc_Absyn_PointerType_struct*_tmp274=(struct Cyc_Absyn_PointerType_struct*)
_tmp262;if(_tmp274->tag != 5)goto _LL16B;else{_tmp275=_tmp274->f1;_tmp276=_tmp275.elt_typ;
_tmp277=_tmp275.ptr_atts;_tmp278=_tmp277.rgn;}}_LL16A: {struct Cyc_Absyn_JoinEff_struct
_tmpFEE;struct Cyc_Absyn_AccessEff_struct*_tmpFED;struct Cyc_Absyn_AccessEff_struct
_tmpFEC;void*_tmpFEB[2];struct Cyc_Absyn_JoinEff_struct*_tmpFEA;return Cyc_Tcutil_normalize_effect((
void*)((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA[0]=((_tmpFEE.tag=24,((
_tmpFEE.f1=((_tmpFEB[1]=Cyc_Tcutil_rgns_of(_tmp276),((_tmpFEB[0]=(void*)((
_tmpFED=_cycalloc(sizeof(*_tmpFED)),((_tmpFED[0]=((_tmpFEC.tag=23,((_tmpFEC.f1=(
void*)_tmp278,_tmpFEC)))),_tmpFED)))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFEB,sizeof(void*),2)))))),
_tmpFEE)))),_tmpFEA)))));}_LL16B: {struct Cyc_Absyn_ArrayType_struct*_tmp279=(
struct Cyc_Absyn_ArrayType_struct*)_tmp262;if(_tmp279->tag != 9)goto _LL16D;else{
_tmp27A=_tmp279->f1;_tmp27B=_tmp27A.elt_type;}}_LL16C: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp27B));_LL16D: {struct Cyc_Absyn_TupleType_struct*_tmp27C=(
struct Cyc_Absyn_TupleType_struct*)_tmp262;if(_tmp27C->tag != 11)goto _LL16F;else{
_tmp27D=_tmp27C->f1;}}_LL16E: {struct Cyc_List_List*_tmp2AD=0;for(0;_tmp27D != 0;
_tmp27D=_tmp27D->tl){struct Cyc_List_List*_tmpFEF;_tmp2AD=((_tmpFEF=_cycalloc(
sizeof(*_tmpFEF)),((_tmpFEF->hd=(*((struct _tuple11*)_tmp27D->hd)).f2,((_tmpFEF->tl=
_tmp2AD,_tmpFEF))))));}_tmp280=_tmp2AD;goto _LL170;}_LL16F: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp27E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp262;if(_tmp27E->tag != 4)
goto _LL171;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27F.targs;}}_LL170: _tmp283=
_tmp280;goto _LL172;_LL171: {struct Cyc_Absyn_AggrType_struct*_tmp281=(struct Cyc_Absyn_AggrType_struct*)
_tmp262;if(_tmp281->tag != 12)goto _LL173;else{_tmp282=_tmp281->f1;_tmp283=_tmp282.targs;}}
_LL172: {struct Cyc_Absyn_JoinEff_struct _tmpFF2;struct Cyc_Absyn_JoinEff_struct*
_tmpFF1;return Cyc_Tcutil_normalize_effect((void*)((_tmpFF1=_cycalloc(sizeof(*
_tmpFF1)),((_tmpFF1[0]=((_tmpFF2.tag=24,((_tmpFF2.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp283),
_tmpFF2)))),_tmpFF1)))));}_LL173: {struct Cyc_Absyn_AnonAggrType_struct*_tmp284=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp262;if(_tmp284->tag != 13)goto _LL175;
else{_tmp285=_tmp284->f2;}}_LL174: {struct Cyc_Absyn_JoinEff_struct _tmpFF5;struct
Cyc_Absyn_JoinEff_struct*_tmpFF4;return Cyc_Tcutil_normalize_effect((void*)((
_tmpFF4=_cycalloc(sizeof(*_tmpFF4)),((_tmpFF4[0]=((_tmpFF5.tag=24,((_tmpFF5.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp285),_tmpFF5)))),_tmpFF4)))));}
_LL175: {struct Cyc_Absyn_TagType_struct*_tmp286=(struct Cyc_Absyn_TagType_struct*)
_tmp262;if(_tmp286->tag != 20)goto _LL177;}_LL176: return Cyc_Absyn_empty_effect;
_LL177: {struct Cyc_Absyn_FnType_struct*_tmp287=(struct Cyc_Absyn_FnType_struct*)
_tmp262;if(_tmp287->tag != 10)goto _LL179;else{_tmp288=_tmp287->f1;_tmp289=_tmp288.tvars;
_tmp28A=_tmp288.effect;_tmp28B=_tmp288.ret_typ;_tmp28C=_tmp288.args;_tmp28D=
_tmp288.cyc_varargs;_tmp28E=_tmp288.rgn_po;}}_LL178: {void*_tmp2B3=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp289),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp28A))->v);return Cyc_Tcutil_normalize_effect(_tmp2B3);}_LL179: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp28F=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp262;if(_tmp28F->tag != 22)goto _LL17B;}_LL17A: goto _LL17C;_LL17B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp290=(struct Cyc_Absyn_HeapRgn_struct*)_tmp262;if(_tmp290->tag != 21)goto _LL17D;}
_LL17C: return Cyc_Absyn_empty_effect;_LL17D: {struct Cyc_Absyn_AccessEff_struct*
_tmp291=(struct Cyc_Absyn_AccessEff_struct*)_tmp262;if(_tmp291->tag != 23)goto
_LL17F;}_LL17E: goto _LL180;_LL17F: {struct Cyc_Absyn_JoinEff_struct*_tmp292=(
struct Cyc_Absyn_JoinEff_struct*)_tmp262;if(_tmp292->tag != 24)goto _LL181;}_LL180:
return t;_LL181: {struct Cyc_Absyn_RgnsEff_struct*_tmp293=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp262;if(_tmp293->tag != 25)goto _LL183;else{_tmp294=(void*)_tmp293->f1;}}_LL182:
return Cyc_Tcutil_rgns_of(_tmp294);_LL183: {struct Cyc_Absyn_TypedefType_struct*
_tmp295=(struct Cyc_Absyn_TypedefType_struct*)_tmp262;if(_tmp295->tag != 18)goto
_LL150;else{_tmp296=_tmp295->f2;}}_LL184: {struct Cyc_Absyn_JoinEff_struct _tmpFF8;
struct Cyc_Absyn_JoinEff_struct*_tmpFF7;return Cyc_Tcutil_normalize_effect((void*)((
_tmpFF7=_cycalloc(sizeof(*_tmpFF7)),((_tmpFF7[0]=((_tmpFF8.tag=24,((_tmpFF8.f1=((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp296),_tmpFF8)))),_tmpFF7)))));}_LL150:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp2B6=e;struct Cyc_List_List*_tmp2B8;struct Cyc_List_List**_tmp2B9;void*_tmp2BB;
_LL18F: {struct Cyc_Absyn_JoinEff_struct*_tmp2B7=(struct Cyc_Absyn_JoinEff_struct*)
_tmp2B6;if(_tmp2B7->tag != 24)goto _LL191;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=(struct
Cyc_List_List**)& _tmp2B7->f1;}}_LL190: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp2B9;for(0;effs != 0;effs=effs->tl){void*_tmp2BC=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp2BC));{void*_tmp2BD=(
void*)effs->hd;void*_tmp2C0;_LL196: {struct Cyc_Absyn_JoinEff_struct*_tmp2BE=(
struct Cyc_Absyn_JoinEff_struct*)_tmp2BD;if(_tmp2BE->tag != 24)goto _LL198;}_LL197:
goto _LL199;_LL198: {struct Cyc_Absyn_AccessEff_struct*_tmp2BF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2BD;if(_tmp2BF->tag != 23)goto _LL19A;else{_tmp2C0=(void*)_tmp2BF->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2C1=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2C0;if(
_tmp2C1->tag != 21)goto _LL19A;};}}_LL199: redo_join=1;goto _LL195;_LL19A:;_LL19B:
goto _LL195;_LL195:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp2B9;for(0;effs != 0;effs=effs->tl){void*_tmp2C2=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp2C4;void*_tmp2C6;_LL19D: {struct Cyc_Absyn_JoinEff_struct*
_tmp2C3=(struct Cyc_Absyn_JoinEff_struct*)_tmp2C2;if(_tmp2C3->tag != 24)goto _LL19F;
else{_tmp2C4=_tmp2C3->f1;}}_LL19E: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp2C4,effects);goto _LL19C;_LL19F: {
struct Cyc_Absyn_AccessEff_struct*_tmp2C5=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2C2;if(_tmp2C5->tag != 23)goto _LL1A1;else{_tmp2C6=(void*)_tmp2C5->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2C7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2C6;if(
_tmp2C7->tag != 21)goto _LL1A1;};}}_LL1A0: goto _LL19C;_LL1A1:;_LL1A2:{struct Cyc_List_List*
_tmpFF9;effects=((_tmpFF9=_cycalloc(sizeof(*_tmpFF9)),((_tmpFF9->hd=_tmp2C2,((
_tmpFF9->tl=effects,_tmpFF9))))));}goto _LL19C;_LL19C:;}}*_tmp2B9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL191: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2BA=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2B6;if(_tmp2BA->tag != 25)goto _LL193;
else{_tmp2BB=(void*)_tmp2BA->f1;}}_LL192: {void*_tmp2C9=Cyc_Tcutil_compress(
_tmp2BB);_LL1A4: {struct Cyc_Absyn_Evar_struct*_tmp2CA=(struct Cyc_Absyn_Evar_struct*)
_tmp2C9;if(_tmp2CA->tag != 1)goto _LL1A6;}_LL1A5: goto _LL1A7;_LL1A6: {struct Cyc_Absyn_VarType_struct*
_tmp2CB=(struct Cyc_Absyn_VarType_struct*)_tmp2C9;if(_tmp2CB->tag != 2)goto _LL1A8;}
_LL1A7: return e;_LL1A8:;_LL1A9: return Cyc_Tcutil_rgns_of(_tmp2BB);_LL1A3:;}_LL193:;
_LL194: return e;_LL18E:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmp1003;
struct Cyc_Core_Opt*_tmp1002;struct Cyc_Absyn_FnInfo _tmp1001;struct Cyc_Absyn_FnType_struct*
_tmp1000;struct Cyc_Absyn_FnType_struct*_tmp2CC=(_tmp1000=_cycalloc(sizeof(*
_tmp1000)),((_tmp1000[0]=((_tmp1003.tag=10,((_tmp1003.f1=((_tmp1001.tvars=0,((
_tmp1001.effect=((_tmp1002=_cycalloc(sizeof(*_tmp1002)),((_tmp1002->v=eff,
_tmp1002)))),((_tmp1001.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1001.args=0,((
_tmp1001.c_varargs=0,((_tmp1001.cyc_varargs=0,((_tmp1001.rgn_po=0,((_tmp1001.attributes=
0,_tmp1001)))))))))))))))),_tmp1003)))),_tmp1000)));return Cyc_Absyn_atb_typ((
void*)_tmp2CC,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val)
return 1;{void*_tmp2D1=Cyc_Tcutil_compress(e);void*_tmp2D3;struct Cyc_List_List*
_tmp2D5;void*_tmp2D7;struct Cyc_Core_Opt*_tmp2D9;struct Cyc_Core_Opt*_tmp2DA;
struct Cyc_Core_Opt**_tmp2DB;struct Cyc_Core_Opt*_tmp2DC;_LL1AB: {struct Cyc_Absyn_AccessEff_struct*
_tmp2D2=(struct Cyc_Absyn_AccessEff_struct*)_tmp2D1;if(_tmp2D2->tag != 23)goto
_LL1AD;else{_tmp2D3=(void*)_tmp2D2->f1;}}_LL1AC: if(constrain)return Cyc_Tcutil_unify(
r,_tmp2D3);_tmp2D3=Cyc_Tcutil_compress(_tmp2D3);if(r == _tmp2D3)return 1;{struct
_tuple0 _tmp1004;struct _tuple0 _tmp2DE=(_tmp1004.f1=r,((_tmp1004.f2=_tmp2D3,
_tmp1004)));void*_tmp2DF;struct Cyc_Absyn_Tvar*_tmp2E1;void*_tmp2E2;struct Cyc_Absyn_Tvar*
_tmp2E4;_LL1B6: _tmp2DF=_tmp2DE.f1;{struct Cyc_Absyn_VarType_struct*_tmp2E0=(
struct Cyc_Absyn_VarType_struct*)_tmp2DF;if(_tmp2E0->tag != 2)goto _LL1B8;else{
_tmp2E1=_tmp2E0->f1;}};_tmp2E2=_tmp2DE.f2;{struct Cyc_Absyn_VarType_struct*
_tmp2E3=(struct Cyc_Absyn_VarType_struct*)_tmp2E2;if(_tmp2E3->tag != 2)goto _LL1B8;
else{_tmp2E4=_tmp2E3->f1;}};_LL1B7: return Cyc_Absyn_tvar_cmp(_tmp2E1,_tmp2E4)== 0;
_LL1B8:;_LL1B9: return 0;_LL1B5:;};_LL1AD: {struct Cyc_Absyn_JoinEff_struct*_tmp2D4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp2D1;if(_tmp2D4->tag != 24)goto _LL1AF;else{
_tmp2D5=_tmp2D4->f1;}}_LL1AE: for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp2D5->hd))return 1;}return 0;_LL1AF: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2D6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2D1;if(_tmp2D6->tag != 25)goto _LL1B1;
else{_tmp2D7=(void*)_tmp2D6->f1;}}_LL1B0: {void*_tmp2E5=Cyc_Tcutil_rgns_of(
_tmp2D7);void*_tmp2E7;_LL1BB: {struct Cyc_Absyn_RgnsEff_struct*_tmp2E6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp2E5;if(_tmp2E6->tag != 25)goto _LL1BD;else{_tmp2E7=(void*)_tmp2E6->f1;}}_LL1BC:
if(!constrain)return 0;{void*_tmp2E8=Cyc_Tcutil_compress(_tmp2E7);struct Cyc_Core_Opt*
_tmp2EA;struct Cyc_Core_Opt*_tmp2EB;struct Cyc_Core_Opt**_tmp2EC;struct Cyc_Core_Opt*
_tmp2ED;_LL1C0: {struct Cyc_Absyn_Evar_struct*_tmp2E9=(struct Cyc_Absyn_Evar_struct*)
_tmp2E8;if(_tmp2E9->tag != 1)goto _LL1C2;else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2E9->f2;
_tmp2EC=(struct Cyc_Core_Opt**)& _tmp2E9->f2;_tmp2ED=_tmp2E9->f4;}}_LL1C1: {void*
_tmp2EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp2ED);Cyc_Tcutil_occurs(
_tmp2EE,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2ED))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1013;struct Cyc_Absyn_AccessEff_struct*
_tmp1012;struct Cyc_Absyn_AccessEff_struct _tmp1011;void*_tmp1010[2];struct Cyc_Absyn_JoinEff_struct*
_tmp100F;void*_tmp2EF=Cyc_Tcutil_dummy_fntype((void*)((_tmp100F=_cycalloc(
sizeof(*_tmp100F)),((_tmp100F[0]=((_tmp1013.tag=24,((_tmp1013.f1=((_tmp1010[1]=(
void*)((_tmp1012=_cycalloc(sizeof(*_tmp1012)),((_tmp1012[0]=((_tmp1011.tag=23,((
_tmp1011.f1=(void*)r,_tmp1011)))),_tmp1012)))),((_tmp1010[0]=_tmp2EE,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1010,sizeof(void*),2)))))),
_tmp1013)))),_tmp100F)))));{struct Cyc_Core_Opt*_tmp1014;*_tmp2EC=((_tmp1014=
_cycalloc(sizeof(*_tmp1014)),((_tmp1014->v=_tmp2EF,_tmp1014))));}return 1;};}
_LL1C2:;_LL1C3: return 0;_LL1BF:;};_LL1BD:;_LL1BE: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp2E5);_LL1BA:;}_LL1B1: {struct Cyc_Absyn_Evar_struct*_tmp2D8=(
struct Cyc_Absyn_Evar_struct*)_tmp2D1;if(_tmp2D8->tag != 1)goto _LL1B3;else{_tmp2D9=
_tmp2D8->f1;_tmp2DA=_tmp2D8->f2;_tmp2DB=(struct Cyc_Core_Opt**)& _tmp2D8->f2;
_tmp2DC=_tmp2D8->f4;}}_LL1B2: if(_tmp2D9 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D9->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp1017;void*_tmp1016;(_tmp1016=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1017="effect evar has wrong kind",
_tag_dyneither(_tmp1017,sizeof(char),27))),_tag_dyneither(_tmp1016,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp2F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp2DC);Cyc_Tcutil_occurs(_tmp2F8,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DC))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp102C;struct Cyc_List_List*_tmp102B;struct Cyc_Absyn_AccessEff_struct _tmp102A;
struct Cyc_Absyn_AccessEff_struct*_tmp1029;struct Cyc_List_List*_tmp1028;struct Cyc_Absyn_JoinEff_struct*
_tmp1027;struct Cyc_Absyn_JoinEff_struct*_tmp2F9=(_tmp1027=_cycalloc(sizeof(*
_tmp1027)),((_tmp1027[0]=((_tmp102C.tag=24,((_tmp102C.f1=((_tmp1028=_cycalloc(
sizeof(*_tmp1028)),((_tmp1028->hd=_tmp2F8,((_tmp1028->tl=((_tmp102B=_cycalloc(
sizeof(*_tmp102B)),((_tmp102B->hd=(void*)((_tmp1029=_cycalloc(sizeof(*_tmp1029)),((
_tmp1029[0]=((_tmp102A.tag=23,((_tmp102A.f1=(void*)r,_tmp102A)))),_tmp1029)))),((
_tmp102B->tl=0,_tmp102B)))))),_tmp1028)))))),_tmp102C)))),_tmp1027)));{struct Cyc_Core_Opt*
_tmp102D;*_tmp2DB=((_tmp102D=_cycalloc(sizeof(*_tmp102D)),((_tmp102D->v=(void*)
_tmp2F9,_tmp102D))));}return 1;};};_LL1B3:;_LL1B4: return 0;_LL1AA:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp301=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp304;void*_tmp306;struct Cyc_Core_Opt*
_tmp308;struct Cyc_Core_Opt*_tmp309;struct Cyc_Core_Opt**_tmp30A;struct Cyc_Core_Opt*
_tmp30B;_LL1C5: {struct Cyc_Absyn_AccessEff_struct*_tmp302=(struct Cyc_Absyn_AccessEff_struct*)
_tmp301;if(_tmp302->tag != 23)goto _LL1C7;}_LL1C6: return 0;_LL1C7: {struct Cyc_Absyn_JoinEff_struct*
_tmp303=(struct Cyc_Absyn_JoinEff_struct*)_tmp301;if(_tmp303->tag != 24)goto _LL1C9;
else{_tmp304=_tmp303->f1;}}_LL1C8: for(0;_tmp304 != 0;_tmp304=_tmp304->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp304->hd))return 1;}return 0;_LL1C9: {struct Cyc_Absyn_RgnsEff_struct*
_tmp305=(struct Cyc_Absyn_RgnsEff_struct*)_tmp301;if(_tmp305->tag != 25)goto _LL1CB;
else{_tmp306=(void*)_tmp305->f1;}}_LL1CA: _tmp306=Cyc_Tcutil_compress(_tmp306);
if(t == _tmp306)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp306);{
void*_tmp30C=Cyc_Tcutil_rgns_of(t);void*_tmp30E;_LL1D0: {struct Cyc_Absyn_RgnsEff_struct*
_tmp30D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp30C;if(_tmp30D->tag != 25)goto _LL1D2;
else{_tmp30E=(void*)_tmp30D->f1;}}_LL1D1: {struct _tuple0 _tmp102E;struct _tuple0
_tmp310=(_tmp102E.f1=Cyc_Tcutil_compress(_tmp30E),((_tmp102E.f2=_tmp306,_tmp102E)));
void*_tmp311;struct Cyc_Absyn_Tvar*_tmp313;void*_tmp314;struct Cyc_Absyn_Tvar*
_tmp316;_LL1D5: _tmp311=_tmp310.f1;{struct Cyc_Absyn_VarType_struct*_tmp312=(
struct Cyc_Absyn_VarType_struct*)_tmp311;if(_tmp312->tag != 2)goto _LL1D7;else{
_tmp313=_tmp312->f1;}};_tmp314=_tmp310.f2;{struct Cyc_Absyn_VarType_struct*
_tmp315=(struct Cyc_Absyn_VarType_struct*)_tmp314;if(_tmp315->tag != 2)goto _LL1D7;
else{_tmp316=_tmp315->f1;}};_LL1D6: return Cyc_Tcutil_unify(t,_tmp306);_LL1D7:;
_LL1D8: return _tmp30E == _tmp306;_LL1D4:;}_LL1D2:;_LL1D3: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp30C);_LL1CF:;};_LL1CB: {struct Cyc_Absyn_Evar_struct*
_tmp307=(struct Cyc_Absyn_Evar_struct*)_tmp301;if(_tmp307->tag != 1)goto _LL1CD;
else{_tmp308=_tmp307->f1;_tmp309=_tmp307->f2;_tmp30A=(struct Cyc_Core_Opt**)&
_tmp307->f2;_tmp30B=_tmp307->f4;}}_LL1CC: if(_tmp308 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp308->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1031;void*_tmp1030;(
_tmp1030=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1031="effect evar has wrong kind",_tag_dyneither(_tmp1031,sizeof(char),27))),
_tag_dyneither(_tmp1030,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp319=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp30B);
Cyc_Tcutil_occurs(_tmp319,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp30B))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1046;struct Cyc_List_List*
_tmp1045;struct Cyc_Absyn_RgnsEff_struct _tmp1044;struct Cyc_Absyn_RgnsEff_struct*
_tmp1043;struct Cyc_List_List*_tmp1042;struct Cyc_Absyn_JoinEff_struct*_tmp1041;
struct Cyc_Absyn_JoinEff_struct*_tmp31A=(_tmp1041=_cycalloc(sizeof(*_tmp1041)),((
_tmp1041[0]=((_tmp1046.tag=24,((_tmp1046.f1=((_tmp1042=_cycalloc(sizeof(*
_tmp1042)),((_tmp1042->hd=_tmp319,((_tmp1042->tl=((_tmp1045=_cycalloc(sizeof(*
_tmp1045)),((_tmp1045->hd=(void*)((_tmp1043=_cycalloc(sizeof(*_tmp1043)),((
_tmp1043[0]=((_tmp1044.tag=25,((_tmp1044.f1=(void*)t,_tmp1044)))),_tmp1043)))),((
_tmp1045->tl=0,_tmp1045)))))),_tmp1042)))))),_tmp1046)))),_tmp1041)));{struct Cyc_Core_Opt*
_tmp1047;*_tmp30A=((_tmp1047=_cycalloc(sizeof(*_tmp1047)),((_tmp1047->v=(void*)
_tmp31A,_tmp1047))));}return 1;};};_LL1CD:;_LL1CE: return 0;_LL1C4:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp322=e;struct Cyc_Absyn_Tvar*_tmp324;struct Cyc_List_List*_tmp326;void*
_tmp328;struct Cyc_Core_Opt*_tmp32A;struct Cyc_Core_Opt*_tmp32B;struct Cyc_Core_Opt**
_tmp32C;struct Cyc_Core_Opt*_tmp32D;_LL1DA: {struct Cyc_Absyn_VarType_struct*
_tmp323=(struct Cyc_Absyn_VarType_struct*)_tmp322;if(_tmp323->tag != 2)goto _LL1DC;
else{_tmp324=_tmp323->f1;}}_LL1DB: return Cyc_Absyn_tvar_cmp(v,_tmp324)== 0;_LL1DC: {
struct Cyc_Absyn_JoinEff_struct*_tmp325=(struct Cyc_Absyn_JoinEff_struct*)_tmp322;
if(_tmp325->tag != 24)goto _LL1DE;else{_tmp326=_tmp325->f1;}}_LL1DD: for(0;_tmp326
!= 0;_tmp326=_tmp326->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp326->hd))return 1;}return 0;_LL1DE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp327=(struct Cyc_Absyn_RgnsEff_struct*)_tmp322;if(_tmp327->tag != 25)goto _LL1E0;
else{_tmp328=(void*)_tmp327->f1;}}_LL1DF: {void*_tmp32E=Cyc_Tcutil_rgns_of(
_tmp328);void*_tmp330;_LL1E5: {struct Cyc_Absyn_RgnsEff_struct*_tmp32F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp32E;if(_tmp32F->tag != 25)goto _LL1E7;else{_tmp330=(void*)_tmp32F->f1;}}_LL1E6:
if(!may_constrain_evars)return 0;{void*_tmp331=Cyc_Tcutil_compress(_tmp330);
struct Cyc_Core_Opt*_tmp333;struct Cyc_Core_Opt*_tmp334;struct Cyc_Core_Opt**
_tmp335;struct Cyc_Core_Opt*_tmp336;_LL1EA: {struct Cyc_Absyn_Evar_struct*_tmp332=(
struct Cyc_Absyn_Evar_struct*)_tmp331;if(_tmp332->tag != 1)goto _LL1EC;else{_tmp333=
_tmp332->f1;_tmp334=_tmp332->f2;_tmp335=(struct Cyc_Core_Opt**)& _tmp332->f2;
_tmp336=_tmp332->f4;}}_LL1EB: {void*_tmp337=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp336);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp336))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1056;struct Cyc_Absyn_VarType_struct*_tmp1055;
struct Cyc_Absyn_VarType_struct _tmp1054;void*_tmp1053[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1052;void*_tmp338=Cyc_Tcutil_dummy_fntype((void*)((_tmp1052=_cycalloc(
sizeof(*_tmp1052)),((_tmp1052[0]=((_tmp1056.tag=24,((_tmp1056.f1=((_tmp1053[1]=(
void*)((_tmp1055=_cycalloc(sizeof(*_tmp1055)),((_tmp1055[0]=((_tmp1054.tag=2,((
_tmp1054.f1=v,_tmp1054)))),_tmp1055)))),((_tmp1053[0]=_tmp337,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1053,sizeof(void*),2)))))),
_tmp1056)))),_tmp1052)))));{struct Cyc_Core_Opt*_tmp1057;*_tmp335=((_tmp1057=
_cycalloc(sizeof(*_tmp1057)),((_tmp1057->v=_tmp338,_tmp1057))));}return 1;};}
_LL1EC:;_LL1ED: return 0;_LL1E9:;};_LL1E7:;_LL1E8: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp32E);_LL1E4:;}_LL1E0: {struct Cyc_Absyn_Evar_struct*
_tmp329=(struct Cyc_Absyn_Evar_struct*)_tmp322;if(_tmp329->tag != 1)goto _LL1E2;
else{_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;_tmp32C=(struct Cyc_Core_Opt**)&
_tmp329->f2;_tmp32D=_tmp329->f4;}}_LL1E1: if(_tmp32A == 0  || ((struct Cyc_Absyn_Kind*)
_tmp32A->v)->kind != Cyc_Absyn_EffKind){const char*_tmp105A;void*_tmp1059;(
_tmp1059=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp105A="effect evar has wrong kind",_tag_dyneither(_tmp105A,sizeof(char),27))),
_tag_dyneither(_tmp1059,sizeof(void*),0)));}{void*_tmp341=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp32D);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp32D))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp106F;
struct Cyc_List_List*_tmp106E;struct Cyc_Absyn_VarType_struct _tmp106D;struct Cyc_Absyn_VarType_struct*
_tmp106C;struct Cyc_List_List*_tmp106B;struct Cyc_Absyn_JoinEff_struct*_tmp106A;
struct Cyc_Absyn_JoinEff_struct*_tmp342=(_tmp106A=_cycalloc(sizeof(*_tmp106A)),((
_tmp106A[0]=((_tmp106F.tag=24,((_tmp106F.f1=((_tmp106B=_cycalloc(sizeof(*
_tmp106B)),((_tmp106B->hd=_tmp341,((_tmp106B->tl=((_tmp106E=_cycalloc(sizeof(*
_tmp106E)),((_tmp106E->hd=(void*)((_tmp106C=_cycalloc(sizeof(*_tmp106C)),((
_tmp106C[0]=((_tmp106D.tag=2,((_tmp106D.f1=v,_tmp106D)))),_tmp106C)))),((
_tmp106E->tl=0,_tmp106E)))))),_tmp106B)))))),_tmp106F)))),_tmp106A)));{struct Cyc_Core_Opt*
_tmp1070;*_tmp32C=((_tmp1070=_cycalloc(sizeof(*_tmp1070)),((_tmp1070->v=(void*)
_tmp342,_tmp1070))));}return 1;};};_LL1E2:;_LL1E3: return 0;_LL1D9:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp34A=e;struct Cyc_List_List*_tmp34C;void*_tmp34E;_LL1EF: {struct Cyc_Absyn_JoinEff_struct*
_tmp34B=(struct Cyc_Absyn_JoinEff_struct*)_tmp34A;if(_tmp34B->tag != 24)goto _LL1F1;
else{_tmp34C=_tmp34B->f1;}}_LL1F0: for(0;_tmp34C != 0;_tmp34C=_tmp34C->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp34C->hd))return 1;}return 0;_LL1F1: {struct Cyc_Absyn_RgnsEff_struct*
_tmp34D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp34A;if(_tmp34D->tag != 25)goto _LL1F3;
else{_tmp34E=(void*)_tmp34D->f1;}}_LL1F2: {void*_tmp350=Cyc_Tcutil_rgns_of(
_tmp34E);void*_tmp352;_LL1F8: {struct Cyc_Absyn_RgnsEff_struct*_tmp351=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp350;if(_tmp351->tag != 25)goto _LL1FA;else{_tmp352=(void*)_tmp351->f1;}}_LL1F9:
return 0;_LL1FA:;_LL1FB: return Cyc_Tcutil_evar_in_effect(evar,_tmp350);_LL1F7:;}
_LL1F3: {struct Cyc_Absyn_Evar_struct*_tmp34F=(struct Cyc_Absyn_Evar_struct*)
_tmp34A;if(_tmp34F->tag != 1)goto _LL1F5;}_LL1F4: return evar == e;_LL1F5:;_LL1F6:
return 0;_LL1EE:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp353=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp355;void*_tmp357;
struct Cyc_Absyn_Tvar*_tmp359;void*_tmp35B;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Core_Opt**
_tmp35E;struct Cyc_Core_Opt*_tmp35F;_LL1FD: {struct Cyc_Absyn_JoinEff_struct*
_tmp354=(struct Cyc_Absyn_JoinEff_struct*)_tmp353;if(_tmp354->tag != 24)goto _LL1FF;
else{_tmp355=_tmp354->f1;}}_LL1FE: for(0;_tmp355 != 0;_tmp355=_tmp355->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp355->hd,e2))return 0;}return 1;_LL1FF: {struct Cyc_Absyn_AccessEff_struct*
_tmp356=(struct Cyc_Absyn_AccessEff_struct*)_tmp353;if(_tmp356->tag != 23)goto
_LL201;else{_tmp357=(void*)_tmp356->f1;}}_LL200: return Cyc_Tcutil_region_in_effect(
0,_tmp357,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp357,(void*)& Cyc_Absyn_HeapRgn_val);
_LL201: {struct Cyc_Absyn_VarType_struct*_tmp358=(struct Cyc_Absyn_VarType_struct*)
_tmp353;if(_tmp358->tag != 2)goto _LL203;else{_tmp359=_tmp358->f1;}}_LL202: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp359,e2);_LL203: {struct Cyc_Absyn_RgnsEff_struct*
_tmp35A=(struct Cyc_Absyn_RgnsEff_struct*)_tmp353;if(_tmp35A->tag != 25)goto _LL205;
else{_tmp35B=(void*)_tmp35A->f1;}}_LL204: {void*_tmp360=Cyc_Tcutil_rgns_of(
_tmp35B);void*_tmp362;_LL20A: {struct Cyc_Absyn_RgnsEff_struct*_tmp361=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp360;if(_tmp361->tag != 25)goto _LL20C;else{_tmp362=(void*)_tmp361->f1;}}_LL20B:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp362,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp362,Cyc_Absyn_sint_typ);_LL20C:;
_LL20D: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp360,e2);_LL209:;}
_LL205: {struct Cyc_Absyn_Evar_struct*_tmp35C=(struct Cyc_Absyn_Evar_struct*)
_tmp353;if(_tmp35C->tag != 1)goto _LL207;else{_tmp35D=_tmp35C->f2;_tmp35E=(struct
Cyc_Core_Opt**)& _tmp35C->f2;_tmp35F=_tmp35C->f4;}}_LL206: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp1071;*_tmp35E=((_tmp1071=_cycalloc(sizeof(*
_tmp1071)),((_tmp1071->v=Cyc_Absyn_empty_effect,_tmp1071))));}return 1;_LL207:;
_LL208: {const char*_tmp1075;void*_tmp1074[1];struct Cyc_String_pa_struct _tmp1073;(
_tmp1073.tag=0,((_tmp1073.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1074[0]=& _tmp1073,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1075="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1075,sizeof(char),30))),_tag_dyneither(_tmp1074,sizeof(void*),
1)))))));}_LL1FC:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int
Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=
Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(
0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(
1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp368;void*_tmp369;void*_tmp36A;struct _tuple0*_tmp367=(struct
_tuple0*)r1->hd;_tmp368=*_tmp367;_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;{int found=
_tmp369 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp36C;void*_tmp36D;void*_tmp36E;struct
_tuple0*_tmp36B=(struct _tuple0*)r2->hd;_tmp36C=*_tmp36B;_tmp36D=_tmp36C.f1;
_tmp36E=_tmp36C.f2;if(Cyc_Tcutil_unify(_tmp369,_tmp36D) && Cyc_Tcutil_unify(
_tmp36A,_tmp36E)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple19{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple20{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp36F=t1;struct Cyc_Core_Opt*_tmp371;struct Cyc_Core_Opt*
_tmp372;struct Cyc_Core_Opt**_tmp373;struct Cyc_Core_Opt*_tmp374;_LL20F: {struct
Cyc_Absyn_Evar_struct*_tmp370=(struct Cyc_Absyn_Evar_struct*)_tmp36F;if(_tmp370->tag
!= 1)goto _LL211;else{_tmp371=_tmp370->f1;_tmp372=_tmp370->f2;_tmp373=(struct Cyc_Core_Opt**)&
_tmp370->f2;_tmp374=_tmp370->f4;}}_LL210: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp374))->v,t2);{struct
Cyc_Absyn_Kind*_tmp375=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp375,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp371))->v)){{struct
Cyc_Core_Opt*_tmp1076;*_tmp373=((_tmp1076=_cycalloc(sizeof(*_tmp1076)),((
_tmp1076->v=t2,_tmp1076))));}return;}else{{void*_tmp377=t2;struct Cyc_Core_Opt*
_tmp379;struct Cyc_Core_Opt**_tmp37A;struct Cyc_Core_Opt*_tmp37B;struct Cyc_Absyn_PtrInfo
_tmp37D;_LL214: {struct Cyc_Absyn_Evar_struct*_tmp378=(struct Cyc_Absyn_Evar_struct*)
_tmp377;if(_tmp378->tag != 1)goto _LL216;else{_tmp379=_tmp378->f2;_tmp37A=(struct
Cyc_Core_Opt**)& _tmp378->f2;_tmp37B=_tmp378->f4;}}_LL215: {struct Cyc_List_List*
_tmp37E=(struct Cyc_List_List*)_tmp374->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp37B))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp37E,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1077;Cyc_Tcutil_failure_reason=((
_tmp1077="(type variable would escape scope)",_tag_dyneither(_tmp1077,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp371->v,_tmp375)){{struct Cyc_Core_Opt*_tmp1078;*_tmp37A=((
_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078->v=t1,_tmp1078))));}return;}{
const char*_tmp1079;Cyc_Tcutil_failure_reason=((_tmp1079="(kinds are incompatible)",
_tag_dyneither(_tmp1079,sizeof(char),25)));}goto _LL213;}_LL216:{struct Cyc_Absyn_PointerType_struct*
_tmp37C=(struct Cyc_Absyn_PointerType_struct*)_tmp377;if(_tmp37C->tag != 5)goto
_LL218;else{_tmp37D=_tmp37C->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp371->v)->kind
== Cyc_Absyn_BoxKind))goto _LL218;_LL217: {union Cyc_Absyn_Constraint*_tmp382=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp37D.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp383=_tmp382;union Cyc_Absyn_Constraint
_tmp384;int _tmp385;_LL21B: _tmp384=*_tmp383;if((_tmp384.No_constr).tag != 3)goto
_LL21D;_tmp385=(int)(_tmp384.No_constr).val;_LL21C:{struct
_union_Constraint_Eq_constr*_tmp107A;(_tmp107A=& _tmp382->Eq_constr,((_tmp107A->tag=
1,_tmp107A->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp107B;*_tmp373=((
_tmp107B=_cycalloc(sizeof(*_tmp107B)),((_tmp107B->v=t2,_tmp107B))));}return;
_LL21D:;_LL21E: goto _LL21A;_LL21A:;}goto _LL213;}_LL218:;_LL219: goto _LL213;_LL213:;}{
const char*_tmp107C;Cyc_Tcutil_failure_reason=((_tmp107C="(kinds are incompatible)",
_tag_dyneither(_tmp107C,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL211:;_LL212: goto _LL20E;_LL20E:;}{struct _tuple0 _tmp107D;struct _tuple0 _tmp38A=(
_tmp107D.f1=t2,((_tmp107D.f2=t1,_tmp107D)));void*_tmp38B;void*_tmp38D;void*
_tmp38F;void*_tmp391;struct Cyc_Absyn_Tvar*_tmp393;void*_tmp394;struct Cyc_Absyn_Tvar*
_tmp396;void*_tmp397;struct Cyc_Absyn_AggrInfo _tmp399;union Cyc_Absyn_AggrInfoU
_tmp39A;struct Cyc_List_List*_tmp39B;void*_tmp39C;struct Cyc_Absyn_AggrInfo _tmp39E;
union Cyc_Absyn_AggrInfoU _tmp39F;struct Cyc_List_List*_tmp3A0;void*_tmp3A1;struct
_tuple2*_tmp3A3;void*_tmp3A4;struct _tuple2*_tmp3A6;void*_tmp3A7;struct Cyc_List_List*
_tmp3A9;void*_tmp3AA;struct Cyc_List_List*_tmp3AC;void*_tmp3AD;struct Cyc_Absyn_DatatypeInfo
_tmp3AF;union Cyc_Absyn_DatatypeInfoU _tmp3B0;struct Cyc_Absyn_Datatypedecl**
_tmp3B1;struct Cyc_Absyn_Datatypedecl*_tmp3B2;struct Cyc_List_List*_tmp3B3;void*
_tmp3B4;struct Cyc_Absyn_DatatypeInfo _tmp3B6;union Cyc_Absyn_DatatypeInfoU _tmp3B7;
struct Cyc_Absyn_Datatypedecl**_tmp3B8;struct Cyc_Absyn_Datatypedecl*_tmp3B9;
struct Cyc_List_List*_tmp3BA;void*_tmp3BB;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3BD;union Cyc_Absyn_DatatypeFieldInfoU _tmp3BE;struct _tuple3 _tmp3BF;struct Cyc_Absyn_Datatypedecl*
_tmp3C0;struct Cyc_Absyn_Datatypefield*_tmp3C1;struct Cyc_List_List*_tmp3C2;void*
_tmp3C3;struct Cyc_Absyn_DatatypeFieldInfo _tmp3C5;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3C6;struct _tuple3 _tmp3C7;struct Cyc_Absyn_Datatypedecl*_tmp3C8;struct Cyc_Absyn_Datatypefield*
_tmp3C9;struct Cyc_List_List*_tmp3CA;void*_tmp3CB;struct Cyc_Absyn_PtrInfo _tmp3CD;
void*_tmp3CE;struct Cyc_Absyn_Tqual _tmp3CF;struct Cyc_Absyn_PtrAtts _tmp3D0;void*
_tmp3D1;union Cyc_Absyn_Constraint*_tmp3D2;union Cyc_Absyn_Constraint*_tmp3D3;
union Cyc_Absyn_Constraint*_tmp3D4;void*_tmp3D5;struct Cyc_Absyn_PtrInfo _tmp3D7;
void*_tmp3D8;struct Cyc_Absyn_Tqual _tmp3D9;struct Cyc_Absyn_PtrAtts _tmp3DA;void*
_tmp3DB;union Cyc_Absyn_Constraint*_tmp3DC;union Cyc_Absyn_Constraint*_tmp3DD;
union Cyc_Absyn_Constraint*_tmp3DE;void*_tmp3DF;enum Cyc_Absyn_Sign _tmp3E1;enum 
Cyc_Absyn_Size_of _tmp3E2;void*_tmp3E3;enum Cyc_Absyn_Sign _tmp3E5;enum Cyc_Absyn_Size_of
_tmp3E6;void*_tmp3E7;void*_tmp3E9;void*_tmp3EB;int _tmp3ED;void*_tmp3EE;int
_tmp3F0;void*_tmp3F1;void*_tmp3F3;void*_tmp3F4;void*_tmp3F6;void*_tmp3F7;struct
Cyc_Absyn_Exp*_tmp3F9;void*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FD;
struct Cyc_Absyn_ArrayInfo _tmp3FF;void*_tmp400;struct Cyc_Absyn_Tqual _tmp401;
struct Cyc_Absyn_Exp*_tmp402;union Cyc_Absyn_Constraint*_tmp403;void*_tmp404;
struct Cyc_Absyn_ArrayInfo _tmp406;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;
struct Cyc_Absyn_Exp*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;void*_tmp40B;
struct Cyc_Absyn_FnInfo _tmp40D;struct Cyc_List_List*_tmp40E;struct Cyc_Core_Opt*
_tmp40F;void*_tmp410;struct Cyc_List_List*_tmp411;int _tmp412;struct Cyc_Absyn_VarargInfo*
_tmp413;struct Cyc_List_List*_tmp414;struct Cyc_List_List*_tmp415;void*_tmp416;
struct Cyc_Absyn_FnInfo _tmp418;struct Cyc_List_List*_tmp419;struct Cyc_Core_Opt*
_tmp41A;void*_tmp41B;struct Cyc_List_List*_tmp41C;int _tmp41D;struct Cyc_Absyn_VarargInfo*
_tmp41E;struct Cyc_List_List*_tmp41F;struct Cyc_List_List*_tmp420;void*_tmp421;
struct Cyc_List_List*_tmp423;void*_tmp424;struct Cyc_List_List*_tmp426;void*
_tmp427;enum Cyc_Absyn_AggrKind _tmp429;struct Cyc_List_List*_tmp42A;void*_tmp42B;
enum Cyc_Absyn_AggrKind _tmp42D;struct Cyc_List_List*_tmp42E;void*_tmp42F;void*
_tmp431;void*_tmp433;void*_tmp435;void*_tmp437;void*_tmp439;void*_tmp43A;void*
_tmp43C;void*_tmp43D;void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp443;void*
_tmp444;void*_tmp445;void*_tmp447;void*_tmp449;void*_tmp44B;void*_tmp44D;void*
_tmp44F;_LL220: _tmp38B=_tmp38A.f1;{struct Cyc_Absyn_Evar_struct*_tmp38C=(struct
Cyc_Absyn_Evar_struct*)_tmp38B;if(_tmp38C->tag != 1)goto _LL222;};_LL221: Cyc_Tcutil_unify_it(
t2,t1);return;_LL222: _tmp38D=_tmp38A.f1;{struct Cyc_Absyn_VoidType_struct*_tmp38E=(
struct Cyc_Absyn_VoidType_struct*)_tmp38D;if(_tmp38E->tag != 0)goto _LL224;};
_tmp38F=_tmp38A.f2;{struct Cyc_Absyn_VoidType_struct*_tmp390=(struct Cyc_Absyn_VoidType_struct*)
_tmp38F;if(_tmp390->tag != 0)goto _LL224;};_LL223: return;_LL224: _tmp391=_tmp38A.f1;{
struct Cyc_Absyn_VarType_struct*_tmp392=(struct Cyc_Absyn_VarType_struct*)_tmp391;
if(_tmp392->tag != 2)goto _LL226;else{_tmp393=_tmp392->f1;}};_tmp394=_tmp38A.f2;{
struct Cyc_Absyn_VarType_struct*_tmp395=(struct Cyc_Absyn_VarType_struct*)_tmp394;
if(_tmp395->tag != 2)goto _LL226;else{_tmp396=_tmp395->f1;}};_LL225: {struct
_dyneither_ptr*_tmp451=_tmp393->name;struct _dyneither_ptr*_tmp452=_tmp396->name;
int _tmp453=_tmp393->identity;int _tmp454=_tmp396->identity;if(_tmp454 == _tmp453)
return;{const char*_tmp107E;Cyc_Tcutil_failure_reason=((_tmp107E="(variable types are not the same)",
_tag_dyneither(_tmp107E,sizeof(char),34)));}goto _LL21F;}_LL226: _tmp397=_tmp38A.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp398=(struct Cyc_Absyn_AggrType_struct*)
_tmp397;if(_tmp398->tag != 12)goto _LL228;else{_tmp399=_tmp398->f1;_tmp39A=_tmp399.aggr_info;
_tmp39B=_tmp399.targs;}};_tmp39C=_tmp38A.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp39D=(struct Cyc_Absyn_AggrType_struct*)_tmp39C;if(_tmp39D->tag != 12)goto
_LL228;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39E.aggr_info;_tmp3A0=_tmp39E.targs;}};
_LL227: {enum Cyc_Absyn_AggrKind _tmp457;struct _tuple2*_tmp458;struct _tuple12
_tmp456=Cyc_Absyn_aggr_kinded_name(_tmp39F);_tmp457=_tmp456.f1;_tmp458=_tmp456.f2;{
enum Cyc_Absyn_AggrKind _tmp45A;struct _tuple2*_tmp45B;struct _tuple12 _tmp459=Cyc_Absyn_aggr_kinded_name(
_tmp39A);_tmp45A=_tmp459.f1;_tmp45B=_tmp459.f2;if(_tmp457 != _tmp45A){{const char*
_tmp107F;Cyc_Tcutil_failure_reason=((_tmp107F="(struct and union type)",
_tag_dyneither(_tmp107F,sizeof(char),24)));}goto _LL21F;}if(Cyc_Absyn_qvar_cmp(
_tmp458,_tmp45B)!= 0){{const char*_tmp1080;Cyc_Tcutil_failure_reason=((_tmp1080="(different type name)",
_tag_dyneither(_tmp1080,sizeof(char),22)));}goto _LL21F;}Cyc_Tcutil_unify_list(
_tmp3A0,_tmp39B);return;};}_LL228: _tmp3A1=_tmp38A.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp3A2=(struct Cyc_Absyn_EnumType_struct*)_tmp3A1;if(_tmp3A2->tag != 14)goto
_LL22A;else{_tmp3A3=_tmp3A2->f1;}};_tmp3A4=_tmp38A.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp3A5=(struct Cyc_Absyn_EnumType_struct*)_tmp3A4;if(_tmp3A5->tag != 14)goto
_LL22A;else{_tmp3A6=_tmp3A5->f1;}};_LL229: if(Cyc_Absyn_qvar_cmp(_tmp3A3,_tmp3A6)
== 0)return;{const char*_tmp1081;Cyc_Tcutil_failure_reason=((_tmp1081="(different enum types)",
_tag_dyneither(_tmp1081,sizeof(char),23)));}goto _LL21F;_LL22A: _tmp3A7=_tmp38A.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp3A8=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A7;if(_tmp3A8->tag != 15)goto _LL22C;else{_tmp3A9=_tmp3A8->f1;}};_tmp3AA=
_tmp38A.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp3AB=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3AA;if(_tmp3AB->tag != 15)goto _LL22C;else{_tmp3AC=_tmp3AB->f1;}};_LL22B: {int
bad=0;for(0;_tmp3A9 != 0  && _tmp3AC != 0;(_tmp3A9=_tmp3A9->tl,_tmp3AC=_tmp3AC->tl)){
struct Cyc_Absyn_Enumfield*_tmp45F=(struct Cyc_Absyn_Enumfield*)_tmp3A9->hd;struct
Cyc_Absyn_Enumfield*_tmp460=(struct Cyc_Absyn_Enumfield*)_tmp3AC->hd;if(Cyc_Absyn_qvar_cmp(
_tmp45F->name,_tmp460->name)!= 0){{const char*_tmp1082;Cyc_Tcutil_failure_reason=((
_tmp1082="(different names for enum fields)",_tag_dyneither(_tmp1082,sizeof(char),
34)));}bad=1;break;}if(_tmp45F->tag == _tmp460->tag)continue;if(_tmp45F->tag == 0
 || _tmp460->tag == 0){{const char*_tmp1083;Cyc_Tcutil_failure_reason=((_tmp1083="(different tag values for enum fields)",
_tag_dyneither(_tmp1083,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp45F->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp460->tag))){{const char*_tmp1084;Cyc_Tcutil_failure_reason=((_tmp1084="(different tag values for enum fields)",
_tag_dyneither(_tmp1084,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21F;if(
_tmp3A9 == 0  && _tmp3AC == 0)return;{const char*_tmp1085;Cyc_Tcutil_failure_reason=((
_tmp1085="(different number of fields for enums)",_tag_dyneither(_tmp1085,
sizeof(char),39)));}goto _LL21F;}_LL22C: _tmp3AD=_tmp38A.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp3AE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp3AD;if(_tmp3AE->tag != 3)goto
_LL22E;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AF.datatype_info;if((_tmp3B0.KnownDatatype).tag
!= 2)goto _LL22E;_tmp3B1=(struct Cyc_Absyn_Datatypedecl**)(_tmp3B0.KnownDatatype).val;
_tmp3B2=*_tmp3B1;_tmp3B3=_tmp3AF.targs;}};_tmp3B4=_tmp38A.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp3B5=(struct Cyc_Absyn_DatatypeType_struct*)_tmp3B4;if(_tmp3B5->tag != 3)goto
_LL22E;else{_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B6.datatype_info;if((_tmp3B7.KnownDatatype).tag
!= 2)goto _LL22E;_tmp3B8=(struct Cyc_Absyn_Datatypedecl**)(_tmp3B7.KnownDatatype).val;
_tmp3B9=*_tmp3B8;_tmp3BA=_tmp3B6.targs;}};_LL22D: if(_tmp3B2 == _tmp3B9  || Cyc_Absyn_qvar_cmp(
_tmp3B2->name,_tmp3B9->name)== 0){Cyc_Tcutil_unify_list(_tmp3BA,_tmp3B3);return;}
goto _LL21F;_LL22E: _tmp3BB=_tmp38A.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3BC=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3BB;if(_tmp3BC->tag != 4)
goto _LL230;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BD.field_info;if((_tmp3BE.KnownDatatypefield).tag
!= 2)goto _LL230;_tmp3BF=(struct _tuple3)(_tmp3BE.KnownDatatypefield).val;_tmp3C0=
_tmp3BF.f1;_tmp3C1=_tmp3BF.f2;_tmp3C2=_tmp3BD.targs;}};_tmp3C3=_tmp38A.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp3C4=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3C3;if(_tmp3C4->tag != 4)goto _LL230;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C5.field_info;
if((_tmp3C6.KnownDatatypefield).tag != 2)goto _LL230;_tmp3C7=(struct _tuple3)(
_tmp3C6.KnownDatatypefield).val;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;_tmp3CA=
_tmp3C5.targs;}};_LL22F: if((_tmp3C0 == _tmp3C8  || Cyc_Absyn_qvar_cmp(_tmp3C0->name,
_tmp3C8->name)== 0) && (_tmp3C1 == _tmp3C9  || Cyc_Absyn_qvar_cmp(_tmp3C1->name,
_tmp3C9->name)== 0)){Cyc_Tcutil_unify_list(_tmp3CA,_tmp3C2);return;}{const char*
_tmp1086;Cyc_Tcutil_failure_reason=((_tmp1086="(different datatype field types)",
_tag_dyneither(_tmp1086,sizeof(char),33)));}goto _LL21F;_LL230: _tmp3CB=_tmp38A.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp3CC=(struct Cyc_Absyn_PointerType_struct*)
_tmp3CB;if(_tmp3CC->tag != 5)goto _LL232;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CD.elt_typ;
_tmp3CF=_tmp3CD.elt_tq;_tmp3D0=_tmp3CD.ptr_atts;_tmp3D1=_tmp3D0.rgn;_tmp3D2=
_tmp3D0.nullable;_tmp3D3=_tmp3D0.bounds;_tmp3D4=_tmp3D0.zero_term;}};_tmp3D5=
_tmp38A.f2;{struct Cyc_Absyn_PointerType_struct*_tmp3D6=(struct Cyc_Absyn_PointerType_struct*)
_tmp3D5;if(_tmp3D6->tag != 5)goto _LL232;else{_tmp3D7=_tmp3D6->f1;_tmp3D8=_tmp3D7.elt_typ;
_tmp3D9=_tmp3D7.elt_tq;_tmp3DA=_tmp3D7.ptr_atts;_tmp3DB=_tmp3DA.rgn;_tmp3DC=
_tmp3DA.nullable;_tmp3DD=_tmp3DA.bounds;_tmp3DE=_tmp3DA.zero_term;}};_LL231: Cyc_Tcutil_unify_it(
_tmp3D8,_tmp3CE);Cyc_Tcutil_unify_it(_tmp3D1,_tmp3DB);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1087;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3DE,_tmp3D4,((_tmp1087="(not both zero terminated)",
_tag_dyneither(_tmp1087,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp3D9,
_tmp3D8,_tmp3CF,_tmp3CE);{const char*_tmp1088;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3DD,_tmp3D3,((
_tmp1088="(different pointer bounds)",_tag_dyneither(_tmp1088,sizeof(char),27))));}{
void*_tmp468=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp3DD);_LL25B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp469=(struct Cyc_Absyn_DynEither_b_struct*)_tmp468;if(_tmp469->tag != 0)goto
_LL25D;}_LL25C: return;_LL25D:;_LL25E: goto _LL25A;_LL25A:;}{const char*_tmp1089;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3DC,_tmp3D2,((_tmp1089="(incompatible pointer types)",_tag_dyneither(
_tmp1089,sizeof(char),29))));}return;_LL232: _tmp3DF=_tmp38A.f1;{struct Cyc_Absyn_IntType_struct*
_tmp3E0=(struct Cyc_Absyn_IntType_struct*)_tmp3DF;if(_tmp3E0->tag != 6)goto _LL234;
else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}};_tmp3E3=_tmp38A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp3E4=(struct Cyc_Absyn_IntType_struct*)_tmp3E3;if(_tmp3E4->tag != 6)goto _LL234;
else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;}};_LL233: if(_tmp3E5 == _tmp3E1  && ((
_tmp3E6 == _tmp3E2  || _tmp3E6 == Cyc_Absyn_Int_sz  && _tmp3E2 == Cyc_Absyn_Long_sz)
 || _tmp3E6 == Cyc_Absyn_Long_sz  && _tmp3E2 == Cyc_Absyn_Int_sz))return;{const char*
_tmp108A;Cyc_Tcutil_failure_reason=((_tmp108A="(different integral types)",
_tag_dyneither(_tmp108A,sizeof(char),27)));}goto _LL21F;_LL234: _tmp3E7=_tmp38A.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp3E8=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E7;if(_tmp3E8->tag != 7)goto _LL236;};_tmp3E9=_tmp38A.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp3EA=(struct Cyc_Absyn_FloatType_struct*)_tmp3E9;if(_tmp3EA->tag != 7)goto
_LL236;};_LL235: return;_LL236: _tmp3EB=_tmp38A.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp3EC=(struct Cyc_Absyn_DoubleType_struct*)_tmp3EB;if(_tmp3EC->tag != 8)goto
_LL238;else{_tmp3ED=_tmp3EC->f1;}};_tmp3EE=_tmp38A.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp3EF=(struct Cyc_Absyn_DoubleType_struct*)_tmp3EE;if(_tmp3EF->tag != 8)goto
_LL238;else{_tmp3F0=_tmp3EF->f1;}};_LL237: if(_tmp3ED == _tmp3F0)return;goto _LL21F;
_LL238: _tmp3F1=_tmp38A.f1;{struct Cyc_Absyn_TagType_struct*_tmp3F2=(struct Cyc_Absyn_TagType_struct*)
_tmp3F1;if(_tmp3F2->tag != 20)goto _LL23A;else{_tmp3F3=(void*)_tmp3F2->f1;}};
_tmp3F4=_tmp38A.f2;{struct Cyc_Absyn_TagType_struct*_tmp3F5=(struct Cyc_Absyn_TagType_struct*)
_tmp3F4;if(_tmp3F5->tag != 20)goto _LL23A;else{_tmp3F6=(void*)_tmp3F5->f1;}};
_LL239: Cyc_Tcutil_unify_it(_tmp3F3,_tmp3F6);return;_LL23A: _tmp3F7=_tmp38A.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp3F8=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3F7;if(_tmp3F8->tag != 19)goto _LL23C;else{_tmp3F9=_tmp3F8->f1;}};_tmp3FA=
_tmp38A.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp3FB=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3FA;if(_tmp3FB->tag != 19)goto _LL23C;else{_tmp3FC=_tmp3FB->f1;}};_LL23B: if(!
Cyc_Evexp_same_const_exp(_tmp3F9,_tmp3FC)){{const char*_tmp108B;Cyc_Tcutil_failure_reason=((
_tmp108B="(cannot prove expressions are the same)",_tag_dyneither(_tmp108B,
sizeof(char),40)));}goto _LL21F;}return;_LL23C: _tmp3FD=_tmp38A.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp3FE=(struct Cyc_Absyn_ArrayType_struct*)_tmp3FD;if(_tmp3FE->tag != 9)goto
_LL23E;else{_tmp3FF=_tmp3FE->f1;_tmp400=_tmp3FF.elt_type;_tmp401=_tmp3FF.tq;
_tmp402=_tmp3FF.num_elts;_tmp403=_tmp3FF.zero_term;}};_tmp404=_tmp38A.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp405=(struct Cyc_Absyn_ArrayType_struct*)_tmp404;
if(_tmp405->tag != 9)goto _LL23E;else{_tmp406=_tmp405->f1;_tmp407=_tmp406.elt_type;
_tmp408=_tmp406.tq;_tmp409=_tmp406.num_elts;_tmp40A=_tmp406.zero_term;}};_LL23D:
Cyc_Tcutil_unify_it(_tmp407,_tmp400);Cyc_Tcutil_unify_tqual(_tmp408,_tmp407,
_tmp401,_tmp400);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp108C;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp403,_tmp40A,((_tmp108C="(not both zero terminated)",_tag_dyneither(_tmp108C,
sizeof(char),27))));}if(_tmp402 == _tmp409)return;if(_tmp402 == 0  || _tmp409 == 0)
goto _LL21F;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp402,(struct Cyc_Absyn_Exp*)
_tmp409))return;{const char*_tmp108D;Cyc_Tcutil_failure_reason=((_tmp108D="(different array sizes)",
_tag_dyneither(_tmp108D,sizeof(char),24)));}goto _LL21F;_LL23E: _tmp40B=_tmp38A.f1;{
struct Cyc_Absyn_FnType_struct*_tmp40C=(struct Cyc_Absyn_FnType_struct*)_tmp40B;
if(_tmp40C->tag != 10)goto _LL240;else{_tmp40D=_tmp40C->f1;_tmp40E=_tmp40D.tvars;
_tmp40F=_tmp40D.effect;_tmp410=_tmp40D.ret_typ;_tmp411=_tmp40D.args;_tmp412=
_tmp40D.c_varargs;_tmp413=_tmp40D.cyc_varargs;_tmp414=_tmp40D.rgn_po;_tmp415=
_tmp40D.attributes;}};_tmp416=_tmp38A.f2;{struct Cyc_Absyn_FnType_struct*_tmp417=(
struct Cyc_Absyn_FnType_struct*)_tmp416;if(_tmp417->tag != 10)goto _LL240;else{
_tmp418=_tmp417->f1;_tmp419=_tmp418.tvars;_tmp41A=_tmp418.effect;_tmp41B=_tmp418.ret_typ;
_tmp41C=_tmp418.args;_tmp41D=_tmp418.c_varargs;_tmp41E=_tmp418.cyc_varargs;
_tmp41F=_tmp418.rgn_po;_tmp420=_tmp418.attributes;}};_LL23F: {int done=0;struct
_RegionHandle _tmp46F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp46F;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp419 != 0){if(_tmp40E == 0){{
const char*_tmp108E;Cyc_Tcutil_failure_reason=((_tmp108E="(second function type has too few type variables)",
_tag_dyneither(_tmp108E,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp471=((struct Cyc_Absyn_Tvar*)_tmp419->hd)->kind;void*_tmp472=((struct Cyc_Absyn_Tvar*)
_tmp40E->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp471,_tmp472)){{const char*
_tmp1094;void*_tmp1093[3];struct Cyc_String_pa_struct _tmp1092;struct Cyc_String_pa_struct
_tmp1091;struct Cyc_String_pa_struct _tmp1090;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1090.tag=0,((_tmp1090.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp40E->hd,& Cyc_Tcutil_bk))),((_tmp1091.tag=0,((_tmp1091.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp419->hd,& Cyc_Tcutil_bk))),((_tmp1092.tag=0,((_tmp1092.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp419->hd)),((_tmp1093[0]=& _tmp1092,((_tmp1093[1]=& _tmp1091,((_tmp1093[2]=&
_tmp1090,Cyc_aprintf(((_tmp1094="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1094,sizeof(char),44))),_tag_dyneither(_tmp1093,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple14*_tmp109E;struct Cyc_Absyn_VarType_struct _tmp109D;struct Cyc_Absyn_VarType_struct*
_tmp109C;struct Cyc_List_List*_tmp109B;inst=((_tmp109B=_region_malloc(rgn,sizeof(*
_tmp109B)),((_tmp109B->hd=((_tmp109E=_region_malloc(rgn,sizeof(*_tmp109E)),((
_tmp109E->f1=(struct Cyc_Absyn_Tvar*)_tmp40E->hd,((_tmp109E->f2=(void*)((_tmp109C=
_cycalloc(sizeof(*_tmp109C)),((_tmp109C[0]=((_tmp109D.tag=2,((_tmp109D.f1=(
struct Cyc_Absyn_Tvar*)_tmp419->hd,_tmp109D)))),_tmp109C)))),_tmp109E)))))),((
_tmp109B->tl=inst,_tmp109B))))));}_tmp419=_tmp419->tl;_tmp40E=_tmp40E->tl;};}if(
_tmp40E != 0){{const char*_tmp109F;Cyc_Tcutil_failure_reason=((_tmp109F="(second function type has too many type variables)",
_tag_dyneither(_tmp109F,sizeof(char),51)));}_npop_handler(0);goto _LL21F;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp10AB;struct Cyc_Absyn_FnInfo _tmp10AA;
struct Cyc_Absyn_FnType_struct*_tmp10A9;struct Cyc_Absyn_FnType_struct _tmp10A5;
struct Cyc_Absyn_FnInfo _tmp10A4;struct Cyc_Absyn_FnType_struct*_tmp10A3;Cyc_Tcutil_unify_it((
void*)((_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((_tmp10A3[0]=((_tmp10A5.tag=10,((
_tmp10A5.f1=((_tmp10A4.tvars=0,((_tmp10A4.effect=_tmp41A,((_tmp10A4.ret_typ=
_tmp41B,((_tmp10A4.args=_tmp41C,((_tmp10A4.c_varargs=_tmp41D,((_tmp10A4.cyc_varargs=
_tmp41E,((_tmp10A4.rgn_po=_tmp41F,((_tmp10A4.attributes=_tmp420,_tmp10A4)))))))))))))))),
_tmp10A5)))),_tmp10A3)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp10A9=
_cycalloc(sizeof(*_tmp10A9)),((_tmp10A9[0]=((_tmp10AB.tag=10,((_tmp10AB.f1=((
_tmp10AA.tvars=0,((_tmp10AA.effect=_tmp40F,((_tmp10AA.ret_typ=_tmp410,((_tmp10AA.args=
_tmp411,((_tmp10AA.c_varargs=_tmp412,((_tmp10AA.cyc_varargs=_tmp413,((_tmp10AA.rgn_po=
_tmp414,((_tmp10AA.attributes=_tmp415,_tmp10AA)))))))))))))))),_tmp10AB)))),
_tmp10A9))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp41B,_tmp410);for(0;_tmp41C != 0  && _tmp411 != 0;(_tmp41C=_tmp41C->tl,_tmp411=
_tmp411->tl)){struct Cyc_Absyn_Tqual _tmp484;void*_tmp485;struct _tuple9 _tmp483=*((
struct _tuple9*)_tmp41C->hd);_tmp484=_tmp483.f2;_tmp485=_tmp483.f3;{struct Cyc_Absyn_Tqual
_tmp487;void*_tmp488;struct _tuple9 _tmp486=*((struct _tuple9*)_tmp411->hd);_tmp487=
_tmp486.f2;_tmp488=_tmp486.f3;Cyc_Tcutil_unify_it(_tmp485,_tmp488);Cyc_Tcutil_unify_tqual(
_tmp484,_tmp485,_tmp487,_tmp488);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp41C != 0  || _tmp411 != 0){{const char*_tmp10AC;Cyc_Tcutil_failure_reason=((
_tmp10AC="(function types have different number of arguments)",_tag_dyneither(
_tmp10AC,sizeof(char),52)));}_npop_handler(0);goto _LL21F;}if(_tmp41D != _tmp412){{
const char*_tmp10AD;Cyc_Tcutil_failure_reason=((_tmp10AD="(only one function type takes C varargs)",
_tag_dyneither(_tmp10AD,sizeof(char),41)));}_npop_handler(0);goto _LL21F;}{int
bad_cyc_vararg=0;{struct _tuple19 _tmp10AE;struct _tuple19 _tmp48C=(_tmp10AE.f1=
_tmp41E,((_tmp10AE.f2=_tmp413,_tmp10AE)));struct Cyc_Absyn_VarargInfo*_tmp48D;
struct Cyc_Absyn_VarargInfo*_tmp48E;struct Cyc_Absyn_VarargInfo*_tmp48F;struct Cyc_Absyn_VarargInfo*
_tmp490;struct Cyc_Absyn_VarargInfo*_tmp491;struct Cyc_Absyn_VarargInfo _tmp492;
struct Cyc_Core_Opt*_tmp493;struct Cyc_Absyn_Tqual _tmp494;void*_tmp495;int _tmp496;
struct Cyc_Absyn_VarargInfo*_tmp497;struct Cyc_Absyn_VarargInfo _tmp498;struct Cyc_Core_Opt*
_tmp499;struct Cyc_Absyn_Tqual _tmp49A;void*_tmp49B;int _tmp49C;_LL260: _tmp48D=
_tmp48C.f1;if(_tmp48D != 0)goto _LL262;_tmp48E=_tmp48C.f2;if(_tmp48E != 0)goto
_LL262;_LL261: goto _LL25F;_LL262: _tmp48F=_tmp48C.f1;if(_tmp48F != 0)goto _LL264;
_LL263: goto _LL265;_LL264: _tmp490=_tmp48C.f2;if(_tmp490 != 0)goto _LL266;_LL265:
bad_cyc_vararg=1;{const char*_tmp10AF;Cyc_Tcutil_failure_reason=((_tmp10AF="(only one function type takes varargs)",
_tag_dyneither(_tmp10AF,sizeof(char),39)));}goto _LL25F;_LL266: _tmp491=_tmp48C.f1;
if(_tmp491 == 0)goto _LL25F;_tmp492=*_tmp491;_tmp493=_tmp492.name;_tmp494=_tmp492.tq;
_tmp495=_tmp492.type;_tmp496=_tmp492.inject;_tmp497=_tmp48C.f2;if(_tmp497 == 0)
goto _LL25F;_tmp498=*_tmp497;_tmp499=_tmp498.name;_tmp49A=_tmp498.tq;_tmp49B=
_tmp498.type;_tmp49C=_tmp498.inject;_LL267: Cyc_Tcutil_unify_it(_tmp495,_tmp49B);
Cyc_Tcutil_unify_tqual(_tmp494,_tmp495,_tmp49A,_tmp49B);if(_tmp496 != _tmp49C){
bad_cyc_vararg=1;{const char*_tmp10B0;Cyc_Tcutil_failure_reason=((_tmp10B0="(only one function type injects varargs)",
_tag_dyneither(_tmp10B0,sizeof(char),41)));};}goto _LL25F;_LL25F:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21F;}{int bad_effect=0;{struct _tuple20
_tmp10B1;struct _tuple20 _tmp4A0=(_tmp10B1.f1=_tmp41A,((_tmp10B1.f2=_tmp40F,
_tmp10B1)));struct Cyc_Core_Opt*_tmp4A1;struct Cyc_Core_Opt*_tmp4A2;struct Cyc_Core_Opt*
_tmp4A3;struct Cyc_Core_Opt*_tmp4A4;_LL269: _tmp4A1=_tmp4A0.f1;if(_tmp4A1 != 0)goto
_LL26B;_tmp4A2=_tmp4A0.f2;if(_tmp4A2 != 0)goto _LL26B;_LL26A: goto _LL268;_LL26B:
_tmp4A3=_tmp4A0.f1;if(_tmp4A3 != 0)goto _LL26D;_LL26C: goto _LL26E;_LL26D: _tmp4A4=
_tmp4A0.f2;if(_tmp4A4 != 0)goto _LL26F;_LL26E: bad_effect=1;goto _LL268;_LL26F:;
_LL270: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp41A))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp40F))->v);
goto _LL268;_LL268:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp10B2;Cyc_Tcutil_failure_reason=((_tmp10B2="(function type effects do not unify)",
_tag_dyneither(_tmp10B2,sizeof(char),37)));}_npop_handler(0);goto _LL21F;}if(!Cyc_Tcutil_same_atts(
_tmp415,_tmp420)){{const char*_tmp10B3;Cyc_Tcutil_failure_reason=((_tmp10B3="(function types have different attributes)",
_tag_dyneither(_tmp10B3,sizeof(char),43)));}_npop_handler(0);goto _LL21F;}if(!Cyc_Tcutil_same_rgn_po(
_tmp414,_tmp41F)){{const char*_tmp10B4;Cyc_Tcutil_failure_reason=((_tmp10B4="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp10B4,sizeof(char),58)));}_npop_handler(0);goto _LL21F;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL240: _tmp421=_tmp38A.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp422=(struct Cyc_Absyn_TupleType_struct*)_tmp421;
if(_tmp422->tag != 11)goto _LL242;else{_tmp423=_tmp422->f1;}};_tmp424=_tmp38A.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp425=(struct Cyc_Absyn_TupleType_struct*)
_tmp424;if(_tmp425->tag != 11)goto _LL242;else{_tmp426=_tmp425->f1;}};_LL241: for(0;
_tmp426 != 0  && _tmp423 != 0;(_tmp426=_tmp426->tl,_tmp423=_tmp423->tl)){struct Cyc_Absyn_Tqual
_tmp4A9;void*_tmp4AA;struct _tuple11 _tmp4A8=*((struct _tuple11*)_tmp426->hd);
_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;{struct Cyc_Absyn_Tqual _tmp4AC;void*_tmp4AD;
struct _tuple11 _tmp4AB=*((struct _tuple11*)_tmp423->hd);_tmp4AC=_tmp4AB.f1;_tmp4AD=
_tmp4AB.f2;Cyc_Tcutil_unify_it(_tmp4AA,_tmp4AD);Cyc_Tcutil_unify_tqual(_tmp4A9,
_tmp4AA,_tmp4AC,_tmp4AD);};}if(_tmp426 == 0  && _tmp423 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp10B5;Cyc_Tcutil_failure_reason=((
_tmp10B5="(tuple types have different numbers of components)",_tag_dyneither(
_tmp10B5,sizeof(char),51)));}goto _LL21F;_LL242: _tmp427=_tmp38A.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp428=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp427;if(_tmp428->tag != 13)goto
_LL244;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}};_tmp42B=_tmp38A.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp42C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42B;if(_tmp42C->tag != 13)goto _LL244;else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}};
_LL243: if(_tmp42D != _tmp429){{const char*_tmp10B6;Cyc_Tcutil_failure_reason=((
_tmp10B6="(struct and union type)",_tag_dyneither(_tmp10B6,sizeof(char),24)));}
goto _LL21F;}for(0;_tmp42E != 0  && _tmp42A != 0;(_tmp42E=_tmp42E->tl,_tmp42A=
_tmp42A->tl)){struct Cyc_Absyn_Aggrfield*_tmp4B0=(struct Cyc_Absyn_Aggrfield*)
_tmp42E->hd;struct Cyc_Absyn_Aggrfield*_tmp4B1=(struct Cyc_Absyn_Aggrfield*)
_tmp42A->hd;if(Cyc_strptrcmp(_tmp4B0->name,_tmp4B1->name)!= 0){{const char*
_tmp10B7;Cyc_Tcutil_failure_reason=((_tmp10B7="(different member names)",
_tag_dyneither(_tmp10B7,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp4B0->type,_tmp4B1->type);Cyc_Tcutil_unify_tqual(_tmp4B0->tq,
_tmp4B0->type,_tmp4B1->tq,_tmp4B1->type);if(!Cyc_Tcutil_same_atts(_tmp4B0->attributes,
_tmp4B1->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp10B8;Cyc_Tcutil_failure_reason=((_tmp10B8="(different attributes on member)",
_tag_dyneither(_tmp10B8,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp4B0->width != 0  && _tmp4B1->width == 0  || _tmp4B1->width != 0  && _tmp4B0->width
== 0) || (_tmp4B0->width != 0  && _tmp4B1->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp4B0->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp4B1->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp10B9;Cyc_Tcutil_failure_reason=((_tmp10B9="(different bitfield widths on member)",
_tag_dyneither(_tmp10B9,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp42E == 0  && _tmp42A == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp10BA;Cyc_Tcutil_failure_reason=((_tmp10BA="(different number of members)",
_tag_dyneither(_tmp10BA,sizeof(char),30)));}goto _LL21F;_LL244: _tmp42F=_tmp38A.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp430=(struct Cyc_Absyn_HeapRgn_struct*)_tmp42F;
if(_tmp430->tag != 21)goto _LL246;};_tmp431=_tmp38A.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp432=(struct Cyc_Absyn_HeapRgn_struct*)_tmp431;if(_tmp432->tag != 21)goto _LL246;};
_LL245: return;_LL246: _tmp433=_tmp38A.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp434=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp433;if(_tmp434->tag != 22)goto
_LL248;};_tmp435=_tmp38A.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp436=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp435;if(_tmp436->tag != 22)goto _LL248;};_LL247: return;_LL248: _tmp437=_tmp38A.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp438=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp437;if(_tmp438->tag != 16)goto _LL24A;else{_tmp439=(void*)_tmp438->f1;}};
_tmp43A=_tmp38A.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp43B=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43A;if(_tmp43B->tag != 16)goto _LL24A;else{_tmp43C=(void*)_tmp43B->f1;}};
_LL249: Cyc_Tcutil_unify_it(_tmp439,_tmp43C);return;_LL24A: _tmp43D=_tmp38A.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp43E=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp43D;if(_tmp43E->tag != 17)goto _LL24C;else{_tmp43F=(void*)_tmp43E->f1;_tmp440=(
void*)_tmp43E->f2;}};_tmp441=_tmp38A.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp442=(struct Cyc_Absyn_DynRgnType_struct*)_tmp441;if(_tmp442->tag != 17)goto
_LL24C;else{_tmp443=(void*)_tmp442->f1;_tmp444=(void*)_tmp442->f2;}};_LL24B: Cyc_Tcutil_unify_it(
_tmp43F,_tmp443);Cyc_Tcutil_unify_it(_tmp440,_tmp444);return;_LL24C: _tmp445=
_tmp38A.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp446=(struct Cyc_Absyn_JoinEff_struct*)
_tmp445;if(_tmp446->tag != 24)goto _LL24E;};_LL24D: goto _LL24F;_LL24E: _tmp447=
_tmp38A.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp448=(struct Cyc_Absyn_JoinEff_struct*)
_tmp447;if(_tmp448->tag != 24)goto _LL250;};_LL24F: goto _LL251;_LL250: _tmp449=
_tmp38A.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp44A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp449;if(_tmp44A->tag != 23)goto _LL252;};_LL251: goto _LL253;_LL252: _tmp44B=
_tmp38A.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp44C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp44B;if(_tmp44C->tag != 25)goto _LL254;};_LL253: goto _LL255;_LL254: _tmp44D=
_tmp38A.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp44E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp44D;if(_tmp44E->tag != 25)goto _LL256;};_LL255: goto _LL257;_LL256: _tmp44F=
_tmp38A.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp450=(struct Cyc_Absyn_AccessEff_struct*)
_tmp44F;if(_tmp450->tag != 23)goto _LL258;};_LL257: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp10BB;Cyc_Tcutil_failure_reason=((_tmp10BB="(effects don't unify)",
_tag_dyneither(_tmp10BB,sizeof(char),22)));}goto _LL21F;_LL258:;_LL259: goto _LL21F;
_LL21F:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp4B7=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp4B8=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp4B7,_tmp4B8);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4B9=x;union Cyc_Absyn_Constraint _tmp4BA;int _tmp4BB;
union Cyc_Absyn_Constraint _tmp4BC;void*_tmp4BD;union Cyc_Absyn_Constraint _tmp4BE;
union Cyc_Absyn_Constraint*_tmp4BF;_LL272: _tmp4BA=*_tmp4B9;if((_tmp4BA.No_constr).tag
!= 3)goto _LL274;_tmp4BB=(int)(_tmp4BA.No_constr).val;_LL273: return - 1;_LL274:
_tmp4BC=*_tmp4B9;if((_tmp4BC.Eq_constr).tag != 1)goto _LL276;_tmp4BD=(void*)(
_tmp4BC.Eq_constr).val;_LL275: {union Cyc_Absyn_Constraint*_tmp4C0=y;union Cyc_Absyn_Constraint
_tmp4C1;int _tmp4C2;union Cyc_Absyn_Constraint _tmp4C3;void*_tmp4C4;union Cyc_Absyn_Constraint
_tmp4C5;union Cyc_Absyn_Constraint*_tmp4C6;_LL279: _tmp4C1=*_tmp4C0;if((_tmp4C1.No_constr).tag
!= 3)goto _LL27B;_tmp4C2=(int)(_tmp4C1.No_constr).val;_LL27A: return 1;_LL27B:
_tmp4C3=*_tmp4C0;if((_tmp4C3.Eq_constr).tag != 1)goto _LL27D;_tmp4C4=(void*)(
_tmp4C3.Eq_constr).val;_LL27C: return cmp(_tmp4BD,_tmp4C4);_LL27D: _tmp4C5=*_tmp4C0;
if((_tmp4C5.Forward_constr).tag != 2)goto _LL278;_tmp4C6=(union Cyc_Absyn_Constraint*)(
_tmp4C5.Forward_constr).val;_LL27E: {const char*_tmp10BE;void*_tmp10BD;(_tmp10BD=
0,Cyc_Tcutil_impos(((_tmp10BE="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp10BE,sizeof(char),40))),_tag_dyneither(_tmp10BD,sizeof(void*),
0)));}_LL278:;}_LL276: _tmp4BE=*_tmp4B9;if((_tmp4BE.Forward_constr).tag != 2)goto
_LL271;_tmp4BF=(union Cyc_Absyn_Constraint*)(_tmp4BE.Forward_constr).val;_LL277: {
const char*_tmp10C1;void*_tmp10C0;(_tmp10C0=0,Cyc_Tcutil_impos(((_tmp10C1="unify_conref: forward after compress",
_tag_dyneither(_tmp10C1,sizeof(char),37))),_tag_dyneither(_tmp10C0,sizeof(void*),
0)));}_LL271:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp4CC;struct Cyc_Absyn_Tqual _tmp4CD;void*_tmp4CE;
struct _tuple11*_tmp4CB=tqt1;_tmp4CC=*_tmp4CB;_tmp4CD=_tmp4CC.f1;_tmp4CE=_tmp4CC.f2;{
struct _tuple11 _tmp4D0;struct Cyc_Absyn_Tqual _tmp4D1;void*_tmp4D2;struct _tuple11*
_tmp4CF=tqt2;_tmp4D0=*_tmp4CF;_tmp4D1=_tmp4D0.f1;_tmp4D2=_tmp4D0.f2;{int _tmp4D3=
Cyc_Tcutil_tqual_cmp(_tmp4CD,_tmp4D1);if(_tmp4D3 != 0)return _tmp4D3;return Cyc_Tcutil_typecmp(
_tmp4CE,_tmp4D2);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp4D4=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp4D4 != 0)return _tmp4D4;{int _tmp4D5=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp4D5 != 0)return _tmp4D5;{int _tmp4D6=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp4D6 != 0)return _tmp4D6;{int _tmp4D7=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp4D7 != 0)return _tmp4D7;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp4D8=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp4D8 != 0)return _tmp4D8;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp4D9=t;_LL280: {
struct Cyc_Absyn_VoidType_struct*_tmp4DA=(struct Cyc_Absyn_VoidType_struct*)
_tmp4D9;if(_tmp4DA->tag != 0)goto _LL282;}_LL281: return 0;_LL282: {struct Cyc_Absyn_Evar_struct*
_tmp4DB=(struct Cyc_Absyn_Evar_struct*)_tmp4D9;if(_tmp4DB->tag != 1)goto _LL284;}
_LL283: return 1;_LL284: {struct Cyc_Absyn_VarType_struct*_tmp4DC=(struct Cyc_Absyn_VarType_struct*)
_tmp4D9;if(_tmp4DC->tag != 2)goto _LL286;}_LL285: return 2;_LL286: {struct Cyc_Absyn_DatatypeType_struct*
_tmp4DD=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4D9;if(_tmp4DD->tag != 3)goto
_LL288;}_LL287: return 3;_LL288: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4DE=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4D9;if(_tmp4DE->tag != 4)goto _LL28A;}
_LL289: return 4;_LL28A: {struct Cyc_Absyn_PointerType_struct*_tmp4DF=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D9;if(_tmp4DF->tag != 5)goto _LL28C;}_LL28B: return 5;_LL28C: {struct Cyc_Absyn_IntType_struct*
_tmp4E0=(struct Cyc_Absyn_IntType_struct*)_tmp4D9;if(_tmp4E0->tag != 6)goto _LL28E;}
_LL28D: return 6;_LL28E: {struct Cyc_Absyn_FloatType_struct*_tmp4E1=(struct Cyc_Absyn_FloatType_struct*)
_tmp4D9;if(_tmp4E1->tag != 7)goto _LL290;}_LL28F: return 7;_LL290: {struct Cyc_Absyn_DoubleType_struct*
_tmp4E2=(struct Cyc_Absyn_DoubleType_struct*)_tmp4D9;if(_tmp4E2->tag != 8)goto
_LL292;}_LL291: return 8;_LL292: {struct Cyc_Absyn_ArrayType_struct*_tmp4E3=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4D9;if(_tmp4E3->tag != 9)goto _LL294;}_LL293:
return 9;_LL294: {struct Cyc_Absyn_FnType_struct*_tmp4E4=(struct Cyc_Absyn_FnType_struct*)
_tmp4D9;if(_tmp4E4->tag != 10)goto _LL296;}_LL295: return 10;_LL296: {struct Cyc_Absyn_TupleType_struct*
_tmp4E5=(struct Cyc_Absyn_TupleType_struct*)_tmp4D9;if(_tmp4E5->tag != 11)goto
_LL298;}_LL297: return 11;_LL298: {struct Cyc_Absyn_AggrType_struct*_tmp4E6=(struct
Cyc_Absyn_AggrType_struct*)_tmp4D9;if(_tmp4E6->tag != 12)goto _LL29A;}_LL299:
return 12;_LL29A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4E7=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4D9;if(_tmp4E7->tag != 13)goto _LL29C;}_LL29B: return 14;_LL29C: {struct Cyc_Absyn_EnumType_struct*
_tmp4E8=(struct Cyc_Absyn_EnumType_struct*)_tmp4D9;if(_tmp4E8->tag != 14)goto
_LL29E;}_LL29D: return 16;_LL29E: {struct Cyc_Absyn_AnonEnumType_struct*_tmp4E9=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp4D9;if(_tmp4E9->tag != 15)goto _LL2A0;}
_LL29F: return 17;_LL2A0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4EA=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4D9;if(_tmp4EA->tag != 16)goto _LL2A2;}_LL2A1: return 18;_LL2A2: {struct Cyc_Absyn_TypedefType_struct*
_tmp4EB=(struct Cyc_Absyn_TypedefType_struct*)_tmp4D9;if(_tmp4EB->tag != 18)goto
_LL2A4;}_LL2A3: return 19;_LL2A4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4EC=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp4D9;if(_tmp4EC->tag != 22)goto _LL2A6;}
_LL2A5: return 20;_LL2A6: {struct Cyc_Absyn_HeapRgn_struct*_tmp4ED=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp4D9;if(_tmp4ED->tag != 21)goto _LL2A8;}_LL2A7: return 21;_LL2A8: {struct Cyc_Absyn_AccessEff_struct*
_tmp4EE=(struct Cyc_Absyn_AccessEff_struct*)_tmp4D9;if(_tmp4EE->tag != 23)goto
_LL2AA;}_LL2A9: return 22;_LL2AA: {struct Cyc_Absyn_JoinEff_struct*_tmp4EF=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4D9;if(_tmp4EF->tag != 24)goto _LL2AC;}_LL2AB: return
23;_LL2AC: {struct Cyc_Absyn_RgnsEff_struct*_tmp4F0=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp4D9;if(_tmp4F0->tag != 25)goto _LL2AE;}_LL2AD: return 24;_LL2AE: {struct Cyc_Absyn_TagType_struct*
_tmp4F1=(struct Cyc_Absyn_TagType_struct*)_tmp4D9;if(_tmp4F1->tag != 20)goto _LL2B0;}
_LL2AF: return 26;_LL2B0: {struct Cyc_Absyn_DynRgnType_struct*_tmp4F2=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4D9;if(_tmp4F2->tag != 17)goto _LL2B2;}_LL2B1: return 27;_LL2B2: {struct Cyc_Absyn_ValueofType_struct*
_tmp4F3=(struct Cyc_Absyn_ValueofType_struct*)_tmp4D9;if(_tmp4F3->tag != 19)goto
_LL27F;}_LL2B3: return 28;_LL27F:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp4F4=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp4F4 != 0)return _tmp4F4;{struct _tuple0 _tmp10C2;struct _tuple0 _tmp4F6=(
_tmp10C2.f1=t2,((_tmp10C2.f2=t1,_tmp10C2)));void*_tmp4F7;void*_tmp4F9;void*
_tmp4FB;struct Cyc_Absyn_Tvar*_tmp4FD;void*_tmp4FE;struct Cyc_Absyn_Tvar*_tmp500;
void*_tmp501;struct Cyc_Absyn_AggrInfo _tmp503;union Cyc_Absyn_AggrInfoU _tmp504;
struct Cyc_List_List*_tmp505;void*_tmp506;struct Cyc_Absyn_AggrInfo _tmp508;union
Cyc_Absyn_AggrInfoU _tmp509;struct Cyc_List_List*_tmp50A;void*_tmp50B;struct
_tuple2*_tmp50D;void*_tmp50E;struct _tuple2*_tmp510;void*_tmp511;struct Cyc_List_List*
_tmp513;void*_tmp514;struct Cyc_List_List*_tmp516;void*_tmp517;struct Cyc_Absyn_DatatypeInfo
_tmp519;union Cyc_Absyn_DatatypeInfoU _tmp51A;struct Cyc_Absyn_Datatypedecl**
_tmp51B;struct Cyc_Absyn_Datatypedecl*_tmp51C;struct Cyc_List_List*_tmp51D;void*
_tmp51E;struct Cyc_Absyn_DatatypeInfo _tmp520;union Cyc_Absyn_DatatypeInfoU _tmp521;
struct Cyc_Absyn_Datatypedecl**_tmp522;struct Cyc_Absyn_Datatypedecl*_tmp523;
struct Cyc_List_List*_tmp524;void*_tmp525;struct Cyc_Absyn_DatatypeFieldInfo
_tmp527;union Cyc_Absyn_DatatypeFieldInfoU _tmp528;struct _tuple3 _tmp529;struct Cyc_Absyn_Datatypedecl*
_tmp52A;struct Cyc_Absyn_Datatypefield*_tmp52B;struct Cyc_List_List*_tmp52C;void*
_tmp52D;struct Cyc_Absyn_DatatypeFieldInfo _tmp52F;union Cyc_Absyn_DatatypeFieldInfoU
_tmp530;struct _tuple3 _tmp531;struct Cyc_Absyn_Datatypedecl*_tmp532;struct Cyc_Absyn_Datatypefield*
_tmp533;struct Cyc_List_List*_tmp534;void*_tmp535;struct Cyc_Absyn_PtrInfo _tmp537;
void*_tmp538;struct Cyc_Absyn_Tqual _tmp539;struct Cyc_Absyn_PtrAtts _tmp53A;void*
_tmp53B;union Cyc_Absyn_Constraint*_tmp53C;union Cyc_Absyn_Constraint*_tmp53D;
union Cyc_Absyn_Constraint*_tmp53E;void*_tmp53F;struct Cyc_Absyn_PtrInfo _tmp541;
void*_tmp542;struct Cyc_Absyn_Tqual _tmp543;struct Cyc_Absyn_PtrAtts _tmp544;void*
_tmp545;union Cyc_Absyn_Constraint*_tmp546;union Cyc_Absyn_Constraint*_tmp547;
union Cyc_Absyn_Constraint*_tmp548;void*_tmp549;enum Cyc_Absyn_Sign _tmp54B;enum 
Cyc_Absyn_Size_of _tmp54C;void*_tmp54D;enum Cyc_Absyn_Sign _tmp54F;enum Cyc_Absyn_Size_of
_tmp550;void*_tmp551;void*_tmp553;void*_tmp555;int _tmp557;void*_tmp558;int
_tmp55A;void*_tmp55B;struct Cyc_Absyn_ArrayInfo _tmp55D;void*_tmp55E;struct Cyc_Absyn_Tqual
_tmp55F;struct Cyc_Absyn_Exp*_tmp560;union Cyc_Absyn_Constraint*_tmp561;void*
_tmp562;struct Cyc_Absyn_ArrayInfo _tmp564;void*_tmp565;struct Cyc_Absyn_Tqual
_tmp566;struct Cyc_Absyn_Exp*_tmp567;union Cyc_Absyn_Constraint*_tmp568;void*
_tmp569;struct Cyc_Absyn_FnInfo _tmp56B;struct Cyc_List_List*_tmp56C;struct Cyc_Core_Opt*
_tmp56D;void*_tmp56E;struct Cyc_List_List*_tmp56F;int _tmp570;struct Cyc_Absyn_VarargInfo*
_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_List_List*_tmp573;void*_tmp574;
struct Cyc_Absyn_FnInfo _tmp576;struct Cyc_List_List*_tmp577;struct Cyc_Core_Opt*
_tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;int _tmp57B;struct Cyc_Absyn_VarargInfo*
_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_List_List*_tmp57E;void*_tmp57F;
struct Cyc_List_List*_tmp581;void*_tmp582;struct Cyc_List_List*_tmp584;void*
_tmp585;enum Cyc_Absyn_AggrKind _tmp587;struct Cyc_List_List*_tmp588;void*_tmp589;
enum Cyc_Absyn_AggrKind _tmp58B;struct Cyc_List_List*_tmp58C;void*_tmp58D;void*
_tmp58F;void*_tmp590;void*_tmp592;void*_tmp593;void*_tmp595;void*_tmp596;void*
_tmp597;void*_tmp599;void*_tmp59A;void*_tmp59B;void*_tmp59D;void*_tmp59E;void*
_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;void*_tmp5A4;struct Cyc_Absyn_Exp*
_tmp5A6;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;void*_tmp5AD;void*_tmp5AF;void*
_tmp5B1;_LL2B5: _tmp4F7=_tmp4F6.f1;{struct Cyc_Absyn_Evar_struct*_tmp4F8=(struct
Cyc_Absyn_Evar_struct*)_tmp4F7;if(_tmp4F8->tag != 1)goto _LL2B7;};_tmp4F9=_tmp4F6.f2;{
struct Cyc_Absyn_Evar_struct*_tmp4FA=(struct Cyc_Absyn_Evar_struct*)_tmp4F9;if(
_tmp4FA->tag != 1)goto _LL2B7;};_LL2B6: {const char*_tmp10C5;void*_tmp10C4;(
_tmp10C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10C5="typecmp: can only compare closed types",_tag_dyneither(_tmp10C5,
sizeof(char),39))),_tag_dyneither(_tmp10C4,sizeof(void*),0)));}_LL2B7: _tmp4FB=
_tmp4F6.f1;{struct Cyc_Absyn_VarType_struct*_tmp4FC=(struct Cyc_Absyn_VarType_struct*)
_tmp4FB;if(_tmp4FC->tag != 2)goto _LL2B9;else{_tmp4FD=_tmp4FC->f1;}};_tmp4FE=
_tmp4F6.f2;{struct Cyc_Absyn_VarType_struct*_tmp4FF=(struct Cyc_Absyn_VarType_struct*)
_tmp4FE;if(_tmp4FF->tag != 2)goto _LL2B9;else{_tmp500=_tmp4FF->f1;}};_LL2B8: return
Cyc_Core_intcmp(_tmp500->identity,_tmp4FD->identity);_LL2B9: _tmp501=_tmp4F6.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp502=(struct Cyc_Absyn_AggrType_struct*)
_tmp501;if(_tmp502->tag != 12)goto _LL2BB;else{_tmp503=_tmp502->f1;_tmp504=_tmp503.aggr_info;
_tmp505=_tmp503.targs;}};_tmp506=_tmp4F6.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp507=(struct Cyc_Absyn_AggrType_struct*)_tmp506;if(_tmp507->tag != 12)goto
_LL2BB;else{_tmp508=_tmp507->f1;_tmp509=_tmp508.aggr_info;_tmp50A=_tmp508.targs;}};
_LL2BA: {struct _tuple2*_tmp5B6;struct _tuple12 _tmp5B5=Cyc_Absyn_aggr_kinded_name(
_tmp504);_tmp5B6=_tmp5B5.f2;{struct _tuple2*_tmp5B8;struct _tuple12 _tmp5B7=Cyc_Absyn_aggr_kinded_name(
_tmp509);_tmp5B8=_tmp5B7.f2;{int _tmp5B9=Cyc_Absyn_qvar_cmp(_tmp5B6,_tmp5B8);if(
_tmp5B9 != 0)return _tmp5B9;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp505,_tmp50A);}};};}
_LL2BB: _tmp50B=_tmp4F6.f1;{struct Cyc_Absyn_EnumType_struct*_tmp50C=(struct Cyc_Absyn_EnumType_struct*)
_tmp50B;if(_tmp50C->tag != 14)goto _LL2BD;else{_tmp50D=_tmp50C->f1;}};_tmp50E=
_tmp4F6.f2;{struct Cyc_Absyn_EnumType_struct*_tmp50F=(struct Cyc_Absyn_EnumType_struct*)
_tmp50E;if(_tmp50F->tag != 14)goto _LL2BD;else{_tmp510=_tmp50F->f1;}};_LL2BC:
return Cyc_Absyn_qvar_cmp(_tmp50D,_tmp510);_LL2BD: _tmp511=_tmp4F6.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp512=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp511;if(_tmp512->tag != 15)goto
_LL2BF;else{_tmp513=_tmp512->f1;}};_tmp514=_tmp4F6.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp515=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp514;if(_tmp515->tag != 15)goto
_LL2BF;else{_tmp516=_tmp515->f1;}};_LL2BE: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp513,_tmp516);_LL2BF: _tmp517=_tmp4F6.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp518=(struct Cyc_Absyn_DatatypeType_struct*)_tmp517;if(_tmp518->tag != 3)goto
_LL2C1;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.datatype_info;if((_tmp51A.KnownDatatype).tag
!= 2)goto _LL2C1;_tmp51B=(struct Cyc_Absyn_Datatypedecl**)(_tmp51A.KnownDatatype).val;
_tmp51C=*_tmp51B;_tmp51D=_tmp519.targs;}};_tmp51E=_tmp4F6.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp51F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp51E;if(_tmp51F->tag != 3)goto
_LL2C1;else{_tmp520=_tmp51F->f1;_tmp521=_tmp520.datatype_info;if((_tmp521.KnownDatatype).tag
!= 2)goto _LL2C1;_tmp522=(struct Cyc_Absyn_Datatypedecl**)(_tmp521.KnownDatatype).val;
_tmp523=*_tmp522;_tmp524=_tmp520.targs;}};_LL2C0: if(_tmp523 == _tmp51C)return 0;{
int _tmp5BA=Cyc_Absyn_qvar_cmp(_tmp523->name,_tmp51C->name);if(_tmp5BA != 0)return
_tmp5BA;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp524,_tmp51D);};_LL2C1: _tmp525=
_tmp4F6.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp526=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp525;if(_tmp526->tag != 4)goto _LL2C3;else{_tmp527=_tmp526->f1;_tmp528=_tmp527.field_info;
if((_tmp528.KnownDatatypefield).tag != 2)goto _LL2C3;_tmp529=(struct _tuple3)(
_tmp528.KnownDatatypefield).val;_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;_tmp52C=
_tmp527.targs;}};_tmp52D=_tmp4F6.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp52E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp52D;if(_tmp52E->tag != 4)
goto _LL2C3;else{_tmp52F=_tmp52E->f1;_tmp530=_tmp52F.field_info;if((_tmp530.KnownDatatypefield).tag
!= 2)goto _LL2C3;_tmp531=(struct _tuple3)(_tmp530.KnownDatatypefield).val;_tmp532=
_tmp531.f1;_tmp533=_tmp531.f2;_tmp534=_tmp52F.targs;}};_LL2C2: if(_tmp532 == 
_tmp52A)return 0;{int _tmp5BB=Cyc_Absyn_qvar_cmp(_tmp52A->name,_tmp532->name);if(
_tmp5BB != 0)return _tmp5BB;{int _tmp5BC=Cyc_Absyn_qvar_cmp(_tmp52B->name,_tmp533->name);
if(_tmp5BC != 0)return _tmp5BC;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp534,_tmp52C);};};
_LL2C3: _tmp535=_tmp4F6.f1;{struct Cyc_Absyn_PointerType_struct*_tmp536=(struct Cyc_Absyn_PointerType_struct*)
_tmp535;if(_tmp536->tag != 5)goto _LL2C5;else{_tmp537=_tmp536->f1;_tmp538=_tmp537.elt_typ;
_tmp539=_tmp537.elt_tq;_tmp53A=_tmp537.ptr_atts;_tmp53B=_tmp53A.rgn;_tmp53C=
_tmp53A.nullable;_tmp53D=_tmp53A.bounds;_tmp53E=_tmp53A.zero_term;}};_tmp53F=
_tmp4F6.f2;{struct Cyc_Absyn_PointerType_struct*_tmp540=(struct Cyc_Absyn_PointerType_struct*)
_tmp53F;if(_tmp540->tag != 5)goto _LL2C5;else{_tmp541=_tmp540->f1;_tmp542=_tmp541.elt_typ;
_tmp543=_tmp541.elt_tq;_tmp544=_tmp541.ptr_atts;_tmp545=_tmp544.rgn;_tmp546=
_tmp544.nullable;_tmp547=_tmp544.bounds;_tmp548=_tmp544.zero_term;}};_LL2C4: {
int _tmp5BD=Cyc_Tcutil_typecmp(_tmp542,_tmp538);if(_tmp5BD != 0)return _tmp5BD;{int
_tmp5BE=Cyc_Tcutil_typecmp(_tmp545,_tmp53B);if(_tmp5BE != 0)return _tmp5BE;{int
_tmp5BF=Cyc_Tcutil_tqual_cmp(_tmp543,_tmp539);if(_tmp5BF != 0)return _tmp5BF;{int
_tmp5C0=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp547,_tmp53D);if(_tmp5C0 != 0)
return _tmp5C0;{int _tmp5C1=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp548,
_tmp53E);if(_tmp5C1 != 0)return _tmp5C1;{void*_tmp5C2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp547);_LL2EA: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5C3=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C2;if(_tmp5C3->tag != 0)goto
_LL2EC;}_LL2EB: return 0;_LL2EC:;_LL2ED: goto _LL2E9;_LL2E9:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp546,_tmp53C);};};};};}_LL2C5: _tmp549=_tmp4F6.f1;{struct Cyc_Absyn_IntType_struct*
_tmp54A=(struct Cyc_Absyn_IntType_struct*)_tmp549;if(_tmp54A->tag != 6)goto _LL2C7;
else{_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;}};_tmp54D=_tmp4F6.f2;{struct Cyc_Absyn_IntType_struct*
_tmp54E=(struct Cyc_Absyn_IntType_struct*)_tmp54D;if(_tmp54E->tag != 6)goto _LL2C7;
else{_tmp54F=_tmp54E->f1;_tmp550=_tmp54E->f2;}};_LL2C6: if(_tmp54F != _tmp54B)
return Cyc_Core_intcmp((int)((unsigned int)_tmp54F),(int)((unsigned int)_tmp54B));
if(_tmp550 != _tmp54C)return Cyc_Core_intcmp((int)((unsigned int)_tmp550),(int)((
unsigned int)_tmp54C));return 0;_LL2C7: _tmp551=_tmp4F6.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp552=(struct Cyc_Absyn_FloatType_struct*)_tmp551;if(_tmp552->tag != 7)goto
_LL2C9;};_tmp553=_tmp4F6.f2;{struct Cyc_Absyn_FloatType_struct*_tmp554=(struct Cyc_Absyn_FloatType_struct*)
_tmp553;if(_tmp554->tag != 7)goto _LL2C9;};_LL2C8: return 0;_LL2C9: _tmp555=_tmp4F6.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp556=(struct Cyc_Absyn_DoubleType_struct*)
_tmp555;if(_tmp556->tag != 8)goto _LL2CB;else{_tmp557=_tmp556->f1;}};_tmp558=
_tmp4F6.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp559=(struct Cyc_Absyn_DoubleType_struct*)
_tmp558;if(_tmp559->tag != 8)goto _LL2CB;else{_tmp55A=_tmp559->f1;}};_LL2CA: if(
_tmp557 == _tmp55A)return 0;else{if(_tmp557)return - 1;else{return 1;}}_LL2CB: _tmp55B=
_tmp4F6.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp55C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp55B;if(_tmp55C->tag != 9)goto _LL2CD;else{_tmp55D=_tmp55C->f1;_tmp55E=_tmp55D.elt_type;
_tmp55F=_tmp55D.tq;_tmp560=_tmp55D.num_elts;_tmp561=_tmp55D.zero_term;}};_tmp562=
_tmp4F6.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp563=(struct Cyc_Absyn_ArrayType_struct*)
_tmp562;if(_tmp563->tag != 9)goto _LL2CD;else{_tmp564=_tmp563->f1;_tmp565=_tmp564.elt_type;
_tmp566=_tmp564.tq;_tmp567=_tmp564.num_elts;_tmp568=_tmp564.zero_term;}};_LL2CC: {
int _tmp5C4=Cyc_Tcutil_tqual_cmp(_tmp566,_tmp55F);if(_tmp5C4 != 0)return _tmp5C4;{
int _tmp5C5=Cyc_Tcutil_typecmp(_tmp565,_tmp55E);if(_tmp5C5 != 0)return _tmp5C5;{int
_tmp5C6=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp561,_tmp568);if(_tmp5C6 != 0)
return _tmp5C6;if(_tmp560 == _tmp567)return 0;if(_tmp560 == 0  || _tmp567 == 0){const
char*_tmp10C8;void*_tmp10C7;(_tmp10C7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10C8="missing expression in array index",
_tag_dyneither(_tmp10C8,sizeof(char),34))),_tag_dyneither(_tmp10C7,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp560,_tmp567);};};}_LL2CD: _tmp569=_tmp4F6.f1;{struct Cyc_Absyn_FnType_struct*
_tmp56A=(struct Cyc_Absyn_FnType_struct*)_tmp569;if(_tmp56A->tag != 10)goto _LL2CF;
else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56B.tvars;_tmp56D=_tmp56B.effect;_tmp56E=
_tmp56B.ret_typ;_tmp56F=_tmp56B.args;_tmp570=_tmp56B.c_varargs;_tmp571=_tmp56B.cyc_varargs;
_tmp572=_tmp56B.rgn_po;_tmp573=_tmp56B.attributes;}};_tmp574=_tmp4F6.f2;{struct
Cyc_Absyn_FnType_struct*_tmp575=(struct Cyc_Absyn_FnType_struct*)_tmp574;if(
_tmp575->tag != 10)goto _LL2CF;else{_tmp576=_tmp575->f1;_tmp577=_tmp576.tvars;
_tmp578=_tmp576.effect;_tmp579=_tmp576.ret_typ;_tmp57A=_tmp576.args;_tmp57B=
_tmp576.c_varargs;_tmp57C=_tmp576.cyc_varargs;_tmp57D=_tmp576.rgn_po;_tmp57E=
_tmp576.attributes;}};_LL2CE: {const char*_tmp10CB;void*_tmp10CA;(_tmp10CA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10CB="typecmp: function types not handled",_tag_dyneither(_tmp10CB,sizeof(
char),36))),_tag_dyneither(_tmp10CA,sizeof(void*),0)));}_LL2CF: _tmp57F=_tmp4F6.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp580=(struct Cyc_Absyn_TupleType_struct*)
_tmp57F;if(_tmp580->tag != 11)goto _LL2D1;else{_tmp581=_tmp580->f1;}};_tmp582=
_tmp4F6.f2;{struct Cyc_Absyn_TupleType_struct*_tmp583=(struct Cyc_Absyn_TupleType_struct*)
_tmp582;if(_tmp583->tag != 11)goto _LL2D1;else{_tmp584=_tmp583->f1;}};_LL2D0:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp584,
_tmp581);_LL2D1: _tmp585=_tmp4F6.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp586=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp585;if(_tmp586->tag != 13)goto _LL2D3;
else{_tmp587=_tmp586->f1;_tmp588=_tmp586->f2;}};_tmp589=_tmp4F6.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp58A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp589;if(_tmp58A->tag != 13)goto
_LL2D3;else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58A->f2;}};_LL2D2: if(_tmp58B != 
_tmp587){if(_tmp58B == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp58C,
_tmp588);_LL2D3: _tmp58D=_tmp4F6.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp58E=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp58D;if(_tmp58E->tag != 16)goto _LL2D5;
else{_tmp58F=(void*)_tmp58E->f1;}};_tmp590=_tmp4F6.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp591=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp590;if(_tmp591->tag != 16)goto
_LL2D5;else{_tmp592=(void*)_tmp591->f1;}};_LL2D4: return Cyc_Tcutil_typecmp(
_tmp58F,_tmp592);_LL2D5: _tmp593=_tmp4F6.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp594=(struct Cyc_Absyn_DynRgnType_struct*)_tmp593;if(_tmp594->tag != 17)goto
_LL2D7;else{_tmp595=(void*)_tmp594->f1;_tmp596=(void*)_tmp594->f2;}};_tmp597=
_tmp4F6.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp598=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp597;if(_tmp598->tag != 17)goto _LL2D7;else{_tmp599=(void*)_tmp598->f1;_tmp59A=(
void*)_tmp598->f2;}};_LL2D6: {int _tmp5CB=Cyc_Tcutil_typecmp(_tmp595,_tmp599);if(
_tmp5CB != 0)return _tmp5CB;else{return Cyc_Tcutil_typecmp(_tmp596,_tmp59A);}}
_LL2D7: _tmp59B=_tmp4F6.f1;{struct Cyc_Absyn_TagType_struct*_tmp59C=(struct Cyc_Absyn_TagType_struct*)
_tmp59B;if(_tmp59C->tag != 20)goto _LL2D9;else{_tmp59D=(void*)_tmp59C->f1;}};
_tmp59E=_tmp4F6.f2;{struct Cyc_Absyn_TagType_struct*_tmp59F=(struct Cyc_Absyn_TagType_struct*)
_tmp59E;if(_tmp59F->tag != 20)goto _LL2D9;else{_tmp5A0=(void*)_tmp59F->f1;}};
_LL2D8: return Cyc_Tcutil_typecmp(_tmp59D,_tmp5A0);_LL2D9: _tmp5A1=_tmp4F6.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp5A2=(struct Cyc_Absyn_ValueofType_struct*)
_tmp5A1;if(_tmp5A2->tag != 19)goto _LL2DB;else{_tmp5A3=_tmp5A2->f1;}};_tmp5A4=
_tmp4F6.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp5A5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp5A4;if(_tmp5A5->tag != 19)goto _LL2DB;else{_tmp5A6=_tmp5A5->f1;}};_LL2DA:
return Cyc_Evexp_const_exp_cmp(_tmp5A3,_tmp5A6);_LL2DB: _tmp5A7=_tmp4F6.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp5A8=(struct Cyc_Absyn_JoinEff_struct*)_tmp5A7;if(
_tmp5A8->tag != 24)goto _LL2DD;};_LL2DC: goto _LL2DE;_LL2DD: _tmp5A9=_tmp4F6.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp5AA=(struct Cyc_Absyn_JoinEff_struct*)_tmp5A9;
if(_tmp5AA->tag != 24)goto _LL2DF;};_LL2DE: goto _LL2E0;_LL2DF: _tmp5AB=_tmp4F6.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp5AC=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5AB;if(_tmp5AC->tag != 23)goto _LL2E1;};_LL2E0: goto _LL2E2;_LL2E1: _tmp5AD=
_tmp4F6.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp5AE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5AD;if(_tmp5AE->tag != 25)goto _LL2E3;};_LL2E2: goto _LL2E4;_LL2E3: _tmp5AF=
_tmp4F6.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp5B0=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5AF;if(_tmp5B0->tag != 25)goto _LL2E5;};_LL2E4: goto _LL2E6;_LL2E5: _tmp5B1=
_tmp4F6.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp5B2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5B1;if(_tmp5B2->tag != 23)goto _LL2E7;};_LL2E6: {const char*_tmp10CE;void*
_tmp10CD;(_tmp10CD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10CE="typecmp: effects not handled",_tag_dyneither(
_tmp10CE,sizeof(char),29))),_tag_dyneither(_tmp10CD,sizeof(void*),0)));}_LL2E7:;
_LL2E8: {const char*_tmp10D1;void*_tmp10D0;(_tmp10D0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D1="Unmatched case in typecmp",
_tag_dyneither(_tmp10D1,sizeof(char),26))),_tag_dyneither(_tmp10D0,sizeof(void*),
0)));}_LL2B4:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp5D0=Cyc_Tcutil_compress(t);_LL2EF: {struct Cyc_Absyn_IntType_struct*
_tmp5D1=(struct Cyc_Absyn_IntType_struct*)_tmp5D0;if(_tmp5D1->tag != 6)goto _LL2F1;}
_LL2F0: goto _LL2F2;_LL2F1: {struct Cyc_Absyn_FloatType_struct*_tmp5D2=(struct Cyc_Absyn_FloatType_struct*)
_tmp5D0;if(_tmp5D2->tag != 7)goto _LL2F3;}_LL2F2: goto _LL2F4;_LL2F3: {struct Cyc_Absyn_DoubleType_struct*
_tmp5D3=(struct Cyc_Absyn_DoubleType_struct*)_tmp5D0;if(_tmp5D3->tag != 8)goto
_LL2F5;}_LL2F4: goto _LL2F6;_LL2F5: {struct Cyc_Absyn_EnumType_struct*_tmp5D4=(
struct Cyc_Absyn_EnumType_struct*)_tmp5D0;if(_tmp5D4->tag != 14)goto _LL2F7;}_LL2F6:
goto _LL2F8;_LL2F7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5D5=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5D0;if(_tmp5D5->tag != 15)goto _LL2F9;}_LL2F8: return 1;_LL2F9:;_LL2FA: return 0;
_LL2EE:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp10D2;struct _tuple0 _tmp5D7=(_tmp10D2.f1=t1,((_tmp10D2.f2=t2,_tmp10D2)));
void*_tmp5D8;int _tmp5DA;void*_tmp5DB;int _tmp5DD;void*_tmp5DE;void*_tmp5E0;void*
_tmp5E2;void*_tmp5E4;void*_tmp5E6;void*_tmp5E8;void*_tmp5EA;void*_tmp5EC;void*
_tmp5EE;void*_tmp5F0;void*_tmp5F2;enum Cyc_Absyn_Size_of _tmp5F4;void*_tmp5F5;
enum Cyc_Absyn_Size_of _tmp5F7;void*_tmp5F8;enum Cyc_Absyn_Size_of _tmp5FA;void*
_tmp5FB;enum Cyc_Absyn_Size_of _tmp5FD;void*_tmp5FE;enum Cyc_Absyn_Size_of _tmp600;
void*_tmp601;enum Cyc_Absyn_Size_of _tmp603;void*_tmp604;enum Cyc_Absyn_Size_of
_tmp606;void*_tmp607;enum Cyc_Absyn_Size_of _tmp609;void*_tmp60A;void*_tmp60C;
enum Cyc_Absyn_Size_of _tmp60E;void*_tmp60F;void*_tmp611;enum Cyc_Absyn_Size_of
_tmp613;void*_tmp614;enum Cyc_Absyn_Size_of _tmp616;void*_tmp617;enum Cyc_Absyn_Size_of
_tmp619;void*_tmp61A;enum Cyc_Absyn_Size_of _tmp61C;void*_tmp61D;void*_tmp61F;
enum Cyc_Absyn_Size_of _tmp621;void*_tmp622;enum Cyc_Absyn_Size_of _tmp624;void*
_tmp625;enum Cyc_Absyn_Size_of _tmp627;void*_tmp628;enum Cyc_Absyn_Size_of _tmp62A;
void*_tmp62B;enum Cyc_Absyn_Size_of _tmp62D;void*_tmp62E;enum Cyc_Absyn_Size_of
_tmp630;void*_tmp631;enum Cyc_Absyn_Size_of _tmp633;void*_tmp634;void*_tmp636;
enum Cyc_Absyn_Size_of _tmp638;_LL2FC: _tmp5D8=_tmp5D7.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5D9=(struct Cyc_Absyn_DoubleType_struct*)_tmp5D8;if(_tmp5D9->tag != 8)goto
_LL2FE;else{_tmp5DA=_tmp5D9->f1;}};_tmp5DB=_tmp5D7.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp5DC=(struct Cyc_Absyn_DoubleType_struct*)_tmp5DB;if(_tmp5DC->tag != 8)goto
_LL2FE;else{_tmp5DD=_tmp5DC->f1;}};_LL2FD: return !_tmp5DD  && _tmp5DA;_LL2FE:
_tmp5DE=_tmp5D7.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5DF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5DE;if(_tmp5DF->tag != 8)goto _LL300;};_tmp5E0=_tmp5D7.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp5E1=(struct Cyc_Absyn_FloatType_struct*)_tmp5E0;if(_tmp5E1->tag != 7)goto
_LL300;};_LL2FF: goto _LL301;_LL300: _tmp5E2=_tmp5D7.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5E3=(struct Cyc_Absyn_DoubleType_struct*)_tmp5E2;if(_tmp5E3->tag != 8)goto
_LL302;};_tmp5E4=_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp5E5=(struct Cyc_Absyn_IntType_struct*)
_tmp5E4;if(_tmp5E5->tag != 6)goto _LL302;};_LL301: goto _LL303;_LL302: _tmp5E6=
_tmp5D7.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5E7=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5E6;if(_tmp5E7->tag != 8)goto _LL304;};_tmp5E8=_tmp5D7.f2;{struct Cyc_Absyn_TagType_struct*
_tmp5E9=(struct Cyc_Absyn_TagType_struct*)_tmp5E8;if(_tmp5E9->tag != 20)goto _LL304;};
_LL303: goto _LL305;_LL304: _tmp5EA=_tmp5D7.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp5EB=(struct Cyc_Absyn_FloatType_struct*)_tmp5EA;if(_tmp5EB->tag != 7)goto
_LL306;};_tmp5EC=_tmp5D7.f2;{struct Cyc_Absyn_TagType_struct*_tmp5ED=(struct Cyc_Absyn_TagType_struct*)
_tmp5EC;if(_tmp5ED->tag != 20)goto _LL306;};_LL305: goto _LL307;_LL306: _tmp5EE=
_tmp5D7.f1;{struct Cyc_Absyn_FloatType_struct*_tmp5EF=(struct Cyc_Absyn_FloatType_struct*)
_tmp5EE;if(_tmp5EF->tag != 7)goto _LL308;};_tmp5F0=_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5F1=(struct Cyc_Absyn_IntType_struct*)_tmp5F0;if(_tmp5F1->tag != 6)goto _LL308;};
_LL307: return 1;_LL308: _tmp5F2=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*_tmp5F3=(
struct Cyc_Absyn_IntType_struct*)_tmp5F2;if(_tmp5F3->tag != 6)goto _LL30A;else{
_tmp5F4=_tmp5F3->f2;if(_tmp5F4 != Cyc_Absyn_LongLong_sz)goto _LL30A;}};_tmp5F5=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp5F6=(struct Cyc_Absyn_IntType_struct*)
_tmp5F5;if(_tmp5F6->tag != 6)goto _LL30A;else{_tmp5F7=_tmp5F6->f2;if(_tmp5F7 != Cyc_Absyn_LongLong_sz)
goto _LL30A;}};_LL309: return 0;_LL30A: _tmp5F8=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5F9=(struct Cyc_Absyn_IntType_struct*)_tmp5F8;if(_tmp5F9->tag != 6)goto _LL30C;
else{_tmp5FA=_tmp5F9->f2;if(_tmp5FA != Cyc_Absyn_LongLong_sz)goto _LL30C;}};_LL30B:
return 1;_LL30C: _tmp5FB=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*_tmp5FC=(
struct Cyc_Absyn_IntType_struct*)_tmp5FB;if(_tmp5FC->tag != 6)goto _LL30E;else{
_tmp5FD=_tmp5FC->f2;if(_tmp5FD != Cyc_Absyn_Long_sz)goto _LL30E;}};_tmp5FE=_tmp5D7.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5FF=(struct Cyc_Absyn_IntType_struct*)_tmp5FE;
if(_tmp5FF->tag != 6)goto _LL30E;else{_tmp600=_tmp5FF->f2;if(_tmp600 != Cyc_Absyn_Int_sz)
goto _LL30E;}};_LL30D: goto _LL30F;_LL30E: _tmp601=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp602=(struct Cyc_Absyn_IntType_struct*)_tmp601;if(_tmp602->tag != 6)goto _LL310;
else{_tmp603=_tmp602->f2;if(_tmp603 != Cyc_Absyn_Int_sz)goto _LL310;}};_tmp604=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp605=(struct Cyc_Absyn_IntType_struct*)
_tmp604;if(_tmp605->tag != 6)goto _LL310;else{_tmp606=_tmp605->f2;if(_tmp606 != Cyc_Absyn_Long_sz)
goto _LL310;}};_LL30F: return 0;_LL310: _tmp607=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp608=(struct Cyc_Absyn_IntType_struct*)_tmp607;if(_tmp608->tag != 6)goto _LL312;
else{_tmp609=_tmp608->f2;if(_tmp609 != Cyc_Absyn_Long_sz)goto _LL312;}};_tmp60A=
_tmp5D7.f2;{struct Cyc_Absyn_FloatType_struct*_tmp60B=(struct Cyc_Absyn_FloatType_struct*)
_tmp60A;if(_tmp60B->tag != 7)goto _LL312;};_LL311: goto _LL313;_LL312: _tmp60C=
_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*_tmp60D=(struct Cyc_Absyn_IntType_struct*)
_tmp60C;if(_tmp60D->tag != 6)goto _LL314;else{_tmp60E=_tmp60D->f2;if(_tmp60E != Cyc_Absyn_Int_sz)
goto _LL314;}};_tmp60F=_tmp5D7.f2;{struct Cyc_Absyn_FloatType_struct*_tmp610=(
struct Cyc_Absyn_FloatType_struct*)_tmp60F;if(_tmp610->tag != 7)goto _LL314;};
_LL313: goto _LL315;_LL314: _tmp611=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp612=(struct Cyc_Absyn_IntType_struct*)_tmp611;if(_tmp612->tag != 6)goto _LL316;
else{_tmp613=_tmp612->f2;if(_tmp613 != Cyc_Absyn_Long_sz)goto _LL316;}};_tmp614=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp615=(struct Cyc_Absyn_IntType_struct*)
_tmp614;if(_tmp615->tag != 6)goto _LL316;else{_tmp616=_tmp615->f2;if(_tmp616 != Cyc_Absyn_Short_sz)
goto _LL316;}};_LL315: goto _LL317;_LL316: _tmp617=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp618=(struct Cyc_Absyn_IntType_struct*)_tmp617;if(_tmp618->tag != 6)goto _LL318;
else{_tmp619=_tmp618->f2;if(_tmp619 != Cyc_Absyn_Int_sz)goto _LL318;}};_tmp61A=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp61B=(struct Cyc_Absyn_IntType_struct*)
_tmp61A;if(_tmp61B->tag != 6)goto _LL318;else{_tmp61C=_tmp61B->f2;if(_tmp61C != Cyc_Absyn_Short_sz)
goto _LL318;}};_LL317: goto _LL319;_LL318: _tmp61D=_tmp5D7.f1;{struct Cyc_Absyn_TagType_struct*
_tmp61E=(struct Cyc_Absyn_TagType_struct*)_tmp61D;if(_tmp61E->tag != 20)goto _LL31A;};
_tmp61F=_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp620=(struct Cyc_Absyn_IntType_struct*)
_tmp61F;if(_tmp620->tag != 6)goto _LL31A;else{_tmp621=_tmp620->f2;if(_tmp621 != Cyc_Absyn_Short_sz)
goto _LL31A;}};_LL319: goto _LL31B;_LL31A: _tmp622=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp623=(struct Cyc_Absyn_IntType_struct*)_tmp622;if(_tmp623->tag != 6)goto _LL31C;
else{_tmp624=_tmp623->f2;if(_tmp624 != Cyc_Absyn_Long_sz)goto _LL31C;}};_tmp625=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp626=(struct Cyc_Absyn_IntType_struct*)
_tmp625;if(_tmp626->tag != 6)goto _LL31C;else{_tmp627=_tmp626->f2;if(_tmp627 != Cyc_Absyn_Char_sz)
goto _LL31C;}};_LL31B: goto _LL31D;_LL31C: _tmp628=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp629=(struct Cyc_Absyn_IntType_struct*)_tmp628;if(_tmp629->tag != 6)goto _LL31E;
else{_tmp62A=_tmp629->f2;if(_tmp62A != Cyc_Absyn_Int_sz)goto _LL31E;}};_tmp62B=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp62C=(struct Cyc_Absyn_IntType_struct*)
_tmp62B;if(_tmp62C->tag != 6)goto _LL31E;else{_tmp62D=_tmp62C->f2;if(_tmp62D != Cyc_Absyn_Char_sz)
goto _LL31E;}};_LL31D: goto _LL31F;_LL31E: _tmp62E=_tmp5D7.f1;{struct Cyc_Absyn_IntType_struct*
_tmp62F=(struct Cyc_Absyn_IntType_struct*)_tmp62E;if(_tmp62F->tag != 6)goto _LL320;
else{_tmp630=_tmp62F->f2;if(_tmp630 != Cyc_Absyn_Short_sz)goto _LL320;}};_tmp631=
_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp632=(struct Cyc_Absyn_IntType_struct*)
_tmp631;if(_tmp632->tag != 6)goto _LL320;else{_tmp633=_tmp632->f2;if(_tmp633 != Cyc_Absyn_Char_sz)
goto _LL320;}};_LL31F: goto _LL321;_LL320: _tmp634=_tmp5D7.f1;{struct Cyc_Absyn_TagType_struct*
_tmp635=(struct Cyc_Absyn_TagType_struct*)_tmp634;if(_tmp635->tag != 20)goto _LL322;};
_tmp636=_tmp5D7.f2;{struct Cyc_Absyn_IntType_struct*_tmp637=(struct Cyc_Absyn_IntType_struct*)
_tmp636;if(_tmp637->tag != 6)goto _LL322;else{_tmp638=_tmp637->f2;if(_tmp638 != Cyc_Absyn_Char_sz)
goto _LL322;}};_LL321: return 1;_LL322:;_LL323: return 0;_LL2FB:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp639=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp10D3;max_arith_type=((_tmp10D3=
_region_malloc(_tmp639,sizeof(*_tmp10D3)),((_tmp10D3->v=t1,_tmp10D3))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp10D8;void*_tmp10D7[2];struct
Cyc_String_pa_struct _tmp10D6;struct Cyc_String_pa_struct _tmp10D5;(_tmp10D5.tag=0,((
_tmp10D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp10D6.tag=0,((_tmp10D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp10D7[0]=& _tmp10D6,((_tmp10D7[1]=& _tmp10D5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp10D8="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp10D8,sizeof(char),41))),_tag_dyneither(_tmp10D7,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp63F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL325: {struct Cyc_Absyn_PointerType_struct*
_tmp640=(struct Cyc_Absyn_PointerType_struct*)_tmp63F;if(_tmp640->tag != 5)goto
_LL327;}_LL326: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL324;_LL327:;_LL328: return 0;_LL324:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp641=Cyc_Tcutil_compress(
t);_LL32A: {struct Cyc_Absyn_IntType_struct*_tmp642=(struct Cyc_Absyn_IntType_struct*)
_tmp641;if(_tmp642->tag != 6)goto _LL32C;}_LL32B: goto _LL32D;_LL32C: {struct Cyc_Absyn_TagType_struct*
_tmp643=(struct Cyc_Absyn_TagType_struct*)_tmp641;if(_tmp643->tag != 20)goto _LL32E;}
_LL32D: goto _LL32F;_LL32E: {struct Cyc_Absyn_EnumType_struct*_tmp644=(struct Cyc_Absyn_EnumType_struct*)
_tmp641;if(_tmp644->tag != 14)goto _LL330;}_LL32F: goto _LL331;_LL330: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp645=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp641;if(_tmp645->tag != 15)goto
_LL332;}_LL331: return 1;_LL332:;_LL333: return 0;_LL329:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp10DB;void*_tmp10DA;(_tmp10DA=0,Cyc_Tcutil_warn(e->loc,((_tmp10DB="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp10DB,sizeof(char),44))),_tag_dyneither(_tmp10DA,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp10DE;void*_tmp10DD;(
_tmp10DD=0,Cyc_Tcutil_warn(e->loc,((_tmp10DE="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp10DE,sizeof(char),44))),_tag_dyneither(_tmp10DD,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp10DF;struct _tuple0 _tmp64B=(
_tmp10DF.f1=t1,((_tmp10DF.f2=t2,_tmp10DF)));void*_tmp64C;struct Cyc_Absyn_PtrInfo
_tmp64E;void*_tmp64F;struct Cyc_Absyn_PtrInfo _tmp651;void*_tmp652;struct Cyc_Absyn_ArrayInfo
_tmp654;void*_tmp655;struct Cyc_Absyn_Tqual _tmp656;struct Cyc_Absyn_Exp*_tmp657;
union Cyc_Absyn_Constraint*_tmp658;void*_tmp659;struct Cyc_Absyn_ArrayInfo _tmp65B;
void*_tmp65C;struct Cyc_Absyn_Tqual _tmp65D;struct Cyc_Absyn_Exp*_tmp65E;union Cyc_Absyn_Constraint*
_tmp65F;void*_tmp660;void*_tmp662;_LL335: _tmp64C=_tmp64B.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64D=(struct Cyc_Absyn_PointerType_struct*)_tmp64C;if(_tmp64D->tag != 5)goto
_LL337;else{_tmp64E=_tmp64D->f1;}};_tmp64F=_tmp64B.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp650=(struct Cyc_Absyn_PointerType_struct*)_tmp64F;if(_tmp650->tag != 5)goto
_LL337;else{_tmp651=_tmp650->f1;}};_LL336: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp64E.ptr_atts).nullable,(_tmp651.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64E.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp64E.ptr_atts).bounds,(
_tmp651.ptr_atts).bounds)){struct _tuple0 _tmp10E0;struct _tuple0 _tmp665=(_tmp10E0.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp64E.ptr_atts).bounds),((_tmp10E0.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp651.ptr_atts).bounds),
_tmp10E0)));void*_tmp666;void*_tmp668;void*_tmp66A;void*_tmp66C;void*_tmp66E;
struct Cyc_Absyn_Exp*_tmp670;void*_tmp671;struct Cyc_Absyn_Exp*_tmp673;void*
_tmp674;void*_tmp676;struct Cyc_Absyn_Exp*_tmp678;_LL33E: _tmp666=_tmp665.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp667=(struct Cyc_Absyn_Upper_b_struct*)_tmp666;
if(_tmp667->tag != 1)goto _LL340;};_tmp668=_tmp665.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp669=(struct Cyc_Absyn_DynEither_b_struct*)_tmp668;if(_tmp669->tag != 0)goto
_LL340;};_LL33F: goto _LL341;_LL340: _tmp66A=_tmp665.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp66B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp66A;if(_tmp66B->tag != 0)goto
_LL342;};_tmp66C=_tmp665.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp66D=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp66C;if(_tmp66D->tag != 0)goto _LL342;};_LL341:
okay=1;goto _LL33D;_LL342: _tmp66E=_tmp665.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp66F=(struct Cyc_Absyn_Upper_b_struct*)_tmp66E;if(_tmp66F->tag != 1)goto _LL344;
else{_tmp670=_tmp66F->f1;}};_tmp671=_tmp665.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp672=(struct Cyc_Absyn_Upper_b_struct*)_tmp671;if(_tmp672->tag != 1)goto _LL344;
else{_tmp673=_tmp672->f1;}};_LL343: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp673,
_tmp670);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp651.ptr_atts).zero_term)){const char*_tmp10E3;void*_tmp10E2;(_tmp10E2=0,
Cyc_Tcutil_warn(loc,((_tmp10E3="implicit cast to shorter array",_tag_dyneither(
_tmp10E3,sizeof(char),31))),_tag_dyneither(_tmp10E2,sizeof(void*),0)));}goto
_LL33D;_LL344: _tmp674=_tmp665.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp675=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp674;if(_tmp675->tag != 0)goto _LL33D;};
_tmp676=_tmp665.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp677=(struct Cyc_Absyn_Upper_b_struct*)
_tmp676;if(_tmp677->tag != 1)goto _LL33D;else{_tmp678=_tmp677->f1;}};_LL345: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64E.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp651.ptr_atts).bounds))goto _LL33D;okay=0;goto
_LL33D;_LL33D:;}okay=okay  && (!(_tmp64E.elt_tq).real_const  || (_tmp651.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp64E.ptr_atts).rgn,(_tmp651.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp64E.ptr_atts).rgn,(_tmp651.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp64E.ptr_atts).zero_term,(
_tmp651.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp64E.ptr_atts).zero_term) && (_tmp651.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp64E.elt_typ,_tmp651.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp64E.elt_typ,_tmp651.elt_typ));return okay;}_LL337: _tmp652=_tmp64B.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp653=(struct Cyc_Absyn_ArrayType_struct*)_tmp652;
if(_tmp653->tag != 9)goto _LL339;else{_tmp654=_tmp653->f1;_tmp655=_tmp654.elt_type;
_tmp656=_tmp654.tq;_tmp657=_tmp654.num_elts;_tmp658=_tmp654.zero_term;}};_tmp659=
_tmp64B.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp65A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp659;if(_tmp65A->tag != 9)goto _LL339;else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65B.elt_type;
_tmp65D=_tmp65B.tq;_tmp65E=_tmp65B.num_elts;_tmp65F=_tmp65B.zero_term;}};_LL338: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp658,_tmp65F) && ((_tmp657 != 0
 && _tmp65E != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp657,(
struct Cyc_Absyn_Exp*)_tmp65E));return(okay  && Cyc_Tcutil_unify(_tmp655,_tmp65C))
 && (!_tmp656.real_const  || _tmp65D.real_const);}_LL339: _tmp660=_tmp64B.f1;{
struct Cyc_Absyn_TagType_struct*_tmp661=(struct Cyc_Absyn_TagType_struct*)_tmp660;
if(_tmp661->tag != 20)goto _LL33B;};_tmp662=_tmp64B.f2;{struct Cyc_Absyn_IntType_struct*
_tmp663=(struct Cyc_Absyn_IntType_struct*)_tmp662;if(_tmp663->tag != 6)goto _LL33B;};
_LL33A: return 0;_LL33B:;_LL33C: return Cyc_Tcutil_unify(t1,t2);_LL334:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp67B=Cyc_Tcutil_compress(t);
_LL347: {struct Cyc_Absyn_PointerType_struct*_tmp67C=(struct Cyc_Absyn_PointerType_struct*)
_tmp67B;if(_tmp67C->tag != 5)goto _LL349;}_LL348: return 1;_LL349:;_LL34A: return 0;
_LL346:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp67D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp67F;void*
_tmp680;_LL34C: {struct Cyc_Absyn_PointerType_struct*_tmp67E=(struct Cyc_Absyn_PointerType_struct*)
_tmp67D;if(_tmp67E->tag != 5)goto _LL34E;else{_tmp67F=_tmp67E->f1;_tmp680=_tmp67F.elt_typ;}}
_LL34D: return _tmp680;_LL34E:;_LL34F: {const char*_tmp10E6;void*_tmp10E5;(_tmp10E5=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10E6="pointer_elt_type",_tag_dyneither(_tmp10E6,sizeof(char),17))),
_tag_dyneither(_tmp10E5,sizeof(void*),0)));}_LL34B:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp683=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp685;struct Cyc_Absyn_PtrAtts _tmp686;struct Cyc_Absyn_PtrAtts*
_tmp687;_LL351: {struct Cyc_Absyn_PointerType_struct*_tmp684=(struct Cyc_Absyn_PointerType_struct*)
_tmp683;if(_tmp684->tag != 5)goto _LL353;else{_tmp685=_tmp684->f1;_tmp686=_tmp685.ptr_atts;
_tmp687=(struct Cyc_Absyn_PtrAtts*)&(_tmp684->f1).ptr_atts;}}_LL352: return _tmp687->rgn;
_LL353:;_LL354: {const char*_tmp10E9;void*_tmp10E8;(_tmp10E8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E9="pointer_elt_type",
_tag_dyneither(_tmp10E9,sizeof(char),17))),_tag_dyneither(_tmp10E8,sizeof(void*),
0)));}_LL350:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp68A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp68C;struct Cyc_Absyn_PtrAtts _tmp68D;void*_tmp68E;_LL356: {struct Cyc_Absyn_PointerType_struct*
_tmp68B=(struct Cyc_Absyn_PointerType_struct*)_tmp68A;if(_tmp68B->tag != 5)goto
_LL358;else{_tmp68C=_tmp68B->f1;_tmp68D=_tmp68C.ptr_atts;_tmp68E=_tmp68D.rgn;}}
_LL357:*rgn=_tmp68E;return 1;_LL358:;_LL359: return 0;_LL355:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp68F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp691;struct Cyc_Absyn_PtrAtts _tmp692;union Cyc_Absyn_Constraint*_tmp693;_LL35B: {
struct Cyc_Absyn_PointerType_struct*_tmp690=(struct Cyc_Absyn_PointerType_struct*)
_tmp68F;if(_tmp690->tag != 5)goto _LL35D;else{_tmp691=_tmp690->f1;_tmp692=_tmp691.ptr_atts;
_tmp693=_tmp692.bounds;}}_LL35C:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp693)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL35D:;_LL35E: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL35A:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp694=e->r;union Cyc_Absyn_Cnst _tmp696;struct
_tuple7 _tmp697;int _tmp698;union Cyc_Absyn_Cnst _tmp69A;struct _tuple5 _tmp69B;char
_tmp69C;union Cyc_Absyn_Cnst _tmp69E;struct _tuple6 _tmp69F;short _tmp6A0;union Cyc_Absyn_Cnst
_tmp6A2;struct _tuple8 _tmp6A3;long long _tmp6A4;void*_tmp6A6;struct Cyc_Absyn_Exp*
_tmp6A7;_LL360: {struct Cyc_Absyn_Const_e_struct*_tmp695=(struct Cyc_Absyn_Const_e_struct*)
_tmp694;if(_tmp695->tag != 0)goto _LL362;else{_tmp696=_tmp695->f1;if((_tmp696.Int_c).tag
!= 4)goto _LL362;_tmp697=(struct _tuple7)(_tmp696.Int_c).val;_tmp698=_tmp697.f2;
if(_tmp698 != 0)goto _LL362;}}_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_Const_e_struct*
_tmp699=(struct Cyc_Absyn_Const_e_struct*)_tmp694;if(_tmp699->tag != 0)goto _LL364;
else{_tmp69A=_tmp699->f1;if((_tmp69A.Char_c).tag != 2)goto _LL364;_tmp69B=(struct
_tuple5)(_tmp69A.Char_c).val;_tmp69C=_tmp69B.f2;if(_tmp69C != 0)goto _LL364;}}
_LL363: goto _LL365;_LL364: {struct Cyc_Absyn_Const_e_struct*_tmp69D=(struct Cyc_Absyn_Const_e_struct*)
_tmp694;if(_tmp69D->tag != 0)goto _LL366;else{_tmp69E=_tmp69D->f1;if((_tmp69E.Short_c).tag
!= 3)goto _LL366;_tmp69F=(struct _tuple6)(_tmp69E.Short_c).val;_tmp6A0=_tmp69F.f2;
if(_tmp6A0 != 0)goto _LL366;}}_LL365: goto _LL367;_LL366: {struct Cyc_Absyn_Const_e_struct*
_tmp6A1=(struct Cyc_Absyn_Const_e_struct*)_tmp694;if(_tmp6A1->tag != 0)goto _LL368;
else{_tmp6A2=_tmp6A1->f1;if((_tmp6A2.LongLong_c).tag != 5)goto _LL368;_tmp6A3=(
struct _tuple8)(_tmp6A2.LongLong_c).val;_tmp6A4=_tmp6A3.f2;if(_tmp6A4 != 0)goto
_LL368;}}_LL367: return 1;_LL368: {struct Cyc_Absyn_Cast_e_struct*_tmp6A5=(struct
Cyc_Absyn_Cast_e_struct*)_tmp694;if(_tmp6A5->tag != 15)goto _LL36A;else{_tmp6A6=(
void*)_tmp6A5->f1;_tmp6A7=_tmp6A5->f2;}}_LL369: return Cyc_Tcutil_is_zero(_tmp6A7)
 && Cyc_Tcutil_admits_zero(_tmp6A6);_LL36A:;_LL36B: return 0;_LL35F:;}struct Cyc_Absyn_Kind
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
Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6A9;
enum Cyc_Absyn_KindQual _tmp6AA;enum Cyc_Absyn_AliasQual _tmp6AB;struct Cyc_Absyn_Kind*
_tmp6A8=ka;_tmp6A9=*_tmp6A8;_tmp6AA=_tmp6A9.kind;_tmp6AB=_tmp6A9.aliasqual;
switch(_tmp6AB){case Cyc_Absyn_Aliasable: _LL36C: switch(_tmp6AA){case Cyc_Absyn_AnyKind:
_LL36E: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL36F:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL370: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL371: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL372: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL373: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL36D: switch(_tmp6AA){case Cyc_Absyn_AnyKind: _LL376: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL377: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL378: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL379: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL37A: break;}break;case Cyc_Absyn_Top: _LL375: switch(_tmp6AA){case Cyc_Absyn_AnyKind:
_LL37D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL37E:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL37F: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL380: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL381: break;}break;}{const char*_tmp10ED;
void*_tmp10EC[1];struct Cyc_String_pa_struct _tmp10EB;(_tmp10EB.tag=0,((_tmp10EB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp10EC[0]=& _tmp10EB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10ED="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp10ED,sizeof(char),26))),_tag_dyneither(_tmp10EC,sizeof(void*),1)))))));};}
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
_tmp6B0;enum Cyc_Absyn_KindQual _tmp6B1;enum Cyc_Absyn_AliasQual _tmp6B2;struct Cyc_Absyn_Kind*
_tmp6AF=ka;_tmp6B0=*_tmp6AF;_tmp6B1=_tmp6B0.kind;_tmp6B2=_tmp6B0.aliasqual;
switch(_tmp6B2){case Cyc_Absyn_Aliasable: _LL383: switch(_tmp6B1){case Cyc_Absyn_AnyKind:
_LL385: return& ab;case Cyc_Absyn_MemKind: _LL386: return& mb;case Cyc_Absyn_BoxKind:
_LL387: return& bb;case Cyc_Absyn_RgnKind: _LL388: return& rb;case Cyc_Absyn_EffKind:
_LL389: return& eb;case Cyc_Absyn_IntKind: _LL38A: return& ib;}case Cyc_Absyn_Unique:
_LL384: switch(_tmp6B1){case Cyc_Absyn_AnyKind: _LL38D: return& uab;case Cyc_Absyn_MemKind:
_LL38E: return& umb;case Cyc_Absyn_BoxKind: _LL38F: return& ubb;case Cyc_Absyn_RgnKind:
_LL390: return& urb;default: _LL391: break;}break;case Cyc_Absyn_Top: _LL38C: switch(
_tmp6B1){case Cyc_Absyn_AnyKind: _LL394: return& tab;case Cyc_Absyn_MemKind: _LL395:
return& tmb;case Cyc_Absyn_BoxKind: _LL396: return& tbb;case Cyc_Absyn_RgnKind: _LL397:
return& trb;default: _LL398: break;}break;}{const char*_tmp10F1;void*_tmp10F0[1];
struct Cyc_String_pa_struct _tmp10EF;(_tmp10EF.tag=0,((_tmp10EF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10F0[0]=&
_tmp10EF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10F1="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp10F1,sizeof(char),24))),
_tag_dyneither(_tmp10F0,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct
Cyc_Absyn_Kind*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp10F4;struct Cyc_Absyn_Const_e_struct*
_tmp10F3;e1->r=(void*)((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3[0]=((
_tmp10F4.tag=0,((_tmp10F4.f1=Cyc_Absyn_Null_c,_tmp10F4)))),_tmp10F3))));}{struct
Cyc_Core_Opt*_tmp6C6=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp10FE;struct Cyc_Absyn_PtrAtts _tmp10FD;struct Cyc_Absyn_PtrInfo _tmp10FC;struct
Cyc_Absyn_PointerType_struct*_tmp10FB;struct Cyc_Absyn_PointerType_struct*_tmp6C7=(
_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FE.tag=5,((_tmp10FE.f1=((
_tmp10FC.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp6C6),((
_tmp10FC.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp10FC.ptr_atts=((_tmp10FD.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp6C6),((_tmp10FD.nullable=Cyc_Absyn_true_conref,((
_tmp10FD.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp10FD.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp10FD.ptrloc=0,_tmp10FD)))))))))),_tmp10FC)))))),_tmp10FE)))),_tmp10FB)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp6C7);return Cyc_Tcutil_coerce_arg(
te,e1,t2);};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion
c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL39A: {const char*_tmp10FF;return(
_tmp10FF="unknown",_tag_dyneither(_tmp10FF,sizeof(char),8));}case Cyc_Absyn_No_coercion:
_LL39B: {const char*_tmp1100;return(_tmp1100="no coercion",_tag_dyneither(
_tmp1100,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL39C: {const char*
_tmp1101;return(_tmp1101="null check",_tag_dyneither(_tmp1101,sizeof(char),11));}
case Cyc_Absyn_Other_coercion: _LL39D: {const char*_tmp1102;return(_tmp1102="other coercion",
_tag_dyneither(_tmp1102,sizeof(char),15));}}}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);enum Cyc_Absyn_Coercion c;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1107;void*_tmp1106[2];
struct Cyc_String_pa_struct _tmp1105;struct Cyc_String_pa_struct _tmp1104;(_tmp1104.tag=
0,((_tmp1104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1105.tag=0,((_tmp1105.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1106[0]=& _tmp1105,((_tmp1106[1]=& _tmp1104,Cyc_Tcutil_warn(
e->loc,((_tmp1107="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1107,sizeof(char),53))),_tag_dyneither(_tmp1106,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,Cyc_Absyn_Other_coercion);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*
_tmp110C;void*_tmp110B[2];struct Cyc_String_pa_struct _tmp110A;struct Cyc_String_pa_struct
_tmp1109;(_tmp1109.tag=0,((_tmp1109.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp110A.tag=0,((_tmp110A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp110B[0]=& _tmp110A,((_tmp110B[1]=& _tmp1109,Cyc_Tcutil_warn(e->loc,((_tmp110C="implicit cast from %s to %s",
_tag_dyneither(_tmp110C,sizeof(char),28))),_tag_dyneither(_tmp110B,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp6D8=Cyc_Tcutil_compress(t);
_LL3A0: {struct Cyc_Absyn_IntType_struct*_tmp6D9=(struct Cyc_Absyn_IntType_struct*)
_tmp6D8;if(_tmp6D9->tag != 6)goto _LL3A2;}_LL3A1: goto _LL3A3;_LL3A2: {struct Cyc_Absyn_FloatType_struct*
_tmp6DA=(struct Cyc_Absyn_FloatType_struct*)_tmp6D8;if(_tmp6DA->tag != 7)goto
_LL3A4;}_LL3A3: goto _LL3A5;_LL3A4: {struct Cyc_Absyn_DoubleType_struct*_tmp6DB=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6D8;if(_tmp6DB->tag != 8)goto _LL3A6;}
_LL3A5: return 1;_LL3A6:;_LL3A7: return 0;_LL39F:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp6DD;
struct _RegionHandle*_tmp6DE;struct _tuple15 _tmp6DC=*env;_tmp6DD=_tmp6DC.f1;
_tmp6DE=_tmp6DC.f2;{struct _tuple11*_tmp110D;return(_tmp110D=_region_malloc(
_tmp6DE,sizeof(*_tmp110D)),((_tmp110D->f1=x->tq,((_tmp110D->f2=Cyc_Tcutil_rsubstitute(
_tmp6DE,_tmp6DD,x->type),_tmp110D)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp6E1;void*
_tmp6E2;struct _tuple11 _tmp6E0=*x;_tmp6E1=_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;{struct
_tuple11*_tmp110E;return(_tmp110E=_region_malloc(r,sizeof(*_tmp110E)),((_tmp110E->f1=
_tmp6E1,((_tmp110E->f2=_tmp6E2,_tmp110E)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp6E4=t1;struct Cyc_List_List*_tmp6E7;struct Cyc_Absyn_AggrInfo
_tmp6E9;union Cyc_Absyn_AggrInfoU _tmp6EA;struct Cyc_Absyn_Aggrdecl**_tmp6EB;struct
Cyc_Absyn_Aggrdecl*_tmp6EC;struct Cyc_List_List*_tmp6ED;enum Cyc_Absyn_AggrKind
_tmp6EF;struct Cyc_List_List*_tmp6F0;_LL3A9: {struct Cyc_Absyn_VoidType_struct*
_tmp6E5=(struct Cyc_Absyn_VoidType_struct*)_tmp6E4;if(_tmp6E5->tag != 0)goto _LL3AB;}
_LL3AA: return 0;_LL3AB: {struct Cyc_Absyn_TupleType_struct*_tmp6E6=(struct Cyc_Absyn_TupleType_struct*)
_tmp6E4;if(_tmp6E6->tag != 11)goto _LL3AD;else{_tmp6E7=_tmp6E6->f1;}}_LL3AC: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp6E7);_LL3AD: {struct Cyc_Absyn_AggrType_struct*
_tmp6E8=(struct Cyc_Absyn_AggrType_struct*)_tmp6E4;if(_tmp6E8->tag != 12)goto
_LL3AF;else{_tmp6E9=_tmp6E8->f1;_tmp6EA=_tmp6E9.aggr_info;if((_tmp6EA.KnownAggr).tag
!= 2)goto _LL3AF;_tmp6EB=(struct Cyc_Absyn_Aggrdecl**)(_tmp6EA.KnownAggr).val;
_tmp6EC=*_tmp6EB;_tmp6ED=_tmp6E9.targs;}}_LL3AE: if(((_tmp6EC->kind == Cyc_Absyn_UnionA
 || _tmp6EC->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6EC->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6EC->impl))->rgn_po != 0){
struct _tuple11*_tmp1111;struct Cyc_List_List*_tmp1110;return(_tmp1110=
_region_malloc(r,sizeof(*_tmp1110)),((_tmp1110->hd=((_tmp1111=_region_malloc(r,
sizeof(*_tmp1111)),((_tmp1111->f1=Cyc_Absyn_const_tqual(0),((_tmp1111->f2=t1,
_tmp1111)))))),((_tmp1110->tl=0,_tmp1110)))));}{struct Cyc_List_List*_tmp6F3=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6EC->tvs,_tmp6ED);struct _tuple15
_tmp1112;struct _tuple15 env=(_tmp1112.f1=_tmp6F3,((_tmp1112.f2=r,_tmp1112)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6EC->impl))->fields);};_LL3AF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6EE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6E4;if(_tmp6EE->tag != 13)goto
_LL3B1;else{_tmp6EF=_tmp6EE->f1;if(_tmp6EF != Cyc_Absyn_StructA)goto _LL3B1;
_tmp6F0=_tmp6EE->f2;}}_LL3B0: {struct _tuple15 _tmp1113;struct _tuple15 env=(
_tmp1113.f1=0,((_tmp1113.f2=r,_tmp1113)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp6F0);}_LL3B1:;_LL3B2: {struct _tuple11*_tmp1116;struct Cyc_List_List*
_tmp1115;return(_tmp1115=_region_malloc(r,sizeof(*_tmp1115)),((_tmp1115->hd=((
_tmp1116=_region_malloc(r,sizeof(*_tmp1116)),((_tmp1116->f1=Cyc_Absyn_const_tqual(
0),((_tmp1116->f2=t1,_tmp1116)))))),((_tmp1115->tl=0,_tmp1115)))));}_LL3A8:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp6F8=(void*)t->hd;_LL3B4: {
struct Cyc_Absyn_Pure_att_struct*_tmp6F9=(struct Cyc_Absyn_Pure_att_struct*)
_tmp6F8;if(_tmp6F9->tag != 22)goto _LL3B6;}_LL3B5: goto _LL3B7;_LL3B6: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp6FA=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp6F8;if(_tmp6FA->tag != 4)goto
_LL3B8;}_LL3B7: goto _LL3B9;_LL3B8: {struct Cyc_Absyn_Initializes_att_struct*
_tmp6FB=(struct Cyc_Absyn_Initializes_att_struct*)_tmp6F8;if(_tmp6FB->tag != 20)
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
_tmp1117;struct _tuple0 _tmp6FD=(_tmp1117.f1=t1,((_tmp1117.f2=t2,_tmp1117)));void*
_tmp6FE;struct Cyc_Absyn_PtrInfo _tmp700;void*_tmp701;struct Cyc_Absyn_Tqual _tmp702;
struct Cyc_Absyn_PtrAtts _tmp703;void*_tmp704;union Cyc_Absyn_Constraint*_tmp705;
union Cyc_Absyn_Constraint*_tmp706;union Cyc_Absyn_Constraint*_tmp707;void*_tmp708;
struct Cyc_Absyn_PtrInfo _tmp70A;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;struct
Cyc_Absyn_PtrAtts _tmp70D;void*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*
_tmp710;union Cyc_Absyn_Constraint*_tmp711;void*_tmp712;struct Cyc_Absyn_DatatypeFieldInfo
_tmp714;union Cyc_Absyn_DatatypeFieldInfoU _tmp715;struct _tuple3 _tmp716;struct Cyc_Absyn_Datatypedecl*
_tmp717;struct Cyc_Absyn_Datatypefield*_tmp718;struct Cyc_List_List*_tmp719;void*
_tmp71A;struct Cyc_Absyn_DatatypeInfo _tmp71C;union Cyc_Absyn_DatatypeInfoU _tmp71D;
struct Cyc_Absyn_Datatypedecl**_tmp71E;struct Cyc_Absyn_Datatypedecl*_tmp71F;
struct Cyc_List_List*_tmp720;void*_tmp721;struct Cyc_Absyn_FnInfo _tmp723;void*
_tmp724;struct Cyc_Absyn_FnInfo _tmp726;_LL3BD: _tmp6FE=_tmp6FD.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6FF=(struct Cyc_Absyn_PointerType_struct*)_tmp6FE;if(_tmp6FF->tag != 5)goto
_LL3BF;else{_tmp700=_tmp6FF->f1;_tmp701=_tmp700.elt_typ;_tmp702=_tmp700.elt_tq;
_tmp703=_tmp700.ptr_atts;_tmp704=_tmp703.rgn;_tmp705=_tmp703.nullable;_tmp706=
_tmp703.bounds;_tmp707=_tmp703.zero_term;}};_tmp708=_tmp6FD.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp709=(struct Cyc_Absyn_PointerType_struct*)_tmp708;if(_tmp709->tag != 5)goto
_LL3BF;else{_tmp70A=_tmp709->f1;_tmp70B=_tmp70A.elt_typ;_tmp70C=_tmp70A.elt_tq;
_tmp70D=_tmp70A.ptr_atts;_tmp70E=_tmp70D.rgn;_tmp70F=_tmp70D.nullable;_tmp710=
_tmp70D.bounds;_tmp711=_tmp70D.zero_term;}};_LL3BE: if(_tmp702.real_const  && !
_tmp70C.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp705,
_tmp70F) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp705)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp70F))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp707,
_tmp711) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp707)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp711))return 0;if(!Cyc_Tcutil_unify(_tmp704,_tmp70E) && !Cyc_Tcenv_region_outlives(
te,_tmp704,_tmp70E))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp706,_tmp710)){struct _tuple0 _tmp1118;struct _tuple0 _tmp728=(_tmp1118.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp706),((_tmp1118.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp710),_tmp1118)));void*
_tmp729;void*_tmp72B;void*_tmp72D;struct Cyc_Absyn_Exp*_tmp72F;void*_tmp730;
struct Cyc_Absyn_Exp*_tmp732;_LL3C6: _tmp729=_tmp728.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp72A=(struct Cyc_Absyn_Upper_b_struct*)_tmp729;if(_tmp72A->tag != 1)goto _LL3C8;};
_tmp72B=_tmp728.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp72C=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp72B;if(_tmp72C->tag != 0)goto _LL3C8;};_LL3C7: goto _LL3C5;_LL3C8: _tmp72D=
_tmp728.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp72E=(struct Cyc_Absyn_Upper_b_struct*)
_tmp72D;if(_tmp72E->tag != 1)goto _LL3CA;else{_tmp72F=_tmp72E->f1;}};_tmp730=
_tmp728.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp731=(struct Cyc_Absyn_Upper_b_struct*)
_tmp730;if(_tmp731->tag != 1)goto _LL3CA;else{_tmp732=_tmp731->f1;}};_LL3C9: if(!
Cyc_Evexp_lte_const_exp(_tmp732,_tmp72F))return 0;goto _LL3C5;_LL3CA:;_LL3CB:
return 0;_LL3C5:;}{struct _tuple0*_tmp111B;struct Cyc_List_List*_tmp111A;return Cyc_Tcutil_ptrsubtype(
te,((_tmp111A=_cycalloc(sizeof(*_tmp111A)),((_tmp111A->hd=((_tmp111B=_cycalloc(
sizeof(*_tmp111B)),((_tmp111B->f1=t1,((_tmp111B->f2=t2,_tmp111B)))))),((_tmp111A->tl=
assume,_tmp111A)))))),_tmp701,_tmp70B);};_LL3BF: _tmp712=_tmp6FD.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp713=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp712;if(_tmp713->tag != 4)
goto _LL3C1;else{_tmp714=_tmp713->f1;_tmp715=_tmp714.field_info;if((_tmp715.KnownDatatypefield).tag
!= 2)goto _LL3C1;_tmp716=(struct _tuple3)(_tmp715.KnownDatatypefield).val;_tmp717=
_tmp716.f1;_tmp718=_tmp716.f2;_tmp719=_tmp714.targs;}};_tmp71A=_tmp6FD.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp71B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp71A;if(_tmp71B->tag != 3)goto _LL3C1;else{_tmp71C=_tmp71B->f1;_tmp71D=_tmp71C.datatype_info;
if((_tmp71D.KnownDatatype).tag != 2)goto _LL3C1;_tmp71E=(struct Cyc_Absyn_Datatypedecl**)(
_tmp71D.KnownDatatype).val;_tmp71F=*_tmp71E;_tmp720=_tmp71C.targs;}};_LL3C0: if(
_tmp717 != _tmp71F  && Cyc_Absyn_qvar_cmp(_tmp717->name,_tmp71F->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp719)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp720))return 0;for(0;_tmp719 != 0;(_tmp719=
_tmp719->tl,_tmp720=_tmp720->tl)){if(!Cyc_Tcutil_unify((void*)_tmp719->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp720))->hd))return 0;}return 1;_LL3C1: _tmp721=
_tmp6FD.f1;{struct Cyc_Absyn_FnType_struct*_tmp722=(struct Cyc_Absyn_FnType_struct*)
_tmp721;if(_tmp722->tag != 10)goto _LL3C3;else{_tmp723=_tmp722->f1;}};_tmp724=
_tmp6FD.f2;{struct Cyc_Absyn_FnType_struct*_tmp725=(struct Cyc_Absyn_FnType_struct*)
_tmp724;if(_tmp725->tag != 10)goto _LL3C3;else{_tmp726=_tmp725->f1;}};_LL3C2: if(
_tmp723.tvars != 0  || _tmp726.tvars != 0){struct Cyc_List_List*_tmp735=_tmp723.tvars;
struct Cyc_List_List*_tmp736=_tmp726.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp735)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp736))return 0;{
struct _RegionHandle*_tmp737=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp735 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp735->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp736))->hd)->kind))return 0;{struct _tuple14*_tmp1125;struct Cyc_Absyn_VarType_struct
_tmp1124;struct Cyc_Absyn_VarType_struct*_tmp1123;struct Cyc_List_List*_tmp1122;
inst=((_tmp1122=_region_malloc(_tmp737,sizeof(*_tmp1122)),((_tmp1122->hd=((
_tmp1125=_region_malloc(_tmp737,sizeof(*_tmp1125)),((_tmp1125->f1=(struct Cyc_Absyn_Tvar*)
_tmp736->hd,((_tmp1125->f2=(void*)((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((
_tmp1123[0]=((_tmp1124.tag=2,((_tmp1124.f1=(struct Cyc_Absyn_Tvar*)_tmp735->hd,
_tmp1124)))),_tmp1123)))),_tmp1125)))))),((_tmp1122->tl=inst,_tmp1122))))));}
_tmp735=_tmp735->tl;_tmp736=_tmp736->tl;}if(inst != 0){_tmp723.tvars=0;_tmp726.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp112B;struct Cyc_Absyn_FnType_struct*_tmp112A;
struct Cyc_Absyn_FnType_struct _tmp1128;struct Cyc_Absyn_FnType_struct*_tmp1127;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((
_tmp1127[0]=((_tmp1128.tag=10,((_tmp1128.f1=_tmp723,_tmp1128)))),_tmp1127)))),(
void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=10,((
_tmp112B.f1=_tmp726,_tmp112B)))),_tmp112A)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp723.ret_typ,_tmp726.ret_typ))return 0;{struct Cyc_List_List*_tmp740=
_tmp723.args;struct Cyc_List_List*_tmp741=_tmp726.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp740)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp741))return 0;for(0;_tmp740 != 0;(_tmp740=_tmp740->tl,_tmp741=_tmp741->tl)){
struct Cyc_Absyn_Tqual _tmp743;void*_tmp744;struct _tuple9 _tmp742=*((struct _tuple9*)
_tmp740->hd);_tmp743=_tmp742.f2;_tmp744=_tmp742.f3;{struct Cyc_Absyn_Tqual _tmp746;
void*_tmp747;struct _tuple9 _tmp745=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp741))->hd);_tmp746=_tmp745.f2;_tmp747=_tmp745.f3;if(_tmp746.real_const
 && !_tmp743.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp747,_tmp744))return
0;};}if(_tmp723.c_varargs != _tmp726.c_varargs)return 0;if(_tmp723.cyc_varargs != 0
 && _tmp726.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp748=*_tmp723.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp749=*_tmp726.cyc_varargs;if((_tmp749.tq).real_const
 && !(_tmp748.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp749.type,
_tmp748.type))return 0;}else{if(_tmp723.cyc_varargs != 0  || _tmp726.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp723.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp726.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp723.rgn_po,_tmp726.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp723.attributes,_tmp726.attributes))return 0;return 1;};_LL3C3:;_LL3C4: return 0;
_LL3BC:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp112C;struct _tuple0 _tmp74B=(_tmp112C.f1=Cyc_Tcutil_compress(
t1),((_tmp112C.f2=Cyc_Tcutil_compress(t2),_tmp112C)));void*_tmp74C;enum Cyc_Absyn_Size_of
_tmp74E;void*_tmp74F;enum Cyc_Absyn_Size_of _tmp751;_LL3CD: _tmp74C=_tmp74B.f1;{
struct Cyc_Absyn_IntType_struct*_tmp74D=(struct Cyc_Absyn_IntType_struct*)_tmp74C;
if(_tmp74D->tag != 6)goto _LL3CF;else{_tmp74E=_tmp74D->f2;}};_tmp74F=_tmp74B.f2;{
struct Cyc_Absyn_IntType_struct*_tmp750=(struct Cyc_Absyn_IntType_struct*)_tmp74F;
if(_tmp750->tag != 6)goto _LL3CF;else{_tmp751=_tmp750->f2;}};_LL3CE: return(_tmp74E
== _tmp751  || _tmp74E == Cyc_Absyn_Int_sz  && _tmp751 == Cyc_Absyn_Long_sz) || 
_tmp74E == Cyc_Absyn_Long_sz  && _tmp751 == Cyc_Absyn_Int_sz;_LL3CF:;_LL3D0: return 0;
_LL3CC:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp752=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp752,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp752,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp754;struct Cyc_Absyn_Tqual
_tmp755;void*_tmp756;struct _tuple11*_tmp753=(struct _tuple11*)tqs1->hd;_tmp754=*
_tmp753;_tmp755=_tmp754.f1;_tmp756=_tmp754.f2;{struct _tuple11 _tmp758;struct Cyc_Absyn_Tqual
_tmp759;void*_tmp75A;struct _tuple11*_tmp757=(struct _tuple11*)tqs2->hd;_tmp758=*
_tmp757;_tmp759=_tmp758.f1;_tmp75A=_tmp758.f2;if(_tmp759.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp756,_tmp75A))continue;else{if(Cyc_Tcutil_unify(_tmp756,_tmp75A))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp756,_tmp75A))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp75B=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp75D;_LL3D2: {
struct Cyc_Absyn_IntType_struct*_tmp75C=(struct Cyc_Absyn_IntType_struct*)_tmp75B;
if(_tmp75C->tag != 6)goto _LL3D4;else{_tmp75D=_tmp75C->f2;if(_tmp75D != Cyc_Absyn_Char_sz)
goto _LL3D4;}}_LL3D3: return 1;_LL3D4:;_LL3D5: return 0;_LL3D1:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp75E=
t2;enum Cyc_Absyn_Size_of _tmp760;enum Cyc_Absyn_Size_of _tmp762;_LL3D7: {struct
Cyc_Absyn_IntType_struct*_tmp75F=(struct Cyc_Absyn_IntType_struct*)_tmp75E;if(
_tmp75F->tag != 6)goto _LL3D9;else{_tmp760=_tmp75F->f2;if(_tmp760 != Cyc_Absyn_Int_sz)
goto _LL3D9;}}_LL3D8: goto _LL3DA;_LL3D9: {struct Cyc_Absyn_IntType_struct*_tmp761=(
struct Cyc_Absyn_IntType_struct*)_tmp75E;if(_tmp761->tag != 6)goto _LL3DB;else{
_tmp762=_tmp761->f2;if(_tmp762 != Cyc_Absyn_Long_sz)goto _LL3DB;}}_LL3DA: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL3D6;_LL3DB:;
_LL3DC: goto _LL3D6;_LL3D6:;}{void*_tmp763=t1;struct Cyc_Absyn_PtrInfo _tmp765;void*
_tmp766;struct Cyc_Absyn_Tqual _tmp767;struct Cyc_Absyn_PtrAtts _tmp768;void*_tmp769;
union Cyc_Absyn_Constraint*_tmp76A;union Cyc_Absyn_Constraint*_tmp76B;union Cyc_Absyn_Constraint*
_tmp76C;struct Cyc_Absyn_ArrayInfo _tmp76E;void*_tmp76F;struct Cyc_Absyn_Tqual
_tmp770;struct Cyc_Absyn_Exp*_tmp771;union Cyc_Absyn_Constraint*_tmp772;struct Cyc_Absyn_Enumdecl*
_tmp774;void*_tmp779;_LL3DE: {struct Cyc_Absyn_PointerType_struct*_tmp764=(struct
Cyc_Absyn_PointerType_struct*)_tmp763;if(_tmp764->tag != 5)goto _LL3E0;else{
_tmp765=_tmp764->f1;_tmp766=_tmp765.elt_typ;_tmp767=_tmp765.elt_tq;_tmp768=
_tmp765.ptr_atts;_tmp769=_tmp768.rgn;_tmp76A=_tmp768.nullable;_tmp76B=_tmp768.bounds;
_tmp76C=_tmp768.zero_term;}}_LL3DF:{void*_tmp77A=t2;struct Cyc_Absyn_PtrInfo
_tmp77C;void*_tmp77D;struct Cyc_Absyn_Tqual _tmp77E;struct Cyc_Absyn_PtrAtts _tmp77F;
void*_tmp780;union Cyc_Absyn_Constraint*_tmp781;union Cyc_Absyn_Constraint*_tmp782;
union Cyc_Absyn_Constraint*_tmp783;_LL3EF: {struct Cyc_Absyn_PointerType_struct*
_tmp77B=(struct Cyc_Absyn_PointerType_struct*)_tmp77A;if(_tmp77B->tag != 5)goto
_LL3F1;else{_tmp77C=_tmp77B->f1;_tmp77D=_tmp77C.elt_typ;_tmp77E=_tmp77C.elt_tq;
_tmp77F=_tmp77C.ptr_atts;_tmp780=_tmp77F.rgn;_tmp781=_tmp77F.nullable;_tmp782=
_tmp77F.bounds;_tmp783=_tmp77F.zero_term;}}_LL3F0: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp112F;struct Cyc_List_List*
_tmp112E;struct Cyc_List_List*_tmp784=(_tmp112E=_cycalloc(sizeof(*_tmp112E)),((
_tmp112E->hd=((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F->f1=t1,((
_tmp112F->f2=t2,_tmp112F)))))),((_tmp112E->tl=0,_tmp112E)))));int _tmp785=Cyc_Tcutil_ptrsubtype(
te,_tmp784,_tmp766,_tmp77D) && (!_tmp767.real_const  || _tmp77E.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp76C,
_tmp783) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp783);
int _tmp786=_tmp785?0:((Cyc_Tcutil_bits_only(_tmp766) && Cyc_Tcutil_is_char_type(
_tmp77D)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp783)) && (_tmp77E.real_const  || !_tmp767.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp76B,_tmp782);if(!
bounds_ok  && !_tmp786){struct _tuple0 _tmp1130;struct _tuple0 _tmp788=(_tmp1130.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp76B),((_tmp1130.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp782),_tmp1130)));
void*_tmp789;struct Cyc_Absyn_Exp*_tmp78B;void*_tmp78C;struct Cyc_Absyn_Exp*
_tmp78E;_LL3F4: _tmp789=_tmp788.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp78A=(
struct Cyc_Absyn_Upper_b_struct*)_tmp789;if(_tmp78A->tag != 1)goto _LL3F6;else{
_tmp78B=_tmp78A->f1;}};_tmp78C=_tmp788.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp78D=(struct Cyc_Absyn_Upper_b_struct*)_tmp78C;if(_tmp78D->tag != 1)goto _LL3F6;
else{_tmp78E=_tmp78D->f1;}};_LL3F5: if(Cyc_Evexp_lte_const_exp(_tmp78E,_tmp78B))
bounds_ok=1;goto _LL3F3;_LL3F6:;_LL3F7: bounds_ok=1;goto _LL3F3;_LL3F3:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp76A) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp781))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp785  || _tmp786)) && (Cyc_Tcutil_unify(
_tmp769,_tmp780) || Cyc_Tcenv_region_outlives(te,_tmp769,_tmp780)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL3F1:;_LL3F2: goto _LL3EE;
_LL3EE:;}return Cyc_Absyn_Unknown_coercion;_LL3E0: {struct Cyc_Absyn_ArrayType_struct*
_tmp76D=(struct Cyc_Absyn_ArrayType_struct*)_tmp763;if(_tmp76D->tag != 9)goto
_LL3E2;else{_tmp76E=_tmp76D->f1;_tmp76F=_tmp76E.elt_type;_tmp770=_tmp76E.tq;
_tmp771=_tmp76E.num_elts;_tmp772=_tmp76E.zero_term;}}_LL3E1:{void*_tmp791=t2;
struct Cyc_Absyn_ArrayInfo _tmp793;void*_tmp794;struct Cyc_Absyn_Tqual _tmp795;
struct Cyc_Absyn_Exp*_tmp796;union Cyc_Absyn_Constraint*_tmp797;_LL3F9: {struct Cyc_Absyn_ArrayType_struct*
_tmp792=(struct Cyc_Absyn_ArrayType_struct*)_tmp791;if(_tmp792->tag != 9)goto
_LL3FB;else{_tmp793=_tmp792->f1;_tmp794=_tmp793.elt_type;_tmp795=_tmp793.tq;
_tmp796=_tmp793.num_elts;_tmp797=_tmp793.zero_term;}}_LL3FA: {int okay;okay=((
_tmp771 != 0  && _tmp796 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp772,
_tmp797)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp796,(struct Cyc_Absyn_Exp*)
_tmp771);return(okay  && Cyc_Tcutil_unify(_tmp76F,_tmp794)) && (!_tmp770.real_const
 || _tmp795.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL3FB:;_LL3FC: return Cyc_Absyn_Unknown_coercion;_LL3F8:;}return Cyc_Absyn_Unknown_coercion;
_LL3E2: {struct Cyc_Absyn_EnumType_struct*_tmp773=(struct Cyc_Absyn_EnumType_struct*)
_tmp763;if(_tmp773->tag != 14)goto _LL3E4;else{_tmp774=_tmp773->f2;}}_LL3E3:{void*
_tmp798=t2;struct Cyc_Absyn_Enumdecl*_tmp79A;_LL3FE: {struct Cyc_Absyn_EnumType_struct*
_tmp799=(struct Cyc_Absyn_EnumType_struct*)_tmp798;if(_tmp799->tag != 14)goto
_LL400;else{_tmp79A=_tmp799->f2;}}_LL3FF: if((_tmp774->fields != 0  && _tmp79A->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp774->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp79A->fields))->v))return Cyc_Absyn_No_coercion;goto _LL3FD;_LL400:;_LL401: goto
_LL3FD;_LL3FD:;}goto _LL3E5;_LL3E4: {struct Cyc_Absyn_IntType_struct*_tmp775=(
struct Cyc_Absyn_IntType_struct*)_tmp763;if(_tmp775->tag != 6)goto _LL3E6;}_LL3E5:
goto _LL3E7;_LL3E6: {struct Cyc_Absyn_FloatType_struct*_tmp776=(struct Cyc_Absyn_FloatType_struct*)
_tmp763;if(_tmp776->tag != 7)goto _LL3E8;}_LL3E7: goto _LL3E9;_LL3E8: {struct Cyc_Absyn_DoubleType_struct*
_tmp777=(struct Cyc_Absyn_DoubleType_struct*)_tmp763;if(_tmp777->tag != 8)goto
_LL3EA;}_LL3E9: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL3EA: {struct Cyc_Absyn_RgnHandleType_struct*_tmp778=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp763;if(_tmp778->tag != 16)goto _LL3EC;else{_tmp779=(void*)_tmp778->f1;}}_LL3EB:{
void*_tmp79B=t2;void*_tmp79D;_LL403: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp79C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp79B;if(_tmp79C->tag != 16)goto
_LL405;else{_tmp79D=(void*)_tmp79C->f1;}}_LL404: if(Cyc_Tcenv_region_outlives(te,
_tmp779,_tmp79D))return Cyc_Absyn_No_coercion;goto _LL402;_LL405:;_LL406: goto
_LL402;_LL402:;}return Cyc_Absyn_Unknown_coercion;_LL3EC:;_LL3ED: return Cyc_Absyn_Unknown_coercion;
_LL3DD:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp79E=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp1133;struct Cyc_Absyn_Cast_e_struct*
_tmp1132;e->r=(void*)((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((
_tmp1133.tag=15,((_tmp1133.f1=(void*)t,((_tmp1133.f2=_tmp79E,((_tmp1133.f3=0,((
_tmp1133.f4=c,_tmp1133)))))))))),_tmp1132))));}{struct Cyc_Core_Opt*_tmp1134;e->topt=((
_tmp1134=_cycalloc(sizeof(*_tmp1134)),((_tmp1134->v=t,_tmp1134))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp7A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL408: {struct Cyc_Absyn_IntType_struct*_tmp7A3=(struct Cyc_Absyn_IntType_struct*)
_tmp7A2;if(_tmp7A3->tag != 6)goto _LL40A;}_LL409: goto _LL40B;_LL40A: {struct Cyc_Absyn_EnumType_struct*
_tmp7A4=(struct Cyc_Absyn_EnumType_struct*)_tmp7A2;if(_tmp7A4->tag != 14)goto
_LL40C;}_LL40B: goto _LL40D;_LL40C: {struct Cyc_Absyn_AnonEnumType_struct*_tmp7A5=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp7A2;if(_tmp7A5->tag != 15)goto _LL40E;}
_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_TagType_struct*_tmp7A6=(struct Cyc_Absyn_TagType_struct*)
_tmp7A2;if(_tmp7A6->tag != 20)goto _LL410;}_LL40F: return 1;_LL410: {struct Cyc_Absyn_Evar_struct*
_tmp7A7=(struct Cyc_Absyn_Evar_struct*)_tmp7A2;if(_tmp7A7->tag != 1)goto _LL412;}
_LL411: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL412:;_LL413: return 0;_LL407:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp7A8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL415: {struct Cyc_Absyn_FloatType_struct*_tmp7A9=(
struct Cyc_Absyn_FloatType_struct*)_tmp7A8;if(_tmp7A9->tag != 7)goto _LL417;}_LL416:
goto _LL418;_LL417: {struct Cyc_Absyn_DoubleType_struct*_tmp7AA=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7A8;if(_tmp7AA->tag != 8)goto _LL419;}_LL418: return 1;_LL419:;_LL41A: return 0;
_LL414:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp7AB=Cyc_Tcutil_compress(t);_LL41C: {struct Cyc_Absyn_FnType_struct*
_tmp7AC=(struct Cyc_Absyn_FnType_struct*)_tmp7AB;if(_tmp7AC->tag != 10)goto _LL41E;}
_LL41D: return 1;_LL41E:;_LL41F: return 0;_LL41B:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp1135;struct _tuple0 _tmp7AE=(_tmp1135.f1=t1,((_tmp1135.f2=t2,_tmp1135)));
void*_tmp7AF;int _tmp7B1;void*_tmp7B2;int _tmp7B4;void*_tmp7B5;void*_tmp7B7;void*
_tmp7B9;void*_tmp7BB;void*_tmp7BD;enum Cyc_Absyn_Sign _tmp7BF;enum Cyc_Absyn_Size_of
_tmp7C0;void*_tmp7C1;enum Cyc_Absyn_Sign _tmp7C3;enum Cyc_Absyn_Size_of _tmp7C4;
void*_tmp7C5;enum Cyc_Absyn_Size_of _tmp7C7;void*_tmp7C8;enum Cyc_Absyn_Size_of
_tmp7CA;void*_tmp7CB;enum Cyc_Absyn_Sign _tmp7CD;enum Cyc_Absyn_Size_of _tmp7CE;
void*_tmp7CF;enum Cyc_Absyn_Sign _tmp7D1;enum Cyc_Absyn_Size_of _tmp7D2;void*
_tmp7D3;enum Cyc_Absyn_Sign _tmp7D5;enum Cyc_Absyn_Size_of _tmp7D6;void*_tmp7D7;
enum Cyc_Absyn_Sign _tmp7D9;enum Cyc_Absyn_Size_of _tmp7DA;void*_tmp7DB;enum Cyc_Absyn_Size_of
_tmp7DD;void*_tmp7DE;enum Cyc_Absyn_Size_of _tmp7E0;_LL421: _tmp7AF=_tmp7AE.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp7B0=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7AF;if(_tmp7B0->tag != 8)goto _LL423;else{_tmp7B1=_tmp7B0->f1;}};_tmp7B2=
_tmp7AE.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp7B3=(struct Cyc_Absyn_DoubleType_struct*)
_tmp7B2;if(_tmp7B3->tag != 8)goto _LL423;else{_tmp7B4=_tmp7B3->f1;}};_LL422: if(
_tmp7B1)return t1;else{return t2;}_LL423: _tmp7B5=_tmp7AE.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp7B6=(struct Cyc_Absyn_DoubleType_struct*)_tmp7B5;if(_tmp7B6->tag != 8)goto
_LL425;};_LL424: return t1;_LL425: _tmp7B7=_tmp7AE.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp7B8=(struct Cyc_Absyn_DoubleType_struct*)_tmp7B7;if(_tmp7B8->tag != 8)goto
_LL427;};_LL426: return t2;_LL427: _tmp7B9=_tmp7AE.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp7BA=(struct Cyc_Absyn_FloatType_struct*)_tmp7B9;if(_tmp7BA->tag != 7)goto
_LL429;};_LL428: goto _LL42A;_LL429: _tmp7BB=_tmp7AE.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp7BC=(struct Cyc_Absyn_FloatType_struct*)_tmp7BB;if(_tmp7BC->tag != 7)goto
_LL42B;};_LL42A: return(void*)& Cyc_Absyn_FloatType_val;_LL42B: _tmp7BD=_tmp7AE.f1;{
struct Cyc_Absyn_IntType_struct*_tmp7BE=(struct Cyc_Absyn_IntType_struct*)_tmp7BD;
if(_tmp7BE->tag != 6)goto _LL42D;else{_tmp7BF=_tmp7BE->f1;if(_tmp7BF != Cyc_Absyn_Unsigned)
goto _LL42D;_tmp7C0=_tmp7BE->f2;if(_tmp7C0 != Cyc_Absyn_LongLong_sz)goto _LL42D;}};
_LL42C: goto _LL42E;_LL42D: _tmp7C1=_tmp7AE.f2;{struct Cyc_Absyn_IntType_struct*
_tmp7C2=(struct Cyc_Absyn_IntType_struct*)_tmp7C1;if(_tmp7C2->tag != 6)goto _LL42F;
else{_tmp7C3=_tmp7C2->f1;if(_tmp7C3 != Cyc_Absyn_Unsigned)goto _LL42F;_tmp7C4=
_tmp7C2->f2;if(_tmp7C4 != Cyc_Absyn_LongLong_sz)goto _LL42F;}};_LL42E: return Cyc_Absyn_ulonglong_typ;
_LL42F: _tmp7C5=_tmp7AE.f1;{struct Cyc_Absyn_IntType_struct*_tmp7C6=(struct Cyc_Absyn_IntType_struct*)
_tmp7C5;if(_tmp7C6->tag != 6)goto _LL431;else{_tmp7C7=_tmp7C6->f2;if(_tmp7C7 != Cyc_Absyn_LongLong_sz)
goto _LL431;}};_LL430: goto _LL432;_LL431: _tmp7C8=_tmp7AE.f2;{struct Cyc_Absyn_IntType_struct*
_tmp7C9=(struct Cyc_Absyn_IntType_struct*)_tmp7C8;if(_tmp7C9->tag != 6)goto _LL433;
else{_tmp7CA=_tmp7C9->f2;if(_tmp7CA != Cyc_Absyn_LongLong_sz)goto _LL433;}};_LL432:
return Cyc_Absyn_slonglong_typ;_LL433: _tmp7CB=_tmp7AE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7CC=(struct Cyc_Absyn_IntType_struct*)_tmp7CB;if(_tmp7CC->tag != 6)goto _LL435;
else{_tmp7CD=_tmp7CC->f1;if(_tmp7CD != Cyc_Absyn_Unsigned)goto _LL435;_tmp7CE=
_tmp7CC->f2;if(_tmp7CE != Cyc_Absyn_Long_sz)goto _LL435;}};_LL434: goto _LL436;
_LL435: _tmp7CF=_tmp7AE.f2;{struct Cyc_Absyn_IntType_struct*_tmp7D0=(struct Cyc_Absyn_IntType_struct*)
_tmp7CF;if(_tmp7D0->tag != 6)goto _LL437;else{_tmp7D1=_tmp7D0->f1;if(_tmp7D1 != Cyc_Absyn_Unsigned)
goto _LL437;_tmp7D2=_tmp7D0->f2;if(_tmp7D2 != Cyc_Absyn_Long_sz)goto _LL437;}};
_LL436: return Cyc_Absyn_ulong_typ;_LL437: _tmp7D3=_tmp7AE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7D4=(struct Cyc_Absyn_IntType_struct*)_tmp7D3;if(_tmp7D4->tag != 6)goto _LL439;
else{_tmp7D5=_tmp7D4->f1;if(_tmp7D5 != Cyc_Absyn_Unsigned)goto _LL439;_tmp7D6=
_tmp7D4->f2;if(_tmp7D6 != Cyc_Absyn_Int_sz)goto _LL439;}};_LL438: goto _LL43A;_LL439:
_tmp7D7=_tmp7AE.f2;{struct Cyc_Absyn_IntType_struct*_tmp7D8=(struct Cyc_Absyn_IntType_struct*)
_tmp7D7;if(_tmp7D8->tag != 6)goto _LL43B;else{_tmp7D9=_tmp7D8->f1;if(_tmp7D9 != Cyc_Absyn_Unsigned)
goto _LL43B;_tmp7DA=_tmp7D8->f2;if(_tmp7DA != Cyc_Absyn_Int_sz)goto _LL43B;}};
_LL43A: return Cyc_Absyn_uint_typ;_LL43B: _tmp7DB=_tmp7AE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7DC=(struct Cyc_Absyn_IntType_struct*)_tmp7DB;if(_tmp7DC->tag != 6)goto _LL43D;
else{_tmp7DD=_tmp7DC->f2;if(_tmp7DD != Cyc_Absyn_Long_sz)goto _LL43D;}};_LL43C:
goto _LL43E;_LL43D: _tmp7DE=_tmp7AE.f2;{struct Cyc_Absyn_IntType_struct*_tmp7DF=(
struct Cyc_Absyn_IntType_struct*)_tmp7DE;if(_tmp7DF->tag != 6)goto _LL43F;else{
_tmp7E0=_tmp7DF->f2;if(_tmp7E0 != Cyc_Absyn_Long_sz)goto _LL43F;}};_LL43E: return
Cyc_Absyn_slong_typ;_LL43F:;_LL440: return Cyc_Absyn_sint_typ;_LL420:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp7E1=e->r;struct Cyc_Core_Opt*_tmp7E3;_LL442: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp7E2=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp7E1;if(_tmp7E2->tag != 4)goto
_LL444;else{_tmp7E3=_tmp7E2->f2;if(_tmp7E3 != 0)goto _LL444;}}_LL443:{const char*
_tmp1138;void*_tmp1137;(_tmp1137=0,Cyc_Tcutil_warn(e->loc,((_tmp1138="assignment in test",
_tag_dyneither(_tmp1138,sizeof(char),19))),_tag_dyneither(_tmp1137,sizeof(void*),
0)));}goto _LL441;_LL444:;_LL445: goto _LL441;_LL441:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1139;struct _tuple0 _tmp7E7=(
_tmp1139.f1=c1,((_tmp1139.f2=c2,_tmp1139)));void*_tmp7E8;struct Cyc_Absyn_Kind*
_tmp7EA;void*_tmp7EB;struct Cyc_Absyn_Kind*_tmp7ED;void*_tmp7EE;struct Cyc_Core_Opt*
_tmp7F0;struct Cyc_Core_Opt**_tmp7F1;void*_tmp7F2;struct Cyc_Core_Opt*_tmp7F4;
struct Cyc_Core_Opt**_tmp7F5;void*_tmp7F6;struct Cyc_Core_Opt*_tmp7F8;struct Cyc_Core_Opt**
_tmp7F9;struct Cyc_Absyn_Kind*_tmp7FA;void*_tmp7FB;struct Cyc_Absyn_Kind*_tmp7FD;
void*_tmp7FE;struct Cyc_Absyn_Kind*_tmp800;void*_tmp801;struct Cyc_Core_Opt*
_tmp803;struct Cyc_Core_Opt**_tmp804;struct Cyc_Absyn_Kind*_tmp805;void*_tmp806;
struct Cyc_Core_Opt*_tmp808;struct Cyc_Core_Opt**_tmp809;struct Cyc_Absyn_Kind*
_tmp80A;void*_tmp80B;struct Cyc_Core_Opt*_tmp80D;struct Cyc_Core_Opt**_tmp80E;
struct Cyc_Absyn_Kind*_tmp80F;_LL447: _tmp7E8=_tmp7E7.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7E9=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7E8;if(_tmp7E9->tag != 0)goto _LL449;
else{_tmp7EA=_tmp7E9->f1;}};_tmp7EB=_tmp7E7.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7EC=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7EB;if(_tmp7EC->tag != 0)goto _LL449;
else{_tmp7ED=_tmp7EC->f1;}};_LL448: return _tmp7EA == _tmp7ED;_LL449: _tmp7EE=
_tmp7E7.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp7EF=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7EE;if(_tmp7EF->tag != 1)goto _LL44B;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=(struct
Cyc_Core_Opt**)& _tmp7EF->f1;}};_LL44A:{struct Cyc_Core_Opt*_tmp113A;*_tmp7F1=((
_tmp113A=_cycalloc(sizeof(*_tmp113A)),((_tmp113A->v=c1,_tmp113A))));}return 1;
_LL44B: _tmp7F2=_tmp7E7.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp7F3=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7F2;if(_tmp7F3->tag != 1)goto _LL44D;else{_tmp7F4=_tmp7F3->f1;_tmp7F5=(struct
Cyc_Core_Opt**)& _tmp7F3->f1;}};_LL44C:{struct Cyc_Core_Opt*_tmp113B;*_tmp7F5=((
_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B->v=c2,_tmp113B))));}return 1;
_LL44D: _tmp7F6=_tmp7E7.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp7F7=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7F6;if(_tmp7F7->tag != 2)goto _LL44F;else{_tmp7F8=_tmp7F7->f1;_tmp7F9=(struct
Cyc_Core_Opt**)& _tmp7F7->f1;_tmp7FA=_tmp7F7->f2;}};_tmp7FB=_tmp7E7.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7FC=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7FB;if(_tmp7FC->tag != 0)goto _LL44F;
else{_tmp7FD=_tmp7FC->f1;}};_LL44E: if(Cyc_Tcutil_kind_leq(_tmp7FD,_tmp7FA)){{
struct Cyc_Core_Opt*_tmp113C;*_tmp7F9=((_tmp113C=_cycalloc(sizeof(*_tmp113C)),((
_tmp113C->v=c2,_tmp113C))));}return 1;}else{return 0;}_LL44F: _tmp7FE=_tmp7E7.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp7FF=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7FE;if(
_tmp7FF->tag != 0)goto _LL451;else{_tmp800=_tmp7FF->f1;}};_tmp801=_tmp7E7.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp802=(struct Cyc_Absyn_Less_kb_struct*)_tmp801;
if(_tmp802->tag != 2)goto _LL451;else{_tmp803=_tmp802->f1;_tmp804=(struct Cyc_Core_Opt**)&
_tmp802->f1;_tmp805=_tmp802->f2;}};_LL450: if(Cyc_Tcutil_kind_leq(_tmp800,_tmp805)){{
struct Cyc_Core_Opt*_tmp113D;*_tmp804=((_tmp113D=_cycalloc(sizeof(*_tmp113D)),((
_tmp113D->v=c1,_tmp113D))));}return 1;}else{return 0;}_LL451: _tmp806=_tmp7E7.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp807=(struct Cyc_Absyn_Less_kb_struct*)_tmp806;
if(_tmp807->tag != 2)goto _LL446;else{_tmp808=_tmp807->f1;_tmp809=(struct Cyc_Core_Opt**)&
_tmp807->f1;_tmp80A=_tmp807->f2;}};_tmp80B=_tmp7E7.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp80C=(struct Cyc_Absyn_Less_kb_struct*)_tmp80B;if(_tmp80C->tag != 2)goto _LL446;
else{_tmp80D=_tmp80C->f1;_tmp80E=(struct Cyc_Core_Opt**)& _tmp80C->f1;_tmp80F=
_tmp80C->f2;}};_LL452: if(Cyc_Tcutil_kind_leq(_tmp80A,_tmp80F)){{struct Cyc_Core_Opt*
_tmp113E;*_tmp80E=((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((_tmp113E->v=c1,
_tmp113E))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp80F,_tmp80A)){{struct Cyc_Core_Opt*
_tmp113F;*_tmp809=((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F->v=c2,
_tmp113F))));}return 1;}else{return 0;}}_LL446:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp1143;void*_tmp1142[1];struct Cyc_Int_pa_struct _tmp1141;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1141.tag=1,((_tmp1141.f1=(
unsigned long)i,((_tmp1142[0]=& _tmp1141,Cyc_aprintf(((_tmp1143="#%d",
_tag_dyneither(_tmp1143,sizeof(char),4))),_tag_dyneither(_tmp1142,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp1146;struct Cyc_Absyn_Tvar*_tmp1145;return(
_tmp1145=_cycalloc(sizeof(*_tmp1145)),((_tmp1145->name=((_tmp1146=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp1146[0]=s,_tmp1146)))),((_tmp1145->identity=
- 1,((_tmp1145->kind=k,_tmp1145)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp81B=*t->name;return*((const char*)_check_dyneither_subscript(_tmp81B,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp114A;void*
_tmp1149[1];struct Cyc_String_pa_struct _tmp1148;(_tmp1148.tag=0,((_tmp1148.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1149[0]=& _tmp1148,
Cyc_printf(((_tmp114A="%s",_tag_dyneither(_tmp114A,sizeof(char),3))),
_tag_dyneither(_tmp1149,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp114B;struct _dyneither_ptr _tmp81F=Cyc_strconcat(((_tmp114B="`",
_tag_dyneither(_tmp114B,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp114E;char _tmp114D;struct _dyneither_ptr _tmp114C;(_tmp114C=_dyneither_ptr_plus(
_tmp81F,sizeof(char),1),((_tmp114D=*((char*)_check_dyneither_subscript(_tmp114C,
sizeof(char),0)),((_tmp114E='t',((_get_dyneither_size(_tmp114C,sizeof(char))== 1
 && (_tmp114D == '\000'  && _tmp114E != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp114C.curr)=_tmp114E)))))));}{struct _dyneither_ptr*_tmp114F;t->name=((
_tmp114F=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp114F[0]=(struct
_dyneither_ptr)_tmp81F,_tmp114F))));};};}struct _tuple21{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple21*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple21*x){
struct Cyc_Core_Opt*_tmp1152;struct _tuple9*_tmp1151;return(_tmp1151=_cycalloc(
sizeof(*_tmp1151)),((_tmp1151->f1=(struct Cyc_Core_Opt*)((_tmp1152=_cycalloc(
sizeof(*_tmp1152)),((_tmp1152->v=(*x).f1,_tmp1152)))),((_tmp1151->f2=(*x).f2,((
_tmp1151->f3=(*x).f3,_tmp1151)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp827=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp1153;_tmp827=((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153->hd=(void*)
atts->hd,((_tmp1153->tl=_tmp827,_tmp1153))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1159;struct Cyc_Absyn_FnInfo _tmp1158;struct Cyc_Absyn_FnType_struct*_tmp1157;
return(void*)((_tmp1157=_cycalloc(sizeof(*_tmp1157)),((_tmp1157[0]=((_tmp1159.tag=
10,((_tmp1159.f1=((_tmp1158.tvars=fd->tvs,((_tmp1158.effect=fd->effect,((
_tmp1158.ret_typ=fd->ret_type,((_tmp1158.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp1158.c_varargs=fd->c_varargs,((_tmp1158.cyc_varargs=fd->cyc_varargs,((
_tmp1158.rgn_po=fd->rgn_po,((_tmp1158.attributes=_tmp827,_tmp1158)))))))))))))))),
_tmp1159)))),_tmp1157))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple22{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple22*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple22*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp115A;return(_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A->f1=(*
pr).f1,((_tmp115A->f2=t,_tmp115A)))));}struct _tuple23{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple24{struct _tuple23*f1;void*f2;};static
struct _tuple24*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple24*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple23*_tmp115D;struct _tuple24*_tmp115C;return(_tmp115C=
_region_malloc(rgn,sizeof(*_tmp115C)),((_tmp115C->f1=((_tmp115D=_region_malloc(
rgn,sizeof(*_tmp115D)),((_tmp115D->f1=(*y).f1,((_tmp115D->f2=(*y).f2,_tmp115D)))))),((
_tmp115C->f2=(*y).f3,_tmp115C)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple24*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple24*w){
struct _tuple23*_tmp830;void*_tmp831;struct _tuple24 _tmp82F=*w;_tmp830=_tmp82F.f1;
_tmp831=_tmp82F.f2;{struct Cyc_Core_Opt*_tmp833;struct Cyc_Absyn_Tqual _tmp834;
struct _tuple23 _tmp832=*_tmp830;_tmp833=_tmp832.f1;_tmp834=_tmp832.f2;{struct
_tuple9*_tmp115E;return(_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E->f1=
_tmp833,((_tmp115E->f2=_tmp834,((_tmp115E->f3=_tmp831,_tmp115E)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp115F;return(
_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F->name=f->name,((_tmp115F->tq=f->tq,((
_tmp115F->type=t,((_tmp115F->width=f->width,((_tmp115F->attributes=f->attributes,
_tmp115F)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1160;
return(_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160->topt=old->topt,((
_tmp1160->r=r,((_tmp1160->loc=old->loc,((_tmp1160->annot=old->annot,_tmp1160)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp838=e->r;
enum Cyc_Absyn_Primop _tmp83E;struct Cyc_List_List*_tmp83F;struct Cyc_Absyn_Exp*
_tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_Absyn_Exp*
_tmp845;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Exp*
_tmp849;struct Cyc_Absyn_Exp*_tmp84B;struct Cyc_Absyn_Exp*_tmp84C;void*_tmp84E;
struct Cyc_Absyn_Exp*_tmp84F;int _tmp850;enum Cyc_Absyn_Coercion _tmp851;void*
_tmp853;struct Cyc_Absyn_Exp*_tmp855;void*_tmp857;void*_tmp858;void*_tmp85A;
_LL454: {struct Cyc_Absyn_Const_e_struct*_tmp839=(struct Cyc_Absyn_Const_e_struct*)
_tmp838;if(_tmp839->tag != 0)goto _LL456;}_LL455: goto _LL457;_LL456: {struct Cyc_Absyn_Enum_e_struct*
_tmp83A=(struct Cyc_Absyn_Enum_e_struct*)_tmp838;if(_tmp83A->tag != 32)goto _LL458;}
_LL457: goto _LL459;_LL458: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp83B=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp838;if(_tmp83B->tag != 33)goto _LL45A;}_LL459: goto _LL45B;_LL45A: {struct Cyc_Absyn_Var_e_struct*
_tmp83C=(struct Cyc_Absyn_Var_e_struct*)_tmp838;if(_tmp83C->tag != 1)goto _LL45C;}
_LL45B: return e;_LL45C: {struct Cyc_Absyn_Primop_e_struct*_tmp83D=(struct Cyc_Absyn_Primop_e_struct*)
_tmp838;if(_tmp83D->tag != 3)goto _LL45E;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;}}
_LL45D: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83F)== 1){struct
Cyc_Absyn_Exp*_tmp85B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp83F))->hd;struct Cyc_Absyn_Exp*_tmp85C=Cyc_Tcutil_rsubsexp(r,inst,_tmp85B);
if(_tmp85C == _tmp85B)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1166;struct Cyc_Absyn_Exp*
_tmp1165[1];struct Cyc_Absyn_Primop_e_struct*_tmp1164;return Cyc_Tcutil_copye(e,(
void*)((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=((_tmp1166.tag=3,((
_tmp1166.f1=_tmp83E,((_tmp1166.f2=((_tmp1165[0]=_tmp85C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1165,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp1166)))))),_tmp1164)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp83F)== 2){struct Cyc_Absyn_Exp*_tmp860=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp83F))->hd;struct Cyc_Absyn_Exp*_tmp861=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp83F->tl))->hd;struct
Cyc_Absyn_Exp*_tmp862=Cyc_Tcutil_rsubsexp(r,inst,_tmp860);struct Cyc_Absyn_Exp*
_tmp863=Cyc_Tcutil_rsubsexp(r,inst,_tmp861);if(_tmp862 == _tmp860  && _tmp863 == 
_tmp861)return e;{struct Cyc_Absyn_Primop_e_struct _tmp116C;struct Cyc_Absyn_Exp*
_tmp116B[2];struct Cyc_Absyn_Primop_e_struct*_tmp116A;return Cyc_Tcutil_copye(e,(
void*)((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp116C.tag=3,((
_tmp116C.f1=_tmp83E,((_tmp116C.f2=((_tmp116B[1]=_tmp863,((_tmp116B[0]=_tmp862,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp116B,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp116C)))))),_tmp116A)))));};}
else{const char*_tmp116F;void*_tmp116E;return(_tmp116E=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp116F="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp116F,sizeof(char),34))),_tag_dyneither(_tmp116E,sizeof(void*),
0)));}}_LL45E: {struct Cyc_Absyn_Conditional_e_struct*_tmp840=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp838;if(_tmp840->tag != 6)goto _LL460;else{_tmp841=_tmp840->f1;_tmp842=_tmp840->f2;
_tmp843=_tmp840->f3;}}_LL45F: {struct Cyc_Absyn_Exp*_tmp869=Cyc_Tcutil_rsubsexp(r,
inst,_tmp841);struct Cyc_Absyn_Exp*_tmp86A=Cyc_Tcutil_rsubsexp(r,inst,_tmp842);
struct Cyc_Absyn_Exp*_tmp86B=Cyc_Tcutil_rsubsexp(r,inst,_tmp843);if((_tmp869 == 
_tmp841  && _tmp86A == _tmp842) && _tmp86B == _tmp843)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1172;struct Cyc_Absyn_Conditional_e_struct*_tmp1171;return Cyc_Tcutil_copye(e,(
void*)((_tmp1171=_cycalloc(sizeof(*_tmp1171)),((_tmp1171[0]=((_tmp1172.tag=6,((
_tmp1172.f1=_tmp869,((_tmp1172.f2=_tmp86A,((_tmp1172.f3=_tmp86B,_tmp1172)))))))),
_tmp1171)))));};}_LL460: {struct Cyc_Absyn_And_e_struct*_tmp844=(struct Cyc_Absyn_And_e_struct*)
_tmp838;if(_tmp844->tag != 7)goto _LL462;else{_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;}}
_LL461: {struct Cyc_Absyn_Exp*_tmp86E=Cyc_Tcutil_rsubsexp(r,inst,_tmp845);struct
Cyc_Absyn_Exp*_tmp86F=Cyc_Tcutil_rsubsexp(r,inst,_tmp846);if(_tmp86E == _tmp845
 && _tmp86F == _tmp846)return e;{struct Cyc_Absyn_And_e_struct _tmp1175;struct Cyc_Absyn_And_e_struct*
_tmp1174;return Cyc_Tcutil_copye(e,(void*)((_tmp1174=_cycalloc(sizeof(*_tmp1174)),((
_tmp1174[0]=((_tmp1175.tag=7,((_tmp1175.f1=_tmp86E,((_tmp1175.f2=_tmp86F,
_tmp1175)))))),_tmp1174)))));};}_LL462: {struct Cyc_Absyn_Or_e_struct*_tmp847=(
struct Cyc_Absyn_Or_e_struct*)_tmp838;if(_tmp847->tag != 8)goto _LL464;else{_tmp848=
_tmp847->f1;_tmp849=_tmp847->f2;}}_LL463: {struct Cyc_Absyn_Exp*_tmp872=Cyc_Tcutil_rsubsexp(
r,inst,_tmp848);struct Cyc_Absyn_Exp*_tmp873=Cyc_Tcutil_rsubsexp(r,inst,_tmp849);
if(_tmp872 == _tmp848  && _tmp873 == _tmp849)return e;{struct Cyc_Absyn_Or_e_struct
_tmp1178;struct Cyc_Absyn_Or_e_struct*_tmp1177;return Cyc_Tcutil_copye(e,(void*)((
_tmp1177=_cycalloc(sizeof(*_tmp1177)),((_tmp1177[0]=((_tmp1178.tag=8,((_tmp1178.f1=
_tmp872,((_tmp1178.f2=_tmp873,_tmp1178)))))),_tmp1177)))));};}_LL464: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp84A=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp838;if(_tmp84A->tag != 9)goto _LL466;
else{_tmp84B=_tmp84A->f1;_tmp84C=_tmp84A->f2;}}_LL465: {struct Cyc_Absyn_Exp*
_tmp876=Cyc_Tcutil_rsubsexp(r,inst,_tmp84B);struct Cyc_Absyn_Exp*_tmp877=Cyc_Tcutil_rsubsexp(
r,inst,_tmp84C);if(_tmp876 == _tmp84B  && _tmp877 == _tmp84C)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp117B;struct Cyc_Absyn_SeqExp_e_struct*_tmp117A;return Cyc_Tcutil_copye(e,(void*)((
_tmp117A=_cycalloc(sizeof(*_tmp117A)),((_tmp117A[0]=((_tmp117B.tag=9,((_tmp117B.f1=
_tmp876,((_tmp117B.f2=_tmp877,_tmp117B)))))),_tmp117A)))));};}_LL466: {struct Cyc_Absyn_Cast_e_struct*
_tmp84D=(struct Cyc_Absyn_Cast_e_struct*)_tmp838;if(_tmp84D->tag != 15)goto _LL468;
else{_tmp84E=(void*)_tmp84D->f1;_tmp84F=_tmp84D->f2;_tmp850=_tmp84D->f3;_tmp851=
_tmp84D->f4;}}_LL467: {struct Cyc_Absyn_Exp*_tmp87A=Cyc_Tcutil_rsubsexp(r,inst,
_tmp84F);void*_tmp87B=Cyc_Tcutil_rsubstitute(r,inst,_tmp84E);if(_tmp87A == 
_tmp84F  && _tmp87B == _tmp84E)return e;{struct Cyc_Absyn_Cast_e_struct _tmp117E;
struct Cyc_Absyn_Cast_e_struct*_tmp117D;return Cyc_Tcutil_copye(e,(void*)((
_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((_tmp117E.tag=15,((_tmp117E.f1=(
void*)_tmp87B,((_tmp117E.f2=_tmp87A,((_tmp117E.f3=_tmp850,((_tmp117E.f4=_tmp851,
_tmp117E)))))))))),_tmp117D)))));};}_LL468: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp852=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp838;if(_tmp852->tag != 18)goto
_LL46A;else{_tmp853=(void*)_tmp852->f1;}}_LL469: {void*_tmp87E=Cyc_Tcutil_rsubstitute(
r,inst,_tmp853);if(_tmp87E == _tmp853)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp1181;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1180;return Cyc_Tcutil_copye(e,(
void*)((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((_tmp1180[0]=((_tmp1181.tag=18,((
_tmp1181.f1=(void*)_tmp87E,_tmp1181)))),_tmp1180)))));};}_LL46A: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp854=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp838;if(_tmp854->tag != 19)goto
_LL46C;else{_tmp855=_tmp854->f1;}}_LL46B: {struct Cyc_Absyn_Exp*_tmp881=Cyc_Tcutil_rsubsexp(
r,inst,_tmp855);if(_tmp881 == _tmp855)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp1184;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1183;return Cyc_Tcutil_copye(e,(
void*)((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((_tmp1183[0]=((_tmp1184.tag=19,((
_tmp1184.f1=_tmp881,_tmp1184)))),_tmp1183)))));};}_LL46C: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp856=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp838;if(_tmp856->tag != 20)goto
_LL46E;else{_tmp857=(void*)_tmp856->f1;_tmp858=(void*)_tmp856->f2;}}_LL46D: {
void*_tmp884=Cyc_Tcutil_rsubstitute(r,inst,_tmp857);if(_tmp884 == _tmp857)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp1187;struct Cyc_Absyn_Offsetof_e_struct*
_tmp1186;return Cyc_Tcutil_copye(e,(void*)((_tmp1186=_cycalloc(sizeof(*_tmp1186)),((
_tmp1186[0]=((_tmp1187.tag=20,((_tmp1187.f1=(void*)_tmp884,((_tmp1187.f2=(void*)
_tmp858,_tmp1187)))))),_tmp1186)))));};}_LL46E: {struct Cyc_Absyn_Valueof_e_struct*
_tmp859=(struct Cyc_Absyn_Valueof_e_struct*)_tmp838;if(_tmp859->tag != 39)goto
_LL470;else{_tmp85A=(void*)_tmp859->f1;}}_LL46F: {void*_tmp887=Cyc_Tcutil_rsubstitute(
r,inst,_tmp85A);if(_tmp887 == _tmp85A)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp118A;struct Cyc_Absyn_Valueof_e_struct*_tmp1189;return Cyc_Tcutil_copye(e,(
void*)((_tmp1189=_cycalloc(sizeof(*_tmp1189)),((_tmp1189[0]=((_tmp118A.tag=39,((
_tmp118A.f1=(void*)_tmp887,_tmp118A)))),_tmp1189)))));};}_LL470:;_LL471: {const
char*_tmp118D;void*_tmp118C;return(_tmp118C=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118D="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp118D,sizeof(char),46))),_tag_dyneither(_tmp118C,sizeof(void*),
0)));}_LL453:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp88C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp88E;
struct Cyc_Absyn_AggrInfo _tmp890;union Cyc_Absyn_AggrInfoU _tmp891;struct Cyc_List_List*
_tmp892;struct Cyc_Absyn_DatatypeInfo _tmp894;union Cyc_Absyn_DatatypeInfoU _tmp895;
struct Cyc_List_List*_tmp896;struct Cyc_Absyn_DatatypeFieldInfo _tmp898;union Cyc_Absyn_DatatypeFieldInfoU
_tmp899;struct Cyc_List_List*_tmp89A;struct _tuple2*_tmp89C;struct Cyc_List_List*
_tmp89D;struct Cyc_Absyn_Typedefdecl*_tmp89E;void**_tmp89F;struct Cyc_Absyn_ArrayInfo
_tmp8A1;void*_tmp8A2;struct Cyc_Absyn_Tqual _tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;
union Cyc_Absyn_Constraint*_tmp8A5;struct Cyc_Position_Segment*_tmp8A6;struct Cyc_Absyn_PtrInfo
_tmp8A8;void*_tmp8A9;struct Cyc_Absyn_Tqual _tmp8AA;struct Cyc_Absyn_PtrAtts _tmp8AB;
void*_tmp8AC;union Cyc_Absyn_Constraint*_tmp8AD;union Cyc_Absyn_Constraint*_tmp8AE;
union Cyc_Absyn_Constraint*_tmp8AF;struct Cyc_Absyn_FnInfo _tmp8B1;struct Cyc_List_List*
_tmp8B2;struct Cyc_Core_Opt*_tmp8B3;void*_tmp8B4;struct Cyc_List_List*_tmp8B5;int
_tmp8B6;struct Cyc_Absyn_VarargInfo*_tmp8B7;struct Cyc_List_List*_tmp8B8;struct Cyc_List_List*
_tmp8B9;struct Cyc_List_List*_tmp8BB;enum Cyc_Absyn_AggrKind _tmp8BD;struct Cyc_List_List*
_tmp8BE;struct Cyc_Core_Opt*_tmp8C0;void*_tmp8C2;void*_tmp8C4;void*_tmp8C5;void*
_tmp8C7;struct Cyc_Absyn_Exp*_tmp8C9;void*_tmp8D3;void*_tmp8D5;struct Cyc_List_List*
_tmp8D7;_LL473: {struct Cyc_Absyn_VarType_struct*_tmp88D=(struct Cyc_Absyn_VarType_struct*)
_tmp88C;if(_tmp88D->tag != 2)goto _LL475;else{_tmp88E=_tmp88D->f1;}}_LL474: {
struct _handler_cons _tmp8D8;_push_handler(& _tmp8D8);{int _tmp8DA=0;if(setjmp(
_tmp8D8.handler))_tmp8DA=1;if(!_tmp8DA){{void*_tmp8DB=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp88E);_npop_handler(0);return
_tmp8DB;};_pop_handler();}else{void*_tmp8D9=(void*)_exn_thrown;void*_tmp8DD=
_tmp8D9;_LL4A8: {struct Cyc_Core_Not_found_struct*_tmp8DE=(struct Cyc_Core_Not_found_struct*)
_tmp8DD;if(_tmp8DE->tag != Cyc_Core_Not_found)goto _LL4AA;}_LL4A9: return t;_LL4AA:;
_LL4AB:(void)_throw(_tmp8DD);_LL4A7:;}};}_LL475: {struct Cyc_Absyn_AggrType_struct*
_tmp88F=(struct Cyc_Absyn_AggrType_struct*)_tmp88C;if(_tmp88F->tag != 12)goto
_LL477;else{_tmp890=_tmp88F->f1;_tmp891=_tmp890.aggr_info;_tmp892=_tmp890.targs;}}
_LL476: {struct Cyc_List_List*_tmp8DF=Cyc_Tcutil_substs(rgn,inst,_tmp892);struct
Cyc_Absyn_AggrType_struct _tmp1193;struct Cyc_Absyn_AggrInfo _tmp1192;struct Cyc_Absyn_AggrType_struct*
_tmp1191;return _tmp8DF == _tmp892?t:(void*)((_tmp1191=_cycalloc(sizeof(*_tmp1191)),((
_tmp1191[0]=((_tmp1193.tag=12,((_tmp1193.f1=((_tmp1192.aggr_info=_tmp891,((
_tmp1192.targs=_tmp8DF,_tmp1192)))),_tmp1193)))),_tmp1191))));}_LL477: {struct
Cyc_Absyn_DatatypeType_struct*_tmp893=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp88C;if(_tmp893->tag != 3)goto _LL479;else{_tmp894=_tmp893->f1;_tmp895=_tmp894.datatype_info;
_tmp896=_tmp894.targs;}}_LL478: {struct Cyc_List_List*_tmp8E3=Cyc_Tcutil_substs(
rgn,inst,_tmp896);struct Cyc_Absyn_DatatypeType_struct _tmp1199;struct Cyc_Absyn_DatatypeInfo
_tmp1198;struct Cyc_Absyn_DatatypeType_struct*_tmp1197;return _tmp8E3 == _tmp896?t:(
void*)((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197[0]=((_tmp1199.tag=3,((
_tmp1199.f1=((_tmp1198.datatype_info=_tmp895,((_tmp1198.targs=_tmp8E3,_tmp1198)))),
_tmp1199)))),_tmp1197))));}_LL479: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp897=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp88C;if(_tmp897->tag != 4)
goto _LL47B;else{_tmp898=_tmp897->f1;_tmp899=_tmp898.field_info;_tmp89A=_tmp898.targs;}}
_LL47A: {struct Cyc_List_List*_tmp8E7=Cyc_Tcutil_substs(rgn,inst,_tmp89A);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp119F;struct Cyc_Absyn_DatatypeFieldInfo
_tmp119E;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp119D;return _tmp8E7 == 
_tmp89A?t:(void*)((_tmp119D=_cycalloc(sizeof(*_tmp119D)),((_tmp119D[0]=((
_tmp119F.tag=4,((_tmp119F.f1=((_tmp119E.field_info=_tmp899,((_tmp119E.targs=
_tmp8E7,_tmp119E)))),_tmp119F)))),_tmp119D))));}_LL47B: {struct Cyc_Absyn_TypedefType_struct*
_tmp89B=(struct Cyc_Absyn_TypedefType_struct*)_tmp88C;if(_tmp89B->tag != 18)goto
_LL47D;else{_tmp89C=_tmp89B->f1;_tmp89D=_tmp89B->f2;_tmp89E=_tmp89B->f3;_tmp89F=
_tmp89B->f4;}}_LL47C: {struct Cyc_List_List*_tmp8EB=Cyc_Tcutil_substs(rgn,inst,
_tmp89D);struct Cyc_Absyn_TypedefType_struct _tmp11A2;struct Cyc_Absyn_TypedefType_struct*
_tmp11A1;return _tmp8EB == _tmp89D?t:(void*)((_tmp11A1=_cycalloc(sizeof(*_tmp11A1)),((
_tmp11A1[0]=((_tmp11A2.tag=18,((_tmp11A2.f1=_tmp89C,((_tmp11A2.f2=_tmp8EB,((
_tmp11A2.f3=_tmp89E,((_tmp11A2.f4=_tmp89F,_tmp11A2)))))))))),_tmp11A1))));}
_LL47D: {struct Cyc_Absyn_ArrayType_struct*_tmp8A0=(struct Cyc_Absyn_ArrayType_struct*)
_tmp88C;if(_tmp8A0->tag != 9)goto _LL47F;else{_tmp8A1=_tmp8A0->f1;_tmp8A2=_tmp8A1.elt_type;
_tmp8A3=_tmp8A1.tq;_tmp8A4=_tmp8A1.num_elts;_tmp8A5=_tmp8A1.zero_term;_tmp8A6=
_tmp8A1.zt_loc;}}_LL47E: {void*_tmp8EE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A2);
struct Cyc_Absyn_ArrayType_struct _tmp11A8;struct Cyc_Absyn_ArrayInfo _tmp11A7;
struct Cyc_Absyn_ArrayType_struct*_tmp11A6;return _tmp8EE == _tmp8A2?t:(void*)((
_tmp11A6=_cycalloc(sizeof(*_tmp11A6)),((_tmp11A6[0]=((_tmp11A8.tag=9,((_tmp11A8.f1=((
_tmp11A7.elt_type=_tmp8EE,((_tmp11A7.tq=_tmp8A3,((_tmp11A7.num_elts=_tmp8A4,((
_tmp11A7.zero_term=_tmp8A5,((_tmp11A7.zt_loc=_tmp8A6,_tmp11A7)))))))))),_tmp11A8)))),
_tmp11A6))));}_LL47F: {struct Cyc_Absyn_PointerType_struct*_tmp8A7=(struct Cyc_Absyn_PointerType_struct*)
_tmp88C;if(_tmp8A7->tag != 5)goto _LL481;else{_tmp8A8=_tmp8A7->f1;_tmp8A9=_tmp8A8.elt_typ;
_tmp8AA=_tmp8A8.elt_tq;_tmp8AB=_tmp8A8.ptr_atts;_tmp8AC=_tmp8AB.rgn;_tmp8AD=
_tmp8AB.nullable;_tmp8AE=_tmp8AB.bounds;_tmp8AF=_tmp8AB.zero_term;}}_LL480: {
void*_tmp8F2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A9);void*_tmp8F3=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8AC);union Cyc_Absyn_Constraint*_tmp8F4=_tmp8AE;{void*_tmp8F5=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp8AE);struct Cyc_Absyn_Exp*_tmp8F7;_LL4AD: {struct Cyc_Absyn_Upper_b_struct*
_tmp8F6=(struct Cyc_Absyn_Upper_b_struct*)_tmp8F5;if(_tmp8F6->tag != 1)goto _LL4AF;
else{_tmp8F7=_tmp8F6->f1;}}_LL4AE: {struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp8F7);if(_tmp8F8 != _tmp8F7){struct Cyc_Absyn_Upper_b_struct _tmp11AB;
struct Cyc_Absyn_Upper_b_struct*_tmp11AA;_tmp8F4=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((
_tmp11AA[0]=((_tmp11AB.tag=1,((_tmp11AB.f1=_tmp8F8,_tmp11AB)))),_tmp11AA)))));}
goto _LL4AC;}_LL4AF:;_LL4B0: goto _LL4AC;_LL4AC:;}if((_tmp8F2 == _tmp8A9  && _tmp8F3
== _tmp8AC) && _tmp8F4 == _tmp8AE)return t;{struct Cyc_Absyn_PointerType_struct
_tmp11B5;struct Cyc_Absyn_PtrAtts _tmp11B4;struct Cyc_Absyn_PtrInfo _tmp11B3;struct
Cyc_Absyn_PointerType_struct*_tmp11B2;return(void*)((_tmp11B2=_cycalloc(sizeof(*
_tmp11B2)),((_tmp11B2[0]=((_tmp11B5.tag=5,((_tmp11B5.f1=((_tmp11B3.elt_typ=
_tmp8F2,((_tmp11B3.elt_tq=_tmp8AA,((_tmp11B3.ptr_atts=((_tmp11B4.rgn=_tmp8F3,((
_tmp11B4.nullable=_tmp8AD,((_tmp11B4.bounds=_tmp8F4,((_tmp11B4.zero_term=_tmp8AF,((
_tmp11B4.ptrloc=0,_tmp11B4)))))))))),_tmp11B3)))))),_tmp11B5)))),_tmp11B2))));};}
_LL481: {struct Cyc_Absyn_FnType_struct*_tmp8B0=(struct Cyc_Absyn_FnType_struct*)
_tmp88C;if(_tmp8B0->tag != 10)goto _LL483;else{_tmp8B1=_tmp8B0->f1;_tmp8B2=_tmp8B1.tvars;
_tmp8B3=_tmp8B1.effect;_tmp8B4=_tmp8B1.ret_typ;_tmp8B5=_tmp8B1.args;_tmp8B6=
_tmp8B1.c_varargs;_tmp8B7=_tmp8B1.cyc_varargs;_tmp8B8=_tmp8B1.rgn_po;_tmp8B9=
_tmp8B1.attributes;}}_LL482:{struct Cyc_List_List*_tmp8FF=_tmp8B2;for(0;_tmp8FF != 
0;_tmp8FF=_tmp8FF->tl){struct _tuple14*_tmp11BF;struct Cyc_Absyn_VarType_struct
_tmp11BE;struct Cyc_Absyn_VarType_struct*_tmp11BD;struct Cyc_List_List*_tmp11BC;
inst=((_tmp11BC=_region_malloc(rgn,sizeof(*_tmp11BC)),((_tmp11BC->hd=((_tmp11BF=
_region_malloc(rgn,sizeof(*_tmp11BF)),((_tmp11BF->f1=(struct Cyc_Absyn_Tvar*)
_tmp8FF->hd,((_tmp11BF->f2=(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((
_tmp11BD[0]=((_tmp11BE.tag=2,((_tmp11BE.f1=(struct Cyc_Absyn_Tvar*)_tmp8FF->hd,
_tmp11BE)))),_tmp11BD)))),_tmp11BF)))))),((_tmp11BC->tl=inst,_tmp11BC))))));}}{
struct Cyc_List_List*_tmp905;struct Cyc_List_List*_tmp906;struct _tuple1 _tmp904=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple24*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8B5));
_tmp905=_tmp904.f1;_tmp906=_tmp904.f2;{struct Cyc_List_List*_tmp907=Cyc_Tcutil_substs(
rgn,inst,_tmp906);struct Cyc_List_List*_tmp908=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple24*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp905,_tmp907));struct Cyc_Core_Opt*
eff2;if(_tmp8B3 == 0)eff2=0;else{void*_tmp909=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp8B3->v);if(_tmp909 == (void*)_tmp8B3->v)eff2=_tmp8B3;else{struct Cyc_Core_Opt*
_tmp11C0;eff2=((_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0->v=_tmp909,
_tmp11C0))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp8B7 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp90C;struct Cyc_Absyn_Tqual _tmp90D;void*
_tmp90E;int _tmp90F;struct Cyc_Absyn_VarargInfo _tmp90B=*_tmp8B7;_tmp90C=_tmp90B.name;
_tmp90D=_tmp90B.tq;_tmp90E=_tmp90B.type;_tmp90F=_tmp90B.inject;{void*_tmp910=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp90E);if(_tmp910 == _tmp90E)cyc_varargs2=_tmp8B7;else{struct Cyc_Absyn_VarargInfo*
_tmp11C1;cyc_varargs2=((_tmp11C1=_cycalloc(sizeof(*_tmp11C1)),((_tmp11C1->name=
_tmp90C,((_tmp11C1->tq=_tmp90D,((_tmp11C1->type=_tmp910,((_tmp11C1->inject=
_tmp90F,_tmp11C1))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp913;struct Cyc_List_List*_tmp914;struct _tuple1 _tmp912=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp8B8);_tmp913=_tmp912.f1;_tmp914=_tmp912.f2;{struct Cyc_List_List*
_tmp915=Cyc_Tcutil_substs(rgn,inst,_tmp913);struct Cyc_List_List*_tmp916=Cyc_Tcutil_substs(
rgn,inst,_tmp914);if(_tmp915 == _tmp913  && _tmp916 == _tmp914)rgn_po2=_tmp8B8;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp915,_tmp916);}{struct Cyc_Absyn_FnType_struct _tmp11C7;struct
Cyc_Absyn_FnInfo _tmp11C6;struct Cyc_Absyn_FnType_struct*_tmp11C5;return(void*)((
_tmp11C5=_cycalloc(sizeof(*_tmp11C5)),((_tmp11C5[0]=((_tmp11C7.tag=10,((_tmp11C7.f1=((
_tmp11C6.tvars=_tmp8B2,((_tmp11C6.effect=eff2,((_tmp11C6.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8B4),((_tmp11C6.args=_tmp908,((_tmp11C6.c_varargs=_tmp8B6,((
_tmp11C6.cyc_varargs=cyc_varargs2,((_tmp11C6.rgn_po=rgn_po2,((_tmp11C6.attributes=
_tmp8B9,_tmp11C6)))))))))))))))),_tmp11C7)))),_tmp11C5))));};};};};};};_LL483: {
struct Cyc_Absyn_TupleType_struct*_tmp8BA=(struct Cyc_Absyn_TupleType_struct*)
_tmp88C;if(_tmp8BA->tag != 11)goto _LL485;else{_tmp8BB=_tmp8BA->f1;}}_LL484: {
struct Cyc_List_List*_tmp91A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp8BB);struct Cyc_List_List*_tmp91B=Cyc_Tcutil_substs(rgn,inst,_tmp91A);if(
_tmp91B == _tmp91A)return t;{struct Cyc_List_List*_tmp91C=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8BB,_tmp91B);struct Cyc_Absyn_TupleType_struct
_tmp11CA;struct Cyc_Absyn_TupleType_struct*_tmp11C9;return(void*)((_tmp11C9=
_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[0]=((_tmp11CA.tag=11,((_tmp11CA.f1=
_tmp91C,_tmp11CA)))),_tmp11C9))));};}_LL485: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp8BC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp88C;if(_tmp8BC->tag != 13)goto
_LL487;else{_tmp8BD=_tmp8BC->f1;_tmp8BE=_tmp8BC->f2;}}_LL486: {struct Cyc_List_List*
_tmp91F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp8BE);struct
Cyc_List_List*_tmp920=Cyc_Tcutil_substs(rgn,inst,_tmp91F);if(_tmp920 == _tmp91F)
return t;{struct Cyc_List_List*_tmp921=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp8BE,_tmp920);struct Cyc_Absyn_AnonAggrType_struct
_tmp11CD;struct Cyc_Absyn_AnonAggrType_struct*_tmp11CC;return(void*)((_tmp11CC=
_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC[0]=((_tmp11CD.tag=13,((_tmp11CD.f1=
_tmp8BD,((_tmp11CD.f2=_tmp921,_tmp11CD)))))),_tmp11CC))));};}_LL487: {struct Cyc_Absyn_Evar_struct*
_tmp8BF=(struct Cyc_Absyn_Evar_struct*)_tmp88C;if(_tmp8BF->tag != 1)goto _LL489;
else{_tmp8C0=_tmp8BF->f2;}}_LL488: if(_tmp8C0 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp8C0->v);else{return t;}_LL489: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8C1=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp88C;if(_tmp8C1->tag != 16)goto
_LL48B;else{_tmp8C2=(void*)_tmp8C1->f1;}}_LL48A: {void*_tmp924=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8C2);struct Cyc_Absyn_RgnHandleType_struct _tmp11D0;struct Cyc_Absyn_RgnHandleType_struct*
_tmp11CF;return _tmp924 == _tmp8C2?t:(void*)((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((
_tmp11CF[0]=((_tmp11D0.tag=16,((_tmp11D0.f1=(void*)_tmp924,_tmp11D0)))),_tmp11CF))));}
_LL48B: {struct Cyc_Absyn_DynRgnType_struct*_tmp8C3=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp88C;if(_tmp8C3->tag != 17)goto _LL48D;else{_tmp8C4=(void*)_tmp8C3->f1;_tmp8C5=(
void*)_tmp8C3->f2;}}_LL48C: {void*_tmp927=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp8C4);void*_tmp928=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C5);struct Cyc_Absyn_DynRgnType_struct
_tmp11D3;struct Cyc_Absyn_DynRgnType_struct*_tmp11D2;return _tmp927 == _tmp8C4  && 
_tmp928 == _tmp8C5?t:(void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((
_tmp11D3.tag=17,((_tmp11D3.f1=(void*)_tmp927,((_tmp11D3.f2=(void*)_tmp928,
_tmp11D3)))))),_tmp11D2))));}_LL48D: {struct Cyc_Absyn_TagType_struct*_tmp8C6=(
struct Cyc_Absyn_TagType_struct*)_tmp88C;if(_tmp8C6->tag != 20)goto _LL48F;else{
_tmp8C7=(void*)_tmp8C6->f1;}}_LL48E: {void*_tmp92B=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp8C7);struct Cyc_Absyn_TagType_struct _tmp11D6;struct Cyc_Absyn_TagType_struct*
_tmp11D5;return _tmp92B == _tmp8C7?t:(void*)((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((
_tmp11D5[0]=((_tmp11D6.tag=20,((_tmp11D6.f1=(void*)_tmp92B,_tmp11D6)))),_tmp11D5))));}
_LL48F: {struct Cyc_Absyn_ValueofType_struct*_tmp8C8=(struct Cyc_Absyn_ValueofType_struct*)
_tmp88C;if(_tmp8C8->tag != 19)goto _LL491;else{_tmp8C9=_tmp8C8->f1;}}_LL490: {
struct Cyc_Absyn_Exp*_tmp92E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8C9);struct Cyc_Absyn_ValueofType_struct
_tmp11D9;struct Cyc_Absyn_ValueofType_struct*_tmp11D8;return _tmp92E == _tmp8C9?t:(
void*)((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D9.tag=19,((
_tmp11D9.f1=_tmp92E,_tmp11D9)))),_tmp11D8))));}_LL491: {struct Cyc_Absyn_EnumType_struct*
_tmp8CA=(struct Cyc_Absyn_EnumType_struct*)_tmp88C;if(_tmp8CA->tag != 14)goto
_LL493;}_LL492: goto _LL494;_LL493: {struct Cyc_Absyn_AnonEnumType_struct*_tmp8CB=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp88C;if(_tmp8CB->tag != 15)goto _LL495;}
_LL494: goto _LL496;_LL495: {struct Cyc_Absyn_VoidType_struct*_tmp8CC=(struct Cyc_Absyn_VoidType_struct*)
_tmp88C;if(_tmp8CC->tag != 0)goto _LL497;}_LL496: goto _LL498;_LL497: {struct Cyc_Absyn_IntType_struct*
_tmp8CD=(struct Cyc_Absyn_IntType_struct*)_tmp88C;if(_tmp8CD->tag != 6)goto _LL499;}
_LL498: goto _LL49A;_LL499: {struct Cyc_Absyn_FloatType_struct*_tmp8CE=(struct Cyc_Absyn_FloatType_struct*)
_tmp88C;if(_tmp8CE->tag != 7)goto _LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_DoubleType_struct*
_tmp8CF=(struct Cyc_Absyn_DoubleType_struct*)_tmp88C;if(_tmp8CF->tag != 8)goto
_LL49D;}_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp8D0=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp88C;if(_tmp8D0->tag != 22)goto _LL49F;}
_LL49E: goto _LL4A0;_LL49F: {struct Cyc_Absyn_HeapRgn_struct*_tmp8D1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp88C;if(_tmp8D1->tag != 21)goto _LL4A1;}_LL4A0: return t;_LL4A1: {struct Cyc_Absyn_RgnsEff_struct*
_tmp8D2=(struct Cyc_Absyn_RgnsEff_struct*)_tmp88C;if(_tmp8D2->tag != 25)goto _LL4A3;
else{_tmp8D3=(void*)_tmp8D2->f1;}}_LL4A2: {void*_tmp931=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8D3);struct Cyc_Absyn_RgnsEff_struct _tmp11DC;struct Cyc_Absyn_RgnsEff_struct*
_tmp11DB;return _tmp931 == _tmp8D3?t:(void*)((_tmp11DB=_cycalloc(sizeof(*_tmp11DB)),((
_tmp11DB[0]=((_tmp11DC.tag=25,((_tmp11DC.f1=(void*)_tmp931,_tmp11DC)))),_tmp11DB))));}
_LL4A3: {struct Cyc_Absyn_AccessEff_struct*_tmp8D4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp88C;if(_tmp8D4->tag != 23)goto _LL4A5;else{_tmp8D5=(void*)_tmp8D4->f1;}}_LL4A4: {
void*_tmp934=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D5);struct Cyc_Absyn_AccessEff_struct
_tmp11DF;struct Cyc_Absyn_AccessEff_struct*_tmp11DE;return _tmp934 == _tmp8D5?t:(
void*)((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE[0]=((_tmp11DF.tag=23,((
_tmp11DF.f1=(void*)_tmp934,_tmp11DF)))),_tmp11DE))));}_LL4A5: {struct Cyc_Absyn_JoinEff_struct*
_tmp8D6=(struct Cyc_Absyn_JoinEff_struct*)_tmp88C;if(_tmp8D6->tag != 24)goto _LL472;
else{_tmp8D7=_tmp8D6->f1;}}_LL4A6: {struct Cyc_List_List*_tmp937=Cyc_Tcutil_substs(
rgn,inst,_tmp8D7);struct Cyc_Absyn_JoinEff_struct _tmp11E2;struct Cyc_Absyn_JoinEff_struct*
_tmp11E1;return _tmp937 == _tmp8D7?t:(void*)((_tmp11E1=_cycalloc(sizeof(*_tmp11E1)),((
_tmp11E1[0]=((_tmp11E2.tag=24,((_tmp11E2.f1=_tmp937,_tmp11E2)))),_tmp11E1))));}
_LL472:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmp93A=(void*)ts->hd;struct Cyc_List_List*_tmp93B=ts->tl;void*
_tmp93C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp93A);struct Cyc_List_List*_tmp93D=Cyc_Tcutil_substs(
rgn,inst,_tmp93B);if(_tmp93A == _tmp93C  && _tmp93B == _tmp93D)return ts;{struct Cyc_List_List*
_tmp11E3;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp11E3=_cycalloc(
sizeof(*_tmp11E3)),((_tmp11E3->hd=_tmp93C,((_tmp11E3->tl=_tmp93D,_tmp11E3)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp93F=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,&
Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp11E6;struct _tuple14*_tmp11E5;return(
_tmp11E5=_cycalloc(sizeof(*_tmp11E5)),((_tmp11E5->f1=tv,((_tmp11E5->f2=Cyc_Absyn_new_evar(
_tmp93F,((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6->v=s,_tmp11E6))))),
_tmp11E5)))));}struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*env,struct Cyc_Absyn_Tvar*tv){struct _tuple15 _tmp943;struct Cyc_List_List*
_tmp944;struct _RegionHandle*_tmp945;struct _tuple15*_tmp942=env;_tmp943=*_tmp942;
_tmp944=_tmp943.f1;_tmp945=_tmp943.f2;{struct Cyc_Core_Opt*_tmp946=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp11E9;struct
_tuple14*_tmp11E8;return(_tmp11E8=_region_malloc(_tmp945,sizeof(*_tmp11E8)),((
_tmp11E8->f1=tv,((_tmp11E8->f2=Cyc_Absyn_new_evar(_tmp946,((_tmp11E9=_cycalloc(
sizeof(*_tmp11E9)),((_tmp11E9->v=_tmp944,_tmp11E9))))),_tmp11E8)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp11EF;void*_tmp11EE[3];struct Cyc_String_pa_struct _tmp11ED;struct Cyc_String_pa_struct
_tmp11EC;struct Cyc_String_pa_struct _tmp11EB;(_tmp11EB.tag=0,((_tmp11EB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp11EC.tag=0,((_tmp11EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp11ED.tag=0,((_tmp11ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp11EE[0]=& _tmp11ED,((_tmp11EE[1]=& _tmp11EC,((_tmp11EE[2]=&
_tmp11EB,Cyc_Tcutil_terr(loc,((_tmp11EF="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp11EF,sizeof(char),59))),_tag_dyneither(_tmp11EE,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp11F2;void*_tmp11F1;(_tmp11F1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F2="same type variable has different identity!",
_tag_dyneither(_tmp11F2,sizeof(char),43))),_tag_dyneither(_tmp11F1,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp11F3;return(_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3->hd=tv,((
_tmp11F3->tl=tvs,_tmp11F3)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp11F6;void*_tmp11F5;(_tmp11F5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F6="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp11F6,sizeof(char),39))),_tag_dyneither(_tmp11F5,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp953=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp953->identity == - 1){const char*
_tmp11F9;void*_tmp11F8;(_tmp11F8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F9="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp11F9,sizeof(char),41))),_tag_dyneither(_tmp11F8,sizeof(void*),
0)));}if(_tmp953->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp11FA;return(_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA->hd=tv,((
_tmp11FA->tl=tvs,_tmp11FA)))));};}struct _tuple25{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp11FD;void*_tmp11FC;(_tmp11FC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11FD="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp11FD,sizeof(char),44))),_tag_dyneither(_tmp11FC,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple25
_tmp95A;struct Cyc_Absyn_Tvar*_tmp95B;int _tmp95C;int*_tmp95D;struct _tuple25*
_tmp959=(struct _tuple25*)tvs2->hd;_tmp95A=*_tmp959;_tmp95B=_tmp95A.f1;_tmp95C=
_tmp95A.f2;_tmp95D=(int*)&(*_tmp959).f2;if(_tmp95B->identity == - 1){const char*
_tmp1200;void*_tmp11FF;(_tmp11FF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1200="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1200,sizeof(char),46))),_tag_dyneither(_tmp11FF,sizeof(void*),
0)));}if(_tmp95B->identity == tv->identity){*_tmp95D=*_tmp95D  || b;return tvs;}}}{
struct _tuple25*_tmp1203;struct Cyc_List_List*_tmp1202;return(_tmp1202=
_region_malloc(r,sizeof(*_tmp1202)),((_tmp1202->hd=((_tmp1203=_region_malloc(r,
sizeof(*_tmp1203)),((_tmp1203->f1=tv,((_tmp1203->f2=b,_tmp1203)))))),((_tmp1202->tl=
tvs,_tmp1202)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1207;void*_tmp1206[1];struct Cyc_String_pa_struct _tmp1205;(_tmp1205.tag=0,((
_tmp1205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1206[0]=& _tmp1205,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1207="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1207,sizeof(char),29))),_tag_dyneither(_tmp1206,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1208;return(_tmp1208=_cycalloc(sizeof(*
_tmp1208)),((_tmp1208->hd=tv,((_tmp1208->tl=tvs,_tmp1208)))));};}struct _tuple26{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp966=Cyc_Tcutil_compress(e);int _tmp968;_LL4B2: {struct Cyc_Absyn_Evar_struct*
_tmp967=(struct Cyc_Absyn_Evar_struct*)_tmp966;if(_tmp967->tag != 1)goto _LL4B4;
else{_tmp968=_tmp967->f3;}}_LL4B3:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple26 _tmp96A;void*_tmp96B;int _tmp96C;int*_tmp96D;struct
_tuple26*_tmp969=(struct _tuple26*)es2->hd;_tmp96A=*_tmp969;_tmp96B=_tmp96A.f1;
_tmp96C=_tmp96A.f2;_tmp96D=(int*)&(*_tmp969).f2;{void*_tmp96E=Cyc_Tcutil_compress(
_tmp96B);int _tmp970;_LL4B7: {struct Cyc_Absyn_Evar_struct*_tmp96F=(struct Cyc_Absyn_Evar_struct*)
_tmp96E;if(_tmp96F->tag != 1)goto _LL4B9;else{_tmp970=_tmp96F->f3;}}_LL4B8: if(
_tmp968 == _tmp970){if(b != *_tmp96D)*_tmp96D=1;return es;}goto _LL4B6;_LL4B9:;
_LL4BA: goto _LL4B6;_LL4B6:;};}}{struct _tuple26*_tmp120B;struct Cyc_List_List*
_tmp120A;return(_tmp120A=_region_malloc(r,sizeof(*_tmp120A)),((_tmp120A->hd=((
_tmp120B=_region_malloc(r,sizeof(*_tmp120B)),((_tmp120B->f1=e,((_tmp120B->f2=b,
_tmp120B)))))),((_tmp120A->tl=es,_tmp120A)))));};_LL4B4:;_LL4B5: return es;_LL4B1:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp120C;r=((_tmp120C=_region_malloc(rgn,sizeof(*_tmp120C)),((
_tmp120C->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp120C->tl=r,_tmp120C))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp975;int _tmp976;struct _tuple25 _tmp974=*((struct _tuple25*)
tvs->hd);_tmp975=_tmp974.f1;_tmp976=_tmp974.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmp975->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp120D;res=((_tmp120D=
_region_malloc(r,sizeof(*_tmp120D)),((_tmp120D->hd=(struct _tuple25*)tvs->hd,((
_tmp120D->tl=res,_tmp120D))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp1211;void*_tmp1210[1];struct Cyc_String_pa_struct
_tmp120F;(_tmp120F.tag=0,((_tmp120F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1210[0]=& _tmp120F,Cyc_Tcutil_terr(loc,((_tmp1211="bitfield %s does not have constant width",
_tag_dyneither(_tmp1211,sizeof(char),41))),_tag_dyneither(_tmp1210,sizeof(void*),
1)))))));}else{unsigned int _tmp97C;int _tmp97D;struct _tuple13 _tmp97B=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp97C=_tmp97B.f1;_tmp97D=_tmp97B.f2;if(!_tmp97D){
const char*_tmp1214;void*_tmp1213;(_tmp1213=0,Cyc_Tcutil_terr(loc,((_tmp1214="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1214,sizeof(char),45))),_tag_dyneither(_tmp1213,sizeof(void*),
0)));}w=_tmp97C;}{void*_tmp980=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmp982;_LL4BC: {struct Cyc_Absyn_IntType_struct*_tmp981=(struct Cyc_Absyn_IntType_struct*)
_tmp980;if(_tmp981->tag != 6)goto _LL4BE;else{_tmp982=_tmp981->f2;}}_LL4BD: switch(
_tmp982){case Cyc_Absyn_Char_sz: _LL4C0: if(w > 8){const char*_tmp1217;void*_tmp1216;(
_tmp1216=0,Cyc_Tcutil_terr(loc,((_tmp1217="bitfield larger than type",
_tag_dyneither(_tmp1217,sizeof(char),26))),_tag_dyneither(_tmp1216,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL4C1: if(w > 16){const char*_tmp121A;void*
_tmp1219;(_tmp1219=0,Cyc_Tcutil_terr(loc,((_tmp121A="bitfield larger than type",
_tag_dyneither(_tmp121A,sizeof(char),26))),_tag_dyneither(_tmp1219,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL4C2: goto _LL4C3;case Cyc_Absyn_Int_sz: _LL4C3:
if(w > 32){const char*_tmp121D;void*_tmp121C;(_tmp121C=0,Cyc_Tcutil_terr(loc,((
_tmp121D="bitfield larger than type",_tag_dyneither(_tmp121D,sizeof(char),26))),
_tag_dyneither(_tmp121C,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL4C4: if(w > 64){const char*_tmp1220;void*_tmp121F;(_tmp121F=0,Cyc_Tcutil_terr(
loc,((_tmp1220="bitfield larger than type",_tag_dyneither(_tmp1220,sizeof(char),
26))),_tag_dyneither(_tmp121F,sizeof(void*),0)));}break;}goto _LL4BB;_LL4BE:;
_LL4BF:{const char*_tmp1225;void*_tmp1224[2];struct Cyc_String_pa_struct _tmp1223;
struct Cyc_String_pa_struct _tmp1222;(_tmp1222.tag=0,((_tmp1222.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1223.tag=0,((_tmp1223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1224[0]=& _tmp1223,((_tmp1224[1]=& _tmp1222,Cyc_Tcutil_terr(loc,((_tmp1225="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1225,sizeof(char),52))),_tag_dyneither(_tmp1224,sizeof(void*),
2)))))))))))));}goto _LL4BB;_LL4BB:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmp98F=(void*)atts->hd;_LL4C7: {struct Cyc_Absyn_Packed_att_struct*_tmp990=(
struct Cyc_Absyn_Packed_att_struct*)_tmp98F;if(_tmp990->tag != 7)goto _LL4C9;}
_LL4C8: continue;_LL4C9: {struct Cyc_Absyn_Aligned_att_struct*_tmp991=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp98F;if(_tmp991->tag != 6)goto _LL4CB;}_LL4CA: continue;_LL4CB:;_LL4CC: {const
char*_tmp122A;void*_tmp1229[2];struct Cyc_String_pa_struct _tmp1228;struct Cyc_String_pa_struct
_tmp1227;(_tmp1227.tag=0,((_tmp1227.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1228.tag=0,((_tmp1228.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1229[0]=&
_tmp1228,((_tmp1229[1]=& _tmp1227,Cyc_Tcutil_terr(loc,((_tmp122A="bad attribute %s on member %s",
_tag_dyneither(_tmp122A,sizeof(char),30))),_tag_dyneither(_tmp1229,sizeof(void*),
2)))))))))))));}_LL4C6:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp996=t;struct
Cyc_Absyn_Typedefdecl*_tmp998;void**_tmp999;_LL4CE: {struct Cyc_Absyn_TypedefType_struct*
_tmp997=(struct Cyc_Absyn_TypedefType_struct*)_tmp996;if(_tmp997->tag != 18)goto
_LL4D0;else{_tmp998=_tmp997->f3;_tmp999=_tmp997->f4;}}_LL4CF: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp998))->tq).real_const  || (_tmp998->tq).print_const){if(
declared_const){const char*_tmp122D;void*_tmp122C;(_tmp122C=0,Cyc_Tcutil_warn(loc,((
_tmp122D="extra const",_tag_dyneither(_tmp122D,sizeof(char),12))),_tag_dyneither(
_tmp122C,sizeof(void*),0)));}return 1;}if((unsigned int)_tmp999)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp999);else{return declared_const;}_LL4D0:;_LL4D1: return
declared_const;_LL4CD:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td);static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(
struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn
!= 0){void*_tmp99C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
td->defn))->v);struct Cyc_Absyn_PtrInfo _tmp99E;struct Cyc_Absyn_PtrAtts _tmp99F;
void*_tmp9A0;_LL4D3: {struct Cyc_Absyn_PointerType_struct*_tmp99D=(struct Cyc_Absyn_PointerType_struct*)
_tmp99C;if(_tmp99D->tag != 5)goto _LL4D5;else{_tmp99E=_tmp99D->f1;_tmp99F=_tmp99E.ptr_atts;
_tmp9A0=_tmp99F.rgn;}}_LL4D4:{void*_tmp9A1=Cyc_Tcutil_compress(_tmp9A0);struct
Cyc_Absyn_Tvar*_tmp9A3;_LL4D8: {struct Cyc_Absyn_VarType_struct*_tmp9A2=(struct
Cyc_Absyn_VarType_struct*)_tmp9A1;if(_tmp9A2->tag != 2)goto _LL4DA;else{_tmp9A3=
_tmp9A2->f1;}}_LL4D9: return Cyc_Absyn_tvar_cmp(tvar,_tmp9A3)== 0;_LL4DA:;_LL4DB:
goto _LL4D7;_LL4D7:;}goto _LL4D2;_LL4D5:;_LL4D6: goto _LL4D2;_LL4D2:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td);static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmp9A4=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmp9A6;
struct Cyc_Absyn_Kind _tmp9A7;enum Cyc_Absyn_KindQual _tmp9A8;enum Cyc_Absyn_AliasQual
_tmp9A9;struct Cyc_Absyn_Kind*_tmp9AB;struct Cyc_Absyn_Kind _tmp9AC;enum Cyc_Absyn_KindQual
_tmp9AD;enum Cyc_Absyn_AliasQual _tmp9AE;_LL4DD: {struct Cyc_Absyn_Less_kb_struct*
_tmp9A5=(struct Cyc_Absyn_Less_kb_struct*)_tmp9A4;if(_tmp9A5->tag != 2)goto _LL4DF;
else{_tmp9A6=_tmp9A5->f2;_tmp9A7=*_tmp9A6;_tmp9A8=_tmp9A7.kind;if(_tmp9A8 != Cyc_Absyn_RgnKind)
goto _LL4DF;_tmp9A9=_tmp9A7.aliasqual;if(_tmp9A9 != Cyc_Absyn_Top)goto _LL4DF;}}
_LL4DE: goto _LL4E0;_LL4DF: {struct Cyc_Absyn_Eq_kb_struct*_tmp9AA=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp9A4;if(_tmp9AA->tag != 0)goto _LL4E1;else{_tmp9AB=_tmp9AA->f1;_tmp9AC=*_tmp9AB;
_tmp9AD=_tmp9AC.kind;if(_tmp9AD != Cyc_Absyn_RgnKind)goto _LL4E1;_tmp9AE=_tmp9AC.aliasqual;
if(_tmp9AE != Cyc_Absyn_Top)goto _LL4E1;}}_LL4E0: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL4E1:;_LL4E2: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL4DC:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple27{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmp9AF=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp9B2;struct Cyc_Core_Opt**_tmp9B3;struct Cyc_Core_Opt*
_tmp9B4;struct Cyc_Core_Opt**_tmp9B5;struct Cyc_Absyn_Tvar*_tmp9B7;struct Cyc_List_List*
_tmp9B9;struct _tuple2*_tmp9BB;struct Cyc_Absyn_Enumdecl*_tmp9BC;struct Cyc_Absyn_Enumdecl**
_tmp9BD;struct Cyc_Absyn_DatatypeInfo _tmp9BF;union Cyc_Absyn_DatatypeInfoU _tmp9C0;
union Cyc_Absyn_DatatypeInfoU*_tmp9C1;struct Cyc_List_List*_tmp9C2;struct Cyc_List_List**
_tmp9C3;struct Cyc_Absyn_DatatypeFieldInfo _tmp9C5;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9C6;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9C7;struct Cyc_List_List*_tmp9C8;
struct Cyc_Absyn_PtrInfo _tmp9CA;void*_tmp9CB;struct Cyc_Absyn_Tqual _tmp9CC;struct
Cyc_Absyn_Tqual*_tmp9CD;struct Cyc_Absyn_PtrAtts _tmp9CE;void*_tmp9CF;union Cyc_Absyn_Constraint*
_tmp9D0;union Cyc_Absyn_Constraint*_tmp9D1;union Cyc_Absyn_Constraint*_tmp9D2;void*
_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_Absyn_ArrayInfo _tmp9DB;void*
_tmp9DC;struct Cyc_Absyn_Tqual _tmp9DD;struct Cyc_Absyn_Tqual*_tmp9DE;struct Cyc_Absyn_Exp*
_tmp9DF;struct Cyc_Absyn_Exp**_tmp9E0;union Cyc_Absyn_Constraint*_tmp9E1;struct Cyc_Position_Segment*
_tmp9E2;struct Cyc_Absyn_FnInfo _tmp9E4;struct Cyc_List_List*_tmp9E5;struct Cyc_List_List**
_tmp9E6;struct Cyc_Core_Opt*_tmp9E7;struct Cyc_Core_Opt**_tmp9E8;void*_tmp9E9;
struct Cyc_List_List*_tmp9EA;int _tmp9EB;struct Cyc_Absyn_VarargInfo*_tmp9EC;struct
Cyc_List_List*_tmp9ED;struct Cyc_List_List*_tmp9EE;struct Cyc_List_List*_tmp9F0;
enum Cyc_Absyn_AggrKind _tmp9F2;struct Cyc_List_List*_tmp9F3;struct Cyc_Absyn_AggrInfo
_tmp9F5;union Cyc_Absyn_AggrInfoU _tmp9F6;union Cyc_Absyn_AggrInfoU*_tmp9F7;struct
Cyc_List_List*_tmp9F8;struct Cyc_List_List**_tmp9F9;struct _tuple2*_tmp9FB;struct
Cyc_List_List*_tmp9FC;struct Cyc_List_List**_tmp9FD;struct Cyc_Absyn_Typedefdecl*
_tmp9FE;struct Cyc_Absyn_Typedefdecl**_tmp9FF;void**_tmpA00;void***_tmpA01;void*
_tmpA05;void*_tmpA07;void*_tmpA08;void*_tmpA0A;void*_tmpA0C;struct Cyc_List_List*
_tmpA0E;_LL4E4: {struct Cyc_Absyn_VoidType_struct*_tmp9B0=(struct Cyc_Absyn_VoidType_struct*)
_tmp9AF;if(_tmp9B0->tag != 0)goto _LL4E6;}_LL4E5: goto _LL4E3;_LL4E6: {struct Cyc_Absyn_Evar_struct*
_tmp9B1=(struct Cyc_Absyn_Evar_struct*)_tmp9AF;if(_tmp9B1->tag != 1)goto _LL4E8;
else{_tmp9B2=_tmp9B1->f1;_tmp9B3=(struct Cyc_Core_Opt**)& _tmp9B1->f1;_tmp9B4=
_tmp9B1->f2;_tmp9B5=(struct Cyc_Core_Opt**)& _tmp9B1->f2;}}_LL4E7: if(*_tmp9B3 == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmp9B3))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmp9B3))->v,expected_kind))*_tmp9B3=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmp9B5=(
struct Cyc_Core_Opt*)& urgn;else{*_tmp9B5=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1230;struct Cyc_Absyn_Less_kb_struct*
_tmp122F;struct Cyc_Absyn_Tvar*_tmpA0F=Cyc_Tcutil_new_tvar((void*)((_tmp122F=
_cycalloc(sizeof(*_tmp122F)),((_tmp122F[0]=((_tmp1230.tag=2,((_tmp1230.f1=0,((
_tmp1230.f2=expected_kind,_tmp1230)))))),_tmp122F)))));{struct Cyc_Absyn_VarType_struct*
_tmp1236;struct Cyc_Absyn_VarType_struct _tmp1235;struct Cyc_Core_Opt*_tmp1234;*
_tmp9B5=((_tmp1234=_cycalloc(sizeof(*_tmp1234)),((_tmp1234->v=(void*)((_tmp1236=
_cycalloc(sizeof(*_tmp1236)),((_tmp1236[0]=((_tmp1235.tag=2,((_tmp1235.f1=
_tmpA0F,_tmp1235)))),_tmp1236)))),_tmp1234))));}_tmp9B7=_tmpA0F;goto _LL4E9;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL4E3;_LL4E8: {struct Cyc_Absyn_VarType_struct*_tmp9B6=(
struct Cyc_Absyn_VarType_struct*)_tmp9AF;if(_tmp9B6->tag != 2)goto _LL4EA;else{
_tmp9B7=_tmp9B6->f1;}}_LL4E9:{void*_tmpA15=Cyc_Absyn_compress_kb(_tmp9B7->kind);
struct Cyc_Core_Opt*_tmpA17;struct Cyc_Core_Opt**_tmpA18;_LL519: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpA16=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpA15;if(_tmpA16->tag != 1)goto
_LL51B;else{_tmpA17=_tmpA16->f1;_tmpA18=(struct Cyc_Core_Opt**)& _tmpA16->f1;}}
_LL51A:{struct Cyc_Absyn_Less_kb_struct*_tmp123C;struct Cyc_Absyn_Less_kb_struct
_tmp123B;struct Cyc_Core_Opt*_tmp123A;*_tmpA18=((_tmp123A=_cycalloc(sizeof(*
_tmp123A)),((_tmp123A->v=(void*)((_tmp123C=_cycalloc(sizeof(*_tmp123C)),((
_tmp123C[0]=((_tmp123B.tag=2,((_tmp123B.f1=0,((_tmp123B.f2=expected_kind,
_tmp123B)))))),_tmp123C)))),_tmp123A))));}goto _LL518;_LL51B:;_LL51C: goto _LL518;
_LL518:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9B7);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp9B7,put_in_effect);{void*_tmpA1C=Cyc_Absyn_compress_kb(_tmp9B7->kind);struct
Cyc_Core_Opt*_tmpA1E;struct Cyc_Core_Opt**_tmpA1F;struct Cyc_Absyn_Kind*_tmpA20;
_LL51E: {struct Cyc_Absyn_Less_kb_struct*_tmpA1D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpA1C;if(_tmpA1D->tag != 2)goto _LL520;else{_tmpA1E=_tmpA1D->f1;_tmpA1F=(struct
Cyc_Core_Opt**)& _tmpA1D->f1;_tmpA20=_tmpA1D->f2;}}_LL51F: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpA20)){struct Cyc_Absyn_Less_kb_struct*_tmp1242;struct Cyc_Absyn_Less_kb_struct
_tmp1241;struct Cyc_Core_Opt*_tmp1240;*_tmpA1F=((_tmp1240=_cycalloc(sizeof(*
_tmp1240)),((_tmp1240->v=(void*)((_tmp1242=_cycalloc(sizeof(*_tmp1242)),((
_tmp1242[0]=((_tmp1241.tag=2,((_tmp1241.f1=0,((_tmp1241.f2=expected_kind,
_tmp1241)))))),_tmp1242)))),_tmp1240))));}goto _LL51D;_LL520:;_LL521: goto _LL51D;
_LL51D:;}goto _LL4E3;_LL4EA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9B8=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmp9AF;if(_tmp9B8->tag != 15)goto _LL4EC;else{
_tmp9B9=_tmp9B8->f1;}}_LL4EB: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpA24=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp9B9 != 0;_tmp9B9=_tmp9B9->tl){
struct Cyc_Absyn_Enumfield*_tmpA25=(struct Cyc_Absyn_Enumfield*)_tmp9B9->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA25->name).f2)){
const char*_tmp1246;void*_tmp1245[1];struct Cyc_String_pa_struct _tmp1244;(_tmp1244.tag=
0,((_tmp1244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA25->name).f2),((
_tmp1245[0]=& _tmp1244,Cyc_Tcutil_terr(_tmpA25->loc,((_tmp1246="duplicate enum field name %s",
_tag_dyneither(_tmp1246,sizeof(char),29))),_tag_dyneither(_tmp1245,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1247;prev_fields=((_tmp1247=
_region_malloc(_tmpA24,sizeof(*_tmp1247)),((_tmp1247->hd=(*_tmpA25->name).f2,((
_tmp1247->tl=prev_fields,_tmp1247))))));}if(_tmpA25->tag == 0)_tmpA25->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpA25->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpA25->tag))){const char*_tmp124B;void*
_tmp124A[1];struct Cyc_String_pa_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA25->name).f2),((_tmp124A[0]=&
_tmp1249,Cyc_Tcutil_terr(loc,((_tmp124B="enum field %s: expression is not constant",
_tag_dyneither(_tmp124B,sizeof(char),42))),_tag_dyneither(_tmp124A,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpA25->tag))).f1;tag_count=t1 + 1;(*_tmpA25->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp1251;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp1250;struct _tuple26*_tmp124F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple26*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpA25->name).f2,(struct _tuple26*)((_tmp124F=_cycalloc(sizeof(*_tmp124F)),((
_tmp124F->f1=(void*)((_tmp1251=_cycalloc(sizeof(*_tmp1251)),((_tmp1251[0]=((
_tmp1250.tag=4,((_tmp1250.f1=(void*)t,((_tmp1250.f2=_tmpA25,_tmp1250)))))),
_tmp1251)))),((_tmp124F->f2=1,_tmp124F)))))));};};}}goto _LL4E3;}_LL4EC: {struct
Cyc_Absyn_EnumType_struct*_tmp9BA=(struct Cyc_Absyn_EnumType_struct*)_tmp9AF;if(
_tmp9BA->tag != 14)goto _LL4EE;else{_tmp9BB=_tmp9BA->f1;_tmp9BC=_tmp9BA->f2;
_tmp9BD=(struct Cyc_Absyn_Enumdecl**)& _tmp9BA->f2;}}_LL4ED: if(*_tmp9BD == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9BD))->fields == 0){struct
_handler_cons _tmpA30;_push_handler(& _tmpA30);{int _tmpA32=0;if(setjmp(_tmpA30.handler))
_tmpA32=1;if(!_tmpA32){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp9BB);*_tmp9BD=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpA31=(void*)_exn_thrown;void*_tmpA34=_tmpA31;_LL523: {struct Cyc_Dict_Absent_struct*
_tmpA35=(struct Cyc_Dict_Absent_struct*)_tmpA34;if(_tmpA35->tag != Cyc_Dict_Absent)
goto _LL525;}_LL524: {struct Cyc_Tcenv_Genv*_tmpA36=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1252;struct Cyc_Absyn_Enumdecl*_tmpA37=(_tmp1252=
_cycalloc(sizeof(*_tmp1252)),((_tmp1252->sc=Cyc_Absyn_Extern,((_tmp1252->name=
_tmp9BB,((_tmp1252->fields=0,_tmp1252)))))));Cyc_Tc_tcEnumdecl(te,_tmpA36,loc,
_tmpA37);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9BB);*
_tmp9BD=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL522;};}_LL525:;_LL526:(void)_throw(
_tmpA34);_LL522:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp9BD);*_tmp9BB=(ed->name)[0];goto _LL4E3;};_LL4EE: {struct Cyc_Absyn_DatatypeType_struct*
_tmp9BE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp9AF;if(_tmp9BE->tag != 3)goto
_LL4F0;else{_tmp9BF=_tmp9BE->f1;_tmp9C0=_tmp9BF.datatype_info;_tmp9C1=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmp9BE->f1).datatype_info;_tmp9C2=_tmp9BF.targs;_tmp9C3=(struct Cyc_List_List**)&(
_tmp9BE->f1).targs;}}_LL4EF: {struct Cyc_List_List*_tmpA39=*_tmp9C3;{union Cyc_Absyn_DatatypeInfoU
_tmpA3A=*_tmp9C1;struct Cyc_Absyn_UnknownDatatypeInfo _tmpA3B;struct _tuple2*
_tmpA3C;int _tmpA3D;struct Cyc_Absyn_Datatypedecl**_tmpA3E;struct Cyc_Absyn_Datatypedecl*
_tmpA3F;_LL528: if((_tmpA3A.UnknownDatatype).tag != 1)goto _LL52A;_tmpA3B=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpA3A.UnknownDatatype).val;_tmpA3C=_tmpA3B.name;
_tmpA3D=_tmpA3B.is_extensible;_LL529: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpA40;_push_handler(& _tmpA40);{int _tmpA42=0;if(setjmp(
_tmpA40.handler))_tmpA42=1;if(!_tmpA42){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpA3C);;_pop_handler();}else{void*_tmpA41=(void*)_exn_thrown;void*_tmpA44=
_tmpA41;_LL52D: {struct Cyc_Dict_Absent_struct*_tmpA45=(struct Cyc_Dict_Absent_struct*)
_tmpA44;if(_tmpA45->tag != Cyc_Dict_Absent)goto _LL52F;}_LL52E: {struct Cyc_Tcenv_Genv*
_tmpA46=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1253;struct Cyc_Absyn_Datatypedecl*
_tmpA47=(_tmp1253=_cycalloc(sizeof(*_tmp1253)),((_tmp1253->sc=Cyc_Absyn_Extern,((
_tmp1253->name=_tmpA3C,((_tmp1253->tvs=0,((_tmp1253->fields=0,((_tmp1253->is_extensible=
_tmpA3D,_tmp1253)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpA46,loc,_tmpA47);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA3C);if(_tmpA39 != 0){{const char*
_tmp1257;void*_tmp1256[1];struct Cyc_String_pa_struct _tmp1255;(_tmp1255.tag=0,((
_tmp1255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA3C)),((_tmp1256[0]=& _tmp1255,Cyc_Tcutil_terr(loc,((_tmp1257="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1257,sizeof(char),47))),_tag_dyneither(_tmp1256,sizeof(void*),
1)))))));}return cvtenv;}goto _LL52C;}_LL52F:;_LL530:(void)_throw(_tmpA44);_LL52C:;}};}
if(_tmpA3D  && !(*tudp)->is_extensible){const char*_tmp125B;void*_tmp125A[1];
struct Cyc_String_pa_struct _tmp1259;(_tmp1259.tag=0,((_tmp1259.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA3C)),((
_tmp125A[0]=& _tmp1259,Cyc_Tcutil_terr(loc,((_tmp125B="datatype %s was not declared @extensible",
_tag_dyneither(_tmp125B,sizeof(char),41))),_tag_dyneither(_tmp125A,sizeof(void*),
1)))))));}*_tmp9C1=Cyc_Absyn_KnownDatatype(tudp);_tmpA3F=*tudp;goto _LL52B;}
_LL52A: if((_tmpA3A.KnownDatatype).tag != 2)goto _LL527;_tmpA3E=(struct Cyc_Absyn_Datatypedecl**)(
_tmpA3A.KnownDatatype).val;_tmpA3F=*_tmpA3E;_LL52B: {struct Cyc_List_List*tvs=
_tmpA3F->tvs;for(0;_tmpA39 != 0  && tvs != 0;(_tmpA39=_tmpA39->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA39->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp125C;struct _tuple0 _tmpA50=(_tmp125C.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp125C.f2=t,_tmp125C)));void*_tmpA51;void*_tmpA53;struct Cyc_Absyn_Tvar*_tmpA55;
_LL532: _tmpA51=_tmpA50.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpA52=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA51;if(_tmpA52->tag != 1)goto _LL534;};_tmpA53=_tmpA50.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA54=(struct Cyc_Absyn_VarType_struct*)_tmpA53;if(_tmpA54->tag != 2)goto _LL534;
else{_tmpA55=_tmpA54->f1;}};_LL533: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA55);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA55,1);continue;_LL534:;_LL535: goto _LL531;_LL531:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpA39 != 0){const char*_tmp1260;void*_tmp125F[1];
struct Cyc_String_pa_struct _tmp125E;(_tmp125E.tag=0,((_tmp125E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA3F->name)),((
_tmp125F[0]=& _tmp125E,Cyc_Tcutil_terr(loc,((_tmp1260="too many type arguments for datatype %s",
_tag_dyneither(_tmp1260,sizeof(char),40))),_tag_dyneither(_tmp125F,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1261;hidden_ts=((_tmp1261=_cycalloc(sizeof(*_tmp1261)),((_tmp1261->hd=e,((
_tmp1261->tl=hidden_ts,_tmp1261))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmp9C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9C3,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL527;}_LL527:;}goto
_LL4E3;}_LL4F0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp9C4=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp9AF;if(_tmp9C4->tag != 4)goto _LL4F2;else{_tmp9C5=_tmp9C4->f1;_tmp9C6=_tmp9C5.field_info;
_tmp9C7=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9C4->f1).field_info;_tmp9C8=
_tmp9C5.targs;}}_LL4F1:{union Cyc_Absyn_DatatypeFieldInfoU _tmpA5A=*_tmp9C7;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA5B;struct _tuple2*_tmpA5C;struct _tuple2*
_tmpA5D;int _tmpA5E;struct _tuple3 _tmpA5F;struct Cyc_Absyn_Datatypedecl*_tmpA60;
struct Cyc_Absyn_Datatypefield*_tmpA61;_LL537: if((_tmpA5A.UnknownDatatypefield).tag
!= 1)goto _LL539;_tmpA5B=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA5A.UnknownDatatypefield).val;
_tmpA5C=_tmpA5B.datatype_name;_tmpA5D=_tmpA5B.field_name;_tmpA5E=_tmpA5B.is_extensible;
_LL538: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpA62;_push_handler(& _tmpA62);{int _tmpA64=0;if(setjmp(
_tmpA62.handler))_tmpA64=1;if(!_tmpA64){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpA5C);;_pop_handler();}else{void*_tmpA63=(void*)_exn_thrown;void*_tmpA66=
_tmpA63;_LL53C: {struct Cyc_Dict_Absent_struct*_tmpA67=(struct Cyc_Dict_Absent_struct*)
_tmpA66;if(_tmpA67->tag != Cyc_Dict_Absent)goto _LL53E;}_LL53D:{const char*_tmp1265;
void*_tmp1264[1];struct Cyc_String_pa_struct _tmp1263;(_tmp1263.tag=0,((_tmp1263.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA5C)),((
_tmp1264[0]=& _tmp1263,Cyc_Tcutil_terr(loc,((_tmp1265="unbound datatype %s",
_tag_dyneither(_tmp1265,sizeof(char),20))),_tag_dyneither(_tmp1264,sizeof(void*),
1)))))));}return cvtenv;_LL53E:;_LL53F:(void)_throw(_tmpA66);_LL53B:;}};}{struct
_handler_cons _tmpA6B;_push_handler(& _tmpA6B);{int _tmpA6D=0;if(setjmp(_tmpA6B.handler))
_tmpA6D=1;if(!_tmpA6D){{struct _RegionHandle*_tmpA6E=Cyc_Tcenv_get_fnrgn(te);void*
_tmpA6F=Cyc_Tcenv_lookup_ordinary(_tmpA6E,te,loc,_tmpA5D);struct Cyc_Absyn_Datatypedecl*
_tmpA71;struct Cyc_Absyn_Datatypefield*_tmpA72;_LL541: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpA70=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpA6F;if(_tmpA70->tag != 2)goto
_LL543;else{_tmpA71=_tmpA70->f1;_tmpA72=_tmpA70->f2;}}_LL542: tuf=_tmpA72;tud=
_tmpA71;if(_tmpA5E  && !tud->is_extensible){const char*_tmp1269;void*_tmp1268[1];
struct Cyc_String_pa_struct _tmp1267;(_tmp1267.tag=0,((_tmp1267.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA5C)),((
_tmp1268[0]=& _tmp1267,Cyc_Tcutil_terr(loc,((_tmp1269="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1269,sizeof(char),41))),_tag_dyneither(_tmp1268,sizeof(void*),
1)))))));}goto _LL540;_LL543:;_LL544:{const char*_tmp126E;void*_tmp126D[2];struct
Cyc_String_pa_struct _tmp126C;struct Cyc_String_pa_struct _tmp126B;(_tmp126B.tag=0,((
_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA5C)),((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA5D)),((_tmp126D[0]=& _tmp126C,((
_tmp126D[1]=& _tmp126B,Cyc_Tcutil_terr(loc,((_tmp126E="unbound field %s in type datatype %s",
_tag_dyneither(_tmp126E,sizeof(char),37))),_tag_dyneither(_tmp126D,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpA7A=cvtenv;_npop_handler(0);return
_tmpA7A;};_LL540:;};_pop_handler();}else{void*_tmpA6C=(void*)_exn_thrown;void*
_tmpA7C=_tmpA6C;_LL546: {struct Cyc_Dict_Absent_struct*_tmpA7D=(struct Cyc_Dict_Absent_struct*)
_tmpA7C;if(_tmpA7D->tag != Cyc_Dict_Absent)goto _LL548;}_LL547:{const char*_tmp1273;
void*_tmp1272[2];struct Cyc_String_pa_struct _tmp1271;struct Cyc_String_pa_struct
_tmp1270;(_tmp1270.tag=0,((_tmp1270.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA5C)),((_tmp1271.tag=0,((_tmp1271.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA5D)),((
_tmp1272[0]=& _tmp1271,((_tmp1272[1]=& _tmp1270,Cyc_Tcutil_terr(loc,((_tmp1273="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1273,sizeof(char),37))),_tag_dyneither(_tmp1272,sizeof(void*),
2)))))))))))));}return cvtenv;_LL548:;_LL549:(void)_throw(_tmpA7C);_LL545:;}};}*
_tmp9C7=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpA60=tud;_tmpA61=tuf;goto _LL53A;}
_LL539: if((_tmpA5A.KnownDatatypefield).tag != 2)goto _LL536;_tmpA5F=(struct _tuple3)(
_tmpA5A.KnownDatatypefield).val;_tmpA60=_tmpA5F.f1;_tmpA61=_tmpA5F.f2;_LL53A: {
struct Cyc_List_List*tvs=_tmpA60->tvs;for(0;_tmp9C8 != 0  && tvs != 0;(_tmp9C8=
_tmp9C8->tl,tvs=tvs->tl)){void*t=(void*)_tmp9C8->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp1274;struct _tuple0 _tmpA83=(
_tmp1274.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1274.f2=t,_tmp1274)));void*
_tmpA84;void*_tmpA86;struct Cyc_Absyn_Tvar*_tmpA88;_LL54B: _tmpA84=_tmpA83.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpA85=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA84;if(_tmpA85->tag != 1)goto _LL54D;};_tmpA86=_tmpA83.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA87=(struct Cyc_Absyn_VarType_struct*)_tmpA86;if(_tmpA87->tag != 2)goto _LL54D;
else{_tmpA88=_tmpA87->f1;}};_LL54C: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA88);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA88,1);continue;_LL54D:;_LL54E: goto _LL54A;_LL54A:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp9C8 != 0){const char*_tmp1279;void*_tmp1278[2];
struct Cyc_String_pa_struct _tmp1277;struct Cyc_String_pa_struct _tmp1276;(_tmp1276.tag=
0,((_tmp1276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA61->name)),((_tmp1277.tag=0,((_tmp1277.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA60->name)),((_tmp1278[0]=& _tmp1277,((
_tmp1278[1]=& _tmp1276,Cyc_Tcutil_terr(loc,((_tmp1279="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp1279,sizeof(char),43))),_tag_dyneither(_tmp1278,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp127E;void*_tmp127D[2];struct Cyc_String_pa_struct
_tmp127C;struct Cyc_String_pa_struct _tmp127B;(_tmp127B.tag=0,((_tmp127B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA61->name)),((
_tmp127C.tag=0,((_tmp127C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA60->name)),((_tmp127D[0]=& _tmp127C,((_tmp127D[1]=& _tmp127B,Cyc_Tcutil_terr(
loc,((_tmp127E="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp127E,sizeof(char),42))),_tag_dyneither(_tmp127D,sizeof(void*),2)))))))))))));}
goto _LL536;}_LL536:;}goto _LL4E3;_LL4F2: {struct Cyc_Absyn_PointerType_struct*
_tmp9C9=(struct Cyc_Absyn_PointerType_struct*)_tmp9AF;if(_tmp9C9->tag != 5)goto
_LL4F4;else{_tmp9CA=_tmp9C9->f1;_tmp9CB=_tmp9CA.elt_typ;_tmp9CC=_tmp9CA.elt_tq;
_tmp9CD=(struct Cyc_Absyn_Tqual*)&(_tmp9C9->f1).elt_tq;_tmp9CE=_tmp9CA.ptr_atts;
_tmp9CF=_tmp9CE.rgn;_tmp9D0=_tmp9CE.nullable;_tmp9D1=_tmp9CE.bounds;_tmp9D2=
_tmp9CE.zero_term;}}_LL4F3: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9CB,1);_tmp9CD->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp9CD->print_const,_tmp9CB);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL54F: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL550: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL551: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp9CF,1);{union Cyc_Absyn_Constraint*
_tmpA91=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmp9D2);union Cyc_Absyn_Constraint _tmpA92;int _tmpA93;union Cyc_Absyn_Constraint
_tmpA94;int _tmpA95;_LL554: _tmpA92=*_tmpA91;if((_tmpA92.No_constr).tag != 3)goto
_LL556;_tmpA93=(int)(_tmpA92.No_constr).val;_LL555:{void*_tmpA96=Cyc_Tcutil_compress(
_tmp9CB);enum Cyc_Absyn_Size_of _tmpA98;_LL55B: {struct Cyc_Absyn_IntType_struct*
_tmpA97=(struct Cyc_Absyn_IntType_struct*)_tmpA96;if(_tmpA97->tag != 6)goto _LL55D;
else{_tmpA98=_tmpA97->f2;if(_tmpA98 != Cyc_Absyn_Char_sz)goto _LL55D;}}_LL55C:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9D2,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL55A;_LL55D:;_LL55E:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp9D2,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL55A;
_LL55A:;}goto _LL553;_LL556: _tmpA94=*_tmpA91;if((_tmpA94.Eq_constr).tag != 1)goto
_LL558;_tmpA95=(int)(_tmpA94.Eq_constr).val;if(_tmpA95 != 1)goto _LL558;_LL557: if(
!Cyc_Tcutil_admits_zero(_tmp9CB)){const char*_tmp1282;void*_tmp1281[1];struct Cyc_String_pa_struct
_tmp1280;(_tmp1280.tag=0,((_tmp1280.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9CB)),((_tmp1281[0]=& _tmp1280,Cyc_Tcutil_terr(
loc,((_tmp1282="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp1282,sizeof(char),53))),_tag_dyneither(_tmp1281,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL553;_LL558:;_LL559: is_zero_terminated=0;
goto _LL553;_LL553:;}{void*_tmpA9C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp9D1);struct Cyc_Absyn_Exp*
_tmpA9F;_LL560: {struct Cyc_Absyn_DynEither_b_struct*_tmpA9D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpA9C;if(_tmpA9D->tag != 0)goto _LL562;}_LL561: goto _LL55F;_LL562: {struct Cyc_Absyn_Upper_b_struct*
_tmpA9E=(struct Cyc_Absyn_Upper_b_struct*)_tmpA9C;if(_tmpA9E->tag != 1)goto _LL55F;
else{_tmpA9F=_tmpA9E->f1;}}_LL563: {struct _RegionHandle*_tmpAA0=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpAA1=Cyc_Tcenv_allow_valueof(_tmpAA0,te);Cyc_Tcexp_tcExp(
_tmpAA1,0,_tmpA9F);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA9F,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA9F)){const char*_tmp1285;void*
_tmp1284;(_tmp1284=0,Cyc_Tcutil_terr(loc,((_tmp1285="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp1285,sizeof(char),49))),_tag_dyneither(_tmp1284,sizeof(void*),
0)));}{unsigned int _tmpAA5;int _tmpAA6;struct _tuple13 _tmpAA4=Cyc_Evexp_eval_const_uint_exp(
_tmpA9F);_tmpAA5=_tmpAA4.f1;_tmpAA6=_tmpAA4.f2;if(is_zero_terminated  && (!
_tmpAA6  || _tmpAA5 < 1)){const char*_tmp1288;void*_tmp1287;(_tmp1287=0,Cyc_Tcutil_terr(
loc,((_tmp1288="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp1288,sizeof(char),55))),_tag_dyneither(_tmp1287,sizeof(void*),
0)));}goto _LL55F;};}_LL55F:;}goto _LL4E3;};}_LL4F4: {struct Cyc_Absyn_TagType_struct*
_tmp9D3=(struct Cyc_Absyn_TagType_struct*)_tmp9AF;if(_tmp9D3->tag != 20)goto _LL4F6;
else{_tmp9D4=(void*)_tmp9D3->f1;}}_LL4F5: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp9D4,1);goto _LL4E3;_LL4F6: {struct Cyc_Absyn_ValueofType_struct*
_tmp9D5=(struct Cyc_Absyn_ValueofType_struct*)_tmp9AF;if(_tmp9D5->tag != 19)goto
_LL4F8;else{_tmp9D6=_tmp9D5->f1;}}_LL4F7: {struct _RegionHandle*_tmpAA9=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpAAA=Cyc_Tcenv_allow_valueof(_tmpAA9,te);Cyc_Tcexp_tcExp(
_tmpAAA,0,_tmp9D6);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9D6)){const char*
_tmp128B;void*_tmp128A;(_tmp128A=0,Cyc_Tcutil_terr(loc,((_tmp128B="valueof_t requires an int expression",
_tag_dyneither(_tmp128B,sizeof(char),37))),_tag_dyneither(_tmp128A,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9D6,te,cvtenv);goto
_LL4E3;}_LL4F8: {struct Cyc_Absyn_IntType_struct*_tmp9D7=(struct Cyc_Absyn_IntType_struct*)
_tmp9AF;if(_tmp9D7->tag != 6)goto _LL4FA;}_LL4F9: goto _LL4FB;_LL4FA: {struct Cyc_Absyn_FloatType_struct*
_tmp9D8=(struct Cyc_Absyn_FloatType_struct*)_tmp9AF;if(_tmp9D8->tag != 7)goto
_LL4FC;}_LL4FB: goto _LL4FD;_LL4FC: {struct Cyc_Absyn_DoubleType_struct*_tmp9D9=(
struct Cyc_Absyn_DoubleType_struct*)_tmp9AF;if(_tmp9D9->tag != 8)goto _LL4FE;}
_LL4FD: goto _LL4E3;_LL4FE: {struct Cyc_Absyn_ArrayType_struct*_tmp9DA=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9AF;if(_tmp9DA->tag != 9)goto _LL500;else{_tmp9DB=_tmp9DA->f1;_tmp9DC=_tmp9DB.elt_type;
_tmp9DD=_tmp9DB.tq;_tmp9DE=(struct Cyc_Absyn_Tqual*)&(_tmp9DA->f1).tq;_tmp9DF=
_tmp9DB.num_elts;_tmp9E0=(struct Cyc_Absyn_Exp**)&(_tmp9DA->f1).num_elts;_tmp9E1=
_tmp9DB.zero_term;_tmp9E2=_tmp9DB.zt_loc;}}_LL4FF: {struct Cyc_Absyn_Exp*_tmpAAD=*
_tmp9E0;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmp9DC,1);_tmp9DE->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9DE->print_const,
_tmp9DC);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpAAE=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9E1);union Cyc_Absyn_Constraint
_tmpAAF;int _tmpAB0;union Cyc_Absyn_Constraint _tmpAB1;int _tmpAB2;_LL565: _tmpAAF=*
_tmpAAE;if((_tmpAAF.No_constr).tag != 3)goto _LL567;_tmpAB0=(int)(_tmpAAF.No_constr).val;
_LL566:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9E1,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL564;_LL567: _tmpAB1=*_tmpAAE;if((_tmpAB1.Eq_constr).tag
!= 1)goto _LL569;_tmpAB2=(int)(_tmpAB1.Eq_constr).val;if(_tmpAB2 != 1)goto _LL569;
_LL568: if(!Cyc_Tcutil_admits_zero(_tmp9DC)){const char*_tmp128F;void*_tmp128E[1];
struct Cyc_String_pa_struct _tmp128D;(_tmp128D.tag=0,((_tmp128D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9DC)),((
_tmp128E[0]=& _tmp128D,Cyc_Tcutil_terr(loc,((_tmp128F="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp128F,sizeof(char),51))),_tag_dyneither(_tmp128E,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL564;_LL569:;_LL56A: is_zero_terminated=0;
goto _LL564;_LL564:;}if(_tmpAAD == 0){if(is_zero_terminated)*_tmp9E0=(_tmpAAD=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp1292;void*
_tmp1291;(_tmp1291=0,Cyc_Tcutil_warn(loc,((_tmp1292="array bound defaults to 1 here",
_tag_dyneither(_tmp1292,sizeof(char),31))),_tag_dyneither(_tmp1291,sizeof(void*),
0)));}*_tmp9E0=(_tmpAAD=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpAAD);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpAAD)){const char*_tmp1295;void*_tmp1294;(_tmp1294=0,Cyc_Tcutil_terr(loc,((
_tmp1295="array bounds expression is not constant",_tag_dyneither(_tmp1295,
sizeof(char),40))),_tag_dyneither(_tmp1294,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpAAD)){const char*_tmp1298;void*_tmp1297;(_tmp1297=0,
Cyc_Tcutil_terr(loc,((_tmp1298="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp1298,sizeof(char),47))),_tag_dyneither(_tmp1297,sizeof(void*),
0)));}{unsigned int _tmpABD;int _tmpABE;struct _tuple13 _tmpABC=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpAAD);_tmpABD=_tmpABC.f1;_tmpABE=_tmpABC.f2;if((
is_zero_terminated  && _tmpABE) && _tmpABD < 1){const char*_tmp129B;void*_tmp129A;(
_tmp129A=0,Cyc_Tcutil_warn(loc,((_tmp129B="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp129B,sizeof(char),48))),_tag_dyneither(_tmp129A,sizeof(void*),
0)));}if((_tmpABE  && _tmpABD < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp129E;void*
_tmp129D;(_tmp129D=0,Cyc_Tcutil_warn(loc,((_tmp129E="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp129E,sizeof(char),75))),_tag_dyneither(_tmp129D,sizeof(void*),
0)));}*_tmp9E0=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4E3;};};}
_LL500: {struct Cyc_Absyn_FnType_struct*_tmp9E3=(struct Cyc_Absyn_FnType_struct*)
_tmp9AF;if(_tmp9E3->tag != 10)goto _LL502;else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E4.tvars;
_tmp9E6=(struct Cyc_List_List**)&(_tmp9E3->f1).tvars;_tmp9E7=_tmp9E4.effect;
_tmp9E8=(struct Cyc_Core_Opt**)&(_tmp9E3->f1).effect;_tmp9E9=_tmp9E4.ret_typ;
_tmp9EA=_tmp9E4.args;_tmp9EB=_tmp9E4.c_varargs;_tmp9EC=_tmp9E4.cyc_varargs;
_tmp9ED=_tmp9E4.rgn_po;_tmp9EE=_tmp9E4.attributes;}}_LL501: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp9EE != 0;
_tmp9EE=_tmp9EE->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp9EE->hd)){const char*
_tmp12A2;void*_tmp12A1[1];struct Cyc_String_pa_struct _tmp12A0;(_tmp12A0.tag=0,((
_tmp12A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp9EE->hd)),((_tmp12A1[0]=& _tmp12A0,Cyc_Tcutil_terr(loc,((_tmp12A2="bad function type attribute %s",
_tag_dyneither(_tmp12A2,sizeof(char),31))),_tag_dyneither(_tmp12A1,sizeof(void*),
1)))))));}{void*_tmpAC6=(void*)_tmp9EE->hd;enum Cyc_Absyn_Format_Type _tmpACB;int
_tmpACC;int _tmpACD;_LL56C: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAC7=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpAC6;if(_tmpAC7->tag != 1)goto _LL56E;}_LL56D: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL56B;_LL56E: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAC8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAC6;if(_tmpAC8->tag != 2)goto
_LL570;}_LL56F: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL56B;_LL570: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpAC9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpAC6;if(_tmpAC9->tag != 3)goto _LL572;}_LL571: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL56B;_LL572: {struct Cyc_Absyn_Format_att_struct*_tmpACA=(
struct Cyc_Absyn_Format_att_struct*)_tmpAC6;if(_tmpACA->tag != 19)goto _LL574;else{
_tmpACB=_tmpACA->f1;_tmpACC=_tmpACA->f2;_tmpACD=_tmpACA->f3;}}_LL573: if(!
seen_format){seen_format=1;ft=_tmpACB;fmt_desc_arg=_tmpACC;fmt_arg_start=_tmpACD;}
else{const char*_tmp12A5;void*_tmp12A4;(_tmp12A4=0,Cyc_Tcutil_terr(loc,((_tmp12A5="function can't have multiple format attributes",
_tag_dyneither(_tmp12A5,sizeof(char),47))),_tag_dyneither(_tmp12A4,sizeof(void*),
0)));}goto _LL56B;_LL574:;_LL575: goto _LL56B;_LL56B:;};}if(num_convs > 1){const char*
_tmp12A8;void*_tmp12A7;(_tmp12A7=0,Cyc_Tcutil_terr(loc,((_tmp12A8="function can't have multiple calling conventions",
_tag_dyneither(_tmp12A8,sizeof(char),49))),_tag_dyneither(_tmp12A7,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp9E6);{struct Cyc_List_List*b=*
_tmp9E6;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpAD2=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpAD4;struct Cyc_Absyn_Kind _tmpAD5;enum Cyc_Absyn_KindQual
_tmpAD6;_LL577: {struct Cyc_Absyn_Eq_kb_struct*_tmpAD3=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAD2;if(_tmpAD3->tag != 0)goto _LL579;else{_tmpAD4=_tmpAD3->f1;_tmpAD5=*_tmpAD4;
_tmpAD6=_tmpAD5.kind;if(_tmpAD6 != Cyc_Absyn_MemKind)goto _LL579;}}_LL578:{const
char*_tmp12AC;void*_tmp12AB[1];struct Cyc_String_pa_struct _tmp12AA;(_tmp12AA.tag=
0,((_tmp12AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp12AB[0]=& _tmp12AA,Cyc_Tcutil_terr(loc,((_tmp12AC="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp12AC,sizeof(char),51))),_tag_dyneither(_tmp12AB,sizeof(void*),
1)))))));}goto _LL576;_LL579:;_LL57A: goto _LL576;_LL576:;};}}{struct _RegionHandle*
_tmpADA=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp12AD;struct Cyc_Tcutil_CVTEnv
_tmpADB=(_tmp12AD.r=_tmpADA,((_tmp12AD.kind_env=cvtenv.kind_env,((_tmp12AD.free_vars=
0,((_tmp12AD.free_evars=0,((_tmp12AD.generalize_evars=cvtenv.generalize_evars,((
_tmp12AD.fn_result=1,_tmp12AD)))))))))));_tmpADB=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpADB,& Cyc_Tcutil_tmk,_tmp9E9,1);_tmpADB.fn_result=0;{struct Cyc_List_List*
a=_tmp9EA;for(0;a != 0;a=a->tl){struct _tuple9*_tmpADC=(struct _tuple9*)a->hd;void*
_tmpADD=(*_tmpADC).f3;_tmpADB=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpADB,& Cyc_Tcutil_tmk,
_tmpADD,1);((*_tmpADC).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpADC).f2).print_const,_tmpADD);}}if(_tmp9EC != 0){if(_tmp9EB){const char*
_tmp12B0;void*_tmp12AF;(_tmp12AF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B0="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp12B0,sizeof(char),29))),_tag_dyneither(_tmp12AF,sizeof(void*),
0)));}{void*_tmpAE1;int _tmpAE2;struct Cyc_Absyn_VarargInfo _tmpAE0=*_tmp9EC;
_tmpAE1=_tmpAE0.type;_tmpAE2=_tmpAE0.inject;_tmpADB=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpADB,& Cyc_Tcutil_tmk,_tmpAE1,1);(_tmp9EC->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp9EC->tq).print_const,_tmpAE1);if(_tmpAE2){void*_tmpAE3=Cyc_Tcutil_compress(
_tmpAE1);struct Cyc_Absyn_PtrInfo _tmpAE5;void*_tmpAE6;struct Cyc_Absyn_PtrAtts
_tmpAE7;union Cyc_Absyn_Constraint*_tmpAE8;union Cyc_Absyn_Constraint*_tmpAE9;
_LL57C: {struct Cyc_Absyn_PointerType_struct*_tmpAE4=(struct Cyc_Absyn_PointerType_struct*)
_tmpAE3;if(_tmpAE4->tag != 5)goto _LL57E;else{_tmpAE5=_tmpAE4->f1;_tmpAE6=_tmpAE5.elt_typ;
_tmpAE7=_tmpAE5.ptr_atts;_tmpAE8=_tmpAE7.bounds;_tmpAE9=_tmpAE7.zero_term;}}
_LL57D:{void*_tmpAEA=Cyc_Tcutil_compress(_tmpAE6);_LL581: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAEB=(struct Cyc_Absyn_DatatypeType_struct*)_tmpAEA;if(_tmpAEB->tag != 3)goto
_LL583;}_LL582: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpAE9)){const char*_tmp12B3;void*_tmp12B2;(_tmp12B2=0,Cyc_Tcutil_terr(loc,((
_tmp12B3="can't inject into a zeroterm pointer",_tag_dyneither(_tmp12B3,sizeof(
char),37))),_tag_dyneither(_tmp12B2,sizeof(void*),0)));}{void*_tmpAEE=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpAE8);_LL586: {struct Cyc_Absyn_DynEither_b_struct*_tmpAEF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAEE;if(_tmpAEF->tag != 0)goto _LL588;}_LL587:{const char*_tmp12B6;void*_tmp12B5;(
_tmp12B5=0,Cyc_Tcutil_terr(loc,((_tmp12B6="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp12B6,sizeof(char),44))),_tag_dyneither(_tmp12B5,sizeof(void*),
0)));}goto _LL585;_LL588:;_LL589: goto _LL585;_LL585:;}goto _LL580;_LL583:;_LL584:{
const char*_tmp12B9;void*_tmp12B8;(_tmp12B8=0,Cyc_Tcutil_terr(loc,((_tmp12B9="can't inject a non-datatype type",
_tag_dyneither(_tmp12B9,sizeof(char),33))),_tag_dyneither(_tmp12B8,sizeof(void*),
0)));}goto _LL580;_LL580:;}goto _LL57B;_LL57E:;_LL57F:{const char*_tmp12BC;void*
_tmp12BB;(_tmp12BB=0,Cyc_Tcutil_terr(loc,((_tmp12BC="expecting a datatype pointer type",
_tag_dyneither(_tmp12BC,sizeof(char),34))),_tag_dyneither(_tmp12BB,sizeof(void*),
0)));}goto _LL57B;_LL57B:;}};}if(seen_format){int _tmpAF6=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp9EA);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpAF6) || 
fmt_arg_start < 0) || _tmp9EC == 0  && fmt_arg_start != 0) || _tmp9EC != 0  && 
fmt_arg_start != _tmpAF6 + 1){const char*_tmp12BF;void*_tmp12BE;(_tmp12BE=0,Cyc_Tcutil_terr(
loc,((_tmp12BF="bad format descriptor",_tag_dyneither(_tmp12BF,sizeof(char),22))),
_tag_dyneither(_tmp12BE,sizeof(void*),0)));}else{void*_tmpAFA;struct _tuple9
_tmpAF9=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9EA,
fmt_desc_arg - 1);_tmpAFA=_tmpAF9.f3;{void*_tmpAFB=Cyc_Tcutil_compress(_tmpAFA);
struct Cyc_Absyn_PtrInfo _tmpAFD;void*_tmpAFE;struct Cyc_Absyn_PtrAtts _tmpAFF;union
Cyc_Absyn_Constraint*_tmpB00;union Cyc_Absyn_Constraint*_tmpB01;_LL58B: {struct
Cyc_Absyn_PointerType_struct*_tmpAFC=(struct Cyc_Absyn_PointerType_struct*)
_tmpAFB;if(_tmpAFC->tag != 5)goto _LL58D;else{_tmpAFD=_tmpAFC->f1;_tmpAFE=_tmpAFD.elt_typ;
_tmpAFF=_tmpAFD.ptr_atts;_tmpB00=_tmpAFF.bounds;_tmpB01=_tmpAFF.zero_term;}}
_LL58C:{struct _tuple0 _tmp12C0;struct _tuple0 _tmpB03=(_tmp12C0.f1=Cyc_Tcutil_compress(
_tmpAFE),((_tmp12C0.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpB00),_tmp12C0)));void*_tmpB04;enum Cyc_Absyn_Sign
_tmpB06;enum Cyc_Absyn_Size_of _tmpB07;void*_tmpB08;_LL590: _tmpB04=_tmpB03.f1;{
struct Cyc_Absyn_IntType_struct*_tmpB05=(struct Cyc_Absyn_IntType_struct*)_tmpB04;
if(_tmpB05->tag != 6)goto _LL592;else{_tmpB06=_tmpB05->f1;if(_tmpB06 != Cyc_Absyn_None)
goto _LL592;_tmpB07=_tmpB05->f2;if(_tmpB07 != Cyc_Absyn_Char_sz)goto _LL592;}};
_tmpB08=_tmpB03.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpB09=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpB08;if(_tmpB09->tag != 0)goto _LL592;};_LL591: goto _LL58F;_LL592:;_LL593:{const
char*_tmp12C3;void*_tmp12C2;(_tmp12C2=0,Cyc_Tcutil_terr(loc,((_tmp12C3="format descriptor is not a char ? type",
_tag_dyneither(_tmp12C3,sizeof(char),39))),_tag_dyneither(_tmp12C2,sizeof(void*),
0)));}goto _LL58F;_LL58F:;}goto _LL58A;_LL58D:;_LL58E:{const char*_tmp12C6;void*
_tmp12C5;(_tmp12C5=0,Cyc_Tcutil_terr(loc,((_tmp12C6="format descriptor is not a char ? type",
_tag_dyneither(_tmp12C6,sizeof(char),39))),_tag_dyneither(_tmp12C5,sizeof(void*),
0)));}goto _LL58A;_LL58A:;}if(fmt_arg_start != 0){int problem;{struct _tuple27
_tmp12C7;struct _tuple27 _tmpB0F=(_tmp12C7.f1=ft,((_tmp12C7.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9EC))->type)),
_tmp12C7)));enum Cyc_Absyn_Format_Type _tmpB10;void*_tmpB11;struct Cyc_Absyn_DatatypeInfo
_tmpB13;union Cyc_Absyn_DatatypeInfoU _tmpB14;struct Cyc_Absyn_Datatypedecl**
_tmpB15;struct Cyc_Absyn_Datatypedecl*_tmpB16;enum Cyc_Absyn_Format_Type _tmpB17;
void*_tmpB18;struct Cyc_Absyn_DatatypeInfo _tmpB1A;union Cyc_Absyn_DatatypeInfoU
_tmpB1B;struct Cyc_Absyn_Datatypedecl**_tmpB1C;struct Cyc_Absyn_Datatypedecl*
_tmpB1D;_LL595: _tmpB10=_tmpB0F.f1;if(_tmpB10 != Cyc_Absyn_Printf_ft)goto _LL597;
_tmpB11=_tmpB0F.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpB12=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpB11;if(_tmpB12->tag != 3)goto _LL597;else{_tmpB13=_tmpB12->f1;_tmpB14=_tmpB13.datatype_info;
if((_tmpB14.KnownDatatype).tag != 2)goto _LL597;_tmpB15=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB14.KnownDatatype).val;_tmpB16=*_tmpB15;}};_LL596: problem=Cyc_Absyn_qvar_cmp(
_tmpB16->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL594;_LL597: _tmpB17=
_tmpB0F.f1;if(_tmpB17 != Cyc_Absyn_Scanf_ft)goto _LL599;_tmpB18=_tmpB0F.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpB19=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpB18;if(_tmpB19->tag != 3)goto _LL599;else{_tmpB1A=_tmpB19->f1;_tmpB1B=_tmpB1A.datatype_info;
if((_tmpB1B.KnownDatatype).tag != 2)goto _LL599;_tmpB1C=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB1B.KnownDatatype).val;_tmpB1D=*_tmpB1C;}};_LL598: problem=Cyc_Absyn_qvar_cmp(
_tmpB1D->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL594;_LL599:;_LL59A:
problem=1;goto _LL594;_LL594:;}if(problem){const char*_tmp12CA;void*_tmp12C9;(
_tmp12C9=0,Cyc_Tcutil_terr(loc,((_tmp12CA="format attribute and vararg types don't match",
_tag_dyneither(_tmp12CA,sizeof(char),46))),_tag_dyneither(_tmp12C9,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmp9ED;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpB21;void*_tmpB22;void*_tmpB23;struct _tuple0*_tmpB20=(struct _tuple0*)
rpo->hd;_tmpB21=*_tmpB20;_tmpB22=_tmpB21.f1;_tmpB23=_tmpB21.f2;_tmpADB=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpADB,& Cyc_Tcutil_ek,_tmpB22,1);_tmpADB=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpADB,& Cyc_Tcutil_trk,_tmpB23,1);}}if(*_tmp9E8 != 0)_tmpADB=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpADB,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp9E8))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpADB.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB25;int _tmpB26;struct
_tuple25 _tmpB24=*((struct _tuple25*)tvs->hd);_tmpB25=_tmpB24.f1;_tmpB26=_tmpB24.f2;
if(!_tmpB26)continue;{void*_tmpB27=Cyc_Absyn_compress_kb(_tmpB25->kind);struct
Cyc_Core_Opt*_tmpB29;struct Cyc_Core_Opt**_tmpB2A;struct Cyc_Absyn_Kind*_tmpB2B;
struct Cyc_Absyn_Kind*_tmpB2D;struct Cyc_Absyn_Kind*_tmpB2F;struct Cyc_Absyn_Kind
_tmpB30;enum Cyc_Absyn_KindQual _tmpB31;struct Cyc_Absyn_Kind*_tmpB33;struct Cyc_Absyn_Kind
_tmpB34;enum Cyc_Absyn_KindQual _tmpB35;struct Cyc_Core_Opt*_tmpB37;struct Cyc_Core_Opt**
_tmpB38;struct Cyc_Absyn_Kind*_tmpB39;struct Cyc_Absyn_Kind _tmpB3A;enum Cyc_Absyn_KindQual
_tmpB3B;struct Cyc_Absyn_Kind*_tmpB3D;struct Cyc_Absyn_Kind _tmpB3E;enum Cyc_Absyn_KindQual
_tmpB3F;struct Cyc_Core_Opt*_tmpB41;struct Cyc_Core_Opt**_tmpB42;_LL59C:{struct Cyc_Absyn_Less_kb_struct*
_tmpB28=(struct Cyc_Absyn_Less_kb_struct*)_tmpB27;if(_tmpB28->tag != 2)goto _LL59E;
else{_tmpB29=_tmpB28->f1;_tmpB2A=(struct Cyc_Core_Opt**)& _tmpB28->f1;_tmpB2B=
_tmpB28->f2;}}if(!(_tmpB2B->kind == Cyc_Absyn_RgnKind))goto _LL59E;_LL59D: if(
_tmpB2B->aliasqual == Cyc_Absyn_Top){*_tmpB2A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpB2D=_tmpB2B;goto _LL59F;}*_tmpB2A=Cyc_Tcutil_kind_to_bound_opt(_tmpB2B);
_tmpB2D=_tmpB2B;goto _LL59F;_LL59E:{struct Cyc_Absyn_Eq_kb_struct*_tmpB2C=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpB27;if(_tmpB2C->tag != 0)goto _LL5A0;else{_tmpB2D=
_tmpB2C->f1;}}if(!(_tmpB2D->kind == Cyc_Absyn_RgnKind))goto _LL5A0;_LL59F:{struct
Cyc_Absyn_AccessEff_struct*_tmp12D9;struct Cyc_Absyn_VarType_struct*_tmp12D8;
struct Cyc_Absyn_VarType_struct _tmp12D7;struct Cyc_Absyn_AccessEff_struct _tmp12D6;
struct Cyc_List_List*_tmp12D5;effect=((_tmp12D5=_cycalloc(sizeof(*_tmp12D5)),((
_tmp12D5->hd=(void*)((_tmp12D9=_cycalloc(sizeof(*_tmp12D9)),((_tmp12D9[0]=((
_tmp12D6.tag=23,((_tmp12D6.f1=(void*)((void*)((_tmp12D8=_cycalloc(sizeof(*
_tmp12D8)),((_tmp12D8[0]=((_tmp12D7.tag=2,((_tmp12D7.f1=_tmpB25,_tmp12D7)))),
_tmp12D8))))),_tmp12D6)))),_tmp12D9)))),((_tmp12D5->tl=effect,_tmp12D5))))));}
goto _LL59B;_LL5A0: {struct Cyc_Absyn_Less_kb_struct*_tmpB2E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB27;if(_tmpB2E->tag != 2)goto _LL5A2;else{_tmpB2F=_tmpB2E->f2;_tmpB30=*_tmpB2F;
_tmpB31=_tmpB30.kind;if(_tmpB31 != Cyc_Absyn_IntKind)goto _LL5A2;}}_LL5A1: goto
_LL5A3;_LL5A2: {struct Cyc_Absyn_Eq_kb_struct*_tmpB32=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB27;if(_tmpB32->tag != 0)goto _LL5A4;else{_tmpB33=_tmpB32->f1;_tmpB34=*_tmpB33;
_tmpB35=_tmpB34.kind;if(_tmpB35 != Cyc_Absyn_IntKind)goto _LL5A4;}}_LL5A3: goto
_LL59B;_LL5A4: {struct Cyc_Absyn_Less_kb_struct*_tmpB36=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB27;if(_tmpB36->tag != 2)goto _LL5A6;else{_tmpB37=_tmpB36->f1;_tmpB38=(struct
Cyc_Core_Opt**)& _tmpB36->f1;_tmpB39=_tmpB36->f2;_tmpB3A=*_tmpB39;_tmpB3B=_tmpB3A.kind;
if(_tmpB3B != Cyc_Absyn_EffKind)goto _LL5A6;}}_LL5A5:*_tmpB38=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL5A7;_LL5A6: {struct Cyc_Absyn_Eq_kb_struct*_tmpB3C=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpB27;if(_tmpB3C->tag != 0)goto _LL5A8;else{_tmpB3D=
_tmpB3C->f1;_tmpB3E=*_tmpB3D;_tmpB3F=_tmpB3E.kind;if(_tmpB3F != Cyc_Absyn_EffKind)
goto _LL5A8;}}_LL5A7:{struct Cyc_Absyn_VarType_struct*_tmp12DF;struct Cyc_Absyn_VarType_struct
_tmp12DE;struct Cyc_List_List*_tmp12DD;effect=((_tmp12DD=_cycalloc(sizeof(*
_tmp12DD)),((_tmp12DD->hd=(void*)((_tmp12DF=_cycalloc(sizeof(*_tmp12DF)),((
_tmp12DF[0]=((_tmp12DE.tag=2,((_tmp12DE.f1=_tmpB25,_tmp12DE)))),_tmp12DF)))),((
_tmp12DD->tl=effect,_tmp12DD))))));}goto _LL59B;_LL5A8: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB40=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB27;if(_tmpB40->tag != 1)goto
_LL5AA;else{_tmpB41=_tmpB40->f1;_tmpB42=(struct Cyc_Core_Opt**)& _tmpB40->f1;}}
_LL5A9:{struct Cyc_Absyn_Less_kb_struct*_tmp12E5;struct Cyc_Absyn_Less_kb_struct
_tmp12E4;struct Cyc_Core_Opt*_tmp12E3;*_tmpB42=((_tmp12E3=_cycalloc(sizeof(*
_tmp12E3)),((_tmp12E3->v=(void*)((_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((
_tmp12E5[0]=((_tmp12E4.tag=2,((_tmp12E4.f1=0,((_tmp12E4.f2=& Cyc_Tcutil_ak,
_tmp12E4)))))),_tmp12E5)))),_tmp12E3))));}goto _LL5AB;_LL5AA:;_LL5AB:{struct Cyc_Absyn_RgnsEff_struct*
_tmp12F4;struct Cyc_Absyn_VarType_struct*_tmp12F3;struct Cyc_Absyn_VarType_struct
_tmp12F2;struct Cyc_Absyn_RgnsEff_struct _tmp12F1;struct Cyc_List_List*_tmp12F0;
effect=((_tmp12F0=_cycalloc(sizeof(*_tmp12F0)),((_tmp12F0->hd=(void*)((_tmp12F4=
_cycalloc(sizeof(*_tmp12F4)),((_tmp12F4[0]=((_tmp12F1.tag=25,((_tmp12F1.f1=(void*)((
void*)((_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((_tmp12F3[0]=((_tmp12F2.tag=2,((
_tmp12F2.f1=_tmpB25,_tmp12F2)))),_tmp12F3))))),_tmp12F1)))),_tmp12F4)))),((
_tmp12F0->tl=effect,_tmp12F0))))));}goto _LL59B;_LL59B:;};}}{struct Cyc_List_List*
ts=_tmpADB.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpB54;int _tmpB55;struct
_tuple26 _tmpB53=*((struct _tuple26*)ts->hd);_tmpB54=_tmpB53.f1;_tmpB55=_tmpB53.f2;
if(!_tmpB55)continue;{struct Cyc_Absyn_Kind*_tmpB56=Cyc_Tcutil_typ_kind(_tmpB54);
struct Cyc_Absyn_Kind _tmpB57;enum Cyc_Absyn_KindQual _tmpB58;struct Cyc_Absyn_Kind
_tmpB59;enum Cyc_Absyn_KindQual _tmpB5A;struct Cyc_Absyn_Kind _tmpB5B;enum Cyc_Absyn_KindQual
_tmpB5C;_LL5AD: _tmpB57=*_tmpB56;_tmpB58=_tmpB57.kind;if(_tmpB58 != Cyc_Absyn_RgnKind)
goto _LL5AF;_LL5AE:{struct Cyc_Absyn_AccessEff_struct*_tmp12FA;struct Cyc_Absyn_AccessEff_struct
_tmp12F9;struct Cyc_List_List*_tmp12F8;effect=((_tmp12F8=_cycalloc(sizeof(*
_tmp12F8)),((_tmp12F8->hd=(void*)((_tmp12FA=_cycalloc(sizeof(*_tmp12FA)),((
_tmp12FA[0]=((_tmp12F9.tag=23,((_tmp12F9.f1=(void*)_tmpB54,_tmp12F9)))),_tmp12FA)))),((
_tmp12F8->tl=effect,_tmp12F8))))));}goto _LL5AC;_LL5AF: _tmpB59=*_tmpB56;_tmpB5A=
_tmpB59.kind;if(_tmpB5A != Cyc_Absyn_EffKind)goto _LL5B1;_LL5B0:{struct Cyc_List_List*
_tmp12FB;effect=((_tmp12FB=_cycalloc(sizeof(*_tmp12FB)),((_tmp12FB->hd=_tmpB54,((
_tmp12FB->tl=effect,_tmp12FB))))));}goto _LL5AC;_LL5B1: _tmpB5B=*_tmpB56;_tmpB5C=
_tmpB5B.kind;if(_tmpB5C != Cyc_Absyn_IntKind)goto _LL5B3;_LL5B2: goto _LL5AC;_LL5B3:;
_LL5B4:{struct Cyc_Absyn_RgnsEff_struct*_tmp1301;struct Cyc_Absyn_RgnsEff_struct
_tmp1300;struct Cyc_List_List*_tmp12FF;effect=((_tmp12FF=_cycalloc(sizeof(*
_tmp12FF)),((_tmp12FF->hd=(void*)((_tmp1301=_cycalloc(sizeof(*_tmp1301)),((
_tmp1301[0]=((_tmp1300.tag=25,((_tmp1300.f1=(void*)_tmpB54,_tmp1300)))),_tmp1301)))),((
_tmp12FF->tl=effect,_tmp12FF))))));}goto _LL5AC;_LL5AC:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1307;struct Cyc_Absyn_JoinEff_struct _tmp1306;struct Cyc_Core_Opt*_tmp1305;*
_tmp9E8=((_tmp1305=_cycalloc(sizeof(*_tmp1305)),((_tmp1305->v=(void*)((_tmp1307=
_cycalloc(sizeof(*_tmp1307)),((_tmp1307[0]=((_tmp1306.tag=24,((_tmp1306.f1=
effect,_tmp1306)))),_tmp1307)))),_tmp1305))));};}if(*_tmp9E6 != 0){struct Cyc_List_List*
bs=*_tmp9E6;for(0;bs != 0;bs=bs->tl){void*_tmpB67=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpB69;struct Cyc_Core_Opt**
_tmpB6A;struct Cyc_Core_Opt*_tmpB6C;struct Cyc_Core_Opt**_tmpB6D;struct Cyc_Absyn_Kind*
_tmpB6E;struct Cyc_Absyn_Kind _tmpB6F;enum Cyc_Absyn_KindQual _tmpB70;enum Cyc_Absyn_AliasQual
_tmpB71;struct Cyc_Core_Opt*_tmpB73;struct Cyc_Core_Opt**_tmpB74;struct Cyc_Absyn_Kind*
_tmpB75;struct Cyc_Absyn_Kind _tmpB76;enum Cyc_Absyn_KindQual _tmpB77;enum Cyc_Absyn_AliasQual
_tmpB78;struct Cyc_Core_Opt*_tmpB7A;struct Cyc_Core_Opt**_tmpB7B;struct Cyc_Absyn_Kind*
_tmpB7C;struct Cyc_Absyn_Kind _tmpB7D;enum Cyc_Absyn_KindQual _tmpB7E;enum Cyc_Absyn_AliasQual
_tmpB7F;struct Cyc_Core_Opt*_tmpB81;struct Cyc_Core_Opt**_tmpB82;struct Cyc_Absyn_Kind*
_tmpB83;struct Cyc_Absyn_Kind _tmpB84;enum Cyc_Absyn_KindQual _tmpB85;enum Cyc_Absyn_AliasQual
_tmpB86;struct Cyc_Core_Opt*_tmpB88;struct Cyc_Core_Opt**_tmpB89;struct Cyc_Absyn_Kind*
_tmpB8A;struct Cyc_Absyn_Kind _tmpB8B;enum Cyc_Absyn_KindQual _tmpB8C;enum Cyc_Absyn_AliasQual
_tmpB8D;struct Cyc_Core_Opt*_tmpB8F;struct Cyc_Core_Opt**_tmpB90;struct Cyc_Absyn_Kind*
_tmpB91;struct Cyc_Absyn_Kind _tmpB92;enum Cyc_Absyn_KindQual _tmpB93;enum Cyc_Absyn_AliasQual
_tmpB94;struct Cyc_Core_Opt*_tmpB96;struct Cyc_Core_Opt**_tmpB97;struct Cyc_Absyn_Kind*
_tmpB98;struct Cyc_Absyn_Kind _tmpB99;enum Cyc_Absyn_KindQual _tmpB9A;enum Cyc_Absyn_AliasQual
_tmpB9B;struct Cyc_Core_Opt*_tmpB9D;struct Cyc_Core_Opt**_tmpB9E;struct Cyc_Absyn_Kind*
_tmpB9F;struct Cyc_Absyn_Kind _tmpBA0;enum Cyc_Absyn_KindQual _tmpBA1;enum Cyc_Absyn_AliasQual
_tmpBA2;struct Cyc_Core_Opt*_tmpBA4;struct Cyc_Core_Opt**_tmpBA5;struct Cyc_Absyn_Kind*
_tmpBA6;struct Cyc_Absyn_Kind*_tmpBA8;struct Cyc_Absyn_Kind _tmpBA9;enum Cyc_Absyn_KindQual
_tmpBAA;_LL5B6: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB68=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB67;if(_tmpB68->tag != 1)goto _LL5B8;else{_tmpB69=_tmpB68->f1;_tmpB6A=(struct
Cyc_Core_Opt**)& _tmpB68->f1;}}_LL5B7:{const char*_tmp130B;void*_tmp130A[1];struct
Cyc_String_pa_struct _tmp1309;(_tmp1309.tag=0,((_tmp1309.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp130A[0]=&
_tmp1309,Cyc_Tcutil_warn(loc,((_tmp130B="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp130B,sizeof(char),47))),_tag_dyneither(_tmp130A,sizeof(void*),
1)))))));}_tmpB6D=_tmpB6A;goto _LL5B9;_LL5B8: {struct Cyc_Absyn_Less_kb_struct*
_tmpB6B=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB6B->tag != 2)goto _LL5BA;
else{_tmpB6C=_tmpB6B->f1;_tmpB6D=(struct Cyc_Core_Opt**)& _tmpB6B->f1;_tmpB6E=
_tmpB6B->f2;_tmpB6F=*_tmpB6E;_tmpB70=_tmpB6F.kind;if(_tmpB70 != Cyc_Absyn_BoxKind)
goto _LL5BA;_tmpB71=_tmpB6F.aliasqual;if(_tmpB71 != Cyc_Absyn_Top)goto _LL5BA;}}
_LL5B9: _tmpB74=_tmpB6D;goto _LL5BB;_LL5BA: {struct Cyc_Absyn_Less_kb_struct*
_tmpB72=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB72->tag != 2)goto _LL5BC;
else{_tmpB73=_tmpB72->f1;_tmpB74=(struct Cyc_Core_Opt**)& _tmpB72->f1;_tmpB75=
_tmpB72->f2;_tmpB76=*_tmpB75;_tmpB77=_tmpB76.kind;if(_tmpB77 != Cyc_Absyn_MemKind)
goto _LL5BC;_tmpB78=_tmpB76.aliasqual;if(_tmpB78 != Cyc_Absyn_Top)goto _LL5BC;}}
_LL5BB: _tmpB7B=_tmpB74;goto _LL5BD;_LL5BC: {struct Cyc_Absyn_Less_kb_struct*
_tmpB79=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB79->tag != 2)goto _LL5BE;
else{_tmpB7A=_tmpB79->f1;_tmpB7B=(struct Cyc_Core_Opt**)& _tmpB79->f1;_tmpB7C=
_tmpB79->f2;_tmpB7D=*_tmpB7C;_tmpB7E=_tmpB7D.kind;if(_tmpB7E != Cyc_Absyn_MemKind)
goto _LL5BE;_tmpB7F=_tmpB7D.aliasqual;if(_tmpB7F != Cyc_Absyn_Aliasable)goto _LL5BE;}}
_LL5BD: _tmpB82=_tmpB7B;goto _LL5BF;_LL5BE: {struct Cyc_Absyn_Less_kb_struct*
_tmpB80=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB80->tag != 2)goto _LL5C0;
else{_tmpB81=_tmpB80->f1;_tmpB82=(struct Cyc_Core_Opt**)& _tmpB80->f1;_tmpB83=
_tmpB80->f2;_tmpB84=*_tmpB83;_tmpB85=_tmpB84.kind;if(_tmpB85 != Cyc_Absyn_AnyKind)
goto _LL5C0;_tmpB86=_tmpB84.aliasqual;if(_tmpB86 != Cyc_Absyn_Top)goto _LL5C0;}}
_LL5BF: _tmpB89=_tmpB82;goto _LL5C1;_LL5C0: {struct Cyc_Absyn_Less_kb_struct*
_tmpB87=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB87->tag != 2)goto _LL5C2;
else{_tmpB88=_tmpB87->f1;_tmpB89=(struct Cyc_Core_Opt**)& _tmpB87->f1;_tmpB8A=
_tmpB87->f2;_tmpB8B=*_tmpB8A;_tmpB8C=_tmpB8B.kind;if(_tmpB8C != Cyc_Absyn_AnyKind)
goto _LL5C2;_tmpB8D=_tmpB8B.aliasqual;if(_tmpB8D != Cyc_Absyn_Aliasable)goto _LL5C2;}}
_LL5C1:*_tmpB89=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL5B5;_LL5C2: {
struct Cyc_Absyn_Less_kb_struct*_tmpB8E=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;
if(_tmpB8E->tag != 2)goto _LL5C4;else{_tmpB8F=_tmpB8E->f1;_tmpB90=(struct Cyc_Core_Opt**)&
_tmpB8E->f1;_tmpB91=_tmpB8E->f2;_tmpB92=*_tmpB91;_tmpB93=_tmpB92.kind;if(_tmpB93
!= Cyc_Absyn_MemKind)goto _LL5C4;_tmpB94=_tmpB92.aliasqual;if(_tmpB94 != Cyc_Absyn_Unique)
goto _LL5C4;}}_LL5C3: _tmpB97=_tmpB90;goto _LL5C5;_LL5C4: {struct Cyc_Absyn_Less_kb_struct*
_tmpB95=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;if(_tmpB95->tag != 2)goto _LL5C6;
else{_tmpB96=_tmpB95->f1;_tmpB97=(struct Cyc_Core_Opt**)& _tmpB95->f1;_tmpB98=
_tmpB95->f2;_tmpB99=*_tmpB98;_tmpB9A=_tmpB99.kind;if(_tmpB9A != Cyc_Absyn_AnyKind)
goto _LL5C6;_tmpB9B=_tmpB99.aliasqual;if(_tmpB9B != Cyc_Absyn_Unique)goto _LL5C6;}}
_LL5C5:*_tmpB97=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL5B5;_LL5C6: {
struct Cyc_Absyn_Less_kb_struct*_tmpB9C=(struct Cyc_Absyn_Less_kb_struct*)_tmpB67;
if(_tmpB9C->tag != 2)goto _LL5C8;else{_tmpB9D=_tmpB9C->f1;_tmpB9E=(struct Cyc_Core_Opt**)&
_tmpB9C->f1;_tmpB9F=_tmpB9C->f2;_tmpBA0=*_tmpB9F;_tmpBA1=_tmpBA0.kind;if(_tmpBA1
!= Cyc_Absyn_RgnKind)goto _LL5C8;_tmpBA2=_tmpBA0.aliasqual;if(_tmpBA2 != Cyc_Absyn_Top)
goto _LL5C8;}}_LL5C7:*_tmpB9E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL5B5;_LL5C8: {struct Cyc_Absyn_Less_kb_struct*_tmpBA3=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB67;if(_tmpBA3->tag != 2)goto _LL5CA;else{_tmpBA4=_tmpBA3->f1;_tmpBA5=(struct
Cyc_Core_Opt**)& _tmpBA3->f1;_tmpBA6=_tmpBA3->f2;}}_LL5C9:*_tmpBA5=Cyc_Tcutil_kind_to_bound_opt(
_tmpBA6);goto _LL5B5;_LL5CA: {struct Cyc_Absyn_Eq_kb_struct*_tmpBA7=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB67;if(_tmpBA7->tag != 0)goto _LL5CC;else{_tmpBA8=_tmpBA7->f1;_tmpBA9=*_tmpBA8;
_tmpBAA=_tmpBA9.kind;if(_tmpBAA != Cyc_Absyn_MemKind)goto _LL5CC;}}_LL5CB:{const
char*_tmp130E;void*_tmp130D;(_tmp130D=0,Cyc_Tcutil_terr(loc,((_tmp130E="functions can't abstract M types",
_tag_dyneither(_tmp130E,sizeof(char),33))),_tag_dyneither(_tmp130D,sizeof(void*),
0)));}goto _LL5B5;_LL5CC:;_LL5CD: goto _LL5B5;_LL5B5:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpADB.kind_env,*_tmp9E6);_tmpADB.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpADB.r,_tmpADB.free_vars,*_tmp9E6);{struct Cyc_List_List*tvs=_tmpADB.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpBB1;int _tmpBB2;struct
_tuple25 _tmpBB0=*((struct _tuple25*)tvs->hd);_tmpBB1=_tmpBB0.f1;_tmpBB2=_tmpBB0.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpBB1,_tmpBB2);}}{struct Cyc_List_List*evs=_tmpADB.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpBB4;int _tmpBB5;struct _tuple26 _tmpBB3=*((struct _tuple26*)evs->hd);
_tmpBB4=_tmpBB3.f1;_tmpBB5=_tmpBB3.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpBB4,_tmpBB5);}};}goto _LL4E3;};}_LL502: {struct Cyc_Absyn_TupleType_struct*
_tmp9EF=(struct Cyc_Absyn_TupleType_struct*)_tmp9AF;if(_tmp9EF->tag != 11)goto
_LL504;else{_tmp9F0=_tmp9EF->f1;}}_LL503: for(0;_tmp9F0 != 0;_tmp9F0=_tmp9F0->tl){
struct _tuple11*_tmpBB7=(struct _tuple11*)_tmp9F0->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpBB7).f2,1);((*_tmpBB7).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpBB7).f1).print_const,(*_tmpBB7).f2);}goto _LL4E3;_LL504: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9F1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp9AF;if(_tmp9F1->tag != 13)goto
_LL506;else{_tmp9F2=_tmp9F1->f1;_tmp9F3=_tmp9F1->f2;}}_LL505: {struct
_RegionHandle*_tmpBB8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp9F3 != 0;_tmp9F3=_tmp9F3->tl){struct Cyc_Absyn_Aggrfield _tmpBBA;struct
_dyneither_ptr*_tmpBBB;struct Cyc_Absyn_Tqual _tmpBBC;struct Cyc_Absyn_Tqual*
_tmpBBD;void*_tmpBBE;struct Cyc_Absyn_Exp*_tmpBBF;struct Cyc_List_List*_tmpBC0;
struct Cyc_Absyn_Aggrfield*_tmpBB9=(struct Cyc_Absyn_Aggrfield*)_tmp9F3->hd;
_tmpBBA=*_tmpBB9;_tmpBBB=_tmpBBA.name;_tmpBBC=_tmpBBA.tq;_tmpBBD=(struct Cyc_Absyn_Tqual*)&(*
_tmpBB9).tq;_tmpBBE=_tmpBBA.type;_tmpBBF=_tmpBBA.width;_tmpBC0=_tmpBBA.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpBBB)){
const char*_tmp1312;void*_tmp1311[1];struct Cyc_String_pa_struct _tmp1310;(_tmp1310.tag=
0,((_tmp1310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBBB),((
_tmp1311[0]=& _tmp1310,Cyc_Tcutil_terr(loc,((_tmp1312="duplicate field %s",
_tag_dyneither(_tmp1312,sizeof(char),19))),_tag_dyneither(_tmp1311,sizeof(void*),
1)))))));}{const char*_tmp1313;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpBBB,((
_tmp1313="",_tag_dyneither(_tmp1313,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1314;prev_fields=((_tmp1314=_region_malloc(_tmpBB8,sizeof(*_tmp1314)),((
_tmp1314->hd=_tmpBBB,((_tmp1314->tl=prev_fields,_tmp1314))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpBBE,1);_tmpBBD->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpBBD->print_const,_tmpBBE);if(_tmp9F2 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpBBE)){const char*_tmp1318;void*_tmp1317[1];struct Cyc_String_pa_struct _tmp1316;(
_tmp1316.tag=0,((_tmp1316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpBBB),((_tmp1317[0]=& _tmp1316,Cyc_Tcutil_warn(loc,((_tmp1318="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1318,sizeof(char),74))),_tag_dyneither(_tmp1317,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpBBE,_tmpBBF,_tmpBBB);Cyc_Tcutil_check_field_atts(
loc,_tmpBBB,_tmpBC0);}}goto _LL4E3;}_LL506: {struct Cyc_Absyn_AggrType_struct*
_tmp9F4=(struct Cyc_Absyn_AggrType_struct*)_tmp9AF;if(_tmp9F4->tag != 12)goto
_LL508;else{_tmp9F5=_tmp9F4->f1;_tmp9F6=_tmp9F5.aggr_info;_tmp9F7=(union Cyc_Absyn_AggrInfoU*)&(
_tmp9F4->f1).aggr_info;_tmp9F8=_tmp9F5.targs;_tmp9F9=(struct Cyc_List_List**)&(
_tmp9F4->f1).targs;}}_LL507:{union Cyc_Absyn_AggrInfoU _tmpBC9=*_tmp9F7;struct
_tuple4 _tmpBCA;enum Cyc_Absyn_AggrKind _tmpBCB;struct _tuple2*_tmpBCC;struct Cyc_Core_Opt*
_tmpBCD;struct Cyc_Absyn_Aggrdecl**_tmpBCE;struct Cyc_Absyn_Aggrdecl*_tmpBCF;
_LL5CF: if((_tmpBC9.UnknownAggr).tag != 1)goto _LL5D1;_tmpBCA=(struct _tuple4)(
_tmpBC9.UnknownAggr).val;_tmpBCB=_tmpBCA.f1;_tmpBCC=_tmpBCA.f2;_tmpBCD=_tmpBCA.f3;
_LL5D0: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpBD0;_push_handler(&
_tmpBD0);{int _tmpBD2=0;if(setjmp(_tmpBD0.handler))_tmpBD2=1;if(!_tmpBD2){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpBCC);{struct Cyc_Absyn_Aggrdecl*_tmpBD3=*adp;if(_tmpBD3->kind != 
_tmpBCB){if(_tmpBD3->kind == Cyc_Absyn_StructA){const char*_tmp131D;void*_tmp131C[
2];struct Cyc_String_pa_struct _tmp131B;struct Cyc_String_pa_struct _tmp131A;(
_tmp131A.tag=0,((_tmp131A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCC)),((_tmp131B.tag=0,((_tmp131B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCC)),((_tmp131C[0]=& _tmp131B,((
_tmp131C[1]=& _tmp131A,Cyc_Tcutil_terr(loc,((_tmp131D="expecting struct %s instead of union %s",
_tag_dyneither(_tmp131D,sizeof(char),40))),_tag_dyneither(_tmp131C,sizeof(void*),
2)))))))))))));}else{const char*_tmp1322;void*_tmp1321[2];struct Cyc_String_pa_struct
_tmp1320;struct Cyc_String_pa_struct _tmp131F;(_tmp131F.tag=0,((_tmp131F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCC)),((
_tmp1320.tag=0,((_tmp1320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCC)),((_tmp1321[0]=& _tmp1320,((_tmp1321[1]=& _tmp131F,Cyc_Tcutil_terr(loc,((
_tmp1322="expecting union %s instead of struct %s",_tag_dyneither(_tmp1322,
sizeof(char),40))),_tag_dyneither(_tmp1321,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpBCD  && (int)_tmpBCD->v){if(!((unsigned int)_tmpBD3->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBD3->impl))->tagged){const char*
_tmp1326;void*_tmp1325[1];struct Cyc_String_pa_struct _tmp1324;(_tmp1324.tag=0,((
_tmp1324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCC)),((_tmp1325[0]=& _tmp1324,Cyc_Tcutil_terr(loc,((_tmp1326="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1326,sizeof(char),42))),_tag_dyneither(_tmp1325,sizeof(void*),
1)))))));}}*_tmp9F7=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpBD1=(void*)_exn_thrown;void*_tmpBE0=_tmpBD1;_LL5D4: {struct Cyc_Dict_Absent_struct*
_tmpBE1=(struct Cyc_Dict_Absent_struct*)_tmpBE0;if(_tmpBE1->tag != Cyc_Dict_Absent)
goto _LL5D6;}_LL5D5: {struct Cyc_Tcenv_Genv*_tmpBE2=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1327;struct Cyc_Absyn_Aggrdecl*_tmpBE3=(_tmp1327=
_cycalloc(sizeof(*_tmp1327)),((_tmp1327->kind=_tmpBCB,((_tmp1327->sc=Cyc_Absyn_Extern,((
_tmp1327->name=_tmpBCC,((_tmp1327->tvs=0,((_tmp1327->impl=0,((_tmp1327->attributes=
0,_tmp1327)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpBE2,loc,_tmpBE3);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpBCC);*_tmp9F7=Cyc_Absyn_KnownAggr(adp);if(*_tmp9F9 != 0){{const char*
_tmp132B;void*_tmp132A[1];struct Cyc_String_pa_struct _tmp1329;(_tmp1329.tag=0,((
_tmp1329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCC)),((_tmp132A[0]=& _tmp1329,Cyc_Tcutil_terr(loc,((_tmp132B="declare parameterized type %s before using",
_tag_dyneither(_tmp132B,sizeof(char),43))),_tag_dyneither(_tmp132A,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5D3;}_LL5D6:;_LL5D7:(void)_throw(_tmpBE0);_LL5D3:;}};}
_tmpBCF=*adp;goto _LL5D2;}_LL5D1: if((_tmpBC9.KnownAggr).tag != 2)goto _LL5CE;
_tmpBCE=(struct Cyc_Absyn_Aggrdecl**)(_tmpBC9.KnownAggr).val;_tmpBCF=*_tmpBCE;
_LL5D2: {struct Cyc_List_List*tvs=_tmpBCF->tvs;struct Cyc_List_List*ts=*_tmp9F9;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpBE8=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpBE9=(void*)ts->hd;{struct _tuple0 _tmp132C;
struct _tuple0 _tmpBEB=(_tmp132C.f1=Cyc_Absyn_compress_kb(_tmpBE8->kind),((
_tmp132C.f2=_tmpBE9,_tmp132C)));void*_tmpBEC;void*_tmpBEE;struct Cyc_Absyn_Tvar*
_tmpBF0;_LL5D9: _tmpBEC=_tmpBEB.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBED=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpBEC;if(_tmpBED->tag != 1)goto _LL5DB;};
_tmpBEE=_tmpBEB.f2;{struct Cyc_Absyn_VarType_struct*_tmpBEF=(struct Cyc_Absyn_VarType_struct*)
_tmpBEE;if(_tmpBEF->tag != 2)goto _LL5DB;else{_tmpBF0=_tmpBEF->f1;}};_LL5DA: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBF0);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBF0,1);continue;_LL5DB:;_LL5DC: goto _LL5D8;_LL5D8:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1330;void*_tmp132F[1];struct Cyc_String_pa_struct _tmp132E;(
_tmp132E.tag=0,((_tmp132E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCF->name)),((_tmp132F[0]=& _tmp132E,Cyc_Tcutil_terr(loc,((_tmp1330="too many parameters for type %s",
_tag_dyneither(_tmp1330,sizeof(char),32))),_tag_dyneither(_tmp132F,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1331;hidden_ts=((_tmp1331=_cycalloc(sizeof(*_tmp1331)),((_tmp1331->hd=e,((
_tmp1331->tl=hidden_ts,_tmp1331))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmp9F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9F9,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL5CE:;}goto _LL4E3;
_LL508: {struct Cyc_Absyn_TypedefType_struct*_tmp9FA=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9AF;if(_tmp9FA->tag != 18)goto _LL50A;else{_tmp9FB=_tmp9FA->f1;_tmp9FC=_tmp9FA->f2;
_tmp9FD=(struct Cyc_List_List**)& _tmp9FA->f2;_tmp9FE=_tmp9FA->f3;_tmp9FF=(struct
Cyc_Absyn_Typedefdecl**)& _tmp9FA->f3;_tmpA00=_tmp9FA->f4;_tmpA01=(void***)&
_tmp9FA->f4;}}_LL509: {struct Cyc_List_List*targs=*_tmp9FD;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpBF5;_push_handler(& _tmpBF5);{int _tmpBF7=0;if(setjmp(
_tmpBF5.handler))_tmpBF7=1;if(!_tmpBF7){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp9FB);;_pop_handler();}else{void*_tmpBF6=(void*)_exn_thrown;void*_tmpBF9=
_tmpBF6;_LL5DE: {struct Cyc_Dict_Absent_struct*_tmpBFA=(struct Cyc_Dict_Absent_struct*)
_tmpBF9;if(_tmpBFA->tag != Cyc_Dict_Absent)goto _LL5E0;}_LL5DF:{const char*_tmp1335;
void*_tmp1334[1];struct Cyc_String_pa_struct _tmp1333;(_tmp1333.tag=0,((_tmp1333.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9FB)),((
_tmp1334[0]=& _tmp1333,Cyc_Tcutil_terr(loc,((_tmp1335="unbound typedef name %s",
_tag_dyneither(_tmp1335,sizeof(char),24))),_tag_dyneither(_tmp1334,sizeof(void*),
1)))))));}return cvtenv;_LL5E0:;_LL5E1:(void)_throw(_tmpBF9);_LL5DD:;}};}*_tmp9FF=(
struct Cyc_Absyn_Typedefdecl*)td;_tmp9FB[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpBFE=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple14*_tmp1338;struct Cyc_List_List*
_tmp1337;inst=((_tmp1337=_region_malloc(_tmpBFE,sizeof(*_tmp1337)),((_tmp1337->hd=((
_tmp1338=_region_malloc(_tmpBFE,sizeof(*_tmp1338)),((_tmp1338->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1338->f2=(void*)ts->hd,_tmp1338)))))),((_tmp1337->tl=inst,_tmp1337))))));};}
if(ts != 0){const char*_tmp133C;void*_tmp133B[1];struct Cyc_String_pa_struct
_tmp133A;(_tmp133A.tag=0,((_tmp133A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9FB)),((_tmp133B[0]=& _tmp133A,Cyc_Tcutil_terr(
loc,((_tmp133C="too many parameters for typedef %s",_tag_dyneither(_tmp133C,
sizeof(char),35))),_tag_dyneither(_tmp133B,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp133D;hidden_ts=((_tmp133D=_cycalloc(sizeof(*_tmp133D)),((
_tmp133D->hd=e,((_tmp133D->tl=hidden_ts,_tmp133D))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple14*_tmp1340;struct Cyc_List_List*_tmp133F;inst=(
struct Cyc_List_List*)((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F->hd=(
struct _tuple14*)((_tmp1340=_cycalloc(sizeof(*_tmp1340)),((_tmp1340->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1340->f2=e,_tmp1340)))))),((_tmp133F->tl=inst,
_tmp133F))))));};}*_tmp9FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpBFE,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1341;*_tmpA01=((_tmp1341=_cycalloc(sizeof(*
_tmp1341)),((_tmp1341[0]=new_typ,_tmp1341))));}}goto _LL4E3;};}_LL50A: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpA02=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp9AF;if(_tmpA02->tag != 22)goto
_LL50C;}_LL50B: goto _LL50D;_LL50C: {struct Cyc_Absyn_HeapRgn_struct*_tmpA03=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp9AF;if(_tmpA03->tag != 21)goto _LL50E;}_LL50D:
goto _LL4E3;_LL50E: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA04=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9AF;if(_tmpA04->tag != 16)goto _LL510;else{_tmpA05=(void*)_tmpA04->f1;}}_LL50F:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA05,1);
goto _LL4E3;_LL510: {struct Cyc_Absyn_DynRgnType_struct*_tmpA06=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp9AF;if(_tmpA06->tag != 17)goto _LL512;else{_tmpA07=(void*)_tmpA06->f1;_tmpA08=(
void*)_tmpA06->f2;}}_LL511: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpA07,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpA08,1);goto _LL4E3;_LL512: {struct Cyc_Absyn_AccessEff_struct*_tmpA09=(struct
Cyc_Absyn_AccessEff_struct*)_tmp9AF;if(_tmpA09->tag != 23)goto _LL514;else{_tmpA0A=(
void*)_tmpA09->f1;}}_LL513: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpA0A,1);goto _LL4E3;_LL514: {struct Cyc_Absyn_RgnsEff_struct*
_tmpA0B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp9AF;if(_tmpA0B->tag != 25)goto _LL516;
else{_tmpA0C=(void*)_tmpA0B->f1;}}_LL515: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpA0C,1);goto _LL4E3;_LL516: {struct Cyc_Absyn_JoinEff_struct*
_tmpA0D=(struct Cyc_Absyn_JoinEff_struct*)_tmp9AF;if(_tmpA0D->tag != 24)goto _LL4E3;
else{_tmpA0E=_tmpA0D->f1;}}_LL517: for(0;_tmpA0E != 0;_tmpA0E=_tmpA0E->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpA0E->hd,1);}
goto _LL4E3;_LL4E3:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpC08=t;struct Cyc_Core_Opt*_tmpC0A;struct Cyc_Core_Opt*_tmpC0B;_LL5E3: {
struct Cyc_Absyn_Evar_struct*_tmpC09=(struct Cyc_Absyn_Evar_struct*)_tmpC08;if(
_tmpC09->tag != 1)goto _LL5E5;else{_tmpC0A=_tmpC09->f1;_tmpC0B=_tmpC09->f2;}}
_LL5E4: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpC0C=_tmpC0A;struct Cyc_Core_Opt
_tmpC0D;struct Cyc_Absyn_Kind*_tmpC0E;_LL5E8: if(_tmpC0C != 0)goto _LL5EA;_LL5E9:{
const char*_tmp1342;s=((_tmp1342="kind=NULL ",_tag_dyneither(_tmp1342,sizeof(char),
11)));}goto _LL5E7;_LL5EA: if(_tmpC0C == 0)goto _LL5E7;_tmpC0D=*_tmpC0C;_tmpC0E=(
struct Cyc_Absyn_Kind*)_tmpC0D.v;_LL5EB:{const char*_tmp1346;void*_tmp1345[1];
struct Cyc_String_pa_struct _tmp1344;s=(struct _dyneither_ptr)((_tmp1344.tag=0,((
_tmp1344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpC0E)),((_tmp1345[0]=& _tmp1344,Cyc_aprintf(((_tmp1346="kind=%s ",
_tag_dyneither(_tmp1346,sizeof(char),9))),_tag_dyneither(_tmp1345,sizeof(void*),
1))))))));}goto _LL5E7;_LL5E7:;}{struct Cyc_Core_Opt*_tmpC13=_tmpC0B;struct Cyc_Core_Opt
_tmpC14;void*_tmpC15;_LL5ED: if(_tmpC13 != 0)goto _LL5EF;_LL5EE:{const char*_tmp134A;
void*_tmp1349[1];struct Cyc_String_pa_struct _tmp1348;s=(struct _dyneither_ptr)((
_tmp1348.tag=0,((_tmp1348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp1349[0]=& _tmp1348,Cyc_aprintf(((_tmp134A="%s ref=NULL",_tag_dyneither(
_tmp134A,sizeof(char),12))),_tag_dyneither(_tmp1349,sizeof(void*),1))))))));}
goto _LL5EC;_LL5EF: if(_tmpC13 == 0)goto _LL5EC;_tmpC14=*_tmpC13;_tmpC15=(void*)
_tmpC14.v;_LL5F0:{const char*_tmp134F;void*_tmp134E[2];struct Cyc_String_pa_struct
_tmp134D;struct Cyc_String_pa_struct _tmp134C;s=(struct _dyneither_ptr)((_tmp134C.tag=
0,((_tmp134C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpC15)),((_tmp134D.tag=0,((_tmp134D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp134E[0]=& _tmp134D,((_tmp134E[1]=& _tmp134C,Cyc_aprintf(((
_tmp134F="%s ref=%s",_tag_dyneither(_tmp134F,sizeof(char),10))),_tag_dyneither(
_tmp134E,sizeof(void*),2))))))))))))));}goto _LL5EC;_LL5EC:;}{const char*_tmp1353;
void*_tmp1352[1];struct Cyc_String_pa_struct _tmp1351;(_tmp1351.tag=0,((_tmp1351.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1352[0]=& _tmp1351,Cyc_fprintf(
Cyc_stderr,((_tmp1353="evar info: %s\n",_tag_dyneither(_tmp1353,sizeof(char),15))),
_tag_dyneither(_tmp1352,sizeof(void*),1)))))));}goto _LL5E2;}_LL5E5:;_LL5E6: goto
_LL5E2;_LL5E2:;}{const char*_tmp1359;void*_tmp1358[3];struct Cyc_String_pa_struct
_tmp1357;struct Cyc_String_pa_struct _tmp1356;struct Cyc_String_pa_struct _tmp1355;(
_tmp1355.tag=0,((_tmp1355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1356.tag=0,((_tmp1356.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1357.tag=0,((
_tmp1357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1358[0]=& _tmp1357,((_tmp1358[1]=& _tmp1356,((_tmp1358[2]=& _tmp1355,Cyc_Tcutil_terr(
loc,((_tmp1359="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1359,sizeof(char),51))),_tag_dyneither(_tmp1358,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpC25=e->r;
struct Cyc_List_List*_tmpC2B;struct Cyc_Absyn_Exp*_tmpC2D;struct Cyc_Absyn_Exp*
_tmpC2E;struct Cyc_Absyn_Exp*_tmpC2F;struct Cyc_Absyn_Exp*_tmpC31;struct Cyc_Absyn_Exp*
_tmpC32;struct Cyc_Absyn_Exp*_tmpC34;struct Cyc_Absyn_Exp*_tmpC35;struct Cyc_Absyn_Exp*
_tmpC37;struct Cyc_Absyn_Exp*_tmpC38;void*_tmpC3A;struct Cyc_Absyn_Exp*_tmpC3B;
void*_tmpC3D;void*_tmpC3F;void*_tmpC41;struct Cyc_Absyn_Exp*_tmpC43;_LL5F2: {
struct Cyc_Absyn_Const_e_struct*_tmpC26=(struct Cyc_Absyn_Const_e_struct*)_tmpC25;
if(_tmpC26->tag != 0)goto _LL5F4;}_LL5F3: goto _LL5F5;_LL5F4: {struct Cyc_Absyn_Enum_e_struct*
_tmpC27=(struct Cyc_Absyn_Enum_e_struct*)_tmpC25;if(_tmpC27->tag != 32)goto _LL5F6;}
_LL5F5: goto _LL5F7;_LL5F6: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpC28=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpC25;if(_tmpC28->tag != 33)goto _LL5F8;}_LL5F7: goto _LL5F9;_LL5F8: {struct Cyc_Absyn_Var_e_struct*
_tmpC29=(struct Cyc_Absyn_Var_e_struct*)_tmpC25;if(_tmpC29->tag != 1)goto _LL5FA;}
_LL5F9: goto _LL5F1;_LL5FA: {struct Cyc_Absyn_Primop_e_struct*_tmpC2A=(struct Cyc_Absyn_Primop_e_struct*)
_tmpC25;if(_tmpC2A->tag != 3)goto _LL5FC;else{_tmpC2B=_tmpC2A->f2;}}_LL5FB: for(0;
_tmpC2B != 0;_tmpC2B=_tmpC2B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpC2B->hd,te,cvtenv);}goto _LL5F1;_LL5FC: {struct Cyc_Absyn_Conditional_e_struct*
_tmpC2C=(struct Cyc_Absyn_Conditional_e_struct*)_tmpC25;if(_tmpC2C->tag != 6)goto
_LL5FE;else{_tmpC2D=_tmpC2C->f1;_tmpC2E=_tmpC2C->f2;_tmpC2F=_tmpC2C->f3;}}_LL5FD:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC2D,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpC2E,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC2F,te,
cvtenv);goto _LL5F1;_LL5FE: {struct Cyc_Absyn_And_e_struct*_tmpC30=(struct Cyc_Absyn_And_e_struct*)
_tmpC25;if(_tmpC30->tag != 7)goto _LL600;else{_tmpC31=_tmpC30->f1;_tmpC32=_tmpC30->f2;}}
_LL5FF: _tmpC34=_tmpC31;_tmpC35=_tmpC32;goto _LL601;_LL600: {struct Cyc_Absyn_Or_e_struct*
_tmpC33=(struct Cyc_Absyn_Or_e_struct*)_tmpC25;if(_tmpC33->tag != 8)goto _LL602;
else{_tmpC34=_tmpC33->f1;_tmpC35=_tmpC33->f2;}}_LL601: _tmpC37=_tmpC34;_tmpC38=
_tmpC35;goto _LL603;_LL602: {struct Cyc_Absyn_SeqExp_e_struct*_tmpC36=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpC25;if(_tmpC36->tag != 9)goto _LL604;else{_tmpC37=_tmpC36->f1;_tmpC38=_tmpC36->f2;}}
_LL603: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC37,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC38,te,cvtenv);goto _LL5F1;_LL604: {
struct Cyc_Absyn_Cast_e_struct*_tmpC39=(struct Cyc_Absyn_Cast_e_struct*)_tmpC25;
if(_tmpC39->tag != 15)goto _LL606;else{_tmpC3A=(void*)_tmpC39->f1;_tmpC3B=_tmpC39->f2;}}
_LL605: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC3B,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC3A,1);goto
_LL5F1;_LL606: {struct Cyc_Absyn_Offsetof_e_struct*_tmpC3C=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpC25;if(_tmpC3C->tag != 20)goto _LL608;else{_tmpC3D=(void*)_tmpC3C->f1;}}_LL607:
_tmpC3F=_tmpC3D;goto _LL609;_LL608: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC3E=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpC25;if(_tmpC3E->tag != 18)goto _LL60A;
else{_tmpC3F=(void*)_tmpC3E->f1;}}_LL609: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpC3F,1);goto _LL5F1;_LL60A: {struct Cyc_Absyn_Valueof_e_struct*
_tmpC40=(struct Cyc_Absyn_Valueof_e_struct*)_tmpC25;if(_tmpC40->tag != 39)goto
_LL60C;else{_tmpC41=(void*)_tmpC40->f1;}}_LL60B: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC41,1);goto _LL5F1;_LL60C: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpC42=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpC25;if(_tmpC42->tag != 19)goto
_LL60E;else{_tmpC43=_tmpC42->f1;}}_LL60D: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpC43,te,cvtenv);goto _LL5F1;_LL60E:;_LL60F: {const char*_tmp135C;void*_tmp135B;(
_tmp135B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp135C="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp135C,sizeof(char),66))),_tag_dyneither(_tmp135B,sizeof(void*),
0)));}_LL5F1:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpC46=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpC48;struct
_tuple25 _tmpC47=*((struct _tuple25*)vs->hd);_tmpC48=_tmpC47.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpC46,_tmpC48);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpC4A;struct _tuple26 _tmpC49=*((struct _tuple26*)evs->hd);_tmpC4A=_tmpC49.f1;{
void*_tmpC4B=Cyc_Tcutil_compress(_tmpC4A);struct Cyc_Core_Opt*_tmpC4D;struct Cyc_Core_Opt**
_tmpC4E;_LL611: {struct Cyc_Absyn_Evar_struct*_tmpC4C=(struct Cyc_Absyn_Evar_struct*)
_tmpC4B;if(_tmpC4C->tag != 1)goto _LL613;else{_tmpC4D=_tmpC4C->f4;_tmpC4E=(struct
Cyc_Core_Opt**)& _tmpC4C->f4;}}_LL612: if(*_tmpC4E == 0){struct Cyc_Core_Opt*
_tmp135D;*_tmpC4E=((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((_tmp135D->v=_tmpC46,
_tmp135D))));}else{struct Cyc_List_List*_tmpC50=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpC4E))->v;struct Cyc_List_List*_tmpC51=0;for(0;_tmpC50 != 0;
_tmpC50=_tmpC50->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpC46,(struct Cyc_Absyn_Tvar*)_tmpC50->hd)){struct Cyc_List_List*_tmp135E;
_tmpC51=((_tmp135E=_cycalloc(sizeof(*_tmp135E)),((_tmp135E->hd=(struct Cyc_Absyn_Tvar*)
_tmpC50->hd,((_tmp135E->tl=_tmpC51,_tmp135E))))));}}{struct Cyc_Core_Opt*_tmp135F;*
_tmpC4E=((_tmp135F=_cycalloc(sizeof(*_tmp135F)),((_tmp135F->v=_tmpC51,_tmp135F))));};}
goto _LL610;_LL613:;_LL614: goto _LL610;_LL610:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpC54=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpC55=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp1360;struct Cyc_Tcutil_CVTEnv _tmpC56=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp1360.r=_tmpC55,((_tmp1360.kind_env=_tmpC54,((_tmp1360.free_vars=0,((_tmp1360.free_evars=
0,((_tmp1360.generalize_evars=generalize_evars,((_tmp1360.fn_result=0,_tmp1360)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpC57=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC55,(struct Cyc_Absyn_Tvar*(*)(struct _tuple25*))Cyc_Core_fst,
_tmpC56.free_vars);struct Cyc_List_List*_tmpC58=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpC55,(void*(*)(struct _tuple26*))Cyc_Core_fst,_tmpC56.free_evars);if(_tmpC54 != 
0){struct Cyc_List_List*_tmpC59=0;{struct Cyc_List_List*_tmpC5A=_tmpC57;for(0;(
unsigned int)_tmpC5A;_tmpC5A=_tmpC5A->tl){struct Cyc_Absyn_Tvar*_tmpC5B=(struct
Cyc_Absyn_Tvar*)_tmpC5A->hd;int found=0;{struct Cyc_List_List*_tmpC5C=_tmpC54;for(
0;(unsigned int)_tmpC5C;_tmpC5C=_tmpC5C->tl){if(Cyc_Absyn_tvar_cmp(_tmpC5B,(
struct Cyc_Absyn_Tvar*)_tmpC5C->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp1361;_tmpC59=((_tmp1361=_region_malloc(_tmpC55,sizeof(*_tmp1361)),((_tmp1361->hd=(
struct Cyc_Absyn_Tvar*)_tmpC5A->hd,((_tmp1361->tl=_tmpC59,_tmp1361))))));}}}
_tmpC57=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpC59);}{struct Cyc_List_List*x=_tmpC57;for(0;x != 0;x=x->tl){void*_tmpC5E=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpC60;struct Cyc_Core_Opt**
_tmpC61;struct Cyc_Core_Opt*_tmpC63;struct Cyc_Core_Opt**_tmpC64;struct Cyc_Absyn_Kind*
_tmpC65;struct Cyc_Absyn_Kind _tmpC66;enum Cyc_Absyn_KindQual _tmpC67;enum Cyc_Absyn_AliasQual
_tmpC68;struct Cyc_Core_Opt*_tmpC6A;struct Cyc_Core_Opt**_tmpC6B;struct Cyc_Absyn_Kind*
_tmpC6C;struct Cyc_Absyn_Kind _tmpC6D;enum Cyc_Absyn_KindQual _tmpC6E;enum Cyc_Absyn_AliasQual
_tmpC6F;struct Cyc_Core_Opt*_tmpC71;struct Cyc_Core_Opt**_tmpC72;struct Cyc_Absyn_Kind*
_tmpC73;struct Cyc_Absyn_Kind _tmpC74;enum Cyc_Absyn_KindQual _tmpC75;enum Cyc_Absyn_AliasQual
_tmpC76;struct Cyc_Core_Opt*_tmpC78;struct Cyc_Core_Opt**_tmpC79;struct Cyc_Absyn_Kind*
_tmpC7A;struct Cyc_Absyn_Kind _tmpC7B;enum Cyc_Absyn_KindQual _tmpC7C;enum Cyc_Absyn_AliasQual
_tmpC7D;struct Cyc_Core_Opt*_tmpC7F;struct Cyc_Core_Opt**_tmpC80;struct Cyc_Absyn_Kind*
_tmpC81;struct Cyc_Absyn_Kind _tmpC82;enum Cyc_Absyn_KindQual _tmpC83;enum Cyc_Absyn_AliasQual
_tmpC84;struct Cyc_Core_Opt*_tmpC86;struct Cyc_Core_Opt**_tmpC87;struct Cyc_Absyn_Kind*
_tmpC88;struct Cyc_Absyn_Kind*_tmpC8A;struct Cyc_Absyn_Kind _tmpC8B;enum Cyc_Absyn_KindQual
_tmpC8C;enum Cyc_Absyn_AliasQual _tmpC8D;_LL616: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC5F=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC5E;if(_tmpC5F->tag != 1)goto
_LL618;else{_tmpC60=_tmpC5F->f1;_tmpC61=(struct Cyc_Core_Opt**)& _tmpC5F->f1;}}
_LL617: _tmpC64=_tmpC61;goto _LL619;_LL618: {struct Cyc_Absyn_Less_kb_struct*
_tmpC62=(struct Cyc_Absyn_Less_kb_struct*)_tmpC5E;if(_tmpC62->tag != 2)goto _LL61A;
else{_tmpC63=_tmpC62->f1;_tmpC64=(struct Cyc_Core_Opt**)& _tmpC62->f1;_tmpC65=
_tmpC62->f2;_tmpC66=*_tmpC65;_tmpC67=_tmpC66.kind;if(_tmpC67 != Cyc_Absyn_BoxKind)
goto _LL61A;_tmpC68=_tmpC66.aliasqual;if(_tmpC68 != Cyc_Absyn_Top)goto _LL61A;}}
_LL619: _tmpC6B=_tmpC64;goto _LL61B;_LL61A: {struct Cyc_Absyn_Less_kb_struct*
_tmpC69=(struct Cyc_Absyn_Less_kb_struct*)_tmpC5E;if(_tmpC69->tag != 2)goto _LL61C;
else{_tmpC6A=_tmpC69->f1;_tmpC6B=(struct Cyc_Core_Opt**)& _tmpC69->f1;_tmpC6C=
_tmpC69->f2;_tmpC6D=*_tmpC6C;_tmpC6E=_tmpC6D.kind;if(_tmpC6E != Cyc_Absyn_MemKind)
goto _LL61C;_tmpC6F=_tmpC6D.aliasqual;if(_tmpC6F != Cyc_Absyn_Top)goto _LL61C;}}
_LL61B: _tmpC72=_tmpC6B;goto _LL61D;_LL61C: {struct Cyc_Absyn_Less_kb_struct*
_tmpC70=(struct Cyc_Absyn_Less_kb_struct*)_tmpC5E;if(_tmpC70->tag != 2)goto _LL61E;
else{_tmpC71=_tmpC70->f1;_tmpC72=(struct Cyc_Core_Opt**)& _tmpC70->f1;_tmpC73=
_tmpC70->f2;_tmpC74=*_tmpC73;_tmpC75=_tmpC74.kind;if(_tmpC75 != Cyc_Absyn_MemKind)
goto _LL61E;_tmpC76=_tmpC74.aliasqual;if(_tmpC76 != Cyc_Absyn_Aliasable)goto _LL61E;}}
_LL61D:*_tmpC72=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL615;_LL61E: {
struct Cyc_Absyn_Less_kb_struct*_tmpC77=(struct Cyc_Absyn_Less_kb_struct*)_tmpC5E;
if(_tmpC77->tag != 2)goto _LL620;else{_tmpC78=_tmpC77->f1;_tmpC79=(struct Cyc_Core_Opt**)&
_tmpC77->f1;_tmpC7A=_tmpC77->f2;_tmpC7B=*_tmpC7A;_tmpC7C=_tmpC7B.kind;if(_tmpC7C
!= Cyc_Absyn_MemKind)goto _LL620;_tmpC7D=_tmpC7B.aliasqual;if(_tmpC7D != Cyc_Absyn_Unique)
goto _LL620;}}_LL61F:*_tmpC79=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL615;_LL620: {struct Cyc_Absyn_Less_kb_struct*_tmpC7E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC5E;if(_tmpC7E->tag != 2)goto _LL622;else{_tmpC7F=_tmpC7E->f1;_tmpC80=(struct
Cyc_Core_Opt**)& _tmpC7E->f1;_tmpC81=_tmpC7E->f2;_tmpC82=*_tmpC81;_tmpC83=_tmpC82.kind;
if(_tmpC83 != Cyc_Absyn_RgnKind)goto _LL622;_tmpC84=_tmpC82.aliasqual;if(_tmpC84 != 
Cyc_Absyn_Top)goto _LL622;}}_LL621:*_tmpC80=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL615;_LL622: {struct Cyc_Absyn_Less_kb_struct*_tmpC85=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC5E;if(_tmpC85->tag != 2)goto _LL624;else{_tmpC86=_tmpC85->f1;_tmpC87=(struct
Cyc_Core_Opt**)& _tmpC85->f1;_tmpC88=_tmpC85->f2;}}_LL623:*_tmpC87=Cyc_Tcutil_kind_to_bound_opt(
_tmpC88);goto _LL615;_LL624: {struct Cyc_Absyn_Eq_kb_struct*_tmpC89=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC5E;if(_tmpC89->tag != 0)goto _LL626;else{_tmpC8A=_tmpC89->f1;_tmpC8B=*_tmpC8A;
_tmpC8C=_tmpC8B.kind;if(_tmpC8C != Cyc_Absyn_MemKind)goto _LL626;_tmpC8D=_tmpC8B.aliasqual;}}
_LL625:{const char*_tmp1368;void*_tmp1367[2];struct Cyc_String_pa_struct _tmp1366;
struct Cyc_Absyn_Kind*_tmp1365;struct Cyc_String_pa_struct _tmp1364;(_tmp1364.tag=0,((
_tmp1364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1365=_cycalloc_atomic(sizeof(*_tmp1365)),((_tmp1365->kind=Cyc_Absyn_MemKind,((
_tmp1365->aliasqual=_tmpC8D,_tmp1365)))))))),((_tmp1366.tag=0,((_tmp1366.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1367[0]=& _tmp1366,((_tmp1367[1]=& _tmp1364,Cyc_Tcutil_terr(loc,((
_tmp1368="type variable %s cannot have kind %s",_tag_dyneither(_tmp1368,sizeof(
char),37))),_tag_dyneither(_tmp1367,sizeof(void*),2)))))))))))));}goto _LL615;
_LL626:;_LL627: goto _LL615;_LL615:;}}if(_tmpC57 != 0  || _tmpC58 != 0){{void*_tmpC93=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpC95;struct Cyc_List_List*_tmpC96;
struct Cyc_List_List**_tmpC97;struct Cyc_Core_Opt*_tmpC98;void*_tmpC99;struct Cyc_List_List*
_tmpC9A;int _tmpC9B;struct Cyc_Absyn_VarargInfo*_tmpC9C;struct Cyc_List_List*
_tmpC9D;struct Cyc_List_List*_tmpC9E;_LL629: {struct Cyc_Absyn_FnType_struct*
_tmpC94=(struct Cyc_Absyn_FnType_struct*)_tmpC93;if(_tmpC94->tag != 10)goto _LL62B;
else{_tmpC95=_tmpC94->f1;_tmpC96=_tmpC95.tvars;_tmpC97=(struct Cyc_List_List**)&(
_tmpC94->f1).tvars;_tmpC98=_tmpC95.effect;_tmpC99=_tmpC95.ret_typ;_tmpC9A=
_tmpC95.args;_tmpC9B=_tmpC95.c_varargs;_tmpC9C=_tmpC95.cyc_varargs;_tmpC9D=
_tmpC95.rgn_po;_tmpC9E=_tmpC95.attributes;}}_LL62A: if(*_tmpC97 == 0){*_tmpC97=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpC57);_tmpC57=0;}
goto _LL628;_LL62B:;_LL62C: goto _LL628;_LL628:;}if(_tmpC57 != 0){const char*_tmp136C;
void*_tmp136B[1];struct Cyc_String_pa_struct _tmp136A;(_tmp136A.tag=0,((_tmp136A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpC57->hd)->name),((
_tmp136B[0]=& _tmp136A,Cyc_Tcutil_terr(loc,((_tmp136C="unbound type variable %s",
_tag_dyneither(_tmp136C,sizeof(char),25))),_tag_dyneither(_tmp136B,sizeof(void*),
1)))))));}if(_tmpC58 != 0)for(0;_tmpC58 != 0;_tmpC58=_tmpC58->tl){void*e=(void*)
_tmpC58->hd;struct Cyc_Absyn_Kind*_tmpCA2=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpCA3;enum Cyc_Absyn_KindQual _tmpCA4;enum Cyc_Absyn_AliasQual _tmpCA5;struct Cyc_Absyn_Kind
_tmpCA6;enum Cyc_Absyn_KindQual _tmpCA7;enum Cyc_Absyn_AliasQual _tmpCA8;struct Cyc_Absyn_Kind
_tmpCA9;enum Cyc_Absyn_KindQual _tmpCAA;enum Cyc_Absyn_AliasQual _tmpCAB;struct Cyc_Absyn_Kind
_tmpCAC;enum Cyc_Absyn_KindQual _tmpCAD;_LL62E: _tmpCA3=*_tmpCA2;_tmpCA4=_tmpCA3.kind;
if(_tmpCA4 != Cyc_Absyn_RgnKind)goto _LL630;_tmpCA5=_tmpCA3.aliasqual;if(_tmpCA5 != 
Cyc_Absyn_Unique)goto _LL630;_LL62F: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp136F;void*_tmp136E;(_tmp136E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136F="can't unify evar with unique region!",
_tag_dyneither(_tmp136F,sizeof(char),37))),_tag_dyneither(_tmp136E,sizeof(void*),
0)));}goto _LL62D;_LL630: _tmpCA6=*_tmpCA2;_tmpCA7=_tmpCA6.kind;if(_tmpCA7 != Cyc_Absyn_RgnKind)
goto _LL632;_tmpCA8=_tmpCA6.aliasqual;if(_tmpCA8 != Cyc_Absyn_Aliasable)goto _LL632;
_LL631: goto _LL633;_LL632: _tmpCA9=*_tmpCA2;_tmpCAA=_tmpCA9.kind;if(_tmpCAA != Cyc_Absyn_RgnKind)
goto _LL634;_tmpCAB=_tmpCA9.aliasqual;if(_tmpCAB != Cyc_Absyn_Top)goto _LL634;
_LL633: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1372;
void*_tmp1371;(_tmp1371=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1372="can't unify evar with heap!",_tag_dyneither(
_tmp1372,sizeof(char),28))),_tag_dyneither(_tmp1371,sizeof(void*),0)));}goto
_LL62D;_LL634: _tmpCAC=*_tmpCA2;_tmpCAD=_tmpCAC.kind;if(_tmpCAD != Cyc_Absyn_EffKind)
goto _LL636;_LL635: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1375;void*_tmp1374;(_tmp1374=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1375="can't unify evar with {}!",
_tag_dyneither(_tmp1375,sizeof(char),26))),_tag_dyneither(_tmp1374,sizeof(void*),
0)));}goto _LL62D;_LL636:;_LL637:{const char*_tmp137A;void*_tmp1379[2];struct Cyc_String_pa_struct
_tmp1378;struct Cyc_String_pa_struct _tmp1377;(_tmp1377.tag=0,((_tmp1377.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1378.tag=0,((_tmp1378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1379[0]=& _tmp1378,((_tmp1379[1]=& _tmp1377,Cyc_Tcutil_terr(loc,((
_tmp137A="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp137A,sizeof(char),52))),_tag_dyneither(_tmp1379,sizeof(void*),2)))))))))))));}
goto _LL62D;_LL62D:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpCB9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpCBB;struct Cyc_List_List*
_tmpCBC;struct Cyc_Core_Opt*_tmpCBD;void*_tmpCBE;_LL639: {struct Cyc_Absyn_FnType_struct*
_tmpCBA=(struct Cyc_Absyn_FnType_struct*)_tmpCB9;if(_tmpCBA->tag != 10)goto _LL63B;
else{_tmpCBB=_tmpCBA->f1;_tmpCBC=_tmpCBB.tvars;_tmpCBD=_tmpCBB.effect;_tmpCBE=
_tmpCBB.ret_typ;}}_LL63A: fd->tvs=_tmpCBC;fd->effect=_tmpCBD;goto _LL638;_LL63B:;
_LL63C: {const char*_tmp137D;void*_tmp137C;(_tmp137C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp137D="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp137D,sizeof(char),38))),_tag_dyneither(_tmp137C,sizeof(void*),
0)));}_LL638:;}{struct _RegionHandle*_tmpCC1=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp137E;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple21*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpCC1,(struct _dyneither_ptr*(*)(struct _tuple21*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp137E="function declaration has repeated parameter",
_tag_dyneither(_tmp137E,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp137F;fd->cached_typ=((
_tmp137F=_cycalloc(sizeof(*_tmp137F)),((_tmp137F->v=t,_tmp137F))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpCC4=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp1380;
struct Cyc_Tcutil_CVTEnv _tmpCC5=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1380.r=
_tmpCC4,((_tmp1380.kind_env=bound_tvars,((_tmp1380.free_vars=0,((_tmp1380.free_evars=
0,((_tmp1380.generalize_evars=0,((_tmp1380.fn_result=0,_tmp1380)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpCC6=Cyc_Tcutil_remove_bound_tvars(
_tmpCC4,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpCC4,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple25*))Cyc_Core_fst,_tmpCC5.free_vars),bound_tvars);struct Cyc_List_List*
_tmpCC7=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple26*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpCC4,(void*(*)(struct _tuple26*))Cyc_Core_fst,
_tmpCC5.free_evars);{struct Cyc_List_List*fs=_tmpCC6;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpCC8=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp1385;void*_tmp1384[2];struct Cyc_String_pa_struct _tmp1383;struct Cyc_String_pa_struct
_tmp1382;(_tmp1382.tag=0,((_tmp1382.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1383.tag=0,((_tmp1383.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpCC8),((_tmp1384[0]=& _tmp1383,((
_tmp1384[1]=& _tmp1382,Cyc_Tcutil_terr(loc,((_tmp1385="unbound type variable %s in type %s",
_tag_dyneither(_tmp1385,sizeof(char),36))),_tag_dyneither(_tmp1384,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpCC7 != 0)for(0;_tmpCC7 != 0;_tmpCC7=
_tmpCC7->tl){void*e=(void*)_tmpCC7->hd;struct Cyc_Absyn_Kind*_tmpCCD=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpCCE;enum Cyc_Absyn_KindQual _tmpCCF;enum Cyc_Absyn_AliasQual
_tmpCD0;struct Cyc_Absyn_Kind _tmpCD1;enum Cyc_Absyn_KindQual _tmpCD2;enum Cyc_Absyn_AliasQual
_tmpCD3;struct Cyc_Absyn_Kind _tmpCD4;enum Cyc_Absyn_KindQual _tmpCD5;enum Cyc_Absyn_AliasQual
_tmpCD6;struct Cyc_Absyn_Kind _tmpCD7;enum Cyc_Absyn_KindQual _tmpCD8;_LL63E:
_tmpCCE=*_tmpCCD;_tmpCCF=_tmpCCE.kind;if(_tmpCCF != Cyc_Absyn_RgnKind)goto _LL640;
_tmpCD0=_tmpCCE.aliasqual;if(_tmpCD0 != Cyc_Absyn_Unique)goto _LL640;_LL63F: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp1388;void*
_tmp1387;(_tmp1387=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1388="can't unify evar with unique region!",
_tag_dyneither(_tmp1388,sizeof(char),37))),_tag_dyneither(_tmp1387,sizeof(void*),
0)));}goto _LL63D;_LL640: _tmpCD1=*_tmpCCD;_tmpCD2=_tmpCD1.kind;if(_tmpCD2 != Cyc_Absyn_RgnKind)
goto _LL642;_tmpCD3=_tmpCD1.aliasqual;if(_tmpCD3 != Cyc_Absyn_Aliasable)goto _LL642;
_LL641: goto _LL643;_LL642: _tmpCD4=*_tmpCCD;_tmpCD5=_tmpCD4.kind;if(_tmpCD5 != Cyc_Absyn_RgnKind)
goto _LL644;_tmpCD6=_tmpCD4.aliasqual;if(_tmpCD6 != Cyc_Absyn_Top)goto _LL644;
_LL643: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp138B;
void*_tmp138A;(_tmp138A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp138B="can't unify evar with heap!",_tag_dyneither(
_tmp138B,sizeof(char),28))),_tag_dyneither(_tmp138A,sizeof(void*),0)));}goto
_LL63D;_LL644: _tmpCD7=*_tmpCCD;_tmpCD8=_tmpCD7.kind;if(_tmpCD8 != Cyc_Absyn_EffKind)
goto _LL646;_LL645: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp138E;void*_tmp138D;(_tmp138D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp138E="can't unify evar with {}!",
_tag_dyneither(_tmp138E,sizeof(char),26))),_tag_dyneither(_tmp138D,sizeof(void*),
0)));}goto _LL63D;_LL646:;_LL647:{const char*_tmp1393;void*_tmp1392[2];struct Cyc_String_pa_struct
_tmp1391;struct Cyc_String_pa_struct _tmp1390;(_tmp1390.tag=0,((_tmp1390.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1391.tag=0,((_tmp1391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1392[0]=& _tmp1391,((_tmp1392[1]=& _tmp1390,Cyc_Tcutil_terr(loc,((
_tmp1393="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1393,sizeof(char),52))),_tag_dyneither(_tmp1392,sizeof(void*),2)))))))))))));}
goto _LL63D;_LL63D:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
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
vs->hd,(void*)vs2->hd)== 0){const char*_tmp1398;void*_tmp1397[2];struct Cyc_String_pa_struct
_tmp1396;struct Cyc_String_pa_struct _tmp1395;(_tmp1395.tag=0,((_tmp1395.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1396.tag=
0,((_tmp1396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1397[0]=&
_tmp1396,((_tmp1397[1]=& _tmp1395,Cyc_Tcutil_terr(loc,((_tmp1398="%s: %s",
_tag_dyneither(_tmp1398,sizeof(char),7))),_tag_dyneither(_tmp1397,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp1399;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1399="duplicate type variable",
_tag_dyneither(_tmp1399,sizeof(char),24))));}struct _tuple28{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple29{struct Cyc_List_List*f1;void*f2;};struct _tuple30{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp139A;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp139A="",_tag_dyneither(_tmp139A,sizeof(char),1))))!= 0){struct _tuple28*
_tmp139D;struct Cyc_List_List*_tmp139C;fields=((_tmp139C=_cycalloc(sizeof(*
_tmp139C)),((_tmp139C->hd=((_tmp139D=_cycalloc(sizeof(*_tmp139D)),((_tmp139D->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp139D->f2=0,_tmp139D)))))),((
_tmp139C->tl=fields,_tmp139C))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp139F;const char*_tmp139E;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp139F="struct",
_tag_dyneither(_tmp139F,sizeof(char),7)):((_tmp139E="union",_tag_dyneither(
_tmp139E,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple29 _tmpCED;struct Cyc_List_List*_tmpCEE;void*_tmpCEF;struct _tuple29*
_tmpCEC=(struct _tuple29*)des->hd;_tmpCED=*_tmpCEC;_tmpCEE=_tmpCED.f1;_tmpCEF=
_tmpCED.f2;if(_tmpCEE == 0){struct Cyc_List_List*_tmpCF0=fields;for(0;_tmpCF0 != 0;
_tmpCF0=_tmpCF0->tl){if(!(*((struct _tuple28*)_tmpCF0->hd)).f2){(*((struct
_tuple28*)_tmpCF0->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp13A5;struct
Cyc_Absyn_FieldName_struct _tmp13A4;struct Cyc_List_List*_tmp13A3;(*((struct
_tuple29*)des->hd)).f1=(struct Cyc_List_List*)((_tmp13A3=_cycalloc(sizeof(*
_tmp13A3)),((_tmp13A3->hd=(void*)((_tmp13A5=_cycalloc(sizeof(*_tmp13A5)),((
_tmp13A5[0]=((_tmp13A4.tag=1,((_tmp13A4.f1=((*((struct _tuple28*)_tmpCF0->hd)).f1)->name,
_tmp13A4)))),_tmp13A5)))),((_tmp13A3->tl=0,_tmp13A3))))));}{struct _tuple30*
_tmp13A8;struct Cyc_List_List*_tmp13A7;ans=((_tmp13A7=_region_malloc(rgn,sizeof(*
_tmp13A7)),((_tmp13A7->hd=((_tmp13A8=_region_malloc(rgn,sizeof(*_tmp13A8)),((
_tmp13A8->f1=(*((struct _tuple28*)_tmpCF0->hd)).f1,((_tmp13A8->f2=_tmpCEF,
_tmp13A8)))))),((_tmp13A7->tl=ans,_tmp13A7))))));}break;}}if(_tmpCF0 == 0){const
char*_tmp13AC;void*_tmp13AB[1];struct Cyc_String_pa_struct _tmp13AA;(_tmp13AA.tag=
0,((_tmp13AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp13AB[0]=& _tmp13AA,Cyc_Tcutil_terr(loc,((_tmp13AC="too many arguments to %s",
_tag_dyneither(_tmp13AC,sizeof(char),25))),_tag_dyneither(_tmp13AB,sizeof(void*),
1)))))));}}else{if(_tmpCEE->tl != 0){const char*_tmp13AF;void*_tmp13AE;(_tmp13AE=0,
Cyc_Tcutil_terr(loc,((_tmp13AF="multiple designators are not yet supported",
_tag_dyneither(_tmp13AF,sizeof(char),43))),_tag_dyneither(_tmp13AE,sizeof(void*),
0)));}else{void*_tmpCFB=(void*)_tmpCEE->hd;struct _dyneither_ptr*_tmpCFE;_LL649: {
struct Cyc_Absyn_ArrayElement_struct*_tmpCFC=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpCFB;if(_tmpCFC->tag != 0)goto _LL64B;}_LL64A:{const char*_tmp13B3;void*_tmp13B2[
1];struct Cyc_String_pa_struct _tmp13B1;(_tmp13B1.tag=0,((_tmp13B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13B2[0]=& _tmp13B1,Cyc_Tcutil_terr(
loc,((_tmp13B3="array designator used in argument to %s",_tag_dyneither(_tmp13B3,
sizeof(char),40))),_tag_dyneither(_tmp13B2,sizeof(void*),1)))))));}goto _LL648;
_LL64B: {struct Cyc_Absyn_FieldName_struct*_tmpCFD=(struct Cyc_Absyn_FieldName_struct*)
_tmpCFB;if(_tmpCFD->tag != 1)goto _LL648;else{_tmpCFE=_tmpCFD->f1;}}_LL64C: {
struct Cyc_List_List*_tmpD02=fields;for(0;_tmpD02 != 0;_tmpD02=_tmpD02->tl){if(Cyc_strptrcmp(
_tmpCFE,((*((struct _tuple28*)_tmpD02->hd)).f1)->name)== 0){if((*((struct _tuple28*)
_tmpD02->hd)).f2){const char*_tmp13B7;void*_tmp13B6[1];struct Cyc_String_pa_struct
_tmp13B5;(_tmp13B5.tag=0,((_tmp13B5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpCFE),((_tmp13B6[0]=& _tmp13B5,Cyc_Tcutil_terr(loc,((_tmp13B7="member %s has already been used as an argument",
_tag_dyneither(_tmp13B7,sizeof(char),47))),_tag_dyneither(_tmp13B6,sizeof(void*),
1)))))));}(*((struct _tuple28*)_tmpD02->hd)).f2=1;{struct _tuple30*_tmp13BA;struct
Cyc_List_List*_tmp13B9;ans=((_tmp13B9=_region_malloc(rgn,sizeof(*_tmp13B9)),((
_tmp13B9->hd=((_tmp13BA=_region_malloc(rgn,sizeof(*_tmp13BA)),((_tmp13BA->f1=(*((
struct _tuple28*)_tmpD02->hd)).f1,((_tmp13BA->f2=_tmpCEF,_tmp13BA)))))),((
_tmp13B9->tl=ans,_tmp13B9))))));}break;}}if(_tmpD02 == 0){const char*_tmp13BE;void*
_tmp13BD[1];struct Cyc_String_pa_struct _tmp13BC;(_tmp13BC.tag=0,((_tmp13BC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCFE),((_tmp13BD[0]=& _tmp13BC,
Cyc_Tcutil_terr(loc,((_tmp13BE="bad field designator %s",_tag_dyneither(_tmp13BE,
sizeof(char),24))),_tag_dyneither(_tmp13BD,sizeof(void*),1)))))));}goto _LL648;}
_LL648:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple28*)fields->hd)).f2){{const char*_tmp13C1;void*_tmp13C0;(
_tmp13C0=0,Cyc_Tcutil_terr(loc,((_tmp13C1="too few arguments to struct",
_tag_dyneither(_tmp13C1,sizeof(char),28))),_tag_dyneither(_tmp13C0,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple28*)fields->hd)).f2){if(found){const char*_tmp13C4;void*_tmp13C3;(_tmp13C3=
0,Cyc_Tcutil_terr(loc,((_tmp13C4="only one member of a union is allowed",
_tag_dyneither(_tmp13C4,sizeof(char),38))),_tag_dyneither(_tmp13C3,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp13C7;void*_tmp13C6;(_tmp13C6=0,Cyc_Tcutil_terr(
loc,((_tmp13C7="missing member for union",_tag_dyneither(_tmp13C7,sizeof(char),
25))),_tag_dyneither(_tmp13C6,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpD13=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpD15;
void*_tmpD16;struct Cyc_Absyn_PtrAtts _tmpD17;union Cyc_Absyn_Constraint*_tmpD18;
_LL64E: {struct Cyc_Absyn_PointerType_struct*_tmpD14=(struct Cyc_Absyn_PointerType_struct*)
_tmpD13;if(_tmpD14->tag != 5)goto _LL650;else{_tmpD15=_tmpD14->f1;_tmpD16=_tmpD15.elt_typ;
_tmpD17=_tmpD15.ptr_atts;_tmpD18=_tmpD17.bounds;}}_LL64F: {void*_tmpD19=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpD18);_LL653: {struct Cyc_Absyn_DynEither_b_struct*_tmpD1A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpD19;if(_tmpD1A->tag != 0)goto _LL655;}_LL654:*elt_typ_dest=_tmpD16;return 1;
_LL655:;_LL656: return 0;_LL652:;}_LL650:;_LL651: return 0;_LL64D:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpD1B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpD1D;
void*_tmpD1E;struct Cyc_Absyn_PtrAtts _tmpD1F;union Cyc_Absyn_Constraint*_tmpD20;
_LL658: {struct Cyc_Absyn_PointerType_struct*_tmpD1C=(struct Cyc_Absyn_PointerType_struct*)
_tmpD1B;if(_tmpD1C->tag != 5)goto _LL65A;else{_tmpD1D=_tmpD1C->f1;_tmpD1E=_tmpD1D.elt_typ;
_tmpD1F=_tmpD1D.ptr_atts;_tmpD20=_tmpD1F.zero_term;}}_LL659:*elt_typ_dest=
_tmpD1E;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpD20);_LL65A:;_LL65B: return 0;_LL657:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpD21=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpD23;void*_tmpD24;struct Cyc_Absyn_PtrAtts _tmpD25;
union Cyc_Absyn_Constraint*_tmpD26;union Cyc_Absyn_Constraint*_tmpD27;struct Cyc_Absyn_ArrayInfo
_tmpD29;void*_tmpD2A;struct Cyc_Absyn_Tqual _tmpD2B;struct Cyc_Absyn_Exp*_tmpD2C;
union Cyc_Absyn_Constraint*_tmpD2D;_LL65D: {struct Cyc_Absyn_PointerType_struct*
_tmpD22=(struct Cyc_Absyn_PointerType_struct*)_tmpD21;if(_tmpD22->tag != 5)goto
_LL65F;else{_tmpD23=_tmpD22->f1;_tmpD24=_tmpD23.elt_typ;_tmpD25=_tmpD23.ptr_atts;
_tmpD26=_tmpD25.bounds;_tmpD27=_tmpD25.zero_term;}}_LL65E: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD27)){*ptr_type=t;*elt_type=
_tmpD24;{void*_tmpD2E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpD26);_LL664: {struct Cyc_Absyn_DynEither_b_struct*
_tmpD2F=(struct Cyc_Absyn_DynEither_b_struct*)_tmpD2E;if(_tmpD2F->tag != 0)goto
_LL666;}_LL665:*is_dyneither=1;goto _LL663;_LL666:;_LL667:*is_dyneither=0;goto
_LL663;_LL663:;}return 1;}else{return 0;}_LL65F: {struct Cyc_Absyn_ArrayType_struct*
_tmpD28=(struct Cyc_Absyn_ArrayType_struct*)_tmpD21;if(_tmpD28->tag != 9)goto
_LL661;else{_tmpD29=_tmpD28->f1;_tmpD2A=_tmpD29.elt_type;_tmpD2B=_tmpD29.tq;
_tmpD2C=_tmpD29.num_elts;_tmpD2D=_tmpD29.zero_term;}}_LL660: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD2D)){*elt_type=_tmpD2A;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp13DC;struct Cyc_Absyn_PtrAtts
_tmp13DB;struct Cyc_Absyn_Upper_b_struct _tmp13DA;struct Cyc_Absyn_Upper_b_struct*
_tmp13D9;struct Cyc_Absyn_PtrInfo _tmp13D8;struct Cyc_Absyn_PointerType_struct*
_tmp13D7;*ptr_type=(void*)((_tmp13D7=_cycalloc(sizeof(*_tmp13D7)),((_tmp13D7[0]=((
_tmp13DC.tag=5,((_tmp13DC.f1=((_tmp13D8.elt_typ=_tmpD2A,((_tmp13D8.elt_tq=
_tmpD2B,((_tmp13D8.ptr_atts=((_tmp13DB.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp13DB.nullable=Cyc_Absyn_false_conref,((_tmp13DB.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((
_tmp13D9[0]=((_tmp13DA.tag=1,((_tmp13DA.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpD2C),_tmp13DA)))),_tmp13D9))))),((_tmp13DB.zero_term=_tmpD2D,((_tmp13DB.ptrloc=
0,_tmp13DB)))))))))),_tmp13D8)))))),_tmp13DC)))),_tmp13D7))));}return 1;}else{
return 0;}_LL661:;_LL662: return 0;_LL65C:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpD36=e1->r;struct Cyc_Absyn_Exp*_tmpD39;struct Cyc_Absyn_Exp*_tmpD3B;struct Cyc_Absyn_Exp*
_tmpD3D;struct Cyc_Absyn_Exp*_tmpD3F;struct Cyc_Absyn_Exp*_tmpD41;struct Cyc_Absyn_Exp*
_tmpD43;_LL669: {struct Cyc_Absyn_Cast_e_struct*_tmpD37=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD36;if(_tmpD37->tag != 15)goto _LL66B;}_LL66A: {const char*_tmp13E0;void*
_tmp13DF[1];struct Cyc_String_pa_struct _tmp13DE;(_tmp13DE.tag=0,((_tmp13DE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp13DF[0]=& _tmp13DE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp13E0="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp13E0,sizeof(char),29))),_tag_dyneither(_tmp13DF,sizeof(void*),1)))))));}
_LL66B: {struct Cyc_Absyn_Deref_e_struct*_tmpD38=(struct Cyc_Absyn_Deref_e_struct*)
_tmpD36;if(_tmpD38->tag != 21)goto _LL66D;else{_tmpD39=_tmpD38->f1;}}_LL66C:
_tmpD3B=_tmpD39;goto _LL66E;_LL66D: {struct Cyc_Absyn_Subscript_e_struct*_tmpD3A=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpD36;if(_tmpD3A->tag != 24)goto _LL66F;
else{_tmpD3B=_tmpD3A->f1;}}_LL66E: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpD3B->topt))->v,ptr_type,is_dyneither,
elt_type);_LL66F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpD3C=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD36;if(_tmpD3C->tag != 23)goto _LL671;else{_tmpD3D=_tmpD3C->f1;}}_LL670:
_tmpD3F=_tmpD3D;goto _LL672;_LL671: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD3E=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpD36;if(_tmpD3E->tag != 22)goto _LL673;
else{_tmpD3F=_tmpD3E->f1;}}_LL672: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpD3F->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp13E4;void*_tmp13E3[1];struct Cyc_String_pa_struct _tmp13E2;(_tmp13E2.tag=
0,((_tmp13E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp13E3[0]=& _tmp13E2,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E4="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp13E4,sizeof(char),51))),_tag_dyneither(_tmp13E3,sizeof(void*),
1)))))));}return 0;_LL673: {struct Cyc_Absyn_Instantiate_e_struct*_tmpD40=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpD36;if(_tmpD40->tag != 14)goto _LL675;else{
_tmpD41=_tmpD40->f1;}}_LL674: _tmpD43=_tmpD41;goto _LL676;_LL675: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpD42=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpD36;if(_tmpD42->tag != 13)
goto _LL677;else{_tmpD43=_tmpD42->f1;}}_LL676: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD43->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp13E8;void*_tmp13E7[1];struct Cyc_String_pa_struct
_tmp13E6;(_tmp13E6.tag=0,((_tmp13E6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp13E7[0]=& _tmp13E6,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E8="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp13E8,sizeof(char),49))),_tag_dyneither(_tmp13E7,sizeof(void*),
1)))))));}return 0;_LL677: {struct Cyc_Absyn_Var_e_struct*_tmpD44=(struct Cyc_Absyn_Var_e_struct*)
_tmpD36;if(_tmpD44->tag != 1)goto _LL679;}_LL678: return 0;_LL679:;_LL67A: {const
char*_tmp13EC;void*_tmp13EB[1];struct Cyc_String_pa_struct _tmp13EA;(_tmp13EA.tag=
0,((_tmp13EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp13EB[0]=& _tmp13EA,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13EC="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp13EC,sizeof(char),39))),_tag_dyneither(_tmp13EB,sizeof(void*),
1)))))));}_LL668:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpD51=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpD54;_LL67C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpD52=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpD51;if(_tmpD52->tag != 22)goto _LL67E;}_LL67D: return 1;_LL67E: {struct Cyc_Absyn_VarType_struct*
_tmpD53=(struct Cyc_Absyn_VarType_struct*)_tmpD51;if(_tmpD53->tag != 2)goto _LL680;
else{_tmpD54=_tmpD53->f1;}}_LL67F: {struct Cyc_Absyn_Kind _tmpD56;enum Cyc_Absyn_KindQual
_tmpD57;enum Cyc_Absyn_AliasQual _tmpD58;struct Cyc_Absyn_Kind*_tmpD55=Cyc_Tcutil_tvar_kind(
_tmpD54,& Cyc_Tcutil_rk);_tmpD56=*_tmpD55;_tmpD57=_tmpD56.kind;_tmpD58=_tmpD56.aliasqual;
if(_tmpD57 == Cyc_Absyn_RgnKind  && (_tmpD58 == Cyc_Absyn_Unique  || _tmpD58 == Cyc_Absyn_Top)){
void*_tmpD59=Cyc_Absyn_compress_kb(_tmpD54->kind);struct Cyc_Core_Opt*_tmpD5B;
struct Cyc_Core_Opt**_tmpD5C;struct Cyc_Absyn_Kind*_tmpD5D;struct Cyc_Absyn_Kind
_tmpD5E;enum Cyc_Absyn_KindQual _tmpD5F;enum Cyc_Absyn_AliasQual _tmpD60;_LL683: {
struct Cyc_Absyn_Less_kb_struct*_tmpD5A=(struct Cyc_Absyn_Less_kb_struct*)_tmpD59;
if(_tmpD5A->tag != 2)goto _LL685;else{_tmpD5B=_tmpD5A->f1;_tmpD5C=(struct Cyc_Core_Opt**)&
_tmpD5A->f1;_tmpD5D=_tmpD5A->f2;_tmpD5E=*_tmpD5D;_tmpD5F=_tmpD5E.kind;if(_tmpD5F
!= Cyc_Absyn_RgnKind)goto _LL685;_tmpD60=_tmpD5E.aliasqual;if(_tmpD60 != Cyc_Absyn_Top)
goto _LL685;}}_LL684:{struct Cyc_Absyn_Less_kb_struct*_tmp13F2;struct Cyc_Absyn_Less_kb_struct
_tmp13F1;struct Cyc_Core_Opt*_tmp13F0;*_tmpD5C=((_tmp13F0=_cycalloc(sizeof(*
_tmp13F0)),((_tmp13F0->v=(void*)((_tmp13F2=_cycalloc(sizeof(*_tmp13F2)),((
_tmp13F2[0]=((_tmp13F1.tag=2,((_tmp13F1.f1=0,((_tmp13F1.f2=& Cyc_Tcutil_rk,
_tmp13F1)))))),_tmp13F2)))),_tmp13F0))));}return 0;_LL685:;_LL686: return 1;_LL682:;}
return 0;}_LL680:;_LL681: return 0;_LL67B:;}int Cyc_Tcutil_is_noalias_pointer(void*t);
int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpD64=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpD66;struct Cyc_Absyn_PtrAtts _tmpD67;void*_tmpD68;
struct Cyc_Absyn_Tvar*_tmpD6A;_LL688: {struct Cyc_Absyn_PointerType_struct*_tmpD65=(
struct Cyc_Absyn_PointerType_struct*)_tmpD64;if(_tmpD65->tag != 5)goto _LL68A;else{
_tmpD66=_tmpD65->f1;_tmpD67=_tmpD66.ptr_atts;_tmpD68=_tmpD67.rgn;}}_LL689: return
Cyc_Tcutil_is_noalias_region(_tmpD68);_LL68A: {struct Cyc_Absyn_VarType_struct*
_tmpD69=(struct Cyc_Absyn_VarType_struct*)_tmpD64;if(_tmpD69->tag != 2)goto _LL68C;
else{_tmpD6A=_tmpD69->f1;}}_LL68B: {struct Cyc_Absyn_Kind _tmpD6C;enum Cyc_Absyn_KindQual
_tmpD6D;enum Cyc_Absyn_AliasQual _tmpD6E;struct Cyc_Absyn_Kind*_tmpD6B=Cyc_Tcutil_tvar_kind(
_tmpD6A,& Cyc_Tcutil_bk);_tmpD6C=*_tmpD6B;_tmpD6D=_tmpD6C.kind;_tmpD6E=_tmpD6C.aliasqual;
switch(_tmpD6D){case Cyc_Absyn_BoxKind: _LL68E: goto _LL68F;case Cyc_Absyn_AnyKind:
_LL68F: goto _LL690;case Cyc_Absyn_MemKind: _LL690: if(_tmpD6E == Cyc_Absyn_Unique  || 
_tmpD6E == Cyc_Absyn_Top){void*_tmpD6F=Cyc_Absyn_compress_kb(_tmpD6A->kind);
struct Cyc_Core_Opt*_tmpD71;struct Cyc_Core_Opt**_tmpD72;struct Cyc_Absyn_Kind*
_tmpD73;struct Cyc_Absyn_Kind _tmpD74;enum Cyc_Absyn_KindQual _tmpD75;enum Cyc_Absyn_AliasQual
_tmpD76;_LL693: {struct Cyc_Absyn_Less_kb_struct*_tmpD70=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD6F;if(_tmpD70->tag != 2)goto _LL695;else{_tmpD71=_tmpD70->f1;_tmpD72=(struct
Cyc_Core_Opt**)& _tmpD70->f1;_tmpD73=_tmpD70->f2;_tmpD74=*_tmpD73;_tmpD75=_tmpD74.kind;
_tmpD76=_tmpD74.aliasqual;if(_tmpD76 != Cyc_Absyn_Top)goto _LL695;}}_LL694:{struct
Cyc_Absyn_Less_kb_struct*_tmp13FC;struct Cyc_Absyn_Kind*_tmp13FB;struct Cyc_Absyn_Less_kb_struct
_tmp13FA;struct Cyc_Core_Opt*_tmp13F9;*_tmpD72=((_tmp13F9=_cycalloc(sizeof(*
_tmp13F9)),((_tmp13F9->v=(void*)((_tmp13FC=_cycalloc(sizeof(*_tmp13FC)),((
_tmp13FC[0]=((_tmp13FA.tag=2,((_tmp13FA.f1=0,((_tmp13FA.f2=((_tmp13FB=
_cycalloc_atomic(sizeof(*_tmp13FB)),((_tmp13FB->kind=_tmpD75,((_tmp13FB->aliasqual=
Cyc_Absyn_Aliasable,_tmp13FB)))))),_tmp13FA)))))),_tmp13FC)))),_tmp13F9))));}
return 0;_LL695:;_LL696: return 1;_LL692:;}return 0;default: _LL691: return 0;}}_LL68C:;
_LL68D: return 0;_LL687:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpD7B=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpD7B))return 1;{void*_tmpD7C=_tmpD7B;struct Cyc_List_List*_tmpD7E;struct Cyc_Absyn_AggrInfo
_tmpD80;union Cyc_Absyn_AggrInfoU _tmpD81;struct Cyc_Absyn_Aggrdecl**_tmpD82;struct
Cyc_List_List*_tmpD83;struct Cyc_List_List*_tmpD85;struct Cyc_Absyn_AggrInfo
_tmpD87;union Cyc_Absyn_AggrInfoU _tmpD88;struct _tuple4 _tmpD89;struct Cyc_Absyn_DatatypeInfo
_tmpD8B;union Cyc_Absyn_DatatypeInfoU _tmpD8C;struct Cyc_List_List*_tmpD8D;struct
Cyc_Absyn_DatatypeFieldInfo _tmpD8F;union Cyc_Absyn_DatatypeFieldInfoU _tmpD90;
struct Cyc_List_List*_tmpD91;_LL699: {struct Cyc_Absyn_TupleType_struct*_tmpD7D=(
struct Cyc_Absyn_TupleType_struct*)_tmpD7C;if(_tmpD7D->tag != 11)goto _LL69B;else{
_tmpD7E=_tmpD7D->f1;}}_LL69A: while(_tmpD7E != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpD7E->hd)).f2))return 1;_tmpD7E=_tmpD7E->tl;}return 0;
_LL69B: {struct Cyc_Absyn_AggrType_struct*_tmpD7F=(struct Cyc_Absyn_AggrType_struct*)
_tmpD7C;if(_tmpD7F->tag != 12)goto _LL69D;else{_tmpD80=_tmpD7F->f1;_tmpD81=_tmpD80.aggr_info;
if((_tmpD81.KnownAggr).tag != 2)goto _LL69D;_tmpD82=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD81.KnownAggr).val;_tmpD83=_tmpD80.targs;}}_LL69C: if((*_tmpD82)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpD92=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpD82)->tvs,_tmpD83);struct Cyc_List_List*_tmpD93=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD82)->impl))->fields;void*t;
while(_tmpD93 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpD92,((struct Cyc_Absyn_Aggrfield*)
_tmpD93->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD93=_tmpD93->tl;}return 0;}_LL69D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD84=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD7C;if(_tmpD84->tag != 13)goto
_LL69F;else{_tmpD85=_tmpD84->f2;}}_LL69E: while(_tmpD85 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD85->hd)->type))return 1;_tmpD85=_tmpD85->tl;}
return 0;_LL69F: {struct Cyc_Absyn_AggrType_struct*_tmpD86=(struct Cyc_Absyn_AggrType_struct*)
_tmpD7C;if(_tmpD86->tag != 12)goto _LL6A1;else{_tmpD87=_tmpD86->f1;_tmpD88=_tmpD87.aggr_info;
if((_tmpD88.UnknownAggr).tag != 1)goto _LL6A1;_tmpD89=(struct _tuple4)(_tmpD88.UnknownAggr).val;}}
_LL6A0: {const char*_tmp13FF;void*_tmp13FE;(_tmp13FE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FF="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp13FF,sizeof(char),36))),_tag_dyneither(_tmp13FE,sizeof(void*),
0)));}_LL6A1: {struct Cyc_Absyn_DatatypeType_struct*_tmpD8A=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpD7C;if(_tmpD8A->tag != 3)goto _LL6A3;else{_tmpD8B=_tmpD8A->f1;_tmpD8C=_tmpD8B.datatype_info;
_tmpD8D=_tmpD8B.targs;}}_LL6A2: {union Cyc_Absyn_DatatypeInfoU _tmpD96=_tmpD8C;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpD97;struct _tuple2*_tmpD98;int _tmpD99;
struct Cyc_Absyn_Datatypedecl**_tmpD9A;struct Cyc_Absyn_Datatypedecl*_tmpD9B;
struct Cyc_Absyn_Datatypedecl _tmpD9C;struct Cyc_List_List*_tmpD9D;struct Cyc_Core_Opt*
_tmpD9E;_LL6A8: if((_tmpD96.UnknownDatatype).tag != 1)goto _LL6AA;_tmpD97=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpD96.UnknownDatatype).val;_tmpD98=_tmpD97.name;
_tmpD99=_tmpD97.is_extensible;_LL6A9: {const char*_tmp1402;void*_tmp1401;(
_tmp1401=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1402="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1402,
sizeof(char),40))),_tag_dyneither(_tmp1401,sizeof(void*),0)));}_LL6AA: if((
_tmpD96.KnownDatatype).tag != 2)goto _LL6A7;_tmpD9A=(struct Cyc_Absyn_Datatypedecl**)(
_tmpD96.KnownDatatype).val;_tmpD9B=*_tmpD9A;_tmpD9C=*_tmpD9B;_tmpD9D=_tmpD9C.tvs;
_tmpD9E=_tmpD9C.fields;_LL6AB: return 0;_LL6A7:;}_LL6A3: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpD8E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpD7C;if(_tmpD8E->tag != 4)
goto _LL6A5;else{_tmpD8F=_tmpD8E->f1;_tmpD90=_tmpD8F.field_info;_tmpD91=_tmpD8F.targs;}}
_LL6A4: {union Cyc_Absyn_DatatypeFieldInfoU _tmpDA1=_tmpD90;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpDA2;struct _tuple3 _tmpDA3;struct Cyc_Absyn_Datatypedecl*_tmpDA4;struct Cyc_Absyn_Datatypefield*
_tmpDA5;_LL6AD: if((_tmpDA1.UnknownDatatypefield).tag != 1)goto _LL6AF;_tmpDA2=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDA1.UnknownDatatypefield).val;
_LL6AE: {const char*_tmp1405;void*_tmp1404;(_tmp1404=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1405="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1405,sizeof(char),46))),_tag_dyneither(_tmp1404,sizeof(void*),
0)));}_LL6AF: if((_tmpDA1.KnownDatatypefield).tag != 2)goto _LL6AC;_tmpDA3=(struct
_tuple3)(_tmpDA1.KnownDatatypefield).val;_tmpDA4=_tmpDA3.f1;_tmpDA5=_tmpDA3.f2;
_LL6B0: {struct Cyc_List_List*_tmpDA8=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpDA4->tvs,_tmpD91);struct Cyc_List_List*_tmpDA9=
_tmpDA5->typs;while(_tmpDA9 != 0){_tmpD7B=Cyc_Tcutil_rsubstitute(rgn,_tmpDA8,(*((
struct _tuple11*)_tmpDA9->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpD7B))return 1;_tmpDA9=_tmpDA9->tl;}return 0;}_LL6AC:;}_LL6A5:;_LL6A6: return 0;
_LL698:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpDAA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpDAC;union Cyc_Absyn_AggrInfoU _tmpDAD;struct Cyc_Absyn_Aggrdecl**_tmpDAE;struct
Cyc_Absyn_Aggrdecl*_tmpDAF;struct Cyc_List_List*_tmpDB0;struct Cyc_List_List*
_tmpDB2;_LL6B2: {struct Cyc_Absyn_AggrType_struct*_tmpDAB=(struct Cyc_Absyn_AggrType_struct*)
_tmpDAA;if(_tmpDAB->tag != 12)goto _LL6B4;else{_tmpDAC=_tmpDAB->f1;_tmpDAD=_tmpDAC.aggr_info;
if((_tmpDAD.KnownAggr).tag != 2)goto _LL6B4;_tmpDAE=(struct Cyc_Absyn_Aggrdecl**)(
_tmpDAD.KnownAggr).val;_tmpDAF=*_tmpDAE;_tmpDB0=_tmpDAC.targs;}}_LL6B3: _tmpDB2=
_tmpDAF->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDAF->impl))->fields;
goto _LL6B5;_LL6B4: {struct Cyc_Absyn_AnonAggrType_struct*_tmpDB1=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDAA;if(_tmpDB1->tag != 13)goto _LL6B6;else{_tmpDB2=_tmpDB1->f2;}}_LL6B5: {
struct Cyc_Absyn_Aggrfield*_tmpDB3=Cyc_Absyn_lookup_field(_tmpDB2,f);{struct Cyc_Absyn_Aggrfield*
_tmpDB4=_tmpDB3;struct Cyc_Absyn_Aggrfield _tmpDB5;void*_tmpDB6;_LL6B9: if(_tmpDB4
!= 0)goto _LL6BB;_LL6BA: {const char*_tmp1408;void*_tmp1407;(_tmp1407=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1408="is_noalias_field: missing field",
_tag_dyneither(_tmp1408,sizeof(char),32))),_tag_dyneither(_tmp1407,sizeof(void*),
0)));}_LL6BB: if(_tmpDB4 == 0)goto _LL6B8;_tmpDB5=*_tmpDB4;_tmpDB6=_tmpDB5.type;
_LL6BC: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpDB6);_LL6B8:;}return 0;}
_LL6B6:;_LL6B7: {const char*_tmp140C;void*_tmp140B[1];struct Cyc_String_pa_struct
_tmp140A;(_tmp140A.tag=0,((_tmp140A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp140B[0]=& _tmp140A,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140C="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp140C,sizeof(char),36))),_tag_dyneither(_tmp140B,sizeof(void*),
1)))))));}_LL6B1:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_is_noalias_path_aux(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){void*_tmpDBC=e->r;void*_tmpDBE;struct Cyc_Absyn_Exp*
_tmpDC1;struct Cyc_Absyn_Exp*_tmpDC3;struct Cyc_Absyn_Exp*_tmpDC5;struct
_dyneither_ptr*_tmpDC6;struct Cyc_Absyn_Exp*_tmpDC8;struct Cyc_Absyn_Exp*_tmpDC9;
struct Cyc_Absyn_Exp*_tmpDCB;struct Cyc_Absyn_Exp*_tmpDCC;struct Cyc_Absyn_Exp*
_tmpDCE;struct Cyc_Absyn_Exp*_tmpDD0;struct Cyc_Absyn_Stmt*_tmpDD2;_LL6BE: {struct
Cyc_Absyn_Var_e_struct*_tmpDBD=(struct Cyc_Absyn_Var_e_struct*)_tmpDBC;if(_tmpDBD->tag
!= 1)goto _LL6C0;else{_tmpDBE=(void*)_tmpDBD->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpDBF=(struct Cyc_Absyn_Global_b_struct*)_tmpDBE;if(_tmpDBF->tag != 1)goto _LL6C0;};}}
_LL6BF: return 0;_LL6C0: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpDC0=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDBC;if(_tmpDC0->tag != 23)goto _LL6C2;else{_tmpDC1=_tmpDC0->f1;}}_LL6C1:
_tmpDC3=_tmpDC1;goto _LL6C3;_LL6C2: {struct Cyc_Absyn_Deref_e_struct*_tmpDC2=(
struct Cyc_Absyn_Deref_e_struct*)_tmpDBC;if(_tmpDC2->tag != 21)goto _LL6C4;else{
_tmpDC3=_tmpDC2->f1;}}_LL6C3: return Cyc_Tcutil_is_noalias_pointer((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpDC3->topt))->v) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpDC3);_LL6C4: {struct Cyc_Absyn_AggrMember_e_struct*_tmpDC4=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDBC;if(_tmpDC4->tag != 22)goto _LL6C6;else{_tmpDC5=_tmpDC4->f1;_tmpDC6=_tmpDC4->f2;}}
_LL6C5: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpDC5);_LL6C6: {struct Cyc_Absyn_Subscript_e_struct*
_tmpDC7=(struct Cyc_Absyn_Subscript_e_struct*)_tmpDBC;if(_tmpDC7->tag != 24)goto
_LL6C8;else{_tmpDC8=_tmpDC7->f1;_tmpDC9=_tmpDC7->f2;}}_LL6C7: {void*_tmpDD3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDC8->topt))->v);_LL6D3: {struct Cyc_Absyn_TupleType_struct*
_tmpDD4=(struct Cyc_Absyn_TupleType_struct*)_tmpDD3;if(_tmpDD4->tag != 11)goto
_LL6D5;}_LL6D4: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpDC8);_LL6D5:;_LL6D6:
return 0;_LL6D2:;}_LL6C8: {struct Cyc_Absyn_Conditional_e_struct*_tmpDCA=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpDBC;if(_tmpDCA->tag != 6)goto _LL6CA;else{
_tmpDCB=_tmpDCA->f2;_tmpDCC=_tmpDCA->f3;}}_LL6C9: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpDCB) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpDCC);_LL6CA: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpDCD=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpDBC;if(_tmpDCD->tag != 9)goto _LL6CC;
else{_tmpDCE=_tmpDCD->f2;}}_LL6CB: _tmpDD0=_tmpDCE;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Cast_e_struct*
_tmpDCF=(struct Cyc_Absyn_Cast_e_struct*)_tmpDBC;if(_tmpDCF->tag != 15)goto _LL6CE;
else{_tmpDD0=_tmpDCF->f2;}}_LL6CD: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpDD0);
_LL6CE: {struct Cyc_Absyn_StmtExp_e_struct*_tmpDD1=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpDBC;if(_tmpDD1->tag != 37)goto _LL6D0;else{_tmpDD2=_tmpDD1->f1;}}_LL6CF: while(
1){void*_tmpDD5=_tmpDD2->r;struct Cyc_Absyn_Stmt*_tmpDD7;struct Cyc_Absyn_Stmt*
_tmpDD8;struct Cyc_Absyn_Decl*_tmpDDA;struct Cyc_Absyn_Stmt*_tmpDDB;struct Cyc_Absyn_Exp*
_tmpDDD;_LL6D8: {struct Cyc_Absyn_Seq_s_struct*_tmpDD6=(struct Cyc_Absyn_Seq_s_struct*)
_tmpDD5;if(_tmpDD6->tag != 2)goto _LL6DA;else{_tmpDD7=_tmpDD6->f1;_tmpDD8=_tmpDD6->f2;}}
_LL6D9: _tmpDD2=_tmpDD8;goto _LL6D7;_LL6DA: {struct Cyc_Absyn_Decl_s_struct*_tmpDD9=(
struct Cyc_Absyn_Decl_s_struct*)_tmpDD5;if(_tmpDD9->tag != 12)goto _LL6DC;else{
_tmpDDA=_tmpDD9->f1;_tmpDDB=_tmpDD9->f2;}}_LL6DB: _tmpDD2=_tmpDDB;goto _LL6D7;
_LL6DC: {struct Cyc_Absyn_Exp_s_struct*_tmpDDC=(struct Cyc_Absyn_Exp_s_struct*)
_tmpDD5;if(_tmpDDC->tag != 1)goto _LL6DE;else{_tmpDDD=_tmpDDC->f1;}}_LL6DD: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpDDD);_LL6DE:;_LL6DF: {const char*_tmp140F;
void*_tmp140E;(_tmp140E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp140F="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp140F,sizeof(char),40))),_tag_dyneither(_tmp140E,sizeof(void*),
0)));}_LL6D7:;}_LL6D0:;_LL6D1: return 1;_LL6BD:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple16 _tmp1410;struct _tuple16 bogus_ans=(_tmp1410.f1=0,((_tmp1410.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1410)));void*_tmpDE0=e->r;struct _tuple2*_tmpDE2;
void*_tmpDE3;struct Cyc_Absyn_Exp*_tmpDE5;struct _dyneither_ptr*_tmpDE6;int _tmpDE7;
struct Cyc_Absyn_Exp*_tmpDE9;struct _dyneither_ptr*_tmpDEA;int _tmpDEB;struct Cyc_Absyn_Exp*
_tmpDED;struct Cyc_Absyn_Exp*_tmpDEF;struct Cyc_Absyn_Exp*_tmpDF0;_LL6E1: {struct
Cyc_Absyn_Var_e_struct*_tmpDE1=(struct Cyc_Absyn_Var_e_struct*)_tmpDE0;if(_tmpDE1->tag
!= 1)goto _LL6E3;else{_tmpDE2=_tmpDE1->f1;_tmpDE3=(void*)_tmpDE1->f2;}}_LL6E2: {
void*_tmpDF1=_tmpDE3;struct Cyc_Absyn_Vardecl*_tmpDF5;struct Cyc_Absyn_Vardecl*
_tmpDF7;struct Cyc_Absyn_Vardecl*_tmpDF9;struct Cyc_Absyn_Vardecl*_tmpDFB;_LL6EE: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpDF2=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpDF1;if(_tmpDF2->tag != 0)goto _LL6F0;}_LL6EF: goto _LL6F1;_LL6F0: {struct Cyc_Absyn_Funname_b_struct*
_tmpDF3=(struct Cyc_Absyn_Funname_b_struct*)_tmpDF1;if(_tmpDF3->tag != 2)goto
_LL6F2;}_LL6F1: return bogus_ans;_LL6F2: {struct Cyc_Absyn_Global_b_struct*_tmpDF4=(
struct Cyc_Absyn_Global_b_struct*)_tmpDF1;if(_tmpDF4->tag != 1)goto _LL6F4;else{
_tmpDF5=_tmpDF4->f1;}}_LL6F3: {void*_tmpDFC=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6FB: {struct Cyc_Absyn_ArrayType_struct*
_tmpDFD=(struct Cyc_Absyn_ArrayType_struct*)_tmpDFC;if(_tmpDFD->tag != 9)goto
_LL6FD;}_LL6FC: {struct _tuple16 _tmp1411;return(_tmp1411.f1=1,((_tmp1411.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1411)));}_LL6FD:;_LL6FE: {struct _tuple16 _tmp1412;
return(_tmp1412.f1=(_tmpDF5->tq).real_const,((_tmp1412.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1412)));}_LL6FA:;}_LL6F4: {struct Cyc_Absyn_Local_b_struct*_tmpDF6=(struct Cyc_Absyn_Local_b_struct*)
_tmpDF1;if(_tmpDF6->tag != 4)goto _LL6F6;else{_tmpDF7=_tmpDF6->f1;}}_LL6F5: {void*
_tmpE00=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL700: {struct Cyc_Absyn_ArrayType_struct*_tmpE01=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE00;if(_tmpE01->tag != 9)goto _LL702;}_LL701: {struct _tuple16 _tmp1413;return(
_tmp1413.f1=1,((_tmp1413.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpDF7->rgn))->v,
_tmp1413)));}_LL702:;_LL703: _tmpDF7->escapes=1;{struct _tuple16 _tmp1414;return(
_tmp1414.f1=(_tmpDF7->tq).real_const,((_tmp1414.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpDF7->rgn))->v,_tmp1414)));};_LL6FF:;}_LL6F6: {struct Cyc_Absyn_Pat_b_struct*
_tmpDF8=(struct Cyc_Absyn_Pat_b_struct*)_tmpDF1;if(_tmpDF8->tag != 5)goto _LL6F8;
else{_tmpDF9=_tmpDF8->f1;}}_LL6F7: _tmpDFB=_tmpDF9;goto _LL6F9;_LL6F8: {struct Cyc_Absyn_Param_b_struct*
_tmpDFA=(struct Cyc_Absyn_Param_b_struct*)_tmpDF1;if(_tmpDFA->tag != 3)goto _LL6ED;
else{_tmpDFB=_tmpDFA->f1;}}_LL6F9: _tmpDFB->escapes=1;{struct _tuple16 _tmp1415;
return(_tmp1415.f1=(_tmpDFB->tq).real_const,((_tmp1415.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpDFB->rgn))->v,_tmp1415)));};_LL6ED:;}_LL6E3: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpDE4=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpDE0;if(_tmpDE4->tag != 22)goto
_LL6E5;else{_tmpDE5=_tmpDE4->f1;_tmpDE6=_tmpDE4->f2;_tmpDE7=_tmpDE4->f3;}}_LL6E4:
if(_tmpDE7)return bogus_ans;{void*_tmpE05=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpDE5->topt))->v);struct Cyc_List_List*_tmpE07;struct Cyc_Absyn_AggrInfo
_tmpE09;union Cyc_Absyn_AggrInfoU _tmpE0A;struct Cyc_Absyn_Aggrdecl**_tmpE0B;struct
Cyc_Absyn_Aggrdecl*_tmpE0C;_LL705: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE06=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpE05;if(_tmpE06->tag != 13)goto _LL707;
else{_tmpE07=_tmpE06->f2;}}_LL706: {struct Cyc_Absyn_Aggrfield*_tmpE0D=Cyc_Absyn_lookup_field(
_tmpE07,_tmpDE6);if(_tmpE0D != 0  && _tmpE0D->width != 0){struct _tuple16 _tmp1416;
return(_tmp1416.f1=(_tmpE0D->tq).real_const,((_tmp1416.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDE5)).f2,_tmp1416)));}return bogus_ans;}_LL707: {struct Cyc_Absyn_AggrType_struct*
_tmpE08=(struct Cyc_Absyn_AggrType_struct*)_tmpE05;if(_tmpE08->tag != 12)goto
_LL709;else{_tmpE09=_tmpE08->f1;_tmpE0A=_tmpE09.aggr_info;if((_tmpE0A.KnownAggr).tag
!= 2)goto _LL709;_tmpE0B=(struct Cyc_Absyn_Aggrdecl**)(_tmpE0A.KnownAggr).val;
_tmpE0C=*_tmpE0B;}}_LL708: {struct Cyc_Absyn_Aggrfield*_tmpE0F=Cyc_Absyn_lookup_decl_field(
_tmpE0C,_tmpDE6);if(_tmpE0F != 0  && _tmpE0F->width != 0){struct _tuple16 _tmp1417;
return(_tmp1417.f1=(_tmpE0F->tq).real_const,((_tmp1417.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDE5)).f2,_tmp1417)));}return bogus_ans;}_LL709:;_LL70A: return bogus_ans;
_LL704:;};_LL6E5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpDE8=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDE0;if(_tmpDE8->tag != 23)goto _LL6E7;else{_tmpDE9=_tmpDE8->f1;_tmpDEA=_tmpDE8->f2;
_tmpDEB=_tmpDE8->f3;}}_LL6E6: if(_tmpDEB)return bogus_ans;{void*_tmpE11=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDE9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpE13;void*_tmpE14;struct Cyc_Absyn_PtrAtts _tmpE15;void*_tmpE16;_LL70C: {struct
Cyc_Absyn_PointerType_struct*_tmpE12=(struct Cyc_Absyn_PointerType_struct*)
_tmpE11;if(_tmpE12->tag != 5)goto _LL70E;else{_tmpE13=_tmpE12->f1;_tmpE14=_tmpE13.elt_typ;
_tmpE15=_tmpE13.ptr_atts;_tmpE16=_tmpE15.rgn;}}_LL70D: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpE17=Cyc_Tcutil_compress(_tmpE14);struct Cyc_List_List*_tmpE19;
struct Cyc_Absyn_AggrInfo _tmpE1B;union Cyc_Absyn_AggrInfoU _tmpE1C;struct Cyc_Absyn_Aggrdecl**
_tmpE1D;struct Cyc_Absyn_Aggrdecl*_tmpE1E;_LL711: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpE18=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpE17;if(_tmpE18->tag != 13)goto
_LL713;else{_tmpE19=_tmpE18->f2;}}_LL712: finfo=Cyc_Absyn_lookup_field(_tmpE19,
_tmpDEA);goto _LL710;_LL713: {struct Cyc_Absyn_AggrType_struct*_tmpE1A=(struct Cyc_Absyn_AggrType_struct*)
_tmpE17;if(_tmpE1A->tag != 12)goto _LL715;else{_tmpE1B=_tmpE1A->f1;_tmpE1C=_tmpE1B.aggr_info;
if((_tmpE1C.KnownAggr).tag != 2)goto _LL715;_tmpE1D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE1C.KnownAggr).val;_tmpE1E=*_tmpE1D;}}_LL714: finfo=Cyc_Absyn_lookup_decl_field(
_tmpE1E,_tmpDEA);goto _LL710;_LL715:;_LL716: return bogus_ans;_LL710:;}if(finfo != 0
 && finfo->width != 0){struct _tuple16 _tmp1418;return(_tmp1418.f1=(finfo->tq).real_const,((
_tmp1418.f2=_tmpE16,_tmp1418)));}return bogus_ans;}_LL70E:;_LL70F: return bogus_ans;
_LL70B:;};_LL6E7: {struct Cyc_Absyn_Deref_e_struct*_tmpDEC=(struct Cyc_Absyn_Deref_e_struct*)
_tmpDE0;if(_tmpDEC->tag != 21)goto _LL6E9;else{_tmpDED=_tmpDEC->f1;}}_LL6E8: {void*
_tmpE20=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpDED->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpE22;struct Cyc_Absyn_Tqual _tmpE23;struct Cyc_Absyn_PtrAtts
_tmpE24;void*_tmpE25;_LL718: {struct Cyc_Absyn_PointerType_struct*_tmpE21=(struct
Cyc_Absyn_PointerType_struct*)_tmpE20;if(_tmpE21->tag != 5)goto _LL71A;else{
_tmpE22=_tmpE21->f1;_tmpE23=_tmpE22.elt_tq;_tmpE24=_tmpE22.ptr_atts;_tmpE25=
_tmpE24.rgn;}}_LL719: {struct _tuple16 _tmp1419;return(_tmp1419.f1=_tmpE23.real_const,((
_tmp1419.f2=_tmpE25,_tmp1419)));}_LL71A:;_LL71B: return bogus_ans;_LL717:;}_LL6E9: {
struct Cyc_Absyn_Subscript_e_struct*_tmpDEE=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpDE0;if(_tmpDEE->tag != 24)goto _LL6EB;else{_tmpDEF=_tmpDEE->f1;_tmpDF0=_tmpDEE->f2;}}
_LL6EA: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpDEF->topt))->v);void*_tmpE27=t;struct Cyc_List_List*_tmpE29;struct Cyc_Absyn_PtrInfo
_tmpE2B;struct Cyc_Absyn_Tqual _tmpE2C;struct Cyc_Absyn_PtrAtts _tmpE2D;void*_tmpE2E;
struct Cyc_Absyn_ArrayInfo _tmpE30;struct Cyc_Absyn_Tqual _tmpE31;_LL71D: {struct Cyc_Absyn_TupleType_struct*
_tmpE28=(struct Cyc_Absyn_TupleType_struct*)_tmpE27;if(_tmpE28->tag != 11)goto
_LL71F;else{_tmpE29=_tmpE28->f1;}}_LL71E: {unsigned int _tmpE33;int _tmpE34;struct
_tuple13 _tmpE32=Cyc_Evexp_eval_const_uint_exp(_tmpDF0);_tmpE33=_tmpE32.f1;
_tmpE34=_tmpE32.f2;if(!_tmpE34)return bogus_ans;{struct _tuple11*_tmpE35=Cyc_Absyn_lookup_tuple_field(
_tmpE29,(int)_tmpE33);if(_tmpE35 != 0){struct _tuple16 _tmp141A;return(_tmp141A.f1=((*
_tmpE35).f1).real_const,((_tmp141A.f2=(Cyc_Tcutil_addressof_props(te,_tmpDEF)).f2,
_tmp141A)));}return bogus_ans;};}_LL71F: {struct Cyc_Absyn_PointerType_struct*
_tmpE2A=(struct Cyc_Absyn_PointerType_struct*)_tmpE27;if(_tmpE2A->tag != 5)goto
_LL721;else{_tmpE2B=_tmpE2A->f1;_tmpE2C=_tmpE2B.elt_tq;_tmpE2D=_tmpE2B.ptr_atts;
_tmpE2E=_tmpE2D.rgn;}}_LL720: {struct _tuple16 _tmp141B;return(_tmp141B.f1=_tmpE2C.real_const,((
_tmp141B.f2=_tmpE2E,_tmp141B)));}_LL721: {struct Cyc_Absyn_ArrayType_struct*
_tmpE2F=(struct Cyc_Absyn_ArrayType_struct*)_tmpE27;if(_tmpE2F->tag != 9)goto
_LL723;else{_tmpE30=_tmpE2F->f1;_tmpE31=_tmpE30.tq;}}_LL722: {struct _tuple16
_tmp141C;return(_tmp141C.f1=_tmpE31.real_const,((_tmp141C.f2=(Cyc_Tcutil_addressof_props(
te,_tmpDEF)).f2,_tmp141C)));}_LL723:;_LL724: return bogus_ans;_LL71C:;}_LL6EB:;
_LL6EC:{const char*_tmp141F;void*_tmp141E;(_tmp141E=0,Cyc_Tcutil_terr(e->loc,((
_tmp141F="unary & applied to non-lvalue",_tag_dyneither(_tmp141F,sizeof(char),30))),
_tag_dyneither(_tmp141E,sizeof(void*),0)));}return bogus_ans;_LL6E0:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpE3C=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpE3E;void*_tmpE3F;struct Cyc_Absyn_Tqual
_tmpE40;union Cyc_Absyn_Constraint*_tmpE41;_LL726: {struct Cyc_Absyn_ArrayType_struct*
_tmpE3D=(struct Cyc_Absyn_ArrayType_struct*)_tmpE3C;if(_tmpE3D->tag != 9)goto
_LL728;else{_tmpE3E=_tmpE3D->f1;_tmpE3F=_tmpE3E.elt_type;_tmpE40=_tmpE3E.tq;
_tmpE41=_tmpE3E.zero_term;}}_LL727: {void*_tmpE43;struct _tuple16 _tmpE42=Cyc_Tcutil_addressof_props(
te,e);_tmpE43=_tmpE42.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1422;struct Cyc_Absyn_Upper_b_struct*
_tmp1421;return Cyc_Absyn_atb_typ(_tmpE3F,_tmpE43,_tmpE40,(void*)((_tmp1421=
_cycalloc(sizeof(*_tmp1421)),((_tmp1421[0]=((_tmp1422.tag=1,((_tmp1422.f1=e,
_tmp1422)))),_tmp1421)))),_tmpE41);};}_LL728:;_LL729: return e_typ;_LL725:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpE46=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpE49;
_LL72B: {struct Cyc_Absyn_DynEither_b_struct*_tmpE47=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE46;if(_tmpE47->tag != 0)goto _LL72D;}_LL72C: return;_LL72D: {struct Cyc_Absyn_Upper_b_struct*
_tmpE48=(struct Cyc_Absyn_Upper_b_struct*)_tmpE46;if(_tmpE48->tag != 1)goto _LL72A;
else{_tmpE49=_tmpE48->f1;}}_LL72E: {unsigned int _tmpE4B;int _tmpE4C;struct
_tuple13 _tmpE4A=Cyc_Evexp_eval_const_uint_exp(_tmpE49);_tmpE4B=_tmpE4A.f1;
_tmpE4C=_tmpE4A.f2;if(_tmpE4C  && _tmpE4B <= i){const char*_tmp1427;void*_tmp1426[2];
struct Cyc_Int_pa_struct _tmp1425;struct Cyc_Int_pa_struct _tmp1424;(_tmp1424.tag=1,((
_tmp1424.f1=(unsigned long)((int)i),((_tmp1425.tag=1,((_tmp1425.f1=(
unsigned long)((int)_tmpE4B),((_tmp1426[0]=& _tmp1425,((_tmp1426[1]=& _tmp1424,Cyc_Tcutil_terr(
loc,((_tmp1427="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1427,
sizeof(char),39))),_tag_dyneither(_tmp1426,sizeof(void*),2)))))))))))));}return;}
_LL72A:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpE51=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpE53;_LL730: {struct
Cyc_Absyn_Upper_b_struct*_tmpE52=(struct Cyc_Absyn_Upper_b_struct*)_tmpE51;if(
_tmpE52->tag != 1)goto _LL732;else{_tmpE53=_tmpE52->f1;}}_LL731: {unsigned int
_tmpE55;int _tmpE56;struct _tuple13 _tmpE54=Cyc_Evexp_eval_const_uint_exp(_tmpE53);
_tmpE55=_tmpE54.f1;_tmpE56=_tmpE54.f2;return _tmpE56  && _tmpE55 == 1;}_LL732:;
_LL733: return 0;_LL72F:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpE57=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpE5F;
void*_tmpE60;union Cyc_Absyn_Constraint*_tmpE61;struct Cyc_List_List*_tmpE63;
struct Cyc_Absyn_AggrInfo _tmpE65;union Cyc_Absyn_AggrInfoU _tmpE66;struct _tuple4
_tmpE67;struct Cyc_Absyn_AggrInfo _tmpE69;union Cyc_Absyn_AggrInfoU _tmpE6A;struct
Cyc_Absyn_Aggrdecl**_tmpE6B;struct Cyc_Absyn_Aggrdecl*_tmpE6C;struct Cyc_List_List*
_tmpE6D;struct Cyc_List_List*_tmpE6F;_LL735: {struct Cyc_Absyn_VoidType_struct*
_tmpE58=(struct Cyc_Absyn_VoidType_struct*)_tmpE57;if(_tmpE58->tag != 0)goto _LL737;}
_LL736: goto _LL738;_LL737: {struct Cyc_Absyn_IntType_struct*_tmpE59=(struct Cyc_Absyn_IntType_struct*)
_tmpE57;if(_tmpE59->tag != 6)goto _LL739;}_LL738: goto _LL73A;_LL739: {struct Cyc_Absyn_FloatType_struct*
_tmpE5A=(struct Cyc_Absyn_FloatType_struct*)_tmpE57;if(_tmpE5A->tag != 7)goto
_LL73B;}_LL73A: goto _LL73C;_LL73B: {struct Cyc_Absyn_DoubleType_struct*_tmpE5B=(
struct Cyc_Absyn_DoubleType_struct*)_tmpE57;if(_tmpE5B->tag != 8)goto _LL73D;}
_LL73C: return 1;_LL73D: {struct Cyc_Absyn_EnumType_struct*_tmpE5C=(struct Cyc_Absyn_EnumType_struct*)
_tmpE57;if(_tmpE5C->tag != 14)goto _LL73F;}_LL73E: goto _LL740;_LL73F: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpE5D=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE57;if(_tmpE5D->tag != 15)goto
_LL741;}_LL740: return 0;_LL741: {struct Cyc_Absyn_ArrayType_struct*_tmpE5E=(struct
Cyc_Absyn_ArrayType_struct*)_tmpE57;if(_tmpE5E->tag != 9)goto _LL743;else{_tmpE5F=
_tmpE5E->f1;_tmpE60=_tmpE5F.elt_type;_tmpE61=_tmpE5F.zero_term;}}_LL742: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE61) && Cyc_Tcutil_bits_only(
_tmpE60);_LL743: {struct Cyc_Absyn_TupleType_struct*_tmpE62=(struct Cyc_Absyn_TupleType_struct*)
_tmpE57;if(_tmpE62->tag != 11)goto _LL745;else{_tmpE63=_tmpE62->f1;}}_LL744: for(0;
_tmpE63 != 0;_tmpE63=_tmpE63->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpE63->hd)).f2))return 0;}return 1;_LL745: {struct Cyc_Absyn_AggrType_struct*
_tmpE64=(struct Cyc_Absyn_AggrType_struct*)_tmpE57;if(_tmpE64->tag != 12)goto
_LL747;else{_tmpE65=_tmpE64->f1;_tmpE66=_tmpE65.aggr_info;if((_tmpE66.UnknownAggr).tag
!= 1)goto _LL747;_tmpE67=(struct _tuple4)(_tmpE66.UnknownAggr).val;}}_LL746: return
0;_LL747: {struct Cyc_Absyn_AggrType_struct*_tmpE68=(struct Cyc_Absyn_AggrType_struct*)
_tmpE57;if(_tmpE68->tag != 12)goto _LL749;else{_tmpE69=_tmpE68->f1;_tmpE6A=_tmpE69.aggr_info;
if((_tmpE6A.KnownAggr).tag != 2)goto _LL749;_tmpE6B=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE6A.KnownAggr).val;_tmpE6C=*_tmpE6B;_tmpE6D=_tmpE69.targs;}}_LL748: if(
_tmpE6C->impl == 0)return 0;{struct _RegionHandle _tmpE70=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpE70;_push_region(rgn);{struct Cyc_List_List*_tmpE71=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpE6C->tvs,_tmpE6D);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE6C->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpE71,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpE72=0;_npop_handler(0);return
_tmpE72;}}}{int _tmpE73=1;_npop_handler(0);return _tmpE73;};};_pop_region(rgn);};
_LL749: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE6E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE57;if(_tmpE6E->tag != 13)goto _LL74B;else{_tmpE6F=_tmpE6E->f2;}}_LL74A: for(0;
_tmpE6F != 0;_tmpE6F=_tmpE6F->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpE6F->hd)->type))return 0;}return 1;_LL74B:;_LL74C: return 0;_LL734:;}struct
_tuple31{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpE74=e->r;
struct _tuple2*_tmpE7C;void*_tmpE7D;struct Cyc_Absyn_Exp*_tmpE7F;struct Cyc_Absyn_Exp*
_tmpE80;struct Cyc_Absyn_Exp*_tmpE81;struct Cyc_Absyn_Exp*_tmpE83;struct Cyc_Absyn_Exp*
_tmpE84;struct Cyc_Absyn_Exp*_tmpE86;struct Cyc_Absyn_Exp*_tmpE88;void*_tmpE8A;
struct Cyc_Absyn_Exp*_tmpE8B;enum Cyc_Absyn_Coercion _tmpE8C;void*_tmpE8E;struct
Cyc_Absyn_Exp*_tmpE8F;struct Cyc_Absyn_Exp*_tmpE91;struct Cyc_Absyn_Exp*_tmpE93;
struct Cyc_Absyn_Exp*_tmpE94;struct Cyc_List_List*_tmpE96;struct Cyc_List_List*
_tmpE98;struct Cyc_List_List*_tmpE9A;enum Cyc_Absyn_Primop _tmpE9C;struct Cyc_List_List*
_tmpE9D;struct Cyc_List_List*_tmpE9F;struct Cyc_List_List*_tmpEA1;_LL74E: {struct
Cyc_Absyn_Const_e_struct*_tmpE75=(struct Cyc_Absyn_Const_e_struct*)_tmpE74;if(
_tmpE75->tag != 0)goto _LL750;}_LL74F: goto _LL751;_LL750: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpE76=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE74;if(_tmpE76->tag != 18)goto
_LL752;}_LL751: goto _LL753;_LL752: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpE77=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE74;if(_tmpE77->tag != 19)goto _LL754;}
_LL753: goto _LL755;_LL754: {struct Cyc_Absyn_Offsetof_e_struct*_tmpE78=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpE74;if(_tmpE78->tag != 20)goto _LL756;}_LL755: goto _LL757;_LL756: {struct Cyc_Absyn_Enum_e_struct*
_tmpE79=(struct Cyc_Absyn_Enum_e_struct*)_tmpE74;if(_tmpE79->tag != 32)goto _LL758;}
_LL757: goto _LL759;_LL758: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpE7A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpE74;if(_tmpE7A->tag != 33)goto _LL75A;}_LL759: return 1;_LL75A: {struct Cyc_Absyn_Var_e_struct*
_tmpE7B=(struct Cyc_Absyn_Var_e_struct*)_tmpE74;if(_tmpE7B->tag != 1)goto _LL75C;
else{_tmpE7C=_tmpE7B->f1;_tmpE7D=(void*)_tmpE7B->f2;}}_LL75B: {void*_tmpEA2=
_tmpE7D;struct Cyc_Absyn_Vardecl*_tmpEA5;_LL77B: {struct Cyc_Absyn_Funname_b_struct*
_tmpEA3=(struct Cyc_Absyn_Funname_b_struct*)_tmpEA2;if(_tmpEA3->tag != 2)goto
_LL77D;}_LL77C: return 1;_LL77D: {struct Cyc_Absyn_Global_b_struct*_tmpEA4=(struct
Cyc_Absyn_Global_b_struct*)_tmpEA2;if(_tmpEA4->tag != 1)goto _LL77F;else{_tmpEA5=
_tmpEA4->f1;}}_LL77E: {void*_tmpEA7=Cyc_Tcutil_compress(_tmpEA5->type);_LL784: {
struct Cyc_Absyn_ArrayType_struct*_tmpEA8=(struct Cyc_Absyn_ArrayType_struct*)
_tmpEA7;if(_tmpEA8->tag != 9)goto _LL786;}_LL785: goto _LL787;_LL786: {struct Cyc_Absyn_FnType_struct*
_tmpEA9=(struct Cyc_Absyn_FnType_struct*)_tmpEA7;if(_tmpEA9->tag != 10)goto _LL788;}
_LL787: return 1;_LL788:;_LL789: return var_okay;_LL783:;}_LL77F: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpEA6=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpEA2;if(_tmpEA6->tag != 0)goto
_LL781;}_LL780: return 0;_LL781:;_LL782: return var_okay;_LL77A:;}_LL75C: {struct Cyc_Absyn_Conditional_e_struct*
_tmpE7E=(struct Cyc_Absyn_Conditional_e_struct*)_tmpE74;if(_tmpE7E->tag != 6)goto
_LL75E;else{_tmpE7F=_tmpE7E->f1;_tmpE80=_tmpE7E->f2;_tmpE81=_tmpE7E->f3;}}_LL75D:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpE7F) && Cyc_Tcutil_cnst_exp(te,0,_tmpE80))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpE81);_LL75E: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpE82=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpE74;if(_tmpE82->tag != 9)goto _LL760;
else{_tmpE83=_tmpE82->f1;_tmpE84=_tmpE82->f2;}}_LL75F: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE83) && Cyc_Tcutil_cnst_exp(te,0,_tmpE84);_LL760: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE85=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE74;if(_tmpE85->tag != 13)
goto _LL762;else{_tmpE86=_tmpE85->f1;}}_LL761: _tmpE88=_tmpE86;goto _LL763;_LL762: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpE87=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpE74;if(_tmpE87->tag != 14)goto _LL764;else{_tmpE88=_tmpE87->f1;}}_LL763: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE88);_LL764: {struct Cyc_Absyn_Cast_e_struct*
_tmpE89=(struct Cyc_Absyn_Cast_e_struct*)_tmpE74;if(_tmpE89->tag != 15)goto _LL766;
else{_tmpE8A=(void*)_tmpE89->f1;_tmpE8B=_tmpE89->f2;_tmpE8C=_tmpE89->f4;if(
_tmpE8C != Cyc_Absyn_No_coercion)goto _LL766;}}_LL765: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpE8B);_LL766: {struct Cyc_Absyn_Cast_e_struct*_tmpE8D=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE74;if(_tmpE8D->tag != 15)goto _LL768;else{_tmpE8E=(void*)_tmpE8D->f1;_tmpE8F=
_tmpE8D->f2;}}_LL767: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE8F);_LL768: {
struct Cyc_Absyn_Address_e_struct*_tmpE90=(struct Cyc_Absyn_Address_e_struct*)
_tmpE74;if(_tmpE90->tag != 16)goto _LL76A;else{_tmpE91=_tmpE90->f1;}}_LL769: return
Cyc_Tcutil_cnst_exp(te,1,_tmpE91);_LL76A: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpE92=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpE74;if(_tmpE92->tag != 28)
goto _LL76C;else{_tmpE93=_tmpE92->f2;_tmpE94=_tmpE92->f3;}}_LL76B: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE93) && Cyc_Tcutil_cnst_exp(te,0,_tmpE94);_LL76C: {struct Cyc_Absyn_Array_e_struct*
_tmpE95=(struct Cyc_Absyn_Array_e_struct*)_tmpE74;if(_tmpE95->tag != 27)goto _LL76E;
else{_tmpE96=_tmpE95->f1;}}_LL76D: _tmpE98=_tmpE96;goto _LL76F;_LL76E: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpE97=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpE74;if(_tmpE97->tag != 30)goto
_LL770;else{_tmpE98=_tmpE97->f2;}}_LL76F: _tmpE9A=_tmpE98;goto _LL771;_LL770: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpE99=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpE74;if(_tmpE99->tag != 29)goto _LL772;else{_tmpE9A=_tmpE99->f3;}}_LL771: for(0;
_tmpE9A != 0;_tmpE9A=_tmpE9A->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple31*)
_tmpE9A->hd)).f2))return 0;}return 1;_LL772: {struct Cyc_Absyn_Primop_e_struct*
_tmpE9B=(struct Cyc_Absyn_Primop_e_struct*)_tmpE74;if(_tmpE9B->tag != 3)goto _LL774;
else{_tmpE9C=_tmpE9B->f1;_tmpE9D=_tmpE9B->f2;}}_LL773: _tmpE9F=_tmpE9D;goto _LL775;
_LL774: {struct Cyc_Absyn_Tuple_e_struct*_tmpE9E=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpE74;if(_tmpE9E->tag != 25)goto _LL776;else{_tmpE9F=_tmpE9E->f1;}}_LL775:
_tmpEA1=_tmpE9F;goto _LL777;_LL776: {struct Cyc_Absyn_Datatype_e_struct*_tmpEA0=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpE74;if(_tmpEA0->tag != 31)goto _LL778;else{
_tmpEA1=_tmpEA0->f1;}}_LL777: for(0;_tmpEA1 != 0;_tmpEA1=_tmpEA1->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpEA1->hd))return 0;}return 1;_LL778:;_LL779: return 0;
_LL74D:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpEAA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEB0;struct Cyc_Absyn_PtrAtts _tmpEB1;union Cyc_Absyn_Constraint*
_tmpEB2;union Cyc_Absyn_Constraint*_tmpEB3;struct Cyc_Absyn_ArrayInfo _tmpEB5;void*
_tmpEB6;struct Cyc_List_List*_tmpEB8;struct Cyc_Absyn_AggrInfo _tmpEBA;union Cyc_Absyn_AggrInfoU
_tmpEBB;struct Cyc_List_List*_tmpEBC;struct Cyc_List_List*_tmpEBE;_LL78B: {struct
Cyc_Absyn_VoidType_struct*_tmpEAB=(struct Cyc_Absyn_VoidType_struct*)_tmpEAA;if(
_tmpEAB->tag != 0)goto _LL78D;}_LL78C: goto _LL78E;_LL78D: {struct Cyc_Absyn_IntType_struct*
_tmpEAC=(struct Cyc_Absyn_IntType_struct*)_tmpEAA;if(_tmpEAC->tag != 6)goto _LL78F;}
_LL78E: goto _LL790;_LL78F: {struct Cyc_Absyn_FloatType_struct*_tmpEAD=(struct Cyc_Absyn_FloatType_struct*)
_tmpEAA;if(_tmpEAD->tag != 7)goto _LL791;}_LL790: goto _LL792;_LL791: {struct Cyc_Absyn_DoubleType_struct*
_tmpEAE=(struct Cyc_Absyn_DoubleType_struct*)_tmpEAA;if(_tmpEAE->tag != 8)goto
_LL793;}_LL792: return 1;_LL793: {struct Cyc_Absyn_PointerType_struct*_tmpEAF=(
struct Cyc_Absyn_PointerType_struct*)_tmpEAA;if(_tmpEAF->tag != 5)goto _LL795;else{
_tmpEB0=_tmpEAF->f1;_tmpEB1=_tmpEB0.ptr_atts;_tmpEB2=_tmpEB1.nullable;_tmpEB3=
_tmpEB1.bounds;}}_LL794: {void*_tmpEC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpEB3);_LL7A4: {
struct Cyc_Absyn_DynEither_b_struct*_tmpEC2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEC1;if(_tmpEC2->tag != 0)goto _LL7A6;}_LL7A5: return 1;_LL7A6:;_LL7A7: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpEB2);_LL7A3:;}
_LL795: {struct Cyc_Absyn_ArrayType_struct*_tmpEB4=(struct Cyc_Absyn_ArrayType_struct*)
_tmpEAA;if(_tmpEB4->tag != 9)goto _LL797;else{_tmpEB5=_tmpEB4->f1;_tmpEB6=_tmpEB5.elt_type;}}
_LL796: return Cyc_Tcutil_supports_default(_tmpEB6);_LL797: {struct Cyc_Absyn_TupleType_struct*
_tmpEB7=(struct Cyc_Absyn_TupleType_struct*)_tmpEAA;if(_tmpEB7->tag != 11)goto
_LL799;else{_tmpEB8=_tmpEB7->f1;}}_LL798: for(0;_tmpEB8 != 0;_tmpEB8=_tmpEB8->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpEB8->hd)).f2))return 0;}
return 1;_LL799: {struct Cyc_Absyn_AggrType_struct*_tmpEB9=(struct Cyc_Absyn_AggrType_struct*)
_tmpEAA;if(_tmpEB9->tag != 12)goto _LL79B;else{_tmpEBA=_tmpEB9->f1;_tmpEBB=_tmpEBA.aggr_info;
_tmpEBC=_tmpEBA.targs;}}_LL79A: {struct Cyc_Absyn_Aggrdecl*_tmpEC3=Cyc_Absyn_get_known_aggrdecl(
_tmpEBB);if(_tmpEC3->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpEC3->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpEC3->tvs,_tmpEBC,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEC3->impl))->fields);}
_LL79B: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEBD=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpEAA;if(_tmpEBD->tag != 13)goto _LL79D;else{_tmpEBE=_tmpEBD->f2;}}_LL79C: return
Cyc_Tcutil_fields_support_default(0,0,_tmpEBE);_LL79D: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpEBF=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpEAA;if(_tmpEBF->tag != 15)goto
_LL79F;}_LL79E: goto _LL7A0;_LL79F: {struct Cyc_Absyn_EnumType_struct*_tmpEC0=(
struct Cyc_Absyn_EnumType_struct*)_tmpEAA;if(_tmpEC0->tag != 14)goto _LL7A1;}_LL7A0:
return 1;_LL7A1:;_LL7A2: return 0;_LL78A:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpEC4=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpEC4;_push_region(rgn);{struct Cyc_List_List*_tmpEC5=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpEC5,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpEC6=0;_npop_handler(0);return _tmpEC6;}}}{
int _tmpEC7=1;_npop_handler(0);return _tmpEC7;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpEC8=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpECD;struct Cyc_Absyn_PtrAtts _tmpECE;union Cyc_Absyn_Constraint*
_tmpECF;union Cyc_Absyn_Constraint*_tmpED0;_LL7A9: {struct Cyc_Absyn_IntType_struct*
_tmpEC9=(struct Cyc_Absyn_IntType_struct*)_tmpEC8;if(_tmpEC9->tag != 6)goto _LL7AB;}
_LL7AA: goto _LL7AC;_LL7AB: {struct Cyc_Absyn_FloatType_struct*_tmpECA=(struct Cyc_Absyn_FloatType_struct*)
_tmpEC8;if(_tmpECA->tag != 7)goto _LL7AD;}_LL7AC: goto _LL7AE;_LL7AD: {struct Cyc_Absyn_DoubleType_struct*
_tmpECB=(struct Cyc_Absyn_DoubleType_struct*)_tmpEC8;if(_tmpECB->tag != 8)goto
_LL7AF;}_LL7AE: return 1;_LL7AF: {struct Cyc_Absyn_PointerType_struct*_tmpECC=(
struct Cyc_Absyn_PointerType_struct*)_tmpEC8;if(_tmpECC->tag != 5)goto _LL7B1;else{
_tmpECD=_tmpECC->f1;_tmpECE=_tmpECD.ptr_atts;_tmpECF=_tmpECE.nullable;_tmpED0=
_tmpECE.bounds;}}_LL7B0: {void*_tmpED1=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpED0);_LL7B4: {
struct Cyc_Absyn_DynEither_b_struct*_tmpED2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpED1;if(_tmpED2->tag != 0)goto _LL7B6;}_LL7B5: return 0;_LL7B6:;_LL7B7: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpECF);_LL7B3:;}
_LL7B1:;_LL7B2: return 0;_LL7A8:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpED3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpED5;void*
_tmpED6;struct Cyc_Absyn_FnInfo _tmpED8;struct Cyc_List_List*_tmpED9;_LL7B9: {
struct Cyc_Absyn_PointerType_struct*_tmpED4=(struct Cyc_Absyn_PointerType_struct*)
_tmpED3;if(_tmpED4->tag != 5)goto _LL7BB;else{_tmpED5=_tmpED4->f1;_tmpED6=_tmpED5.elt_typ;}}
_LL7BA: return Cyc_Tcutil_is_noreturn(_tmpED6);_LL7BB: {struct Cyc_Absyn_FnType_struct*
_tmpED7=(struct Cyc_Absyn_FnType_struct*)_tmpED3;if(_tmpED7->tag != 10)goto _LL7BD;
else{_tmpED8=_tmpED7->f1;_tmpED9=_tmpED8.attributes;}}_LL7BC: for(0;_tmpED9 != 0;
_tmpED9=_tmpED9->tl){void*_tmpEDA=(void*)_tmpED9->hd;_LL7C0: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpEDB=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpEDA;if(_tmpEDB->tag != 4)goto
_LL7C2;}_LL7C1: return 1;_LL7C2:;_LL7C3: continue;_LL7BF:;}goto _LL7B8;_LL7BD:;
_LL7BE: goto _LL7B8;_LL7B8:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpEDC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpEDE;struct Cyc_List_List*_tmpEDF;struct Cyc_List_List**_tmpEE0;_LL7C5: {struct
Cyc_Absyn_FnType_struct*_tmpEDD=(struct Cyc_Absyn_FnType_struct*)_tmpEDC;if(
_tmpEDD->tag != 10)goto _LL7C7;else{_tmpEDE=_tmpEDD->f1;_tmpEDF=_tmpEDE.attributes;
_tmpEE0=(struct Cyc_List_List**)&(_tmpEDD->f1).attributes;}}_LL7C6: {struct Cyc_List_List*
_tmpEE1=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1428;*_tmpEE0=((_tmp1428=_cycalloc(sizeof(*_tmp1428)),((
_tmp1428->hd=(void*)atts->hd,((_tmp1428->tl=*_tmpEE0,_tmp1428))))));}else{struct
Cyc_List_List*_tmp1429;_tmpEE1=((_tmp1429=_cycalloc(sizeof(*_tmp1429)),((
_tmp1429->hd=(void*)atts->hd,((_tmp1429->tl=_tmpEE1,_tmp1429))))));}}return
_tmpEE1;}_LL7C7:;_LL7C8: {const char*_tmp142C;void*_tmp142B;(_tmp142B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142C="transfer_fn_type_atts",
_tag_dyneither(_tmp142C,sizeof(char),22))),_tag_dyneither(_tmp142B,sizeof(void*),
0)));}_LL7C4:;}
