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
void*root;struct Cyc_List_List*fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
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
struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
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
Cyc_stderr,fmt,ap);{const char*_tmpA32;void*_tmpA31;(_tmpA31=0,Cyc_fprintf(Cyc_stderr,((
_tmpA32="\n",_tag_dyneither(_tmpA32,sizeof(char),2))),_tag_dyneither(_tmpA31,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA35;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA34;(int)_throw((void*)((
_tmpA34=_cycalloc_atomic(sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA35)),_tmpA34)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA38;void*
_tmpA37;(_tmpA37=0,Cyc_fprintf(Cyc_stderr,((_tmpA38="\n",_tag_dyneither(_tmpA38,
sizeof(char),2))),_tag_dyneither(_tmpA37,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA3B;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA3A;(int)_throw((void*)((_tmpA3A=_cycalloc_atomic(sizeof(*_tmpA3A)),((
_tmpA3A[0]=((_tmpA3B.tag=Cyc_Toc_Toc_Impossible,_tmpA3B)),_tmpA3A)))));};}char
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
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA3C;skip_stmt_opt=((_tmpA3C=
_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C[0]=Cyc_Absyn_skip_stmt(0),_tmpA3C))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA3F;struct Cyc_Absyn_Cast_e_struct*_tmpA3E;
return(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=15,((
_tmpA3F.f1=(void*)t,((_tmpA3F.f2=e,((_tmpA3F.f3=0,((_tmpA3F.f4=Cyc_Absyn_No_coercion,
_tmpA3F)))))))))),_tmpA3E))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA42;struct Cyc_Absyn_Deref_e_struct*_tmpA41;return(void*)((_tmpA41=_cycalloc(
sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=21,((_tmpA42.f1=e,_tmpA42)))),
_tmpA41))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA45;struct Cyc_Absyn_Subscript_e_struct*
_tmpA44;return(void*)((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((
_tmpA45.tag=24,((_tmpA45.f1=e1,((_tmpA45.f2=e2,_tmpA45)))))),_tmpA44))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA48;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA47;return(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((
_tmpA48.tag=37,((_tmpA48.f1=s,_tmpA48)))),_tmpA47))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA4B;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4A;return(void*)((_tmpA4A=
_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=18,((_tmpA4B.f1=(void*)t,
_tmpA4B)))),_tmpA4A))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA4E;struct Cyc_Absyn_FnCall_e_struct*
_tmpA4D;return(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((
_tmpA4E.tag=11,((_tmpA4E.f1=e,((_tmpA4E.f2=es,((_tmpA4E.f3=0,_tmpA4E)))))))),
_tmpA4D))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA51;struct Cyc_Absyn_Exp_s_struct*
_tmpA50;return(void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((
_tmpA51.tag=1,((_tmpA51.f1=e,_tmpA51)))),_tmpA50))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA54;struct Cyc_Absyn_Seq_s_struct*_tmpA53;return(void*)((_tmpA53=_cycalloc(
sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=2,((_tmpA54.f1=s1,((_tmpA54.f2=s2,
_tmpA54)))))),_tmpA53))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA57;struct Cyc_Absyn_Conditional_e_struct*_tmpA56;return(void*)((_tmpA56=
_cycalloc(sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=6,((_tmpA57.f1=e1,((
_tmpA57.f2=e2,((_tmpA57.f3=e3,_tmpA57)))))))),_tmpA56))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA5A;struct Cyc_Absyn_AggrMember_e_struct*_tmpA59;return(void*)((_tmpA59=
_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=22,((_tmpA5A.f1=e,((
_tmpA5A.f2=n,((_tmpA5A.f3=0,((_tmpA5A.f4=0,_tmpA5A)))))))))),_tmpA59))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA5D;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA5C;return(void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((
_tmpA5D.tag=23,((_tmpA5D.f1=e,((_tmpA5D.f2=n,((_tmpA5D.f3=0,((_tmpA5D.f4=0,
_tmpA5D)))))))))),_tmpA5C))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA60;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA5F;return(void*)((_tmpA5F=_cycalloc(
sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=36,((_tmpA60.f1=tdopt,((_tmpA60.f2=
ds,_tmpA60)))))),_tmpA5F))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA63;
struct Cyc_Absyn_Goto_s_struct*_tmpA62;return(void*)((_tmpA62=_cycalloc(sizeof(*
_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=8,((_tmpA63.f1=v,((_tmpA63.f2=s,_tmpA63)))))),
_tmpA62))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA69;const char*_tmpA68;struct Cyc_Core_Impossible_struct*_tmpA67;(int)_throw((
void*)((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA69.tag=Cyc_Core_Impossible,((
_tmpA69.f1=((_tmpA68="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA68,sizeof(char),44))),_tmpA69)))),_tmpA67)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA76;const char*_tmpA75;void*_tmpA74[1];
struct Cyc_String_pa_struct _tmpA73;struct Cyc_Core_Impossible_struct*_tmpA72;(int)
_throw((void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA76.tag=
Cyc_Core_Impossible,((_tmpA76.f1=(struct _dyneither_ptr)((_tmpA73.tag=0,((_tmpA73.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA74[
0]=& _tmpA73,Cyc_aprintf(((_tmpA75="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA75,sizeof(char),67))),_tag_dyneither(_tmpA74,sizeof(void*),1)))))))),
_tmpA76)))),_tmpA72)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
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
_tmpA7C;const char*_tmpA7B;struct Cyc_Core_Impossible_struct*_tmpA7A;(int)_throw((
void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA7C.tag=Cyc_Core_Impossible,((
_tmpA7C.f1=((_tmpA7B="impossible type (not pointer)",_tag_dyneither(_tmpA7B,
sizeof(char),30))),_tmpA7C)))),_tmpA7A)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
_LL34:;_LL35: {const char*_tmpA7F;void*_tmpA7E;(_tmpA7E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7F="is_nullable",
_tag_dyneither(_tmpA7F,sizeof(char),12))),_tag_dyneither(_tmpA7E,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA80;pair=((_tmpA80.f1=x,((_tmpA80.f2=tag,_tmpA80))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA81;struct _tuple13*_tmp108=(_tmpA81=_cycalloc(sizeof(*_tmpA81)),((
_tmpA81->f1=x,((_tmpA81->f2=tag,_tmpA81)))));struct _dyneither_ptr*_tmpA84;struct
_tuple0*_tmpA83;struct _tuple0*res=(_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->f1=(*
x).f1,((_tmpA83->f2=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA84)))),_tmpA83)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA8C;void*_tmpA8B[
1];const char*_tmpA8A;struct _dyneither_ptr*_tmpA89;struct _dyneither_ptr*xname=(
_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=(struct _dyneither_ptr)((_tmpA8C.tag=
1,((_tmpA8C.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA8B[0]=& _tmpA8C,
Cyc_aprintf(((_tmpA8A="_tuple%d",_tag_dyneither(_tmpA8A,sizeof(char),9))),
_tag_dyneither(_tmpA8B,sizeof(void*),1)))))))),_tmpA89)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA8F;struct Cyc_List_List*
_tmpA8E;_tmp11A=((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->hd=((_tmpA8F=
_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->name=Cyc_Absyn_fieldname(i),((_tmpA8F->tq=
Cyc_Toc_mt_tq,((_tmpA8F->type=(void*)ts2->hd,((_tmpA8F->width=0,((_tmpA8F->attributes=
0,_tmpA8F)))))))))))),((_tmpA8E->tl=_tmp11A,_tmpA8E))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA94;struct _tuple0*_tmpA93;struct Cyc_Absyn_Aggrdecl*_tmpA92;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->kind=Cyc_Absyn_StructA,((
_tmpA92->sc=Cyc_Absyn_Public,((_tmpA92->name=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((
_tmpA93->f1=Cyc_Absyn_Rel_n(0),((_tmpA93->f2=xname,_tmpA93)))))),((_tmpA92->tvs=
0,((_tmpA92->impl=((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->exist_vars=0,((
_tmpA94->rgn_po=0,((_tmpA94->fields=_tmp11A,((_tmpA94->tagged=0,_tmpA94)))))))))),((
_tmpA92->attributes=0,_tmpA92)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA9A;struct Cyc_Absyn_Aggr_d_struct _tmpA99;struct Cyc_List_List*_tmpA98;Cyc_Toc_result_decls=((
_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=Cyc_Absyn_new_decl((void*)((
_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA99.tag=6,((_tmpA99.f1=
_tmp11D,_tmpA99)))),_tmpA9A)))),0),((_tmpA98->tl=Cyc_Toc_result_decls,_tmpA98))))));}{
struct _tuple16*_tmpA9D;struct Cyc_List_List*_tmpA9C;*_tmp111=((_tmpA9C=
_region_malloc(d,sizeof(*_tmpA9C)),((_tmpA9C->hd=((_tmpA9D=_region_malloc(d,
sizeof(*_tmpA9D)),((_tmpA9D->f1=x,((_tmpA9D->f2=ts,_tmpA9D)))))),((_tmpA9C->tl=*
_tmp111,_tmpA9C))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpAAA;const char*
_tmpAA9;void*_tmpAA8[1];struct Cyc_Int_pa_struct _tmpAA7;struct _tuple0*_tmpAA6;
struct _tuple0*res=(_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->f1=Cyc_Absyn_Loc_n,((
_tmpAA6->f2=((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=(struct
_dyneither_ptr)((_tmpAA7.tag=1,((_tmpAA7.f1=(unsigned int)_tmp12B,((_tmpAA8[0]=&
_tmpAA7,Cyc_aprintf(((_tmpAA9="_tmp%X",_tag_dyneither(_tmpAA9,sizeof(char),7))),
_tag_dyneither(_tmpAA8,sizeof(void*),1)))))))),_tmpAAA)))),_tmpAA6)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAB2;void*_tmpAB1[1];const char*_tmpAB0;struct _dyneither_ptr*_tmpAAF;struct
_dyneither_ptr*res=(_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=(struct
_dyneither_ptr)((_tmpAB2.tag=1,((_tmpAB2.f1=(unsigned int)_tmp135,((_tmpAB1[0]=&
_tmpAB2,Cyc_aprintf(((_tmpAB0="_LL%X",_tag_dyneither(_tmpAB0,sizeof(char),6))),
_tag_dyneither(_tmpAB1,sizeof(void*),1)))))))),_tmpAAF)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAB5;void*_tmpAB4;(_tmpAB4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB5="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAB5,sizeof(char),43))),_tag_dyneither(_tmpAB4,sizeof(void*),0)));}{
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
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAB6;
return(_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->f1=_tmp140,((_tmpAB6->f2=
_tmp141,((_tmpAB6->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAB6)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpAB7;return(_tmpAB7=
_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->f1=_tmp145,((_tmpAB7->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpAB7)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
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
_tmpAB8;return(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->name=f->name,((
_tmpAB8->tq=Cyc_Toc_mt_tq,((_tmpAB8->type=Cyc_Toc_typ_to_c(f->type),((_tmpAB8->width=
f->width,((_tmpAB8->attributes=f->attributes,_tmpAB8)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAB9;cs=((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((
_tmpAB9[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAB9))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABA;r=((_tmpABA=
_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABA))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABB;r=((_tmpABB=
_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABB))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
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
const char*_tmpABC;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpABC="_struct",_tag_dyneither(_tmpABC,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpABF;void*_tmpABE;(_tmpABE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABF="unresolved DatatypeFieldType",
_tag_dyneither(_tmpABF,sizeof(char),29))),_tag_dyneither(_tmpABE,sizeof(void*),0)));}
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
_tmp1A2;if(_tmp1A5->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1A6=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1A2;if(_tmp1A6->tag != 21)
goto _LL84;}_LL83: continue;_LL84: {struct Cyc_Absyn_Initializes_att_struct*_tmp1A7=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1A2;if(_tmp1A7->tag != 20)goto _LL86;}
_LL85: continue;_LL86:;_LL87:{struct Cyc_List_List*_tmpAC0;_tmp1A1=((_tmpAC0=
_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->hd=(void*)_tmp17F->hd,((_tmpAC0->tl=
_tmp1A1,_tmpAC0))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A9=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1AA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAC1;struct _tuple7*_tmp1AB=(_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((
_tmpAC1->f1=_tmp17E->name,((_tmpAC1->f2=_tmp17E->tq,((_tmpAC1->f3=_tmp1AA,
_tmpAC1)))))));struct Cyc_List_List*_tmpAC2;_tmp1A9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A9,((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->hd=_tmp1AB,((_tmpAC2->tl=0,
_tmpAC2)))))));}{struct Cyc_Absyn_FnType_struct _tmpAC8;struct Cyc_Absyn_FnInfo
_tmpAC7;struct Cyc_Absyn_FnType_struct*_tmpAC6;return(void*)((_tmpAC6=_cycalloc(
sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC8.tag=10,((_tmpAC8.f1=((_tmpAC7.tvars=0,((
_tmpAC7.effect=0,((_tmpAC7.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpAC7.args=
_tmp1A9,((_tmpAC7.c_varargs=_tmp17D,((_tmpAC7.cyc_varargs=0,((_tmpAC7.rgn_po=0,((
_tmpAC7.attributes=_tmp1A1,_tmpAC7)))))))))))))))),_tmpAC8)))),_tmpAC6))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpACB;struct Cyc_Absyn_AnonAggrType_struct*
_tmpACA;return(void*)((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=((
_tmpACB.tag=13,((_tmpACB.f1=_tmp183,((_tmpACB.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpACB)))))),_tmpACA))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp185=(struct Cyc_Absyn_AggrType_struct*)
_tmp158;if(_tmp185->tag != 12)goto _LL5E;else{_tmp186=_tmp185->f1;_tmp187=_tmp186.aggr_info;
_tmp188=_tmp186.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B3=_tmp187;struct
_tuple2 _tmp1B4;_LL89: if((_tmp1B3.UnknownAggr).tag != 1)goto _LL8B;_tmp1B4=(struct
_tuple2)(_tmp1B3.UnknownAggr).val;_LL8A: return t;_LL8B:;_LL8C: goto _LL88;_LL88:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B5=Cyc_Absyn_get_known_aggrdecl(_tmp187);if(
_tmp1B5->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B5->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B5->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp189=(struct Cyc_Absyn_EnumType_struct*)
_tmp158;if(_tmp189->tag != 14)goto _LL60;else{_tmp18A=_tmp189->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18B=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp158;if(_tmp18B->tag != 15)goto _LL62;else{_tmp18C=_tmp18B->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18C);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp158;if(_tmp18D->tag != 18)goto _LL64;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;
_tmp190=_tmp18D->f3;_tmp191=_tmp18D->f4;}}_LL63: if(_tmp191 == 0  || Cyc_noexpand_r){
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpACE;struct Cyc_Absyn_TypedefType_struct*
_tmpACD;return(void*)((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=((
_tmpACE.tag=18,((_tmpACE.f1=_tmp18E,((_tmpACE.f2=0,((_tmpACE.f3=_tmp190,((
_tmpACE.f4=0,_tmpACE)))))))))),_tmpACD))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAD4;void**_tmpAD3;struct Cyc_Absyn_TypedefType_struct*_tmpAD2;return(void*)((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD4.tag=18,((_tmpAD4.f1=
_tmp18E,((_tmpAD4.f2=0,((_tmpAD4.f3=_tmp190,((_tmpAD4.f4=((_tmpAD3=_cycalloc(
sizeof(*_tmpAD3)),((_tmpAD3[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAD3)))),
_tmpAD4)))))))))),_tmpAD2))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD7="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAD7,sizeof(char),55))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpADA;void*_tmpAD9;(
_tmpAD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADA="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpADA,sizeof(char),57))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpADD;void*_tmpADC;(
_tmpADC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADD="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpADD,sizeof(char),49))),_tag_dyneither(_tmpADC,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAE0;void*_tmpADF;(
_tmpADF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE0="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAE0,sizeof(char),51))),_tag_dyneither(_tmpADF,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C3=t;struct
Cyc_Absyn_ArrayInfo _tmp1C5;void*_tmp1C6;struct Cyc_Absyn_Tqual _tmp1C7;_LL8E: {
struct Cyc_Absyn_ArrayType_struct*_tmp1C4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C3;if(_tmp1C4->tag != 9)goto _LL90;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.elt_type;
_tmp1C7=_tmp1C5.tq;}}_LL8F: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C6,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1C7,Cyc_Absyn_false_conref),e);_LL90:;_LL91:
return Cyc_Toc_cast_it(t,e);_LL8D:;}static int Cyc_Toc_atomic_typ(void*t);static int
Cyc_Toc_atomic_typ(void*t){void*_tmp1C8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp1D3;void*_tmp1D4;struct Cyc_Absyn_AggrInfo _tmp1D6;union Cyc_Absyn_AggrInfoU
_tmp1D7;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_DatatypeFieldInfo _tmp1DB;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1DC;struct _tuple1 _tmp1DD;struct Cyc_Absyn_Datatypedecl*
_tmp1DE;struct Cyc_Absyn_Datatypefield*_tmp1DF;struct Cyc_List_List*_tmp1E1;_LL93: {
struct Cyc_Absyn_VoidType_struct*_tmp1C9=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C8;if(_tmp1C9->tag != 0)goto _LL95;}_LL94: return 1;_LL95: {struct Cyc_Absyn_VarType_struct*
_tmp1CA=(struct Cyc_Absyn_VarType_struct*)_tmp1C8;if(_tmp1CA->tag != 2)goto _LL97;}
_LL96: return 0;_LL97: {struct Cyc_Absyn_IntType_struct*_tmp1CB=(struct Cyc_Absyn_IntType_struct*)
_tmp1C8;if(_tmp1CB->tag != 6)goto _LL99;}_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_EnumType_struct*
_tmp1CC=(struct Cyc_Absyn_EnumType_struct*)_tmp1C8;if(_tmp1CC->tag != 14)goto _LL9B;}
_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CD=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C8;if(_tmp1CD->tag != 15)goto _LL9D;}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_FloatType_struct*
_tmp1CE=(struct Cyc_Absyn_FloatType_struct*)_tmp1C8;if(_tmp1CE->tag != 7)goto _LL9F;}
_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_DoubleType_struct*_tmp1CF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C8;if(_tmp1CF->tag != 8)goto _LLA1;}_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_FnType_struct*
_tmp1D0=(struct Cyc_Absyn_FnType_struct*)_tmp1C8;if(_tmp1D0->tag != 10)goto _LLA3;}
_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_TagType_struct*_tmp1D1=(struct Cyc_Absyn_TagType_struct*)
_tmp1C8;if(_tmp1D1->tag != 20)goto _LLA5;}_LLA4: return 1;_LLA5: {struct Cyc_Absyn_ArrayType_struct*
_tmp1D2=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C8;if(_tmp1D2->tag != 9)goto _LLA7;
else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.elt_type;}}_LLA6: return Cyc_Toc_atomic_typ(
_tmp1D4);_LLA7: {struct Cyc_Absyn_AggrType_struct*_tmp1D5=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C8;if(_tmp1D5->tag != 12)goto _LLA9;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D6.aggr_info;}}
_LLA8:{union Cyc_Absyn_AggrInfoU _tmp1E6=_tmp1D7;struct _tuple2 _tmp1E7;_LLBA: if((
_tmp1E6.UnknownAggr).tag != 1)goto _LLBC;_tmp1E7=(struct _tuple2)(_tmp1E6.UnknownAggr).val;
_LLBB: return 0;_LLBC:;_LLBD: goto _LLB9;_LLB9:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E8=
Cyc_Absyn_get_known_aggrdecl(_tmp1D7);if(_tmp1E8->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E8->impl))->fields;for(
0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E9->hd)->type))return 0;}}return 1;};_LLA9: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D8=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C8;if(_tmp1D8->tag != 13)goto
_LLAB;else{_tmp1D9=_tmp1D8->f2;}}_LLAA: for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D9->hd)->type))return 0;}
return 1;_LLAB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1DA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C8;if(_tmp1DA->tag != 4)goto _LLAD;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DB.field_info;
if((_tmp1DC.KnownDatatypefield).tag != 2)goto _LLAD;_tmp1DD=(struct _tuple1)(
_tmp1DC.KnownDatatypefield).val;_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;}}_LLAC:
_tmp1E1=_tmp1DF->typs;goto _LLAE;_LLAD: {struct Cyc_Absyn_TupleType_struct*_tmp1E0=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C8;if(_tmp1E0->tag != 11)goto _LLAF;else{
_tmp1E1=_tmp1E0->f1;}}_LLAE: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1E1->hd)).f2))return 0;}return 1;_LLAF: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1E2=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C8;if(_tmp1E2->tag != 3)goto
_LLB1;}_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_PointerType_struct*_tmp1E3=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C8;if(_tmp1E3->tag != 5)goto _LLB3;}_LLB2:
goto _LLB4;_LLB3: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C8;if(_tmp1E4->tag != 17)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E5=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C8;if(_tmp1E5->tag != 16)goto
_LLB7;}_LLB6: return 0;_LLB7:;_LLB8: {const char*_tmpAE4;void*_tmpAE3[1];struct Cyc_String_pa_struct
_tmpAE2;(_tmpAE2.tag=0,((_tmpAE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAE3[0]=& _tmpAE2,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE4="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAE4,sizeof(char),25))),_tag_dyneither(_tmpAE3,sizeof(void*),1)))))));}
_LL92:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EF;void*
_tmp1F0;_LLBF: {struct Cyc_Absyn_PointerType_struct*_tmp1EE=(struct Cyc_Absyn_PointerType_struct*)
_tmp1ED;if(_tmp1EE->tag != 5)goto _LLC1;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=_tmp1EF.elt_typ;}}
_LLC0: {void*_tmp1F1=Cyc_Tcutil_compress(_tmp1F0);_LLC4: {struct Cyc_Absyn_VoidType_struct*
_tmp1F2=(struct Cyc_Absyn_VoidType_struct*)_tmp1F1;if(_tmp1F2->tag != 0)goto _LLC6;}
_LLC5: return 1;_LLC6:;_LLC7: return 0;_LLC3:;}_LLC1:;_LLC2: return 0;_LLBE:;}static
int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1F3=Cyc_Tcutil_compress(
t);_LLC9: {struct Cyc_Absyn_VarType_struct*_tmp1F4=(struct Cyc_Absyn_VarType_struct*)
_tmp1F3;if(_tmp1F4->tag != 2)goto _LLCB;}_LLCA: return 1;_LLCB:;_LLCC: return 0;_LLC8:;}
static int Cyc_Toc_is_void_star_or_tvar(void*t);static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp1F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F7;union Cyc_Absyn_AggrInfoU _tmp1F8;struct Cyc_List_List*_tmp1FA;_LLCE: {
struct Cyc_Absyn_AggrType_struct*_tmp1F6=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F5;if(_tmp1F6->tag != 12)goto _LLD0;else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F7.aggr_info;}}
_LLCF: {struct Cyc_Absyn_Aggrdecl*_tmp1FB=Cyc_Absyn_get_known_aggrdecl(_tmp1F8);
if(_tmp1FB->impl == 0){const char*_tmpAE7;void*_tmpAE6;(_tmpAE6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE7="is_poly_field: type missing fields",
_tag_dyneither(_tmpAE7,sizeof(char),35))),_tag_dyneither(_tmpAE6,sizeof(void*),0)));}
_tmp1FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FB->impl))->fields;goto
_LLD1;}_LLD0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F9=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F5;if(_tmp1F9->tag != 13)goto _LLD2;else{_tmp1FA=_tmp1F9->f2;}}_LLD1: {struct
Cyc_Absyn_Aggrfield*_tmp1FE=Cyc_Absyn_lookup_field(_tmp1FA,f);if(_tmp1FE == 0){
const char*_tmpAEB;void*_tmpAEA[1];struct Cyc_String_pa_struct _tmpAE9;(_tmpAE9.tag=
0,((_tmpAE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAEA[0]=&
_tmpAE9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEB="is_poly_field: bad field %s",_tag_dyneither(_tmpAEB,sizeof(char),28))),
_tag_dyneither(_tmpAEA,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FE->type);}_LLD2:;_LLD3: {const char*_tmpAEF;void*_tmpAEE[1];struct Cyc_String_pa_struct
_tmpAED;(_tmpAED.tag=0,((_tmpAED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAEE[0]=& _tmpAED,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEF="is_poly_field: bad type %s",
_tag_dyneither(_tmpAEF,sizeof(char),27))),_tag_dyneither(_tmpAEE,sizeof(void*),1)))))));}
_LLCD:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp205=e->r;struct Cyc_Absyn_Exp*_tmp207;struct
_dyneither_ptr*_tmp208;struct Cyc_Absyn_Exp*_tmp20A;struct _dyneither_ptr*_tmp20B;
_LLD5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp206=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp205;if(_tmp206->tag != 22)goto _LLD7;else{_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;}}
_LLD6: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp207->topt))->v,_tmp208);_LLD7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp209=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp205;if(_tmp209->tag != 23)goto _LLD9;else{
_tmp20A=_tmp209->f1;_tmp20B=_tmp209->f2;}}_LLD8: {void*_tmp20C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp20A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20E;void*_tmp20F;_LLDC: {struct Cyc_Absyn_PointerType_struct*_tmp20D=(struct
Cyc_Absyn_PointerType_struct*)_tmp20C;if(_tmp20D->tag != 5)goto _LLDE;else{_tmp20E=
_tmp20D->f1;_tmp20F=_tmp20E.elt_typ;}}_LLDD: return Cyc_Toc_is_poly_field(_tmp20F,
_tmp20B);_LLDE:;_LLDF: {const char*_tmpAF3;void*_tmpAF2[1];struct Cyc_String_pa_struct
_tmpAF1;(_tmpAF1.tag=0,((_tmpAF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp20A->topt))->v)),((
_tmpAF2[0]=& _tmpAF1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAF3="is_poly_project: bad type %s",_tag_dyneither(_tmpAF3,sizeof(char),29))),
_tag_dyneither(_tmpAF2,sizeof(void*),1)))))));}_LLDB:;}_LLD9:;_LLDA: return 0;
_LLD4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAF4;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAF4=_cycalloc(
sizeof(*_tmpAF4)),((_tmpAF4->hd=s,((_tmpAF4->tl=0,_tmpAF4)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAF5;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAF5=_cycalloc(sizeof(*
_tmpAF5)),((_tmpAF5->hd=s,((_tmpAF5->tl=0,_tmpAF5)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAF6[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAF6[1]=s,((_tmpAF6[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAF7[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAF7[1]=n,((
_tmpAF7[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF7,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAF8[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAF8[1]=n,((
_tmpAF8[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAF9[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAF9[2]=n,((_tmpAF9[1]=s,((_tmpAF9[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF9,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAFA;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA->hd=e,((_tmpAFA->tl=0,_tmpAFA)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAFB;throw_match_stmt_opt=((
_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAFB))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp21B=e->r;union Cyc_Absyn_Cnst _tmp21D;struct _dyneither_ptr _tmp21E;_LLE1: {
struct Cyc_Absyn_Const_e_struct*_tmp21C=(struct Cyc_Absyn_Const_e_struct*)_tmp21B;
if(_tmp21C->tag != 0)goto _LLE3;else{_tmp21D=_tmp21C->f1;if((_tmp21D.String_c).tag
!= 7)goto _LLE3;_tmp21E=(struct _dyneither_ptr)(_tmp21D.String_c).val;}}_LLE2:
is_string=1;goto _LLE0;_LLE3:;_LLE4: goto _LLE0;_LLE0:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB01;struct
Cyc_Absyn_Var_d_struct _tmpB00;struct Cyc_List_List*_tmpAFF;Cyc_Toc_result_decls=((
_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->hd=Cyc_Absyn_new_decl((void*)((
_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB00.tag=0,((_tmpB00.f1=vd,
_tmpB00)))),_tmpB01)))),0),((_tmpAFF->tl=Cyc_Toc_result_decls,_tmpAFF))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB08;
struct _tuple15*_tmpB07;struct _tuple15*_tmpB06;struct _tuple15*_tmpB05[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB05[2]=((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((
_tmpB06->f1=0,((_tmpB06->f2=xplussz,_tmpB06)))))),((_tmpB05[1]=((_tmpB07=
_cycalloc(sizeof(*_tmpB07)),((_tmpB07->f1=0,((_tmpB07->f2=xexp,_tmpB07)))))),((
_tmpB05[0]=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->f1=0,((_tmpB08->f2=
xexp,_tmpB08)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB05,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp227;int _tmp228;struct Cyc_Toc_Env*
_tmp226=nv;_tmp227=*_tmp226;_tmp228=_tmp227.toplevel;return _tmp228;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Toc_Env _tmp22A;struct Cyc_Dict_Dict _tmp22B;struct Cyc_Toc_Env*_tmp229=nv;
_tmp22A=*_tmp229;_tmp22B=_tmp22A.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp22B,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB09;return(_tmpB09=_region_malloc(r,
sizeof(*_tmpB09)),((_tmpB09->break_lab=(struct _dyneither_ptr**)0,((_tmpB09->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB09->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB09->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB09->toplevel=(int)1,((_tmpB09->rgn=(struct _RegionHandle*)r,_tmpB09)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22E;struct _dyneither_ptr**_tmp22F;struct _dyneither_ptr**
_tmp230;struct Cyc_Toc_FallthruInfo*_tmp231;struct Cyc_Dict_Dict _tmp232;int _tmp233;
struct Cyc_Toc_Env*_tmp22D=e;_tmp22E=*_tmp22D;_tmp22F=_tmp22E.break_lab;_tmp230=
_tmp22E.continue_lab;_tmp231=_tmp22E.fallthru_info;_tmp232=_tmp22E.varmap;
_tmp233=_tmp22E.toplevel;{struct Cyc_Toc_Env*_tmpB0A;return(_tmpB0A=
_region_malloc(r,sizeof(*_tmpB0A)),((_tmpB0A->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpB0A->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp230),((_tmpB0A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp231,((_tmpB0A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp232),((_tmpB0A->toplevel=(
int)_tmp233,((_tmpB0A->rgn=(struct _RegionHandle*)r,_tmpB0A)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp236;struct _dyneither_ptr**_tmp237;struct
_dyneither_ptr**_tmp238;struct Cyc_Toc_FallthruInfo*_tmp239;struct Cyc_Dict_Dict
_tmp23A;int _tmp23B;struct Cyc_Toc_Env*_tmp235=e;_tmp236=*_tmp235;_tmp237=_tmp236.break_lab;
_tmp238=_tmp236.continue_lab;_tmp239=_tmp236.fallthru_info;_tmp23A=_tmp236.varmap;
_tmp23B=_tmp236.toplevel;{struct Cyc_Toc_Env*_tmpB0B;return(_tmpB0B=
_region_malloc(r,sizeof(*_tmpB0B)),((_tmpB0B->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpB0B->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp238),((_tmpB0B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp239,((_tmpB0B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23A),((_tmpB0B->toplevel=(
int)0,((_tmpB0B->rgn=(struct _RegionHandle*)r,_tmpB0B)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23E;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**
_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;
struct Cyc_Toc_Env*_tmp23D=e;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.break_lab;_tmp240=
_tmp23E.continue_lab;_tmp241=_tmp23E.fallthru_info;_tmp242=_tmp23E.varmap;
_tmp243=_tmp23E.toplevel;{struct Cyc_Toc_Env*_tmpB0C;return(_tmpB0C=
_region_malloc(r,sizeof(*_tmpB0C)),((_tmpB0C->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpB0C->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp240),((_tmpB0C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp241,((_tmpB0C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242),((_tmpB0C->toplevel=(
int)1,((_tmpB0C->rgn=(struct _RegionHandle*)r,_tmpB0C)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp245=(*x).f1;struct Cyc_List_List*_tmp246;_LLE6: if((
_tmp245.Rel_n).tag != 1)goto _LLE8;_tmp246=(struct Cyc_List_List*)(_tmp245.Rel_n).val;
_LLE7: {const char*_tmpB10;void*_tmpB0F[1];struct Cyc_String_pa_struct _tmpB0E;(
_tmpB0E.tag=0,((_tmpB0E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB0F[0]=& _tmpB0E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB10="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB10,sizeof(char),30))),_tag_dyneither(_tmpB0F,sizeof(void*),1)))))));}_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}{struct Cyc_Toc_Env _tmp24B;struct _dyneither_ptr**_tmp24C;
struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*_tmp24E;struct Cyc_Dict_Dict
_tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;
_tmp24D=_tmp24B.continue_lab;_tmp24E=_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;
_tmp250=_tmp24B.toplevel;{struct Cyc_Dict_Dict _tmp251=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F),x,y);struct Cyc_Toc_Env*_tmpB11;return(_tmpB11=_region_malloc(r,
sizeof(*_tmpB11)),((_tmpB11->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24C),((_tmpB11->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24D),((_tmpB11->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24E,((_tmpB11->varmap=(struct Cyc_Dict_Dict)_tmp251,((_tmpB11->toplevel=(int)
_tmp250,((_tmpB11->rgn=(struct _RegionHandle*)r,_tmpB11)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp254;struct _dyneither_ptr**_tmp255;struct _dyneither_ptr**
_tmp256;struct Cyc_Toc_FallthruInfo*_tmp257;struct Cyc_Dict_Dict _tmp258;int _tmp259;
struct Cyc_Toc_Env*_tmp253=e;_tmp254=*_tmp253;_tmp255=_tmp254.break_lab;_tmp256=
_tmp254.continue_lab;_tmp257=_tmp254.fallthru_info;_tmp258=_tmp254.varmap;
_tmp259=_tmp254.toplevel;{struct Cyc_Toc_Env*_tmpB12;return(_tmpB12=
_region_malloc(r,sizeof(*_tmpB12)),((_tmpB12->break_lab=(struct _dyneither_ptr**)
0,((_tmpB12->continue_lab=(struct _dyneither_ptr**)0,((_tmpB12->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp257,((_tmpB12->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp258),((_tmpB12->toplevel=(int)_tmp259,((_tmpB12->rgn=(struct _RegionHandle*)
r,_tmpB12)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB13;fallthru_vars=((
_tmpB13=_region_malloc(r,sizeof(*_tmpB13)),((_tmpB13->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB13->tl=fallthru_vars,_tmpB13))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25D;struct _dyneither_ptr**_tmp25E;struct
_dyneither_ptr**_tmp25F;struct Cyc_Toc_FallthruInfo*_tmp260;struct Cyc_Dict_Dict
_tmp261;int _tmp262;struct Cyc_Toc_Env*_tmp25C=e;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.break_lab;
_tmp25F=_tmp25D.continue_lab;_tmp260=_tmp25D.fallthru_info;_tmp261=_tmp25D.varmap;
_tmp262=_tmp25D.toplevel;{struct Cyc_Toc_Env _tmp264;struct Cyc_Dict_Dict _tmp265;
struct Cyc_Toc_Env*_tmp263=next_case_env;_tmp264=*_tmp263;_tmp265=_tmp264.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB14;struct Cyc_Toc_FallthruInfo*fi=(_tmpB14=
_region_malloc(r,sizeof(*_tmpB14)),((_tmpB14->label=fallthru_l,((_tmpB14->binders=
fallthru_vars,((_tmpB14->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp265),_tmpB14)))))));
struct _dyneither_ptr**_tmpB17;struct Cyc_Toc_Env*_tmpB16;return(_tmpB16=
_region_malloc(r,sizeof(*_tmpB16)),((_tmpB16->break_lab=(struct _dyneither_ptr**)((
_tmpB17=_region_malloc(r,sizeof(*_tmpB17)),((_tmpB17[0]=break_l,_tmpB17)))),((
_tmpB16->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25F),((
_tmpB16->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB16->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp261),((_tmpB16->toplevel=(int)_tmp262,((_tmpB16->rgn=(
struct _RegionHandle*)r,_tmpB16)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp26A;struct _dyneither_ptr**
_tmp26B;struct _dyneither_ptr**_tmp26C;struct Cyc_Toc_FallthruInfo*_tmp26D;struct
Cyc_Dict_Dict _tmp26E;int _tmp26F;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=*_tmp269;
_tmp26B=_tmp26A.break_lab;_tmp26C=_tmp26A.continue_lab;_tmp26D=_tmp26A.fallthru_info;
_tmp26E=_tmp26A.varmap;_tmp26F=_tmp26A.toplevel;{struct _dyneither_ptr**_tmpB1A;
struct Cyc_Toc_Env*_tmpB19;return(_tmpB19=_region_malloc(r,sizeof(*_tmpB19)),((
_tmpB19->break_lab=(struct _dyneither_ptr**)((_tmpB1A=_region_malloc(r,sizeof(*
_tmpB1A)),((_tmpB1A[0]=break_l,_tmpB1A)))),((_tmpB19->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB19->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB19->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26E),((_tmpB19->toplevel=(
int)_tmp26F,((_tmpB19->rgn=(struct _RegionHandle*)r,_tmpB19)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp273;struct _dyneither_ptr**_tmp274;struct _dyneither_ptr**_tmp275;struct Cyc_Toc_FallthruInfo*
_tmp276;struct Cyc_Dict_Dict _tmp277;int _tmp278;struct Cyc_Toc_Env*_tmp272=e;
_tmp273=*_tmp272;_tmp274=_tmp273.break_lab;_tmp275=_tmp273.continue_lab;_tmp276=
_tmp273.fallthru_info;_tmp277=_tmp273.varmap;_tmp278=_tmp273.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB1D;struct Cyc_Toc_Env*_tmpB1C;return(_tmpB1C=_region_malloc(r,sizeof(*
_tmpB1C)),((_tmpB1C->break_lab=(struct _dyneither_ptr**)0,((_tmpB1C->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp275),((_tmpB1C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB1D=_region_malloc(r,sizeof(*_tmpB1D)),((
_tmpB1D->label=next_l,((_tmpB1D->binders=0,((_tmpB1D->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB1D)))))))),((_tmpB1C->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp277),((_tmpB1C->toplevel=(int)_tmp278,((_tmpB1C->rgn=(struct _RegionHandle*)
r,_tmpB1C)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27B=e->annot;_LLEB: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27C=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27B;if(_tmp27C->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLED;}_LLEC: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLED: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27B;if(_tmp27D->tag != Cyc_CfFlowInfo_NotZero)
goto _LLEF;}_LLEE: return 0;_LLEF: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27E=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27B;if(_tmp27E->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF1;}_LLF0:{const char*_tmpB20;void*_tmpB1F;(_tmpB1F=0,Cyc_Tcutil_terr(e->loc,((
_tmpB20="dereference of NULL pointer",_tag_dyneither(_tmpB20,sizeof(char),28))),
_tag_dyneither(_tmpB1F,sizeof(void*),0)));}return 0;_LLF1: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27F=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27B;if(_tmp27F->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF3;}_LLF2: return 0;_LLF3:;_LLF4: {const char*_tmpB23;void*_tmpB22;(_tmpB22=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB23="need_null_check",_tag_dyneither(_tmpB23,sizeof(char),16))),
_tag_dyneither(_tmpB22,sizeof(void*),0)));}_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e);static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*
e){void*_tmp284=e->annot;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;
_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF8;else{_tmp286=_tmp285->f1;}}
_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp287=(struct
Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB26;void*_tmpB25;(_tmpB25=0,Cyc_Tcutil_terr(e->loc,((
_tmpB26="dereference of NULL pointer",_tag_dyneither(_tmpB26,sizeof(char),28))),
_tag_dyneither(_tmpB25,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp28A=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28A->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFE;}_LLFD: return 0;_LLFE:;_LLFF: {const char*_tmpB29;void*_tmpB28;(_tmpB28=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB29="get_relns",_tag_dyneither(_tmpB29,sizeof(char),10))),_tag_dyneither(
_tmpB28,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp291;struct Cyc_Absyn_PtrAtts
_tmp292;union Cyc_Absyn_Constraint*_tmp293;union Cyc_Absyn_Constraint*_tmp294;
struct Cyc_Absyn_ArrayInfo _tmp296;struct Cyc_Absyn_Exp*_tmp297;_LL101: {struct Cyc_Absyn_PointerType_struct*
_tmp290=(struct Cyc_Absyn_PointerType_struct*)_tmp28F;if(_tmp290->tag != 5)goto
_LL103;else{_tmp291=_tmp290->f1;_tmp292=_tmp291.ptr_atts;_tmp293=_tmp292.bounds;
_tmp294=_tmp292.zero_term;}}_LL102: {void*_tmp298=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp293);struct Cyc_Absyn_Exp*
_tmp29B;_LL108: {struct Cyc_Absyn_DynEither_b_struct*_tmp299=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp298;if(_tmp299->tag != 0)goto _LL10A;}_LL109: return 0;_LL10A: {struct Cyc_Absyn_Upper_b_struct*
_tmp29A=(struct Cyc_Absyn_Upper_b_struct*)_tmp298;if(_tmp29A->tag != 1)goto _LL107;
else{_tmp29B=_tmp29A->f1;}}_LL10B: {unsigned int _tmp29D;int _tmp29E;struct
_tuple10 _tmp29C=Cyc_Evexp_eval_const_uint_exp(_tmp29B);_tmp29D=_tmp29C.f1;
_tmp29E=_tmp29C.f2;return _tmp29E  && i <= _tmp29D;}_LL107:;}_LL103: {struct Cyc_Absyn_ArrayType_struct*
_tmp295=(struct Cyc_Absyn_ArrayType_struct*)_tmp28F;if(_tmp295->tag != 9)goto
_LL105;else{_tmp296=_tmp295->f1;_tmp297=_tmp296.num_elts;}}_LL104: if(_tmp297 == 0)
return 0;{unsigned int _tmp2A0;int _tmp2A1;struct _tuple10 _tmp29F=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp297);_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;return
_tmp2A1  && i <= _tmp2A0;};_LL105:;_LL106: return 0;_LL100:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A2=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A2->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A3=_tmp2A2->rop;struct Cyc_Absyn_Vardecl*_tmp2A4;struct
Cyc_Absyn_Vardecl*_tmp2A5;_LL10D: if((_tmp2A3.LessNumelts).tag != 3)goto _LL10F;
_tmp2A4=(struct Cyc_Absyn_Vardecl*)(_tmp2A3.LessNumelts).val;_LL10E: _tmp2A5=
_tmp2A4;goto _LL110;_LL10F: if((_tmp2A3.LessEqNumelts).tag != 5)goto _LL111;_tmp2A5=(
struct Cyc_Absyn_Vardecl*)(_tmp2A3.LessEqNumelts).val;_LL110: if(_tmp2A5 == v)
return 1;else{goto _LL10C;}_LL111:;_LL112: continue;_LL10C:;};}return 0;}static int
Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e,void*vtype);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2A6=
e->r;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2AA;void*_tmp2AC;struct Cyc_Absyn_Vardecl*
_tmp2AE;void*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B2;void*_tmp2B4;struct Cyc_Absyn_Vardecl*
_tmp2B6;union Cyc_Absyn_Cnst _tmp2B8;struct _tuple5 _tmp2B9;enum Cyc_Absyn_Sign
_tmp2BA;int _tmp2BB;union Cyc_Absyn_Cnst _tmp2BD;struct _tuple5 _tmp2BE;enum Cyc_Absyn_Sign
_tmp2BF;int _tmp2C0;union Cyc_Absyn_Cnst _tmp2C2;struct _tuple5 _tmp2C3;enum Cyc_Absyn_Sign
_tmp2C4;int _tmp2C5;enum Cyc_Absyn_Primop _tmp2C7;struct Cyc_List_List*_tmp2C8;
struct Cyc_List_List _tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;_LL114: {struct Cyc_Absyn_Var_e_struct*
_tmp2A7=(struct Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2A7->tag != 1)goto _LL116;
else{_tmp2A8=(void*)_tmp2A7->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2A9=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2A8;if(_tmp2A9->tag != 5)goto _LL116;else{_tmp2AA=_tmp2A9->f1;}};}}_LL115:
_tmp2AE=_tmp2AA;goto _LL117;_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2AB=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2AB->tag != 1)goto _LL118;else{_tmp2AC=(
void*)_tmp2AB->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2AD=(struct Cyc_Absyn_Local_b_struct*)
_tmp2AC;if(_tmp2AD->tag != 4)goto _LL118;else{_tmp2AE=_tmp2AD->f1;}};}}_LL117:
_tmp2B2=_tmp2AE;goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AF=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2AF->tag != 1)goto _LL11A;else{_tmp2B0=(
void*)_tmp2AF->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2B1=(struct Cyc_Absyn_Global_b_struct*)
_tmp2B0;if(_tmp2B1->tag != 1)goto _LL11A;else{_tmp2B2=_tmp2B1->f1;}};}}_LL119:
_tmp2B6=_tmp2B2;goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B3=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2B3->tag != 1)goto _LL11C;else{_tmp2B4=(
void*)_tmp2B3->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2B5=(struct Cyc_Absyn_Param_b_struct*)
_tmp2B4;if(_tmp2B5->tag != 3)goto _LL11C;else{_tmp2B6=_tmp2B5->f1;}};}}_LL11B: if(
_tmp2B6->escapes)return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B6))return 1;
goto _LL113;_LL11C: {struct Cyc_Absyn_Const_e_struct*_tmp2B7=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A6;if(_tmp2B7->tag != 0)goto _LL11E;else{_tmp2B8=_tmp2B7->f1;if((_tmp2B8.Int_c).tag
!= 4)goto _LL11E;_tmp2B9=(struct _tuple5)(_tmp2B8.Int_c).val;_tmp2BA=_tmp2B9.f1;
if(_tmp2BA != Cyc_Absyn_None)goto _LL11E;_tmp2BB=_tmp2B9.f2;}}_LL11D: _tmp2C0=
_tmp2BB;goto _LL11F;_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp2BC=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A6;if(_tmp2BC->tag != 0)goto _LL120;else{_tmp2BD=_tmp2BC->f1;if((_tmp2BD.Int_c).tag
!= 4)goto _LL120;_tmp2BE=(struct _tuple5)(_tmp2BD.Int_c).val;_tmp2BF=_tmp2BE.f1;
if(_tmp2BF != Cyc_Absyn_Signed)goto _LL120;_tmp2C0=_tmp2BE.f2;}}_LL11F: return
_tmp2C0 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2C0,vtype);_LL120: {
struct Cyc_Absyn_Const_e_struct*_tmp2C1=(struct Cyc_Absyn_Const_e_struct*)_tmp2A6;
if(_tmp2C1->tag != 0)goto _LL122;else{_tmp2C2=_tmp2C1->f1;if((_tmp2C2.Int_c).tag != 
4)goto _LL122;_tmp2C3=(struct _tuple5)(_tmp2C2.Int_c).val;_tmp2C4=_tmp2C3.f1;if(
_tmp2C4 != Cyc_Absyn_Unsigned)goto _LL122;_tmp2C5=_tmp2C3.f2;}}_LL121: return Cyc_Toc_check_const_array((
unsigned int)_tmp2C5,vtype);_LL122: {struct Cyc_Absyn_Primop_e_struct*_tmp2C6=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2A6;if(_tmp2C6->tag != 3)goto _LL124;else{
_tmp2C7=_tmp2C6->f1;if(_tmp2C7 != Cyc_Absyn_Numelts)goto _LL124;_tmp2C8=_tmp2C6->f2;
if(_tmp2C8 == 0)goto _LL124;_tmp2C9=*_tmp2C8;_tmp2CA=(struct Cyc_Absyn_Exp*)_tmp2C9.hd;}}
_LL123:{void*_tmp2CB=_tmp2CA->r;void*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2CF;
void*_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2D3;void*_tmp2D5;struct Cyc_Absyn_Vardecl*
_tmp2D7;void*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DB;_LL127: {struct Cyc_Absyn_Var_e_struct*
_tmp2CC=(struct Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2CC->tag != 1)goto _LL129;
else{_tmp2CD=(void*)_tmp2CC->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2CE=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2CD;if(_tmp2CE->tag != 5)goto _LL129;else{_tmp2CF=_tmp2CE->f1;}};}}_LL128:
_tmp2D3=_tmp2CF;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2D0=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D0->tag != 1)goto _LL12B;else{_tmp2D1=(
void*)_tmp2D0->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D2=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D1;if(_tmp2D2->tag != 4)goto _LL12B;else{_tmp2D3=_tmp2D2->f1;}};}}_LL12A:
_tmp2D7=_tmp2D3;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*_tmp2D4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D4->tag != 1)goto _LL12D;else{_tmp2D5=(
void*)_tmp2D4->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D6=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D5;if(_tmp2D6->tag != 1)goto _LL12D;else{_tmp2D7=_tmp2D6->f1;}};}}_LL12C:
_tmp2DB=_tmp2D7;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_struct*_tmp2D8=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D8->tag != 1)goto _LL12F;else{_tmp2D9=(
void*)_tmp2D8->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DA=(struct Cyc_Absyn_Param_b_struct*)
_tmp2D9;if(_tmp2DA->tag != 3)goto _LL12F;else{_tmp2DB=_tmp2DA->f1;}};}}_LL12E:
return _tmp2DB == v;_LL12F:;_LL130: goto _LL126;_LL126:;}goto _LL113;_LL124:;_LL125:
goto _LL113;_LL113:;}return 0;}static char _tmp2ED[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(void*
a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
struct Cyc_Absyn_Vardecl*x;{void*_tmp2DC=a->r;void*_tmp2DE;struct Cyc_Absyn_Vardecl*
_tmp2E0;void*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E4;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E8;void*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EC;_LL132: {struct Cyc_Absyn_Var_e_struct*
_tmp2DD=(struct Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2DD->tag != 1)goto _LL134;
else{_tmp2DE=(void*)_tmp2DD->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2DF=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2DE;if(_tmp2DF->tag != 5)goto _LL134;else{_tmp2E0=_tmp2DF->f1;}};}}_LL133:
_tmp2E4=_tmp2E0;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2E1=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E1->tag != 1)goto _LL136;else{_tmp2E2=(
void*)_tmp2E1->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2E3=(struct Cyc_Absyn_Local_b_struct*)
_tmp2E2;if(_tmp2E3->tag != 4)goto _LL136;else{_tmp2E4=_tmp2E3->f1;}};}}_LL135:
_tmp2E8=_tmp2E4;goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_struct*_tmp2E5=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E5->tag != 1)goto _LL138;else{_tmp2E6=(
void*)_tmp2E5->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2E7=(struct Cyc_Absyn_Global_b_struct*)
_tmp2E6;if(_tmp2E7->tag != 1)goto _LL138;else{_tmp2E8=_tmp2E7->f1;}};}}_LL137:
_tmp2EC=_tmp2E8;goto _LL139;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp2E9=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E9->tag != 1)goto _LL13A;else{_tmp2EA=(
void*)_tmp2E9->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2EB=(struct Cyc_Absyn_Param_b_struct*)
_tmp2EA;if(_tmp2EB->tag != 3)goto _LL13A;else{_tmp2EC=_tmp2EB->f1;}};}}_LL139: if(
_tmp2EC->escapes)goto _LL13B;x=_tmp2EC;goto _LL131;_LL13A:;_LL13B: {static struct
_dyneither_ptr bogus_string={_tmp2ED,_tmp2ED,_tmp2ED + 8};static struct _tuple0
bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL131:;}{void*_tmp2EE=a_typ;inner_loop: {
void*_tmp2EF=i->r;void*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;union Cyc_Absyn_Cnst
_tmp2F4;struct _tuple5 _tmp2F5;enum Cyc_Absyn_Sign _tmp2F6;int _tmp2F7;union Cyc_Absyn_Cnst
_tmp2F9;struct _tuple5 _tmp2FA;enum Cyc_Absyn_Sign _tmp2FB;int _tmp2FC;union Cyc_Absyn_Cnst
_tmp2FE;struct _tuple5 _tmp2FF;enum Cyc_Absyn_Sign _tmp300;int _tmp301;enum Cyc_Absyn_Primop
_tmp303;struct Cyc_List_List*_tmp304;struct Cyc_List_List _tmp305;struct Cyc_Absyn_Exp*
_tmp306;struct Cyc_List_List*_tmp307;struct Cyc_List_List _tmp308;struct Cyc_Absyn_Exp*
_tmp309;void*_tmp30B;struct Cyc_Absyn_Vardecl*_tmp30D;void*_tmp30F;struct Cyc_Absyn_Vardecl*
_tmp311;void*_tmp313;struct Cyc_Absyn_Vardecl*_tmp315;void*_tmp317;struct Cyc_Absyn_Vardecl*
_tmp319;_LL13D: {struct Cyc_Absyn_Cast_e_struct*_tmp2F0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2EF;if(_tmp2F0->tag != 15)goto _LL13F;else{_tmp2F1=(void*)_tmp2F0->f1;_tmp2F2=
_tmp2F0->f2;}}_LL13E: i=_tmp2F2;goto inner_loop;_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2F3=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2F3->tag != 0)goto _LL141;
else{_tmp2F4=_tmp2F3->f1;if((_tmp2F4.Int_c).tag != 4)goto _LL141;_tmp2F5=(struct
_tuple5)(_tmp2F4.Int_c).val;_tmp2F6=_tmp2F5.f1;if(_tmp2F6 != Cyc_Absyn_None)goto
_LL141;_tmp2F7=_tmp2F5.f2;}}_LL140: _tmp2FC=_tmp2F7;goto _LL142;_LL141: {struct Cyc_Absyn_Const_e_struct*
_tmp2F8=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2F8->tag != 0)goto _LL143;
else{_tmp2F9=_tmp2F8->f1;if((_tmp2F9.Int_c).tag != 4)goto _LL143;_tmp2FA=(struct
_tuple5)(_tmp2F9.Int_c).val;_tmp2FB=_tmp2FA.f1;if(_tmp2FB != Cyc_Absyn_Signed)
goto _LL143;_tmp2FC=_tmp2FA.f2;}}_LL142: return _tmp2FC >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2FC + 1),_tmp2EE);_LL143: {struct Cyc_Absyn_Const_e_struct*
_tmp2FD=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2FD->tag != 0)goto _LL145;
else{_tmp2FE=_tmp2FD->f1;if((_tmp2FE.Int_c).tag != 4)goto _LL145;_tmp2FF=(struct
_tuple5)(_tmp2FE.Int_c).val;_tmp300=_tmp2FF.f1;if(_tmp300 != Cyc_Absyn_Unsigned)
goto _LL145;_tmp301=_tmp2FF.f2;}}_LL144: return Cyc_Toc_check_const_array((
unsigned int)(_tmp301 + 1),_tmp2EE);_LL145: {struct Cyc_Absyn_Primop_e_struct*
_tmp302=(struct Cyc_Absyn_Primop_e_struct*)_tmp2EF;if(_tmp302->tag != 3)goto _LL147;
else{_tmp303=_tmp302->f1;if(_tmp303 != Cyc_Absyn_Mod)goto _LL147;_tmp304=_tmp302->f2;
if(_tmp304 == 0)goto _LL147;_tmp305=*_tmp304;_tmp306=(struct Cyc_Absyn_Exp*)_tmp305.hd;
_tmp307=_tmp305.tl;if(_tmp307 == 0)goto _LL147;_tmp308=*_tmp307;_tmp309=(struct Cyc_Absyn_Exp*)
_tmp308.hd;}}_LL146: return Cyc_Toc_check_leq_size(relns,x,_tmp309,_tmp2EE);_LL147: {
struct Cyc_Absyn_Var_e_struct*_tmp30A=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(
_tmp30A->tag != 1)goto _LL149;else{_tmp30B=(void*)_tmp30A->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp30C=(struct Cyc_Absyn_Pat_b_struct*)_tmp30B;if(_tmp30C->tag != 5)goto _LL149;
else{_tmp30D=_tmp30C->f1;}};}}_LL148: _tmp311=_tmp30D;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp30E=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp30E->tag
!= 1)goto _LL14B;else{_tmp30F=(void*)_tmp30E->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp310=(struct Cyc_Absyn_Local_b_struct*)_tmp30F;if(_tmp310->tag != 4)goto _LL14B;
else{_tmp311=_tmp310->f1;}};}}_LL14A: _tmp315=_tmp311;goto _LL14C;_LL14B: {struct
Cyc_Absyn_Var_e_struct*_tmp312=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp312->tag
!= 1)goto _LL14D;else{_tmp313=(void*)_tmp312->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp314=(struct Cyc_Absyn_Global_b_struct*)_tmp313;if(_tmp314->tag != 1)goto _LL14D;
else{_tmp315=_tmp314->f1;}};}}_LL14C: _tmp319=_tmp315;goto _LL14E;_LL14D: {struct
Cyc_Absyn_Var_e_struct*_tmp316=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp316->tag
!= 1)goto _LL14F;else{_tmp317=(void*)_tmp316->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp318=(struct Cyc_Absyn_Param_b_struct*)_tmp317;if(_tmp318->tag != 3)goto _LL14F;
else{_tmp319=_tmp318->f1;}};}}_LL14E: if(_tmp319->escapes)return 0;{struct Cyc_List_List*
_tmp31A=relns;for(0;_tmp31A != 0;_tmp31A=_tmp31A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp31B=(struct Cyc_CfFlowInfo_Reln*)_tmp31A->hd;if(_tmp31B->vd == _tmp319){union
Cyc_CfFlowInfo_RelnOp _tmp31C=_tmp31B->rop;struct Cyc_Absyn_Vardecl*_tmp31D;struct
_tuple11 _tmp31E;struct Cyc_Absyn_Vardecl*_tmp31F;void*_tmp320;unsigned int _tmp321;
unsigned int _tmp322;_LL152: if((_tmp31C.LessNumelts).tag != 3)goto _LL154;_tmp31D=(
struct Cyc_Absyn_Vardecl*)(_tmp31C.LessNumelts).val;_LL153: if(x == _tmp31D)return 1;
else{goto _LL151;}_LL154: if((_tmp31C.LessVar).tag != 2)goto _LL156;_tmp31E=(struct
_tuple11)(_tmp31C.LessVar).val;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;_LL155:{
struct _tuple17 _tmpB2A;struct _tuple17 _tmp324=(_tmpB2A.f1=Cyc_Tcutil_compress(
_tmp320),((_tmpB2A.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB2A)));void*_tmp325;void*_tmp327;void*_tmp328;
struct Cyc_Absyn_PtrInfo _tmp32A;struct Cyc_Absyn_PtrAtts _tmp32B;union Cyc_Absyn_Constraint*
_tmp32C;_LL15D: _tmp325=_tmp324.f1;{struct Cyc_Absyn_TagType_struct*_tmp326=(
struct Cyc_Absyn_TagType_struct*)_tmp325;if(_tmp326->tag != 20)goto _LL15F;else{
_tmp327=(void*)_tmp326->f1;}};_tmp328=_tmp324.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp329=(struct Cyc_Absyn_PointerType_struct*)_tmp328;if(_tmp329->tag != 5)goto
_LL15F;else{_tmp32A=_tmp329->f1;_tmp32B=_tmp32A.ptr_atts;_tmp32C=_tmp32B.bounds;}};
_LL15E:{void*_tmp32D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp32C);struct Cyc_Absyn_Exp*_tmp32F;_LL162: {struct Cyc_Absyn_Upper_b_struct*
_tmp32E=(struct Cyc_Absyn_Upper_b_struct*)_tmp32D;if(_tmp32E->tag != 1)goto _LL164;
else{_tmp32F=_tmp32E->f1;}}_LL163: {struct Cyc_Absyn_Exp*_tmp330=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp327,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp330,_tmp32F))return 1;goto _LL161;}_LL164:;_LL165:
goto _LL161;_LL161:;}goto _LL15C;_LL15F:;_LL160: goto _LL15C;_LL15C:;}{struct Cyc_List_List*
_tmp331=relns;for(0;_tmp331 != 0;_tmp331=_tmp331->tl){struct Cyc_CfFlowInfo_Reln*
_tmp332=(struct Cyc_CfFlowInfo_Reln*)_tmp331->hd;if(_tmp332->vd == _tmp31F){union
Cyc_CfFlowInfo_RelnOp _tmp333=_tmp332->rop;struct Cyc_Absyn_Vardecl*_tmp334;struct
Cyc_Absyn_Vardecl*_tmp335;unsigned int _tmp336;unsigned int _tmp337;struct _tuple11
_tmp338;struct Cyc_Absyn_Vardecl*_tmp339;_LL167: if((_tmp333.LessEqNumelts).tag != 
5)goto _LL169;_tmp334=(struct Cyc_Absyn_Vardecl*)(_tmp333.LessEqNumelts).val;
_LL168: _tmp335=_tmp334;goto _LL16A;_LL169: if((_tmp333.LessNumelts).tag != 3)goto
_LL16B;_tmp335=(struct Cyc_Absyn_Vardecl*)(_tmp333.LessNumelts).val;_LL16A: if(x == 
_tmp335)return 1;goto _LL166;_LL16B: if((_tmp333.EqualConst).tag != 1)goto _LL16D;
_tmp336=(unsigned int)(_tmp333.EqualConst).val;_LL16C: return Cyc_Toc_check_const_array(
_tmp336,_tmp2EE);_LL16D: if((_tmp333.LessEqConst).tag != 6)goto _LL16F;_tmp337=(
unsigned int)(_tmp333.LessEqConst).val;if(!(_tmp337 > 0))goto _LL16F;_LL16E: return
Cyc_Toc_check_const_array(_tmp337,_tmp2EE);_LL16F: if((_tmp333.LessVar).tag != 2)
goto _LL171;_tmp338=(struct _tuple11)(_tmp333.LessVar).val;_tmp339=_tmp338.f1;
_LL170: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp339))return 1;goto _LL166;_LL171:;
_LL172: goto _LL166;_LL166:;}}}goto _LL151;_LL156: if((_tmp31C.LessConst).tag != 4)
goto _LL158;_tmp321=(unsigned int)(_tmp31C.LessConst).val;_LL157: return Cyc_Toc_check_const_array(
_tmp321,_tmp2EE);_LL158: if((_tmp31C.LessEqConst).tag != 6)goto _LL15A;_tmp322=(
unsigned int)(_tmp31C.LessEqConst).val;_LL159: return Cyc_Toc_check_const_array(
_tmp322 + 1,_tmp2EE);_LL15A:;_LL15B: goto _LL151;_LL151:;}}}goto _LL13C;_LL14F:;
_LL150: goto _LL13C;_LL13C:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB2D;void*_tmpB2C;(_tmpB2C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="Missing type in primop ",
_tag_dyneither(_tmpB2D,sizeof(char),24))),_tag_dyneither(_tmpB2C,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB30;void*_tmpB2F;(_tmpB2F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB30="Missing type in primop ",_tag_dyneither(_tmpB30,sizeof(char),24))),
_tag_dyneither(_tmpB2F,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*
_tmpB31;return(_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->f1=Cyc_Toc_mt_tq,((
_tmpB31->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB31)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB32;return(
_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->f1=0,((_tmpB32->f2=e,_tmpB32)))));};}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*
x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,
int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp340=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp340): Cyc_Toc_malloc_ptr(
_tmp340));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp340);}};}else{t=
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
dles0)- 1;{struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(dles0);for(0;_tmp341 != 0;_tmp341=_tmp341->tl){struct _tuple15
_tmp343;struct Cyc_List_List*_tmp344;struct Cyc_Absyn_Exp*_tmp345;struct _tuple15*
_tmp342=(struct _tuple15*)_tmp341->hd;_tmp343=*_tmp342;_tmp344=_tmp343.f1;_tmp345=
_tmp343.f2;{struct Cyc_Absyn_Exp*e_index;if(_tmp344 == 0)e_index=Cyc_Absyn_signed_int_exp(
count --,0);else{if(_tmp344->tl != 0){const char*_tmpB35;void*_tmpB34;(_tmpB34=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB35="multiple designators in array",
_tag_dyneither(_tmpB35,sizeof(char),30))),_tag_dyneither(_tmpB34,sizeof(void*),0)));}{
void*_tmp348=(void*)_tmp344->hd;void*_tmp349=_tmp348;struct Cyc_Absyn_Exp*_tmp34B;
_LL174: {struct Cyc_Absyn_ArrayElement_struct*_tmp34A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp349;if(_tmp34A->tag != 0)goto _LL176;else{_tmp34B=_tmp34A->f1;}}_LL175: Cyc_Toc_exp_to_c(
nv,_tmp34B);e_index=_tmp34B;goto _LL173;_LL176: {struct Cyc_Absyn_FieldName_struct*
_tmp34C=(struct Cyc_Absyn_FieldName_struct*)_tmp349;if(_tmp34C->tag != 1)goto
_LL173;}_LL177: {const char*_tmpB38;void*_tmpB37;(_tmpB37=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB38="field name designators in array",
_tag_dyneither(_tmpB38,sizeof(char),32))),_tag_dyneither(_tmpB37,sizeof(void*),0)));}
_LL173:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp34F=_tmp345->r;struct Cyc_List_List*_tmp351;struct Cyc_Absyn_Vardecl*_tmp353;
struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp355;int _tmp356;void*_tmp358;
struct Cyc_List_List*_tmp359;_LL179: {struct Cyc_Absyn_Array_e_struct*_tmp350=(
struct Cyc_Absyn_Array_e_struct*)_tmp34F;if(_tmp350->tag != 27)goto _LL17B;else{
_tmp351=_tmp350->f1;}}_LL17A: s=Cyc_Toc_init_array(nv,lval,_tmp351,s);goto _LL178;
_LL17B: {struct Cyc_Absyn_Comprehension_e_struct*_tmp352=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34F;if(_tmp352->tag != 28)goto _LL17D;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;
_tmp355=_tmp352->f3;_tmp356=_tmp352->f4;}}_LL17C: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp353,_tmp354,_tmp355,_tmp356,s,0);goto _LL178;_LL17D: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp357=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp34F;if(_tmp357->tag != 30)goto
_LL17F;else{_tmp358=(void*)_tmp357->f1;_tmp359=_tmp357->f2;}}_LL17E: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp358,_tmp359,s);goto _LL178;_LL17F:;_LL180: Cyc_Toc_exp_to_c(nv,_tmp345);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp345,0),s,0);goto _LL178;_LL178:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp35A=vd->name;void*_tmp35B=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp35D;struct _RegionHandle*_tmp35E;
struct Cyc_Toc_Env*_tmp35C=nv;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB3B;struct Cyc_Absyn_Local_b_struct*_tmpB3A;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp35E,nv,_tmp35A,Cyc_Absyn_varb_exp(_tmp35A,(void*)((_tmpB3A=_cycalloc(sizeof(*
_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=4,((_tmpB3B.f1=vd,_tmpB3B)))),_tmpB3A)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp35A,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp35A,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp35A,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp35A,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp35F=e2->r;struct Cyc_List_List*_tmp361;
struct Cyc_Absyn_Vardecl*_tmp363;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*
_tmp365;int _tmp366;void*_tmp368;struct Cyc_List_List*_tmp369;_LL182: {struct Cyc_Absyn_Array_e_struct*
_tmp360=(struct Cyc_Absyn_Array_e_struct*)_tmp35F;if(_tmp360->tag != 27)goto _LL184;
else{_tmp361=_tmp360->f1;}}_LL183: body=Cyc_Toc_init_array(nv2,lval,_tmp361,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL184: {struct Cyc_Absyn_Comprehension_e_struct*_tmp362=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp35F;if(_tmp362->tag != 28)goto _LL186;else{_tmp363=_tmp362->f1;_tmp364=_tmp362->f2;
_tmp365=_tmp362->f3;_tmp366=_tmp362->f4;}}_LL185: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp363,_tmp364,_tmp365,_tmp366,Cyc_Toc_skip_stmt_dl(),0);goto _LL181;
_LL186: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp367=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp35F;if(_tmp367->tag != 30)goto _LL188;else{_tmp368=(void*)_tmp367->f1;_tmp369=
_tmp367->f2;}}_LL187: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp368,_tmp369,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL188:;_LL189: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL181;_LL181:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp35B,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp35A,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){struct _tuple15 _tmp36E;
struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct _tuple15*_tmp36D=(
struct _tuple15*)_tmp36C->hd;_tmp36E=*_tmp36D;_tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;
if(_tmp36F == 0){const char*_tmpB3E;void*_tmpB3D;(_tmpB3D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3E="empty designator list",
_tag_dyneither(_tmpB3E,sizeof(char),22))),_tag_dyneither(_tmpB3D,sizeof(void*),0)));}
if(_tmp36F->tl != 0){const char*_tmpB41;void*_tmpB40;(_tmpB40=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB41="too many designators in anonymous struct",
_tag_dyneither(_tmpB41,sizeof(char),41))),_tag_dyneither(_tmpB40,sizeof(void*),0)));}{
void*_tmp375=(void*)_tmp36F->hd;struct _dyneither_ptr*_tmp377;_LL18B: {struct Cyc_Absyn_FieldName_struct*
_tmp376=(struct Cyc_Absyn_FieldName_struct*)_tmp375;if(_tmp376->tag != 1)goto
_LL18D;else{_tmp377=_tmp376->f1;}}_LL18C: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp377,0);{void*_tmp378=_tmp370->r;struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Vardecl*
_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;int _tmp37F;void*
_tmp381;struct Cyc_List_List*_tmp382;_LL190: {struct Cyc_Absyn_Array_e_struct*
_tmp379=(struct Cyc_Absyn_Array_e_struct*)_tmp378;if(_tmp379->tag != 27)goto _LL192;
else{_tmp37A=_tmp379->f1;}}_LL191: s=Cyc_Toc_init_array(nv,lval,_tmp37A,s);goto
_LL18F;_LL192: {struct Cyc_Absyn_Comprehension_e_struct*_tmp37B=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp378;if(_tmp37B->tag != 28)goto _LL194;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37B->f2;
_tmp37E=_tmp37B->f3;_tmp37F=_tmp37B->f4;}}_LL193: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp37C,_tmp37D,_tmp37E,_tmp37F,s,0);goto _LL18F;_LL194: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp380=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp378;if(_tmp380->tag != 30)goto
_LL196;else{_tmp381=(void*)_tmp380->f1;_tmp382=_tmp380->f2;}}_LL195: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp381,_tmp382,s);goto _LL18F;_LL196:;_LL197: Cyc_Toc_exp_to_c(nv,_tmp370);
if(Cyc_Toc_is_poly_field(struct_type,_tmp377))_tmp370=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp370);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp370,0),0),s,0);goto _LL18F;_LL18F:;}goto _LL18A;}_LL18D:;_LL18E: {const char*
_tmpB44;void*_tmpB43;(_tmpB43=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB44="array designator in struct",
_tag_dyneither(_tmpB44,sizeof(char),27))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}
_LL18A:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp386;struct _RegionHandle*_tmp387;
struct Cyc_Toc_Env*_tmp385=nv;_tmp386=*_tmp385;_tmp387=_tmp386.rgn;{struct Cyc_List_List*
_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple9*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp387,Cyc_Toc_tup_to_c,
es);void*_tmp389=Cyc_Toc_add_tuple_type(_tmp388);struct _tuple0*_tmp38A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38B=Cyc_Absyn_var_exp(_tmp38A,0);struct Cyc_Absyn_Stmt*
_tmp38C=Cyc_Absyn_exp_stmt(_tmp38B,0);struct Cyc_Absyn_Exp*(*_tmp38D)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp387,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38E);for(0;_tmp38E != 0;(
_tmp38E=_tmp38E->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp38E->hd;
struct Cyc_Absyn_Exp*lval=_tmp38D(_tmp38B,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp38F=e->r;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Vardecl*_tmp393;
struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;int _tmp396;_LL199: {
struct Cyc_Absyn_Array_e_struct*_tmp390=(struct Cyc_Absyn_Array_e_struct*)_tmp38F;
if(_tmp390->tag != 27)goto _LL19B;else{_tmp391=_tmp390->f1;}}_LL19A: _tmp38C=Cyc_Toc_init_array(
nv,lval,_tmp391,_tmp38C);goto _LL198;_LL19B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp392=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp38F;if(_tmp392->tag != 28)
goto _LL19D;else{_tmp393=_tmp392->f1;_tmp394=_tmp392->f2;_tmp395=_tmp392->f3;
_tmp396=_tmp392->f4;}}_LL19C: _tmp38C=Cyc_Toc_init_comprehension(nv,lval,_tmp393,
_tmp394,_tmp395,_tmp396,_tmp38C,0);goto _LL198;_LL19D:;_LL19E: Cyc_Toc_exp_to_c(nv,
e);_tmp38C=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38D(
_tmp38B,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp38C,0);goto _LL198;_LL198:;};}}
return Cyc_Toc_make_struct(nv,_tmp38A,_tmp389,_tmp38C,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp397=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp397 != 0;_tmp397=_tmp397->tl){struct Cyc_Absyn_Aggrfield*
_tmp398=(struct Cyc_Absyn_Aggrfield*)_tmp397->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp398->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB4C;void*_tmpB4B[1];const char*_tmpB4A;void*_tmpB49;(
_tmpB49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB4C.tag=0,((_tmpB4C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB4B[0]=& _tmpB4C,Cyc_aprintf(((_tmpB4A="get_member_offset %s failed",
_tag_dyneither(_tmpB4A,sizeof(char),28))),_tag_dyneither(_tmpB4B,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB49,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0);struct Cyc_Absyn_Stmt*
_tmp39F=Cyc_Absyn_exp_stmt(_tmp39E,0);struct Cyc_Absyn_Exp*(*_tmp3A0)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp3A1=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3A2=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3A4;union Cyc_Absyn_AggrInfoU _tmp3A5;
_LL1A0: {struct Cyc_Absyn_AggrType_struct*_tmp3A3=(struct Cyc_Absyn_AggrType_struct*)
_tmp3A2;if(_tmp3A3->tag != 12)goto _LL1A2;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A4.aggr_info;}}
_LL1A1: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3A5);goto _LL19F;_LL1A2:;_LL1A3: {
const char*_tmpB4F;void*_tmpB4E;(_tmpB4E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4F="init_struct: bad struct type",
_tag_dyneither(_tmpB4F,sizeof(char),29))),_tag_dyneither(_tmpB4E,sizeof(void*),0)));}
_LL19F:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3A9;struct _RegionHandle*_tmp3AA;struct Cyc_Toc_Env*_tmp3A8=
nv;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A9.rgn;{struct Cyc_List_List*_tmp3AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AA,dles);for(0;
_tmp3AB != 0;_tmp3AB=_tmp3AB->tl){struct _tuple15 _tmp3AD;struct Cyc_List_List*
_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct _tuple15*_tmp3AC=(struct _tuple15*)
_tmp3AB->hd;_tmp3AD=*_tmp3AC;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v);
if(_tmp3AE == 0){const char*_tmpB52;void*_tmpB51;(_tmpB51=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="empty designator list",
_tag_dyneither(_tmpB52,sizeof(char),22))),_tag_dyneither(_tmpB51,sizeof(void*),0)));}
if(_tmp3AE->tl != 0){struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B3=Cyc_Absyn_var_exp(_tmp3B2,0);for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){void*
_tmp3B4=(void*)_tmp3AE->hd;struct _dyneither_ptr*_tmp3B6;_LL1A5: {struct Cyc_Absyn_FieldName_struct*
_tmp3B5=(struct Cyc_Absyn_FieldName_struct*)_tmp3B4;if(_tmp3B5->tag != 1)goto
_LL1A7;else{_tmp3B6=_tmp3B5->f1;}}_LL1A6: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B6))_tmp3B3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B3);_tmp39F=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A0(_tmp39E,_tmp3B6,0),_tmp3B3,0),0),
_tmp39F,0);goto _LL1A4;_LL1A7:;_LL1A8: {const char*_tmpB55;void*_tmpB54;(_tmpB54=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB55="array designator in struct",_tag_dyneither(_tmpB55,sizeof(char),27))),
_tag_dyneither(_tmpB54,sizeof(void*),0)));}_LL1A4:;}Cyc_Toc_exp_to_c(nv,_tmp3AF);
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B3,
_tmp3AF,0),0),_tmp39F,0);}else{void*_tmp3B9=(void*)_tmp3AE->hd;struct
_dyneither_ptr*_tmp3BB;_LL1AA: {struct Cyc_Absyn_FieldName_struct*_tmp3BA=(struct
Cyc_Absyn_FieldName_struct*)_tmp3B9;if(_tmp3BA->tag != 1)goto _LL1AC;else{_tmp3BB=
_tmp3BA->f1;}}_LL1AB: {struct Cyc_Absyn_Exp*lval=_tmp3A0(_tmp39E,_tmp3BB,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3BB);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp39F,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3BC=_tmp3AF->r;
struct Cyc_List_List*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;struct Cyc_Absyn_Exp*
_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;int _tmp3C3;void*_tmp3C5;struct Cyc_List_List*
_tmp3C6;_LL1AF: {struct Cyc_Absyn_Array_e_struct*_tmp3BD=(struct Cyc_Absyn_Array_e_struct*)
_tmp3BC;if(_tmp3BD->tag != 27)goto _LL1B1;else{_tmp3BE=_tmp3BD->f1;}}_LL1B0:
_tmp39F=Cyc_Toc_init_array(nv,lval,_tmp3BE,_tmp39F);goto _LL1AE;_LL1B1: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3BF=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BC;if(_tmp3BF->tag != 28)goto _LL1B3;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;
_tmp3C2=_tmp3BF->f3;_tmp3C3=_tmp3BF->f4;}}_LL1B2: _tmp39F=Cyc_Toc_init_comprehension(
nv,lval,_tmp3C0,_tmp3C1,_tmp3C2,_tmp3C3,_tmp39F,0);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3C4=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3BC;if(_tmp3C4->tag != 30)goto
_LL1B5;else{_tmp3C5=(void*)_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL1B4: _tmp39F=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3C5,_tmp3C6,_tmp39F);goto _LL1AE;_LL1B5:;_LL1B6: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3AF);{struct Cyc_Absyn_Aggrfield*_tmp3C7=Cyc_Absyn_lookup_decl_field(ad,
_tmp3BB);if(Cyc_Toc_is_poly_field(struct_type,_tmp3BB) && !was_ptr_type)_tmp3AF=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3AF);if(has_exists)_tmp3AF=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C7))->type),
_tmp3AF);_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3AF,0),0),_tmp39F,0);goto _LL1AE;};}_LL1AE:;}goto _LL1A9;}_LL1AC:;_LL1AD: {
const char*_tmpB58;void*_tmpB57;(_tmpB57=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="array designator in struct",
_tag_dyneither(_tmpB58,sizeof(char),27))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}
_LL1A9:;}}}return Cyc_Toc_make_struct(nv,_tmp39D,_tmp3A1,_tmp39F,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB5B;struct Cyc_Absyn_Increment_e_struct*
_tmpB5A;return Cyc_Absyn_new_exp((void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((
_tmpB5A[0]=((_tmpB5B.tag=5,((_tmpB5B.f1=e1,((_tmpB5B.f2=incr,_tmpB5B)))))),
_tmpB5A)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3CC=e1->r;struct Cyc_Absyn_Stmt*
_tmp3CE;void*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D3;
struct _dyneither_ptr*_tmp3D4;int _tmp3D5;int _tmp3D6;_LL1B8: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3CD=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3CC;if(_tmp3CD->tag != 37)goto
_LL1BA;else{_tmp3CE=_tmp3CD->f1;}}_LL1B9: Cyc_Toc_lvalue_assign_stmt(_tmp3CE,fs,f,
f_env);goto _LL1B7;_LL1BA: {struct Cyc_Absyn_Cast_e_struct*_tmp3CF=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3CC;if(_tmp3CF->tag != 15)goto _LL1BC;else{_tmp3D0=(void*)_tmp3CF->f1;_tmp3D1=
_tmp3CF->f2;}}_LL1BB: Cyc_Toc_lvalue_assign(_tmp3D1,fs,f,f_env);goto _LL1B7;_LL1BC: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3D2=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3CC;if(_tmp3D2->tag != 22)goto _LL1BE;else{_tmp3D3=_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;
_tmp3D5=_tmp3D2->f3;_tmp3D6=_tmp3D2->f4;}}_LL1BD: e1->r=_tmp3D3->r;{struct Cyc_List_List*
_tmpB5C;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB5C=_cycalloc(
sizeof(*_tmpB5C)),((_tmpB5C->hd=_tmp3D4,((_tmpB5C->tl=fs,_tmpB5C)))))),f,f_env);}
goto _LL1B7;_LL1BE:;_LL1BF: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1B7;}
_LL1B7:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3D8=s->r;
struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Decl*_tmp3DC;struct Cyc_Absyn_Stmt*
_tmp3DD;struct Cyc_Absyn_Stmt*_tmp3DF;_LL1C1: {struct Cyc_Absyn_Exp_s_struct*
_tmp3D9=(struct Cyc_Absyn_Exp_s_struct*)_tmp3D8;if(_tmp3D9->tag != 1)goto _LL1C3;
else{_tmp3DA=_tmp3D9->f1;}}_LL1C2: Cyc_Toc_lvalue_assign(_tmp3DA,fs,f,f_env);goto
_LL1C0;_LL1C3: {struct Cyc_Absyn_Decl_s_struct*_tmp3DB=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3D8;if(_tmp3DB->tag != 12)goto _LL1C5;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;}}
_LL1C4: Cyc_Toc_lvalue_assign_stmt(_tmp3DD,fs,f,f_env);goto _LL1C0;_LL1C5: {struct
Cyc_Absyn_Seq_s_struct*_tmp3DE=(struct Cyc_Absyn_Seq_s_struct*)_tmp3D8;if(_tmp3DE->tag
!= 2)goto _LL1C7;else{_tmp3DF=_tmp3DE->f2;}}_LL1C6: Cyc_Toc_lvalue_assign_stmt(
_tmp3DF,fs,f,f_env);goto _LL1C0;_LL1C7:;_LL1C8: {const char*_tmpB60;void*_tmpB5F[1];
struct Cyc_String_pa_struct _tmpB5E;(_tmpB5E.tag=0,((_tmpB5E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB5F[0]=&
_tmpB5E,Cyc_Toc_toc_impos(((_tmpB60="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB60,sizeof(char),23))),_tag_dyneither(_tmpB5F,sizeof(void*),1)))))));}_LL1C0:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3E3=
e->r;void*_tmp3E5;void**_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Exp**
_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;_LL1CA: {struct Cyc_Absyn_Cast_e_struct*
_tmp3E4=(struct Cyc_Absyn_Cast_e_struct*)_tmp3E3;if(_tmp3E4->tag != 15)goto _LL1CC;
else{_tmp3E5=(void**)& _tmp3E4->f1;_tmp3E6=(void**)((void**)& _tmp3E4->f1);_tmp3E7=
_tmp3E4->f2;_tmp3E8=(struct Cyc_Absyn_Exp**)& _tmp3E4->f2;}}_LL1CB:*_tmp3E8=Cyc_Toc_push_address_exp(*
_tmp3E8);*_tmp3E6=Cyc_Absyn_cstar_typ(*_tmp3E6,Cyc_Toc_mt_tq);return e;_LL1CC: {
struct Cyc_Absyn_Deref_e_struct*_tmp3E9=(struct Cyc_Absyn_Deref_e_struct*)_tmp3E3;
if(_tmp3E9->tag != 21)goto _LL1CE;else{_tmp3EA=_tmp3E9->f1;}}_LL1CD: return _tmp3EA;
_LL1CE:;_LL1CF: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB64;void*_tmpB63[1];struct Cyc_String_pa_struct _tmpB62;(_tmpB62.tag=0,((
_tmpB62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB63[0]=& _tmpB62,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB64="can't take & of %s",_tag_dyneither(_tmpB64,sizeof(char),19))),
_tag_dyneither(_tmpB63,sizeof(void*),1)))))));};_LL1C9:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB65;result=((
_tmpB65=_region_malloc(r2,sizeof(*_tmpB65)),((_tmpB65->hd=(void*)f((void*)x->hd,
env),((_tmpB65->tl=0,_tmpB65))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB66;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB66=_region_malloc(r2,sizeof(*_tmpB66)),((_tmpB66->hd=(void*)f((void*)x->hd,
env),((_tmpB66->tl=0,_tmpB66))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);static
struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpB67;return(_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->f1=
0,((_tmpB67->f2=e,_tmpB67)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t);static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3F1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3F3;_LL1D1: {struct Cyc_Absyn_PointerType_struct*
_tmp3F2=(struct Cyc_Absyn_PointerType_struct*)_tmp3F1;if(_tmp3F2->tag != 5)goto
_LL1D3;else{_tmp3F3=_tmp3F2->f1;}}_LL1D2: return _tmp3F3;_LL1D3:;_LL1D4: {const
char*_tmpB6A;void*_tmpB69;(_tmpB69=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6A="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB6A,sizeof(char),28))),_tag_dyneither(_tmpB69,sizeof(void*),0)));}
_LL1D0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3F6=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Size_of _tmp3FA;enum Cyc_Absyn_Sign
_tmp3FC;enum Cyc_Absyn_Size_of _tmp3FD;enum Cyc_Absyn_Sign _tmp401;enum Cyc_Absyn_Size_of
_tmp402;enum Cyc_Absyn_Sign _tmp404;enum Cyc_Absyn_Size_of _tmp405;enum Cyc_Absyn_Sign
_tmp407;enum Cyc_Absyn_Size_of _tmp408;_LL1D6: {struct Cyc_Absyn_PointerType_struct*
_tmp3F7=(struct Cyc_Absyn_PointerType_struct*)_tmp3F6;if(_tmp3F7->tag != 5)goto
_LL1D8;}_LL1D7: res=Cyc_Absyn_null_exp(0);goto _LL1D5;_LL1D8: {struct Cyc_Absyn_IntType_struct*
_tmp3F8=(struct Cyc_Absyn_IntType_struct*)_tmp3F6;if(_tmp3F8->tag != 6)goto _LL1DA;
else{_tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F8->f2;if(_tmp3FA != Cyc_Absyn_Char_sz)goto
_LL1DA;}}_LL1D9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F9,'\000'),0);goto
_LL1D5;_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3FB=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp3FB->tag != 6)goto _LL1DC;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=_tmp3FB->f2;
if(_tmp3FD != Cyc_Absyn_Short_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3FC,0),0);goto _LL1D5;_LL1DC: {struct Cyc_Absyn_EnumType_struct*_tmp3FE=(
struct Cyc_Absyn_EnumType_struct*)_tmp3F6;if(_tmp3FE->tag != 14)goto _LL1DE;}_LL1DD:
goto _LL1DF;_LL1DE: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3FF=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3F6;if(_tmp3FF->tag != 15)goto _LL1E0;}_LL1DF: _tmp401=Cyc_Absyn_Unsigned;goto
_LL1E1;_LL1E0: {struct Cyc_Absyn_IntType_struct*_tmp400=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp400->tag != 6)goto _LL1E2;else{_tmp401=_tmp400->f1;_tmp402=_tmp400->f2;
if(_tmp402 != Cyc_Absyn_Int_sz)goto _LL1E2;}}_LL1E1: _tmp404=_tmp401;goto _LL1E3;
_LL1E2: {struct Cyc_Absyn_IntType_struct*_tmp403=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp403->tag != 6)goto _LL1E4;else{_tmp404=_tmp403->f1;_tmp405=_tmp403->f2;
if(_tmp405 != Cyc_Absyn_Long_sz)goto _LL1E4;}}_LL1E3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp404,0),0);goto _LL1D5;_LL1E4: {struct Cyc_Absyn_IntType_struct*_tmp406=(struct
Cyc_Absyn_IntType_struct*)_tmp3F6;if(_tmp406->tag != 6)goto _LL1E6;else{_tmp407=
_tmp406->f1;_tmp408=_tmp406->f2;if(_tmp408 != Cyc_Absyn_LongLong_sz)goto _LL1E6;}}
_LL1E5: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp407,(long long)0),0);
goto _LL1D5;_LL1E6: {struct Cyc_Absyn_FloatType_struct*_tmp409=(struct Cyc_Absyn_FloatType_struct*)
_tmp3F6;if(_tmp409->tag != 7)goto _LL1E8;}_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_DoubleType_struct*
_tmp40A=(struct Cyc_Absyn_DoubleType_struct*)_tmp3F6;if(_tmp40A->tag != 8)goto
_LL1EA;}_LL1E9:{const char*_tmpB6B;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB6B="0.0",_tag_dyneither(_tmpB6B,sizeof(char),4)))),0);}goto _LL1D5;_LL1EA:;
_LL1EB: {const char*_tmpB6F;void*_tmpB6E[1];struct Cyc_String_pa_struct _tmpB6D;(
_tmpB6D.tag=0,((_tmpB6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB6E[0]=& _tmpB6D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB6F="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB6F,sizeof(char),40))),_tag_dyneither(_tmpB6E,sizeof(void*),1)))))));}
_LL1D5:;}{struct Cyc_Core_Opt*_tmpB70;res->topt=((_tmpB70=_cycalloc(sizeof(*
_tmpB70)),((_tmpB70->v=t,_tmpB70))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp410=Cyc_Toc_typ_to_c(elt_type);void*_tmp411=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp412=Cyc_Absyn_cstar_typ(_tmp410,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB71;struct Cyc_Core_Opt*_tmp413=(_tmpB71=_cycalloc(sizeof(*_tmpB71)),((
_tmpB71->v=_tmp412,_tmpB71)));struct Cyc_Absyn_Exp*xinit;{void*_tmp414=e1->r;
struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*
_tmp419;_LL1ED: {struct Cyc_Absyn_Deref_e_struct*_tmp415=(struct Cyc_Absyn_Deref_e_struct*)
_tmp414;if(_tmp415->tag != 21)goto _LL1EF;else{_tmp416=_tmp415->f1;}}_LL1EE: if(!
is_dyneither){_tmp416=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp416,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB72;_tmp416->topt=((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((
_tmpB72->v=fat_ptr_type,_tmpB72))));};}Cyc_Toc_exp_to_c(nv,_tmp416);xinit=
_tmp416;goto _LL1EC;_LL1EF: {struct Cyc_Absyn_Subscript_e_struct*_tmp417=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp414;if(_tmp417->tag != 24)goto _LL1F1;else{
_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}}_LL1F0: if(!is_dyneither){_tmp418=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp418,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB73;
_tmp418->topt=((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->v=fat_ptr_type,
_tmpB73))));};}Cyc_Toc_exp_to_c(nv,_tmp418);Cyc_Toc_exp_to_c(nv,_tmp419);{struct
Cyc_Absyn_Exp*_tmpB74[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB74[2]=_tmp419,((_tmpB74[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB74[0]=_tmp418,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB74,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EC;_LL1F1:;_LL1F2: {const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB77="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB77,sizeof(char),53))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}
_LL1EC:;}{struct _tuple0*_tmp41F=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp421;
struct _RegionHandle*_tmp422;struct Cyc_Toc_Env*_tmp420=nv;_tmp421=*_tmp420;
_tmp422=_tmp421.rgn;{struct Cyc_Toc_Env*_tmp423=Cyc_Toc_add_varmap(_tmp422,nv,
_tmp41F,Cyc_Absyn_var_exp(_tmp41F,0));struct Cyc_Absyn_Vardecl*_tmpB78;struct Cyc_Absyn_Vardecl*
_tmp424=(_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->sc=Cyc_Absyn_Public,((
_tmpB78->name=_tmp41F,((_tmpB78->tq=Cyc_Toc_mt_tq,((_tmpB78->type=_tmp411,((
_tmpB78->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB78->rgn=0,((_tmpB78->attributes=
0,((_tmpB78->escapes=0,_tmpB78)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB7B;struct Cyc_Absyn_Local_b_struct*_tmpB7A;struct Cyc_Absyn_Local_b_struct*
_tmp425=(_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=((_tmpB7B.tag=4,((
_tmpB7B.f1=_tmp424,_tmpB7B)))),_tmpB7A)));struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_varb_exp(
_tmp41F,(void*)_tmp425,0);{struct Cyc_Core_Opt*_tmpB7C;_tmp426->topt=((_tmpB7C=
_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->v=fat_ptr_type,_tmpB7C))));}{struct Cyc_Absyn_Exp*
_tmp428=Cyc_Absyn_deref_exp(_tmp426,0);{struct Cyc_Core_Opt*_tmpB7D;_tmp428->topt=((
_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->v=elt_type,_tmpB7D))));}Cyc_Toc_exp_to_c(
_tmp423,_tmp428);{struct _tuple0*_tmp42A=Cyc_Toc_temp_var();_tmp423=Cyc_Toc_add_varmap(
_tmp422,_tmp423,_tmp42A,Cyc_Absyn_var_exp(_tmp42A,0));{struct Cyc_Absyn_Vardecl*
_tmpB7E;struct Cyc_Absyn_Vardecl*_tmp42B=(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((
_tmpB7E->sc=Cyc_Absyn_Public,((_tmpB7E->name=_tmp42A,((_tmpB7E->tq=Cyc_Toc_mt_tq,((
_tmpB7E->type=_tmp410,((_tmpB7E->initializer=(struct Cyc_Absyn_Exp*)_tmp428,((
_tmpB7E->rgn=0,((_tmpB7E->attributes=0,((_tmpB7E->escapes=0,_tmpB7E)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB81;struct Cyc_Absyn_Local_b_struct*_tmpB80;
struct Cyc_Absyn_Local_b_struct*_tmp42C=(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((
_tmpB80[0]=((_tmpB81.tag=4,((_tmpB81.f1=_tmp42B,_tmpB81)))),_tmpB80)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_varb_exp(_tmp42A,(
void*)_tmp42C,0);_tmp42D->topt=_tmp428->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp42D,e2,0);z_init->topt=_tmp42D->topt;}Cyc_Toc_exp_to_c(
_tmp423,z_init);{struct _tuple0*_tmp42E=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB82;struct Cyc_Absyn_Vardecl*_tmp42F=(_tmpB82=_cycalloc(sizeof(*_tmpB82)),((
_tmpB82->sc=Cyc_Absyn_Public,((_tmpB82->name=_tmp42E,((_tmpB82->tq=Cyc_Toc_mt_tq,((
_tmpB82->type=_tmp410,((_tmpB82->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB82->rgn=0,((_tmpB82->attributes=0,((_tmpB82->escapes=0,_tmpB82)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB85;struct Cyc_Absyn_Local_b_struct*_tmpB84;
struct Cyc_Absyn_Local_b_struct*_tmp430=(_tmpB84=_cycalloc(sizeof(*_tmpB84)),((
_tmpB84[0]=((_tmpB85.tag=4,((_tmpB85.f1=_tmp42F,_tmpB85)))),_tmpB84)));_tmp423=
Cyc_Toc_add_varmap(_tmp422,_tmp423,_tmp42E,Cyc_Absyn_var_exp(_tmp42E,0));{struct
Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_varb_exp(_tmp42A,(void*)_tmp42C,0);_tmp431->topt=
_tmp428->topt;{struct Cyc_Absyn_Exp*_tmp432=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp431,_tmp432,0);{
struct Cyc_Core_Opt*_tmpB86;_tmp433->topt=((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((
_tmpB86->v=Cyc_Absyn_sint_typ,_tmpB86))));}Cyc_Toc_exp_to_c(_tmp423,_tmp433);{
struct Cyc_Absyn_Exp*_tmp435=Cyc_Absyn_varb_exp(_tmp42E,(void*)_tmp430,0);_tmp435->topt=
_tmp428->topt;{struct Cyc_Absyn_Exp*_tmp436=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp435,_tmp436,0);{
struct Cyc_Core_Opt*_tmpB87;_tmp437->topt=((_tmpB87=_cycalloc(sizeof(*_tmpB87)),((
_tmpB87->v=Cyc_Absyn_sint_typ,_tmpB87))));}Cyc_Toc_exp_to_c(_tmp423,_tmp437);{
struct Cyc_Absyn_Exp*_tmpB88[2];struct Cyc_List_List*_tmp439=(_tmpB88[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB88[0]=Cyc_Absyn_varb_exp(_tmp41F,(void*)
_tmp425,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB88,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp43A=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp439,0),
_tmp43A,0);{struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp433,_tmp437,0),0);struct Cyc_Absyn_Stmt*_tmp43C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp41F,(void*)_tmp425,0),Cyc_Toc_curr_sp,0);_tmp43D=Cyc_Toc_cast_it(
_tmp412,_tmp43D);{struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_deref_exp(_tmp43D,0);
struct Cyc_Absyn_Exp*_tmp43F=Cyc_Absyn_assign_exp(_tmp43E,Cyc_Absyn_var_exp(
_tmp42E,0),0);struct Cyc_Absyn_Stmt*_tmp440=Cyc_Absyn_exp_stmt(_tmp43F,0);_tmp440=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp43B,_tmp43C,Cyc_Absyn_skip_stmt(
0),0),_tmp440,0);{struct Cyc_Absyn_Var_d_struct*_tmpB8E;struct Cyc_Absyn_Var_d_struct
_tmpB8D;struct Cyc_Absyn_Decl*_tmpB8C;_tmp440=Cyc_Absyn_decl_stmt(((_tmpB8C=
_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->r=(void*)((_tmpB8E=_cycalloc(sizeof(*
_tmpB8E)),((_tmpB8E[0]=((_tmpB8D.tag=0,((_tmpB8D.f1=_tmp42F,_tmpB8D)))),_tmpB8E)))),((
_tmpB8C->loc=0,_tmpB8C)))))),_tmp440,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB94;
struct Cyc_Absyn_Var_d_struct _tmpB93;struct Cyc_Absyn_Decl*_tmpB92;_tmp440=Cyc_Absyn_decl_stmt(((
_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->r=(void*)((_tmpB94=_cycalloc(
sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB93.tag=0,((_tmpB93.f1=_tmp42B,_tmpB93)))),
_tmpB94)))),((_tmpB92->loc=0,_tmpB92)))))),_tmp440,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB9A;struct Cyc_Absyn_Var_d_struct _tmpB99;struct Cyc_Absyn_Decl*_tmpB98;_tmp440=
Cyc_Absyn_decl_stmt(((_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->r=(void*)((
_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB99.tag=0,((_tmpB99.f1=
_tmp424,_tmpB99)))),_tmpB9A)))),((_tmpB98->loc=0,_tmpB98)))))),_tmp440,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp440);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp455=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp457;union Cyc_Absyn_AggrInfoU _tmp458;_LL1F4: {struct Cyc_Absyn_AggrType_struct*
_tmp456=(struct Cyc_Absyn_AggrType_struct*)_tmp455;if(_tmp456->tag != 12)goto
_LL1F6;else{_tmp457=_tmp456->f1;_tmp458=_tmp457.aggr_info;}}_LL1F5: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp458);goto _LL1F3;_LL1F6:;_LL1F7: {struct Cyc_String_pa_struct _tmpBA2;void*
_tmpBA1[1];const char*_tmpBA0;void*_tmpB9F;(_tmpB9F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBA2.tag=0,((_tmpBA2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBA1[0]=& _tmpBA2,Cyc_aprintf(((
_tmpBA0="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBA0,sizeof(char),51))),_tag_dyneither(_tmpBA1,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB9F,sizeof(void*),0)));}_LL1F3:;}{struct _tuple0*_tmp45D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_var_exp(_tmp45D,0);struct Cyc_Absyn_Exp*
_tmp45F=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp45E,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp461=Cyc_Absyn_neq_exp(_tmp45F,_tmp460,0);struct Cyc_Absyn_Exp*
_tmp462=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp45E,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp463=Cyc_Absyn_exp_stmt(_tmp462,0);struct Cyc_Absyn_Stmt*
_tmp464=Cyc_Absyn_ifthenelse_stmt(_tmp461,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp465=Cyc_Absyn_declare_stmt(_tmp45D,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp464,_tmp463,0),0);return Cyc_Toc_stmt_exp_r(
_tmp465);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp466=e->r;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp46A;struct
_dyneither_ptr*_tmp46B;int _tmp46C;int*_tmp46D;struct Cyc_Absyn_Exp*_tmp46F;struct
_dyneither_ptr*_tmp470;int _tmp471;int*_tmp472;_LL1F9: {struct Cyc_Absyn_Cast_e_struct*
_tmp467=(struct Cyc_Absyn_Cast_e_struct*)_tmp466;if(_tmp467->tag != 15)goto _LL1FB;
else{_tmp468=_tmp467->f2;}}_LL1FA: {const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA5="cast on lhs!",_tag_dyneither(_tmpBA5,sizeof(char),13))),_tag_dyneither(
_tmpBA4,sizeof(void*),0)));}_LL1FB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp469=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp466;if(_tmp469->tag != 22)goto _LL1FD;
else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;_tmp46C=_tmp469->f4;_tmp46D=(int*)&
_tmp469->f4;}}_LL1FC: {void*_tmp475=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46A->topt))->v);struct Cyc_Absyn_AggrInfo _tmp477;union Cyc_Absyn_AggrInfoU
_tmp478;_LL202: {struct Cyc_Absyn_AggrType_struct*_tmp476=(struct Cyc_Absyn_AggrType_struct*)
_tmp475;if(_tmp476->tag != 12)goto _LL204;else{_tmp477=_tmp476->f1;_tmp478=_tmp477.aggr_info;}}
_LL203: {struct Cyc_Absyn_Aggrdecl*_tmp479=Cyc_Absyn_get_known_aggrdecl(_tmp478);*
f_tag=Cyc_Toc_get_member_offset(_tmp479,_tmp46B);{const char*_tmpBAA;void*_tmpBA9[
2];struct Cyc_String_pa_struct _tmpBA8;struct Cyc_String_pa_struct _tmpBA7;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBA7.tag=0,((_tmpBA7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp46B),((_tmpBA8.tag=0,((_tmpBA8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp479->name).f2),((_tmpBA9[0]=&
_tmpBA8,((_tmpBA9[1]=& _tmpBA7,Cyc_aprintf(((_tmpBAA="_union_%s_%s",
_tag_dyneither(_tmpBAA,sizeof(char),13))),_tag_dyneither(_tmpBA9,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBAB;*tagged_member_type=Cyc_Absyn_strct(((_tmpBAB=
_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB[0]=str,_tmpBAB)))));}if(clear_read)*
_tmp46D=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp479->impl))->tagged;};}
_LL204:;_LL205: return 0;_LL201:;}_LL1FD: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp46E=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp466;if(_tmp46E->tag != 23)goto
_LL1FF;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46E->f2;_tmp471=_tmp46E->f4;_tmp472=(
int*)& _tmp46E->f4;}}_LL1FE: {void*_tmp47F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp481;void*_tmp482;
_LL207: {struct Cyc_Absyn_PointerType_struct*_tmp480=(struct Cyc_Absyn_PointerType_struct*)
_tmp47F;if(_tmp480->tag != 5)goto _LL209;else{_tmp481=_tmp480->f1;_tmp482=_tmp481.elt_typ;}}
_LL208: {void*_tmp483=Cyc_Tcutil_compress(_tmp482);struct Cyc_Absyn_AggrInfo
_tmp485;union Cyc_Absyn_AggrInfoU _tmp486;_LL20C: {struct Cyc_Absyn_AggrType_struct*
_tmp484=(struct Cyc_Absyn_AggrType_struct*)_tmp483;if(_tmp484->tag != 12)goto
_LL20E;else{_tmp485=_tmp484->f1;_tmp486=_tmp485.aggr_info;}}_LL20D: {struct Cyc_Absyn_Aggrdecl*
_tmp487=Cyc_Absyn_get_known_aggrdecl(_tmp486);*f_tag=Cyc_Toc_get_member_offset(
_tmp487,_tmp470);{const char*_tmpBB0;void*_tmpBAF[2];struct Cyc_String_pa_struct
_tmpBAE;struct Cyc_String_pa_struct _tmpBAD;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBAD.tag=0,((_tmpBAD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp470),((_tmpBAE.tag=0,((_tmpBAE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp487->name).f2),((_tmpBAF[0]=& _tmpBAE,((_tmpBAF[1]=&
_tmpBAD,Cyc_aprintf(((_tmpBB0="_union_%s_%s",_tag_dyneither(_tmpBB0,sizeof(char),
13))),_tag_dyneither(_tmpBAF,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBB1;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((
_tmpBB1[0]=str,_tmpBB1)))));}if(clear_read)*_tmp472=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp487->impl))->tagged;};}_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;
_LL20A: return 0;_LL206:;}_LL1FF:;_LL200: return 0;_LL1F8:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp48D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48D,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp48E=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp48E,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp48D,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC10(unsigned int*_tmpC0F,unsigned int*_tmpC0E,struct _tuple0***
_tmpC0C){for(*_tmpC0F=0;*_tmpC0F < *_tmpC0E;(*_tmpC0F)++){(*_tmpC0C)[*_tmpC0F]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp48F=e->r;if(e->topt == 0){const char*_tmpBB5;void*_tmpBB4[1];struct Cyc_String_pa_struct
_tmpBB3;(_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBB4[0]=& _tmpBB3,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB5="exp_to_c: no type for %s",
_tag_dyneither(_tmpBB5,sizeof(char),25))),_tag_dyneither(_tmpBB4,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp493=
_tmp48F;union Cyc_Absyn_Cnst _tmp495;int _tmp496;struct _tuple0*_tmp499;void*_tmp49A;
struct _tuple0*_tmp49C;enum Cyc_Absyn_Primop _tmp49E;struct Cyc_List_List*_tmp49F;
struct Cyc_Absyn_Exp*_tmp4A1;enum Cyc_Absyn_Incrementor _tmp4A2;struct Cyc_Absyn_Exp*
_tmp4A4;struct Cyc_Core_Opt*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*
_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*
_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_List_List*
_tmp4B9;struct Cyc_Absyn_VarargCallInfo*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;
struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_VarargCallInfo*_tmp4BE;struct Cyc_Absyn_VarargCallInfo
_tmp4BF;int _tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_VarargInfo*
_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*
_tmp4C8;struct Cyc_List_List*_tmp4C9;void*_tmp4CB;void**_tmp4CC;struct Cyc_Absyn_Exp*
_tmp4CD;int _tmp4CE;enum Cyc_Absyn_Coercion _tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;
struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*
_tmp4D6;void*_tmp4D8;void*_tmp4DA;void*_tmp4DB;struct _dyneither_ptr*_tmp4DD;void*
_tmp4DF;void*_tmp4E0;unsigned int _tmp4E2;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*
_tmp4E6;struct _dyneither_ptr*_tmp4E7;int _tmp4E8;int _tmp4E9;struct Cyc_Absyn_Exp*
_tmp4EB;struct _dyneither_ptr*_tmp4EC;int _tmp4ED;int _tmp4EE;struct Cyc_Absyn_Exp*
_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*
_tmp4F5;struct Cyc_Absyn_Vardecl*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*
_tmp4F9;int _tmp4FA;struct _tuple0*_tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_List_List*
_tmp4FE;struct Cyc_Absyn_Aggrdecl*_tmp4FF;void*_tmp501;struct Cyc_List_List*
_tmp502;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp505;struct
Cyc_Absyn_Datatypefield*_tmp506;struct Cyc_Absyn_MallocInfo _tmp50A;int _tmp50B;
struct Cyc_Absyn_Exp*_tmp50C;void**_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;int _tmp50F;
struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*
_tmp514;struct _dyneither_ptr*_tmp515;struct Cyc_Absyn_Stmt*_tmp517;_LL211: {
struct Cyc_Absyn_Const_e_struct*_tmp494=(struct Cyc_Absyn_Const_e_struct*)_tmp493;
if(_tmp494->tag != 0)goto _LL213;else{_tmp495=_tmp494->f1;if((_tmp495.Null_c).tag
!= 1)goto _LL213;_tmp496=(int)(_tmp495.Null_c).val;}}_LL212: {struct Cyc_Absyn_Exp*
_tmp51B=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp51B,
_tmp51B))->r;else{struct Cyc_Absyn_Exp*_tmpBB6[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBB6[2]=_tmp51B,((_tmpBB6[1]=_tmp51B,((_tmpBB6[0]=_tmp51B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_struct*
_tmp497=(struct Cyc_Absyn_Const_e_struct*)_tmp493;if(_tmp497->tag != 0)goto _LL215;}
_LL214: goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_struct*_tmp498=(struct Cyc_Absyn_Var_e_struct*)
_tmp493;if(_tmp498->tag != 1)goto _LL217;else{_tmp499=_tmp498->f1;_tmp49A=(void*)
_tmp498->f2;}}_LL216:{struct _handler_cons _tmp51D;_push_handler(& _tmp51D);{int
_tmp51F=0;if(setjmp(_tmp51D.handler))_tmp51F=1;if(!_tmp51F){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp499))->r;;_pop_handler();}else{void*_tmp51E=(void*)_exn_thrown;void*
_tmp521=_tmp51E;_LL268: {struct Cyc_Dict_Absent_struct*_tmp522=(struct Cyc_Dict_Absent_struct*)
_tmp521;if(_tmp522->tag != Cyc_Dict_Absent)goto _LL26A;}_LL269: {const char*_tmpBBA;
void*_tmpBB9[1];struct Cyc_String_pa_struct _tmpBB8;(_tmpBB8.tag=0,((_tmpBB8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp499)),((
_tmpBB9[0]=& _tmpBB8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBA="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBBA,sizeof(char),
32))),_tag_dyneither(_tmpBB9,sizeof(void*),1)))))));}_LL26A:;_LL26B:(void)_throw(
_tmp521);_LL267:;}};}goto _LL210;_LL217: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp49B=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp493;if(_tmp49B->tag != 2)goto
_LL219;else{_tmp49C=_tmp49B->f1;}}_LL218: {const char*_tmpBBD;void*_tmpBBC;(
_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBD="unknownid",_tag_dyneither(_tmpBBD,sizeof(char),10))),_tag_dyneither(
_tmpBBC,sizeof(void*),0)));}_LL219: {struct Cyc_Absyn_Primop_e_struct*_tmp49D=(
struct Cyc_Absyn_Primop_e_struct*)_tmp493;if(_tmp49D->tag != 3)goto _LL21B;else{
_tmp49E=_tmp49D->f1;_tmp49F=_tmp49D->f2;}}_LL21A: {struct Cyc_List_List*_tmp528=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp49F);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp49F);switch(_tmp49E){case Cyc_Absyn_Numelts: _LL26C: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp49F))->hd;{void*_tmp529=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp52B;struct Cyc_Absyn_Exp*
_tmp52C;struct Cyc_Absyn_PtrInfo _tmp52E;void*_tmp52F;struct Cyc_Absyn_PtrAtts
_tmp530;union Cyc_Absyn_Constraint*_tmp531;union Cyc_Absyn_Constraint*_tmp532;
union Cyc_Absyn_Constraint*_tmp533;_LL26F: {struct Cyc_Absyn_ArrayType_struct*
_tmp52A=(struct Cyc_Absyn_ArrayType_struct*)_tmp529;if(_tmp52A->tag != 9)goto
_LL271;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52B.num_elts;}}_LL270: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp52C))){const char*_tmpBC0;void*_tmpBBF;(
_tmpBBF=0,Cyc_Tcutil_terr(e->loc,((_tmpBC0="can't calculate numelts",
_tag_dyneither(_tmpBC0,sizeof(char),24))),_tag_dyneither(_tmpBBF,sizeof(void*),0)));}
e->r=_tmp52C->r;goto _LL26E;_LL271: {struct Cyc_Absyn_PointerType_struct*_tmp52D=(
struct Cyc_Absyn_PointerType_struct*)_tmp529;if(_tmp52D->tag != 5)goto _LL273;else{
_tmp52E=_tmp52D->f1;_tmp52F=_tmp52E.elt_typ;_tmp530=_tmp52E.ptr_atts;_tmp531=
_tmp530.nullable;_tmp532=_tmp530.bounds;_tmp533=_tmp530.zero_term;}}_LL272:{void*
_tmp536=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp532);struct Cyc_Absyn_Exp*_tmp539;_LL276: {struct Cyc_Absyn_DynEither_b_struct*
_tmp537=(struct Cyc_Absyn_DynEither_b_struct*)_tmp536;if(_tmp537->tag != 0)goto
_LL278;}_LL277:{struct Cyc_Absyn_Exp*_tmpBC1[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBC1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F),0),((_tmpBC1[0]=(
struct Cyc_Absyn_Exp*)_tmp49F->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBC1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL275;_LL278: {struct Cyc_Absyn_Upper_b_struct*_tmp538=(struct Cyc_Absyn_Upper_b_struct*)
_tmp536;if(_tmp538->tag != 1)goto _LL275;else{_tmp539=_tmp538->f1;}}_LL279: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp533)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp49F->hd);struct Cyc_Absyn_Exp*_tmpBC2[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBC2[1]=_tmp539,((_tmpBC2[0]=(struct Cyc_Absyn_Exp*)_tmp49F->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBC2,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp531)){if(!Cyc_Evexp_c_can_eval(_tmp539)){const
char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,Cyc_Tcutil_terr(e->loc,((_tmpBC5="can't calculate numelts",
_tag_dyneither(_tmpBC5,sizeof(char),24))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp539,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp539->r;goto _LL275;}}goto _LL275;_LL275:;}goto _LL26E;_LL273:;_LL274: {const
char*_tmpBCA;void*_tmpBC9[2];struct Cyc_String_pa_struct _tmpBC8;struct Cyc_String_pa_struct
_tmpBC7;(_tmpBC7.tag=0,((_tmpBC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBC8.tag=0,((_tmpBC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBC9[0]=& _tmpBC8,((
_tmpBC9[1]=& _tmpBC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBCA="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBCA,
sizeof(char),41))),_tag_dyneither(_tmpBC9,sizeof(void*),2)))))))))))));}_LL26E:;}
break;}case Cyc_Absyn_Plus: _LL26D:{void*_tmp542=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp528))->hd);struct Cyc_Absyn_PtrInfo _tmp544;
void*_tmp545;struct Cyc_Absyn_PtrAtts _tmp546;union Cyc_Absyn_Constraint*_tmp547;
union Cyc_Absyn_Constraint*_tmp548;_LL27C: {struct Cyc_Absyn_PointerType_struct*
_tmp543=(struct Cyc_Absyn_PointerType_struct*)_tmp542;if(_tmp543->tag != 5)goto
_LL27E;else{_tmp544=_tmp543->f1;_tmp545=_tmp544.elt_typ;_tmp546=_tmp544.ptr_atts;
_tmp547=_tmp546.bounds;_tmp548=_tmp546.zero_term;}}_LL27D:{void*_tmp549=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp547);struct Cyc_Absyn_Exp*_tmp54C;_LL281: {struct Cyc_Absyn_DynEither_b_struct*
_tmp54A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp549;if(_tmp54A->tag != 0)goto
_LL283;}_LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp49F->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBCB[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBCB[2]=e2,((_tmpBCB[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp545),0),((_tmpBCB[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL280;}_LL283: {struct Cyc_Absyn_Upper_b_struct*_tmp54B=(struct
Cyc_Absyn_Upper_b_struct*)_tmp549;if(_tmp54B->tag != 1)goto _LL280;else{_tmp54C=
_tmp54B->f1;}}_LL284: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp548)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp49F->tl))->hd;struct Cyc_Absyn_Exp*_tmpBCC[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBCC[2]=e2,((_tmpBCC[1]=_tmp54C,((_tmpBCC[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCC,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL280;_LL280:;}goto _LL27B;_LL27E:;_LL27F: goto _LL27B;
_LL27B:;}break;case Cyc_Absyn_Minus: _LL27A: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp528))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp528->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBCD[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBCD[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBCD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBCD[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBCD,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL285: goto _LL286;case Cyc_Absyn_Neq: _LL286: goto _LL287;case Cyc_Absyn_Gt: _LL287:
goto _LL288;case Cyc_Absyn_Gte: _LL288: goto _LL289;case Cyc_Absyn_Lt: _LL289: goto
_LL28A;case Cyc_Absyn_Lte: _LL28A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp528))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp528->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL28B: break;}goto _LL210;}_LL21B: {
struct Cyc_Absyn_Increment_e_struct*_tmp4A0=(struct Cyc_Absyn_Increment_e_struct*)
_tmp493;if(_tmp4A0->tag != 5)goto _LL21D;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}
_LL21C: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A1->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBCE;struct _dyneither_ptr incr_str=(_tmpBCE="increment",
_tag_dyneither(_tmpBCE,sizeof(char),10));if(_tmp4A2 == Cyc_Absyn_PreDec  || 
_tmp4A2 == Cyc_Absyn_PostDec){const char*_tmpBCF;incr_str=((_tmpBCF="decrement",
_tag_dyneither(_tmpBCF,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A1,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBD3;void*_tmpBD2[1];
struct Cyc_String_pa_struct _tmpBD1;(_tmpBD1.tag=0,((_tmpBD1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD2[0]=& _tmpBD1,Cyc_Tcutil_terr(
e->loc,((_tmpBD3="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBD3,sizeof(char),74))),_tag_dyneither(_tmpBD2,sizeof(void*),1)))))));}{
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBD6,sizeof(char),30))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4A1,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp556=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBD7;_tmp556->topt=((_tmpBD7=_cycalloc(sizeof(*
_tmpBD7)),((_tmpBD7->v=Cyc_Absyn_sint_typ,_tmpBD7))));}switch(_tmp4A2){case Cyc_Absyn_PreInc:
_LL28D:{struct Cyc_Absyn_AssignOp_e_struct _tmpBDD;struct Cyc_Core_Opt*_tmpBDC;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBDB;e->r=(void*)((_tmpBDB=_cycalloc(
sizeof(*_tmpBDB)),((_tmpBDB[0]=((_tmpBDD.tag=4,((_tmpBDD.f1=_tmp4A1,((_tmpBDD.f2=((
_tmpBDC=_cycalloc_atomic(sizeof(*_tmpBDC)),((_tmpBDC->v=(void*)Cyc_Absyn_Plus,
_tmpBDC)))),((_tmpBDD.f3=_tmp556,_tmpBDD)))))))),_tmpBDB))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL28E:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBE3;struct Cyc_Core_Opt*_tmpBE2;struct Cyc_Absyn_AssignOp_e_struct*_tmpBE1;e->r=(
void*)((_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE3.tag=4,((
_tmpBE3.f1=_tmp4A1,((_tmpBE3.f2=((_tmpBE2=_cycalloc_atomic(sizeof(*_tmpBE2)),((
_tmpBE2->v=(void*)Cyc_Absyn_Minus,_tmpBE2)))),((_tmpBE3.f3=_tmp556,_tmpBE3)))))))),
_tmpBE1))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL28F:{const char*_tmpBE7;
void*_tmpBE6[1];struct Cyc_String_pa_struct _tmpBE5;(_tmpBE5.tag=0,((_tmpBE5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBE6[0]=& _tmpBE5,Cyc_Tcutil_terr(
e->loc,((_tmpBE7="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBE7,sizeof(char),59))),_tag_dyneither(_tmpBE6,sizeof(void*),1)))))));}{
const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBEA,sizeof(char),34))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4A1);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4A2 == Cyc_Absyn_PostInc
 || _tmp4A2 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4A2 == Cyc_Absyn_PreDec  || _tmp4A2 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBEB[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBEB[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBEB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBEB[0]=Cyc_Toc_push_address_exp(_tmp4A1),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp564=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A1->topt))->v);enum Cyc_Absyn_Size_of _tmp566;int _tmp569;_LL292: {
struct Cyc_Absyn_IntType_struct*_tmp565=(struct Cyc_Absyn_IntType_struct*)_tmp564;
if(_tmp565->tag != 6)goto _LL294;else{_tmp566=_tmp565->f2;}}_LL293: switch(_tmp566){
case Cyc_Absyn_Char_sz: _LL29C: fn_e=_tmp4A2 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL29D: fn_e=
_tmp4A2 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL29E: fn_e=_tmp4A2 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL29F: {struct Cyc_Core_Impossible_struct
_tmpBF1;const char*_tmpBF0;struct Cyc_Core_Impossible_struct*_tmpBEF;(int)_throw((
void*)((_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF1.tag=Cyc_Core_Impossible,((
_tmpBF1.f1=((_tmpBF0="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBF0,sizeof(char),44))),_tmpBF1)))),_tmpBEF)))));}}goto _LL291;
_LL294: {struct Cyc_Absyn_FloatType_struct*_tmp567=(struct Cyc_Absyn_FloatType_struct*)
_tmp564;if(_tmp567->tag != 7)goto _LL296;}_LL295: fn_e=_tmp4A2 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL291;_LL296: {struct Cyc_Absyn_DoubleType_struct*_tmp568=(struct Cyc_Absyn_DoubleType_struct*)
_tmp564;if(_tmp568->tag != 8)goto _LL298;else{_tmp569=_tmp568->f1;}}_LL297: switch(
_tmp569){case 1: _LL2A1: fn_e=_tmp4A2 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL2A2: fn_e=_tmp4A2 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL291;_LL298: {struct Cyc_Absyn_PointerType_struct*_tmp56A=(struct Cyc_Absyn_PointerType_struct*)
_tmp564;if(_tmp56A->tag != 5)goto _LL29A;}_LL299: fn_e=_tmp4A2 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL291;_LL29A:;_LL29B: {struct Cyc_Core_Impossible_struct _tmpBF7;const char*
_tmpBF6;struct Cyc_Core_Impossible_struct*_tmpBF5;(int)_throw((void*)((_tmpBF5=
_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF7.tag=Cyc_Core_Impossible,((
_tmpBF7.f1=((_tmpBF6="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBF6,sizeof(char),64))),_tmpBF7)))),_tmpBF5)))));}_LL291:;}{
struct Cyc_Absyn_Exp*_tmpBF8[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBF8[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBF8[0]=_tmp4A1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF8,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
== (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A1)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4A1,0,Cyc_Toc_incr_lvalue,_tmp4A2);e->r=_tmp4A1->r;}}}goto _LL210;};};}_LL21D: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4A3=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp493;if(_tmp4A3->tag != 4)goto _LL21F;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;
_tmp4A6=_tmp4A3->f3;}}_LL21E: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A4->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A6->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4A4,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4A4);Cyc_Toc_exp_to_c(nv,
_tmp4A6);e->r=Cyc_Toc_tagged_union_assignop(_tmp4A4,e1_old_typ,_tmp4A5,_tmp4A6,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A4,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4A4,_tmp4A5,_tmp4A6,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4A4);Cyc_Toc_exp_to_c(nv,_tmp4A4);Cyc_Toc_exp_to_c(nv,_tmp4A6);{int done=0;
if(_tmp4A5 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4A5->v){case Cyc_Absyn_Plus: _LL2A4: change=_tmp4A6;break;case Cyc_Absyn_Minus:
_LL2A5: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A6,0);break;default:
_LL2A6: {const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFB="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBFB,sizeof(char),27))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp575=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBFC[3];e->r=Cyc_Toc_fncall_exp_r(_tmp575,((_tmpBFC[2]=change,((
_tmpBFC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBFC[0]=Cyc_Toc_push_address_exp(
_tmp4A4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4A5->v){case Cyc_Absyn_Plus:
_LL2A8: done=1;{struct Cyc_Absyn_Exp*_tmpBFD[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A4),((_tmpBFD[1]=_tmp4A6,((
_tmpBFD[0]=_tmp4A4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A9: {const char*_tmpC00;void*_tmpBFF;(_tmpBFF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC00="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC00,sizeof(char),39))),_tag_dyneither(_tmpBFF,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4A6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4A6->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4A4)){{struct _tuple18 _tmpC03;struct
_tuple18*_tmpC02;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4A4,0,Cyc_Toc_assignop_lvalue,((_tmpC02=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC02[0]=((_tmpC03.f1=_tmp4A5,((_tmpC03.f2=
_tmp4A6,_tmpC03)))),_tmpC02)))));}e->r=_tmp4A4->r;}}goto _LL210;};};};}_LL21F: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4A7=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp493;if(_tmp4A7->tag != 6)goto _LL221;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A7->f2;
_tmp4AA=_tmp4A7->f3;}}_LL220: Cyc_Toc_exp_to_c(nv,_tmp4A8);Cyc_Toc_exp_to_c(nv,
_tmp4A9);Cyc_Toc_exp_to_c(nv,_tmp4AA);goto _LL210;_LL221: {struct Cyc_Absyn_And_e_struct*
_tmp4AB=(struct Cyc_Absyn_And_e_struct*)_tmp493;if(_tmp4AB->tag != 7)goto _LL223;
else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;}}_LL222: Cyc_Toc_exp_to_c(nv,
_tmp4AC);Cyc_Toc_exp_to_c(nv,_tmp4AD);goto _LL210;_LL223: {struct Cyc_Absyn_Or_e_struct*
_tmp4AE=(struct Cyc_Absyn_Or_e_struct*)_tmp493;if(_tmp4AE->tag != 8)goto _LL225;
else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AE->f2;}}_LL224: Cyc_Toc_exp_to_c(nv,
_tmp4AF);Cyc_Toc_exp_to_c(nv,_tmp4B0);goto _LL210;_LL225: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4B1=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp493;if(_tmp4B1->tag != 9)goto _LL227;
else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;}}_LL226: Cyc_Toc_exp_to_c(nv,
_tmp4B2);Cyc_Toc_exp_to_c(nv,_tmp4B3);goto _LL210;_LL227: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4B4=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp493;if(_tmp4B4->tag != 10)goto
_LL229;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;}}_LL228: _tmp4B8=_tmp4B5;
_tmp4B9=_tmp4B6;goto _LL22A;_LL229: {struct Cyc_Absyn_FnCall_e_struct*_tmp4B7=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp493;if(_tmp4B7->tag != 11)goto _LL22B;else{
_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;_tmp4BA=_tmp4B7->f3;if(_tmp4BA != 0)goto
_LL22B;}}_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B8);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4B9);goto _LL210;_LL22B: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4BB=(struct Cyc_Absyn_FnCall_e_struct*)_tmp493;if(_tmp4BB->tag != 11)goto
_LL22D;else{_tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;_tmp4BE=_tmp4BB->f3;if(
_tmp4BE == 0)goto _LL22D;_tmp4BF=*_tmp4BE;_tmp4C0=_tmp4BF.num_varargs;_tmp4C1=
_tmp4BF.injectors;_tmp4C2=_tmp4BF.vai;}}_LL22C: {struct Cyc_Toc_Env _tmp57D;struct
_RegionHandle*_tmp57E;struct Cyc_Toc_Env*_tmp57C=nv;_tmp57D=*_tmp57C;_tmp57E=
_tmp57D.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4C0,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4C2->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4BD);int num_normargs=num_args - _tmp4C0;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4BD=_tmp4BD->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd);{
struct Cyc_List_List*_tmpC04;new_args=((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((
_tmpC04->hd=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((_tmpC04->tl=new_args,_tmpC04))))));};}}{
struct Cyc_Absyn_Exp*_tmpC07[3];struct Cyc_List_List*_tmpC06;new_args=((_tmpC06=
_cycalloc(sizeof(*_tmpC06)),((_tmpC06->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC07[2]=num_varargs_exp,((_tmpC07[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC07[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC07,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC06->tl=
new_args,_tmpC06))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4BC);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4BC,new_args,0),0);if(_tmp4C2->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp582=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4C2->type));struct Cyc_Absyn_DatatypeInfo _tmp584;union Cyc_Absyn_DatatypeInfoU
_tmp585;struct Cyc_Absyn_Datatypedecl**_tmp586;struct Cyc_Absyn_Datatypedecl*
_tmp587;_LL2AC: {struct Cyc_Absyn_DatatypeType_struct*_tmp583=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp582;if(_tmp583->tag != 3)goto _LL2AE;else{_tmp584=_tmp583->f1;_tmp585=_tmp584.datatype_info;
if((_tmp585.KnownDatatype).tag != 2)goto _LL2AE;_tmp586=(struct Cyc_Absyn_Datatypedecl**)(
_tmp585.KnownDatatype).val;_tmp587=*_tmp586;}}_LL2AD: tud=_tmp587;goto _LL2AB;
_LL2AE:;_LL2AF: {const char*_tmpC0A;void*_tmpC09;(_tmpC09=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0A="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC0A,sizeof(char),44))),_tag_dyneither(_tmpC09,sizeof(void*),0)));}
_LL2AB:;}{unsigned int _tmpC0F;unsigned int _tmpC0E;struct _dyneither_ptr _tmpC0D;
struct _tuple0**_tmpC0C;unsigned int _tmpC0B;struct _dyneither_ptr vs=(_tmpC0B=(
unsigned int)_tmp4C0,((_tmpC0C=(struct _tuple0**)_region_malloc(_tmp57E,
_check_times(sizeof(struct _tuple0*),_tmpC0B)),((_tmpC0D=_tag_dyneither(_tmpC0C,
sizeof(struct _tuple0*),_tmpC0B),((((_tmpC0E=_tmpC0B,_tmpC10(& _tmpC0F,& _tmpC0E,&
_tmpC0C))),_tmpC0D)))))));if(_tmp4C0 != 0){struct Cyc_List_List*_tmp58A=0;{int i=
_tmp4C0 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC11;_tmp58A=((_tmpC11=
_cycalloc(sizeof(*_tmpC11)),((_tmpC11->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC11->tl=_tmp58A,_tmpC11))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp58A,0),s,0);{
int i=0;for(0;_tmp4BD != 0;(((_tmp4BD=_tmp4BD->tl,_tmp4C1=_tmp4C1->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4BD->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp58D;struct
_tuple0*_tmp58E;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_Datatypefield*
_tmp58C=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4C1))->hd;_tmp58D=*_tmp58C;_tmp58E=_tmp58D.name;_tmp58F=_tmp58D.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp58F))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp58E),0),s,0);{const char*_tmpC12;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp58E,((_tmpC12="_struct",_tag_dyneither(_tmpC12,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC13[3];struct Cyc_List_List*
_tmp591=(_tmpC13[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC13[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC13[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC13,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4BD
!= 0;(_tmp4BD=_tmp4BD->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4BD->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4BD->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL210;}_LL22D: {struct Cyc_Absyn_Throw_e_struct*_tmp4C3=(struct Cyc_Absyn_Throw_e_struct*)
_tmp493;if(_tmp4C3->tag != 12)goto _LL22F;else{_tmp4C4=_tmp4C3->f1;}}_LL22E: Cyc_Toc_exp_to_c(
nv,_tmp4C4);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4C4),0))->r;goto _LL210;_LL22F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4C5=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp493;if(_tmp4C5->tag != 13)
goto _LL231;else{_tmp4C6=_tmp4C5->f1;}}_LL230: Cyc_Toc_exp_to_c(nv,_tmp4C6);goto
_LL210;_LL231: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4C7=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp493;if(_tmp4C7->tag != 14)goto _LL233;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C7->f2;}}
_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C8);for(0;_tmp4C9 != 0;_tmp4C9=_tmp4C9->tl){enum 
Cyc_Absyn_KindQual _tmp597=(Cyc_Tcutil_typ_kind((void*)_tmp4C9->hd))->kind;if(
_tmp597 != Cyc_Absyn_EffKind  && _tmp597 != Cyc_Absyn_RgnKind){{void*_tmp598=Cyc_Tcutil_compress((
void*)_tmp4C9->hd);_LL2B1: {struct Cyc_Absyn_VarType_struct*_tmp599=(struct Cyc_Absyn_VarType_struct*)
_tmp598;if(_tmp599->tag != 2)goto _LL2B3;}_LL2B2: goto _LL2B4;_LL2B3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp59A=(struct Cyc_Absyn_DatatypeType_struct*)_tmp598;if(_tmp59A->tag != 3)goto
_LL2B5;}_LL2B4: continue;_LL2B5:;_LL2B6: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4C8,0))->r;goto _LL2B0;
_LL2B0:;}break;}}goto _LL210;_LL233: {struct Cyc_Absyn_Cast_e_struct*_tmp4CA=(
struct Cyc_Absyn_Cast_e_struct*)_tmp493;if(_tmp4CA->tag != 15)goto _LL235;else{
_tmp4CB=(void**)& _tmp4CA->f1;_tmp4CC=(void**)((void**)& _tmp4CA->f1);_tmp4CD=
_tmp4CA->f2;_tmp4CE=_tmp4CA->f3;_tmp4CF=_tmp4CA->f4;}}_LL234: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4CD->topt))->v;void*new_typ=*_tmp4CC;*
_tmp4CC=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4CD);{struct _tuple17
_tmpC14;struct _tuple17 _tmp59C=(_tmpC14.f1=Cyc_Tcutil_compress(old_t2),((_tmpC14.f2=
Cyc_Tcutil_compress(new_typ),_tmpC14)));void*_tmp59D;struct Cyc_Absyn_PtrInfo
_tmp59F;void*_tmp5A0;struct Cyc_Absyn_PtrInfo _tmp5A2;void*_tmp5A3;struct Cyc_Absyn_PtrInfo
_tmp5A5;void*_tmp5A6;_LL2B8: _tmp59D=_tmp59C.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp59E=(struct Cyc_Absyn_PointerType_struct*)_tmp59D;if(_tmp59E->tag != 5)goto
_LL2BA;else{_tmp59F=_tmp59E->f1;}};_tmp5A0=_tmp59C.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5A1=(struct Cyc_Absyn_PointerType_struct*)_tmp5A0;if(_tmp5A1->tag != 5)goto
_LL2BA;else{_tmp5A2=_tmp5A1->f1;}};_LL2B9: {int _tmp5A8=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp59F.ptr_atts).nullable);int _tmp5A9=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A2.ptr_atts).nullable);
void*_tmp5AA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp59F.ptr_atts).bounds);void*_tmp5AB=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A2.ptr_atts).bounds);
int _tmp5AC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp59F.ptr_atts).zero_term);int _tmp5AD=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A2.ptr_atts).zero_term);{struct _tuple17 _tmpC15;
struct _tuple17 _tmp5AF=(_tmpC15.f1=_tmp5AA,((_tmpC15.f2=_tmp5AB,_tmpC15)));void*
_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B2;void*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B5;
void*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B8;void*_tmp5B9;void*_tmp5BB;void*_tmp5BD;
struct Cyc_Absyn_Exp*_tmp5BF;void*_tmp5C0;void*_tmp5C2;_LL2BF: _tmp5B0=_tmp5AF.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B1=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B0;
if(_tmp5B1->tag != 1)goto _LL2C1;else{_tmp5B2=_tmp5B1->f1;}};_tmp5B3=_tmp5AF.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B4=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B3;
if(_tmp5B4->tag != 1)goto _LL2C1;else{_tmp5B5=_tmp5B4->f1;}};_LL2C0: if((!Cyc_Evexp_c_can_eval(
_tmp5B2) || !Cyc_Evexp_c_can_eval(_tmp5B5)) && !Cyc_Evexp_same_const_exp(_tmp5B2,
_tmp5B5)){const char*_tmpC18;void*_tmpC17;(_tmpC17=0,Cyc_Tcutil_terr(e->loc,((
_tmpC18="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC18,sizeof(char),54))),_tag_dyneither(_tmpC17,sizeof(void*),0)));}if(_tmp5A8
 && !_tmp5A9){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC1B;void*_tmpC1A;(
_tmpC1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC1B="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC1B,
sizeof(char),44))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}if(_tmp4CF != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC1F;void*_tmpC1E[1];struct Cyc_String_pa_struct _tmpC1D;(_tmpC1D.tag=
0,((_tmpC1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC1E[0]=& _tmpC1D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC1F="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC1F,sizeof(char),41))),_tag_dyneither(_tmpC1E,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4CD);if(do_null_check){if(!_tmp4CE){
const char*_tmpC22;void*_tmpC21;(_tmpC21=0,Cyc_Tcutil_warn(e->loc,((_tmpC22="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC22,sizeof(char),58))),_tag_dyneither(_tmpC21,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC23;e->r=Cyc_Toc_cast_it_r(*_tmp4CC,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->hd=
_tmp4CD,((_tmpC23->tl=0,_tmpC23)))))),0));};}};}goto _LL2BE;_LL2C1: _tmp5B6=
_tmp5AF.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5B7=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5B6;if(_tmp5B7->tag != 1)goto _LL2C3;else{_tmp5B8=_tmp5B7->f1;}};_tmp5B9=
_tmp5AF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5B9;if(_tmp5BA->tag != 0)goto _LL2C3;};_LL2C2: if(!Cyc_Evexp_c_can_eval(_tmp5B8)){
const char*_tmpC26;void*_tmpC25;(_tmpC25=0,Cyc_Tcutil_terr(e->loc,((_tmpC26="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC26,sizeof(char),71))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
if(_tmp4CF == Cyc_Absyn_NonNull_to_Null){const char*_tmpC2A;void*_tmpC29[1];struct
Cyc_String_pa_struct _tmpC28;(_tmpC28.tag=0,((_tmpC28.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC29[0]=& _tmpC28,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC2A="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC2A,sizeof(char),44))),_tag_dyneither(_tmpC29,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5AC  && !(_tmp5A2.elt_tq).real_const) && !
_tmp5AD)_tmp5B8=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5B8,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5B8,_tmp4CD))->r;}else{
struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc__tag_dyneither_e;if(_tmp5AC){struct _tuple0*
_tmp5D4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Absyn_var_exp(_tmp5D4,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5D6=_tmp4CD->r;union Cyc_Absyn_Cnst _tmp5D8;
struct _dyneither_ptr _tmp5D9;_LL2C8: {struct Cyc_Absyn_Const_e_struct*_tmp5D7=(
struct Cyc_Absyn_Const_e_struct*)_tmp5D6;if(_tmp5D7->tag != 0)goto _LL2CA;else{
_tmp5D8=_tmp5D7->f1;if((_tmp5D8.String_c).tag != 7)goto _LL2CA;_tmp5D9=(struct
_dyneither_ptr)(_tmp5D8.String_c).val;}}_LL2C9: arg3=_tmp5B8;goto _LL2C7;_LL2CA:;
_LL2CB:{struct Cyc_Absyn_Exp*_tmpC2B[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4CD),((_tmpC2B[1]=_tmp5B8,((_tmpC2B[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5D5),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2B,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C7;_LL2C7:;}if(!_tmp5AD  && !(_tmp5A2.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5DB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A2.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC2C[3];struct Cyc_Absyn_Exp*_tmp5DC=Cyc_Absyn_fncall_exp(_tmp5D3,((_tmpC2C[2]=
arg3,((_tmpC2C[1]=_tmp5DB,((_tmpC2C[0]=_tmp5D5,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5DD=Cyc_Absyn_exp_stmt(_tmp5DC,0);_tmp5DD=
Cyc_Absyn_declare_stmt(_tmp5D4,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4CD,_tmp5DD,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5DD);};}else{struct Cyc_Absyn_Exp*
_tmpC2D[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D3,((_tmpC2D[2]=_tmp5B8,((_tmpC2D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A2.elt_typ),0),((_tmpC2D[0]=
_tmp4CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2BE;
_LL2C3: _tmp5BB=_tmp5AF.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BB;if(_tmp5BC->tag != 0)goto _LL2C5;};_tmp5BD=_tmp5AF.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5BE=(struct Cyc_Absyn_Upper_b_struct*)_tmp5BD;if(_tmp5BE->tag != 1)goto _LL2C5;
else{_tmp5BF=_tmp5BE->f1;}};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5BF)){const char*
_tmpC30;void*_tmpC2F;(_tmpC2F=0,Cyc_Tcutil_terr(e->loc,((_tmpC30="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC30,sizeof(char),71))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC33;void*_tmpC32;(_tmpC32=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC33="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC33,sizeof(char),45))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E4=_tmp5BF;if(_tmp5AC  && !_tmp5AD)_tmp5E4=Cyc_Absyn_add_exp(
_tmp5BF,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5E5=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC34[3];struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_fncall_exp(
_tmp5E5,((_tmpC34[2]=_tmp5E4,((_tmpC34[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp59F.elt_typ),0),((_tmpC34[0]=_tmp4CD,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC34,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5A9){struct Cyc_List_List*_tmpC35;_tmp5E6->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35->hd=Cyc_Absyn_copy_exp(
_tmp5E6),((_tmpC35->tl=0,_tmpC35)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4CC,_tmp5E6);
goto _LL2BE;};};_LL2C5: _tmp5C0=_tmp5AF.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C0;if(_tmp5C1->tag != 0)goto
_LL2BE;};_tmp5C2=_tmp5AF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C3=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5C2;if(_tmp5C3->tag != 0)goto _LL2BE;};_LL2C6:
DynCast: if((_tmp5AC  && !_tmp5AD) && !(_tmp5A2.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC38="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC38,sizeof(char),70))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5EB=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC39[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5EB,((_tmpC39[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC39[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp59F.elt_typ),0),((
_tmpC39[0]=_tmp4CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2BE;
_LL2BE:;}goto _LL2B7;}_LL2BA: _tmp5A3=_tmp59C.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5A4=(struct Cyc_Absyn_PointerType_struct*)_tmp5A3;if(_tmp5A4->tag != 5)goto
_LL2BC;else{_tmp5A5=_tmp5A4->f1;}};_tmp5A6=_tmp59C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5A7=(struct Cyc_Absyn_IntType_struct*)_tmp5A6;if(_tmp5A7->tag != 6)goto _LL2BC;};
_LL2BB:{void*_tmp5ED=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A5.ptr_atts).bounds);_LL2CD: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5EE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5ED;if(_tmp5EE->tag != 0)goto
_LL2CF;}_LL2CE: _tmp4CD->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4CD->r,
_tmp4CD->loc),Cyc_Toc_curr_sp);goto _LL2CC;_LL2CF:;_LL2D0: goto _LL2CC;_LL2CC:;}
goto _LL2B7;_LL2BC:;_LL2BD: goto _LL2B7;_LL2B7:;}goto _LL210;}_LL235: {struct Cyc_Absyn_Address_e_struct*
_tmp4D0=(struct Cyc_Absyn_Address_e_struct*)_tmp493;if(_tmp4D0->tag != 16)goto
_LL237;else{_tmp4D1=_tmp4D0->f1;}}_LL236:{void*_tmp5EF=_tmp4D1->r;struct _tuple0*
_tmp5F1;struct Cyc_List_List*_tmp5F2;struct Cyc_List_List*_tmp5F3;struct Cyc_List_List*
_tmp5F5;_LL2D2: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5EF;if(_tmp5F0->tag != 29)goto _LL2D4;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;
_tmp5F3=_tmp5F0->f3;}}_LL2D3: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC3D;void*
_tmpC3C[1];struct Cyc_String_pa_struct _tmpC3B;(_tmpC3B.tag=0,((_tmpC3B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D1->loc)),((
_tmpC3C[0]=& _tmpC3B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC3D="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC3D,
sizeof(char),42))),_tag_dyneither(_tmpC3C,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D1->topt))->v,_tmp5F2 != 0,1,0,
_tmp5F3,_tmp5F1))->r;goto _LL2D1;_LL2D4: {struct Cyc_Absyn_Tuple_e_struct*_tmp5F4=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5EF;if(_tmp5F4->tag != 25)goto _LL2D6;else{
_tmp5F5=_tmp5F4->f1;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC41;void*
_tmpC40[1];struct Cyc_String_pa_struct _tmpC3F;(_tmpC3F.tag=0,((_tmpC3F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D1->loc)),((
_tmpC40[0]=& _tmpC3F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC41="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC41,
sizeof(char),42))),_tag_dyneither(_tmpC40,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5F5))->r;goto _LL2D1;_LL2D6:;_LL2D7: Cyc_Toc_exp_to_c(nv,_tmp4D1);if(!
Cyc_Absyn_is_lvalue(_tmp4D1)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4D1,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D1);}goto _LL2D1;
_LL2D1:;}goto _LL210;_LL237: {struct Cyc_Absyn_New_e_struct*_tmp4D2=(struct Cyc_Absyn_New_e_struct*)
_tmp493;if(_tmp4D2->tag != 17)goto _LL239;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}
_LL238: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC45;void*_tmpC44[1];struct Cyc_String_pa_struct
_tmpC43;(_tmpC43.tag=0,((_tmpC43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D4->loc)),((_tmpC44[0]=& _tmpC43,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC45="%s: new at top-level",
_tag_dyneither(_tmpC45,sizeof(char),21))),_tag_dyneither(_tmpC44,sizeof(void*),1)))))));}{
void*_tmp5FF=_tmp4D4->r;struct Cyc_List_List*_tmp601;struct Cyc_Absyn_Vardecl*
_tmp603;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Exp*_tmp605;int _tmp606;
struct _tuple0*_tmp608;struct Cyc_List_List*_tmp609;struct Cyc_List_List*_tmp60A;
struct Cyc_Absyn_Aggrdecl*_tmp60B;struct Cyc_List_List*_tmp60D;_LL2D9: {struct Cyc_Absyn_Array_e_struct*
_tmp600=(struct Cyc_Absyn_Array_e_struct*)_tmp5FF;if(_tmp600->tag != 27)goto _LL2DB;
else{_tmp601=_tmp600->f1;}}_LL2DA: {struct _tuple0*_tmp60E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60F=Cyc_Absyn_var_exp(_tmp60E,0);struct Cyc_Absyn_Stmt*
_tmp610=Cyc_Toc_init_array(nv,_tmp60F,_tmp601,Cyc_Absyn_exp_stmt(_tmp60F,0));
void*old_elt_typ;{void*_tmp611=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp613;void*_tmp614;struct Cyc_Absyn_Tqual _tmp615;struct Cyc_Absyn_PtrAtts _tmp616;
union Cyc_Absyn_Constraint*_tmp617;_LL2E4: {struct Cyc_Absyn_PointerType_struct*
_tmp612=(struct Cyc_Absyn_PointerType_struct*)_tmp611;if(_tmp612->tag != 5)goto
_LL2E6;else{_tmp613=_tmp612->f1;_tmp614=_tmp613.elt_typ;_tmp615=_tmp613.elt_tq;
_tmp616=_tmp613.ptr_atts;_tmp617=_tmp616.zero_term;}}_LL2E5: old_elt_typ=_tmp614;
goto _LL2E3;_LL2E6:;_LL2E7: {const char*_tmpC48;void*_tmpC47;old_elt_typ=((_tmpC47=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC48="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC48,sizeof(char),52))),_tag_dyneither(_tmpC47,sizeof(void*),0))));}_LL2E3:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp61A=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp61B=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp601),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D3 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp61B);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4D3;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp61B);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp60E,_tmp61A,(struct Cyc_Absyn_Exp*)e1,_tmp610,0));goto
_LL2D8;};}_LL2DB: {struct Cyc_Absyn_Comprehension_e_struct*_tmp602=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5FF;if(_tmp602->tag != 28)goto _LL2DD;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;
_tmp605=_tmp602->f3;_tmp606=_tmp602->f4;}}_LL2DC: {int is_dyneither_ptr=0;{void*
_tmp61C=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp61E;void*_tmp61F;
struct Cyc_Absyn_Tqual _tmp620;struct Cyc_Absyn_PtrAtts _tmp621;union Cyc_Absyn_Constraint*
_tmp622;union Cyc_Absyn_Constraint*_tmp623;_LL2E9: {struct Cyc_Absyn_PointerType_struct*
_tmp61D=(struct Cyc_Absyn_PointerType_struct*)_tmp61C;if(_tmp61D->tag != 5)goto
_LL2EB;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61E.elt_typ;_tmp620=_tmp61E.elt_tq;
_tmp621=_tmp61E.ptr_atts;_tmp622=_tmp621.bounds;_tmp623=_tmp621.zero_term;}}
_LL2EA: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp622)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E8;
_LL2EB:;_LL2EC: {const char*_tmpC4B;void*_tmpC4A;(_tmpC4A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4B="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC4B,sizeof(char),42))),_tag_dyneither(_tmpC4A,sizeof(void*),0)));}
_LL2E8:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp605->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp604);{struct Cyc_Absyn_Exp*_tmp626=
Cyc_Absyn_var_exp(max,0);if(_tmp606)_tmp626=Cyc_Absyn_add_exp(_tmp626,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp603,Cyc_Absyn_var_exp(max,0),_tmp605,_tmp606,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp628;struct _RegionHandle*_tmp629;struct Cyc_Toc_Env*_tmp627=
nv;_tmp628=*_tmp627;_tmp629=_tmp628.rgn;{struct _tuple19*_tmpC4E;struct Cyc_List_List*
_tmpC4D;struct Cyc_List_List*decls=(_tmpC4D=_region_malloc(_tmp629,sizeof(*
_tmpC4D)),((_tmpC4D->hd=((_tmpC4E=_region_malloc(_tmp629,sizeof(*_tmpC4E)),((
_tmpC4E->f1=max,((_tmpC4E->f2=Cyc_Absyn_uint_typ,((_tmpC4E->f3=(struct Cyc_Absyn_Exp*)
_tmp604,_tmpC4E)))))))),((_tmpC4D->tl=0,_tmpC4D)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4D3 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC4F[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC4F[1]=_tmp626,((
_tmpC4F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4F,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D3;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC50[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC50[1]=_tmp626,((_tmpC50[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC50,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC53;struct Cyc_List_List*
_tmpC52;decls=((_tmpC52=_region_malloc(_tmp629,sizeof(*_tmpC52)),((_tmpC52->hd=((
_tmpC53=_region_malloc(_tmp629,sizeof(*_tmpC53)),((_tmpC53->f1=a,((_tmpC53->f2=
ptr_typ,((_tmpC53->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC53)))))))),((_tmpC52->tl=
decls,_tmpC52))))));}if(is_dyneither_ptr){struct _tuple0*_tmp62E=Cyc_Toc_temp_var();
void*_tmp62F=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp630=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC54[3];struct Cyc_Absyn_Exp*_tmp631=Cyc_Absyn_fncall_exp(
_tmp630,((_tmpC54[2]=_tmp626,((_tmpC54[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC54[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC57;struct Cyc_List_List*_tmpC56;decls=((_tmpC56=
_region_malloc(_tmp629,sizeof(*_tmpC56)),((_tmpC56->hd=((_tmpC57=_region_malloc(
_tmp629,sizeof(*_tmpC57)),((_tmpC57->f1=_tmp62E,((_tmpC57->f2=_tmp62F,((_tmpC57->f3=(
struct Cyc_Absyn_Exp*)_tmp631,_tmpC57)))))))),((_tmpC56->tl=decls,_tmpC56))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp62E,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp635=decls;for(0;_tmp635 != 0;_tmp635=_tmp635->tl){struct
_tuple0*_tmp637;void*_tmp638;struct Cyc_Absyn_Exp*_tmp639;struct _tuple19 _tmp636=*((
struct _tuple19*)_tmp635->hd);_tmp637=_tmp636.f1;_tmp638=_tmp636.f2;_tmp639=
_tmp636.f3;s=Cyc_Absyn_declare_stmt(_tmp637,_tmp638,_tmp639,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D8;};};};}_LL2DD: {struct Cyc_Absyn_Aggregate_e_struct*_tmp607=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5FF;if(_tmp607->tag != 29)goto _LL2DF;
else{_tmp608=_tmp607->f1;_tmp609=_tmp607->f2;_tmp60A=_tmp607->f3;_tmp60B=_tmp607->f4;}}
_LL2DE: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D4->topt))->v,_tmp609 != 0,1,_tmp4D3,_tmp60A,_tmp608))->r;goto _LL2D8;_LL2DF: {
struct Cyc_Absyn_Tuple_e_struct*_tmp60C=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5FF;
if(_tmp60C->tag != 25)goto _LL2E1;else{_tmp60D=_tmp60C->f1;}}_LL2E0: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4D3,_tmp60D))->r;goto _LL2D8;_LL2E1:;_LL2E2: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp63C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp63D=Cyc_Absyn_var_exp(_tmp63C,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp63D,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4D3
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D3;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp63E=_tmp4D4->r;void*_tmp640;
struct Cyc_Absyn_Exp*_tmp641;_LL2EE: {struct Cyc_Absyn_Cast_e_struct*_tmp63F=(
struct Cyc_Absyn_Cast_e_struct*)_tmp63E;if(_tmp63F->tag != 15)goto _LL2F0;else{
_tmp640=(void*)_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL2EF:{struct _tuple17 _tmpC58;
struct _tuple17 _tmp643=(_tmpC58.f1=Cyc_Tcutil_compress(_tmp640),((_tmpC58.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp641->topt))->v),_tmpC58)));void*
_tmp644;struct Cyc_Absyn_PtrInfo _tmp646;void*_tmp647;struct Cyc_Absyn_PtrAtts
_tmp648;union Cyc_Absyn_Constraint*_tmp649;void*_tmp64A;struct Cyc_Absyn_PtrInfo
_tmp64C;struct Cyc_Absyn_PtrAtts _tmp64D;union Cyc_Absyn_Constraint*_tmp64E;_LL2F3:
_tmp644=_tmp643.f1;{struct Cyc_Absyn_PointerType_struct*_tmp645=(struct Cyc_Absyn_PointerType_struct*)
_tmp644;if(_tmp645->tag != 5)goto _LL2F5;else{_tmp646=_tmp645->f1;_tmp647=_tmp646.elt_typ;
_tmp648=_tmp646.ptr_atts;_tmp649=_tmp648.bounds;}};_tmp64A=_tmp643.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp64B=(struct Cyc_Absyn_PointerType_struct*)_tmp64A;if(_tmp64B->tag != 5)goto
_LL2F5;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64C.ptr_atts;_tmp64E=_tmp64D.bounds;}};
_LL2F4:{struct _tuple17 _tmpC59;struct _tuple17 _tmp650=(_tmpC59.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp649),((_tmpC59.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp64E),_tmpC59)));void*_tmp651;void*_tmp653;struct Cyc_Absyn_Exp*
_tmp655;_LL2F8: _tmp651=_tmp650.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp652=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp651;if(_tmp652->tag != 0)goto _LL2FA;};
_tmp653=_tmp650.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp654=(struct Cyc_Absyn_Upper_b_struct*)
_tmp653;if(_tmp654->tag != 1)goto _LL2FA;else{_tmp655=_tmp654->f1;}};_LL2F9: Cyc_Toc_exp_to_c(
nv,_tmp641);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp656=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC5A[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp656,((_tmpC5A[3]=_tmp655,((_tmpC5A[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp647),0),((_tmpC5A[1]=_tmp641,((_tmpC5A[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F7;};_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}goto _LL2F2;_LL2F5:;
_LL2F6: goto _LL2F2;_LL2F2:;}goto _LL2ED;_LL2F0:;_LL2F1: goto _LL2ED;_LL2ED:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp658=Cyc_Absyn_exp_stmt(_tmp63D,0);struct Cyc_Absyn_Exp*
_tmp659=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4D4);_tmp658=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp63D,_tmp659,0),_tmp4D4,0),
_tmp658,0);{void*_tmp65A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63C,_tmp65A,(struct Cyc_Absyn_Exp*)mexp,_tmp658,0));};}
goto _LL2D8;};}_LL2D8:;}goto _LL210;_LL239: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4D5=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp493;if(_tmp4D5->tag != 19)goto
_LL23B;else{_tmp4D6=_tmp4D5->f1;}}_LL23A: Cyc_Toc_exp_to_c(nv,_tmp4D6);goto _LL210;
_LL23B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4D7=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp493;if(_tmp4D7->tag != 18)goto _LL23D;else{_tmp4D8=(void*)_tmp4D7->f1;}}_LL23C:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC5D;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC5C;e->r=(void*)((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C[0]=((_tmpC5D.tag=
18,((_tmpC5D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4D8),_tmpC5D)))),_tmpC5C))));}
goto _LL210;_LL23D: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4D9=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp493;if(_tmp4D9->tag != 20)goto _LL23F;else{_tmp4DA=(void*)_tmp4D9->f1;_tmp4DB=(
void*)_tmp4D9->f2;{struct Cyc_Absyn_StructField_struct*_tmp4DC=(struct Cyc_Absyn_StructField_struct*)
_tmp4DB;if(_tmp4DC->tag != 0)goto _LL23F;else{_tmp4DD=_tmp4DC->f1;}};}}_LL23E:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC67;struct Cyc_Absyn_StructField_struct
_tmpC66;struct Cyc_Absyn_StructField_struct*_tmpC65;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC64;e->r=(void*)((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC67.tag=
20,((_tmpC67.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DA),((_tmpC67.f2=(void*)((
void*)((_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65[0]=((_tmpC66.tag=0,((
_tmpC66.f1=_tmp4DD,_tmpC66)))),_tmpC65))))),_tmpC67)))))),_tmpC64))));}goto
_LL210;_LL23F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4DE=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp493;if(_tmp4DE->tag != 20)goto _LL241;else{_tmp4DF=(void*)_tmp4DE->f1;_tmp4E0=(
void*)_tmp4DE->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4E1=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4E0;if(_tmp4E1->tag != 1)goto _LL241;else{_tmp4E2=_tmp4E1->f1;}};}}_LL240:{
void*_tmp661=Cyc_Tcutil_compress(_tmp4DF);struct Cyc_Absyn_AggrInfo _tmp663;union
Cyc_Absyn_AggrInfoU _tmp664;struct Cyc_List_List*_tmp666;_LL2FD: {struct Cyc_Absyn_AggrType_struct*
_tmp662=(struct Cyc_Absyn_AggrType_struct*)_tmp661;if(_tmp662->tag != 12)goto
_LL2FF;else{_tmp663=_tmp662->f1;_tmp664=_tmp663.aggr_info;}}_LL2FE: {struct Cyc_Absyn_Aggrdecl*
_tmp669=Cyc_Absyn_get_known_aggrdecl(_tmp664);if(_tmp669->impl == 0){const char*
_tmpC6A;void*_tmpC69;(_tmpC69=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6A="struct fields must be known",
_tag_dyneither(_tmpC6A,sizeof(char),28))),_tag_dyneither(_tmpC69,sizeof(void*),0)));}
_tmp666=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp669->impl))->fields;goto
_LL300;}_LL2FF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp665=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp661;if(_tmp665->tag != 13)goto _LL301;else{_tmp666=_tmp665->f2;}}_LL300: {
struct Cyc_Absyn_Aggrfield*_tmp66C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp666,(int)_tmp4E2);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC74;struct Cyc_Absyn_StructField_struct _tmpC73;struct Cyc_Absyn_StructField_struct*
_tmpC72;struct Cyc_Absyn_Offsetof_e_struct*_tmpC71;e->r=(void*)((_tmpC71=
_cycalloc(sizeof(*_tmpC71)),((_tmpC71[0]=((_tmpC74.tag=20,((_tmpC74.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC74.f2=(void*)((void*)((_tmpC72=_cycalloc(
sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=0,((_tmpC73.f1=_tmp66C->name,
_tmpC73)))),_tmpC72))))),_tmpC74)))))),_tmpC71))));}goto _LL2FC;}_LL301: {struct
Cyc_Absyn_TupleType_struct*_tmp667=(struct Cyc_Absyn_TupleType_struct*)_tmp661;
if(_tmp667->tag != 11)goto _LL303;}_LL302:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC7E;struct Cyc_Absyn_StructField_struct _tmpC7D;struct Cyc_Absyn_StructField_struct*
_tmpC7C;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7B;e->r=(void*)((_tmpC7B=
_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7E.tag=20,((_tmpC7E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC7E.f2=(void*)((void*)((_tmpC7C=_cycalloc(
sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=0,((_tmpC7D.f1=Cyc_Absyn_fieldname((
int)(_tmp4E2 + 1)),_tmpC7D)))),_tmpC7C))))),_tmpC7E)))))),_tmpC7B))));}goto _LL2FC;
_LL303: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp668=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp661;if(_tmp668->tag != 4)goto _LL305;}_LL304: if(_tmp4E2 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC88;struct Cyc_Absyn_StructField_struct _tmpC87;struct Cyc_Absyn_StructField_struct*
_tmpC86;struct Cyc_Absyn_Offsetof_e_struct*_tmpC85;e->r=(void*)((_tmpC85=
_cycalloc(sizeof(*_tmpC85)),((_tmpC85[0]=((_tmpC88.tag=20,((_tmpC88.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC88.f2=(void*)((void*)((_tmpC86=_cycalloc(
sizeof(*_tmpC86)),((_tmpC86[0]=((_tmpC87.tag=0,((_tmpC87.f1=Cyc_Toc_tag_sp,
_tmpC87)))),_tmpC86))))),_tmpC88)))))),_tmpC85))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC92;struct Cyc_Absyn_StructField_struct _tmpC91;struct Cyc_Absyn_StructField_struct*
_tmpC90;struct Cyc_Absyn_Offsetof_e_struct*_tmpC8F;e->r=(void*)((_tmpC8F=
_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC92.tag=20,((_tmpC92.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC92.f2=(void*)((void*)((_tmpC90=_cycalloc(
sizeof(*_tmpC90)),((_tmpC90[0]=((_tmpC91.tag=0,((_tmpC91.f1=Cyc_Absyn_fieldname((
int)_tmp4E2),_tmpC91)))),_tmpC90))))),_tmpC92)))))),_tmpC8F))));}goto _LL2FC;
_LL305:;_LL306: {const char*_tmpC95;void*_tmpC94;(_tmpC94=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC95="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC95,sizeof(char),41))),_tag_dyneither(_tmpC94,sizeof(void*),0)));}
_LL2FC:;}goto _LL210;_LL241: {struct Cyc_Absyn_Deref_e_struct*_tmp4E3=(struct Cyc_Absyn_Deref_e_struct*)
_tmp493;if(_tmp4E3->tag != 21)goto _LL243;else{_tmp4E4=_tmp4E3->f1;}}_LL242: {void*
_tmp67F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E4->topt))->v);{
void*_tmp680=_tmp67F;struct Cyc_Absyn_PtrInfo _tmp682;void*_tmp683;struct Cyc_Absyn_Tqual
_tmp684;struct Cyc_Absyn_PtrAtts _tmp685;void*_tmp686;union Cyc_Absyn_Constraint*
_tmp687;union Cyc_Absyn_Constraint*_tmp688;union Cyc_Absyn_Constraint*_tmp689;
_LL308: {struct Cyc_Absyn_PointerType_struct*_tmp681=(struct Cyc_Absyn_PointerType_struct*)
_tmp680;if(_tmp681->tag != 5)goto _LL30A;else{_tmp682=_tmp681->f1;_tmp683=_tmp682.elt_typ;
_tmp684=_tmp682.elt_tq;_tmp685=_tmp682.ptr_atts;_tmp686=_tmp685.rgn;_tmp687=
_tmp685.nullable;_tmp688=_tmp685.bounds;_tmp689=_tmp685.zero_term;}}_LL309:{void*
_tmp68A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp688);struct Cyc_Absyn_Exp*_tmp68C;_LL30D: {struct Cyc_Absyn_Upper_b_struct*
_tmp68B=(struct Cyc_Absyn_Upper_b_struct*)_tmp68A;if(_tmp68B->tag != 1)goto _LL30F;
else{_tmp68C=_tmp68B->f1;}}_LL30E: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4E4);Cyc_Toc_exp_to_c(nv,_tmp4E4);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,Cyc_Tcutil_warn(e->loc,((_tmpC98="inserted null check due to dereference",
_tag_dyneither(_tmpC98,sizeof(char),39))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC99;_tmp4E4->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp67F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC99=_cycalloc(sizeof(*
_tmpC99)),((_tmpC99->hd=Cyc_Absyn_copy_exp(_tmp4E4),((_tmpC99->tl=0,_tmpC99)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp689)){unsigned int _tmp692;int _tmp693;struct _tuple10 _tmp691=Cyc_Evexp_eval_const_uint_exp(
_tmp68C);_tmp692=_tmp691.f1;_tmp693=_tmp691.f2;if(!_tmp693  || _tmp692 <= 0){const
char*_tmpC9C;void*_tmpC9B;(_tmpC9B=0,Cyc_Tcutil_terr(e->loc,((_tmpC9C="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC9C,sizeof(char),42))),_tag_dyneither(_tmpC9B,sizeof(void*),0)));}}
goto _LL30C;}_LL30F: {struct Cyc_Absyn_DynEither_b_struct*_tmp68D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp68A;if(_tmp68D->tag != 0)goto _LL30C;}_LL310: {struct Cyc_Absyn_Exp*_tmp696=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC9D;_tmp696->topt=((_tmpC9D=_cycalloc(sizeof(*
_tmpC9D)),((_tmpC9D->v=Cyc_Absyn_uint_typ,_tmpC9D))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4E4,_tmp696);Cyc_Toc_exp_to_c(nv,e);goto _LL30C;}_LL30C:;}goto _LL307;_LL30A:;
_LL30B: {const char*_tmpCA0;void*_tmpC9F;(_tmpC9F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA0="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCA0,sizeof(char),29))),_tag_dyneither(_tmpC9F,sizeof(void*),0)));}
_LL307:;}goto _LL210;}_LL243: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4E5=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp493;if(_tmp4E5->tag != 22)goto _LL245;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL244: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4E6->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4E6);if(
_tmp4E8  && _tmp4E9)e->r=Cyc_Toc_check_tagged_union(_tmp4E6,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4E7,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;}_LL245: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4EA=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp493;if(_tmp4EA->tag != 23)goto _LL247;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL246: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4EB->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4EB);Cyc_Toc_exp_to_c(
nv,_tmp4EB);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp69B;struct Cyc_Absyn_Tqual
_tmp69C;struct Cyc_Absyn_PtrAtts _tmp69D;void*_tmp69E;union Cyc_Absyn_Constraint*
_tmp69F;union Cyc_Absyn_Constraint*_tmp6A0;union Cyc_Absyn_Constraint*_tmp6A1;
struct Cyc_Absyn_PtrInfo _tmp69A=Cyc_Toc_get_ptr_type(e1typ);_tmp69B=_tmp69A.elt_typ;
_tmp69C=_tmp69A.elt_tq;_tmp69D=_tmp69A.ptr_atts;_tmp69E=_tmp69D.rgn;_tmp69F=
_tmp69D.nullable;_tmp6A0=_tmp69D.bounds;_tmp6A1=_tmp69D.zero_term;{void*_tmp6A2=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6A0);struct Cyc_Absyn_Exp*_tmp6A4;_LL312: {struct Cyc_Absyn_Upper_b_struct*
_tmp6A3=(struct Cyc_Absyn_Upper_b_struct*)_tmp6A2;if(_tmp6A3->tag != 1)goto _LL314;
else{_tmp6A4=_tmp6A3->f1;}}_LL313: {unsigned int _tmp6A7;int _tmp6A8;struct
_tuple10 _tmp6A6=Cyc_Evexp_eval_const_uint_exp(_tmp6A4);_tmp6A7=_tmp6A6.f1;
_tmp6A8=_tmp6A6.f2;if(_tmp6A8){if(_tmp6A7 < 1){const char*_tmpCA3;void*_tmpCA2;(
_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCA3="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCA3,
sizeof(char),44))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCA6;void*_tmpCA5;(_tmpCA5=0,Cyc_Tcutil_warn(
e->loc,((_tmpCA6="inserted null check due to dereference",_tag_dyneither(_tmpCA6,
sizeof(char),39))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCA7[1];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCA7[0]=Cyc_Absyn_new_exp(_tmp4EB->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6A4)){const char*_tmpCAA;void*
_tmpCA9;(_tmpCA9=0,Cyc_Tcutil_terr(e->loc,((_tmpCAA="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCAA,sizeof(char),47))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCAB[4];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCAB[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCAB[2]=Cyc_Absyn_sizeoftyp_exp(_tmp69B,0),((
_tmpCAB[1]=_tmp6A4,((_tmpCAB[0]=Cyc_Absyn_new_exp(_tmp4EB->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL311;}_LL314: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6A5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6A2;if(_tmp6A5->tag != 0)goto
_LL311;}_LL315: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp69B);{struct Cyc_Absyn_Exp*
_tmpCAC[3];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp69C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCAC[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCAC[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCAC[0]=Cyc_Absyn_new_exp(_tmp4EB->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL311;}_LL311:;}if(
_tmp4ED  && _tmp4EE)e->r=Cyc_Toc_check_tagged_union(_tmp4EB,Cyc_Toc_typ_to_c(
e1typ),_tmp69B,_tmp4EC,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4EE)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;};}_LL247: {struct Cyc_Absyn_Subscript_e_struct*_tmp4EF=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp493;if(_tmp4EF->tag != 24)goto _LL249;
else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}_LL248: {void*_tmp6B2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v);{void*_tmp6B3=
_tmp6B2;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_PtrInfo _tmp6B7;void*_tmp6B8;
struct Cyc_Absyn_Tqual _tmp6B9;struct Cyc_Absyn_PtrAtts _tmp6BA;void*_tmp6BB;union
Cyc_Absyn_Constraint*_tmp6BC;union Cyc_Absyn_Constraint*_tmp6BD;union Cyc_Absyn_Constraint*
_tmp6BE;_LL317: {struct Cyc_Absyn_TupleType_struct*_tmp6B4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B3;if(_tmp6B4->tag != 11)goto _LL319;else{_tmp6B5=_tmp6B4->f1;}}_LL318: Cyc_Toc_exp_to_c(
nv,_tmp4F0);Cyc_Toc_exp_to_c(nv,_tmp4F1);{unsigned int _tmp6C0;int _tmp6C1;struct
_tuple10 _tmp6BF=Cyc_Evexp_eval_const_uint_exp(_tmp4F1);_tmp6C0=_tmp6BF.f1;
_tmp6C1=_tmp6BF.f2;if(!_tmp6C1){const char*_tmpCAF;void*_tmpCAE;(_tmpCAE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAF="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCAF,sizeof(char),44))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F0,Cyc_Absyn_fieldname((int)(_tmp6C0 + 1)));
goto _LL316;};_LL319: {struct Cyc_Absyn_PointerType_struct*_tmp6B6=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B3;if(_tmp6B6->tag != 5)goto _LL31B;else{_tmp6B7=_tmp6B6->f1;_tmp6B8=_tmp6B7.elt_typ;
_tmp6B9=_tmp6B7.elt_tq;_tmp6BA=_tmp6B7.ptr_atts;_tmp6BB=_tmp6BA.rgn;_tmp6BC=
_tmp6BA.nullable;_tmp6BD=_tmp6BA.bounds;_tmp6BE=_tmp6BA.zero_term;}}_LL31A: {
struct Cyc_List_List*_tmp6C4=Cyc_Toc_get_relns(_tmp4F0);int in_bnds=0;{void*
_tmp6C5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BD);_LL31E:;_LL31F: in_bnds=Cyc_Toc_check_bounds(_tmp6B2,_tmp6C4,_tmp4F0,
_tmp4F1);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCB3;void*
_tmpCB2[1];struct Cyc_String_pa_struct _tmpCB1;(_tmpCB1.tag=0,((_tmpCB1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB2[0]=&
_tmpCB1,Cyc_Tcutil_warn(e->loc,((_tmpCB3="bounds check necessary for %s",
_tag_dyneither(_tmpCB3,sizeof(char),30))),_tag_dyneither(_tmpCB2,sizeof(void*),1)))))));}
_LL31D:;}Cyc_Toc_exp_to_c(nv,_tmp4F0);Cyc_Toc_exp_to_c(nv,_tmp4F1);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6C9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6BD);struct Cyc_Absyn_Exp*_tmp6CB;_LL321: {struct Cyc_Absyn_Upper_b_struct*
_tmp6CA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6C9;if(_tmp6CA->tag != 1)goto _LL323;
else{_tmp6CB=_tmp6CA->f1;}}_LL322: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6BC);void*ta1=Cyc_Toc_typ_to_c(_tmp6B8);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6B9);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6BE)){if(!Cyc_Evexp_c_can_eval(_tmp6CB)){const char*_tmpCB6;void*_tmpCB5;(
_tmpCB5=0,Cyc_Tcutil_terr(e->loc,((_tmpCB6="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCB6,sizeof(char),40))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4F0);struct Cyc_Absyn_Exp*_tmpCB7[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCB7[2]=_tmp4F1,((_tmpCB7[1]=_tmp6CB,((
_tmpCB7[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6CB)){const char*_tmpCBA;void*_tmpCB9;(
_tmpCB9=0,Cyc_Tcutil_terr(e->loc,((_tmpCBA="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCBA,sizeof(char),40))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,Cyc_Tcutil_warn(
e->loc,((_tmpCBD="inserted null check due to dereference",_tag_dyneither(_tmpCBD,
sizeof(char),39))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCBE[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCBE[3]=_tmp4F1,((_tmpCBE[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCBE[1]=
_tmp6CB,((_tmpCBE[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCBE,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6CB)){const char*_tmpCC1;void*_tmpCC0;(_tmpCC0=0,Cyc_Tcutil_terr(
e->loc,((_tmpCC1="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCC1,sizeof(char),40))),_tag_dyneither(_tmpCC0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC2[2];_tmp4F1->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCC2[1]=Cyc_Absyn_copy_exp(_tmp4F1),((_tmpCC2[0]=_tmp6CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC2,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL320;}_LL323: {struct Cyc_Absyn_DynEither_b_struct*_tmp6CC=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6C9;if(_tmp6CC->tag != 0)goto _LL320;}
_LL324: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6B8);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B9),
Cyc_Absyn_aggrmember_exp(_tmp4F0,Cyc_Toc_curr_sp,0)),_tmp4F1);}else{struct Cyc_Absyn_Exp*
_tmp6D8=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCC3[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B9),Cyc_Absyn_fncall_exp(
_tmp6D8,((_tmpCC3[2]=_tmp4F1,((_tmpCC3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCC3[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL320;}
_LL320:;}goto _LL316;}_LL31B:;_LL31C: {const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC6="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCC6,sizeof(char),49))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}_LL316:;}
goto _LL210;}_LL249: {struct Cyc_Absyn_Tuple_e_struct*_tmp4F2=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp493;if(_tmp4F2->tag != 25)goto _LL24B;else{_tmp4F3=_tmp4F2->f1;}}_LL24A: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F3))->r;else{struct
Cyc_List_List*_tmp6DC=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F3);void*_tmp6DD=Cyc_Toc_add_tuple_type(
_tmp6DC);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F3 != 0;(_tmp4F3=_tmp4F3->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);{struct _tuple15*
_tmpCC9;struct Cyc_List_List*_tmpCC8;dles=((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((
_tmpCC8->hd=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->f1=0,((_tmpCC9->f2=(
struct Cyc_Absyn_Exp*)_tmp4F3->hd,_tmpCC9)))))),((_tmpCC8->tl=dles,_tmpCC8))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL210;_LL24B: {struct Cyc_Absyn_Array_e_struct*
_tmp4F4=(struct Cyc_Absyn_Array_e_struct*)_tmp493;if(_tmp4F4->tag != 27)goto _LL24D;
else{_tmp4F5=_tmp4F4->f1;}}_LL24C: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F5);{
struct Cyc_List_List*_tmp6E0=_tmp4F5;for(0;_tmp6E0 != 0;_tmp6E0=_tmp6E0->tl){
struct _tuple15 _tmp6E2;struct Cyc_Absyn_Exp*_tmp6E3;struct _tuple15*_tmp6E1=(struct
_tuple15*)_tmp6E0->hd;_tmp6E2=*_tmp6E1;_tmp6E3=_tmp6E2.f2;Cyc_Toc_exp_to_c(nv,
_tmp6E3);}}goto _LL210;_LL24D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4F6=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp493;if(_tmp4F6->tag != 28)goto _LL24F;
else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_tmp4F9=_tmp4F6->f3;_tmp4FA=_tmp4F6->f4;}}
_LL24E: {unsigned int _tmp6E5;int _tmp6E6;struct _tuple10 _tmp6E4=Cyc_Evexp_eval_const_uint_exp(
_tmp4F8);_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{void*_tmp6E7=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F9->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4F9);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4F9)){if(!_tmp6E6){
const char*_tmpCCC;void*_tmpCCB;(_tmpCCB=0,Cyc_Tcutil_terr(_tmp4F8->loc,((_tmpCCC="cannot determine value of constant",
_tag_dyneither(_tmpCCC,sizeof(char),35))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6E5;++ i){struct _tuple15*_tmpCCF;struct Cyc_List_List*
_tmpCCE;es=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->hd=((_tmpCCF=
_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=0,((_tmpCCF->f2=_tmp4F9,_tmpCCF)))))),((
_tmpCCE->tl=es,_tmpCCE))))));}}if(_tmp4FA){struct Cyc_Absyn_Exp*_tmp6EC=Cyc_Toc_cast_it(
_tmp6E7,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCD2;struct Cyc_List_List*
_tmpCD1;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=((
_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->f1=0,((_tmpCD2->f2=_tmp6EC,
_tmpCD2)))))),((_tmpCD1->tl=0,_tmpCD1)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL210;};};}_LL24F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4FB=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp493;if(_tmp4FB->tag != 29)goto _LL251;
else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;_tmp4FE=_tmp4FB->f3;_tmp4FF=_tmp4FB->f4;}}
_LL250: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4FD
!= 0,0,0,_tmp4FE,_tmp4FC))->r;else{if(_tmp4FF == 0){const char*_tmpCD5;void*
_tmpCD4;(_tmpCD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCD5="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCD5,sizeof(
char),38))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4FF;struct Cyc_Toc_Env _tmp6F2;struct
_RegionHandle*_tmp6F3;struct Cyc_Toc_Env*_tmp6F1=nv;_tmp6F2=*_tmp6F1;_tmp6F3=
_tmp6F2.rgn;{struct Cyc_List_List*_tmp6F4=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6F3,e->loc,_tmp4FE,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FF->impl))->tagged){struct
_tuple20 _tmp6F6;struct Cyc_Absyn_Aggrfield*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F8;
struct _tuple20*_tmp6F5=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6F4))->hd;_tmp6F6=*_tmp6F5;_tmp6F7=_tmp6F6.f1;_tmp6F8=_tmp6F6.f2;{void*
_tmp6F9=_tmp6F7->type;Cyc_Toc_exp_to_c(nv,_tmp6F8);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6F9))_tmp6F8->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6F8->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4FF,_tmp6F7->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCDA;struct _tuple15*_tmpCD9;struct _tuple15*_tmpCD8[2];
struct Cyc_List_List*_tmp6FA=(_tmpCD8[1]=((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((
_tmpCD9->f1=0,((_tmpCD9->f2=_tmp6F8,_tmpCD9)))))),((_tmpCD8[0]=((_tmpCDA=
_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->f1=0,((_tmpCDA->f2=field_tag_exp,_tmpCDA)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCD8,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6FA,0);struct Cyc_Absyn_FieldName_struct*_tmpCE0;struct Cyc_Absyn_FieldName_struct
_tmpCDF;void*_tmpCDE[1];struct Cyc_List_List*ds=(_tmpCDE[0]=(void*)((_tmpCE0=
_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCDF.tag=1,((_tmpCDF.f1=_tmp6F7->name,
_tmpCDF)))),_tmpCE0)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCDE,sizeof(void*),1)));struct _tuple15*_tmpCE3;struct _tuple15*
_tmpCE2[1];struct Cyc_List_List*dles=(_tmpCE2[0]=((_tmpCE3=_cycalloc(sizeof(*
_tmpCE3)),((_tmpCE3->f1=ds,((_tmpCE3->f2=umem,_tmpCE3)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE2,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp703=0;struct Cyc_List_List*_tmp704=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp704 != 0;_tmp704=_tmp704->tl){struct Cyc_List_List*
_tmp705=_tmp6F4;for(0;_tmp705 != 0;_tmp705=_tmp705->tl){if((*((struct _tuple20*)
_tmp705->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp704->hd){struct _tuple20
_tmp707;struct Cyc_Absyn_Aggrfield*_tmp708;struct Cyc_Absyn_Exp*_tmp709;struct
_tuple20*_tmp706=(struct _tuple20*)_tmp705->hd;_tmp707=*_tmp706;_tmp708=_tmp707.f1;
_tmp709=_tmp707.f2;{void*_tmp70A=_tmp708->type;Cyc_Toc_exp_to_c(nv,_tmp709);if(
Cyc_Toc_is_void_star_or_tvar(_tmp70A))_tmp709->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp709->r,0));{struct _tuple15*_tmpCE6;struct Cyc_List_List*
_tmpCE5;_tmp703=((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->hd=((_tmpCE6=
_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->f1=0,((_tmpCE6->f2=_tmp709,_tmpCE6)))))),((
_tmpCE5->tl=_tmp703,_tmpCE5))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp703));}};};}
goto _LL210;_LL251: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp500=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp493;if(_tmp500->tag != 30)goto _LL253;else{_tmp501=(void*)_tmp500->f1;_tmp502=
_tmp500->f2;}}_LL252: {struct Cyc_List_List*fs;{void*_tmp70D=Cyc_Tcutil_compress(
_tmp501);struct Cyc_List_List*_tmp70F;_LL326: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp70E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp70D;if(_tmp70E->tag != 13)goto
_LL328;else{_tmp70F=_tmp70E->f2;}}_LL327: fs=_tmp70F;goto _LL325;_LL328:;_LL329: {
const char*_tmpCEA;void*_tmpCE9[1];struct Cyc_String_pa_struct _tmpCE8;(_tmpCE8.tag=
0,((_tmpCE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp501)),((_tmpCE9[0]=& _tmpCE8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEA="anon struct has type %s",
_tag_dyneither(_tmpCEA,sizeof(char),24))),_tag_dyneither(_tmpCE9,sizeof(void*),1)))))));}
_LL325:;}{struct Cyc_Toc_Env _tmp714;struct _RegionHandle*_tmp715;struct Cyc_Toc_Env*
_tmp713=nv;_tmp714=*_tmp713;_tmp715=_tmp714.rgn;{struct Cyc_List_List*_tmp716=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp715,e->loc,_tmp502,Cyc_Absyn_StructA,fs);for(0;_tmp716 != 0;_tmp716=_tmp716->tl){
struct _tuple20 _tmp718;struct Cyc_Absyn_Aggrfield*_tmp719;struct Cyc_Absyn_Exp*
_tmp71A;struct _tuple20*_tmp717=(struct _tuple20*)_tmp716->hd;_tmp718=*_tmp717;
_tmp719=_tmp718.f1;_tmp71A=_tmp718.f2;{void*_tmp71B=_tmp719->type;Cyc_Toc_exp_to_c(
nv,_tmp71A);if(Cyc_Toc_is_void_star_or_tvar(_tmp71B))_tmp71A->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp71A->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp502);}goto _LL210;};}_LL253: {struct Cyc_Absyn_Datatype_e_struct*_tmp503=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp493;if(_tmp503->tag != 31)goto _LL255;else{
_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;_tmp506=_tmp503->f3;}}_LL254: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp71C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp71D=Cyc_Absyn_var_exp(_tmp71C,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCEB;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp506->name,((_tmpCEB="_struct",_tag_dyneither(_tmpCEB,sizeof(char),8)))));}
tag_exp=_tmp505->is_extensible?Cyc_Absyn_var_exp(_tmp506->name,0): Cyc_Toc_datatype_tag(
_tmp505,_tmp506->name);member_exp=_tmp71D;{struct Cyc_List_List*_tmp71F=_tmp506->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp504 != 0;(
_tmp504=_tmp504->tl,_tmp71F=_tmp71F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp504->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp71F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCEE;struct
Cyc_List_List*_tmpCED;dles=((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->hd=((
_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->f1=0,((_tmpCEE->f2=cur_e,_tmpCEE)))))),((
_tmpCED->tl=dles,_tmpCED))))));};}{struct _tuple15*_tmpCF1;struct Cyc_List_List*
_tmpCF0;dles=((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=((_tmpCF1=
_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=0,((_tmpCF1->f2=tag_exp,_tmpCF1)))))),((
_tmpCF0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCF0))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCF2;struct Cyc_List_List*_tmp724=(_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((
_tmpCF2->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCF2->tl=0,_tmpCF2)))));{int i=1;for(0;_tmp504 != 0;(((_tmp504=
_tmp504->tl,i ++)),_tmp71F=_tmp71F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp504->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp71F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp725=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCF3;_tmp724=((_tmpCF3=_cycalloc(sizeof(*
_tmpCF3)),((_tmpCF3->hd=_tmp725,((_tmpCF3->tl=_tmp724,_tmpCF3))))));};}}{struct
Cyc_Absyn_Stmt*_tmp727=Cyc_Absyn_exp_stmt(_tmp71D,0);struct Cyc_List_List*_tmpCF4;
struct Cyc_Absyn_Stmt*_tmp728=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((
_tmpCF4->hd=_tmp727,((_tmpCF4->tl=_tmp724,_tmpCF4))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp71C,datatype_ctype,0,_tmp728,0));};}goto _LL210;};}
_LL255: {struct Cyc_Absyn_Enum_e_struct*_tmp507=(struct Cyc_Absyn_Enum_e_struct*)
_tmp493;if(_tmp507->tag != 32)goto _LL257;}_LL256: goto _LL258;_LL257: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp508=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp493;if(_tmp508->tag != 33)goto
_LL259;}_LL258: goto _LL210;_LL259: {struct Cyc_Absyn_Malloc_e_struct*_tmp509=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp493;if(_tmp509->tag != 34)goto _LL25B;else{
_tmp50A=_tmp509->f1;_tmp50B=_tmp50A.is_calloc;_tmp50C=_tmp50A.rgn;_tmp50D=
_tmp50A.elt_type;_tmp50E=_tmp50A.num_elts;_tmp50F=_tmp50A.fat_result;}}_LL25A: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp50D)));Cyc_Toc_exp_to_c(nv,
_tmp50E);if(_tmp50F){struct _tuple0*_tmp72B=Cyc_Toc_temp_var();struct _tuple0*
_tmp72C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp50B){xexp=_tmp50E;if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp72B,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp50D,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp72B,0));}{struct Cyc_Absyn_Exp*_tmpCF5[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCF5[2]=Cyc_Absyn_var_exp(_tmp72B,0),((_tmpCF5[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCF5[0]=Cyc_Absyn_var_exp(_tmp72C,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp50E,0);if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp72B,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp50D,Cyc_Absyn_var_exp(_tmp72B,0));}{struct Cyc_Absyn_Exp*_tmpCF6[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCF6[2]=Cyc_Absyn_var_exp(_tmp72B,0),((_tmpCF6[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCF6[0]=Cyc_Absyn_var_exp(_tmp72C,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp72F=Cyc_Absyn_declare_stmt(_tmp72B,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp72C,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp72F);};}else{struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp731=_tmp50E->r;union Cyc_Absyn_Cnst _tmp733;struct _tuple5 _tmp734;
int _tmp735;_LL32B: {struct Cyc_Absyn_Const_e_struct*_tmp732=(struct Cyc_Absyn_Const_e_struct*)
_tmp731;if(_tmp732->tag != 0)goto _LL32D;else{_tmp733=_tmp732->f1;if((_tmp733.Int_c).tag
!= 4)goto _LL32D;_tmp734=(struct _tuple5)(_tmp733.Int_c).val;_tmp735=_tmp734.f2;
if(_tmp735 != 1)goto _LL32D;}}_LL32C: goto _LL32A;_LL32D:;_LL32E: _tmp730=Cyc_Absyn_times_exp(
_tmp730,_tmp50E,0);goto _LL32A;_LL32A:;}if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp730))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp50D,
_tmp730))->r;}}goto _LL210;}_LL25B: {struct Cyc_Absyn_Swap_e_struct*_tmp510=(
struct Cyc_Absyn_Swap_e_struct*)_tmp493;if(_tmp510->tag != 35)goto _LL25D;else{
_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;}}_LL25C: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp511->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp512->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF9="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCF9,sizeof(char),57))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp511);Cyc_Toc_exp_to_c(
nv,_tmp512);{struct Cyc_Absyn_Exp*_tmpCFA[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCFA[1]=Cyc_Toc_push_address_exp(_tmp512),((_tmpCFA[0]=Cyc_Toc_push_address_exp(
_tmp511),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCFA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL210;};}
_LL25D: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp513=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp493;if(_tmp513->tag != 38)goto _LL25F;else{_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;}}
_LL25E: {void*_tmp739=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp514->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp514);{void*_tmp73A=
_tmp739;struct Cyc_Absyn_AggrInfo _tmp73C;union Cyc_Absyn_AggrInfoU _tmp73D;struct
Cyc_Absyn_Aggrdecl**_tmp73E;struct Cyc_Absyn_Aggrdecl*_tmp73F;_LL330: {struct Cyc_Absyn_AggrType_struct*
_tmp73B=(struct Cyc_Absyn_AggrType_struct*)_tmp73A;if(_tmp73B->tag != 12)goto
_LL332;else{_tmp73C=_tmp73B->f1;_tmp73D=_tmp73C.aggr_info;if((_tmp73D.KnownAggr).tag
!= 2)goto _LL332;_tmp73E=(struct Cyc_Absyn_Aggrdecl**)(_tmp73D.KnownAggr).val;
_tmp73F=*_tmp73E;}}_LL331: {struct Cyc_Absyn_Exp*_tmp740=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp73F,_tmp515),0);struct Cyc_Absyn_Exp*_tmp741=Cyc_Absyn_aggrmember_exp(
_tmp514,_tmp515,0);struct Cyc_Absyn_Exp*_tmp742=Cyc_Absyn_aggrmember_exp(_tmp741,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp742,_tmp740,0))->r;goto _LL32F;}
_LL332:;_LL333: {const char*_tmpCFD;void*_tmpCFC;(_tmpCFC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFD="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCFD,sizeof(char),31))),_tag_dyneither(_tmpCFC,sizeof(void*),0)));}
_LL32F:;}goto _LL210;}_LL25F: {struct Cyc_Absyn_StmtExp_e_struct*_tmp516=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp493;if(_tmp516->tag != 37)goto _LL261;else{_tmp517=
_tmp516->f1;}}_LL260: Cyc_Toc_stmt_to_c(nv,_tmp517);goto _LL210;_LL261: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp518=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp493;if(_tmp518->tag != 36)
goto _LL263;}_LL262: {const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD00="UnresolvedMem",
_tag_dyneither(_tmpD00,sizeof(char),14))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}
_LL263: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp519=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp493;if(_tmp519->tag != 26)goto _LL265;}_LL264: {const char*_tmpD03;void*_tmpD02;(
_tmpD02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD03="compoundlit",_tag_dyneither(_tmpD03,sizeof(char),12))),_tag_dyneither(
_tmpD02,sizeof(void*),0)));}_LL265: {struct Cyc_Absyn_Valueof_e_struct*_tmp51A=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp493;if(_tmp51A->tag != 39)goto _LL210;}
_LL266: {const char*_tmpD06;void*_tmpD05;(_tmpD05=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD06="valueof(-)",
_tag_dyneither(_tmpD06,sizeof(char),11))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
_LL210:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
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
struct Cyc_Absyn_Stmt*s;{void*_tmp74B=p->r;struct Cyc_Absyn_Vardecl*_tmp74D;struct
Cyc_Absyn_Vardecl _tmp74E;struct _tuple0*_tmp74F;struct Cyc_Absyn_Pat*_tmp750;
struct Cyc_Absyn_Vardecl*_tmp752;struct Cyc_Absyn_Vardecl _tmp753;struct _tuple0*
_tmp754;struct Cyc_Absyn_Vardecl*_tmp757;struct Cyc_Absyn_Pat*_tmp758;enum Cyc_Absyn_Sign
_tmp75B;int _tmp75C;char _tmp75E;struct _dyneither_ptr _tmp760;struct Cyc_Absyn_Enumdecl*
_tmp762;struct Cyc_Absyn_Enumfield*_tmp763;void*_tmp765;struct Cyc_Absyn_Enumfield*
_tmp766;struct Cyc_Absyn_Pat*_tmp768;struct Cyc_Absyn_Pat _tmp769;void*_tmp76A;
struct Cyc_Absyn_Datatypedecl*_tmp76C;struct Cyc_Absyn_Datatypefield*_tmp76D;
struct Cyc_List_List*_tmp76E;struct Cyc_List_List*_tmp770;struct Cyc_List_List*
_tmp772;struct Cyc_Absyn_AggrInfo*_tmp774;struct Cyc_Absyn_AggrInfo*_tmp776;struct
Cyc_Absyn_AggrInfo _tmp777;union Cyc_Absyn_AggrInfoU _tmp778;struct Cyc_List_List*
_tmp779;struct Cyc_Absyn_Pat*_tmp77B;_LL335: {struct Cyc_Absyn_Var_p_struct*
_tmp74C=(struct Cyc_Absyn_Var_p_struct*)_tmp74B;if(_tmp74C->tag != 1)goto _LL337;
else{_tmp74D=_tmp74C->f1;_tmp74E=*_tmp74D;_tmp74F=_tmp74E.name;_tmp750=_tmp74C->f2;}}
_LL336: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp74F,r),rgn,t,r,path,
_tmp750,fail_stmt,decls);_LL337: {struct Cyc_Absyn_TagInt_p_struct*_tmp751=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp74B;if(_tmp751->tag != 3)goto _LL339;else{
_tmp752=_tmp751->f2;_tmp753=*_tmp752;_tmp754=_tmp753.name;}}_LL338: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp754,r);goto _LL33A;_LL339: {struct Cyc_Absyn_Wild_p_struct*_tmp755=(
struct Cyc_Absyn_Wild_p_struct*)_tmp74B;if(_tmp755->tag != 0)goto _LL33B;}_LL33A: s=
Cyc_Toc_skip_stmt_dl();goto _LL334;_LL33B: {struct Cyc_Absyn_Reference_p_struct*
_tmp756=(struct Cyc_Absyn_Reference_p_struct*)_tmp74B;if(_tmp756->tag != 2)goto
_LL33D;else{_tmp757=_tmp756->f1;_tmp758=_tmp756->f2;}}_LL33C: {struct _tuple0*
_tmp77F=Cyc_Toc_temp_var();{struct _tuple22*_tmpD09;struct Cyc_List_List*_tmpD08;
decls=((_tmpD08=_region_malloc(rgn,sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=
_region_malloc(rgn,sizeof(*_tmpD09)),((_tmpD09->f1=_tmp77F,((_tmpD09->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD09)))))),((_tmpD08->tl=decls,_tmpD08))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp757->name,Cyc_Absyn_var_exp(_tmp77F,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77F,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp782=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp758,fail_stmt,decls);_tmp782.f3=Cyc_Absyn_seq_stmt(s,_tmp782.f3,
0);return _tmp782;};}_LL33D: {struct Cyc_Absyn_Null_p_struct*_tmp759=(struct Cyc_Absyn_Null_p_struct*)
_tmp74B;if(_tmp759->tag != 8)goto _LL33F;}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL334;_LL33F: {struct Cyc_Absyn_Int_p_struct*_tmp75A=(struct
Cyc_Absyn_Int_p_struct*)_tmp74B;if(_tmp75A->tag != 9)goto _LL341;else{_tmp75B=
_tmp75A->f1;_tmp75C=_tmp75A->f2;}}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp75B,_tmp75C,0),fail_stmt);goto _LL334;_LL341: {struct Cyc_Absyn_Char_p_struct*
_tmp75D=(struct Cyc_Absyn_Char_p_struct*)_tmp74B;if(_tmp75D->tag != 10)goto _LL343;
else{_tmp75E=_tmp75D->f1;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp75E,0),fail_stmt);goto _LL334;_LL343: {struct Cyc_Absyn_Float_p_struct*_tmp75F=(
struct Cyc_Absyn_Float_p_struct*)_tmp74B;if(_tmp75F->tag != 11)goto _LL345;else{
_tmp760=_tmp75F->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp760,
0),fail_stmt);goto _LL334;_LL345: {struct Cyc_Absyn_Enum_p_struct*_tmp761=(struct
Cyc_Absyn_Enum_p_struct*)_tmp74B;if(_tmp761->tag != 12)goto _LL347;else{_tmp762=
_tmp761->f1;_tmp763=_tmp761->f2;}}_LL346:{struct Cyc_Absyn_Enum_e_struct _tmpD0C;
struct Cyc_Absyn_Enum_e_struct*_tmpD0B;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B[0]=((_tmpD0C.tag=32,((
_tmpD0C.f1=_tmp763->name,((_tmpD0C.f2=(struct Cyc_Absyn_Enumdecl*)_tmp762,((
_tmpD0C.f3=(struct Cyc_Absyn_Enumfield*)_tmp763,_tmpD0C)))))))),_tmpD0B)))),0),
fail_stmt);}goto _LL334;_LL347: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp764=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp74B;if(_tmp764->tag != 13)goto _LL349;else{
_tmp765=(void*)_tmp764->f1;_tmp766=_tmp764->f2;}}_LL348:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD0F;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD0E;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=33,((
_tmpD0F.f1=_tmp766->name,((_tmpD0F.f2=(void*)_tmp765,((_tmpD0F.f3=(struct Cyc_Absyn_Enumfield*)
_tmp766,_tmpD0F)))))))),_tmpD0E)))),0),fail_stmt);}goto _LL334;_LL349: {struct Cyc_Absyn_Pointer_p_struct*
_tmp767=(struct Cyc_Absyn_Pointer_p_struct*)_tmp74B;if(_tmp767->tag != 5)goto
_LL34B;else{_tmp768=_tmp767->f1;_tmp769=*_tmp768;_tmp76A=_tmp769.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp76B=(struct Cyc_Absyn_Datatype_p_struct*)_tmp76A;if(_tmp76B->tag != 7)goto
_LL34B;else{_tmp76C=_tmp76B->f1;_tmp76D=_tmp76B->f2;_tmp76E=_tmp76B->f3;}};}}
_LL34A: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp787=Cyc_Toc_temp_var();const
char*_tmpD10;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp76D->name,((
_tmpD10="_struct",_tag_dyneither(_tmpD10,sizeof(char),8))));void*_tmp788=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp788,r);struct Cyc_List_List*_tmp789=_tmp76D->typs;for(0;_tmp76E != 0;(((
_tmp76E=_tmp76E->tl,_tmp789=((struct Cyc_List_List*)_check_null(_tmp789))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp78A=(struct Cyc_Absyn_Pat*)_tmp76E->hd;if(_tmp78A->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp78B=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp789))->hd)).f2;struct _tuple0*_tmp78C=Cyc_Toc_temp_var();
void*_tmp78D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp78A->topt))->v;void*
_tmp78E=Cyc_Toc_typ_to_c(_tmp78D);struct Cyc_Absyn_Exp*_tmp78F=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp78B)))_tmp78F=Cyc_Toc_cast_it(_tmp78E,_tmp78F);{struct
_tuple22*_tmpD13;struct Cyc_List_List*_tmpD12;decls=((_tmpD12=_region_malloc(rgn,
sizeof(*_tmpD12)),((_tmpD12->hd=((_tmpD13=_region_malloc(rgn,sizeof(*_tmpD13)),((
_tmpD13->f1=_tmp78C,((_tmpD13->f2=_tmp78E,_tmpD13)))))),((_tmpD12->tl=decls,
_tmpD12))))));}{struct _tuple21 _tmp792=Cyc_Toc_xlate_pat(nv,rgn,_tmp78D,Cyc_Absyn_var_exp(
_tmp78C,0),_tmp78F,_tmp78A,fail_stmt,decls);nv=_tmp792.f1;decls=_tmp792.f2;{
struct Cyc_Absyn_Stmt*_tmp793=_tmp792.f3;struct Cyc_Absyn_Stmt*_tmp794=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp78C,0),_tmp78F,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp794,_tmp793,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp787,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp76C->is_extensible?Cyc_Absyn_var_exp(_tmp76D->name,0): Cyc_Toc_datatype_tag(
_tmp76C,_tmp76D->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp787,_tmp788,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL334;};};_LL34B: {struct Cyc_Absyn_Datatype_p_struct*_tmp76F=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp74B;if(_tmp76F->tag != 7)goto _LL34D;else{
_tmp770=_tmp76F->f3;}}_LL34C: _tmp772=_tmp770;goto _LL34E;_LL34D: {struct Cyc_Absyn_Tuple_p_struct*
_tmp771=(struct Cyc_Absyn_Tuple_p_struct*)_tmp74B;if(_tmp771->tag != 4)goto _LL34F;
else{_tmp772=_tmp771->f1;}}_LL34E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp772 != 0;(_tmp772=_tmp772->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp796=(struct Cyc_Absyn_Pat*)
_tmp772->hd;if(_tmp796->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp797=Cyc_Toc_temp_var();void*_tmp798=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp796->topt))->v;{struct _tuple22*_tmpD16;struct Cyc_List_List*_tmpD15;decls=((
_tmpD15=_region_malloc(rgn,sizeof(*_tmpD15)),((_tmpD15->hd=((_tmpD16=
_region_malloc(rgn,sizeof(*_tmpD16)),((_tmpD16->f1=_tmp797,((_tmpD16->f2=Cyc_Toc_typ_to_c(
_tmp798),_tmpD16)))))),((_tmpD15->tl=decls,_tmpD15))))));}{struct _tuple21 _tmp79B=
Cyc_Toc_xlate_pat(nv,rgn,_tmp798,Cyc_Absyn_var_exp(_tmp797,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp796,fail_stmt,decls);nv=_tmp79B.f1;decls=
_tmp79B.f2;{struct Cyc_Absyn_Stmt*_tmp79C=_tmp79B.f3;struct Cyc_Absyn_Stmt*_tmp79D=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp797,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp79D,
_tmp79C,0),0);};};};}goto _LL334;};_LL34F: {struct Cyc_Absyn_Aggr_p_struct*_tmp773=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp74B;if(_tmp773->tag != 6)goto _LL351;else{
_tmp774=_tmp773->f1;if(_tmp774 != 0)goto _LL351;}}_LL350: {const char*_tmpD19;void*
_tmpD18;(_tmpD18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD19="unresolved aggregate pattern!",_tag_dyneither(_tmpD19,sizeof(char),30))),
_tag_dyneither(_tmpD18,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Aggr_p_struct*
_tmp775=(struct Cyc_Absyn_Aggr_p_struct*)_tmp74B;if(_tmp775->tag != 6)goto _LL353;
else{_tmp776=_tmp775->f1;if(_tmp776 == 0)goto _LL353;_tmp777=*_tmp776;_tmp778=
_tmp777.aggr_info;_tmp779=_tmp775->f3;}}_LL352: {struct Cyc_Absyn_Aggrdecl*
_tmp7A0=Cyc_Absyn_get_known_aggrdecl(_tmp778);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7A0->impl))->tagged){struct _tuple23 _tmp7A2;struct Cyc_List_List*
_tmp7A3;struct Cyc_Absyn_Pat*_tmp7A4;struct _tuple23*_tmp7A1=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp779))->hd;_tmp7A2=*_tmp7A1;_tmp7A3=_tmp7A2.f1;
_tmp7A4=_tmp7A2.f2;{struct _dyneither_ptr*f;{void*_tmp7A5=(void*)((struct Cyc_List_List*)
_check_null(_tmp7A3))->hd;struct _dyneither_ptr*_tmp7A7;_LL35C: {struct Cyc_Absyn_FieldName_struct*
_tmp7A6=(struct Cyc_Absyn_FieldName_struct*)_tmp7A5;if(_tmp7A6->tag != 1)goto
_LL35E;else{_tmp7A7=_tmp7A6->f1;}}_LL35D: f=_tmp7A7;goto _LL35B;_LL35E:;_LL35F: {
const char*_tmpD1C;void*_tmpD1B;(_tmpD1B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1C="no field name in tagged union pattern",
_tag_dyneither(_tmpD1C,sizeof(char),38))),_tag_dyneither(_tmpD1B,sizeof(void*),0)));}
_LL35B:;}{struct _tuple0*_tmp7AA=Cyc_Toc_temp_var();void*_tmp7AB=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7A4->topt))->v;void*_tmp7AC=Cyc_Toc_typ_to_c(
_tmp7AB);{struct _tuple22*_tmpD1F;struct Cyc_List_List*_tmpD1E;decls=((_tmpD1E=
_region_malloc(rgn,sizeof(*_tmpD1E)),((_tmpD1E->hd=((_tmpD1F=_region_malloc(rgn,
sizeof(*_tmpD1F)),((_tmpD1F->f1=_tmp7AA,((_tmpD1F->f2=_tmp7AC,_tmpD1F)))))),((
_tmpD1E->tl=decls,_tmpD1E))))));}{struct Cyc_Absyn_Exp*_tmp7AF=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7B0=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7B0=Cyc_Toc_cast_it(_tmp7AC,_tmp7B0);_tmp7AF=Cyc_Toc_cast_it(_tmp7AC,_tmp7AF);{
struct _tuple21 _tmp7B1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AB,Cyc_Absyn_var_exp(_tmp7AA,
0),_tmp7AF,_tmp7A4,fail_stmt,decls);nv=_tmp7B1.f1;decls=_tmp7B1.f2;{struct Cyc_Absyn_Stmt*
_tmp7B2=_tmp7B1.f3;struct Cyc_Absyn_Stmt*_tmp7B3=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7A0,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7B4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7AA,0),_tmp7B0,0);s=Cyc_Absyn_seq_stmt(_tmp7B3,Cyc_Absyn_seq_stmt(_tmp7B4,
_tmp7B2,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp779 != 0;_tmp779=
_tmp779->tl){struct _tuple23*_tmp7B5=(struct _tuple23*)_tmp779->hd;struct Cyc_Absyn_Pat*
_tmp7B6=(*_tmp7B5).f2;if(_tmp7B6->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7B7=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7B5).f1))->hd;struct _dyneither_ptr*_tmp7B9;_LL361: {struct Cyc_Absyn_FieldName_struct*
_tmp7B8=(struct Cyc_Absyn_FieldName_struct*)_tmp7B7;if(_tmp7B8->tag != 1)goto
_LL363;else{_tmp7B9=_tmp7B8->f1;}}_LL362: f=_tmp7B9;goto _LL360;_LL363:;_LL364: {
struct Cyc_Toc_Match_error_struct _tmpD22;struct Cyc_Toc_Match_error_struct*_tmpD21;(
int)_throw((void*)((_tmpD21=_cycalloc_atomic(sizeof(*_tmpD21)),((_tmpD21[0]=((
_tmpD22.tag=Cyc_Toc_Match_error,_tmpD22)),_tmpD21)))));}_LL360:;}{struct _tuple0*
_tmp7BC=Cyc_Toc_temp_var();void*_tmp7BD=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7B6->topt))->v;void*_tmp7BE=Cyc_Toc_typ_to_c(_tmp7BD);{struct _tuple22*
_tmpD25;struct Cyc_List_List*_tmpD24;decls=((_tmpD24=_region_malloc(rgn,sizeof(*
_tmpD24)),((_tmpD24->hd=((_tmpD25=_region_malloc(rgn,sizeof(*_tmpD25)),((_tmpD25->f1=
_tmp7BC,((_tmpD25->f2=_tmp7BE,_tmpD25)))))),((_tmpD24->tl=decls,_tmpD24))))));}{
struct _tuple21 _tmp7C1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BD,Cyc_Absyn_var_exp(_tmp7BC,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7B6,fail_stmt,decls);nv=_tmp7C1.f1;
decls=_tmp7C1.f2;{struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7A0->impl))->fields,f)))->type))
_tmp7C2=Cyc_Toc_cast_it(_tmp7BE,_tmp7C2);{struct Cyc_Absyn_Stmt*_tmp7C3=_tmp7C1.f3;
struct Cyc_Absyn_Stmt*_tmp7C4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BC,0),
_tmp7C2,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7C4,_tmp7C3,0),0);};};};};};}}
goto _LL334;}_LL353: {struct Cyc_Absyn_Pointer_p_struct*_tmp77A=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp74B;if(_tmp77A->tag != 5)goto _LL355;else{_tmp77B=_tmp77A->f1;}}_LL354: {
struct _tuple0*_tmp7C5=Cyc_Toc_temp_var();void*_tmp7C6=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77B->topt))->v;{struct _tuple22*_tmpD28;struct Cyc_List_List*
_tmpD27;decls=((_tmpD27=_region_malloc(rgn,sizeof(*_tmpD27)),((_tmpD27->hd=((
_tmpD28=_region_malloc(rgn,sizeof(*_tmpD28)),((_tmpD28->f1=_tmp7C5,((_tmpD28->f2=
Cyc_Toc_typ_to_c(_tmp7C6),_tmpD28)))))),((_tmpD27->tl=decls,_tmpD27))))));}{
struct _tuple21 _tmp7C9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C6,Cyc_Absyn_var_exp(_tmp7C5,
0),Cyc_Absyn_deref_exp(path,0),_tmp77B,fail_stmt,decls);nv=_tmp7C9.f1;decls=
_tmp7C9.f2;{struct Cyc_Absyn_Stmt*_tmp7CA=_tmp7C9.f3;struct Cyc_Absyn_Stmt*_tmp7CB=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C5,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7CA,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7CB,0);else{s=_tmp7CB;}goto _LL334;};};}_LL355: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp77C=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp74B;if(_tmp77C->tag != 14)goto
_LL357;}_LL356: {const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2B="unknownid pat",
_tag_dyneither(_tmpD2B,sizeof(char),14))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}
_LL357: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp77D=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp74B;if(_tmp77D->tag != 15)goto _LL359;}_LL358: {const char*_tmpD2E;void*_tmpD2D;(
_tmpD2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD2E="unknowncall pat",_tag_dyneither(_tmpD2E,sizeof(char),16))),
_tag_dyneither(_tmpD2D,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_Exp_p_struct*
_tmp77E=(struct Cyc_Absyn_Exp_p_struct*)_tmp74B;if(_tmp77E->tag != 16)goto _LL334;}
_LL35A: {const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="exp pat",
_tag_dyneither(_tmpD31,sizeof(char),8))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
_LL334:;}{struct _tuple21 _tmpD32;return(_tmpD32.f1=nv,((_tmpD32.f2=decls,((
_tmpD32.f3=s,_tmpD32)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD33;
return(_tmpD33=_region_malloc(r,sizeof(*_tmpD33)),((_tmpD33->f1=Cyc_Toc_fresh_label(),((
_tmpD33->f2=Cyc_Toc_fresh_label(),((_tmpD33->f3=sc,_tmpD33)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7D4=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7D5=Cyc_Tcutil_compress(_tmp7D4);_LL366: {struct Cyc_Absyn_IntType_struct*
_tmp7D6=(struct Cyc_Absyn_IntType_struct*)_tmp7D5;if(_tmp7D6->tag != 6)goto _LL368;}
_LL367: goto _LL369;_LL368: {struct Cyc_Absyn_EnumType_struct*_tmp7D7=(struct Cyc_Absyn_EnumType_struct*)
_tmp7D5;if(_tmp7D7->tag != 14)goto _LL36A;}_LL369: leave_as_switch=1;goto _LL365;
_LL36A:;_LL36B: leave_as_switch=0;goto _LL365;_LL365:;}{struct Cyc_List_List*
_tmp7D8=scs;for(0;_tmp7D8 != 0;_tmp7D8=_tmp7D8->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7D8->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7D8->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7D9=scs;for(0;_tmp7D9 != 0;_tmp7D9=_tmp7D9->tl){struct Cyc_Absyn_Stmt*
_tmp7DA=((struct Cyc_Absyn_Switch_clause*)_tmp7D9->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7D9->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7DA,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7DC;struct _RegionHandle*_tmp7DD;struct Cyc_Toc_Env*_tmp7DB=
nv;_tmp7DC=*_tmp7DB;_tmp7DD=_tmp7DC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7DD,nv,next_l),_tmp7DA);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7DF;
struct _RegionHandle*_tmp7E0;struct Cyc_Toc_Env*_tmp7DE=nv;_tmp7DF=*_tmp7DE;
_tmp7E0=_tmp7DF.rgn;{struct Cyc_Toc_Env*_tmp7E1=Cyc_Toc_share_env(_tmp7E0,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7E0,Cyc_Toc_gen_label,
_tmp7E0,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7E2=lscs;for(0;_tmp7E2 != 0;
_tmp7E2=_tmp7E2->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7E2->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7E2->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7E2->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7E4;struct Cyc_List_List*_tmp7E5;struct Cyc_Absyn_Stmt*_tmp7E6;struct _tuple21
_tmp7E3=Cyc_Toc_xlate_pat(_tmp7E1,_tmp7E0,_tmp7D4,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7E4=_tmp7E3.f1;_tmp7E5=_tmp7E3.f2;_tmp7E6=_tmp7E3.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7E7=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7E4,_tmp7E7);_tmp7E6=Cyc_Absyn_seq_stmt(
_tmp7E6,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7E7,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7E5;{
struct Cyc_List_List*_tmpD34;nvs=((_tmpD34=_region_malloc(_tmp7E0,sizeof(*_tmpD34)),((
_tmpD34->hd=_tmp7E4,((_tmpD34->tl=nvs,_tmpD34))))));}{struct Cyc_List_List*
_tmpD35;test_stmts=((_tmpD35=_region_malloc(_tmp7E0,sizeof(*_tmpD35)),((_tmpD35->hd=
_tmp7E6,((_tmpD35->tl=test_stmts,_tmpD35))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7EB;struct _dyneither_ptr*_tmp7EC;struct _dyneither_ptr*_tmp7ED;
struct Cyc_Absyn_Switch_clause*_tmp7EE;struct _tuple24*_tmp7EA=(struct _tuple24*)
lscs->hd;_tmp7EB=*_tmp7EA;_tmp7EC=_tmp7EB.f1;_tmp7ED=_tmp7EB.f2;_tmp7EE=_tmp7EB.f3;{
struct Cyc_Toc_Env*_tmp7EF=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7EE->body;struct Cyc_Toc_Env _tmp7F1;struct
_RegionHandle*_tmp7F2;struct Cyc_Toc_Env*_tmp7F0=_tmp7E1;_tmp7F1=*_tmp7F0;_tmp7F2=
_tmp7F1.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7F4;struct _dyneither_ptr*_tmp7F5;
struct Cyc_Absyn_Switch_clause*_tmp7F6;struct _tuple24*_tmp7F3=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7F4=*_tmp7F3;_tmp7F5=_tmp7F4.f2;
_tmp7F6=_tmp7F4.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7F2,
_tmp7EF,end_l,_tmp7F5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F6->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7F2,
_tmp7EF,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7EC,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7ED,s,0),0);{struct Cyc_List_List*_tmpD36;stmts=((_tmpD36=_region_malloc(
_tmp7E0,sizeof(*_tmpD36)),((_tmpD36->hd=s,((_tmpD36->tl=stmts,_tmpD36))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7F9;struct _tuple0*_tmp7FA;void*_tmp7FB;struct _tuple22*_tmp7F8=(struct
_tuple22*)decls->hd;_tmp7F9=*_tmp7F8;_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F9.f2;res=
Cyc_Absyn_declare_stmt(_tmp7FA,_tmp7FB,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD37;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((
_tmpD37->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD37->tl=0,_tmpD37)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7FD=s->r;struct Cyc_Absyn_Exp*_tmp800;struct
Cyc_Absyn_Stmt*_tmp802;struct Cyc_Absyn_Stmt*_tmp803;struct Cyc_Absyn_Exp*_tmp805;
struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*_tmp808;struct Cyc_Absyn_Stmt*
_tmp809;struct _tuple8 _tmp80B;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Stmt*
_tmp80D;struct Cyc_Absyn_Stmt*_tmp80F;struct Cyc_Absyn_Stmt*_tmp811;struct Cyc_Absyn_Stmt*
_tmp813;struct Cyc_Absyn_Exp*_tmp815;struct _tuple8 _tmp816;struct Cyc_Absyn_Exp*
_tmp817;struct _tuple8 _tmp818;struct Cyc_Absyn_Exp*_tmp819;struct Cyc_Absyn_Stmt*
_tmp81A;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_List_List*
_tmp81F;struct Cyc_Absyn_Switch_clause**_tmp820;struct Cyc_Absyn_Decl*_tmp822;
struct Cyc_Absyn_Stmt*_tmp823;struct _dyneither_ptr*_tmp825;struct Cyc_Absyn_Stmt*
_tmp826;struct Cyc_Absyn_Stmt*_tmp828;struct _tuple8 _tmp829;struct Cyc_Absyn_Exp*
_tmp82A;struct Cyc_Absyn_Stmt*_tmp82C;struct Cyc_List_List*_tmp82D;struct Cyc_Absyn_Exp*
_tmp82F;_LL36D: {struct Cyc_Absyn_Skip_s_struct*_tmp7FE=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7FD;if(_tmp7FE->tag != 0)goto _LL36F;}_LL36E: return;_LL36F: {struct Cyc_Absyn_Exp_s_struct*
_tmp7FF=(struct Cyc_Absyn_Exp_s_struct*)_tmp7FD;if(_tmp7FF->tag != 1)goto _LL371;
else{_tmp800=_tmp7FF->f1;}}_LL370: Cyc_Toc_exp_to_c(nv,_tmp800);return;_LL371: {
struct Cyc_Absyn_Seq_s_struct*_tmp801=(struct Cyc_Absyn_Seq_s_struct*)_tmp7FD;if(
_tmp801->tag != 2)goto _LL373;else{_tmp802=_tmp801->f1;_tmp803=_tmp801->f2;}}
_LL372: Cyc_Toc_stmt_to_c(nv,_tmp802);s=_tmp803;continue;_LL373: {struct Cyc_Absyn_Return_s_struct*
_tmp804=(struct Cyc_Absyn_Return_s_struct*)_tmp7FD;if(_tmp804->tag != 3)goto _LL375;
else{_tmp805=_tmp804->f1;}}_LL374: {struct Cyc_Core_Opt*topt=0;if(_tmp805 != 0){{
struct Cyc_Core_Opt*_tmpD38;topt=((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp805->topt))->v),
_tmpD38))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp805);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp831=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp832=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp831,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp831,(void*)topt->v,_tmp805,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp832,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL375: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp806=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7FD;if(_tmp806->tag != 4)goto _LL377;
else{_tmp807=_tmp806->f1;_tmp808=_tmp806->f2;_tmp809=_tmp806->f3;}}_LL376: Cyc_Toc_exp_to_c(
nv,_tmp807);Cyc_Toc_stmt_to_c(nv,_tmp808);s=_tmp809;continue;_LL377: {struct Cyc_Absyn_While_s_struct*
_tmp80A=(struct Cyc_Absyn_While_s_struct*)_tmp7FD;if(_tmp80A->tag != 5)goto _LL379;
else{_tmp80B=_tmp80A->f1;_tmp80C=_tmp80B.f1;_tmp80D=_tmp80A->f2;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp80C);{struct Cyc_Toc_Env _tmp834;struct _RegionHandle*_tmp835;struct Cyc_Toc_Env*
_tmp833=nv;_tmp834=*_tmp833;_tmp835=_tmp834.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp835,nv),_tmp80D);return;};_LL379: {struct Cyc_Absyn_Break_s_struct*_tmp80E=(
struct Cyc_Absyn_Break_s_struct*)_tmp7FD;if(_tmp80E->tag != 6)goto _LL37B;else{
_tmp80F=_tmp80E->f1;}}_LL37A: {struct Cyc_Toc_Env _tmp837;struct _dyneither_ptr**
_tmp838;struct Cyc_Toc_Env*_tmp836=nv;_tmp837=*_tmp836;_tmp838=_tmp837.break_lab;
if(_tmp838 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp838,0);{int dest_depth=_tmp80F == 0?0:
_tmp80F->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37B: {struct Cyc_Absyn_Continue_s_struct*_tmp810=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7FD;if(_tmp810->tag != 7)goto _LL37D;else{_tmp811=_tmp810->f1;}}_LL37C: {
struct Cyc_Toc_Env _tmp83A;struct _dyneither_ptr**_tmp83B;struct Cyc_Toc_Env*_tmp839=
nv;_tmp83A=*_tmp839;_tmp83B=_tmp83A.continue_lab;if(_tmp83B != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp83B,0);_tmp813=_tmp811;goto _LL37E;}_LL37D: {struct Cyc_Absyn_Goto_s_struct*
_tmp812=(struct Cyc_Absyn_Goto_s_struct*)_tmp7FD;if(_tmp812->tag != 8)goto _LL37F;
else{_tmp813=_tmp812->f2;}}_LL37E: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp813))->try_depth,s);return;_LL37F: {struct Cyc_Absyn_For_s_struct*
_tmp814=(struct Cyc_Absyn_For_s_struct*)_tmp7FD;if(_tmp814->tag != 9)goto _LL381;
else{_tmp815=_tmp814->f1;_tmp816=_tmp814->f2;_tmp817=_tmp816.f1;_tmp818=_tmp814->f3;
_tmp819=_tmp818.f1;_tmp81A=_tmp814->f4;}}_LL380: Cyc_Toc_exp_to_c(nv,_tmp815);Cyc_Toc_exp_to_c(
nv,_tmp817);Cyc_Toc_exp_to_c(nv,_tmp819);{struct Cyc_Toc_Env _tmp83D;struct
_RegionHandle*_tmp83E;struct Cyc_Toc_Env*_tmp83C=nv;_tmp83D=*_tmp83C;_tmp83E=
_tmp83D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp83E,nv),_tmp81A);return;};
_LL381: {struct Cyc_Absyn_Switch_s_struct*_tmp81B=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7FD;if(_tmp81B->tag != 10)goto _LL383;else{_tmp81C=_tmp81B->f1;_tmp81D=_tmp81B->f2;}}
_LL382: Cyc_Toc_xlate_switch(nv,s,_tmp81C,_tmp81D);return;_LL383: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp81E=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7FD;if(_tmp81E->tag != 11)goto
_LL385;else{_tmp81F=_tmp81E->f1;_tmp820=_tmp81E->f2;}}_LL384: {struct Cyc_Toc_Env
_tmp840;struct Cyc_Toc_FallthruInfo*_tmp841;struct Cyc_Toc_Env*_tmp83F=nv;_tmp840=*
_tmp83F;_tmp841=_tmp840.fallthru_info;if(_tmp841 == 0){const char*_tmpD3B;void*
_tmpD3A;(_tmpD3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD3B="fallthru in unexpected place",_tag_dyneither(_tmpD3B,sizeof(char),29))),
_tag_dyneither(_tmpD3A,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp845;struct
Cyc_List_List*_tmp846;struct Cyc_Dict_Dict _tmp847;struct Cyc_Toc_FallthruInfo
_tmp844=*_tmp841;_tmp845=_tmp844.label;_tmp846=_tmp844.binders;_tmp847=_tmp844.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp845,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp820)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp848=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp846);struct Cyc_List_List*_tmp849=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp81F);for(0;_tmp848 != 0;(_tmp848=_tmp848->tl,
_tmp849=_tmp849->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp849))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp847,(
struct _tuple0*)_tmp848->hd),(struct Cyc_Absyn_Exp*)_tmp849->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL385: {struct Cyc_Absyn_Decl_s_struct*_tmp821=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7FD;if(_tmp821->tag != 12)goto _LL387;else{_tmp822=_tmp821->f1;_tmp823=_tmp821->f2;}}
_LL386:{void*_tmp84A=_tmp822->r;struct Cyc_Absyn_Vardecl*_tmp84C;struct Cyc_Absyn_Pat*
_tmp84E;struct Cyc_Absyn_Exp*_tmp84F;struct Cyc_List_List*_tmp851;struct Cyc_Absyn_Fndecl*
_tmp853;struct Cyc_Absyn_Tvar*_tmp855;struct Cyc_Absyn_Vardecl*_tmp856;int _tmp857;
struct Cyc_Absyn_Exp*_tmp858;struct Cyc_Absyn_Exp*_tmp85A;struct Cyc_Absyn_Tvar*
_tmp85B;struct Cyc_Absyn_Vardecl*_tmp85C;_LL390: {struct Cyc_Absyn_Var_d_struct*
_tmp84B=(struct Cyc_Absyn_Var_d_struct*)_tmp84A;if(_tmp84B->tag != 0)goto _LL392;
else{_tmp84C=_tmp84B->f1;}}_LL391: {struct Cyc_Toc_Env _tmp85E;struct _RegionHandle*
_tmp85F;struct Cyc_Toc_Env*_tmp85D=nv;_tmp85E=*_tmp85D;_tmp85F=_tmp85E.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD3E;struct Cyc_Absyn_Local_b_struct*_tmpD3D;
struct Cyc_Toc_Env*_tmp860=Cyc_Toc_add_varmap(_tmp85F,nv,_tmp84C->name,Cyc_Absyn_varb_exp(
_tmp84C->name,(void*)((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D[0]=((
_tmpD3E.tag=4,((_tmpD3E.f1=_tmp84C,_tmpD3E)))),_tmpD3D)))),0));Cyc_Toc_local_decl_to_c(
_tmp860,_tmp860,_tmp84C,_tmp823);}goto _LL38F;}_LL392: {struct Cyc_Absyn_Let_d_struct*
_tmp84D=(struct Cyc_Absyn_Let_d_struct*)_tmp84A;if(_tmp84D->tag != 2)goto _LL394;
else{_tmp84E=_tmp84D->f1;_tmp84F=_tmp84D->f3;}}_LL393:{void*_tmp863=_tmp84E->r;
struct Cyc_Absyn_Vardecl*_tmp865;struct Cyc_Absyn_Pat*_tmp866;struct Cyc_Absyn_Pat
_tmp867;void*_tmp868;_LL39F: {struct Cyc_Absyn_Var_p_struct*_tmp864=(struct Cyc_Absyn_Var_p_struct*)
_tmp863;if(_tmp864->tag != 1)goto _LL3A1;else{_tmp865=_tmp864->f1;_tmp866=_tmp864->f2;
_tmp867=*_tmp866;_tmp868=_tmp867.r;{struct Cyc_Absyn_Wild_p_struct*_tmp869=(
struct Cyc_Absyn_Wild_p_struct*)_tmp868;if(_tmp869->tag != 0)goto _LL3A1;};}}_LL3A0: {
struct _tuple0*old_name=_tmp865->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp865->name=new_name;_tmp865->initializer=(struct Cyc_Absyn_Exp*)_tmp84F;{
struct Cyc_Absyn_Var_d_struct _tmpD41;struct Cyc_Absyn_Var_d_struct*_tmpD40;_tmp822->r=(
void*)((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40[0]=((_tmpD41.tag=0,((
_tmpD41.f1=_tmp865,_tmpD41)))),_tmpD40))));}{struct Cyc_Toc_Env _tmp86D;struct
_RegionHandle*_tmp86E;struct Cyc_Toc_Env*_tmp86C=nv;_tmp86D=*_tmp86C;_tmp86E=
_tmp86D.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD44;struct Cyc_Absyn_Local_b_struct*
_tmpD43;struct Cyc_Toc_Env*_tmp86F=Cyc_Toc_add_varmap(_tmp86E,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=((_tmpD44.tag=
4,((_tmpD44.f1=_tmp865,_tmpD44)))),_tmpD43)))),0));Cyc_Toc_local_decl_to_c(
_tmp86F,nv,_tmp865,_tmp823);}goto _LL39E;};}_LL3A1:;_LL3A2: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp84E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp84F->topt))->v,_tmp84F,
_tmp823))->r;goto _LL39E;_LL39E:;}goto _LL38F;_LL394: {struct Cyc_Absyn_Letv_d_struct*
_tmp850=(struct Cyc_Absyn_Letv_d_struct*)_tmp84A;if(_tmp850->tag != 3)goto _LL396;
else{_tmp851=_tmp850->f1;}}_LL395: {struct Cyc_List_List*_tmp872=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp851);if(_tmp872 == 0){const char*_tmpD47;
void*_tmpD46;(_tmpD46=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD47="empty Letv_d",_tag_dyneither(_tmpD47,sizeof(char),
13))),_tag_dyneither(_tmpD46,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD4A;struct Cyc_Absyn_Var_d_struct*_tmpD49;_tmp822->r=(void*)((_tmpD49=
_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=0,((_tmpD4A.f1=(struct Cyc_Absyn_Vardecl*)
_tmp872->hd,_tmpD4A)))),_tmpD49))));}_tmp872=_tmp872->tl;for(0;_tmp872 != 0;
_tmp872=_tmp872->tl){struct Cyc_Absyn_Var_d_struct _tmpD4D;struct Cyc_Absyn_Var_d_struct*
_tmpD4C;struct Cyc_Absyn_Decl*_tmp877=Cyc_Absyn_new_decl((void*)((_tmpD4C=
_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C[0]=((_tmpD4D.tag=0,((_tmpD4D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp872->hd,_tmpD4D)))),_tmpD4C)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp877,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL38F;}_LL396: {struct Cyc_Absyn_Fn_d_struct*
_tmp852=(struct Cyc_Absyn_Fn_d_struct*)_tmp84A;if(_tmp852->tag != 1)goto _LL398;
else{_tmp853=_tmp852->f1;}}_LL397: {struct _tuple0*_tmp87A=_tmp853->name;struct
Cyc_Toc_Env _tmp87C;struct _RegionHandle*_tmp87D;struct Cyc_Toc_Env*_tmp87B=nv;
_tmp87C=*_tmp87B;_tmp87D=_tmp87C.rgn;{struct Cyc_Toc_Env*_tmp87E=Cyc_Toc_add_varmap(
_tmp87D,nv,_tmp853->name,Cyc_Absyn_var_exp(_tmp87A,0));Cyc_Toc_fndecl_to_c(
_tmp87E,_tmp853,0);Cyc_Toc_stmt_to_c(_tmp87E,_tmp823);}goto _LL38F;}_LL398: {
struct Cyc_Absyn_Region_d_struct*_tmp854=(struct Cyc_Absyn_Region_d_struct*)
_tmp84A;if(_tmp854->tag != 4)goto _LL39A;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;
_tmp857=_tmp854->f3;_tmp858=_tmp854->f4;}}_LL399: {struct Cyc_Absyn_Stmt*_tmp87F=
_tmp823;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp856->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp881;struct _RegionHandle*_tmp882;struct Cyc_Toc_Env*
_tmp880=nv;_tmp881=*_tmp880;_tmp882=_tmp881.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp882,nv,x_var,x_exp),_tmp87F);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp87F,0))->r;
else{if(_tmp858 == 0){struct Cyc_Absyn_Exp*_tmpD50[1];struct Cyc_Absyn_Exp*_tmpD4F[
1];struct Cyc_List_List*_tmpD4E;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD4E=
_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD4E->tl=0,_tmpD4E)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD4F[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD4F,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp87F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD50[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD50,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp858);{struct Cyc_Absyn_Exp*
_tmpD52[1];struct Cyc_Absyn_Exp*_tmpD51[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD51[1]=(struct Cyc_Absyn_Exp*)_tmp858,((_tmpD51[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD51,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp87F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD52[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD52,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39A: {struct Cyc_Absyn_Alias_d_struct*_tmp859=(struct Cyc_Absyn_Alias_d_struct*)
_tmp84A;if(_tmp859->tag != 5)goto _LL39C;else{_tmp85A=_tmp859->f1;_tmp85B=_tmp859->f2;
_tmp85C=_tmp859->f3;}}_LL39B: {struct _tuple0*old_name=_tmp85C->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp85C->name=new_name;_tmp85C->initializer=(
struct Cyc_Absyn_Exp*)_tmp85A;{struct Cyc_Absyn_Decl_s_struct _tmpD61;struct Cyc_Absyn_Var_d_struct*
_tmpD60;struct Cyc_Absyn_Var_d_struct _tmpD5F;struct Cyc_Absyn_Decl*_tmpD5E;struct
Cyc_Absyn_Decl_s_struct*_tmpD5D;s->r=(void*)((_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((
_tmpD5D[0]=((_tmpD61.tag=12,((_tmpD61.f1=((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((
_tmpD5E->r=(void*)((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60[0]=((_tmpD5F.tag=
0,((_tmpD5F.f1=_tmp85C,_tmpD5F)))),_tmpD60)))),((_tmpD5E->loc=0,_tmpD5E)))))),((
_tmpD61.f2=_tmp823,_tmpD61)))))),_tmpD5D))));}{struct Cyc_Toc_Env _tmp88E;struct
_RegionHandle*_tmp88F;struct Cyc_Toc_Env*_tmp88D=nv;_tmp88E=*_tmp88D;_tmp88F=
_tmp88E.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD64;struct Cyc_Absyn_Local_b_struct*
_tmpD63;struct Cyc_Toc_Env*_tmp890=Cyc_Toc_add_varmap(_tmp88F,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD64.tag=
4,((_tmpD64.f1=_tmp85C,_tmpD64)))),_tmpD63)))),0));Cyc_Toc_local_decl_to_c(
_tmp890,nv,_tmp85C,_tmp823);}return;};}_LL39C:;_LL39D: {const char*_tmpD67;void*
_tmpD66;(_tmpD66=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD67="bad nested declaration within function",_tag_dyneither(_tmpD67,sizeof(
char),39))),_tag_dyneither(_tmpD66,sizeof(void*),0)));}_LL38F:;}return;_LL387: {
struct Cyc_Absyn_Label_s_struct*_tmp824=(struct Cyc_Absyn_Label_s_struct*)_tmp7FD;
if(_tmp824->tag != 13)goto _LL389;else{_tmp825=_tmp824->f1;_tmp826=_tmp824->f2;}}
_LL388: s=_tmp826;continue;_LL389: {struct Cyc_Absyn_Do_s_struct*_tmp827=(struct
Cyc_Absyn_Do_s_struct*)_tmp7FD;if(_tmp827->tag != 14)goto _LL38B;else{_tmp828=
_tmp827->f1;_tmp829=_tmp827->f2;_tmp82A=_tmp829.f1;}}_LL38A: {struct Cyc_Toc_Env
_tmp896;struct _RegionHandle*_tmp897;struct Cyc_Toc_Env*_tmp895=nv;_tmp896=*
_tmp895;_tmp897=_tmp896.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp897,nv),
_tmp828);Cyc_Toc_exp_to_c(nv,_tmp82A);return;}_LL38B: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp82B=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7FD;if(_tmp82B->tag != 15)goto
_LL38D;else{_tmp82C=_tmp82B->f1;_tmp82D=_tmp82B->f2;}}_LL38C: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD68;
e_exp->topt=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->v=e_typ,_tmpD68))));}{
struct Cyc_Toc_Env _tmp89A;struct _RegionHandle*_tmp89B;struct Cyc_Toc_Env*_tmp899=
nv;_tmp89A=*_tmp899;_tmp89B=_tmp89A.rgn;{struct Cyc_Toc_Env*_tmp89C=Cyc_Toc_add_varmap(
_tmp89B,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp89C,_tmp82C);{struct Cyc_Absyn_Stmt*
_tmp89D=Cyc_Absyn_seq_stmt(_tmp82C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp89E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp89F=
Cyc_Absyn_var_exp(_tmp89E,0);struct Cyc_Absyn_Vardecl*_tmp8A0=Cyc_Absyn_new_vardecl(
_tmp89E,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD69;_tmp89F->topt=((
_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->v=Cyc_Absyn_exn_typ(),_tmpD69))));}{
struct Cyc_Core_Opt*_tmpD7A;struct Cyc_Absyn_Var_p_struct*_tmpD79;struct Cyc_Absyn_Pat*
_tmpD78;struct Cyc_Core_Opt*_tmpD77;struct Cyc_Absyn_Var_p_struct _tmpD76;struct Cyc_Absyn_Pat*
_tmpD75;struct Cyc_Absyn_Pat*_tmp8A2=(_tmpD75=_cycalloc(sizeof(*_tmpD75)),((
_tmpD75->r=(void*)((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79[0]=((_tmpD76.tag=
1,((_tmpD76.f1=_tmp8A0,((_tmpD76.f2=((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((
_tmpD78->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD78->topt=((_tmpD77=_cycalloc(
sizeof(*_tmpD77)),((_tmpD77->v=Cyc_Absyn_exn_typ(),_tmpD77)))),((_tmpD78->loc=0,
_tmpD78)))))))),_tmpD76)))))),_tmpD79)))),((_tmpD75->topt=((_tmpD7A=_cycalloc(
sizeof(*_tmpD7A)),((_tmpD7A->v=Cyc_Absyn_exn_typ(),_tmpD7A)))),((_tmpD75->loc=0,
_tmpD75)))))));struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Absyn_throw_exp(_tmp89F,0);{
struct Cyc_Core_Opt*_tmpD7B;_tmp8A3->topt=((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((
_tmpD7B->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD7B))));}{struct Cyc_Absyn_Stmt*
_tmp8A5=Cyc_Absyn_exp_stmt(_tmp8A3,0);struct Cyc_Core_Opt*_tmpD81;struct Cyc_List_List*
_tmpD80;struct Cyc_Absyn_Switch_clause*_tmpD7F;struct Cyc_Absyn_Switch_clause*
_tmp8A6=(_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->pattern=_tmp8A2,((
_tmpD7F->pat_vars=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->v=((_tmpD80=
_cycalloc(sizeof(*_tmpD80)),((_tmpD80->hd=_tmp8A0,((_tmpD80->tl=0,_tmpD80)))))),
_tmpD81)))),((_tmpD7F->where_clause=0,((_tmpD7F->body=_tmp8A5,((_tmpD7F->loc=0,
_tmpD7F)))))))))));struct Cyc_List_List*_tmpD82;struct Cyc_Absyn_Stmt*_tmp8A7=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp82D,((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->hd=_tmp8A6,((_tmpD82->tl=
0,_tmpD82))))))),0);Cyc_Toc_stmt_to_c(_tmp89C,_tmp8A7);{struct Cyc_List_List*
_tmpD83;struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD83->tl=0,_tmpD83)))))),0);struct Cyc_List_List*
_tmpD84;struct Cyc_Absyn_Stmt*_tmp8A9=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD84->tl=0,_tmpD84)))))),0),0);struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8A9,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8AA,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8A8,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8AB,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp89D,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8A7,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38D: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp82E=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7FD;if(_tmp82E->tag != 16)goto _LL36C;else{_tmp82F=_tmp82E->f1;}}_LL38E: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp82F);{struct Cyc_List_List*
_tmpD85;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=_tmp82F,((_tmpD85->tl=0,
_tmpD85)))))),0));};}return;_LL36C:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8B9=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8B9;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8BA=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8BB=f->args;for(0;_tmp8BB != 0;_tmp8BB=_tmp8BB->tl){struct
_tuple0*_tmpD86;struct _tuple0*_tmp8BC=(_tmpD86=_cycalloc(sizeof(*_tmpD86)),((
_tmpD86->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD86->f2=(*((struct
_tuple25*)_tmp8BB->hd)).f1,_tmpD86)))));(*((struct _tuple25*)_tmp8BB->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8BB->hd)).f3);_tmp8BA=Cyc_Toc_add_varmap(frgn,_tmp8BA,
_tmp8BC,Cyc_Absyn_var_exp(_tmp8BC,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8BF;struct Cyc_Absyn_Tqual
_tmp8C0;void*_tmp8C1;int _tmp8C2;struct Cyc_Absyn_VarargInfo _tmp8BE=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8BF=_tmp8BE.name;_tmp8C0=_tmp8BE.tq;_tmp8C1=
_tmp8BE.type;_tmp8C2=_tmp8BE.inject;{void*_tmp8C3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8C1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C0,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD87;struct _tuple0*_tmp8C4=(_tmpD87=_cycalloc(sizeof(*_tmpD87)),((
_tmpD87->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD87->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8BF))->v,_tmpD87)))));{
struct _tuple25*_tmpD8A;struct Cyc_List_List*_tmpD89;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD89=
_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((
_tmpD8A->f1=(struct _dyneither_ptr*)_tmp8BF->v,((_tmpD8A->f2=_tmp8C0,((_tmpD8A->f3=
_tmp8C3,_tmpD8A)))))))),((_tmpD89->tl=0,_tmpD89)))))));}_tmp8BA=Cyc_Toc_add_varmap(
frgn,_tmp8BA,_tmp8C4,Cyc_Absyn_var_exp(_tmp8C4,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8C8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8C8 != 0;_tmp8C8=_tmp8C8->tl){((struct Cyc_Absyn_Vardecl*)_tmp8C8->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8C8->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8BA),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A3: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A4: return Cyc_Absyn_Extern;
default: _LL3A5: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8C9=ad->name;struct _DynRegionHandle*_tmp8CB;struct Cyc_Dict_Dict*_tmp8CC;
struct Cyc_Toc_TocState _tmp8CA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8CB=_tmp8CA.dyn;_tmp8CC=_tmp8CA.aggrs_so_far;{struct _DynRegionFrame _tmp8CD;
struct _RegionHandle*d=_open_dynregion(& _tmp8CD,_tmp8CB);{int seen_defn_before;
struct _tuple14**_tmp8CE=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8CC,_tmp8C9);if(_tmp8CE == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD8B;v=((
_tmpD8B=_region_malloc(d,sizeof(*_tmpD8B)),((_tmpD8B->f1=ad,((_tmpD8B->f2=Cyc_Absyn_strctq(
_tmp8C9),_tmpD8B))))));}else{struct _tuple14*_tmpD8C;v=((_tmpD8C=_region_malloc(d,
sizeof(*_tmpD8C)),((_tmpD8C->f1=ad,((_tmpD8C->f2=Cyc_Absyn_unionq_typ(_tmp8C9),
_tmpD8C))))));}*_tmp8CC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CC,_tmp8C9,v);};}else{struct
_tuple14 _tmp8D2;struct Cyc_Absyn_Aggrdecl*_tmp8D3;void*_tmp8D4;struct _tuple14*
_tmp8D1=*_tmp8CE;_tmp8D2=*_tmp8D1;_tmp8D3=_tmp8D2.f1;_tmp8D4=_tmp8D2.f2;if(
_tmp8D3->impl == 0){{struct _tuple14*_tmpD8D;*_tmp8CC=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CC,
_tmp8C9,((_tmpD8D=_region_malloc(d,sizeof(*_tmpD8D)),((_tmpD8D->f1=ad,((_tmpD8D->f2=
_tmp8D4,_tmpD8D)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD8E;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((
_tmpD8E->kind=ad->kind,((_tmpD8E->sc=Cyc_Absyn_Public,((_tmpD8E->name=ad->name,((
_tmpD8E->tvs=0,((_tmpD8E->impl=0,((_tmpD8E->attributes=ad->attributes,_tmpD8E)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD8F;
new_ad->impl=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->exist_vars=0,((
_tmpD8F->rgn_po=0,((_tmpD8F->fields=0,((_tmpD8F->tagged=0,_tmpD8F))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8D7=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){struct Cyc_Absyn_Aggrfield*
_tmp8D8=(struct Cyc_Absyn_Aggrfield*)_tmp8D7->hd;struct Cyc_Absyn_Aggrfield*
_tmpD90;struct Cyc_Absyn_Aggrfield*_tmp8D9=(_tmpD90=_cycalloc(sizeof(*_tmpD90)),((
_tmpD90->name=_tmp8D8->name,((_tmpD90->tq=Cyc_Toc_mt_tq,((_tmpD90->type=Cyc_Toc_typ_to_c_array(
_tmp8D8->type),((_tmpD90->width=_tmp8D8->width,((_tmpD90->attributes=_tmp8D8->attributes,
_tmpD90)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8DA=_tmp8D9->type;struct _dyneither_ptr*_tmp8DB=_tmp8D9->name;const char*
_tmpD95;void*_tmpD94[2];struct Cyc_String_pa_struct _tmpD93;struct Cyc_String_pa_struct
_tmpD92;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD92.tag=0,((_tmpD92.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DB),((_tmpD93.tag=0,((_tmpD93.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD94[0]=&
_tmpD93,((_tmpD94[1]=& _tmpD92,Cyc_aprintf(((_tmpD95="_union_%s_%s",
_tag_dyneither(_tmpD95,sizeof(char),13))),_tag_dyneither(_tmpD94,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD96;struct _dyneither_ptr*str=(_tmpD96=_cycalloc(sizeof(*
_tmpD96)),((_tmpD96[0]=s,_tmpD96)));struct Cyc_Absyn_Aggrfield*_tmpD97;struct Cyc_Absyn_Aggrfield*
_tmp8DC=(_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->name=Cyc_Toc_val_sp,((
_tmpD97->tq=Cyc_Toc_mt_tq,((_tmpD97->type=_tmp8DA,((_tmpD97->width=0,((_tmpD97->attributes=
0,_tmpD97)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD98;struct Cyc_Absyn_Aggrfield*
_tmp8DD=(_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->name=Cyc_Toc_tag_sp,((
_tmpD98->tq=Cyc_Toc_mt_tq,((_tmpD98->type=Cyc_Absyn_sint_typ,((_tmpD98->width=0,((
_tmpD98->attributes=0,_tmpD98)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD99[2];
struct Cyc_List_List*_tmp8DE=(_tmpD99[1]=_tmp8DC,((_tmpD99[0]=_tmp8DD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD99,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD9E;struct _tuple0*_tmpD9D;struct Cyc_Absyn_Aggrdecl*
_tmpD9C;struct Cyc_Absyn_Aggrdecl*_tmp8DF=(_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((
_tmpD9C->kind=Cyc_Absyn_StructA,((_tmpD9C->sc=Cyc_Absyn_Public,((_tmpD9C->name=((
_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->f1=Cyc_Absyn_Loc_n,((_tmpD9D->f2=
str,_tmpD9D)))))),((_tmpD9C->tvs=0,((_tmpD9C->impl=((_tmpD9E=_cycalloc(sizeof(*
_tmpD9E)),((_tmpD9E->exist_vars=0,((_tmpD9E->rgn_po=0,((_tmpD9E->fields=_tmp8DE,((
_tmpD9E->tagged=0,_tmpD9E)))))))))),((_tmpD9C->attributes=0,_tmpD9C)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDA4;struct Cyc_Absyn_Aggr_d_struct _tmpDA3;
struct Cyc_List_List*_tmpDA2;Cyc_Toc_result_decls=((_tmpDA2=_cycalloc(sizeof(*
_tmpDA2)),((_tmpDA2->hd=Cyc_Absyn_new_decl((void*)((_tmpDA4=_cycalloc(sizeof(*
_tmpDA4)),((_tmpDA4[0]=((_tmpDA3.tag=6,((_tmpDA3.f1=_tmp8DF,_tmpDA3)))),_tmpDA4)))),
0),((_tmpDA2->tl=Cyc_Toc_result_decls,_tmpDA2))))));}_tmp8D9->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDA5;new_fields=((_tmpDA5=_cycalloc(sizeof(*
_tmpDA5)),((_tmpDA5->hd=_tmp8D9,((_tmpDA5->tl=new_fields,_tmpDA5))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDAF;struct Cyc_Absyn_Aggr_d_struct
_tmpDAE;struct Cyc_Absyn_Aggr_d_struct*_tmpDAD;struct Cyc_List_List*_tmpDAC;Cyc_Toc_result_decls=((
_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=((_tmpDAF=_cycalloc(sizeof(*
_tmpDAF)),((_tmpDAF->r=(void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((
_tmpDAE.tag=6,((_tmpDAE.f1=new_ad,_tmpDAE)))),_tmpDAD)))),((_tmpDAF->loc=0,
_tmpDAF)))))),((_tmpDAC->tl=Cyc_Toc_result_decls,_tmpDAC))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8F6;struct Cyc_Set_Set**_tmp8F7;struct Cyc_Toc_TocState
_tmp8F5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F6=
_tmp8F5.dyn;_tmp8F7=_tmp8F5.datatypes_so_far;{struct _DynRegionFrame _tmp8F8;
struct _RegionHandle*d=_open_dynregion(& _tmp8F8,_tmp8F6);{struct _tuple0*_tmp8F9=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8F7,_tmp8F9)){_npop_handler(0);return;}*_tmp8F7=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8F7,_tmp8F9);}{struct Cyc_List_List*_tmp8FA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8FA != 0;_tmp8FA=_tmp8FA->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FA->hd;struct Cyc_List_List*_tmp8FB=0;int i=
1;{struct Cyc_List_List*_tmp8FC=f->typs;for(0;_tmp8FC != 0;(_tmp8FC=_tmp8FC->tl,i
++)){struct _dyneither_ptr*_tmp8FD=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDB0;struct Cyc_Absyn_Aggrfield*_tmp8FE=(_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((
_tmpDB0->name=_tmp8FD,((_tmpDB0->tq=(*((struct _tuple9*)_tmp8FC->hd)).f1,((
_tmpDB0->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8FC->hd)).f2),((
_tmpDB0->width=0,((_tmpDB0->attributes=0,_tmpDB0)))))))))));struct Cyc_List_List*
_tmpDB1;_tmp8FB=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->hd=_tmp8FE,((
_tmpDB1->tl=_tmp8FB,_tmpDB1))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDB4;struct Cyc_List_List*
_tmpDB3;_tmp8FB=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->hd=((_tmpDB4=
_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->name=Cyc_Toc_tag_sp,((_tmpDB4->tq=Cyc_Toc_mt_tq,((
_tmpDB4->type=Cyc_Absyn_sint_typ,((_tmpDB4->width=0,((_tmpDB4->attributes=0,
_tmpDB4)))))))))))),((_tmpDB3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8FB),_tmpDB3))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDB9;const char*_tmpDB8;struct Cyc_Absyn_Aggrdecl*_tmpDB7;struct Cyc_Absyn_Aggrdecl*
_tmp903=(_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->kind=Cyc_Absyn_StructA,((
_tmpDB7->sc=Cyc_Absyn_Public,((_tmpDB7->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDB8="_struct",_tag_dyneither(_tmpDB8,sizeof(char),8)))),((_tmpDB7->tvs=0,((
_tmpDB7->impl=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->exist_vars=0,((
_tmpDB9->rgn_po=0,((_tmpDB9->fields=_tmp8FB,((_tmpDB9->tagged=0,_tmpDB9)))))))))),((
_tmpDB7->attributes=0,_tmpDB7)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDBF;
struct Cyc_Absyn_Aggr_d_struct _tmpDBE;struct Cyc_List_List*_tmpDBD;Cyc_Toc_result_decls=((
_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=Cyc_Absyn_new_decl((void*)((
_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDBE.tag=6,((_tmpDBE.f1=
_tmp903,_tmpDBE)))),_tmpDBF)))),0),((_tmpDBD->tl=Cyc_Toc_result_decls,_tmpDBD))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp90B;struct Cyc_Dict_Dict*_tmp90C;struct
Cyc_Toc_TocState _tmp90A=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp90B=_tmp90A.dyn;_tmp90C=_tmp90A.xdatatypes_so_far;{struct _DynRegionFrame
_tmp90D;struct _RegionHandle*d=_open_dynregion(& _tmp90D,_tmp90B);{struct _tuple0*
_tmp90E=xd->name;struct Cyc_List_List*_tmp90F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp90F != 0;_tmp90F=_tmp90F->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp90F->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp910=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp911=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp910,Cyc_Absyn_false_conref,0);int*_tmp912=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp90C,f->name);int
_tmp913;_LL3A8: if(_tmp912 != 0)goto _LL3AA;_LL3A9: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp914=Cyc_Absyn_new_vardecl(f->name,_tmp911,
initopt);_tmp914->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDC5;struct Cyc_Absyn_Var_d_struct
_tmpDC4;struct Cyc_List_List*_tmpDC3;Cyc_Toc_result_decls=((_tmpDC3=_cycalloc(
sizeof(*_tmpDC3)),((_tmpDC3->hd=Cyc_Absyn_new_decl((void*)((_tmpDC5=_cycalloc(
sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC4.tag=0,((_tmpDC4.f1=_tmp914,_tmpDC4)))),
_tmpDC5)))),0),((_tmpDC3->tl=Cyc_Toc_result_decls,_tmpDC3))))));}*_tmp90C=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90C,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp918=f->typs;for(0;_tmp918 != 0;(_tmp918=_tmp918->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDCD;void*_tmpDCC[1];const char*_tmpDCB;struct
_dyneither_ptr*_tmpDCA;struct _dyneither_ptr*_tmp919=(_tmpDCA=_cycalloc(sizeof(*
_tmpDCA)),((_tmpDCA[0]=(struct _dyneither_ptr)((_tmpDCD.tag=1,((_tmpDCD.f1=(
unsigned long)i,((_tmpDCC[0]=& _tmpDCD,Cyc_aprintf(((_tmpDCB="f%d",_tag_dyneither(
_tmpDCB,sizeof(char),4))),_tag_dyneither(_tmpDCC,sizeof(void*),1)))))))),_tmpDCA)));
struct Cyc_Absyn_Aggrfield*_tmpDCE;struct Cyc_Absyn_Aggrfield*_tmp91A=(_tmpDCE=
_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->name=_tmp919,((_tmpDCE->tq=(*((struct
_tuple9*)_tmp918->hd)).f1,((_tmpDCE->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp918->hd)).f2),((_tmpDCE->width=0,((_tmpDCE->attributes=0,_tmpDCE)))))))))));
struct Cyc_List_List*_tmpDCF;fields=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((
_tmpDCF->hd=_tmp91A,((_tmpDCF->tl=fields,_tmpDCF))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDD2;struct Cyc_List_List*_tmpDD1;fields=((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((
_tmpDD1->hd=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->name=Cyc_Toc_tag_sp,((
_tmpDD2->tq=Cyc_Toc_mt_tq,((_tmpDD2->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDD2->width=0,((_tmpDD2->attributes=0,_tmpDD2)))))))))))),((
_tmpDD1->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDD1))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDD7;const char*_tmpDD6;
struct Cyc_Absyn_Aggrdecl*_tmpDD5;struct Cyc_Absyn_Aggrdecl*_tmp923=(_tmpDD5=
_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->kind=Cyc_Absyn_StructA,((_tmpDD5->sc=Cyc_Absyn_Public,((
_tmpDD5->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDD6="_struct",
_tag_dyneither(_tmpDD6,sizeof(char),8)))),((_tmpDD5->tvs=0,((_tmpDD5->impl=((
_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->exist_vars=0,((_tmpDD7->rgn_po=0,((
_tmpDD7->fields=fields,((_tmpDD7->tagged=0,_tmpDD7)))))))))),((_tmpDD5->attributes=
0,_tmpDD5)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDDD;struct Cyc_Absyn_Aggr_d_struct
_tmpDDC;struct Cyc_List_List*_tmpDDB;Cyc_Toc_result_decls=((_tmpDDB=_cycalloc(
sizeof(*_tmpDDB)),((_tmpDDB->hd=Cyc_Absyn_new_decl((void*)((_tmpDDD=_cycalloc(
sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDC.tag=6,((_tmpDDC.f1=_tmp923,_tmpDDC)))),
_tmpDDD)))),0),((_tmpDDB->tl=Cyc_Toc_result_decls,_tmpDDB))))));}goto _LL3A7;};};};}
_LL3AA: if(_tmp912 == 0)goto _LL3AC;_tmp913=*_tmp912;if(_tmp913 != 0)goto _LL3AC;
_LL3AB: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp92A=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp92B=Cyc_Absyn_new_vardecl(f->name,_tmp911,(
struct Cyc_Absyn_Exp*)_tmp92A);_tmp92B->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDE3;struct Cyc_Absyn_Var_d_struct _tmpDE2;struct Cyc_List_List*_tmpDE1;Cyc_Toc_result_decls=((
_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->hd=Cyc_Absyn_new_decl((void*)((
_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE2.tag=0,((_tmpDE2.f1=
_tmp92B,_tmpDE2)))),_tmpDE3)))),0),((_tmpDE1->tl=Cyc_Toc_result_decls,_tmpDE1))))));}*
_tmp90C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90C,f->name,1);}goto _LL3A7;_LL3AC:;_LL3AD: goto _LL3A7;_LL3A7:;}};
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
void*_tmp92F=init->r;struct Cyc_Absyn_Vardecl*_tmp931;struct Cyc_Absyn_Exp*_tmp932;
struct Cyc_Absyn_Exp*_tmp933;int _tmp934;_LL3AF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp930=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp92F;if(_tmp930->tag != 28)
goto _LL3B1;else{_tmp931=_tmp930->f1;_tmp932=_tmp930->f2;_tmp933=_tmp930->f3;
_tmp934=_tmp930->f4;}}_LL3B0: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp931,_tmp932,_tmp933,_tmp934,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3AE;_LL3B1:;_LL3B2: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp936;struct _RegionHandle*_tmp937;struct Cyc_Toc_Env*_tmp935=init_nv;_tmp936=*
_tmp935;_tmp937=_tmp936.rgn;{struct Cyc_Toc_Env*_tmp938=Cyc_Toc_set_toplevel(
_tmp937,init_nv);Cyc_Toc_exp_to_c(_tmp938,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3AE;_LL3AE:;}else{void*_tmp939=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp93B;void*_tmp93C;struct Cyc_Absyn_Exp*_tmp93D;union Cyc_Absyn_Constraint*
_tmp93E;_LL3B4:{struct Cyc_Absyn_ArrayType_struct*_tmp93A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp939;if(_tmp93A->tag != 9)goto _LL3B6;else{_tmp93B=_tmp93A->f1;_tmp93C=_tmp93B.elt_type;
_tmp93D=_tmp93B.num_elts;_tmp93E=_tmp93B.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp93E))goto _LL3B6;_LL3B5: if(_tmp93D == 0){const char*
_tmpDE6;void*_tmpDE5;(_tmpDE5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDE6="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDE6,sizeof(char),55))),_tag_dyneither(_tmpDE5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp93D;struct Cyc_Absyn_Exp*
_tmp941=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp942=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp941,
_tmp942,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B3;};_LL3B6:;_LL3B7: goto _LL3B3;
_LL3B3:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp944;struct _RegionHandle*
_tmp945;struct Cyc_Toc_Env*_tmp943=nv;_tmp944=*_tmp943;_tmp945=_tmp944.rgn;{
struct Cyc_Absyn_Stmt*_tmp946=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp947=Cyc_Toc_share_env(_tmp945,nv);struct Cyc_Toc_Env*_tmp949;struct Cyc_List_List*
_tmp94A;struct Cyc_Absyn_Stmt*_tmp94B;struct _tuple21 _tmp948=Cyc_Toc_xlate_pat(
_tmp947,_tmp945,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp949=_tmp948.f1;_tmp94A=_tmp948.f2;_tmp94B=_tmp948.f3;Cyc_Toc_stmt_to_c(
_tmp949,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp94B,s,0),0);for(0;_tmp94A != 0;_tmp94A=_tmp94A->tl){
struct _tuple22 _tmp94D;struct _tuple0*_tmp94E;void*_tmp94F;struct _tuple22*_tmp94C=(
struct _tuple22*)_tmp94A->hd;_tmp94D=*_tmp94C;_tmp94E=_tmp94D.f1;_tmp94F=_tmp94D.f2;
s=Cyc_Absyn_declare_stmt(_tmp94E,_tmp94F,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp950=e->r;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*_tmp954;struct
Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*_tmp95A;
struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*
_tmp960;struct Cyc_List_List*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*
_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*
_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*
_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*
_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*
_tmp977;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_List_List*
_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_List_List*_tmp97E;void*_tmp980;
void**_tmp981;struct Cyc_Absyn_Exp*_tmp982;struct _tuple7*_tmp984;struct _tuple7
_tmp985;void*_tmp986;void**_tmp987;struct Cyc_List_List*_tmp988;struct Cyc_List_List*
_tmp98A;struct Cyc_List_List*_tmp98C;void*_tmp98E;void**_tmp98F;void*_tmp991;void**
_tmp992;struct Cyc_Absyn_Stmt*_tmp994;struct Cyc_Absyn_MallocInfo _tmp996;struct Cyc_Absyn_MallocInfo*
_tmp997;_LL3B9: {struct Cyc_Absyn_Deref_e_struct*_tmp951=(struct Cyc_Absyn_Deref_e_struct*)
_tmp950;if(_tmp951->tag != 21)goto _LL3BB;else{_tmp952=_tmp951->f1;}}_LL3BA:
_tmp954=_tmp952;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp953=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp950;if(_tmp953->tag != 22)goto _LL3BD;
else{_tmp954=_tmp953->f1;}}_LL3BC: _tmp956=_tmp954;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp955=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp950;if(_tmp955->tag != 23)goto
_LL3BF;else{_tmp956=_tmp955->f1;}}_LL3BE: _tmp958=_tmp956;goto _LL3C0;_LL3BF: {
struct Cyc_Absyn_Address_e_struct*_tmp957=(struct Cyc_Absyn_Address_e_struct*)
_tmp950;if(_tmp957->tag != 16)goto _LL3C1;else{_tmp958=_tmp957->f1;}}_LL3C0:
_tmp95A=_tmp958;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Throw_e_struct*_tmp959=(
struct Cyc_Absyn_Throw_e_struct*)_tmp950;if(_tmp959->tag != 12)goto _LL3C3;else{
_tmp95A=_tmp959->f1;}}_LL3C2: _tmp95C=_tmp95A;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp95B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp950;if(_tmp95B->tag != 13)
goto _LL3C5;else{_tmp95C=_tmp95B->f1;}}_LL3C4: _tmp95E=_tmp95C;goto _LL3C6;_LL3C5: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp95D=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp950;if(_tmp95D->tag != 19)goto _LL3C7;else{_tmp95E=_tmp95D->f1;}}_LL3C6:
_tmp960=_tmp95E;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Increment_e_struct*_tmp95F=(
struct Cyc_Absyn_Increment_e_struct*)_tmp950;if(_tmp95F->tag != 5)goto _LL3C9;else{
_tmp960=_tmp95F->f1;}}_LL3C8: Cyc_Toc_exptypes_to_c(_tmp960);goto _LL3B8;_LL3C9: {
struct Cyc_Absyn_Primop_e_struct*_tmp961=(struct Cyc_Absyn_Primop_e_struct*)
_tmp950;if(_tmp961->tag != 3)goto _LL3CB;else{_tmp962=_tmp961->f2;}}_LL3CA:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp962);goto _LL3B8;_LL3CB: {struct Cyc_Absyn_And_e_struct*_tmp963=(struct Cyc_Absyn_And_e_struct*)
_tmp950;if(_tmp963->tag != 7)goto _LL3CD;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;}}
_LL3CC: _tmp967=_tmp964;_tmp968=_tmp965;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Or_e_struct*
_tmp966=(struct Cyc_Absyn_Or_e_struct*)_tmp950;if(_tmp966->tag != 8)goto _LL3CF;
else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;}}_LL3CE: _tmp96A=_tmp967;_tmp96B=
_tmp968;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_SeqExp_e_struct*_tmp969=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp950;if(_tmp969->tag != 9)goto _LL3D1;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}
_LL3D0: _tmp96D=_tmp96A;_tmp96E=_tmp96B;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp96C=(struct Cyc_Absyn_Subscript_e_struct*)_tmp950;if(_tmp96C->tag != 24)goto
_LL3D3;else{_tmp96D=_tmp96C->f1;_tmp96E=_tmp96C->f2;}}_LL3D2: _tmp970=_tmp96D;
_tmp971=_tmp96E;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Swap_e_struct*_tmp96F=(
struct Cyc_Absyn_Swap_e_struct*)_tmp950;if(_tmp96F->tag != 35)goto _LL3D5;else{
_tmp970=_tmp96F->f1;_tmp971=_tmp96F->f2;}}_LL3D4: _tmp973=_tmp970;_tmp974=_tmp971;
goto _LL3D6;_LL3D5: {struct Cyc_Absyn_AssignOp_e_struct*_tmp972=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp950;if(_tmp972->tag != 4)goto _LL3D7;else{_tmp973=_tmp972->f1;_tmp974=_tmp972->f3;}}
_LL3D6: Cyc_Toc_exptypes_to_c(_tmp973);Cyc_Toc_exptypes_to_c(_tmp974);goto _LL3B8;
_LL3D7: {struct Cyc_Absyn_Conditional_e_struct*_tmp975=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp950;if(_tmp975->tag != 6)goto _LL3D9;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;
_tmp978=_tmp975->f3;}}_LL3D8: Cyc_Toc_exptypes_to_c(_tmp976);Cyc_Toc_exptypes_to_c(
_tmp977);Cyc_Toc_exptypes_to_c(_tmp978);goto _LL3B8;_LL3D9: {struct Cyc_Absyn_FnCall_e_struct*
_tmp979=(struct Cyc_Absyn_FnCall_e_struct*)_tmp950;if(_tmp979->tag != 11)goto
_LL3DB;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp979->f2;}}_LL3DA: _tmp97D=_tmp97A;
_tmp97E=_tmp97B;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp97C=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp950;if(_tmp97C->tag != 10)goto _LL3DD;
else{_tmp97D=_tmp97C->f1;_tmp97E=_tmp97C->f2;}}_LL3DC: Cyc_Toc_exptypes_to_c(
_tmp97D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp97E);goto _LL3B8;_LL3DD: {struct Cyc_Absyn_Cast_e_struct*
_tmp97F=(struct Cyc_Absyn_Cast_e_struct*)_tmp950;if(_tmp97F->tag != 15)goto _LL3DF;
else{_tmp980=(void**)& _tmp97F->f1;_tmp981=(void**)((void**)& _tmp97F->f1);_tmp982=
_tmp97F->f2;}}_LL3DE:*_tmp981=Cyc_Toc_typ_to_c(*_tmp981);Cyc_Toc_exptypes_to_c(
_tmp982);goto _LL3B8;_LL3DF: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp983=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp950;if(_tmp983->tag != 26)goto _LL3E1;
else{_tmp984=_tmp983->f1;_tmp985=*_tmp984;_tmp986=_tmp985.f3;_tmp987=(void**)&(*
_tmp983->f1).f3;_tmp988=_tmp983->f2;}}_LL3E0:*_tmp987=Cyc_Toc_typ_to_c(*_tmp987);
_tmp98A=_tmp988;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp989=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp950;if(_tmp989->tag != 36)
goto _LL3E3;else{_tmp98A=_tmp989->f2;}}_LL3E2: _tmp98C=_tmp98A;goto _LL3E4;_LL3E3: {
struct Cyc_Absyn_Array_e_struct*_tmp98B=(struct Cyc_Absyn_Array_e_struct*)_tmp950;
if(_tmp98B->tag != 27)goto _LL3E5;else{_tmp98C=_tmp98B->f1;}}_LL3E4: for(0;_tmp98C
!= 0;_tmp98C=_tmp98C->tl){struct Cyc_Absyn_Exp*_tmp9A7;struct _tuple15 _tmp9A6=*((
struct _tuple15*)_tmp98C->hd);_tmp9A7=_tmp9A6.f2;Cyc_Toc_exptypes_to_c(_tmp9A7);}
goto _LL3B8;_LL3E5: {struct Cyc_Absyn_Offsetof_e_struct*_tmp98D=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp950;if(_tmp98D->tag != 20)goto _LL3E7;else{_tmp98E=(void**)& _tmp98D->f1;
_tmp98F=(void**)((void**)& _tmp98D->f1);}}_LL3E6: _tmp992=_tmp98F;goto _LL3E8;
_LL3E7: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp990=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp950;if(_tmp990->tag != 18)goto _LL3E9;else{_tmp991=(void**)& _tmp990->f1;
_tmp992=(void**)((void**)& _tmp990->f1);}}_LL3E8:*_tmp992=Cyc_Toc_typ_to_c(*
_tmp992);goto _LL3B8;_LL3E9: {struct Cyc_Absyn_StmtExp_e_struct*_tmp993=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp950;if(_tmp993->tag != 37)goto _LL3EB;else{_tmp994=_tmp993->f1;}}_LL3EA: Cyc_Toc_stmttypes_to_c(
_tmp994);goto _LL3B8;_LL3EB: {struct Cyc_Absyn_Malloc_e_struct*_tmp995=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp950;if(_tmp995->tag != 34)goto _LL3ED;else{_tmp996=_tmp995->f1;_tmp997=(struct
Cyc_Absyn_MallocInfo*)& _tmp995->f1;}}_LL3EC: if(_tmp997->elt_type != 0){void**
_tmpDE7;_tmp997->elt_type=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp997->elt_type))),_tmpDE7))));}Cyc_Toc_exptypes_to_c(
_tmp997->num_elts);goto _LL3B8;_LL3ED: {struct Cyc_Absyn_Const_e_struct*_tmp998=(
struct Cyc_Absyn_Const_e_struct*)_tmp950;if(_tmp998->tag != 0)goto _LL3EF;}_LL3EE:
goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Var_e_struct*_tmp999=(struct Cyc_Absyn_Var_e_struct*)
_tmp950;if(_tmp999->tag != 1)goto _LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp99A=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp950;if(_tmp99A->tag != 2)goto
_LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Enum_e_struct*_tmp99B=(struct
Cyc_Absyn_Enum_e_struct*)_tmp950;if(_tmp99B->tag != 32)goto _LL3F5;}_LL3F4: goto
_LL3F6;_LL3F5: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp99C=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp950;if(_tmp99C->tag != 33)goto _LL3F7;}_LL3F6: goto _LL3B8;_LL3F7: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp99D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp950;if(_tmp99D->tag != 30)goto
_LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Datatype_e_struct*_tmp99E=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp950;if(_tmp99E->tag != 31)goto _LL3FB;}
_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Aggregate_e_struct*_tmp99F=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp950;if(_tmp99F->tag != 29)goto _LL3FD;}_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9A0=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp950;if(_tmp9A0->tag != 28)
goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tuple_e_struct*_tmp9A1=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp950;if(_tmp9A1->tag != 25)goto _LL401;}_LL400:
goto _LL402;_LL401: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9A2=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp950;if(_tmp9A2->tag != 14)goto _LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_New_e_struct*
_tmp9A3=(struct Cyc_Absyn_New_e_struct*)_tmp950;if(_tmp9A3->tag != 17)goto _LL405;}
_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_Valueof_e_struct*_tmp9A4=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp950;if(_tmp9A4->tag != 39)goto _LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9A5=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp950;if(_tmp9A5->tag != 38)goto
_LL3B8;}_LL408:{const char*_tmpDEA;void*_tmpDE9;(_tmpDE9=0,Cyc_Tcutil_terr(e->loc,((
_tmpDEA="Cyclone expression within C code",_tag_dyneither(_tmpDEA,sizeof(char),
33))),_tag_dyneither(_tmpDE9,sizeof(void*),0)));}goto _LL3B8;_LL3B8:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp9AB=d->r;struct Cyc_Absyn_Vardecl*_tmp9AD;struct
Cyc_Absyn_Fndecl*_tmp9AF;struct Cyc_Absyn_Aggrdecl*_tmp9B1;struct Cyc_Absyn_Enumdecl*
_tmp9B3;struct Cyc_Absyn_Typedefdecl*_tmp9B5;_LL40A: {struct Cyc_Absyn_Var_d_struct*
_tmp9AC=(struct Cyc_Absyn_Var_d_struct*)_tmp9AB;if(_tmp9AC->tag != 0)goto _LL40C;
else{_tmp9AD=_tmp9AC->f1;}}_LL40B: _tmp9AD->type=Cyc_Toc_typ_to_c(_tmp9AD->type);
if(_tmp9AD->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9AD->initializer));goto _LL409;_LL40C: {struct Cyc_Absyn_Fn_d_struct*
_tmp9AE=(struct Cyc_Absyn_Fn_d_struct*)_tmp9AB;if(_tmp9AE->tag != 1)goto _LL40E;
else{_tmp9AF=_tmp9AE->f1;}}_LL40D: _tmp9AF->ret_type=Cyc_Toc_typ_to_c(_tmp9AF->ret_type);{
struct Cyc_List_List*_tmp9C1=_tmp9AF->args;for(0;_tmp9C1 != 0;_tmp9C1=_tmp9C1->tl){(*((
struct _tuple25*)_tmp9C1->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9C1->hd)).f3);}}
goto _LL409;_LL40E: {struct Cyc_Absyn_Aggr_d_struct*_tmp9B0=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9AB;if(_tmp9B0->tag != 6)goto _LL410;else{_tmp9B1=_tmp9B0->f1;}}_LL40F: Cyc_Toc_aggrdecl_to_c(
_tmp9B1);goto _LL409;_LL410: {struct Cyc_Absyn_Enum_d_struct*_tmp9B2=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9AB;if(_tmp9B2->tag != 8)goto _LL412;else{_tmp9B3=_tmp9B2->f1;}}_LL411: if(
_tmp9B3->fields != 0){struct Cyc_List_List*_tmp9C2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9B3->fields))->v;for(0;_tmp9C2 != 0;_tmp9C2=_tmp9C2->tl){
struct Cyc_Absyn_Enumfield*_tmp9C3=(struct Cyc_Absyn_Enumfield*)_tmp9C2->hd;if(
_tmp9C3->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C3->tag));}}
goto _LL409;_LL412: {struct Cyc_Absyn_Typedef_d_struct*_tmp9B4=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9AB;if(_tmp9B4->tag != 9)goto _LL414;else{_tmp9B5=_tmp9B4->f1;}}_LL413:{struct
Cyc_Core_Opt*_tmpDEB;_tmp9B5->defn=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((
_tmpDEB->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B5->defn))->v),_tmpDEB))));}goto _LL409;_LL414: {struct Cyc_Absyn_Let_d_struct*
_tmp9B6=(struct Cyc_Absyn_Let_d_struct*)_tmp9AB;if(_tmp9B6->tag != 2)goto _LL416;}
_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_Letv_d_struct*_tmp9B7=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9AB;if(_tmp9B7->tag != 3)goto _LL418;}_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9B8=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9AB;if(_tmp9B8->tag != 7)goto
_LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Namespace_d_struct*_tmp9B9=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9AB;if(_tmp9B9->tag != 10)goto _LL41C;}
_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Using_d_struct*_tmp9BA=(struct Cyc_Absyn_Using_d_struct*)
_tmp9AB;if(_tmp9BA->tag != 11)goto _LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9BB=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9AB;if(_tmp9BB->tag != 12)goto
_LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9BC=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9AB;if(_tmp9BC->tag != 13)
goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Region_d_struct*_tmp9BD=(
struct Cyc_Absyn_Region_d_struct*)_tmp9AB;if(_tmp9BD->tag != 4)goto _LL424;}_LL423:
goto _LL425;_LL424: {struct Cyc_Absyn_Alias_d_struct*_tmp9BE=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9AB;if(_tmp9BE->tag != 5)goto _LL426;}_LL425:{const char*_tmpDEE;void*_tmpDED;(
_tmpDED=0,Cyc_Tcutil_terr(d->loc,((_tmpDEE="Cyclone declaration within C code",
_tag_dyneither(_tmpDEE,sizeof(char),34))),_tag_dyneither(_tmpDED,sizeof(void*),0)));}
goto _LL409;_LL426: {struct Cyc_Absyn_Porton_d_struct*_tmp9BF=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9AB;if(_tmp9BF->tag != 14)goto _LL428;}_LL427: goto _LL429;_LL428: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9C0=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9AB;if(_tmp9C0->tag != 15)goto
_LL409;}_LL429: goto _LL409;_LL409:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9C7=s->r;
struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Stmt*
_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_Absyn_Stmt*
_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D2;struct _tuple8 _tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;struct Cyc_Absyn_Stmt*_tmp9D6;struct Cyc_Absyn_Exp*_tmp9D8;struct _tuple8
_tmp9D9;struct Cyc_Absyn_Exp*_tmp9DA;struct _tuple8 _tmp9DB;struct Cyc_Absyn_Exp*
_tmp9DC;struct Cyc_Absyn_Stmt*_tmp9DD;struct Cyc_Absyn_Exp*_tmp9DF;struct Cyc_List_List*
_tmp9E0;struct Cyc_Absyn_Decl*_tmp9E2;struct Cyc_Absyn_Stmt*_tmp9E3;struct Cyc_Absyn_Stmt*
_tmp9E5;struct _tuple8 _tmp9E6;struct Cyc_Absyn_Exp*_tmp9E7;_LL42B: {struct Cyc_Absyn_Exp_s_struct*
_tmp9C8=(struct Cyc_Absyn_Exp_s_struct*)_tmp9C7;if(_tmp9C8->tag != 1)goto _LL42D;
else{_tmp9C9=_tmp9C8->f1;}}_LL42C: Cyc_Toc_exptypes_to_c(_tmp9C9);goto _LL42A;
_LL42D: {struct Cyc_Absyn_Seq_s_struct*_tmp9CA=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9C7;if(_tmp9CA->tag != 2)goto _LL42F;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=_tmp9CA->f2;}}
_LL42E: Cyc_Toc_stmttypes_to_c(_tmp9CB);Cyc_Toc_stmttypes_to_c(_tmp9CC);goto
_LL42A;_LL42F: {struct Cyc_Absyn_Return_s_struct*_tmp9CD=(struct Cyc_Absyn_Return_s_struct*)
_tmp9C7;if(_tmp9CD->tag != 3)goto _LL431;else{_tmp9CE=_tmp9CD->f1;}}_LL430: if(
_tmp9CE != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9CE);goto _LL42A;
_LL431: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9CF=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9C7;if(_tmp9CF->tag != 4)goto _LL433;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;
_tmp9D2=_tmp9CF->f3;}}_LL432: Cyc_Toc_exptypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(
_tmp9D1);Cyc_Toc_stmttypes_to_c(_tmp9D2);goto _LL42A;_LL433: {struct Cyc_Absyn_While_s_struct*
_tmp9D3=(struct Cyc_Absyn_While_s_struct*)_tmp9C7;if(_tmp9D3->tag != 5)goto _LL435;
else{_tmp9D4=_tmp9D3->f1;_tmp9D5=_tmp9D4.f1;_tmp9D6=_tmp9D3->f2;}}_LL434: Cyc_Toc_exptypes_to_c(
_tmp9D5);Cyc_Toc_stmttypes_to_c(_tmp9D6);goto _LL42A;_LL435: {struct Cyc_Absyn_For_s_struct*
_tmp9D7=(struct Cyc_Absyn_For_s_struct*)_tmp9C7;if(_tmp9D7->tag != 9)goto _LL437;
else{_tmp9D8=_tmp9D7->f1;_tmp9D9=_tmp9D7->f2;_tmp9DA=_tmp9D9.f1;_tmp9DB=_tmp9D7->f3;
_tmp9DC=_tmp9DB.f1;_tmp9DD=_tmp9D7->f4;}}_LL436: Cyc_Toc_exptypes_to_c(_tmp9D8);
Cyc_Toc_exptypes_to_c(_tmp9DA);Cyc_Toc_exptypes_to_c(_tmp9DC);Cyc_Toc_stmttypes_to_c(
_tmp9DD);goto _LL42A;_LL437: {struct Cyc_Absyn_Switch_s_struct*_tmp9DE=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9C7;if(_tmp9DE->tag != 10)goto _LL439;else{_tmp9DF=_tmp9DE->f1;_tmp9E0=_tmp9DE->f2;}}
_LL438: Cyc_Toc_exptypes_to_c(_tmp9DF);for(0;_tmp9E0 != 0;_tmp9E0=_tmp9E0->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9E0->hd)->body);}goto _LL42A;_LL439: {struct
Cyc_Absyn_Decl_s_struct*_tmp9E1=(struct Cyc_Absyn_Decl_s_struct*)_tmp9C7;if(
_tmp9E1->tag != 12)goto _LL43B;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E1->f2;}}
_LL43A: Cyc_Toc_decltypes_to_c(_tmp9E2);Cyc_Toc_stmttypes_to_c(_tmp9E3);goto
_LL42A;_LL43B: {struct Cyc_Absyn_Do_s_struct*_tmp9E4=(struct Cyc_Absyn_Do_s_struct*)
_tmp9C7;if(_tmp9E4->tag != 14)goto _LL43D;else{_tmp9E5=_tmp9E4->f1;_tmp9E6=_tmp9E4->f2;
_tmp9E7=_tmp9E6.f1;}}_LL43C: Cyc_Toc_stmttypes_to_c(_tmp9E5);Cyc_Toc_exptypes_to_c(
_tmp9E7);goto _LL42A;_LL43D: {struct Cyc_Absyn_Skip_s_struct*_tmp9E8=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9C7;if(_tmp9E8->tag != 0)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Break_s_struct*
_tmp9E9=(struct Cyc_Absyn_Break_s_struct*)_tmp9C7;if(_tmp9E9->tag != 6)goto _LL441;}
_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Continue_s_struct*_tmp9EA=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9C7;if(_tmp9EA->tag != 7)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Goto_s_struct*
_tmp9EB=(struct Cyc_Absyn_Goto_s_struct*)_tmp9C7;if(_tmp9EB->tag != 8)goto _LL445;}
_LL444: goto _LL42A;_LL445: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9EC=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9C7;if(_tmp9EC->tag != 11)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Label_s_struct*
_tmp9ED=(struct Cyc_Absyn_Label_s_struct*)_tmp9C7;if(_tmp9ED->tag != 13)goto _LL449;}
_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9EE=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9C7;if(_tmp9EE->tag != 15)goto _LL44B;}_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9EF=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9C7;if(_tmp9EF->tag != 16)goto
_LL42A;}_LL44C:{const char*_tmpDF1;void*_tmpDF0;(_tmpDF0=0,Cyc_Tcutil_terr(s->loc,((
_tmpDF1="Cyclone statement in C code",_tag_dyneither(_tmpDF1,sizeof(char),28))),
_tag_dyneither(_tmpDF0,sizeof(void*),0)));}goto _LL42A;_LL42A:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDF4;void*
_tmpDF3;(_tmpDF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDF4="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDF4,sizeof(char),29))),
_tag_dyneither(_tmpDF3,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9F4=d->r;struct Cyc_Absyn_Vardecl*_tmp9F6;struct Cyc_Absyn_Fndecl*
_tmp9F8;struct Cyc_Absyn_Aggrdecl*_tmp9FE;struct Cyc_Absyn_Datatypedecl*_tmpA00;
struct Cyc_Absyn_Enumdecl*_tmpA02;struct Cyc_Absyn_Typedefdecl*_tmpA04;struct Cyc_List_List*
_tmpA08;struct Cyc_List_List*_tmpA0A;struct Cyc_List_List*_tmpA0C;struct Cyc_List_List*
_tmpA0E;_LL44E: {struct Cyc_Absyn_Var_d_struct*_tmp9F5=(struct Cyc_Absyn_Var_d_struct*)
_tmp9F4;if(_tmp9F5->tag != 0)goto _LL450;else{_tmp9F6=_tmp9F5->f1;}}_LL44F: {
struct _tuple0*_tmpA0F=_tmp9F6->name;if(_tmp9F6->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDF5;_tmpA0F=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->f1=Cyc_Absyn_Rel_n(
0),((_tmpDF5->f2=(*_tmpA0F).f2,_tmpDF5))))));}if(_tmp9F6->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F6->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9F6->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDF8;struct Cyc_Absyn_Global_b_struct*_tmpDF7;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F6->name,Cyc_Absyn_varb_exp(_tmpA0F,(void*)((
_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=1,((_tmpDF8.f1=
_tmp9F6,_tmpDF8)))),_tmpDF7)))),0));}_tmp9F6->name=_tmpA0F;_tmp9F6->sc=Cyc_Toc_scope_to_c(
_tmp9F6->sc);_tmp9F6->type=Cyc_Toc_typ_to_c_array(_tmp9F6->type);{struct Cyc_List_List*
_tmpDF9;Cyc_Toc_result_decls=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->hd=
d,((_tmpDF9->tl=Cyc_Toc_result_decls,_tmpDF9))))));}goto _LL44D;}_LL450: {struct
Cyc_Absyn_Fn_d_struct*_tmp9F7=(struct Cyc_Absyn_Fn_d_struct*)_tmp9F4;if(_tmp9F7->tag
!= 1)goto _LL452;else{_tmp9F8=_tmp9F7->f1;}}_LL451: {struct _tuple0*_tmpA14=
_tmp9F8->name;if(_tmp9F8->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDFA;_tmpA14=((
_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->f1=Cyc_Absyn_Rel_n(0),((_tmpDFA->f2=(*
_tmpA14).f2,_tmpDFA))))));}_tmp9F8->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9F8->name,Cyc_Absyn_var_exp(_tmpA14,0));_tmp9F8->name=_tmpA14;Cyc_Toc_fndecl_to_c(
nv,_tmp9F8,cinclude);{struct Cyc_List_List*_tmpDFB;Cyc_Toc_result_decls=((_tmpDFB=
_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->hd=d,((_tmpDFB->tl=Cyc_Toc_result_decls,
_tmpDFB))))));}goto _LL44D;}_LL452: {struct Cyc_Absyn_Let_d_struct*_tmp9F9=(struct
Cyc_Absyn_Let_d_struct*)_tmp9F4;if(_tmp9F9->tag != 2)goto _LL454;}_LL453: goto
_LL455;_LL454: {struct Cyc_Absyn_Letv_d_struct*_tmp9FA=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9F4;if(_tmp9FA->tag != 3)goto _LL456;}_LL455: {const char*_tmpDFE;void*_tmpDFD;(
_tmpDFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDFE="letdecl at toplevel",_tag_dyneither(_tmpDFE,sizeof(char),20))),
_tag_dyneither(_tmpDFD,sizeof(void*),0)));}_LL456: {struct Cyc_Absyn_Region_d_struct*
_tmp9FB=(struct Cyc_Absyn_Region_d_struct*)_tmp9F4;if(_tmp9FB->tag != 4)goto _LL458;}
_LL457: {const char*_tmpE01;void*_tmpE00;(_tmpE00=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE01="region decl at toplevel",
_tag_dyneither(_tmpE01,sizeof(char),24))),_tag_dyneither(_tmpE00,sizeof(void*),0)));}
_LL458: {struct Cyc_Absyn_Alias_d_struct*_tmp9FC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9F4;if(_tmp9FC->tag != 5)goto _LL45A;}_LL459: {const char*_tmpE04;void*_tmpE03;(
_tmpE03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE04="alias decl at toplevel",_tag_dyneither(_tmpE04,sizeof(char),23))),
_tag_dyneither(_tmpE03,sizeof(void*),0)));}_LL45A: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9FD=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9F4;if(_tmp9FD->tag != 6)goto _LL45C;
else{_tmp9FE=_tmp9FD->f1;}}_LL45B: Cyc_Toc_aggrdecl_to_c(_tmp9FE);goto _LL44D;
_LL45C: {struct Cyc_Absyn_Datatype_d_struct*_tmp9FF=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9F4;if(_tmp9FF->tag != 7)goto _LL45E;else{_tmpA00=_tmp9FF->f1;}}_LL45D: if(
_tmpA00->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA00);else{Cyc_Toc_datatypedecl_to_c(
_tmpA00);}goto _LL44D;_LL45E: {struct Cyc_Absyn_Enum_d_struct*_tmpA01=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9F4;if(_tmpA01->tag != 8)goto _LL460;else{_tmpA02=_tmpA01->f1;}}_LL45F: Cyc_Toc_enumdecl_to_c(
nv,_tmpA02);{struct Cyc_List_List*_tmpE05;Cyc_Toc_result_decls=((_tmpE05=
_cycalloc(sizeof(*_tmpE05)),((_tmpE05->hd=d,((_tmpE05->tl=Cyc_Toc_result_decls,
_tmpE05))))));}goto _LL44D;_LL460: {struct Cyc_Absyn_Typedef_d_struct*_tmpA03=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9F4;if(_tmpA03->tag != 9)goto _LL462;else{
_tmpA04=_tmpA03->f1;}}_LL461: _tmpA04->name=_tmpA04->name;_tmpA04->tvs=0;if(
_tmpA04->defn != 0){struct Cyc_Core_Opt*_tmpE06;_tmpA04->defn=((_tmpE06=_cycalloc(
sizeof(*_tmpE06)),((_tmpE06->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA04->defn))->v),_tmpE06))));}else{switch(((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmpA04->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL46E:{struct Cyc_Core_Opt*_tmpE07;_tmpA04->defn=((_tmpE07=_cycalloc(sizeof(*
_tmpE07)),((_tmpE07->v=Cyc_Absyn_void_star_typ(),_tmpE07))));}break;default:
_LL46F:{struct Cyc_Core_Opt*_tmpE08;_tmpA04->defn=((_tmpE08=_cycalloc(sizeof(*
_tmpE08)),((_tmpE08->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE08))));}break;}}{
struct Cyc_List_List*_tmpE09;Cyc_Toc_result_decls=((_tmpE09=_cycalloc(sizeof(*
_tmpE09)),((_tmpE09->hd=d,((_tmpE09->tl=Cyc_Toc_result_decls,_tmpE09))))));}goto
_LL44D;_LL462: {struct Cyc_Absyn_Porton_d_struct*_tmpA05=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9F4;if(_tmpA05->tag != 14)goto _LL464;}_LL463: goto _LL465;_LL464: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA06=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9F4;if(_tmpA06->tag != 15)goto
_LL466;}_LL465: goto _LL44D;_LL466: {struct Cyc_Absyn_Namespace_d_struct*_tmpA07=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9F4;if(_tmpA07->tag != 10)goto _LL468;
else{_tmpA08=_tmpA07->f2;}}_LL467: _tmpA0A=_tmpA08;goto _LL469;_LL468: {struct Cyc_Absyn_Using_d_struct*
_tmpA09=(struct Cyc_Absyn_Using_d_struct*)_tmp9F4;if(_tmpA09->tag != 11)goto _LL46A;
else{_tmpA0A=_tmpA09->f2;}}_LL469: _tmpA0C=_tmpA0A;goto _LL46B;_LL46A: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA0B=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9F4;if(_tmpA0B->tag != 12)goto
_LL46C;else{_tmpA0C=_tmpA0B->f1;}}_LL46B: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0C,top,
cinclude);goto _LL44D;_LL46C: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA0D=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9F4;if(_tmpA0D->tag != 13)goto _LL44D;
else{_tmpA0E=_tmpA0D->f1;}}_LL46D: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0E,top,1);goto
_LL44D;_LL44D:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA23;struct Cyc_Core_NewRegion _tmpA22=Cyc_Core_new_dynregion();
_tmpA23=_tmpA22.dynregion;{struct _DynRegionFrame _tmpA24;struct _RegionHandle*d=
_open_dynregion(& _tmpA24,_tmpA23);{struct Cyc_Dict_Dict*_tmpE14;struct Cyc_Dict_Dict*
_tmpE13;struct Cyc_Set_Set**_tmpE12;struct Cyc_Dict_Dict*_tmpE11;struct Cyc_List_List**
_tmpE10;struct Cyc_Toc_TocState*_tmpE0F;Cyc_Toc_toc_state=((_tmpE0F=_cycalloc(
sizeof(*_tmpE0F)),((_tmpE0F->dyn=(struct _DynRegionHandle*)_tmpA23,((_tmpE0F->tuple_types=(
struct Cyc_List_List**)((_tmpE10=_region_malloc(d,sizeof(*_tmpE10)),((_tmpE10[0]=
0,_tmpE10)))),((_tmpE0F->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE11=
_region_malloc(d,sizeof(*_tmpE11)),((_tmpE11[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE11)))),((_tmpE0F->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE12=
_region_malloc(d,sizeof(*_tmpE12)),((_tmpE12[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE12)))),((_tmpE0F->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE13=
_region_malloc(d,sizeof(*_tmpE13)),((_tmpE13[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE13)))),((_tmpE0F->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE14=_region_malloc(
d,sizeof(*_tmpE14)),((_tmpE14[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE14)))),((_tmpE0F->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE0F))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE15;Cyc_Toc_globals=_tag_dyneither(((_tmpE15=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE15[0]=& Cyc_Toc__throw_str,((_tmpE15[1]=&
Cyc_Toc_setjmp_str,((_tmpE15[2]=& Cyc_Toc__push_handler_str,((_tmpE15[3]=& Cyc_Toc__pop_handler_str,((
_tmpE15[4]=& Cyc_Toc__exn_thrown_str,((_tmpE15[5]=& Cyc_Toc__npop_handler_str,((
_tmpE15[6]=& Cyc_Toc__check_null_str,((_tmpE15[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE15[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE15[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE15[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE15[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE15[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE15[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE15[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE15[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE15[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE15[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE15[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE15[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE15[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE15[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE15[22]=& Cyc_Toc__cycalloc_str,((_tmpE15[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE15[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE15[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE15[26]=& Cyc_Toc__region_malloc_str,((_tmpE15[27]=& Cyc_Toc__region_calloc_str,((
_tmpE15[28]=& Cyc_Toc__check_times_str,((_tmpE15[29]=& Cyc_Toc__new_region_str,((
_tmpE15[30]=& Cyc_Toc__push_region_str,((_tmpE15[31]=& Cyc_Toc__pop_region_str,((
_tmpE15[32]=& Cyc_Toc__open_dynregion_str,((_tmpE15[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE15[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE15[35]=& Cyc_Toc__reset_region_str,((
_tmpE15[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE15[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE15)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA2C=_new_region("start");struct
_RegionHandle*start=& _tmpA2C;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA2E;struct Cyc_Toc_TocState _tmpA2D=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA2E=_tmpA2D.dyn;Cyc_Core_free_dynregion(
_tmpA2E);};{struct Cyc_List_List*_tmpA2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA2F;};;
_pop_region(start);};}
