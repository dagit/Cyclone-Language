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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;int Cyc_Tcutil_unify(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple9{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);struct _tuple10{unsigned int f1;
int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int
Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};enum Cyc_CfFlowInfo_InitLevel{
Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop
cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
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
x,struct _tuple13*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple13*x,struct _tuple13*y){
struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple13 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct _dyneither_ptr _tmp5;struct _tuple13
_tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(
i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(struct _dyneither_ptr)
_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple0*q,void*(*type_maker)(struct _tuple0*));void*Cyc_Toc_aggrdecl_type(
struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple14 _tmpC;void*_tmpD;struct _tuple14*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap);static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA35;void*_tmpA34;(_tmpA34=0,Cyc_fprintf(Cyc_stderr,((
_tmpA35="\n",_tag_dyneither(_tmpA35,sizeof(char),2))),_tag_dyneither(_tmpA34,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA38;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA37;(int)_throw((void*)((
_tmpA37=_cycalloc_atomic(sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA38.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA38)),_tmpA37)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA3B;void*
_tmpA3A;(_tmpA3A=0,Cyc_fprintf(Cyc_stderr,((_tmpA3B="\n",_tag_dyneither(_tmpA3B,
sizeof(char),2))),_tag_dyneither(_tmpA3A,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA3E;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA3D;(int)_throw((void*)((_tmpA3D=_cycalloc_atomic(sizeof(*_tmpA3D)),((
_tmpA3D[0]=((_tmpA3E.tag=Cyc_Toc_Toc_Impossible,_tmpA3E)),_tmpA3D)))));};}char
Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{char*tag;
};static char _tmp17[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={
_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp18[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,
_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp19[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,
_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp1A[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp1D[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1F[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,
_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={3,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
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
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA3F;skip_stmt_opt=((_tmpA3F=
_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=Cyc_Absyn_skip_stmt(0),_tmpA3F))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA42;struct Cyc_Absyn_Cast_e_struct*_tmpA41;
return(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=15,((
_tmpA42.f1=(void*)t,((_tmpA42.f2=e,((_tmpA42.f3=0,((_tmpA42.f4=Cyc_Absyn_No_coercion,
_tmpA42)))))))))),_tmpA41))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA45;struct Cyc_Absyn_Deref_e_struct*_tmpA44;return(void*)((_tmpA44=_cycalloc(
sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=21,((_tmpA45.f1=e,_tmpA45)))),
_tmpA44))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA48;struct Cyc_Absyn_Subscript_e_struct*
_tmpA47;return(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((
_tmpA48.tag=24,((_tmpA48.f1=e1,((_tmpA48.f2=e2,_tmpA48)))))),_tmpA47))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA4B;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA4A;return(void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((
_tmpA4B.tag=37,((_tmpA4B.f1=s,_tmpA4B)))),_tmpA4A))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA4E;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4D;return(void*)((_tmpA4D=
_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=18,((_tmpA4E.f1=(void*)t,
_tmpA4E)))),_tmpA4D))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA51;struct Cyc_Absyn_FnCall_e_struct*
_tmpA50;return(void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((
_tmpA51.tag=11,((_tmpA51.f1=e,((_tmpA51.f2=es,((_tmpA51.f3=0,_tmpA51)))))))),
_tmpA50))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA54;struct Cyc_Absyn_Exp_s_struct*
_tmpA53;return(void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((
_tmpA54.tag=1,((_tmpA54.f1=e,_tmpA54)))),_tmpA53))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA57;struct Cyc_Absyn_Seq_s_struct*_tmpA56;return(void*)((_tmpA56=_cycalloc(
sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=2,((_tmpA57.f1=s1,((_tmpA57.f2=s2,
_tmpA57)))))),_tmpA56))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA5A;struct Cyc_Absyn_Conditional_e_struct*_tmpA59;return(void*)((_tmpA59=
_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=6,((_tmpA5A.f1=e1,((
_tmpA5A.f2=e2,((_tmpA5A.f3=e3,_tmpA5A)))))))),_tmpA59))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA5D;struct Cyc_Absyn_AggrMember_e_struct*_tmpA5C;return(void*)((_tmpA5C=
_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=22,((_tmpA5D.f1=e,((
_tmpA5D.f2=n,((_tmpA5D.f3=0,((_tmpA5D.f4=0,_tmpA5D)))))))))),_tmpA5C))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA60;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA5F;return(void*)((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F[0]=((
_tmpA60.tag=23,((_tmpA60.f1=e,((_tmpA60.f2=n,((_tmpA60.f3=0,((_tmpA60.f4=0,
_tmpA60)))))))))),_tmpA5F))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA63;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA62;return(void*)((_tmpA62=_cycalloc(
sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=36,((_tmpA63.f1=tdopt,((_tmpA63.f2=
ds,_tmpA63)))))),_tmpA62))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA66;
struct Cyc_Absyn_Goto_s_struct*_tmpA65;return(void*)((_tmpA65=_cycalloc(sizeof(*
_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=8,((_tmpA66.f1=v,((_tmpA66.f2=s,_tmpA66)))))),
_tmpA65))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;
struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;
struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*
fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=
Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE: {struct Cyc_Core_Impossible_struct
_tmpA6C;const char*_tmpA6B;struct Cyc_Core_Impossible_struct*_tmpA6A;(int)_throw((
void*)((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6C.tag=Cyc_Core_Impossible,((
_tmpA6C.f1=((_tmpA6B="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA6B,sizeof(char),44))),_tmpA6C)))),_tmpA6A)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA79;const char*_tmpA78;void*_tmpA77[1];
struct Cyc_String_pa_struct _tmpA76;struct Cyc_Core_Impossible_struct*_tmpA75;(int)
_throw((void*)((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75[0]=((_tmpA79.tag=
Cyc_Core_Impossible,((_tmpA79.f1=(struct _dyneither_ptr)((_tmpA76.tag=0,((_tmpA76.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA77[
0]=& _tmpA76,Cyc_aprintf(((_tmpA78="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA78,sizeof(char),67))),_tag_dyneither(_tmpA77,sizeof(void*),1)))))))),
_tmpA79)))),_tmpA75)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmpD3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD5;void*_tmpD6;_LL14: {
struct Cyc_Absyn_PointerType_struct*_tmpD4=(struct Cyc_Absyn_PointerType_struct*)
_tmpD3;if(_tmpD4->tag != 5)goto _LL16;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD5.elt_typ;}}
_LL15: return Cyc_Toc_getFunctionType(fS,_tmpD6);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct
_tmpA7F;const char*_tmpA7E;struct Cyc_Core_Impossible_struct*_tmpA7D;(int)_throw((
void*)((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7F.tag=Cyc_Core_Impossible,((
_tmpA7F.f1=((_tmpA7E="impossible type (not pointer)",_tag_dyneither(_tmpA7E,
sizeof(char),30))),_tmpA7F)))),_tmpA7D)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmpDA=e->r;union Cyc_Absyn_Cnst
_tmpDC;struct _tuple3 _tmpDD;char _tmpDE;union Cyc_Absyn_Cnst _tmpE0;struct _tuple4
_tmpE1;short _tmpE2;union Cyc_Absyn_Cnst _tmpE4;struct _tuple5 _tmpE5;int _tmpE6;union
Cyc_Absyn_Cnst _tmpE8;struct _tuple6 _tmpE9;long long _tmpEA;union Cyc_Absyn_Cnst
_tmpEC;int _tmpED;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF1;struct Cyc_List_List*
_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*
_tmpF9;_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpDB=(struct Cyc_Absyn_Const_e_struct*)
_tmpDA;if(_tmpDB->tag != 0)goto _LL1B;else{_tmpDC=_tmpDB->f1;if((_tmpDC.Char_c).tag
!= 2)goto _LL1B;_tmpDD=(struct _tuple3)(_tmpDC.Char_c).val;_tmpDE=_tmpDD.f2;}}
_LL1A: return _tmpDE == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpDF=(
struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpDF->tag != 0)goto _LL1D;else{_tmpE0=
_tmpDF->f1;if((_tmpE0.Short_c).tag != 3)goto _LL1D;_tmpE1=(struct _tuple4)(_tmpE0.Short_c).val;
_tmpE2=_tmpE1.f2;}}_LL1C: return _tmpE2 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE3=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpE3->tag != 0)goto _LL1F;
else{_tmpE4=_tmpE3->f1;if((_tmpE4.Int_c).tag != 4)goto _LL1F;_tmpE5=(struct _tuple5)(
_tmpE4.Int_c).val;_tmpE6=_tmpE5.f2;}}_LL1E: return _tmpE6 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE7=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpE7->tag != 0)goto _LL21;
else{_tmpE8=_tmpE7->f1;if((_tmpE8.LongLong_c).tag != 5)goto _LL21;_tmpE9=(struct
_tuple6)(_tmpE8.LongLong_c).val;_tmpEA=_tmpE9.f2;}}_LL20: return _tmpEA == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpEB=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;
if(_tmpEB->tag != 0)goto _LL23;else{_tmpEC=_tmpEB->f1;if((_tmpEC.Null_c).tag != 1)
goto _LL23;_tmpED=(int)(_tmpEC.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpEE=(struct Cyc_Absyn_Cast_e_struct*)_tmpDA;if(_tmpEE->tag != 15)goto _LL25;
else{_tmpEF=_tmpEE->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpEF);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF0=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDA;if(_tmpF0->tag != 25)goto _LL27;
else{_tmpF1=_tmpF0->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF1);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF2=(struct Cyc_Absyn_Array_e_struct*)_tmpDA;if(
_tmpF2->tag != 27)goto _LL29;else{_tmpF3=_tmpF2->f1;}}_LL28: _tmpF5=_tmpF3;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF4=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDA;if(_tmpF4->tag != 29)goto _LL2B;else{_tmpF5=_tmpF4->f3;}}_LL2A: _tmpF7=
_tmpF5;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF6=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDA;if(_tmpF6->tag != 26)goto _LL2D;else{_tmpF7=_tmpF6->f2;}}_LL2C: _tmpF9=
_tmpF7;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF8=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDA;if(_tmpF8->tag != 36)goto _LL2F;else{
_tmpF9=_tmpF8->f2;}}_LL2E: for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpF9->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t);static int Cyc_Toc_is_nullable(void*t){void*
_tmpFA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFC;struct Cyc_Absyn_PtrAtts
_tmpFD;union Cyc_Absyn_Constraint*_tmpFE;_LL32: {struct Cyc_Absyn_PointerType_struct*
_tmpFB=(struct Cyc_Absyn_PointerType_struct*)_tmpFA;if(_tmpFB->tag != 5)goto _LL34;
else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFC.ptr_atts;_tmpFE=_tmpFD.nullable;}}_LL33:
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFE);
_LL34:;_LL35: {const char*_tmpA82;void*_tmpA81;(_tmpA81=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA82="is_nullable",
_tag_dyneither(_tmpA82,sizeof(char),12))),_tag_dyneither(_tmpA81,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA83;pair=((_tmpA83.f1=x,((_tmpA83.f2=tag,_tmpA83))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA84;struct _tuple13*_tmp108=(_tmpA84=_cycalloc(sizeof(*_tmpA84)),((
_tmpA84->f1=x,((_tmpA84->f2=tag,_tmpA84)))));struct _dyneither_ptr*_tmpA87;struct
_tuple0*_tmpA86;struct _tuple0*res=(_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->f1=(*
x).f1,((_tmpA86->f2=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA87)))),_tmpA86)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp103,(
struct _tuple13*)_tmp108,res);{struct _tuple0*_tmp109=res;_npop_handler(0);return
_tmp109;};};};_pop_dynregion(d);};};}struct _tuple16{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp110;struct Cyc_List_List**
_tmp111;struct Cyc_Toc_TocState _tmp10F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp110=_tmp10F.dyn;_tmp111=_tmp10F.tuple_types;{struct _DynRegionFrame _tmp112;
struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp110);{struct Cyc_List_List*
_tmp113=*_tmp111;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){struct _tuple16 _tmp115;
void*_tmp116;struct Cyc_List_List*_tmp117;struct _tuple16*_tmp114=(struct _tuple16*)
_tmp113->hd;_tmp115=*_tmp114;_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;{struct Cyc_List_List*
_tmp118=tqs0;for(0;_tmp118 != 0  && _tmp117 != 0;(_tmp118=_tmp118->tl,_tmp117=
_tmp117->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp118->hd)).f2,(void*)
_tmp117->hd))break;}if(_tmp118 == 0  && _tmp117 == 0){void*_tmp119=_tmp116;
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA8F;void*_tmpA8E[
1];const char*_tmpA8D;struct _dyneither_ptr*_tmpA8C;struct _dyneither_ptr*xname=(
_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=(struct _dyneither_ptr)((_tmpA8F.tag=
1,((_tmpA8F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA8E[0]=& _tmpA8F,
Cyc_aprintf(((_tmpA8D="_tuple%d",_tag_dyneither(_tmpA8D,sizeof(char),9))),
_tag_dyneither(_tmpA8E,sizeof(void*),1)))))))),_tmpA8C)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA92;struct Cyc_List_List*
_tmpA91;_tmp11A=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->hd=((_tmpA92=
_cycalloc(sizeof(*_tmpA92)),((_tmpA92->name=Cyc_Absyn_fieldname(i),((_tmpA92->tq=
Cyc_Toc_mt_tq,((_tmpA92->type=(void*)ts2->hd,((_tmpA92->width=0,((_tmpA92->attributes=
0,_tmpA92)))))))))))),((_tmpA91->tl=_tmp11A,_tmpA91))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA97;struct _tuple0*_tmpA96;struct Cyc_Absyn_Aggrdecl*_tmpA95;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->kind=Cyc_Absyn_StructA,((
_tmpA95->sc=Cyc_Absyn_Public,((_tmpA95->name=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((
_tmpA96->f1=Cyc_Absyn_Rel_n(0),((_tmpA96->f2=xname,_tmpA96)))))),((_tmpA95->tvs=
0,((_tmpA95->impl=((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->exist_vars=0,((
_tmpA97->rgn_po=0,((_tmpA97->fields=_tmp11A,((_tmpA97->tagged=0,_tmpA97)))))))))),((
_tmpA95->attributes=0,_tmpA95)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA9D;struct Cyc_Absyn_Aggr_d_struct _tmpA9C;struct Cyc_List_List*_tmpA9B;Cyc_Toc_result_decls=((
_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=Cyc_Absyn_new_decl((void*)((
_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9C.tag=6,((_tmpA9C.f1=
_tmp11D,_tmpA9C)))),_tmpA9D)))),0),((_tmpA9B->tl=Cyc_Toc_result_decls,_tmpA9B))))));}{
struct _tuple16*_tmpAA0;struct Cyc_List_List*_tmpA9F;*_tmp111=((_tmpA9F=
_region_malloc(d,sizeof(*_tmpA9F)),((_tmpA9F->hd=((_tmpAA0=_region_malloc(d,
sizeof(*_tmpAA0)),((_tmpAA0->f1=x,((_tmpAA0->f2=ts,_tmpAA0)))))),((_tmpA9F->tl=*
_tmp111,_tmpA9F))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpAAD;const char*
_tmpAAC;void*_tmpAAB[1];struct Cyc_Int_pa_struct _tmpAAA;struct _tuple0*_tmpAA9;
struct _tuple0*res=(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->f1=Cyc_Absyn_Loc_n,((
_tmpAA9->f2=((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=(struct
_dyneither_ptr)((_tmpAAA.tag=1,((_tmpAAA.f1=(unsigned int)_tmp12B,((_tmpAAB[0]=&
_tmpAAA,Cyc_aprintf(((_tmpAAC="_tmp%X",_tag_dyneither(_tmpAAC,sizeof(char),7))),
_tag_dyneither(_tmpAAB,sizeof(void*),1)))))))),_tmpAAD)))),_tmpAA9)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAB5;void*_tmpAB4[1];const char*_tmpAB3;struct _dyneither_ptr*_tmpAB2;struct
_dyneither_ptr*res=(_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=(struct
_dyneither_ptr)((_tmpAB5.tag=1,((_tmpAB5.f1=(unsigned int)_tmp135,((_tmpAB4[0]=&
_tmpAB5,Cyc_aprintf(((_tmpAB3="_LL%X",_tag_dyneither(_tmpAB3,sizeof(char),6))),
_tag_dyneither(_tmpAB4,sizeof(void*),1)))))))),_tmpAB2)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAB8;void*_tmpAB7;(_tmpAB7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB8="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAB8,sizeof(char),43))),_tag_dyneither(_tmpAB7,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp139=res;_npop_handler(0);return _tmp139;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp13E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13E))->hd)->name)
!= 0){++ ans;_tmp13E=_tmp13E->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a);static struct _tuple7*Cyc_Toc_arg_to_c(struct _tuple7*a){struct Cyc_Core_Opt*
_tmp140;struct Cyc_Absyn_Tqual _tmp141;void*_tmp142;struct _tuple7 _tmp13F=*a;
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAB9;
return(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->f1=_tmp140,((_tmpAB9->f2=
_tmp141,((_tmpAB9->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAB9)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpABA;return(_tmpABA=
_cycalloc(sizeof(*_tmpABA)),((_tmpABA->f1=_tmp145,((_tmpABA->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpABA)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
Cyc_Toc_typ_to_c_array(void*t){void*_tmp148=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp14A;void*_tmp14B;struct Cyc_Absyn_Tqual _tmp14C;struct Cyc_Absyn_Exp*_tmp14D;
union Cyc_Absyn_Constraint*_tmp14E;struct Cyc_Position_Segment*_tmp14F;struct Cyc_Core_Opt*
_tmp151;struct Cyc_Core_Opt _tmp152;void*_tmp153;_LL37: {struct Cyc_Absyn_ArrayType_struct*
_tmp149=(struct Cyc_Absyn_ArrayType_struct*)_tmp148;if(_tmp149->tag != 9)goto _LL39;
else{_tmp14A=_tmp149->f1;_tmp14B=_tmp14A.elt_type;_tmp14C=_tmp14A.tq;_tmp14D=
_tmp14A.num_elts;_tmp14E=_tmp14A.zero_term;_tmp14F=_tmp14A.zt_loc;}}_LL38: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp14B),_tmp14C,_tmp14D,Cyc_Absyn_false_conref,
_tmp14F);_LL39: {struct Cyc_Absyn_Evar_struct*_tmp150=(struct Cyc_Absyn_Evar_struct*)
_tmp148;if(_tmp150->tag != 1)goto _LL3B;else{_tmp151=_tmp150->f2;if(_tmp151 == 0)
goto _LL3B;_tmp152=*_tmp151;_tmp153=(void*)_tmp152.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(
_tmp153);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(t);_LL36:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpABB;return(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->name=f->name,((
_tmpABB->tq=Cyc_Toc_mt_tq,((_tmpABB->type=Cyc_Toc_typ_to_c(f->type),((_tmpABB->width=
f->width,((_tmpABB->attributes=f->attributes,_tmpABB)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpABC;cs=((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((
_tmpABC[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpABC))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABD;r=((_tmpABD=
_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABD))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABE;r=((_tmpABE=
_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABE))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
void*Cyc_Toc_typ_to_c(void*t){void*_tmp158=t;struct Cyc_Core_Opt*_tmp15B;struct
Cyc_Core_Opt*_tmp15D;struct Cyc_Core_Opt _tmp15E;void*_tmp15F;struct Cyc_Absyn_Tvar*
_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp164;union Cyc_Absyn_DatatypeFieldInfoU
_tmp165;struct _tuple1 _tmp166;struct Cyc_Absyn_Datatypedecl*_tmp167;struct Cyc_Absyn_Datatypefield*
_tmp168;struct Cyc_Absyn_PtrInfo _tmp16B;void*_tmp16C;struct Cyc_Absyn_Tqual _tmp16D;
struct Cyc_Absyn_PtrAtts _tmp16E;union Cyc_Absyn_Constraint*_tmp16F;struct Cyc_Absyn_ArrayInfo
_tmp174;void*_tmp175;struct Cyc_Absyn_Tqual _tmp176;struct Cyc_Absyn_Exp*_tmp177;
struct Cyc_Position_Segment*_tmp178;struct Cyc_Absyn_FnInfo _tmp17A;void*_tmp17B;
struct Cyc_List_List*_tmp17C;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17E;struct
Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;enum Cyc_Absyn_AggrKind _tmp183;
struct Cyc_List_List*_tmp184;struct Cyc_Absyn_AggrInfo _tmp186;union Cyc_Absyn_AggrInfoU
_tmp187;struct Cyc_List_List*_tmp188;struct _tuple0*_tmp18A;struct Cyc_List_List*
_tmp18C;struct _tuple0*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Typedefdecl*
_tmp190;void**_tmp191;void*_tmp194;_LL3E: {struct Cyc_Absyn_VoidType_struct*
_tmp159=(struct Cyc_Absyn_VoidType_struct*)_tmp158;if(_tmp159->tag != 0)goto _LL40;}
_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*_tmp15A=(struct Cyc_Absyn_Evar_struct*)
_tmp158;if(_tmp15A->tag != 1)goto _LL42;else{_tmp15B=_tmp15A->f2;if(_tmp15B != 0)
goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;_LL42: {struct Cyc_Absyn_Evar_struct*
_tmp15C=(struct Cyc_Absyn_Evar_struct*)_tmp158;if(_tmp15C->tag != 1)goto _LL44;
else{_tmp15D=_tmp15C->f2;if(_tmp15D == 0)goto _LL44;_tmp15E=*_tmp15D;_tmp15F=(void*)
_tmp15E.v;}}_LL43: return Cyc_Toc_typ_to_c(_tmp15F);_LL44: {struct Cyc_Absyn_VarType_struct*
_tmp160=(struct Cyc_Absyn_VarType_struct*)_tmp158;if(_tmp160->tag != 2)goto _LL46;
else{_tmp161=_tmp160->f1;}}_LL45: if((Cyc_Tcutil_tvar_kind(_tmp161,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp158;if(_tmp162->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp163=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp158;if(_tmp163->tag != 4)goto _LL4A;else{_tmp164=_tmp163->f1;_tmp165=_tmp164.field_info;
if((_tmp165.KnownDatatypefield).tag != 2)goto _LL4A;_tmp166=(struct _tuple1)(
_tmp165.KnownDatatypefield).val;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;}}_LL49: {
const char*_tmpABF;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpABF="_struct",_tag_dyneither(_tmpABF,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAC2;void*_tmpAC1;(_tmpAC1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC2="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAC2,sizeof(char),29))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16A=(struct Cyc_Absyn_PointerType_struct*)
_tmp158;if(_tmp16A->tag != 5)goto _LL4E;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.elt_typ;
_tmp16D=_tmp16B.elt_tq;_tmp16E=_tmp16B.ptr_atts;_tmp16F=_tmp16E.bounds;}}_LL4D:
_tmp16C=Cyc_Toc_typ_to_c_array(_tmp16C);{void*_tmp19F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16F);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1A0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19F;if(_tmp1A0->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16C,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16D,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp170=(struct Cyc_Absyn_IntType_struct*)_tmp158;if(_tmp170->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp171=(struct Cyc_Absyn_FloatType_struct*)
_tmp158;if(_tmp171->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp172=(struct Cyc_Absyn_DoubleType_struct*)_tmp158;if(_tmp172->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp173=(struct Cyc_Absyn_ArrayType_struct*)
_tmp158;if(_tmp173->tag != 9)goto _LL56;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.elt_type;
_tmp176=_tmp174.tq;_tmp177=_tmp174.num_elts;_tmp178=_tmp174.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp175),_tmp176,_tmp177,Cyc_Absyn_false_conref,
_tmp178);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp179=(struct Cyc_Absyn_FnType_struct*)
_tmp158;if(_tmp179->tag != 10)goto _LL58;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.ret_typ;
_tmp17C=_tmp17A.args;_tmp17D=_tmp17A.c_varargs;_tmp17E=_tmp17A.cyc_varargs;
_tmp17F=_tmp17A.attributes;}}_LL57: {struct Cyc_List_List*_tmp1A1=0;for(0;_tmp17F
!= 0;_tmp17F=_tmp17F->tl){void*_tmp1A2=(void*)_tmp17F->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A3=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A2;if(_tmp1A3->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A4=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A2;if(_tmp1A4->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A5=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A2;if(_tmp1A5->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1A6=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1A2;if(_tmp1A6->tag != 20)
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAC3;_tmp1A1=((
_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->hd=(void*)_tmp17F->hd,((_tmpAC3->tl=
_tmp1A1,_tmpAC3))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A8=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1A9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAC4;struct _tuple7*_tmp1AA=(_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((
_tmpAC4->f1=_tmp17E->name,((_tmpAC4->f2=_tmp17E->tq,((_tmpAC4->f3=_tmp1A9,
_tmpAC4)))))));struct Cyc_List_List*_tmpAC5;_tmp1A8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A8,((
_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->hd=_tmp1AA,((_tmpAC5->tl=0,
_tmpAC5)))))));}{struct Cyc_Absyn_FnType_struct _tmpACB;struct Cyc_Absyn_FnInfo
_tmpACA;struct Cyc_Absyn_FnType_struct*_tmpAC9;return(void*)((_tmpAC9=_cycalloc(
sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACB.tag=10,((_tmpACB.f1=((_tmpACA.tvars=0,((
_tmpACA.effect=0,((_tmpACA.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpACA.args=
_tmp1A8,((_tmpACA.c_varargs=_tmp17D,((_tmpACA.cyc_varargs=0,((_tmpACA.rgn_po=0,((
_tmpACA.attributes=_tmp1A1,_tmpACA)))))))))))))))),_tmpACB)))),_tmpAC9))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpACE;struct Cyc_Absyn_AnonAggrType_struct*
_tmpACD;return(void*)((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=((
_tmpACE.tag=13,((_tmpACE.f1=_tmp183,((_tmpACE.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpACE)))))),_tmpACD))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp185=(struct Cyc_Absyn_AggrType_struct*)
_tmp158;if(_tmp185->tag != 12)goto _LL5E;else{_tmp186=_tmp185->f1;_tmp187=_tmp186.aggr_info;
_tmp188=_tmp186.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B2=_tmp187;struct
_tuple2 _tmp1B3;_LL87: if((_tmp1B2.UnknownAggr).tag != 1)goto _LL89;_tmp1B3=(struct
_tuple2)(_tmp1B2.UnknownAggr).val;_LL88: return t;_LL89:;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B4=Cyc_Absyn_get_known_aggrdecl(_tmp187);if(
_tmp1B4->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B4->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B4->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp189=(struct Cyc_Absyn_EnumType_struct*)
_tmp158;if(_tmp189->tag != 14)goto _LL60;else{_tmp18A=_tmp189->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18B=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp158;if(_tmp18B->tag != 15)goto _LL62;else{_tmp18C=_tmp18B->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18C);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp158;if(_tmp18D->tag != 18)goto _LL64;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;
_tmp190=_tmp18D->f3;_tmp191=_tmp18D->f4;}}_LL63: if(_tmp191 == 0  || Cyc_noexpand_r){
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpAD1;struct Cyc_Absyn_TypedefType_struct*
_tmpAD0;return(void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((
_tmpAD1.tag=18,((_tmpAD1.f1=_tmp18E,((_tmpAD1.f2=0,((_tmpAD1.f3=_tmp190,((
_tmpAD1.f4=0,_tmpAD1)))))))))),_tmpAD0))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAD7;void**_tmpAD6;struct Cyc_Absyn_TypedefType_struct*_tmpAD5;return(void*)((
_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD7.tag=18,((_tmpAD7.f1=
_tmp18E,((_tmpAD7.f2=0,((_tmpAD7.f3=_tmp190,((_tmpAD7.f4=((_tmpAD6=_cycalloc(
sizeof(*_tmpAD6)),((_tmpAD6[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAD6)))),
_tmpAD7)))))))))),_tmpAD5))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpADA;void*_tmpAD9;(_tmpAD9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADA="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpADA,sizeof(char),55))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpADD;void*_tmpADC;(
_tmpADC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADD="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpADD,sizeof(char),57))),_tag_dyneither(_tmpADC,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpAE0;void*_tmpADF;(
_tmpADF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE0="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAE0,sizeof(char),49))),_tag_dyneither(_tmpADF,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAE3;void*_tmpAE2;(
_tmpAE2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE3="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAE3,sizeof(char),51))),_tag_dyneither(_tmpAE2,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C2=t;struct
Cyc_Absyn_ArrayInfo _tmp1C4;void*_tmp1C5;struct Cyc_Absyn_Tqual _tmp1C6;_LL8C: {
struct Cyc_Absyn_ArrayType_struct*_tmp1C3=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C2;if(_tmp1C3->tag != 9)goto _LL8E;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C4.elt_type;
_tmp1C6=_tmp1C4.tq;}}_LL8D: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C5,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1C6,Cyc_Absyn_false_conref),e);_LL8E:;_LL8F:
return Cyc_Toc_cast_it(t,e);_LL8B:;}static int Cyc_Toc_atomic_typ(void*t);static int
Cyc_Toc_atomic_typ(void*t){void*_tmp1C7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp1D2;void*_tmp1D3;struct Cyc_Absyn_AggrInfo _tmp1D5;union Cyc_Absyn_AggrInfoU
_tmp1D6;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_DatatypeFieldInfo _tmp1DA;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1DB;struct _tuple1 _tmp1DC;struct Cyc_Absyn_Datatypedecl*
_tmp1DD;struct Cyc_Absyn_Datatypefield*_tmp1DE;struct Cyc_List_List*_tmp1E0;_LL91: {
struct Cyc_Absyn_VoidType_struct*_tmp1C8=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C7;if(_tmp1C8->tag != 0)goto _LL93;}_LL92: return 1;_LL93: {struct Cyc_Absyn_VarType_struct*
_tmp1C9=(struct Cyc_Absyn_VarType_struct*)_tmp1C7;if(_tmp1C9->tag != 2)goto _LL95;}
_LL94: return 0;_LL95: {struct Cyc_Absyn_IntType_struct*_tmp1CA=(struct Cyc_Absyn_IntType_struct*)
_tmp1C7;if(_tmp1CA->tag != 6)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_EnumType_struct*
_tmp1CB=(struct Cyc_Absyn_EnumType_struct*)_tmp1C7;if(_tmp1CB->tag != 14)goto _LL99;}
_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CC=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C7;if(_tmp1CC->tag != 15)goto _LL9B;}_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_FloatType_struct*
_tmp1CD=(struct Cyc_Absyn_FloatType_struct*)_tmp1C7;if(_tmp1CD->tag != 7)goto _LL9D;}
_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_DoubleType_struct*_tmp1CE=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C7;if(_tmp1CE->tag != 8)goto _LL9F;}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_FnType_struct*
_tmp1CF=(struct Cyc_Absyn_FnType_struct*)_tmp1C7;if(_tmp1CF->tag != 10)goto _LLA1;}
_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_TagType_struct*_tmp1D0=(struct Cyc_Absyn_TagType_struct*)
_tmp1C7;if(_tmp1D0->tag != 20)goto _LLA3;}_LLA2: return 1;_LLA3: {struct Cyc_Absyn_ArrayType_struct*
_tmp1D1=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C7;if(_tmp1D1->tag != 9)goto _LLA5;
else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D2.elt_type;}}_LLA4: return Cyc_Toc_atomic_typ(
_tmp1D3);_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp1D4=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C7;if(_tmp1D4->tag != 12)goto _LLA7;else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D5.aggr_info;}}
_LLA6:{union Cyc_Absyn_AggrInfoU _tmp1E5=_tmp1D6;struct _tuple2 _tmp1E6;_LLB8: if((
_tmp1E5.UnknownAggr).tag != 1)goto _LLBA;_tmp1E6=(struct _tuple2)(_tmp1E5.UnknownAggr).val;
_LLB9: return 0;_LLBA:;_LLBB: goto _LLB7;_LLB7:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E7=
Cyc_Absyn_get_known_aggrdecl(_tmp1D6);if(_tmp1E7->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E7->impl))->fields;for(
0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E8->hd)->type))return 0;}}return 1;};_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C7;if(_tmp1D7->tag != 13)goto
_LLA9;else{_tmp1D8=_tmp1D7->f2;}}_LLA8: for(0;_tmp1D8 != 0;_tmp1D8=_tmp1D8->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D8->hd)->type))return 0;}
return 1;_LLA9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1D9=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C7;if(_tmp1D9->tag != 4)goto _LLAB;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1DA.field_info;
if((_tmp1DB.KnownDatatypefield).tag != 2)goto _LLAB;_tmp1DC=(struct _tuple1)(
_tmp1DB.KnownDatatypefield).val;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;}}_LLAA:
_tmp1E0=_tmp1DE->typs;goto _LLAC;_LLAB: {struct Cyc_Absyn_TupleType_struct*_tmp1DF=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C7;if(_tmp1DF->tag != 11)goto _LLAD;else{
_tmp1E0=_tmp1DF->f1;}}_LLAC: for(0;_tmp1E0 != 0;_tmp1E0=_tmp1E0->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1E0->hd)).f2))return 0;}return 1;_LLAD: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1E1=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C7;if(_tmp1E1->tag != 3)goto
_LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_PointerType_struct*_tmp1E2=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C7;if(_tmp1E2->tag != 5)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E3=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C7;if(_tmp1E3->tag != 17)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E4=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C7;if(_tmp1E4->tag != 16)goto
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_String_pa_struct
_tmpAE5;(_tmpAE5.tag=0,((_tmpAE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAE6[0]=& _tmpAE5,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE7="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAE7,sizeof(char),25))),_tag_dyneither(_tmpAE6,sizeof(void*),1)))))));}
_LL90:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1EC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EE;void*
_tmp1EF;_LLBD: {struct Cyc_Absyn_PointerType_struct*_tmp1ED=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EC;if(_tmp1ED->tag != 5)goto _LLBF;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1EE.elt_typ;}}
_LLBE: {void*_tmp1F0=Cyc_Tcutil_compress(_tmp1EF);_LLC2: {struct Cyc_Absyn_VoidType_struct*
_tmp1F1=(struct Cyc_Absyn_VoidType_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LLC4;}
_LLC3: return 1;_LLC4:;_LLC5: return 0;_LLC1:;}_LLBF:;_LLC0: return 0;_LLBC:;}static
int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1F2=Cyc_Tcutil_compress(
t);_LLC7: {struct Cyc_Absyn_VarType_struct*_tmp1F3=(struct Cyc_Absyn_VarType_struct*)
_tmp1F2;if(_tmp1F3->tag != 2)goto _LLC9;}_LLC8: return 1;_LLC9:;_LLCA: return 0;_LLC6:;}
static int Cyc_Toc_is_void_star_or_tvar(void*t);static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp1F4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F6;union Cyc_Absyn_AggrInfoU _tmp1F7;struct Cyc_List_List*_tmp1F9;_LLCC: {
struct Cyc_Absyn_AggrType_struct*_tmp1F5=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F4;if(_tmp1F5->tag != 12)goto _LLCE;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F6.aggr_info;}}
_LLCD: {struct Cyc_Absyn_Aggrdecl*_tmp1FA=Cyc_Absyn_get_known_aggrdecl(_tmp1F7);
if(_tmp1FA->impl == 0){const char*_tmpAEA;void*_tmpAE9;(_tmpAE9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEA="is_poly_field: type missing fields",
_tag_dyneither(_tmpAEA,sizeof(char),35))),_tag_dyneither(_tmpAE9,sizeof(void*),0)));}
_tmp1F9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FA->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F4;if(_tmp1F8->tag != 13)goto _LLD0;else{_tmp1F9=_tmp1F8->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FD=Cyc_Absyn_lookup_field(_tmp1F9,f);if(_tmp1FD == 0){
const char*_tmpAEE;void*_tmpAED[1];struct Cyc_String_pa_struct _tmpAEC;(_tmpAEC.tag=
0,((_tmpAEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAED[0]=&
_tmpAEC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEE="is_poly_field: bad field %s",_tag_dyneither(_tmpAEE,sizeof(char),28))),
_tag_dyneither(_tmpAED,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FD->type);}_LLD0:;_LLD1: {const char*_tmpAF2;void*_tmpAF1[1];struct Cyc_String_pa_struct
_tmpAF0;(_tmpAF0.tag=0,((_tmpAF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAF1[0]=& _tmpAF0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF2="is_poly_field: bad type %s",
_tag_dyneither(_tmpAF2,sizeof(char),27))),_tag_dyneither(_tmpAF1,sizeof(void*),1)))))));}
_LLCB:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp204=e->r;struct Cyc_Absyn_Exp*_tmp206;struct
_dyneither_ptr*_tmp207;struct Cyc_Absyn_Exp*_tmp209;struct _dyneither_ptr*_tmp20A;
_LLD3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp205=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp204;if(_tmp205->tag != 22)goto _LLD5;else{_tmp206=_tmp205->f1;_tmp207=_tmp205->f2;}}
_LLD4: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp206->topt))->v,_tmp207);_LLD5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp208=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp204;if(_tmp208->tag != 23)goto _LLD7;else{
_tmp209=_tmp208->f1;_tmp20A=_tmp208->f2;}}_LLD6: {void*_tmp20B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp209->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20D;void*_tmp20E;_LLDA: {struct Cyc_Absyn_PointerType_struct*_tmp20C=(struct
Cyc_Absyn_PointerType_struct*)_tmp20B;if(_tmp20C->tag != 5)goto _LLDC;else{_tmp20D=
_tmp20C->f1;_tmp20E=_tmp20D.elt_typ;}}_LLDB: return Cyc_Toc_is_poly_field(_tmp20E,
_tmp20A);_LLDC:;_LLDD: {const char*_tmpAF6;void*_tmpAF5[1];struct Cyc_String_pa_struct
_tmpAF4;(_tmpAF4.tag=0,((_tmpAF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp209->topt))->v)),((
_tmpAF5[0]=& _tmpAF4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAF6="is_poly_project: bad type %s",_tag_dyneither(_tmpAF6,sizeof(char),29))),
_tag_dyneither(_tmpAF5,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAF7;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAF7=_cycalloc(
sizeof(*_tmpAF7)),((_tmpAF7->hd=s,((_tmpAF7->tl=0,_tmpAF7)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAF8;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAF8=_cycalloc(sizeof(*
_tmpAF8)),((_tmpAF8->hd=s,((_tmpAF8->tl=0,_tmpAF8)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAF9[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAF9[1]=s,((_tmpAF9[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAFA[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAFA[1]=n,((
_tmpAFA[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAFA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAFB[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAFB[1]=n,((
_tmpAFB[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAFB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAFC[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAFC[2]=n,((_tmpAFC[1]=s,((_tmpAFC[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFC,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAFD;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->hd=e,((_tmpAFD->tl=0,_tmpAFD)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAFE;throw_match_stmt_opt=((
_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAFE))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp21A=e->r;union Cyc_Absyn_Cnst _tmp21C;struct _dyneither_ptr _tmp21D;_LLDF: {
struct Cyc_Absyn_Const_e_struct*_tmp21B=(struct Cyc_Absyn_Const_e_struct*)_tmp21A;
if(_tmp21B->tag != 0)goto _LLE1;else{_tmp21C=_tmp21B->f1;if((_tmp21C.String_c).tag
!= 7)goto _LLE1;_tmp21D=(struct _dyneither_ptr)(_tmp21C.String_c).val;}}_LLE0:
is_string=1;goto _LLDE;_LLE1:;_LLE2: goto _LLDE;_LLDE:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB04;struct
Cyc_Absyn_Var_d_struct _tmpB03;struct Cyc_List_List*_tmpB02;Cyc_Toc_result_decls=((
_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->hd=Cyc_Absyn_new_decl((void*)((
_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB03.tag=0,((_tmpB03.f1=vd,
_tmpB03)))),_tmpB04)))),0),((_tmpB02->tl=Cyc_Toc_result_decls,_tmpB02))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB0B;
struct _tuple15*_tmpB0A;struct _tuple15*_tmpB09;struct _tuple15*_tmpB08[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB08[2]=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((
_tmpB09->f1=0,((_tmpB09->f2=xplussz,_tmpB09)))))),((_tmpB08[1]=((_tmpB0A=
_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->f1=0,((_tmpB0A->f2=xexp,_tmpB0A)))))),((
_tmpB08[0]=((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->f1=0,((_tmpB0B->f2=
xexp,_tmpB0B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB08,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp226;int _tmp227;struct Cyc_Toc_Env*
_tmp225=nv;_tmp226=*_tmp225;_tmp227=_tmp226.toplevel;return _tmp227;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Toc_Env _tmp229;struct Cyc_Dict_Dict _tmp22A;struct Cyc_Toc_Env*_tmp228=nv;
_tmp229=*_tmp228;_tmp22A=_tmp229.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp22A,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB0C;return(_tmpB0C=_region_malloc(r,
sizeof(*_tmpB0C)),((_tmpB0C->break_lab=(struct _dyneither_ptr**)0,((_tmpB0C->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB0C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB0C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB0C->toplevel=(int)1,((_tmpB0C->rgn=(struct _RegionHandle*)r,_tmpB0C)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22D;struct _dyneither_ptr**_tmp22E;struct _dyneither_ptr**
_tmp22F;struct Cyc_Toc_FallthruInfo*_tmp230;struct Cyc_Dict_Dict _tmp231;int _tmp232;
struct Cyc_Toc_Env*_tmp22C=e;_tmp22D=*_tmp22C;_tmp22E=_tmp22D.break_lab;_tmp22F=
_tmp22D.continue_lab;_tmp230=_tmp22D.fallthru_info;_tmp231=_tmp22D.varmap;
_tmp232=_tmp22D.toplevel;{struct Cyc_Toc_Env*_tmpB0D;return(_tmpB0D=
_region_malloc(r,sizeof(*_tmpB0D)),((_tmpB0D->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22E),((_tmpB0D->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpB0D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp230,((_tmpB0D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp231),((_tmpB0D->toplevel=(
int)_tmp232,((_tmpB0D->rgn=(struct _RegionHandle*)r,_tmpB0D)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp235;struct _dyneither_ptr**_tmp236;struct
_dyneither_ptr**_tmp237;struct Cyc_Toc_FallthruInfo*_tmp238;struct Cyc_Dict_Dict
_tmp239;int _tmp23A;struct Cyc_Toc_Env*_tmp234=e;_tmp235=*_tmp234;_tmp236=_tmp235.break_lab;
_tmp237=_tmp235.continue_lab;_tmp238=_tmp235.fallthru_info;_tmp239=_tmp235.varmap;
_tmp23A=_tmp235.toplevel;{struct Cyc_Toc_Env*_tmpB0E;return(_tmpB0E=
_region_malloc(r,sizeof(*_tmpB0E)),((_tmpB0E->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp236),((_tmpB0E->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpB0E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp238,((_tmpB0E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp239),((_tmpB0E->toplevel=(
int)0,((_tmpB0E->rgn=(struct _RegionHandle*)r,_tmpB0E)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23D;struct _dyneither_ptr**_tmp23E;struct _dyneither_ptr**
_tmp23F;struct Cyc_Toc_FallthruInfo*_tmp240;struct Cyc_Dict_Dict _tmp241;int _tmp242;
struct Cyc_Toc_Env*_tmp23C=e;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.break_lab;_tmp23F=
_tmp23D.continue_lab;_tmp240=_tmp23D.fallthru_info;_tmp241=_tmp23D.varmap;
_tmp242=_tmp23D.toplevel;{struct Cyc_Toc_Env*_tmpB0F;return(_tmpB0F=
_region_malloc(r,sizeof(*_tmpB0F)),((_tmpB0F->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23E),((_tmpB0F->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpB0F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp240,((_tmpB0F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp241),((_tmpB0F->toplevel=(
int)1,((_tmpB0F->rgn=(struct _RegionHandle*)r,_tmpB0F)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp244=(*x).f1;struct Cyc_List_List*_tmp245;_LLE4: if((
_tmp244.Rel_n).tag != 1)goto _LLE6;_tmp245=(struct Cyc_List_List*)(_tmp244.Rel_n).val;
_LLE5: {const char*_tmpB13;void*_tmpB12[1];struct Cyc_String_pa_struct _tmpB11;(
_tmpB11.tag=0,((_tmpB11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB12[0]=& _tmpB11,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB13="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB13,sizeof(char),30))),_tag_dyneither(_tmpB12,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp24A;struct _dyneither_ptr**_tmp24B;
struct _dyneither_ptr**_tmp24C;struct Cyc_Toc_FallthruInfo*_tmp24D;struct Cyc_Dict_Dict
_tmp24E;int _tmp24F;struct Cyc_Toc_Env*_tmp249=e;_tmp24A=*_tmp249;_tmp24B=_tmp24A.break_lab;
_tmp24C=_tmp24A.continue_lab;_tmp24D=_tmp24A.fallthru_info;_tmp24E=_tmp24A.varmap;
_tmp24F=_tmp24A.toplevel;{struct Cyc_Dict_Dict _tmp250=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24E),x,y);struct Cyc_Toc_Env*_tmpB14;return(_tmpB14=_region_malloc(r,
sizeof(*_tmpB14)),((_tmpB14->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24B),((_tmpB14->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24C),((_tmpB14->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24D,((_tmpB14->varmap=(struct Cyc_Dict_Dict)_tmp250,((_tmpB14->toplevel=(int)
_tmp24F,((_tmpB14->rgn=(struct _RegionHandle*)r,_tmpB14)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp253;struct _dyneither_ptr**_tmp254;struct _dyneither_ptr**
_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;
struct Cyc_Toc_Env*_tmp252=e;_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=
_tmp253.continue_lab;_tmp256=_tmp253.fallthru_info;_tmp257=_tmp253.varmap;
_tmp258=_tmp253.toplevel;{struct Cyc_Toc_Env*_tmpB15;return(_tmpB15=
_region_malloc(r,sizeof(*_tmpB15)),((_tmpB15->break_lab=(struct _dyneither_ptr**)
0,((_tmpB15->continue_lab=(struct _dyneither_ptr**)0,((_tmpB15->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpB15->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp257),((_tmpB15->toplevel=(int)_tmp258,((_tmpB15->rgn=(struct _RegionHandle*)
r,_tmpB15)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB16;fallthru_vars=((
_tmpB16=_region_malloc(r,sizeof(*_tmpB16)),((_tmpB16->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB16->tl=fallthru_vars,_tmpB16))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25C;struct _dyneither_ptr**_tmp25D;struct
_dyneither_ptr**_tmp25E;struct Cyc_Toc_FallthruInfo*_tmp25F;struct Cyc_Dict_Dict
_tmp260;int _tmp261;struct Cyc_Toc_Env*_tmp25B=e;_tmp25C=*_tmp25B;_tmp25D=_tmp25C.break_lab;
_tmp25E=_tmp25C.continue_lab;_tmp25F=_tmp25C.fallthru_info;_tmp260=_tmp25C.varmap;
_tmp261=_tmp25C.toplevel;{struct Cyc_Toc_Env _tmp263;struct Cyc_Dict_Dict _tmp264;
struct Cyc_Toc_Env*_tmp262=next_case_env;_tmp263=*_tmp262;_tmp264=_tmp263.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB17;struct Cyc_Toc_FallthruInfo*fi=(_tmpB17=
_region_malloc(r,sizeof(*_tmpB17)),((_tmpB17->label=fallthru_l,((_tmpB17->binders=
fallthru_vars,((_tmpB17->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp264),_tmpB17)))))));
struct _dyneither_ptr**_tmpB1A;struct Cyc_Toc_Env*_tmpB19;return(_tmpB19=
_region_malloc(r,sizeof(*_tmpB19)),((_tmpB19->break_lab=(struct _dyneither_ptr**)((
_tmpB1A=_region_malloc(r,sizeof(*_tmpB1A)),((_tmpB1A[0]=break_l,_tmpB1A)))),((
_tmpB19->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25E),((
_tmpB19->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB19->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp260),((_tmpB19->toplevel=(int)_tmp261,((_tmpB19->rgn=(
struct _RegionHandle*)r,_tmpB19)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp269;struct _dyneither_ptr**
_tmp26A;struct _dyneither_ptr**_tmp26B;struct Cyc_Toc_FallthruInfo*_tmp26C;struct
Cyc_Dict_Dict _tmp26D;int _tmp26E;struct Cyc_Toc_Env*_tmp268=e;_tmp269=*_tmp268;
_tmp26A=_tmp269.break_lab;_tmp26B=_tmp269.continue_lab;_tmp26C=_tmp269.fallthru_info;
_tmp26D=_tmp269.varmap;_tmp26E=_tmp269.toplevel;{struct _dyneither_ptr**_tmpB1D;
struct Cyc_Toc_Env*_tmpB1C;return(_tmpB1C=_region_malloc(r,sizeof(*_tmpB1C)),((
_tmpB1C->break_lab=(struct _dyneither_ptr**)((_tmpB1D=_region_malloc(r,sizeof(*
_tmpB1D)),((_tmpB1D[0]=break_l,_tmpB1D)))),((_tmpB1C->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpB1C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB1C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26D),((_tmpB1C->toplevel=(
int)_tmp26E,((_tmpB1C->rgn=(struct _RegionHandle*)r,_tmpB1C)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp272;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*
_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;struct Cyc_Toc_Env*_tmp271=e;
_tmp272=*_tmp271;_tmp273=_tmp272.break_lab;_tmp274=_tmp272.continue_lab;_tmp275=
_tmp272.fallthru_info;_tmp276=_tmp272.varmap;_tmp277=_tmp272.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB20;struct Cyc_Toc_Env*_tmpB1F;return(_tmpB1F=_region_malloc(r,sizeof(*
_tmpB1F)),((_tmpB1F->break_lab=(struct _dyneither_ptr**)0,((_tmpB1F->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp274),((_tmpB1F->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB20=_region_malloc(r,sizeof(*_tmpB20)),((
_tmpB20->label=next_l,((_tmpB20->binders=0,((_tmpB20->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB20)))))))),((_tmpB1F->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp276),((_tmpB1F->toplevel=(int)_tmp277,((_tmpB1F->rgn=(struct _RegionHandle*)
r,_tmpB1F)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27A=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27B=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27A;if(_tmp27B->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27C=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27A;if(_tmp27C->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27A;if(_tmp27D->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB23;void*_tmpB22;(_tmpB22=0,Cyc_Tcutil_terr(e->loc,((
_tmpB23="dereference of NULL pointer",_tag_dyneither(_tmpB23,sizeof(char),28))),
_tag_dyneither(_tmpB22,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27E=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27A;if(_tmp27E->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27F=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp27A;if(_tmp27F->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB26;void*_tmpB25;(_tmpB25=0,Cyc_Tcutil_warn(e->loc,((
_tmpB26="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB26,
sizeof(char),46))),_tag_dyneither(_tmpB25,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB29;void*_tmpB28;(_tmpB28=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB29="need_null_check",_tag_dyneither(
_tmpB29,sizeof(char),16))),_tag_dyneither(_tmpB28,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp286=e->annot;
struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp28A;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp287=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp286;if(_tmp287->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp288=_tmp287->f1;}}_LLF7: return _tmp288;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp286;if(_tmp289->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp28A=_tmp289->f1;}}_LLF9: return _tmp28A;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp28B=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp286;if(_tmp28B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB2C;void*_tmpB2B;(_tmpB2B=0,Cyc_Tcutil_terr(e->loc,((
_tmpB2C="dereference of NULL pointer",_tag_dyneither(_tmpB2C,sizeof(char),28))),
_tag_dyneither(_tmpB2B,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28C=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp286;if(_tmp28C->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28D=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp286;if(_tmp28D->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB2F;void*_tmpB2E;(
_tmpB2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2F="get_relns",_tag_dyneither(_tmpB2F,sizeof(char),10))),_tag_dyneither(
_tmpB2E,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp292=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp294;struct Cyc_Absyn_PtrAtts
_tmp295;union Cyc_Absyn_Constraint*_tmp296;union Cyc_Absyn_Constraint*_tmp297;
struct Cyc_Absyn_ArrayInfo _tmp299;struct Cyc_Absyn_Exp*_tmp29A;_LL103: {struct Cyc_Absyn_PointerType_struct*
_tmp293=(struct Cyc_Absyn_PointerType_struct*)_tmp292;if(_tmp293->tag != 5)goto
_LL105;else{_tmp294=_tmp293->f1;_tmp295=_tmp294.ptr_atts;_tmp296=_tmp295.bounds;
_tmp297=_tmp295.zero_term;}}_LL104: {void*_tmp29B=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp296);struct Cyc_Absyn_Exp*
_tmp29E;_LL10A: {struct Cyc_Absyn_DynEither_b_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp29B;if(_tmp29C->tag != 0)goto _LL10C;}_LL10B: return 0;_LL10C: {struct Cyc_Absyn_Upper_b_struct*
_tmp29D=(struct Cyc_Absyn_Upper_b_struct*)_tmp29B;if(_tmp29D->tag != 1)goto _LL109;
else{_tmp29E=_tmp29D->f1;}}_LL10D: {unsigned int _tmp2A0;int _tmp2A1;struct
_tuple10 _tmp29F=Cyc_Evexp_eval_const_uint_exp(_tmp29E);_tmp2A0=_tmp29F.f1;
_tmp2A1=_tmp29F.f2;return _tmp2A1  && i <= _tmp2A0;}_LL109:;}_LL105: {struct Cyc_Absyn_ArrayType_struct*
_tmp298=(struct Cyc_Absyn_ArrayType_struct*)_tmp292;if(_tmp298->tag != 9)goto
_LL107;else{_tmp299=_tmp298->f1;_tmp29A=_tmp299.num_elts;}}_LL106: if(_tmp29A == 0)
return 0;{unsigned int _tmp2A3;int _tmp2A4;struct _tuple10 _tmp2A2=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp29A);_tmp2A3=_tmp2A2.f1;_tmp2A4=_tmp2A2.f2;return
_tmp2A4  && i <= _tmp2A3;};_LL107:;_LL108: return 0;_LL102:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A5=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A5->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A6=_tmp2A5->rop;struct Cyc_Absyn_Vardecl*_tmp2A7;struct
Cyc_Absyn_Vardecl*_tmp2A8;_LL10F: if((_tmp2A6.LessNumelts).tag != 3)goto _LL111;
_tmp2A7=(struct Cyc_Absyn_Vardecl*)(_tmp2A6.LessNumelts).val;_LL110: _tmp2A8=
_tmp2A7;goto _LL112;_LL111: if((_tmp2A6.LessEqNumelts).tag != 5)goto _LL113;_tmp2A8=(
struct Cyc_Absyn_Vardecl*)(_tmp2A6.LessEqNumelts).val;_LL112: if(_tmp2A8 == v)
return 1;else{goto _LL10E;}_LL113:;_LL114: continue;_LL10E:;};}return 0;}static int
Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e,void*vtype);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2A9=
e->r;void*_tmp2AB;struct Cyc_Absyn_Vardecl*_tmp2AD;void*_tmp2AF;struct Cyc_Absyn_Vardecl*
_tmp2B1;void*_tmp2B3;struct Cyc_Absyn_Vardecl*_tmp2B5;void*_tmp2B7;struct Cyc_Absyn_Vardecl*
_tmp2B9;union Cyc_Absyn_Cnst _tmp2BB;struct _tuple5 _tmp2BC;enum Cyc_Absyn_Sign
_tmp2BD;int _tmp2BE;union Cyc_Absyn_Cnst _tmp2C0;struct _tuple5 _tmp2C1;enum Cyc_Absyn_Sign
_tmp2C2;int _tmp2C3;union Cyc_Absyn_Cnst _tmp2C5;struct _tuple5 _tmp2C6;enum Cyc_Absyn_Sign
_tmp2C7;int _tmp2C8;enum Cyc_Absyn_Primop _tmp2CA;struct Cyc_List_List*_tmp2CB;
struct Cyc_List_List _tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;_LL116: {struct Cyc_Absyn_Var_e_struct*
_tmp2AA=(struct Cyc_Absyn_Var_e_struct*)_tmp2A9;if(_tmp2AA->tag != 1)goto _LL118;
else{_tmp2AB=(void*)_tmp2AA->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2AC=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2AB;if(_tmp2AC->tag != 5)goto _LL118;else{_tmp2AD=_tmp2AC->f1;}};}}_LL117:
_tmp2B1=_tmp2AD;goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AE=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A9;if(_tmp2AE->tag != 1)goto _LL11A;else{_tmp2AF=(
void*)_tmp2AE->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2B0=(struct Cyc_Absyn_Local_b_struct*)
_tmp2AF;if(_tmp2B0->tag != 4)goto _LL11A;else{_tmp2B1=_tmp2B0->f1;}};}}_LL119:
_tmp2B5=_tmp2B1;goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B2=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A9;if(_tmp2B2->tag != 1)goto _LL11C;else{_tmp2B3=(
void*)_tmp2B2->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2B4=(struct Cyc_Absyn_Global_b_struct*)
_tmp2B3;if(_tmp2B4->tag != 1)goto _LL11C;else{_tmp2B5=_tmp2B4->f1;}};}}_LL11B:
_tmp2B9=_tmp2B5;goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2B6=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A9;if(_tmp2B6->tag != 1)goto _LL11E;else{_tmp2B7=(
void*)_tmp2B6->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2B8=(struct Cyc_Absyn_Param_b_struct*)
_tmp2B7;if(_tmp2B8->tag != 3)goto _LL11E;else{_tmp2B9=_tmp2B8->f1;}};}}_LL11D: if(
_tmp2B9->escapes)return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B9))return 1;
goto _LL115;_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp2BA=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A9;if(_tmp2BA->tag != 0)goto _LL120;else{_tmp2BB=_tmp2BA->f1;if((_tmp2BB.Int_c).tag
!= 4)goto _LL120;_tmp2BC=(struct _tuple5)(_tmp2BB.Int_c).val;_tmp2BD=_tmp2BC.f1;
if(_tmp2BD != Cyc_Absyn_None)goto _LL120;_tmp2BE=_tmp2BC.f2;}}_LL11F: _tmp2C3=
_tmp2BE;goto _LL121;_LL120: {struct Cyc_Absyn_Const_e_struct*_tmp2BF=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A9;if(_tmp2BF->tag != 0)goto _LL122;else{_tmp2C0=_tmp2BF->f1;if((_tmp2C0.Int_c).tag
!= 4)goto _LL122;_tmp2C1=(struct _tuple5)(_tmp2C0.Int_c).val;_tmp2C2=_tmp2C1.f1;
if(_tmp2C2 != Cyc_Absyn_Signed)goto _LL122;_tmp2C3=_tmp2C1.f2;}}_LL121: return
_tmp2C3 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2C3,vtype);_LL122: {
struct Cyc_Absyn_Const_e_struct*_tmp2C4=(struct Cyc_Absyn_Const_e_struct*)_tmp2A9;
if(_tmp2C4->tag != 0)goto _LL124;else{_tmp2C5=_tmp2C4->f1;if((_tmp2C5.Int_c).tag != 
4)goto _LL124;_tmp2C6=(struct _tuple5)(_tmp2C5.Int_c).val;_tmp2C7=_tmp2C6.f1;if(
_tmp2C7 != Cyc_Absyn_Unsigned)goto _LL124;_tmp2C8=_tmp2C6.f2;}}_LL123: return Cyc_Toc_check_const_array((
unsigned int)_tmp2C8,vtype);_LL124: {struct Cyc_Absyn_Primop_e_struct*_tmp2C9=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2A9;if(_tmp2C9->tag != 3)goto _LL126;else{
_tmp2CA=_tmp2C9->f1;if(_tmp2CA != Cyc_Absyn_Numelts)goto _LL126;_tmp2CB=_tmp2C9->f2;
if(_tmp2CB == 0)goto _LL126;_tmp2CC=*_tmp2CB;_tmp2CD=(struct Cyc_Absyn_Exp*)_tmp2CC.hd;}}
_LL125:{void*_tmp2CE=_tmp2CD->r;void*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D2;
void*_tmp2D4;struct Cyc_Absyn_Vardecl*_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*
_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DE;_LL129: {struct Cyc_Absyn_Var_e_struct*
_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2CF->tag != 1)goto _LL12B;
else{_tmp2D0=(void*)_tmp2CF->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2D0;if(_tmp2D1->tag != 5)goto _LL12B;else{_tmp2D2=_tmp2D1->f1;}};}}_LL12A:
_tmp2D6=_tmp2D2;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2D3->tag != 1)goto _LL12D;else{_tmp2D4=(
void*)_tmp2D3->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D4;if(_tmp2D5->tag != 4)goto _LL12D;else{_tmp2D6=_tmp2D5->f1;}};}}_LL12C:
_tmp2DA=_tmp2D6;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2D7->tag != 1)goto _LL12F;else{_tmp2D8=(
void*)_tmp2D7->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D8;if(_tmp2D9->tag != 1)goto _LL12F;else{_tmp2DA=_tmp2D9->f1;}};}}_LL12E:
_tmp2DE=_tmp2DA;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2DB->tag != 1)goto _LL131;else{_tmp2DC=(
void*)_tmp2DB->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)
_tmp2DC;if(_tmp2DD->tag != 3)goto _LL131;else{_tmp2DE=_tmp2DD->f1;}};}}_LL130:
return _tmp2DE == v;_LL131:;_LL132: goto _LL128;_LL128:;}goto _LL115;_LL126:;_LL127:
goto _LL115;_LL115:;}return 0;}static char _tmp2F0[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(void*
a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
struct Cyc_Absyn_Vardecl*x;{void*_tmp2DF=a->r;void*_tmp2E1;struct Cyc_Absyn_Vardecl*
_tmp2E3;void*_tmp2E5;struct Cyc_Absyn_Vardecl*_tmp2E7;void*_tmp2E9;struct Cyc_Absyn_Vardecl*
_tmp2EB;void*_tmp2ED;struct Cyc_Absyn_Vardecl*_tmp2EF;_LL134: {struct Cyc_Absyn_Var_e_struct*
_tmp2E0=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2E0->tag != 1)goto _LL136;
else{_tmp2E1=(void*)_tmp2E0->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2E2=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2E1;if(_tmp2E2->tag != 5)goto _LL136;else{_tmp2E3=_tmp2E2->f1;}};}}_LL135:
_tmp2E7=_tmp2E3;goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_struct*_tmp2E4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2E4->tag != 1)goto _LL138;else{_tmp2E5=(
void*)_tmp2E4->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2E6=(struct Cyc_Absyn_Local_b_struct*)
_tmp2E5;if(_tmp2E6->tag != 4)goto _LL138;else{_tmp2E7=_tmp2E6->f1;}};}}_LL137:
_tmp2EB=_tmp2E7;goto _LL139;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp2E8=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2E8->tag != 1)goto _LL13A;else{_tmp2E9=(
void*)_tmp2E8->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2EA=(struct Cyc_Absyn_Global_b_struct*)
_tmp2E9;if(_tmp2EA->tag != 1)goto _LL13A;else{_tmp2EB=_tmp2EA->f1;}};}}_LL139:
_tmp2EF=_tmp2EB;goto _LL13B;_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp2EC=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2EC->tag != 1)goto _LL13C;else{_tmp2ED=(
void*)_tmp2EC->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2EE=(struct Cyc_Absyn_Param_b_struct*)
_tmp2ED;if(_tmp2EE->tag != 3)goto _LL13C;else{_tmp2EF=_tmp2EE->f1;}};}}_LL13B: if(
_tmp2EF->escapes)goto _LL13D;x=_tmp2EF;goto _LL133;_LL13C:;_LL13D: {static struct
_dyneither_ptr bogus_string={_tmp2F0,_tmp2F0,_tmp2F0 + 8};static struct _tuple0
bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL133:;}{void*_tmp2F1=a_typ;inner_loop: {
void*_tmp2F2=i->r;void*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;union Cyc_Absyn_Cnst
_tmp2F7;struct _tuple5 _tmp2F8;enum Cyc_Absyn_Sign _tmp2F9;int _tmp2FA;union Cyc_Absyn_Cnst
_tmp2FC;struct _tuple5 _tmp2FD;enum Cyc_Absyn_Sign _tmp2FE;int _tmp2FF;union Cyc_Absyn_Cnst
_tmp301;struct _tuple5 _tmp302;enum Cyc_Absyn_Sign _tmp303;int _tmp304;enum Cyc_Absyn_Primop
_tmp306;struct Cyc_List_List*_tmp307;struct Cyc_List_List _tmp308;struct Cyc_Absyn_Exp*
_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_List_List _tmp30B;struct Cyc_Absyn_Exp*
_tmp30C;void*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp312;struct Cyc_Absyn_Vardecl*
_tmp314;void*_tmp316;struct Cyc_Absyn_Vardecl*_tmp318;void*_tmp31A;struct Cyc_Absyn_Vardecl*
_tmp31C;_LL13F: {struct Cyc_Absyn_Cast_e_struct*_tmp2F3=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2F2;if(_tmp2F3->tag != 15)goto _LL141;else{_tmp2F4=(void*)_tmp2F3->f1;_tmp2F5=
_tmp2F3->f2;}}_LL140: i=_tmp2F5;goto inner_loop;_LL141: {struct Cyc_Absyn_Const_e_struct*
_tmp2F6=(struct Cyc_Absyn_Const_e_struct*)_tmp2F2;if(_tmp2F6->tag != 0)goto _LL143;
else{_tmp2F7=_tmp2F6->f1;if((_tmp2F7.Int_c).tag != 4)goto _LL143;_tmp2F8=(struct
_tuple5)(_tmp2F7.Int_c).val;_tmp2F9=_tmp2F8.f1;if(_tmp2F9 != Cyc_Absyn_None)goto
_LL143;_tmp2FA=_tmp2F8.f2;}}_LL142: _tmp2FF=_tmp2FA;goto _LL144;_LL143: {struct Cyc_Absyn_Const_e_struct*
_tmp2FB=(struct Cyc_Absyn_Const_e_struct*)_tmp2F2;if(_tmp2FB->tag != 0)goto _LL145;
else{_tmp2FC=_tmp2FB->f1;if((_tmp2FC.Int_c).tag != 4)goto _LL145;_tmp2FD=(struct
_tuple5)(_tmp2FC.Int_c).val;_tmp2FE=_tmp2FD.f1;if(_tmp2FE != Cyc_Absyn_Signed)
goto _LL145;_tmp2FF=_tmp2FD.f2;}}_LL144: return _tmp2FF >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2FF + 1),_tmp2F1);_LL145: {struct Cyc_Absyn_Const_e_struct*
_tmp300=(struct Cyc_Absyn_Const_e_struct*)_tmp2F2;if(_tmp300->tag != 0)goto _LL147;
else{_tmp301=_tmp300->f1;if((_tmp301.Int_c).tag != 4)goto _LL147;_tmp302=(struct
_tuple5)(_tmp301.Int_c).val;_tmp303=_tmp302.f1;if(_tmp303 != Cyc_Absyn_Unsigned)
goto _LL147;_tmp304=_tmp302.f2;}}_LL146: return Cyc_Toc_check_const_array((
unsigned int)(_tmp304 + 1),_tmp2F1);_LL147: {struct Cyc_Absyn_Primop_e_struct*
_tmp305=(struct Cyc_Absyn_Primop_e_struct*)_tmp2F2;if(_tmp305->tag != 3)goto _LL149;
else{_tmp306=_tmp305->f1;if(_tmp306 != Cyc_Absyn_Mod)goto _LL149;_tmp307=_tmp305->f2;
if(_tmp307 == 0)goto _LL149;_tmp308=*_tmp307;_tmp309=(struct Cyc_Absyn_Exp*)_tmp308.hd;
_tmp30A=_tmp308.tl;if(_tmp30A == 0)goto _LL149;_tmp30B=*_tmp30A;_tmp30C=(struct Cyc_Absyn_Exp*)
_tmp30B.hd;}}_LL148: return Cyc_Toc_check_leq_size(relns,x,_tmp30C,_tmp2F1);_LL149: {
struct Cyc_Absyn_Var_e_struct*_tmp30D=(struct Cyc_Absyn_Var_e_struct*)_tmp2F2;if(
_tmp30D->tag != 1)goto _LL14B;else{_tmp30E=(void*)_tmp30D->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp30F=(struct Cyc_Absyn_Pat_b_struct*)_tmp30E;if(_tmp30F->tag != 5)goto _LL14B;
else{_tmp310=_tmp30F->f1;}};}}_LL14A: _tmp314=_tmp310;goto _LL14C;_LL14B: {struct
Cyc_Absyn_Var_e_struct*_tmp311=(struct Cyc_Absyn_Var_e_struct*)_tmp2F2;if(_tmp311->tag
!= 1)goto _LL14D;else{_tmp312=(void*)_tmp311->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp313=(struct Cyc_Absyn_Local_b_struct*)_tmp312;if(_tmp313->tag != 4)goto _LL14D;
else{_tmp314=_tmp313->f1;}};}}_LL14C: _tmp318=_tmp314;goto _LL14E;_LL14D: {struct
Cyc_Absyn_Var_e_struct*_tmp315=(struct Cyc_Absyn_Var_e_struct*)_tmp2F2;if(_tmp315->tag
!= 1)goto _LL14F;else{_tmp316=(void*)_tmp315->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp317=(struct Cyc_Absyn_Global_b_struct*)_tmp316;if(_tmp317->tag != 1)goto _LL14F;
else{_tmp318=_tmp317->f1;}};}}_LL14E: _tmp31C=_tmp318;goto _LL150;_LL14F: {struct
Cyc_Absyn_Var_e_struct*_tmp319=(struct Cyc_Absyn_Var_e_struct*)_tmp2F2;if(_tmp319->tag
!= 1)goto _LL151;else{_tmp31A=(void*)_tmp319->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp31B=(struct Cyc_Absyn_Param_b_struct*)_tmp31A;if(_tmp31B->tag != 3)goto _LL151;
else{_tmp31C=_tmp31B->f1;}};}}_LL150: if(_tmp31C->escapes)return 0;{struct Cyc_List_List*
_tmp31D=relns;for(0;_tmp31D != 0;_tmp31D=_tmp31D->tl){struct Cyc_CfFlowInfo_Reln*
_tmp31E=(struct Cyc_CfFlowInfo_Reln*)_tmp31D->hd;if(_tmp31E->vd == _tmp31C){union
Cyc_CfFlowInfo_RelnOp _tmp31F=_tmp31E->rop;struct Cyc_Absyn_Vardecl*_tmp320;struct
_tuple11 _tmp321;struct Cyc_Absyn_Vardecl*_tmp322;void*_tmp323;unsigned int _tmp324;
unsigned int _tmp325;_LL154: if((_tmp31F.LessNumelts).tag != 3)goto _LL156;_tmp320=(
struct Cyc_Absyn_Vardecl*)(_tmp31F.LessNumelts).val;_LL155: if(x == _tmp320)return 1;
else{goto _LL153;}_LL156: if((_tmp31F.LessVar).tag != 2)goto _LL158;_tmp321=(struct
_tuple11)(_tmp31F.LessVar).val;_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;_LL157:{
struct _tuple17 _tmpB30;struct _tuple17 _tmp327=(_tmpB30.f1=Cyc_Tcutil_compress(
_tmp323),((_tmpB30.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB30)));void*_tmp328;void*_tmp32A;void*_tmp32B;
struct Cyc_Absyn_PtrInfo _tmp32D;struct Cyc_Absyn_PtrAtts _tmp32E;union Cyc_Absyn_Constraint*
_tmp32F;_LL15F: _tmp328=_tmp327.f1;{struct Cyc_Absyn_TagType_struct*_tmp329=(
struct Cyc_Absyn_TagType_struct*)_tmp328;if(_tmp329->tag != 20)goto _LL161;else{
_tmp32A=(void*)_tmp329->f1;}};_tmp32B=_tmp327.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp32C=(struct Cyc_Absyn_PointerType_struct*)_tmp32B;if(_tmp32C->tag != 5)goto
_LL161;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32D.ptr_atts;_tmp32F=_tmp32E.bounds;}};
_LL160:{void*_tmp330=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp32F);struct Cyc_Absyn_Exp*_tmp332;_LL164: {struct Cyc_Absyn_Upper_b_struct*
_tmp331=(struct Cyc_Absyn_Upper_b_struct*)_tmp330;if(_tmp331->tag != 1)goto _LL166;
else{_tmp332=_tmp331->f1;}}_LL165: {struct Cyc_Absyn_Exp*_tmp333=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp32A,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp333,_tmp332))return 1;goto _LL163;}_LL166:;_LL167:
goto _LL163;_LL163:;}goto _LL15E;_LL161:;_LL162: goto _LL15E;_LL15E:;}{struct Cyc_List_List*
_tmp334=relns;for(0;_tmp334 != 0;_tmp334=_tmp334->tl){struct Cyc_CfFlowInfo_Reln*
_tmp335=(struct Cyc_CfFlowInfo_Reln*)_tmp334->hd;if(_tmp335->vd == _tmp322){union
Cyc_CfFlowInfo_RelnOp _tmp336=_tmp335->rop;struct Cyc_Absyn_Vardecl*_tmp337;struct
Cyc_Absyn_Vardecl*_tmp338;unsigned int _tmp339;unsigned int _tmp33A;struct _tuple11
_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33C;_LL169: if((_tmp336.LessEqNumelts).tag != 
5)goto _LL16B;_tmp337=(struct Cyc_Absyn_Vardecl*)(_tmp336.LessEqNumelts).val;
_LL16A: _tmp338=_tmp337;goto _LL16C;_LL16B: if((_tmp336.LessNumelts).tag != 3)goto
_LL16D;_tmp338=(struct Cyc_Absyn_Vardecl*)(_tmp336.LessNumelts).val;_LL16C: if(x == 
_tmp338)return 1;goto _LL168;_LL16D: if((_tmp336.EqualConst).tag != 1)goto _LL16F;
_tmp339=(unsigned int)(_tmp336.EqualConst).val;_LL16E: return Cyc_Toc_check_const_array(
_tmp339,_tmp2F1);_LL16F: if((_tmp336.LessEqConst).tag != 6)goto _LL171;_tmp33A=(
unsigned int)(_tmp336.LessEqConst).val;if(!(_tmp33A > 0))goto _LL171;_LL170: return
Cyc_Toc_check_const_array(_tmp33A,_tmp2F1);_LL171: if((_tmp336.LessVar).tag != 2)
goto _LL173;_tmp33B=(struct _tuple11)(_tmp336.LessVar).val;_tmp33C=_tmp33B.f1;
_LL172: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp33C))return 1;goto _LL168;_LL173:;
_LL174: goto _LL168;_LL168:;}}}goto _LL153;_LL158: if((_tmp31F.LessConst).tag != 4)
goto _LL15A;_tmp324=(unsigned int)(_tmp31F.LessConst).val;_LL159: return Cyc_Toc_check_const_array(
_tmp324,_tmp2F1);_LL15A: if((_tmp31F.LessEqConst).tag != 6)goto _LL15C;_tmp325=(
unsigned int)(_tmp31F.LessEqConst).val;_LL15B: return Cyc_Toc_check_const_array(
_tmp325 + 1,_tmp2F1);_LL15C:;_LL15D: goto _LL153;_LL153:;}}}goto _LL13E;_LL151:;
_LL152: goto _LL13E;_LL13E:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB33;void*_tmpB32;(_tmpB32=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB33="Missing type in primop ",
_tag_dyneither(_tmpB33,sizeof(char),24))),_tag_dyneither(_tmpB32,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB36;void*_tmpB35;(_tmpB35=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB36="Missing type in primop ",_tag_dyneither(_tmpB36,sizeof(char),24))),
_tag_dyneither(_tmpB35,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*
_tmpB37;return(_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->f1=Cyc_Toc_mt_tq,((
_tmpB37->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB37)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB38;return(
_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->f1=0,((_tmpB38->f2=e,_tmpB38)))));};}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*
x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,
int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp343=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp343): Cyc_Toc_malloc_ptr(
_tmp343));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp343);}};}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*
dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
dles0)- 1;{struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(dles0);for(0;_tmp344 != 0;_tmp344=_tmp344->tl){struct _tuple15
_tmp346;struct Cyc_List_List*_tmp347;struct Cyc_Absyn_Exp*_tmp348;struct _tuple15*
_tmp345=(struct _tuple15*)_tmp344->hd;_tmp346=*_tmp345;_tmp347=_tmp346.f1;_tmp348=
_tmp346.f2;{struct Cyc_Absyn_Exp*e_index;if(_tmp347 == 0)e_index=Cyc_Absyn_signed_int_exp(
count --,0);else{if(_tmp347->tl != 0){const char*_tmpB3B;void*_tmpB3A;(_tmpB3A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB3B="multiple designators in array",
_tag_dyneither(_tmpB3B,sizeof(char),30))),_tag_dyneither(_tmpB3A,sizeof(void*),0)));}{
void*_tmp34B=(void*)_tmp347->hd;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Exp*_tmp34E;
_LL176: {struct Cyc_Absyn_ArrayElement_struct*_tmp34D=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp34C;if(_tmp34D->tag != 0)goto _LL178;else{_tmp34E=_tmp34D->f1;}}_LL177: Cyc_Toc_exp_to_c(
nv,_tmp34E);e_index=_tmp34E;goto _LL175;_LL178: {struct Cyc_Absyn_FieldName_struct*
_tmp34F=(struct Cyc_Absyn_FieldName_struct*)_tmp34C;if(_tmp34F->tag != 1)goto
_LL175;}_LL179: {const char*_tmpB3E;void*_tmpB3D;(_tmpB3D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB3E="field name designators in array",
_tag_dyneither(_tmpB3E,sizeof(char),32))),_tag_dyneither(_tmpB3D,sizeof(void*),0)));}
_LL175:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp352=_tmp348->r;struct Cyc_List_List*_tmp354;struct Cyc_Absyn_Vardecl*_tmp356;
struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp358;int _tmp359;void*_tmp35B;
struct Cyc_List_List*_tmp35C;_LL17B: {struct Cyc_Absyn_Array_e_struct*_tmp353=(
struct Cyc_Absyn_Array_e_struct*)_tmp352;if(_tmp353->tag != 27)goto _LL17D;else{
_tmp354=_tmp353->f1;}}_LL17C: s=Cyc_Toc_init_array(nv,lval,_tmp354,s);goto _LL17A;
_LL17D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp355=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp352;if(_tmp355->tag != 28)goto _LL17F;else{_tmp356=_tmp355->f1;_tmp357=_tmp355->f2;
_tmp358=_tmp355->f3;_tmp359=_tmp355->f4;}}_LL17E: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp356,_tmp357,_tmp358,_tmp359,s,0);goto _LL17A;_LL17F: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp35A=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp352;if(_tmp35A->tag != 30)goto
_LL181;else{_tmp35B=(void*)_tmp35A->f1;_tmp35C=_tmp35A->f2;}}_LL180: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp35B,_tmp35C,s);goto _LL17A;_LL181:;_LL182: Cyc_Toc_exp_to_c(nv,_tmp348);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp348,0),s,0);goto _LL17A;_LL17A:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp35D=vd->name;void*_tmp35E=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp360;struct _RegionHandle*_tmp361;
struct Cyc_Toc_Env*_tmp35F=nv;_tmp360=*_tmp35F;_tmp361=_tmp360.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB41;struct Cyc_Absyn_Local_b_struct*_tmpB40;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp361,nv,_tmp35D,Cyc_Absyn_varb_exp(_tmp35D,(void*)((_tmpB40=_cycalloc(sizeof(*
_tmpB40)),((_tmpB40[0]=((_tmpB41.tag=4,((_tmpB41.f1=vd,_tmpB41)))),_tmpB40)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp35D,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp35D,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp35D,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp35D,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp362=e2->r;struct Cyc_List_List*_tmp364;
struct Cyc_Absyn_Vardecl*_tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*
_tmp368;int _tmp369;void*_tmp36B;struct Cyc_List_List*_tmp36C;_LL184: {struct Cyc_Absyn_Array_e_struct*
_tmp363=(struct Cyc_Absyn_Array_e_struct*)_tmp362;if(_tmp363->tag != 27)goto _LL186;
else{_tmp364=_tmp363->f1;}}_LL185: body=Cyc_Toc_init_array(nv2,lval,_tmp364,Cyc_Toc_skip_stmt_dl());
goto _LL183;_LL186: {struct Cyc_Absyn_Comprehension_e_struct*_tmp365=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp362;if(_tmp365->tag != 28)goto _LL188;else{_tmp366=_tmp365->f1;_tmp367=_tmp365->f2;
_tmp368=_tmp365->f3;_tmp369=_tmp365->f4;}}_LL187: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp366,_tmp367,_tmp368,_tmp369,Cyc_Toc_skip_stmt_dl(),0);goto _LL183;
_LL188: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp36A=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp362;if(_tmp36A->tag != 30)goto _LL18A;else{_tmp36B=(void*)_tmp36A->f1;_tmp36C=
_tmp36A->f2;}}_LL189: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp36B,_tmp36C,Cyc_Toc_skip_stmt_dl());
goto _LL183;_LL18A:;_LL18B: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL183;_LL183:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp35E,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp35D,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp36F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp36F != 0;_tmp36F=_tmp36F->tl){struct _tuple15 _tmp371;
struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct _tuple15*_tmp370=(
struct _tuple15*)_tmp36F->hd;_tmp371=*_tmp370;_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;
if(_tmp372 == 0){const char*_tmpB44;void*_tmpB43;(_tmpB43=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB44="empty designator list",
_tag_dyneither(_tmpB44,sizeof(char),22))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}
if(_tmp372->tl != 0){const char*_tmpB47;void*_tmpB46;(_tmpB46=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB47="too many designators in anonymous struct",
_tag_dyneither(_tmpB47,sizeof(char),41))),_tag_dyneither(_tmpB46,sizeof(void*),0)));}{
void*_tmp378=(void*)_tmp372->hd;struct _dyneither_ptr*_tmp37A;_LL18D: {struct Cyc_Absyn_FieldName_struct*
_tmp379=(struct Cyc_Absyn_FieldName_struct*)_tmp378;if(_tmp379->tag != 1)goto
_LL18F;else{_tmp37A=_tmp379->f1;}}_LL18E: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp37A,0);{void*_tmp37B=_tmp373->r;struct Cyc_List_List*_tmp37D;struct Cyc_Absyn_Vardecl*
_tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_Absyn_Exp*_tmp381;int _tmp382;void*
_tmp384;struct Cyc_List_List*_tmp385;_LL192: {struct Cyc_Absyn_Array_e_struct*
_tmp37C=(struct Cyc_Absyn_Array_e_struct*)_tmp37B;if(_tmp37C->tag != 27)goto _LL194;
else{_tmp37D=_tmp37C->f1;}}_LL193: s=Cyc_Toc_init_array(nv,lval,_tmp37D,s);goto
_LL191;_LL194: {struct Cyc_Absyn_Comprehension_e_struct*_tmp37E=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp37B;if(_tmp37E->tag != 28)goto _LL196;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;
_tmp381=_tmp37E->f3;_tmp382=_tmp37E->f4;}}_LL195: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp37F,_tmp380,_tmp381,_tmp382,s,0);goto _LL191;_LL196: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp383=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp37B;if(_tmp383->tag != 30)goto
_LL198;else{_tmp384=(void*)_tmp383->f1;_tmp385=_tmp383->f2;}}_LL197: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp384,_tmp385,s);goto _LL191;_LL198:;_LL199: Cyc_Toc_exp_to_c(nv,_tmp373);
if(Cyc_Toc_is_poly_field(struct_type,_tmp37A))_tmp373=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp373);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp373,0),0),s,0);goto _LL191;_LL191:;}goto _LL18C;}_LL18F:;_LL190: {const char*
_tmpB4A;void*_tmpB49;(_tmpB49=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4A="array designator in struct",
_tag_dyneither(_tmpB4A,sizeof(char),27))),_tag_dyneither(_tmpB49,sizeof(void*),0)));}
_LL18C:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp389;struct _RegionHandle*_tmp38A;
struct Cyc_Toc_Env*_tmp388=nv;_tmp389=*_tmp388;_tmp38A=_tmp389.rgn;{struct Cyc_List_List*
_tmp38B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple9*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp38A,Cyc_Toc_tup_to_c,
es);void*_tmp38C=Cyc_Toc_add_tuple_type(_tmp38B);struct _tuple0*_tmp38D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38E=Cyc_Absyn_var_exp(_tmp38D,0);struct Cyc_Absyn_Stmt*
_tmp38F=Cyc_Absyn_exp_stmt(_tmp38E,0);struct Cyc_Absyn_Exp*(*_tmp390)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp38A,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp391);for(0;_tmp391 != 0;(
_tmp391=_tmp391->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp391->hd;
struct Cyc_Absyn_Exp*lval=_tmp390(_tmp38E,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp392=e->r;struct Cyc_List_List*_tmp394;struct Cyc_Absyn_Vardecl*_tmp396;
struct Cyc_Absyn_Exp*_tmp397;struct Cyc_Absyn_Exp*_tmp398;int _tmp399;_LL19B: {
struct Cyc_Absyn_Array_e_struct*_tmp393=(struct Cyc_Absyn_Array_e_struct*)_tmp392;
if(_tmp393->tag != 27)goto _LL19D;else{_tmp394=_tmp393->f1;}}_LL19C: _tmp38F=Cyc_Toc_init_array(
nv,lval,_tmp394,_tmp38F);goto _LL19A;_LL19D: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp395=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp392;if(_tmp395->tag != 28)
goto _LL19F;else{_tmp396=_tmp395->f1;_tmp397=_tmp395->f2;_tmp398=_tmp395->f3;
_tmp399=_tmp395->f4;}}_LL19E: _tmp38F=Cyc_Toc_init_comprehension(nv,lval,_tmp396,
_tmp397,_tmp398,_tmp399,_tmp38F,0);goto _LL19A;_LL19F:;_LL1A0: Cyc_Toc_exp_to_c(nv,
e);_tmp38F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp390(
_tmp38E,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp38F,0);goto _LL19A;_LL19A:;};}}
return Cyc_Toc_make_struct(nv,_tmp38D,_tmp38C,_tmp38F,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp39A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){struct Cyc_Absyn_Aggrfield*
_tmp39B=(struct Cyc_Absyn_Aggrfield*)_tmp39A->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp39B->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB52;void*_tmpB51[1];const char*_tmpB50;void*_tmpB4F;(
_tmpB4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB52.tag=0,((_tmpB52.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB51[0]=& _tmpB52,Cyc_aprintf(((_tmpB50="get_member_offset %s failed",
_tag_dyneither(_tmpB50,sizeof(char),28))),_tag_dyneither(_tmpB51,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB4F,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);struct Cyc_Absyn_Stmt*
_tmp3A2=Cyc_Absyn_exp_stmt(_tmp3A1,0);struct Cyc_Absyn_Exp*(*_tmp3A3)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp3A4=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3A5=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3A7;union Cyc_Absyn_AggrInfoU _tmp3A8;
_LL1A2: {struct Cyc_Absyn_AggrType_struct*_tmp3A6=(struct Cyc_Absyn_AggrType_struct*)
_tmp3A5;if(_tmp3A6->tag != 12)goto _LL1A4;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A7.aggr_info;}}
_LL1A3: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3A8);goto _LL1A1;_LL1A4:;_LL1A5: {
const char*_tmpB55;void*_tmpB54;(_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="init_struct: bad struct type",
_tag_dyneither(_tmpB55,sizeof(char),29))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}
_LL1A1:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3AC;struct _RegionHandle*_tmp3AD;struct Cyc_Toc_Env*_tmp3AB=
nv;_tmp3AC=*_tmp3AB;_tmp3AD=_tmp3AC.rgn;{struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AD,dles);for(0;
_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){struct _tuple15 _tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;struct _tuple15*_tmp3AF=(struct _tuple15*)
_tmp3AE->hd;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B2->topt))->v);
if(_tmp3B1 == 0){const char*_tmpB58;void*_tmpB57;(_tmpB57=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="empty designator list",
_tag_dyneither(_tmpB58,sizeof(char),22))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}
if(_tmp3B1->tl != 0){struct _tuple0*_tmp3B5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B6=Cyc_Absyn_var_exp(_tmp3B5,0);for(0;_tmp3B1 != 0;_tmp3B1=_tmp3B1->tl){void*
_tmp3B7=(void*)_tmp3B1->hd;struct _dyneither_ptr*_tmp3B9;_LL1A7: {struct Cyc_Absyn_FieldName_struct*
_tmp3B8=(struct Cyc_Absyn_FieldName_struct*)_tmp3B7;if(_tmp3B8->tag != 1)goto
_LL1A9;else{_tmp3B9=_tmp3B8->f1;}}_LL1A8: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B9))_tmp3B6=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B6);_tmp3A2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A3(_tmp3A1,_tmp3B9,0),_tmp3B6,0),0),
_tmp3A2,0);goto _LL1A6;_LL1A9:;_LL1AA: {const char*_tmpB5B;void*_tmpB5A;(_tmpB5A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5B="array designator in struct",_tag_dyneither(_tmpB5B,sizeof(char),27))),
_tag_dyneither(_tmpB5A,sizeof(void*),0)));}_LL1A6:;}Cyc_Toc_exp_to_c(nv,_tmp3B2);
_tmp3A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B6,
_tmp3B2,0),0),_tmp3A2,0);}else{void*_tmp3BC=(void*)_tmp3B1->hd;struct
_dyneither_ptr*_tmp3BE;_LL1AC: {struct Cyc_Absyn_FieldName_struct*_tmp3BD=(struct
Cyc_Absyn_FieldName_struct*)_tmp3BC;if(_tmp3BD->tag != 1)goto _LL1AE;else{_tmp3BE=
_tmp3BD->f1;}}_LL1AD: {struct Cyc_Absyn_Exp*lval=_tmp3A3(_tmp3A1,_tmp3BE,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3BE);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3A2,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3BF=_tmp3B2->r;
struct Cyc_List_List*_tmp3C1;struct Cyc_Absyn_Vardecl*_tmp3C3;struct Cyc_Absyn_Exp*
_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;int _tmp3C6;void*_tmp3C8;struct Cyc_List_List*
_tmp3C9;_LL1B1: {struct Cyc_Absyn_Array_e_struct*_tmp3C0=(struct Cyc_Absyn_Array_e_struct*)
_tmp3BF;if(_tmp3C0->tag != 27)goto _LL1B3;else{_tmp3C1=_tmp3C0->f1;}}_LL1B2:
_tmp3A2=Cyc_Toc_init_array(nv,lval,_tmp3C1,_tmp3A2);goto _LL1B0;_LL1B3: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3C2=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BF;if(_tmp3C2->tag != 28)goto _LL1B5;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;
_tmp3C5=_tmp3C2->f3;_tmp3C6=_tmp3C2->f4;}}_LL1B4: _tmp3A2=Cyc_Toc_init_comprehension(
nv,lval,_tmp3C3,_tmp3C4,_tmp3C5,_tmp3C6,_tmp3A2,0);goto _LL1B0;_LL1B5: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3C7=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3BF;if(_tmp3C7->tag != 30)goto
_LL1B7;else{_tmp3C8=(void*)_tmp3C7->f1;_tmp3C9=_tmp3C7->f2;}}_LL1B6: _tmp3A2=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3C8,_tmp3C9,_tmp3A2);goto _LL1B0;_LL1B7:;_LL1B8: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B2->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3B2);{struct Cyc_Absyn_Aggrfield*_tmp3CA=Cyc_Absyn_lookup_decl_field(ad,
_tmp3BE);if(Cyc_Toc_is_poly_field(struct_type,_tmp3BE) && !was_ptr_type)_tmp3B2=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B2);if(has_exists)_tmp3B2=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3CA))->type),
_tmp3B2);_tmp3A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3B2,0),0),_tmp3A2,0);goto _LL1B0;};}_LL1B0:;}goto _LL1AB;}_LL1AE:;_LL1AF: {
const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="array designator in struct",
_tag_dyneither(_tmpB5E,sizeof(char),27))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
_LL1AB:;}}}return Cyc_Toc_make_struct(nv,_tmp3A0,_tmp3A4,_tmp3A2,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB61;struct Cyc_Absyn_Increment_e_struct*
_tmpB60;return Cyc_Absyn_new_exp((void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((
_tmpB60[0]=((_tmpB61.tag=5,((_tmpB61.f1=e1,((_tmpB61.f2=incr,_tmpB61)))))),
_tmpB60)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3CF=e1->r;struct Cyc_Absyn_Stmt*
_tmp3D1;void*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;
struct _dyneither_ptr*_tmp3D7;int _tmp3D8;int _tmp3D9;_LL1BA: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3D0=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3CF;if(_tmp3D0->tag != 37)goto
_LL1BC;else{_tmp3D1=_tmp3D0->f1;}}_LL1BB: Cyc_Toc_lvalue_assign_stmt(_tmp3D1,fs,f,
f_env);goto _LL1B9;_LL1BC: {struct Cyc_Absyn_Cast_e_struct*_tmp3D2=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3CF;if(_tmp3D2->tag != 15)goto _LL1BE;else{_tmp3D3=(void*)_tmp3D2->f1;_tmp3D4=
_tmp3D2->f2;}}_LL1BD: Cyc_Toc_lvalue_assign(_tmp3D4,fs,f,f_env);goto _LL1B9;_LL1BE: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3D5=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3CF;if(_tmp3D5->tag != 22)goto _LL1C0;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;
_tmp3D8=_tmp3D5->f3;_tmp3D9=_tmp3D5->f4;}}_LL1BF: e1->r=_tmp3D6->r;{struct Cyc_List_List*
_tmpB62;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB62=_cycalloc(
sizeof(*_tmpB62)),((_tmpB62->hd=_tmp3D7,((_tmpB62->tl=fs,_tmpB62)))))),f,f_env);}
goto _LL1B9;_LL1C0:;_LL1C1: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1B9;}
_LL1B9:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3DB=s->r;
struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Decl*_tmp3DF;struct Cyc_Absyn_Stmt*
_tmp3E0;struct Cyc_Absyn_Stmt*_tmp3E2;_LL1C3: {struct Cyc_Absyn_Exp_s_struct*
_tmp3DC=(struct Cyc_Absyn_Exp_s_struct*)_tmp3DB;if(_tmp3DC->tag != 1)goto _LL1C5;
else{_tmp3DD=_tmp3DC->f1;}}_LL1C4: Cyc_Toc_lvalue_assign(_tmp3DD,fs,f,f_env);goto
_LL1C2;_LL1C5: {struct Cyc_Absyn_Decl_s_struct*_tmp3DE=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3DB;if(_tmp3DE->tag != 12)goto _LL1C7;else{_tmp3DF=_tmp3DE->f1;_tmp3E0=_tmp3DE->f2;}}
_LL1C6: Cyc_Toc_lvalue_assign_stmt(_tmp3E0,fs,f,f_env);goto _LL1C2;_LL1C7: {struct
Cyc_Absyn_Seq_s_struct*_tmp3E1=(struct Cyc_Absyn_Seq_s_struct*)_tmp3DB;if(_tmp3E1->tag
!= 2)goto _LL1C9;else{_tmp3E2=_tmp3E1->f2;}}_LL1C8: Cyc_Toc_lvalue_assign_stmt(
_tmp3E2,fs,f,f_env);goto _LL1C2;_LL1C9:;_LL1CA: {const char*_tmpB66;void*_tmpB65[1];
struct Cyc_String_pa_struct _tmpB64;(_tmpB64.tag=0,((_tmpB64.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB65[0]=&
_tmpB64,Cyc_Toc_toc_impos(((_tmpB66="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB66,sizeof(char),23))),_tag_dyneither(_tmpB65,sizeof(void*),1)))))));}_LL1C2:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3E6=
e->r;void*_tmp3E8;void**_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp**
_tmp3EB;struct Cyc_Absyn_Exp*_tmp3ED;_LL1CC: {struct Cyc_Absyn_Cast_e_struct*
_tmp3E7=(struct Cyc_Absyn_Cast_e_struct*)_tmp3E6;if(_tmp3E7->tag != 15)goto _LL1CE;
else{_tmp3E8=(void**)& _tmp3E7->f1;_tmp3E9=(void**)((void**)& _tmp3E7->f1);_tmp3EA=
_tmp3E7->f2;_tmp3EB=(struct Cyc_Absyn_Exp**)& _tmp3E7->f2;}}_LL1CD:*_tmp3EB=Cyc_Toc_push_address_exp(*
_tmp3EB);*_tmp3E9=Cyc_Absyn_cstar_typ(*_tmp3E9,Cyc_Toc_mt_tq);return e;_LL1CE: {
struct Cyc_Absyn_Deref_e_struct*_tmp3EC=(struct Cyc_Absyn_Deref_e_struct*)_tmp3E6;
if(_tmp3EC->tag != 21)goto _LL1D0;else{_tmp3ED=_tmp3EC->f1;}}_LL1CF: return _tmp3ED;
_LL1D0:;_LL1D1: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB6A;void*_tmpB69[1];struct Cyc_String_pa_struct _tmpB68;(_tmpB68.tag=0,((
_tmpB68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB69[0]=& _tmpB68,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6A="can't take & of %s",_tag_dyneither(_tmpB6A,sizeof(char),19))),
_tag_dyneither(_tmpB69,sizeof(void*),1)))))));};_LL1CB:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB6B;result=((
_tmpB6B=_region_malloc(r2,sizeof(*_tmpB6B)),((_tmpB6B->hd=(void*)f((void*)x->hd,
env),((_tmpB6B->tl=0,_tmpB6B))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB6C;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB6C=_region_malloc(r2,sizeof(*_tmpB6C)),((_tmpB6C->hd=(void*)f((void*)x->hd,
env),((_tmpB6C->tl=0,_tmpB6C))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*_tmpB6D;
return(_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->f1=0,((_tmpB6D->f2=e,
_tmpB6D)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3F4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3F6;_LL1D3: {struct Cyc_Absyn_PointerType_struct*
_tmp3F5=(struct Cyc_Absyn_PointerType_struct*)_tmp3F4;if(_tmp3F5->tag != 5)goto
_LL1D5;else{_tmp3F6=_tmp3F5->f1;}}_LL1D4: return _tmp3F6;_LL1D5:;_LL1D6: {const
char*_tmpB70;void*_tmpB6F;(_tmpB6F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB70="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB70,sizeof(char),28))),_tag_dyneither(_tmpB6F,sizeof(void*),0)));}
_LL1D2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3F9=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Size_of _tmp3FD;enum Cyc_Absyn_Sign
_tmp3FF;enum Cyc_Absyn_Size_of _tmp400;enum Cyc_Absyn_Sign _tmp404;enum Cyc_Absyn_Size_of
_tmp405;enum Cyc_Absyn_Sign _tmp407;enum Cyc_Absyn_Size_of _tmp408;enum Cyc_Absyn_Sign
_tmp40A;enum Cyc_Absyn_Size_of _tmp40B;_LL1D8: {struct Cyc_Absyn_PointerType_struct*
_tmp3FA=(struct Cyc_Absyn_PointerType_struct*)_tmp3F9;if(_tmp3FA->tag != 5)goto
_LL1DA;}_LL1D9: res=Cyc_Absyn_null_exp(0);goto _LL1D7;_LL1DA: {struct Cyc_Absyn_IntType_struct*
_tmp3FB=(struct Cyc_Absyn_IntType_struct*)_tmp3F9;if(_tmp3FB->tag != 6)goto _LL1DC;
else{_tmp3FC=_tmp3FB->f1;_tmp3FD=_tmp3FB->f2;if(_tmp3FD != Cyc_Absyn_Char_sz)goto
_LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3FC,'\000'),0);goto
_LL1D7;_LL1DC: {struct Cyc_Absyn_IntType_struct*_tmp3FE=(struct Cyc_Absyn_IntType_struct*)
_tmp3F9;if(_tmp3FE->tag != 6)goto _LL1DE;else{_tmp3FF=_tmp3FE->f1;_tmp400=_tmp3FE->f2;
if(_tmp400 != Cyc_Absyn_Short_sz)goto _LL1DE;}}_LL1DD: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3FF,0),0);goto _LL1D7;_LL1DE: {struct Cyc_Absyn_EnumType_struct*_tmp401=(
struct Cyc_Absyn_EnumType_struct*)_tmp3F9;if(_tmp401->tag != 14)goto _LL1E0;}_LL1DF:
goto _LL1E1;_LL1E0: {struct Cyc_Absyn_AnonEnumType_struct*_tmp402=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3F9;if(_tmp402->tag != 15)goto _LL1E2;}_LL1E1: _tmp404=Cyc_Absyn_Unsigned;goto
_LL1E3;_LL1E2: {struct Cyc_Absyn_IntType_struct*_tmp403=(struct Cyc_Absyn_IntType_struct*)
_tmp3F9;if(_tmp403->tag != 6)goto _LL1E4;else{_tmp404=_tmp403->f1;_tmp405=_tmp403->f2;
if(_tmp405 != Cyc_Absyn_Int_sz)goto _LL1E4;}}_LL1E3: _tmp407=_tmp404;goto _LL1E5;
_LL1E4: {struct Cyc_Absyn_IntType_struct*_tmp406=(struct Cyc_Absyn_IntType_struct*)
_tmp3F9;if(_tmp406->tag != 6)goto _LL1E6;else{_tmp407=_tmp406->f1;_tmp408=_tmp406->f2;
if(_tmp408 != Cyc_Absyn_Long_sz)goto _LL1E6;}}_LL1E5: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp407,0),0);goto _LL1D7;_LL1E6: {struct Cyc_Absyn_IntType_struct*_tmp409=(struct
Cyc_Absyn_IntType_struct*)_tmp3F9;if(_tmp409->tag != 6)goto _LL1E8;else{_tmp40A=
_tmp409->f1;_tmp40B=_tmp409->f2;if(_tmp40B != Cyc_Absyn_LongLong_sz)goto _LL1E8;}}
_LL1E7: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp40A,(long long)0),0);
goto _LL1D7;_LL1E8: {struct Cyc_Absyn_FloatType_struct*_tmp40C=(struct Cyc_Absyn_FloatType_struct*)
_tmp3F9;if(_tmp40C->tag != 7)goto _LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_DoubleType_struct*
_tmp40D=(struct Cyc_Absyn_DoubleType_struct*)_tmp3F9;if(_tmp40D->tag != 8)goto
_LL1EC;}_LL1EB:{const char*_tmpB71;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB71="0.0",_tag_dyneither(_tmpB71,sizeof(char),4)))),0);}goto _LL1D7;_LL1EC:;
_LL1ED: {const char*_tmpB75;void*_tmpB74[1];struct Cyc_String_pa_struct _tmpB73;(
_tmpB73.tag=0,((_tmpB73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB74[0]=& _tmpB73,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB75="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB75,sizeof(char),40))),_tag_dyneither(_tmpB74,sizeof(void*),1)))))));}
_LL1D7:;}{struct Cyc_Core_Opt*_tmpB76;res->topt=((_tmpB76=_cycalloc(sizeof(*
_tmpB76)),((_tmpB76->v=t,_tmpB76))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp413=Cyc_Toc_typ_to_c(elt_type);void*_tmp414=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp415=Cyc_Absyn_cstar_typ(_tmp413,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB77;struct Cyc_Core_Opt*_tmp416=(_tmpB77=_cycalloc(sizeof(*_tmpB77)),((
_tmpB77->v=_tmp415,_tmpB77)));struct Cyc_Absyn_Exp*xinit;{void*_tmp417=e1->r;
struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*
_tmp41C;_LL1EF: {struct Cyc_Absyn_Deref_e_struct*_tmp418=(struct Cyc_Absyn_Deref_e_struct*)
_tmp417;if(_tmp418->tag != 21)goto _LL1F1;else{_tmp419=_tmp418->f1;}}_LL1F0: if(!
is_dyneither){_tmp419=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp419,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB78;_tmp419->topt=((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((
_tmpB78->v=fat_ptr_type,_tmpB78))));};}Cyc_Toc_exp_to_c(nv,_tmp419);xinit=
_tmp419;goto _LL1EE;_LL1F1: {struct Cyc_Absyn_Subscript_e_struct*_tmp41A=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp417;if(_tmp41A->tag != 24)goto _LL1F3;else{
_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL1F2: if(!is_dyneither){_tmp41B=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp41B,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB79;
_tmp41B->topt=((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->v=fat_ptr_type,
_tmpB79))));};}Cyc_Toc_exp_to_c(nv,_tmp41B);Cyc_Toc_exp_to_c(nv,_tmp41C);{struct
Cyc_Absyn_Exp*_tmpB7A[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB7A[2]=_tmp41C,((_tmpB7A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB7A[0]=_tmp41B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB7A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EE;_LL1F3:;_LL1F4: {const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7D="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB7D,sizeof(char),53))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
_LL1EE:;}{struct _tuple0*_tmp422=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp424;
struct _RegionHandle*_tmp425;struct Cyc_Toc_Env*_tmp423=nv;_tmp424=*_tmp423;
_tmp425=_tmp424.rgn;{struct Cyc_Toc_Env*_tmp426=Cyc_Toc_add_varmap(_tmp425,nv,
_tmp422,Cyc_Absyn_var_exp(_tmp422,0));struct Cyc_Absyn_Vardecl*_tmpB7E;struct Cyc_Absyn_Vardecl*
_tmp427=(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->sc=Cyc_Absyn_Public,((
_tmpB7E->name=_tmp422,((_tmpB7E->tq=Cyc_Toc_mt_tq,((_tmpB7E->type=_tmp414,((
_tmpB7E->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB7E->rgn=0,((_tmpB7E->attributes=
0,((_tmpB7E->escapes=0,_tmpB7E)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB81;struct Cyc_Absyn_Local_b_struct*_tmpB80;struct Cyc_Absyn_Local_b_struct*
_tmp428=(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80[0]=((_tmpB81.tag=4,((
_tmpB81.f1=_tmp427,_tmpB81)))),_tmpB80)));struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_varb_exp(
_tmp422,(void*)_tmp428,0);{struct Cyc_Core_Opt*_tmpB82;_tmp429->topt=((_tmpB82=
_cycalloc(sizeof(*_tmpB82)),((_tmpB82->v=fat_ptr_type,_tmpB82))));}{struct Cyc_Absyn_Exp*
_tmp42B=Cyc_Absyn_deref_exp(_tmp429,0);{struct Cyc_Core_Opt*_tmpB83;_tmp42B->topt=((
_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->v=elt_type,_tmpB83))));}Cyc_Toc_exp_to_c(
_tmp426,_tmp42B);{struct _tuple0*_tmp42D=Cyc_Toc_temp_var();_tmp426=Cyc_Toc_add_varmap(
_tmp425,_tmp426,_tmp42D,Cyc_Absyn_var_exp(_tmp42D,0));{struct Cyc_Absyn_Vardecl*
_tmpB84;struct Cyc_Absyn_Vardecl*_tmp42E=(_tmpB84=_cycalloc(sizeof(*_tmpB84)),((
_tmpB84->sc=Cyc_Absyn_Public,((_tmpB84->name=_tmp42D,((_tmpB84->tq=Cyc_Toc_mt_tq,((
_tmpB84->type=_tmp413,((_tmpB84->initializer=(struct Cyc_Absyn_Exp*)_tmp42B,((
_tmpB84->rgn=0,((_tmpB84->attributes=0,((_tmpB84->escapes=0,_tmpB84)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB87;struct Cyc_Absyn_Local_b_struct*_tmpB86;
struct Cyc_Absyn_Local_b_struct*_tmp42F=(_tmpB86=_cycalloc(sizeof(*_tmpB86)),((
_tmpB86[0]=((_tmpB87.tag=4,((_tmpB87.f1=_tmp42E,_tmpB87)))),_tmpB86)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_varb_exp(_tmp42D,(
void*)_tmp42F,0);_tmp430->topt=_tmp42B->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp430,e2,0);z_init->topt=_tmp430->topt;}Cyc_Toc_exp_to_c(
_tmp426,z_init);{struct _tuple0*_tmp431=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB88;struct Cyc_Absyn_Vardecl*_tmp432=(_tmpB88=_cycalloc(sizeof(*_tmpB88)),((
_tmpB88->sc=Cyc_Absyn_Public,((_tmpB88->name=_tmp431,((_tmpB88->tq=Cyc_Toc_mt_tq,((
_tmpB88->type=_tmp413,((_tmpB88->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB88->rgn=0,((_tmpB88->attributes=0,((_tmpB88->escapes=0,_tmpB88)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB8B;struct Cyc_Absyn_Local_b_struct*_tmpB8A;
struct Cyc_Absyn_Local_b_struct*_tmp433=(_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((
_tmpB8A[0]=((_tmpB8B.tag=4,((_tmpB8B.f1=_tmp432,_tmpB8B)))),_tmpB8A)));_tmp426=
Cyc_Toc_add_varmap(_tmp425,_tmp426,_tmp431,Cyc_Absyn_var_exp(_tmp431,0));{struct
Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_varb_exp(_tmp42D,(void*)_tmp42F,0);_tmp434->topt=
_tmp42B->topt;{struct Cyc_Absyn_Exp*_tmp435=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp434,_tmp435,0);{
struct Cyc_Core_Opt*_tmpB8C;_tmp436->topt=((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((
_tmpB8C->v=Cyc_Absyn_sint_typ,_tmpB8C))));}Cyc_Toc_exp_to_c(_tmp426,_tmp436);{
struct Cyc_Absyn_Exp*_tmp438=Cyc_Absyn_varb_exp(_tmp431,(void*)_tmp433,0);_tmp438->topt=
_tmp42B->topt;{struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp438,_tmp439,0);{
struct Cyc_Core_Opt*_tmpB8D;_tmp43A->topt=((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((
_tmpB8D->v=Cyc_Absyn_sint_typ,_tmpB8D))));}Cyc_Toc_exp_to_c(_tmp426,_tmp43A);{
struct Cyc_Absyn_Exp*_tmpB8E[2];struct Cyc_List_List*_tmp43C=(_tmpB8E[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB8E[0]=Cyc_Absyn_varb_exp(_tmp422,(void*)
_tmp428,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB8E,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp43D=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp43C,0),
_tmp43D,0);{struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp436,_tmp43A,0),0);struct Cyc_Absyn_Stmt*_tmp43F=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp440=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp428,0),Cyc_Toc_curr_sp,0);_tmp440=Cyc_Toc_cast_it(
_tmp415,_tmp440);{struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_deref_exp(_tmp440,0);
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_assign_exp(_tmp441,Cyc_Absyn_var_exp(
_tmp431,0),0);struct Cyc_Absyn_Stmt*_tmp443=Cyc_Absyn_exp_stmt(_tmp442,0);_tmp443=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp43E,_tmp43F,Cyc_Absyn_skip_stmt(
0),0),_tmp443,0);{struct Cyc_Absyn_Var_d_struct*_tmpB94;struct Cyc_Absyn_Var_d_struct
_tmpB93;struct Cyc_Absyn_Decl*_tmpB92;_tmp443=Cyc_Absyn_decl_stmt(((_tmpB92=
_cycalloc(sizeof(*_tmpB92)),((_tmpB92->r=(void*)((_tmpB94=_cycalloc(sizeof(*
_tmpB94)),((_tmpB94[0]=((_tmpB93.tag=0,((_tmpB93.f1=_tmp432,_tmpB93)))),_tmpB94)))),((
_tmpB92->loc=0,_tmpB92)))))),_tmp443,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB9A;
struct Cyc_Absyn_Var_d_struct _tmpB99;struct Cyc_Absyn_Decl*_tmpB98;_tmp443=Cyc_Absyn_decl_stmt(((
_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->r=(void*)((_tmpB9A=_cycalloc(
sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB99.tag=0,((_tmpB99.f1=_tmp42E,_tmpB99)))),
_tmpB9A)))),((_tmpB98->loc=0,_tmpB98)))))),_tmp443,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpBA0;struct Cyc_Absyn_Var_d_struct _tmpB9F;struct Cyc_Absyn_Decl*_tmpB9E;_tmp443=
Cyc_Absyn_decl_stmt(((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->r=(void*)((
_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0[0]=((_tmpB9F.tag=0,((_tmpB9F.f1=
_tmp427,_tmpB9F)))),_tmpBA0)))),((_tmpB9E->loc=0,_tmpB9E)))))),_tmp443,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp443);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp458=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp45A;union Cyc_Absyn_AggrInfoU _tmp45B;_LL1F6: {struct Cyc_Absyn_AggrType_struct*
_tmp459=(struct Cyc_Absyn_AggrType_struct*)_tmp458;if(_tmp459->tag != 12)goto
_LL1F8;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp45A.aggr_info;}}_LL1F7: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp45B);goto _LL1F5;_LL1F8:;_LL1F9: {struct Cyc_String_pa_struct _tmpBA8;void*
_tmpBA7[1];const char*_tmpBA6;void*_tmpBA5;(_tmpBA5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBA7[0]=& _tmpBA8,Cyc_aprintf(((
_tmpBA6="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBA6,sizeof(char),51))),_tag_dyneither(_tmpBA7,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBA5,sizeof(void*),0)));}_LL1F5:;}{struct _tuple0*_tmp460=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp461=Cyc_Absyn_var_exp(_tmp460,0);struct Cyc_Absyn_Exp*
_tmp462=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp461,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp464=Cyc_Absyn_neq_exp(_tmp462,_tmp463,0);struct Cyc_Absyn_Exp*
_tmp465=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp461,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp466=Cyc_Absyn_exp_stmt(_tmp465,0);struct Cyc_Absyn_Stmt*
_tmp467=Cyc_Absyn_ifthenelse_stmt(_tmp464,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp468=Cyc_Absyn_declare_stmt(_tmp460,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp467,_tmp466,0),0);return Cyc_Toc_stmt_exp_r(
_tmp468);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp469=e->r;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*_tmp46D;struct
_dyneither_ptr*_tmp46E;int _tmp46F;int*_tmp470;struct Cyc_Absyn_Exp*_tmp472;struct
_dyneither_ptr*_tmp473;int _tmp474;int*_tmp475;_LL1FB: {struct Cyc_Absyn_Cast_e_struct*
_tmp46A=(struct Cyc_Absyn_Cast_e_struct*)_tmp469;if(_tmp46A->tag != 15)goto _LL1FD;
else{_tmp46B=_tmp46A->f2;}}_LL1FC: {const char*_tmpBAB;void*_tmpBAA;(_tmpBAA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAB="cast on lhs!",_tag_dyneither(_tmpBAB,sizeof(char),13))),_tag_dyneither(
_tmpBAA,sizeof(void*),0)));}_LL1FD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp46C=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp469;if(_tmp46C->tag != 22)goto _LL1FF;
else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;_tmp46F=_tmp46C->f4;_tmp470=(int*)&
_tmp46C->f4;}}_LL1FE: {void*_tmp478=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46D->topt))->v);struct Cyc_Absyn_AggrInfo _tmp47A;union Cyc_Absyn_AggrInfoU
_tmp47B;_LL204: {struct Cyc_Absyn_AggrType_struct*_tmp479=(struct Cyc_Absyn_AggrType_struct*)
_tmp478;if(_tmp479->tag != 12)goto _LL206;else{_tmp47A=_tmp479->f1;_tmp47B=_tmp47A.aggr_info;}}
_LL205: {struct Cyc_Absyn_Aggrdecl*_tmp47C=Cyc_Absyn_get_known_aggrdecl(_tmp47B);*
f_tag=Cyc_Toc_get_member_offset(_tmp47C,_tmp46E);{const char*_tmpBB0;void*_tmpBAF[
2];struct Cyc_String_pa_struct _tmpBAE;struct Cyc_String_pa_struct _tmpBAD;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBAD.tag=0,((_tmpBAD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp46E),((_tmpBAE.tag=0,((_tmpBAE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47C->name).f2),((_tmpBAF[0]=&
_tmpBAE,((_tmpBAF[1]=& _tmpBAD,Cyc_aprintf(((_tmpBB0="_union_%s_%s",
_tag_dyneither(_tmpBB0,sizeof(char),13))),_tag_dyneither(_tmpBAF,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBB1;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB1=
_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1[0]=str,_tmpBB1)))));}if(clear_read)*
_tmp470=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47C->impl))->tagged;};}
_LL206:;_LL207: return 0;_LL203:;}_LL1FF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp471=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp469;if(_tmp471->tag != 23)goto
_LL201;else{_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;_tmp474=_tmp471->f4;_tmp475=(
int*)& _tmp471->f4;}}_LL200: {void*_tmp482=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp472->topt))->v);struct Cyc_Absyn_PtrInfo _tmp484;void*_tmp485;
_LL209: {struct Cyc_Absyn_PointerType_struct*_tmp483=(struct Cyc_Absyn_PointerType_struct*)
_tmp482;if(_tmp483->tag != 5)goto _LL20B;else{_tmp484=_tmp483->f1;_tmp485=_tmp484.elt_typ;}}
_LL20A: {void*_tmp486=Cyc_Tcutil_compress(_tmp485);struct Cyc_Absyn_AggrInfo
_tmp488;union Cyc_Absyn_AggrInfoU _tmp489;_LL20E: {struct Cyc_Absyn_AggrType_struct*
_tmp487=(struct Cyc_Absyn_AggrType_struct*)_tmp486;if(_tmp487->tag != 12)goto
_LL210;else{_tmp488=_tmp487->f1;_tmp489=_tmp488.aggr_info;}}_LL20F: {struct Cyc_Absyn_Aggrdecl*
_tmp48A=Cyc_Absyn_get_known_aggrdecl(_tmp489);*f_tag=Cyc_Toc_get_member_offset(
_tmp48A,_tmp473);{const char*_tmpBB6;void*_tmpBB5[2];struct Cyc_String_pa_struct
_tmpBB4;struct Cyc_String_pa_struct _tmpBB3;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp473),((_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp48A->name).f2),((_tmpBB5[0]=& _tmpBB4,((_tmpBB5[1]=&
_tmpBB3,Cyc_aprintf(((_tmpBB6="_union_%s_%s",_tag_dyneither(_tmpBB6,sizeof(char),
13))),_tag_dyneither(_tmpBB5,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBB7;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((
_tmpBB7[0]=str,_tmpBB7)))));}if(clear_read)*_tmp475=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp48A->impl))->tagged;};}_LL210:;_LL211: return 0;_LL20D:;}_LL20B:;
_LL20C: return 0;_LL208:;}_LL201:;_LL202: return 0;_LL1FA:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp490=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp490,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp491=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp491,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp490,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC16(unsigned int*_tmpC15,unsigned int*_tmpC14,struct _tuple0***
_tmpC12){for(*_tmpC15=0;*_tmpC15 < *_tmpC14;(*_tmpC15)++){(*_tmpC12)[*_tmpC15]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp492=e->r;if(e->topt == 0){const char*_tmpBBB;void*_tmpBBA[1];struct Cyc_String_pa_struct
_tmpBB9;(_tmpBB9.tag=0,((_tmpBB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBBA[0]=& _tmpBB9,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="exp_to_c: no type for %s",
_tag_dyneither(_tmpBBB,sizeof(char),25))),_tag_dyneither(_tmpBBA,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp496=
_tmp492;union Cyc_Absyn_Cnst _tmp498;int _tmp499;struct _tuple0*_tmp49C;void*_tmp49D;
struct _tuple0*_tmp49F;enum Cyc_Absyn_Primop _tmp4A1;struct Cyc_List_List*_tmp4A2;
struct Cyc_Absyn_Exp*_tmp4A4;enum Cyc_Absyn_Incrementor _tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A7;struct Cyc_Core_Opt*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*
_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*
_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*
_tmp4B8;struct Cyc_List_List*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_List_List*
_tmp4BC;struct Cyc_Absyn_VarargCallInfo*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;
struct Cyc_List_List*_tmp4C0;struct Cyc_Absyn_VarargCallInfo*_tmp4C1;struct Cyc_Absyn_VarargCallInfo
_tmp4C2;int _tmp4C3;struct Cyc_List_List*_tmp4C4;struct Cyc_Absyn_VarargInfo*
_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*
_tmp4CB;struct Cyc_List_List*_tmp4CC;void*_tmp4CE;void**_tmp4CF;struct Cyc_Absyn_Exp*
_tmp4D0;int _tmp4D1;enum Cyc_Absyn_Coercion _tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;
struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*
_tmp4D9;void*_tmp4DB;void*_tmp4DD;void*_tmp4DE;struct _dyneither_ptr*_tmp4E0;void*
_tmp4E2;void*_tmp4E3;unsigned int _tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E9;struct _dyneither_ptr*_tmp4EA;int _tmp4EB;int _tmp4EC;struct Cyc_Absyn_Exp*
_tmp4EE;struct _dyneither_ptr*_tmp4EF;int _tmp4F0;int _tmp4F1;struct Cyc_Absyn_Exp*
_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_List_List*_tmp4F6;struct Cyc_List_List*
_tmp4F8;struct Cyc_Absyn_Vardecl*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*
_tmp4FC;int _tmp4FD;struct _tuple0*_tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_List_List*
_tmp501;struct Cyc_Absyn_Aggrdecl*_tmp502;void*_tmp504;struct Cyc_List_List*
_tmp505;struct Cyc_List_List*_tmp507;struct Cyc_Absyn_Datatypedecl*_tmp508;struct
Cyc_Absyn_Datatypefield*_tmp509;struct Cyc_Absyn_MallocInfo _tmp50D;int _tmp50E;
struct Cyc_Absyn_Exp*_tmp50F;void**_tmp510;struct Cyc_Absyn_Exp*_tmp511;int _tmp512;
struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*
_tmp517;struct _dyneither_ptr*_tmp518;struct Cyc_Absyn_Stmt*_tmp51A;_LL213: {
struct Cyc_Absyn_Const_e_struct*_tmp497=(struct Cyc_Absyn_Const_e_struct*)_tmp496;
if(_tmp497->tag != 0)goto _LL215;else{_tmp498=_tmp497->f1;if((_tmp498.Null_c).tag
!= 1)goto _LL215;_tmp499=(int)(_tmp498.Null_c).val;}}_LL214: {struct Cyc_Absyn_Exp*
_tmp51E=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp51E,
_tmp51E))->r;else{struct Cyc_Absyn_Exp*_tmpBBC[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBBC[2]=_tmp51E,((_tmpBBC[1]=_tmp51E,((_tmpBBC[0]=_tmp51E,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBC,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL212;}_LL215: {struct Cyc_Absyn_Const_e_struct*
_tmp49A=(struct Cyc_Absyn_Const_e_struct*)_tmp496;if(_tmp49A->tag != 0)goto _LL217;}
_LL216: goto _LL212;_LL217: {struct Cyc_Absyn_Var_e_struct*_tmp49B=(struct Cyc_Absyn_Var_e_struct*)
_tmp496;if(_tmp49B->tag != 1)goto _LL219;else{_tmp49C=_tmp49B->f1;_tmp49D=(void*)
_tmp49B->f2;}}_LL218:{struct _handler_cons _tmp520;_push_handler(& _tmp520);{int
_tmp522=0;if(setjmp(_tmp520.handler))_tmp522=1;if(!_tmp522){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp49C))->r;;_pop_handler();}else{void*_tmp521=(void*)_exn_thrown;void*
_tmp524=_tmp521;_LL26A: {struct Cyc_Dict_Absent_struct*_tmp525=(struct Cyc_Dict_Absent_struct*)
_tmp524;if(_tmp525->tag != Cyc_Dict_Absent)goto _LL26C;}_LL26B: {const char*_tmpBC0;
void*_tmpBBF[1];struct Cyc_String_pa_struct _tmpBBE;(_tmpBBE.tag=0,((_tmpBBE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp49C)),((
_tmpBBF[0]=& _tmpBBE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC0="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBC0,sizeof(char),
32))),_tag_dyneither(_tmpBBF,sizeof(void*),1)))))));}_LL26C:;_LL26D:(void)_throw(
_tmp524);_LL269:;}};}goto _LL212;_LL219: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp49E=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp496;if(_tmp49E->tag != 2)goto
_LL21B;else{_tmp49F=_tmp49E->f1;}}_LL21A: {const char*_tmpBC3;void*_tmpBC2;(
_tmpBC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC3="unknownid",_tag_dyneither(_tmpBC3,sizeof(char),10))),_tag_dyneither(
_tmpBC2,sizeof(void*),0)));}_LL21B: {struct Cyc_Absyn_Primop_e_struct*_tmp4A0=(
struct Cyc_Absyn_Primop_e_struct*)_tmp496;if(_tmp4A0->tag != 3)goto _LL21D;else{
_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}_LL21C: {struct Cyc_List_List*_tmp52B=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4A2);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4A2);switch(_tmp4A1){case Cyc_Absyn_Numelts: _LL26E: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4A2))->hd;{void*_tmp52C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp52E;struct Cyc_Absyn_Exp*
_tmp52F;struct Cyc_Absyn_PtrInfo _tmp531;void*_tmp532;struct Cyc_Absyn_PtrAtts
_tmp533;union Cyc_Absyn_Constraint*_tmp534;union Cyc_Absyn_Constraint*_tmp535;
union Cyc_Absyn_Constraint*_tmp536;_LL271: {struct Cyc_Absyn_ArrayType_struct*
_tmp52D=(struct Cyc_Absyn_ArrayType_struct*)_tmp52C;if(_tmp52D->tag != 9)goto
_LL273;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52E.num_elts;}}_LL272: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp52F))){const char*_tmpBC6;void*_tmpBC5;(
_tmpBC5=0,Cyc_Tcutil_terr(e->loc,((_tmpBC6="can't calculate numelts",
_tag_dyneither(_tmpBC6,sizeof(char),24))),_tag_dyneither(_tmpBC5,sizeof(void*),0)));}
e->r=_tmp52F->r;goto _LL270;_LL273: {struct Cyc_Absyn_PointerType_struct*_tmp530=(
struct Cyc_Absyn_PointerType_struct*)_tmp52C;if(_tmp530->tag != 5)goto _LL275;else{
_tmp531=_tmp530->f1;_tmp532=_tmp531.elt_typ;_tmp533=_tmp531.ptr_atts;_tmp534=
_tmp533.nullable;_tmp535=_tmp533.bounds;_tmp536=_tmp533.zero_term;}}_LL274:{void*
_tmp539=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp535);struct Cyc_Absyn_Exp*_tmp53C;_LL278: {struct Cyc_Absyn_DynEither_b_struct*
_tmp53A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp539;if(_tmp53A->tag != 0)goto
_LL27A;}_LL279:{struct Cyc_Absyn_Exp*_tmpBC7[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBC7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp532),0),((_tmpBC7[0]=(
struct Cyc_Absyn_Exp*)_tmp4A2->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBC7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL277;_LL27A: {struct Cyc_Absyn_Upper_b_struct*_tmp53B=(struct Cyc_Absyn_Upper_b_struct*)
_tmp539;if(_tmp53B->tag != 1)goto _LL277;else{_tmp53C=_tmp53B->f1;}}_LL27B: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp536)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4A2->hd);struct Cyc_Absyn_Exp*_tmpBC8[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBC8[1]=_tmp53C,((_tmpBC8[0]=(struct Cyc_Absyn_Exp*)_tmp4A2->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBC8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp534)){if(!Cyc_Evexp_c_can_eval(_tmp53C)){const
char*_tmpBCB;void*_tmpBCA;(_tmpBCA=0,Cyc_Tcutil_terr(e->loc,((_tmpBCB="can't calculate numelts",
_tag_dyneither(_tmpBCB,sizeof(char),24))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp53C,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp53C->r;goto _LL277;}}goto _LL277;_LL277:;}goto _LL270;_LL275:;_LL276: {const
char*_tmpBD0;void*_tmpBCF[2];struct Cyc_String_pa_struct _tmpBCE;struct Cyc_String_pa_struct
_tmpBCD;(_tmpBCD.tag=0,((_tmpBCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBCE.tag=0,((_tmpBCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBCF[0]=& _tmpBCE,((
_tmpBCF[1]=& _tmpBCD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBD0="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBD0,
sizeof(char),41))),_tag_dyneither(_tmpBCF,sizeof(void*),2)))))))))))));}_LL270:;}
break;}case Cyc_Absyn_Plus: _LL26F:{void*_tmp545=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp52B))->hd);struct Cyc_Absyn_PtrInfo _tmp547;
void*_tmp548;struct Cyc_Absyn_PtrAtts _tmp549;union Cyc_Absyn_Constraint*_tmp54A;
union Cyc_Absyn_Constraint*_tmp54B;_LL27E: {struct Cyc_Absyn_PointerType_struct*
_tmp546=(struct Cyc_Absyn_PointerType_struct*)_tmp545;if(_tmp546->tag != 5)goto
_LL280;else{_tmp547=_tmp546->f1;_tmp548=_tmp547.elt_typ;_tmp549=_tmp547.ptr_atts;
_tmp54A=_tmp549.bounds;_tmp54B=_tmp549.zero_term;}}_LL27F:{void*_tmp54C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp54A);struct Cyc_Absyn_Exp*_tmp54F;_LL283: {struct Cyc_Absyn_DynEither_b_struct*
_tmp54D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp54C;if(_tmp54D->tag != 0)goto
_LL285;}_LL284: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBD1[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD1[2]=e2,((_tmpBD1[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp548),0),((_tmpBD1[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL282;}_LL285: {struct Cyc_Absyn_Upper_b_struct*_tmp54E=(struct
Cyc_Absyn_Upper_b_struct*)_tmp54C;if(_tmp54E->tag != 1)goto _LL282;else{_tmp54F=
_tmp54E->f1;}}_LL286: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp54B)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;struct Cyc_Absyn_Exp*_tmpBD2[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBD2[2]=e2,((_tmpBD2[1]=_tmp54F,((_tmpBD2[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD2,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL282;_LL282:;}goto _LL27D;_LL280:;_LL281: goto _LL27D;
_LL27D:;}break;case Cyc_Absyn_Minus: _LL27C: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp52B))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp52B->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBD3[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD3[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBD3[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBD3[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBD3,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL287: goto _LL288;case Cyc_Absyn_Neq: _LL288: goto _LL289;case Cyc_Absyn_Gt: _LL289:
goto _LL28A;case Cyc_Absyn_Gte: _LL28A: goto _LL28B;case Cyc_Absyn_Lt: _LL28B: goto
_LL28C;case Cyc_Absyn_Lte: _LL28C: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp52B))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp52B->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL28D: break;}goto _LL212;}_LL21D: {
struct Cyc_Absyn_Increment_e_struct*_tmp4A3=(struct Cyc_Absyn_Increment_e_struct*)
_tmp496;if(_tmp4A3->tag != 5)goto _LL21F;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}}
_LL21E: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A4->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBD4;struct _dyneither_ptr incr_str=(_tmpBD4="increment",
_tag_dyneither(_tmpBD4,sizeof(char),10));if(_tmp4A5 == Cyc_Absyn_PreDec  || 
_tmp4A5 == Cyc_Absyn_PostDec){const char*_tmpBD5;incr_str=((_tmpBD5="decrement",
_tag_dyneither(_tmpBD5,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A4,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBD9;void*_tmpBD8[1];
struct Cyc_String_pa_struct _tmpBD7;(_tmpBD7.tag=0,((_tmpBD7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD8[0]=& _tmpBD7,Cyc_Tcutil_terr(
e->loc,((_tmpBD9="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBD9,sizeof(char),74))),_tag_dyneither(_tmpBD8,sizeof(void*),1)))))));}{
const char*_tmpBDC;void*_tmpBDB;(_tmpBDB=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDC="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBDC,sizeof(char),30))),_tag_dyneither(_tmpBDB,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4A4,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBDD;_tmp559->topt=((_tmpBDD=_cycalloc(sizeof(*
_tmpBDD)),((_tmpBDD->v=Cyc_Absyn_sint_typ,_tmpBDD))));}switch(_tmp4A5){case Cyc_Absyn_PreInc:
_LL28F:{struct Cyc_Absyn_AssignOp_e_struct _tmpBE3;struct Cyc_Core_Opt*_tmpBE2;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBE1;e->r=(void*)((_tmpBE1=_cycalloc(
sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE3.tag=4,((_tmpBE3.f1=_tmp4A4,((_tmpBE3.f2=((
_tmpBE2=_cycalloc_atomic(sizeof(*_tmpBE2)),((_tmpBE2->v=(void*)Cyc_Absyn_Plus,
_tmpBE2)))),((_tmpBE3.f3=_tmp559,_tmpBE3)))))))),_tmpBE1))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL290:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBE9;struct Cyc_Core_Opt*_tmpBE8;struct Cyc_Absyn_AssignOp_e_struct*_tmpBE7;e->r=(
void*)((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBE9.tag=4,((
_tmpBE9.f1=_tmp4A4,((_tmpBE9.f2=((_tmpBE8=_cycalloc_atomic(sizeof(*_tmpBE8)),((
_tmpBE8->v=(void*)Cyc_Absyn_Minus,_tmpBE8)))),((_tmpBE9.f3=_tmp559,_tmpBE9)))))))),
_tmpBE7))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL291:{const char*_tmpBED;
void*_tmpBEC[1];struct Cyc_String_pa_struct _tmpBEB;(_tmpBEB.tag=0,((_tmpBEB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBEC[0]=& _tmpBEB,Cyc_Tcutil_terr(
e->loc,((_tmpBED="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBED,sizeof(char),59))),_tag_dyneither(_tmpBEC,sizeof(void*),1)))))));}{
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF0="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBF0,sizeof(char),34))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4A4);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4A5 == Cyc_Absyn_PostInc
 || _tmp4A5 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4A5 == Cyc_Absyn_PreDec  || _tmp4A5 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBF1[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBF1[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBF1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBF1[0]=Cyc_Toc_push_address_exp(_tmp4A4),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp567=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A4->topt))->v);enum Cyc_Absyn_Size_of _tmp569;int _tmp56C;_LL294: {
struct Cyc_Absyn_IntType_struct*_tmp568=(struct Cyc_Absyn_IntType_struct*)_tmp567;
if(_tmp568->tag != 6)goto _LL296;else{_tmp569=_tmp568->f2;}}_LL295: switch(_tmp569){
case Cyc_Absyn_Char_sz: _LL29E: fn_e=_tmp4A5 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL29F: fn_e=
_tmp4A5 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL2A0: fn_e=_tmp4A5 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL2A1: {struct Cyc_Core_Impossible_struct
_tmpBF7;const char*_tmpBF6;struct Cyc_Core_Impossible_struct*_tmpBF5;(int)_throw((
void*)((_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF7.tag=Cyc_Core_Impossible,((
_tmpBF7.f1=((_tmpBF6="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBF6,sizeof(char),44))),_tmpBF7)))),_tmpBF5)))));}}goto _LL293;
_LL296: {struct Cyc_Absyn_FloatType_struct*_tmp56A=(struct Cyc_Absyn_FloatType_struct*)
_tmp567;if(_tmp56A->tag != 7)goto _LL298;}_LL297: fn_e=_tmp4A5 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL293;_LL298: {struct Cyc_Absyn_DoubleType_struct*_tmp56B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp567;if(_tmp56B->tag != 8)goto _LL29A;else{_tmp56C=_tmp56B->f1;}}_LL299: switch(
_tmp56C){case 1: _LL2A3: fn_e=_tmp4A5 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL2A4: fn_e=_tmp4A5 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL293;_LL29A: {struct Cyc_Absyn_PointerType_struct*_tmp56D=(struct Cyc_Absyn_PointerType_struct*)
_tmp567;if(_tmp56D->tag != 5)goto _LL29C;}_LL29B: fn_e=_tmp4A5 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL293;_LL29C:;_LL29D: {struct Cyc_Core_Impossible_struct _tmpBFD;const char*
_tmpBFC;struct Cyc_Core_Impossible_struct*_tmpBFB;(int)_throw((void*)((_tmpBFB=
_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFD.tag=Cyc_Core_Impossible,((
_tmpBFD.f1=((_tmpBFC="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBFC,sizeof(char),64))),_tmpBFD)))),_tmpBFB)))));}_LL293:;}{
struct Cyc_Absyn_Exp*_tmpBFE[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBFE[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBFE[0]=_tmp4A4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFE,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
== (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A4)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4A4,0,Cyc_Toc_incr_lvalue,_tmp4A5);e->r=_tmp4A4->r;}}}goto _LL212;};};}_LL21F: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4A6=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp496;if(_tmp4A6->tag != 4)goto _LL221;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;
_tmp4A9=_tmp4A6->f3;}}_LL220: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A7->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A9->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4A7,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4A7);Cyc_Toc_exp_to_c(nv,
_tmp4A9);e->r=Cyc_Toc_tagged_union_assignop(_tmp4A7,e1_old_typ,_tmp4A8,_tmp4A9,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A7,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4A7,_tmp4A8,_tmp4A9,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4A7);Cyc_Toc_exp_to_c(nv,_tmp4A7);Cyc_Toc_exp_to_c(nv,_tmp4A9);{int done=0;
if(_tmp4A8 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4A8->v){case Cyc_Absyn_Plus: _LL2A6: change=_tmp4A9;break;case Cyc_Absyn_Minus:
_LL2A7: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A9,0);break;default:
_LL2A8: {const char*_tmpC01;void*_tmpC00;(_tmpC00=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC01="bad t ? pointer arithmetic",
_tag_dyneither(_tmpC01,sizeof(char),27))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp578=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpC02[3];e->r=Cyc_Toc_fncall_exp_r(_tmp578,((_tmpC02[2]=change,((
_tmpC02[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC02[0]=Cyc_Toc_push_address_exp(
_tmp4A7),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC02,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4A8->v){case Cyc_Absyn_Plus:
_LL2AA: done=1;{struct Cyc_Absyn_Exp*_tmpC03[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A7),((_tmpC03[1]=_tmp4A9,((
_tmpC03[0]=_tmp4A7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC03,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2AB: {const char*_tmpC06;void*_tmpC05;(_tmpC05=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC06="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC06,sizeof(char),39))),_tag_dyneither(_tmpC05,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4A9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4A9->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4A7)){{struct _tuple18 _tmpC09;struct
_tuple18*_tmpC08;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4A7,0,Cyc_Toc_assignop_lvalue,((_tmpC08=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC08[0]=((_tmpC09.f1=_tmp4A8,((_tmpC09.f2=
_tmp4A9,_tmpC09)))),_tmpC08)))));}e->r=_tmp4A7->r;}}goto _LL212;};};};}_LL221: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4AA=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp496;if(_tmp4AA->tag != 6)goto _LL223;else{_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AA->f2;
_tmp4AD=_tmp4AA->f3;}}_LL222: Cyc_Toc_exp_to_c(nv,_tmp4AB);Cyc_Toc_exp_to_c(nv,
_tmp4AC);Cyc_Toc_exp_to_c(nv,_tmp4AD);goto _LL212;_LL223: {struct Cyc_Absyn_And_e_struct*
_tmp4AE=(struct Cyc_Absyn_And_e_struct*)_tmp496;if(_tmp4AE->tag != 7)goto _LL225;
else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AE->f2;}}_LL224: Cyc_Toc_exp_to_c(nv,
_tmp4AF);Cyc_Toc_exp_to_c(nv,_tmp4B0);goto _LL212;_LL225: {struct Cyc_Absyn_Or_e_struct*
_tmp4B1=(struct Cyc_Absyn_Or_e_struct*)_tmp496;if(_tmp4B1->tag != 8)goto _LL227;
else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;}}_LL226: Cyc_Toc_exp_to_c(nv,
_tmp4B2);Cyc_Toc_exp_to_c(nv,_tmp4B3);goto _LL212;_LL227: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4B4=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp496;if(_tmp4B4->tag != 9)goto _LL229;
else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;}}_LL228: Cyc_Toc_exp_to_c(nv,
_tmp4B5);Cyc_Toc_exp_to_c(nv,_tmp4B6);goto _LL212;_LL229: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4B7=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp496;if(_tmp4B7->tag != 10)goto
_LL22B;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;}}_LL22A: _tmp4BB=_tmp4B8;
_tmp4BC=_tmp4B9;goto _LL22C;_LL22B: {struct Cyc_Absyn_FnCall_e_struct*_tmp4BA=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp496;if(_tmp4BA->tag != 11)goto _LL22D;else{
_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;_tmp4BD=_tmp4BA->f3;if(_tmp4BD != 0)goto
_LL22D;}}_LL22C: Cyc_Toc_exp_to_c(nv,_tmp4BB);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4BC);goto _LL212;_LL22D: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4BE=(struct Cyc_Absyn_FnCall_e_struct*)_tmp496;if(_tmp4BE->tag != 11)goto
_LL22F;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BE->f2;_tmp4C1=_tmp4BE->f3;if(
_tmp4C1 == 0)goto _LL22F;_tmp4C2=*_tmp4C1;_tmp4C3=_tmp4C2.num_varargs;_tmp4C4=
_tmp4C2.injectors;_tmp4C5=_tmp4C2.vai;}}_LL22E: {struct Cyc_Toc_Env _tmp580;struct
_RegionHandle*_tmp581;struct Cyc_Toc_Env*_tmp57F=nv;_tmp580=*_tmp57F;_tmp581=
_tmp580.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4C3,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4C5->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4C0);int num_normargs=num_args - _tmp4C3;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4C0=_tmp4C0->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C0))->hd);{
struct Cyc_List_List*_tmpC0A;new_args=((_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((
_tmpC0A->hd=(struct Cyc_Absyn_Exp*)_tmp4C0->hd,((_tmpC0A->tl=new_args,_tmpC0A))))));};}}{
struct Cyc_Absyn_Exp*_tmpC0D[3];struct Cyc_List_List*_tmpC0C;new_args=((_tmpC0C=
_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC0D[2]=num_varargs_exp,((_tmpC0D[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC0D[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC0D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC0C->tl=
new_args,_tmpC0C))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4BF);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4BF,new_args,0),0);if(_tmp4C5->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp585=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4C5->type));struct Cyc_Absyn_DatatypeInfo _tmp587;union Cyc_Absyn_DatatypeInfoU
_tmp588;struct Cyc_Absyn_Datatypedecl**_tmp589;struct Cyc_Absyn_Datatypedecl*
_tmp58A;_LL2AE: {struct Cyc_Absyn_DatatypeType_struct*_tmp586=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp585;if(_tmp586->tag != 3)goto _LL2B0;else{_tmp587=_tmp586->f1;_tmp588=_tmp587.datatype_info;
if((_tmp588.KnownDatatype).tag != 2)goto _LL2B0;_tmp589=(struct Cyc_Absyn_Datatypedecl**)(
_tmp588.KnownDatatype).val;_tmp58A=*_tmp589;}}_LL2AF: tud=_tmp58A;goto _LL2AD;
_LL2B0:;_LL2B1: {const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC10="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC10,sizeof(char),44))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}
_LL2AD:;}{unsigned int _tmpC15;unsigned int _tmpC14;struct _dyneither_ptr _tmpC13;
struct _tuple0**_tmpC12;unsigned int _tmpC11;struct _dyneither_ptr vs=(_tmpC11=(
unsigned int)_tmp4C3,((_tmpC12=(struct _tuple0**)_region_malloc(_tmp581,
_check_times(sizeof(struct _tuple0*),_tmpC11)),((_tmpC13=_tag_dyneither(_tmpC12,
sizeof(struct _tuple0*),_tmpC11),((((_tmpC14=_tmpC11,_tmpC16(& _tmpC15,& _tmpC14,&
_tmpC12))),_tmpC13)))))));if(_tmp4C3 != 0){struct Cyc_List_List*_tmp58D=0;{int i=
_tmp4C3 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC17;_tmp58D=((_tmpC17=
_cycalloc(sizeof(*_tmpC17)),((_tmpC17->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC17->tl=_tmp58D,_tmpC17))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp58D,0),s,0);{
int i=0;for(0;_tmp4C0 != 0;(((_tmp4C0=_tmp4C0->tl,_tmp4C4=_tmp4C4->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4C0->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp590;struct
_tuple0*_tmp591;struct Cyc_List_List*_tmp592;struct Cyc_Absyn_Datatypefield*
_tmp58F=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4C4))->hd;_tmp590=*_tmp58F;_tmp591=_tmp590.name;_tmp592=_tmp590.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp592))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp591),0),s,0);{const char*_tmpC18;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp591,((_tmpC18="_struct",_tag_dyneither(_tmpC18,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC19[3];struct Cyc_List_List*
_tmp594=(_tmpC19[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC19[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC19[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC19,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4C0
!= 0;(_tmp4C0=_tmp4C0->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4C0->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4C0->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL212;}_LL22F: {struct Cyc_Absyn_Throw_e_struct*_tmp4C6=(struct Cyc_Absyn_Throw_e_struct*)
_tmp496;if(_tmp4C6->tag != 12)goto _LL231;else{_tmp4C7=_tmp4C6->f1;}}_LL230: Cyc_Toc_exp_to_c(
nv,_tmp4C7);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4C7),0))->r;goto _LL212;_LL231: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4C8=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp496;if(_tmp4C8->tag != 13)
goto _LL233;else{_tmp4C9=_tmp4C8->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C9);goto
_LL212;_LL233: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4CA=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp496;if(_tmp4CA->tag != 14)goto _LL235;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CA->f2;}}
_LL234: Cyc_Toc_exp_to_c(nv,_tmp4CB);for(0;_tmp4CC != 0;_tmp4CC=_tmp4CC->tl){enum 
Cyc_Absyn_KindQual _tmp59A=(Cyc_Tcutil_typ_kind((void*)_tmp4CC->hd))->kind;if(
_tmp59A != Cyc_Absyn_EffKind  && _tmp59A != Cyc_Absyn_RgnKind){{void*_tmp59B=Cyc_Tcutil_compress((
void*)_tmp4CC->hd);_LL2B3: {struct Cyc_Absyn_VarType_struct*_tmp59C=(struct Cyc_Absyn_VarType_struct*)
_tmp59B;if(_tmp59C->tag != 2)goto _LL2B5;}_LL2B4: goto _LL2B6;_LL2B5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp59D=(struct Cyc_Absyn_DatatypeType_struct*)_tmp59B;if(_tmp59D->tag != 3)goto
_LL2B7;}_LL2B6: continue;_LL2B7:;_LL2B8: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4CB,0))->r;goto _LL2B2;
_LL2B2:;}break;}}goto _LL212;_LL235: {struct Cyc_Absyn_Cast_e_struct*_tmp4CD=(
struct Cyc_Absyn_Cast_e_struct*)_tmp496;if(_tmp4CD->tag != 15)goto _LL237;else{
_tmp4CE=(void**)& _tmp4CD->f1;_tmp4CF=(void**)((void**)& _tmp4CD->f1);_tmp4D0=
_tmp4CD->f2;_tmp4D1=_tmp4CD->f3;_tmp4D2=_tmp4CD->f4;}}_LL236: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D0->topt))->v;void*new_typ=*_tmp4CF;*
_tmp4CF=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4D0);{struct _tuple17
_tmpC1A;struct _tuple17 _tmp59F=(_tmpC1A.f1=Cyc_Tcutil_compress(old_t2),((_tmpC1A.f2=
Cyc_Tcutil_compress(new_typ),_tmpC1A)));void*_tmp5A0;struct Cyc_Absyn_PtrInfo
_tmp5A2;void*_tmp5A3;struct Cyc_Absyn_PtrInfo _tmp5A5;void*_tmp5A6;struct Cyc_Absyn_PtrInfo
_tmp5A8;void*_tmp5A9;_LL2BA: _tmp5A0=_tmp59F.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5A1=(struct Cyc_Absyn_PointerType_struct*)_tmp5A0;if(_tmp5A1->tag != 5)goto
_LL2BC;else{_tmp5A2=_tmp5A1->f1;}};_tmp5A3=_tmp59F.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5A4=(struct Cyc_Absyn_PointerType_struct*)_tmp5A3;if(_tmp5A4->tag != 5)goto
_LL2BC;else{_tmp5A5=_tmp5A4->f1;}};_LL2BB: {int _tmp5AB=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A2.ptr_atts).nullable);int _tmp5AC=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A5.ptr_atts).nullable);
void*_tmp5AD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A2.ptr_atts).bounds);void*_tmp5AE=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A5.ptr_atts).bounds);
int _tmp5AF=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5A2.ptr_atts).zero_term);int _tmp5B0=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A5.ptr_atts).zero_term);{struct _tuple17 _tmpC1B;
struct _tuple17 _tmp5B2=(_tmpC1B.f1=_tmp5AD,((_tmpC1B.f2=_tmp5AE,_tmpC1B)));void*
_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B5;void*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B8;
void*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BB;void*_tmp5BC;void*_tmp5BE;void*_tmp5C0;
struct Cyc_Absyn_Exp*_tmp5C2;void*_tmp5C3;void*_tmp5C5;_LL2C1: _tmp5B3=_tmp5B2.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B4=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B3;
if(_tmp5B4->tag != 1)goto _LL2C3;else{_tmp5B5=_tmp5B4->f1;}};_tmp5B6=_tmp5B2.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B7=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B6;
if(_tmp5B7->tag != 1)goto _LL2C3;else{_tmp5B8=_tmp5B7->f1;}};_LL2C2: if((!Cyc_Evexp_c_can_eval(
_tmp5B5) || !Cyc_Evexp_c_can_eval(_tmp5B8)) && !Cyc_Evexp_same_const_exp(_tmp5B5,
_tmp5B8)){const char*_tmpC1E;void*_tmpC1D;(_tmpC1D=0,Cyc_Tcutil_terr(e->loc,((
_tmpC1E="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC1E,sizeof(char),54))),_tag_dyneither(_tmpC1D,sizeof(void*),0)));}if(_tmp5AB
 && !_tmp5AC){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC21;void*_tmpC20;(
_tmpC20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC21="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC21,
sizeof(char),44))),_tag_dyneither(_tmpC20,sizeof(void*),0)));}if(_tmp4D2 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC25;void*_tmpC24[1];struct Cyc_String_pa_struct _tmpC23;(_tmpC23.tag=
0,((_tmpC23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC24[0]=& _tmpC23,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC25="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC25,sizeof(char),41))),_tag_dyneither(_tmpC24,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4D0);if(do_null_check){if(!_tmp4D1){
const char*_tmpC28;void*_tmpC27;(_tmpC27=0,Cyc_Tcutil_warn(e->loc,((_tmpC28="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC28,sizeof(char),58))),_tag_dyneither(_tmpC27,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC29;e->r=Cyc_Toc_cast_it_r(*_tmp4CF,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=
_tmp4D0,((_tmpC29->tl=0,_tmpC29)))))),0));};}};}goto _LL2C0;_LL2C3: _tmp5B9=
_tmp5B2.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5BA=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5B9;if(_tmp5BA->tag != 1)goto _LL2C5;else{_tmp5BB=_tmp5BA->f1;}};_tmp5BC=
_tmp5B2.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BC;if(_tmp5BD->tag != 0)goto _LL2C5;};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5BB)){
const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,Cyc_Tcutil_terr(e->loc,((_tmpC2C="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC2C,sizeof(char),71))),_tag_dyneither(_tmpC2B,sizeof(void*),0)));}
if(_tmp4D2 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC30;void*_tmpC2F[1];struct
Cyc_String_pa_struct _tmpC2E;(_tmpC2E.tag=0,((_tmpC2E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC2F[0]=& _tmpC2E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC30="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC30,sizeof(char),44))),_tag_dyneither(_tmpC2F,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5AF  && !(_tmp5A5.elt_tq).real_const) && !
_tmp5B0)_tmp5BB=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5BB,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5BB,_tmp4D0))->r;}else{
struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Toc__tag_dyneither_e;if(_tmp5AF){struct _tuple0*
_tmp5D7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5D8=Cyc_Absyn_var_exp(_tmp5D7,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5D9=_tmp4D0->r;union Cyc_Absyn_Cnst _tmp5DB;
struct _dyneither_ptr _tmp5DC;_LL2CA: {struct Cyc_Absyn_Const_e_struct*_tmp5DA=(
struct Cyc_Absyn_Const_e_struct*)_tmp5D9;if(_tmp5DA->tag != 0)goto _LL2CC;else{
_tmp5DB=_tmp5DA->f1;if((_tmp5DB.String_c).tag != 7)goto _LL2CC;_tmp5DC=(struct
_dyneither_ptr)(_tmp5DB.String_c).val;}}_LL2CB: arg3=_tmp5BB;goto _LL2C9;_LL2CC:;
_LL2CD:{struct Cyc_Absyn_Exp*_tmpC31[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4D0),((_tmpC31[1]=_tmp5BB,((_tmpC31[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5D8),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC31,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C9;_LL2C9:;}if(!_tmp5B0  && !(_tmp5A5.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5DE=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A5.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC32[3];struct Cyc_Absyn_Exp*_tmp5DF=Cyc_Absyn_fncall_exp(_tmp5D6,((_tmpC32[2]=
arg3,((_tmpC32[1]=_tmp5DE,((_tmpC32[0]=_tmp5D8,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC32,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5E0=Cyc_Absyn_exp_stmt(_tmp5DF,0);_tmp5E0=
Cyc_Absyn_declare_stmt(_tmp5D7,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4D0,_tmp5E0,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5E0);};}else{struct Cyc_Absyn_Exp*
_tmpC33[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D6,((_tmpC33[2]=_tmp5BB,((_tmpC33[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A5.elt_typ),0),((_tmpC33[0]=
_tmp4D0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C0;
_LL2C5: _tmp5BE=_tmp5B2.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BE;if(_tmp5BF->tag != 0)goto _LL2C7;};_tmp5C0=_tmp5B2.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5C1=(struct Cyc_Absyn_Upper_b_struct*)_tmp5C0;if(_tmp5C1->tag != 1)goto _LL2C7;
else{_tmp5C2=_tmp5C1->f1;}};_LL2C6: if(!Cyc_Evexp_c_can_eval(_tmp5C2)){const char*
_tmpC36;void*_tmpC35;(_tmpC35=0,Cyc_Tcutil_terr(e->loc,((_tmpC36="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC36,sizeof(char),71))),_tag_dyneither(_tmpC35,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC39;void*_tmpC38;(_tmpC38=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC39="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC39,sizeof(char),45))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E7=_tmp5C2;if(_tmp5AF  && !_tmp5B0)_tmp5E7=Cyc_Absyn_add_exp(
_tmp5C2,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5E8=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC3A[3];struct Cyc_Absyn_Exp*_tmp5E9=Cyc_Absyn_fncall_exp(
_tmp5E8,((_tmpC3A[2]=_tmp5E7,((_tmpC3A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5A2.elt_typ),0),((_tmpC3A[0]=_tmp4D0,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5AC){struct Cyc_List_List*_tmpC3B;_tmp5E9->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B->hd=Cyc_Absyn_copy_exp(
_tmp5E9),((_tmpC3B->tl=0,_tmpC3B)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4CF,_tmp5E9);
goto _LL2C0;};};_LL2C7: _tmp5C3=_tmp5B2.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5C4=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C3;if(_tmp5C4->tag != 0)goto
_LL2C0;};_tmp5C5=_tmp5B2.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C6=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5C5;if(_tmp5C6->tag != 0)goto _LL2C0;};_LL2C8:
DynCast: if((_tmp5AF  && !_tmp5B0) && !(_tmp5A5.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC3E;void*_tmpC3D;(_tmpC3D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC3E="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC3E,sizeof(char),70))),_tag_dyneither(_tmpC3D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5EE=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC3F[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5EE,((_tmpC3F[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC3F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A2.elt_typ),0),((
_tmpC3F[0]=_tmp4D0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C0;
_LL2C0:;}goto _LL2B9;}_LL2BC: _tmp5A6=_tmp59F.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5A7=(struct Cyc_Absyn_PointerType_struct*)_tmp5A6;if(_tmp5A7->tag != 5)goto
_LL2BE;else{_tmp5A8=_tmp5A7->f1;}};_tmp5A9=_tmp59F.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5AA=(struct Cyc_Absyn_IntType_struct*)_tmp5A9;if(_tmp5AA->tag != 6)goto _LL2BE;};
_LL2BD:{void*_tmp5F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A8.ptr_atts).bounds);_LL2CF: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5F1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5F0;if(_tmp5F1->tag != 0)goto
_LL2D1;}_LL2D0: _tmp4D0->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4D0->r,
_tmp4D0->loc),Cyc_Toc_curr_sp);goto _LL2CE;_LL2D1:;_LL2D2: goto _LL2CE;_LL2CE:;}
goto _LL2B9;_LL2BE:;_LL2BF: goto _LL2B9;_LL2B9:;}goto _LL212;}_LL237: {struct Cyc_Absyn_Address_e_struct*
_tmp4D3=(struct Cyc_Absyn_Address_e_struct*)_tmp496;if(_tmp4D3->tag != 16)goto
_LL239;else{_tmp4D4=_tmp4D3->f1;}}_LL238:{void*_tmp5F2=_tmp4D4->r;struct _tuple0*
_tmp5F4;struct Cyc_List_List*_tmp5F5;struct Cyc_List_List*_tmp5F6;struct Cyc_List_List*
_tmp5F8;_LL2D4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F3=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5F2;if(_tmp5F3->tag != 29)goto _LL2D6;else{_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;
_tmp5F6=_tmp5F3->f3;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC43;void*
_tmpC42[1];struct Cyc_String_pa_struct _tmpC41;(_tmpC41.tag=0,((_tmpC41.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D4->loc)),((
_tmpC42[0]=& _tmpC41,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC43="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC43,
sizeof(char),42))),_tag_dyneither(_tmpC42,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v,_tmp5F5 != 0,1,0,
_tmp5F6,_tmp5F4))->r;goto _LL2D3;_LL2D6: {struct Cyc_Absyn_Tuple_e_struct*_tmp5F7=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5F2;if(_tmp5F7->tag != 25)goto _LL2D8;else{
_tmp5F8=_tmp5F7->f1;}}_LL2D7: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC47;void*
_tmpC46[1];struct Cyc_String_pa_struct _tmpC45;(_tmpC45.tag=0,((_tmpC45.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D4->loc)),((
_tmpC46[0]=& _tmpC45,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC47="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC47,
sizeof(char),42))),_tag_dyneither(_tmpC46,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5F8))->r;goto _LL2D3;_LL2D8:;_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp4D4);if(!
Cyc_Absyn_is_lvalue(_tmp4D4)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4D4,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D4);}goto _LL2D3;
_LL2D3:;}goto _LL212;_LL239: {struct Cyc_Absyn_New_e_struct*_tmp4D5=(struct Cyc_Absyn_New_e_struct*)
_tmp496;if(_tmp4D5->tag != 17)goto _LL23B;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;}}
_LL23A: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC4B;void*_tmpC4A[1];struct Cyc_String_pa_struct
_tmpC49;(_tmpC49.tag=0,((_tmpC49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D7->loc)),((_tmpC4A[0]=& _tmpC49,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC4B="%s: new at top-level",
_tag_dyneither(_tmpC4B,sizeof(char),21))),_tag_dyneither(_tmpC4A,sizeof(void*),1)))))));}{
void*_tmp602=_tmp4D7->r;struct Cyc_List_List*_tmp604;struct Cyc_Absyn_Vardecl*
_tmp606;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_Absyn_Exp*_tmp608;int _tmp609;
struct _tuple0*_tmp60B;struct Cyc_List_List*_tmp60C;struct Cyc_List_List*_tmp60D;
struct Cyc_Absyn_Aggrdecl*_tmp60E;struct Cyc_List_List*_tmp610;_LL2DB: {struct Cyc_Absyn_Array_e_struct*
_tmp603=(struct Cyc_Absyn_Array_e_struct*)_tmp602;if(_tmp603->tag != 27)goto _LL2DD;
else{_tmp604=_tmp603->f1;}}_LL2DC: {struct _tuple0*_tmp611=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp612=Cyc_Absyn_var_exp(_tmp611,0);struct Cyc_Absyn_Stmt*
_tmp613=Cyc_Toc_init_array(nv,_tmp612,_tmp604,Cyc_Absyn_exp_stmt(_tmp612,0));
void*old_elt_typ;{void*_tmp614=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp616;void*_tmp617;struct Cyc_Absyn_Tqual _tmp618;struct Cyc_Absyn_PtrAtts _tmp619;
union Cyc_Absyn_Constraint*_tmp61A;_LL2E6: {struct Cyc_Absyn_PointerType_struct*
_tmp615=(struct Cyc_Absyn_PointerType_struct*)_tmp614;if(_tmp615->tag != 5)goto
_LL2E8;else{_tmp616=_tmp615->f1;_tmp617=_tmp616.elt_typ;_tmp618=_tmp616.elt_tq;
_tmp619=_tmp616.ptr_atts;_tmp61A=_tmp619.zero_term;}}_LL2E7: old_elt_typ=_tmp617;
goto _LL2E5;_LL2E8:;_LL2E9: {const char*_tmpC4E;void*_tmpC4D;old_elt_typ=((_tmpC4D=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4E="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC4E,sizeof(char),52))),_tag_dyneither(_tmpC4D,sizeof(void*),0))));}_LL2E5:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp61D=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp61E=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp604),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D6 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp61E);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4D6;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp61E);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp611,_tmp61D,(struct Cyc_Absyn_Exp*)e1,_tmp613,0));goto
_LL2DA;};}_LL2DD: {struct Cyc_Absyn_Comprehension_e_struct*_tmp605=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp602;if(_tmp605->tag != 28)goto _LL2DF;else{_tmp606=_tmp605->f1;_tmp607=_tmp605->f2;
_tmp608=_tmp605->f3;_tmp609=_tmp605->f4;}}_LL2DE: {int is_dyneither_ptr=0;{void*
_tmp61F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp621;void*_tmp622;
struct Cyc_Absyn_Tqual _tmp623;struct Cyc_Absyn_PtrAtts _tmp624;union Cyc_Absyn_Constraint*
_tmp625;union Cyc_Absyn_Constraint*_tmp626;_LL2EB: {struct Cyc_Absyn_PointerType_struct*
_tmp620=(struct Cyc_Absyn_PointerType_struct*)_tmp61F;if(_tmp620->tag != 5)goto
_LL2ED;else{_tmp621=_tmp620->f1;_tmp622=_tmp621.elt_typ;_tmp623=_tmp621.elt_tq;
_tmp624=_tmp621.ptr_atts;_tmp625=_tmp624.bounds;_tmp626=_tmp624.zero_term;}}
_LL2EC: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp625)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2EA;
_LL2ED:;_LL2EE: {const char*_tmpC51;void*_tmpC50;(_tmpC50=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC51="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC51,sizeof(char),42))),_tag_dyneither(_tmpC50,sizeof(void*),0)));}
_LL2EA:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp608->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp607);{struct Cyc_Absyn_Exp*_tmp629=
Cyc_Absyn_var_exp(max,0);if(_tmp609)_tmp629=Cyc_Absyn_add_exp(_tmp629,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp606,Cyc_Absyn_var_exp(max,0),_tmp608,_tmp609,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp62B;struct _RegionHandle*_tmp62C;struct Cyc_Toc_Env*_tmp62A=
nv;_tmp62B=*_tmp62A;_tmp62C=_tmp62B.rgn;{struct _tuple19*_tmpC54;struct Cyc_List_List*
_tmpC53;struct Cyc_List_List*decls=(_tmpC53=_region_malloc(_tmp62C,sizeof(*
_tmpC53)),((_tmpC53->hd=((_tmpC54=_region_malloc(_tmp62C,sizeof(*_tmpC54)),((
_tmpC54->f1=max,((_tmpC54->f2=Cyc_Absyn_uint_typ,((_tmpC54->f3=(struct Cyc_Absyn_Exp*)
_tmp607,_tmpC54)))))))),((_tmpC53->tl=0,_tmpC53)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4D6 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC55[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC55[1]=_tmp629,((
_tmpC55[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D6;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC56[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC56[1]=_tmp629,((_tmpC56[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC56,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC59;struct Cyc_List_List*
_tmpC58;decls=((_tmpC58=_region_malloc(_tmp62C,sizeof(*_tmpC58)),((_tmpC58->hd=((
_tmpC59=_region_malloc(_tmp62C,sizeof(*_tmpC59)),((_tmpC59->f1=a,((_tmpC59->f2=
ptr_typ,((_tmpC59->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC59)))))))),((_tmpC58->tl=
decls,_tmpC58))))));}if(is_dyneither_ptr){struct _tuple0*_tmp631=Cyc_Toc_temp_var();
void*_tmp632=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp633=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC5A[3];struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_fncall_exp(
_tmp633,((_tmpC5A[2]=_tmp629,((_tmpC5A[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC5A[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC5D;struct Cyc_List_List*_tmpC5C;decls=((_tmpC5C=
_region_malloc(_tmp62C,sizeof(*_tmpC5C)),((_tmpC5C->hd=((_tmpC5D=_region_malloc(
_tmp62C,sizeof(*_tmpC5D)),((_tmpC5D->f1=_tmp631,((_tmpC5D->f2=_tmp632,((_tmpC5D->f3=(
struct Cyc_Absyn_Exp*)_tmp634,_tmpC5D)))))))),((_tmpC5C->tl=decls,_tmpC5C))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp631,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp638=decls;for(0;_tmp638 != 0;_tmp638=_tmp638->tl){struct
_tuple0*_tmp63A;void*_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct _tuple19 _tmp639=*((
struct _tuple19*)_tmp638->hd);_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;_tmp63C=
_tmp639.f3;s=Cyc_Absyn_declare_stmt(_tmp63A,_tmp63B,_tmp63C,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2DA;};};};}_LL2DF: {struct Cyc_Absyn_Aggregate_e_struct*_tmp60A=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp602;if(_tmp60A->tag != 29)goto _LL2E1;
else{_tmp60B=_tmp60A->f1;_tmp60C=_tmp60A->f2;_tmp60D=_tmp60A->f3;_tmp60E=_tmp60A->f4;}}
_LL2E0: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D7->topt))->v,_tmp60C != 0,1,_tmp4D6,_tmp60D,_tmp60B))->r;goto _LL2DA;_LL2E1: {
struct Cyc_Absyn_Tuple_e_struct*_tmp60F=(struct Cyc_Absyn_Tuple_e_struct*)_tmp602;
if(_tmp60F->tag != 25)goto _LL2E3;else{_tmp610=_tmp60F->f1;}}_LL2E2: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4D6,_tmp610))->r;goto _LL2DA;_LL2E3:;_LL2E4: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D7->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp63F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp640=Cyc_Absyn_var_exp(_tmp63F,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp640,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4D6
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D6;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp641=_tmp4D7->r;void*_tmp643;
struct Cyc_Absyn_Exp*_tmp644;_LL2F0: {struct Cyc_Absyn_Cast_e_struct*_tmp642=(
struct Cyc_Absyn_Cast_e_struct*)_tmp641;if(_tmp642->tag != 15)goto _LL2F2;else{
_tmp643=(void*)_tmp642->f1;_tmp644=_tmp642->f2;}}_LL2F1:{struct _tuple17 _tmpC5E;
struct _tuple17 _tmp646=(_tmpC5E.f1=Cyc_Tcutil_compress(_tmp643),((_tmpC5E.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp644->topt))->v),_tmpC5E)));void*
_tmp647;struct Cyc_Absyn_PtrInfo _tmp649;void*_tmp64A;struct Cyc_Absyn_PtrAtts
_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;void*_tmp64D;struct Cyc_Absyn_PtrInfo
_tmp64F;struct Cyc_Absyn_PtrAtts _tmp650;union Cyc_Absyn_Constraint*_tmp651;_LL2F5:
_tmp647=_tmp646.f1;{struct Cyc_Absyn_PointerType_struct*_tmp648=(struct Cyc_Absyn_PointerType_struct*)
_tmp647;if(_tmp648->tag != 5)goto _LL2F7;else{_tmp649=_tmp648->f1;_tmp64A=_tmp649.elt_typ;
_tmp64B=_tmp649.ptr_atts;_tmp64C=_tmp64B.bounds;}};_tmp64D=_tmp646.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp64E=(struct Cyc_Absyn_PointerType_struct*)_tmp64D;if(_tmp64E->tag != 5)goto
_LL2F7;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64F.ptr_atts;_tmp651=_tmp650.bounds;}};
_LL2F6:{struct _tuple17 _tmpC5F;struct _tuple17 _tmp653=(_tmpC5F.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp64C),((_tmpC5F.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp651),_tmpC5F)));void*_tmp654;void*_tmp656;struct Cyc_Absyn_Exp*
_tmp658;_LL2FA: _tmp654=_tmp653.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp655=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp654;if(_tmp655->tag != 0)goto _LL2FC;};
_tmp656=_tmp653.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp657=(struct Cyc_Absyn_Upper_b_struct*)
_tmp656;if(_tmp657->tag != 1)goto _LL2FC;else{_tmp658=_tmp657->f1;}};_LL2FB: Cyc_Toc_exp_to_c(
nv,_tmp644);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp659=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC60[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp659,((_tmpC60[3]=_tmp658,((_tmpC60[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp64A),0),((_tmpC60[1]=_tmp644,((_tmpC60[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC60,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F9;};_LL2FC:;_LL2FD: goto _LL2F9;_LL2F9:;}goto _LL2F4;_LL2F7:;
_LL2F8: goto _LL2F4;_LL2F4:;}goto _LL2EF;_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp65B=Cyc_Absyn_exp_stmt(_tmp640,0);struct Cyc_Absyn_Exp*
_tmp65C=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4D7);_tmp65B=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp640,_tmp65C,0),_tmp4D7,0),
_tmp65B,0);{void*_tmp65D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63F,_tmp65D,(struct Cyc_Absyn_Exp*)mexp,_tmp65B,0));};}
goto _LL2DA;};}_LL2DA:;}goto _LL212;_LL23B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4D8=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp496;if(_tmp4D8->tag != 19)goto
_LL23D;else{_tmp4D9=_tmp4D8->f1;}}_LL23C: Cyc_Toc_exp_to_c(nv,_tmp4D9);goto _LL212;
_LL23D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4DA=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp496;if(_tmp4DA->tag != 18)goto _LL23F;else{_tmp4DB=(void*)_tmp4DA->f1;}}_LL23E:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC63;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC62;e->r=(void*)((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC63.tag=
18,((_tmpC63.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DB),_tmpC63)))),_tmpC62))));}
goto _LL212;_LL23F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4DC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp496;if(_tmp4DC->tag != 20)goto _LL241;else{_tmp4DD=(void*)_tmp4DC->f1;_tmp4DE=(
void*)_tmp4DC->f2;{struct Cyc_Absyn_StructField_struct*_tmp4DF=(struct Cyc_Absyn_StructField_struct*)
_tmp4DE;if(_tmp4DF->tag != 0)goto _LL241;else{_tmp4E0=_tmp4DF->f1;}};}}_LL240:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC6D;struct Cyc_Absyn_StructField_struct
_tmpC6C;struct Cyc_Absyn_StructField_struct*_tmpC6B;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC6A;e->r=(void*)((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6D.tag=
20,((_tmpC6D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DD),((_tmpC6D.f2=(void*)((
void*)((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6C.tag=0,((
_tmpC6C.f1=_tmp4E0,_tmpC6C)))),_tmpC6B))))),_tmpC6D)))))),_tmpC6A))));}goto
_LL212;_LL241: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4E1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp496;if(_tmp4E1->tag != 20)goto _LL243;else{_tmp4E2=(void*)_tmp4E1->f1;_tmp4E3=(
void*)_tmp4E1->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4E4=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4E3;if(_tmp4E4->tag != 1)goto _LL243;else{_tmp4E5=_tmp4E4->f1;}};}}_LL242:{
void*_tmp664=Cyc_Tcutil_compress(_tmp4E2);struct Cyc_Absyn_AggrInfo _tmp666;union
Cyc_Absyn_AggrInfoU _tmp667;struct Cyc_List_List*_tmp669;_LL2FF: {struct Cyc_Absyn_AggrType_struct*
_tmp665=(struct Cyc_Absyn_AggrType_struct*)_tmp664;if(_tmp665->tag != 12)goto
_LL301;else{_tmp666=_tmp665->f1;_tmp667=_tmp666.aggr_info;}}_LL300: {struct Cyc_Absyn_Aggrdecl*
_tmp66C=Cyc_Absyn_get_known_aggrdecl(_tmp667);if(_tmp66C->impl == 0){const char*
_tmpC70;void*_tmpC6F;(_tmpC6F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC70="struct fields must be known",
_tag_dyneither(_tmpC70,sizeof(char),28))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}
_tmp669=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66C->impl))->fields;goto
_LL302;}_LL301: {struct Cyc_Absyn_AnonAggrType_struct*_tmp668=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp664;if(_tmp668->tag != 13)goto _LL303;else{_tmp669=_tmp668->f2;}}_LL302: {
struct Cyc_Absyn_Aggrfield*_tmp66F=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp669,(int)_tmp4E5);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC7A;struct Cyc_Absyn_StructField_struct _tmpC79;struct Cyc_Absyn_StructField_struct*
_tmpC78;struct Cyc_Absyn_Offsetof_e_struct*_tmpC77;e->r=(void*)((_tmpC77=
_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC7A.tag=20,((_tmpC7A.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC7A.f2=(void*)((void*)((_tmpC78=_cycalloc(
sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=0,((_tmpC79.f1=_tmp66F->name,
_tmpC79)))),_tmpC78))))),_tmpC7A)))))),_tmpC77))));}goto _LL2FE;}_LL303: {struct
Cyc_Absyn_TupleType_struct*_tmp66A=(struct Cyc_Absyn_TupleType_struct*)_tmp664;
if(_tmp66A->tag != 11)goto _LL305;}_LL304:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC84;struct Cyc_Absyn_StructField_struct _tmpC83;struct Cyc_Absyn_StructField_struct*
_tmpC82;struct Cyc_Absyn_Offsetof_e_struct*_tmpC81;e->r=(void*)((_tmpC81=
_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC84.tag=20,((_tmpC84.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC84.f2=(void*)((void*)((_tmpC82=_cycalloc(
sizeof(*_tmpC82)),((_tmpC82[0]=((_tmpC83.tag=0,((_tmpC83.f1=Cyc_Absyn_fieldname((
int)(_tmp4E5 + 1)),_tmpC83)))),_tmpC82))))),_tmpC84)))))),_tmpC81))));}goto _LL2FE;
_LL305: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp66B=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp664;if(_tmp66B->tag != 4)goto _LL307;}_LL306: if(_tmp4E5 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC8E;struct Cyc_Absyn_StructField_struct _tmpC8D;struct Cyc_Absyn_StructField_struct*
_tmpC8C;struct Cyc_Absyn_Offsetof_e_struct*_tmpC8B;e->r=(void*)((_tmpC8B=
_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B[0]=((_tmpC8E.tag=20,((_tmpC8E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC8E.f2=(void*)((void*)((_tmpC8C=_cycalloc(
sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8D.tag=0,((_tmpC8D.f1=Cyc_Toc_tag_sp,
_tmpC8D)))),_tmpC8C))))),_tmpC8E)))))),_tmpC8B))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC98;struct Cyc_Absyn_StructField_struct _tmpC97;struct Cyc_Absyn_StructField_struct*
_tmpC96;struct Cyc_Absyn_Offsetof_e_struct*_tmpC95;e->r=(void*)((_tmpC95=
_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC98.tag=20,((_tmpC98.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC98.f2=(void*)((void*)((_tmpC96=_cycalloc(
sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=0,((_tmpC97.f1=Cyc_Absyn_fieldname((
int)_tmp4E5),_tmpC97)))),_tmpC96))))),_tmpC98)))))),_tmpC95))));}goto _LL2FE;
_LL307:;_LL308: {const char*_tmpC9B;void*_tmpC9A;(_tmpC9A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9B="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC9B,sizeof(char),41))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}
_LL2FE:;}goto _LL212;_LL243: {struct Cyc_Absyn_Deref_e_struct*_tmp4E6=(struct Cyc_Absyn_Deref_e_struct*)
_tmp496;if(_tmp4E6->tag != 21)goto _LL245;else{_tmp4E7=_tmp4E6->f1;}}_LL244: {void*
_tmp682=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E7->topt))->v);{
void*_tmp683=_tmp682;struct Cyc_Absyn_PtrInfo _tmp685;void*_tmp686;struct Cyc_Absyn_Tqual
_tmp687;struct Cyc_Absyn_PtrAtts _tmp688;void*_tmp689;union Cyc_Absyn_Constraint*
_tmp68A;union Cyc_Absyn_Constraint*_tmp68B;union Cyc_Absyn_Constraint*_tmp68C;
_LL30A: {struct Cyc_Absyn_PointerType_struct*_tmp684=(struct Cyc_Absyn_PointerType_struct*)
_tmp683;if(_tmp684->tag != 5)goto _LL30C;else{_tmp685=_tmp684->f1;_tmp686=_tmp685.elt_typ;
_tmp687=_tmp685.elt_tq;_tmp688=_tmp685.ptr_atts;_tmp689=_tmp688.rgn;_tmp68A=
_tmp688.nullable;_tmp68B=_tmp688.bounds;_tmp68C=_tmp688.zero_term;}}_LL30B:{void*
_tmp68D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp68B);struct Cyc_Absyn_Exp*_tmp68F;_LL30F: {struct Cyc_Absyn_Upper_b_struct*
_tmp68E=(struct Cyc_Absyn_Upper_b_struct*)_tmp68D;if(_tmp68E->tag != 1)goto _LL311;
else{_tmp68F=_tmp68E->f1;}}_LL310: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4E7);Cyc_Toc_exp_to_c(nv,_tmp4E7);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC9E;void*_tmpC9D;(_tmpC9D=0,Cyc_Tcutil_warn(e->loc,((_tmpC9E="inserted null check due to dereference",
_tag_dyneither(_tmpC9E,sizeof(char),39))),_tag_dyneither(_tmpC9D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC9F;_tmp4E7->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp682),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC9F=_cycalloc(sizeof(*
_tmpC9F)),((_tmpC9F->hd=Cyc_Absyn_copy_exp(_tmp4E7),((_tmpC9F->tl=0,_tmpC9F)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp68C)){unsigned int _tmp695;int _tmp696;struct _tuple10 _tmp694=Cyc_Evexp_eval_const_uint_exp(
_tmp68F);_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;if(!_tmp696  || _tmp695 <= 0){const
char*_tmpCA2;void*_tmpCA1;(_tmpCA1=0,Cyc_Tcutil_terr(e->loc,((_tmpCA2="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCA2,sizeof(char),42))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}}
goto _LL30E;}_LL311: {struct Cyc_Absyn_DynEither_b_struct*_tmp690=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp68D;if(_tmp690->tag != 0)goto _LL30E;}_LL312: {struct Cyc_Absyn_Exp*_tmp699=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCA3;_tmp699->topt=((_tmpCA3=_cycalloc(sizeof(*
_tmpCA3)),((_tmpCA3->v=Cyc_Absyn_uint_typ,_tmpCA3))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4E7,_tmp699);Cyc_Toc_exp_to_c(nv,e);goto _LL30E;}_LL30E:;}goto _LL309;_LL30C:;
_LL30D: {const char*_tmpCA6;void*_tmpCA5;(_tmpCA5=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA6="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCA6,sizeof(char),29))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}
_LL309:;}goto _LL212;}_LL245: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4E8=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp496;if(_tmp4E8->tag != 22)goto _LL247;
else{_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;_tmp4EB=_tmp4E8->f3;_tmp4EC=_tmp4E8->f4;}}
_LL246: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4E9->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4E9);if(
_tmp4EB  && _tmp4EC)e->r=Cyc_Toc_check_tagged_union(_tmp4E9,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4EA,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL212;}_LL247: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4ED=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp496;if(_tmp4ED->tag != 23)goto _LL249;
else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;_tmp4F0=_tmp4ED->f3;_tmp4F1=_tmp4ED->f4;}}
_LL248: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4EE->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4EE);Cyc_Toc_exp_to_c(
nv,_tmp4EE);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp69E;struct Cyc_Absyn_Tqual
_tmp69F;struct Cyc_Absyn_PtrAtts _tmp6A0;void*_tmp6A1;union Cyc_Absyn_Constraint*
_tmp6A2;union Cyc_Absyn_Constraint*_tmp6A3;union Cyc_Absyn_Constraint*_tmp6A4;
struct Cyc_Absyn_PtrInfo _tmp69D=Cyc_Toc_get_ptr_type(e1typ);_tmp69E=_tmp69D.elt_typ;
_tmp69F=_tmp69D.elt_tq;_tmp6A0=_tmp69D.ptr_atts;_tmp6A1=_tmp6A0.rgn;_tmp6A2=
_tmp6A0.nullable;_tmp6A3=_tmp6A0.bounds;_tmp6A4=_tmp6A0.zero_term;{void*_tmp6A5=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6A3);struct Cyc_Absyn_Exp*_tmp6A7;_LL314: {struct Cyc_Absyn_Upper_b_struct*
_tmp6A6=(struct Cyc_Absyn_Upper_b_struct*)_tmp6A5;if(_tmp6A6->tag != 1)goto _LL316;
else{_tmp6A7=_tmp6A6->f1;}}_LL315: {unsigned int _tmp6AA;int _tmp6AB;struct
_tuple10 _tmp6A9=Cyc_Evexp_eval_const_uint_exp(_tmp6A7);_tmp6AA=_tmp6A9.f1;
_tmp6AB=_tmp6A9.f2;if(_tmp6AB){if(_tmp6AA < 1){const char*_tmpCA9;void*_tmpCA8;(
_tmpCA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCA9="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCA9,
sizeof(char),44))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCAC;void*_tmpCAB;(_tmpCAB=0,Cyc_Tcutil_warn(
e->loc,((_tmpCAC="inserted null check due to dereference",_tag_dyneither(_tmpCAC,
sizeof(char),39))),_tag_dyneither(_tmpCAB,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAD[1];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCAD[0]=Cyc_Absyn_new_exp(_tmp4EE->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAD,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6A7)){const char*_tmpCB0;void*
_tmpCAF;(_tmpCAF=0,Cyc_Tcutil_terr(e->loc,((_tmpCB0="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCB0,sizeof(char),47))),_tag_dyneither(_tmpCAF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCB1[4];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCB1[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCB1[2]=Cyc_Absyn_sizeoftyp_exp(_tmp69E,0),((
_tmpCB1[1]=_tmp6A7,((_tmpCB1[0]=Cyc_Absyn_new_exp(_tmp4EE->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB1,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL313;}_LL316: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6A8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6A5;if(_tmp6A8->tag != 0)goto
_LL313;}_LL317: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp69E);{struct Cyc_Absyn_Exp*
_tmpCB2[3];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp69F),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCB2[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCB2[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCB2[0]=Cyc_Absyn_new_exp(_tmp4EE->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCB2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL313;}_LL313:;}if(
_tmp4F0  && _tmp4F1)e->r=Cyc_Toc_check_tagged_union(_tmp4EE,Cyc_Toc_typ_to_c(
e1typ),_tmp69E,_tmp4EF,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4F1)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL212;};}_LL249: {struct Cyc_Absyn_Subscript_e_struct*_tmp4F2=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp496;if(_tmp4F2->tag != 24)goto _LL24B;
else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}}_LL24A: {void*_tmp6B5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F3->topt))->v);{void*_tmp6B6=
_tmp6B5;struct Cyc_List_List*_tmp6B8;struct Cyc_Absyn_PtrInfo _tmp6BA;void*_tmp6BB;
struct Cyc_Absyn_Tqual _tmp6BC;struct Cyc_Absyn_PtrAtts _tmp6BD;void*_tmp6BE;union
Cyc_Absyn_Constraint*_tmp6BF;union Cyc_Absyn_Constraint*_tmp6C0;union Cyc_Absyn_Constraint*
_tmp6C1;_LL319: {struct Cyc_Absyn_TupleType_struct*_tmp6B7=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B6;if(_tmp6B7->tag != 11)goto _LL31B;else{_tmp6B8=_tmp6B7->f1;}}_LL31A: Cyc_Toc_exp_to_c(
nv,_tmp4F3);Cyc_Toc_exp_to_c(nv,_tmp4F4);{unsigned int _tmp6C3;int _tmp6C4;struct
_tuple10 _tmp6C2=Cyc_Evexp_eval_const_uint_exp(_tmp4F4);_tmp6C3=_tmp6C2.f1;
_tmp6C4=_tmp6C2.f2;if(!_tmp6C4){const char*_tmpCB5;void*_tmpCB4;(_tmpCB4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB5="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCB5,sizeof(char),44))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F3,Cyc_Absyn_fieldname((int)(_tmp6C3 + 1)));
goto _LL318;};_LL31B: {struct Cyc_Absyn_PointerType_struct*_tmp6B9=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B6;if(_tmp6B9->tag != 5)goto _LL31D;else{_tmp6BA=_tmp6B9->f1;_tmp6BB=_tmp6BA.elt_typ;
_tmp6BC=_tmp6BA.elt_tq;_tmp6BD=_tmp6BA.ptr_atts;_tmp6BE=_tmp6BD.rgn;_tmp6BF=
_tmp6BD.nullable;_tmp6C0=_tmp6BD.bounds;_tmp6C1=_tmp6BD.zero_term;}}_LL31C: {
struct Cyc_List_List*_tmp6C7=Cyc_Toc_get_relns(_tmp4F3);int in_bnds=0;{void*
_tmp6C8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6C0);_LL320:;_LL321: in_bnds=Cyc_Toc_check_bounds(_tmp6B5,_tmp6C7,_tmp4F3,
_tmp4F4);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCB9;void*
_tmpCB8[1];struct Cyc_String_pa_struct _tmpCB7;(_tmpCB7.tag=0,((_tmpCB7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB8[0]=&
_tmpCB7,Cyc_Tcutil_warn(e->loc,((_tmpCB9="bounds check necessary for %s",
_tag_dyneither(_tmpCB9,sizeof(char),30))),_tag_dyneither(_tmpCB8,sizeof(void*),1)))))));}
_LL31F:;}Cyc_Toc_exp_to_c(nv,_tmp4F3);Cyc_Toc_exp_to_c(nv,_tmp4F4);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6CC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6C0);struct Cyc_Absyn_Exp*_tmp6CE;_LL323: {struct Cyc_Absyn_Upper_b_struct*
_tmp6CD=(struct Cyc_Absyn_Upper_b_struct*)_tmp6CC;if(_tmp6CD->tag != 1)goto _LL325;
else{_tmp6CE=_tmp6CD->f1;}}_LL324: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6BF);void*ta1=Cyc_Toc_typ_to_c(_tmp6BB);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6BC);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6C1)){if(!Cyc_Evexp_c_can_eval(_tmp6CE)){const char*_tmpCBC;void*_tmpCBB;(
_tmpCBB=0,Cyc_Tcutil_terr(e->loc,((_tmpCBC="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCBC,sizeof(char),40))),_tag_dyneither(_tmpCBB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4F3);struct Cyc_Absyn_Exp*_tmpCBD[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCBD[2]=_tmp4F4,((_tmpCBD[1]=_tmp6CE,((
_tmpCBD[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCBD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6CE)){const char*_tmpCC0;void*_tmpCBF;(
_tmpCBF=0,Cyc_Tcutil_terr(e->loc,((_tmpCC0="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCC0,sizeof(char),40))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,Cyc_Tcutil_warn(
e->loc,((_tmpCC3="inserted null check due to dereference",_tag_dyneither(_tmpCC3,
sizeof(char),39))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC4[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCC4[3]=_tmp4F4,((_tmpCC4[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCC4[1]=
_tmp6CE,((_tmpCC4[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC4,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6CE)){const char*_tmpCC7;void*_tmpCC6;(_tmpCC6=0,Cyc_Tcutil_terr(
e->loc,((_tmpCC7="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCC7,sizeof(char),40))),_tag_dyneither(_tmpCC6,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC8[2];_tmp4F4->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCC8[1]=Cyc_Absyn_copy_exp(_tmp4F4),((_tmpCC8[0]=_tmp6CE,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC8,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL322;}_LL325: {struct Cyc_Absyn_DynEither_b_struct*_tmp6CF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6CC;if(_tmp6CF->tag != 0)goto _LL322;}
_LL326: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6BB);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BC),
Cyc_Absyn_aggrmember_exp(_tmp4F3,Cyc_Toc_curr_sp,0)),_tmp4F4);}else{struct Cyc_Absyn_Exp*
_tmp6DB=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCC9[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BC),Cyc_Absyn_fncall_exp(
_tmp6DB,((_tmpCC9[2]=_tmp4F4,((_tmpCC9[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCC9[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL322;}
_LL322:;}goto _LL318;}_LL31D:;_LL31E: {const char*_tmpCCC;void*_tmpCCB;(_tmpCCB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCCC="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCCC,sizeof(char),49))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}_LL318:;}
goto _LL212;}_LL24B: {struct Cyc_Absyn_Tuple_e_struct*_tmp4F5=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp496;if(_tmp4F5->tag != 25)goto _LL24D;else{_tmp4F6=_tmp4F5->f1;}}_LL24C: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F6))->r;else{struct
Cyc_List_List*_tmp6DF=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F6);void*_tmp6E0=Cyc_Toc_add_tuple_type(
_tmp6DF);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F6 != 0;(_tmp4F6=_tmp4F6->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F6->hd);{struct _tuple15*
_tmpCCF;struct Cyc_List_List*_tmpCCE;dles=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((
_tmpCCE->hd=((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=0,((_tmpCCF->f2=(
struct Cyc_Absyn_Exp*)_tmp4F6->hd,_tmpCCF)))))),((_tmpCCE->tl=dles,_tmpCCE))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL212;_LL24D: {struct Cyc_Absyn_Array_e_struct*
_tmp4F7=(struct Cyc_Absyn_Array_e_struct*)_tmp496;if(_tmp4F7->tag != 27)goto _LL24F;
else{_tmp4F8=_tmp4F7->f1;}}_LL24E: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F8);{
struct Cyc_List_List*_tmp6E3=_tmp4F8;for(0;_tmp6E3 != 0;_tmp6E3=_tmp6E3->tl){
struct _tuple15 _tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;struct _tuple15*_tmp6E4=(struct
_tuple15*)_tmp6E3->hd;_tmp6E5=*_tmp6E4;_tmp6E6=_tmp6E5.f2;Cyc_Toc_exp_to_c(nv,
_tmp6E6);}}goto _LL212;_LL24F: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4F9=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp496;if(_tmp4F9->tag != 28)goto _LL251;
else{_tmp4FA=_tmp4F9->f1;_tmp4FB=_tmp4F9->f2;_tmp4FC=_tmp4F9->f3;_tmp4FD=_tmp4F9->f4;}}
_LL250: {unsigned int _tmp6E8;int _tmp6E9;struct _tuple10 _tmp6E7=Cyc_Evexp_eval_const_uint_exp(
_tmp4FB);_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;{void*_tmp6EA=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FC->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4FC);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4FC)){if(!_tmp6E9){
const char*_tmpCD2;void*_tmpCD1;(_tmpCD1=0,Cyc_Tcutil_terr(_tmp4FB->loc,((_tmpCD2="cannot determine value of constant",
_tag_dyneither(_tmpCD2,sizeof(char),35))),_tag_dyneither(_tmpCD1,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6E8;++ i){struct _tuple15*_tmpCD5;struct Cyc_List_List*
_tmpCD4;es=((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=((_tmpCD5=
_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=0,((_tmpCD5->f2=_tmp4FC,_tmpCD5)))))),((
_tmpCD4->tl=es,_tmpCD4))))));}}if(_tmp4FD){struct Cyc_Absyn_Exp*_tmp6EF=Cyc_Toc_cast_it(
_tmp6EA,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCD8;struct Cyc_List_List*
_tmpCD7;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=((
_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->f1=0,((_tmpCD8->f2=_tmp6EF,
_tmpCD8)))))),((_tmpCD7->tl=0,_tmpCD7)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL212;};};}_LL251: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4FE=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp496;if(_tmp4FE->tag != 29)goto _LL253;
else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;_tmp501=_tmp4FE->f3;_tmp502=_tmp4FE->f4;}}
_LL252: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp500
!= 0,0,0,_tmp501,_tmp4FF))->r;else{if(_tmp502 == 0){const char*_tmpCDB;void*
_tmpCDA;(_tmpCDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCDB="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCDB,sizeof(
char),38))),_tag_dyneither(_tmpCDA,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp502;struct Cyc_Toc_Env _tmp6F5;struct
_RegionHandle*_tmp6F6;struct Cyc_Toc_Env*_tmp6F4=nv;_tmp6F5=*_tmp6F4;_tmp6F6=
_tmp6F5.rgn;{struct Cyc_List_List*_tmp6F7=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6F6,e->loc,_tmp501,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp502->impl))->tagged){struct
_tuple20 _tmp6F9;struct Cyc_Absyn_Aggrfield*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6FB;
struct _tuple20*_tmp6F8=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6F7))->hd;_tmp6F9=*_tmp6F8;_tmp6FA=_tmp6F9.f1;_tmp6FB=_tmp6F9.f2;{void*
_tmp6FC=_tmp6FA->type;Cyc_Toc_exp_to_c(nv,_tmp6FB);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6FC))_tmp6FB->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6FB->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp502,_tmp6FA->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCE0;struct _tuple15*_tmpCDF;struct _tuple15*_tmpCDE[2];
struct Cyc_List_List*_tmp6FD=(_tmpCDE[1]=((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((
_tmpCDF->f1=0,((_tmpCDF->f2=_tmp6FB,_tmpCDF)))))),((_tmpCDE[0]=((_tmpCE0=
_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->f1=0,((_tmpCE0->f2=field_tag_exp,_tmpCE0)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDE,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6FD,0);struct Cyc_Absyn_FieldName_struct*_tmpCE6;struct Cyc_Absyn_FieldName_struct
_tmpCE5;void*_tmpCE4[1];struct Cyc_List_List*ds=(_tmpCE4[0]=(void*)((_tmpCE6=
_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE5.tag=1,((_tmpCE5.f1=_tmp6FA->name,
_tmpCE5)))),_tmpCE6)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE4,sizeof(void*),1)));struct _tuple15*_tmpCE9;struct _tuple15*
_tmpCE8[1];struct Cyc_List_List*dles=(_tmpCE8[0]=((_tmpCE9=_cycalloc(sizeof(*
_tmpCE9)),((_tmpCE9->f1=ds,((_tmpCE9->f2=umem,_tmpCE9)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE8,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp706=0;struct Cyc_List_List*_tmp707=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp707 != 0;_tmp707=_tmp707->tl){struct Cyc_List_List*
_tmp708=_tmp6F7;for(0;_tmp708 != 0;_tmp708=_tmp708->tl){if((*((struct _tuple20*)
_tmp708->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp707->hd){struct _tuple20
_tmp70A;struct Cyc_Absyn_Aggrfield*_tmp70B;struct Cyc_Absyn_Exp*_tmp70C;struct
_tuple20*_tmp709=(struct _tuple20*)_tmp708->hd;_tmp70A=*_tmp709;_tmp70B=_tmp70A.f1;
_tmp70C=_tmp70A.f2;{void*_tmp70D=_tmp70B->type;Cyc_Toc_exp_to_c(nv,_tmp70C);if(
Cyc_Toc_is_void_star_or_tvar(_tmp70D))_tmp70C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp70C->r,0));{struct _tuple15*_tmpCEC;struct Cyc_List_List*
_tmpCEB;_tmp706=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->hd=((_tmpCEC=
_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->f1=0,((_tmpCEC->f2=_tmp70C,_tmpCEC)))))),((
_tmpCEB->tl=_tmp706,_tmpCEB))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp706));}};};}
goto _LL212;_LL253: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp503=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp496;if(_tmp503->tag != 30)goto _LL255;else{_tmp504=(void*)_tmp503->f1;_tmp505=
_tmp503->f2;}}_LL254: {struct Cyc_List_List*fs;{void*_tmp710=Cyc_Tcutil_compress(
_tmp504);struct Cyc_List_List*_tmp712;_LL328: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp711=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp710;if(_tmp711->tag != 13)goto
_LL32A;else{_tmp712=_tmp711->f2;}}_LL329: fs=_tmp712;goto _LL327;_LL32A:;_LL32B: {
const char*_tmpCF0;void*_tmpCEF[1];struct Cyc_String_pa_struct _tmpCEE;(_tmpCEE.tag=
0,((_tmpCEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp504)),((_tmpCEF[0]=& _tmpCEE,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="anon struct has type %s",
_tag_dyneither(_tmpCF0,sizeof(char),24))),_tag_dyneither(_tmpCEF,sizeof(void*),1)))))));}
_LL327:;}{struct Cyc_Toc_Env _tmp717;struct _RegionHandle*_tmp718;struct Cyc_Toc_Env*
_tmp716=nv;_tmp717=*_tmp716;_tmp718=_tmp717.rgn;{struct Cyc_List_List*_tmp719=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp718,e->loc,_tmp505,Cyc_Absyn_StructA,fs);for(0;_tmp719 != 0;_tmp719=_tmp719->tl){
struct _tuple20 _tmp71B;struct Cyc_Absyn_Aggrfield*_tmp71C;struct Cyc_Absyn_Exp*
_tmp71D;struct _tuple20*_tmp71A=(struct _tuple20*)_tmp719->hd;_tmp71B=*_tmp71A;
_tmp71C=_tmp71B.f1;_tmp71D=_tmp71B.f2;{void*_tmp71E=_tmp71C->type;Cyc_Toc_exp_to_c(
nv,_tmp71D);if(Cyc_Toc_is_void_star_or_tvar(_tmp71E))_tmp71D->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp71D->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp505);}goto _LL212;};}_LL255: {struct Cyc_Absyn_Datatype_e_struct*_tmp506=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp496;if(_tmp506->tag != 31)goto _LL257;else{
_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;_tmp509=_tmp506->f3;}}_LL256: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp71F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp720=Cyc_Absyn_var_exp(_tmp71F,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCF1;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp509->name,((_tmpCF1="_struct",_tag_dyneither(_tmpCF1,sizeof(char),8)))));}
tag_exp=_tmp508->is_extensible?Cyc_Absyn_var_exp(_tmp509->name,0): Cyc_Toc_datatype_tag(
_tmp508,_tmp509->name);member_exp=_tmp720;{struct Cyc_List_List*_tmp722=_tmp509->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp507 != 0;(
_tmp507=_tmp507->tl,_tmp722=_tmp722->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp507->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp722))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCF4;struct
Cyc_List_List*_tmpCF3;dles=((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=((
_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->f1=0,((_tmpCF4->f2=cur_e,_tmpCF4)))))),((
_tmpCF3->tl=dles,_tmpCF3))))));};}{struct _tuple15*_tmpCF7;struct Cyc_List_List*
_tmpCF6;dles=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=((_tmpCF7=
_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->f1=0,((_tmpCF7->f2=tag_exp,_tmpCF7)))))),((
_tmpCF6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCF6))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCF8;struct Cyc_List_List*_tmp727=(_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((
_tmpCF8->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCF8->tl=0,_tmpCF8)))));{int i=1;for(0;_tmp507 != 0;(((_tmp507=
_tmp507->tl,i ++)),_tmp722=_tmp722->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp507->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp722))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp728=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCF9;_tmp727=((_tmpCF9=_cycalloc(sizeof(*
_tmpCF9)),((_tmpCF9->hd=_tmp728,((_tmpCF9->tl=_tmp727,_tmpCF9))))));};}}{struct
Cyc_Absyn_Stmt*_tmp72A=Cyc_Absyn_exp_stmt(_tmp720,0);struct Cyc_List_List*_tmpCFA;
struct Cyc_Absyn_Stmt*_tmp72B=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((
_tmpCFA->hd=_tmp72A,((_tmpCFA->tl=_tmp727,_tmpCFA))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp71F,datatype_ctype,0,_tmp72B,0));};}goto _LL212;};}
_LL257: {struct Cyc_Absyn_Enum_e_struct*_tmp50A=(struct Cyc_Absyn_Enum_e_struct*)
_tmp496;if(_tmp50A->tag != 32)goto _LL259;}_LL258: goto _LL25A;_LL259: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp50B=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp496;if(_tmp50B->tag != 33)goto
_LL25B;}_LL25A: goto _LL212;_LL25B: {struct Cyc_Absyn_Malloc_e_struct*_tmp50C=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp496;if(_tmp50C->tag != 34)goto _LL25D;else{
_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.is_calloc;_tmp50F=_tmp50D.rgn;_tmp510=
_tmp50D.elt_type;_tmp511=_tmp50D.num_elts;_tmp512=_tmp50D.fat_result;}}_LL25C: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp510)));Cyc_Toc_exp_to_c(nv,
_tmp511);if(_tmp512){struct _tuple0*_tmp72E=Cyc_Toc_temp_var();struct _tuple0*
_tmp72F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp50E){xexp=_tmp511;if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp72E,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp510,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp72E,0));}{struct Cyc_Absyn_Exp*_tmpCFB[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCFB[2]=Cyc_Absyn_var_exp(_tmp72E,0),((_tmpCFB[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCFB[0]=Cyc_Absyn_var_exp(_tmp72F,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCFB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp511,0);if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp72E,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp510,Cyc_Absyn_var_exp(_tmp72E,0));}{struct Cyc_Absyn_Exp*_tmpCFC[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCFC[2]=Cyc_Absyn_var_exp(_tmp72E,0),((_tmpCFC[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCFC[0]=Cyc_Absyn_var_exp(_tmp72F,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFC,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp732=Cyc_Absyn_declare_stmt(_tmp72E,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp72F,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp732);};}else{struct Cyc_Absyn_Exp*_tmp733=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp734=_tmp511->r;union Cyc_Absyn_Cnst _tmp736;struct _tuple5 _tmp737;
int _tmp738;_LL32D: {struct Cyc_Absyn_Const_e_struct*_tmp735=(struct Cyc_Absyn_Const_e_struct*)
_tmp734;if(_tmp735->tag != 0)goto _LL32F;else{_tmp736=_tmp735->f1;if((_tmp736.Int_c).tag
!= 4)goto _LL32F;_tmp737=(struct _tuple5)(_tmp736.Int_c).val;_tmp738=_tmp737.f2;
if(_tmp738 != 1)goto _LL32F;}}_LL32E: goto _LL32C;_LL32F:;_LL330: _tmp733=Cyc_Absyn_times_exp(
_tmp733,_tmp511,0);goto _LL32C;_LL32C:;}if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp733))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp510,
_tmp733))->r;}}goto _LL212;}_LL25D: {struct Cyc_Absyn_Swap_e_struct*_tmp513=(
struct Cyc_Absyn_Swap_e_struct*)_tmp496;if(_tmp513->tag != 35)goto _LL25F;else{
_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;}}_LL25E: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp514->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp515->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCFF;void*_tmpCFE;(_tmpCFE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFF="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCFF,sizeof(char),57))),_tag_dyneither(_tmpCFE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp514);Cyc_Toc_exp_to_c(
nv,_tmp515);{struct Cyc_Absyn_Exp*_tmpD00[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpD00[1]=Cyc_Toc_push_address_exp(_tmp515),((_tmpD00[0]=Cyc_Toc_push_address_exp(
_tmp514),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD00,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL212;};}
_LL25F: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp516=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp496;if(_tmp516->tag != 38)goto _LL261;else{_tmp517=_tmp516->f1;_tmp518=_tmp516->f2;}}
_LL260: {void*_tmp73C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp517->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp517);{void*_tmp73D=
_tmp73C;struct Cyc_Absyn_AggrInfo _tmp73F;union Cyc_Absyn_AggrInfoU _tmp740;struct
Cyc_Absyn_Aggrdecl**_tmp741;struct Cyc_Absyn_Aggrdecl*_tmp742;_LL332: {struct Cyc_Absyn_AggrType_struct*
_tmp73E=(struct Cyc_Absyn_AggrType_struct*)_tmp73D;if(_tmp73E->tag != 12)goto
_LL334;else{_tmp73F=_tmp73E->f1;_tmp740=_tmp73F.aggr_info;if((_tmp740.KnownAggr).tag
!= 2)goto _LL334;_tmp741=(struct Cyc_Absyn_Aggrdecl**)(_tmp740.KnownAggr).val;
_tmp742=*_tmp741;}}_LL333: {struct Cyc_Absyn_Exp*_tmp743=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp742,_tmp518),0);struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_aggrmember_exp(
_tmp517,_tmp518,0);struct Cyc_Absyn_Exp*_tmp745=Cyc_Absyn_aggrmember_exp(_tmp744,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp745,_tmp743,0))->r;goto _LL331;}
_LL334:;_LL335: {const char*_tmpD03;void*_tmpD02;(_tmpD02=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD03="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD03,sizeof(char),31))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}
_LL331:;}goto _LL212;}_LL261: {struct Cyc_Absyn_StmtExp_e_struct*_tmp519=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp496;if(_tmp519->tag != 37)goto _LL263;else{_tmp51A=
_tmp519->f1;}}_LL262: Cyc_Toc_stmt_to_c(nv,_tmp51A);goto _LL212;_LL263: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp51B=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp496;if(_tmp51B->tag != 36)
goto _LL265;}_LL264: {const char*_tmpD06;void*_tmpD05;(_tmpD05=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD06="UnresolvedMem",
_tag_dyneither(_tmpD06,sizeof(char),14))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
_LL265: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp51C=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp496;if(_tmp51C->tag != 26)goto _LL267;}_LL266: {const char*_tmpD09;void*_tmpD08;(
_tmpD08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD09="compoundlit",_tag_dyneither(_tmpD09,sizeof(char),12))),_tag_dyneither(
_tmpD08,sizeof(void*),0)));}_LL267: {struct Cyc_Absyn_Valueof_e_struct*_tmp51D=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp496;if(_tmp51D->tag != 39)goto _LL212;}
_LL268: {const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0C="valueof(-)",
_tag_dyneither(_tmpD0C,sizeof(char),11))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}
_LL212:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct
_tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls);static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp74E=p->r;struct Cyc_Absyn_Vardecl*_tmp750;struct
Cyc_Absyn_Vardecl _tmp751;struct _tuple0*_tmp752;struct Cyc_Absyn_Pat*_tmp753;
struct Cyc_Absyn_Vardecl*_tmp755;struct Cyc_Absyn_Vardecl _tmp756;struct _tuple0*
_tmp757;struct Cyc_Absyn_Vardecl*_tmp75A;struct Cyc_Absyn_Pat*_tmp75B;enum Cyc_Absyn_Sign
_tmp75E;int _tmp75F;char _tmp761;struct _dyneither_ptr _tmp763;struct Cyc_Absyn_Enumdecl*
_tmp765;struct Cyc_Absyn_Enumfield*_tmp766;void*_tmp768;struct Cyc_Absyn_Enumfield*
_tmp769;struct Cyc_Absyn_Pat*_tmp76B;struct Cyc_Absyn_Pat _tmp76C;void*_tmp76D;
struct Cyc_Absyn_Datatypedecl*_tmp76F;struct Cyc_Absyn_Datatypefield*_tmp770;
struct Cyc_List_List*_tmp771;struct Cyc_List_List*_tmp773;struct Cyc_List_List*
_tmp775;struct Cyc_Absyn_AggrInfo*_tmp777;struct Cyc_Absyn_AggrInfo*_tmp779;struct
Cyc_Absyn_AggrInfo _tmp77A;union Cyc_Absyn_AggrInfoU _tmp77B;struct Cyc_List_List*
_tmp77C;struct Cyc_Absyn_Pat*_tmp77E;_LL337: {struct Cyc_Absyn_Var_p_struct*
_tmp74F=(struct Cyc_Absyn_Var_p_struct*)_tmp74E;if(_tmp74F->tag != 1)goto _LL339;
else{_tmp750=_tmp74F->f1;_tmp751=*_tmp750;_tmp752=_tmp751.name;_tmp753=_tmp74F->f2;}}
_LL338: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp752,r),rgn,t,r,path,
_tmp753,fail_stmt,decls);_LL339: {struct Cyc_Absyn_TagInt_p_struct*_tmp754=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp74E;if(_tmp754->tag != 3)goto _LL33B;else{
_tmp755=_tmp754->f2;_tmp756=*_tmp755;_tmp757=_tmp756.name;}}_LL33A: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp757,r);goto _LL33C;_LL33B: {struct Cyc_Absyn_Wild_p_struct*_tmp758=(
struct Cyc_Absyn_Wild_p_struct*)_tmp74E;if(_tmp758->tag != 0)goto _LL33D;}_LL33C: s=
Cyc_Toc_skip_stmt_dl();goto _LL336;_LL33D: {struct Cyc_Absyn_Reference_p_struct*
_tmp759=(struct Cyc_Absyn_Reference_p_struct*)_tmp74E;if(_tmp759->tag != 2)goto
_LL33F;else{_tmp75A=_tmp759->f1;_tmp75B=_tmp759->f2;}}_LL33E: {struct _tuple0*
_tmp782=Cyc_Toc_temp_var();{struct _tuple22*_tmpD0F;struct Cyc_List_List*_tmpD0E;
decls=((_tmpD0E=_region_malloc(rgn,sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=
_region_malloc(rgn,sizeof(*_tmpD0F)),((_tmpD0F->f1=_tmp782,((_tmpD0F->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD0F)))))),((_tmpD0E->tl=decls,_tmpD0E))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp75A->name,Cyc_Absyn_var_exp(_tmp782,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp782,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp785=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp75B,fail_stmt,decls);_tmp785.f3=Cyc_Absyn_seq_stmt(s,_tmp785.f3,
0);return _tmp785;};}_LL33F: {struct Cyc_Absyn_Null_p_struct*_tmp75C=(struct Cyc_Absyn_Null_p_struct*)
_tmp74E;if(_tmp75C->tag != 8)goto _LL341;}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL336;_LL341: {struct Cyc_Absyn_Int_p_struct*_tmp75D=(struct
Cyc_Absyn_Int_p_struct*)_tmp74E;if(_tmp75D->tag != 9)goto _LL343;else{_tmp75E=
_tmp75D->f1;_tmp75F=_tmp75D->f2;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp75E,_tmp75F,0),fail_stmt);goto _LL336;_LL343: {struct Cyc_Absyn_Char_p_struct*
_tmp760=(struct Cyc_Absyn_Char_p_struct*)_tmp74E;if(_tmp760->tag != 10)goto _LL345;
else{_tmp761=_tmp760->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp761,0),fail_stmt);goto _LL336;_LL345: {struct Cyc_Absyn_Float_p_struct*_tmp762=(
struct Cyc_Absyn_Float_p_struct*)_tmp74E;if(_tmp762->tag != 11)goto _LL347;else{
_tmp763=_tmp762->f1;}}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp763,
0),fail_stmt);goto _LL336;_LL347: {struct Cyc_Absyn_Enum_p_struct*_tmp764=(struct
Cyc_Absyn_Enum_p_struct*)_tmp74E;if(_tmp764->tag != 12)goto _LL349;else{_tmp765=
_tmp764->f1;_tmp766=_tmp764->f2;}}_LL348:{struct Cyc_Absyn_Enum_e_struct _tmpD12;
struct Cyc_Absyn_Enum_e_struct*_tmpD11;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD12.tag=32,((
_tmpD12.f1=_tmp766->name,((_tmpD12.f2=(struct Cyc_Absyn_Enumdecl*)_tmp765,((
_tmpD12.f3=(struct Cyc_Absyn_Enumfield*)_tmp766,_tmpD12)))))))),_tmpD11)))),0),
fail_stmt);}goto _LL336;_LL349: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp767=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp74E;if(_tmp767->tag != 13)goto _LL34B;else{
_tmp768=(void*)_tmp767->f1;_tmp769=_tmp767->f2;}}_LL34A:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD15;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD14;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14[0]=((_tmpD15.tag=33,((
_tmpD15.f1=_tmp769->name,((_tmpD15.f2=(void*)_tmp768,((_tmpD15.f3=(struct Cyc_Absyn_Enumfield*)
_tmp769,_tmpD15)))))))),_tmpD14)))),0),fail_stmt);}goto _LL336;_LL34B: {struct Cyc_Absyn_Pointer_p_struct*
_tmp76A=(struct Cyc_Absyn_Pointer_p_struct*)_tmp74E;if(_tmp76A->tag != 5)goto
_LL34D;else{_tmp76B=_tmp76A->f1;_tmp76C=*_tmp76B;_tmp76D=_tmp76C.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp76E=(struct Cyc_Absyn_Datatype_p_struct*)_tmp76D;if(_tmp76E->tag != 7)goto
_LL34D;else{_tmp76F=_tmp76E->f1;_tmp770=_tmp76E->f2;_tmp771=_tmp76E->f3;}};}}
_LL34C: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp78A=Cyc_Toc_temp_var();const
char*_tmpD16;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp770->name,((
_tmpD16="_struct",_tag_dyneither(_tmpD16,sizeof(char),8))));void*_tmp78B=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp78B,r);struct Cyc_List_List*_tmp78C=_tmp770->typs;for(0;_tmp771 != 0;(((
_tmp771=_tmp771->tl,_tmp78C=((struct Cyc_List_List*)_check_null(_tmp78C))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp78D=(struct Cyc_Absyn_Pat*)_tmp771->hd;if(_tmp78D->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp78E=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp78C))->hd)).f2;struct _tuple0*_tmp78F=Cyc_Toc_temp_var();
void*_tmp790=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp78D->topt))->v;void*
_tmp791=Cyc_Toc_typ_to_c(_tmp790);struct Cyc_Absyn_Exp*_tmp792=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp78A,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp78E)))_tmp792=Cyc_Toc_cast_it(_tmp791,_tmp792);{struct
_tuple22*_tmpD19;struct Cyc_List_List*_tmpD18;decls=((_tmpD18=_region_malloc(rgn,
sizeof(*_tmpD18)),((_tmpD18->hd=((_tmpD19=_region_malloc(rgn,sizeof(*_tmpD19)),((
_tmpD19->f1=_tmp78F,((_tmpD19->f2=_tmp791,_tmpD19)))))),((_tmpD18->tl=decls,
_tmpD18))))));}{struct _tuple21 _tmp795=Cyc_Toc_xlate_pat(nv,rgn,_tmp790,Cyc_Absyn_var_exp(
_tmp78F,0),_tmp792,_tmp78D,fail_stmt,decls);nv=_tmp795.f1;decls=_tmp795.f2;{
struct Cyc_Absyn_Stmt*_tmp796=_tmp795.f3;struct Cyc_Absyn_Stmt*_tmp797=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp78F,0),_tmp792,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp797,_tmp796,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp78A,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp76F->is_extensible?Cyc_Absyn_var_exp(_tmp770->name,0): Cyc_Toc_datatype_tag(
_tmp76F,_tmp770->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp78A,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp78A,_tmp78B,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL336;};};_LL34D: {struct Cyc_Absyn_Datatype_p_struct*_tmp772=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp74E;if(_tmp772->tag != 7)goto _LL34F;else{
_tmp773=_tmp772->f3;}}_LL34E: _tmp775=_tmp773;goto _LL350;_LL34F: {struct Cyc_Absyn_Tuple_p_struct*
_tmp774=(struct Cyc_Absyn_Tuple_p_struct*)_tmp74E;if(_tmp774->tag != 4)goto _LL351;
else{_tmp775=_tmp774->f1;}}_LL350: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp775 != 0;(_tmp775=_tmp775->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp799=(struct Cyc_Absyn_Pat*)
_tmp775->hd;if(_tmp799->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp79A=Cyc_Toc_temp_var();void*_tmp79B=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp799->topt))->v;{struct _tuple22*_tmpD1C;struct Cyc_List_List*_tmpD1B;decls=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->hd=((_tmpD1C=
_region_malloc(rgn,sizeof(*_tmpD1C)),((_tmpD1C->f1=_tmp79A,((_tmpD1C->f2=Cyc_Toc_typ_to_c(
_tmp79B),_tmpD1C)))))),((_tmpD1B->tl=decls,_tmpD1B))))));}{struct _tuple21 _tmp79E=
Cyc_Toc_xlate_pat(nv,rgn,_tmp79B,Cyc_Absyn_var_exp(_tmp79A,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp799,fail_stmt,decls);nv=_tmp79E.f1;decls=
_tmp79E.f2;{struct Cyc_Absyn_Stmt*_tmp79F=_tmp79E.f3;struct Cyc_Absyn_Stmt*_tmp7A0=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp79A,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7A0,
_tmp79F,0),0);};};};}goto _LL336;};_LL351: {struct Cyc_Absyn_Aggr_p_struct*_tmp776=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp74E;if(_tmp776->tag != 6)goto _LL353;else{
_tmp777=_tmp776->f1;if(_tmp777 != 0)goto _LL353;}}_LL352: {const char*_tmpD1F;void*
_tmpD1E;(_tmpD1E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD1F="unresolved aggregate pattern!",_tag_dyneither(_tmpD1F,sizeof(char),30))),
_tag_dyneither(_tmpD1E,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Aggr_p_struct*
_tmp778=(struct Cyc_Absyn_Aggr_p_struct*)_tmp74E;if(_tmp778->tag != 6)goto _LL355;
else{_tmp779=_tmp778->f1;if(_tmp779 == 0)goto _LL355;_tmp77A=*_tmp779;_tmp77B=
_tmp77A.aggr_info;_tmp77C=_tmp778->f3;}}_LL354: {struct Cyc_Absyn_Aggrdecl*
_tmp7A3=Cyc_Absyn_get_known_aggrdecl(_tmp77B);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7A3->impl))->tagged){struct _tuple23 _tmp7A5;struct Cyc_List_List*
_tmp7A6;struct Cyc_Absyn_Pat*_tmp7A7;struct _tuple23*_tmp7A4=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp77C))->hd;_tmp7A5=*_tmp7A4;_tmp7A6=_tmp7A5.f1;
_tmp7A7=_tmp7A5.f2;{struct _dyneither_ptr*f;{void*_tmp7A8=(void*)((struct Cyc_List_List*)
_check_null(_tmp7A6))->hd;struct _dyneither_ptr*_tmp7AA;_LL35E: {struct Cyc_Absyn_FieldName_struct*
_tmp7A9=(struct Cyc_Absyn_FieldName_struct*)_tmp7A8;if(_tmp7A9->tag != 1)goto
_LL360;else{_tmp7AA=_tmp7A9->f1;}}_LL35F: f=_tmp7AA;goto _LL35D;_LL360:;_LL361: {
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD22="no field name in tagged union pattern",
_tag_dyneither(_tmpD22,sizeof(char),38))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}
_LL35D:;}{struct _tuple0*_tmp7AD=Cyc_Toc_temp_var();void*_tmp7AE=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7A7->topt))->v;void*_tmp7AF=Cyc_Toc_typ_to_c(
_tmp7AE);{struct _tuple22*_tmpD25;struct Cyc_List_List*_tmpD24;decls=((_tmpD24=
_region_malloc(rgn,sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_region_malloc(rgn,
sizeof(*_tmpD25)),((_tmpD25->f1=_tmp7AD,((_tmpD25->f2=_tmp7AF,_tmpD25)))))),((
_tmpD24->tl=decls,_tmpD24))))));}{struct Cyc_Absyn_Exp*_tmp7B2=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7B3=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7B3=Cyc_Toc_cast_it(_tmp7AF,_tmp7B3);_tmp7B2=Cyc_Toc_cast_it(_tmp7AF,_tmp7B2);{
struct _tuple21 _tmp7B4=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AE,Cyc_Absyn_var_exp(_tmp7AD,
0),_tmp7B2,_tmp7A7,fail_stmt,decls);nv=_tmp7B4.f1;decls=_tmp7B4.f2;{struct Cyc_Absyn_Stmt*
_tmp7B5=_tmp7B4.f3;struct Cyc_Absyn_Stmt*_tmp7B6=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7A3,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7B7=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7AD,0),_tmp7B3,0);s=Cyc_Absyn_seq_stmt(_tmp7B6,Cyc_Absyn_seq_stmt(_tmp7B7,
_tmp7B5,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp77C != 0;_tmp77C=
_tmp77C->tl){struct _tuple23*_tmp7B8=(struct _tuple23*)_tmp77C->hd;struct Cyc_Absyn_Pat*
_tmp7B9=(*_tmp7B8).f2;if(_tmp7B9->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7BA=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7B8).f1))->hd;struct _dyneither_ptr*_tmp7BC;_LL363: {struct Cyc_Absyn_FieldName_struct*
_tmp7BB=(struct Cyc_Absyn_FieldName_struct*)_tmp7BA;if(_tmp7BB->tag != 1)goto
_LL365;else{_tmp7BC=_tmp7BB->f1;}}_LL364: f=_tmp7BC;goto _LL362;_LL365:;_LL366: {
struct Cyc_Toc_Match_error_struct _tmpD28;struct Cyc_Toc_Match_error_struct*_tmpD27;(
int)_throw((void*)((_tmpD27=_cycalloc_atomic(sizeof(*_tmpD27)),((_tmpD27[0]=((
_tmpD28.tag=Cyc_Toc_Match_error,_tmpD28)),_tmpD27)))));}_LL362:;}{struct _tuple0*
_tmp7BF=Cyc_Toc_temp_var();void*_tmp7C0=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7B9->topt))->v;void*_tmp7C1=Cyc_Toc_typ_to_c(_tmp7C0);{struct _tuple22*
_tmpD2B;struct Cyc_List_List*_tmpD2A;decls=((_tmpD2A=_region_malloc(rgn,sizeof(*
_tmpD2A)),((_tmpD2A->hd=((_tmpD2B=_region_malloc(rgn,sizeof(*_tmpD2B)),((_tmpD2B->f1=
_tmp7BF,((_tmpD2B->f2=_tmp7C1,_tmpD2B)))))),((_tmpD2A->tl=decls,_tmpD2A))))));}{
struct _tuple21 _tmp7C4=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C0,Cyc_Absyn_var_exp(_tmp7BF,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7B9,fail_stmt,decls);nv=_tmp7C4.f1;
decls=_tmp7C4.f2;{struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7A3->impl))->fields,f)))->type))
_tmp7C5=Cyc_Toc_cast_it(_tmp7C1,_tmp7C5);{struct Cyc_Absyn_Stmt*_tmp7C6=_tmp7C4.f3;
struct Cyc_Absyn_Stmt*_tmp7C7=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BF,0),
_tmp7C5,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7C7,_tmp7C6,0),0);};};};};};}}
goto _LL336;}_LL355: {struct Cyc_Absyn_Pointer_p_struct*_tmp77D=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp74E;if(_tmp77D->tag != 5)goto _LL357;else{_tmp77E=_tmp77D->f1;}}_LL356: {
struct _tuple0*_tmp7C8=Cyc_Toc_temp_var();void*_tmp7C9=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77E->topt))->v;{struct _tuple22*_tmpD2E;struct Cyc_List_List*
_tmpD2D;decls=((_tmpD2D=_region_malloc(rgn,sizeof(*_tmpD2D)),((_tmpD2D->hd=((
_tmpD2E=_region_malloc(rgn,sizeof(*_tmpD2E)),((_tmpD2E->f1=_tmp7C8,((_tmpD2E->f2=
Cyc_Toc_typ_to_c(_tmp7C9),_tmpD2E)))))),((_tmpD2D->tl=decls,_tmpD2D))))));}{
struct _tuple21 _tmp7CC=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C9,Cyc_Absyn_var_exp(_tmp7C8,
0),Cyc_Absyn_deref_exp(path,0),_tmp77E,fail_stmt,decls);nv=_tmp7CC.f1;decls=
_tmp7CC.f2;{struct Cyc_Absyn_Stmt*_tmp7CD=_tmp7CC.f3;struct Cyc_Absyn_Stmt*_tmp7CE=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C8,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7CD,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7CE,0);else{s=_tmp7CE;}goto _LL336;};};}_LL357: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp77F=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp74E;if(_tmp77F->tag != 14)goto
_LL359;}_LL358: {const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="unknownid pat",
_tag_dyneither(_tmpD31,sizeof(char),14))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
_LL359: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp780=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp74E;if(_tmp780->tag != 15)goto _LL35B;}_LL35A: {const char*_tmpD34;void*_tmpD33;(
_tmpD33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD34="unknowncall pat",_tag_dyneither(_tmpD34,sizeof(char),16))),
_tag_dyneither(_tmpD33,sizeof(void*),0)));}_LL35B: {struct Cyc_Absyn_Exp_p_struct*
_tmp781=(struct Cyc_Absyn_Exp_p_struct*)_tmp74E;if(_tmp781->tag != 16)goto _LL336;}
_LL35C: {const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD37="exp pat",
_tag_dyneither(_tmpD37,sizeof(char),8))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
_LL336:;}{struct _tuple21 _tmpD38;return(_tmpD38.f1=nv,((_tmpD38.f2=decls,((
_tmpD38.f3=s,_tmpD38)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD39;
return(_tmpD39=_region_malloc(r,sizeof(*_tmpD39)),((_tmpD39->f1=Cyc_Toc_fresh_label(),((
_tmpD39->f2=Cyc_Toc_fresh_label(),((_tmpD39->f3=sc,_tmpD39)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7D7=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7D8=Cyc_Tcutil_compress(_tmp7D7);_LL368: {struct Cyc_Absyn_IntType_struct*
_tmp7D9=(struct Cyc_Absyn_IntType_struct*)_tmp7D8;if(_tmp7D9->tag != 6)goto _LL36A;}
_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_EnumType_struct*_tmp7DA=(struct Cyc_Absyn_EnumType_struct*)
_tmp7D8;if(_tmp7DA->tag != 14)goto _LL36C;}_LL36B: leave_as_switch=1;goto _LL367;
_LL36C:;_LL36D: leave_as_switch=0;goto _LL367;_LL367:;}{struct Cyc_List_List*
_tmp7DB=scs;for(0;_tmp7DB != 0;_tmp7DB=_tmp7DB->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7DB->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7DB->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7DC=scs;for(0;_tmp7DC != 0;_tmp7DC=_tmp7DC->tl){struct Cyc_Absyn_Stmt*
_tmp7DD=((struct Cyc_Absyn_Switch_clause*)_tmp7DC->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7DC->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7DD,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7DF;struct _RegionHandle*_tmp7E0;struct Cyc_Toc_Env*_tmp7DE=
nv;_tmp7DF=*_tmp7DE;_tmp7E0=_tmp7DF.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7E0,nv,next_l),_tmp7DD);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7E2;
struct _RegionHandle*_tmp7E3;struct Cyc_Toc_Env*_tmp7E1=nv;_tmp7E2=*_tmp7E1;
_tmp7E3=_tmp7E2.rgn;{struct Cyc_Toc_Env*_tmp7E4=Cyc_Toc_share_env(_tmp7E3,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7E3,Cyc_Toc_gen_label,
_tmp7E3,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7E5=lscs;for(0;_tmp7E5 != 0;
_tmp7E5=_tmp7E5->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7E5->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7E5->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7E5->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7E7;struct Cyc_List_List*_tmp7E8;struct Cyc_Absyn_Stmt*_tmp7E9;struct _tuple21
_tmp7E6=Cyc_Toc_xlate_pat(_tmp7E4,_tmp7E3,_tmp7D7,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7E7=_tmp7E6.f1;_tmp7E8=_tmp7E6.f2;_tmp7E9=_tmp7E6.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7EA=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7E7,_tmp7EA);_tmp7E9=Cyc_Absyn_seq_stmt(
_tmp7E9,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7EA,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7E8;{
struct Cyc_List_List*_tmpD3A;nvs=((_tmpD3A=_region_malloc(_tmp7E3,sizeof(*_tmpD3A)),((
_tmpD3A->hd=_tmp7E7,((_tmpD3A->tl=nvs,_tmpD3A))))));}{struct Cyc_List_List*
_tmpD3B;test_stmts=((_tmpD3B=_region_malloc(_tmp7E3,sizeof(*_tmpD3B)),((_tmpD3B->hd=
_tmp7E9,((_tmpD3B->tl=test_stmts,_tmpD3B))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7EE;struct _dyneither_ptr*_tmp7EF;struct _dyneither_ptr*_tmp7F0;
struct Cyc_Absyn_Switch_clause*_tmp7F1;struct _tuple24*_tmp7ED=(struct _tuple24*)
lscs->hd;_tmp7EE=*_tmp7ED;_tmp7EF=_tmp7EE.f1;_tmp7F0=_tmp7EE.f2;_tmp7F1=_tmp7EE.f3;{
struct Cyc_Toc_Env*_tmp7F2=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7F1->body;struct Cyc_Toc_Env _tmp7F4;struct
_RegionHandle*_tmp7F5;struct Cyc_Toc_Env*_tmp7F3=_tmp7E4;_tmp7F4=*_tmp7F3;_tmp7F5=
_tmp7F4.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7F7;struct _dyneither_ptr*_tmp7F8;
struct Cyc_Absyn_Switch_clause*_tmp7F9;struct _tuple24*_tmp7F6=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7F7=*_tmp7F6;_tmp7F8=_tmp7F7.f2;
_tmp7F9=_tmp7F7.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7F5,
_tmp7F2,end_l,_tmp7F8,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F9->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7F5,
_tmp7F2,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7EF,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7F0,s,0),0);{struct Cyc_List_List*_tmpD3C;stmts=((_tmpD3C=_region_malloc(
_tmp7E3,sizeof(*_tmpD3C)),((_tmpD3C->hd=s,((_tmpD3C->tl=stmts,_tmpD3C))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7FC;struct _tuple0*_tmp7FD;void*_tmp7FE;struct _tuple22*_tmp7FB=(struct
_tuple22*)decls->hd;_tmp7FC=*_tmp7FB;_tmp7FD=_tmp7FC.f1;_tmp7FE=_tmp7FC.f2;res=
Cyc_Absyn_declare_stmt(_tmp7FD,_tmp7FE,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD3D;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((
_tmpD3D->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD3D->tl=0,_tmpD3D)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp800=s->r;struct Cyc_Absyn_Exp*_tmp803;struct
Cyc_Absyn_Stmt*_tmp805;struct Cyc_Absyn_Stmt*_tmp806;struct Cyc_Absyn_Exp*_tmp808;
struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_Absyn_Stmt*_tmp80B;struct Cyc_Absyn_Stmt*
_tmp80C;struct _tuple8 _tmp80E;struct Cyc_Absyn_Exp*_tmp80F;struct Cyc_Absyn_Stmt*
_tmp810;struct Cyc_Absyn_Stmt*_tmp812;struct Cyc_Absyn_Stmt*_tmp814;struct Cyc_Absyn_Stmt*
_tmp816;struct Cyc_Absyn_Exp*_tmp818;struct _tuple8 _tmp819;struct Cyc_Absyn_Exp*
_tmp81A;struct _tuple8 _tmp81B;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_Absyn_Stmt*
_tmp81D;struct Cyc_Absyn_Exp*_tmp81F;struct Cyc_List_List*_tmp820;struct Cyc_List_List*
_tmp822;struct Cyc_Absyn_Switch_clause**_tmp823;struct Cyc_Absyn_Decl*_tmp825;
struct Cyc_Absyn_Stmt*_tmp826;struct _dyneither_ptr*_tmp828;struct Cyc_Absyn_Stmt*
_tmp829;struct Cyc_Absyn_Stmt*_tmp82B;struct _tuple8 _tmp82C;struct Cyc_Absyn_Exp*
_tmp82D;struct Cyc_Absyn_Stmt*_tmp82F;struct Cyc_List_List*_tmp830;struct Cyc_Absyn_Exp*
_tmp832;_LL36F: {struct Cyc_Absyn_Skip_s_struct*_tmp801=(struct Cyc_Absyn_Skip_s_struct*)
_tmp800;if(_tmp801->tag != 0)goto _LL371;}_LL370: return;_LL371: {struct Cyc_Absyn_Exp_s_struct*
_tmp802=(struct Cyc_Absyn_Exp_s_struct*)_tmp800;if(_tmp802->tag != 1)goto _LL373;
else{_tmp803=_tmp802->f1;}}_LL372: Cyc_Toc_exp_to_c(nv,_tmp803);return;_LL373: {
struct Cyc_Absyn_Seq_s_struct*_tmp804=(struct Cyc_Absyn_Seq_s_struct*)_tmp800;if(
_tmp804->tag != 2)goto _LL375;else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;}}
_LL374: Cyc_Toc_stmt_to_c(nv,_tmp805);s=_tmp806;continue;_LL375: {struct Cyc_Absyn_Return_s_struct*
_tmp807=(struct Cyc_Absyn_Return_s_struct*)_tmp800;if(_tmp807->tag != 3)goto _LL377;
else{_tmp808=_tmp807->f1;}}_LL376: {struct Cyc_Core_Opt*topt=0;if(_tmp808 != 0){{
struct Cyc_Core_Opt*_tmpD3E;topt=((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp808->topt))->v),
_tmpD3E))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp808);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp834=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp835=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp834,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp834,(void*)topt->v,_tmp808,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp835,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL377: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp809=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp800;if(_tmp809->tag != 4)goto _LL379;
else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;_tmp80C=_tmp809->f3;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp80A);Cyc_Toc_stmt_to_c(nv,_tmp80B);s=_tmp80C;continue;_LL379: {struct Cyc_Absyn_While_s_struct*
_tmp80D=(struct Cyc_Absyn_While_s_struct*)_tmp800;if(_tmp80D->tag != 5)goto _LL37B;
else{_tmp80E=_tmp80D->f1;_tmp80F=_tmp80E.f1;_tmp810=_tmp80D->f2;}}_LL37A: Cyc_Toc_exp_to_c(
nv,_tmp80F);{struct Cyc_Toc_Env _tmp837;struct _RegionHandle*_tmp838;struct Cyc_Toc_Env*
_tmp836=nv;_tmp837=*_tmp836;_tmp838=_tmp837.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp838,nv),_tmp810);return;};_LL37B: {struct Cyc_Absyn_Break_s_struct*_tmp811=(
struct Cyc_Absyn_Break_s_struct*)_tmp800;if(_tmp811->tag != 6)goto _LL37D;else{
_tmp812=_tmp811->f1;}}_LL37C: {struct Cyc_Toc_Env _tmp83A;struct _dyneither_ptr**
_tmp83B;struct Cyc_Toc_Env*_tmp839=nv;_tmp83A=*_tmp839;_tmp83B=_tmp83A.break_lab;
if(_tmp83B != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp83B,0);{int dest_depth=_tmp812 == 0?0:
_tmp812->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37D: {struct Cyc_Absyn_Continue_s_struct*_tmp813=(struct Cyc_Absyn_Continue_s_struct*)
_tmp800;if(_tmp813->tag != 7)goto _LL37F;else{_tmp814=_tmp813->f1;}}_LL37E: {
struct Cyc_Toc_Env _tmp83D;struct _dyneither_ptr**_tmp83E;struct Cyc_Toc_Env*_tmp83C=
nv;_tmp83D=*_tmp83C;_tmp83E=_tmp83D.continue_lab;if(_tmp83E != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp83E,0);_tmp816=_tmp814;goto _LL380;}_LL37F: {struct Cyc_Absyn_Goto_s_struct*
_tmp815=(struct Cyc_Absyn_Goto_s_struct*)_tmp800;if(_tmp815->tag != 8)goto _LL381;
else{_tmp816=_tmp815->f2;}}_LL380: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp816))->try_depth,s);return;_LL381: {struct Cyc_Absyn_For_s_struct*
_tmp817=(struct Cyc_Absyn_For_s_struct*)_tmp800;if(_tmp817->tag != 9)goto _LL383;
else{_tmp818=_tmp817->f1;_tmp819=_tmp817->f2;_tmp81A=_tmp819.f1;_tmp81B=_tmp817->f3;
_tmp81C=_tmp81B.f1;_tmp81D=_tmp817->f4;}}_LL382: Cyc_Toc_exp_to_c(nv,_tmp818);Cyc_Toc_exp_to_c(
nv,_tmp81A);Cyc_Toc_exp_to_c(nv,_tmp81C);{struct Cyc_Toc_Env _tmp840;struct
_RegionHandle*_tmp841;struct Cyc_Toc_Env*_tmp83F=nv;_tmp840=*_tmp83F;_tmp841=
_tmp840.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp841,nv),_tmp81D);return;};
_LL383: {struct Cyc_Absyn_Switch_s_struct*_tmp81E=(struct Cyc_Absyn_Switch_s_struct*)
_tmp800;if(_tmp81E->tag != 10)goto _LL385;else{_tmp81F=_tmp81E->f1;_tmp820=_tmp81E->f2;}}
_LL384: Cyc_Toc_xlate_switch(nv,s,_tmp81F,_tmp820);return;_LL385: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp821=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp800;if(_tmp821->tag != 11)goto
_LL387;else{_tmp822=_tmp821->f1;_tmp823=_tmp821->f2;}}_LL386: {struct Cyc_Toc_Env
_tmp843;struct Cyc_Toc_FallthruInfo*_tmp844;struct Cyc_Toc_Env*_tmp842=nv;_tmp843=*
_tmp842;_tmp844=_tmp843.fallthru_info;if(_tmp844 == 0){const char*_tmpD41;void*
_tmpD40;(_tmpD40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD41="fallthru in unexpected place",_tag_dyneither(_tmpD41,sizeof(char),29))),
_tag_dyneither(_tmpD40,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp848;struct
Cyc_List_List*_tmp849;struct Cyc_Dict_Dict _tmp84A;struct Cyc_Toc_FallthruInfo
_tmp847=*_tmp844;_tmp848=_tmp847.label;_tmp849=_tmp847.binders;_tmp84A=_tmp847.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp848,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp823)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp84B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp849);struct Cyc_List_List*_tmp84C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp822);for(0;_tmp84B != 0;(_tmp84B=_tmp84B->tl,
_tmp84C=_tmp84C->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp84C))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp84A,(
struct _tuple0*)_tmp84B->hd),(struct Cyc_Absyn_Exp*)_tmp84C->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL387: {struct Cyc_Absyn_Decl_s_struct*_tmp824=(struct Cyc_Absyn_Decl_s_struct*)
_tmp800;if(_tmp824->tag != 12)goto _LL389;else{_tmp825=_tmp824->f1;_tmp826=_tmp824->f2;}}
_LL388:{void*_tmp84D=_tmp825->r;struct Cyc_Absyn_Vardecl*_tmp84F;struct Cyc_Absyn_Pat*
_tmp851;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_List_List*_tmp854;struct Cyc_Absyn_Fndecl*
_tmp856;struct Cyc_Absyn_Tvar*_tmp858;struct Cyc_Absyn_Vardecl*_tmp859;int _tmp85A;
struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Exp*_tmp85D;struct Cyc_Absyn_Tvar*
_tmp85E;struct Cyc_Absyn_Vardecl*_tmp85F;_LL392: {struct Cyc_Absyn_Var_d_struct*
_tmp84E=(struct Cyc_Absyn_Var_d_struct*)_tmp84D;if(_tmp84E->tag != 0)goto _LL394;
else{_tmp84F=_tmp84E->f1;}}_LL393: {struct Cyc_Toc_Env _tmp861;struct _RegionHandle*
_tmp862;struct Cyc_Toc_Env*_tmp860=nv;_tmp861=*_tmp860;_tmp862=_tmp861.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD44;struct Cyc_Absyn_Local_b_struct*_tmpD43;
struct Cyc_Toc_Env*_tmp863=Cyc_Toc_add_varmap(_tmp862,nv,_tmp84F->name,Cyc_Absyn_varb_exp(
_tmp84F->name,(void*)((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=((
_tmpD44.tag=4,((_tmpD44.f1=_tmp84F,_tmpD44)))),_tmpD43)))),0));Cyc_Toc_local_decl_to_c(
_tmp863,_tmp863,_tmp84F,_tmp826);}goto _LL391;}_LL394: {struct Cyc_Absyn_Let_d_struct*
_tmp850=(struct Cyc_Absyn_Let_d_struct*)_tmp84D;if(_tmp850->tag != 2)goto _LL396;
else{_tmp851=_tmp850->f1;_tmp852=_tmp850->f3;}}_LL395:{void*_tmp866=_tmp851->r;
struct Cyc_Absyn_Vardecl*_tmp868;struct Cyc_Absyn_Pat*_tmp869;struct Cyc_Absyn_Pat
_tmp86A;void*_tmp86B;_LL3A1: {struct Cyc_Absyn_Var_p_struct*_tmp867=(struct Cyc_Absyn_Var_p_struct*)
_tmp866;if(_tmp867->tag != 1)goto _LL3A3;else{_tmp868=_tmp867->f1;_tmp869=_tmp867->f2;
_tmp86A=*_tmp869;_tmp86B=_tmp86A.r;{struct Cyc_Absyn_Wild_p_struct*_tmp86C=(
struct Cyc_Absyn_Wild_p_struct*)_tmp86B;if(_tmp86C->tag != 0)goto _LL3A3;};}}_LL3A2: {
struct _tuple0*old_name=_tmp868->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp868->name=new_name;_tmp868->initializer=(struct Cyc_Absyn_Exp*)_tmp852;{
struct Cyc_Absyn_Var_d_struct _tmpD47;struct Cyc_Absyn_Var_d_struct*_tmpD46;_tmp825->r=(
void*)((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD47.tag=0,((
_tmpD47.f1=_tmp868,_tmpD47)))),_tmpD46))));}{struct Cyc_Toc_Env _tmp870;struct
_RegionHandle*_tmp871;struct Cyc_Toc_Env*_tmp86F=nv;_tmp870=*_tmp86F;_tmp871=
_tmp870.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD4A;struct Cyc_Absyn_Local_b_struct*
_tmpD49;struct Cyc_Toc_Env*_tmp872=Cyc_Toc_add_varmap(_tmp871,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=
4,((_tmpD4A.f1=_tmp868,_tmpD4A)))),_tmpD49)))),0));Cyc_Toc_local_decl_to_c(
_tmp872,nv,_tmp868,_tmp826);}goto _LL3A0;};}_LL3A3:;_LL3A4: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp851,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp852->topt))->v,_tmp852,
_tmp826))->r;goto _LL3A0;_LL3A0:;}goto _LL391;_LL396: {struct Cyc_Absyn_Letv_d_struct*
_tmp853=(struct Cyc_Absyn_Letv_d_struct*)_tmp84D;if(_tmp853->tag != 3)goto _LL398;
else{_tmp854=_tmp853->f1;}}_LL397: {struct Cyc_List_List*_tmp875=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp854);if(_tmp875 == 0){const char*_tmpD4D;
void*_tmpD4C;(_tmpD4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4D="empty Letv_d",_tag_dyneither(_tmpD4D,sizeof(char),
13))),_tag_dyneither(_tmpD4C,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD50;struct Cyc_Absyn_Var_d_struct*_tmpD4F;_tmp825->r=(void*)((_tmpD4F=
_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F[0]=((_tmpD50.tag=0,((_tmpD50.f1=(struct Cyc_Absyn_Vardecl*)
_tmp875->hd,_tmpD50)))),_tmpD4F))));}_tmp875=_tmp875->tl;for(0;_tmp875 != 0;
_tmp875=_tmp875->tl){struct Cyc_Absyn_Var_d_struct _tmpD53;struct Cyc_Absyn_Var_d_struct*
_tmpD52;struct Cyc_Absyn_Decl*_tmp87A=Cyc_Absyn_new_decl((void*)((_tmpD52=
_cycalloc(sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD53.tag=0,((_tmpD53.f1=(struct Cyc_Absyn_Vardecl*)
_tmp875->hd,_tmpD53)))),_tmpD52)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp87A,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL391;}_LL398: {struct Cyc_Absyn_Fn_d_struct*
_tmp855=(struct Cyc_Absyn_Fn_d_struct*)_tmp84D;if(_tmp855->tag != 1)goto _LL39A;
else{_tmp856=_tmp855->f1;}}_LL399: {struct _tuple0*_tmp87D=_tmp856->name;struct
Cyc_Toc_Env _tmp87F;struct _RegionHandle*_tmp880;struct Cyc_Toc_Env*_tmp87E=nv;
_tmp87F=*_tmp87E;_tmp880=_tmp87F.rgn;{struct Cyc_Toc_Env*_tmp881=Cyc_Toc_add_varmap(
_tmp880,nv,_tmp856->name,Cyc_Absyn_var_exp(_tmp87D,0));Cyc_Toc_fndecl_to_c(
_tmp881,_tmp856,0);Cyc_Toc_stmt_to_c(_tmp881,_tmp826);}goto _LL391;}_LL39A: {
struct Cyc_Absyn_Region_d_struct*_tmp857=(struct Cyc_Absyn_Region_d_struct*)
_tmp84D;if(_tmp857->tag != 4)goto _LL39C;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;
_tmp85A=_tmp857->f3;_tmp85B=_tmp857->f4;}}_LL39B: {struct Cyc_Absyn_Stmt*_tmp882=
_tmp826;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp859->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp884;struct _RegionHandle*_tmp885;struct Cyc_Toc_Env*
_tmp883=nv;_tmp884=*_tmp883;_tmp885=_tmp884.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp885,nv,x_var,x_exp),_tmp882);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp882,0))->r;
else{if(_tmp85B == 0){struct Cyc_Absyn_Exp*_tmpD56[1];struct Cyc_Absyn_Exp*_tmpD55[
1];struct Cyc_List_List*_tmpD54;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD54=
_cycalloc(sizeof(*_tmpD54)),((_tmpD54->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD54->tl=0,_tmpD54)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD55[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD55,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp882,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD56[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD56,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp85B);{struct Cyc_Absyn_Exp*
_tmpD58[1];struct Cyc_Absyn_Exp*_tmpD57[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD57[1]=(struct Cyc_Absyn_Exp*)_tmp85B,((_tmpD57[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD57,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp882,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD58[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD58,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39C: {struct Cyc_Absyn_Alias_d_struct*_tmp85C=(struct Cyc_Absyn_Alias_d_struct*)
_tmp84D;if(_tmp85C->tag != 5)goto _LL39E;else{_tmp85D=_tmp85C->f1;_tmp85E=_tmp85C->f2;
_tmp85F=_tmp85C->f3;}}_LL39D: {struct _tuple0*old_name=_tmp85F->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp85F->name=new_name;_tmp85F->initializer=(
struct Cyc_Absyn_Exp*)_tmp85D;{struct Cyc_Absyn_Decl_s_struct _tmpD67;struct Cyc_Absyn_Var_d_struct*
_tmpD66;struct Cyc_Absyn_Var_d_struct _tmpD65;struct Cyc_Absyn_Decl*_tmpD64;struct
Cyc_Absyn_Decl_s_struct*_tmpD63;s->r=(void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((
_tmpD63[0]=((_tmpD67.tag=12,((_tmpD67.f1=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((
_tmpD64->r=(void*)((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66[0]=((_tmpD65.tag=
0,((_tmpD65.f1=_tmp85F,_tmpD65)))),_tmpD66)))),((_tmpD64->loc=0,_tmpD64)))))),((
_tmpD67.f2=_tmp826,_tmpD67)))))),_tmpD63))));}{struct Cyc_Toc_Env _tmp891;struct
_RegionHandle*_tmp892;struct Cyc_Toc_Env*_tmp890=nv;_tmp891=*_tmp890;_tmp892=
_tmp891.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD6A;struct Cyc_Absyn_Local_b_struct*
_tmpD69;struct Cyc_Toc_Env*_tmp893=Cyc_Toc_add_varmap(_tmp892,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD6A.tag=
4,((_tmpD6A.f1=_tmp85F,_tmpD6A)))),_tmpD69)))),0));Cyc_Toc_local_decl_to_c(
_tmp893,nv,_tmp85F,_tmp826);}return;};}_LL39E:;_LL39F: {const char*_tmpD6D;void*
_tmpD6C;(_tmpD6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6D="bad nested declaration within function",_tag_dyneither(_tmpD6D,sizeof(
char),39))),_tag_dyneither(_tmpD6C,sizeof(void*),0)));}_LL391:;}return;_LL389: {
struct Cyc_Absyn_Label_s_struct*_tmp827=(struct Cyc_Absyn_Label_s_struct*)_tmp800;
if(_tmp827->tag != 13)goto _LL38B;else{_tmp828=_tmp827->f1;_tmp829=_tmp827->f2;}}
_LL38A: s=_tmp829;continue;_LL38B: {struct Cyc_Absyn_Do_s_struct*_tmp82A=(struct
Cyc_Absyn_Do_s_struct*)_tmp800;if(_tmp82A->tag != 14)goto _LL38D;else{_tmp82B=
_tmp82A->f1;_tmp82C=_tmp82A->f2;_tmp82D=_tmp82C.f1;}}_LL38C: {struct Cyc_Toc_Env
_tmp899;struct _RegionHandle*_tmp89A;struct Cyc_Toc_Env*_tmp898=nv;_tmp899=*
_tmp898;_tmp89A=_tmp899.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp89A,nv),
_tmp82B);Cyc_Toc_exp_to_c(nv,_tmp82D);return;}_LL38D: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp82E=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp800;if(_tmp82E->tag != 15)goto
_LL38F;else{_tmp82F=_tmp82E->f1;_tmp830=_tmp82E->f2;}}_LL38E: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD6E;
e_exp->topt=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->v=e_typ,_tmpD6E))));}{
struct Cyc_Toc_Env _tmp89D;struct _RegionHandle*_tmp89E;struct Cyc_Toc_Env*_tmp89C=
nv;_tmp89D=*_tmp89C;_tmp89E=_tmp89D.rgn;{struct Cyc_Toc_Env*_tmp89F=Cyc_Toc_add_varmap(
_tmp89E,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp89F,_tmp82F);{struct Cyc_Absyn_Stmt*
_tmp8A0=Cyc_Absyn_seq_stmt(_tmp82F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8A1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8A2=
Cyc_Absyn_var_exp(_tmp8A1,0);struct Cyc_Absyn_Vardecl*_tmp8A3=Cyc_Absyn_new_vardecl(
_tmp8A1,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD6F;_tmp8A2->topt=((
_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->v=Cyc_Absyn_exn_typ(),_tmpD6F))));}{
struct Cyc_Core_Opt*_tmpD80;struct Cyc_Absyn_Var_p_struct*_tmpD7F;struct Cyc_Absyn_Pat*
_tmpD7E;struct Cyc_Core_Opt*_tmpD7D;struct Cyc_Absyn_Var_p_struct _tmpD7C;struct Cyc_Absyn_Pat*
_tmpD7B;struct Cyc_Absyn_Pat*_tmp8A5=(_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((
_tmpD7B->r=(void*)((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD7C.tag=
1,((_tmpD7C.f1=_tmp8A3,((_tmpD7C.f2=((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((
_tmpD7E->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD7E->topt=((_tmpD7D=_cycalloc(
sizeof(*_tmpD7D)),((_tmpD7D->v=Cyc_Absyn_exn_typ(),_tmpD7D)))),((_tmpD7E->loc=0,
_tmpD7E)))))))),_tmpD7C)))))),_tmpD7F)))),((_tmpD7B->topt=((_tmpD80=_cycalloc(
sizeof(*_tmpD80)),((_tmpD80->v=Cyc_Absyn_exn_typ(),_tmpD80)))),((_tmpD7B->loc=0,
_tmpD7B)))))));struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Absyn_throw_exp(_tmp8A2,0);{
struct Cyc_Core_Opt*_tmpD81;_tmp8A6->topt=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((
_tmpD81->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD81))));}{struct Cyc_Absyn_Stmt*
_tmp8A8=Cyc_Absyn_exp_stmt(_tmp8A6,0);struct Cyc_Core_Opt*_tmpD87;struct Cyc_List_List*
_tmpD86;struct Cyc_Absyn_Switch_clause*_tmpD85;struct Cyc_Absyn_Switch_clause*
_tmp8A9=(_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->pattern=_tmp8A5,((
_tmpD85->pat_vars=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->v=((_tmpD86=
_cycalloc(sizeof(*_tmpD86)),((_tmpD86->hd=_tmp8A3,((_tmpD86->tl=0,_tmpD86)))))),
_tmpD87)))),((_tmpD85->where_clause=0,((_tmpD85->body=_tmp8A8,((_tmpD85->loc=0,
_tmpD85)))))))))));struct Cyc_List_List*_tmpD88;struct Cyc_Absyn_Stmt*_tmp8AA=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp830,((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->hd=_tmp8A9,((_tmpD88->tl=
0,_tmpD88))))))),0);Cyc_Toc_stmt_to_c(_tmp89F,_tmp8AA);{struct Cyc_List_List*
_tmpD89;struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD89->tl=0,_tmpD89)))))),0);struct Cyc_List_List*
_tmpD8A;struct Cyc_Absyn_Stmt*_tmp8AC=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD8A->tl=0,_tmpD8A)))))),0),0);struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8AC,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8AD,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8AB,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8AE,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8A0,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8AA,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38F: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp831=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp800;if(_tmp831->tag != 16)goto _LL36E;else{_tmp832=_tmp831->f1;}}_LL390: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp832);{struct Cyc_List_List*
_tmpD8B;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=_tmp832,((_tmpD8B->tl=0,
_tmpD8B)))))),0));};}return;_LL36E:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8BC=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8BC;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8BD=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8BE=f->args;for(0;_tmp8BE != 0;_tmp8BE=_tmp8BE->tl){struct
_tuple0*_tmpD8C;struct _tuple0*_tmp8BF=(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((
_tmpD8C->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8C->f2=(*((struct
_tuple25*)_tmp8BE->hd)).f1,_tmpD8C)))));(*((struct _tuple25*)_tmp8BE->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8BE->hd)).f3);_tmp8BD=Cyc_Toc_add_varmap(frgn,_tmp8BD,
_tmp8BF,Cyc_Absyn_var_exp(_tmp8BF,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8C2;struct Cyc_Absyn_Tqual
_tmp8C3;void*_tmp8C4;int _tmp8C5;struct Cyc_Absyn_VarargInfo _tmp8C1=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8C2=_tmp8C1.name;_tmp8C3=_tmp8C1.tq;_tmp8C4=
_tmp8C1.type;_tmp8C5=_tmp8C1.inject;{void*_tmp8C6=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8C4,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C3,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD8D;struct _tuple0*_tmp8C7=(_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((
_tmpD8D->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8D->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8C2))->v,_tmpD8D)))));{
struct _tuple25*_tmpD90;struct Cyc_List_List*_tmpD8F;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD8F=
_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->hd=((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((
_tmpD90->f1=(struct _dyneither_ptr*)_tmp8C2->v,((_tmpD90->f2=_tmp8C3,((_tmpD90->f3=
_tmp8C6,_tmpD90)))))))),((_tmpD8F->tl=0,_tmpD8F)))))));}_tmp8BD=Cyc_Toc_add_varmap(
frgn,_tmp8BD,_tmp8C7,Cyc_Absyn_var_exp(_tmp8C7,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8CB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){((struct Cyc_Absyn_Vardecl*)_tmp8CB->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8CB->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8BD),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A5: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A6: return Cyc_Absyn_Extern;
default: _LL3A7: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8CC=ad->name;struct _DynRegionHandle*_tmp8CE;struct Cyc_Dict_Dict*_tmp8CF;
struct Cyc_Toc_TocState _tmp8CD=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8CE=_tmp8CD.dyn;_tmp8CF=_tmp8CD.aggrs_so_far;{struct _DynRegionFrame _tmp8D0;
struct _RegionHandle*d=_open_dynregion(& _tmp8D0,_tmp8CE);{int seen_defn_before;
struct _tuple14**_tmp8D1=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8CF,_tmp8CC);if(_tmp8D1 == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD91;v=((
_tmpD91=_region_malloc(d,sizeof(*_tmpD91)),((_tmpD91->f1=ad,((_tmpD91->f2=Cyc_Absyn_strctq(
_tmp8CC),_tmpD91))))));}else{struct _tuple14*_tmpD92;v=((_tmpD92=_region_malloc(d,
sizeof(*_tmpD92)),((_tmpD92->f1=ad,((_tmpD92->f2=Cyc_Absyn_unionq_typ(_tmp8CC),
_tmpD92))))));}*_tmp8CF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CF,_tmp8CC,v);};}else{struct
_tuple14 _tmp8D5;struct Cyc_Absyn_Aggrdecl*_tmp8D6;void*_tmp8D7;struct _tuple14*
_tmp8D4=*_tmp8D1;_tmp8D5=*_tmp8D4;_tmp8D6=_tmp8D5.f1;_tmp8D7=_tmp8D5.f2;if(
_tmp8D6->impl == 0){{struct _tuple14*_tmpD93;*_tmp8CF=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CF,
_tmp8CC,((_tmpD93=_region_malloc(d,sizeof(*_tmpD93)),((_tmpD93->f1=ad,((_tmpD93->f2=
_tmp8D7,_tmpD93)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD94;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD94=_cycalloc(sizeof(*_tmpD94)),((
_tmpD94->kind=ad->kind,((_tmpD94->sc=Cyc_Absyn_Public,((_tmpD94->name=ad->name,((
_tmpD94->tvs=0,((_tmpD94->impl=0,((_tmpD94->attributes=ad->attributes,_tmpD94)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD95;
new_ad->impl=((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->exist_vars=0,((
_tmpD95->rgn_po=0,((_tmpD95->fields=0,((_tmpD95->tagged=0,_tmpD95))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8DA=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8DA != 0;_tmp8DA=_tmp8DA->tl){struct Cyc_Absyn_Aggrfield*
_tmp8DB=(struct Cyc_Absyn_Aggrfield*)_tmp8DA->hd;struct Cyc_Absyn_Aggrfield*
_tmpD96;struct Cyc_Absyn_Aggrfield*_tmp8DC=(_tmpD96=_cycalloc(sizeof(*_tmpD96)),((
_tmpD96->name=_tmp8DB->name,((_tmpD96->tq=Cyc_Toc_mt_tq,((_tmpD96->type=Cyc_Toc_typ_to_c_array(
_tmp8DB->type),((_tmpD96->width=_tmp8DB->width,((_tmpD96->attributes=_tmp8DB->attributes,
_tmpD96)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8DD=_tmp8DC->type;struct _dyneither_ptr*_tmp8DE=_tmp8DC->name;const char*
_tmpD9B;void*_tmpD9A[2];struct Cyc_String_pa_struct _tmpD99;struct Cyc_String_pa_struct
_tmpD98;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD98.tag=0,((_tmpD98.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DE),((_tmpD99.tag=0,((_tmpD99.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD9A[0]=&
_tmpD99,((_tmpD9A[1]=& _tmpD98,Cyc_aprintf(((_tmpD9B="_union_%s_%s",
_tag_dyneither(_tmpD9B,sizeof(char),13))),_tag_dyneither(_tmpD9A,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD9C;struct _dyneither_ptr*str=(_tmpD9C=_cycalloc(sizeof(*
_tmpD9C)),((_tmpD9C[0]=s,_tmpD9C)));struct Cyc_Absyn_Aggrfield*_tmpD9D;struct Cyc_Absyn_Aggrfield*
_tmp8DF=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->name=Cyc_Toc_val_sp,((
_tmpD9D->tq=Cyc_Toc_mt_tq,((_tmpD9D->type=_tmp8DD,((_tmpD9D->width=0,((_tmpD9D->attributes=
0,_tmpD9D)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9E;struct Cyc_Absyn_Aggrfield*
_tmp8E0=(_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->name=Cyc_Toc_tag_sp,((
_tmpD9E->tq=Cyc_Toc_mt_tq,((_tmpD9E->type=Cyc_Absyn_sint_typ,((_tmpD9E->width=0,((
_tmpD9E->attributes=0,_tmpD9E)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9F[2];
struct Cyc_List_List*_tmp8E1=(_tmpD9F[1]=_tmp8DF,((_tmpD9F[0]=_tmp8E0,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD9F,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDA4;struct _tuple0*_tmpDA3;struct Cyc_Absyn_Aggrdecl*
_tmpDA2;struct Cyc_Absyn_Aggrdecl*_tmp8E2=(_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((
_tmpDA2->kind=Cyc_Absyn_StructA,((_tmpDA2->sc=Cyc_Absyn_Public,((_tmpDA2->name=((
_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->f1=Cyc_Absyn_Loc_n,((_tmpDA3->f2=
str,_tmpDA3)))))),((_tmpDA2->tvs=0,((_tmpDA2->impl=((_tmpDA4=_cycalloc(sizeof(*
_tmpDA4)),((_tmpDA4->exist_vars=0,((_tmpDA4->rgn_po=0,((_tmpDA4->fields=_tmp8E1,((
_tmpDA4->tagged=0,_tmpDA4)))))))))),((_tmpDA2->attributes=0,_tmpDA2)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDAA;struct Cyc_Absyn_Aggr_d_struct _tmpDA9;
struct Cyc_List_List*_tmpDA8;Cyc_Toc_result_decls=((_tmpDA8=_cycalloc(sizeof(*
_tmpDA8)),((_tmpDA8->hd=Cyc_Absyn_new_decl((void*)((_tmpDAA=_cycalloc(sizeof(*
_tmpDAA)),((_tmpDAA[0]=((_tmpDA9.tag=6,((_tmpDA9.f1=_tmp8E2,_tmpDA9)))),_tmpDAA)))),
0),((_tmpDA8->tl=Cyc_Toc_result_decls,_tmpDA8))))));}_tmp8DC->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDAB;new_fields=((_tmpDAB=_cycalloc(sizeof(*
_tmpDAB)),((_tmpDAB->hd=_tmp8DC,((_tmpDAB->tl=new_fields,_tmpDAB))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDB5;struct Cyc_Absyn_Aggr_d_struct
_tmpDB4;struct Cyc_Absyn_Aggr_d_struct*_tmpDB3;struct Cyc_List_List*_tmpDB2;Cyc_Toc_result_decls=((
_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->hd=((_tmpDB5=_cycalloc(sizeof(*
_tmpDB5)),((_tmpDB5->r=(void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((
_tmpDB4.tag=6,((_tmpDB4.f1=new_ad,_tmpDB4)))),_tmpDB3)))),((_tmpDB5->loc=0,
_tmpDB5)))))),((_tmpDB2->tl=Cyc_Toc_result_decls,_tmpDB2))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8F9;struct Cyc_Set_Set**_tmp8FA;struct Cyc_Toc_TocState
_tmp8F8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F9=
_tmp8F8.dyn;_tmp8FA=_tmp8F8.datatypes_so_far;{struct _DynRegionFrame _tmp8FB;
struct _RegionHandle*d=_open_dynregion(& _tmp8FB,_tmp8F9);{struct _tuple0*_tmp8FC=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8FA,_tmp8FC)){_npop_handler(0);return;}*_tmp8FA=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8FA,_tmp8FC);}{struct Cyc_List_List*_tmp8FD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FD->hd;struct Cyc_List_List*_tmp8FE=0;int i=
1;{struct Cyc_List_List*_tmp8FF=f->typs;for(0;_tmp8FF != 0;(_tmp8FF=_tmp8FF->tl,i
++)){struct _dyneither_ptr*_tmp900=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDB6;struct Cyc_Absyn_Aggrfield*_tmp901=(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((
_tmpDB6->name=_tmp900,((_tmpDB6->tq=(*((struct _tuple9*)_tmp8FF->hd)).f1,((
_tmpDB6->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8FF->hd)).f2),((
_tmpDB6->width=0,((_tmpDB6->attributes=0,_tmpDB6)))))))))));struct Cyc_List_List*
_tmpDB7;_tmp8FE=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=_tmp901,((
_tmpDB7->tl=_tmp8FE,_tmpDB7))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDBA;struct Cyc_List_List*
_tmpDB9;_tmp8FE=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=((_tmpDBA=
_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->name=Cyc_Toc_tag_sp,((_tmpDBA->tq=Cyc_Toc_mt_tq,((
_tmpDBA->type=Cyc_Absyn_sint_typ,((_tmpDBA->width=0,((_tmpDBA->attributes=0,
_tmpDBA)))))))))))),((_tmpDB9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8FE),_tmpDB9))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDBF;const char*_tmpDBE;struct Cyc_Absyn_Aggrdecl*_tmpDBD;struct Cyc_Absyn_Aggrdecl*
_tmp906=(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->kind=Cyc_Absyn_StructA,((
_tmpDBD->sc=Cyc_Absyn_Public,((_tmpDBD->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDBE="_struct",_tag_dyneither(_tmpDBE,sizeof(char),8)))),((_tmpDBD->tvs=0,((
_tmpDBD->impl=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->exist_vars=0,((
_tmpDBF->rgn_po=0,((_tmpDBF->fields=_tmp8FE,((_tmpDBF->tagged=0,_tmpDBF)))))))))),((
_tmpDBD->attributes=0,_tmpDBD)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDC5;
struct Cyc_Absyn_Aggr_d_struct _tmpDC4;struct Cyc_List_List*_tmpDC3;Cyc_Toc_result_decls=((
_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->hd=Cyc_Absyn_new_decl((void*)((
_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC4.tag=6,((_tmpDC4.f1=
_tmp906,_tmpDC4)))),_tmpDC5)))),0),((_tmpDC3->tl=Cyc_Toc_result_decls,_tmpDC3))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp90E;struct Cyc_Dict_Dict*_tmp90F;struct
Cyc_Toc_TocState _tmp90D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp90E=_tmp90D.dyn;_tmp90F=_tmp90D.xdatatypes_so_far;{struct _DynRegionFrame
_tmp910;struct _RegionHandle*d=_open_dynregion(& _tmp910,_tmp90E);{struct _tuple0*
_tmp911=xd->name;struct Cyc_List_List*_tmp912=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp912 != 0;_tmp912=_tmp912->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp912->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp913=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp914=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp913,Cyc_Absyn_false_conref,0);int*_tmp915=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp90F,f->name);int
_tmp916;_LL3AA: if(_tmp915 != 0)goto _LL3AC;_LL3AB: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp917=Cyc_Absyn_new_vardecl(f->name,_tmp914,
initopt);_tmp917->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDCB;struct Cyc_Absyn_Var_d_struct
_tmpDCA;struct Cyc_List_List*_tmpDC9;Cyc_Toc_result_decls=((_tmpDC9=_cycalloc(
sizeof(*_tmpDC9)),((_tmpDC9->hd=Cyc_Absyn_new_decl((void*)((_tmpDCB=_cycalloc(
sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCA.tag=0,((_tmpDCA.f1=_tmp917,_tmpDCA)))),
_tmpDCB)))),0),((_tmpDC9->tl=Cyc_Toc_result_decls,_tmpDC9))))));}*_tmp90F=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90F,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp91B=f->typs;for(0;_tmp91B != 0;(_tmp91B=_tmp91B->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDD3;void*_tmpDD2[1];const char*_tmpDD1;struct
_dyneither_ptr*_tmpDD0;struct _dyneither_ptr*_tmp91C=(_tmpDD0=_cycalloc(sizeof(*
_tmpDD0)),((_tmpDD0[0]=(struct _dyneither_ptr)((_tmpDD3.tag=1,((_tmpDD3.f1=(
unsigned long)i,((_tmpDD2[0]=& _tmpDD3,Cyc_aprintf(((_tmpDD1="f%d",_tag_dyneither(
_tmpDD1,sizeof(char),4))),_tag_dyneither(_tmpDD2,sizeof(void*),1)))))))),_tmpDD0)));
struct Cyc_Absyn_Aggrfield*_tmpDD4;struct Cyc_Absyn_Aggrfield*_tmp91D=(_tmpDD4=
_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->name=_tmp91C,((_tmpDD4->tq=(*((struct
_tuple9*)_tmp91B->hd)).f1,((_tmpDD4->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp91B->hd)).f2),((_tmpDD4->width=0,((_tmpDD4->attributes=0,_tmpDD4)))))))))));
struct Cyc_List_List*_tmpDD5;fields=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((
_tmpDD5->hd=_tmp91D,((_tmpDD5->tl=fields,_tmpDD5))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDD8;struct Cyc_List_List*_tmpDD7;fields=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((
_tmpDD7->hd=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->name=Cyc_Toc_tag_sp,((
_tmpDD8->tq=Cyc_Toc_mt_tq,((_tmpDD8->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDD8->width=0,((_tmpDD8->attributes=0,_tmpDD8)))))))))))),((
_tmpDD7->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDD7))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDDD;const char*_tmpDDC;
struct Cyc_Absyn_Aggrdecl*_tmpDDB;struct Cyc_Absyn_Aggrdecl*_tmp926=(_tmpDDB=
_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->kind=Cyc_Absyn_StructA,((_tmpDDB->sc=Cyc_Absyn_Public,((
_tmpDDB->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDDC="_struct",
_tag_dyneither(_tmpDDC,sizeof(char),8)))),((_tmpDDB->tvs=0,((_tmpDDB->impl=((
_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->exist_vars=0,((_tmpDDD->rgn_po=0,((
_tmpDDD->fields=fields,((_tmpDDD->tagged=0,_tmpDDD)))))))))),((_tmpDDB->attributes=
0,_tmpDDB)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDE3;struct Cyc_Absyn_Aggr_d_struct
_tmpDE2;struct Cyc_List_List*_tmpDE1;Cyc_Toc_result_decls=((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1->hd=Cyc_Absyn_new_decl((void*)((_tmpDE3=_cycalloc(
sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE2.tag=6,((_tmpDE2.f1=_tmp926,_tmpDE2)))),
_tmpDE3)))),0),((_tmpDE1->tl=Cyc_Toc_result_decls,_tmpDE1))))));}goto _LL3A9;};};};}
_LL3AC: if(_tmp915 == 0)goto _LL3AE;_tmp916=*_tmp915;if(_tmp916 != 0)goto _LL3AE;
_LL3AD: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp92D=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp92E=Cyc_Absyn_new_vardecl(f->name,_tmp914,(
struct Cyc_Absyn_Exp*)_tmp92D);_tmp92E->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDE9;struct Cyc_Absyn_Var_d_struct _tmpDE8;struct Cyc_List_List*_tmpDE7;Cyc_Toc_result_decls=((
_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=Cyc_Absyn_new_decl((void*)((
_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDE8.tag=0,((_tmpDE8.f1=
_tmp92E,_tmpDE8)))),_tmpDE9)))),0),((_tmpDE7->tl=Cyc_Toc_result_decls,_tmpDE7))))));}*
_tmp90F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90F,f->name,1);}goto _LL3A9;_LL3AE:;_LL3AF: goto _LL3A9;_LL3A9:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp932=init->r;struct Cyc_Absyn_Vardecl*_tmp934;struct Cyc_Absyn_Exp*_tmp935;
struct Cyc_Absyn_Exp*_tmp936;int _tmp937;_LL3B1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp933=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp932;if(_tmp933->tag != 28)
goto _LL3B3;else{_tmp934=_tmp933->f1;_tmp935=_tmp933->f2;_tmp936=_tmp933->f3;
_tmp937=_tmp933->f4;}}_LL3B2: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp934,_tmp935,_tmp936,_tmp937,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B0;_LL3B3:;_LL3B4: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp939;struct _RegionHandle*_tmp93A;struct Cyc_Toc_Env*_tmp938=init_nv;_tmp939=*
_tmp938;_tmp93A=_tmp939.rgn;{struct Cyc_Toc_Env*_tmp93B=Cyc_Toc_set_toplevel(
_tmp93A,init_nv);Cyc_Toc_exp_to_c(_tmp93B,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B0;_LL3B0:;}else{void*_tmp93C=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp93E;void*_tmp93F;struct Cyc_Absyn_Exp*_tmp940;union Cyc_Absyn_Constraint*
_tmp941;_LL3B6:{struct Cyc_Absyn_ArrayType_struct*_tmp93D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp93C;if(_tmp93D->tag != 9)goto _LL3B8;else{_tmp93E=_tmp93D->f1;_tmp93F=_tmp93E.elt_type;
_tmp940=_tmp93E.num_elts;_tmp941=_tmp93E.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp941))goto _LL3B8;_LL3B7: if(_tmp940 == 0){const char*
_tmpDEC;void*_tmpDEB;(_tmpDEB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDEC="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDEC,sizeof(char),55))),_tag_dyneither(_tmpDEB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp940;struct Cyc_Absyn_Exp*
_tmp944=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp945=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp944,
_tmp945,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B5;};_LL3B8:;_LL3B9: goto _LL3B5;
_LL3B5:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp947;struct _RegionHandle*
_tmp948;struct Cyc_Toc_Env*_tmp946=nv;_tmp947=*_tmp946;_tmp948=_tmp947.rgn;{
struct Cyc_Absyn_Stmt*_tmp949=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp94A=Cyc_Toc_share_env(_tmp948,nv);struct Cyc_Toc_Env*_tmp94C;struct Cyc_List_List*
_tmp94D;struct Cyc_Absyn_Stmt*_tmp94E;struct _tuple21 _tmp94B=Cyc_Toc_xlate_pat(
_tmp94A,_tmp948,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp94C=_tmp94B.f1;_tmp94D=_tmp94B.f2;_tmp94E=_tmp94B.f3;Cyc_Toc_stmt_to_c(
_tmp94C,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp94E,s,0),0);for(0;_tmp94D != 0;_tmp94D=_tmp94D->tl){
struct _tuple22 _tmp950;struct _tuple0*_tmp951;void*_tmp952;struct _tuple22*_tmp94F=(
struct _tuple22*)_tmp94D->hd;_tmp950=*_tmp94F;_tmp951=_tmp950.f1;_tmp952=_tmp950.f2;
s=Cyc_Absyn_declare_stmt(_tmp951,_tmp952,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp953=e->r;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp957;struct
Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95D;
struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*
_tmp963;struct Cyc_List_List*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*
_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*
_tmp96D;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*
_tmp971;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*
_tmp976;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*
_tmp97A;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_List_List*
_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_List_List*_tmp981;void*_tmp983;
void**_tmp984;struct Cyc_Absyn_Exp*_tmp985;struct _tuple7*_tmp987;struct _tuple7
_tmp988;void*_tmp989;void**_tmp98A;struct Cyc_List_List*_tmp98B;struct Cyc_List_List*
_tmp98D;struct Cyc_List_List*_tmp98F;void*_tmp991;void**_tmp992;void*_tmp994;void**
_tmp995;struct Cyc_Absyn_Stmt*_tmp997;struct Cyc_Absyn_MallocInfo _tmp999;struct Cyc_Absyn_MallocInfo*
_tmp99A;_LL3BB: {struct Cyc_Absyn_Deref_e_struct*_tmp954=(struct Cyc_Absyn_Deref_e_struct*)
_tmp953;if(_tmp954->tag != 21)goto _LL3BD;else{_tmp955=_tmp954->f1;}}_LL3BC:
_tmp957=_tmp955;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp956=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp953;if(_tmp956->tag != 22)goto _LL3BF;
else{_tmp957=_tmp956->f1;}}_LL3BE: _tmp959=_tmp957;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp958=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp953;if(_tmp958->tag != 23)goto
_LL3C1;else{_tmp959=_tmp958->f1;}}_LL3C0: _tmp95B=_tmp959;goto _LL3C2;_LL3C1: {
struct Cyc_Absyn_Address_e_struct*_tmp95A=(struct Cyc_Absyn_Address_e_struct*)
_tmp953;if(_tmp95A->tag != 16)goto _LL3C3;else{_tmp95B=_tmp95A->f1;}}_LL3C2:
_tmp95D=_tmp95B;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_struct*_tmp95C=(
struct Cyc_Absyn_Throw_e_struct*)_tmp953;if(_tmp95C->tag != 12)goto _LL3C5;else{
_tmp95D=_tmp95C->f1;}}_LL3C4: _tmp95F=_tmp95D;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp95E=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp953;if(_tmp95E->tag != 13)
goto _LL3C7;else{_tmp95F=_tmp95E->f1;}}_LL3C6: _tmp961=_tmp95F;goto _LL3C8;_LL3C7: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp960=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp953;if(_tmp960->tag != 19)goto _LL3C9;else{_tmp961=_tmp960->f1;}}_LL3C8:
_tmp963=_tmp961;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_struct*_tmp962=(
struct Cyc_Absyn_Increment_e_struct*)_tmp953;if(_tmp962->tag != 5)goto _LL3CB;else{
_tmp963=_tmp962->f1;}}_LL3CA: Cyc_Toc_exptypes_to_c(_tmp963);goto _LL3BA;_LL3CB: {
struct Cyc_Absyn_Primop_e_struct*_tmp964=(struct Cyc_Absyn_Primop_e_struct*)
_tmp953;if(_tmp964->tag != 3)goto _LL3CD;else{_tmp965=_tmp964->f2;}}_LL3CC:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp965);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_struct*_tmp966=(struct Cyc_Absyn_And_e_struct*)
_tmp953;if(_tmp966->tag != 7)goto _LL3CF;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;}}
_LL3CE: _tmp96A=_tmp967;_tmp96B=_tmp968;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_struct*
_tmp969=(struct Cyc_Absyn_Or_e_struct*)_tmp953;if(_tmp969->tag != 8)goto _LL3D1;
else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}_LL3D0: _tmp96D=_tmp96A;_tmp96E=
_tmp96B;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp96C=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp953;if(_tmp96C->tag != 9)goto _LL3D3;else{_tmp96D=_tmp96C->f1;_tmp96E=_tmp96C->f2;}}
_LL3D2: _tmp970=_tmp96D;_tmp971=_tmp96E;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_struct*
_tmp96F=(struct Cyc_Absyn_Subscript_e_struct*)_tmp953;if(_tmp96F->tag != 24)goto
_LL3D5;else{_tmp970=_tmp96F->f1;_tmp971=_tmp96F->f2;}}_LL3D4: _tmp973=_tmp970;
_tmp974=_tmp971;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_struct*_tmp972=(
struct Cyc_Absyn_Swap_e_struct*)_tmp953;if(_tmp972->tag != 35)goto _LL3D7;else{
_tmp973=_tmp972->f1;_tmp974=_tmp972->f2;}}_LL3D6: _tmp976=_tmp973;_tmp977=_tmp974;
goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_struct*_tmp975=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp953;if(_tmp975->tag != 4)goto _LL3D9;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f3;}}
_LL3D8: Cyc_Toc_exptypes_to_c(_tmp976);Cyc_Toc_exptypes_to_c(_tmp977);goto _LL3BA;
_LL3D9: {struct Cyc_Absyn_Conditional_e_struct*_tmp978=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp953;if(_tmp978->tag != 6)goto _LL3DB;else{_tmp979=_tmp978->f1;_tmp97A=_tmp978->f2;
_tmp97B=_tmp978->f3;}}_LL3DA: Cyc_Toc_exptypes_to_c(_tmp979);Cyc_Toc_exptypes_to_c(
_tmp97A);Cyc_Toc_exptypes_to_c(_tmp97B);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_struct*
_tmp97C=(struct Cyc_Absyn_FnCall_e_struct*)_tmp953;if(_tmp97C->tag != 11)goto
_LL3DD;else{_tmp97D=_tmp97C->f1;_tmp97E=_tmp97C->f2;}}_LL3DC: _tmp980=_tmp97D;
_tmp981=_tmp97E;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp97F=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp953;if(_tmp97F->tag != 10)goto _LL3DF;
else{_tmp980=_tmp97F->f1;_tmp981=_tmp97F->f2;}}_LL3DE: Cyc_Toc_exptypes_to_c(
_tmp980);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp981);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_Cast_e_struct*
_tmp982=(struct Cyc_Absyn_Cast_e_struct*)_tmp953;if(_tmp982->tag != 15)goto _LL3E1;
else{_tmp983=(void**)& _tmp982->f1;_tmp984=(void**)((void**)& _tmp982->f1);_tmp985=
_tmp982->f2;}}_LL3E0:*_tmp984=Cyc_Toc_typ_to_c(*_tmp984);Cyc_Toc_exptypes_to_c(
_tmp985);goto _LL3BA;_LL3E1: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp986=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp953;if(_tmp986->tag != 26)goto _LL3E3;
else{_tmp987=_tmp986->f1;_tmp988=*_tmp987;_tmp989=_tmp988.f3;_tmp98A=(void**)&(*
_tmp986->f1).f3;_tmp98B=_tmp986->f2;}}_LL3E2:*_tmp98A=Cyc_Toc_typ_to_c(*_tmp98A);
_tmp98D=_tmp98B;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp98C=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp953;if(_tmp98C->tag != 36)
goto _LL3E5;else{_tmp98D=_tmp98C->f2;}}_LL3E4: _tmp98F=_tmp98D;goto _LL3E6;_LL3E5: {
struct Cyc_Absyn_Array_e_struct*_tmp98E=(struct Cyc_Absyn_Array_e_struct*)_tmp953;
if(_tmp98E->tag != 27)goto _LL3E7;else{_tmp98F=_tmp98E->f1;}}_LL3E6: for(0;_tmp98F
!= 0;_tmp98F=_tmp98F->tl){struct Cyc_Absyn_Exp*_tmp9AA;struct _tuple15 _tmp9A9=*((
struct _tuple15*)_tmp98F->hd);_tmp9AA=_tmp9A9.f2;Cyc_Toc_exptypes_to_c(_tmp9AA);}
goto _LL3BA;_LL3E7: {struct Cyc_Absyn_Offsetof_e_struct*_tmp990=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp953;if(_tmp990->tag != 20)goto _LL3E9;else{_tmp991=(void**)& _tmp990->f1;
_tmp992=(void**)((void**)& _tmp990->f1);}}_LL3E8: _tmp995=_tmp992;goto _LL3EA;
_LL3E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp993=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp953;if(_tmp993->tag != 18)goto _LL3EB;else{_tmp994=(void**)& _tmp993->f1;
_tmp995=(void**)((void**)& _tmp993->f1);}}_LL3EA:*_tmp995=Cyc_Toc_typ_to_c(*
_tmp995);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_StmtExp_e_struct*_tmp996=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp953;if(_tmp996->tag != 37)goto _LL3ED;else{_tmp997=_tmp996->f1;}}_LL3EC: Cyc_Toc_stmttypes_to_c(
_tmp997);goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Malloc_e_struct*_tmp998=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp953;if(_tmp998->tag != 34)goto _LL3EF;else{_tmp999=_tmp998->f1;_tmp99A=(struct
Cyc_Absyn_MallocInfo*)& _tmp998->f1;}}_LL3EE: if(_tmp99A->elt_type != 0){void**
_tmpDED;_tmp99A->elt_type=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp99A->elt_type))),_tmpDED))));}Cyc_Toc_exptypes_to_c(
_tmp99A->num_elts);goto _LL3BA;_LL3EF: {struct Cyc_Absyn_Const_e_struct*_tmp99B=(
struct Cyc_Absyn_Const_e_struct*)_tmp953;if(_tmp99B->tag != 0)goto _LL3F1;}_LL3F0:
goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Var_e_struct*_tmp99C=(struct Cyc_Absyn_Var_e_struct*)
_tmp953;if(_tmp99C->tag != 1)goto _LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp99D=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp953;if(_tmp99D->tag != 2)goto
_LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Enum_e_struct*_tmp99E=(struct
Cyc_Absyn_Enum_e_struct*)_tmp953;if(_tmp99E->tag != 32)goto _LL3F7;}_LL3F6: goto
_LL3F8;_LL3F7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp99F=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp953;if(_tmp99F->tag != 33)goto _LL3F9;}_LL3F8: goto _LL3BA;_LL3F9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp9A0=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp953;if(_tmp9A0->tag != 30)goto
_LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Datatype_e_struct*_tmp9A1=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp953;if(_tmp9A1->tag != 31)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Aggregate_e_struct*_tmp9A2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp953;if(_tmp9A2->tag != 29)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9A3=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp953;if(_tmp9A3->tag != 28)
goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_struct*_tmp9A4=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp953;if(_tmp9A4->tag != 25)goto _LL403;}_LL402:
goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9A5=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp953;if(_tmp9A5->tag != 14)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_New_e_struct*
_tmp9A6=(struct Cyc_Absyn_New_e_struct*)_tmp953;if(_tmp9A6->tag != 17)goto _LL407;}
_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_struct*_tmp9A7=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp953;if(_tmp9A7->tag != 39)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9A8=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp953;if(_tmp9A8->tag != 38)goto
_LL3BA;}_LL40A:{const char*_tmpDF0;void*_tmpDEF;(_tmpDEF=0,Cyc_Tcutil_terr(e->loc,((
_tmpDF0="Cyclone expression within C code",_tag_dyneither(_tmpDF0,sizeof(char),
33))),_tag_dyneither(_tmpDEF,sizeof(void*),0)));}goto _LL3BA;_LL3BA:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp9AE=d->r;struct Cyc_Absyn_Vardecl*_tmp9B0;struct
Cyc_Absyn_Fndecl*_tmp9B2;struct Cyc_Absyn_Aggrdecl*_tmp9B4;struct Cyc_Absyn_Enumdecl*
_tmp9B6;struct Cyc_Absyn_Typedefdecl*_tmp9B8;_LL40C: {struct Cyc_Absyn_Var_d_struct*
_tmp9AF=(struct Cyc_Absyn_Var_d_struct*)_tmp9AE;if(_tmp9AF->tag != 0)goto _LL40E;
else{_tmp9B0=_tmp9AF->f1;}}_LL40D: _tmp9B0->type=Cyc_Toc_typ_to_c(_tmp9B0->type);
if(_tmp9B0->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9B0->initializer));goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_struct*
_tmp9B1=(struct Cyc_Absyn_Fn_d_struct*)_tmp9AE;if(_tmp9B1->tag != 1)goto _LL410;
else{_tmp9B2=_tmp9B1->f1;}}_LL40F: _tmp9B2->ret_type=Cyc_Toc_typ_to_c(_tmp9B2->ret_type);{
struct Cyc_List_List*_tmp9C4=_tmp9B2->args;for(0;_tmp9C4 != 0;_tmp9C4=_tmp9C4->tl){(*((
struct _tuple25*)_tmp9C4->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9C4->hd)).f3);}}
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_struct*_tmp9B3=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9AE;if(_tmp9B3->tag != 6)goto _LL412;else{_tmp9B4=_tmp9B3->f1;}}_LL411: Cyc_Toc_aggrdecl_to_c(
_tmp9B4);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_struct*_tmp9B5=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9AE;if(_tmp9B5->tag != 8)goto _LL414;else{_tmp9B6=_tmp9B5->f1;}}_LL413: if(
_tmp9B6->fields != 0){struct Cyc_List_List*_tmp9C5=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9B6->fields))->v;for(0;_tmp9C5 != 0;_tmp9C5=_tmp9C5->tl){
struct Cyc_Absyn_Enumfield*_tmp9C6=(struct Cyc_Absyn_Enumfield*)_tmp9C5->hd;if(
_tmp9C6->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C6->tag));}}
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_struct*_tmp9B7=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9AE;if(_tmp9B7->tag != 9)goto _LL416;else{_tmp9B8=_tmp9B7->f1;}}_LL415:{struct
Cyc_Core_Opt*_tmpDF1;_tmp9B8->defn=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((
_tmpDF1->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B8->defn))->v),_tmpDF1))));}goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_struct*
_tmp9B9=(struct Cyc_Absyn_Let_d_struct*)_tmp9AE;if(_tmp9B9->tag != 2)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_struct*_tmp9BA=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9AE;if(_tmp9BA->tag != 3)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9BB=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9AE;if(_tmp9BB->tag != 7)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_struct*_tmp9BC=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9AE;if(_tmp9BC->tag != 10)goto _LL41E;}
_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_struct*_tmp9BD=(struct Cyc_Absyn_Using_d_struct*)
_tmp9AE;if(_tmp9BD->tag != 11)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9BE=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9AE;if(_tmp9BE->tag != 12)goto
_LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9BF=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9AE;if(_tmp9BF->tag != 13)
goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_struct*_tmp9C0=(
struct Cyc_Absyn_Region_d_struct*)_tmp9AE;if(_tmp9C0->tag != 4)goto _LL426;}_LL425:
goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_struct*_tmp9C1=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9AE;if(_tmp9C1->tag != 5)goto _LL428;}_LL427:{const char*_tmpDF4;void*_tmpDF3;(
_tmpDF3=0,Cyc_Tcutil_terr(d->loc,((_tmpDF4="Cyclone declaration within C code",
_tag_dyneither(_tmpDF4,sizeof(char),34))),_tag_dyneither(_tmpDF3,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_struct*_tmp9C2=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9AE;if(_tmp9C2->tag != 14)goto _LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9C3=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9AE;if(_tmp9C3->tag != 15)goto
_LL40B;}_LL42B: goto _LL40B;_LL40B:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9CA=s->r;
struct Cyc_Absyn_Exp*_tmp9CC;struct Cyc_Absyn_Stmt*_tmp9CE;struct Cyc_Absyn_Stmt*
_tmp9CF;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Stmt*
_tmp9D4;struct Cyc_Absyn_Stmt*_tmp9D5;struct _tuple8 _tmp9D7;struct Cyc_Absyn_Exp*
_tmp9D8;struct Cyc_Absyn_Stmt*_tmp9D9;struct Cyc_Absyn_Exp*_tmp9DB;struct _tuple8
_tmp9DC;struct Cyc_Absyn_Exp*_tmp9DD;struct _tuple8 _tmp9DE;struct Cyc_Absyn_Exp*
_tmp9DF;struct Cyc_Absyn_Stmt*_tmp9E0;struct Cyc_Absyn_Exp*_tmp9E2;struct Cyc_List_List*
_tmp9E3;struct Cyc_Absyn_Decl*_tmp9E5;struct Cyc_Absyn_Stmt*_tmp9E6;struct Cyc_Absyn_Stmt*
_tmp9E8;struct _tuple8 _tmp9E9;struct Cyc_Absyn_Exp*_tmp9EA;_LL42D: {struct Cyc_Absyn_Exp_s_struct*
_tmp9CB=(struct Cyc_Absyn_Exp_s_struct*)_tmp9CA;if(_tmp9CB->tag != 1)goto _LL42F;
else{_tmp9CC=_tmp9CB->f1;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9CC);goto _LL42C;
_LL42F: {struct Cyc_Absyn_Seq_s_struct*_tmp9CD=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9CA;if(_tmp9CD->tag != 2)goto _LL431;else{_tmp9CE=_tmp9CD->f1;_tmp9CF=_tmp9CD->f2;}}
_LL430: Cyc_Toc_stmttypes_to_c(_tmp9CE);Cyc_Toc_stmttypes_to_c(_tmp9CF);goto
_LL42C;_LL431: {struct Cyc_Absyn_Return_s_struct*_tmp9D0=(struct Cyc_Absyn_Return_s_struct*)
_tmp9CA;if(_tmp9D0->tag != 3)goto _LL433;else{_tmp9D1=_tmp9D0->f1;}}_LL432: if(
_tmp9D1 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9D1);goto _LL42C;
_LL433: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9D2=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9CA;if(_tmp9D2->tag != 4)goto _LL435;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;
_tmp9D5=_tmp9D2->f3;}}_LL434: Cyc_Toc_exptypes_to_c(_tmp9D3);Cyc_Toc_stmttypes_to_c(
_tmp9D4);Cyc_Toc_stmttypes_to_c(_tmp9D5);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_struct*
_tmp9D6=(struct Cyc_Absyn_While_s_struct*)_tmp9CA;if(_tmp9D6->tag != 5)goto _LL437;
else{_tmp9D7=_tmp9D6->f1;_tmp9D8=_tmp9D7.f1;_tmp9D9=_tmp9D6->f2;}}_LL436: Cyc_Toc_exptypes_to_c(
_tmp9D8);Cyc_Toc_stmttypes_to_c(_tmp9D9);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_struct*
_tmp9DA=(struct Cyc_Absyn_For_s_struct*)_tmp9CA;if(_tmp9DA->tag != 9)goto _LL439;
else{_tmp9DB=_tmp9DA->f1;_tmp9DC=_tmp9DA->f2;_tmp9DD=_tmp9DC.f1;_tmp9DE=_tmp9DA->f3;
_tmp9DF=_tmp9DE.f1;_tmp9E0=_tmp9DA->f4;}}_LL438: Cyc_Toc_exptypes_to_c(_tmp9DB);
Cyc_Toc_exptypes_to_c(_tmp9DD);Cyc_Toc_exptypes_to_c(_tmp9DF);Cyc_Toc_stmttypes_to_c(
_tmp9E0);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_struct*_tmp9E1=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9CA;if(_tmp9E1->tag != 10)goto _LL43B;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E1->f2;}}
_LL43A: Cyc_Toc_exptypes_to_c(_tmp9E2);for(0;_tmp9E3 != 0;_tmp9E3=_tmp9E3->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9E3->hd)->body);}goto _LL42C;_LL43B: {struct
Cyc_Absyn_Decl_s_struct*_tmp9E4=(struct Cyc_Absyn_Decl_s_struct*)_tmp9CA;if(
_tmp9E4->tag != 12)goto _LL43D;else{_tmp9E5=_tmp9E4->f1;_tmp9E6=_tmp9E4->f2;}}
_LL43C: Cyc_Toc_decltypes_to_c(_tmp9E5);Cyc_Toc_stmttypes_to_c(_tmp9E6);goto
_LL42C;_LL43D: {struct Cyc_Absyn_Do_s_struct*_tmp9E7=(struct Cyc_Absyn_Do_s_struct*)
_tmp9CA;if(_tmp9E7->tag != 14)goto _LL43F;else{_tmp9E8=_tmp9E7->f1;_tmp9E9=_tmp9E7->f2;
_tmp9EA=_tmp9E9.f1;}}_LL43E: Cyc_Toc_stmttypes_to_c(_tmp9E8);Cyc_Toc_exptypes_to_c(
_tmp9EA);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_struct*_tmp9EB=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9CA;if(_tmp9EB->tag != 0)goto _LL441;}_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_struct*
_tmp9EC=(struct Cyc_Absyn_Break_s_struct*)_tmp9CA;if(_tmp9EC->tag != 6)goto _LL443;}
_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_struct*_tmp9ED=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9CA;if(_tmp9ED->tag != 7)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_struct*
_tmp9EE=(struct Cyc_Absyn_Goto_s_struct*)_tmp9CA;if(_tmp9EE->tag != 8)goto _LL447;}
_LL446: goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9EF=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9CA;if(_tmp9EF->tag != 11)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_struct*
_tmp9F0=(struct Cyc_Absyn_Label_s_struct*)_tmp9CA;if(_tmp9F0->tag != 13)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9F1=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9CA;if(_tmp9F1->tag != 15)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9F2=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9CA;if(_tmp9F2->tag != 16)goto
_LL42C;}_LL44E:{const char*_tmpDF7;void*_tmpDF6;(_tmpDF6=0,Cyc_Tcutil_terr(s->loc,((
_tmpDF7="Cyclone statement in C code",_tag_dyneither(_tmpDF7,sizeof(char),28))),
_tag_dyneither(_tmpDF6,sizeof(void*),0)));}goto _LL42C;_LL42C:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDFA;void*
_tmpDF9;(_tmpDF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDFA="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDFA,sizeof(char),29))),
_tag_dyneither(_tmpDF9,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9F7=d->r;struct Cyc_Absyn_Vardecl*_tmp9F9;struct Cyc_Absyn_Fndecl*
_tmp9FB;struct Cyc_Absyn_Aggrdecl*_tmpA01;struct Cyc_Absyn_Datatypedecl*_tmpA03;
struct Cyc_Absyn_Enumdecl*_tmpA05;struct Cyc_Absyn_Typedefdecl*_tmpA07;struct Cyc_List_List*
_tmpA0B;struct Cyc_List_List*_tmpA0D;struct Cyc_List_List*_tmpA0F;struct Cyc_List_List*
_tmpA11;_LL450: {struct Cyc_Absyn_Var_d_struct*_tmp9F8=(struct Cyc_Absyn_Var_d_struct*)
_tmp9F7;if(_tmp9F8->tag != 0)goto _LL452;else{_tmp9F9=_tmp9F8->f1;}}_LL451: {
struct _tuple0*_tmpA12=_tmp9F9->name;if(_tmp9F9->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDFB;_tmpA12=((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->f1=Cyc_Absyn_Rel_n(
0),((_tmpDFB->f2=(*_tmpA12).f2,_tmpDFB))))));}if(_tmp9F9->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F9->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9F9->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDFE;struct Cyc_Absyn_Global_b_struct*_tmpDFD;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F9->name,Cyc_Absyn_varb_exp(_tmpA12,(void*)((
_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=1,((_tmpDFE.f1=
_tmp9F9,_tmpDFE)))),_tmpDFD)))),0));}_tmp9F9->name=_tmpA12;_tmp9F9->sc=Cyc_Toc_scope_to_c(
_tmp9F9->sc);_tmp9F9->type=Cyc_Toc_typ_to_c_array(_tmp9F9->type);{struct Cyc_List_List*
_tmpDFF;Cyc_Toc_result_decls=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->hd=
d,((_tmpDFF->tl=Cyc_Toc_result_decls,_tmpDFF))))));}goto _LL44F;}_LL452: {struct
Cyc_Absyn_Fn_d_struct*_tmp9FA=(struct Cyc_Absyn_Fn_d_struct*)_tmp9F7;if(_tmp9FA->tag
!= 1)goto _LL454;else{_tmp9FB=_tmp9FA->f1;}}_LL453: {struct _tuple0*_tmpA17=
_tmp9FB->name;if(_tmp9FB->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpE00;_tmpA17=((
_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->f1=Cyc_Absyn_Rel_n(0),((_tmpE00->f2=(*
_tmpA17).f2,_tmpE00))))));}_tmp9FB->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9FB->name,Cyc_Absyn_var_exp(_tmpA17,0));_tmp9FB->name=_tmpA17;Cyc_Toc_fndecl_to_c(
nv,_tmp9FB,cinclude);{struct Cyc_List_List*_tmpE01;Cyc_Toc_result_decls=((_tmpE01=
_cycalloc(sizeof(*_tmpE01)),((_tmpE01->hd=d,((_tmpE01->tl=Cyc_Toc_result_decls,
_tmpE01))))));}goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_struct*_tmp9FC=(struct
Cyc_Absyn_Let_d_struct*)_tmp9F7;if(_tmp9FC->tag != 2)goto _LL456;}_LL455: goto
_LL457;_LL456: {struct Cyc_Absyn_Letv_d_struct*_tmp9FD=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9F7;if(_tmp9FD->tag != 3)goto _LL458;}_LL457: {const char*_tmpE04;void*_tmpE03;(
_tmpE03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE04="letdecl at toplevel",_tag_dyneither(_tmpE04,sizeof(char),20))),
_tag_dyneither(_tmpE03,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_struct*
_tmp9FE=(struct Cyc_Absyn_Region_d_struct*)_tmp9F7;if(_tmp9FE->tag != 4)goto _LL45A;}
_LL459: {const char*_tmpE07;void*_tmpE06;(_tmpE06=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE07="region decl at toplevel",
_tag_dyneither(_tmpE07,sizeof(char),24))),_tag_dyneither(_tmpE06,sizeof(void*),0)));}
_LL45A: {struct Cyc_Absyn_Alias_d_struct*_tmp9FF=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9F7;if(_tmp9FF->tag != 5)goto _LL45C;}_LL45B: {const char*_tmpE0A;void*_tmpE09;(
_tmpE09=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE0A="alias decl at toplevel",_tag_dyneither(_tmpE0A,sizeof(char),23))),
_tag_dyneither(_tmpE09,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA00=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9F7;if(_tmpA00->tag != 6)goto _LL45E;
else{_tmpA01=_tmpA00->f1;}}_LL45D: Cyc_Toc_aggrdecl_to_c(_tmpA01);goto _LL44F;
_LL45E: {struct Cyc_Absyn_Datatype_d_struct*_tmpA02=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9F7;if(_tmpA02->tag != 7)goto _LL460;else{_tmpA03=_tmpA02->f1;}}_LL45F: if(
_tmpA03->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA03);else{Cyc_Toc_datatypedecl_to_c(
_tmpA03);}goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_struct*_tmpA04=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9F7;if(_tmpA04->tag != 8)goto _LL462;else{_tmpA05=_tmpA04->f1;}}_LL461: Cyc_Toc_enumdecl_to_c(
nv,_tmpA05);{struct Cyc_List_List*_tmpE0B;Cyc_Toc_result_decls=((_tmpE0B=
_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->hd=d,((_tmpE0B->tl=Cyc_Toc_result_decls,
_tmpE0B))))));}goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_struct*_tmpA06=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9F7;if(_tmpA06->tag != 9)goto _LL464;else{
_tmpA07=_tmpA06->f1;}}_LL463: _tmpA07->name=_tmpA07->name;_tmpA07->tvs=0;if(
_tmpA07->defn != 0){struct Cyc_Core_Opt*_tmpE0C;_tmpA07->defn=((_tmpE0C=_cycalloc(
sizeof(*_tmpE0C)),((_tmpE0C->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA07->defn))->v),_tmpE0C))));}else{switch(((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmpA07->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL470:{struct Cyc_Core_Opt*_tmpE0D;_tmpA07->defn=((_tmpE0D=_cycalloc(sizeof(*
_tmpE0D)),((_tmpE0D->v=Cyc_Absyn_void_star_typ(),_tmpE0D))));}break;default:
_LL471:{struct Cyc_Core_Opt*_tmpE0E;_tmpA07->defn=((_tmpE0E=_cycalloc(sizeof(*
_tmpE0E)),((_tmpE0E->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE0E))));}break;}}{
struct Cyc_List_List*_tmpE0F;Cyc_Toc_result_decls=((_tmpE0F=_cycalloc(sizeof(*
_tmpE0F)),((_tmpE0F->hd=d,((_tmpE0F->tl=Cyc_Toc_result_decls,_tmpE0F))))));}goto
_LL44F;_LL464: {struct Cyc_Absyn_Porton_d_struct*_tmpA08=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9F7;if(_tmpA08->tag != 14)goto _LL466;}_LL465: goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA09=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9F7;if(_tmpA09->tag != 15)goto
_LL468;}_LL467: goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_struct*_tmpA0A=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9F7;if(_tmpA0A->tag != 10)goto _LL46A;
else{_tmpA0B=_tmpA0A->f2;}}_LL469: _tmpA0D=_tmpA0B;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_struct*
_tmpA0C=(struct Cyc_Absyn_Using_d_struct*)_tmp9F7;if(_tmpA0C->tag != 11)goto _LL46C;
else{_tmpA0D=_tmpA0C->f2;}}_LL46B: _tmpA0F=_tmpA0D;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA0E=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9F7;if(_tmpA0E->tag != 12)goto
_LL46E;else{_tmpA0F=_tmpA0E->f1;}}_LL46D: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0F,top,
cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA10=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9F7;if(_tmpA10->tag != 13)goto _LL44F;
else{_tmpA11=_tmpA10->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA11,top,1);goto
_LL44F;_LL44F:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA26;struct Cyc_Core_NewRegion _tmpA25=Cyc_Core_new_dynregion();
_tmpA26=_tmpA25.dynregion;{struct _DynRegionFrame _tmpA27;struct _RegionHandle*d=
_open_dynregion(& _tmpA27,_tmpA26);{struct Cyc_Dict_Dict*_tmpE1A;struct Cyc_Dict_Dict*
_tmpE19;struct Cyc_Set_Set**_tmpE18;struct Cyc_Dict_Dict*_tmpE17;struct Cyc_List_List**
_tmpE16;struct Cyc_Toc_TocState*_tmpE15;Cyc_Toc_toc_state=((_tmpE15=_cycalloc(
sizeof(*_tmpE15)),((_tmpE15->dyn=(struct _DynRegionHandle*)_tmpA26,((_tmpE15->tuple_types=(
struct Cyc_List_List**)((_tmpE16=_region_malloc(d,sizeof(*_tmpE16)),((_tmpE16[0]=
0,_tmpE16)))),((_tmpE15->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE17=
_region_malloc(d,sizeof(*_tmpE17)),((_tmpE17[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE17)))),((_tmpE15->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE18=
_region_malloc(d,sizeof(*_tmpE18)),((_tmpE18[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE18)))),((_tmpE15->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE19=
_region_malloc(d,sizeof(*_tmpE19)),((_tmpE19[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE19)))),((_tmpE15->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE1A=_region_malloc(
d,sizeof(*_tmpE1A)),((_tmpE1A[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE1A)))),((_tmpE15->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE15))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE1B;Cyc_Toc_globals=_tag_dyneither(((_tmpE1B=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE1B[0]=& Cyc_Toc__throw_str,((_tmpE1B[1]=&
Cyc_Toc_setjmp_str,((_tmpE1B[2]=& Cyc_Toc__push_handler_str,((_tmpE1B[3]=& Cyc_Toc__pop_handler_str,((
_tmpE1B[4]=& Cyc_Toc__exn_thrown_str,((_tmpE1B[5]=& Cyc_Toc__npop_handler_str,((
_tmpE1B[6]=& Cyc_Toc__check_null_str,((_tmpE1B[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE1B[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE1B[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE1B[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE1B[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE1B[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE1B[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE1B[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE1B[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE1B[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE1B[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE1B[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE1B[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE1B[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE1B[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE1B[22]=& Cyc_Toc__cycalloc_str,((_tmpE1B[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE1B[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE1B[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE1B[26]=& Cyc_Toc__region_malloc_str,((_tmpE1B[27]=& Cyc_Toc__region_calloc_str,((
_tmpE1B[28]=& Cyc_Toc__check_times_str,((_tmpE1B[29]=& Cyc_Toc__new_region_str,((
_tmpE1B[30]=& Cyc_Toc__push_region_str,((_tmpE1B[31]=& Cyc_Toc__pop_region_str,((
_tmpE1B[32]=& Cyc_Toc__open_dynregion_str,((_tmpE1B[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE1B[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE1B[35]=& Cyc_Toc__reset_region_str,((
_tmpE1B[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE1B[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE1B)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA2F=_new_region("start");struct
_RegionHandle*start=& _tmpA2F;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA31;struct Cyc_Toc_TocState _tmpA30=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA31=_tmpA30.dyn;Cyc_Core_free_dynregion(
_tmpA31);};{struct Cyc_List_List*_tmpA32=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA32;};;
_pop_region(start);};}
